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

#ifndef SRSRAN_RLC_AM_BASE_H
#define SRSRAN_RLC_AM_BASE_H

#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/timers.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include "srsran/rlc/rlc_common.h"
#include "srsran/upper/byte_buffer_queue.h"
#include <map>
#include <mutex>
#include <pthread.h>
#include <queue>

namespace srsue {

class pdcp_interface_rlc;
class rrc_interface_rlc;

} // namespace srsue

namespace srsran {

bool rlc_am_is_control_pdu(uint8_t* payload);
bool rlc_am_is_control_pdu(byte_buffer_t* pdu);

/*******************************
 *     rlc_am_base class
 ******************************/
template <typename T_rlc_tx, typename T_rlc_rx>
class rlc_am_base : public rlc_common
{
public:
  rlc_am_base(srslog::basic_logger&      logger,
              uint32_t                   lcid_,
              srsue::pdcp_interface_rlc* pdcp_,
              srsue::rrc_interface_rlc*  rrc_,
              srsran::timer_handler*     timers_) :
    logger(logger), rrc(rrc_), pdcp(pdcp_), timers(timers_), lcid(lcid_), tx(this), rx(this)
  {}

  bool configure(const rlc_config_t& cfg_) final
  {
    // determine bearer name and configure Rx/Tx objects
    rb_name = rrc->get_rb_name(lcid);

    // store config
    cfg = cfg_;

    if (not rx.configure(cfg)) {
      logger.error("Error configuring bearer (RX)");
      return false;
    }

    if (not tx.configure(cfg)) {
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

  void reestablish() final
  {
    logger.debug("Reestablished bearer %s", rb_name.c_str());
    tx.reestablish(); // calls stop and enables tx again
    rx.reestablish(); // calls only stop
  }

  void stop() final
  {
    logger.debug("Stopped bearer %s", rb_name.c_str());
    tx.stop();
    rx.stop();
  }

  void empty_queue() final
  {
    // Drop all messages in TX SDU queue
    tx.empty_queue();
  }

  rlc_mode_t get_mode() final { return rlc_mode_t::am; }

  uint32_t get_bearer() final { return lcid; }

  /****************************************************************************
   * PDCP interface
   ***************************************************************************/
  void write_sdu(unique_byte_buffer_t sdu) final
  {
    if (tx.write_sdu(std::move(sdu)) == SRSRAN_SUCCESS) {
      std::lock_guard<std::mutex> lock(metrics_mutex);
      metrics.num_tx_sdus++;
    }
  }

  void discard_sdu(uint32_t discard_sn) final
  {
    tx.discard_sdu(discard_sn);

    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.num_lost_sdus++;
  }

  bool sdu_queue_is_full() final { return tx.sdu_queue_is_full(); }

  /****************************************************************************
   * MAC interface
   ***************************************************************************/
  bool     has_data() final { return tx.has_data(); }
  uint32_t get_buffer_state() final { return tx.get_buffer_state(); }
  void     get_buffer_state(uint32_t& tx_queue, uint32_t& prio_tx_queue) final
  {
    tx.get_buffer_state(tx_queue, prio_tx_queue);
  }

  uint32_t read_pdu(uint8_t* payload, uint32_t nof_bytes) final
  {
    uint32_t read_bytes = tx.read_pdu(payload, nof_bytes);

    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.num_tx_pdus++;
    metrics.num_tx_pdu_bytes += read_bytes;

    return read_bytes;
  }

  void write_pdu(uint8_t* payload, uint32_t nof_bytes) final
  {
    rx.write_pdu(payload, nof_bytes);

    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.num_rx_pdus++;
    metrics.num_rx_pdu_bytes += nof_bytes;
  }

  /****************************************************************************
   * Metrics
   ***************************************************************************/
  rlc_bearer_metrics_t get_metrics()
  {
    // update values that aren't calculated on the fly
    uint32_t latency        = rx.get_sdu_rx_latency_ms();
    uint32_t buffered_bytes = rx.get_rx_buffered_bytes();

    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics.rx_latency_ms     = latency;
    metrics.rx_buffered_bytes = buffered_bytes;

    return metrics;
  }

  void reset_metrics()
  {
    std::lock_guard<std::mutex> lock(metrics_mutex);
    metrics = {};
  }

  // BSR callback
  void set_bsr_callback(bsr_callback_t callback) final { tx.set_bsr_callback(callback); }

private:
  // Common variables needed/provided by parent class
  srsue::rrc_interface_rlc*  rrc = nullptr;
  srslog::basic_logger&      logger;
  srsue::pdcp_interface_rlc* pdcp   = nullptr;
  srsran::timer_handler*     timers = nullptr;
  uint32_t                   lcid   = 0;
  rlc_config_t               cfg    = {};
  std::string                rb_name;

  static const int poll_periodicity = 8; // After how many data PDUs a status PDU shall be requested

  // Rx and Tx objects
  friend class rlc_am_lte_tx;
  friend class rlc_am_lte_rx;
  friend class rlc_am_nr_tx;
  friend class rlc_am_nr_rx;
  T_rlc_tx tx;
  T_rlc_rx rx;

  std::mutex           metrics_mutex;
  rlc_bearer_metrics_t metrics = {};
};

} // namespace srsran

#endif // SRSRAN_RLC_AM_BASE_H
