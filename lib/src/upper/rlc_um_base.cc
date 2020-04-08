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

#include "srslte/upper/rlc_um_base.h"
#include <sstream>

namespace srslte {

rlc_um_base::rlc_um_base(srslte::log_ref            log_,
                         uint32_t                   lcid_,
                         srsue::pdcp_interface_rlc* pdcp_,
                         srsue::rrc_interface_rlc*  rrc_,
                         srslte::timer_handler*     timers_) :
  log(log_),
  lcid(lcid_),
  pdcp(pdcp_),
  rrc(rrc_),
  timers(timers_),
  pool(byte_buffer_pool::get_instance())
{
}

rlc_um_base::~rlc_um_base() {}

void rlc_um_base::stop()
{
  if (tx) {
    tx->stop();
  }

  if (rx) {
    rx->stop();
  }
}

rlc_mode_t rlc_um_base::get_mode()
{
  return rlc_mode_t::um;
}

uint32_t rlc_um_base::get_bearer()
{
  return lcid;
}

bool rlc_um_base::is_mrb()
{
  return cfg.um.is_mrb;
}

void rlc_um_base::reestablish()
{
  tx_enabled = false;

  if (tx) {
    tx->reestablish(); // calls stop and enables tx again
  }

  if (rx) {
    rx->reestablish(); // nothing else needed
  }

  tx_enabled = true;
}

void rlc_um_base::empty_queue()
{
  // Drop all messages in TX SDU queue
  if (tx) {
    tx->empty_queue();
  }
}

/****************************************************************************
 * PDCP interface
 ***************************************************************************/
void rlc_um_base::write_sdu(unique_byte_buffer_t sdu, bool blocking)
{
  if (not tx_enabled || not tx) {
    log->debug("%s is currently deactivated. Dropping SDU (%d B)\n", rb_name.c_str(), sdu->N_bytes);
    metrics.num_dropped_sdus++;
    return;
  }

  if (blocking) {
    tx->write_sdu(std::move(sdu));
  } else {
    tx->try_write_sdu(std::move(sdu));
  }
}

void rlc_um_base::discard_sdu(uint32_t discard_sn)
{
  if (not tx_enabled || not tx) {
    log->debug("%s is currently deactivated. Ignoring SDU discard(SN %u)\n", rb_name.c_str(), discard_sn);
    metrics.num_dropped_sdus++;
    return;
  }
  tx->discard_sdu(discard_sn);
}
/****************************************************************************
 * MAC interface
 ***************************************************************************/

bool rlc_um_base::has_data()
{
  if (tx) {
    return tx->has_data();
  }
  return false;
}

uint32_t rlc_um_base::get_buffer_state()
{
  if (tx) {
    return tx->get_buffer_state();
  }
  return 0;
}

int rlc_um_base::read_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  if (tx && tx_enabled) {
    uint32_t len = tx->build_data_pdu(payload, nof_bytes);
    metrics.num_tx_bytes += len;
    metrics.num_tx_pdus++;
    return len;
  }
  return 0;
}

void rlc_um_base::write_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  if (rx && rx_enabled) {
    metrics.num_rx_pdus++;
    metrics.num_rx_bytes += nof_bytes;
    rx->handle_data_pdu(payload, nof_bytes);
  }
}

rlc_bearer_metrics_t rlc_um_base::get_metrics()
{
  return metrics;
}

void rlc_um_base::reset_metrics()
{
  metrics = {};
}

/****************************************************************************
 * Helper functions
 ***************************************************************************/

std::string rlc_um_base::get_rb_name(srsue::rrc_interface_rlc* rrc, uint32_t lcid, bool is_mrb)
{
  if (is_mrb) {
    std::stringstream ss;
    ss << "MRB" << lcid;
    return ss.str();
  } else {
    return rrc->get_rb_name(lcid);
  }
}

/****************************************************************************
 * Rx subclass implementation (base)
 ***************************************************************************/

rlc_um_base::rlc_um_base_rx::rlc_um_base_rx(rlc_um_base* parent_) :
  pool(parent_->pool),
  log(parent_->log),
  timers(parent_->timers),
  pdcp(parent_->pdcp),
  rrc(parent_->rrc),
  cfg(parent_->cfg),
  metrics(parent_->metrics),
  lcid(parent_->lcid)
{
}

rlc_um_base::rlc_um_base_rx::~rlc_um_base_rx() {}

/****************************************************************************
 * Tx subclass implementation (base)
 ***************************************************************************/

rlc_um_base::rlc_um_base_tx::rlc_um_base_tx(rlc_um_base* parent_) : log(parent_->log), pool(parent_->pool) {}

rlc_um_base::rlc_um_base_tx::~rlc_um_base_tx() {}

void rlc_um_base::rlc_um_base_tx::stop()
{
  empty_queue();
}

void rlc_um_base::rlc_um_base_tx::reestablish()
{
  stop();
  // bearer is enabled in base class
}

void rlc_um_base::rlc_um_base_tx::empty_queue()
{
  std::lock_guard<std::mutex> lock(mutex);

  // deallocate all SDUs in transmit queue
  while (not tx_sdu_queue.is_empty()) {
    unique_byte_buffer_t buf = tx_sdu_queue.read();
  }

  // deallocate SDU that is currently processed
  tx_sdu.reset();
}

bool rlc_um_base::rlc_um_base_tx::has_data()
{
  return (tx_sdu != nullptr || !tx_sdu_queue.is_empty());
}

void rlc_um_base::rlc_um_base_tx::write_sdu(unique_byte_buffer_t sdu)
{
  if (sdu) {
    log->info_hex(sdu->msg,
                  sdu->N_bytes,
                  "%s Tx SDU (%d B, tx_sdu_queue_len=%d)",
                  rb_name.c_str(),
                  sdu->N_bytes,
                  tx_sdu_queue.size());
    tx_sdu_queue.write(std::move(sdu));
  } else {
    log->warning("NULL SDU pointer in write_sdu()\n");
  }
}

void rlc_um_base::rlc_um_base_tx::try_write_sdu(unique_byte_buffer_t sdu)
{
  if (sdu) {
    uint8_t*                              msg_ptr   = sdu->msg;
    uint32_t                              nof_bytes = sdu->N_bytes;
    std::pair<bool, unique_byte_buffer_t> ret       = tx_sdu_queue.try_write(std::move(sdu));
    if (ret.first) {
      log->info_hex(
          msg_ptr, nof_bytes, "%s Tx SDU (%d B, tx_sdu_queue_len=%d)", rb_name.c_str(), nof_bytes, tx_sdu_queue.size());
    } else {
      log->info_hex(ret.second->msg,
                    ret.second->N_bytes,
                    "[Dropped SDU] %s Tx SDU (%d B, tx_sdu_queue_len=%d)",
                    rb_name.c_str(),
                    ret.second->N_bytes,
                    tx_sdu_queue.size());
    }
  } else {
    log->warning("NULL SDU pointer in write_sdu()\n");
  }
}

void rlc_um_base::rlc_um_base_tx::discard_sdu(uint32_t discard_sn)
{
  log->warning("RLC UM: Discard SDU not implemented yet.\n");
}

int rlc_um_base::rlc_um_base_tx::build_data_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  unique_byte_buffer_t pdu;
  {
    std::lock_guard<std::mutex> lock(mutex);
    log->debug("MAC opportunity - %d bytes\n", nof_bytes);

    if (!tx_sdu && tx_sdu_queue.size() == 0) {
      log->info("No data available to be sent\n");
      return 0;
    }

    pdu = allocate_unique_buffer(*pool);
    if (!pdu || pdu->N_bytes != 0) {
      log->error("Failed to allocate PDU buffer\n");
      return 0;
    }
  }
  return build_data_pdu(std::move(pdu), payload, nof_bytes);
}

} // namespace srslte
