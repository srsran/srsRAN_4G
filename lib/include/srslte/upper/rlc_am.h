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
#include "srslte/common/msg_queue.h"
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


class rlc_am
    :public rlc_common
{
public:
  rlc_am();
  ~rlc_am();
  void init(log          *rlc_entity_log_,
            uint32_t              lcid_,
            srsue::pdcp_interface_rlc   *pdcp_,
            srsue::rrc_interface_rlc    *rrc_,
            mac_interface_timers *mac_timers);
  void configure(srslte_rlc_config_t cnfg);
  void reset();
  void reestablish();
  void stop();
  void empty_queue(); 
  
  rlc_mode_t    get_mode();
  uint32_t      get_bearer();

  // PDCP interface
  void write_sdu(byte_buffer_t *sdu);

  // MAC interface
  uint32_t get_buffer_state();
  uint32_t get_total_buffer_state(); 
  int      read_pdu(uint8_t *payload, uint32_t nof_bytes);
  void     write_pdu(uint8_t *payload, uint32_t nof_bytes);

private:

  byte_buffer_pool          *pool;
  srslte::log               *log;
  uint32_t                   lcid;
  srsue::pdcp_interface_rlc *pdcp;
  srsue::rrc_interface_rlc  *rrc;

  // TX SDU buffers
  msg_queue      tx_sdu_queue;
  byte_buffer_t *tx_sdu;

  // PDU being resegmented
  rlc_amd_tx_pdu_t tx_pdu_segments;

  // Tx and Rx windows
  std::map<uint32_t, rlc_amd_tx_pdu_t>          tx_window;
  std::deque<rlc_amd_retx_t>                    retx_queue;
  std::map<uint32_t, rlc_amd_rx_pdu_t>          rx_window;
  std::map<uint32_t, rlc_amd_rx_pdu_segments_t> rx_segments;

  // RX SDU buffers
  byte_buffer_t *rx_sdu;

  // Mutexes
  pthread_mutex_t     mutex;

  bool                poll_received;
  bool                do_status;
  rlc_status_pdu_t    status;

  /****************************************************************************
   * Configurable parameters
   * Ref: 3GPP TS 36.322 v10.0.0 Section 7
   ***************************************************************************/

  srslte_rlc_am_config_t cfg;

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

  // Rx state variables
  uint32_t vr_r;  // Receive state. SN following last in-sequence received PDU. Low edge of rx window
  uint32_t vr_mr; // Max acceptable receive state. High edge of rx window. vr_r + window size.
  uint32_t vr_x;  // t_reordering state. SN following PDU which triggered t_reordering.
  uint32_t vr_ms; // Max status tx state. Highest possible value of SN for ACK_SN in status PDU.
  uint32_t vr_h;  // Highest rx state. SN following PDU with highest SN among rxed PDUs.

  /****************************************************************************
   * Timers
   * Ref: 3GPP TS 36.322 v10.0.0 Section 7
   ***************************************************************************/
  timeout poll_retx_timeout;
  timeout reordering_timeout;
  timeout status_prohibit_timeout;

  static const int reordering_timeout_id = 1;

  static const int poll_periodicity = 8; // After how many data PDUs a status PDU shall be requested

  // Timer checks
  bool status_prohibited();
  bool poll_retx();
  void check_reordering_timeout();

  // Helpers
  bool poll_required();

  int  prepare_status();
  int  build_status_pdu(uint8_t *payload, uint32_t nof_bytes);
  int  build_retx_pdu(uint8_t *payload, uint32_t nof_bytes);
  int  build_segment(uint8_t *payload, uint32_t nof_bytes, rlc_amd_retx_t retx);
  int  build_data_pdu(uint8_t *payload, uint32_t nof_bytes);

  void handle_data_pdu(uint8_t *payload, uint32_t nof_bytes, rlc_amd_pdu_header_t &header);
  void handle_data_pdu_segment(uint8_t *payload, uint32_t nof_bytes, rlc_amd_pdu_header_t &header);
  void handle_control_pdu(uint8_t *payload, uint32_t nof_bytes);

  void reassemble_rx_sdus();

  bool inside_tx_window(uint16_t sn);
  bool inside_rx_window(uint16_t sn);
  void debug_state();
  void print_rx_segments();

  bool add_segment_and_check(rlc_amd_rx_pdu_segments_t *pdu, rlc_amd_rx_pdu_t *segment);
  int  required_buffer_size(rlc_amd_retx_t retx);
  bool retx_queue_has_sn(uint32_t sn);
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
std::string rlc_am_to_string(rlc_status_pdu_t *status);
bool        rlc_am_start_aligned(uint8_t fi);
bool        rlc_am_end_aligned(uint8_t fi);

} // namespace srslte


#endif // SRSLTE_RLC_AM_H
