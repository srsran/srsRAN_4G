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

class pdu_retx_queue
{
public:
  rlc_amd_retx_t& push()
  {
    assert(not full());
    rlc_amd_retx_t& p = buffer[wpos];
    wpos              = (wpos + 1) % RLC_AM_WINDOW_SIZE;
    return p;
  }

  void pop() { rpos = (rpos + 1) % RLC_AM_WINDOW_SIZE; }

  rlc_amd_retx_t& front()
  {
    assert(not empty());
    return buffer[rpos];
  }

  void clear()
  {
    wpos = 0;
    rpos = 0;
  }

  bool has_sn(uint32_t sn) const
  {
    for (size_t i = rpos; i != wpos; i = (i + 1) % RLC_AM_WINDOW_SIZE) {
      if (buffer[i].sn == sn) {
        return true;
      }
    }
    return false;
  }

  size_t size() const { return (wpos >= rpos) ? wpos - rpos : RLC_AM_WINDOW_SIZE + wpos - rpos; }
  bool   empty() const { return wpos == rpos; }
  bool   full() const { return size() == RLC_AM_WINDOW_SIZE - 1; }

private:
  std::array<rlc_amd_retx_t, RLC_AM_WINDOW_SIZE> buffer;
  size_t                                         wpos = 0;
  size_t                                         rpos = 0;
};

class rlc_am_lte : public rlc_common
{
public:
  rlc_am_lte(srslog::basic_logger&      logger,
             uint32_t                   lcid_,
             srsue::pdcp_interface_rlc* pdcp_,
             srsue::rrc_interface_rlc*  rrc_,
             srsran::timer_handler*     timers_);

  bool configure(const rlc_config_t& cfg_);
  void reestablish();
  void stop();

  void empty_queue();

  rlc_mode_t get_mode();
  uint32_t   get_bearer();

  // PDCP interface
  void write_sdu(unique_byte_buffer_t sdu);
  void discard_sdu(uint32_t pdcp_sn);
  bool sdu_queue_is_full();

  // MAC interface
  bool     has_data();
  uint32_t get_buffer_state();
  void     get_buffer_state(uint32_t& tx_queue, uint32_t& prio_tx_queue);
  uint32_t read_pdu(uint8_t* payload, uint32_t nof_bytes);
  void     write_pdu(uint8_t* payload, uint32_t nof_bytes);

  rlc_bearer_metrics_t get_metrics();
  void                 reset_metrics();

  void set_bsr_callback(bsr_callback_t callback);

private:
  // Transmitter sub-class
  class rlc_am_lte_tx : public timer_callback
  {
  public:
    rlc_am_lte_tx(rlc_am_lte* parent_);
    ~rlc_am_lte_tx();

    bool configure(const rlc_config_t& cfg_);

    void empty_queue();
    void reestablish();
    void stop();

    int      write_sdu(unique_byte_buffer_t sdu);
    uint32_t read_pdu(uint8_t* payload, uint32_t nof_bytes);
    void     discard_sdu(uint32_t discard_sn);
    bool     sdu_queue_is_full();

    bool     has_data();
    uint32_t get_buffer_state();
    void     get_buffer_state(uint32_t& new_tx, uint32_t& prio_tx);

    // Timeout callback interface
    void timer_expired(uint32_t timeout_id);

    // Interface for Rx subclass
    void handle_control_pdu(uint8_t* payload, uint32_t nof_bytes);

    void set_bsr_callback(bsr_callback_t callback);

  private:
    void stop_nolock();

    int  build_status_pdu(uint8_t* payload, uint32_t nof_bytes);
    int  build_retx_pdu(uint8_t* payload, uint32_t nof_bytes);
    int  build_segment(uint8_t* payload, uint32_t nof_bytes, rlc_amd_retx_t retx);
    int  build_data_pdu(uint8_t* payload, uint32_t nof_bytes);
    void update_notification_ack_info(uint32_t rlc_sn);

    void debug_state();
    void empty_queue_nolock();

    int  required_buffer_size(const rlc_amd_retx_t& retx);
    void retransmit_pdu(uint32_t sn);

    void get_buffer_state_nolock(uint32_t& new_tx, uint32_t& prio_tx);

    // Helpers
    bool poll_required();
    bool do_status();
    void check_sn_reached_max_retx(uint32_t sn);

    rlc_am_lte*             parent = nullptr;
    byte_buffer_pool*       pool   = nullptr;
    srslog::basic_logger&   logger;
    rlc_am_pdu_segment_pool segment_pool;

    /****************************************************************************
     * Configurable parameters
     * Ref: 3GPP TS 36.322 v10.0.0 Section 7
     ***************************************************************************/

    rlc_am_config_t cfg = {};

    // TX SDU buffers
    byte_buffer_queue    tx_sdu_queue;
    unique_byte_buffer_t tx_sdu;

    bool tx_enabled = false;

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
    buffered_pdcp_pdu_list undelivered_sdu_info_queue;

    // Callback function for buffer status report
    bsr_callback_t bsr_callback;

    // Tx windows
    rlc_ringbuffer_t<rlc_amd_tx_pdu, RLC_AM_WINDOW_SIZE> tx_window;
    pdu_retx_queue                                       retx_queue;
    pdcp_sn_vector_t                                     notify_info_vec;

    // Mutexes
    std::mutex mutex;

    // default to RLC SDU queue length
    const uint32_t MAX_SDUS_PER_RLC_PDU = RLC_TX_QUEUE_LEN;
  };

  // Receiver sub-class
  class rlc_am_lte_rx : public timer_callback
  {
  public:
    rlc_am_lte_rx(rlc_am_lte* parent_);
    ~rlc_am_lte_rx();

    bool configure(rlc_am_config_t cfg_);
    void reestablish();
    void stop();

    void write_pdu(uint8_t* payload, uint32_t nof_bytes);

    uint32_t get_rx_buffered_bytes(); // returns sum of PDUs in rx_window
    uint32_t get_sdu_rx_latency_ms();

    // Timeout callback interface
    void timer_expired(uint32_t timeout_id);

    // Functions needed by Tx subclass to query rx state
    int  get_status_pdu_length();
    int  get_status_pdu(rlc_status_pdu_t* status, const uint32_t nof_bytes);
    bool get_do_status();

  private:
    void handle_data_pdu(uint8_t* payload, uint32_t nof_bytes, rlc_amd_pdu_header_t& header);
    void handle_data_pdu_segment(uint8_t* payload, uint32_t nof_bytes, rlc_amd_pdu_header_t& header);
    void reassemble_rx_sdus();
    bool inside_rx_window(const int16_t sn);
    void debug_state();
    void print_rx_segments();
    bool add_segment_and_check(rlc_amd_rx_pdu_segments_t* pdu, rlc_amd_rx_pdu* segment);
    void reset_status();

    rlc_am_lte*           parent = nullptr;
    byte_buffer_pool*     pool   = nullptr;
    srslog::basic_logger& logger;

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
  rlc_am_lte_tx tx;
  rlc_am_lte_rx rx;

  std::mutex           metrics_mutex;
  rlc_bearer_metrics_t metrics = {};
};

} // namespace srsran

#endif // SRSRAN_RLC_AM_LTE_H
