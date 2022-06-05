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

#include "srsran/rlc/rlc_am_base.h"
#include "srsran/rlc/rlc_am_lte.h"
#include "srsran/rlc/rlc_am_nr.h"
#include <sstream>

namespace srsran {

bool rlc_am_is_control_pdu(uint8_t* payload)
{
  return ((*(payload) >> 7) & 0x01) == RLC_DC_FIELD_CONTROL_PDU;
}

bool rlc_am_is_control_pdu(byte_buffer_t* pdu)
{
  return rlc_am_is_control_pdu(pdu->msg);
}

/*******************************************************
 *     RLC AM entity
 *     This entity is common between LTE and NR
 *     and only the TX/RX entities change between them
 *******************************************************/
rlc_am::rlc_am(srsran_rat_t               rat,
               srslog::basic_logger&      logger,
               uint32_t                   lcid_,
               srsue::pdcp_interface_rlc* pdcp_,
               srsue::rrc_interface_rlc*  rrc_,
               srsran::timer_handler*     timers_) :
  rlc_common(logger), rrc(rrc_), pdcp(pdcp_), timers(timers_), lcid(lcid_)
{
  if (rat == srsran_rat_t::lte) {
    rlc_am_lte_tx* tx = new rlc_am_lte_tx(this);
    rlc_am_lte_rx* rx = new rlc_am_lte_rx(this);
    tx_base           = std::unique_ptr<rlc_am_base_tx>(tx);
    rx_base           = std::unique_ptr<rlc_am_base_rx>(rx);
    tx->set_rx(rx);
    rx->set_tx(tx);
  } else if (rat == srsran_rat_t::nr) {
    rlc_am_nr_tx* tx = new rlc_am_nr_tx(this);
    rlc_am_nr_rx* rx = new rlc_am_nr_rx(this);
    tx_base          = std::unique_ptr<rlc_am_base_tx>(tx);
    rx_base          = std::unique_ptr<rlc_am_base_rx>(rx);
    tx->set_rx(rx);
    rx->set_tx(tx);
  } else {
    RlcError("Invalid RAT at entity initialization");
  }
}

bool rlc_am::configure(const rlc_config_t& cfg_)
{
  // determine bearer name and configure rx/tx objects
  rb_name = rrc->get_rb_name(lcid);

  // store configuration
  cfg = cfg_;

  if (not rx_base->configure(cfg)) {
    RlcError("Error configuring bearer (RX)");
    return false;
  }

  if (not tx_base->configure(cfg)) {
    RlcError("Error configuring bearer (TX)");
    return false;
  }

  if (cfg.rat == srsran_rat_t::lte) {
    RlcInfo("AM LTE configured - t_poll_retx=%d, poll_pdu=%d, poll_byte=%d, max_retx_thresh=%d, "
            "t_reordering=%d, t_status_prohibit=%d, tx_queue_length=%d",
            cfg.am.t_poll_retx,
            cfg.am.poll_pdu,
            cfg.am.poll_byte,
            cfg.am.max_retx_thresh,
            cfg.am.t_reordering,
            cfg.am.t_status_prohibit,
            cfg.tx_queue_length);
  } else if (cfg.rat == srsran_rat_t::nr) {
    RlcInfo("AM NR configured - tx_sn_field_length=%d, rx_sn_field_length=%d, "
            "t_poll_retx=%d, poll_pdu=%d, poll_byte=%d, "
            "max_retx_thresh=%d, t_reassembly=%d, t_status_prohibit=%d, tx_queue_length=%d",
            to_number(cfg.am_nr.tx_sn_field_length),
            to_number(cfg.am_nr.rx_sn_field_length),
            cfg.am_nr.t_poll_retx,
            cfg.am_nr.poll_pdu,
            cfg.am_nr.poll_byte,
            cfg.am_nr.max_retx_thresh,
            cfg.am_nr.t_reassembly,
            cfg.am_nr.t_status_prohibit,
            cfg.tx_queue_length);
  } else {
    RlcError("Invalid RAT at entity configuration");
  }
  return true;
}

void rlc_am::stop()
{
  RlcDebug("Stopped bearer");
  tx_base->stop();
  rx_base->stop();
}

void rlc_am::reestablish()
{
  RlcDebug("Reestablished bearer");
  tx_base->reestablish(); // calls stop and enables tx again
  rx_base->reestablish(); // calls only stop
}

/****************************************************************************
 * PDCP interface
 ***************************************************************************/
void rlc_am::write_sdu(unique_byte_buffer_t sdu)
{
  uint32_t nof_bytes = sdu->N_bytes;
  if (tx_base->write_sdu(std::move(sdu)) == SRSRAN_SUCCESS) {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.num_tx_sdus++;
    metrics.num_tx_sdu_bytes += nof_bytes;
  }
}

void rlc_am::discard_sdu(uint32_t discard_sn)
{
  tx_base->discard_sdu(discard_sn);

  std::lock_guard<std::mutex> lock(metrics_mutex);
  metrics.num_lost_sdus++;
}

bool rlc_am::sdu_queue_is_full()
{
  return tx_base->sdu_queue_is_full();
}

/****************************************************************************
 * MAC interface
 ***************************************************************************/
bool rlc_am::has_data()
{
  return tx_base->has_data();
}

uint32_t rlc_am::get_buffer_state()
{
  return tx_base->get_buffer_state();
}

void rlc_am::get_buffer_state(uint32_t& n_bytes_newtx, uint32_t& n_bytes_prio)
{
  tx_base->get_buffer_state(n_bytes_newtx, n_bytes_prio);
  return;
}

uint32_t rlc_am::read_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  uint32_t read_bytes = tx_base->read_pdu(payload, nof_bytes);

  std::lock_guard<std::mutex> lock(metrics_mutex);
  metrics.num_tx_pdus += read_bytes > 0 ? 1 : 0;
  metrics.num_tx_pdu_bytes += read_bytes;
  return read_bytes;
}

void rlc_am::write_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  rx_base->write_pdu(payload, nof_bytes);

  std::lock_guard<std::mutex> lock(metrics_mutex);
  metrics.num_rx_pdus++;
  metrics.num_rx_pdu_bytes += nof_bytes;
}

/****************************************************************************
 * Metrics
 ***************************************************************************/
rlc_bearer_metrics_t rlc_am::get_metrics()
{
  // update values that aren't calculated on the fly
  uint32_t latency        = rx_base->get_sdu_rx_latency_ms();
  uint32_t buffered_bytes = rx_base->get_rx_buffered_bytes();

  std::lock_guard<std::mutex> lock(metrics_mutex);
  metrics.rx_latency_ms     = latency;
  metrics.rx_buffered_bytes = buffered_bytes;
  return metrics;
}

void rlc_am::reset_metrics()
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  metrics = {};
}

/****************************************************************************
 * BSR callback
 ***************************************************************************/
void rlc_am::set_bsr_callback(bsr_callback_t callback)
{
  tx_base->set_bsr_callback(callback);
}

/*******************************************************
 *     RLC AM TX entity
 *     This class is used for common code between the
 *     LTE and NR TX entities
 *******************************************************/
int rlc_am::rlc_am_base_tx::write_sdu(unique_byte_buffer_t sdu)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (!tx_enabled) {
    return SRSRAN_ERROR;
  }

  if (sdu.get() == nullptr) {
    RlcWarning("NULL SDU pointer in write_sdu()");
    return SRSRAN_ERROR;
  }

  // Get SDU info
  uint32_t sdu_pdcp_sn = sdu->md.pdcp_sn;

  // Store SDU
  uint8_t*                                 msg_ptr   = sdu->msg;
  uint32_t                                 nof_bytes = sdu->N_bytes;
  srsran::error_type<unique_byte_buffer_t> ret       = tx_sdu_queue.try_write(std::move(sdu));
  if (ret) {
    RlcHexInfo(msg_ptr,
               nof_bytes,
               "Tx SDU (%d B, PDCP_SN=%ld tx_sdu_queue_len=%d)",
               nof_bytes,
               sdu_pdcp_sn,
               tx_sdu_queue.size());
  } else {
    // in case of fail, the try_write returns back the sdu
    RlcHexWarning(ret.error()->msg,
                  ret.error()->N_bytes,
                  "[Dropped SDU] Tx SDU (%d B, PDCP_SN=%ld, tx_sdu_queue_len=%d)",
                  ret.error()->N_bytes,
                  sdu_pdcp_sn,
                  tx_sdu_queue.size());
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

void rlc_am::rlc_am_base_tx::discard_sdu(uint32_t discard_sn)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (!tx_enabled) {
    return;
  }
  bool discarded = tx_sdu_queue.apply_first([&discard_sn, this](unique_byte_buffer_t& sdu) {
    if (sdu != nullptr && sdu->md.pdcp_sn == discard_sn) {
      tx_sdu_queue.queue.pop_func(sdu);
      sdu = nullptr;
      return true;
    }
    return false;
  });

  // Discard fails when the PDCP PDU is already in Tx window.
  RlcInfo("%s PDU with PDCP_SN=%d", discarded ? "Discarding" : "Couldn't discard", discard_sn);
}

bool rlc_am::rlc_am_base_tx::sdu_queue_is_full()
{
  return tx_sdu_queue.is_full();
}

void rlc_am::rlc_am_base_tx::set_bsr_callback(bsr_callback_t callback)
{
  bsr_callback = callback;
}

/*******************************************************
 *     RLC AM RX entity
 *     This class is used for common code between the
 *     LTE and NR TX entities
 *******************************************************/
void rlc_am::rlc_am_base_rx::write_pdu(uint8_t* payload, const uint32_t nof_bytes)
{
  RlcInfo("Rx PDU - N bytes %d", nof_bytes);
  if (nof_bytes < 1) {
    return;
  }

  if (rlc_am_is_control_pdu(payload)) {
    parent->tx_base->handle_control_pdu(payload, nof_bytes);
  } else {
    handle_data_pdu(payload, nof_bytes);
  }
}
} // namespace srsran
