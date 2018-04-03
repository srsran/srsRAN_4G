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

#ifndef SRSUE_PROC_RA_H
#define SRSUE_PROC_RA_H

#include <stdint.h>

#include "srslte/common/log.h"
#include "srslte/common/timers.h"
#include "mux.h"
#include "demux.h"
#include "srslte/common/pdu.h"
#include "srslte/common/mac_pcap.h"

/* Random access procedure as specified in Section 5.1 of 36.321 */


namespace srsue {

class ra_proc : public srslte::timer_callback
{
public:
  ra_proc() : rar_pdu_msg(20) {
    bzero(&softbuffer_rar, sizeof(srslte_softbuffer_rx_t));
    pcap = NULL;
    backoff_interval_start    = 0;
    backoff_inteval           = 0;
    received_target_power_dbm = 0;
    ra_rnti                   = 0;
    current_ta                = 0;
    state                     = IDLE;
    last_msg3_group           = RA_GROUP_A;
    msg3_transmitted          = false;
    first_rar_received        = false;
    phy_h                     = NULL;
    log_h                     = NULL;
    mac_cfg                   = NULL;
    mux_unit                  = NULL;
    demux_unit                = NULL;
    rrc                       = NULL;
    transmitted_contention_id = 0;
    transmitted_crnti         = 0;
    pdcch_to_crnti_received   = PDCCH_CRNTI_NOT_RECEIVED;
    started_by_pdcch          = false;
    rar_grant_nbytes          = 0;
    rar_grant_tti             = 0;
    msg3_flushed              = false;

    noncontention_enabled     = false;
    next_preamble_idx         = 0;
    next_prach_mask           = 0;

    time_alignment_timer        = NULL;
    contention_resolution_timer = NULL;
  };

  ~ra_proc();

  void init(phy_interface_mac *phy_h,
            rrc_interface_mac *rrc_,
            srslte::log *log_h,
            mac_interface_rrc::ue_rnti_t *rntis,
            mac_interface_rrc::mac_cfg_t *mac_cfg,
            srslte::timers::timer* time_alignment_timer_,
            srslte::timers::timer* contention_resolution_timer_,
            mux *mux_unit,
            demux *demux_unit);
  void reset();
  void start_pdcch_order();
  void start_mac_order(uint32_t msg_len_bits = 56, bool is_ho = false);
  void step(uint32_t tti);
  bool is_successful();
  bool is_response_error();
  bool is_contention_resolution();
  void harq_retx();
  bool is_error();
  bool in_progress();
  void pdcch_to_crnti(bool contains_uplink_grant);
  void timer_expired(uint32_t timer_id);

  void new_grant_dl(mac_interface_phy::mac_grant_t grant, mac_interface_phy::tb_action_dl_t* action);
  void tb_decoded_ok();

  void start_noncont(uint32_t preamble_index, uint32_t prach_mask);

  void start_pcap(srslte::mac_pcap* pcap);
private: 
  static bool uecrid_callback(void *arg, uint64_t uecri);

  bool contention_resolution_id_received(uint64_t uecri);
  void process_timeadv_cmd(uint32_t ta_cmd);
  void step_initialization();
  void step_resource_selection();
  void step_preamble_transmission();
  void step_pdcch_setup();
  void step_response_reception();
  void step_response_error();
  void step_backoff_wait();
  void step_contention_resolution();
  void step_completition();

  //  Buffer to receive RAR PDU
  static const uint32_t MAX_RAR_PDU_LEN = 2048;
  uint8_t     rar_pdu_buffer[MAX_RAR_PDU_LEN];
  srslte::rar_pdu rar_pdu_msg;

  // Random Access parameters provided by higher layers defined in 5.1.1
  uint32_t configIndex;
  uint32_t nof_preambles;
  uint32_t nof_groupA_preambles;
  uint32_t nof_groupB_preambles;
  uint32_t messagePowerOffsetGroupB;
  uint32_t messageSizeGroupA;
  uint32_t responseWindowSize;
  uint32_t powerRampingStep;
  uint32_t preambleTransMax;
  uint32_t iniReceivedTargetPower;
  int      delta_preamble_db;
  uint32_t contentionResolutionTimer;
  uint32_t maskIndex;
  int      preambleIndex;
  uint32_t new_ra_msg_len;

  bool     noncontention_enabled;
  uint32_t next_preamble_idx;
  uint32_t next_prach_mask;

  // Internal variables
  uint32_t preambleTransmissionCounter;
  uint32_t backoff_param_ms;
  uint32_t sel_maskIndex;
  uint32_t sel_preamble;
  uint32_t backoff_interval_start;
  uint32_t backoff_inteval;
  int      received_target_power_dbm;
  uint32_t ra_rnti;
  uint32_t current_ta;

  srslte_softbuffer_rx_t softbuffer_rar;

  enum {
    IDLE = 0,
    INITIALIZATION,           // Section 5.1.1
    RESOURCE_SELECTION,       // Section 5.1.2
    PREAMBLE_TRANSMISSION,    // Section 5.1.3
    PDCCH_SETUP,
    RESPONSE_RECEPTION,       // Section 5.1.4
    RESPONSE_ERROR,
    BACKOFF_WAIT,
    CONTENTION_RESOLUTION,    // Section 5.1.5
    COMPLETION,               // Section 5.1.6
    COMPLETION_DONE,
    RA_PROBLEM                // Section 5.1.5 last part
  } state;

  typedef enum {RA_GROUP_A, RA_GROUP_B} ra_group_t;

  ra_group_t  last_msg3_group;
  bool        msg3_transmitted;
  bool        first_rar_received;
  void        read_params();

  phy_interface_mac *phy_h;
  srslte::log       *log_h;
  mux               *mux_unit;
  demux             *demux_unit;
  srslte::mac_pcap  *pcap;
  rrc_interface_mac *rrc;

  srslte::timers::timer  *time_alignment_timer;
  srslte::timers::timer  *contention_resolution_timer;

  mac_interface_rrc::ue_rnti_t *rntis;
  mac_interface_rrc::mac_cfg_t *mac_cfg;

  uint64_t    transmitted_contention_id;
  uint16_t    transmitted_crnti;

  enum {
     PDCCH_CRNTI_NOT_RECEIVED = 0,
     PDCCH_CRNTI_UL_GRANT,
     PDCCH_CRNTI_DL_GRANT
  } pdcch_to_crnti_received;

  bool ra_is_ho;
  bool started_by_pdcch;
  uint32_t rar_grant_nbytes;
  uint32_t rar_grant_tti;
  bool msg3_flushed;
  bool rar_received;
};

} // namespace srsue

#endif // SRSUE_PROC_RA_H
