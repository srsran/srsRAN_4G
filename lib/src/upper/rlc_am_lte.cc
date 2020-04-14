/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srslte/upper/rlc_am_lte.h"

#include <iostream>
#include <sstream>

#define MOD 1024
#define RX_MOD_BASE(x) (((x)-vr_r) % 1024)
#define TX_MOD_BASE(x) (((x)-vt_a) % 1024)
#define LCID (parent->lcid)
#define RB_NAME (parent->rb_name.c_str())

namespace srslte {

rlc_am_lte::rlc_am_lte(srslte::log_ref            log_,
                       uint32_t                   lcid_,
                       srsue::pdcp_interface_rlc* pdcp_,
                       srsue::rrc_interface_rlc*  rrc_,
                       srslte::timer_handler*     timers_) :
  log(log_),
  rrc(rrc_),
  pdcp(pdcp_),
  timers(timers_),
  lcid(lcid_),
  tx(this),
  rx(this)
{
}

// Applies new configuration. Must be just reestablished or initiated
bool rlc_am_lte::configure(const rlc_config_t& cfg_)
{
  // determine bearer name and configure Rx/Tx objects
  rb_name = rrc->get_rb_name(lcid);

  // store config
  cfg = cfg_;

  if (not rx.configure(cfg.am)) {
    log->error("Error configuring bearer (RX)\n");
    return false;
  }

  if (not tx.configure(cfg)) {
    log->error("Error configuring bearer (TX)\n");
    return false;
  }

  log->info("%s configured: t_poll_retx=%d, poll_pdu=%d, poll_byte=%d, max_retx_thresh=%d, "
            "t_reordering=%d, t_status_prohibit=%d\n",
            rb_name.c_str(),
            cfg.am.t_poll_retx,
            cfg.am.poll_pdu,
            cfg.am.poll_byte,
            cfg.am.max_retx_thresh,
            cfg.am.t_reordering,
            cfg.am.t_status_prohibit);
  return true;
}

void rlc_am_lte::empty_queue()
{
  // Drop all messages in TX SDU queue
  tx.empty_queue();
}

void rlc_am_lte::reestablish()
{
  log->debug("Reestablished bearer %s\n", rb_name.c_str());
  tx.reestablish(); // calls stop and enables tx again
  rx.reestablish(); // calls only stop
}

void rlc_am_lte::stop()
{
  log->debug("Stopped bearer %s\n", rb_name.c_str());
  tx.stop();
  rx.stop();
}

rlc_mode_t rlc_am_lte::get_mode()
{
  return rlc_mode_t::am;
}

uint32_t rlc_am_lte::get_bearer()
{
  return lcid;
}

rlc_bearer_metrics_t rlc_am_lte::get_metrics()
{
  return metrics;
}

void rlc_am_lte::reset_metrics()
{
  tx.reset_metrics();
  rx.reset_metrics();
}

/****************************************************************************
 * PDCP interface
 ***************************************************************************/

void rlc_am_lte::write_sdu(unique_byte_buffer_t sdu, bool blocking)
{
  tx.write_sdu(std::move(sdu), blocking);
}

void rlc_am_lte::discard_sdu(uint32_t discard_sn)
{
  tx.discard_sdu(discard_sn);
}

/****************************************************************************
 * MAC interface
 ***************************************************************************/

bool rlc_am_lte::has_data()
{
  return tx.has_data();
}

uint32_t rlc_am_lte::get_buffer_state()
{
  return tx.get_buffer_state();
}

int rlc_am_lte::read_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  return tx.read_pdu(payload, nof_bytes);
}

void rlc_am_lte::write_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  rx.write_pdu(payload, nof_bytes);
}

/****************************************************************************
 * Tx subclass implementation
 ***************************************************************************/

rlc_am_lte::rlc_am_lte_tx::rlc_am_lte_tx(rlc_am_lte* parent_) :
  parent(parent_),
  log(parent_->log),
  pool(byte_buffer_pool::get_instance()),
  poll_retx_timer(parent_->timers->get_unique_timer()),
  status_prohibit_timer(parent_->timers->get_unique_timer())
{
  pthread_mutex_init(&mutex, NULL);
}

rlc_am_lte::rlc_am_lte_tx::~rlc_am_lte_tx()
{
  pthread_mutex_destroy(&mutex);
}

bool rlc_am_lte::rlc_am_lte_tx::configure(const rlc_config_t& cfg_)
{
  // TODO: add config checks
  cfg = cfg_.am;

  // check timers
  if (not poll_retx_timer.is_valid() or not status_prohibit_timer.is_valid()) {
    log->error("Configuring RLC AM TX: timers not configured\n");
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

  tx_sdu_queue.resize(cfg_.tx_queue_length);

  tx_enabled = true;

  return true;
}

void rlc_am_lte::rlc_am_lte_tx::stop()
{
  empty_queue();

  pthread_mutex_lock(&mutex);

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
  pthread_mutex_unlock(&mutex);
}

void rlc_am_lte::rlc_am_lte_tx::empty_queue()
{
  pthread_mutex_lock(&mutex);

  // deallocate all SDUs in transmit queue
  while (tx_sdu_queue.size() > 0) {
    unique_byte_buffer_t buf = tx_sdu_queue.read();
  }

  // deallocate SDU that is currently processed
  tx_sdu.reset();

  pthread_mutex_unlock(&mutex);
}

void rlc_am_lte::rlc_am_lte_tx::reestablish()
{
  stop();
  tx_enabled = true;
}

bool rlc_am_lte::rlc_am_lte_tx::do_status()
{
  return parent->rx.get_do_status();
}

// Function is supposed to return as fast as possible
bool rlc_am_lte::rlc_am_lte_tx::has_data()
{
  return (((do_status() && not status_prohibit_timer.is_running())) || // if we have a status PDU to transmit
          (not retx_queue.empty()) ||                                  // if we have a retransmission
          (tx_sdu != NULL) ||                                          // if we are currently transmitting a SDU
          (not tx_sdu_queue.is_empty())); // or if there is a SDU queued up for transmission
}

uint32_t rlc_am_lte::rlc_am_lte_tx::get_buffer_state()
{
  pthread_mutex_lock(&mutex);
  uint32_t n_bytes = 0;
  uint32_t n_sdus  = 0;

  log->debug("%s Buffer state - do_status=%s, status_prohibit_running=%s (%d/%d)\n",
             RB_NAME,
             do_status() ? "yes" : "no",
             status_prohibit_timer.is_running() ? "yes" : "no",
             status_prohibit_timer.time_elapsed(),
             status_prohibit_timer.duration());

  // Bytes needed for status report
  if (do_status() && not status_prohibit_timer.is_running()) {
    n_bytes += parent->rx.get_status_pdu_length();
    log->debug("%s Buffer state - total status report: %d bytes\n", RB_NAME, n_bytes);
  }

  // Bytes needed for retx
  if (not retx_queue.empty()) {
    rlc_amd_retx_t retx = retx_queue.front();
    log->debug("%s Buffer state - retx - SN: %d, Segment: %s, %d:%d\n",
               RB_NAME,
               retx.sn,
               retx.is_segment ? "true" : "false",
               retx.so_start,
               retx.so_end);
    if (tx_window.end() != tx_window.find(retx.sn)) {
      int req_bytes = required_buffer_size(retx);
      if (req_bytes < 0) {
        log->error("In get_buffer_state(): Removing retx.sn=%d from queue\n", retx.sn);
        retx_queue.pop_front();
      } else {
        n_bytes += req_bytes;
        log->debug("Buffer state - retx: %d bytes\n", n_bytes);
      }
    }
  }

  // Bytes needed for tx SDUs
  if (tx_window.size() < 1024) {
    n_sdus = tx_sdu_queue.size();
    n_bytes += tx_sdu_queue.size_bytes();
    if (tx_sdu != NULL) {
      n_sdus++;
      n_bytes += tx_sdu->N_bytes;
    }
  }

  // Room needed for header extensions? (integer rounding)
  if (n_sdus > 1) {
    n_bytes += ((n_sdus - 1) * 1.5) + 0.5;
  }

  // Room needed for fixed header of data PDUs
  if (n_bytes > 0 && n_sdus > 0) {
    n_bytes += 3;
    log->debug("%s Total buffer state - %d SDUs (%d B)\n", RB_NAME, n_sdus, n_bytes);
  }

  pthread_mutex_unlock(&mutex);
  return n_bytes;
}

void rlc_am_lte::rlc_am_lte_tx::write_sdu(unique_byte_buffer_t sdu, bool blocking)
{
  if (!tx_enabled) {
    return;
  }

  if (sdu.get() == nullptr) {
    log->warning("NULL SDU pointer in write_sdu()\n");
    return;
  }

  if (blocking) {
    // block on write to queue
    log->info_hex(
        sdu->msg, sdu->N_bytes, "%s Tx SDU (%d B, tx_sdu_queue_len=%d)\n", RB_NAME, sdu->N_bytes, tx_sdu_queue.size());
    tx_sdu_queue.write(std::move(sdu));
  } else {
    // non-blocking write
    uint8_t*                              msg_ptr   = sdu->msg;
    uint32_t                              nof_bytes = sdu->N_bytes;
    std::pair<bool, unique_byte_buffer_t> ret       = tx_sdu_queue.try_write(std::move(sdu));
    if (ret.first) {
      log->info_hex(
          msg_ptr, nof_bytes, "%s Tx SDU (%d B, tx_sdu_queue_len=%d)\n", RB_NAME, nof_bytes, tx_sdu_queue.size());
    } else {
      // in case of fail, the try_write returns back the sdu
      log->info_hex(ret.second->msg,
                    ret.second->N_bytes,
                    "[Dropped SDU] %s Tx SDU (%d B, tx_sdu_queue_len=%d)\n",
                    RB_NAME,
                    ret.second->N_bytes,
                    tx_sdu_queue.size());
    }
  }
}

void rlc_am_lte::rlc_am_lte_tx::discard_sdu(uint32_t discard_sn)
{
  if (!tx_enabled) {
    return;
  }
  log->warning("Discard SDU not implemented yet\n");
}

int rlc_am_lte::rlc_am_lte_tx::read_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  pthread_mutex_lock(&mutex);

  int pdu_size = 0;

  log->debug("MAC opportunity - %d bytes\n", nof_bytes);
  log->debug("tx_window size - %zu PDUs\n", tx_window.size());

  if (not tx_enabled) {
    log->debug("RLC entity not active. Not generating PDU.\n");
    goto unlock_and_exit;
  }

  // Tx STATUS if requested
  if (do_status() && not status_prohibit_timer.is_running()) {
    pdu_size = build_status_pdu(payload, nof_bytes);
    goto unlock_and_exit;
  }

  // Section 5.2.2.3 in TS 36.311, if tx_window is full and retx_queue empty, retransmit random PDU
  if (tx_window.size() >= RLC_AM_WINDOW_SIZE && retx_queue.empty()) {
    retransmit_random_pdu();
  }

  // RETX if required
  if (not retx_queue.empty()) {
    pdu_size = build_retx_pdu(payload, nof_bytes);
    if (pdu_size > 0) {
      goto unlock_and_exit;
    }
  }

  // Build a PDU from SDUs
  pdu_size = build_data_pdu(payload, nof_bytes);

unlock_and_exit:
  num_tx_bytes += pdu_size;
  pthread_mutex_unlock(&mutex);
  return pdu_size;
}

void rlc_am_lte::rlc_am_lte_tx::timer_expired(uint32_t timeout_id)
{
  pthread_mutex_lock(&mutex);
  if (poll_retx_timer.is_valid() && poll_retx_timer.id() == timeout_id) {
    log->debug("Poll reTx timer expired for LCID=%d after %d ms\n", parent->lcid, poll_retx_timer.duration());
    // Section 5.2.2.3 in TS 36.311, schedule random PDU for retransmission if
    // (a) both tx and retx buffer are empty, or
    // (b) no new data PDU can be transmitted (tx window is full)
    if ((retx_queue.empty() && tx_sdu_queue.size() == 0) || tx_window.size() >= RLC_AM_WINDOW_SIZE) {
      retransmit_random_pdu();
    }
  }
  pthread_mutex_unlock(&mutex);
}

void rlc_am_lte::rlc_am_lte_tx::retransmit_random_pdu()
{
  if (not tx_window.empty()) {
    // randomly select PDU in tx window for retransmission
    std::map<uint32_t, rlc_amd_tx_pdu_t>::iterator it = tx_window.begin();
    std::advance(it, rand() % tx_window.size());
    log->info("Schedule SN=%d for reTx.\n", it->first);
    rlc_amd_retx_t retx = {};
    retx.is_segment     = false;
    retx.so_start       = 0;
    retx.so_end         = it->second.buf->N_bytes;
    retx.sn             = it->first;
    retx_queue.push_back(retx);
  }
}

uint32_t rlc_am_lte::rlc_am_lte_tx::get_num_tx_bytes()
{
  return num_tx_bytes;
}

void rlc_am_lte::rlc_am_lte_tx::reset_metrics()
{
  pthread_mutex_lock(&mutex);
  num_tx_bytes = 0;
  pthread_mutex_unlock(&mutex);
}

/****************************************************************************
 * Helper functions
 ***************************************************************************/

/**
 * Called when building a RLC PDU for checking whether the poll bit needs
 * to be set.
 *
 * Note that this is called from a PHY worker thread.
 *
 * @return True if a status PDU needs to be requested, false otherwise.
 */
bool rlc_am_lte::rlc_am_lte_tx::poll_required()
{
  if (cfg.poll_pdu > 0 && pdu_without_poll > static_cast<uint32_t>(cfg.poll_pdu)) {
    return true;
  }

  if (cfg.poll_byte > 0 && byte_without_poll > static_cast<uint32_t>(cfg.poll_byte)) {
    return true;
  }

  if (poll_retx_timer.is_valid() && poll_retx_timer.is_expired()) {
    // re-arming of timer is handled by caller
    return true;
  }

  if (tx_window.size() >= RLC_AM_WINDOW_SIZE) {
    return true;
  }

  if (tx_sdu_queue.size() == 0 && retx_queue.empty()) {
    return true;
  }

  /* According to 5.2.2.1 in 36.322 v13.3.0 a poll should be requested if
   * the entire AM window is unacknowledged, i.e. no new PDU can be transmitted.
   * However, it seems more appropiate to request more often if polling
   * is disabled otherwise, e.g. every N PDUs.
   */
  if (cfg.poll_pdu == 0 && cfg.poll_byte == 0 && vt_s % poll_periodicity == 0) {
    return true;
  }

  return false;
}

int rlc_am_lte::rlc_am_lte_tx::build_status_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  int pdu_len = parent->rx.get_status_pdu(&tx_status, nof_bytes);
  log->debug("%s\n", rlc_am_status_pdu_to_string(&tx_status).c_str());
  if (pdu_len > 0 && nof_bytes >= static_cast<uint32_t>(pdu_len)) {
    log->info("%s Tx status PDU - %s\n", RB_NAME, rlc_am_status_pdu_to_string(&tx_status).c_str());

    parent->rx.reset_status();

    if (cfg.t_status_prohibit > 0 && status_prohibit_timer.is_valid()) {
      // re-arm timer
      status_prohibit_timer.run();
    }
    debug_state();
    pdu_len = rlc_am_write_status_pdu(&tx_status, payload);
  } else {
    log->info("%s Cannot tx status PDU - %d bytes available, %d bytes required\n", RB_NAME, nof_bytes, pdu_len);
    pdu_len = 0;
  }

  return pdu_len;
}

int rlc_am_lte::rlc_am_lte_tx::build_retx_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  // Check there is at least 1 element before calling front()
  if (retx_queue.empty()) {
    log->error("In build_retx_pdu(): retx_queue is empty\n");
    return -1;
  }

  rlc_amd_retx_t retx = retx_queue.front();

  // Sanity check - drop any retx SNs not present in tx_window
  while (tx_window.end() == tx_window.find(retx.sn)) {
    retx_queue.pop_front();
    if (!retx_queue.empty()) {
      retx = retx_queue.front();
    } else {
      log->info("In build_retx_pdu(): retx_queue is empty during sanity check, sn=%d\n", retx.sn);
      return 0;
    }
  }

  // Is resegmentation needed?
  int req_size = required_buffer_size(retx);
  if (req_size < 0) {
    log->error("In build_retx_pdu(): Removing retx.sn=%d from queue\n", retx.sn);
    retx_queue.pop_front();
    return -1;
  }

  if (retx.is_segment || req_size > static_cast<int>(nof_bytes)) {
    log->debug("%s build_retx_pdu - resegmentation required\n", RB_NAME);
    return build_segment(payload, nof_bytes, retx);
  }

  // Update & write header
  rlc_amd_pdu_header_t new_header = tx_window[retx.sn].header;
  new_header.p                    = 0;

  // Set poll bit
  pdu_without_poll++;
  byte_without_poll += (tx_window[retx.sn].buf->N_bytes + rlc_am_packed_length(&new_header));
  log->info("%s pdu_without_poll: %d\n", RB_NAME, pdu_without_poll);
  log->info("%s byte_without_poll: %d\n", RB_NAME, byte_without_poll);
  if (poll_required()) {
    new_header.p      = 1;
    poll_sn           = vt_s;
    pdu_without_poll  = 0;
    byte_without_poll = 0;
    if (poll_retx_timer.is_valid()) {
      // re-arm timer (will be stopped when status PDU is received)
      poll_retx_timer.run();
    }
  }

  uint8_t* ptr = payload;
  rlc_am_write_data_pdu_header(&new_header, &ptr);
  memcpy(ptr, tx_window[retx.sn].buf->msg, tx_window[retx.sn].buf->N_bytes);

  retx_queue.pop_front();
  tx_window[retx.sn].retx_count++;
  if (tx_window[retx.sn].retx_count >= cfg.max_retx_thresh) {
    log->warning(
        "%s Signaling max number of reTx=%d for for PDU %d\n", RB_NAME, tx_window[retx.sn].retx_count, retx.sn);
    parent->rrc->max_retx_attempted();
  }

  log->info("%s Retx PDU scheduled for tx. SN: %d, retx count: %d\n", RB_NAME, retx.sn, tx_window[retx.sn].retx_count);

  debug_state();
  return (ptr - payload) + tx_window[retx.sn].buf->N_bytes;
}

int rlc_am_lte::rlc_am_lte_tx::build_segment(uint8_t* payload, uint32_t nof_bytes, rlc_amd_retx_t retx)
{
  if (tx_window[retx.sn].buf == NULL) {
    log->error("In build_segment: retx.sn=%d has null buffer\n", retx.sn);
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
  log->info("%s pdu_without_poll: %d\n", RB_NAME, pdu_without_poll);
  log->info("%s byte_without_poll: %d\n", RB_NAME, byte_without_poll);

  new_header.dc   = RLC_DC_FIELD_DATA_PDU;
  new_header.rf   = 1;
  new_header.fi   = RLC_FI_FIELD_NOT_START_OR_END_ALIGNED;
  new_header.sn   = old_header.sn;
  new_header.lsf  = 0;
  new_header.so   = retx.so_start;
  new_header.N_li = 0;
  new_header.p    = 0;
  if (poll_required()) {
    log->debug("%s setting poll bit to request status\n", RB_NAME);
    new_header.p      = 1;
    poll_sn           = vt_s;
    pdu_without_poll  = 0;
    byte_without_poll = 0;
    if (poll_retx_timer.is_valid()) {
      poll_retx_timer.run();
    }
  }

  uint32_t head_len  = 0;
  uint32_t pdu_space = 0;

  head_len = rlc_am_packed_length(&new_header);
  if (old_header.N_li > 0) {
    // Make sure we can fit at least one N_li element if old header contained at least one
    head_len += 2;
  }

  if (nof_bytes <= head_len) {
    log->info("%s Cannot build a PDU segment - %d bytes available, %d bytes required for header\n",
              RB_NAME,
              nof_bytes,
              head_len);
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

    if (pdu_space <= 2) {
      break;
    }

    upper += old_header.li[i];

    head_len = rlc_am_packed_length(&new_header);

    // Accomodate some extra space for for LIs if old header contained segments too
    head_len += old_header.N_li;

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

      // only increment N_li if more SDU (segments) are being added
      if (retx.so_end > upper) {
        new_header.N_li++;
      }
    }

    lower += old_header.li[i];
  }

  // Update retx_queue
  if (tx_window[retx.sn].buf->N_bytes == retx.so_end) {
    retx_queue.pop_front();
    new_header.lsf = 1;
    if (rlc_am_end_aligned(old_header.fi)) {
      new_header.fi &= RLC_FI_FIELD_NOT_START_ALIGNED; // segment is end aligned
    }
  } else if (retx_queue.front().so_end == retx.so_end) {
    retx_queue.pop_front();
  } else {
    retx_queue.front().is_segment = true;
    retx_queue.front().so_start   = retx.so_end;
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
    log->error("%s Retx PDU segment length error. Available: %d, Used: %d\n", RB_NAME, nof_bytes, pdu_len);
    int header_len = (ptr - payload);
    log->debug("%s Retx PDU segment length error. Header len: %d, Payload len: %d, N_li: %d\n",
               RB_NAME,
               header_len,
               len,
               new_header.N_li);
  }

  log->info_hex(payload,
                pdu_len,
                "%s Retx PDU segment of SN=%d (%d B), SO: %d, N_li: %d\n",
                RB_NAME,
                retx.sn,
                pdu_len,
                retx.so_start,
                new_header.N_li);

  return pdu_len;
}

int rlc_am_lte::rlc_am_lte_tx::build_data_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  if (tx_sdu == NULL && tx_sdu_queue.size() == 0) {
    log->info("No data available to be sent\n");
    return 0;
  }

  // do not build any more PDU if window is already full
  if (tx_sdu == NULL && tx_window.size() >= RLC_AM_WINDOW_SIZE) {
    log->info("Tx window full.\n");
    return 0;
  }

  unique_byte_buffer_t pdu = srslte::allocate_unique_buffer(*pool, true);
  if (pdu == NULL) {
#ifdef RLC_AM_BUFFER_DEBUG
    log->console("Fatal Error: Could not allocate PDU in build_data_pdu()\n");
    log->console("tx_window size: %zd PDUs\n", tx_window.size());
    log->console("vt_a = %d, vt_ms = %d, vt_s = %d, poll_sn = %d\n", vt_a, vt_ms, vt_s, poll_sn);
    log->console("retx_queue size: %zd PDUs\n", retx_queue.size());
    std::map<uint32_t, rlc_amd_tx_pdu_t>::iterator txit;
    for (txit = tx_window.begin(); txit != tx_window.end(); txit++) {
      log->console("tx_window - SN: %d\n", txit->first);
    }
    exit(-1);
#else
    log->error("Fatal Error: Couldn't allocate PDU in build_data_pdu().\n");
    return 0;
#endif
  }
  rlc_amd_pdu_header_t header;
  header.dc   = RLC_DC_FIELD_DATA_PDU;
  header.rf   = 0;
  header.p    = 0;
  header.fi   = RLC_FI_FIELD_START_AND_END_ALIGNED;
  header.sn   = vt_s;
  header.lsf  = 0;
  header.so   = 0;
  header.N_li = 0;

  uint32_t head_len  = rlc_am_packed_length(&header);
  uint32_t to_move   = 0;
  uint32_t last_li   = 0;
  uint32_t pdu_space = SRSLTE_MIN(nof_bytes, pdu->get_tailroom());
  uint8_t* pdu_ptr   = pdu->msg;

  if (pdu_space <= head_len + 1) {
    log->info(
        "%s Cannot build a PDU - %d bytes available, %d bytes required for header\n", RB_NAME, nof_bytes, head_len);
    return 0;
  }

  log->debug("%s Building PDU - pdu_space: %d, head_len: %d \n", RB_NAME, pdu_space, head_len);

  // Check for SDU segment
  if (tx_sdu != NULL) {
    to_move = ((pdu_space - head_len) >= tx_sdu->N_bytes) ? tx_sdu->N_bytes : pdu_space - head_len;
    memcpy(pdu_ptr, tx_sdu->msg, to_move);
    last_li = to_move;
    pdu_ptr += to_move;
    pdu->N_bytes += to_move;
    tx_sdu->N_bytes -= to_move;
    tx_sdu->msg += to_move;
    if (tx_sdu->N_bytes == 0) {
      log->debug("%s Complete SDU scheduled for tx. Stack latency: %ld us\n", RB_NAME, tx_sdu->get_latency_us());
      tx_sdu.reset();
    }
    if (pdu_space > to_move) {
      pdu_space -= SRSLTE_MIN(to_move, pdu->get_tailroom());
    } else {
      pdu_space = 0;
    }
    header.fi |= RLC_FI_FIELD_NOT_START_ALIGNED; // First byte does not correspond to first byte of SDU

    log->debug("%s Building PDU - added SDU segment (len:%d) - pdu_space: %d, head_len: %d \n",
               RB_NAME,
               to_move,
               pdu_space,
               head_len);
  }

  // Pull SDUs from queue
  while (pdu_space > head_len + 1 && tx_sdu_queue.size() > 0 && header.N_li < RLC_AM_WINDOW_SIZE) {
    if (last_li > 0) {
      header.li[header.N_li] = last_li;
      header.N_li++;
    }
    head_len = rlc_am_packed_length(&header);
    if (head_len >= pdu_space) {
      header.N_li--;
      break;
    }
    tx_sdu  = tx_sdu_queue.read();
    to_move = ((pdu_space - head_len) >= tx_sdu->N_bytes) ? tx_sdu->N_bytes : pdu_space - head_len;
    memcpy(pdu_ptr, tx_sdu->msg, to_move);
    last_li = to_move;
    pdu_ptr += to_move;
    pdu->N_bytes += to_move;
    tx_sdu->N_bytes -= to_move;
    tx_sdu->msg += to_move;
    if (tx_sdu->N_bytes == 0) {
      log->debug("%s Complete SDU scheduled for tx. Stack latency: %ld us\n", RB_NAME, tx_sdu->get_latency_us());
      tx_sdu.reset();
    }
    if (pdu_space > to_move) {
      pdu_space -= to_move;
    } else {
      pdu_space = 0;
    }

    log->debug("%s Building PDU - added SDU segment (len:%d) - pdu_space: %d, head_len: %d \n",
               RB_NAME,
               to_move,
               pdu_space,
               head_len);
  }

  // Make sure, at least one SDU (segment) has been added until this point
  if (pdu->N_bytes == 0) {
    log->error("Generated empty RLC PDU.\n");
    return 0;
  }

  if (tx_sdu != NULL) {
    header.fi |= RLC_FI_FIELD_NOT_END_ALIGNED; // Last byte does not correspond to last byte of SDU
  }

  // Set Poll bit
  pdu_without_poll++;
  byte_without_poll += (pdu->N_bytes + head_len);
  log->debug("%s pdu_without_poll: %d\n", RB_NAME, pdu_without_poll);
  log->debug("%s byte_without_poll: %d\n", RB_NAME, byte_without_poll);
  if (poll_required()) {
    log->debug("%s setting poll bit to request status\n", RB_NAME);
    header.p          = 1;
    poll_sn           = vt_s;
    pdu_without_poll  = 0;
    byte_without_poll = 0;
    if (poll_retx_timer.is_valid()) {
      poll_retx_timer.run();
    }
  }

  // Set SN
  header.sn = vt_s;
  vt_s      = (vt_s + 1) % MOD;

  // Place PDU in tx_window, write header and TX
  tx_window[header.sn].buf        = std::move(pdu);
  tx_window[header.sn].header     = header;
  tx_window[header.sn].is_acked   = false;
  tx_window[header.sn].retx_count = 0;
  const byte_buffer_t* buffer_ptr = tx_window[header.sn].buf.get();

  uint8_t* ptr = payload;
  rlc_am_write_data_pdu_header(&header, &ptr);
  memcpy(ptr, buffer_ptr->msg, buffer_ptr->N_bytes);
  int total_len = (ptr - payload) + buffer_ptr->N_bytes;
  log->info_hex(payload, total_len, "%s Tx PDU SN=%d (%d B)\n", RB_NAME, header.sn, total_len);
  log->debug("%s\n", rlc_amd_pdu_header_to_string(header).c_str());
  debug_state();
  return total_len;
}

void rlc_am_lte::rlc_am_lte_tx::handle_control_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  pthread_mutex_lock(&mutex);

  log->info_hex(payload, nof_bytes, "%s Rx control PDU", RB_NAME);

  rlc_status_pdu_t status;
  rlc_am_read_status_pdu(payload, nof_bytes, &status);

  log->info("%s Rx Status PDU: %s\n", RB_NAME, rlc_am_status_pdu_to_string(&status).c_str());

  if (poll_retx_timer.is_valid()) {
    poll_retx_timer.stop();
  }

  // flush retx queue to avoid unordered SNs, we expect the Rx to request lost PDUs again
  if (status.N_nack > 0) {
    retx_queue.clear();
  }

  // Handle ACKs and NACKs
  std::map<uint32_t, rlc_amd_tx_pdu_t>::iterator it;
  bool                                           update_vt_a = true;
  uint32_t                                       i           = vt_a;

  while (TX_MOD_BASE(i) < TX_MOD_BASE(status.ack_sn) && TX_MOD_BASE(i) < TX_MOD_BASE(vt_s)) {
    bool nack = false;
    for (uint32_t j = 0; j < status.N_nack; j++) {
      if (status.nacks[j].nack_sn == i) {
        nack        = true;
        update_vt_a = false;
        it          = tx_window.find(i);
        if (tx_window.end() != it) {
          if (!retx_queue_has_sn(i)) {
            rlc_amd_retx_t retx = {};
            retx.sn             = i;
            retx.is_segment     = false;
            retx.so_start       = 0;
            retx.so_end         = it->second.buf->N_bytes;

            if (status.nacks[j].has_so) {
              // sanity check
              if (status.nacks[j].so_start >= it->second.buf->N_bytes) {
                // print error but try to send original PDU again
                log->info("SO_start is larger than original PDU (%d >= %d)\n",
                          status.nacks[j].so_start,
                          it->second.buf->N_bytes);
                status.nacks[j].so_start = 0;
              }

              // check for special SO_end value
              if (status.nacks[j].so_end == 0x7FFF) {
                status.nacks[j].so_end = it->second.buf->N_bytes;
              } else {
                retx.so_end = status.nacks[j].so_end + 1;
              }

              if (status.nacks[j].so_start < it->second.buf->N_bytes &&
                  status.nacks[j].so_end <= it->second.buf->N_bytes) {
                retx.is_segment = true;
                retx.so_start   = status.nacks[j].so_start;
              } else {
                log->warning("%s invalid segment NACK received for SN %d. so_start: %d, so_end: %d, N_bytes: %d\n",
                             RB_NAME,
                             i,
                             status.nacks[j].so_start,
                             status.nacks[j].so_end,
                             it->second.buf->N_bytes);
              }
            }
            retx_queue.push_back(retx);
          }
        }
      }
    }

    if (!nack) {
      // ACKed SNs get marked and removed from tx_window if possible
      if (tx_window.count(i) > 0) {
        it = tx_window.find(i);
        if (it != tx_window.end()) {
          if (update_vt_a) {
            tx_window.erase(it);
            vt_a  = (vt_a + 1) % MOD;
            vt_ms = (vt_ms + 1) % MOD;
          }
        }
      }
    }
    i = (i + 1) % MOD;
  }

  debug_state();

  pthread_mutex_unlock(&mutex);
}

void rlc_am_lte::rlc_am_lte_tx::debug_state()
{
  log->debug("%s vt_a = %d, vt_ms = %d, vt_s = %d, poll_sn = %d\n", RB_NAME, vt_a, vt_ms, vt_s, poll_sn);
}

int rlc_am_lte::rlc_am_lte_tx::required_buffer_size(rlc_amd_retx_t retx)
{
  if (!retx.is_segment) {
    if (tx_window.count(retx.sn) == 1) {
      if (tx_window[retx.sn].buf) {
        return rlc_am_packed_length(&tx_window[retx.sn].header) + tx_window[retx.sn].buf->N_bytes;
      } else {
        log->warning("retx.sn=%d has null ptr in required_buffer_size()\n", retx.sn);
        return -1;
      }
    } else {
      log->warning("retx.sn=%d does not exist in required_buffer_size()\n", retx.sn);
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

bool rlc_am_lte::rlc_am_lte_tx::retx_queue_has_sn(uint32_t sn)
{
  std::deque<rlc_amd_retx_t>::iterator q_it;
  for (q_it = retx_queue.begin(); q_it != retx_queue.end(); ++q_it) {
    if (q_it->sn == sn) {
      return true;
    }
  }
  return false;
}

/****************************************************************************
 * Rx subclass implementation
 ***************************************************************************/

rlc_am_lte::rlc_am_lte_rx::rlc_am_lte_rx(rlc_am_lte* parent_) :
  parent(parent_),
  pool(byte_buffer_pool::get_instance()),
  log(parent_->log),
  reordering_timer(parent_->timers->get_unique_timer())
{
  pthread_mutex_init(&mutex, NULL);
}

rlc_am_lte::rlc_am_lte_rx::~rlc_am_lte_rx()
{
  pthread_mutex_destroy(&mutex);
}

bool rlc_am_lte::rlc_am_lte_rx::configure(rlc_am_config_t cfg_)
{
  // TODO: add config checks
  cfg = cfg_;

  // check timers
  if (not reordering_timer.is_valid()) {
    log->error("Configuring RLC AM TX: timers not configured\n");
    return false;
  }

  // configure timer
  if (cfg.t_reordering > 0) {
    reordering_timer.set(static_cast<uint32_t>(cfg.t_reordering), [this](uint32_t tid) { timer_expired(tid); });
  }

  return true;
}

void rlc_am_lte::rlc_am_lte_rx::reestablish()
{
  stop();
}

void rlc_am_lte::rlc_am_lte_rx::stop()
{
  pthread_mutex_lock(&mutex);

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

  pthread_mutex_unlock(&mutex);
}

/** Called from stack thread when MAC has received a new RLC PDU
 *
 * @param payload Pointer to payload
 * @param nof_bytes Payload length
 * @param header Reference to PDU header (unpacked by caller)
 */
void rlc_am_lte::rlc_am_lte_rx::handle_data_pdu(uint8_t* payload, uint32_t nof_bytes, rlc_amd_pdu_header_t& header)
{
  std::map<uint32_t, rlc_amd_rx_pdu_t>::iterator it;

  log->info_hex(payload, nof_bytes, "%s Rx data PDU SN=%d (%d B)", RB_NAME, header.sn, nof_bytes);
  log->debug("%s\n", rlc_amd_pdu_header_to_string(header).c_str());

  // sanity check for segments not exceeding PDU length
  if (header.N_li > 0) {
    uint32_t segments_len = 0;
    for (uint32_t i = 0; i < header.N_li; i++) {
      segments_len += header.li[i];
      if (segments_len > nof_bytes) {
        log->info("Dropping corrupted PDU (segments_len=%d > pdu_len=%d)\n", segments_len, nof_bytes);
        return;
      }
    }
  }

  if (!inside_rx_window(header.sn)) {
    if (header.p) {
      log->info("%s Status packet requested through polling bit\n", RB_NAME);
      do_status = true;
    }
    log->info("%s SN: %d outside rx window [%d:%d] - discarding\n", RB_NAME, header.sn, vr_r, vr_mr);
    return;
  }

  it = rx_window.find(header.sn);
  if (rx_window.end() != it) {
    if (header.p) {
      log->info("%s Status packet requested through polling bit\n", RB_NAME);
      do_status = true;
    }
    log->info("%s Discarding duplicate SN: %d\n", RB_NAME, header.sn);
    return;
  }

  // Write to rx window
  rlc_amd_rx_pdu_t pdu;
  pdu.buf = srslte::allocate_unique_buffer(*pool, true);
  if (pdu.buf == NULL) {
#ifdef RLC_AM_BUFFER_DEBUG
    log->console("Fatal Error: Couldn't allocate PDU in handle_data_pdu().\n");
    exit(-1);
#else
    log->error("Fatal Error: Couldn't allocate PDU in handle_data_pdu().\n");
    return;
#endif
  }

  // check available space for payload
  if (nof_bytes > pdu.buf->get_tailroom()) {
    log->error("%s Discarding SN: %d of size %d B (available space %d B)\n",
               RB_NAME,
               header.sn,
               nof_bytes,
               pdu.buf->get_tailroom());
    return;
  }
  memcpy(pdu.buf->msg, payload, nof_bytes);
  pdu.buf->N_bytes = nof_bytes;
  pdu.header       = header;

  rx_window[header.sn] = std::move(pdu);

  // Update vr_h
  if (RX_MOD_BASE(header.sn) >= RX_MOD_BASE(vr_h)) {
    vr_h = (header.sn + 1) % MOD;
  }

  // Update vr_ms
  it = rx_window.find(vr_ms);
  while (rx_window.end() != it) {
    vr_ms = (vr_ms + 1) % MOD;
    it    = rx_window.find(vr_ms);
  }

  // Check poll bit
  if (header.p) {
    log->info("%s Status packet requested through polling bit\n", RB_NAME);
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
        log->debug("Stopping reordering timer.\n");
        reordering_timer.stop();
      } else {
        log->debug("Leave reordering timer running.\n");
      }
      debug_state();
    }

    if (not reordering_timer.is_running()) {
      if (RX_MOD_BASE(vr_h) > RX_MOD_BASE(vr_r)) {
        log->debug("Starting reordering timer.\n");
        reordering_timer.run();
        vr_x = vr_h;
      } else {
        log->debug("Leave reordering timer stopped.\n");
      }
      debug_state();
    }
  }

  debug_state();
}

void rlc_am_lte::rlc_am_lte_rx::handle_data_pdu_segment(uint8_t*              payload,
                                                        uint32_t              nof_bytes,
                                                        rlc_amd_pdu_header_t& header)
{
  std::map<uint32_t, rlc_amd_rx_pdu_segments_t>::iterator it;

  log->info_hex(payload,
                nof_bytes,
                "%s Rx data PDU segment of SN=%d (%d B), SO=%d, N_li=%d",
                RB_NAME,
                header.sn,
                nof_bytes,
                header.so,
                header.N_li);
  log->debug("%s\n", rlc_amd_pdu_header_to_string(header).c_str());

  // Check inside rx window
  if (!inside_rx_window(header.sn)) {
    if (header.p) {
      log->info("%s Status packet requested through polling bit\n", RB_NAME);
      do_status = true;
    }
    log->info("%s SN: %d outside rx window [%d:%d] - discarding\n", RB_NAME, header.sn, vr_r, vr_mr);
    return;
  }

  rlc_amd_rx_pdu_t segment;
  segment.buf = srslte::allocate_unique_buffer(*pool, true);
  if (segment.buf == NULL) {
#ifdef RLC_AM_BUFFER_DEBUG
    log->console("Fatal Error: Couldn't allocate PDU in handle_data_pdu_segment().\n");
    exit(-1);
#else
    log->error("Fatal Error: Couldn't allocate PDU in handle_data_pdu_segment().\n");
    return;
#endif
  }

  if (segment.buf->get_tailroom() < nof_bytes) {
    log->info("Dropping corrupted segment SN=%d, not enough space to fit %d B\n", header.sn, nof_bytes);
    return;
  }

  memcpy(segment.buf->msg, payload, nof_bytes);
  segment.buf->N_bytes = nof_bytes;
  segment.header       = header;

  // Check if we already have a segment from the same PDU
  it = rx_segments.find(header.sn);
  if (rx_segments.end() != it) {

    if (header.p) {
      log->info("%s Status packet requested through polling bit\n", RB_NAME);
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
      log->info("%s Status packet requested through polling bit\n", RB_NAME);
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

void rlc_am_lte::rlc_am_lte_rx::reassemble_rx_sdus()
{
  uint32_t len = 0;
  if (rx_sdu == NULL) {
    rx_sdu = allocate_unique_buffer(*pool, true);
    if (rx_sdu == NULL) {
#ifdef RLC_AM_BUFFER_DEBUG
      log->console("Fatal Error: Could not allocate PDU in reassemble_rx_sdus() (1)\n");
      exit(-1);
#else
      log->error("Fatal Error: Could not allocate PDU in reassemble_rx_sdus() (1)\n");
      return;
#endif
    }
  }

  // Iterate through rx_window, assembling and delivering SDUs
  while (rx_window.end() != rx_window.find(vr_r)) {
    // Handle any SDU segments
    for (uint32_t i = 0; i < rx_window[vr_r].header.N_li; i++) {
      len = rx_window[vr_r].header.li[i];

      log->debug_hex(rx_window[vr_r].buf->msg,
                     len,
                     "Handling segment %d/%d of length %d B of SN=%d\n",
                     i + 1,
                     rx_window[vr_r].header.N_li,
                     len,
                     vr_r);

      // sanity check to avoid zero-size SDUs
      if (len == 0) {
        break;
      }

      if (rx_sdu->get_tailroom() >= len) {
        if ((rx_window[vr_r].buf->msg - rx_window[vr_r].buf->buffer) + len < SRSLTE_MAX_BUFFER_SIZE_BYTES) {
          if (rx_window[vr_r].buf->N_bytes < len) {
            log->error("Dropping corrupted SN=%d\n", vr_r);
            rx_sdu.reset();
            goto exit;
          }

          memcpy(&rx_sdu->msg[rx_sdu->N_bytes], rx_window[vr_r].buf->msg, len);
          rx_sdu->N_bytes += len;

          rx_window[vr_r].buf->msg += len;
          rx_window[vr_r].buf->N_bytes -= len;

          log->info_hex(rx_sdu->msg, rx_sdu->N_bytes, "%s Rx SDU (%d B)", RB_NAME, rx_sdu->N_bytes);
          rx_sdu->set_timestamp();
          parent->pdcp->write_pdu(parent->lcid, std::move(rx_sdu));

          rx_sdu = allocate_unique_buffer(*pool, true);
          if (rx_sdu == nullptr) {
#ifdef RLC_AM_BUFFER_DEBUG
            log->console("Fatal Error: Could not allocate PDU in reassemble_rx_sdus() (2)\n");
            exit(-1);
#else
            log->error("Fatal Error: Could not allocate PDU in reassemble_rx_sdus() (2)\n");
            return;
#endif
          }
        } else {
          int buf_len = rx_window[vr_r].buf->msg - rx_window[vr_r].buf->buffer;
          log->error("Cannot read %d bytes from rx_window. vr_r=%d, msg-buffer=%d B\n", len, vr_r, buf_len);
          rx_sdu.reset();
          goto exit;
        }
      } else {
        log->error("Cannot fit RLC PDU in SDU buffer, dropping both.\n");
        rx_sdu.reset();
        goto exit;
      }
    }

    // Handle last segment
    len = rx_window[vr_r].buf->N_bytes;
    log->debug_hex(rx_window[vr_r].buf->msg, len, "Handling last segment of length %d B of SN=%d\n", len, vr_r);
    if (rx_sdu->get_tailroom() >= len) {
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
      log->info_hex(rx_sdu->msg, rx_sdu->N_bytes, "%s Rx SDU (%d B)", RB_NAME, rx_sdu->N_bytes);
      rx_sdu->set_timestamp();
      parent->pdcp->write_pdu(parent->lcid, std::move(rx_sdu));
      rx_sdu = allocate_unique_buffer(*pool, true);
      if (rx_sdu == NULL) {
#ifdef RLC_AM_BUFFER_DEBUG
        log->console("Fatal Error: Could not allocate PDU in reassemble_rx_sdus() (3)\n");
        exit(-1);
#else
        log->error("Fatal Error: Could not allocate PDU in reassemble_rx_sdus() (3)\n");
        return;
#endif
      }
    }

  exit:
    // Move the rx_window
    log->debug("Erasing SN=%d.\n", vr_r);
    // also erase any segments of this SN
    std::map<uint32_t, rlc_amd_rx_pdu_segments_t>::iterator it;
    it = rx_segments.find(vr_r);
    if (rx_segments.end() != it) {
      log->debug("Erasing segments of SN=%d\n", vr_r);
      std::list<rlc_amd_rx_pdu_t>::iterator segit;
      for (segit = it->second.segments.begin(); segit != it->second.segments.end(); ++segit) {
        log->debug(" Erasing segment of SN=%d SO=%d Len=%d N_li=%d\n",
                   segit->header.sn,
                   segit->header.so,
                   segit->buf->N_bytes,
                   segit->header.N_li);
      }
      it->second.segments.clear();
    }
    rx_window.erase(vr_r);
    vr_r  = (vr_r + 1) % MOD;
    vr_mr = (vr_mr + 1) % MOD;
  }
}

void rlc_am_lte::rlc_am_lte_rx::reset_status()
{
  pthread_mutex_lock(&mutex);
  do_status     = false;
  poll_received = false;
  pthread_mutex_unlock(&mutex);
}

bool rlc_am_lte::rlc_am_lte_rx::get_do_status()
{
  return do_status;
}

uint32_t rlc_am_lte::rlc_am_lte_rx::get_num_rx_bytes()
{
  return num_rx_bytes;
}

void rlc_am_lte::rlc_am_lte_rx::reset_metrics()
{
  pthread_mutex_lock(&mutex);
  num_rx_bytes = 0;
  pthread_mutex_unlock(&mutex);
}

void rlc_am_lte::rlc_am_lte_rx::write_pdu(uint8_t* payload, const uint32_t nof_bytes)
{
  if (nof_bytes < 1)
    return;

  pthread_mutex_lock(&mutex);
  num_rx_bytes += nof_bytes;
  pthread_mutex_unlock(&mutex);

  if (rlc_am_is_control_pdu(payload)) {
    // unlock mutex and pass to Tx subclass
    parent->tx.handle_control_pdu(payload, nof_bytes);
  } else {
    rlc_amd_pdu_header_t header      = {};
    uint32_t             payload_len = nof_bytes;
    rlc_am_read_data_pdu_header(&payload, &payload_len, &header);
    if (payload_len > nof_bytes) {
      log->info("Dropping corrupted PDU (%d B). Remaining length after header %d B.\n", nof_bytes, payload_len);
      return;
    }
    if (header.rf) {
      handle_data_pdu_segment(payload, payload_len, header);
    } else {
      handle_data_pdu(payload, payload_len, header);
    }
  }
}

/**
 * Function called from stack thread when timer has expired
 *
 * @param timeout_id
 */
void rlc_am_lte::rlc_am_lte_rx::timer_expired(uint32_t timeout_id)
{
  pthread_mutex_lock(&mutex);
  if (reordering_timer.is_valid() and reordering_timer.id() == timeout_id) {
    log->debug("%s reordering timeout expiry - updating vr_ms (was %d)\n", RB_NAME, vr_ms);

    // 36.322 v10 Section 5.1.3.2.4
    vr_ms                                             = vr_x;
    std::map<uint32_t, rlc_amd_rx_pdu_t>::iterator it = rx_window.find(vr_ms);
    while (rx_window.end() != it) {
      vr_ms = (vr_ms + 1) % MOD;
      it    = rx_window.find(vr_ms);
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
  pthread_mutex_unlock(&mutex);
}

// Called from Tx object to pack status PDU that doesn't exceed a given size
int rlc_am_lte::rlc_am_lte_rx::get_status_pdu(rlc_status_pdu_t* status, const uint32_t max_pdu_size)
{
  pthread_mutex_lock(&mutex);
  status->N_nack = 0;
  status->ack_sn = vr_r; // start with lower edge of the rx window

  // We don't use segment NACKs - just NACK the full PDU
  uint32_t i = vr_r;
  while (RX_MOD_BASE(i) < RX_MOD_BASE(vr_ms) && status->N_nack < RLC_AM_WINDOW_SIZE) {
    if (rx_window.find(i) == rx_window.end()) {
      status->nacks[status->N_nack].nack_sn = i;
      status->N_nack++;
    } else {
      // only update ACK_SN if this SN has been received
      status->ack_sn = i;
    }

    // make sure we don't exceed grant size
    if (rlc_am_packed_length(status) > max_pdu_size) {
      log->debug("Status PDU too big (%d > %d)\n", rlc_am_packed_length(status), max_pdu_size);
      if (status->N_nack >= 1 && status->N_nack < RLC_AM_WINDOW_SIZE) {
        log->debug("Removing last NACK SN=%d\n", status->nacks[status->N_nack].nack_sn);
        status->N_nack--;
        // make sure we don't have the current ACK_SN in the NACK list
        if (rlc_am_is_valid_status_pdu(*status) == false) {
          // No space to send any NACKs
          log->debug("Resetting N_nack to zero\n");
          status->N_nack = 0;
        }
      } else {
        log->error("Failed to generate small enough status PDU\n");
      }
      break;
    }
    i = (i + 1) % MOD;
  }

  pthread_mutex_unlock(&mutex);
  return rlc_am_packed_length(status);
}

// Called from Tx object to obtain length of the full status PDU
int rlc_am_lte::rlc_am_lte_rx::get_status_pdu_length()
{
  pthread_mutex_lock(&mutex);
  rlc_status_pdu_t status = {};
  status.ack_sn           = vr_ms;
  uint32_t i              = vr_r;
  while (RX_MOD_BASE(i) < RX_MOD_BASE(vr_ms) && status.N_nack < RLC_AM_WINDOW_SIZE) {
    if (rx_window.find(i) == rx_window.end()) {
      status.N_nack++;
    }
    i = (i + 1) % MOD;
  }
  pthread_mutex_unlock(&mutex);
  return rlc_am_packed_length(&status);
}

void rlc_am_lte::rlc_am_lte_rx::print_rx_segments()
{
  std::map<uint32_t, rlc_amd_rx_pdu_segments_t>::iterator it;
  std::stringstream                                       ss;
  ss << "rx_segments:" << std::endl;
  for (it = rx_segments.begin(); it != rx_segments.end(); it++) {
    std::list<rlc_amd_rx_pdu_t>::iterator segit;
    for (segit = it->second.segments.begin(); segit != it->second.segments.end(); segit++) {
      ss << "    SN:" << segit->header.sn << " SO:" << segit->header.so << " N:" << segit->buf->N_bytes
         << " N_li: " << segit->header.N_li << std::endl;
    }
  }
  log->debug("%s\n", ss.str().c_str());
}

// NOTE: Preference would be to capture by value, and then move; but header is stack allocated
bool rlc_am_lte::rlc_am_lte_rx::add_segment_and_check(rlc_amd_rx_pdu_segments_t* pdu, rlc_amd_rx_pdu_t* segment)
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
    rlc_amd_rx_pdu_t& s = *it1;
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
  uint32_t                              so = 0;
  std::list<rlc_amd_rx_pdu_t>::iterator it, tmpit;
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
      so = SRSLTE_MAX(so, it->header.so + it->buf->N_bytes);
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

  log->debug("Starting header reconstruction of %zd segments\n", pdu->segments.size());

  // Reconstruct li fields
  uint16_t count     = 0;
  uint16_t carryover = 0;
  for (it = pdu->segments.begin(); it != pdu->segments.end(); it++) {
    log->debug(" Handling %d PDU segments\n", it->header.N_li);
    for (uint32_t i = 0; i < it->header.N_li; i++) {
      header.li[header.N_li] = it->header.li[i];
      if (i == 0) {
        header.li[header.N_li] += carryover;
      }
      log->debug("  - adding segment %d/%d (%d B, SO=%d, carryover=%d, count=%d)\n",
                 i + 1,
                 it->header.N_li,
                 header.li[header.N_li],
                 header.so,
                 carryover,
                 count);
      header.N_li++;
      count += it->header.li[i];
      carryover = 0;
    }

    if (count <= it->buf->N_bytes) {
      carryover += it->buf->N_bytes - count;
      log->debug("Incremented carryover (it->buf->N_bytes=%d, count=%d). New carryover=%d\n",
                 it->buf->N_bytes,
                 count,
                 carryover);
    } else {
      // Next segment would be too long, recalculate carryover
      header.N_li--;
      carryover = it->buf->N_bytes - (count - header.li[header.N_li]);
      log->debug("Recalculated carryover=%d (it->buf->N_bytes=%d, count=%d, header.li[header.N_li]=%d)\n",
                 carryover,
                 it->buf->N_bytes,
                 count,
                 header.li[header.N_li]);
    }

    tmpit = it;
    if (rlc_am_end_aligned(it->header.fi) && ++tmpit != pdu->segments.end()) {
      log->debug("Header is end-aligned, overwrite header.li[%d]=%d\n", header.N_li, carryover);
      header.li[header.N_li] = carryover;
      header.N_li++;
      carryover = 0;
    }
    count = 0;
  }

  log->debug("Finished header reconstruction of %zd segments\n", pdu->segments.size());

  // Copy data
  unique_byte_buffer_t full_pdu = srslte::allocate_unique_buffer(*pool, true);
  if (full_pdu == NULL) {
#ifdef RLC_AM_BUFFER_DEBUG
    log->console("Fatal Error: Could not allocate PDU in add_segment_and_check()\n");
    exit(-1);
#else
    log->error("Fatal Error: Could not allocate PDU in add_segment_and_check()\n");
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

  handle_data_pdu(full_pdu->msg, full_pdu->N_bytes, header);
  return true;
}

bool rlc_am_lte::rlc_am_lte_rx::inside_rx_window(const int16_t sn)
{
  if (RX_MOD_BASE(sn) >= RX_MOD_BASE(static_cast<int16_t>(vr_r)) && RX_MOD_BASE(sn) < RX_MOD_BASE(vr_mr)) {
    return true;
  } else {
    return false;
  }
}

void rlc_am_lte::rlc_am_lte_rx::debug_state()
{
  log->debug("%s vr_r = %d, vr_mr = %d, vr_x = %d, vr_ms = %d, vr_h = %d\n", RB_NAME, vr_r, vr_mr, vr_x, vr_ms, vr_h);
}

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 36.322 v10.0.0 Section 6.2.1
 ***************************************************************************/

// Read header from pdu struct, don't strip header
void rlc_am_read_data_pdu_header(byte_buffer_t* pdu, rlc_amd_pdu_header_t* header)
{
  uint8_t* ptr = pdu->msg;
  uint32_t n   = 0;
  rlc_am_read_data_pdu_header(&ptr, &n, header);
}

// Read header from raw pointer, strip header
void rlc_am_read_data_pdu_header(uint8_t** payload, uint32_t* nof_bytes, rlc_amd_pdu_header_t* header)
{
  uint8_t  ext;
  uint8_t* ptr = *payload;

  header->dc = static_cast<rlc_dc_field_t>((*ptr >> 7) & 0x01);

  if (RLC_DC_FIELD_DATA_PDU == header->dc) {
    // Fixed part
    header->rf = ((*ptr >> 6) & 0x01);
    header->p  = ((*ptr >> 5) & 0x01);
    header->fi = static_cast<rlc_fi_field_t>((*ptr >> 3) & 0x03);
    ext        = ((*ptr >> 2) & 0x01);
    header->sn = (*ptr & 0x03) << 8; // 2 bits SN
    ptr++;
    header->sn |= (*ptr & 0xFF); // 8 bits SN
    ptr++;

    if (header->rf) {
      header->lsf = ((*ptr >> 7) & 0x01);
      header->so  = (*ptr & 0x7F) << 8; // 7 bits of SO
      ptr++;
      header->so |= (*ptr & 0xFF); // 8 bits of SO
      ptr++;
    }

    // Extension part
    header->N_li = 0;
    while (ext) {
      if (header->N_li % 2 == 0) {
        ext                      = ((*ptr >> 7) & 0x01);
        header->li[header->N_li] = (*ptr & 0x7F) << 4; // 7 bits of LI
        ptr++;
        header->li[header->N_li] |= (*ptr & 0xF0) >> 4; // 4 bits of LI
        header->N_li++;
      } else {
        ext                      = (*ptr >> 3) & 0x01;
        header->li[header->N_li] = (*ptr & 0x07) << 8; // 3 bits of LI
        ptr++;
        header->li[header->N_li] |= (*ptr & 0xFF); // 8 bits of LI
        header->N_li++;
        ptr++;
      }
    }

    // Account for padding if N_li is odd
    if (header->N_li % 2 == 1) {
      ptr++;
    }

    *nof_bytes -= ptr - *payload;
    *payload = ptr;
  }
}

// Write header to pdu struct
void rlc_am_write_data_pdu_header(rlc_amd_pdu_header_t* header, byte_buffer_t* pdu)
{
  uint8_t* ptr = pdu->msg;
  rlc_am_write_data_pdu_header(header, &ptr);
  pdu->N_bytes += ptr - pdu->msg;
}

// Write header to pointer & move pointer
void rlc_am_write_data_pdu_header(rlc_amd_pdu_header_t* header, uint8_t** payload)
{
  uint32_t i;
  uint8_t  ext = (header->N_li > 0) ? 1 : 0;

  uint8_t* ptr = *payload;

  // Fixed part
  *ptr = (header->dc & 0x01) << 7;
  *ptr |= (header->rf & 0x01) << 6;
  *ptr |= (header->p & 0x01) << 5;
  *ptr |= (header->fi & 0x03) << 3;
  *ptr |= (ext & 0x01) << 2;

  *ptr |= (header->sn & 0x300) >> 8; // 2 bits SN
  ptr++;
  *ptr = (header->sn & 0xFF); // 8 bits SN
  ptr++;

  // Segment part
  if (header->rf) {
    *ptr = (header->lsf & 0x01) << 7;
    *ptr |= (header->so & 0x7F00) >> 8; // 7 bits of SO
    ptr++;
    *ptr = (header->so & 0x00FF); // 8 bits of SO
    ptr++;
  }

  // Extension part
  i = 0;
  while (i < header->N_li) {
    ext  = ((i + 1) == header->N_li) ? 0 : 1;
    *ptr = (ext & 0x01) << 7;             // 1 bit header
    *ptr |= (header->li[i] & 0x7F0) >> 4; // 7 bits of LI
    ptr++;
    *ptr = (header->li[i] & 0x00F) << 4; // 4 bits of LI
    i++;
    if (i < header->N_li) {
      ext = ((i + 1) == header->N_li) ? 0 : 1;
      *ptr |= (ext & 0x01) << 3;            // 1 bit header
      *ptr |= (header->li[i] & 0x700) >> 8; // 3 bits of LI
      ptr++;
      *ptr = (header->li[i] & 0x0FF); // 8 bits of LI
      ptr++;
      i++;
    }
  }
  // Pad if N_li is odd
  if (header->N_li % 2 == 1) {
    ptr++;
  }

  *payload = ptr;
}

void rlc_am_read_status_pdu(byte_buffer_t* pdu, rlc_status_pdu_t* status)
{
  rlc_am_read_status_pdu(pdu->msg, pdu->N_bytes, status);
}

void rlc_am_read_status_pdu(uint8_t* payload, uint32_t nof_bytes, rlc_status_pdu_t* status)
{
  uint32_t     i;
  uint8_t      ext1, ext2;
  bit_buffer_t tmp;
  uint8_t*     ptr = tmp.msg;

  srslte_bit_unpack_vector(payload, tmp.msg, nof_bytes * 8);
  tmp.N_bits = nof_bytes * 8;

  rlc_dc_field_t dc = static_cast<rlc_dc_field_t>(srslte_bit_pack(&ptr, 1));

  if (RLC_DC_FIELD_CONTROL_PDU == dc) {
    uint8_t cpt = srslte_bit_pack(&ptr, 3); // 3-bit Control PDU Type (0 == status)
    if (0 == cpt) {
      status->ack_sn = srslte_bit_pack(&ptr, 10); // 10 bits ACK_SN
      ext1           = srslte_bit_pack(&ptr, 1);  // 1 bits E1
      status->N_nack = 0;
      while (ext1) {
        status->nacks[status->N_nack].nack_sn = srslte_bit_pack(&ptr, 10);
        ext1                                  = srslte_bit_pack(&ptr, 1); // 1 bits E1
        ext2                                  = srslte_bit_pack(&ptr, 1); // 1 bits E2
        if (ext2) {
          status->nacks[status->N_nack].has_so   = true;
          status->nacks[status->N_nack].so_start = srslte_bit_pack(&ptr, 15);
          status->nacks[status->N_nack].so_end   = srslte_bit_pack(&ptr, 15);
        }
        status->N_nack++;
      }
    }
  }
}

void rlc_am_write_status_pdu(rlc_status_pdu_t* status, byte_buffer_t* pdu)
{
  pdu->N_bytes = rlc_am_write_status_pdu(status, pdu->msg);
}

int rlc_am_write_status_pdu(rlc_status_pdu_t* status, uint8_t* payload)
{
  uint32_t     i;
  uint8_t      ext1;
  bit_buffer_t tmp;
  uint8_t*     ptr = tmp.msg;

  srslte_bit_unpack(RLC_DC_FIELD_CONTROL_PDU, &ptr, 1); // D/C
  srslte_bit_unpack(0, &ptr, 3);                        // CPT (0 == STATUS)
  srslte_bit_unpack(status->ack_sn, &ptr, 10);          // 10 bit ACK_SN
  ext1 = (status->N_nack == 0) ? 0 : 1;
  srslte_bit_unpack(ext1, &ptr, 1); // E1
  for (i = 0; i < status->N_nack; i++) {
    srslte_bit_unpack(status->nacks[i].nack_sn, &ptr, 10); // 10 bit NACK_SN
    ext1 = ((status->N_nack - 1) == i) ? 0 : 1;
    srslte_bit_unpack(ext1, &ptr, 1); // E1
    if (status->nacks[i].has_so) {
      srslte_bit_unpack(1, &ptr, 1); // E2
      srslte_bit_unpack(status->nacks[i].so_start, &ptr, 15);
      srslte_bit_unpack(status->nacks[i].so_end, &ptr, 15);
    } else {
      srslte_bit_unpack(0, &ptr, 1); // E2
    }
  }

  // Pad
  tmp.N_bits    = ptr - tmp.msg;
  uint8_t n_pad = 8 - (tmp.N_bits % 8);
  srslte_bit_unpack(0, &ptr, n_pad);
  tmp.N_bits = ptr - tmp.msg;

  // Pack bits
  srslte_bit_pack_vector(tmp.msg, payload, tmp.N_bits);
  return tmp.N_bits / 8;
}

bool rlc_am_is_valid_status_pdu(const rlc_status_pdu_t& status)
{
  for (uint32_t i = 0; i < status.N_nack; ++i) {
    if (status.nacks[i].nack_sn == status.ack_sn) {
      return false;
    }
  }
  return true;
}

uint32_t rlc_am_packed_length(rlc_amd_pdu_header_t* header)
{
  uint32_t len = 2; // Fixed part is 2 bytes
  if (header->rf) {
    len += 2; // Segment header is 2 bytes
  }
  len += header->N_li * 1.5 + 0.5; // Extension part - integer rounding up
  return len;
}

uint32_t rlc_am_packed_length(rlc_status_pdu_t* status)
{
  uint32_t len_bits = 15; // Fixed part is 15 bits
  for (uint32_t i = 0; i < status->N_nack; i++) {
    if (status->nacks[i].has_so) {
      len_bits += 42; // 10 bits SN, 2 bits ext, 15 bits so_start, 15 bits so_end
    } else {
      len_bits += 12; // 10 bits SN, 2 bits ext
    }
  }

  return (len_bits + 7) / 8; // Convert to bytes - integer rounding up
}

bool rlc_am_is_pdu_segment(uint8_t* payload)
{
  return ((*(payload) >> 6) & 0x01) == 1;
}

std::string rlc_am_status_pdu_to_string(rlc_status_pdu_t* status)
{
  std::stringstream ss;
  ss << "ACK_SN = " << status->ack_sn;
  ss << ", N_nack = " << status->N_nack;
  if (status->N_nack > 0) {
    ss << ", NACK_SN = ";
    for (uint32_t i = 0; i < status->N_nack; i++) {
      if (status->nacks[i].has_so) {
        ss << "[" << status->nacks[i].nack_sn << " " << status->nacks[i].so_start << ":" << status->nacks[i].so_end
           << "]";
      } else {
        ss << "[" << status->nacks[i].nack_sn << "]";
      }
    }
  }
  return ss.str();
}

std::string rlc_amd_pdu_header_to_string(const rlc_amd_pdu_header_t& header)
{
  std::stringstream ss;
  ss << "[" << rlc_dc_field_text[header.dc];
  ss << ", RF=" << (header.rf ? "1" : "0");
  ss << ", P=" << (header.p ? "1" : "0");
  ss << ", FI=" << (header.fi ? "1" : "0");
  ss << ", SN=" << header.sn;
  ss << ", LSF=" << (header.lsf ? "1" : "0");
  ss << ", SO=" << header.so;
  ss << ", N_li=" << header.N_li;
  if (header.N_li > 0) {
    ss << " (";
    for (uint32_t i = 0; i < header.N_li; i++) {
      ss << header.li[i] << ", ";
    }
    ss << ")";
  }
  ss << "]";
  return ss.str();
}

bool rlc_am_start_aligned(const uint8_t fi)
{
  return (fi == RLC_FI_FIELD_START_AND_END_ALIGNED || fi == RLC_FI_FIELD_NOT_END_ALIGNED);
}

bool rlc_am_end_aligned(const uint8_t fi)
{
  return (fi == RLC_FI_FIELD_START_AND_END_ALIGNED || fi == RLC_FI_FIELD_NOT_START_ALIGNED);
}

bool rlc_am_is_unaligned(const uint8_t fi)
{
  return (fi == RLC_FI_FIELD_NOT_START_OR_END_ALIGNED);
}

bool rlc_am_not_start_aligned(const uint8_t fi)
{
  return (fi == RLC_FI_FIELD_NOT_START_ALIGNED || fi == RLC_FI_FIELD_NOT_START_OR_END_ALIGNED);
}

} // namespace srslte
