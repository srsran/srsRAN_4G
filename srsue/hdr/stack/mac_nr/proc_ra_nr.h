/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSUE_PROC_RA_NR_H
#define SRSUE_PROC_RA_NR_H

#include <atomic>
#include <mutex>
#include <stdint.h>

#include "mac_nr_interfaces.h"
#include "srsran/common/common.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/interfaces/ue_nr_interfaces.h"
#include "srsran/srslog/srslog.h"

namespace srsue {

class proc_ra_nr
{
public:
  proc_ra_nr(mac_interface_proc_ra_nr& mac_, srslog::basic_logger& logger_);
  ~proc_ra_nr() { srsran_random_free(random_gen); };

  void init(phy_interface_mac_nr* phy_h_, srsran::ext_task_sched_handle* task_sched_);
  void set_config(const srsran::rach_cfg_nr_t& rach_cfg_nr);
  bool is_contention_resolution();

  bool     is_rar_opportunity(uint32_t tti);
  bool     has_rar_rnti();
  uint16_t get_rar_rnti();
  bool     has_temp_crnti();
  uint16_t get_temp_crnti();
  void     set_crnti_to_temp();
  void     received_contention_resolution(bool is_successful);

  // PHY interfaces
  void prach_sent(uint32_t tti, uint32_t s_id, uint32_t t_id, uint32_t f_id, uint32_t ul_carrier_id);
  void handle_rar_pdu(mac_interface_phy_nr::tb_action_dl_result_t& grant);
  void pdcch_to_crnti();

  void start_by_rrc();
  void start_by_mac();
  void reset();

private:
  static const uint32_t PRACH_SEND_CALLBACK_TIMEOUT =
      16 * 10; ///< Limited from frame system number opportunity period in TS 38.211 tables 6.3.3.2-2, 6.3.3.2-3
               ///< and 6.3.3.2-4

  mac_interface_proc_ra_nr& mac;
  srslog::basic_logger&     logger;
  srsran_random_t           random_gen;

  phy_interface_mac_nr*                 phy        = nullptr;
  srsran::ext_task_sched_handle*        task_sched = nullptr;
  srsran::task_multiqueue::queue_handle task_queue;

  int        ra_window_length = -1, ra_window_start = -1;
  uint16_t   rar_rnti   = SRSRAN_INVALID_RNTI;
  std::mutex mutex;

  srsran::rach_cfg_nr_t rach_cfg   = {};
  bool                  configured = false;

  enum ra_state_t {
    IDLE = 0,
    PDCCH_SETUP,
    WAITING_FOR_PRACH_SENT,
    WAITING_FOR_RESPONSE_RECEPTION,
    WAITING_FOR_CONTENTION_RESOLUTION,
    WAITING_FOR_COMPLETION,
    MAX_RA_STATES,
  };

  std::atomic<ra_state_t> state = {IDLE};

  enum initiators_t { MAC, RRC, initiators_t_NULLTYPE };
  std::atomic<initiators_t> started_by = {initiators_t_NULLTYPE};

  srsran::timer_handler::unique_timer prach_send_timer;
  srsran::timer_handler::unique_timer rar_timeout_timer;
  srsran::timer_handler::unique_timer contention_resolution_timer;
  srsran::timer_handler::unique_timer backoff_timer;

  // 38.321 5.1.1 Variables
  uint32_t preamble_index                 = 0;
  uint32_t preamble_transmission_counter  = 0;
  uint32_t preamble_backoff               = 0; // in ms
  uint32_t preamble_power_ramping_step    = 0;
  int      preamble_received_target_power = 0;
  uint32_t scaling_factor_bi              = 0;
  // uint32_t temporary_c_rnti;
  uint32_t power_offset_2step_ra = 0;

  // not explicty mentioned
  uint32_t preambleTransMax = 0;
  uint32_t prach_occasion   = 0;

  uint32_t current_ta = 0;
  void     timer_expired(uint32_t timer_id);
  // 38.321 Steps 5.1.1 - 5.1.6
  void ra_procedure_initialization();
  void ra_resource_selection();
  void ra_preamble_transmission();
  void ra_response_reception(const mac_interface_phy_nr::tb_action_dl_result_t& tb);
  void ra_contention_resolution(bool is_successful, bool is_ul_grant);
  void ra_completion();
  void ra_error();
};
} // namespace srsue
#endif