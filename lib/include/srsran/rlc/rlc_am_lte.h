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

#ifndef SRSRAN_RLC_AM_LTE_H
#define SRSRAN_RLC_AM_LTE_H

#include "srsran/adt/accumulators.h"
#include "srsran/adt/circular_array.h"
#include "srsran/adt/circular_map.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/common/timeout.h"
#include "srsran/interfaces/pdcp_interface_types.h"
#include "srsran/rlc/rlc_am_base.h"
#include "srsran/rlc/rlc_am_data_structs.h"
#include "srsran/rlc/rlc_am_lte_packing.h"
#include "srsran/rlc/rlc_common.h"
#include "srsran/support/srsran_assert.h"
#include "srsran/upper/byte_buffer_queue.h"
#include <deque>
#include <list>
#include <map>

namespace srsran {

#undef RLC_AM_BUFFER_DEBUG

/******************************
 *
 * RLC AM LTE entity
 *
 *****************************/

/******************************
 * RLC AM LTE TX entity
 *****************************/
class rlc_am_lte_tx;
class rlc_am_lte_rx;
class rlc_am_lte_tx : public rlc_am::rlc_am_base_tx, timer_callback
{
public:
  explicit rlc_am_lte_tx(rlc_am* parent_);
  ~rlc_am_lte_tx() = default;

  void set_rx(rlc_am_lte_rx* rx_) { rx = rx_; };
  bool configure(const rlc_config_t& cfg_);
  void empty_queue();
  void reestablish();
  void stop();

  uint32_t read_pdu(uint8_t* payload, uint32_t nof_bytes);

  bool     has_data();
  uint32_t get_buffer_state();
  void     get_buffer_state(uint32_t& n_bytes_newtx, uint32_t& n_bytes_prio);

  void empty_queue_nolock();
  void debug_state();

  // Timeout callback interface
  void timer_expired(uint32_t timeout_id) final;

  // Interface for Rx subclass
  void handle_control_pdu(uint8_t* payload, uint32_t nof_bytes);

private:
  void stop_nolock();

  int  build_status_pdu(uint8_t* payload, uint32_t nof_bytes);
  int  build_retx_pdu(uint8_t* payload, uint32_t nof_bytes);
  int  build_segment(uint8_t* payload, uint32_t nof_bytes, rlc_amd_retx_lte_t retx);
  int  build_data_pdu(uint8_t* payload, uint32_t nof_bytes);
  void update_notification_ack_info(uint32_t rlc_sn);

  int  required_buffer_size(const rlc_amd_retx_lte_t& retx);
  void retransmit_pdu(uint32_t sn);

  // Helpers
  bool window_full();
  bool poll_required();
  bool do_status();
  void check_sn_reached_max_retx(uint32_t sn);
  void get_buffer_state_nolock(uint32_t& new_tx, uint32_t& prio_tx);

  rlc_am*                                       parent = nullptr;
  rlc_am_lte_rx*                                rx     = nullptr;
  byte_buffer_pool*                             pool   = nullptr;
  rlc_am_pdu_segment_pool<rlc_amd_pdu_header_t> segment_pool;

  /****************************************************************************
   * Configurable parameters
   * Ref: 3GPP TS 36.322 v10.0.0 Section 7
   ***************************************************************************/

  rlc_am_config_t cfg = {};

  // TX SDU buffers
  unique_byte_buffer_t tx_sdu;

  /****************************************************************************
   * State variables and counters
   * Ref: 3GPP TS 36.322 v10.0.0 Section 7
   ***************************************************************************/

  // Tx state variables
  uint32_t vt_a    = 0;                  // ACK state. SN of next PDU in sequence to be ACKed. Low edge of tx window.
  uint32_t vt_ms   = RLC_AM_WINDOW_SIZE; // Max send state. High edge of tx window. vt_a + window_size.
  uint32_t vt_s    = 0;                  // Send state. SN to be assigned for next PDU.
  uint32_t poll_sn = 0;                  // Poll send state. SN of most recent PDU txed with poll bit set.

  // Tx counters
  uint32_t pdu_without_poll  = 0;
  uint32_t byte_without_poll = 0;

  rlc_status_pdu_t tx_status;

  /****************************************************************************
   * Timers
   * Ref: 3GPP TS 36.322 v10.0.0 Section 7
   ***************************************************************************/

  srsran::timer_handler::unique_timer poll_retx_timer;
  srsran::timer_handler::unique_timer status_prohibit_timer;

  // SDU info for PDCP notifications
  buffered_pdcp_pdu_list<rlc_amd_pdu_header_t> undelivered_sdu_info_queue;

  // Tx windows
  rlc_ringbuffer_t<rlc_amd_tx_pdu<rlc_amd_pdu_header_t>, RLC_AM_WINDOW_SIZE> tx_window;
  pdu_retx_queue<rlc_amd_retx_lte_t, RLC_AM_WINDOW_SIZE>                     retx_queue;
  pdcp_sn_vector_t                                                           notify_info_vec;

  // Mutexes
  std::mutex mutex;

  // default to RLC SDU queue length
  const uint32_t MAX_SDUS_PER_RLC_PDU = RLC_TX_QUEUE_LEN;
};

/******************************
 * RLC AM LTE RX entity
 *****************************/
class rlc_am_lte_rx : public rlc_am::rlc_am_base_rx, public timer_callback
{
public:
  explicit rlc_am_lte_rx(rlc_am* parent_);
  ~rlc_am_lte_rx() = default;

  void set_tx(rlc_am_lte_tx* tx_) { tx = tx_; };
  bool configure(const rlc_config_t& cfg_) final;
  void reestablish() final;
  void stop() final;

  uint32_t get_rx_buffered_bytes() final; // returns sum of PDUs in rx_window
  uint32_t get_sdu_rx_latency_ms() final;

  // Timeout callback interface
  void timer_expired(uint32_t timeout_id) final;

  // Functions needed by Tx subclass to query rx state
  int  get_status_pdu_length();
  int  get_status_pdu(rlc_status_pdu_t* status, uint32_t nof_bytes);
  bool get_do_status();

private:
  void handle_data_pdu(uint8_t* payload, uint32_t nof_bytes) final;
  void handle_data_pdu_full(uint8_t* payload, uint32_t nof_bytes, rlc_amd_pdu_header_t& header);
  void handle_data_pdu_segment(uint8_t* payload, uint32_t nof_bytes, rlc_amd_pdu_header_t& header);
  void reassemble_rx_sdus();
  bool inside_rx_window(const int16_t sn);
  void debug_state();
  void print_rx_segments();
  bool add_segment_and_check(rlc_amd_rx_pdu_segments_t* pdu, rlc_amd_rx_pdu* segment);
  void reset_status();

  rlc_am*           parent = nullptr;
  rlc_am_lte_tx*    tx     = nullptr;
  byte_buffer_pool* pool   = nullptr;

  /****************************************************************************
   * Configurable parameters
   * Ref: 3GPP TS 36.322 v10.0.0 Section 7
   ***************************************************************************/
  rlc_am_config_t cfg = {};

  // RX SDU buffers
  unique_byte_buffer_t rx_sdu;

  /****************************************************************************
   * State variables and counters
   * Ref: 3GPP TS 36.322 v10.0.0 Section 7
   ***************************************************************************/

  // Rx state variables
  uint32_t vr_r  = 0; // Receive state. SN following last in-sequence received PDU. Low edge of rx window
  uint32_t vr_mr = RLC_AM_WINDOW_SIZE; // Max acceptable receive state. High edge of rx window. vr_r + window size.
  uint32_t vr_x  = 0;                  // t_reordering state. SN following PDU which triggered t_reordering.
  uint32_t vr_ms = 0;                  // Max status tx state. Highest possible value of SN for ACK_SN in status PDU.
  uint32_t vr_h  = 0;                  // Highest rx state. SN following PDU with highest SN among rxed PDUs.

  // Mutex to protect members
  std::mutex mutex;

  // Rx windows
  rlc_ringbuffer_t<rlc_amd_rx_pdu, RLC_AM_WINDOW_SIZE> rx_window;
  std::map<uint32_t, rlc_amd_rx_pdu_segments_t>        rx_segments;

  bool              poll_received = false;
  std::atomic<bool> do_status     = {false}; // light-weight access from Tx entity

  /****************************************************************************
   * Timers
   * Ref: 3GPP TS 36.322 v10.0.0 Section 7
   ***************************************************************************/

  srsran::timer_handler::unique_timer reordering_timer;

  srsran::rolling_average<double> sdu_rx_latency_ms;
};

} // namespace srsran

#endif // SRSRAN_RLC_AM_LTE_H
