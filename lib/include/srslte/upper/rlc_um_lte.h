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

#ifndef SRSLTE_RLC_UM_LTE_H
#define SRSLTE_RLC_UM_LTE_H

#include "srslte/common/buffer_pool.h"
#include "srslte/common/common.h"
#include "srslte/common/log.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/upper/byte_buffer_queue.h"
#include "srslte/upper/rlc_um_base.h"
#include <map>
#include <mutex>
#include <pthread.h>
#include <queue>

namespace srslte {

typedef struct {
  rlc_umd_pdu_header_t header;
  unique_byte_buffer_t buf;
} rlc_umd_pdu_t;

class rlc_um_lte : public rlc_um_base
{
public:
  rlc_um_lte(srslte::log_ref            log_,
             uint32_t                   lcid_,
             srsue::pdcp_interface_rlc* pdcp_,
             srsue::rrc_interface_rlc*  rrc_,
             srslte::timer_handler*     timers_);
  ~rlc_um_lte();
  bool configure(const rlc_config_t& cnfg);

private:
  // Transmitter sub-class for LTE
  class rlc_um_lte_tx : public rlc_um_base_tx
  {
  public:
    rlc_um_lte_tx(rlc_um_base* parent_);

    bool     configure(const rlc_config_t& cfg, std::string rb_name);
    int      build_data_pdu(unique_byte_buffer_t pdu, uint8_t* payload, uint32_t nof_bytes);
    uint32_t get_buffer_state();
    bool     sdu_queue_is_full();

  private:
    void reset();

    /****************************************************************************
     * State variables and counters
     * Ref: 3GPP TS 36.322 v10.0.0 Section 7
     ***************************************************************************/
    uint32_t vt_us = 0; // Send state. SN to be assigned for next PDU.

    void debug_state();
  };

  // Receiver sub-class for LTE
  class rlc_um_lte_rx : public rlc_um_base_rx
  {
  public:
    rlc_um_lte_rx(rlc_um_base* parent_);
    ~rlc_um_lte_rx();
    void stop();
    void reestablish();
    bool configure();
    void handle_data_pdu(uint8_t* payload, uint32_t nof_bytes);
    void reassemble_rx_sdus();
    bool pdu_belongs_to_rx_sdu();
    bool inside_reordering_window(uint16_t sn);

    // Timeout callback interface
    void timer_expired(uint32_t timeout_id);

  private:
    void reset();

    // Rx window
    std::map<uint32_t, rlc_umd_pdu_t> rx_window;

    // RX SDU buffers
    uint32_t vr_ur_in_rx_sdu = 0;

    // Rx state variables and counter
    uint32_t vr_ur    = 0; // Receive state. SN of earliest PDU still considered for reordering.
    uint32_t vr_ux    = 0; // t_reordering state. SN following PDU which triggered t_reordering.
    uint32_t vr_uh    = 0; // Highest rx state. SN following PDU with highest SN among rxed PDUs.
    bool     pdu_lost = false;

    /****************************************************************************
     * Timers
     * Ref: 3GPP TS 36.322 v10.0.0 Section 7
     ***************************************************************************/
    srslte::timer_handler::unique_timer reordering_timer;

    // helper functions
    void debug_state();
  };
};

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 36.322 v10.0.0 Section 6.2.1
 ***************************************************************************/
void rlc_um_read_data_pdu_header(byte_buffer_t* pdu, rlc_umd_sn_size_t sn_size, rlc_umd_pdu_header_t* header);
void rlc_um_read_data_pdu_header(uint8_t*              payload,
                                 uint32_t              nof_bytes,
                                 rlc_umd_sn_size_t     sn_size,
                                 rlc_umd_pdu_header_t* header);
void rlc_um_write_data_pdu_header(rlc_umd_pdu_header_t* header, byte_buffer_t* pdu);

uint32_t rlc_um_packed_length(rlc_umd_pdu_header_t* header);
bool     rlc_um_start_aligned(uint8_t fi);
bool     rlc_um_end_aligned(uint8_t fi);

} // namespace srslte

#endif // SRSLTE_RLC_UM_LTE_H
