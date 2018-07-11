/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include "srslte/upper/rlc_am.h"

#include <iostream>
#include <sstream>

#define MOD 1024
#define RX_MOD_BASE(x) ((x-vr_r)%1024)
#define TX_MOD_BASE(x) ((x-vt_a)%1024)

namespace srslte {

rlc_am::rlc_am(uint32_t queue_len) : tx_sdu_queue(queue_len)
{
  log = NULL;
  pdcp = NULL;
  rrc = NULL;
  lcid = 0;
  bzero(&cfg, sizeof(srslte_rlc_am_config_t));

  tx_sdu = NULL;
  rx_sdu = NULL;
  pool = byte_buffer_pool::get_instance();

  pthread_mutex_init(&mutex, NULL);
  
  vt_a    = 0;
  vt_ms   = RLC_AM_WINDOW_SIZE;
  vt_s    = 0;
  poll_sn = 0;

  vr_r    = 0;
  vr_mr   = RLC_AM_WINDOW_SIZE;
  vr_x    = 0;
  vr_ms   = 0;
  vr_h    = 0;

  pdu_without_poll  = 0;
  byte_without_poll = 0;

  poll_received = false;
  do_status     = false;
}

// Warning: must call stop() to properly deallocate all buffers
rlc_am::~rlc_am()
{
  pthread_mutex_destroy(&mutex);
  pool = NULL;
}

void rlc_am::init(srslte::log                  *log_,
                  uint32_t                      lcid_,
                  srsue::pdcp_interface_rlc    *pdcp_,
                  srsue::rrc_interface_rlc     *rrc_,
                  srslte::mac_interface_timers *mac_timers)
{
  log  = log_;
  lcid = lcid_;
  pdcp = pdcp_;
  rrc  = rrc_;
  tx_enabled = true;
}

void rlc_am::configure(srslte_rlc_config_t cfg_)
{
  cfg = cfg_.am;
  log->warning("%s configured: t_poll_retx=%d, poll_pdu=%d, poll_byte=%d, max_retx_thresh=%d, "
            "t_reordering=%d, t_status_prohibit=%d\n",
            rrc->get_rb_name(lcid).c_str(), cfg.t_poll_retx, cfg.poll_pdu, cfg.poll_byte, cfg.max_retx_thresh,
            cfg.t_reordering, cfg.t_status_prohibit);
}


void rlc_am::empty_queue() {
  // Drop all messages in TX SDU queue
  byte_buffer_t *buf;
  while(tx_sdu_queue.try_read(&buf)) {
    pool->deallocate(buf);
  }
  tx_sdu_queue.reset();
}

void rlc_am::reestablish() {
  stop();
  tx_enabled = true;
}

void rlc_am::stop()
{
  // Empty tx_sdu_queue before locking the mutex
  tx_enabled = false;
  usleep(100);
  empty_queue();

  pthread_mutex_lock(&mutex);
  reordering_timeout.reset();
  if(tx_sdu) {
    pool->deallocate(tx_sdu);
    tx_sdu = NULL;
  }
  if(rx_sdu) {
    pool->deallocate(rx_sdu);
    rx_sdu = NULL;
  }

  vt_a    = 0;
  vt_ms   = RLC_AM_WINDOW_SIZE;
  vt_s    = 0;
  poll_sn = 0;

  vr_r    = 0;
  vr_mr   = RLC_AM_WINDOW_SIZE;
  vr_x    = 0;
  vr_ms   = 0;
  vr_h    = 0;

  pdu_without_poll  = 0;
  byte_without_poll = 0;

  poll_received = false;
  do_status     = false;

  // Drop all messages in RX segments
  std::map<uint32_t, rlc_amd_rx_pdu_segments_t>::iterator rxsegsit;
  std::list<rlc_amd_rx_pdu_t>::iterator                   segit;
  for(rxsegsit = rx_segments.begin(); rxsegsit != rx_segments.end(); rxsegsit++) {
    std::list<rlc_amd_rx_pdu_t> l = rxsegsit->second.segments;
    for(segit = l.begin(); segit != l.end(); segit++) {
      pool->deallocate(segit->buf);
    }
    l.clear();
  }
  rx_segments.clear();
  
  // Drop all messages in RX window
  std::map<uint32_t, rlc_amd_rx_pdu_t>::iterator rxit;
  for(rxit = rx_window.begin(); rxit != rx_window.end(); rxit++) {
    pool->deallocate(rxit->second.buf);
  }
  rx_window.clear();

  // Drop all messages in TX window
  std::map<uint32_t, rlc_amd_tx_pdu_t>::iterator txit;
  for(txit = tx_window.begin(); txit != tx_window.end(); txit++) {
    pool->deallocate(txit->second.buf);
  }
  tx_window.clear();

  // Drop all messages in RETX queue
  retx_queue.clear();
  pthread_mutex_unlock(&mutex);
}

rlc_mode_t rlc_am::get_mode()
{
  return RLC_MODE_AM;
}

uint32_t rlc_am::get_bearer()
{
  return lcid;
}

/****************************************************************************
 * PDCP interface
 ***************************************************************************/

void rlc_am::write_sdu(byte_buffer_t *sdu)
{
  if (!tx_enabled) {
    byte_buffer_pool::get_instance()->deallocate(sdu);
    return;
  }
  if (sdu) {
    tx_sdu_queue.write(sdu);
    log->info_hex(sdu->msg, sdu->N_bytes, "%s Tx SDU (%d B, tx_sdu_queue_len=%d)", rrc->get_rb_name(lcid).c_str(), sdu->N_bytes, tx_sdu_queue.size());
  } else {
    log->warning("NULL SDU pointer in write_sdu()\n");
  }
}

void rlc_am::write_sdu_nb(byte_buffer_t *sdu)
{
  if (!tx_enabled) {
    byte_buffer_pool::get_instance()->deallocate(sdu);
    return;
  }
  if (sdu) {
    if (tx_sdu_queue.try_write(sdu)) {
      log->info_hex(sdu->msg, sdu->N_bytes, "%s Tx SDU (%d B, tx_sdu_queue_len=%d)", rrc->get_rb_name(lcid).c_str(), sdu->N_bytes, tx_sdu_queue.size());
    } else {
      log->debug_hex(sdu->msg, sdu->N_bytes, "[Dropped SDU] %s Tx SDU (%d B, tx_sdu_queue_len=%d)", rrc->get_rb_name(lcid).c_str(), sdu->N_bytes, tx_sdu_queue.size());
      pool->deallocate(sdu);
    }
  } else {
    log->warning("NULL SDU pointer in write_sdu()\n");
  }
}

/****************************************************************************
 * MAC interface
 ***************************************************************************/

uint32_t rlc_am::get_total_buffer_state()
{
  pthread_mutex_lock(&mutex);
  uint32_t n_bytes = 0;
  uint32_t n_sdus  = 0;

  // Bytes needed for status report
  check_reordering_timeout();
  if(do_status && !status_prohibited()) {
    n_bytes += prepare_status();
    log->debug("Buffer state - status report: %d bytes\n", n_bytes);
  }

  // Bytes needed for retx
  if(retx_queue.size() > 0) {
    rlc_amd_retx_t retx = retx_queue.front();
    log->debug("Buffer state - retx - SN: %d, Segment: %s, %d:%d\n", retx.sn, retx.is_segment ? "true" : "false", retx.so_start, retx.so_end);
    if(tx_window.end() != tx_window.find(retx.sn)) {
      int req_bytes = required_buffer_size(retx);
      if (req_bytes < 0) {
        log->error("In get_total_buffer_state(): Removing retx.sn=%d from queue\n", retx.sn);
        retx_queue.pop_front();
      } else {
        n_bytes += req_bytes;
        log->debug("Buffer state - retx: %d bytes\n", n_bytes);
      }
    }
  }

  // Bytes needed for tx SDUs
  if(tx_window.size() < 1024) {
    n_sdus  = tx_sdu_queue.size();
    n_bytes += tx_sdu_queue.size_bytes();
    if(tx_sdu)
    {
      n_sdus++;
      n_bytes += tx_sdu->N_bytes;
    }
  }

  // Room needed for header extensions? (integer rounding)
  if(n_sdus > 1)
    n_bytes += ((n_sdus-1)*1.5)+0.5;

  // Room needed for fixed header?
  if(n_bytes > 0) {
    n_bytes += 3;
    log->debug("Buffer state - tx SDUs: %d bytes\n", n_bytes);
  }

  pthread_mutex_unlock(&mutex);
  return n_bytes;
}

uint32_t rlc_am::get_buffer_state()
{
  pthread_mutex_lock(&mutex);
  uint32_t n_bytes = 0;
  uint32_t n_sdus  = 0;

  // Bytes needed for status report
  check_reordering_timeout();
  if(do_status && !status_prohibited()) {
    n_bytes = prepare_status();
    log->debug("Buffer state - status report: %d bytes\n", n_bytes);
    goto unlock_and_return;
  }

  // check if pollRetx timer expired (Section 5.2.2.3 in TS 36.322)
  if (poll_retx()) {
    // if both tx and retx buffer are empty, retransmit next PDU to be ack'ed
    log->debug("Poll reTx timer expired (lcid=%d)\n", lcid);
    if ((tx_window.size() > 0 && retx_queue.size() == 0 && tx_sdu_queue.size() == 0)) {
      std::map<uint32_t, rlc_amd_tx_pdu_t>::iterator it = tx_window.find(vt_s - 1);
      if (it != tx_window.end()) {
        log->info("Schedule last PDU (SN=%d) for reTx.\n", vt_s - 1);
        rlc_amd_retx_t retx;
        retx.is_segment = false;
        retx.so_start = 0;
        retx.so_end = tx_window[vt_s - 1].buf->N_bytes;
        retx.sn = vt_s - 1;
        retx_queue.push_back(retx);
      } else {
        log->error("Found invalid PDU in tx_window.\n");
      }
      poll_retx_timeout.start(cfg.t_poll_retx);
    }
  }

  // Bytes needed for retx
  if(retx_queue.size() > 0) {
    rlc_amd_retx_t retx = retx_queue.front();
    log->debug("Buffer state - retx - SN: %d, Segment: %s, %d:%d\n", retx.sn, retx.is_segment ? "true" : "false", retx.so_start, retx.so_end);
    if(tx_window.end() != tx_window.find(retx.sn)) {
      int req_bytes = required_buffer_size(retx);
      if (req_bytes < 0) {
        log->error("In get_buffer_state(): Removing retx.sn=%d from queue\n", retx.sn);
        retx_queue.pop_front();
        goto unlock_and_return;
      }
      n_bytes = (uint32_t) req_bytes;
      log->debug("Buffer state - retx: %d bytes\n", n_bytes);
      goto unlock_and_return;
    }
  }

  // Bytes needed for tx SDUs
  if(tx_window.size() < 1024) {
    n_sdus  = tx_sdu_queue.size();
    n_bytes = tx_sdu_queue.size_bytes();
    if(tx_sdu)
    {
      n_sdus++;
      n_bytes += tx_sdu->N_bytes;
    }
  }

  // Room needed for header extensions? (integer rounding)
  if(n_sdus > 1)
    n_bytes += ((n_sdus-1)*1.5)+0.5;

  // Room needed for fixed header?
  if(n_bytes > 0) {
    n_bytes += 3;
    log->debug("Buffer state - tx SDUs: %d bytes\n", n_bytes);
  }

unlock_and_return:
  pthread_mutex_unlock(&mutex);
  return n_bytes;
}

int rlc_am::read_pdu(uint8_t *payload, uint32_t nof_bytes)
{
  pthread_mutex_lock(&mutex);

  log->debug("MAC opportunity - %d bytes\n", nof_bytes);
  log->debug("tx_window size - %zu PDUs\n", tx_window.size());

  // Tx STATUS if requested
  if(do_status && !status_prohibited()) {
    pthread_mutex_unlock(&mutex);
    return build_status_pdu(payload, nof_bytes);
  }

  // if tx_window is full and retx_queue empty, retransmit next PDU to be ack'ed
  if (tx_window.size() >= RLC_AM_WINDOW_SIZE && retx_queue.size() == 0) {
    if (tx_window[vt_a].buf != NULL) {
      log->warning("Full Tx window, ReTx'ing first outstanding PDU\n");
      rlc_amd_retx_t retx;
      retx.is_segment = false;
      retx.so_start   = 0;
      retx.so_end     = tx_window[vt_a].buf->N_bytes;
      retx.sn         = vt_a;
      retx_queue.push_back(retx);
    } else {
      log->error("Found invalid PDU in tx_window.\n");
    }
  }

  // RETX if required
  if(retx_queue.size() > 0) {
    int ret = build_retx_pdu(payload, nof_bytes);
    if (ret > 0) {
      pthread_mutex_unlock(&mutex);
      return ret;
    }
  }

  // Build a PDU from SDUs
  int ret = build_data_pdu(payload, nof_bytes);

  pthread_mutex_unlock(&mutex);
  return ret;
}

void rlc_am::write_pdu(uint8_t *payload, uint32_t nof_bytes)
{
  if(nof_bytes < 1)
    return;
  pthread_mutex_lock(&mutex);

  if(rlc_am_is_control_pdu(payload)) {
    handle_control_pdu(payload, nof_bytes);
  } else {
    rlc_amd_pdu_header_t header;
    rlc_am_read_data_pdu_header(&payload, &nof_bytes, &header);
    if(header.rf) {
      handle_data_pdu_segment(payload, nof_bytes, header);
    }else{
      handle_data_pdu(payload, nof_bytes, header);
    }
  }
  pthread_mutex_unlock(&mutex);
}

/****************************************************************************
 * Timer checks
 ***************************************************************************/

bool rlc_am::status_prohibited()
{
  return (status_prohibit_timeout.is_running() && !status_prohibit_timeout.expired());
}

bool rlc_am::poll_retx()
{
  return (poll_retx_timeout.is_running() && poll_retx_timeout.expired());
}

void rlc_am::check_reordering_timeout()
{
  if(reordering_timeout.is_running() && reordering_timeout.expired())
  {
    reordering_timeout.reset();
    log->debug("%s reordering timeout expiry - updating vr_ms\n", rrc->get_rb_name(lcid).c_str());

    // 36.322 v10 Section 5.1.3.2.4
    vr_ms = vr_x;
    std::map<uint32_t, rlc_amd_rx_pdu_t>::iterator it = rx_window.find(vr_ms);
    while(rx_window.end() != it)
    {
      vr_ms = (vr_ms + 1)%MOD;
      it = rx_window.find(vr_ms);
    }
    if(poll_received)
      do_status = true;

    if(RX_MOD_BASE(vr_h) > RX_MOD_BASE(vr_ms))
    {
      reordering_timeout.start(cfg.t_reordering);
      vr_x = vr_h;
    }

    debug_state();
  }
}

/****************************************************************************
 * Helpers
 ***************************************************************************/

bool rlc_am::poll_required()
{
  if(cfg.poll_pdu > 0 && pdu_without_poll > (uint32_t)cfg.poll_pdu)
    return true;
  if(cfg.poll_byte > 0 && byte_without_poll > (uint32_t)cfg.poll_byte)
    return true;
  if(poll_retx())
    return true;

  if(tx_sdu_queue.size() == 0 && retx_queue.size() == 0)
    return true;

  /* According to 5.2.2.1 in 36.322 v13.3.0 a poll should be requested if
   * the entire AM window is unacknowledged, i.e. no new PDU can be transmitted.
   * However, it seems more appropiate to request more often if polling
   * is disabled otherwise, e.g. every N PDUs.
   */
  if (cfg.poll_pdu == 0 && cfg.poll_byte == 0 && vt_s % poll_periodicity == 0)
    return true;

  return false;
}

int rlc_am::prepare_status()
{
  status.N_nack = 0;
  status.ack_sn = vr_ms;

  // We don't use segment NACKs - just NACK the full PDU

  uint32_t i = vr_r;
  while(RX_MOD_BASE(i) < RX_MOD_BASE(vr_ms))
  {
    if(rx_window.find(i) == rx_window.end())
      status.nacks[status.N_nack++].nack_sn = i;
    i = (i + 1)%MOD;
  }

  return rlc_am_packed_length(&status);
}

int  rlc_am::build_status_pdu(uint8_t *payload, uint32_t nof_bytes)
{
  int pdu_len = rlc_am_packed_length(&status);
  if(pdu_len > 0 && nof_bytes >= (uint32_t)pdu_len)
  {
    log->info("%s Tx status PDU - %s\n",
              rrc->get_rb_name(lcid).c_str(), rlc_am_to_string(&status).c_str());

    do_status     = false;
    poll_received = false;

    if(cfg.t_status_prohibit > 0)
      status_prohibit_timeout.start(cfg.t_status_prohibit);
    debug_state();
    return rlc_am_write_status_pdu(&status, payload);
  }else{
    log->warning("%s Cannot tx status PDU - %d bytes available, %d bytes required\n",
                 rrc->get_rb_name(lcid).c_str(), nof_bytes, pdu_len);
    return 0;
  }
}

int  rlc_am::build_retx_pdu(uint8_t *payload, uint32_t nof_bytes)
{
  // Check there is at least 1 element before calling front()
  if (retx_queue.empty()) {
    log->error("In build_retx_pdu(): retx_queue is empty\n");
    return -1;
  }

  rlc_amd_retx_t retx = retx_queue.front();

  // Sanity check - drop any retx SNs not present in tx_window
  while(tx_window.end() == tx_window.find(retx.sn)) {
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
  if(retx.is_segment || req_size > (int)nof_bytes) {
    log->debug("%s build_retx_pdu - resegmentation required\n", rrc->get_rb_name(lcid).c_str());
    return build_segment(payload, nof_bytes, retx);
  }

  // Update & write header
  rlc_amd_pdu_header_t new_header = tx_window[retx.sn].header;
  new_header.p = 0;

  // Set poll bit
  pdu_without_poll++;
  byte_without_poll += (tx_window[retx.sn].buf->N_bytes + rlc_am_packed_length(&new_header));
  log->info("%s pdu_without_poll: %d\n", rrc->get_rb_name(lcid).c_str(), pdu_without_poll);
  log->info("%s byte_without_poll: %d\n", rrc->get_rb_name(lcid).c_str(), byte_without_poll);
  if(poll_required())
  {
    new_header.p      = 1;
    poll_sn           = vt_s;
    pdu_without_poll  = 0;
    byte_without_poll = 0;
    poll_retx_timeout.start(cfg.t_poll_retx);
  }

  uint8_t *ptr = payload;
  rlc_am_write_data_pdu_header(&new_header, &ptr);
  memcpy(ptr, tx_window[retx.sn].buf->msg, tx_window[retx.sn].buf->N_bytes);

  retx_queue.pop_front();
  tx_window[retx.sn].retx_count++;
  if(tx_window[retx.sn].retx_count >= cfg.max_retx_thresh)
    rrc->max_retx_attempted();
  log->info("%s Retx PDU scheduled for tx. SN: %d, retx count: %d\n",
            rrc->get_rb_name(lcid).c_str(), retx.sn, tx_window[retx.sn].retx_count);

  debug_state();
  return (ptr-payload) + tx_window[retx.sn].buf->N_bytes;
}

int rlc_am::build_segment(uint8_t *payload, uint32_t nof_bytes, rlc_amd_retx_t retx)
{
  if (!tx_window[retx.sn].buf) {
    log->error("In build_segment: retx.sn=%d has null buffer\n", retx.sn);
    return 0;
  }
  if(!retx.is_segment){
    retx.so_start = 0;
    retx.so_end   = tx_window[retx.sn].buf->N_bytes;
  }

  // Construct new header
  rlc_amd_pdu_header_t new_header;
  rlc_amd_pdu_header_t old_header = tx_window[retx.sn].header;

  pdu_without_poll++;
  byte_without_poll += (tx_window[retx.sn].buf->N_bytes + rlc_am_packed_length(&new_header));
  log->info("%s pdu_without_poll: %d\n", rrc->get_rb_name(lcid).c_str(), pdu_without_poll);
  log->info("%s byte_without_poll: %d\n", rrc->get_rb_name(lcid).c_str(), byte_without_poll);

  new_header.dc   = RLC_DC_FIELD_DATA_PDU;
  new_header.rf   = 1;
  new_header.fi   = RLC_FI_FIELD_NOT_START_OR_END_ALIGNED;
  new_header.sn   = old_header.sn;
  new_header.lsf  = 0;
  new_header.so   = retx.so_start;
  new_header.N_li = 0;
  new_header.p    = 0;
  if(poll_required())
  {
    log->debug("%s setting poll bit to request status\n", rrc->get_rb_name(lcid).c_str());
    new_header.p      = 1;
    poll_sn           = vt_s;
    pdu_without_poll  = 0;
    byte_without_poll = 0;
    poll_retx_timeout.start(cfg.t_poll_retx);
  }

  uint32_t head_len  = 0;
  uint32_t pdu_space = 0;

  head_len = rlc_am_packed_length(&new_header);
  if(nof_bytes <= head_len)
  {
    log->warning("%s Cannot build a PDU segment - %d bytes available, %d bytes required for header\n",
                 rrc->get_rb_name(lcid).c_str(), nof_bytes, head_len);
    return 0;
  }

  pdu_space = nof_bytes-head_len;
  if(pdu_space < (retx.so_end-retx.so_start))
    retx.so_end = retx.so_start+pdu_space;

  // Need to rebuild the li table & update fi based on so_start and so_end
  if(retx.so_start == 0 && rlc_am_start_aligned(old_header.fi))
    new_header.fi &= RLC_FI_FIELD_NOT_END_ALIGNED;   // segment is start aligned

  uint32_t lower     = 0;
  uint32_t upper     = 0;
  uint32_t li        = 0;

  for(uint32_t i=0; i<old_header.N_li; i++) {
    if(lower >= retx.so_end)
      break;

    if(pdu_space <= 2)
      break;

    upper += old_header.li[i];

    head_len    = rlc_am_packed_length(&new_header);
    pdu_space   = nof_bytes-head_len;
    if(pdu_space < (retx.so_end-retx.so_start))
      retx.so_end = retx.so_start+pdu_space;

    if(upper > retx.so_start && lower < retx.so_end) {  // Current SDU is needed
      li = upper - lower;
      if(upper > retx.so_end)
        li -= upper - retx.so_end;
      if(lower < retx.so_start)
        li -= retx.so_start - lower;
      if(lower > 0 && lower == retx.so_start)
        new_header.fi &= RLC_FI_FIELD_NOT_END_ALIGNED;   // segment start is aligned with this SDU
      if(upper == retx.so_end) {
        new_header.fi &= RLC_FI_FIELD_NOT_START_ALIGNED; // segment end is aligned with this SDU
      }
      new_header.li[new_header.N_li++] = li;
    }

    lower += old_header.li[i];
  }

  // Make sure LI is not deleted in case the SDU boundary is crossed
  // FIXME: fix if N_li > 1
  if (new_header.N_li == 1 && retx.so_start + new_header.li[0] < retx.so_end && retx.so_end <= retx.so_start + pdu_space) {
    // This segment crosses a SDU boundary
    new_header.N_li++;
  }

  // Update retx_queue
  if(tx_window[retx.sn].buf->N_bytes == retx.so_end) {
    retx_queue.pop_front();
    new_header.lsf = 1;
    if(rlc_am_end_aligned(old_header.fi))
      new_header.fi &= RLC_FI_FIELD_NOT_START_ALIGNED;   // segment is end aligned
  } else if(retx_queue.front().so_end == retx.so_end) {
    retx_queue.pop_front();
  } else {
    retx_queue.front().is_segment = true;
    retx_queue.front().so_start = retx.so_end;
    if(new_header.N_li > 0)
      new_header.N_li--;
  }

  // Write header and pdu
  uint8_t *ptr = payload;
  rlc_am_write_data_pdu_header(&new_header, &ptr);
  uint8_t* data = &tx_window[retx.sn].buf->msg[retx.so_start];
  uint32_t len  = retx.so_end - retx.so_start;
  memcpy(ptr, data, len);

  log->info("%s Retx PDU segment scheduled for tx. SN: %d, SO: %d\n",
            rrc->get_rb_name(lcid).c_str(), retx.sn, retx.so_start);

  debug_state();
  int pdu_len = (ptr-payload) + len;
  if(pdu_len > (int)nof_bytes) {
    log->error("%s Retx PDU segment length error. Available: %d, Used: %d\n",
               rrc->get_rb_name(lcid).c_str(), nof_bytes, pdu_len);
    log->debug("%s Retx PDU segment length error. Header len: %ld, Payload len: %d, N_li: %d\n",
               rrc->get_rb_name(lcid).c_str(), (ptr-payload), len, new_header.N_li);
  }
  return pdu_len;

}

int  rlc_am::build_data_pdu(uint8_t *payload, uint32_t nof_bytes)
{
  if(!tx_sdu && tx_sdu_queue.size() == 0)
  {
    log->info("No data available to be sent\n");
    return 0;
  }

  // do not build any more PDU if window is already full
  if (!tx_sdu && tx_window.size() >= RLC_AM_WINDOW_SIZE) {
    log->info("Tx window full.\n");
    return 0;
  }

  byte_buffer_t *pdu = pool_allocate;
  if (!pdu) {
#ifdef RLC_AM_BUFFER_DEBUG
    log->console("Fatal Error: Could not allocate PDU in build_data_pdu()\n");
    log->console("tx_window size: %d PDUs\n", tx_window.size());
    log->console("vt_a = %d, vt_ms = %d, vt_s = %d, poll_sn = %d "
                 "vr_r = %d, vr_mr = %d, vr_x = %d, vr_ms = %d, vr_h = %d\n",
                 vt_a, vt_ms, vt_s, poll_sn,
                 vr_r, vr_mr, vr_x, vr_ms, vr_h);
    log->console("retx_queue size: %d PDUs\n", retx_queue.size());
    std::map<uint32_t, rlc_amd_tx_pdu_t>::iterator txit;
    for(txit = tx_window.begin(); txit != tx_window.end(); txit++) {
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
  uint32_t pdu_space = nof_bytes;
  uint8_t *pdu_ptr   = pdu->msg;

  if(pdu_space <= head_len + 1)
  {
    log->warning("%s Cannot build a PDU - %d bytes available, %d bytes required for header\n",
                 rrc->get_rb_name(lcid).c_str(), nof_bytes, head_len);
    pool->deallocate(pdu);
    return 0;
  }

  log->debug("%s Building PDU - pdu_space: %d, head_len: %d \n",
             rrc->get_rb_name(lcid).c_str(), pdu_space, head_len);

  // Check for SDU segment
  if(tx_sdu)
  {
    to_move = ((pdu_space-head_len) >= tx_sdu->N_bytes) ? tx_sdu->N_bytes : pdu_space-head_len;
    memcpy(pdu_ptr, tx_sdu->msg, to_move);
    last_li          = to_move;
    pdu_ptr         += to_move;
    pdu->N_bytes    += to_move;
    tx_sdu->N_bytes -= to_move;
    tx_sdu->msg     += to_move;
    if(tx_sdu->N_bytes == 0)
    {
      log->debug("%s Complete SDU scheduled for tx. Stack latency: %ld us\n",
                rrc->get_rb_name(lcid).c_str(), tx_sdu->get_latency_us());
      pool->deallocate(tx_sdu);
      tx_sdu = NULL;
    }
    if(pdu_space > to_move)
      pdu_space -= to_move;
    else
      pdu_space = 0;
    header.fi |= RLC_FI_FIELD_NOT_START_ALIGNED; // First byte does not correspond to first byte of SDU

    log->debug("%s Building PDU - added SDU segment (len:%d) - pdu_space: %d, head_len: %d \n",
               rrc->get_rb_name(lcid).c_str(), to_move, pdu_space, head_len);
  }

  // Pull SDUs from queue
  while(pdu_space > head_len + 1 && tx_sdu_queue.size() > 0)
  {
    if(last_li > 0)
      header.li[header.N_li++] = last_li;
    head_len = rlc_am_packed_length(&header);
    if(head_len >= pdu_space) {
      header.N_li--;
      break;
    }
    tx_sdu_queue.read(&tx_sdu);
    to_move = ((pdu_space-head_len) >= tx_sdu->N_bytes) ? tx_sdu->N_bytes : pdu_space-head_len;
    memcpy(pdu_ptr, tx_sdu->msg, to_move);
    last_li          = to_move;
    pdu_ptr         += to_move;
    pdu->N_bytes    += to_move;
    tx_sdu->N_bytes -= to_move;
    tx_sdu->msg     += to_move;
    if(tx_sdu->N_bytes == 0)
    {
      log->debug("%s Complete SDU scheduled for tx. Stack latency: %ld us\n",
                rrc->get_rb_name(lcid).c_str(), tx_sdu->get_latency_us());
      pool->deallocate(tx_sdu);
      tx_sdu = NULL;
    }
    if(pdu_space > to_move)
      pdu_space -= to_move;
    else
      pdu_space = 0;

    log->debug("%s Building PDU - added SDU segment (len:%d) - pdu_space: %d, head_len: %d \n",
               rrc->get_rb_name(lcid).c_str(), to_move, pdu_space, head_len);
  }

  // Make sure, at least one SDU (segment) has been added until this point
  if (pdu->N_bytes == 0) {
    log->error("Generated empty RLC PDU.\n");
    return 0;
  }

  if(tx_sdu)
    header.fi |= RLC_FI_FIELD_NOT_END_ALIGNED; // Last byte does not correspond to last byte of SDU

  // Set Poll bit
  pdu_without_poll++;
  byte_without_poll += (pdu->N_bytes + head_len);
  log->debug("%s pdu_without_poll: %d\n", rrc->get_rb_name(lcid).c_str(), pdu_without_poll);
  log->debug("%s byte_without_poll: %d\n", rrc->get_rb_name(lcid).c_str(), byte_without_poll);
  if(poll_required())
  {
    log->debug("%s setting poll bit to request status\n", rrc->get_rb_name(lcid).c_str());
    header.p          = 1;
    poll_sn           = vt_s;
    pdu_without_poll  = 0;
    byte_without_poll = 0;
    poll_retx_timeout.start(cfg.t_poll_retx);
  }

  // Set SN
  header.sn = vt_s;
  vt_s = (vt_s + 1)%MOD;

  // Place PDU in tx_window, write header and TX
  tx_window[header.sn].buf        = pdu;
  tx_window[header.sn].header     = header;
  tx_window[header.sn].is_acked   = false;
  tx_window[header.sn].retx_count = 0;

  uint8_t *ptr = payload;
  rlc_am_write_data_pdu_header(&header, &ptr);
  memcpy(ptr, pdu->msg, pdu->N_bytes);
  log->info_hex(payload, pdu->N_bytes, "%s PDU scheduled for tx. SN: %d (%d B)\n", rrc->get_rb_name(lcid).c_str(), header.sn, pdu->N_bytes);

  debug_state();
  return (ptr-payload) + pdu->N_bytes;
}

void rlc_am::handle_data_pdu(uint8_t *payload, uint32_t nof_bytes, rlc_amd_pdu_header_t &header)
{
  std::map<uint32_t, rlc_amd_rx_pdu_t>::iterator it;

  log->info_hex(payload, nof_bytes, "%s Rx data PDU SN: %d (%d B), %s",
                rrc->get_rb_name(lcid).c_str(), header.sn, nof_bytes, rlc_fi_field_text[header.fi]);

  if(!inside_rx_window(header.sn)) {
    if(header.p) {
      log->info("%s Status packet requested through polling bit\n", rrc->get_rb_name(lcid).c_str());
      do_status = true;
    }
    log->info("%s SN: %d outside rx window [%d:%d] - discarding\n",
              rrc->get_rb_name(lcid).c_str(), header.sn, vr_r, vr_mr);
    return;
  }

  it = rx_window.find(header.sn);
  if(rx_window.end() != it) {
    if(header.p) {
      log->info("%s Status packet requested through polling bit\n", rrc->get_rb_name(lcid).c_str());
      do_status = true;
    }
    log->info("%s Discarding duplicate SN: %d\n",
              rrc->get_rb_name(lcid).c_str(), header.sn);
    return;
  }

  // Write to rx window
  rlc_amd_rx_pdu_t pdu;
  pdu.buf = pool_allocate;
  if (!pdu.buf) {
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
              rrc->get_rb_name(lcid).c_str(), header.sn, nof_bytes, pdu.buf->get_tailroom());
    pool->deallocate(pdu.buf);
    return;
  }
  memcpy(pdu.buf->msg, payload, nof_bytes);
  pdu.buf->N_bytes  = nof_bytes;
  memcpy(&pdu.header, &header, sizeof(rlc_amd_pdu_header_t));

  rx_window[header.sn] = pdu;

  // Update vr_h
  if(RX_MOD_BASE(header.sn) >= RX_MOD_BASE(vr_h))
    vr_h  = (header.sn + 1)%MOD;

  // Update vr_ms
  it = rx_window.find(vr_ms);
  while(rx_window.end() != it)
  {
    vr_ms = (vr_ms + 1)%MOD;
    it = rx_window.find(vr_ms);
  }

  // Check poll bit
  if(header.p)
  {
    log->info("%s Status packet requested through polling bit\n", rrc->get_rb_name(lcid).c_str());
    poll_received = true;

    // 36.322 v10 Section 5.2.3
    if(RX_MOD_BASE(header.sn) < RX_MOD_BASE(vr_ms) ||
       RX_MOD_BASE(header.sn) >= RX_MOD_BASE(vr_mr))
    {
      do_status = true;
    }
    // else delay for reordering timer
  }

  // Reassemble and deliver SDUs
  reassemble_rx_sdus();

  // Update reordering variables and timers (36.322 v10.0.0 Section 5.1.3.2.3)
  if(reordering_timeout.is_running())
  {
    if(vr_x == vr_r || (!inside_rx_window(vr_x) && vr_x != vr_mr))
    {
      reordering_timeout.reset();
    }
  }
  if(!reordering_timeout.is_running())
  {
    if(RX_MOD_BASE(vr_h) > RX_MOD_BASE(vr_r))
    {
      reordering_timeout.start(cfg.t_reordering);
      vr_x = vr_h;
    }
  }

  debug_state();
}

void rlc_am::handle_data_pdu_segment(uint8_t *payload, uint32_t nof_bytes, rlc_amd_pdu_header_t &header)
{
  std::map<uint32_t, rlc_amd_rx_pdu_segments_t>::iterator it;

  log->info_hex(payload, nof_bytes, "%s Rx data PDU segment. SN: %d, SO: %d",
                rrc->get_rb_name(lcid).c_str(), header.sn, header.so);

  // Check inside rx window
  if(!inside_rx_window(header.sn)) {
    if(header.p) {
      log->info("%s Status packet requested through polling bit\n", rrc->get_rb_name(lcid).c_str());
      do_status = true;
    }
    log->info("%s SN: %d outside rx window [%d:%d] - discarding\n",
              rrc->get_rb_name(lcid).c_str(), header.sn, vr_r, vr_mr);
    return;
  }

  rlc_amd_rx_pdu_t segment;
  segment.buf = pool_allocate;
  if (!segment.buf) {
#ifdef RLC_AM_BUFFER_DEBUG
    log->console("Fatal Error: Couldn't allocate PDU in handle_data_pdu_segment().\n");
    exit(-1);
#else
    log->error("Fatal Error: Couldn't allocate PDU in handle_data_pdu_segment().\n");
    return;
#endif
  }

  memcpy(segment.buf->msg, payload, nof_bytes);
  segment.buf->N_bytes = nof_bytes;
  memcpy(&segment.header, &header, sizeof(rlc_amd_pdu_header_t));

  // Check if we already have a segment from the same PDU
  it = rx_segments.find(header.sn);
  if(rx_segments.end() != it) {

    if(header.p) {
      log->info("%s Status packet requested through polling bit\n", rrc->get_rb_name(lcid).c_str());
      do_status = true;
    }

    // Add segment to PDU list and check for complete
    if(add_segment_and_check(&it->second, &segment)) {
      std::list<rlc_amd_rx_pdu_t>::iterator segit;
      std::list<rlc_amd_rx_pdu_t>           seglist = it->second.segments;
      for(segit = seglist.begin(); segit != seglist.end(); segit++) {
        pool->deallocate(segit->buf);
      }
      seglist.clear();
      rx_segments.erase(it);
    }

  } else {

    // Create new PDU segment list and write to rx_segments
    rlc_amd_rx_pdu_segments_t pdu;
    pdu.segments.push_back(segment);
    rx_segments[header.sn] = pdu;


    // Update vr_h
    if(RX_MOD_BASE(header.sn) >= RX_MOD_BASE(vr_h))
      vr_h  = (header.sn + 1)%MOD;

    // Check poll bit
    if(header.p)
    {
      log->info("%s Status packet requested through polling bit\n", rrc->get_rb_name(lcid).c_str());
      poll_received = true;

      // 36.322 v10 Section 5.2.3
      if(RX_MOD_BASE(header.sn) < RX_MOD_BASE(vr_ms) ||
         RX_MOD_BASE(header.sn) >= RX_MOD_BASE(vr_mr))
      {
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

void rlc_am::handle_control_pdu(uint8_t *payload, uint32_t nof_bytes)
{
  log->info_hex(payload, nof_bytes, "%s Rx control PDU", rrc->get_rb_name(lcid).c_str());

  rlc_status_pdu_t status;
  rlc_am_read_status_pdu(payload, nof_bytes, &status);

  log->info("%s Rx Status PDU: %s\n", rrc->get_rb_name(lcid).c_str(), rlc_am_to_string(&status).c_str());

  poll_retx_timeout.reset();

  // flush retx queue to avoid unordered SNs, we expect the Rx to request lost PDUs again
  if (status.N_nack > 0) {
    retx_queue.clear();
  }

  // Handle ACKs and NACKs
  std::map<uint32_t, rlc_amd_tx_pdu_t>::iterator it;
  bool update_vt_a = true;
  uint32_t i       = vt_a;

  while(TX_MOD_BASE(i) < TX_MOD_BASE(status.ack_sn) &&
        TX_MOD_BASE(i) < TX_MOD_BASE(vt_s))
  {
    bool nack = false;
    for(uint32_t j=0;j<status.N_nack;j++) {
      if(status.nacks[j].nack_sn == i) {
        nack = true;
        update_vt_a = false;
        it = tx_window.find(i);
        if(tx_window.end() != it)
        {
          if(!retx_queue_has_sn(i)) {
            rlc_amd_retx_t retx;
            retx.is_segment = false;
            retx.so_start   = 0;
            retx.so_end     = it->second.buf->N_bytes;

            if(status.nacks[j].has_so) {
              // sanity check
              if (status.nacks[j].so_start >= it->second.buf->N_bytes) {
                // print error but try to send original PDU again
                log->info("SO_start is larger than original PDU (%d >= %d)\n",
                           status.nacks[j].so_start,
                           it->second.buf->N_bytes);
                status.nacks[j].so_start = 0;
              }

              // check for special SO_end value
              if(status.nacks[j].so_end == 0x7FFF) {
                status.nacks[j].so_end = it->second.buf->N_bytes;
              }else{
                retx.so_end = status.nacks[j].so_end + 1;
              }

              if(status.nacks[j].so_start <  it->second.buf->N_bytes &&
                 status.nacks[j].so_end   <= it->second.buf->N_bytes) {
                  retx.is_segment = true;
                  retx.so_start = status.nacks[j].so_start;
              } else {
                log->warning("%s invalid segment NACK received for SN %d. so_start: %d, so_end: %d, N_bytes: %d\n",
                             rrc->get_rb_name(lcid).c_str(), i, status.nacks[j].so_start, status.nacks[j].so_end, it->second.buf->N_bytes);
              }
            }

            retx.sn         = i;
            retx_queue.push_back(retx);
          }
        }
      }
    }

    if(!nack) {
      //ACKed SNs get marked and removed from tx_window if possible
      if(tx_window.count(i) > 0) {
        it = tx_window.find(i);
        if (it != tx_window.end()) {
          if(update_vt_a) {
            if(it->second.buf) {
              pool->deallocate(it->second.buf);
              it->second.buf = 0;
            }
            tx_window.erase(it);
            vt_a = (vt_a + 1)%MOD;
            vt_ms = (vt_ms + 1)%MOD;
          }
        }
      }
    }
    i = (i+1)%MOD;
  }

  debug_state();
}

void rlc_am::reassemble_rx_sdus()
{
  uint32_t len = 0;
  if(!rx_sdu) {
    rx_sdu = pool_allocate;
    if (!rx_sdu) {
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
  while(rx_window.end() != rx_window.find(vr_r))
  {
    // Handle any SDU segments
    for(uint32_t i=0; i<rx_window[vr_r].header.N_li; i++)
    {
      len = rx_window[vr_r].header.li[i];
      // sanity check to avoid zero-size SDUs
      if (len == 0) {
        break;
      }

      if (rx_sdu->get_tailroom() >= len) {
        if ((rx_window[vr_r].buf->msg - rx_window[vr_r].buf->buffer) + len < SRSLTE_MAX_BUFFER_SIZE_BYTES) {
          memcpy(&rx_sdu->msg[rx_sdu->N_bytes], rx_window[vr_r].buf->msg, len);
          rx_sdu->N_bytes += len;
          rx_window[vr_r].buf->msg += len;
          rx_window[vr_r].buf->N_bytes -= len;
          log->info_hex(rx_sdu->msg, rx_sdu->N_bytes, "%s Rx SDU (%d B)", rrc->get_rb_name(lcid).c_str(), rx_sdu->N_bytes);
          rx_sdu->set_timestamp();
          pdcp->write_pdu(lcid, rx_sdu);

          rx_sdu = pool_allocate;
          if (!rx_sdu) {
#ifdef RLC_AM_BUFFER_DEBUG
            log->console("Fatal Error: Could not allocate PDU in reassemble_rx_sdus() (2)\n");
          exit(-1);
#else
            log->error("Fatal Error: Could not allocate PDU in reassemble_rx_sdus() (2)\n");
            return;
#endif
          }
        } else {
          log->error("Cannot read %d bytes from rx_window. vr_r=%d, msg-buffer=%ld bytes\n", len, vr_r, (rx_window[vr_r].buf->msg - rx_window[vr_r].buf->buffer));
          pool->deallocate(rx_sdu);
          goto exit;
        }
      } else {
        log->error("Cannot fit RLC PDU in SDU buffer, dropping both.\n");
        pool->deallocate(rx_sdu);
        goto exit;
      }
    }

    // Handle last segment
    len = rx_window[vr_r].buf->N_bytes;
    if (rx_sdu->get_tailroom() >= len) {
      memcpy(&rx_sdu->msg[rx_sdu->N_bytes], rx_window[vr_r].buf->msg, len);
      rx_sdu->N_bytes += rx_window[vr_r].buf->N_bytes;
    } else {
      log->error("Cannot fit RLC PDU in SDU buffer, dropping both.\n");
      pool->deallocate(rx_sdu);
      pool->deallocate(rx_window[vr_r].buf);
      rx_window.erase(vr_r);
    }

    if(rlc_am_end_aligned(rx_window[vr_r].header.fi)) {
      log->info_hex(rx_sdu->msg, rx_sdu->N_bytes, "%s Rx SDU (%d B)", rrc->get_rb_name(lcid).c_str(), rx_sdu->N_bytes);
      rx_sdu->set_timestamp();
      pdcp->write_pdu(lcid, rx_sdu);
      rx_sdu = pool_allocate;
      if (!rx_sdu) {
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
    pool->deallocate(rx_window[vr_r].buf);
    rx_window.erase(vr_r);
    vr_r = (vr_r + 1)%MOD;
    vr_mr = (vr_mr + 1)%MOD;
  }
}

bool rlc_am::inside_tx_window(uint16_t sn)
{
  if(RX_MOD_BASE(sn) >= RX_MOD_BASE(vt_a) &&
     RX_MOD_BASE(sn) <  RX_MOD_BASE(vt_ms))
  {
    return true;
  }else{
    return false;
  }
}

bool rlc_am::inside_rx_window(uint16_t sn)
{
  if(RX_MOD_BASE(sn) >= RX_MOD_BASE(vr_r) &&
     RX_MOD_BASE(sn) <  RX_MOD_BASE(vr_mr))
  {
    return true;
  }else{
    return false;
  }
}

void rlc_am::debug_state()
{
  log->debug("%s vt_a = %d, vt_ms = %d, vt_s = %d, poll_sn = %d "
             "vr_r = %d, vr_mr = %d, vr_x = %d, vr_ms = %d, vr_h = %d\n",
             rrc->get_rb_name(lcid).c_str(), vt_a, vt_ms, vt_s, poll_sn,
             vr_r, vr_mr, vr_x, vr_ms, vr_h);
}

void rlc_am::print_rx_segments()
{
  std::map<uint32_t, rlc_amd_rx_pdu_segments_t>::iterator it;
  std::stringstream ss;
  ss << "rx_segments:" << std::endl;
  for(it=rx_segments.begin();it!=rx_segments.end();it++) {
    std::list<rlc_amd_rx_pdu_t>::iterator segit;
    for(segit = it->second.segments.begin(); segit != it->second.segments.end(); segit++) {
      ss << "    SN:" << segit->header.sn << " SO:" << segit->header.so << " N:" << segit->buf->N_bytes <<  " N_li: " << segit->header.N_li << std::endl;
    }
  }
  log->debug("%s\n", ss.str().c_str());
}

bool rlc_am::add_segment_and_check(rlc_amd_rx_pdu_segments_t *pdu, rlc_amd_rx_pdu_t *segment)
{
  // Check for first segment
  if(0 == segment->header.so) {
    std::list<rlc_amd_rx_pdu_t>::iterator it;
    for(it = pdu->segments.begin(); it != pdu->segments.end(); it++) {
      pool->deallocate(it->buf);
    }
    pdu->segments.clear();
    pdu->segments.push_back(*segment);
    return false;
  }

  // Check segment offset
  uint32_t n = 0;
  if(!pdu->segments.empty()) {
    rlc_amd_rx_pdu_t &back = pdu->segments.back();
    n = back.header.so + back.buf->N_bytes;
  }
  if(segment->header.so != n) {
    pool->deallocate(segment->buf);
    return false;
  } else {
    pdu->segments.push_back(*segment);
  }

  // Check for complete
  uint32_t so = 0;
  std::list<rlc_amd_rx_pdu_t>::iterator it, tmpit;
  for(it = pdu->segments.begin(); it != pdu->segments.end(); it++) {
    if(so != it->header.so)
      return false;
    so += it->buf->N_bytes;
  }
  if(!pdu->segments.back().header.lsf)
    return false;

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
  header.fi |= (pdu->segments.back().header.fi  & RLC_FI_FIELD_NOT_END_ALIGNED);

  // Reconstruct li fields
  uint16_t count     = 0;
  uint16_t carryover = 0;
  for(it = pdu->segments.begin(); it != pdu->segments.end(); it++) {
    if(it->header.N_li > 0) {
      header.li[header.N_li++] = it->header.li[0] + carryover;
      count += it->header.li[0];
      for(uint32_t i=1; i<it->header.N_li; i++) {
        header.li[header.N_li++] = it->header.li[i];
        count += it->header.li[i];
      }
    }

    // accumulate segment sizes until end aligned PDU is received
    if (rlc_am_not_start_aligned(it->header.fi)) {
      carryover += it->buf->N_bytes - count;
    } else {
      carryover = it->buf->N_bytes - count;
    }
    tmpit = it;
    if(rlc_am_end_aligned(it->header.fi) && ++tmpit != pdu->segments.end()) {
      header.li[header.N_li++] = carryover;
      carryover = 0;
    }
    count = 0;
  }

  // Copy data
  byte_buffer_t *full_pdu = pool_allocate;
  if (!full_pdu) {
#ifdef RLC_AM_BUFFER_DEBUG
    log->console("Fatal Error: Could not allocate PDU in add_segment_and_check()\n");
    exit(-1);
#else
    log->error("Fatal Error: Could not allocate PDU in add_segment_and_check()\n");
    return false;
#endif
  }
  for(it = pdu->segments.begin(); it != pdu->segments.end(); it++) {
    memcpy(&full_pdu->msg[full_pdu->N_bytes], it->buf->msg, it->buf->N_bytes);
    full_pdu->N_bytes += it->buf->N_bytes;
  }

  handle_data_pdu(full_pdu->msg, full_pdu->N_bytes, header);
  pool->deallocate(full_pdu);
  return true;
}

int rlc_am::required_buffer_size(rlc_amd_retx_t retx)
{
  if(!retx.is_segment){
    if (tx_window.count(retx.sn)) {
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

  uint32_t head_len  = 0;

  // Need to rebuild the li table & update fi based on so_start and so_end
  if(retx.so_start != 0 && rlc_am_start_aligned(old_header.fi))
    new_header.fi &= RLC_FI_FIELD_NOT_END_ALIGNED;   // segment is start aligned

  uint32_t lower     = 0;
  uint32_t upper     = 0;
  uint32_t li        = 0;

  for(uint32_t i=0; i<old_header.N_li; i++) {
    if(lower >= retx.so_end)
      break;

    upper += old_header.li[i];

    head_len    = rlc_am_packed_length(&new_header);

    if(upper > retx.so_start && lower < retx.so_end) {  // Current SDU is needed
      li = upper - lower;
      if(upper > retx.so_end)
        li -= upper - retx.so_end;
      if(lower < retx.so_start)
        li -= retx.so_start - lower;
      if(lower > 0 && lower == retx.so_start)
        new_header.fi &= RLC_FI_FIELD_NOT_END_ALIGNED;   // segment start is aligned with this SDU
      if(upper == retx.so_end) {
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

  return rlc_am_packed_length(&new_header) + (retx.so_end-retx.so_start);
}

bool rlc_am::retx_queue_has_sn(uint32_t sn)
{
  std::deque<rlc_amd_retx_t>::iterator q_it;
  for(q_it = retx_queue.begin(); q_it != retx_queue.end(); q_it++) {
    if(q_it->sn == sn)
      return true;
  }
  return false;
}

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 36.322 v10.0.0 Section 6.2.1
 ***************************************************************************/

// Read header from pdu struct, don't strip header
void rlc_am_read_data_pdu_header(byte_buffer_t *pdu, rlc_amd_pdu_header_t *header)
{
  uint8_t *ptr = pdu->msg;
  uint32_t n   = 0;
  rlc_am_read_data_pdu_header(&ptr, &n, header);
}

// Read header from raw pointer, strip header
void rlc_am_read_data_pdu_header(uint8_t **payload, uint32_t *nof_bytes, rlc_amd_pdu_header_t *header)
{
  uint8_t  ext;
  uint8_t *ptr = *payload;

  header->dc = (rlc_dc_field_t)((*ptr >> 7) & 0x01);

  if(RLC_DC_FIELD_DATA_PDU == header->dc)
  {
    // Fixed part
    header->rf =                 ((*ptr >> 6) & 0x01);
    header->p  =                 ((*ptr >> 5) & 0x01);
    header->fi = (rlc_fi_field_t)((*ptr >> 3) & 0x03);
    ext        =                 ((*ptr >> 2) & 0x01);
    header->sn =                 (*ptr & 0x03) << 8; // 2 bits SN
    ptr++;
    header->sn |=                (*ptr & 0xFF);     // 8 bits SN
    ptr++;

    if(header->rf)
    {
      header->lsf = ((*ptr >> 7) & 0x01);
      header->so  = (*ptr & 0x7F) << 8; // 7 bits of SO
      ptr++;
      header->so |= (*ptr & 0xFF);      // 8 bits of SO
      ptr++;
    }

    // Extension part
    header->N_li = 0;
    while(ext)
    {
      if(header->N_li%2 == 0)
      {
        ext = ((*ptr >> 7) & 0x01);
        header->li[header->N_li]  = (*ptr & 0x7F) << 4; // 7 bits of LI
        ptr++;
        header->li[header->N_li] |= (*ptr & 0xF0) >> 4; // 4 bits of LI
        header->N_li++;
      }
      else
      {
        ext = (*ptr >> 3) & 0x01;
        header->li[header->N_li] = (*ptr & 0x07) << 8; // 3 bits of LI
        ptr++;
        header->li[header->N_li] |= (*ptr & 0xFF);     // 8 bits of LI
        header->N_li++;
        ptr++;
      }
    }

    // Account for padding if N_li is odd
    if(header->N_li%2 == 1)
      ptr++;

    *nof_bytes -= ptr-*payload;
    *payload    = ptr;
  }
}

// Write header to pdu struct
void rlc_am_write_data_pdu_header(rlc_amd_pdu_header_t *header, byte_buffer_t *pdu)
{
  uint8_t *ptr = pdu->msg;
  rlc_am_write_data_pdu_header(header, &ptr);
  pdu->N_bytes += ptr - pdu->msg;
}

// Write header to pointer & move pointer
void rlc_am_write_data_pdu_header(rlc_amd_pdu_header_t *header, uint8_t **payload)
{
  uint32_t i;
  uint8_t ext = (header->N_li > 0) ? 1 : 0;

  uint8_t *ptr = *payload;

  // Fixed part
  *ptr  = (header->dc & 0x01) << 7;
  *ptr |= (header->rf & 0x01) << 6;
  *ptr |= (header->p  & 0x01) << 5;
  *ptr |= (header->fi & 0x03) << 3;
  *ptr |= (ext        & 0x01) << 2;

  *ptr |= (header->sn & 0x300) >> 8; // 2 bits SN
  ptr++;
  *ptr  = (header->sn & 0xFF);       // 8 bits SN
  ptr++;

  // Segment part
  if(header->rf)
  {
    *ptr  = (header->lsf & 0x01) << 7;
    *ptr |= (header->so  & 0x7F00) >> 8; // 7 bits of SO
    ptr++;
    *ptr = (header->so  & 0x00FF);       // 8 bits of SO
    ptr++;
  }

  // Extension part
  i = 0;
  while(i < header->N_li)
  {
    ext = ((i+1) == header->N_li) ? 0 : 1;
    *ptr  = (ext           &  0x01) << 7; // 1 bit header
    *ptr |= (header->li[i] & 0x7F0) >> 4; // 7 bits of LI
    ptr++;
    *ptr  = (header->li[i] & 0x00F) << 4; // 4 bits of LI
    i++;
    if(i < header->N_li)
    {
      ext = ((i+1) == header->N_li) ? 0 : 1;
      *ptr |= (ext           &  0x01) << 3; // 1 bit header
      *ptr |= (header->li[i] & 0x700) >> 8; // 3 bits of LI
      ptr++;
      *ptr  = (header->li[i] & 0x0FF);      // 8 bits of LI
      ptr++;
      i++;
    }
  }
  // Pad if N_li is odd
  if(header->N_li%2 == 1)
    ptr++;

  *payload = ptr;
}

void rlc_am_read_status_pdu(byte_buffer_t *pdu, rlc_status_pdu_t *status)
{
  rlc_am_read_status_pdu(pdu->msg, pdu->N_bytes, status);
}

void rlc_am_read_status_pdu(uint8_t *payload, uint32_t nof_bytes, rlc_status_pdu_t *status)
{
  uint32_t i;
  uint8_t  ext1, ext2;
  bit_buffer_t tmp;
  uint8_t *ptr = tmp.msg;

  srslte_bit_unpack_vector(payload, tmp.msg, nof_bytes*8);
  tmp.N_bits = nof_bytes*8;

  rlc_dc_field_t dc = (rlc_dc_field_t)srslte_bit_pack(&ptr, 1);

  if(RLC_DC_FIELD_CONTROL_PDU == dc)
  {
    uint8_t cpt = srslte_bit_pack(&ptr, 3); // 3-bit Control PDU Type (0 == status)
    if(0 == cpt)
    {
      status->ack_sn  = srslte_bit_pack(&ptr, 10); // 10 bits ACK_SN
      ext1            = srslte_bit_pack(&ptr, 1);  // 1 bits E1
      status->N_nack  = 0;
      while(ext1)
      {
        status->nacks[status->N_nack].nack_sn = srslte_bit_pack(&ptr, 10);
        ext1 = srslte_bit_pack(&ptr, 1);  // 1 bits E1
        ext2 = srslte_bit_pack(&ptr, 1);  // 1 bits E2
        if(ext2)
        {
          status->nacks[status->N_nack].has_so = true;
          status->nacks[status->N_nack].so_start = srslte_bit_pack(&ptr, 15);
          status->nacks[status->N_nack].so_end   = srslte_bit_pack(&ptr, 15);
        }
        status->N_nack++;
      }
    }
  }
}

void rlc_am_write_status_pdu(rlc_status_pdu_t *status, byte_buffer_t *pdu )
{
  pdu->N_bytes = rlc_am_write_status_pdu(status, pdu->msg);
}

int rlc_am_write_status_pdu(rlc_status_pdu_t *status, uint8_t *payload)
{
  uint32_t i;
  uint8_t ext1;
  bit_buffer_t tmp;
  uint8_t *ptr = tmp.msg;

  srslte_bit_unpack(RLC_DC_FIELD_CONTROL_PDU, &ptr, 1);  // D/C
  srslte_bit_unpack(0,                        &ptr, 3);  // CPT (0 == STATUS)
  srslte_bit_unpack(status->ack_sn,           &ptr, 10); // 10 bit ACK_SN
  ext1 = (status->N_nack == 0) ? 0 : 1;
  srslte_bit_unpack(ext1,                     &ptr, 1);  // E1
  for(i=0;i<status->N_nack;i++)
  {
    srslte_bit_unpack(status->nacks[i].nack_sn, &ptr, 10); // 10 bit NACK_SN
    ext1 = ((status->N_nack-1) == i) ? 0 : 1;
    srslte_bit_unpack(ext1, &ptr, 1);  // E1
    if(status->nacks[i].has_so) {
      srslte_bit_unpack(1 , &ptr, 1);  // E2
      srslte_bit_unpack(status->nacks[i].so_start , &ptr, 15);
      srslte_bit_unpack(status->nacks[i].so_end   , &ptr, 15);
    }else{
      srslte_bit_unpack(0 , &ptr, 1);  // E2
    }
  }

  // Pad
  tmp.N_bits = ptr - tmp.msg;
  uint8_t n_pad = 8 - (tmp.N_bits%8);
  srslte_bit_unpack(0, &ptr, n_pad);
  tmp.N_bits = ptr - tmp.msg;

  // Pack bits
  srslte_bit_pack_vector(tmp.msg, payload, tmp.N_bits);
  return tmp.N_bits/8;
}

uint32_t rlc_am_packed_length(rlc_amd_pdu_header_t *header)
{
  uint32_t len = 2;                 // Fixed part is 2 bytes
  if(header->rf) len += 2;          // Segment header is 2 bytes
  len += header->N_li * 1.5 + 0.5;  // Extension part - integer rounding up
  return len;
}

uint32_t rlc_am_packed_length(rlc_status_pdu_t *status)
{
  uint32_t i;
  uint32_t len_bits = 15;                 // Fixed part is 15 bits
  for(i=0;i<status->N_nack;i++)
  {
    if(status->nacks[i].has_so) {
      len_bits += 42;      // 10 bits SN, 2 bits ext, 15 bits so_start, 15 bits so_end
    }else{
      len_bits += 12;      // 10 bits SN, 2 bits ext
    }
  }

  return (len_bits+7)/8;                  // Convert to bytes - integer rounding up
}

bool rlc_am_is_control_pdu(byte_buffer_t *pdu)
{
  return rlc_am_is_control_pdu(pdu->msg);
}

bool rlc_am_is_control_pdu(uint8_t *payload)
{
  return ((*(payload) >> 7) & 0x01) == RLC_DC_FIELD_CONTROL_PDU;
}

bool rlc_am_is_pdu_segment(uint8_t *payload)
{
  return ((*(payload) >> 6) & 0x01) == 1;
}

std::string rlc_am_to_string(rlc_status_pdu_t *status)
{
  std::stringstream ss;
  ss << "ACK_SN = " << status->ack_sn;
  ss << ", N_nack = " << status->N_nack;
  if(status->N_nack > 0)
  {
    ss << ", NACK_SN = ";
    for(uint32_t i=0; i<status->N_nack; i++)
    {
      if(status->nacks[i].has_so) {
        ss << "[" << status->nacks[i].nack_sn << " " << status->nacks[i].so_start \
           << ":" << status->nacks[i].so_end << "]";
      }else{
        ss << "[" << status->nacks[i].nack_sn << "]";
      }
    }
  }
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

} // namespace srsue
