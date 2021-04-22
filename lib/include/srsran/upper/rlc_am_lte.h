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

#ifndef SRSRAN_RLC_AM_LTE_H
#define SRSRAN_RLC_AM_LTE_H

#include "srsran/adt/accumulators.h"
#include "srsran/adt/circular_array.h"
#include "srsran/adt/circular_map.h"
#include "srsran/adt/intrusive_list.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/srsran_assert.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/common/timeout.h"
#include "srsran/interfaces/pdcp_interface_types.h"
#include "srsran/upper/byte_buffer_queue.h"
#include "srsran/upper/rlc_am_base.h"
#include "srsran/upper/rlc_common.h"
#include <deque>
#include <list>
#include <map>

namespace srsran {

#undef RLC_AM_BUFFER_DEBUG

class rlc_amd_tx_pdu;
class pdcp_pdu_info;

/// Pool that manages the allocation of RLC AM PDU Segments to RLC PDUs and tracking of segments ACK state
struct rlc_am_pdu_segment_pool {
  const static size_t MAX_POOL_SIZE = 16384;
  using rlc_list_tag                = default_intrusive_tag;
  struct free_list_tag {};

  /// RLC AM PDU Segment, containing the PDCP SN and RLC SN it has been assigned to, and its current ACK state
  struct segment_resource : public intrusive_forward_list_element<rlc_list_tag>,
                            public intrusive_forward_list_element<free_list_tag>,
                            public intrusive_double_linked_list_element<> {
    const static uint32_t invalid_rlc_sn  = std::numeric_limits<uint32_t>::max();
    const static uint32_t invalid_pdcp_sn = std::numeric_limits<uint32_t>::max() - 1; // -1 for Status Report

    int      id() const;
    void     release_pdcp_sn();
    void     release_rlc_sn();
    uint32_t rlc_sn() const { return rlc_sn_; }
    uint32_t pdcp_sn() const { return pdcp_sn_; }
    bool     empty() const { return rlc_sn_ == invalid_rlc_sn and pdcp_sn_ == invalid_pdcp_sn; }

  private:
    friend struct rlc_am_pdu_segment_pool;
    uint32_t                 rlc_sn_     = invalid_rlc_sn;
    uint32_t                 pdcp_sn_    = invalid_pdcp_sn;
    rlc_am_pdu_segment_pool* parent_pool = nullptr;
  };

  rlc_am_pdu_segment_pool();
  rlc_am_pdu_segment_pool(const rlc_am_pdu_segment_pool&) = delete;
  rlc_am_pdu_segment_pool(rlc_am_pdu_segment_pool&&)      = delete;
  rlc_am_pdu_segment_pool& operator=(const rlc_am_pdu_segment_pool&) = delete;
  rlc_am_pdu_segment_pool& operator=(rlc_am_pdu_segment_pool&&) = delete;
  bool                     has_segments() const { return not free_list.empty(); }
  bool                     make_segment(rlc_amd_tx_pdu& rlc_list, pdcp_pdu_info& pdcp_info);

private:
  intrusive_forward_list<segment_resource, free_list_tag> free_list;
  std::array<segment_resource, MAX_POOL_SIZE>             segments;
};

/// RLC AM PDU Segment, containing the PDCP SN and RLC SN it has been assigned to, and its current ACK state
using rlc_am_pdu_segment = rlc_am_pdu_segment_pool::segment_resource;

struct rlc_amd_rx_pdu {
  rlc_amd_pdu_header_t header;
  unique_byte_buffer_t buf;
  uint32_t             rlc_sn;

  rlc_amd_rx_pdu() = default;
  explicit rlc_amd_rx_pdu(uint32_t rlc_sn_) : rlc_sn(rlc_sn_) {}
};

struct rlc_amd_rx_pdu_segments_t {
  std::list<rlc_amd_rx_pdu> segments;
};

/// Class that contains the parameters and state (e.g. segments) of a RLC PDU
class rlc_amd_tx_pdu
{
  using list_type                      = intrusive_forward_list<rlc_am_pdu_segment>;
  const static uint32_t invalid_rlc_sn = std::numeric_limits<uint32_t>::max();

  list_type list;

public:
  using iterator       = typename list_type::iterator;
  using const_iterator = typename list_type::const_iterator;

  const uint32_t       rlc_sn     = invalid_rlc_sn;
  uint32_t             retx_count = 0;
  rlc_amd_pdu_header_t header;
  unique_byte_buffer_t buf;

  explicit rlc_amd_tx_pdu(uint32_t rlc_sn_) : rlc_sn(rlc_sn_) {}
  rlc_amd_tx_pdu(const rlc_amd_tx_pdu&)           = delete;
  rlc_amd_tx_pdu(rlc_amd_tx_pdu&& other) noexcept = default;
  rlc_amd_tx_pdu& operator=(const rlc_amd_tx_pdu& other) = delete;
  rlc_amd_tx_pdu& operator=(rlc_amd_tx_pdu&& other) = delete;
  ~rlc_amd_tx_pdu();

  // Segment List Interface
  void           add_segment(rlc_am_pdu_segment& segment) { list.push_front(&segment); }
  const_iterator begin() const { return list.begin(); }
  const_iterator end() const { return list.end(); }
  iterator       begin() { return list.begin(); }
  iterator       end() { return list.end(); }
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

/// Class that contains the parameters and state (e.g. unACKed segments) of a PDCP PDU
class pdcp_pdu_info
{
  using list_type = intrusive_double_linked_list<rlc_am_pdu_segment>;

  list_type list; // List of unACKed RLC PDUs that contain segments that belong to the PDCP PDU.

public:
  const static uint32_t status_report_sn = std::numeric_limits<uint32_t>::max();
  const static uint32_t invalid_pdcp_sn  = std::numeric_limits<uint32_t>::max() - 1;

  using iterator       = typename list_type::iterator;
  using const_iterator = typename list_type::const_iterator;

  // Copy is forbidden to avoid multiple PDCP SN references to the same segment
  pdcp_pdu_info()                              = default;
  pdcp_pdu_info(pdcp_pdu_info&&) noexcept      = default;
  pdcp_pdu_info(const pdcp_pdu_info&) noexcept = delete;
  pdcp_pdu_info& operator=(const pdcp_pdu_info&) noexcept = delete;
  pdcp_pdu_info& operator=(pdcp_pdu_info&&) noexcept = default;
  ~pdcp_pdu_info() { clear(); }

  uint32_t sn         = invalid_pdcp_sn;
  bool     fully_txed = false; // Boolean indicating if the SDU is fully transmitted.

  bool fully_acked() const { return fully_txed and list.empty(); }
  bool valid() const { return sn != invalid_pdcp_sn; }

  // Interface for list of unACKed RLC segments of the PDCP PDU
  void add_segment(rlc_am_pdu_segment& segment) { list.push_front(&segment); }
  void ack_segment(rlc_am_pdu_segment& segment);
  void clear()
  {
    sn         = invalid_pdcp_sn;
    fully_txed = false;
    while (not list.empty()) {
      ack_segment(list.front());
    }
  }
  const_iterator begin() const { return list.begin(); }
  const_iterator end() const { return list.end(); }
};

template <class T>
struct rlc_ringbuffer_t {
  T& add_pdu(size_t sn)
  {
    srsran_expect(not has_sn(sn), "The same SN=%zd should not be added twice", sn);
    window.overwrite(sn, T(sn));
    return window[sn];
  }
  void remove_pdu(size_t sn)
  {
    srsran_expect(has_sn(sn), "The removed SN=%zd is not in the window", sn);
    window.erase(sn);
  }
  T&     operator[](size_t sn) { return window[sn]; }
  size_t size() const { return window.size(); }
  bool   empty() const { return window.empty(); }
  void   clear() { window.clear(); }

  bool has_sn(uint32_t sn) const { return window.contains(sn); }

  // Return the sum data bytes of all active PDUs (check PDU is non-null)
  uint32_t get_buffered_bytes()
  {
    uint32_t buff_size = 0;
    for (const auto& pdu : window) {
      if (pdu.second.buf != nullptr) {
        buff_size += pdu.second.buf->N_bytes;
      }
    }
    return buff_size;
  }

private:
  srsran::static_circular_map<uint32_t, T, RLC_AM_WINDOW_SIZE> window;
};

struct buffered_pdcp_pdu_list {
public:
  explicit buffered_pdcp_pdu_list();
  void clear();

  void add_pdcp_sdu(uint32_t sn)
  {
    srsran_expect(sn <= max_pdcp_sn or sn == status_report_sn, "Invalid PDCP SN=%d", sn);
    srsran_assert(not has_pdcp_sn(sn), "Cannot re-add same PDCP SN twice");
    pdcp_pdu_info& pdu = get_pdu_(sn);
    if (pdu.valid()) {
      pdu.clear();
      count--;
    }
    pdu.sn = sn;
    count++;
  }
  void clear_pdcp_sdu(uint32_t sn)
  {
    pdcp_pdu_info& pdu = get_pdu_(sn);
    if (not pdu.valid()) {
      return;
    }
    pdu.clear();
    count--;
  }

  pdcp_pdu_info& operator[](uint32_t sn)
  {
    srsran_expect(has_pdcp_sn(sn), "Invalid access to non-existent PDCP SN=%d", sn);
    return get_pdu_(sn);
  }
  bool has_pdcp_sn(uint32_t pdcp_sn) const
  {
    srsran_expect(pdcp_sn <= max_pdcp_sn or pdcp_sn == status_report_sn, "Invalid PDCP SN=%d", pdcp_sn);
    return get_pdu_(pdcp_sn).sn == pdcp_sn;
  }
  uint32_t nof_sdus() const { return count; }

private:
  const static size_t   max_pdcp_sn      = 262143u;
  const static size_t   buffer_size      = 4096u;
  const static uint32_t status_report_sn = pdcp_pdu_info::status_report_sn;

  pdcp_pdu_info& get_pdu_(uint32_t sn)
  {
    return (sn == status_report_sn) ? status_report_pdu : buffered_pdus[static_cast<size_t>(sn % buffer_size)];
  }
  const pdcp_pdu_info& get_pdu_(uint32_t sn) const
  {
    return (sn == status_report_sn) ? status_report_pdu : buffered_pdus[static_cast<size_t>(sn % buffer_size)];
  }

  // size equal to buffer_size
  std::vector<pdcp_pdu_info> buffered_pdus;
  pdcp_pdu_info              status_report_pdu;
  uint32_t                   count = 0;
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
    void update_notification_ack_info(uint32_t rlc_sn);

    void debug_state();

    int  required_buffer_size(rlc_amd_retx_t retx);
    void retransmit_pdu();

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
    rlc_ringbuffer_t<rlc_amd_tx_pdu> tx_window;
    pdu_retx_queue                   retx_queue;
    pdcp_sn_vector_t                 notify_info_vec;

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
    void reset_status(); // called when status PDU has been sent

  private:
    void handle_data_pdu(uint8_t* payload, uint32_t nof_bytes, rlc_amd_pdu_header_t& header);
    void handle_data_pdu_segment(uint8_t* payload, uint32_t nof_bytes, rlc_amd_pdu_header_t& header);
    void reassemble_rx_sdus();
    bool inside_rx_window(const int16_t sn);
    void debug_state();
    void print_rx_segments();
    bool add_segment_and_check(rlc_amd_rx_pdu_segments_t* pdu, rlc_amd_rx_pdu* segment);

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
    rlc_ringbuffer_t<rlc_amd_rx_pdu>              rx_window;
    std::map<uint32_t, rlc_amd_rx_pdu_segments_t> rx_segments;

    bool poll_received = false;
    bool do_status     = false;

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

uint32_t    rlc_am_packed_length(rlc_amd_pdu_header_t* header);
uint32_t    rlc_am_packed_length(rlc_status_pdu_t* status);
uint32_t    rlc_am_packed_length(rlc_amd_retx_t retx);
bool        rlc_am_is_valid_status_pdu(const rlc_status_pdu_t& status);
bool        rlc_am_is_pdu_segment(uint8_t* payload);
std::string rlc_am_undelivered_sdu_info_to_string(const std::map<uint32_t, pdcp_pdu_info>& info_queue);
void        log_rlc_amd_pdu_header_to_string(srslog::log_channel& log_ch, const rlc_amd_pdu_header_t& header);
bool        rlc_am_start_aligned(const uint8_t fi);
bool        rlc_am_end_aligned(const uint8_t fi);
bool        rlc_am_is_unaligned(const uint8_t fi);
bool        rlc_am_not_start_aligned(const uint8_t fi);

} // namespace srsran

#endif // SRSRAN_RLC_AM_LTE_H
