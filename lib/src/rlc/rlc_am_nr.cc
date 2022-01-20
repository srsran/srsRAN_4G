/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

#define RLC_AM_NR_WINDOW_SIZE 2048

namespace srsran {

const static uint32_t max_tx_queue_size = 256;
/****************************************************************************
 * RLC AM NR entity
 ***************************************************************************/

/***************************************************************************
 *  Tx subclass implementation
 ***************************************************************************/
rlc_am_nr_tx::rlc_am_nr_tx(rlc_am* parent_) : parent(parent_), rlc_am_base_tx(parent_->logger) {}

bool rlc_am_nr_tx::configure(const rlc_config_t& cfg_)
{
  cfg     = cfg_.am_nr;
  rb_name = parent->rb_name;

  if (cfg.tx_sn_field_length != rlc_am_nr_sn_size_t::size12bits) {
    RlcWarning("RLC AM NR only supports 12 bit SN length.");
    return false;
  }

  if (cfg_.tx_queue_length > max_tx_queue_size) {
    RlcError("configuring tx queue length of %d PDUs too big. Maximum value is %d.",
             cfg_.tx_queue_length,
             max_tx_queue_size);
    return false;
  }

  mod_nr = cfg.tx_sn_field_length == rlc_am_nr_sn_size_t::size12bits ? 4096 : 262144;

  min_hdr_size = cfg.tx_sn_field_length == rlc_am_nr_sn_size_t::size12bits ? 2 : 3;
  max_hdr_size = min_hdr_size + so_size;

  tx_enabled = true;

  RlcDebug("RLC AM NR tx entity configured.");
  return true;
}

bool rlc_am_nr_tx::has_data()
{
  return do_status() ||                  // if we have a status PDU to transmit
         tx_sdu_queue.get_n_sdus() != 1; // or if there is a SDU queued up for transmission
}

/**
 * Builds the RLC PDU.
 *
 * Called by the MAC, trough the STACK thread.
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
  RlcDebug("MAC opportunity - bytes=%d, tx_window size=%zu PDUs", nof_bytes, tx_window.size());

  // Tx STATUS if requested
  if (do_status()) {
    unique_byte_buffer_t tx_pdu = srsran::make_byte_buffer();
    if (tx_pdu == nullptr) {
      RlcError("couldn't allocate PDU in %s().", __FUNCTION__);
      return 0;
    }
    build_status_pdu(tx_pdu.get(), nof_bytes);
    memcpy(payload, tx_pdu->msg, tx_pdu->N_bytes);
    RlcDebug("status PDU built - %d bytes", tx_pdu->N_bytes);
    return tx_pdu->N_bytes;
  }

  // Retransmit if required
  if (not retx_queue.empty()) {
    RlcInfo("re-transmission required. Retransmission queue size: %d", retx_queue.size());
    return build_retx_pdu(payload, nof_bytes);
  }

  // Send remaining segment, if it exists
  if (sdu_under_segmentation_sn != INVALID_RLC_SN) {
    if (not tx_window.has_sn(sdu_under_segmentation_sn)) {
      sdu_under_segmentation_sn = INVALID_RLC_SN;
      RlcError("SDU currently being segmented does not exist in tx_window. Aborting segmentation SN=%d",
               sdu_under_segmentation_sn);
      return 0;
    }
    return build_continuation_sdu_segment(tx_window[sdu_under_segmentation_sn], payload, nof_bytes);
  }

  // Check whether there is something to TX
  if (tx_sdu_queue.is_empty()) {
    RlcInfo("no data available to be sent");
    return 0;
  }

  return build_new_pdu(payload, nof_bytes);
}

/**
 * Builds a new RLC PDU, which contains the full SDU.
 *
 * Called by the MAC, trough the STACK thread.
 * This will be called after checking whether control, retransmission,
 * or segment PDUs needed to be transmitted first.
 *
 * This will read an SDU from the SDU queue, build a new PDU, and add it to the tx_window.
 * Segmentation will be done if necessary.
 *
 * \param [payload] is a pointer to the buffer that will hold the PDU.
 * \param [nof_bytes] is the number of bytes the RLC is allowed to fill.
 *
 * \returns the number of bytes written to the payload buffer.
 */
uint32_t rlc_am_nr_tx::build_new_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  // Read new SDU from TX queue
  unique_byte_buffer_t tx_sdu;
  RlcDebug("reading from RLC SDU queue. Queue size %d", tx_sdu_queue.size());
  do {
    tx_sdu = tx_sdu_queue.read();
  } while (tx_sdu == nullptr && tx_sdu_queue.size() != 0);

  if (tx_sdu != nullptr) {
    RlcDebug("read RLC SDU - %d bytes", tx_sdu->N_bytes);
  } else {
    RlcDebug("no SDUs left in the tx queue.");
    return 0;
  }

  // insert newly assigned SN into window and use reference for in-place operations
  // NOTE: from now on, we can't return from this function anymore before increasing tx_next
  rlc_amd_tx_pdu_nr& tx_pdu = tx_window.add_pdu(st.tx_next);
  tx_pdu.sdu_buf            = srsran::make_byte_buffer();
  if (tx_pdu.sdu_buf == nullptr) {
    RlcError("couldn't allocate PDU in %s().", __FUNCTION__);
    return 0;
  }

  // Copy SDU into PDU info
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
  hdr.p                      = get_pdu_poll();
  hdr.si                     = rlc_nr_si_field_t::full_sdu;
  hdr.sn_size                = rlc_am_nr_sn_size_t::size12bits;
  hdr.sn                     = st.tx_next;
  tx_pdu.header              = hdr;
  log_rlc_am_nr_pdu_header_to_string(logger.info, hdr);

  // Write header
  uint32_t len = rlc_am_nr_write_data_pdu_header(hdr, tx_sdu.get());
  if (len > nof_bytes) {
    RlcError("error writing AMD PDU header");
  }

  // Update TX Next
  st.tx_next = (st.tx_next + 1) % MOD;

  memcpy(payload, tx_sdu->msg, tx_sdu->N_bytes);
  RlcDebug("wrote RLC PDU - %d bytes", tx_sdu->N_bytes);

  return tx_sdu->N_bytes;
}

/**
 * Builds a new RLC PDU segment.
 *
 * Called by the MAC, trough the STACK thread.
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
    RlcError("cannot build new sdu_segment, there are not enough bytes allocated to tx header plus data. nof_bytes=%d",
             nof_bytes);
    return 0;
  }

  // Prepare header
  rlc_am_nr_pdu_header_t hdr = {};
  hdr.dc                     = RLC_DC_FIELD_DATA_PDU;
  hdr.p                      = get_pdu_poll();
  hdr.si                     = rlc_nr_si_field_t::first_segment;
  hdr.sn_size                = rlc_am_nr_sn_size_t::size12bits;
  hdr.sn                     = st.tx_next;
  hdr.so                     = 0;
  tx_pdu.header              = hdr;
  log_rlc_am_nr_pdu_header_to_string(logger.info, hdr);

  // Write header
  uint32_t hdr_len = rlc_am_nr_write_data_pdu_header(hdr, payload);
  if (hdr_len >= nof_bytes || hdr_len != min_hdr_size) {
    RlcError("error writing AMD PDU header");
    return 0;
  }

  // Copy PDU to payload
  uint32_t segment_payload_len = nof_bytes - hdr_len;
  srsran_assert((hdr_len + segment_payload_len) <= nof_bytes, "Error calculating hdr_len and segment_payload_len");
  memcpy(&payload[hdr_len], tx_pdu.sdu_buf->msg, segment_payload_len);

  // Save SDU currently being segmented
  sdu_under_segmentation_sn = st.tx_next;

  // Store Segment Info
  rlc_amd_tx_pdu_nr::pdu_segment segment_info;
  segment_info.payload_len = segment_payload_len;
  tx_pdu.segment_list.push_back(segment_info);
  return hdr_len + segment_payload_len;
}

/**
 * Build PDU segment for an RLC SDU that is already on-going segmentation.
 *
 * Called by the MAC, trough the STACK thread.
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
  if ((max_hdr_size + 1) < nof_bytes) { // Larger header size, as SO is present
    RlcError("cannot build new sdu_segment, there are not enough bytes allocated to tx header plus data. nof_bytes=%d",
             nof_bytes);
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
    si = rlc_nr_si_field_t::last_segment;
  }

  // Prepare header
  rlc_am_nr_pdu_header_t hdr = {};
  hdr.dc                     = RLC_DC_FIELD_DATA_PDU;
  hdr.p                      = get_pdu_poll();
  hdr.si                     = si;
  hdr.sn_size                = rlc_am_nr_sn_size_t::size12bits;
  hdr.sn                     = st.tx_next;
  hdr.so                     = last_byte;
  tx_pdu.header              = hdr;
  log_rlc_am_nr_pdu_header_to_string(logger.info, hdr);

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
    sdu_under_segmentation_sn = INVALID_RLC_SN;
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

  rlc_amd_retx_t& retx = retx_queue.front();

  // Sanity check - drop any retx SNs not present in tx_window
  while (not tx_window.has_sn(retx.sn)) {
    RlcWarning("SN=%d not in tx window. Ignoring retx.", retx.sn);
    retx_queue.pop();
    if (!retx_queue.empty()) {
      retx = retx_queue.front();
    } else {
      RlcWarning("empty retx queue, cannot provide retx PDU");
      return 0;
    }
  }

  if (retx.is_segment) {
    return build_retx_pdu_from_sdu_segment(retx, payload, nof_bytes);
  }
  return build_retx_pdu_from_full_sdu(retx, payload, nof_bytes);
}

/**
 * Builds a retx RLC PDU from a full SDU.
 *
 * The RETX of the full SDU may be further segmented if necessary.
 *
 * \param [tx_pdu] is the tx_pdu info contained in the tx_window.
 * \param [payload] is a pointer to the MAC buffer that will hold the PDU segment.
 * \param [nof_bytes] is the number of bytes the RLC is allowed to fill.
 *
 * \returns the number of bytes written to the payload buffer.
 * \remark: This functions assumes that the SDU has already been copied to tx_pdu.sdu_buf.
 */
uint32_t rlc_am_nr_tx::build_retx_pdu_from_full_sdu(rlc_amd_retx_t& retx, uint8_t* payload, uint32_t nof_bytes)
{
  srsran_assert(tx_window.has_sn(retx.sn), "Called %s without checking retx SN", __FUNCTION__);

  // Get tx_pdu info from tx_window
  rlc_amd_tx_pdu_nr& tx_pdu = tx_window[retx.sn];
  // Update & write header
  rlc_am_nr_pdu_header_t new_header = tx_pdu.header;
  new_header.p                      = 0;
  uint32_t hdr_len                  = rlc_am_nr_write_data_pdu_header(new_header, payload);

  // Check if we exceed allocated number of bytes
  if (hdr_len + tx_window[retx.sn].sdu_buf->N_bytes > nof_bytes) {
    RlcInfo("Trying to segment retx PDU. SN=%d", retx.sn);
    log_rlc_am_nr_pdu_header_to_string(logger.debug, new_header);
    return build_retx_segment_from_full_sdu(retx, payload, nof_bytes);
  }

  // RETX full PDU
  memcpy(&payload[hdr_len], tx_window[retx.sn].sdu_buf->msg, tx_window[retx.sn].sdu_buf->N_bytes);
  uint32_t pdu_bytes = hdr_len + tx_window[retx.sn].sdu_buf->N_bytes;

  retx_queue.pop();
  RlcHexInfo(tx_window[retx.sn].sdu_buf->msg,
             tx_window[retx.sn].sdu_buf->N_bytes,
             "Original SDU SN=%d (%d B) (attempt %d/%d)",
             retx.sn,
             tx_window[retx.sn].sdu_buf->N_bytes,
             tx_window[retx.sn].retx_count + 1,
             cfg.max_retx_thresh);
  RlcHexInfo(payload, pdu_bytes, "retx PDU SN=%d (%d B)", retx.sn, pdu_bytes);
  log_rlc_am_nr_pdu_header_to_string(logger.debug, new_header);

  debug_state();
  return pdu_bytes;
}

/**
 * Builds a retx RLC PDU from a full SDU.
 *
 * This function will further segment the previously transmitted SDU.
 * There should not be enough bytes granted to transmit the previous
 * SDU full; this should have been checked previous to calling this
 * function.
 *
 * \param [tx_pdu] is the tx_pdu info contained in the tx_window.
 * \param [payload] is a pointer to the MAC buffer that will hold the PDU segment.
 * \param [nof_bytes] is the number of bytes the RLC is allowed to fill.
 *
 * \returns the number of bytes written to the payload buffer.
 * \remark: This functions assumes that the SDU has already been copied to tx_pdu.sdu_buf.
 */
uint32_t rlc_am_nr_tx::build_retx_segment_from_full_sdu(rlc_amd_retx_t& retx, uint8_t* payload, uint32_t nof_bytes)
{
  // Get tx_pdu info from tx_window
  srsran_assert(tx_window.has_sn(retx.sn), "Called %s without checking retx SN", __FUNCTION__);
  rlc_amd_tx_pdu_nr& tx_pdu = tx_window[retx.sn];

  RlcInfo("creating SDU segment from full SDU. Tx SDU (%d B), nof_bytes=%d B ", tx_pdu.sdu_buf->N_bytes, nof_bytes);

  // Sanity check: is this an SDU segment retx?
  if (retx.is_segment) {
    RlcError("called %s, but retx contains a SDU segment. SN=%d, so_start=%d, so_end=%d",
             __FUNCTION__,
             retx.sn,
             retx.so_start,
             retx.so_end);
    return 0;
  }

  // Sanity check: are there enough bytes for header plus data?
  if (nof_bytes <= min_hdr_size) {
    RlcError("called %s, but there are not enough bytes for data plus header. SN=%d", __FUNCTION__, retx.sn);
    return 0;
  }

  // Sanity check: could this have been transmitted without segmentation?
  if (nof_bytes > (tx_pdu.sdu_buf->N_bytes + min_hdr_size)) {
    RlcError("called %s, but there are enough bytes to avoid segmentation. SN=%d", __FUNCTION__, retx.sn);
    return 0;
  }

  // Can the RETX PDU be transmitted in a single PDU?
  uint32_t retx_pdu_payload_size = nof_bytes - min_hdr_size;

  // Write header
  rlc_am_nr_pdu_header_t hdr = tx_pdu.header;
  hdr.si                     = rlc_nr_si_field_t::first_segment;
  uint32_t hdr_len           = rlc_am_nr_write_data_pdu_header(hdr, payload);
  if (hdr_len >= nof_bytes || hdr_len != min_hdr_size) {
    RlcError("error writing AMD PDU header");
    return 0;
  }
  log_rlc_am_nr_pdu_header_to_string(logger.info, hdr);

  // Copy PDU to payload
  srsran_assert((hdr_len + retx_pdu_payload_size) <= nof_bytes, "Error calculating hdr_len and segment_payload_len");
  memcpy(&payload[hdr_len], tx_pdu.sdu_buf->msg, retx_pdu_payload_size);

  // Store Segment Info
  rlc_amd_tx_pdu_nr::pdu_segment segment_info;
  segment_info.payload_len = retx_pdu_payload_size;
  tx_pdu.segment_list.push_back(segment_info);

  // Update retx queue. Next SDU segment will be the rest of the PDU
  retx.is_segment = true;
  retx.so_start   = retx_pdu_payload_size;
  retx.so_end     = tx_pdu.sdu_buf->N_bytes;

  return hdr_len + retx_pdu_payload_size;
}

/**
 * Builds a retx RLC PDU from an RETX SDU segment.
 *
 * The
 * \param [tx_pdu] is the tx_pdu info contained in the tx_window.
 * \param [payload] is a pointer to the MAC buffer that will hold the PDU segment.
 * \param [nof_bytes] is the number of bytes the RLC is allowed to fill.
 *
 * \returns the number of bytes written to the payload buffer.
 * \remark: This functions assumes that the SDU has already been copied to tx_pdu.sdu_buf.
 */
uint32_t rlc_am_nr_tx::build_retx_pdu_from_sdu_segment(rlc_amd_retx_t& retx, uint8_t* payload, uint32_t nof_bytes)
{
  // Get tx_pdu info from tx_window
  srsran_assert(tx_window.has_sn(retx.sn), "Called %s without checking retx SN", __FUNCTION__);
  rlc_amd_tx_pdu_nr& tx_pdu = tx_window[retx.sn];

  RlcInfo("creating RETX PDU from SDU segment. Tx SDU (%d B), nof_bytes=%d B ", tx_pdu.sdu_buf->N_bytes, nof_bytes);

  // Sanity check: is this an SDU segment retx?
  if (not retx.is_segment) {
    RlcError("called %s, but retx contains a SDU segment. SN=%d, so_start=%d, so_end=%d",
             __FUNCTION__,
             retx.sn,
             retx.so_start,
             retx.so_end);
    return 0;
  }

  uint32_t expected_hdr_len = min_hdr_size;
  if (retx.so_start != 0) {
    expected_hdr_len = max_hdr_size;
  }

  // Sanity check: are there enough bytes for header plus data?
  if (nof_bytes <= expected_hdr_len) {
    RlcError("called %s, but there are not enough bytes for data plus header. SN=%d", __FUNCTION__, retx.sn);
    return 0;
  }

  // Can this have be transmitted without segmentation?
  if (nof_bytes < (retx.so_end - retx.so_start) + expected_hdr_len) {
    RlcInfo("called %s, but there are enough bytes to avoid segmentation. SN=%d", __FUNCTION__, retx.sn);
    return build_retx_segment_from_sdu_segment(retx, payload, nof_bytes);
  }

  // Can the RETX PDU be transmitted in a single PDU?
  uint32_t retx_pdu_payload_size = (retx.so_end - retx.so_start);

  // Write header
  rlc_am_nr_pdu_header_t hdr     = tx_pdu.header;
  uint32_t               hdr_len = rlc_am_nr_write_data_pdu_header(hdr, payload);
  if (hdr_len >= nof_bytes || hdr_len != expected_hdr_len) {
    RlcError("error writing AMD PDU header");
    return 0;
  }
  log_rlc_am_nr_pdu_header_to_string(logger.info, hdr);

  // Copy SDU segment into payload
  srsran_assert((hdr_len + retx_pdu_payload_size) <= nof_bytes, "Error calculating hdr_len and segment_payload_len");
  memcpy(&payload[hdr_len], tx_pdu.sdu_buf->msg, retx_pdu_payload_size);

  // Update retx queue
  retx.so_start = retx.so_end;
  retx.so_end += retx_pdu_payload_size;

  if (retx.so_end == tx_pdu.sdu_buf->N_bytes) {
    // Last segment to retx, remove retx from queue
    retx_queue.pop();
  }

  // Update SDU segment info
  // TODO
  return hdr_len + retx_pdu_payload_size;
}

/**
 * Builds a retx RLC PDU from an SDU SDU segment.
 *
 * \param [tx_pdu] is the tx_pdu info contained in the tx_window.
 * \param [payload] is a pointer to the MAC buffer that will hold the PDU segment.
 * \param [nof_bytes] is the number of bytes the RLC is allowed to fill.
 *
 * \returns the number of bytes written to the payload buffer.
 * \remark: This functions assumes that the SDU has already been copied to tx_pdu.sdu_buf.
 */
uint32_t rlc_am_nr_tx::build_retx_segment_from_sdu_segment(rlc_amd_retx_t& retx, uint8_t* payload, uint32_t nof_bytes)
{
  // Get tx_pdu info from tx_window
  srsran_assert(tx_window.has_sn(retx.sn), "Called %s without checking retx SN", __FUNCTION__);
  rlc_amd_tx_pdu_nr& tx_pdu = tx_window[retx.sn];

  RlcInfo("creating SDU segment from full SDU. Tx SDU (%d B), nof_bytes=%d B ", tx_pdu.sdu_buf->N_bytes, nof_bytes);

  // Sanity check: is this an SDU segment retx?
  if (not retx.is_segment) {
    RlcError("called %s, but retx contains a SDU segment. SN=%d, so_start=%d, so_end=%d",
             __FUNCTION__,
             retx.sn,
             retx.so_start,
             retx.so_end);
    return 0;
  }

  // Sanity check: are there enough bytes for header plus data?
  if (nof_bytes <= min_hdr_size) {
    RlcError("called %s, but there are not enough bytes for data plus header. SN=%d", __FUNCTION__, retx.sn);
    return 0;
  }

  // Sanity check: could this have been transmitted without segmentation?
  if (nof_bytes > (tx_pdu.sdu_buf->N_bytes + min_hdr_size)) {
    RlcError("called %s, but there are enough bytes to avoid segmentation. SN=%d", __FUNCTION__, retx.sn);
    return 0;
  }

  return 0;
}

uint32_t rlc_am_nr_tx::build_status_pdu(byte_buffer_t* payload, uint32_t nof_bytes)
{
  RlcInfo("generating status PDU. Bytes available:%d", nof_bytes);
  rlc_am_nr_status_pdu_t tx_status;
  int                    pdu_len = rx->get_status_pdu(&tx_status, nof_bytes);
  if (pdu_len == SRSRAN_ERROR) {
    RlcDebug("deferred status PDU. Cause: Failed to acquire rx lock");
    pdu_len = 0;
  } else if (pdu_len > 0 && nof_bytes >= static_cast<uint32_t>(pdu_len)) {
    RlcDebug("generated status PDU. Bytes:%d", pdu_len);
    log_rlc_am_nr_status_pdu_to_string(logger.info, "%s tx status PDU - %s", &tx_status, rb_name);
    pdu_len = rlc_am_nr_write_status_pdu(tx_status, rlc_am_nr_sn_size_t::size12bits, payload);
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

  rlc_am_nr_status_pdu_t status = {};
  RlcHexDebug(payload, nof_bytes, "%s Rx control PDU", parent->rb_name);
  rlc_am_nr_read_status_pdu(payload, nof_bytes, rlc_am_nr_sn_size_t::size12bits, &status);
  log_rlc_am_nr_status_pdu_to_string(logger.info, "%s Rx Status PDU: %s", &status, parent->rb_name);
  // Local variables for handling Status PDU will be updated with lock
  /*
   * - if the SN of the corresponding RLC SDU falls within the range
   *   TX_Next_Ack <= SN < = the highest SN of the AMD PDU among the AMD PDUs submitted to lower layer:
   *   - consider the RLC SDU or the RLC SDU segment for which a negative acknowledgement was received for
   *     retransmission.
   */
  // Process ACKs
  uint32_t stop_sn = status.N_nack == 0
                         ? status.ack_sn
                         : status.nacks[0].nack_sn - 1; // Stop processing ACKs at the first NACK, if it exists.
  if (stop_sn > st.tx_next) {
    RlcError("Received ACK or NACK larger than TX_NEXT. Ignoring status report");
    return;
  }
  for (uint32_t sn = st.tx_next_ack; sn < stop_sn; sn++) {
    if (tx_window.has_sn(sn)) {
      tx_window.remove_pdu(sn);
      st.tx_next_ack = sn + 1;
      // TODO notify PDCP
    } else {
      RlcError("Missing ACKed SN from TX window");
      break;
    }
  }

  // Process N_acks
  for (uint32_t nack_idx = 0; nack_idx < status.N_nack; nack_idx++) {
    if (st.tx_next_ack <= status.nacks[nack_idx].nack_sn && status.nacks[nack_idx].nack_sn <= st.tx_next) {
      uint32_t nack_sn = status.nacks[nack_idx].nack_sn;
      if (tx_window.has_sn(nack_sn)) {
        auto& pdu = tx_window[nack_sn];

        // add to retx queue if it's not already there
        if (not retx_queue.has_sn(nack_sn)) {
          // increment Retx counter and inform upper layers if needed
          pdu.retx_count++;

          // check_sn_reached_max_retx(nack_sn);
          rlc_amd_retx_t& retx = retx_queue.push();
          srsran_expect(tx_window[nack_sn].rlc_sn == nack_sn,
                        "Incorrect RLC SN=%d!=%d being accessed",
                        tx_window[nack_sn].rlc_sn,
                        nack_sn);
          retx.sn         = nack_sn;
          retx.is_segment = false;
          retx.so_start   = 0;
          retx.so_end     = pdu.sdu_buf->N_bytes;
        }
      }
    }
  }

  /**
   * Section 5.3.3.3: Reception of a STATUS report
   * - if the STATUS report comprises a positive or negative acknowledgement for the RLC SDU with sequence
   *   number equal to POLL_SN:
   *   - if t-PollRetransmit is running:
   *     - stop and reset t-PollRetransmit.
   */
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

  // Bytes needed for status report
  if (do_status()) {
    n_bytes_prio += rx->get_status_pdu_length();
    RlcDebug("buffer state - total status report: %d bytes", n_bytes_prio);
  }

  // Bytes needed for retx
  if (not retx_queue.empty()) {
    rlc_amd_retx_t& retx = retx_queue.front();
    RlcDebug("buffer state - retx - SN=%d, Segment: %s, %d:%d",
             retx.sn,
             retx.is_segment ? "true" : "false",
             retx.so_start,
             retx.so_end);
    if (tx_window.has_sn(retx.sn)) {
      int req_bytes     = retx.so_end - retx.so_start;
      int hdr_req_bytes = retx.is_segment ? max_hdr_size : min_hdr_size; // Segmentation not supported yet
      if (req_bytes <= 0) {
        RlcError("in get_buffer_state(): Removing retx with SN=%d from queue", retx.sn);
        retx_queue.pop();
      } else {
        n_bytes_prio += (req_bytes + hdr_req_bytes);
        RlcDebug("buffer state - retx: %d bytes", n_bytes_prio);
      }
    }
  }

  // Bytes needed for tx SDUs
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

uint8_t rlc_am_nr_tx::get_pdu_poll()
{
  uint8_t poll = 0;
  if (cfg.poll_pdu > 0) {
    if (st.pdu_without_poll >= (uint32_t)cfg.poll_pdu) {
      poll                = 1;
      st.pdu_without_poll = 0;
    } else {
      st.pdu_without_poll++;
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

void rlc_am_nr_tx::discard_sdu(uint32_t discard_sn) {}

bool rlc_am_nr_tx::sdu_queue_is_full()
{
  return false;
}

void rlc_am_nr_tx::empty_queue() {}

void rlc_am_nr_tx::stop() {}

/*
 * Window helpers
 */
uint32_t rlc_am_nr_tx::tx_mod_base_nr(uint32_t sn) const
{
  return (sn - st.tx_next_ack) % mod_nr;
}

bool rlc_am_nr_tx::inside_tx_window(uint32_t sn)
{
  // TX_Next_Ack <= SN < TX_Next_Ack + AM_Window_Size
  return tx_mod_base_nr(sn) < RLC_AM_NR_WINDOW_SIZE;
}

/*
 * Debug Helpers
 */
void rlc_am_nr_tx::debug_state()
{
  RlcDebug("TX entity state: Tx_Next %d, Rx_Next_Ack %d, POLL_SN %d, PDU_WITHOUT_POLL %d, BYTE_WITHOUT_POLL %d",
           st.tx_next,
           st.tx_next_ack,
           st.poll_sn,
           st.pdu_without_poll,
           st.byte_without_poll);
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
    RlcInfo("configured reassembly timer. t-Reassembly=%d ms", cfg.t_reassembly);
  }

  mod_nr = (cfg.rx_sn_field_length == rlc_am_nr_sn_size_t::size12bits) ? 4096 : 262144;

  RlcDebug("RLC AM NR configured rx entity.");

  return true;
}

void rlc_am_nr_rx::stop() {}

void rlc_am_nr_rx::reestablish()
{
  stop();
}

void rlc_am_nr_rx::handle_data_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  // Get AMD PDU Header
  rlc_am_nr_pdu_header_t header = {};
  uint32_t hdr_len = rlc_am_nr_read_data_pdu_header(payload, nof_bytes, rlc_am_nr_sn_size_t::size12bits, &header);

  RlcHexInfo(payload, nof_bytes, "Rx data PDU SN=%d (%d B)", header.sn, nof_bytes);
  log_rlc_am_nr_pdu_header_to_string(logger.debug, header);

  // Check whether SDU is within Rx Window
  if (!inside_rx_window(header.sn)) {
    RlcInfo("SN=%d outside rx window [%d:%d] - discarding", header.sn, st.rx_next, st.rx_next + RLC_AM_NR_WINDOW_SIZE);
    return;
  }

  // Section 5.2.3.2.2, discard duplicate PDUs
  if (rx_window.has_sn(header.sn) && rx_window[header.sn].fully_received) {
    RlcInfo("discarding duplicate SN=%d", header.sn);
    return;
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

  // Check poll bit
  if (header.p) {
    RlcInfo("status packet requested through polling bit");
    do_status = true;
    status_prohibit_timer.stop();
  }

  debug_state();

  // 5.2.3.2.3 Actions when an AMD PDU is placed in the reception buffer
  // Update Rx_Next_Highest
  if (rx_mod_base_nr(header.sn) >= rx_mod_base_nr(st.rx_next_highest)) {
    st.rx_next_highest = (header.sn + 1) % MOD;
  }

  // Update RX_Highest_Status
  /*
   * - if x = RX_Highest_Status,
   *   - update RX_Highest_Status to the SN of the first RLC SDU with SN > current RX_Highest_Status for which not
   * all bytes have been received.
   */
  if (rx_mod_base_nr(header.sn) == rx_mod_base_nr(st.rx_highest_status)) {
    uint32_t sn_upd     = 0;
    uint32_t window_top = st.rx_next + RLC_AM_WINDOW_SIZE;
    for (sn_upd = st.rx_highest_status; sn_upd < window_top; ++sn_upd) {
      if (rx_window.has_sn(sn_upd)) {
        if (not rx_window[sn_upd].fully_received) {
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
    uint32_t sn_upd     = 0;
    uint32_t window_top = st.rx_next + RLC_AM_WINDOW_SIZE;
    for (sn_upd = st.rx_next; sn_upd < window_top; ++sn_upd) {
      if (rx_window.has_sn(sn_upd)) {
        if (not rx_window[sn_upd].fully_received) {
          break; // first SDU not fully received
        }
        // RX_Next serves as the lower edge of the receiving window
        // As such, we remove any SDU from the window if we update this value
        rx_window.remove_pdu(sn_upd);
      } else {
        break; // first SDU not fully received
      }
    }
    // Update to the SN of the first SDU with missing bytes.
    // If it not exists, update to the end of the rx_window.
    st.rx_next = sn_upd;
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
  } else {
    /*
     * - if RX_Next_Highest> RX_Next +1; or
     * - if RX_Next_Highest = RX_Next + 1 and there is at least one missing byte segment of the SDU associated
     *   with SN = RX_Next before the last byte of all received segments of this SDU:
     *   - start t-Reassembly;
     *   - set RX_Next_Status_Trigger to RX_Next_Highest.
     */
    bool restart_reassembly_timer = false;
    if (st.rx_next_highest > st.rx_next + 1) {
      restart_reassembly_timer = true;
    }
    if (st.rx_next_highest == st.rx_next + 1 && rx_window.has_sn(st.rx_next + 1) &&
        not rx_window[st.rx_next + 1].fully_received) {
      restart_reassembly_timer = true;
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
  rlc_amd_rx_sdu_nr_t& rx_sdu = rx_window.add_pdu(header.sn);
  rx_sdu.buf                  = srsran::make_byte_buffer();
  if (rx_sdu.buf == nullptr) {
    RlcError("fatal error. Couldn't allocate PDU in %s.", __FUNCTION__);
    rx_window.remove_pdu(header.sn);
    return SRSRAN_ERROR;
  }
  rx_sdu.buf->set_timestamp();

  // check available space for payload
  if (nof_bytes > rx_sdu.buf->get_tailroom()) {
    RlcError("discarding SN=%d of size %d B (available space %d B)", header.sn, nof_bytes, rx_sdu.buf->get_tailroom());
    rx_window.remove_pdu(header.sn);
    return SRSRAN_ERROR;
  }
  memcpy(rx_sdu.buf->msg, payload + hdr_len, nof_bytes - hdr_len); // Don't copy header
  rx_sdu.buf->N_bytes   = nof_bytes - hdr_len;
  rx_sdu.fully_received = true;
  write_to_upper_layers(parent->lcid, std::move(rx_window[header.sn].buf));
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
  rlc_amd_rx_sdu_nr_t& rx_sdu = rx_window.has_sn(header.sn) ? rx_window[header.sn] : rx_window.add_pdu(header.sn);

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

  // Store SDU segment. TODO sort by SO and check for duplicate bytes.
  rx_sdu.segments.push_back(std::move(pdu_segment));

  // Check weather all segments have been received
  rx_sdu.fully_received = have_all_segments_been_received(rx_sdu.segments);
  if (rx_sdu.fully_received) {
    RlcInfo("Fully received segmented SDU. SN=%d.", header.sn);
    rx_sdu.buf = srsran::make_byte_buffer();
    if (rx_sdu.buf == nullptr) {
      RlcError("fatal error. Couldn't allocate PDU in %s.", __FUNCTION__);
      rx_window.remove_pdu(header.sn);
      return SRSRAN_ERROR;
    }
    for (const auto& it : rx_sdu.segments) {
      memcpy(&rx_sdu.buf->msg[rx_sdu.buf->N_bytes], it.buf->msg, it.buf->N_bytes);
      rx_sdu.buf->N_bytes += it.buf->N_bytes;
    }
    write_to_upper_layers(parent->lcid, std::move(rx_window[header.sn].buf));
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

  status->N_nack = 0;
  status->ack_sn = st.rx_next; // Start with the lower end of the window
  byte_buffer_t tmp_buf;

  uint32_t i = status->ack_sn;
  while (rx_mod_base_nr(i) <= rx_mod_base_nr(st.rx_highest_status)) {
    if (rx_window.has_sn(i) || i == st.rx_highest_status) {
      // only update ACK_SN if this SN has been received, or if we reached the maximum possible SN
      status->ack_sn = i;
    } else {
      status->nacks[status->N_nack].nack_sn = i;
      status->N_nack++;
    }

    // make sure we don't exceed grant size (FIXME)
    rlc_am_nr_write_status_pdu(*status, rlc_am_nr_sn_size_t::size12bits, &tmp_buf);
    // TODO
    i = (i + 1) % MOD;
  }

  if (max_len != UINT32_MAX) {
    // UINT32_MAX is used just to query the status PDU length
    if (status_prohibit_timer.is_valid()) {
      status_prohibit_timer.run();
    }
  }
  return tmp_buf.N_bytes;
}

uint32_t rlc_am_nr_rx::get_status_pdu_length()
{
  rlc_am_nr_status_pdu_t tmp_status;
  return get_status_pdu(&tmp_status, UINT32_MAX);
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
    for (uint32_t tmp_sn = st.rx_next_status_trigger; tmp_sn < st.rx_next_status_trigger + RLC_AM_WINDOW_SIZE;
         tmp_sn++) {
      if (not rx_window.has_sn(tmp_sn) || not rx_window[tmp_sn].fully_received) {
        st.rx_highest_status = tmp_sn;
        break;
      }
    }
    bool restart_reassembly_timer = false;
    if (st.rx_next_highest > st.rx_highest_status + 1) {
      restart_reassembly_timer = true;
    }
    if (st.rx_next_highest == st.rx_highest_status + 1 && not rx_window[st.rx_next_highest].fully_received) {
      restart_reassembly_timer = true;
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
 * Window Helpers
 */
uint32_t rlc_am_nr_rx::rx_mod_base_nr(uint32_t sn) const
{
  return (sn - st.rx_next) % mod_nr;
}

bool rlc_am_nr_rx::inside_rx_window(uint32_t sn)
{
  // RX_Next <= SN < RX_Next + AM_Window_Size
  return rx_mod_base_nr(sn) < RLC_AM_NR_WINDOW_SIZE;
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

bool rlc_am_nr_rx::have_all_segments_been_received(const std::list<rlc_amd_rx_pdu_nr>& segment_list)
{
  if (segment_list.empty()) {
    return false;
  }

  // Check if we have received the last segment
  if ((--segment_list.end())->header.si != rlc_nr_si_field_t::last_segment) {
    return false;
  }

  // Check if all segments have been received
  uint32_t next_byte = 0;
  for (const auto& it : segment_list) {
    if (it.header.so != next_byte) {
      return false;
    }
    next_byte += it.buf->N_bytes;
  }
  return true;
}

/*
 * Debug Helpers
 */
void rlc_am_nr_rx::debug_state()
{
  RlcDebug("RX entity state: Rx_Next %d, Rx_Next_Status_Trigger %d, Rx_Highest_Status %d, Rx_Next_Highest",
           st.rx_next,
           st.rx_next_status_trigger,
           st.rx_highest_status,
           st.rx_next_highest);
}
} // namespace srsran
