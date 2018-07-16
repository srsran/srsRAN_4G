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


#include "srslte/upper/rlc_um.h"
#include <sstream>
#include <srslte/upper/rlc_interface.h>

#define RX_MOD_BASE(x) (x-vr_uh-cfg.rx_window_size)%cfg.rx_mod

namespace srslte {

rlc_um::rlc_um(uint32_t queue_len) : tx_sdu_queue(queue_len)
{
  log = NULL;
  pdcp = NULL;
  rrc = NULL;
  reordering_timer = NULL;
  lcid = 0;
  reordering_timer_id = 0;
  bzero(&cfg, sizeof(srslte_rlc_um_config_t));

  tx_sdu = NULL;

  rx_sdu = NULL;
  pool = byte_buffer_pool::get_instance();

  pthread_mutex_init(&mutex, NULL);
  
  vt_us    = 0;
  vr_ur    = 0;
  vr_ux    = 0;
  vr_uh    = 0;
  
  vr_ur_in_rx_sdu = 0; 
  
  mac_timers = NULL;

  pdu_lost = false;
}

// Warning: must call stop() to properly deallocate all buffers
rlc_um::~rlc_um()
{
  pthread_mutex_destroy(&mutex);
  pool = NULL;
  if (mac_timers && reordering_timer) {
    mac_timers->timer_release_id(reordering_timer_id);
    reordering_timer = NULL;
  }
}

void rlc_um::init(srslte::log                  *log_,
                  uint32_t                      lcid_,
                  srsue::pdcp_interface_rlc    *pdcp_,
                  srsue::rrc_interface_rlc     *rrc_,
                  srslte::mac_interface_timers *mac_timers_)
{
  log                   = log_;
  lcid                  = lcid_;
  pdcp                  = pdcp_;
  rrc                   = rrc_;
  mac_timers            = mac_timers_;
  reordering_timer_id   = mac_timers->timer_get_unique_id();
  reordering_timer      = mac_timers->timer_get(reordering_timer_id);
  tx_enabled = true;
}

void rlc_um::configure(srslte_rlc_config_t cnfg_)
{
  cfg = cnfg_.um;
  if(cnfg_.um.is_mrb){
    tx_sdu_queue.resize(512);
  }
  switch(cnfg_.rlc_mode)
  {
  case LIBLTE_RRC_RLC_MODE_UM_BI:
    log->warning("%s configured in %s mode: "
              "t_reordering=%d ms, rx_sn_field_length=%u bits, tx_sn_field_length=%u bits\n",
              rb_name().c_str(), liblte_rrc_rlc_mode_text[cnfg_.rlc_mode],
              cfg.t_reordering, rlc_umd_sn_size_num[cfg.rx_sn_field_length], rlc_umd_sn_size_num[cfg.rx_sn_field_length]);
    break;
  case LIBLTE_RRC_RLC_MODE_UM_UNI_UL:
    log->warning("%s configured in %s mode: tx_sn_field_length=%u bits\n",
              rrc->get_rb_name(lcid).c_str(), liblte_rrc_rlc_mode_text[cnfg_.rlc_mode],

              rlc_umd_sn_size_num[cfg.rx_sn_field_length]);
    break;
  case LIBLTE_RRC_RLC_MODE_UM_UNI_DL:
    log->warning("%s configured in %s mode: "
              "t_reordering=%d ms, rx_sn_field_length=%u bits\n",
              rb_name().c_str(), liblte_rrc_rlc_mode_text[cnfg_.rlc_mode],
              cfg.t_reordering, rlc_umd_sn_size_num[cfg.rx_sn_field_length]);
    break;
  default:
    log->error("RLC configuration mode not recognized\n");
  }
}

void rlc_um::empty_queue() {
  // Drop all messages in TX SDU queue
  byte_buffer_t *buf;
  while(tx_sdu_queue.try_read(&buf)) {
    pool->deallocate(buf);
  }
  tx_sdu_queue.reset();
}

bool rlc_um::is_mrb()
{
  return cfg.is_mrb;
}

void rlc_um::reestablish() {
  stop();
  tx_enabled = true;
}

void rlc_um::stop()
{
  // Empty tx_sdu_queue before locking the mutex
  tx_enabled = false;
  empty_queue();

  pthread_mutex_lock(&mutex);
  vt_us    = 0;
  vr_ur    = 0;
  vr_ux    = 0;
  vr_uh    = 0;
  pdu_lost = false;
  if(rx_sdu) {
    pool->deallocate(rx_sdu);
    rx_sdu = NULL;
  }

  if(tx_sdu) {
    pool->deallocate(tx_sdu);
    tx_sdu = NULL;
  }

  if(reordering_timer) {
    reordering_timer->stop();
  }

  // Drop all messages in RX window
  std::map<uint32_t, rlc_umd_pdu_t>::iterator it;
  for(it = rx_window.begin(); it != rx_window.end(); it++) {
    pool->deallocate(it->second.buf);
  }
  rx_window.clear();
  pthread_mutex_unlock(&mutex);

}

rlc_mode_t rlc_um::get_mode()
{
  return RLC_MODE_UM;
}

uint32_t rlc_um::get_bearer()
{
  return lcid;
}

/****************************************************************************
 * PDCP interface
 ***************************************************************************/
void rlc_um::write_sdu(byte_buffer_t *sdu)
{
  if (!tx_enabled) {
    byte_buffer_pool::get_instance()->deallocate(sdu);
    return;
  }
  if (sdu) {
    tx_sdu_queue.write(sdu);
    log->info_hex(sdu->msg, sdu->N_bytes, "%s Tx SDU (%d B ,tx_sdu_queue_len=%d)", rrc->get_rb_name(lcid).c_str(), sdu->N_bytes, tx_sdu_queue.size());
  } else {
    log->warning("NULL SDU pointer in write_sdu()\n");
  }
}

void rlc_um::write_sdu_nb(byte_buffer_t *sdu)
{
  if (!tx_enabled) {
    byte_buffer_pool::get_instance()->deallocate(sdu);
    return;
  }
  if (sdu) {
    if (tx_sdu_queue.try_write(sdu)) {
      log->info_hex(sdu->msg, sdu->N_bytes, "%s Tx SDU (%d B,tx_sdu_queue_len=%d)", rrc->get_rb_name(lcid).c_str(), sdu->N_bytes, tx_sdu_queue.size());
    } else {
      log->debug_hex(sdu->msg, sdu->N_bytes, "[Dropped SDU] %s Tx SDU (%d B,tx_sdu_queue_len=%d)", rrc->get_rb_name(lcid).c_str(), sdu->N_bytes, tx_sdu_queue.size());
      pool->deallocate(sdu);
    }
  } else {
    log->warning("NULL SDU pointer in write_sdu()\n");
  }
}

/****************************************************************************
 * MAC interface
 ***************************************************************************/

uint32_t rlc_um::get_buffer_state()
{
  // Bytes needed for tx SDUs
  uint32_t n_sdus  = tx_sdu_queue.size();

  uint32_t n_bytes = tx_sdu_queue.size_bytes();
  if(tx_sdu)
  {
    n_sdus++;
    n_bytes += tx_sdu->N_bytes;
  }

  // Room needed for header extensions? (integer rounding)
  if(n_sdus > 1)
    n_bytes += ((n_sdus-1)*1.5)+0.5;

  // Room needed for fixed header?
  if(n_bytes > 0)
    n_bytes += (cfg.is_mrb)?2:3;

  return n_bytes;
}

uint32_t rlc_um::get_total_buffer_state()
{
  return get_buffer_state();
}

int rlc_um::read_pdu(uint8_t *payload, uint32_t nof_bytes)
{
  int r;
  log->debug("MAC opportunity - %d bytes\n", nof_bytes);
  pthread_mutex_lock(&mutex);
  r = build_data_pdu(payload, nof_bytes);
  pthread_mutex_unlock(&mutex);
  return r; 
}

void rlc_um::write_pdu(uint8_t *payload, uint32_t nof_bytes)
{
  pthread_mutex_lock(&mutex);
  handle_data_pdu(payload, nof_bytes);
  pthread_mutex_unlock(&mutex);
}

/****************************************************************************
 * Timeout callback interface
 ***************************************************************************/

void rlc_um::timer_expired(uint32_t timeout_id)
{
  if(reordering_timer_id == timeout_id)
  {
    pthread_mutex_lock(&mutex);

    // 36.322 v10 Section 5.1.2.2.4
    log->info("%s reordering timeout expiry - updating vr_ur and reassembling\n",
               rb_name().c_str());

    log->warning("Lost PDU SN: %d\n", vr_ur);
    pdu_lost = true;
    rx_sdu->reset();
    while(RX_MOD_BASE(vr_ur) < RX_MOD_BASE(vr_ux))
    {
      vr_ur = (vr_ur + 1)%cfg.rx_mod;
      log->debug("Entering Reassemble from timeout id=%d\n", timeout_id);
      reassemble_rx_sdus();
      log->debug("Finished reassemble from timeout id=%d\n", timeout_id);
    }
    reordering_timer->stop();
    if(RX_MOD_BASE(vr_uh) > RX_MOD_BASE(vr_ur))
    {
      reordering_timer->set(this, cfg.t_reordering);
      reordering_timer->run();
      vr_ux = vr_uh;
    }

    debug_state();
    pthread_mutex_unlock(&mutex);
  }
}

bool rlc_um::reordering_timeout_running()
{
  return reordering_timer->is_running();
}

/****************************************************************************
 * Helpers
 ***************************************************************************/

int  rlc_um::build_data_pdu(uint8_t *payload, uint32_t nof_bytes)
{
  if(!tx_sdu && tx_sdu_queue.size() == 0)
  {
    log->info("No data available to be sent\n");
    return 0;
  }

  byte_buffer_t *pdu = pool_allocate;
  if(!pdu || pdu->N_bytes != 0)
  {
    log->error("Failed to allocate PDU buffer\n");
    return -1;
  }
  rlc_umd_pdu_header_t header;
  header.fi   = RLC_FI_FIELD_START_AND_END_ALIGNED;
  header.sn   = vt_us;
  header.N_li = 0;
  header.sn_size = cfg.tx_sn_field_length;

  uint32_t to_move   = 0;
  uint32_t last_li   = 0;
  uint8_t *pdu_ptr   = pdu->msg;

  int head_len  = rlc_um_packed_length(&header);
  int pdu_space = nof_bytes;

  if(pdu_space <= head_len + 1)
  {
    pool->deallocate(pdu);
    log->warning("%s Cannot build a PDU - %d bytes available, %d bytes required for header\n",
                 rb_name().c_str(), nof_bytes, head_len);
    return 0;
  }

  // Check for SDU segment
  if(tx_sdu)
  {
    uint32_t space = pdu_space-head_len;
    to_move = space >= tx_sdu->N_bytes ? tx_sdu->N_bytes : space;
    log->debug("%s adding remainder of SDU segment - %d bytes of %d remaining\n",
               rb_name().c_str(), to_move, tx_sdu->N_bytes);
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
    pdu_space -= to_move;
    header.fi |= RLC_FI_FIELD_NOT_START_ALIGNED; // First byte does not correspond to first byte of SDU
  }

  // Pull SDUs from queue
  while(pdu_space > head_len + 1 && tx_sdu_queue.size() > 0)
  {
    log->debug("pdu_space=%d, head_len=%d\n", pdu_space, head_len);
    if(last_li > 0)
      header.li[header.N_li++] = last_li;
    head_len = rlc_um_packed_length(&header);
    tx_sdu_queue.read(&tx_sdu);
    uint32_t space = pdu_space-head_len;
    to_move = space >= tx_sdu->N_bytes ? tx_sdu->N_bytes : space;
    log->debug("%s adding new SDU segment - %d bytes of %d remaining\n",
               rb_name().c_str(), to_move, tx_sdu->N_bytes);
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
    pdu_space -= to_move;
  }

  if(tx_sdu)
    header.fi |= RLC_FI_FIELD_NOT_END_ALIGNED; // Last byte does not correspond to last byte of SDU

  // Set SN

  header.sn = vt_us;
  vt_us = (vt_us + 1)%cfg.tx_mod;

  // Add header and TX
  log->debug("%s packing PDU with length %d\n", rb_name().c_str(), pdu->N_bytes);
  rlc_um_write_data_pdu_header(&header, pdu);
  memcpy(payload, pdu->msg, pdu->N_bytes);
  uint32_t ret = pdu->N_bytes;

  log->debug("%s returning length %d\n", rrc->get_rb_name(lcid).c_str(), pdu->N_bytes);
  pool->deallocate(pdu);

  debug_state();
  return ret;
}

void rlc_um::handle_data_pdu(uint8_t *payload, uint32_t nof_bytes)
{
  std::map<uint32_t, rlc_umd_pdu_t>::iterator it;
  rlc_umd_pdu_header_t header;
  rlc_um_read_data_pdu_header(payload, nof_bytes, cfg.rx_sn_field_length, &header);

  log->info_hex(payload, nof_bytes, "RX %s Rx data PDU SN: %d",
                rb_name().c_str(), header.sn);

  if(RX_MOD_BASE(header.sn) >= RX_MOD_BASE(vr_uh-cfg.rx_window_size) &&
     RX_MOD_BASE(header.sn) <  RX_MOD_BASE(vr_ur))
  {
    log->info("%s SN: %d outside rx window [%d:%d] - discarding\n",
              rb_name().c_str(), header.sn, vr_ur, vr_uh);
    return;
  }
  it = rx_window.find(header.sn);
  if(rx_window.end() != it)
  {
    log->info("%s Discarding duplicate SN: %d\n",
              rb_name().c_str(), header.sn);
    return;
  }

  // Write to rx window
  rlc_umd_pdu_t pdu;
  pdu.buf = pool_allocate;
  if (!pdu.buf) {
    log->error("Discarting packet: no space in buffer pool\n");
    return;
  }
  memcpy(pdu.buf->msg, payload, nof_bytes);
  pdu.buf->N_bytes = nof_bytes;
  //Strip header from PDU
  int header_len = rlc_um_packed_length(&header);
  pdu.buf->msg += header_len;
  pdu.buf->N_bytes -= header_len;
  pdu.header = header;
  rx_window[header.sn] = pdu;
  
  // Update vr_uh
  if(!inside_reordering_window(header.sn))
    vr_uh  = (header.sn + 1)%cfg.rx_mod;

  // Reassemble and deliver SDUs, while updating vr_ur
  log->debug("Entering Reassemble from received PDU\n");
  reassemble_rx_sdus();
  log->debug("Finished reassemble from received PDU\n");
  
  // Update reordering variables and timers
  if(reordering_timer->is_running())
  {
    if(RX_MOD_BASE(vr_ux) <= RX_MOD_BASE(vr_ur) ||
       (!inside_reordering_window(vr_ux) && vr_ux != vr_uh))
    {
      reordering_timer->stop();
    }
  }
  if(!reordering_timer->is_running())
  {
    if(RX_MOD_BASE(vr_uh) > RX_MOD_BASE(vr_ur))
    {
      reordering_timer->set(this, cfg.t_reordering);
      reordering_timer->run();
      vr_ux = vr_uh;
    }
  }

  debug_state();
}

void rlc_um::reassemble_rx_sdus()
{
  if(!rx_sdu) {
    rx_sdu = pool_allocate;
    if (!rx_sdu) {
      log->error("Fatal Error: Couldn't allocate buffer in rlc_um::reassemble_rx_sdus().\n");
      return;
    }
  }

  // First catch up with lower edge of reordering window
  while(!inside_reordering_window(vr_ur))
  {
    if(rx_window.end() == rx_window.find(vr_ur))
    {
      rx_sdu->reset();
    }else{
      // Handle any SDU segments
      for(uint32_t i=0; i<rx_window[vr_ur].header.N_li; i++)
      {
        int len = rx_window[vr_ur].header.li[i];

        // Check if we received a middle or end segment
        if (rx_sdu->N_bytes == 0 && i == 0 && !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
          log->warning("Dropping PDU %d due to lost start segment\n", vr_ur);
          // Advance data pointers and continue with next segment
          rx_window[vr_ur].buf->msg += len;
          rx_window[vr_ur].buf->N_bytes -= len;
          rx_sdu->reset();
          break;
        }

        memcpy(&rx_sdu->msg[rx_sdu->N_bytes], rx_window[vr_ur].buf->msg, len);
        rx_sdu->N_bytes += len;
        rx_window[vr_ur].buf->msg += len;
        rx_window[vr_ur].buf->N_bytes -= len;
        if((pdu_lost && !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) || (vr_ur != ((vr_ur_in_rx_sdu+1)%cfg.rx_mod))) {
          log->warning("Dropping remainder of lost PDU (lower edge middle segments, vr_ur=%d, vr_ur_in_rx_sdu=%d)\n", vr_ur, vr_ur_in_rx_sdu);
          rx_sdu->reset();
        } else {
          log->info_hex(rx_sdu->msg, rx_sdu->N_bytes, "%s Rx SDU vr_ur=%d, i=%d (lower edge middle segments)", rb_name().c_str(), vr_ur, i);
          rx_sdu->set_timestamp();
          if(cfg.is_mrb){
            pdcp->write_pdu_mch(lcid, rx_sdu);
          } else {
            pdcp->write_pdu(lcid, rx_sdu);
          }
          rx_sdu = pool_allocate;
          if (!rx_sdu) {
            log->error("Fatal Error: Couldn't allocate buffer in rlc_um::reassemble_rx_sdus().\n");
            return;
          }
        }
        pdu_lost = false;
      }

      // Handle last segment

      if (rx_sdu->N_bytes > 0 || rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
        log->debug("Writing last segment in SDU buffer. Lower edge vr_ur=%d, Buffer size=%d, segment size=%d\n",
                   vr_ur, rx_sdu->N_bytes, rx_window[vr_ur].buf->N_bytes);

        memcpy(&rx_sdu->msg[rx_sdu->N_bytes], rx_window[vr_ur].buf->msg, rx_window[vr_ur].buf->N_bytes);
        rx_sdu->N_bytes += rx_window[vr_ur].buf->N_bytes;
        vr_ur_in_rx_sdu = vr_ur;
        if(rlc_um_end_aligned(rx_window[vr_ur].header.fi))
        {
          if(pdu_lost && !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
            log->warning("Dropping remainder of lost PDU (lower edge last segments)\n");
            rx_sdu->reset();
          } else {
            log->info_hex(rx_sdu->msg, rx_sdu->N_bytes, "%s Rx SDU vr_ur=%d (lower edge last segments)", rrc->get_rb_name(lcid).c_str(), vr_ur);
            rx_sdu->set_timestamp();
            if(cfg.is_mrb){
              pdcp->write_pdu_mch(lcid, rx_sdu);
            } else {
              pdcp->write_pdu(lcid, rx_sdu);
            }
            rx_sdu = pool_allocate;
            if (!rx_sdu) {
              log->error("Fatal Error: Couldn't allocate buffer in rlc_um::reassemble_rx_sdus().\n");
              return;
            }
          }
          pdu_lost = false;
        }
      }

      // Clean up rx_window
      pool->deallocate(rx_window[vr_ur].buf);
      rx_window.erase(vr_ur);
    }

    vr_ur = (vr_ur + 1)%cfg.rx_mod;
  }


  // Now update vr_ur until we reach an SN we haven't yet received
  while(rx_window.end() != rx_window.find(vr_ur))
  {
    // Handle any SDU segments
    for(uint32_t i=0; i<rx_window[vr_ur].header.N_li; i++)
    {
      int len = rx_window[vr_ur].header.li[i];

      // Check if the first part of the PDU is a middle or end segment
      if (rx_sdu->N_bytes == 0 && i == 0 && !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
        log->warning("Dropping PDU %d due to lost start segment\n", vr_ur);
        // Advance data pointers and continue with next segment
        rx_window[vr_ur].buf->msg += len;
        rx_window[vr_ur].buf->N_bytes -= len;
        rx_sdu->reset();
        break;
      }

      // Check available space in SDU
      if ((uint32_t)len > rx_sdu->get_tailroom()) {
        log->error("Dropping PDU %d due to buffer mis-alignment (current segment len %d B, received %d B)\n", vr_ur, rx_sdu->N_bytes, len);
        rx_sdu->reset();
        goto clean_up_rx_window;
      }

      log->debug("Concatenating %d bytes in to current length %d. rx_window remaining bytes=%d, vr_ur_in_rx_sdu=%d, vr_ur=%d, rx_mod=%d, last_mod=%d\n",
        len, rx_sdu->N_bytes, rx_window[vr_ur].buf->N_bytes, vr_ur_in_rx_sdu, vr_ur, cfg.rx_mod, (vr_ur_in_rx_sdu+1)%cfg.rx_mod);
      memcpy(&rx_sdu->msg[rx_sdu->N_bytes], rx_window[vr_ur].buf->msg, len);
      rx_sdu->N_bytes += len;
      rx_window[vr_ur].buf->msg += len;
      rx_window[vr_ur].buf->N_bytes -= len;
      if((pdu_lost && !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) || (vr_ur != ((vr_ur_in_rx_sdu+1)%cfg.rx_mod))) {
        log->warning("Dropping remainder of lost PDU (update vr_ur middle segments, vr_ur=%d, vr_ur_in_rx_sdu=%d)\n", vr_ur, vr_ur_in_rx_sdu);
        rx_sdu->reset();
      } else {
        log->info_hex(rx_sdu->msg, rx_sdu->N_bytes, "%s Rx SDU vr_ur=%d, i=%d, (update vr_ur middle segments)", rb_name().c_str(), vr_ur, i);
        rx_sdu->set_timestamp();
        if(cfg.is_mrb){
          pdcp->write_pdu_mch(lcid, rx_sdu);
        } else {
          pdcp->write_pdu(lcid, rx_sdu);
        }
        rx_sdu = pool_allocate;
        if (!rx_sdu) {
          log->error("Fatal Error: Couldn't allocate buffer in rlc_um::reassemble_rx_sdus().\n");
          return;
        }
      }
      pdu_lost = false;
    }

    // Handle last segment
    if (rx_sdu->N_bytes == 0 && rx_window[vr_ur].header.N_li == 0 && !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
      log->warning("Dropping PDU %d due to lost start segment\n", vr_ur);
      rx_sdu->reset();
      goto clean_up_rx_window;
    }

    if (rx_sdu->N_bytes                                 < SRSLTE_MAX_BUFFER_SIZE_BYTES   &&
        rx_window[vr_ur].buf->N_bytes                   < SRSLTE_MAX_BUFFER_SIZE_BYTES   &&
        rx_window[vr_ur].buf->N_bytes + rx_sdu->N_bytes < SRSLTE_MAX_BUFFER_SIZE_BYTES)
    {

      memcpy(&rx_sdu->msg[rx_sdu->N_bytes], rx_window[vr_ur].buf->msg, rx_window[vr_ur].buf->N_bytes);
      rx_sdu->N_bytes += rx_window[vr_ur].buf->N_bytes;
      log->debug("Writing last segment in SDU buffer. Updating vr_ur=%d, Buffer size=%d, segment size=%d\n",
                 vr_ur, rx_sdu->N_bytes, rx_window[vr_ur].buf->N_bytes);
    } else {
      log->error("Out of bounds while reassembling SDU buffer in UM: sdu_len=%d, window_buffer_len=%d, vr_ur=%d\n",
                 rx_sdu->N_bytes, rx_window[vr_ur].buf->N_bytes, vr_ur);
    }
    vr_ur_in_rx_sdu = vr_ur;
    if(rlc_um_end_aligned(rx_window[vr_ur].header.fi))
    {
      if(pdu_lost && !rlc_um_start_aligned(rx_window[vr_ur].header.fi)) {
        log->warning("Dropping remainder of lost PDU (update vr_ur last segments)\n");
        rx_sdu->reset();
      } else {
        log->info_hex(rx_sdu->msg, rx_sdu->N_bytes, "%s Rx SDU vr_ur=%d (update vr_ur last segments)", rb_name().c_str(), vr_ur);
        rx_sdu->set_timestamp();
        if(cfg.is_mrb){
          pdcp->write_pdu_mch(lcid, rx_sdu);
        } else {
          pdcp->write_pdu(lcid, rx_sdu);
        }
        rx_sdu = pool_allocate;
        if (!rx_sdu) {
          log->error("Fatal Error: Couldn't allocate buffer in rlc_um::reassemble_rx_sdus().\n");
          return;
        }
      }
      pdu_lost = false;
    }

clean_up_rx_window:

    // Clean up rx_window
    pool->deallocate(rx_window[vr_ur].buf);
    rx_window.erase(vr_ur);

    vr_ur = (vr_ur + 1)%cfg.rx_mod;
  }
}

bool rlc_um::inside_reordering_window(uint16_t sn)
{
  if(cfg.rx_window_size == 0) {
    return true;
  }
  if(RX_MOD_BASE(sn) >= RX_MOD_BASE(vr_uh-cfg.rx_window_size) &&
     RX_MOD_BASE(sn) <  RX_MOD_BASE(vr_uh))
  {
    return true;
  }else{
    return false;
  }
}

void rlc_um::debug_state()
{
  log->debug("%s vt_us = %d, vr_ur = %d, vr_ux = %d, vr_uh = %d \n",
             rb_name().c_str(), vt_us, vr_ur, vr_ux, vr_uh);

}

std::string rlc_um::rb_name() {
  if(cfg.is_mrb) {
    std::stringstream ss;
    ss << "MRB" << lcid;
    return ss.str();
  } else {
    return rrc->get_rb_name(lcid);
  }
}

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 36.322 v10.0.0 Section 6.2.1
 ***************************************************************************/

void rlc_um_read_data_pdu_header(byte_buffer_t *pdu, rlc_umd_sn_size_t sn_size, rlc_umd_pdu_header_t *header)
{
  rlc_um_read_data_pdu_header(pdu->msg, pdu->N_bytes, sn_size, header);
}

void rlc_um_read_data_pdu_header(uint8_t *payload, uint32_t nof_bytes, rlc_umd_sn_size_t sn_size, rlc_umd_pdu_header_t *header)
{
  uint8_t  ext;
  uint8_t *ptr = payload;
  // Fixed part
  if(RLC_UMD_SN_SIZE_5_BITS == sn_size)
  {
    header->fi = (rlc_fi_field_t)((*ptr >> 6) & 0x03);  // 2 bits FI
    ext        =                 ((*ptr >> 5) & 0x01);  // 1 bit EXT
    header->sn =                 *ptr & 0x1F;           // 5 bits SN
    ptr++;
  }else{
    header->fi = (rlc_fi_field_t)((*ptr >> 3) & 0x03);  // 2 bits FI
    ext        =                 ((*ptr >> 2) & 0x01);  // 1 bit EXT
    header->sn =                 (*ptr & 0x03) << 8;    // 2 bits SN
    ptr++;
    header->sn |=                (*ptr & 0xFF);         // 8 bits SN
    ptr++;
  }

  header->sn_size = sn_size;

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
}

void rlc_um_write_data_pdu_header(rlc_umd_pdu_header_t *header, byte_buffer_t *pdu)
{
  uint32_t i;
  uint8_t ext = (header->N_li > 0) ? 1 : 0;
  // Make room for the header
  uint32_t len = rlc_um_packed_length(header);
  pdu->msg -= len;
  uint8_t *ptr = pdu->msg;

  // Fixed part
  if(RLC_UMD_SN_SIZE_5_BITS == header->sn_size)
  {
    *ptr  = (header->fi & 0x03) << 6;   // 2 bits FI
    *ptr |= (ext        & 0x01) << 5;   // 1 bit EXT
    *ptr |= header->sn  & 0x1F;         // 5 bits SN
    ptr++;
  }else{
    *ptr  = (header->fi & 0x03) << 3;   // 3 Reserved bits | 2 bits FI
    *ptr |= (ext        & 0x01) << 2;   // 1 bit EXT
    *ptr |= (header->sn & 0x300) >> 8;  // 2 bits SN
    ptr++;
    *ptr  = (header->sn & 0xFF);        // 8 bits SN
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

  pdu->N_bytes += ptr-pdu->msg;
}

uint32_t rlc_um_packed_length(rlc_umd_pdu_header_t *header)
{
  uint32_t len = 0;
  if(RLC_UMD_SN_SIZE_5_BITS == header->sn_size)
  {
    len += 1; // Fixed part is 1 byte
  }else{
    len += 2; // Fixed part is 2 bytes
  }
  len += header->N_li * 1.5 + 0.5;  // Extension part - integer rounding up
  return len;
}

bool rlc_um_start_aligned(uint8_t fi)
{
  return (fi == RLC_FI_FIELD_START_AND_END_ALIGNED || fi == RLC_FI_FIELD_NOT_END_ALIGNED);
}

bool rlc_um_end_aligned(uint8_t fi)
{
  return (fi == RLC_FI_FIELD_START_AND_END_ALIGNED || fi == RLC_FI_FIELD_NOT_START_ALIGNED);
}

} // namespace srsue
