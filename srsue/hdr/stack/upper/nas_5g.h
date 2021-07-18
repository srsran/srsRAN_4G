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

#ifndef SRSUE_NAS_5G_H
#define SRSUE_NAS_5G_H

#include "nas_base.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/common.h"
#include "srsran/common/nas_pcap.h"
#include "srsran/common/security.h"
#include "srsran/common/stack_procedure.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/interfaces/ue_gw_interfaces.h"
#include "srsran/interfaces/ue_nas_interfaces.h"
#include "srsran/interfaces/ue_rrc_interfaces.h"
#include "srsran/interfaces/ue_usim_interfaces.h"
#include "srsran/srslog/srslog.h"
#include "srsue/hdr/stack/upper/nas_5gmm_state.h"
#include "srsue/hdr/stack/upper/nas_config.h"

using srsran::byte_buffer_t;

namespace srsue {

class nas_5g : public nas_base, public nas_5g_interface_rrc_nr, public nas_5g_interface_procedures
{
public:
  explicit nas_5g(srsran::task_sched_handle task_sched_);
  virtual ~nas_5g();
  int  init(usim_interface_nas* usim_, rrc_nr_interface_nas_5g* rrc_nr_, gw_interface_nas* gw_, const nas_args_t& cfg_);
  void stop();
  void run_tti();

  // Stack+RRC interface
  bool is_registered();

  // timer callback
  void timer_expired(uint32_t timeout_id);

  // Stack interface
  int switch_on();
  int switch_off();
  int enable_data();
  int disable_data();
  int start_service_request();

private:
  rrc_nr_interface_nas_5g* rrc_nr = nullptr;
  usim_interface_nas*      usim   = nullptr;
  gw_interface_nas*        gw     = nullptr;

  bool running = false;

  nas_args_t   cfg   = {};
  mm5g_state_t state = {};

  // Security
  bool ia5g_caps[8] = {};
  bool ea5g_caps[8] = {};

  // timers
  srsran::task_sched_handle           task_sched;
  srsran::timer_handler::unique_timer t3502; // started when registration failure and the attempt counter is equal to 5
  srsran::timer_handler::unique_timer t3510; // started when transmission of REGISTRATION REQUEST message. ON EXPIRY:
                                             // start T3511 or T3502 as specified in subclause 5.5.1.2.7
  srsran::timer_handler::unique_timer t3511; // started when registration failure due to lower layer failure
  srsran::timer_handler::unique_timer t3521; // started when detach request is sent
  srsran::timer_handler::unique_timer reregistration_timer; // started to trigger delayed re-attach

  // Values according to TS 24.501 Sec 10.2
  const uint32_t t3502_duration_ms                = 12 * 60 * 1000; // 12m
  const uint32_t t3510_duration_ms                = 15 * 1000;      // 15s
  const uint32_t t3511_duration_ms                = 10 * 1000;      // 10s
  const uint32_t t3521_duration_ms                = 15 * 1000;      // 15s
  const uint32_t reregistration_timer_duration_ms = 2 * 1000;       // 2s (arbitrarily chosen to delay re-attach)

  srsran::proc_manager_list_t callbacks;

  // Procedures
  // Forward declartion
  class registration_procedure;

  srsran::proc_t<registration_procedure> registration_proc;

  int send_registration_request();
};
} // namespace srsue
#endif