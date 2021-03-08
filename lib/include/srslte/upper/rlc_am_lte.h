/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_RLC_AM_LTE_H
#define SRSLTE_RLC_AM_LTE_H

#include "srslte/adt/accumulators.h"
#include "srslte/adt/circular_array.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/common/task_scheduler.h"
#include "srslte/common/timeout.h"
#include "srslte/upper/byte_buffer_queue.h"
#include "srslte/upper/rlc_am_base.h"
#include "srslte/upper/rlc_common.h"
#include <deque>
#include <list>
#include <map>

namespace srslte {

#undef RLC_AM_BUFFER_DEBUG

struct rlc_amd_rx_pdu_t {
  rlc_amd_pdu_header_t header;
  unique_byte_buffer_t buf;
  uint32_t             rlc_sn;
};

struct rlc_amd_rx_pdu_segments_t {
  std::list<rlc_amd_rx_pdu_t> segments;
};

struct rlc_amd_tx_pdu_t {
  rlc_amd_pdu_header_t  header;
  unique_byte_buffer_t  buf;
  std::vector<uint32_t> pdcp_sns;
  uint32_t              retx_count;
  uint32_t              rlc_sn;
  bool                  is_acked;
};

struct rlc_amd_retx_t {
  uint32_t sn;
  bool     is_segment;
  uint32_t so_start;
  uint32_t so_end;
};

struct rlc_sn_info_t {
  uint32_t sn;
  bool     is_acked;
};

struct pdcp_sdu_info_t {
  uint32_t sn;
  bool     fully_txed;  // Boolean indicating if the SDU is fully transmitted.
  bool     fully_acked; // Boolean indicating if the SDU is fully acked. This is only necessary temporarely to avoid
                        // duplicate removal from the queue while processing the status report
  std::vector<rlc_sn_info_t> rlc_sn_info_list; // List of RLC PDUs in transit and whether they have been acked or not.
};

template <class T>
struct rlc_ringbuffer_t {
  rlc_ringbuffer_t() { clear(); }
  T& add_pdu(size_t sn)
  {
    assert(not has_sn(sn));
    window[sn].rlc_sn = sn;
    active_flag[sn]   = true;
    count++;
    return window[sn];
  }
  void remove_pdu(size_t sn)
  {
    assert(active_flag[sn]);
    window[sn]      = {};
    active_flag[sn] = false;
    count--;
  }
  T& operator[](size_t sn)
  {
    assert(has_sn(sn));
    return window[sn];
  }
  size_t size() const { return count; }
  bool   empty() const { return count == 0; }
  void   clear()
  {
    std::fill(active_flag.begin(), active_flag.end(), false);
    count = 0;
  }

  bool has_sn(uint32_t sn) const { return active_flag[sn] and (window[sn].rlc_sn == sn); }

  // Return the sum data bytes of all active PDUs (check PDU is non-null)
  uint32_t get_buffered_bytes()
  {
    uint32_t buff_size = 0;
    for (const auto& pdu : window) {
      if (pdu.buf != nullptr) {
        buff_size += pdu.buf->N_bytes;
      }
    }
    return buff_size;
  }

private:
  size_t                                           count       = 0;
  srslte::circular_array<bool, RLC_AM_WINDOW_SIZE> active_flag = {};
  srslte::circular_array<T, RLC_AM_WINDOW_SIZE>    window;
};

struct buffered_pdcp_pdu_list {
public:
  explicit buffered_pdcp_pdu_list();
  void clear();

  void add_pdcp_sdu(uint32_t sn)
  {
    assert(not has_pdcp_sn(sn));
    buffered_pdus[get_idx(sn)].sn = sn;
    count++;
  }
  void clear_pdcp_sdu(uint32_t sn)
  {
    uint32_t sn_idx                   = get_idx(sn);
    buffered_pdus[sn_idx].sn          = invalid_sn;
    buffered_pdus[sn_idx].fully_acked = false;
    buffered_pdus[sn_idx].fully_txed  = false;
    buffered_pdus[sn_idx].rlc_sn_info_list.clear();
    count--;
  }

  pdcp_sdu_info_t& operator[](uint32_t sn)
  {
    assert(has_pdcp_sn(sn));
    return buffered_pdus[get_idx(sn)];
  }
  bool has_pdcp_sn(uint32_t pdcp_sn) const
  {
    assert(pdcp_sn <= max_pdcp_sn or pdcp_sn == status_report_sn);
    return buffered_pdus[get_idx(pdcp_sn)].sn == pdcp_sn;
  }
  uint32_t nof_sdus() const { return count; }

private:
  const static size_t   max_pdcp_sn      = 262143u;
  const static size_t   max_buffer_idx   = 4096u;
  const static uint32_t status_report_sn = std::numeric_limits<uint32_t>::max();
  const static uint32_t invalid_sn       = std::numeric_limits<uint32_t>::max() - 1;

  size_t get_idx(uint32_t sn) const
  {
    return (sn != status_report_sn) ? static_cast<size_t>(sn % max_buffer_idx) : max_buffer_idx;
  }

  // size equal to buffer_size + 1 (last element for Status Report)
  std::vector<pdcp_sdu_info_t> buffered_pdus;
  uint32_t                     count = 0;
};

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
             srslte::timer_handler*     timers_);
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
  int      read_pdu(uint8_t* payload, uint32_t nof_bytes);
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

    int  write_sdu(unique_byte_buffer_t sdu);
    int  read_pdu(uint8_t* payload, uint32_t nof_bytes);
    void discard_sdu(uint32_t discard_sn);
    bool sdu_queue_is_full();

    bool     has_data();
    uint32_t get_buffer_state();

    // Timeout callback interface
    void timer_expired(uint32_t timeout_id);

    // Interface for Rx subclass
    void handle_control_pdu(uint8_t* payload, uint32_t nof_bytes);

    void set_bsr_callback(bsr_callback_t callback);

  private:
    int  build_status_pdu(uint8_t* payload, uint32_t nof_bytes);
    int  build_retx_pdu(uint8_t* payload, uint32_t nof_bytes);
    int  build_segment(uint8_t* payload, uint32_t nof_bytes, rlc_amd_retx_t retx);
    int  build_data_pdu(uint8_t* payload, uint32_t nof_bytes);
    void update_notification_ack_info(const rlc_amd_tx_pdu_t& tx_pdu);

    void debug_state();

    int  required_buffer_size(rlc_amd_retx_t retx);
    void retransmit_pdu();

    // Helpers
    bool poll_required();
    bool do_status();
    void check_sn_reached_max_retx(uint32_t sn);

    rlc_am_lte*           parent = nullptr;
    byte_buffer_pool*     pool   = nullptr;
    srslog::basic_logger& logger;

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

    srslte::timer_handler::unique_timer poll_retx_timer;
    srslte::timer_handler::unique_timer status_prohibit_timer;

    // SDU info for PDCP notifications
    buffered_pdcp_pdu_list undelivered_sdu_info_queue;

    // Callback function for buffer status report
    bsr_callback_t bsr_callback;

    // Tx windows
    rlc_ringbuffer_t<rlc_amd_tx_pdu_t> tx_window;
    pdu_retx_queue                     retx_queue;
    std::vector<uint32_t>              notify_info_vec;

    // Mutexes
    pthread_mutex_t mutex;
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
    void reset_status(); // called when status PDU has been sent

  private:
    void handle_data_pdu(uint8_t* payload, uint32_t nof_bytes, rlc_amd_pdu_header_t& header);
    void handle_data_pdu_segment(uint8_t* payload, uint32_t nof_bytes, rlc_amd_pdu_header_t& header);
    void reassemble_rx_sdus();
    bool inside_rx_window(const int16_t sn);
    void debug_state();
    void print_rx_segments();
    bool add_segment_and_check(rlc_amd_rx_pdu_segments_t* pdu, rlc_amd_rx_pdu_t* segment);

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

    // Mutexes
    pthread_mutex_t mutex;

    // Rx windows
    rlc_ringbuffer_t<rlc_amd_rx_pdu_t>            rx_window;
    std::map<uint32_t, rlc_amd_rx_pdu_segments_t> rx_segments;

    bool poll_received = false;
    bool do_status     = false;

    /****************************************************************************
     * Timers
     * Ref: 3GPP TS 36.322 v10.0.0 Section 7
     ***************************************************************************/

    srslte::timer_handler::unique_timer reordering_timer;

    srslte::rolling_average<double> sdu_rx_latency_ms;
  };

  // Common variables needed/provided by parent class
  srsue::rrc_interface_rlc*  rrc = nullptr;
  srslog::basic_logger&      logger;
  srsue::pdcp_interface_rlc* pdcp   = nullptr;
  srslte::timer_handler*     timers = nullptr;
  uint32_t                   lcid   = 0;
  rlc_config_t               cfg    = {};
  std::string                rb_name;

  static const int poll_periodicity = 8; // After how many data PDUs a status PDU shall be requested

  // Rx and Tx objects
  rlc_am_lte_tx tx;
  rlc_am_lte_rx rx;

  rlc_bearer_metrics_t metrics = {};
};

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 36.322 v10.0.0 Section 6.2.1
 ***************************************************************************/
void rlc_am_read_data_pdu_header(byte_buffer_t* pdu, rlc_amd_pdu_header_t* header);
void rlc_am_read_data_pdu_header(uint8_t** payload, uint32_t* nof_bytes, rlc_amd_pdu_header_t* header);
void rlc_am_write_data_pdu_header(rlc_amd_pdu_header_t* header, byte_buffer_t* pdu);
void rlc_am_write_data_pdu_header(rlc_amd_pdu_header_t* header, uint8_t** payload);
void rlc_am_read_status_pdu(byte_buffer_t* pdu, rlc_status_pdu_t* status);
void rlc_am_read_status_pdu(uint8_t* payload, uint32_t nof_bytes, rlc_status_pdu_t* status);
void rlc_am_write_status_pdu(rlc_status_pdu_t* status, byte_buffer_t* pdu);
int  rlc_am_write_status_pdu(rlc_status_pdu_t* status, uint8_t* payload);

uint32_t           rlc_am_packed_length(rlc_amd_pdu_header_t* header);
uint32_t           rlc_am_packed_length(rlc_status_pdu_t* status);
uint32_t           rlc_am_packed_length(rlc_amd_retx_t retx);
bool               rlc_am_is_valid_status_pdu(const rlc_status_pdu_t& status);
bool               rlc_am_is_pdu_segment(uint8_t* payload);
std::string        rlc_am_undelivered_sdu_info_to_string(const std::map<uint32_t, pdcp_sdu_info_t>& info_queue);
fmt::memory_buffer rlc_am_status_pdu_to_string(rlc_status_pdu_t* status);
fmt::memory_buffer rlc_amd_pdu_header_to_string(const rlc_amd_pdu_header_t& header);
bool               rlc_am_start_aligned(const uint8_t fi);
bool               rlc_am_end_aligned(const uint8_t fi);
bool               rlc_am_is_unaligned(const uint8_t fi);
bool               rlc_am_not_start_aligned(const uint8_t fi);

} // namespace srslte

#endif // SRSLTE_RLC_AM_LTE_H
