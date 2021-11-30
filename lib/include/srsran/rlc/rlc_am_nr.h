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

/****************************************************************************
 * Tx state variables
 * Ref: 3GPP TS 38.322 v16.2.0 Section 7.1
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

  int  write_sdu(unique_byte_buffer_t sdu);
  void empty_queue() final;

  // Data PDU helpers
  using rlc_amd_tx_pdu_nr = rlc_amd_tx_pdu<rlc_am_nr_pdu_header_t>;
  /*
  struct rlc_amd_tx_pdu_nr {
    const uint32_t rlc_sn  = INVALID_RLC_SN;
    const uint32_t pdcp_sn = INVALID_RLC_SN;
    struct tx_pdu_segment {
      rlc_am_nr_pdu_header_t header     = {};
      uint32_t               retx_count = 0;
      uint32_t               so         = 0;
      uint32_t               len        = 0;
    };
  };*/
  int build_new_sdu_segment(const unique_byte_buffer_t& tx_sdu,
                            rlc_amd_tx_pdu_nr&          tx_pdu,
                            uint8_t*                    payload,
                            uint32_t                    nof_bytes);
  int build_continuation_sdu_segment(rlc_amd_tx_pdu_nr& tx_pdu, uint8_t* payload, uint32_t nof_bytes);
  int build_retx_pdu(unique_byte_buffer_t& tx_pdu, uint32_t nof_bytes);

  // Buffer State
  bool     has_data() final;
  uint32_t get_buffer_state() final;
  void     get_buffer_state(uint32_t& tx_queue, uint32_t& prio_tx_queue) final;

  // Status PDU
  bool     do_status();
  uint32_t build_status_pdu(byte_buffer_t* payload, uint32_t nof_bytes);

  // Polling
  uint8_t get_pdu_poll();

  void stop() final;

  bool inside_tx_window(uint32_t sn);

private:
  rlc_am*       parent = nullptr;
  rlc_am_nr_rx* rx     = nullptr;

  uint32_t        mod_nr = 4096;
  inline uint32_t tx_mod_base_nr(uint32_t sn) const;

  /****************************************************************************
   * Configurable parameters
   * Ref: 3GPP TS 38.322 v16.2.0 Section 7.4
   ***************************************************************************/
  rlc_am_nr_config_t cfg = {};

  /****************************************************************************
   * Tx state variables
   * Ref: 3GPP TS 38.322 v16.2.0 Section 7.1
   ***************************************************************************/
  struct rlc_am_nr_tx_state_t                             st = {};
  rlc_ringbuffer_t<rlc_amd_tx_pdu_nr, RLC_AM_WINDOW_SIZE> tx_window;

  // Queues and buffers
  pdu_retx_queue<RLC_AM_WINDOW_SIZE> retx_queue;
  rlc_amd_tx_sdu_nr_t                current_sdu; // Currently SDU beind segmented

public:
  // Getters/Setters
  void set_tx_state(const rlc_am_nr_tx_state_t& st_) { st = st_; }       // This should only be used for testing.
  rlc_am_nr_tx_state_t get_tx_state() { return st; }                     // This should only be used for testing.
  uint32_t             get_tx_window_size() { return tx_window.size(); } // This should only be used for testing.
};

/****************************************************************************
 * RX State Variables
 * Ref: 3GPP TS 38.322 v16.2.0 Section 7.1
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

  void stop();
  void reestablish();

  // Status PDU
  bool     get_do_status();
  uint32_t get_status_pdu(rlc_am_nr_status_pdu_t* status, uint32_t len);
  uint32_t get_status_pdu_length();

  // Data handling methods
  void handle_data_pdu_full(uint8_t* payload, uint32_t nof_bytes, rlc_am_nr_pdu_header_t& header);
  bool inside_rx_window(uint32_t sn);
  void write_to_upper_layers(uint32_t lcid, unique_byte_buffer_t sdu);

  // Metrics
  uint32_t get_sdu_rx_latency_ms() final;
  uint32_t get_rx_buffered_bytes() final;

  // Timers
  void timer_expired(uint32_t timeout_id);

  // Helpers
  void debug_state();

private:
  rlc_am*           parent = nullptr;
  rlc_am_nr_tx*     tx     = nullptr;
  byte_buffer_pool* pool   = nullptr;

  uint32_t mod_nr = 4096;
  uint32_t rx_mod_base_nr(uint32_t sn) const;

  // RX Window
  rlc_ringbuffer_t<rlc_amd_rx_sdu_nr_t, RLC_AM_WINDOW_SIZE> rx_window;

  // Mutexes
  std::mutex mutex;

  /****************************************************************************
   * Rx timers
   * Ref: 3GPP TS 38.322 v16.2.0 Section 7.3
   ***************************************************************************/
  srsran::timer_handler::unique_timer status_prohibit_timer;
  srsran::timer_handler::unique_timer reassembly_timer;

  /****************************************************************************
   * Configurable parameters
   * Ref: 3GPP TS 38.322 v16.2.0 Section 7.4
   ***************************************************************************/
  rlc_am_nr_config_t cfg = {};

  /****************************************************************************
   * Tx state variables
   * Ref: 3GPP TS 38.322 v16.2.0 Section 7.1
   ***************************************************************************/
  struct rlc_am_nr_rx_state_t st = {};

public:
  // Getters/Setters
  void set_rx_state(const rlc_am_nr_rx_state_t& st_) { st = st_; }       // This should only be used for testing.
  rlc_am_nr_rx_state_t get_rx_state() { return st; }                     // This should only be used for testing.
  uint32_t             get_rx_window_size() { return rx_window.size(); } // This should only be used for testing.
};

} // namespace srsran
#endif // SRSRAN_RLC_AM_NR_H
