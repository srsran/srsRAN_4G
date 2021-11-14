/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSRAN_RLC_AM_NR_H
#define SRSRAN_RLC_AM_NR_H

#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/timers.h"
#include "srsran/rlc/rlc_am_base.h"
#include "srsran/upper/byte_buffer_queue.h"
#include <map>
#include <mutex>
#include <pthread.h>
#include <queue>

namespace srsran {

class rlc_am_nr : public rlc_common
{
public:
  rlc_am_nr(srslog::basic_logger&      logger,
            uint32_t                   lcid_,
            srsue::pdcp_interface_rlc* pdcp_,
            srsue::rrc_interface_rlc*  rrc_,
            srsran::timer_handler*     timers_);
  bool configure(const rlc_config_t& cfg_) final;
  void stop() final;

  rlc_mode_t get_mode() final;
  uint32_t   get_bearer() final;

  void reestablish() final;
  void empty_queue() final;
  void set_bsr_callback(bsr_callback_t callback) final;

  // PDCP interface
  void write_sdu(unique_byte_buffer_t sdu) final;
  void discard_sdu(uint32_t pdcp_sn) final;
  bool sdu_queue_is_full() final;

  // MAC interface
  bool     has_data() final;
  uint32_t get_buffer_state() final;
  void     get_buffer_state(uint32_t& tx_queue, uint32_t& prio_tx_queue) final;
  uint32_t read_pdu(uint8_t* payload, uint32_t nof_bytes) final;
  void     write_pdu(uint8_t* payload, uint32_t nof_bytes) final;

  rlc_bearer_metrics_t get_metrics() final;
  void                 reset_metrics() final;

private:
  // Transmitter sub-class
  class rlc_am_nr_tx
  {
  public:
    explicit rlc_am_nr_tx(rlc_am_nr* parent_);
    ~rlc_am_nr_tx() = default;

    bool configure(const rlc_am_config_t& cfg_);
    void stop();

    int      write_sdu(unique_byte_buffer_t sdu);
    uint32_t read_pdu(uint8_t* payload, uint32_t nof_bytes);
    void     discard_sdu(uint32_t discard_sn);
    bool     sdu_queue_is_full();

    bool     has_data();
    uint32_t get_buffer_state();

    rlc_am_nr*            parent = nullptr;
    srslog::basic_logger& logger;

  private:
    byte_buffer_pool* pool = nullptr;

    /****************************************************************************
     * Configurable parameters
     * Ref: 3GPP TS 38.322 v10.0.0 Section 7.4
     ***************************************************************************/
    rlc_am_config_t cfg = {};
  };

  // Receiver sub-class
  class rlc_am_nr_rx
  {
  public:
    explicit rlc_am_nr_rx(rlc_am_nr* parent_);
    ~rlc_am_nr_rx() = default;

    bool configure(const rlc_am_config_t& cfg_);
    void stop();

    void write_pdu(uint8_t* payload, uint32_t nof_bytes);

    rlc_am_nr*            parent = nullptr;
    srslog::basic_logger& logger;

  private:
    byte_buffer_pool* pool = nullptr;

    /****************************************************************************
     * Configurable parameters
     * Ref: 3GPP TS 38.322 v10.0.0 Section 7.4
     ***************************************************************************/
    rlc_am_config_t cfg = {};
  };

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
  rlc_am_nr_tx tx;
  rlc_am_nr_rx rx;

  rlc_bearer_metrics_t metrics = {};
};

} // namespace srsran

#endif // SRSRAN_RLC_AM_NR_H
