/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srslte/upper/rlc_tm.h"

namespace srslte {

rlc_tm::rlc_tm(uint32_t queue_len) :
  ul_queue(queue_len),
  tx_enabled(false),
  log(NULL),
  pdcp(NULL),
  rrc(NULL),
  lcid(0),
  num_tx_bytes(0),
  num_rx_bytes(0)
{
  pool = byte_buffer_pool::get_instance();
}

// Warning: must call stop() to properly deallocate all buffers
rlc_tm::~rlc_tm() {
  pool = NULL;
}

void rlc_tm::init(srslte::log               *log_,
                  uint32_t                   lcid_,
                  srsue::pdcp_interface_rlc *pdcp_,
                  srsue::rrc_interface_rlc  *rrc_, 
                  mac_interface_timers      *mac_timers)
{
  log  = log_;
  lcid = lcid_;
  pdcp = pdcp_;
  rrc  = rrc_;
  tx_enabled = true;
}

bool rlc_tm::configure(srslte_rlc_config_t cnfg)
{
  log->error("Attempted to configure TM RLC entity\n");
  return true;
}

void rlc_tm::empty_queue()
{
  // Drop all messages in TX queue
  unique_byte_buffer buf;
  while (ul_queue.try_read(&buf)) {
  }
  ul_queue.reset();
}

void rlc_tm::reestablish() {
  stop();
  tx_enabled = true;
}

void rlc_tm::stop()
{
  tx_enabled = false;
  empty_queue();
}

rlc_mode_t rlc_tm::get_mode()
{
  return RLC_MODE_TM;
}

uint32_t rlc_tm::get_bearer()
{
  return lcid;
}

// PDCP interface
void rlc_tm::write_sdu(unique_byte_buffer sdu, bool blocking)
{
  if (!tx_enabled) {
    return;
  }
  if (sdu) {
    if (blocking) {
      log->info_hex(sdu->msg, sdu->N_bytes, "%s Tx SDU, queue size=%d, bytes=%d",
                    rrc->get_rb_name(lcid).c_str(), ul_queue.size(), ul_queue.size_bytes());
      ul_queue.write(std::move(sdu));
    } else {
      uint8_t* msg_ptr   = sdu->msg;
      uint32_t nof_bytes = sdu->N_bytes;
      if (ul_queue.try_write(std::move(sdu))) {
        log->info_hex(msg_ptr,
                      nof_bytes,
                      "%s Tx SDU, queue size=%d, bytes=%d",
                      rrc->get_rb_name(lcid).c_str(),
                      ul_queue.size(),
                      ul_queue.size_bytes());
      } else {
#warning Find a more elegant solution - the msg was already deallocated at this point
        log->info("[Dropped SDU] %s Tx SDU, queue size=%d, bytes=%d",
                  rrc->get_rb_name(lcid).c_str(),
                  ul_queue.size(),
                  ul_queue.size());
        //        log->info_hex(sdu->msg, sdu->N_bytes, "[Dropped SDU] %s Tx SDU, queue size=%d, bytes=%d",
        //                       rrc->get_rb_name(lcid).c_str(), ul_queue.size(), ul_queue.size_bytes());
      }
    }
  } else {
    log->warning("NULL SDU pointer in write_sdu()\n");
  }
}

// MAC interface
bool rlc_tm::has_data()
{
  return not ul_queue.is_empty();
}

uint32_t rlc_tm::get_buffer_state()
{
  return ul_queue.size_bytes();
}

uint32_t rlc_tm::get_num_tx_bytes()
{
  return num_tx_bytes;
}

uint32_t rlc_tm::get_num_rx_bytes()
{
  return num_rx_bytes;
}

void rlc_tm::reset_metrics()
{
  num_tx_bytes = 0;
  num_rx_bytes = 0;
}

int rlc_tm::read_pdu(uint8_t *payload, uint32_t nof_bytes)
{
  uint32_t pdu_size = ul_queue.size_tail_bytes();
  if (pdu_size > nof_bytes) {
    log->error("TX %s PDU size larger than MAC opportunity (%d > %d)\n", rrc->get_rb_name(lcid).c_str(), pdu_size, nof_bytes);
    return -1;
  }
  unique_byte_buffer buf;
  if (ul_queue.try_read(&buf)) {
    pdu_size = buf->N_bytes;
    memcpy(payload, buf->msg, buf->N_bytes);
    log->debug("%s Complete SDU scheduled for tx. Stack latency: %ld us\n",
               rrc->get_rb_name(lcid).c_str(), buf->get_latency_us());
    log->info_hex(payload, pdu_size, "TX %s, %s PDU, queue size=%d, bytes=%d",
                  rrc->get_rb_name(lcid).c_str(), rlc_mode_text[RLC_MODE_TM], ul_queue.size(), ul_queue.size_bytes());

    num_tx_bytes += pdu_size;
    return pdu_size;
  } else {
    log->warning("Queue empty while trying to read\n");
    if (ul_queue.size_bytes() > 0) {
      log->warning("Corrupted queue: empty but size_bytes > 0. Resetting queue\n");
      ul_queue.reset();
    }
    return 0;
  }
}

void rlc_tm::write_pdu(uint8_t *payload, uint32_t nof_bytes)
{
  unique_byte_buffer buf = allocate_unique_buffer(*pool);
  if (buf) {
    memcpy(buf->msg, payload, nof_bytes);
    buf->N_bytes = nof_bytes;
    buf->set_timestamp();
    num_rx_bytes += nof_bytes;
    pdcp->write_pdu(lcid, std::move(buf));
  } else {
    log->error("Fatal Error: Couldn't allocate buffer in rlc_tm::write_pdu().\n");
  }
}

} // namespace srsue
