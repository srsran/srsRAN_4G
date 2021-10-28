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

#ifndef SRSUE_UE_STACK_NR_H
#define SRSUE_UE_STACK_NR_H

#include <functional>
#include <pthread.h>
#include <stdarg.h>
#include <string>

#include "mac_nr/mac_nr.h"
#include "srsran/radio/radio.h"
#include "srsran/rlc/rlc.h"
#include "srsran/upper/pdcp.h"
#include "upper/nas.h"
#include "upper/usim.h"

#include "srsran/common/buffer_pool.h"
#include "srsran/common/mac_pcap.h"
#include "srsran/common/multiqueue.h"
#include "srsran/common/thread_pool.h"
#include "srsran/interfaces/ue_interfaces.h"
#include "srsran/interfaces/ue_nr_interfaces.h"

#include "srsue/hdr/ue_metrics_interface.h"
#include "ue_stack_base.h"

namespace srsue {

class rrc_nr;

/** \brief L2/L3 stack class for 5G/NR UEs.
 *
 *  This class wraps all L2/L3 blocks and provides a single interface towards the PHY.
 */

class ue_stack_nr final : public ue_stack_base,
                          public stack_interface_phy_nr,
                          public stack_interface_gw,
                          public stack_interface_rrc,
                          public srsran::thread
{
public:
  ue_stack_nr();
  ~ue_stack_nr();

  std::string get_type() final;

  int  init(const stack_args_t& args_);
  int  init(const stack_args_t& args_, phy_interface_stack_nr* phy_, gw_interface_stack* gw_);
  bool switch_on() final;
  bool switch_off() final;
  void stop() final;

  // GW srsue stack_interface_gw dummy interface
  bool is_registered() final { return true; };
  bool start_service_request() final { return true; };

  bool get_metrics(stack_metrics_t* metrics) final;
  bool is_rrc_connected();

  // RRC interface for PHY
  void in_sync() final;
  void out_of_sync() final;
  void run_tti(const uint32_t tti) final;
  void set_phy_config_complete(bool status) override;

  // MAC interface for PHY
  sched_rnti_t get_dl_sched_rnti_nr(const uint32_t tti) final { return mac->get_dl_sched_rnti_nr(tti); }
  sched_rnti_t get_ul_sched_rnti_nr(const uint32_t tti) final { return mac->get_ul_sched_rnti_nr(tti); }
  int          sf_indication(const uint32_t tti) final
  {
    run_tti(tti);
    return SRSRAN_SUCCESS;
  }
  void tb_decoded(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_result_t result) final
  {
    mac->tb_decoded(cc_idx, grant, std::move(result));
  }
  void new_grant_dl(const uint32_t cc_idx, const mac_nr_grant_dl_t& grant, tb_action_dl_t* action) final
  {
    mac->new_grant_dl(cc_idx, grant, action);
  }
  void new_grant_ul(const uint32_t cc_idx, const mac_nr_grant_ul_t& grant, tb_action_ul_t* action) final
  {
    mac->new_grant_ul(cc_idx, grant, action);
  }
  void prach_sent(uint32_t tti, uint32_t s_id, uint32_t t_id, uint32_t f_id, uint32_t ul_carrier_id) final
  {
    mac->prach_sent(tti, s_id, t_id, f_id, ul_carrier_id);
  }
  bool sr_opportunity(uint32_t tti, uint32_t sr_id, bool meas_gap, bool ul_sch_tx) final
  {
    return mac->sr_opportunity(tti, sr_id, meas_gap, ul_sch_tx);
  }

  // Interface for GW
  void write_sdu(uint32_t eps_bearer_id, srsran::unique_byte_buffer_t sdu) final;
  bool has_active_radio_bearer(uint32_t eps_bearer_id) final { return true; /* TODO: add EPS to LCID mapping */ }

  // Interface for RRC
  srsran::tti_point get_current_tti() final { return srsran::tti_point{0}; }
  void              add_eps_bearer(uint8_t eps_bearer_id, srsran::srsran_rat_t rat, uint32_t lcid) final {}
  void              remove_eps_bearer(uint8_t eps_bearer_id) final {}
  void              reset_eps_bearers() final {}

private:
  void run_thread() final;
  void run_tti_impl(uint32_t tti);
  void stop_impl();

  bool                running = false;
  srsue::stack_args_t args    = {};

  // task scheduler
  srsran::task_scheduler                task_sched;
  srsran::task_multiqueue::queue_handle sync_task_queue, ue_task_queue, gw_task_queue;

  // UE stack logging
  srslog::basic_logger& mac_logger;
  srslog::basic_logger& rlc_logger;
  srslog::basic_logger& pdcp_logger;

  // stack components
  std::unique_ptr<mac_nr>       mac;
  std::unique_ptr<rrc_nr>       rrc;
  std::unique_ptr<srsran::rlc>  rlc;
  std::unique_ptr<srsran::pdcp> pdcp;

  // RAT-specific interfaces
  phy_interface_stack_nr* phy = nullptr;
  gw_interface_stack*     gw  = nullptr;

  // Thread
  static const int STACK_MAIN_THREAD_PRIO = 4;
};

} // namespace srsue

#endif // SRSUE_UE_STACK_NR_H
