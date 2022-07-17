/**
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/rlc/rlc_am_nr.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/string_helpers.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include "srsran/rlc/rlc_am_nr_packing.h"
#include "srsran/srslog/event_trace.h"
#include <iostream>
#include <set>

namespace srsran {

const static uint32_t max_tx_queue_size = 256;

/****************************************************************************
 * RLC AM NR entity
 ***************************************************************************/

/***************************************************************************
 *  Tx subclass implementation
 ***************************************************************************/
rlc_am_nr_tx::rlc_am_nr_tx(rlc_am* parent_) :
  parent(parent_), rlc_am_base_tx(parent_->logger), poll_retransmit_timer(parent->timers->get_unique_timer())
{}

bool rlc_am_nr_tx::configure(const rlc_config_t& cfg_)
{
  cfg     = cfg_.am_nr;
  rb_name = parent->rb_name;

  if (cfg_.tx_queue_length > max_tx_queue_size) {
    RlcError("configuring tx queue length of %d PDUs too big. Maximum value is %d.",
             cfg_.tx_queue_length,
             max_tx_queue_size);
    return false;
  }

  mod_nr = cardinality(cfg.tx_sn_field_length);
  switch (cfg.tx_sn_field_length) {
    case rlc_am_nr_sn_size_t::size12bits:
      min_hdr_size = 2;
      tx_window    = std::unique_ptr<rlc_ringbuffer_base<rlc_amd_tx_pdu_nr> >(
          new rlc_ringbuffer_t<rlc_amd_tx_pdu_nr, am_window_size(rlc_am_nr_sn_size_t::size12bits)>);
      break;
    case rlc_am_nr_sn_size_t::size18bits:
      min_hdr_size = 3;
      tx_window    = std::unique_ptr<rlc_ringbuffer_base<rlc_amd_tx_pdu_nr> >(
          new rlc_ringbuffer_t<rlc_amd_tx_pdu_nr, am_window_size(rlc_am_nr_sn_size_t::size18bits)>);
      break;
    default:
      RlcError("attempt to configure unsupported tx_sn_field_length %s", to_string(cfg.tx_sn_field_length));
      return false;
  }

  max_hdr_size = min_hdr_size + so_size;

  // make sure Tx queue is empty before attempting to resize
  empty_queue_no_lock();
  tx_sdu_queue.resize(cfg_.tx_queue_length);

  // Check timers are valid
  if (not poll_retransmit_timer.is_valid()) {
    RlcError("Configuring TX: timers not configured");
    return false;
  }

  // Configure t_poll_retransmission timer
  if (cfg.t_poll_retx > 0) {
    poll_retransmit_timer.set(static_cast<uint32_t>(cfg.t_poll_retx),
                              [this](uint32_t timerid) { timer_expired(timerid); });
  }

  tx_enabled = true;

  RlcDebug("RLC AM NR configured tx entity.");
  return true;
}

bool rlc_am_nr_tx::has_data()
{
  return do_status() ||                                         // if we have a status PDU to transmit
         tx_sdu_queue.get_n_sdus() != 0 || !retx_queue.empty(); // or if there is a SDU queued up for transmission
}

/**
 * Builds the RLC PDU.
 *
 * Called by the MAC, trough one of the PHY worker threads.
 *
 * \param [payload] is a pointer to the buffer that will hold the PDU.
 * \param [nof_bytes] is the number of bytes the RLC is allowed to fill.
 *
 * \returns the number of bytes written to the payload buffer.
 * \remark: This will be called multiple times from the MAC,
 * while there is something to TX and enough space in the TB.
 */
uint32_t rlc_am_nr_tx::read_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (not tx_enabled) {
    RlcDebug("RLC entity not active. Not generating PDU.");
    return 0;
  }
  RlcDebug("MAC opportunity - bytes=%d, tx_window size=%zu PDUs", nof_bytes, tx_window->size());

  // Tx STATUS if requested
  if (do_status()) {
    unique_byte_buffer_t tx_pdu = srsran::make_byte_buffer();
    if (tx_pdu == nullptr) {
      RlcError("Couldn't allocate PDU in %s().", __FUNCTION__);
      return 0;
    }
    build_status_pdu(tx_pdu.get(), nof_bytes);
    memcpy(payload, tx_pdu->msg, tx_pdu->N_bytes);
    RlcDebug("Status PDU built - %d bytes", tx_pdu->N_bytes);
    return tx_pdu->N_bytes;
  }

  // Retransmit if required
  if (not retx_queue.empty()) {
    RlcInfo("Re-transmission required. Retransmission queue size: %d", retx_queue.size());
    return build_retx_pdu(payload, nof_bytes);
  }

  // Send remaining segment, if it exists
  if (sdu_under_segmentation_sn != INVALID_RLC_SN) {
    if (not tx_window->has_sn(sdu_under_segmentation_sn)) {
      sdu_under_segmentation_sn = INVALID_RLC_SN;
      RlcError("SDU currently being segmented does not exist in tx_window. Aborting segmentation SN=%d",
               sdu_under_segmentation_sn);
      return 0;
    }
    return build_continuation_sdu_segment((*tx_window)[sdu_under_segmentation_sn], payload, nof_bytes);
  }

  // Check whether there is something to TX
  if (tx_sdu_queue.is_empty()) {
    RlcInfo("No data available to be sent");
    return 0;
  }

  return build_new_pdu(payload, nof_bytes);
}

/**
 * Builds a new RLC PDU.
 *
 * This will be called after checking whether control, retransmission,
 * or segment PDUs needed to be transmitted first.
 *
 * This will read an SDU from the SDU queue, build a new PDU, and add it to the tx_window.
 * SDU segmentation will be done if necessary.
 *
 * \param [payload] is a pointer to the buffer that will hold the PDU.
 * \param [nof_bytes] is the number of bytes the RLC is allowed to fill.
 *
 * \returns the number of bytes written to the payload buffer.
 */
uint32_t rlc_am_nr_tx::build_new_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  if (nof_bytes <= min_hdr_size) {
    RlcInfo("Not enough bytes for payload plus header. nof_bytes=%d", nof_bytes);
    return 0;
  }

  // do not build any more PDU if window is already full
  if (tx_window->full()) {
    RlcInfo("Cannot build data PDU - Tx window full.");
    return 0;
  }

  // Read new SDU from TX queue
  unique_byte_buffer_t tx_sdu;
  RlcDebug("Reading from RLC SDU queue. Queue size %d", tx_sdu_queue.size());
  do {
    tx_sdu = tx_sdu_queue.read();
  } while (tx_sdu == nullptr && tx_sdu_queue.size() != 0);

  if (tx_sdu != nullptr) {
    RlcDebug("Read RLC SDU - RLC_SN=%d, PDCP_SN=%d, %d bytes", st.tx_next, tx_sdu->md.pdcp_sn, tx_sdu->N_bytes);
  } else {
    RlcDebug("No SDUs left in the tx queue.");
    return 0;
  }

  // insert newly assigned SN into window and use reference for in-place operations
  // NOTE: from now on, we can't return from this function anymore before increasing tx_next
  rlc_amd_tx_pdu_nr& tx_pdu = tx_window->add_pdu(st.tx_next);
  tx_pdu.pdcp_sn            = tx_sdu->md.pdcp_sn;
  tx_pdu.sdu_buf            = srsran::make_byte_buffer();
  if (tx_pdu.sdu_buf == nullptr) {
    RlcError("Couldn't allocate PDU in %s().", __FUNCTION__);
    return 0;
  }

  // Copy SDU into TX window SDU info
  memcpy(tx_pdu.sdu_buf->msg, tx_sdu->msg, tx_sdu->N_bytes);
  tx_pdu.sdu_buf->N_bytes = tx_sdu->N_bytes;

  // Segment new SDU if necessary
  if (tx_sdu->N_bytes + min_hdr_size > nof_bytes) {
    RlcInfo("trying to build PDU segment from SDU.");
    return build_new_sdu_segment(tx_pdu, payload, nof_bytes);
  }

  // Prepare header
  rlc_am_nr_pdu_header_t hdr = {};
  hdr.dc                     = RLC_DC_FIELD_DATA_PDU;
  hdr.p                      = get_pdu_poll(st.tx_next, false, tx_sdu->N_bytes);
  hdr.si                     = rlc_nr_si_field_t::full_sdu;
  hdr.sn_size                = cfg.tx_sn_field_length;
  hdr.sn                     = st.tx_next;
  tx_pdu.header              = hdr;
  log_rlc_am_nr_pdu_header_to_string(logger.info, hdr, rb_name);

  // Write header
  uint32_t len = rlc_am_nr_write_data_pdu_header(hdr, tx_sdu.get());
  if (len > nof_bytes) {
    RlcError("error writing AMD PDU header");
  }

  // Update TX Next
  st.tx_next = (st.tx_next + 1) % mod_nr;

  memcpy(payload, tx_sdu->msg, tx_sdu->N_bytes);
  RlcDebug("wrote RLC PDU - %d bytes", tx_sdu->N_bytes);

  return tx_sdu->N_bytes;
}

/**
 * Builds a new RLC PDU segment, from a RLC SDU.
 *
 * \param [tx_pdu] is the tx_pdu info contained in the tx_window.
 * \param [payload] is a pointer to the MAC buffer that will hold the PDU segment.
 * \param [nof_bytes] is the number of bytes the RLC is allowed to fill.
 *
 * \returns the number of bytes written to the payload buffer.
 * \remark: This functions assumes that the SDU has already been copied to tx_pdu.sdu_buf.
 */
uint32_t rlc_am_nr_tx::build_new_sdu_segment(rlc_amd_tx_pdu_nr& tx_pdu, uint8_t* payload, uint32_t nof_bytes)
{
  RlcInfo("creating new SDU segment. Tx SDU (%d B), nof_bytes=%d B ", tx_pdu.sdu_buf->N_bytes, nof_bytes);

  // Sanity check: can this SDU be sent this in a single PDU?
  if ((tx_pdu.sdu_buf->N_bytes + min_hdr_size) < nof_bytes) {
    RlcError("calling build_new_sdu_segment(), but there are enough bytes to tx in a single PDU. Tx SDU (%d B), "
             "nof_bytes=%d B ",
             tx_pdu.sdu_buf->N_bytes,
             nof_bytes);
    return 0;
  }

  // Sanity check: can this SDU be sent considering header overhead?
  if (nof_bytes <= min_hdr_size) { // Small header as SO is not present
    RlcInfo("cannot build new sdu_segment, there are not enough bytes allocated to tx header plus data. nof_bytes=%d, "
            "min_hdr_size=%d",
            nof_bytes,
            min_hdr_size);
    return 0;
  }

  uint32_t segment_payload_len = nof_bytes - min_hdr_size;

  // Save SDU currently being segmented
  // This needs to be done before calculating the polling bit
  // To make sure we check correctly that the buffers are empty.
  sdu_under_segmentation_sn = st.tx_next;

  // Prepare header
  rlc_am_nr_pdu_header_t hdr = {};
  hdr.dc                     = RLC_DC_FIELD_DATA_PDU;
  hdr.p                      = get_pdu_poll(st.tx_next, false, segment_payload_len);
  hdr.si                     = rlc_nr_si_field_t::first_segment;
  hdr.sn_size                = cfg.tx_sn_field_length;
  hdr.sn                     = st.tx_next;
  hdr.so                     = 0;
  tx_pdu.header              = hdr;
  log_rlc_am_nr_pdu_header_to_string(logger.info, hdr, rb_name);

  // Write header
  uint32_t hdr_len = rlc_am_nr_write_data_pdu_header(hdr, payload);
  if (hdr_len >= nof_bytes || hdr_len != min_hdr_size) {
    RlcError("error writing AMD PDU header");
    return 0;
  }

  // Copy PDU to payload
  srsran_assert((hdr_len + segment_payload_len) <= nof_bytes, "Error calculating hdr_len and segment_payload_len");
  memcpy(&payload[hdr_len], tx_pdu.sdu_buf->msg, segment_payload_len);

  // Store Segment Info
  rlc_amd_tx_pdu_nr::pdu_segment segment_info;
  segment_info.payload_len = segment_payload_len;
  tx_pdu.segment_list.push_back(segment_info);
  return hdr_len + segment_payload_len;
}

/**
 * Build PDU segment for an RLC SDU that is already on-going segmentation.
 *
 * \param [tx_pdu] is the tx_pdu info contained in the tx_window.
 * \param [payload] is a pointer to the MAC buffer that will hold the PDU segment.
 * \param [nof_bytes] is the number of bytes the RLC is allowed to fill.
 *
 * \returns the number of bytes written to the payload buffer.
 * \remark: This functions assumes that the SDU has already been copied to tx_pdu.sdu_buf.
 */
uint32_t rlc_am_nr_tx::build_continuation_sdu_segment(rlc_amd_tx_pdu_nr& tx_pdu, uint8_t* payload, uint32_t nof_bytes)
{
  RlcInfo("continuing SDU segment. SN=%d, Tx SDU (%d B), nof_bytes=%d B ",
          sdu_under_segmentation_sn,
          tx_pdu.sdu_buf->N_bytes,
          nof_bytes);

  // Sanity check: is there an initial SDU segment?
  if (tx_pdu.segment_list.empty()) {
    RlcError("build_continuation_sdu_segment was called, but there was no initial segment. SN=%d, Tx SDU (%d B), "
             "nof_bytes=%d B ",
             sdu_under_segmentation_sn,
             tx_pdu.sdu_buf->N_bytes,
             nof_bytes);
    sdu_under_segmentation_sn = INVALID_RLC_SN;
    return 0;
  }

  // Sanity check: can this SDU be sent considering header overhead?
  if (nof_bytes <= max_hdr_size) { // Larger header size, as SO is present
    RlcInfo("cannot build new sdu_segment, there are not enough bytes allocated to tx header plus data. nof_bytes=%d, "
            "max_header_size=%d",
            nof_bytes,
            max_hdr_size);
    return 0;
  }

  // Can the rest of the SDU be sent on a single segment PDU?
  const rlc_amd_tx_pdu_nr::pdu_segment& seg       = tx_pdu.segment_list.back();
  uint32_t                              last_byte = seg.so + seg.payload_len;
  RlcDebug("continuing SDU segment. SN=%d, last byte transmitted %d", tx_pdu.rlc_sn, last_byte);

  // Sanity check: last byte must be smaller than SDU size
  if (last_byte > tx_pdu.sdu_buf->N_bytes) {
    RlcError(
        "last byte transmitted larger than SDU len. SDU len=%d B, last_byte=%d B", tx_pdu.sdu_buf->N_bytes, last_byte);
    return 0;
  }

  uint32_t          segment_payload_full_len = tx_pdu.sdu_buf->N_bytes - last_byte + max_hdr_size; // SO is included
  uint32_t          segment_payload_len      = tx_pdu.sdu_buf->N_bytes - last_byte;
  rlc_nr_si_field_t si                       = {};

  if (segment_payload_full_len > nof_bytes) {
    RlcInfo("grant is not large enough for full SDU. "
            "SDU bytes left %d, nof_bytes %d, ",
            segment_payload_full_len,
            nof_bytes);
    si                       = rlc_nr_si_field_t::neither_first_nor_last_segment;
    segment_payload_len      = nof_bytes - max_hdr_size;
    segment_payload_full_len = nof_bytes;
  } else {
    RlcInfo("grant is large enough for full SDU."
            "SDU bytes left %d, nof_bytes %d, ",
            segment_payload_full_len,
            nof_bytes);
    si                        = rlc_nr_si_field_t::last_segment;
    sdu_under_segmentation_sn = INVALID_RLC_SN;
  }

  // Prepare header
  rlc_am_nr_pdu_header_t hdr = {};
  hdr.dc                     = RLC_DC_FIELD_DATA_PDU;
  hdr.p                      = get_pdu_poll(st.tx_next, false, segment_payload_len);
  hdr.si                     = si;
  hdr.sn_size                = cfg.tx_sn_field_length;
  hdr.sn                     = st.tx_next;
  hdr.so                     = last_byte;
  tx_pdu.header              = hdr;
  log_rlc_am_nr_pdu_header_to_string(logger.info, hdr, rb_name);

  // Write header
  uint32_t hdr_len = rlc_am_nr_write_data_pdu_header(hdr, payload);
  if (hdr_len >= nof_bytes || hdr_len != max_hdr_size) {
    RlcError("error writing AMD PDU header");
    return 0;
  }

  // Copy PDU to payload
  srsran_assert((hdr_len + segment_payload_len) <= nof_bytes, "Error calculating hdr_len and segment_payload_len");
  memcpy(&payload[hdr_len], &tx_pdu.sdu_buf->msg[last_byte], segment_payload_len);

  // Store PDU segment info into tx_window
  rlc_amd_tx_pdu_nr::pdu_segment segment_info = {};
  segment_info.so                             = last_byte;
  segment_info.payload_len                    = segment_payload_len;
  tx_pdu.segment_list.push_back(segment_info);

  if (si == rlc_nr_si_field_t::neither_first_nor_last_segment) {
    RlcInfo("grant is not large enough for full SDU."
            "Storing SDU segment info");
  } else {
    RlcInfo("grant is large enough for full SDU."
            "Removing current SDU info");
    // SDU is fully TX'ed. Increment TX_NEXT
    st.tx_next = (st.tx_next + 1) % mod_nr;
  }

  return hdr_len + segment_payload_len;
}

/**
 * Builds a retx RLC PDU.
 *
 * This will use the retx_queue to get information about the RLC PDU
 * being retx'ed. The retx may have been previously transmitted as
 * a full SDU or an SDU segment.
 *
 * \param [tx_pdu] is the tx_pdu info contained in the tx_window.
 * \param [payload] is a pointer to the MAC buffer that will hold the PDU segment.
 * \param [nof_bytes] is the number of bytes the RLC is allowed to fill.
 *
 * \returns the number of bytes written to the payload buffer.
 * \remark: This functions assumes that the SDU has already been copied to tx_pdu.sdu_buf.
 */
uint32_t rlc_am_nr_tx::build_retx_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  // Check there is at least 1 element before calling front()
  if (retx_queue.empty()) {
    RlcError("in build_retx_pdu(): retx_queue is empty");
    return 0;
  }

  rlc_amd_retx_nr_t& retx = retx_queue.front();

  // Sanity check - drop any retx SNs not present in tx_window
  while (not tx_window->has_sn(retx.sn)) {
    RlcInfo("SN=%d not in tx window, probably already ACKed. Skip and remove from retx queue", retx.sn);
    retx_queue.pop();
    if (!retx_queue.empty()) {
      retx = retx_queue.front();
    } else {
      RlcInfo("empty retx queue, cannot provide any retx PDU");
      return 0;
    }
  }

  RlcDebug("RETX - SN=%d, is_segment=%s, current_so=%d, so_start=%d, segment_length=%d",
           retx.sn,
           retx.is_segment ? "true" : "false",
           retx.current_so,
           retx.so_start,
           retx.segment_length);

  // Is segmentation/re-segmentation required?
  bool segmentation_required = is_retx_segmentation_required(retx, nof_bytes);

  if (segmentation_required) {
    return build_retx_pdu_with_segmentation(retx, payload, nof_bytes);
  }
  return build_retx_pdu_without_segmentation(retx, payload, nof_bytes);
}

/**
 * Builds a retx RLC PDU, without requiring (re-)segmentation.
 *
 * The RETX PDU may be transporting a full SDU or an SDU segment.
 *
 * \param [retx] is the retx info contained in the retx_queue. This is passed by copy, to avoid
 *               issues when using retx after pop'ing it from the queue.
 * \param [payload] is a pointer to the MAC buffer that will hold the PDU segment.
 * \param [nof_bytes] is the number of bytes the RLC is allowed to fill.
 *
 * \returns the number of bytes written to the payload buffer.
 * \remark this function will not update the SI. This means that if the retx is of the last
 * SDU segment, the SI should already be of the `last_segment` type.
 */
uint32_t
rlc_am_nr_tx::build_retx_pdu_without_segmentation(const rlc_amd_retx_nr_t retx, uint8_t* payload, uint32_t nof_bytes)
{
  srsran_assert(tx_window->has_sn(retx.sn), "Called %s without checking retx SN", __FUNCTION__);
  srsran_assert(not is_retx_segmentation_required(retx, nof_bytes),
                "Called %s without checking if segmentation was required",
                __FUNCTION__);

  // Get tx_pdu info from tx_window
  rlc_amd_tx_pdu_nr& tx_pdu = (*tx_window)[retx.sn];

  // Get expected header and payload len
  uint32_t expected_hdr_len = get_retx_expected_hdr_len(retx);
  uint32_t retx_payload_len = retx.is_segment ? (retx.so_start + retx.segment_length - retx.current_so)
                                              : (*tx_window)[retx.sn].sdu_buf->N_bytes;
  srsran_assert(nof_bytes >= (expected_hdr_len + retx_payload_len),
                "Called %s but segmentation is required. nof_bytes=%d, expeced_hdr_len=%d, retx_payload_len=%d",
                __FUNCTION__,
                nof_bytes,
                expected_hdr_len,
                retx_payload_len);

  // Log RETX info
  RlcDebug("SDU%scan be fully re-transmitted. SN=%d, nof_bytes=%d, expected_hdr_len=%d, "
           "current_so=%d, so_start=%d, segment_length=%d",
           retx.is_segment ? " segment " : " ",
           retx.sn,
           nof_bytes,
           expected_hdr_len,
           retx.current_so,
           retx.so_start,
           retx.segment_length);

  // Get RETX SN, current SO and SI
  rlc_nr_si_field_t si = rlc_nr_si_field_t::full_sdu;
  if (retx.is_segment) {
    if (retx.current_so == 0) {
      si = rlc_nr_si_field_t::first_segment;
    } else if ((retx.current_so + retx_payload_len) < tx_pdu.sdu_buf->N_bytes) {
      si = rlc_nr_si_field_t::neither_first_nor_last_segment;
    } else {
      si = rlc_nr_si_field_t::last_segment;
    }
  }

  // Get RETX PDU payload size
  uint32_t retx_pdu_payload_size = 0;
  if (not retx.is_segment) {
    // RETX full SDU
    retx_pdu_payload_size = (*tx_window)[retx.sn].sdu_buf->N_bytes;
  } else {
    // RETX SDU segment
    retx_pdu_payload_size = (retx.so_start + retx.segment_length - retx.current_so);
  }

  // Update RETX queue. This must be done before calculating
  // the polling bit, to make sure the poll bit is calculated correctly
  retx_queue.pop();

  // Write header to payload
  rlc_am_nr_pdu_header_t new_header = tx_pdu.header;
  new_header.si                     = si;
  new_header.so                     = retx.current_so;
  new_header.p                      = get_pdu_poll(retx.sn, true, 0);
  uint32_t hdr_len                  = rlc_am_nr_write_data_pdu_header(new_header, payload);

  // Write SDU/SDU segment to payload
  uint32_t pdu_bytes = hdr_len + retx_pdu_payload_size;
  srsran_assert(pdu_bytes <= nof_bytes, "Error calculating hdr_len and pdu_payload_len");
  memcpy(&payload[hdr_len], &tx_pdu.sdu_buf->msg[retx.current_so], retx_pdu_payload_size);

  // Log RETX
  RlcHexInfo((*tx_window)[retx.sn].sdu_buf->msg,
             (*tx_window)[retx.sn].sdu_buf->N_bytes,
             "Original SDU SN=%d (%d B) (attempt %d/%d)",
             retx.sn,
             (*tx_window)[retx.sn].sdu_buf->N_bytes,
             (*tx_window)[retx.sn].retx_count + 1,
             cfg.max_retx_thresh);
  RlcHexInfo(payload, pdu_bytes, "RETX PDU SN=%d (%d B)", retx.sn, pdu_bytes);
  log_rlc_am_nr_pdu_header_to_string(logger.debug, new_header, rb_name);

  debug_state();
  return pdu_bytes;
}

/**
 * Builds a retx RLC PDU that requires (re-)segmentation.
 *
 * \param [tx_pdu] is the tx_pdu info contained in the tx_window.
 * \param [payload] is a pointer to the MAC buffer that will hold the PDU segment.
 * \param [nof_bytes] is the number of bytes the RLC is allowed to fill.
 *
 * \returns the number of bytes written to the payload buffer.
 * \remark: This functions assumes that the SDU has already been copied to tx_pdu.sdu_buf.
 */
uint32_t rlc_am_nr_tx::build_retx_pdu_with_segmentation(rlc_amd_retx_nr_t& retx, uint8_t* payload, uint32_t nof_bytes)
{
  // Get tx_pdu info from tx_window
  srsran_assert(tx_window->has_sn(retx.sn), "Called %s without checking retx SN", __FUNCTION__);
  srsran_assert(is_retx_segmentation_required(retx, nof_bytes),
                "Called %s without checking if segmentation was not required",
                __FUNCTION__);

  rlc_amd_tx_pdu_nr& tx_pdu = (*tx_window)[retx.sn];

  // Is this an SDU segment or a full SDU?
  if (not retx.is_segment) {
    RlcDebug("Creating SDU segment from full SDU. SN=%d Tx SDU (%d B), nof_bytes=%d B ",
             retx.sn,
             tx_pdu.sdu_buf->N_bytes,
             nof_bytes);

  } else {
    RlcDebug("Creating SDU segment from SDU segment. SN=%d, current_so=%d, so_start=%d, segment_length=%d",
             retx.sn,
             retx.current_so,
             retx.so_start,
             retx.segment_length);
  }

  uint32_t          expected_hdr_len = min_hdr_size;
  rlc_nr_si_field_t si               = rlc_nr_si_field_t::first_segment;
  if (retx.current_so != 0) {
    si               = rlc_nr_si_field_t::neither_first_nor_last_segment;
    expected_hdr_len = max_hdr_size;
  }

  // Sanity check: are there enough bytes for header plus data?
  if (nof_bytes <= expected_hdr_len) {
    RlcInfo("Not enough bytes for RETX payload plus header. SN=%d, nof_bytes=%d, hdr_len=%d",
            retx.sn,
            nof_bytes,
            expected_hdr_len);
    return 0;
  }

  // Sanity check: could this have been transmitted without segmentation?
  if (nof_bytes > (tx_pdu.sdu_buf->N_bytes + expected_hdr_len)) {
    RlcError("called %s, but there are enough bytes to avoid segmentation. SN=%d", __FUNCTION__, retx.sn);
    return 0;
  }

  // Can the RETX PDU be transmitted in a single PDU?
  uint32_t retx_pdu_payload_size = nof_bytes - expected_hdr_len;

  // Write header
  rlc_am_nr_pdu_header_t hdr = tx_pdu.header;
  hdr.p                      = get_pdu_poll(retx.sn, true, 0);
  hdr.so                     = retx.current_so;
  hdr.si                     = si;
  uint32_t hdr_len           = rlc_am_nr_write_data_pdu_header(hdr, payload);
  if (hdr_len >= nof_bytes || hdr_len != expected_hdr_len) {
    log_rlc_am_nr_pdu_header_to_string(logger.error, hdr, rb_name);
    RlcError("Error writing AMD PDU header. nof_bytes=%d, hdr_len=%d", nof_bytes, hdr_len);
    return 0;
  }
  log_rlc_am_nr_pdu_header_to_string(logger.info, hdr, rb_name);

  // Copy SDU segment into payload
  srsran_assert((hdr_len + retx_pdu_payload_size) <= nof_bytes, "Error calculating hdr_len and segment_payload_len");
  memcpy(&payload[hdr_len], &tx_pdu.sdu_buf->msg[retx.current_so], retx_pdu_payload_size);

  // Store PDU segment info into tx_window
  RlcDebug("Updating RETX segment info. SN=%d, is_segment=%s", retx.sn, retx.is_segment ? "true" : "false");
  if (!retx.is_segment) {
    // Retx is not a segment yet
    rlc_amd_tx_pdu_nr::pdu_segment seg1 = {};
    seg1.so                             = retx.current_so;
    seg1.payload_len                    = retx_pdu_payload_size;
    rlc_amd_tx_pdu_nr::pdu_segment seg2 = {};
    seg2.so                             = retx.current_so + retx_pdu_payload_size;
    seg2.payload_len                    = retx.segment_length - retx_pdu_payload_size;
    tx_pdu.segment_list.push_back(seg1);
    tx_pdu.segment_list.push_back(seg2);
    RlcDebug("New segment: SN=%d, SO=%d len=%d", retx.sn, seg1.so, seg1.payload_len);
    RlcDebug("New segment: SN=%d, SO=%d len=%d", retx.sn, seg2.so, seg2.payload_len);
  } else {
    // Retx is already a segment
    // Find current segment in segment list.
    std::list<rlc_amd_tx_pdu_nr::pdu_segment>::iterator it;
    for (it = tx_pdu.segment_list.begin(); it != tx_pdu.segment_list.end(); ++it) {
      if (it->so == retx.current_so) {
        break;
      }
    }
    if (it != tx_pdu.segment_list.end()) {
      rlc_amd_tx_pdu_nr::pdu_segment seg1 = {};
      seg1.so                             = it->so;
      seg1.payload_len                    = retx_pdu_payload_size;
      rlc_amd_tx_pdu_nr::pdu_segment seg2 = {};
      seg2.so                             = it->so + retx_pdu_payload_size;
      seg2.payload_len                    = it->payload_len - retx_pdu_payload_size;

      std::list<rlc_amd_tx_pdu_nr::pdu_segment>::iterator begin_it   = tx_pdu.segment_list.erase(it);
      std::list<rlc_amd_tx_pdu_nr::pdu_segment>::iterator insert_it  = tx_pdu.segment_list.insert(begin_it, seg2);
      std::list<rlc_amd_tx_pdu_nr::pdu_segment>::iterator insert_it2 = tx_pdu.segment_list.insert(insert_it, seg1);
      RlcDebug("Old segment SN=%d, SO=%d len=%d", retx.sn, retx.current_so, retx.segment_length);
      RlcDebug("New segment SN=%d, SO=%d len=%d", retx.sn, seg1.so, seg1.payload_len);
      RlcDebug("New segment SN=%d, SO=%d len=%d", retx.sn, seg2.so, seg2.payload_len);
    } else {
      RlcDebug("Could not find segment. SN=%d, SO=%d length=%d", retx.sn, retx.current_so, retx.segment_length);
    }
  }

  // Update retx queue
  retx.is_segment = true;
  retx.current_so = retx.current_so + retx_pdu_payload_size;

  RlcDebug("Updated RETX info. is_segment=%s, current_so=%d, so_start=%d, segment_length=%d",
           retx.is_segment ? "true" : "false",
           retx.current_so,
           retx.so_start,
           retx.segment_length);

  if (retx.current_so >= tx_pdu.sdu_buf->N_bytes) {
    RlcError("Current SO larger or equal to SDU size when creating SDU segment. SN=%d, current SO=%d, SO_start=%d, "
             "segment_length=%d",
             retx.sn,
             retx.current_so,
             retx.so_start,
             retx.segment_length);
    return 0;
  }

  if (retx.current_so >= retx.so_start + retx.segment_length) {
    RlcError("Current SO larger than SO_start + segment_length. SN=%d, current SO=%d, SO_start=%d, segment_length=%s",
             retx.sn,
             retx.current_so,
             retx.so_start,
             retx.segment_length);
    return 0;
  }

  return hdr_len + retx_pdu_payload_size;
}

bool rlc_am_nr_tx::is_retx_segmentation_required(const rlc_amd_retx_nr_t& retx, uint32_t nof_bytes)
{
  bool segmentation_required = false;
  if (retx.is_segment) {
    uint32_t expected_hdr_size = retx.current_so == 0 ? min_hdr_size : max_hdr_size;
    if (nof_bytes < ((retx.so_start + retx.segment_length - retx.current_so) + expected_hdr_size)) {
      RlcInfo("Re-segmentation required for RETX. SN=%d", retx.sn);
      segmentation_required = true;
    }
  } else {
    if (nof_bytes < ((*tx_window)[retx.sn].sdu_buf->N_bytes + min_hdr_size)) {
      RlcInfo("Segmentation required for RETX. SN=%d", retx.sn);
      segmentation_required = true;
    }
  }
  return segmentation_required;
}

uint32_t rlc_am_nr_tx::get_retx_expected_hdr_len(const rlc_amd_retx_nr_t& retx)
{
  uint32_t expected_hdr_len = min_hdr_size;
  if (retx.is_segment && retx.current_so != 0) {
    expected_hdr_len = max_hdr_size;
  }
  return expected_hdr_len;
}

uint32_t rlc_am_nr_tx::build_status_pdu(byte_buffer_t* payload, uint32_t nof_bytes)
{
  RlcInfo("generating status PDU. Bytes available:%d", nof_bytes);
  rlc_am_nr_status_pdu_t status(cfg.rx_sn_field_length); // carries status of RX entity, hence use SN length of RX
  int                    pdu_len = rx->get_status_pdu(&status, nof_bytes);
  if (pdu_len == SRSRAN_ERROR) {
    RlcDebug("deferred status PDU. Cause: Failed to acquire rx lock");
    pdu_len = 0;
  } else if (pdu_len > 0 && nof_bytes >= static_cast<uint32_t>(pdu_len)) {
    RlcDebug("generated status PDU. Bytes:%d", pdu_len);
    log_rlc_am_nr_status_pdu_to_string(logger.info, "TX status PDU - %s", &status, rb_name);
    pdu_len = rlc_am_nr_write_status_pdu(status, cfg.tx_sn_field_length, payload);
  } else {
    RlcInfo("cannot tx status PDU - %d bytes available, %d bytes required", nof_bytes, pdu_len);
    pdu_len = 0;
  }

  return payload->N_bytes;
}

void rlc_am_nr_tx::handle_control_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  if (not tx_enabled) {
    return;
  }

  std::lock_guard<std::mutex> lock(mutex);
  rlc_am_nr_status_pdu_t      status(cfg.tx_sn_field_length);
  RlcHexDebug(payload, nof_bytes, "%s Rx control PDU", parent->rb_name);
  rlc_am_nr_read_status_pdu(payload, nof_bytes, cfg.tx_sn_field_length, &status);
  log_rlc_am_nr_status_pdu_to_string(logger.info, "RX status PDU: %s", &status, parent->rb_name);

  /*
   * Sanity check the received status report.
   * Checking if the ACK_SN is inside the valid ACK_SN window (the TX window "off-by-one")
   * makes sure we discard out of order status reports.
   * Checking if ACK_SN > Tx_Next + 1 makes sure we do not receive a ACK/NACK for something we did not TX
   * ACK_SN may be equal to TX_NEXT + 1, if not all SDU segments with SN=TX_NEXT have been transmitted.
   */
  if (not valid_ack_sn(status.ack_sn)) {
    RlcInfo("Received ACK with SN outside of TX_WINDOW, ignoring status report. ACK_SN=%d, TX_NEXT_ACK=%d.",
            status.ack_sn,
            st.tx_next_ack);
    info_state();
    return;
  }

  if (tx_mod_base_nr(status.ack_sn) > tx_mod_base_nr(st.tx_next + 1)) {
    RlcWarning("Received ACK with SN larger than TX_NEXT, ignoring status report.  SN=%d, TX_NEXT_ACK=%d, TX_NEXT=%d",
               status.ack_sn,
               st.tx_next_ack,
               st.tx_next);
    info_state();
    return;
  }

  /**
   * Section 5.3.3.3: Reception of a STATUS report
   * - if the STATUS report comprises a positive or negative acknowledgement for the RLC SDU with sequence
   *   number equal to POLL_SN:
   *   - if t-PollRetransmit is running:
   *     - stop and reset t-PollRetransmit.
   */
  if (tx_mod_base_nr(st.poll_sn) < tx_mod_base_nr(status.ack_sn)) {
    if (poll_retransmit_timer.is_running()) {
      RlcDebug("Received ACK or NACK for POLL_SN=%d. Stopping t-PollRetransmit", st.poll_sn);
      poll_retransmit_timer.stop();
    } else {
      RlcDebug("Received ACK or NACK for POLL_SN=%d. t-PollRetransmit already stopped", st.poll_sn);
    }
  } else {
    RlcDebug("POLL_SN=%d > ACK_SN=%d. Not stopping t-PollRetransmit ", st.poll_sn, status.ack_sn);
  }

  /*
   * - if the SN of the corresponding RLC SDU falls within the range
   *   TX_Next_Ack <= SN < = the highest SN of the AMD PDU among the AMD PDUs submitted to lower layer:
   *   - consider the RLC SDU or the RLC SDU segment for which a negative acknowledgement was received for
   *     retransmission.
   */
  // Process ACKs
  uint32_t stop_sn = status.nacks.size() == 0
                         ? status.ack_sn
                         : status.nacks[0].nack_sn; // Stop processing ACKs at the first NACK, if it exists.
  for (uint32_t sn = st.tx_next_ack; tx_mod_base_nr(sn) < tx_mod_base_nr(stop_sn); sn = (sn + 1) % mod_nr) {
    if (tx_window->has_sn(sn)) {
      notify_info_vec.push_back((*tx_window)[sn].pdcp_sn);
      retx_queue.remove_sn(sn); // remove any pending retx for that SN
      tx_window->remove_pdu(sn);
      st.tx_next_ack = (sn + 1) % mod_nr;
    } else {
      RlcError("Missing ACKed SN from TX window");
      break;
    }
  }
  RlcDebug("Processed status report ACKs. ACK_SN=%d. Tx_Next_Ack=%d", status.ack_sn, st.tx_next_ack);

  // Process N_nacks
  std::set<uint32_t> retx_sn_set; // Set of PDU SNs added for retransmission (no duplicates)
  for (uint32_t nack_idx = 0; nack_idx < status.nacks.size(); nack_idx++) {
    if (status.nacks[nack_idx].has_nack_range) {
      for (uint32_t range_sn = status.nacks[nack_idx].nack_sn;
           range_sn < status.nacks[nack_idx].nack_sn + status.nacks[nack_idx].nack_range;
           range_sn++) {
        rlc_status_nack_t nack = {};
        nack.nack_sn           = range_sn;
        if (status.nacks[nack_idx].has_so) {
          // Apply so_start to first range item
          if (range_sn == status.nacks[nack_idx].nack_sn) {
            nack.so_start = status.nacks[nack_idx].so_start;
          }
          // Apply so_end to last range item
          if (range_sn == (status.nacks[nack_idx].nack_sn + status.nacks[nack_idx].nack_range - 1)) {
            nack.so_end = status.nacks[nack_idx].so_end;
          }
          // Enable has_so only if the offsets do not span the whole SDU
          nack.has_so = (nack.so_start != 0) || (nack.so_end != rlc_status_nack_t::so_end_of_sdu);
        }
        handle_nack(nack, retx_sn_set);
      }
    } else {
      handle_nack(status.nacks[nack_idx], retx_sn_set);
    }
  }

  // Process retx_count and inform upper layers if needed
  for (uint32_t retx_sn : retx_sn_set) {
    auto& pdu = (*tx_window)[retx_sn];
    // Increment retx_count
    if (pdu.retx_count == RETX_COUNT_NOT_STARTED) {
      // Set retx_count = 0 on first RE-transmission of associated SDU (38.322 Sec. 5.3.2)
      pdu.retx_count = 0;
    } else {
      // Increment otherwise
      pdu.retx_count++;
    }

    // Inform upper layers if needed
    check_sn_reached_max_retx(retx_sn);
  }

  // Notify PDCP
  if (not notify_info_vec.empty()) {
    parent->pdcp->notify_delivery(parent->lcid, notify_info_vec);
  }
  notify_info_vec.clear();
}

void rlc_am_nr_tx::handle_nack(const rlc_status_nack_t& nack, std::set<uint32_t>& retx_sn_set)
{
  if (tx_mod_base_nr(st.tx_next_ack) <= tx_mod_base_nr(nack.nack_sn) &&
      tx_mod_base_nr(nack.nack_sn) <= tx_mod_base_nr(st.tx_next)) {
    RlcDebug("Handling NACK for SN=%d", nack.nack_sn);
    if (tx_window->has_sn(nack.nack_sn)) {
      auto& pdu = (*tx_window)[nack.nack_sn];

      if (nack.has_so) {
        // NACK'ing missing bytes in SDU segment.
        // Retransmit all SDU segments within those missing bytes.
        if (pdu.segment_list.empty()) {
          RlcError("Received NACK with SO, but there is no segment information. SN=%d", nack.nack_sn);
        }
        bool segment_found = false;
        for (const rlc_amd_tx_pdu_nr::pdu_segment& segm : pdu.segment_list) {
          if (segm.so >= nack.so_start && segm.so <= nack.so_end) {
            if (not retx_queue.has_sn(nack.nack_sn, segm.so)) {
              rlc_amd_retx_nr_t& retx = retx_queue.push();
              retx.sn                 = nack.nack_sn;
              retx.is_segment         = true;
              retx.so_start           = segm.so;
              retx.current_so         = segm.so;
              retx.segment_length     = segm.payload_len;
              retx_sn_set.insert(nack.nack_sn);
              RlcInfo("Scheduled RETX of SDU segment SN=%d, so_start=%d, segment_length=%d",
                      retx.sn,
                      retx.so_start,
                      retx.segment_length);
            } else {
              RlcInfo("Skip already scheduled RETX of SDU segment SN=%d, so_start=%d, segment_length=%d",
                      nack.nack_sn,
                      segm.so,
                      segm.payload_len);
            }
            segment_found = true;
          }
        }
        if (!segment_found) {
          RlcWarning("Could not find segment for NACK_SN=%d. SO_start=%d, SO_end=%d",
                     nack.nack_sn,
                     nack.so_start,
                     nack.so_end);
          for (const rlc_amd_tx_pdu_nr::pdu_segment& segm : pdu.segment_list) {
            RlcDebug("Segments for SN=%d. SO=%d, SO_end=%d", nack.nack_sn, segm.so, segm.payload_len);
          }
        }
      } else {
        // NACK'ing full SDU.
        // add to retx queue if it's not already there
        if (not retx_queue.has_sn(nack.nack_sn)) {
          // Have we segmented the SDU already?
          if ((*tx_window)[nack.nack_sn].segment_list.empty()) {
            rlc_amd_retx_nr_t& retx = retx_queue.push();
            retx.sn                 = nack.nack_sn;
            retx.is_segment         = false;
            retx.so_start           = 0;
            retx.current_so         = 0;
            retx.segment_length     = pdu.sdu_buf->N_bytes;
            retx_sn_set.insert(nack.nack_sn);
            RlcInfo("Scheduled RETX of SDU SN=%d", retx.sn);
          } else {
            RlcInfo("Scheduled RETX of SDU SN=%d", nack.nack_sn);
            retx_sn_set.insert(nack.nack_sn);
            for (auto segm : (*tx_window)[nack.nack_sn].segment_list) {
              rlc_amd_retx_nr_t& retx = retx_queue.push();
              retx.sn                 = nack.nack_sn;
              retx.is_segment         = true;
              retx.so_start           = segm.so;
              retx.current_so         = segm.so;
              retx.segment_length     = segm.payload_len;
              RlcInfo("Scheduled RETX of SDU Segment. SN=%d, SO=%d, len=%d", retx.sn, segm.so, segm.payload_len);
            }
          }
        } else {
          RlcInfo("RETX queue already has NACK_SN. SDU SN=%d, Tx_Next_Ack=%d, Tx_Next=%d",
                  nack.nack_sn,
                  st.tx_next_ack,
                  st.tx_next);
        }
      }
    } else {
      RlcInfo("TX window does not contain NACK_SN. SDU SN=%d, Tx_Next_Ack=%d, Tx_Next=%d",
              nack.nack_sn,
              st.tx_next_ack,
              st.tx_next);
    } // TX window containts NACK SN
  } else {
    RlcInfo(
        "RETX not in expected range. SDU SN=%d, Tx_Next_Ack=%d, Tx_Next=%d", nack.nack_sn, st.tx_next_ack, st.tx_next);
  } // NACK SN within expected range
}
/**
 * Helper to check if a SN has reached the max reTx threshold
 *
 * Caller _must_ hold the mutex when calling the function.
 * If the retx has been reached for a SN the upper layers (i.e. RRC/PDCP) will be informed.
 * The SN is _not_ removed from the Tx window, so retransmissions of that SN can still occur.
 *
 *
 * @param  sn The SN of the PDU to check
 */
void rlc_am_nr_tx::check_sn_reached_max_retx(uint32_t sn)
{
  if ((*tx_window)[sn].retx_count == cfg.max_retx_thresh) {
    RlcWarning("Signaling max number of reTx=%d for SN=%d", (*tx_window)[sn].retx_count, sn);
    parent->rrc->max_retx_attempted();
    srsran::pdcp_sn_vector_t pdcp_sns;
    pdcp_sns.push_back((*tx_window)[sn].pdcp_sn);
    parent->pdcp->notify_failure(parent->lcid, pdcp_sns);

    std::lock_guard<std::mutex> lock(parent->metrics_mutex);
    parent->metrics.num_lost_pdus++;
  }
}

uint32_t rlc_am_nr_tx::get_buffer_state()
{
  uint32_t tx_queue      = 0;
  uint32_t prio_tx_queue = 0;
  get_buffer_state(tx_queue, prio_tx_queue);
  return tx_queue + prio_tx_queue;
}

void rlc_am_nr_tx::get_buffer_state(uint32_t& n_bytes_new, uint32_t& n_bytes_prio)
{
  std::lock_guard<std::mutex> lock(mutex);
  RlcDebug("buffer state - do_status=%s", do_status() ? "yes" : "no");

  if (!tx_enabled) {
    RlcError("get_buffer_state() failed: TX is not enabled.");
    return;
  }

  // Bytes needed for status report
  if (do_status()) {
    n_bytes_prio += rx->get_status_pdu_length();
    RlcDebug("buffer state - total status report: %d bytes", n_bytes_prio);
  }

  // Bytes needed for retx
  for (const rlc_amd_retx_nr_t& retx : retx_queue.get_inner_queue()) {
    RlcDebug("buffer state - retx - SN=%d, Segment: %s, %d:%d",
             retx.sn,
             retx.is_segment ? "true" : "false",
             retx.so_start,
             retx.so_start + retx.segment_length - 1);
    if (tx_window->has_sn(retx.sn)) {
      int req_bytes     = retx.segment_length;
      int hdr_req_bytes = (retx.is_segment && retx.current_so != 0) ? max_hdr_size : min_hdr_size;
      if (req_bytes <= 0) {
        RlcError("buffer state - retx - invalid length=%d for SN=%d", req_bytes, retx.sn);
      } else {
        n_bytes_prio += (req_bytes + hdr_req_bytes);
        RlcDebug("buffer state - retx: %d bytes", n_bytes_prio);
      }
    } else {
      RlcWarning("buffer state - retx for SN=%d is outside the tx_window", retx.sn);
    }
  }

  // Bytes needed for tx of the rest of the SDU that is currently under segmentation (if any)
  if (sdu_under_segmentation_sn != INVALID_RLC_SN) {
    if (tx_window->has_sn(sdu_under_segmentation_sn)) {
      rlc_amd_tx_pdu_nr& seg_pdu = (*tx_window)[sdu_under_segmentation_sn];
      if (not seg_pdu.segment_list.empty()) {
        // obtain amount of already transmitted Bytes
        const rlc_amd_tx_pdu_nr::pdu_segment& seg       = seg_pdu.segment_list.back();
        uint32_t                              last_byte = seg.so + seg.payload_len;
        if (last_byte <= seg_pdu.sdu_buf->N_bytes) {
          // compute remaining bytes pending for transmission
          uint32_t remaining_bytes = seg_pdu.sdu_buf->N_bytes - last_byte;
          n_bytes_new += remaining_bytes + max_hdr_size;
        } else {
          RlcError(
              "buffer state - last segment of SDU under segmentation exceeds SDU len. SDU len=%d B, last_byte=%d B",
              seg_pdu.sdu_buf->N_bytes,
              last_byte);
        }
      } else {
        RlcError("buffer state - SDU under segmentation has empty segment list. Ignoring SN=%d",
                 sdu_under_segmentation_sn);
      }
    } else {
      sdu_under_segmentation_sn = INVALID_RLC_SN;
      RlcError("buffer state - SDU under segmentation does not exist in tx_window. Aborting segmentation SN=%d",
               sdu_under_segmentation_sn);
    }
  }

  // Bytes needed for tx SDUs in queue
  uint32_t n_sdus = tx_sdu_queue.get_n_sdus();
  n_bytes_new += tx_sdu_queue.size_bytes();

  // Room needed for fixed header of data PDUs
  n_bytes_new += min_hdr_size * n_sdus;
  RlcDebug("total buffer state - %d SDUs (%d B)", n_sdus, n_bytes_new + n_bytes_prio);

  if (bsr_callback) {
    RlcDebug("calling BSR callback - %d new_tx, %d priority bytes", n_bytes_new, n_bytes_prio);
    bsr_callback(parent->lcid, n_bytes_new, n_bytes_prio);
  }
}

/*
 * Check whether the polling bit needs to be set, as specified in
 * TS 38.322, section 5.3.3.2
 */
uint8_t rlc_am_nr_tx::get_pdu_poll(uint32_t sn, bool is_retx, uint32_t sdu_bytes)
{
  RlcDebug("Checking poll bit requirements for PDU. SN=%d, retx=%s, sdu_bytes=%d, POLL_SN=%d",
           sn,
           is_retx ? "true" : "false",
           sdu_bytes,
           st.poll_sn);
  /* For each AMD PDU or AMD PDU segment that has not been previoulsy tranmitted:
   * - increment PDU_WITHOUT_POLL by one;
   * - increment BYTE_WITHOUT_POLL by every new byte of Data field element that it maps to the Data field of the AMD
   * PDU;
   *   - if PDU_WITHOUT_POLL >= pollPDU; or
   *   - if BYTE_WITHOUT_POLL >= pollByte:
   *   	- include a poll in the AMD PDU as described below.
   */
  uint8_t poll = 0;
  if (!is_retx) {
    st.pdu_without_poll++;
    st.byte_without_poll += sdu_bytes;
    if (cfg.poll_pdu > 0 && st.pdu_without_poll >= (uint32_t)cfg.poll_pdu) {
      poll = 1;
      RlcDebug("Setting poll bit due to PollPDU. SN=%d, POLL_SN=%d", sn, st.poll_sn);
    }
    if (cfg.poll_byte > 0 && st.byte_without_poll >= (uint32_t)cfg.poll_byte) {
      poll = 1;
      RlcDebug("Setting poll bit due to PollBYTE. SN=%d, POLL_SN=%d", sn, st.poll_sn);
    }
  }

  /*
   * - if both the transmission buffer and the retransmission buffer becomes empty
   *   (excluding transmitted RLC SDUs or RLC SDU segments awaiting acknowledgements)
   *   after the transmission of the AMD PDU; or
   * - if no new RLC SDU can be transmitted after the transmission of the AMD PDU (e.g. due to window stalling);
   *   - include a poll in the AMD PDU as described below.
   */
  if ((tx_sdu_queue.is_empty() && retx_queue.empty() && sdu_under_segmentation_sn == INVALID_RLC_SN) ||
      tx_window->full()) {
    RlcDebug("Setting poll bit due to empty buffers/inablity to TX. SN=%d, POLL_SN=%d", sn, st.poll_sn);
    poll = 1;
  }

  /*
   * - If poll bit is included:
   *     - set PDU_WITHOUT_POLL to 0;
   *     - set BYTE_WITHOUT_POLL to 0.
   */
  if (poll == 1) {
    st.pdu_without_poll  = 0;
    st.byte_without_poll = 0;
    /*
     * - set POLL_SN to the highest SN of the AMD PDU among the AMD PDUs submitted to lower layer;
     * - if t-PollRetransmit is not running:
     *   - start t-PollRetransmit.
     * - else:
     *   - restart t-PollRetransmit.
     */
    if (!is_retx) {
      // This is not an RETX, but a new transmission
      // As such it should be the highest SN submitted to the lower layers
      st.poll_sn = sn;
      RlcDebug("Setting new POLL_SN. POLL_SN=%d", sn);
    }
    if (cfg.t_poll_retx > 0) {
      if (not poll_retransmit_timer.is_running()) {
        poll_retransmit_timer.run();
      } else {
        poll_retransmit_timer.stop();
        poll_retransmit_timer.run();
      }
      RlcInfo("Started t-PollRetransmit. POLL_SN=%d", st.poll_sn);
    }
  }
  return poll;
}

bool rlc_am_nr_tx::do_status()
{
  return rx->get_do_status();
}

void rlc_am_nr_tx::reestablish()
{
  stop();
}

void rlc_am_nr_tx::empty_queue()
{
  std::lock_guard<std::mutex> lock(mutex);
  empty_queue_no_lock();
}

void rlc_am_nr_tx::empty_queue_no_lock()
{
  // deallocate all SDUs in transmit queue
  while (tx_sdu_queue.size() > 0) {
    unique_byte_buffer_t buf = tx_sdu_queue.read();
  }
}

void rlc_am_nr_tx::stop()
{
  std::lock_guard<std::mutex> lock(mutex);
  empty_queue_no_lock();

  if (parent->timers != nullptr && poll_retransmit_timer.is_valid()) {
    poll_retransmit_timer.stop();
  }

  st = {};

  sdu_under_segmentation_sn = INVALID_RLC_SN;

  // Drop all messages in TX window
  tx_window->clear();

  // Drop all messages in RETX queue
  retx_queue.clear();

  tx_enabled = false;
}

void rlc_am_nr_tx::timer_expired(uint32_t timeout_id)
{
  std::unique_lock<std::mutex> lock(mutex);

  // t-PollRetransmit
  if (poll_retransmit_timer.is_valid() && poll_retransmit_timer.id() == timeout_id) {
    RlcDebug("Poll retransmission timer expired after %dms", poll_retransmit_timer.duration());
    debug_state();
    /*
     * - if both the transmission buffer and the retransmission buffer are empty
     *   (excluding transmitted RLC SDU or RLC SDU segment awaiting acknowledgements); or
     * - if no new RLC SDU or RLC SDU segment can be transmitted (e.g. due to window stalling):
     *   - consider the RLC SDU with the highest SN among the RLC SDUs submitted to lower layer for
     *   retransmission; or
     *   - consider any RLC SDU which has not been positively acknowledged for retransmission.
     * - include a poll in an AMD PDU as described in section 5.3.3.2.
     */
    if ((tx_sdu_queue.is_empty() && retx_queue.empty()) || tx_window->full()) {
      if (tx_window->empty()) {
        RlcError("t-PollRetransmit expired, but the tx_window is empty. POLL_SN=%d, Tx_Next_Ack=%d, tx_window_size=%d",
                 st.poll_sn,
                 st.tx_next_ack,
                 tx_window->size());
        return;
      }
      if (not tx_window->has_sn(st.tx_next_ack)) {
        RlcError("t-PollRetransmit expired, but Tx_Next_Ack is not in the tx_widow. POLL_SN=%d, Tx_Next_Ack=%d, "
                 "tx_window_size=%d",
                 st.poll_sn,
                 st.tx_next_ack,
                 tx_window->size());
        return;
      }
      // RETX first RLC SDU that has not been ACKed
      // or first SDU segment of the first RLC SDU
      // that has not been acked
      rlc_amd_retx_nr_t& retx = retx_queue.push();
      retx.sn                 = st.tx_next_ack;
      if ((*tx_window)[st.tx_next_ack].segment_list.empty()) {
        // Full SDU
        retx.is_segment     = false;
        retx.so_start       = 0;
        retx.segment_length = (*tx_window)[st.tx_next_ack].sdu_buf->N_bytes;
        retx.current_so     = 0;
      } else {
        // To make sure we do not mess up the segment list
        // We RETX an SDU segment instead of the full SDU
        // if the SDU has been segmented before.
        // As we cannot know which segments have been ACKed before
        // we simply RETX the first one.
        retx.is_segment     = true;
        retx.so_start       = 0;
        retx.current_so     = 0;
        retx.segment_length = (*tx_window)[st.tx_next_ack].segment_list.begin()->payload_len;
      }
      RlcDebug("Retransmission because of t-PollRetransmit. RETX SN=%d, is_segment=%s, so_start=%d, segment_length=%d",
               retx.sn,
               retx.is_segment ? "true" : "false",
               retx.so_start,
               retx.segment_length);
    }
    return;
  }
}

/*
 * Window helpers
 */
uint32_t rlc_am_nr_tx::tx_mod_base_nr(uint32_t sn) const
{
  return (sn - st.tx_next_ack) % mod_nr;
}

uint32_t rlc_am_nr_tx::tx_window_size() const
{
  return am_window_size(cfg.tx_sn_field_length);
}

bool rlc_am_nr_tx::inside_tx_window(uint32_t sn) const
{
  // TX_Next_Ack <= SN < TX_Next_Ack + AM_Window_Size
  return tx_mod_base_nr(sn) < tx_window_size();
}

/*
 * This function is used to check if a received status report
 * as a valid ACK_SN.
 *
 * ACK_SN may be equal to TX_NEXT + AM_Window_Size if the PDU
 * with SN=TX_NEXT+AM_Window_Size has been received by the RX
 * An ACK_SN == Tx_Next_Ack doesn't ACK or NACKs any PDUs, as
 * such, such a status report can be discarded.
 */
bool rlc_am_nr_tx::valid_ack_sn(uint32_t sn) const
{
  // Tx_Next_Ack < SN <= TX_Next + AM_Window_Size
  return (0 < tx_mod_base_nr(sn)) && (tx_mod_base_nr(sn) <= tx_window_size());
}

/*
 * Debug Helpers
 */
void rlc_am_nr_tx::debug_state() const
{
  RlcDebug("TX entity state: Tx_Next_Ack=%d, Tx_Next=%d, POLL_SN=%d, PDU_WITHOUT_POLL=%d, BYTE_WITHOUT_POLL=%d",
           st.tx_next_ack,
           st.tx_next,
           st.poll_sn,
           st.pdu_without_poll,
           st.byte_without_poll);
}

void rlc_am_nr_tx::info_state() const
{
  RlcInfo("TX window state: SDUs %d", tx_window->size());
  RlcInfo("TX entity state: Tx_Next_Ack=%d, Tx_Next=%d, POLL_SN=%d, PDU_WITHOUT_POLL=%d, BYTE_WITHOUT_POLL=%d",
          st.tx_next_ack,
          st.tx_next,
          st.poll_sn,
          st.pdu_without_poll,
          st.byte_without_poll);
}

void rlc_am_nr_tx::debug_window() const
{
  RlcDebug("TX window state: Tx_Next_Ack=%d, Tx_Next=%d, SDUs=%d", st.tx_next_ack, st.tx_next, tx_window->size());
}
/****************************************************************************
 * Rx subclass implementation
 ***************************************************************************/
rlc_am_nr_rx::rlc_am_nr_rx(rlc_am* parent_) :
  parent(parent_),
  pool(byte_buffer_pool::get_instance()),
  status_prohibit_timer(parent->timers->get_unique_timer()),
  reassembly_timer(parent->timers->get_unique_timer()),
  rlc_am_base_rx(parent_, parent_->logger)
{}

bool rlc_am_nr_rx::configure(const rlc_config_t& cfg_)
{
  cfg     = cfg_.am_nr;
  rb_name = parent->rb_name;
  // Configure status prohibit timer
  if (cfg.t_status_prohibit > 0) {
    status_prohibit_timer.set(static_cast<uint32_t>(cfg.t_status_prohibit),
                              [this](uint32_t timerid) { timer_expired(timerid); });
  }

  // Configure t_reassembly timer
  if (cfg.t_reassembly > 0) {
    reassembly_timer.set(static_cast<uint32_t>(cfg.t_reassembly), [this](uint32_t timerid) { timer_expired(timerid); });
  }

  mod_nr = cardinality(cfg.rx_sn_field_length);
  switch (cfg.rx_sn_field_length) {
    case rlc_am_nr_sn_size_t::size12bits:
      rx_window = std::unique_ptr<rlc_ringbuffer_base<rlc_amd_rx_sdu_nr_t> >(
          new rlc_ringbuffer_t<rlc_amd_rx_sdu_nr_t, am_window_size(rlc_am_nr_sn_size_t::size12bits)>);
      break;
    case rlc_am_nr_sn_size_t::size18bits:
      rx_window = std::unique_ptr<rlc_ringbuffer_base<rlc_amd_rx_sdu_nr_t> >(
          new rlc_ringbuffer_t<rlc_amd_rx_sdu_nr_t, am_window_size(rlc_am_nr_sn_size_t::size18bits)>);
      break;
    default:
      RlcError("attempt to configure unsupported rx_sn_field_length %s", to_string(cfg.rx_sn_field_length));
      return false;
  }

  RlcDebug("RLC AM NR configured rx entity.");

  return true;
}

void rlc_am_nr_rx::stop()
{
  std::lock_guard<std::mutex> lock(mutex);

  if (parent->timers != nullptr && reassembly_timer.is_valid()) {
    reassembly_timer.stop();
  }

  if (parent->timers != nullptr && status_prohibit_timer.is_valid()) {
    status_prohibit_timer.stop();
  }

  st = {};

  do_status = false;

  // Drop all messages in RX window
  rx_window->clear();
}

void rlc_am_nr_rx::reestablish()
{
  stop();
}

void rlc_am_nr_rx::handle_data_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  std::lock_guard<std::mutex> lock(mutex);

  // Get AMD PDU Header
  rlc_am_nr_pdu_header_t header  = {};
  uint32_t               hdr_len = rlc_am_nr_read_data_pdu_header(payload, nof_bytes, cfg.rx_sn_field_length, &header);

  RlcHexInfo(payload, nof_bytes, "Rx data PDU SN=%d (%d B)", header.sn, nof_bytes);
  log_rlc_am_nr_pdu_header_to_string(logger.debug, header, rb_name);

  // Trigger polling if poll bit is set.
  // We do this before checking if the PDU is inside the RX window,
  // as the RX window may have advanced without the TX having received the ACKs
  // This can cause a data stall, whereby the TX keeps retransmiting
  // a PDU outside of the Rx window.
  // Also, we do this before discarding duplicate SDUs/SDU segments
  // Because t-PollRetransmit may transmit a PDU that was already
  // received.
  if (header.p != 0U) {
    RlcInfo("status packet requested through polling bit");
    do_status = true;
  }

  // Check whether SDU is within Rx Window
  if (!inside_rx_window(header.sn)) {
    RlcInfo("SN=%d outside rx window [%d:%d] - discarding", header.sn, st.rx_next, st.rx_next + rx_window_size());
    return;
  }

  // Section 5.2.3.2.2, discard duplicate PDUs
  if (rx_window->has_sn(header.sn) && (*rx_window)[header.sn].fully_received) {
    RlcInfo("discarding duplicate SN=%d", header.sn);
    return;
  }

  // Section 5.2.3.2.2, discard segments with overlapping bytes
  if (rx_window->has_sn(header.sn) && header.si != rlc_nr_si_field_t::full_sdu) {
    for (const auto& segm : (*rx_window)[header.sn].segments) {
      uint32_t segm_last_byte = segm.header.so + segm.buf->N_bytes - 1;
      uint32_t pdu_last_byte  = header.so + nof_bytes - hdr_len - 1;
      if ((header.so >= segm.header.so && header.so <= segm_last_byte) ||
          (pdu_last_byte >= segm.header.so && pdu_last_byte <= segm_last_byte)) {
        RlcInfo("Got SDU segment with duplicate bytes. Discarding.");
        RlcInfo("Discarded SDU segment. SN=%d, SO=%d, last_byte=%d, payload=%d",
                header.sn,
                header.so,
                header.so + (nof_bytes - hdr_len),
                (nof_bytes - hdr_len));
        RlcInfo("Overlaping with SDU segment with SN=%d, SO=%d, last_byte=%d, payload=%d",
                header.sn,
                segm.header.so,
                segm_last_byte,
                segm.buf->N_bytes);
        return;
      }
    }
  }

  // Write to rx window either full SDU or SDU segment
  if (header.si == rlc_nr_si_field_t::full_sdu) {
    int err = handle_full_data_sdu(header, payload, nof_bytes);
    if (err != SRSRAN_SUCCESS) {
      return;
    }
  } else {
    int err = handle_segment_data_sdu(header, payload, nof_bytes);
    if (err != SRSRAN_SUCCESS) {
      return;
    }
  }

  debug_state();

  // 5.2.3.2.3 Actions when an AMD PDU is placed in the reception buffer
  /*
   * - if x >= RX_Next_Highest
   *   - update RX_Next_Highest to x+ 1.
   */
  if (rx_mod_base_nr(header.sn) >= rx_mod_base_nr(st.rx_next_highest)) {
    st.rx_next_highest = (header.sn + 1) % mod_nr;
  }

  /*
   * - if all bytes of the RLC SDU with SN = x are received:
   */
  if (rx_window->has_sn(header.sn) && (*rx_window)[header.sn].fully_received) {
    /*
     * - reassemble the RLC SDU from AMD PDU(s) with SN = x, remove RLC headers when doing so and deliver
     *   the reassembled RLC SDU to upper layer;
     */
    write_to_upper_layers(parent->lcid, std::move((*rx_window)[header.sn].buf));

    /*
     * - if x = RX_Highest_Status,
     *   - update RX_Highest_Status to the SN of the first RLC SDU with SN > current RX_Highest_Status for which not
     * all bytes have been received.
     */
    if (rx_mod_base_nr(header.sn) == rx_mod_base_nr(st.rx_highest_status)) {
      uint32_t sn_upd = 0;
      for (sn_upd = (st.rx_highest_status + 1) % mod_nr; rx_mod_base_nr(sn_upd) < rx_mod_base_nr(st.rx_next_highest);
           sn_upd = (sn_upd + 1) % mod_nr) {
        if (rx_window->has_sn(sn_upd)) {
          if (not(*rx_window)[sn_upd].fully_received) {
            break; // first SDU not fully received
          }
        } else {
          break; // first SDU not fully received
        }
      }
      // Update to the SN of the first SDU with missing bytes.
      // If it not exists, update to the end of the rx_window.
      st.rx_highest_status = sn_upd;
    }
    /*
     * - if x = RX_Next:
     *   - update RX_Next to the SN of the first RLC SDU with SN > current RX_Next for which not all bytes
     *     have been received.
     */
    if (rx_mod_base_nr(header.sn) == rx_mod_base_nr(st.rx_next)) {
      uint32_t sn_upd = 0;
      // move rx_next forward and remove all fully received SDUs from rx_window
      for (sn_upd = (st.rx_next) % mod_nr; rx_mod_base_nr(sn_upd) < rx_mod_base_nr(st.rx_next_highest);
           sn_upd = (sn_upd + 1) % mod_nr) {
        if (rx_window->has_sn(sn_upd)) {
          if (not(*rx_window)[sn_upd].fully_received) {
            break; // first SDU not fully received
          }
          // RX_Next serves as the lower edge of the receiving window
          // As such, we remove any SDU from the window if we update this value
          rx_window->remove_pdu(sn_upd);
        } else {
          break; // first SDU not fully received
        }
      }
      // Update to the SN of the first SDU with missing bytes.
      // If it not exists, update to the end of the rx_window.
      st.rx_next = sn_upd;
    }
  }

  if (reassembly_timer.is_running()) {
    // if t-Reassembly is running:
    /*
     * - if RX_Next_Status_Trigger = RX_Next; or
     * - if RX_Next_Status_Trigger = RX_Next + 1 and there is no missing byte segment of the SDU associated with
     *   SN = RX_Next before the last byte of all received segments of this SDU; or
     * - if RX_Next_Status_Trigger falls outside of the receiving window and RX_Next_Status_Trigger is not equal
     *   to RX_Next + AM_Window_Size:
     * - stop and reset t-Reassembly.
     */
    bool stop_reassembly_timer = false;
    if (st.rx_next_status_trigger == st.rx_next) {
      stop_reassembly_timer = true;
    }
    if (rx_mod_base_nr(st.rx_next_status_trigger) == rx_mod_base_nr(st.rx_next + 1)) {
      if (not(*rx_window)[st.rx_next].has_gap) {
        stop_reassembly_timer = true;
      }
    }
    if (not inside_rx_window(st.rx_next_status_trigger)) {
      stop_reassembly_timer = true;
    }
    if (stop_reassembly_timer) {
      reassembly_timer.stop();
    }
  }

  if (not reassembly_timer.is_running()) {
    // if t-Reassembly is not running (includes the case t-Reassembly is stopped due to actions above):
    /*
     * - if RX_Next_Highest> RX_Next +1; or
     * - if RX_Next_Highest = RX_Next + 1 and there is at least one missing byte segment of the SDU associated
     *   with SN = RX_Next before the last byte of all received segments of this SDU:
     *   - start t-Reassembly;
     *   - set RX_Next_Status_Trigger to RX_Next_Highest.
     */
    bool restart_reassembly_timer = false;
    if (rx_mod_base_nr(st.rx_next_highest) > rx_mod_base_nr(st.rx_next + 1)) {
      restart_reassembly_timer = true;
    }
    if (rx_mod_base_nr(st.rx_next_highest) == rx_mod_base_nr(st.rx_next + 1)) {
      if (rx_window->has_sn(st.rx_next) && (*rx_window)[st.rx_next].has_gap) {
        restart_reassembly_timer = true;
      }
    }
    if (restart_reassembly_timer) {
      reassembly_timer.run();
      st.rx_next_status_trigger = st.rx_next_highest;
    }
  }
}

/*
 *  SDU handling helpers
 */
int rlc_am_nr_rx::handle_full_data_sdu(const rlc_am_nr_pdu_header_t& header, const uint8_t* payload, uint32_t nof_bytes)
{
  uint32_t hdr_len = rlc_am_nr_packed_length(header);
  // Full SDU received. Add SDU to Rx Window and copy full PDU into SDU buffer.
  rlc_amd_rx_sdu_nr_t& rx_sdu = rx_window->add_pdu(header.sn);
  rx_sdu.buf                  = srsran::make_byte_buffer();
  if (rx_sdu.buf == nullptr) {
    RlcError("fatal error. Couldn't allocate PDU in %s.", __FUNCTION__);
    rx_window->remove_pdu(header.sn);
    return SRSRAN_ERROR;
  }
  rx_sdu.buf->set_timestamp();

  // check available space for payload
  if (nof_bytes > rx_sdu.buf->get_tailroom()) {
    RlcError("discarding SN=%d of size %d B (available space %d B)", header.sn, nof_bytes, rx_sdu.buf->get_tailroom());
    rx_window->remove_pdu(header.sn);
    return SRSRAN_ERROR;
  }
  memcpy(rx_sdu.buf->msg, payload + hdr_len, nof_bytes - hdr_len); // Don't copy header
  rx_sdu.buf->N_bytes   = nof_bytes - hdr_len;
  rx_sdu.fully_received = true;
  rx_sdu.has_gap        = false;
  return SRSRAN_SUCCESS;
}

int rlc_am_nr_rx::handle_segment_data_sdu(const rlc_am_nr_pdu_header_t& header,
                                          const uint8_t*                payload,
                                          uint32_t                      nof_bytes)
{
  if (header.si == rlc_nr_si_field_t::full_sdu) {
    RlcError("called %s but the SI implies a full SDU. SN=%d", __FUNCTION__, header.sn);
    return SRSRAN_ERROR;
  }

  uint32_t hdr_len = rlc_am_nr_packed_length(header);

  // Log SDU segment reception
  if (header.si == rlc_nr_si_field_t::first_segment) { // Check whether it's a full SDU
    RlcDebug("Initial segment PDU. SN=%d.", header.sn);
  } else if (header.si == rlc_nr_si_field_t::neither_first_nor_last_segment) {
    RlcDebug("Middle segment PDU. SN=%d.", header.sn);
  } else if (header.si == rlc_nr_si_field_t::last_segment) {
    RlcDebug("Final segment PDU. SN=%d.", header.sn);
  }

  // Add a new SDU to the RX window if necessary
  rlc_amd_rx_sdu_nr_t& rx_sdu = rx_window->has_sn(header.sn) ? (*rx_window)[header.sn] : rx_window->add_pdu(header.sn);

  // Create PDU segment info, to be stored later
  rlc_amd_rx_pdu_nr pdu_segment = {};
  pdu_segment.header            = header;
  pdu_segment.buf               = srsran::make_byte_buffer();
  if (pdu_segment.buf == nullptr) {
    RlcError("fatal error. Couldn't allocate PDU in %s.", __FUNCTION__);
    return SRSRAN_ERROR;
  }
  memcpy(pdu_segment.buf->msg, payload + hdr_len, nof_bytes - hdr_len); // Don't copy header
  pdu_segment.buf->N_bytes = nof_bytes - hdr_len;

  // Store SDU segment. Sort by SO and check for duplicate bytes.
  insert_received_segment(std::move(pdu_segment), rx_sdu.segments);

  // Check weather all segments have been received
  update_segment_inventory(rx_sdu);
  if (rx_sdu.fully_received) {
    RlcInfo("Fully received segmented SDU. SN=%d.", header.sn);
    rx_sdu.buf = srsran::make_byte_buffer();
    if (rx_sdu.buf == nullptr) {
      RlcError("fatal error. Couldn't allocate PDU in %s.", __FUNCTION__);
      rx_window->remove_pdu(header.sn);
      return SRSRAN_ERROR;
    }
    // Assemble SDU from segments
    for (const auto& it : rx_sdu.segments) {
      memcpy(&rx_sdu.buf->msg[rx_sdu.buf->N_bytes], it.buf->msg, it.buf->N_bytes);
      rx_sdu.buf->N_bytes += it.buf->N_bytes;
    }
  }
  return SRSRAN_SUCCESS;
}

/*
 * Status PDU
 */
uint32_t rlc_am_nr_rx::get_status_pdu(rlc_am_nr_status_pdu_t* status, uint32_t max_len)
{
  std::unique_lock<std::mutex> lock(mutex, std::try_to_lock);
  if (not lock.owns_lock()) {
    return SRSRAN_ERROR;
  }

  status->reset();

  /*
   * - for the RLC SDUs with SN such that RX_Next <= SN < RX_Highest_Status that has not been completely
   *   received yet, in increasing SN order of RLC SDUs and increasing byte segment order within RLC SDUs,
   *   starting with SN = RX_Next up to the point where the resulting STATUS PDU still fits to the total size of RLC
   *   PDU(s) indicated by lower layer:
   */
  RlcDebug("Generating status PDU");
  for (uint32_t i = st.rx_next; rx_mod_base_nr(i) < rx_mod_base_nr(st.rx_highest_status); i = (i + 1) % mod_nr) {
    if ((rx_window->has_sn(i) && (*rx_window)[i].fully_received)) {
      RlcDebug("SDU SN=%d is fully received", i);
    } else {
      if (not rx_window->has_sn(i)) {
        // No segment received, NACK the whole SDU
        RlcDebug("Adding NACK for full SDU. NACK SN=%d", i);
        rlc_status_nack_t nack;
        nack.nack_sn = i;
        nack.has_so  = false;
        status->push_nack(nack);
      } else if (not(*rx_window)[i].fully_received) {
        // Some segments were received, but not all.
        // NACK non consecutive missing bytes
        RlcDebug("Adding NACKs for segmented SDU. NACK SN=%d", i);
        uint32_t last_so         = 0;
        bool     last_segment_rx = false;
        for (auto segm = (*rx_window)[i].segments.begin(); segm != (*rx_window)[i].segments.end(); segm++) {
          if (segm->header.so != last_so) {
            // Some bytes were not received
            rlc_status_nack_t nack;
            nack.nack_sn  = i;
            nack.has_so   = true;
            nack.so_start = last_so;
            nack.so_end   = segm->header.so - 1; // set to last missing byte
            status->push_nack(nack);
            if (nack.so_start > nack.so_end) {
              // Print segment list
              for (auto segm_it = (*rx_window)[i].segments.begin(); segm_it != (*rx_window)[i].segments.end();
                   segm_it++) {
                RlcError("Segment: segm.header.so=%d, segm.buf.N_bytes=%d", segm_it->header.so, segm_it->buf->N_bytes);
              }
              RlcError("Error: SO_start=%d > SO_end=%d. NACK_SN=%d. SO_start=%d, SO_end=%d, seg.so=%d",
                       nack.so_start,
                       nack.so_end,
                       nack.nack_sn,
                       nack.so_start,
                       nack.so_end,
                       segm->header.so);
              srsran_assert(nack.so_start <= nack.so_end,
                            "Error: SO_start=%d > SO_end=%d. NACK_SN=%d",
                            nack.so_start,
                            nack.so_end,
                            nack.nack_sn);
            } else {
              RlcDebug("First/middle segment missing. NACK_SN=%d. SO_start=%d, SO_end=%d",
                       nack.nack_sn,
                       nack.so_start,
                       nack.so_end);
            }
          }
          if (segm->header.si == rlc_nr_si_field_t::last_segment) {
            last_segment_rx = true;
          }
          last_so = segm->header.so + segm->buf->N_bytes;
        } // Segment loop
        if (not last_segment_rx) {
          rlc_status_nack_t nack;
          nack.nack_sn  = i;
          nack.has_so   = true;
          nack.so_start = last_so;
          nack.so_end   = rlc_status_nack_t::so_end_of_sdu;
          status->push_nack(nack);
          RlcDebug(
              "Final segment missing. NACK_SN=%d. SO_start=%d, SO_end=%d", nack.nack_sn, nack.so_start, nack.so_end);
          srsran_assert(nack.so_start <= nack.so_end, "Error: SO_start > SO_end. NACK_SN=%d", nack.nack_sn);
        }
      }
    }
  } // NACK loop

  /*
   * - set the ACK_SN to the SN of the next not received RLC SDU which is not
   * indicated as missing in the resulting STATUS PDU.
   */
  status->ack_sn = st.rx_highest_status;

  // trim PDU if necessary
  if (status->packed_size > max_len) {
    RlcInfo("Trimming status PDU with %d NACKs and packed_size=%d into max_len=%d",
            status->nacks.size(),
            status->packed_size,
            max_len);
    log_rlc_am_nr_status_pdu_to_string(logger.debug, "Untrimmed status PDU - %s", status, rb_name);
    if (not status->trim(max_len)) {
      RlcError("Failed to trim status PDU into provided space: max_len=%d", max_len);
    }
  }

  if (max_len != UINT32_MAX) {
    // UINT32_MAX is used just to query the status PDU length
    if (status_prohibit_timer.is_valid()) {
      status_prohibit_timer.run();
    }
    do_status = false;
  }

  return status->packed_size;
}

uint32_t rlc_am_nr_rx::get_status_pdu_length()
{
  rlc_am_nr_status_pdu_t tmp_status(cfg.rx_sn_field_length);
  get_status_pdu(&tmp_status, UINT32_MAX);
  return tmp_status.get_packed_size();
}

bool rlc_am_nr_rx::get_do_status()
{
  return do_status.load(std::memory_order_relaxed) && not status_prohibit_timer.is_running();
}

void rlc_am_nr_rx::timer_expired(uint32_t timeout_id)
{
  std::unique_lock<std::mutex> lock(mutex);

  // Status Prohibit
  if (status_prohibit_timer.is_valid() && status_prohibit_timer.id() == timeout_id) {
    RlcDebug("Status prohibit timer expired after %dms", status_prohibit_timer.duration());
    return;
  }

  // Reassembly
  if (reassembly_timer.is_valid() && reassembly_timer.id() == timeout_id) {
    RlcDebug("Reassembly timer expired after %dms", reassembly_timer.duration());
    /*
     * 5.2.3.2.4 Actions when t-Reassembly expires:
     * - update RX_Highest_Status to the SN of the first RLC SDU with SN >= RX_Next_Status_Trigger for which not
     *   all bytes have been received;
     * - if RX_Next_Highest> RX_Highest_Status +1: or
     * - if RX_Next_Highest = RX_Highest_Status + 1 and there is at least one missing byte segment of the SDU
     *   associated with SN = RX_Highest_Status before the last byte of all received segments of this SDU:
     *   - start t-Reassembly;
     *   - set RX_Next_Status_Trigger to RX_Next_Highest.
     */
    uint32_t sn_upd = {};
    for (sn_upd = st.rx_next_status_trigger; rx_mod_base_nr(sn_upd) < rx_mod_base_nr(st.rx_next_highest);
         sn_upd = (sn_upd + 1) % mod_nr) {
      if (not rx_window->has_sn(sn_upd) || (rx_window->has_sn(sn_upd) && not(*rx_window)[sn_upd].fully_received)) {
        break;
      }
    }
    st.rx_highest_status = sn_upd;
    if (not valid_ack_sn(st.rx_highest_status)) {
      RlcError("Rx_Highest_Status not inside RX window");
      debug_state();
    }
    srsran_assert(valid_ack_sn(st.rx_highest_status), "Error: rx_highest_status assigned outside rx window");

    bool restart_reassembly_timer = false;
    if (rx_mod_base_nr(st.rx_next_highest) > rx_mod_base_nr(st.rx_highest_status + 1)) {
      restart_reassembly_timer = true;
    }
    if (rx_mod_base_nr(st.rx_next_highest) == rx_mod_base_nr(st.rx_highest_status + 1)) {
      if (rx_window->has_sn(st.rx_highest_status) && (*rx_window)[st.rx_highest_status].has_gap) {
        restart_reassembly_timer = true;
      }
    }
    if (restart_reassembly_timer) {
      reassembly_timer.run();
      st.rx_next_status_trigger = st.rx_next_highest;
    }

    /* 5.3.4 Status reporting:
     * - The receiving side of an AM RLC entity shall trigger a STATUS report when t-Reassembly expires.
     *   NOTE 2: The expiry of t-Reassembly triggers both RX_Highest_Status to be updated and a STATUS report to be
     *   triggered, but the STATUS report shall be triggered after RX_Highest_Status is updated.
     */
    do_status = true;
    debug_state();
    debug_window();
    return;
  }
}

void rlc_am_nr_rx::write_to_upper_layers(uint32_t lcid, unique_byte_buffer_t sdu)
{
  uint32_t nof_bytes = sdu->N_bytes;
  parent->pdcp->write_pdu(lcid, std::move(sdu));
  std::lock_guard<std::mutex> lock(parent->metrics_mutex);
  parent->metrics.num_rx_sdus++;
  parent->metrics.num_rx_sdu_bytes += nof_bytes;
}

/*
 * Segment Helpers
 */
void rlc_am_nr_rx::insert_received_segment(rlc_amd_rx_pdu_nr                                   segment,
                                           std::set<rlc_amd_rx_pdu_nr, rlc_amd_rx_pdu_nr_cmp>& segment_list) const
{
  segment_list.insert(std::move(segment));
}

void rlc_am_nr_rx::update_segment_inventory(rlc_amd_rx_sdu_nr_t& rx_sdu) const
{
  if (rx_sdu.segments.empty()) {
    rx_sdu.fully_received = false;
    rx_sdu.has_gap        = false;
    return;
  }

  // Check for gaps and if all segments have been received
  uint32_t next_byte = 0;
  for (const auto& it : rx_sdu.segments) {
    if (it.header.so != next_byte) {
      // Found gap: set flags and return
      rx_sdu.has_gap        = true;
      rx_sdu.fully_received = false;
      return;
    }
    if (it.header.si == rlc_nr_si_field_t::last_segment) {
      // Reached last segment without any gaps: set flags and return
      rx_sdu.has_gap        = false;
      rx_sdu.fully_received = true;
      return;
    }
    next_byte += it.buf->N_bytes;
  }
  // No gaps, but last segment not yet received
  rx_sdu.has_gap        = false;
  rx_sdu.fully_received = false;
}

/*
 * Window Helpers
 */
uint32_t rlc_am_nr_rx::rx_mod_base_nr(uint32_t sn) const
{
  return (sn - st.rx_next) % mod_nr;
}

uint32_t rlc_am_nr_rx::rx_window_size() const
{
  return am_window_size(cfg.rx_sn_field_length);
}

bool rlc_am_nr_rx::inside_rx_window(uint32_t sn) const
{
  // RX_Next <= SN < RX_Next + AM_Window_Size
  return rx_mod_base_nr(sn) < rx_window_size();
}

/*
 * This function is used to check if the Rx_Highest_Status is
 * valid when t-Reasseambly expires.
 *
 * ACK_SN may be equal to RX_NEXT + AM_Window_Size if the PDU
 * with SN=RX_NEXT+AM_Window_Size has been received by the RX.
 * An ACK_SN == Rx_Next should not update Rx_Highest_Status,
 * it should be updated when Rx_Next is updated.
 */
bool rlc_am_nr_rx::valid_ack_sn(uint32_t sn) const
{
  // RX_Next < SN <= RX_Next + AM_Window_Size
  return (0 < rx_mod_base_nr(sn)) && (rx_mod_base_nr(sn) <= rx_window_size());
}

/*
 * Debug Helpers
 */
void rlc_am_nr_rx::debug_state() const
{
  RlcDebug("RX entity state: Rx_Next=%d, Rx_Next_Status_Trigger=%d, Rx_Highest_Status=%d, Rx_Next_Highest=%d",
           st.rx_next,
           st.rx_next_status_trigger,
           st.rx_highest_status,
           st.rx_next_highest);
}

void rlc_am_nr_rx::debug_window() const
{
  RlcDebug(
      "RX window state: Rx_Next=%d, Rx_Next_Highest=%d, SDUs %d", st.rx_next, st.rx_next_highest, rx_window->size());
}

/*
 * Metrics
 */
uint32_t rlc_am_nr_rx::get_sdu_rx_latency_ms()
{
  return 0;
}

uint32_t rlc_am_nr_rx::get_rx_buffered_bytes()
{
  return 0;
}
} // namespace srsran
