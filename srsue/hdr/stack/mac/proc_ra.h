/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSUE_PROC_RA_H
#define SRSUE_PROC_RA_H

#include <atomic>
#include <mutex>
#include <stdint.h>

#include "demux.h"
#include "mux.h"
#include "srsran/common/mac_pcap.h"
#include "srsran/common/timers.h"
#include "srsran/mac/pdu.h"
#include "srsue/hdr/stack/mac_common/mac_common.h"

/* Random access procedure as specified in Section 5.1 of 36.321 */

namespace srsue {

class ra_proc : public srsran::timer_callback
{
public:
  explicit ra_proc(srslog::basic_logger& logger) : rar_pdu_msg(20), logger(logger) {}

  ~ra_proc();

  void init(phy_interface_mac_lte*               phy_h,
            rrc_interface_mac*                   rrc_,
            ue_rnti*                             rntis,
            srsran::timer_handler::unique_timer* time_alignment_timer_,
            mux*                                 mux_unit,
            srsran::ext_task_sched_handle*       task_sched_);

  void reset();

  void set_config(srsran::rach_cfg_t& rach_cfg);
  void set_config_ded(uint32_t preamble_index, uint32_t prach_mask);

  void start_pdcch_order();
  void start_mac_order(uint32_t msg_len_bits = 56);
  void step(uint32_t tti);

  void update_rar_window(rnti_window_safe& ra_window);
  bool is_contention_resolution();
  void harq_retx();
  void harq_max_retx();
  void pdcch_to_crnti(bool is_new_uplink_transmission);
  void timer_expired(uint32_t timer_id);
  void new_grant_dl(mac_interface_phy_lte::mac_grant_dl_t grant, mac_interface_phy_lte::tb_action_dl_t* action);
  void tb_decoded_ok(const uint8_t cc_idx, const uint32_t tti);
  bool contention_resolution_id_received(uint64_t rx_contention_id);

  void start_pcap(srsran::mac_pcap* pcap);

  bool is_idle() const { return state == IDLE; }

private:
  void state_pdcch_setup();
  void state_response_reception(uint32_t tti);
  void state_backoff_wait(uint32_t tti);
  void state_contention_resolution();

  void process_timeadv_cmd(uint32_t tti, uint32_t ta_cmd);
  void initialization();
  void resource_selection();
  void preamble_transmission();
  void response_error();
  void complete();

  bool contention_resolution_id_received_nolock(uint64_t rx_contention_id);

  //  Buffer to receive RAR PDU
  static const uint32_t MAX_RAR_PDU_LEN                 = 2048;
  uint8_t               rar_pdu_buffer[MAX_RAR_PDU_LEN] = {};
  srsran::rar_pdu       rar_pdu_msg;

  // Random Access parameters provided by higher layers defined in 5.1.1
  srsran::rach_cfg_t rach_cfg = {};

  int      delta_preamble_db = 0;
  uint32_t maskIndex         = 0;
  int      preambleIndex     = 0;
  uint32_t new_ra_msg_len    = 0;

  bool     noncontention_enabled = false;
  uint32_t next_preamble_idx     = 0;
  uint32_t next_prach_mask       = 0;

  // Internal variables
  uint32_t              preambleTransmissionCounter = 0;
  uint32_t              backoff_param_ms            = 0;
  uint32_t              sel_maskIndex               = 0;
  std::atomic<uint32_t> sel_preamble                = {0};
  int                   backoff_interval_start      = 0;
  uint32_t              backoff_interval            = 0;
  int                   received_target_power_dbm   = 0;
  uint32_t              ra_rnti                     = SRSRAN_INVALID_RNTI;
  uint32_t              ra_tti                      = 0;
  uint32_t              current_ta                  = 0;
  // The task_id is a unique number associated with each RA procedure used to track background tasks
  uint32_t current_task_id = 0;

  srsran_softbuffer_rx_t softbuffer_rar = {};

  enum ra_state_t {
    IDLE = 0,
    PDCCH_SETUP,
    RESPONSE_RECEPTION,
    BACKOFF_WAIT,
    CONTENTION_RESOLUTION,
  };
  std::atomic<ra_state_t> state = {IDLE};

  typedef enum { RA_GROUP_A, RA_GROUP_B } ra_group_t;

  ra_group_t last_msg3_group = RA_GROUP_A;

  uint32_t rar_window_st = 0;

  void read_params();

  phy_interface_mac_lte*                phy_h = nullptr;
  srslog::basic_logger&                 logger;
  mux*                                  mux_unit   = nullptr;
  srsran::mac_pcap*                     pcap       = nullptr;
  rrc_interface_mac*                    rrc        = nullptr;
  srsran::ext_task_sched_handle*        task_sched = nullptr;
  srsran::task_multiqueue::queue_handle task_queue;

  srsran::timer_handler::unique_timer* time_alignment_timer = nullptr;
  srsran::timer_handler::unique_timer  contention_resolution_timer;

  ue_rnti* rntis = nullptr;

  std::atomic<uint64_t> transmitted_contention_id = {0};
  std::atomic<uint16_t> transmitted_crnti         = {0};

  bool              started_by_pdcch = false;
  uint32_t          rar_grant_nbytes = 0;
  std::atomic<bool> rar_received     = {false};
};

} // namespace srsue

#endif // SRSUE_PROC_RA_H
