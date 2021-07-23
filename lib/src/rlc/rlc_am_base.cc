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

#include "srsran/rlc/rlc_am_base.h"
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
bool rlc_am_base::configure(const rlc_config_t& cfg_)
{
  // determine bearer name and configure Rx/Tx objects
  rb_name = rrc->get_rb_name(lcid);

  // store config
  cfg = cfg_;

  if (not rx_base->configure(cfg)) {
    logger.error("Error configuring bearer (RX)");
    return false;
  }

  if (not tx_base->configure(cfg)) {
    logger.error("Error configuring bearer (TX)");
    return false;
  }

  logger.info("%s configured: t_poll_retx=%d, poll_pdu=%d, poll_byte=%d, max_retx_thresh=%d, "
              "t_reordering=%d, t_status_prohibit=%d",
              rb_name.c_str(),
              cfg.am.t_poll_retx,
              cfg.am.poll_pdu,
              cfg.am.poll_byte,
              cfg.am.max_retx_thresh,
              cfg.am.t_reordering,
              cfg.am.t_status_prohibit);
  return true;
}

void rlc_am_base::stop()
{
  logger.debug("Stopped bearer %s", rb_name.c_str());
  tx_base->stop();
  rx_base->stop();
}

void rlc_am_base::reestablish()
{
  logger.debug("Reestablished bearer %s", rb_name.c_str());
  tx_base->reestablish(); // calls stop and enables tx again
  rx_base->reestablish(); // calls only stop
}

/****************************************************************************
 * PDCP interface
 ***************************************************************************/
void rlc_am_base::write_sdu(unique_byte_buffer_t sdu)
{
  if (tx_base->write_sdu(std::move(sdu)) == SRSRAN_SUCCESS) {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.num_tx_sdus++;
  }
}

void rlc_am_base::discard_sdu(uint32_t discard_sn)
{
  tx_base->discard_sdu(discard_sn);

  std::lock_guard<std::mutex> lock(metrics_mutex);
  metrics.num_lost_sdus++;
}

bool rlc_am_base::sdu_queue_is_full()
{
  return tx_base->sdu_queue_is_full();
}

/****************************************************************************
 * MAC interface
 ***************************************************************************/
bool rlc_am_base::has_data()
{
  return tx_base->has_data();
}

uint32_t rlc_am_base::get_buffer_state()
{
  return tx_base->get_buffer_state();
}

void rlc_am_base::get_buffer_state(uint32_t& n_bytes_newtx, uint32_t& n_bytes_prio)
{
  tx_base->get_buffer_state(n_bytes_newtx, n_bytes_prio);
  return;
}

uint32_t rlc_am_base::read_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  uint32_t read_bytes = tx_base->read_pdu(payload, nof_bytes);

  std::lock_guard<std::mutex> lock(metrics_mutex);
  metrics.num_tx_pdus++;
  metrics.num_tx_pdu_bytes += read_bytes;
  return read_bytes;
}

void rlc_am_base::write_pdu(uint8_t* payload, uint32_t nof_bytes)
{
  rx_base->write_pdu(payload, nof_bytes);

  std::lock_guard<std::mutex> lock(metrics_mutex);
  metrics.num_rx_pdus++;
  metrics.num_rx_pdu_bytes += nof_bytes;
}

/****************************************************************************
 * Metrics
 ***************************************************************************/
rlc_bearer_metrics_t rlc_am_base::get_metrics()
{
  // update values that aren't calculated on the fly
  uint32_t latency        = rx_base->get_sdu_rx_latency_ms();
  uint32_t buffered_bytes = rx_base->get_rx_buffered_bytes();

  std::lock_guard<std::mutex> lock(metrics_mutex);
  metrics.rx_latency_ms     = latency;
  metrics.rx_buffered_bytes = buffered_bytes;
  return metrics;
}

void rlc_am_base::reset_metrics()
{
  std::lock_guard<std::mutex> lock(metrics_mutex);
  metrics = {};
}

/****************************************************************************
 * BSR callback
 ***************************************************************************/
void rlc_am_base::set_bsr_callback(bsr_callback_t callback)
{
  tx_base->set_bsr_callback(callback);
}

/*******************************************************
 *     RLC AM TX entity
 *     This class is used for common code between the
 *     LTE and NR TX entitites
 *******************************************************/
int rlc_am_base::rlc_am_base_tx::write_sdu(unique_byte_buffer_t sdu)
{
  std::lock_guard<std::mutex> lock(mutex);

  if (!tx_enabled) {
    return SRSRAN_ERROR;
  }

  if (sdu.get() == nullptr) {
    srslog::fetch_basic_logger("RLC").warning("NULL SDU pointer in write_sdu()");
    return SRSRAN_ERROR;
  }

  // Get SDU info
  uint32_t sdu_pdcp_sn = sdu->md.pdcp_sn;

  // Store SDU
  uint8_t*                                 msg_ptr   = sdu->msg;
  uint32_t                                 nof_bytes = sdu->N_bytes;
  srsran::error_type<unique_byte_buffer_t> ret       = tx_sdu_queue.try_write(std::move(sdu));
  if (ret) {
    srslog::fetch_basic_logger("RLC").info(
        msg_ptr, nof_bytes, "%s Tx SDU (%d B, tx_sdu_queue_len=%d)", rb_name, nof_bytes, tx_sdu_queue.size());
  } else {
    // in case of fail, the try_write returns back the sdu
    srslog::fetch_basic_logger("RLC").warning(ret.error()->msg,
                                              ret.error()->N_bytes,
                                              "[Dropped SDU] %s Tx SDU (%d B, tx_sdu_queue_len=%d)",
                                              rb_name,
                                              ret.error()->N_bytes,
                                              tx_sdu_queue.size());
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

void rlc_am_base::rlc_am_base_tx::set_bsr_callback(bsr_callback_t callback)
{
  bsr_callback = callback;
}

} // namespace srsran
