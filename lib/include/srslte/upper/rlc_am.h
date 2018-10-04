/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_RLC_AM_H
#define SRSLTE_RLC_AM_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/log.h"
#include "srslte/common/common.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/upper/rlc_tx_queue.h"
#include "srslte/common/timeout.h"
#include "srslte/upper/rlc_common.h"
#include <map>
#include <deque>
#include <list>

namespace srslte {

#undef RLC_AM_BUFFER_DEBUG

struct rlc_amd_rx_pdu_t{
  rlc_amd_pdu_header_t  header;
  byte_buffer_t         *buf;
};

struct rlc_amd_rx_pdu_segments_t{
  std::list<rlc_amd_rx_pdu_t> segments;
};

struct rlc_amd_tx_pdu_t{
  rlc_amd_pdu_header_t  header;
  byte_buffer_t        *buf;
  uint32_t              retx_count;
  bool                  is_acked;
};

struct rlc_amd_retx_t{
  uint32_t  sn;
  bool      is_segment;
  uint32_t  so_start;
  uint32_t  so_end;
};


class rlc_am : public rlc_common
{
public:
  rlc_am(uint32_t queue_len = 128);
  ~rlc_am();
  void init(log                   *log_,
            uint32_t              lcid_,
            srsue::pdcp_interface_rlc   *pdcp_,
            srsue::rrc_interface_rlc    *rrc_,
            mac_interface_timers *mac_timers_);
  bool configure(srslte_rlc_config_t cfg_);
  void reestablish();
  void stop();

  void empty_queue(); 
  
  rlc_mode_t    get_mode();
  uint32_t      get_bearer();

  // PDCP interface
  void write_sdu(byte_buffer_t *sdu, bool blocking = true);

  // MAC interface
  uint32_t get_buffer_state();
  uint32_t get_total_buffer_state(); 
  int      read_pdu(uint8_t *payload, uint32_t nof_bytes);
  void     write_pdu(uint8_t *payload, uint32_t nof_bytes);

  uint32_t get_num_tx_bytes();
  uint32_t get_num_rx_bytes();
  void reset_metrics();

private:

  // Transmitter sub-class
  class rlc_am_tx : public timer_callback
  {
  public:
    rlc_am_tx(rlc_am *parent_, uint32_t queue_len_);
    ~rlc_am_tx();

    void init();
    bool configure(srslte_rlc_am_config_t cfg_);

    void empty_queue();
    void reestablish();
    void stop();

    void write_sdu(byte_buffer_t *sdu, bool blocking);
    int read_pdu(uint8_t *payload, uint32_t nof_bytes);

    uint32_t get_buffer_state();
    uint32_t get_total_buffer_state();
    uint32_t get_num_tx_bytes();
    void reset_metrics();

    // Timeout callback interface
    void timer_expired(uint32_t timeout_id);

    // Interface for Rx subclass
    void handle_control_pdu(uint8_t *payload, uint32_t nof_bytes);

  private:

    int  build_status_pdu(uint8_t *payload, uint32_t nof_bytes);
    int  build_retx_pdu(uint8_t *payload, uint32_t nof_bytes);
    int  build_segment(uint8_t *payload, uint32_t nof_bytes, rlc_amd_retx_t retx);
    int  build_data_pdu(uint8_t *payload, uint32_t nof_bytes);

    void debug_state();

    bool retx_queue_has_sn(uint32_t sn);
    int  required_buffer_size(rlc_amd_retx_t retx);
    void retransmit_random_pdu();

    // Timer checks
    bool status_prohibited;

    // Helpers
    bool poll_required();
    bool do_status();

    rlc_am                    *parent;
    byte_buffer_pool          *pool;
    srslte::log               *log;

    /****************************************************************************
     * Configurable parameters
     * Ref: 3GPP TS 36.322 v10.0.0 Section 7
     ***************************************************************************/

    srslte_rlc_am_config_t cfg;

    // TX SDU buffers
    rlc_tx_queue   tx_sdu_queue;
    byte_buffer_t *tx_sdu;;

    bool           tx_enabled;

    /****************************************************************************
     * State variables and counters
     * Ref: 3GPP TS 36.322 v10.0.0 Section 7
     ***************************************************************************/

    // Tx state variables
    uint32_t vt_a;    // ACK state. SN of next PDU in sequence to be ACKed. Low edge of tx window.
    uint32_t vt_ms;   // Max send state. High edge of tx window. vt_a + window_size.
    uint32_t vt_s;    // Send state. SN to be assigned for next PDU.
    uint32_t poll_sn; // Poll send state. SN of most recent PDU txed with poll bit set.

    // Tx counters
    uint32_t pdu_without_poll;
    uint32_t byte_without_poll;

    rlc_status_pdu_t tx_status;

    /****************************************************************************
     * Timers
     * Ref: 3GPP TS 36.322 v10.0.0 Section 7
     ***************************************************************************/

    srslte::timers::timer *poll_retx_timer;
    uint32_t               poll_retx_timer_id;

    srslte::timers::timer *status_prohibit_timer;
    uint32_t               status_prohibit_timer_id;

    // Tx windows
    std::map<uint32_t, rlc_amd_tx_pdu_t>          tx_window;
    std::deque<rlc_amd_retx_t>                    retx_queue;

    // Mutexes
    pthread_mutex_t     mutex;

    // Metrics
    uint32_t            num_tx_bytes;
  };

  // Receiver sub-class
  class rlc_am_rx : public timer_callback
  {
  public:
    rlc_am_rx(rlc_am* parent_);
    ~rlc_am_rx();

    void init();
    bool configure(srslte_rlc_am_config_t cfg_);
    void reestablish();
    void stop();

    void write_pdu(uint8_t *payload, uint32_t nof_bytes);

    uint32_t get_num_rx_bytes();
    void reset_metrics();

    // Timeout callback interface
    void timer_expired(uint32_t timeout_id);

    // Functions needed by Tx subclass to query rx state
    int get_status_pdu_length();
    int get_status_pdu(rlc_status_pdu_t* status, const uint32_t nof_bytes);
    bool get_do_status();
    void reset_status(); // called when status PDU has been sent

  private:
    void handle_data_pdu(uint8_t *payload, uint32_t nof_bytes, rlc_amd_pdu_header_t &header);
    void handle_data_pdu_segment(uint8_t *payload, uint32_t nof_bytes, rlc_amd_pdu_header_t &header);
    void reassemble_rx_sdus();
    bool inside_rx_window(uint16_t sn);
    void debug_state();
    void print_rx_segments();
    bool add_segment_and_check(rlc_amd_rx_pdu_segments_t *pdu, rlc_amd_rx_pdu_t *segment);

    rlc_am                    *parent;
    byte_buffer_pool          *pool;
    srslte::log               *log;

    /****************************************************************************
     * Configurable parameters
     * Ref: 3GPP TS 36.322 v10.0.0 Section 7
     ***************************************************************************/
    srslte_rlc_am_config_t cfg;

    // RX SDU buffers
    byte_buffer_t *rx_sdu;

    /****************************************************************************
     * State variables and counters
     * Ref: 3GPP TS 36.322 v10.0.0 Section 7
     ***************************************************************************/

    // Rx state variables
    uint32_t vr_r;  // Receive state. SN following last in-sequence received PDU. Low edge of rx window
    uint32_t vr_mr; // Max acceptable receive state. High edge of rx window. vr_r + window size.
    uint32_t vr_x;  // t_reordering state. SN following PDU which triggered t_reordering.
    uint32_t vr_ms; // Max status tx state. Highest possible value of SN for ACK_SN in status PDU.
    uint32_t vr_h;  // Highest rx state. SN following PDU with highest SN among rxed PDUs.

    // Mutexes
    pthread_mutex_t     mutex;

    // Rx windows
    std::map<uint32_t, rlc_amd_rx_pdu_t>          rx_window;
    std::map<uint32_t, rlc_amd_rx_pdu_segments_t> rx_segments;

    // Metrics
    uint32_t            num_rx_bytes;

    bool                poll_received;
    bool                do_status;

    /****************************************************************************
     * Timers
     * Ref: 3GPP TS 36.322 v10.0.0 Section 7
     ***************************************************************************/

    srslte::timers::timer *reordering_timer;
    uint32_t               reordering_timer_id;
  };

  // Rx and Tx objects
  rlc_am_tx           tx;
  rlc_am_rx           rx;

  // Common variables needed/provided by parent class
  srsue::rrc_interface_rlc  *rrc;
  srslte::log               *log;
  srsue::pdcp_interface_rlc *pdcp;
  mac_interface_timers      *mac_timers;
  uint32_t                  lcid;
  srslte_rlc_am_config_t    cfg;
  std::string               rb_name;

  static const int poll_periodicity = 8; // After how many data PDUs a status PDU shall be requested
};

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 36.322 v10.0.0 Section 6.2.1
 ***************************************************************************/
void        rlc_am_read_data_pdu_header(byte_buffer_t *pdu, rlc_amd_pdu_header_t *header);
void        rlc_am_read_data_pdu_header(uint8_t **payload, uint32_t *nof_bytes, rlc_amd_pdu_header_t *header);
void        rlc_am_write_data_pdu_header(rlc_amd_pdu_header_t *header, byte_buffer_t *pdu);
void        rlc_am_write_data_pdu_header(rlc_amd_pdu_header_t *header, uint8_t **payload);
void        rlc_am_read_status_pdu(byte_buffer_t *pdu, rlc_status_pdu_t *status);
void        rlc_am_read_status_pdu(uint8_t *payload, uint32_t nof_bytes, rlc_status_pdu_t *status);
void        rlc_am_write_status_pdu(rlc_status_pdu_t *status, byte_buffer_t *pdu );
int         rlc_am_write_status_pdu(rlc_status_pdu_t *status, uint8_t *payload);

uint32_t    rlc_am_packed_length(rlc_amd_pdu_header_t *header);
uint32_t    rlc_am_packed_length(rlc_status_pdu_t *status);
uint32_t    rlc_am_packed_length(rlc_amd_retx_t retx);
bool        rlc_am_is_control_pdu(byte_buffer_t *pdu);
bool        rlc_am_is_control_pdu(uint8_t *payload);
bool        rlc_am_is_pdu_segment(uint8_t *payload);
std::string rlc_am_status_pdu_to_string(rlc_status_pdu_t *status);
std::string rlc_amd_pdu_header_to_string(const rlc_amd_pdu_header_t &header);
bool        rlc_am_start_aligned(const uint8_t fi);
bool        rlc_am_end_aligned(const uint8_t fi);
bool        rlc_am_is_unaligned(const uint8_t fi);
bool        rlc_am_not_start_aligned(const uint8_t fi);

} // namespace srslte


#endif // SRSLTE_RLC_AM_H
