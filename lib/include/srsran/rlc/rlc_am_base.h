/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

/*******************************************************
 *     RLC AM entity
 *     This entity is common between LTE and NR
 *     and only the TX/RX entities change between them
 *******************************************************/
class rlc_am : public rlc_common
{
public:
  class rlc_am_base_tx;
  class rlc_am_base_rx;

  friend class rlc_am_lte_tx;
  friend class rlc_am_lte_rx;
  friend class rlc_am_nr_tx;
  friend class rlc_am_nr_rx;

  rlc_am(srsran_rat_t               rat,
         srslog::basic_logger&      logger,
         uint32_t                   lcid_,
         srsue::pdcp_interface_rlc* pdcp_,
         srsue::rrc_interface_rlc*  rrc_,
         srsran::timer_handler*     timers_);

  bool configure(const rlc_config_t& cfg_) final;

  void reestablish() final;

  void stop() final;

  void empty_queue() final { tx_base->empty_queue(); }

  rlc_mode_t get_mode() final { return rlc_mode_t::am; }

  uint32_t get_lcid() final { return lcid; }

  /****************************************************************************
   * PDCP interface
   ***************************************************************************/
  void write_sdu(unique_byte_buffer_t sdu) final;

  void discard_sdu(uint32_t discard_sn) final;

  bool sdu_queue_is_full() final;

  /****************************************************************************
   * MAC interface
   ***************************************************************************/
  bool     has_data() final;
  uint32_t get_buffer_state() final;
  void     get_buffer_state(uint32_t& n_bytes_newtx, uint32_t& n_bytes_prio) final;

  uint32_t read_pdu(uint8_t* payload, uint32_t nof_bytes) final;

  void write_pdu(uint8_t* payload, uint32_t nof_bytes) final;

  /****************************************************************************
   * Metrics
   ***************************************************************************/
  rlc_bearer_metrics_t get_metrics() final;
  void                 reset_metrics() final;

  /****************************************************************************
   * BSR Callback
   ***************************************************************************/
  void set_bsr_callback(bsr_callback_t callback) final;

protected:
  // Common variables needed/provided by parent class
  srsran::timer_handler* timers = nullptr;
  uint32_t               lcid   = 0;
  rlc_config_t           cfg    = {};

  static const int poll_periodicity = 8; // After how many data PDUs a status PDU shall be requested

  std::mutex           metrics_mutex;
  rlc_bearer_metrics_t metrics = {};

  srsue::rrc_interface_rlc*  rrc  = nullptr;
  srsue::pdcp_interface_rlc* pdcp = nullptr;

  /*******************************************************
   *     RLC AM TX entity
   *     This class is used for common code between the
   *     LTE and NR TX entities
   *******************************************************/
public:
  class rlc_am_base_tx
  {
  public:
    explicit rlc_am_base_tx(srslog::basic_logger& logger_) : logger(logger_) {}
    virtual ~rlc_am_base_tx() = default;

    virtual bool     configure(const rlc_config_t& cfg_)                           = 0;
    virtual void     handle_control_pdu(uint8_t* payload, uint32_t nof_bytes)      = 0;
    virtual uint32_t get_buffer_state()                                            = 0;
    virtual void     get_buffer_state(uint32_t& tx_queue, uint32_t& prio_tx_queue) = 0;
    virtual void     reestablish()                                                 = 0;
    virtual void     empty_queue()                                                 = 0;
    virtual bool     has_data()                                                    = 0;
    virtual void     stop()                                                        = 0;

    void set_bsr_callback(bsr_callback_t callback);

    int              write_sdu(unique_byte_buffer_t sdu);
    bool             sdu_queue_is_full();
    virtual void     discard_sdu(uint32_t pdcp_sn);
    virtual uint32_t read_pdu(uint8_t* payload, uint32_t nof_bytes) = 0;

    bool                  tx_enabled = false;
    byte_buffer_pool*     pool       = nullptr;
    srslog::basic_logger& logger;
    std::string           rb_name;

    bsr_callback_t bsr_callback;

    // Tx SDU buffers
    byte_buffer_queue tx_sdu_queue;

    // Mutexes
    std::mutex mutex;
  };

  /*******************************************************
   *     RLC AM RX entity
   *     This class is used for common code between the
   *     LTE and NR RX entities
   *******************************************************/
  class rlc_am_base_rx
  {
  public:
    explicit rlc_am_base_rx(rlc_am* parent_, srslog::basic_logger& logger_) : parent(parent_), logger(logger_) {}
    virtual ~rlc_am_base_rx() = default;

    virtual bool     configure(const rlc_config_t& cfg_)                   = 0;
    virtual void     handle_data_pdu(uint8_t* payload, uint32_t nof_bytes) = 0;
    virtual void     reestablish()                                         = 0;
    virtual void     stop()                                                = 0;
    virtual uint32_t get_sdu_rx_latency_ms()                               = 0;
    virtual uint32_t get_rx_buffered_bytes()                               = 0;

    void write_pdu(uint8_t* payload, uint32_t nof_bytes);

    srslog::basic_logger& logger;
    byte_buffer_pool*     pool   = nullptr;
    rlc_am*               parent = nullptr;
    std::string           rb_name;

  protected:
    std::atomic<bool> do_status = {false}; // light-weight access from Tx entity
  };

protected:
  std::unique_ptr<rlc_am_base_tx> tx_base = {};
  std::unique_ptr<rlc_am_base_rx> rx_base = {};

public:
  // Getters for TX/RX entities. Useful for testing.
  rlc_am_base_rx* get_rx() { return rx_base.get(); }
  rlc_am_base_tx* get_tx() { return tx_base.get(); }
};

} // namespace srsran

#endif // SRSRAN_RLC_AM_BASE_H
