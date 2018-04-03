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

#ifndef SRSLTE_RLC_UM_H
#define SRSLTE_RLC_UM_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/log.h"
#include "srslte/common/common.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/common/msg_queue.h"
#include "srslte/upper/rlc_common.h"
#include <pthread.h>
#include <map>
#include <queue>

namespace srslte {

struct rlc_umd_pdu_t{
  rlc_umd_pdu_header_t  header;
  byte_buffer_t        *buf;
};

class rlc_um
    :public timer_callback
    ,public rlc_common
{
public:
  rlc_um();
  ~rlc_um();

  void init(log          *rlc_entity_log_,
            uint32_t              lcid_,
            srsue::pdcp_interface_rlc   *pdcp_,
            srsue::rrc_interface_rlc    *rrc_,
            mac_interface_timers *mac_timers_);
  void configure(srslte_rlc_config_t cnfg);
  void reset();
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

  // Timeout callback interface
  void timer_expired(uint32_t timeout_id);

  bool reordering_timeout_running();

private:

  byte_buffer_pool            *pool;
  srslte::log                 *log;
  uint32_t                     lcid;
  srsue::pdcp_interface_rlc   *pdcp;
  srsue::rrc_interface_rlc    *rrc;
  mac_interface_timers        *mac_timers; 

  // TX SDU buffers
  msg_queue           tx_sdu_queue;
  byte_buffer_t      *tx_sdu;

  // Rx window
  std::map<uint32_t, rlc_umd_pdu_t>  rx_window;

  // RX SDU buffers
  byte_buffer_t      *rx_sdu;
  uint32_t            vr_ur_in_rx_sdu;

  // Mutexes
  pthread_mutex_t        mutex;

  /****************************************************************************
   * Configurable parameters
   * Ref: 3GPP TS 36.322 v10.0.0 Section 7
   ***************************************************************************/

  srslte_rlc_um_config_t cfg;

  /****************************************************************************
   * State variables and counters
   * Ref: 3GPP TS 36.322 v10.0.0 Section 7
   ***************************************************************************/

  // Tx state variables
  uint32_t vt_us;    // Send state. SN to be assigned for next PDU.

  // Rx state variables
  uint32_t vr_ur;  // Receive state. SN of earliest PDU still considered for reordering.
  uint32_t vr_ux;  // t_reordering state. SN following PDU which triggered t_reordering.
  uint32_t vr_uh;  // Highest rx state. SN following PDU with highest SN among rxed PDUs.

  /****************************************************************************
   * Timers
   * Ref: 3GPP TS 36.322 v10.0.0 Section 7
   ***************************************************************************/
  srslte::timers::timer *reordering_timer;
  uint32_t               reordering_timer_id;

  bool     pdu_lost;

  int  build_data_pdu(uint8_t *payload, uint32_t nof_bytes);
  void handle_data_pdu(uint8_t *payload, uint32_t nof_bytes);
  void reassemble_rx_sdus();
  bool inside_reordering_window(uint16_t sn);
  void debug_state();
};

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 36.322 v10.0.0 Section 6.2.1
 ***************************************************************************/
void        rlc_um_read_data_pdu_header(byte_buffer_t *pdu, rlc_umd_sn_size_t sn_size, rlc_umd_pdu_header_t *header);
void        rlc_um_read_data_pdu_header(uint8_t *payload, uint32_t nof_bytes, rlc_umd_sn_size_t sn_size, rlc_umd_pdu_header_t *header);
void        rlc_um_write_data_pdu_header(rlc_umd_pdu_header_t *header, byte_buffer_t *pdu);

uint32_t    rlc_um_packed_length(rlc_umd_pdu_header_t *header);
bool        rlc_um_start_aligned(uint8_t fi);
bool        rlc_um_end_aligned(uint8_t fi);

} // namespace srsue


#endif // SRSLTE_RLC_UM_H
