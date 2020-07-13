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

#ifndef SRSLTE_RLC_AM_LTE_H
#define SRSLTE_RLC_AM_LTE_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/common/timeout.h"
#include "srslte/interfaces/ue_interfaces.h"
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
};

struct rlc_amd_rx_pdu_segments_t {
  std::list<rlc_amd_rx_pdu_t> segments;
};

struct rlc_amd_tx_pdu_t {
  rlc_amd_pdu_header_t header;
  unique_byte_buffer_t buf;
  uint32_t             retx_count;
  bool                 is_acked;
};

struct rlc_amd_retx_t {
  uint32_t sn;
  bool     is_segment;
  uint32_t so_start;
  uint32_t so_end;
};

class rlc_am_lte : public rlc_common
{
public:
  rlc_am_lte(srslte::log_ref            log_,
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
    uint32_t get_num_tx_bytes();
    void     reset_metrics();

    // Timeout callback interface
    void timer_expired(uint32_t timeout_id);

    // Interface for Rx subclass
    void handle_control_pdu(uint8_t* payload, uint32_t nof_bytes);

    void set_bsr_callback(bsr_callback_t callback);

  private:
    int build_status_pdu(uint8_t* payload, uint32_t nof_bytes);
    int build_retx_pdu(uint8_t* payload, uint32_t nof_bytes);
    int build_segment(uint8_t* payload, uint32_t nof_bytes, rlc_amd_retx_t retx);
    int build_data_pdu(uint8_t* payload, uint32_t nof_bytes);

    void debug_state();

    bool retx_queue_has_sn(uint32_t sn);
    int  required_buffer_size(rlc_amd_retx_t retx);
    void retransmit_random_pdu();

    // Helpers
    bool poll_required();
    bool do_status();

    rlc_am_lte*       parent = nullptr;
    byte_buffer_pool* pool   = nullptr;
    srslte::log_ref   log;

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

    // Callback function for buffer status report
    bsr_callback_t bsr_callback;

    // Tx windows
    std::map<uint32_t, rlc_amd_tx_pdu_t> tx_window;
    std::deque<rlc_amd_retx_t>           retx_queue;

    // Mutexes
    pthread_mutex_t mutex;

    // Metrics
    uint32_t num_tx_bytes = 0;
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

    uint32_t get_num_rx_bytes();
    void     reset_metrics();

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

    rlc_am_lte*       parent = nullptr;
    byte_buffer_pool* pool   = nullptr;
    srslte::log_ref   log;

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
    std::map<uint32_t, rlc_amd_rx_pdu_t>          rx_window;
    std::map<uint32_t, rlc_amd_rx_pdu_segments_t> rx_segments;

    // Metrics
    uint32_t num_rx_bytes = 0;

    bool poll_received = false;
    bool do_status     = false;

    /****************************************************************************
     * Timers
     * Ref: 3GPP TS 36.322 v10.0.0 Section 7
     ***************************************************************************/

    srslte::timer_handler::unique_timer reordering_timer;
  };

  // Common variables needed/provided by parent class
  srsue::rrc_interface_rlc*  rrc = nullptr;
  srslte::log_ref            log;
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

uint32_t    rlc_am_packed_length(rlc_amd_pdu_header_t* header);
uint32_t    rlc_am_packed_length(rlc_status_pdu_t* status);
uint32_t    rlc_am_packed_length(rlc_amd_retx_t retx);
bool        rlc_am_is_valid_status_pdu(const rlc_status_pdu_t& status);
bool        rlc_am_is_pdu_segment(uint8_t* payload);
std::string rlc_am_status_pdu_to_string(rlc_status_pdu_t* status);
std::string rlc_amd_pdu_header_to_string(const rlc_amd_pdu_header_t& header);
bool        rlc_am_start_aligned(const uint8_t fi);
bool        rlc_am_end_aligned(const uint8_t fi);
bool        rlc_am_is_unaligned(const uint8_t fi);
bool        rlc_am_not_start_aligned(const uint8_t fi);

} // namespace srslte

#endif // SRSLTE_RLC_AM_LTE_H
