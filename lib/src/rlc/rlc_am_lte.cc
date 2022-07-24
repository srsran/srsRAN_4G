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

#include "srsran/rlc/rlc_am_lte.h"
#include "srsran/interfaces/ue_pdcp_interfaces.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include "srsran/rlc/rlc_am_lte_packing.h"
#include "srsran/srslog/event_trace.h"
#include <iostream>

#define RX_MOD_BASE(x) (((x)-vr_r) % 1024)
#define TX_MOD_BASE(x) (((x)-vt_a) % 1024)
#define LCID (parent->lcid)
#define MAX_SDUS_PER_PDU (128)

namespace srsran {

using pdcp_pdu_info_lte  = pdcp_pdu_info<rlc_amd_pdu_header_t>;
using rlc_amd_tx_pdu_lte = rlc_amd_tx_pdu<rlc_amd_pdu_header_t>;
using rlc_am_pdu_segment = rlc_am_pdu_segment_pool<rlc_amd_pdu_header_t>::segment_resource;

/****************************************************************************
 * Tx subclass implementation
 ***************************************************************************/
rlc_am_lte_tx::rlc_am_lte_tx(rlc_am* parent_) :
  parent(parent_),
  pool(byte_buffer_pool::get_instance()),
  poll_retx_timer(parent_->timers->get_unique_timer()),
  status_prohibit_timer(parent_->timers->get_unique_timer()),
  rlc_am_base_tx(parent_->logger)
{
  rx = dynamic_cast<rlc_am_lte_rx*>(parent->rx_base.get());
}

bool rlc_am_lte_tx::configure(const rlc_config_t& cfg_)
{
  std::lock_guard<std::mutex> lock(mutex);

  rb_name = parent->rb_name;

  if (cfg_.tx_queue_length > MAX_SDUS_PER_RLC_PDU) {
    RlcError("Configuring Tx queue length of %d PDUs too big. Maximum value is %d.",
             cfg_.tx_queue_length,
             MAX_SDUS_PER_RLC_PDU);
    return false;
  }

  // TODO: add more config checks
  cfg = cfg_.am;

  // check timers
  if (not poll_retx_timer.is_valid() or not status_prohibit_timer.is_valid()) {
    RlcError("Configuring RLC AM TX: timers not configured");
    return false;
  }

  // configure timers
  if (cfg.t_status_prohibit > 0) {
    status_prohibit_timer.set(static_cast<uint32_t>(cfg.t_status_prohibit),
                              [this](uint32_t timerid) { timer_expired(timerid); });
  }

  if (cfg.t_poll_retx > 0) {
    poll_retx_timer.set(static_cast<uint32_t>(cfg.t_poll_retx), [this](uint32_t timerid) { timer_expired(timerid); });
  }

  // make sure Tx queue is empty before attempting to resize
  empty_queue_nolock();
  tx_sdu_queue.resize(cfg_.tx_queue_length);

  tx_enabled = true;

  return true;
}

void rlc_am_lte_tx::stop()
{
  std::lock_guard<std::mutex> lock(mutex);
  stop_nolock();
}

void rlc_am_lte_tx::stop_nolock()
{
  empty_queue_nolock();

  tx_enabled = false;

  if (parent->timers != nullptr && poll_retx_timer.is_valid()) {
    poll_retx_timer.stop();
  }

  if (parent->timers != nullptr && status_prohibit_timer.is_valid()) {
    status_prohibit_timer.stop();
  }

  vt_a    = 0;
  vt_ms   = RLC_AM_WINDOW_SIZE;
  vt_s    = 0;
  poll_sn = 0;

  pdu_without_poll  = 0;
  byte_without_poll = 0;

  // Drop all messages in TX window
  tx_window.clear();

  // Drop all messages in RETX queue
  retx_queue.clear();

  // Drop all SDU info in queue
  undelivered_sdu_info_queue.clear();
}

void rlc_am_lte_tx::empty_queue()
{
  std::lock_guard<std::mutex> lock(mutex);
  empty_queue_nolock();
}

void rlc_am_lte_tx::empty_queue_nolock()
{
  // deallocate all SDUs in transmit queue
  while (tx_sdu_queue.size() > 0) {
    unique_byte_buffer_t buf = tx_sdu_queue.read();
  }

  // deallocate SDU that is currently processed
  if (tx_sdu != nullptr) {
    undelivered_sdu_info_queue.clear_pdcp_sdu(tx_sdu->md.pdcp_sn);
  }
  tx_sdu.reset();
}

void rlc_am_lte_tx::reestablish()
{
  std::lock_guard<std::mutex> lock(mutex);
  stop_nolock();
  tx_enabled = true;
}

bool rlc_am_lte_tx::do_status()
{
  return rx->get_do_status();
}

// Function is supposed to return as fast as possible
bool rlc_am_lte_tx::has_data()
{
  return (((do_status() && not status_prohibit_timer.is_running())) || // if we have a status PDU to transmit
          (not retx_queue.empty()) ||                                  // if we have a retransmission
          (tx_sdu != nullptr) ||                                       // if we are currently transmitting a SDU
          (tx_sdu_queue.get_n_sdus() != 0)); // or if there is a SDU queued up for transmission
}

/**
 * Helper to check if a SN has reached the max reTx threshold
 *
 * Caller _must_ hold the mutex when calling the function.
 * If the retx has been reached for a SN the upper layers (i.e. RRC/PDCP) will be informed.
 * The SN is _not_ removed from the Tx window, so retransmissions of that SN can still occur.
 *
 * @param  sn The SN of the PDU to check
 */
void rlc_am_lte_tx::check_sn_reached_max_retx(uint32_t sn)
{
  if (tx_window[sn].retx_count == cfg.max_retx_thresh) {
    RlcWarning("Signaling max number of reTx=%d for SN=%d", tx_window[sn].retx_count, sn);
    parent->rrc->max_retx_attempted();
    srsran::pdcp_sn_vector_t pdcp_sns;
    for (const rlc_am_pdu_segment& segment : tx_window[sn]) {
      pdcp_sns.push_back(segment.pdcp_sn());
    }
    parent->pdcp->notify_failure(parent->lcid, pdcp_sns);

    std::lock_guard<std::mutex> lock(parent->metrics_mutex);
    parent->metrics.num_lost_pdus++;
  }
}

uint32_t rlc_am_lte_tx::get_buffer_state()
{
  uint32_t new_tx_queue = 0, prio_tx_queue = 0;
  get_buffer_state(new_tx_queue, prio_tx_queue);
  return new_tx_queue + prio_tx_queue;
}

void rlc_am_lte_tx::get_buffer_state(uint32_t& n_bytes_newtx, uint32_t& n_bytes_prio)
{
  std::lock_guard<std::mutex> lock(mutex);
  get_buffer_state_nolock(n_bytes_newtx, n_bytes_prio);
}

void rlc_am_lte_tx::get_buffer_state_nolock(uint32_t& n_bytes_newtx, uint32_t& n_bytes_prio)
{
  n_bytes_newtx   = 0;
  n_bytes_prio    = 0;
  uint32_t n_sdus = 0;

  if (not tx_enabled) {
    return;
  }

  RlcDebug("Buffer state - do_status=%s, status_prohibit_running=%s (%d/%d)",
           do_status() ? "yes" : "no",
           status_prohibit_timer.is_running() ? "yes" : "no",
           status_prohibit_timer.time_elapsed(),
           status_prohibit_timer.duration());

  // Bytes needed for status report
  if (do_status() && not status_prohibit_timer.is_running()) {
    n_bytes_prio += rx->get_status_pdu_length();
    RlcDebug("Buffer state - total status report: %d bytes", n_bytes_prio);
  }

  // Bytes needed for retx
  if (not retx_queue.empty()) {
    rlc_amd_retx_lte_t& retx = retx_queue.front();
    RlcDebug("Buffer state - retx - SN=%d, Segment: %s, %d:%d",
             retx.sn,
             retx.is_segment ? "true" : "false",
             retx.so_start,
             retx.so_end);
    if (tx_window.has_sn(retx.sn)) {
      int req_bytes = required_buffer_size(retx);
      if (req_bytes < 0) {
        RlcError("In get_buffer_state(): Removing retx.sn=%d from queue", retx.sn);
        retx_queue.pop();
      } else {
        n_bytes_prio += req_bytes;
        RlcDebug("Buffer state - retx: %d bytes", n_bytes_prio);
      }
    }
  }

  // Bytes needed for tx SDUs
  if (not window_full()) {
    n_sdus = tx_sdu_queue.get_n_sdus();
    n_bytes_newtx += tx_sdu_queue.size_bytes();
    if (tx_sdu != NULL) {
      n_sdus++;
      n_bytes_newtx += tx_sdu->N_bytes;
    }
  }

  // Room needed for header extensions? (integer rounding)
  if (n_sdus > 1) {
    n_bytes_newtx += ((n_sdus - 1) * 1.5) + 0.5;
  }

  // Room needed for fixed header of data PDUs
  if (n_bytes_newtx > 0 && n_sdus > 0) {
    n_bytes_newtx += 2; // Two bytes for fixed header with SN length = 10
    RlcDebug("Total buffer state - %d SDUs (%d B)", n_sdus, n_bytes_newtx);
  }

  if (bsr_callback) {
    RlcDebug("Calling BSR callback - %d new_tx, %d prio bytes", n_bytes_newtx, n_bytes_prio);
    bsr_callback(parent->lcid, n_bytes_newtx, n_bytes_prio);
  }
}

uint32_t rlc_am_lte_tx::read_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (not tx_enabled) {
    return 0;
  }

  RlcDebug("MAC opportunity - %d bytes", nof_bytes);
  RlcDebug("tx_window size - %zu PDUs", tx_window.size());

  if (not tx_enabled) {
    RlcDebug("RLC entity not active. Not generating PDU.");
    return 0;
  }

  // Tx STATUS if requested
  if (do_status() && not status_prohibit_timer.is_running()) {
    return build_status_pdu(payload, nof_bytes);
  }

  // Section 5.2.2.3 in TS 36.311, if tx_window is full and retx_queue empty, retransmit PDU
  if (window_full() && retx_queue.empty()) {
    retransmit_pdu(vt_a);
  }

  // RETX if required
  if (not retx_queue.empty()) {
    int32_t pdu_size = build_retx_pdu(payload, nof_bytes);
    if (pdu_size > 0) {
      return pdu_size;
    }
  }

  // Build a PDU from SDUs
  return build_data_pdu(payload, nof_bytes);
}

void rlc_am_lte_tx::timer_expired(uint32_t timeout_id)
{
  std::unique_lock<std::mutex> lock(mutex);
  if (poll_retx_timer.is_valid() && poll_retx_timer.id() == timeout_id) {
    RlcDebug("Poll retx timer expired after %dms", poll_retx_timer.duration());
    // Section 5.2.2.3 in TS 36.322, schedule PDU for retransmission if
    // (a) both tx and retx buffer are empty (excluding tx'ed PDU waiting for ack), or
    // (b) no new data PDU can be transmitted (tx window is full)
    if ((retx_queue.empty() && tx_sdu_queue.size() == 0) || window_full()) {
      retransmit_pdu(vt_a); // TODO: TS says to send vt_s - 1 here
    }
  } else if (status_prohibit_timer.is_valid() && status_prohibit_timer.id() == timeout_id) {
    RlcDebug("Status prohibit timer expired after %dms", status_prohibit_timer.duration());
  }

  if (bsr_callback) {
    uint32_t new_tx_queue = 0, prio_tx_queue = 0;
    get_buffer_state_nolock(new_tx_queue, prio_tx_queue);
  }
}

void rlc_am_lte_tx::retransmit_pdu(uint32_t sn)
{
  if (tx_window.empty()) {
    RlcWarning("No PDU to retransmit");
    return;
  }

  if (not tx_window.has_sn(sn)) {
    RlcWarning("Can't retransmit unexisting SN=%d", sn);
    return;
  }

  // select first PDU in tx window for retransmission
  rlc_amd_tx_pdu_lte& pdu = tx_window[sn];

  // increment retx counter and inform upper layers
  pdu.retx_count++;
  check_sn_reached_max_retx(sn);

  RlcInfo("Schedule SN=%d for retx", pdu.rlc_sn);

  rlc_amd_retx_lte_t& retx = retx_queue.push();
  retx.is_segment          = false;
  retx.so_start            = 0;
  retx.so_end              = pdu.buf->N_bytes;
  retx.sn                  = pdu.rlc_sn;
}

/****************************************************************************
 * Helper functions
 ***************************************************************************/

bool rlc_am_lte_tx::window_full()
{
  return TX_MOD_BASE(vt_s) >= RLC_AM_WINDOW_SIZE;
};

/**
 * Called when building a RLC PDU for checking whether the poll bit needs
 * to be set.
 *
 * Note that this is called from a PHY worker thread.
 *
 * @return True if a status PDU needs to be requested, false otherwise.
 */
bool rlc_am_lte_tx::poll_required()
{
  if (cfg.poll_pdu > 0 && pdu_without_poll > static_cast<uint32_t>(cfg.poll_pdu)) {
    RlcDebug("Poll required. Cause: PDU_WITHOUT_POLL > pollPdu.");
    return true;
  }

  if (cfg.poll_byte > 0 && byte_without_poll > static_cast<uint32_t>(cfg.poll_byte)) {
    RlcDebug("Poll required. Cause: BYTE_WITHOUT_POLL > pollByte.");
    return true;
  }

  if (poll_retx_timer.is_valid() && poll_retx_timer.is_expired()) {
    // re-arming of timer is handled by caller
    RlcDebug("Poll required. Cause: t-PollRetransmission expired.");
    return true;
  }

  if (window_full()) {
    RlcDebug("Poll required. Cause: TX window full.");
    return true;
  }

  if (tx_sdu_queue.size() == 0 && retx_queue.empty()) {
    RlcDebug("Poll required. Cause: Empty TX and ReTX queues.");
    return true;
  }

  /* According to 5.2.2.1 in 36.322 v13.3.0 a poll should be requested if
   * the entire AM window is unacknowledged, i.e. no new PDU can be transmitted.
   * However, it seems more appropriate to request more often if polling
   * is disabled otherwise, e.g. every N PDUs.
   */
  if (cfg.poll_pdu == 0 && cfg.poll_byte == 0 && vt_s % rlc_am::poll_periodicity == 0) {
    return true;
  }

  return false;
}

int rlc_am_lte_tx::build_status_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  RlcDebug("Generating status PDU. Nof bytes %d", nof_bytes);
  int pdu_len = rx->get_status_pdu(&tx_status, nof_bytes);
  if (pdu_len == SRSRAN_ERROR) {
    RlcDebug("Deferred Status PDU. Cause: Failed to acquire Rx lock");
    pdu_len = 0;
  } else if (pdu_len > 0 && nof_bytes >= static_cast<uint32_t>(pdu_len)) {
    log_rlc_am_status_pdu_to_string(logger.info, rb_name, "Tx status PDU - %s", &tx_status);
    if (cfg.t_status_prohibit > 0 && status_prohibit_timer.is_valid()) {
      // re-arm timer
      status_prohibit_timer.run();
    }
    debug_state();
    pdu_len = rlc_am_write_status_pdu(&tx_status, payload);
  } else {
    RlcInfo("Cannot tx status PDU - %d bytes available, %d bytes required", nof_bytes, pdu_len);
    pdu_len = 0;
  }

  return pdu_len;
}

int rlc_am_lte_tx::build_retx_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  // Check there is at least 1 element before calling front()
  if (retx_queue.empty()) {
    RlcError("In build_retx_pdu(): retx_queue is empty");
    return -1;
  }

  rlc_amd_retx_lte_t retx = retx_queue.front();

  // Sanity check - drop any retx SNs not present in tx_window
  while (not tx_window.has_sn(retx.sn)) {
    retx_queue.pop();
    if (!retx_queue.empty()) {
      retx = retx_queue.front();
    } else {
      RlcInfo("SN=%d not in Tx window. Ignoring retx.", retx.sn);
      if (tx_window.has_sn(vt_a)) {
        // schedule next SN for retx
        retransmit_pdu(vt_a);
        retx = retx_queue.front();
      } else {
        // empty tx window, can't provide retx PDU
        return 0;
      }
    }
  }

  // Is resegmentation needed?
  int req_size = required_buffer_size(retx);
  if (req_size < 0) {
    RlcError("In build_retx_pdu(): Removing retx.sn=%d from queue", retx.sn);
    retx_queue.pop();
    return -1;
  }

  if (retx.is_segment || req_size > static_cast<int>(nof_bytes)) {
    RlcDebug("build_retx_pdu - resegmentation required");
    return build_segment(payload, nof_bytes, retx);
  }

  // Update & write header
  rlc_amd_pdu_header_t new_header = tx_window[retx.sn].header;
  new_header.p                    = 0;

  // Set poll bit
  pdu_without_poll++;
  byte_without_poll += (tx_window[retx.sn].buf->N_bytes + rlc_am_packed_length(&new_header));
  RlcInfo("pdu_without_poll: %d", pdu_without_poll);
  RlcInfo("byte_without_poll: %d", byte_without_poll);
  if (poll_required()) {
    new_header.p = 1;
    // vt_s won't change for reTx, so don't update poll_sn
    pdu_without_poll  = 0;
    byte_without_poll = 0;
    if (poll_retx_timer.is_valid()) {
      // re-arm timer (will be stopped when status PDU is received)
      RlcDebug("re-arming retx timer");
      poll_retx_timer.run();
    }
  }

  uint8_t* ptr = payload;
  rlc_am_write_data_pdu_header(&new_header, &ptr);
  memcpy(ptr, tx_window[retx.sn].buf->msg, tx_window[retx.sn].buf->N_bytes);

  retx_queue.pop();

  RlcHexInfo(payload,
             tx_window[retx.sn].buf->N_bytes,
             "Tx PDU SN=%d (%d B) (attempt %d/%d)",
             retx.sn,
             tx_window[retx.sn].buf->N_bytes,
             tx_window[retx.sn].retx_count + 1,
             cfg.max_retx_thresh);
  log_rlc_amd_pdu_header_to_string(logger.debug, rb_name, "Tx PDU - %s", new_header);

  debug_state();
  return (ptr - payload) + tx_window[retx.sn].buf->N_bytes;
}

int rlc_am_lte_tx::build_segment(uint8_t* payload, uint32_t nof_bytes, rlc_amd_retx_lte_t retx)
{
  if (tx_window[retx.sn].buf == NULL) {
    RlcError("In build_segment: retx.sn=%d has null buffer", retx.sn);
    return 0;
  }
  if (!retx.is_segment) {
    retx.so_start = 0;
    retx.so_end   = tx_window[retx.sn].buf->N_bytes;
  }

  // Construct new header
  rlc_amd_pdu_header_t new_header;
  rlc_amd_pdu_header_t old_header = tx_window[retx.sn].header;

  pdu_without_poll++;
  byte_without_poll += (tx_window[retx.sn].buf->N_bytes + rlc_am_packed_length(&new_header));
  RlcInfo("pdu_without_poll: %d, byte_without_poll: %d", pdu_without_poll, byte_without_poll);

  new_header.dc   = RLC_DC_FIELD_DATA_PDU;
  new_header.rf   = 1;
  new_header.fi   = RLC_FI_FIELD_NOT_START_OR_END_ALIGNED;
  new_header.sn   = old_header.sn;
  new_header.lsf  = 0;
  new_header.so   = retx.so_start;
  new_header.N_li = 0;
  new_header.p    = 0; // Poll Requirements are done later after updating RETX queue

  uint32_t head_len  = 0;
  uint32_t pdu_space = 0;

  head_len = rlc_am_packed_length(&new_header);
  if (old_header.N_li > 0) {
    // Make sure we can fit at least one N_li element if old header contained at least one
    head_len += 2;
  }

  if (nof_bytes <= head_len) {
    RlcInfo("Cannot build a PDU segment - %d bytes available, %d bytes required for header", nof_bytes, head_len);
    return 0;
  }

  pdu_space = nof_bytes - head_len;
  if (pdu_space < (retx.so_end - retx.so_start)) {
    retx.so_end = retx.so_start + pdu_space;
  }

  // Need to rebuild the li table & update fi based on so_start and so_end
  if (retx.so_start == 0 && rlc_am_start_aligned(old_header.fi)) {
    new_header.fi &= RLC_FI_FIELD_NOT_END_ALIGNED; // segment is start aligned
  }

  uint32_t lower = 0;
  uint32_t upper = 0;
  uint32_t li    = 0;

  for (uint32_t i = 0; i < old_header.N_li; i++) {
    if (lower >= retx.so_end) {
      break;
    }

    upper += old_header.li[i];

    head_len = rlc_am_packed_length(&new_header);

    pdu_space = nof_bytes - head_len;

    if (pdu_space < (retx.so_end - retx.so_start)) {
      retx.so_end = retx.so_start + pdu_space;
    }

    if (upper > retx.so_start && lower < retx.so_end) { // Current SDU is needed
      li = upper - lower;
      if (upper > retx.so_end) {
        li -= upper - retx.so_end;
      }
      if (lower < retx.so_start) {
        li -= retx.so_start - lower;
      }
      if (lower > 0 && lower == retx.so_start) {
        new_header.fi &= RLC_FI_FIELD_NOT_END_ALIGNED; // segment start is aligned with this SDU
      }
      if (upper == retx.so_end) {
        new_header.fi &= RLC_FI_FIELD_NOT_START_ALIGNED; // segment end is aligned with this SDU
      }
      new_header.li[new_header.N_li] = li;

      // only increment N_li if more SDU (segments) are/can being added
      if (retx.so_end > upper) {
        // Calculate header space for possible segment addition
        rlc_amd_pdu_header_t tmp_header = new_header;
        tmp_header.N_li++;
        uint32_t tmp_header_len = rlc_am_packed_length(&tmp_header);
        uint32_t tmp_data_len   = retx.so_end - retx.so_start;
        if (tmp_header_len + tmp_data_len <= nof_bytes) {
          // Space is sufficiant to fit at least 1 B of yet another segment
          new_header.N_li++;
        } else {
          // can't add new SDU, calculate total data length
          uint32_t data_len = 0;
          for (uint32_t k = 0; k <= new_header.N_li; ++k) {
            data_len += new_header.li[k];
          }
          retx.so_end = retx.so_start + data_len;
          new_header.fi &= RLC_FI_FIELD_NOT_START_ALIGNED; // segment end is aligned with this SDU
        }
      }
    }

    lower += old_header.li[i];
  }

  // Santity check we don't pack beyond the provided buffer
  srsran_expect(head_len + (retx.so_end - retx.so_start) <= nof_bytes, "The provided buffer was overflown.");

  // Update retx_queue
  if (tx_window[retx.sn].buf->N_bytes == retx.so_end) {
    retx_queue.pop();
    new_header.lsf = 1;
    if (rlc_am_end_aligned(old_header.fi)) {
      new_header.fi &= RLC_FI_FIELD_NOT_START_ALIGNED; // segment is end aligned
    }
  } else if (retx_queue.front().so_end == retx.so_end) {
    retx_queue.pop();
  } else {
    retx_queue.front().is_segment = true;
    retx_queue.front().so_start   = retx.so_end;
  }

  // Check POLL requeriments for segment
  if (poll_required()) {
    RlcDebug("setting poll bit to request status");
    new_header.p = 1;
    // vt_s won't change for reTx, so don't update poll_sn
    pdu_without_poll  = 0;
    byte_without_poll = 0;
    if (poll_retx_timer.is_valid()) {
      poll_retx_timer.run();
    }
  }

  // Write header and pdu
  uint8_t* ptr = payload;
  rlc_am_write_data_pdu_header(&new_header, &ptr);
  uint8_t* data = &tx_window[retx.sn].buf->msg[retx.so_start];
  uint32_t len  = retx.so_end - retx.so_start;
  memcpy(ptr, data, len);

  debug_state();
  int pdu_len = (ptr - payload) + len;
  if (pdu_len > static_cast<int>(nof_bytes)) {
    RlcError("Retx PDU segment length error. Available: %d, Used: %d", nof_bytes, pdu_len);
    int header_len = (ptr - payload);
    RlcDebug("Retx PDU segment length error. Actual header len: %d, Payload len: %d, N_li: %d",
             header_len,
             len,
             new_header.N_li);
  }

  RlcHexInfo(payload,
             pdu_len,
             "retx PDU segment SN=%d [so=%d] (%d B) (attempt %d/%d)",
             retx.sn,
             retx.so_start,
             pdu_len,
             tx_window[retx.sn].retx_count + 1,
             cfg.max_retx_thresh);

  return pdu_len;
}

int rlc_am_lte_tx::build_data_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  if (tx_sdu == NULL && tx_sdu_queue.is_empty()) {
    RlcInfo("No data available to be sent");
    return 0;
  }

  // do not build any more PDU if window is already full
  if (window_full()) {
    RlcInfo("Cannot build data PDU - Tx window full.");
    return 0;
  }

  if (nof_bytes < RLC_AM_MIN_DATA_PDU_SIZE) {
    RlcInfo("Cannot build data PDU - %d bytes available but at least %d bytes are required ",
            nof_bytes,
            RLC_AM_MIN_DATA_PDU_SIZE);
    return 0;
  }

  unique_byte_buffer_t pdu = srsran::make_byte_buffer();
  if (pdu == NULL) {
#ifdef RLC_AM_BUFFER_DEBUG
    srsran::console("Fatal Error: Could not allocate PDU in build_data_pdu()\n");
    srsran::console("tx_window size: %zd PDUs\n", tx_window.size());
    srsran::console("vt_a = %d, vt_ms = %d, vt_s = %d, poll_sn = %d\n", vt_a, vt_ms, vt_s, poll_sn);
    srsran::console("retx_queue size: %zd PDUs\n", retx_queue.size());
    std::map<uint32_t, rlc_amd_tx_pdu>::iterator txit;
    for (txit = tx_window.begin(); txit != tx_window.end(); txit++) {
      srsran::console("tx_window - SN=%d\n", txit->first);
    }
    exit(-1);
#else
    RlcError("Fatal Error: Couldn't allocate PDU in build_data_pdu().");
    return 0;
#endif
  }
  rlc_amd_pdu_header_t header = {};
  header.dc                   = RLC_DC_FIELD_DATA_PDU;
  header.fi                   = RLC_FI_FIELD_START_AND_END_ALIGNED;
  header.sn                   = vt_s;

  if (not segment_pool.has_segments()) {
    RlcInfo("Can't build a PDU - No segments available");
    return 0;
  }

  // insert newly assigned SN into window and use reference for in-place operations
  // NOTE: from now on, we can't return from this function anymore before increasing vt_s
  rlc_amd_tx_pdu_lte& tx_pdu = tx_window.add_pdu(header.sn);

  uint32_t head_len  = rlc_am_packed_length(&header);
  uint32_t to_move   = 0;
  uint32_t last_li   = 0;
  uint32_t pdu_space = SRSRAN_MIN(nof_bytes, pdu->get_tailroom());
  uint8_t* pdu_ptr   = pdu->msg;

  RlcDebug("Building PDU - pdu_space: %d, head_len: %d ", pdu_space, head_len);

  // Check for SDU segment
  if (tx_sdu != nullptr) {
    to_move = ((pdu_space - head_len) >= tx_sdu->N_bytes) ? tx_sdu->N_bytes : pdu_space - head_len;
    memcpy(pdu_ptr, tx_sdu->msg, to_move);
    last_li = to_move;
    pdu_ptr += to_move;
    pdu->N_bytes += to_move;
    tx_sdu->N_bytes -= to_move;
    tx_sdu->msg += to_move;
    if (undelivered_sdu_info_queue.has_pdcp_sn(tx_sdu->md.pdcp_sn)) {
      pdcp_pdu_info_lte& pdcp_pdu = undelivered_sdu_info_queue[tx_sdu->md.pdcp_sn];
      segment_pool.make_segment(tx_pdu, pdcp_pdu);
      if (tx_sdu->N_bytes == 0) {
        pdcp_pdu.fully_txed = true;
      }
    } else {
      // PDCP SNs for the RLC SDU has been removed from the queue
      RlcWarning("Couldn't find PDCP_SN=%d in SDU info queue (segment)", tx_sdu->md.pdcp_sn);
    }

    if (tx_sdu->N_bytes == 0) {
      RlcDebug("Complete SDU scheduled for tx.");
      tx_sdu.reset();
    }
    if (pdu_space > to_move) {
      pdu_space -= SRSRAN_MIN(to_move, pdu->get_tailroom());
    } else {
      pdu_space = 0;
    }
    header.fi |= RLC_FI_FIELD_NOT_START_ALIGNED; // First byte does not correspond to first byte of SDU

    RlcDebug("Building PDU - added SDU segment from previous PDU (len:%d) - pdu_space: %d, head_len: %d header_sn=%d",
             to_move,
             pdu_space,
             head_len,
             header.sn);
  }

  // Pull SDUs from queue
  while (pdu_space > head_len && tx_sdu_queue.get_n_sdus() > 0 && header.N_li < MAX_SDUS_PER_PDU) {
    if (not segment_pool.has_segments()) {
      RlcInfo("Can't build a PDU segment - No segment resources available");
      if (pdu_ptr != pdu->msg) {
        break; // continue with the segments created up to this point
      }
      tx_window.remove_pdu(tx_pdu.rlc_sn);
      return 0;
    }
    if (last_li > 0) {
      header.li[header.N_li] = last_li;
      header.N_li++;
    }
    head_len = rlc_am_packed_length(&header);
    if (head_len >= pdu_space) {
      if (header.N_li > 0) {
        header.N_li--;
      }
      break;
    }

    do {
      tx_sdu = tx_sdu_queue.read();
    } while (tx_sdu == nullptr && tx_sdu_queue.size() != 0);
    if (tx_sdu == nullptr) {
      if (header.N_li > 0) {
        header.N_li--;
      }
      break;
    }

    // store sdu info
    if (undelivered_sdu_info_queue.has_pdcp_sn(tx_sdu->md.pdcp_sn)) {
      RlcWarning("PDCP_SN=%d already marked as undelivered", tx_sdu->md.pdcp_sn);
    } else {
      RlcDebug("marking pdcp_sn=%d as undelivered (queue_len=%ld)",
               tx_sdu->md.pdcp_sn,
               undelivered_sdu_info_queue.nof_sdus());
      undelivered_sdu_info_queue.add_pdcp_sdu(tx_sdu->md.pdcp_sn);
    }
    pdcp_pdu_info_lte& pdcp_pdu = undelivered_sdu_info_queue[tx_sdu->md.pdcp_sn];

    to_move = ((pdu_space - head_len) >= tx_sdu->N_bytes) ? tx_sdu->N_bytes : pdu_space - head_len;
    memcpy(pdu_ptr, tx_sdu->msg, to_move);
    last_li = to_move;
    pdu_ptr += to_move;
    pdu->N_bytes += to_move;
    tx_sdu->N_bytes -= to_move;
    tx_sdu->msg += to_move;
    segment_pool.make_segment(tx_pdu, pdcp_pdu);
    if (tx_sdu->N_bytes == 0) {
      pdcp_pdu.fully_txed = true;
    }

    if (tx_sdu->N_bytes == 0) {
      RlcDebug("Complete SDU scheduled for tx. PDCP SN=%d", tx_sdu->md.pdcp_sn);
      tx_sdu.reset();
    }
    if (pdu_space > to_move) {
      pdu_space -= to_move;
    } else {
      pdu_space = 0;
    }

    RlcDebug("Building PDU - added SDU segment (len:%d) - pdu_space: %d, head_len: %d ", to_move, pdu_space, head_len);
  }

  // Make sure, at least one SDU (segment) has been added until this point
  if (pdu->N_bytes == 0) {
    RlcError("Generated empty RLC PDU.");
  }

  if (tx_sdu != NULL) {
    header.fi |= RLC_FI_FIELD_NOT_END_ALIGNED; // Last byte does not correspond to last byte of SDU
  }

  // Set Poll bit
  pdu_without_poll++;
  byte_without_poll += (pdu->N_bytes + head_len);
  RlcDebug("pdu_without_poll: %d", pdu_without_poll);
  RlcDebug("byte_without_poll: %d", byte_without_poll);
  if (poll_required()) {
    RlcDebug("setting poll bit to request status");
    header.p          = 1;
    poll_sn           = vt_s;
    pdu_without_poll  = 0;
    byte_without_poll = 0;
    if (poll_retx_timer.is_valid()) {
      poll_retx_timer.run();
    }
  }

  // Update Tx window
  vt_s = (vt_s + 1) % MOD;

  // Write final header and TX
  tx_pdu.buf                      = std::move(pdu);
  tx_pdu.header                   = header;
  const byte_buffer_t* buffer_ptr = tx_pdu.buf.get();

  uint8_t* ptr = payload;
  rlc_am_write_data_pdu_header(&header, &ptr);
  memcpy(ptr, buffer_ptr->msg, buffer_ptr->N_bytes);
  int total_len = (ptr - payload) + buffer_ptr->N_bytes;
  RlcHexInfo(payload, total_len, "Tx PDU SN=%d (%d B)", header.sn, total_len);
  log_rlc_amd_pdu_header_to_string(logger.debug, rb_name, "%s", header);
  debug_state();

  return total_len;
}

void rlc_am_lte_tx::handle_control_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  if (not tx_enabled) {
    return;
  }

  // Local variables for handling Status PDU will be updated with lock
  rlc_status_pdu_t status     = {};
  uint32_t         i          = 0;
  uint32_t         vt_s_local = 0;

  {
    std::lock_guard<std::mutex> lock(mutex);

    RlcHexDebug(payload, nof_bytes, "Rx control PDU");

    rlc_am_read_status_pdu(payload, nof_bytes, &status);

    log_rlc_am_status_pdu_to_string(logger.info, rb_name, "Rx Status PDU %s", &status);

    // make sure ACK_SN is within our Tx window
    if (((MOD + status.ack_sn - vt_a) % MOD > RLC_AM_WINDOW_SIZE) ||
        ((MOD + vt_s - status.ack_sn) % MOD > RLC_AM_WINDOW_SIZE)) {
      RlcWarning("Received invalid status PDU (ack_sn=%d, vt_a=%d, vt_s=%d). Dropping PDU.", status.ack_sn, vt_a, vt_s);
      return;
    }

    // Sec 5.2.2.2, stop poll reTx timer if status PDU comprises a positive _or_ negative acknowledgement
    // for the RLC data PDU with sequence number poll_sn
    if (poll_retx_timer.is_valid() && (TX_MOD_BASE(poll_sn) < TX_MOD_BASE(status.ack_sn))) {
      RlcDebug("Stopping pollRetx timer");
      poll_retx_timer.stop();
    }

    // flush retx queue to avoid unordered SNs, we expect the Rx to request lost PDUs again
    if (status.N_nack > 0) {
      retx_queue.clear();
    }

    i          = vt_a;
    vt_s_local = vt_s;
  }

  bool update_vt_a = true;
  while (TX_MOD_BASE(i) < TX_MOD_BASE(status.ack_sn) && TX_MOD_BASE(i) < TX_MOD_BASE(vt_s_local)) {
    bool nack = false;
    for (uint32_t j = 0; j < status.N_nack; j++) {
      if (status.nacks[j].nack_sn == i) {
        nack        = true;
        update_vt_a = false;
        std::lock_guard<std::mutex> lock(mutex);
        if (tx_window.has_sn(i)) {
          auto& pdu = tx_window[i];

          // add to retx queue if it's not already there
          if (not retx_queue.has_sn(i)) {
            // increment Retx counter and inform upper layers if needed
            pdu.retx_count++;
            check_sn_reached_max_retx(i);

            rlc_amd_retx_lte_t& retx = retx_queue.push();
            srsran_expect(tx_window[i].rlc_sn == i, "Incorrect RLC SN=%d!=%d being accessed", tx_window[i].rlc_sn, i);
            retx.sn         = i;
            retx.is_segment = false;
            retx.so_start   = 0;
            retx.so_end     = pdu.buf->N_bytes;

            if (status.nacks[j].has_so) {
              // sanity check
              if (status.nacks[j].so_start >= pdu.buf->N_bytes) {
                // print error but try to send original PDU again
                RlcInfo("SO_start is larger than original PDU (%d >= %d)", status.nacks[j].so_start, pdu.buf->N_bytes);
                status.nacks[j].so_start = 0;
              }

              // check for special SO_end value
              if (status.nacks[j].so_end == 0x7FFF) {
                status.nacks[j].so_end = pdu.buf->N_bytes;
              } else {
                retx.so_end = status.nacks[j].so_end + 1;
              }

              if (status.nacks[j].so_start < pdu.buf->N_bytes && status.nacks[j].so_end <= pdu.buf->N_bytes) {
                retx.is_segment = true;
                retx.so_start   = status.nacks[j].so_start;
              } else {
                RlcWarning("invalid segment NACK received for SN %d. so_start: %d, so_end: %d, N_bytes: %d",
                           i,
                           status.nacks[j].so_start,
                           status.nacks[j].so_end,
                           pdu.buf->N_bytes);
              }
            }
          } else {
            RlcInfo("NACKed SN=%d already considered for retransmission", i);
          }
        } else {
          RlcError("NACKed SN=%d already removed from Tx window", i);
        }
      }
    }

    if (!nack) {
      // ACKed SNs get marked and removed from tx_window so PDCP get's only notified once
      std::lock_guard<std::mutex> lock(mutex);
      if (tx_window.has_sn(i)) {
        update_notification_ack_info(i);
        RlcDebug("Tx PDU SN=%zd being removed from tx window", i);
        tx_window.remove_pdu(i);
      }
      // Advance window if possible
      if (update_vt_a) {
        vt_a  = (vt_a + 1) % MOD;
        vt_ms = (vt_ms + 1) % MOD;
      }
    }
    i = (i + 1) % MOD;
  }

  {
    // Make sure vt_a points to valid SN
    std::lock_guard<std::mutex> lock(mutex);
    if (not tx_window.empty() && not tx_window.has_sn(vt_a)) {
      RlcError("vt_a=%d points to invalid position in Tx window.", vt_a);
      parent->rrc->protocol_failure();
    }
  }

  debug_state();

  // Notify PDCP without holding Tx mutex
  if (not notify_info_vec.empty()) {
    parent->pdcp->notify_delivery(parent->lcid, notify_info_vec);
  }
  notify_info_vec.clear();
}

/*
 * Helper function to detect whether a PDU has been fully ack'ed and the PDCP needs to be notified about it
 * @tx_pdu: RLC PDU that was ack'ed.
 * @notify_info_vec: Vector which will keep track of the PDCP PDU SNs that have been fully ack'ed.
 */
void rlc_am_lte_tx::update_notification_ack_info(uint32_t rlc_sn)
{
  RlcDebug("Updating ACK info: RLC SN=%d, number of notified SDU=%ld, number of undelivered SDUs=%ld",
           rlc_sn,
           notify_info_vec.size(),
           undelivered_sdu_info_queue.nof_sdus());
  // Iterate over all undelivered SDUs
  if (not tx_window.has_sn(rlc_sn)) {
    return;
  }
  auto& acked_pdu = tx_window[rlc_sn];
  // Iterate over all PDCP SNs of the same RLC PDU that were TX'ed
  for (rlc_am_pdu_segment& acked_segment : acked_pdu) {
    uint32_t pdcp_sn = acked_segment.pdcp_sn();
    if (pdcp_sn == rlc_am_pdu_segment::invalid_pdcp_sn) {
      RlcDebug("ACKed segment in RLC_SN=%d already discarded in PDCP. No need to notify the PDCP.", rlc_sn);
      continue;
    }
    pdcp_pdu_info_lte& info = undelivered_sdu_info_queue[pdcp_sn];

    // Remove RLC SN from PDCP PDU undelivered list
    info.ack_segment(acked_segment);

    // Check whether the SDU was fully acked
    if (info.fully_acked()) {
      // Check if all SNs were ACK'ed
      if (not notify_info_vec.full()) {
        notify_info_vec.push_back(pdcp_sn);
      } else {
        RlcWarning("Can't notify delivery of PDCP_SN=%d.", pdcp_sn);
      }
      RlcDebug("Erasing SDU info: PDCP_SN=%d", pdcp_sn);
      undelivered_sdu_info_queue.clear_pdcp_sdu(pdcp_sn);
    }
  }
}

void rlc_am_lte_tx::debug_state()
{
  RlcDebug("vt_a = %d, vt_ms = %d, vt_s = %d, poll_sn = %d", vt_a, vt_ms, vt_s, poll_sn);
}

int rlc_am_lte_tx::required_buffer_size(const rlc_amd_retx_lte_t& retx)
{
  if (!retx.is_segment) {
    if (tx_window.has_sn(retx.sn)) {
      if (tx_window[retx.sn].buf) {
        return rlc_am_packed_length(&tx_window[retx.sn].header) + tx_window[retx.sn].buf->N_bytes;
      } else {
        RlcWarning("retx.sn=%d has null ptr in required_buffer_size()", retx.sn);
        return -1;
      }
    } else {
      RlcWarning("retx.sn=%d does not exist in required_buffer_size()", retx.sn);
      return -1;
    }
  }

  // Construct new header
  rlc_amd_pdu_header_t new_header;
  rlc_amd_pdu_header_t old_header = tx_window[retx.sn].header;

  new_header.dc   = RLC_DC_FIELD_DATA_PDU;
  new_header.rf   = 1;
  new_header.p    = 0;
  new_header.fi   = RLC_FI_FIELD_NOT_START_OR_END_ALIGNED;
  new_header.sn   = old_header.sn;
  new_header.lsf  = 0;
  new_header.so   = retx.so_start;
  new_header.N_li = 0;

  // Need to rebuild the li table & update fi based on so_start and so_end
  if (retx.so_start != 0 && rlc_am_start_aligned(old_header.fi)) {
    new_header.fi &= RLC_FI_FIELD_NOT_END_ALIGNED; // segment is start aligned
  }

  uint32_t lower = 0;
  uint32_t upper = 0;
  uint32_t li    = 0;

  for (uint32_t i = 0; i < old_header.N_li; i++) {
    if (lower >= retx.so_end) {
      break;
    }

    upper += old_header.li[i];

    if (upper > retx.so_start && lower < retx.so_end) { // Current SDU is needed
      li = upper - lower;
      if (upper > retx.so_end) {
        li -= upper - retx.so_end;
      }
      if (lower < retx.so_start) {
        li -= retx.so_start - lower;
      }
      if (lower > 0 && lower == retx.so_start) {
        new_header.fi &= RLC_FI_FIELD_NOT_END_ALIGNED; // segment start is aligned with this SDU
      }
      if (upper == retx.so_end) {
        new_header.fi &= RLC_FI_FIELD_NOT_START_ALIGNED; // segment end is aligned with this SDU
      }
      new_header.li[new_header.N_li++] = li;
    }

    lower += old_header.li[i];
  }

  //  if(tx_window[retx.sn].buf->N_bytes != retx.so_end) {
  //    if(new_header.N_li > 0)
  //      new_header.N_li--; // No li for last segment
  //  }

  return rlc_am_packed_length(&new_header) + (retx.so_end - retx.so_start);
}

/****************************************************************************
 * Rx subclass implementation
 ***************************************************************************/
rlc_am_lte_rx::rlc_am_lte_rx(rlc_am* parent_) :
  parent(parent_),
  pool(byte_buffer_pool::get_instance()),
  reordering_timer(parent_->timers->get_unique_timer()),
  rlc_am_base_rx(parent_, parent_->logger)
{
}

bool rlc_am_lte_rx::configure(const rlc_config_t& cfg_)
{
  // TODO: add config checks
  cfg = cfg_.am;

  rb_name = parent->rb_name;

  // check timers
  if (not reordering_timer.is_valid()) {
    RlcError("Configuring RLC AM TX: timers not configured");
    return false;
  }

  // configure timer
  if (cfg.t_reordering > 0) {
    reordering_timer.set(static_cast<uint32_t>(cfg.t_reordering), [this](uint32_t tid) { timer_expired(tid); });
  }

  return true;
}

void rlc_am_lte_rx::reestablish()
{
  stop();
}

void rlc_am_lte_rx::stop()
{
  std::lock_guard<std::mutex> lock(mutex);

  if (parent->timers != nullptr && reordering_timer.is_valid()) {
    reordering_timer.stop();
  }

  rx_sdu.reset();

  vr_r  = 0;
  vr_mr = RLC_AM_WINDOW_SIZE;
  vr_x  = 0;
  vr_ms = 0;
  vr_h  = 0;

  poll_received = false;
  do_status     = false;

  // Drop all messages in RX segments
  rx_segments.clear();

  // Drop all messages in RX window
  rx_window.clear();
}

/** Called from stack thread when MAC has received a new RLC PDU
 *
 * @param payload Pointer to payload
 * @param nof_bytes Payload length
 */
void rlc_am_lte_rx::handle_data_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  std::lock_guard<std::mutex> lock(mutex);

  rlc_amd_pdu_header_t header      = {};
  uint32_t             payload_len = nof_bytes;
  rlc_am_read_data_pdu_header(&payload, &payload_len, &header);
  if (payload_len > nof_bytes) {
    RlcInfo("Dropping corrupted PDU (%d B). Remaining length after header %d B.", nof_bytes, payload_len);
    return;
  }
  if (header.rf != 0) {
    handle_data_pdu_segment(payload, payload_len, header);
  } else {
    handle_data_pdu_full(payload, payload_len, header);
  }
}

/** Called from stack thread when MAC has received a new RLC PDU
 *
 * @param payload Pointer to payload
 * @param nof_bytes Payload length
 * @param header Reference to PDU header (unpacked by caller)
 */
void rlc_am_lte_rx::handle_data_pdu_full(uint8_t* payload, uint32_t nof_bytes, rlc_amd_pdu_header_t& header)
{
  std::map<uint32_t, rlc_amd_rx_pdu>::iterator it;

  RlcHexInfo(payload, nof_bytes, "Rx data PDU SN=%d (%d B)", header.sn, nof_bytes);
  log_rlc_amd_pdu_header_to_string(logger.debug, rb_name, "%s", header);

  // sanity check for segments not exceeding PDU length
  if (header.N_li > 0) {
    uint32_t segments_len = 0;
    for (uint32_t i = 0; i < header.N_li; i++) {
      segments_len += header.li[i];
      if (segments_len > nof_bytes) {
        RlcInfo("Dropping corrupted PDU (segments_len=%d > pdu_len=%d)", segments_len, nof_bytes);
        return;
      }
    }
  }

  if (!inside_rx_window(header.sn)) {
    if (header.p) {
      RlcInfo("Status packet requested through polling bit");
      do_status = true;
    }
    RlcInfo("SN=%d outside rx window [%d:%d] - discarding", header.sn, vr_r, vr_mr);
    return;
  }

  if (rx_window.has_sn(header.sn)) {
    if (header.p) {
      RlcInfo("Status packet requested through polling bit");
      do_status = true;
    }
    RlcInfo("Discarding duplicate SN=%d", header.sn);
    return;
  }

  // Write to rx window
  rlc_amd_rx_pdu& pdu = rx_window.add_pdu(header.sn);
  pdu.buf             = srsran::make_byte_buffer();
  if (pdu.buf == NULL) {
#ifdef RLC_AM_BUFFER_DEBUG
    srsran::console("Fatal Error: Couldn't allocate PDU in handle_data_pdu().\n");
    exit(-1);
#else
    RlcError("Fatal Error: Couldn't allocate PDU in handle_data_pdu().");
    rx_window.remove_pdu(header.sn);
    return;
#endif
  }
  pdu.buf->set_timestamp();

  // check available space for payload
  if (nof_bytes > pdu.buf->get_tailroom()) {
    RlcError("Discarding SN=%d of size %d B (available space %d B)", header.sn, nof_bytes, pdu.buf->get_tailroom());
    return;
  }
  memcpy(pdu.buf->msg, payload, nof_bytes);
  pdu.buf->N_bytes = nof_bytes;
  pdu.header       = header;

  // Update vr_h
  if (RX_MOD_BASE(header.sn) >= RX_MOD_BASE(vr_h)) {
    vr_h = (header.sn + 1) % MOD;
  }

  // Update vr_ms
  while (rx_window.has_sn(vr_ms)) {
    vr_ms = (vr_ms + 1) % MOD;
  }

  // Check poll bit
  if (header.p) {
    RlcInfo("Status packet requested through polling bit");
    poll_received = true;

    // 36.322 v10 Section 5.2.3
    if (RX_MOD_BASE(header.sn) < RX_MOD_BASE(vr_ms) || RX_MOD_BASE(header.sn) >= RX_MOD_BASE(vr_mr)) {
      do_status = true;
    }
    // else delay for reordering timer
  }

  // Reassemble and deliver SDUs
  reassemble_rx_sdus();

  // Update reordering variables and timers (36.322 v10.0.0 Section 5.1.3.2.3)
  if (reordering_timer.is_valid()) {
    if (reordering_timer.is_running()) {
      if (vr_x == vr_r || (!inside_rx_window(vr_x) && vr_x != vr_mr)) {
        RlcDebug("Stopping reordering timer.");
        reordering_timer.stop();
      } else {
        RlcDebug("Leave reordering timer running.");
      }
      debug_state();
    }

    if (not reordering_timer.is_running()) {
      if (RX_MOD_BASE(vr_h) > RX_MOD_BASE(vr_r)) {
        RlcDebug("Starting reordering timer.");
        reordering_timer.run();
        vr_x = vr_h;
      } else {
        RlcDebug("Leave reordering timer stopped.");
      }
      debug_state();
    }
  }

  debug_state();
}

void rlc_am_lte_rx::handle_data_pdu_segment(uint8_t* payload, uint32_t nof_bytes, rlc_amd_pdu_header_t& header)
{
  std::map<uint32_t, rlc_amd_rx_pdu_segments_t>::iterator it;

  RlcHexInfo(payload,
             nof_bytes,
             "Rx data PDU segment of SN=%d (%d B), SO=%d, N_li=%d",
             header.sn,
             nof_bytes,
             header.so,
             header.N_li);
  log_rlc_amd_pdu_header_to_string(logger.debug, rb_name, "Rx data PDU segment %s", header);

  // Check inside rx window
  if (!inside_rx_window(header.sn)) {
    if (header.p) {
      logger.info("Status packet requested through polling bit");
      do_status = true;
    }
    logger.info("SN=%d outside rx window [%d:%d] - discarding", header.sn, vr_r, vr_mr);
    return;
  }

  rlc_amd_rx_pdu segment;
  segment.buf = srsran::make_byte_buffer();
  if (segment.buf == NULL) {
#ifdef RLC_AM_BUFFER_DEBUG
    srsran::console("Fatal Error: Couldn't allocate PDU in handle_data_pdu_segment().\n");
    exit(-1);
#else
    logger.error("Fatal Error: Couldn't allocate PDU in handle_data_pdu_segment().");
    return;
#endif
  }

  if (segment.buf->get_tailroom() < nof_bytes) {
    RlcInfo("Dropping corrupted segment SN=%d, not enough space to fit %d B", header.sn, nof_bytes);
    return;
  }

  memcpy(segment.buf->msg, payload, nof_bytes);
  segment.buf->N_bytes = nof_bytes;
  segment.header       = header;

  // Check if we already have a segment from the same PDU
  it = rx_segments.find(header.sn);
  if (rx_segments.end() != it) {
    if (header.p) {
      RlcInfo("Status packet requested through polling bit");
      do_status = true;
    }

    // Add segment to PDU list and check for complete
    // NOTE: MAY MOVE. Preference would be to capture by value, and then move; but header is stack allocated
    if (add_segment_and_check(&it->second, &segment)) {
      rx_segments.erase(it);
    }

  } else {
    // Create new PDU segment list and write to rx_segments
    rlc_amd_rx_pdu_segments_t pdu;
    pdu.segments.push_back(std::move(segment));
    rx_segments[header.sn] = std::move(pdu);

    // Update vr_h
    if (RX_MOD_BASE(header.sn) >= RX_MOD_BASE(vr_h)) {
      vr_h = (header.sn + 1) % MOD;
    }

    // Check poll bit
    if (header.p) {
      RlcInfo("Status packet requested through polling bit");
      poll_received = true;

      // 36.322 v10 Section 5.2.3
      if (RX_MOD_BASE(header.sn) < RX_MOD_BASE(vr_ms) || RX_MOD_BASE(header.sn) >= RX_MOD_BASE(vr_mr)) {
        do_status = true;
      }
      // else delay for reordering timer
    }
  }
#ifdef RLC_AM_BUFFER_DEBUG
  print_rx_segments();
#endif
  debug_state();
}

void rlc_am_lte_rx::reassemble_rx_sdus()
{
  uint32_t len = 0;
  if (rx_sdu == NULL) {
    rx_sdu = srsran::make_byte_buffer();
    if (rx_sdu == NULL) {
#ifdef RLC_AM_BUFFER_DEBUG
      srsran::console("Fatal Error: Could not allocate PDU in reassemble_rx_sdus() (1)\n");
      exit(-1);
#else
      RlcError("Fatal Error: Could not allocate PDU in reassemble_rx_sdus() (1)");
      return;
#endif
    }
  }

  // Iterate through rx_window, assembling and delivering SDUs
  while (rx_window.has_sn(vr_r)) {
    // Handle any SDU segments
    for (uint32_t i = 0; i < rx_window[vr_r].header.N_li; i++) {
      len = rx_window[vr_r].header.li[i];

      RlcHexDebug(rx_window[vr_r].buf->msg,
                  len,
                  "Handling segment %d/%d of length %d B of SN=%d",
                  i + 1,
                  rx_window[vr_r].header.N_li,
                  len,
                  vr_r);

      // sanity check to avoid zero-size SDUs
      if (len == 0) {
        break;
      }

      if (rx_sdu->get_tailroom() >= len) {
        if ((rx_window[vr_r].buf->msg - rx_window[vr_r].buf->buffer) + len < SRSRAN_MAX_BUFFER_SIZE_BYTES) {
          if (rx_window[vr_r].buf->N_bytes < len) {
            RlcError("Dropping corrupted SN=%d", vr_r);
            rx_sdu.reset();
            goto exit;
          }
          // store timestamp of the first segment when starting to assemble SDUs
          if (rx_sdu->N_bytes == 0) {
            rx_sdu->set_timestamp(rx_window[vr_r].buf->get_timestamp());
          }
          memcpy(&rx_sdu->msg[rx_sdu->N_bytes], rx_window[vr_r].buf->msg, len);
          rx_sdu->N_bytes += len;

          rx_window[vr_r].buf->msg += len;
          rx_window[vr_r].buf->N_bytes -= len;

          RlcHexInfo(rx_sdu->msg, rx_sdu->N_bytes, "Rx SDU (%d B)", rx_sdu->N_bytes);
          sdu_rx_latency_ms.push(std::chrono::duration_cast<std::chrono::milliseconds>(
                                     std::chrono::high_resolution_clock::now() - rx_sdu->get_timestamp())
                                     .count());
          parent->pdcp->write_pdu(parent->lcid, std::move(rx_sdu));
          {
            std::lock_guard<std::mutex> lock(parent->metrics_mutex);
            parent->metrics.num_rx_sdus++;
          }

          rx_sdu = srsran::make_byte_buffer();
          if (rx_sdu == nullptr) {
#ifdef RLC_AM_BUFFER_DEBUG
            srsran::console("Fatal Error: Could not allocate PDU in reassemble_rx_sdus() (2)\n");
            exit(-1);
#else
            RlcError("Fatal Error: Could not allocate PDU in reassemble_rx_sdus() (2)");
            return;
#endif
          }
        } else {
          int buf_len = rx_window[vr_r].buf->msg - rx_window[vr_r].buf->buffer;
          RlcError("Cannot read %d bytes from rx_window. vr_r=%d, msg-buffer=%d B", len, vr_r, buf_len);
          rx_sdu.reset();
          goto exit;
        }
      } else {
        RlcError("Cannot fit RLC PDU in SDU buffer, dropping both.");
        rx_sdu.reset();
        goto exit;
      }
    }

    // Handle last segment
    len = rx_window[vr_r].buf->N_bytes;
    RlcHexDebug(rx_window[vr_r].buf->msg, len, "Handling last segment of length %d B of SN=%d", len, vr_r);
    if (rx_sdu->get_tailroom() >= len) {
      // store timestamp of the first segment when starting to assemble SDUs
      if (rx_sdu->N_bytes == 0) {
        rx_sdu->set_timestamp(rx_window[vr_r].buf->get_timestamp());
      }
      memcpy(&rx_sdu->msg[rx_sdu->N_bytes], rx_window[vr_r].buf->msg, len);
      rx_sdu->N_bytes += rx_window[vr_r].buf->N_bytes;
    } else {
      printf("Cannot fit RLC PDU in SDU buffer (tailroom=%d, len=%d), dropping both. Erasing SN=%d.\n",
             rx_sdu->get_tailroom(),
             len,
             vr_r);
      rx_sdu.reset();
      goto exit;
    }

    if (rlc_am_end_aligned(rx_window[vr_r].header.fi)) {
      RlcHexInfo(rx_sdu->msg, rx_sdu->N_bytes, "Rx SDU (%d B)", rx_sdu->N_bytes);
      sdu_rx_latency_ms.push(std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::high_resolution_clock::now() - rx_sdu->get_timestamp())
                                 .count());
      parent->pdcp->write_pdu(parent->lcid, std::move(rx_sdu));
      {
        std::lock_guard<std::mutex> lock(parent->metrics_mutex);
        parent->metrics.num_rx_sdus++;
      }

      rx_sdu = srsran::make_byte_buffer();
      if (rx_sdu == NULL) {
#ifdef RLC_AM_BUFFER_DEBUG
        srsran::console("Fatal Error: Could not allocate PDU in reassemble_rx_sdus() (3)\n");
        exit(-1);
#else
        RlcError("Fatal Error: Could not allocate PDU in reassemble_rx_sdus() (3)");
        return;
#endif
      }
    }

  exit:
    // Move the rx_window
    RlcDebug("Erasing SN=%d.", vr_r);
    // also erase any segments of this SN
    std::map<uint32_t, rlc_amd_rx_pdu_segments_t>::iterator it;
    it = rx_segments.find(vr_r);
    if (rx_segments.end() != it) {
      RlcDebug("Erasing segments of SN=%d", vr_r);
      std::list<rlc_amd_rx_pdu>::iterator segit;
      for (segit = it->second.segments.begin(); segit != it->second.segments.end(); ++segit) {
        RlcDebug(" Erasing segment of SN=%d SO=%d Len=%d N_li=%d",
                 segit->header.sn,
                 segit->header.so,
                 segit->buf->N_bytes,
                 segit->header.N_li);
      }
      it->second.segments.clear();
    }
    rx_window.remove_pdu(vr_r);
    vr_r  = (vr_r + 1) % MOD;
    vr_mr = (vr_mr + 1) % MOD;
  }
}

void rlc_am_lte_rx::reset_status()
{
  do_status     = false;
  poll_received = false;
}

bool rlc_am_lte_rx::get_do_status()
{
  return do_status.load(std::memory_order_relaxed);
}

uint32_t rlc_am_lte_rx::get_rx_buffered_bytes()
{
  std::lock_guard<std::mutex> lock(mutex);
  return rx_window.get_buffered_bytes();
}

uint32_t rlc_am_lte_rx::get_sdu_rx_latency_ms()
{
  std::lock_guard<std::mutex> lock(mutex);
  return sdu_rx_latency_ms.value();
}

/**
 * Function called from stack thread when timer has expired
 *
 * @param timeout_id
 */
void rlc_am_lte_rx::timer_expired(uint32_t timeout_id)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (reordering_timer.is_valid() and reordering_timer.id() == timeout_id) {
    RlcDebug("reordering timeout expiry - updating vr_ms (was %d)", vr_ms);

    // 36.322 v10 Section 5.1.3.2.4
    vr_ms = vr_x;
    while (rx_window.has_sn(vr_ms)) {
      vr_ms = (vr_ms + 1) % MOD;
    }

    if (poll_received) {
      do_status = true;
    }

    if (RX_MOD_BASE(vr_h) > RX_MOD_BASE(vr_ms)) {
      reordering_timer.run();
      vr_x = vr_h;
    }

    debug_state();
  }
}

// Called from Tx object to pack status PDU that doesn't exceed a given size
// If lock-acquisition fails, return -1. Otherwise it returns the length of the generated PDU.
int rlc_am_lte_rx::get_status_pdu(rlc_status_pdu_t* status, const uint32_t max_pdu_size)
{
  std::unique_lock<std::mutex> lock(mutex, std::try_to_lock);
  if (not lock.owns_lock()) {
    return SRSRAN_ERROR;
  }

  status->N_nack = 0;
  status->ack_sn = vr_r; // start with lower edge of the rx window

  // We don't use segment NACKs - just NACK the full PDU
  uint32_t i = vr_r;
  while (RX_MOD_BASE(i) <= RX_MOD_BASE(vr_ms) && status->N_nack < RLC_AM_WINDOW_SIZE) {
    if (rx_window.has_sn(i) || i == vr_ms) {
      // only update ACK_SN if this SN has been received, or if we reached the maximum possible SN
      status->ack_sn = i;
    } else {
      status->nacks[status->N_nack].nack_sn = i;
      status->N_nack++;
    }

    // make sure we don't exceed grant size
    if (rlc_am_packed_length(status) > max_pdu_size) {
      RlcDebug("Status PDU too big (%d > %d)", rlc_am_packed_length(status), max_pdu_size);
      if (status->N_nack >= 1 && status->N_nack < RLC_AM_WINDOW_SIZE) {
        RlcDebug("Removing last NACK SN=%d", status->nacks[status->N_nack].nack_sn);
        status->N_nack--;
        // make sure we don't have the current ACK_SN in the NACK list
        if (rlc_am_is_valid_status_pdu(*status, vr_r) == false) {
          // No space to send any NACKs, play safe and just ack lower edge
          RlcWarning("Resetting ACK_SN and N_nack to initial state");
          status->ack_sn = vr_r;
          status->N_nack = 0;
        }
      } else {
        RlcWarning("Failed to generate small enough status PDU (packed_len=%d, max_pdu_size=%d, status->N_nack=%d)",
                   rlc_am_packed_length(status),
                   max_pdu_size,
                   status->N_nack);
        return 0;
      }
      break;
    }
    i = (i + 1) % MOD;
  }

  // valid PDU could be generated
  reset_status();

  return rlc_am_packed_length(status);
}

// Called from Tx object to obtain length of the full status PDU
int rlc_am_lte_rx::get_status_pdu_length()
{
  std::unique_lock<std::mutex> lock(mutex, std::try_to_lock);
  if (not lock.owns_lock()) {
    return 0;
  }
  rlc_status_pdu_t status = {};
  status.ack_sn           = vr_ms;
  uint32_t i              = vr_r;
  while (RX_MOD_BASE(i) < RX_MOD_BASE(vr_ms) && status.N_nack < RLC_AM_WINDOW_SIZE) {
    if (not rx_window.has_sn(i)) {
      status.N_nack++;
    }
    i = (i + 1) % MOD;
  }
  return rlc_am_packed_length(&status);
}

void rlc_am_lte_rx::print_rx_segments()
{
  std::map<uint32_t, rlc_amd_rx_pdu_segments_t>::iterator it;
  std::stringstream                                       ss;
  ss << "rx_segments:" << std::endl;
  for (it = rx_segments.begin(); it != rx_segments.end(); it++) {
    std::list<rlc_amd_rx_pdu>::iterator segit;
    for (segit = it->second.segments.begin(); segit != it->second.segments.end(); segit++) {
      ss << "    SN=" << segit->header.sn << " SO:" << segit->header.so << " N:" << segit->buf->N_bytes
         << " N_li: " << segit->header.N_li << std::endl;
    }
  }
  RlcDebug("%s", ss.str().c_str());
}

// NOTE: Preference would be to capture by value, and then move; but header is stack allocated
bool rlc_am_lte_rx::add_segment_and_check(rlc_amd_rx_pdu_segments_t* pdu, rlc_amd_rx_pdu* segment)
{
  // Find segment insertion point in the list of segments
  auto it1 = pdu->segments.begin();
  while (it1 != pdu->segments.end() && (*it1).header.so < segment->header.so) {
    // Increment iterator
    it1++;
  }

  // Check if the insertion point was found
  if (it1 != pdu->segments.end()) {
    // Found insertion point
    rlc_amd_rx_pdu& s = *it1;
    if (s.header.so == segment->header.so) {
      // Same Segment offset
      if (segment->buf->N_bytes > s.buf->N_bytes) {
        // replace if the new one is bigger
        s = std::move(*segment);
      } else {
        // Ignore otherwise
      }
    } else if (s.header.so > segment->header.so) {
      pdu->segments.insert(it1, std::move(*segment));
    }
  } else {
    // Either the new segment is the latest or the only one, push back
    pdu->segments.push_back(std::move(*segment));
  }

  // Check for complete
  uint32_t                            so = 0;
  std::list<rlc_amd_rx_pdu>::iterator it, tmpit;
  for (it = pdu->segments.begin(); it != pdu->segments.end(); /* Do not increment */) {
    // Check that there is no gap between last segment and current; overlap allowed
    if (so < it->header.so) {
      // return
      return false;
    }

    // Check if segment is overlapped
    if (it->header.so + it->buf->N_bytes <= so) {
      // completely overlapped with previous segments, erase
      it = pdu->segments.erase(it); // Returns next iterator
    } else {
      // Update segment offset it shall not go backwards
      so = SRSRAN_MAX(so, it->header.so + it->buf->N_bytes);
      it++; // Increments iterator
    }
  }

  // Check for last segment flag available
  if (!pdu->segments.back().header.lsf) {
    return false;
  }

  // We have all segments of the PDU - reconstruct and handle
  rlc_amd_pdu_header_t header;
  header.dc   = RLC_DC_FIELD_DATA_PDU;
  header.rf   = 0;
  header.p    = 0;
  header.fi   = RLC_FI_FIELD_START_AND_END_ALIGNED;
  header.sn   = pdu->segments.front().header.sn;
  header.lsf  = 0;
  header.so   = 0;
  header.N_li = 0;

  // Reconstruct fi field
  header.fi |= (pdu->segments.front().header.fi & RLC_FI_FIELD_NOT_START_ALIGNED);
  header.fi |= (pdu->segments.back().header.fi & RLC_FI_FIELD_NOT_END_ALIGNED);

  RlcDebug("Starting header reconstruction of %zd segments", pdu->segments.size());

  // Reconstruct li fields
  uint16_t count          = 0;
  uint16_t carryover      = 0;
  uint16_t consumed_bytes = 0; // rolling sum of all allocated LIs during segment reconstruction

  for (it = pdu->segments.begin(); it != pdu->segments.end(); ++it) {
    RlcDebug(" Handling %d PDU segments", it->header.N_li);
    for (uint32_t i = 0; i < it->header.N_li; i++) {
      // variable marks total offset of each _processed_ LI of this segment
      uint32_t total_pdu_offset = it->header.so;
      for (uint32_t k = 0; k <= i; k++) {
        total_pdu_offset += it->header.li[k];
      }

      RlcDebug("  - (total_pdu_offset=%d, consumed_bytes=%d, header.li[i]=%d)",
               total_pdu_offset,
               consumed_bytes,
               header.li[i]);
      if (total_pdu_offset > header.li[i] && total_pdu_offset > consumed_bytes) {
        header.li[header.N_li] = total_pdu_offset - consumed_bytes;
        consumed_bytes         = total_pdu_offset;

        RlcDebug("  - adding segment %d/%d (%d B, SO=%d, carryover=%d, count=%d)",
                 i + 1,
                 it->header.N_li,
                 header.li[header.N_li],
                 header.so,
                 carryover,
                 count);
        header.N_li++;
        count += it->header.li[i];
        carryover = 0;
      } else {
        RlcDebug("  - Skipping segment in reTx PDU segment which is already included (%d B, SO=%d)",
                 it->header.li[i],
                 header.so);
      }
    }

    if (count <= it->buf->N_bytes) {
      carryover = it->header.so + it->buf->N_bytes;
      // substract all previous LIs
      for (uint32_t k = 0; k < header.N_li; ++k) {
        carryover -= header.li[k];
      }
      RlcDebug("Incremented carryover (it->buf->N_bytes=%d, count=%d). New carryover=%d",
               it->buf->N_bytes,
               count,
               carryover);
    } else {
      // Next segment would be too long, recalculate carryover
      header.N_li--;
      carryover = it->buf->N_bytes - (count - header.li[header.N_li]);
      RlcDebug("Recalculated carryover=%d (it->buf->N_bytes=%d, count=%d, header.li[header.N_li]=%d)",
               carryover,
               it->buf->N_bytes,
               count,
               header.li[header.N_li]);
    }

    tmpit = it;
    if (rlc_am_end_aligned(it->header.fi) && ++tmpit != pdu->segments.end()) {
      RlcDebug("Header is end-aligned, overwrite header.li[%d]=%d", header.N_li, carryover);
      header.li[header.N_li] = carryover;
      header.N_li++;
      consumed_bytes += carryover;
      carryover = 0;
    }
    count = 0;

    // set Poll bit if any of the segments had it set
    header.p |= it->header.p;
  }

  RlcDebug("Finished header reconstruction of %zd segments", pdu->segments.size());

  // Copy data
  unique_byte_buffer_t full_pdu = srsran::make_byte_buffer();
  if (full_pdu == NULL) {
#ifdef RLC_AM_BUFFER_DEBUG
    srsran::console("Fatal Error: Could not allocate PDU in add_segment_and_check()\n");
    exit(-1);
#else
    RlcError("Fatal Error: Could not allocate PDU in add_segment_and_check()");
    return false;
#endif
  }
  for (it = pdu->segments.begin(); it != pdu->segments.end(); it++) {
    // By default, the segment is not copied. It could be it is fully overlapped with previous segments
    uint32_t overlap = 0;
    uint32_t n       = 0;

    // Check if the segment has non-overlapped bytes
    if (it->header.so + it->buf->N_bytes > full_pdu->N_bytes) {
      // Calculate overlap and number of bytes
      overlap = full_pdu->N_bytes - it->header.so;
      n       = it->buf->N_bytes - overlap;
    }

    // Copy data itself
    memcpy(&full_pdu->msg[full_pdu->N_bytes], &it->buf->msg[overlap], n);
    full_pdu->N_bytes += n;
  }

  handle_data_pdu_full(full_pdu->msg, full_pdu->N_bytes, header);
  return true;
}

bool rlc_am_lte_rx::inside_rx_window(const int16_t sn)
{
  if (RX_MOD_BASE(sn) >= RX_MOD_BASE(static_cast<int16_t>(vr_r)) && RX_MOD_BASE(sn) < RX_MOD_BASE(vr_mr)) {
    return true;
  } else {
    return false;
  }
}

void rlc_am_lte_rx::debug_state()
{
  RlcDebug("vr_r = %d, vr_mr = %d, vr_x = %d, vr_ms = %d, vr_h = %d", vr_r, vr_mr, vr_x, vr_ms, vr_h);
}

} // namespace srsran
