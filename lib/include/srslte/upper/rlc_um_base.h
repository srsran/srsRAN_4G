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

#ifndef SRSLTE_RLC_UM_BASE_H
#define SRSLTE_RLC_UM_BASE_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/upper/byte_buffer_queue.h"
#include "srslte/upper/rlc_common.h"
#include <map>
#include <mutex>
#include <pthread.h>
#include <queue>

namespace srslte {

class rlc_um_base : public rlc_common
{
public:
  rlc_um_base(srslte::log_ref            log_,
              uint32_t                   lcid_,
              srsue::pdcp_interface_rlc* pdcp_,
              srsue::rrc_interface_rlc*  rrc_,
              srslte::timer_handler*     timers_);
  virtual ~rlc_um_base();
  void reestablish();
  void stop();
  void empty_queue();
  bool is_mrb();

  rlc_mode_t get_mode();
  uint32_t   get_bearer();

  // PDCP interface
  void write_sdu(unique_byte_buffer_t sdu);
  void discard_sdu(uint32_t discard_sn);
  bool sdu_queue_is_full();

  // MAC interface
  bool     has_data();
  uint32_t get_buffer_state();
  int      read_pdu(uint8_t* payload, uint32_t nof_bytes);
  void     write_pdu(uint8_t* payload, uint32_t nof_bytes);
  int      get_increment_sequence_num();

  rlc_bearer_metrics_t get_metrics();
  void                 reset_metrics();

  void set_bsr_callback(bsr_callback_t callback) {}

protected:
  // Transmitter sub-class base
  class rlc_um_base_tx
  {
  public:
    rlc_um_base_tx(rlc_um_base* parent_);
    virtual ~rlc_um_base_tx();
    virtual bool     configure(const rlc_config_t& cfg, std::string rb_name) = 0;
    int              build_data_pdu(uint8_t* payload, uint32_t nof_bytes);
    void             stop();
    void             reestablish();
    void             empty_queue();
    void             write_sdu(unique_byte_buffer_t sdu);
    void             discard_sdu(uint32_t discard_sn);
    bool             sdu_queue_is_full();
    int              try_write_sdu(unique_byte_buffer_t sdu);
    void             reset_metrics();
    bool             has_data();
    virtual uint32_t get_buffer_state() = 0;

  protected:
    byte_buffer_pool* pool = nullptr;
    srslte::log_ref   log;
    std::string       rb_name;

    rlc_config_t cfg = {};

    // TX SDU buffers
    byte_buffer_queue    tx_sdu_queue;
    unique_byte_buffer_t tx_sdu;

    // Mutexes
    std::mutex mutex;

    virtual int build_data_pdu(unique_byte_buffer_t pdu, uint8_t* payload, uint32_t nof_bytes) = 0;

    // helper functions
    virtual void debug_state() = 0;
    virtual void reset()       = 0;
  };

  // Receiver sub-class base
  class rlc_um_base_rx : public timer_callback
  {

  public:
    rlc_um_base_rx(rlc_um_base* parent_);
    virtual ~rlc_um_base_rx();
    virtual bool configure() = 0;

    virtual void stop()        = 0;
    virtual void reestablish() = 0;

    virtual void handle_data_pdu(uint8_t* payload, uint32_t nof_bytes) = 0;

  protected:
    byte_buffer_pool*          pool = nullptr;
    srslte::log_ref            log;
    srslte::timer_handler*     timers = nullptr;
    srsue::pdcp_interface_rlc* pdcp   = nullptr;
    srsue::rrc_interface_rlc*  rrc    = nullptr;

    rlc_bearer_metrics_t& metrics;

    std::string  rb_name;
    rlc_config_t cfg = {};

    unique_byte_buffer_t rx_sdu;

    uint32_t& lcid;

    // helper functions
    virtual void debug_state() = 0;
  };

  // Common variables needed by parent class
  srsue::rrc_interface_rlc*  rrc  = nullptr;
  srsue::pdcp_interface_rlc* pdcp = nullptr;
  srslte::log_ref            log;
  srslte::timer_handler*     timers = nullptr;
  uint32_t                   lcid   = 0;
  rlc_config_t               cfg    = {};
  std::string                rb_name;
  byte_buffer_pool*          pool = nullptr;
  std::string                get_rb_name(srsue::rrc_interface_rlc* rrc, uint32_t lcid, bool is_mrb);

  // Rx and Tx objects
  std::unique_ptr<rlc_um_base_tx> tx;
  std::unique_ptr<rlc_um_base_rx> rx;

  bool tx_enabled = false;
  bool rx_enabled = false;

  rlc_bearer_metrics_t metrics = {};
};

} // namespace srslte

#endif // SRSLTE_RLC_UM_BASE_H
