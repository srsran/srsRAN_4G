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

#ifndef SRSRAN_RLC_AM_NR_H
#define SRSRAN_RLC_AM_NR_H

#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/timers.h"
#include "srsran/interfaces/pdcp_interface_types.h"
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

/****************************************************************************
 * Tx state variables
 * Ref: 3GPP TS 38.322 version 16.2.0 Section 7.1
 ***************************************************************************/
struct rlc_am_nr_tx_state_t {
  /*
   * TX_Next_Ack: This state variable holds the value of the SN of the next RLC SDU for which a positive
   * acknowledgment is to be received in-sequence, and it serves as the lower edge of the transmitting window. It is
   * initially set to 0, and is updated whenever the AM RLC entity receives a positive acknowledgment for an RLC SDU
   * with SN = TX_Next_Ack.
   */
  uint32_t tx_next_ack;
  /*
   * TX_Next: This state variable holds the value of the SN to be assigned for the next newly generated AMD PDU. It is
   * initially set to 0, and is updated whenever the AM RLC entity constructs an AMD PDU with SN = TX_Next and
   * contains an RLC SDU or the last segment of a RLC SDU.
   */
  uint32_t tx_next;
  /*
   * POLL_SN: This state variable holds the value of the highest SN of the AMD PDU among the AMD PDUs submitted to
   * lower layer when POLL_SN is set according to sub clause 5.3.3.2. It is initially set to 0.
   */
  uint32_t poll_sn;
  /*
   * PDU_WITHOUT_POLL: This counter is initially set to 0. It counts the number of AMD PDUs sent since the most recent
   * poll bit was transmitted.
   */
  uint32_t pdu_without_poll;
  /*
   * BYTE_WITHOUT_POLL: This counter is initially set to 0. It counts the number of data bytes sent since the most
   * recent poll bit was transmitted.
   */
  uint32_t byte_without_poll;
};

struct rlc_amd_tx_pdu_nr {
  const uint32_t         rlc_sn     = INVALID_RLC_SN;
  uint32_t               pdcp_sn    = INVALID_RLC_SN;
  rlc_am_nr_pdu_header_t header     = {};
  unique_byte_buffer_t   sdu_buf    = nullptr;
  uint32_t               retx_count = RETX_COUNT_NOT_STARTED;
  struct pdu_segment {
    uint32_t so          = 0;
    uint32_t payload_len = 0;
  };
  std::list<pdu_segment> segment_list;
  explicit rlc_amd_tx_pdu_nr(uint32_t sn) : rlc_sn(sn) {}
};

class rlc_am_nr_tx : public rlc_am::rlc_am_base_tx
{
public:
  explicit rlc_am_nr_tx(rlc_am* parent_);
  ~rlc_am_nr_tx() = default;

  void     set_rx(rlc_am_nr_rx* rx_) { rx = rx_; }
  bool     configure(const rlc_config_t& cfg_) final;
  uint32_t read_pdu(uint8_t* payload, uint32_t nof_bytes) final;
  void     handle_control_pdu(uint8_t* payload, uint32_t nof_bytes) final;
  void     handle_nack(const rlc_status_nack_t& nack, std::set<uint32_t>& retx_sn_set);

  void reestablish() final;
  void stop() final;

  int  write_sdu(unique_byte_buffer_t sdu);
  void empty_queue() final;
  void empty_queue_no_lock();

  // Data PDU helpers
  uint32_t build_new_pdu(uint8_t* payload, uint32_t nof_bytes);
  uint32_t build_new_sdu_segment(rlc_amd_tx_pdu_nr& tx_pdu, uint8_t* payload, uint32_t nof_bytes);
  uint32_t build_continuation_sdu_segment(rlc_amd_tx_pdu_nr& tx_pdu, uint8_t* payload, uint32_t nof_bytes);
  uint32_t build_retx_pdu(uint8_t* payload, uint32_t nof_bytes);
  uint32_t build_retx_pdu_without_segmentation(rlc_amd_retx_nr_t retx, uint8_t* payload, uint32_t nof_bytes);
  uint32_t build_retx_pdu_with_segmentation(rlc_amd_retx_nr_t& retx, uint8_t* payload, uint32_t nof_bytes);
  bool     is_retx_segmentation_required(const rlc_amd_retx_nr_t& retx, uint32_t nof_bytes);
  uint32_t get_retx_expected_hdr_len(const rlc_amd_retx_nr_t& retx);

  // Buffer State
  bool     has_data() final;
  uint32_t get_buffer_state() final;
  void     get_buffer_state(uint32_t& tx_queue, uint32_t& prio_tx_queue) final;

  // Status PDU
  bool     do_status();
  uint32_t build_status_pdu(byte_buffer_t* payload, uint32_t nof_bytes);

  // Polling
  uint8_t get_pdu_poll(uint32_t sn, bool is_retx, uint32_t sdu_bytes);

  // Timers
  void timer_expired(uint32_t timeout_id);

  // Window helpers
  bool inside_tx_window(uint32_t sn) const;
  bool valid_ack_sn(uint32_t sn) const;

private:
  rlc_am*       parent = nullptr;
  rlc_am_nr_rx* rx     = nullptr;

  uint32_t        mod_nr = cardinality(rlc_am_nr_sn_size_t());
  inline uint32_t tx_mod_base_nr(uint32_t sn) const;
  void            check_sn_reached_max_retx(uint32_t sn);

  /****************************************************************************
   * Configurable parameters
   * Ref: 3GPP TS 38.322 version 16.2.0 Section 7.4
   ***************************************************************************/
  rlc_am_nr_config_t cfg = {};

  /****************************************************************************
   * Tx state variables
   * Ref: 3GPP TS 38.322 version 16.2.0 Section 7.1
   ***************************************************************************/
  struct rlc_am_nr_tx_state_t                              st = {};
  std::unique_ptr<rlc_ringbuffer_base<rlc_amd_tx_pdu_nr> > tx_window;

  // Queues, buffers and container
  pdu_retx_queue_list<rlc_amd_retx_nr_t> retx_queue;
  uint32_t         sdu_under_segmentation_sn = INVALID_RLC_SN; // SN of the SDU currently being segmented.
  pdcp_sn_vector_t notify_info_vec;

  // Helper constants
  uint32_t min_hdr_size = 2; // Pre-initialized for 12 bit SN, updated by configure()
  uint32_t so_size      = 2;
  uint32_t max_hdr_size = 4; // Pre-initialized for 12 bit SN, updated by configure()

  /****************************************************************************
   * Tx constants
   * Ref: 3GPP TS 38.322 version 16.2.0 Section 7.2
   ***************************************************************************/
  inline uint32_t tx_window_size() const;

  /****************************************************************************
   * TX timers
   * Ref: 3GPP TS 38.322 version 16.2.0 Section 7.3
   ***************************************************************************/
  srsran::timer_handler::unique_timer poll_retransmit_timer;

public:
  // Getters/Setters
  void set_tx_state(const rlc_am_nr_tx_state_t& st_) { st = st_; }   // This should only be used for testing.
  rlc_am_nr_tx_state_t get_tx_state() { return st; }                 // This should only be used for testing.
  uint32_t get_tx_window_utilization() { return tx_window->size(); } // This should only be used for testing.
  size_t   get_retx_queue_size() const { return retx_queue.size(); } // This should only be used for testing.

  // Debug Helpers
  void debug_state() const;
  void info_state() const;
  void debug_window() const;
};

/****************************************************************************
 * State Variables
 * Ref: 3GPP TS 38.322 version 16.2.0 Section 7.1
 ***************************************************************************/
struct rlc_am_nr_rx_state_t {
  /*
   * RX_Next: This state variable holds the value of the SN following the last in-sequence completely received RLC
   * SDU, and it serves as the lower edge of the receiving window. It is initially set to 0, and is updated whenever
   * the AM RLC entity receives an RLC SDU with SN = RX_Next.
   */
  uint32_t rx_next = 0;
  /*
   * RX_Next_Status_Trigger: This state variable holds the value of the SN following the SN of the RLC SDU which
   * triggered t-Reassembly.
   */
  uint32_t rx_next_status_trigger = 0;
  /*
   * RX_Next_Highest: This state variable holds the highest possible value of the SN which can be indicated by
   *"ACK_SN" when a STATUS PDU needs to be constructed. It is initially set to 0.
   */
  uint32_t rx_highest_status = 0;
  /*
   * RX_Next_Highest: This state variable holds the value of the SN following the SN of the RLC SDU with the
   * highest SN among received RLC SDUs. It is initially set to 0.
   */
  uint32_t rx_next_highest = 0;
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

  void reestablish() final;
  void stop() final;

  // Status PDU
  bool     get_do_status();
  uint32_t get_status_pdu(rlc_am_nr_status_pdu_t* status, uint32_t len);
  uint32_t get_status_pdu_length();

  // Data handling methods
  int  handle_full_data_sdu(const rlc_am_nr_pdu_header_t& header, const uint8_t* payload, uint32_t nof_bytes);
  int  handle_segment_data_sdu(const rlc_am_nr_pdu_header_t& header, const uint8_t* payload, uint32_t nof_bytes);
  bool inside_rx_window(uint32_t sn) const;
  bool valid_ack_sn(uint32_t sn) const;
  void write_to_upper_layers(uint32_t lcid, unique_byte_buffer_t sdu);
  void insert_received_segment(rlc_amd_rx_pdu_nr segment, rlc_amd_rx_sdu_nr_t::segment_list_t& segment_list) const;
  /**
   * @brief update_segment_inventory This function updates the flags has_gap and fully_received of an SDU
   * according to the current inventory of received SDU segments
   * @param rx_sdu The SDU to operate on
   */
  void update_segment_inventory(rlc_amd_rx_sdu_nr_t& rx_sdu) const;

  // Metrics
  uint32_t get_sdu_rx_latency_ms() final;
  uint32_t get_rx_buffered_bytes() final;

  // Timers
  void timer_expired(uint32_t timeout_id);

  // Helpers
  void debug_state() const;
  void debug_window() const;

private:
  rlc_am*           parent = nullptr;
  rlc_am_nr_tx*     tx     = nullptr;
  byte_buffer_pool* pool   = nullptr;

  uint32_t mod_nr = cardinality(rlc_am_nr_sn_size_t());
  uint32_t rx_mod_base_nr(uint32_t sn) const;

  // RX Window
  std::unique_ptr<rlc_ringbuffer_base<rlc_amd_rx_sdu_nr_t> > rx_window;

  // Mutexes
  std::mutex mutex;

  /****************************************************************************
   * Rx timers
   * Ref: 3GPP TS 38.322 version 16.2.0 Section 7.3
   ***************************************************************************/
  srsran::timer_handler::unique_timer status_prohibit_timer;
  srsran::timer_handler::unique_timer reassembly_timer;

  /****************************************************************************
   * Configurable parameters
   * Ref: 3GPP TS 38.322 version 16.2.0 Section 7.4
   ***************************************************************************/
  rlc_am_nr_config_t cfg = {};

  /****************************************************************************
   * Rx state variables
   * Ref: 3GPP TS 38.322 version 16.2.0 Section 7.1
   ***************************************************************************/
  struct rlc_am_nr_rx_state_t st = {};

  /****************************************************************************
   * Rx constants
   * Ref: 3GPP TS 38.322 version 16.2.0 Section 7.2
   ***************************************************************************/
  inline uint32_t rx_window_size() const;

public:
  // Getters/Setters
  void set_rx_state(const rlc_am_nr_rx_state_t& st_) { st = st_; }        // This should only be used for testing.
  rlc_am_nr_rx_state_t get_rx_state() { return st; }                      // This should only be used for testing.
  uint32_t             get_rx_window_size() { return rx_window->size(); } // This should only be used for testing.
};

} // namespace srsran
#endif // SRSRAN_RLC_AM_NR_H
