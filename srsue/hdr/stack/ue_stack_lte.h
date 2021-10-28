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
/******************************************************************************
 * File:        ue_stack.h
 * Description: L2/L3 LTE stack class.
 *****************************************************************************/

#ifndef SRSUE_UE_STACK_LTE_H
#define SRSUE_UE_STACK_LTE_H

#include "mac/mac.h"
#include "mac_nr/mac_nr.h"
#include "rrc/rrc.h"
#include "rrc/rrc_nr.h"
#include "srsran/common/bearer_manager.h"
#include "srsran/common/buffer_pool.h"
#include "srsran/common/multiqueue.h"
#include "srsran/common/string_helpers.h"
#include "srsran/common/task_scheduler.h"
#include "srsran/common/thread_pool.h"
#include "srsran/common/time_prof.h"
#include "srsran/interfaces/ue_interfaces.h"
#include "srsran/radio/radio.h"
#include "srsran/rlc/rlc.h"
#include "srsran/upper/pdcp.h"
#include "srsue/hdr/ue_metrics_interface.h"
#include "ue_stack_base.h"
#include "upper/nas.h"
#include "upper/usim.h"
#include <functional>
#include <pthread.h>
#include <stdarg.h>
#include <string>

namespace srsue {

class phy_interface_stack_lte;

class ue_stack_lte final : public ue_stack_base,
                           public stack_interface_phy_lte,
                           public stack_interface_phy_nr,
                           public stack_interface_gw,
                           public stack_interface_rrc,
                           public srsran::thread
{
public:
  explicit ue_stack_lte();
  ~ue_stack_lte();

  std::string get_type() final;

  int  init(const stack_args_t& args_);
  int  init(const stack_args_t& args_, phy_interface_stack_lte* phy_, gw_interface_stack* gw_);
  int  init(const stack_args_t&      args_,
            phy_interface_stack_lte* phy_,
            phy_interface_stack_nr*  phy_nr_,
            gw_interface_stack*      gw_);
  bool switch_on() final;
  bool switch_off() final;
  bool is_registered() final;
  bool start_service_request() final;
  bool enable_data();
  bool disable_data();
  void stop() final;

  bool get_metrics(stack_metrics_t* metrics) final;
  bool is_rrc_connected() { return rrc.is_connected(); };

  // RRC interface for PHY
  void in_sync() final;
  void out_of_sync() final;
  void new_cell_meas(const std::vector<phy_meas_t>& meas) override { rrc.new_cell_meas(meas); }
  void cell_search_complete(cell_search_ret_t ret, phy_cell_t found_cell) final;
  void cell_select_complete(bool status) final;
  void set_config_complete(bool status) final;
  void set_scell_complete(bool status) final;
  void set_phy_config_complete(bool status) final;

  // MAC Interface for EUTRA PHY
  uint16_t get_dl_sched_rnti(uint32_t tti) final { return mac.get_dl_sched_rnti(tti); }
  uint16_t get_ul_sched_rnti(uint32_t tti) final { return mac.get_ul_sched_rnti(tti); }

  sched_rnti_t get_dl_sched_rnti_nr(uint32_t tti) final { return mac_nr.get_dl_sched_rnti_nr(tti); }
  sched_rnti_t get_ul_sched_rnti_nr(uint32_t tti) final { return mac_nr.get_ul_sched_rnti_nr(tti); }

  void new_grant_ul(uint32_t cc_idx, mac_grant_ul_t grant, mac_interface_phy_lte::tb_action_ul_t* action) final
  {
    mac.new_grant_ul(cc_idx, grant, action);
  }

  void new_grant_dl(uint32_t cc_idx, mac_grant_dl_t grant, mac_interface_phy_lte::tb_action_dl_t* action) final
  {
    mac.new_grant_dl(cc_idx, grant, action);
  }

  void tb_decoded(uint32_t cc_idx, mac_grant_dl_t grant, bool ack[SRSRAN_MAX_CODEWORDS]) final
  {
    mac.tb_decoded(cc_idx, grant, ack);
  }

  void bch_decoded_ok(uint32_t cc_idx, uint8_t* payload, uint32_t len) final
  {
    mac.bch_decoded_ok(cc_idx, payload, len);
  }

  void mch_decoded(uint32_t len, bool crc) final { mac.mch_decoded(len, crc); }

  void new_mch_dl(const srsran_pdsch_grant_t& phy_grant, mac_interface_phy_lte::tb_action_dl_t* action) final
  {
    mac.new_mch_dl(phy_grant, action);
  }

  void set_mbsfn_config(uint32_t nof_mbsfn_services) final { mac.set_mbsfn_config(nof_mbsfn_services); }

  void run_tti(uint32_t tti, uint32_t tti_jump) final;

  // MAC Interface for NR PHY
  int  sf_indication(const uint32_t tti) final { return SRSRAN_SUCCESS; }
  void tb_decoded(const uint32_t                              cc_idx,
                  const mac_nr_grant_dl_t&                    grant,
                  mac_interface_phy_nr::tb_action_dl_result_t result) final
  {
    mac_nr.tb_decoded(cc_idx, grant, std::move(result));
  }
  void new_grant_dl(const uint32_t                        cc_idx,
                    const mac_nr_grant_dl_t&              grant,
                    mac_interface_phy_nr::tb_action_dl_t* action) final
  {
    mac_nr.new_grant_dl(cc_idx, grant, action);
  }
  void new_grant_ul(const uint32_t                        cc_idx,
                    const mac_nr_grant_ul_t&              grant,
                    mac_interface_phy_nr::tb_action_ul_t* action) final
  {
    mac_nr.new_grant_ul(cc_idx, grant, action);
  }

  void run_tti(const uint32_t tti) final
  {
    // ignored, timing will be handled by EUTRA
  }

  void prach_sent(uint32_t tti, uint32_t s_id, uint32_t t_id, uint32_t f_id, uint32_t ul_carrier_id) final
  {
    mac_nr.prach_sent(tti, s_id, t_id, f_id, ul_carrier_id);
  }

  bool sr_opportunity(uint32_t tti, uint32_t sr_id, bool meas_gap, bool ul_sch_tx) final
  {
    return mac_nr.sr_opportunity(tti, sr_id, meas_gap, ul_sch_tx);
  }

  // Interface for GW
  void write_sdu(uint32_t eps_bearer_id, srsran::unique_byte_buffer_t sdu) final;
  bool has_active_radio_bearer(uint32_t eps_bearer_id) final;

  // Interface for RRC
  tti_point get_current_tti() final { return current_tti; }
  void      add_eps_bearer(uint8_t eps_bearer_id, srsran::srsran_rat_t rat, uint32_t lcid) final;
  void      remove_eps_bearer(uint8_t eps_bearer_id) final;
  void      reset_eps_bearers() final;

  srsran::ext_task_sched_handle get_task_sched() { return {&task_sched}; }

private:
  void run_thread() final;
  void run_tti_impl(uint32_t tti, uint32_t tti_jump);
  void stop_impl();

  const uint32_t                  TTI_STAT_PERIOD = 1024;
  const std::chrono::milliseconds TTI_WARN_THRESHOLD_MS{5};
  const uint32_t                  SYNC_QUEUE_WARN_THRESHOLD = 5;

  std::atomic<bool>   running{false};
  srsue::stack_args_t args;

  srsran::tti_point current_tti;

  // UE stack logging
  srslog::basic_logger& stack_logger;
  srslog::basic_logger& mac_logger;
  srslog::basic_logger& rlc_logger;
  srslog::basic_logger& pdcp_logger;
  srslog::basic_logger& rrc_logger;
  srslog::basic_logger& usim_logger;
  srslog::basic_logger& nas_logger;

  // UE NR stack logging
  srslog::basic_logger& mac_nr_logger;
  srslog::basic_logger& rrc_nr_logger;
  srslog::basic_logger& rlc_nr_logger;
  srslog::basic_logger& pdcp_nr_logger;

  // tracing
  srsran::mac_pcap mac_pcap;
  srsran::mac_pcap mac_nr_pcap;
  srsran::nas_pcap nas_pcap;

  // RAT-specific interfaces
  phy_interface_stack_lte* phy    = nullptr;
  gw_interface_stack*      gw     = nullptr;
  phy_interface_stack_nr*  phy_nr = nullptr;

  // Thread
  static const int                      STACK_MAIN_THREAD_PRIO = 4; // Next lower priority after PHY workers
  srsran::block_queue<stack_metrics_t>  pending_stack_metrics;
  task_scheduler                        task_sched;
  srsran::task_multiqueue::queue_handle sync_task_queue, ue_task_queue, gw_queue_id, cfg_task_queue;

  // TTI stats
  srsran::tprof<srsran::sliding_window_stats_ms> tti_tprof;

  // stack components
  srsue::mac                 mac;
  srsran::rlc                rlc;
  srsran::pdcp               pdcp;
  srsue::rrc                 rrc;
  srsue::mac_nr              mac_nr;
  srsran::rlc                rlc_nr;
  srsran::pdcp               pdcp_nr;
  srsue::rrc_nr              rrc_nr;
  srsue::nas                 nas;
  std::unique_ptr<usim_base> usim;

  ue_bearer_manager bearers; // helper to manage mapping between EPS and radio bearers

  // Metrics helper
  std::atomic<uint32_t> ul_dropped_sdus{0};
};

} // namespace srsue

#endif // SRSUE_UE_STACK_LTE_H
