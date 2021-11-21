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
#include "srsran/rlc/rlc_am_data_structs.h"
#include "srsran/rlc/rlc_am_nr_packing.h"
#include "srsran/upper/byte_buffer_queue.h"
#include <map>
#include <mutex>
#include <pthread.h>
#include <queue>

namespace srsran {

/******************************
 *
 * RLC AM NR entity
 *
 *****************************/
class rlc_am_nr_tx;
class rlc_am_nr_rx;

// Transmitter sub-class
class rlc_am_nr_tx : public rlc_am::rlc_am_base_tx
{
public:
  explicit rlc_am_nr_tx(rlc_am* parent_);
  ~rlc_am_nr_tx() = default;

  void     set_rx(rlc_am_nr_rx* rx_) { rx = rx_; }
  bool     configure(const rlc_config_t& cfg_) final;
  uint32_t read_pdu(uint8_t* payload, uint32_t nof_bytes) final;
  void     handle_control_pdu(uint8_t* payload, uint32_t nof_bytes) final;

  void discard_sdu(uint32_t discard_sn) final;
  bool sdu_queue_is_full() final;
  void reestablish() final;

  int      write_sdu(unique_byte_buffer_t sdu);
  void     empty_queue() final;
  bool     has_data() final;
  uint32_t get_buffer_state() final;
  void     get_buffer_state(uint32_t& tx_queue, uint32_t& prio_tx_queue);

  void stop() final;

private:
  rlc_am*       parent = nullptr;
  rlc_am_nr_rx* rx     = nullptr;

  /****************************************************************************
   * Configurable parameters
   * Ref: 3GPP TS 38.322 v10.0.0 Section 7.4
   ***************************************************************************/
  rlc_am_config_t cfg = {};

  /****************************************************************************
   * Tx state variables
   * Ref: 3GPP TS 38.322 v10.0.0 Section 7.1
   ***************************************************************************/
  struct rlc_nr_tx_state_t {
    uint32_t tx_next_ack;
    uint32_t tx_next;
    uint32_t poll_sn;
    uint32_t pdu_without_poll;
    uint32_t byte_without_poll;
  } st = {};

  using rlc_amd_tx_pdu_nr = rlc_amd_tx_pdu<rlc_am_nr_pdu_header_t>;
  rlc_ringbuffer_t<rlc_amd_tx_pdu_nr, RLC_AM_WINDOW_SIZE> tx_window;
};

// Receiver sub-class
class rlc_am_nr_rx : public rlc_am::rlc_am_base_rx
{
public:
  explicit rlc_am_nr_rx(rlc_am* parent_);
  ~rlc_am_nr_rx() = default;

  void set_tx(rlc_am_nr_tx* tx_) { tx = tx_; }
  bool configure(const rlc_config_t& cfg_) final;

  void handle_data_pdu(uint8_t* payload, uint32_t nof_bytes) final;

  void stop();
  void reestablish();

  uint32_t get_sdu_rx_latency_ms();
  uint32_t get_rx_buffered_bytes();

private:
  rlc_am*           parent = nullptr;
  rlc_am_nr_tx*     tx     = nullptr;
  byte_buffer_pool* pool   = nullptr;

  /****************************************************************************
   * Configurable parameters
   * Ref: 3GPP TS 38.322 v10.0.0 Section 7.4
   ***************************************************************************/
  rlc_am_config_t cfg = {};
};

} // namespace srsran
#endif // SRSRAN_RLC_AM_NR_H
