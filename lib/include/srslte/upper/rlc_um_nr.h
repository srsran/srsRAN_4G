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

#ifndef SRSLTE_RLC_UM_NR_H
#define SRSLTE_RLC_UM_NR_H

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
  rlc_um_nr_pdu_header_t header;
  unique_byte_buffer_t   buf;
} rlc_umd_pdu_nr_t;

class rlc_um_nr : public rlc_um_base
{
public:
  rlc_um_nr(srslte::log_ref            log_,
            uint32_t                   lcid_,
            srsue::pdcp_interface_rlc* pdcp_,
            srsue::rrc_interface_rlc*  rrc_,
            srslte::timer_handler*     timers_);
  ~rlc_um_nr();
  bool configure(const rlc_config_t& cnfg);

private:
  // Transmitter sub-class for NR
  class rlc_um_nr_tx : public rlc_um_base_tx
  {
  public:
    rlc_um_nr_tx(rlc_um_base* parent_);

    bool     configure(const rlc_config_t& cfg, std::string rb_name);
    int      build_data_pdu(unique_byte_buffer_t pdu, uint8_t* payload, uint32_t nof_bytes);
    uint32_t get_buffer_state();

  private:
    void reset();

    uint32_t TX_Next = 0; // send state as defined in TS 38.322 v15.3 Section 7
                          // It holds the value of the SN to be assigned for the next newly generated UMD PDU with
                          // segment. It is initially set to 0, and is updated after the UM RLC entity submits a UMD PDU
                          // including the last segment of an RLC SDU to lower layers.

    uint32_t next_so = 0; // The segment offset for the next generated PDU

    void debug_state();
  };

  // Receiver sub-class for NR
  class rlc_um_nr_rx : public rlc_um_base_rx
  {
  public:
    rlc_um_nr_rx(rlc_um_base* parent_);

    bool configure();
    void handle_data_pdu(uint8_t* payload, uint32_t nof_bytes);

    void reestablish();
    void stop();

    void timer_expired(uint32_t timeout_id);

  private:
    bool sn_in_reassembly_window(const uint32_t sn);
    bool sn_invalid_for_rx_buffer(const uint32_t sn);
    bool has_missing_byte_segment(const uint32_t sn);
    unique_byte_buffer_t
    rlc_um_nr_strip_pdu_header(const rlc_um_nr_pdu_header_t& header, const uint8_t* payload, const uint32_t nof_bytes);

    void handle_rx_buffer_update(const uint32_t sn);

    uint32_t RX_Next_Reassembly = 0; // the earliest SN that is still considered for reassembly
    uint32_t RX_Timer_Trigger   = 0; // the SN following the SN which triggered t-Reassembly
    uint32_t RX_Next_Highest    = 0; // the SN following the SN of the UMD PDU with the highest SN among
                                     // received UMD PDUs. It serves as the higher edge of the reassembly window.

    // Rx window
    typedef struct {
      std::map<uint32_t, rlc_umd_pdu_nr_t> segments; // Map of segments with SO as key
      unique_byte_buffer_t                 sdu;
      uint32_t                             next_expected_so;
      uint32_t                             total_sdu_length;
    } rlc_umd_pdu_segments_nr_t;
    std::map<uint32_t, rlc_umd_pdu_segments_nr_t> rx_window;

    void update_total_sdu_length(rlc_umd_pdu_segments_nr_t& pdu_segments, const rlc_umd_pdu_nr_t& rx_pdu);

    // TS 38.322 Sec. 7.3
    srslte::timer_handler::unique_timer reassembly_timer; // to detect loss of RLC PDUs at lower layers

    // helper functions
    void reset();
    void debug_state();

    std::mutex mutex;
  };
};

/****************************************************************************
 * Header pack/unpack helper functions for NR
 * Ref: 3GPP TS 38.322 v15.3.0 Section 6.2.2.3
 ***************************************************************************/
uint32_t rlc_um_nr_read_data_pdu_header(const byte_buffer_t*      pdu,
                                        const rlc_um_nr_sn_size_t sn_size,
                                        rlc_um_nr_pdu_header_t*   header);

uint32_t rlc_um_nr_read_data_pdu_header(const uint8_t*            payload,
                                        const uint32_t            nof_bytes,
                                        const rlc_um_nr_sn_size_t sn_size,
                                        rlc_um_nr_pdu_header_t*   header);

uint32_t rlc_um_nr_write_data_pdu_header(const rlc_um_nr_pdu_header_t& header, byte_buffer_t* pdu);

uint32_t rlc_um_nr_packed_length(const rlc_um_nr_pdu_header_t& header);

} // namespace srslte

#endif // SRSLTE_RLC_UM_NR_H
