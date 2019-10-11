/*
 * Copyright 2013-2019 Software Radio Systems Limited
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
/******************************************************************************
 * File:        ue_stack.h
 * Description: L2/L3 LTE stack class.
 *****************************************************************************/

#ifndef SRSUE_UE_STACK_LTE_H
#define SRSUE_UE_STACK_LTE_H

#include <functional>
#include <pthread.h>
#include <stdarg.h>
#include <string>

#include "mac/mac.h"
#include "rrc/rrc.h"
#include "srslte/radio/radio.h"
#include "srslte/upper/pdcp.h"
#include "srslte/upper/rlc.h"
#include "upper/nas.h"
#include "upper/usim.h"

#include "srslte/common/buffer_pool.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/multiqueue.h"
#include "srslte/common/thread_pool.h"
#include "srslte/interfaces/ue_interfaces.h"

#include "srsue/hdr/ue_metrics_interface.h"
#include "ue_stack_base.h"

namespace srsue {

class ue_stack_lte final : public ue_stack_base,
                           public stack_interface_phy_lte,
                           public stack_interface_gw,
                           public stack_interface_mac,
                           public stack_interface_rrc,
                           public thread
{
public:
  ue_stack_lte();
  ~ue_stack_lte();

  std::string get_type() final;

  int  init(const stack_args_t& args_, srslte::logger* logger_);
  int  init(const stack_args_t& args_, srslte::logger* logger_, phy_interface_stack_lte* phy_, gw_interface_stack* gw_);
  bool switch_on() final;
  bool switch_off();
  bool enable_data();
  bool disable_data();
  void stop();

  bool get_metrics(stack_metrics_t* metrics);
  bool is_rrc_connected();

  // RRC interface for PHY
  void in_sync() final;
  void out_of_sync() final;
  void new_phy_meas(float rsrp, float rsrq, uint32_t tti, int earfcn = -1, int pci = -1)
  {
    rrc.new_phy_meas(rsrp, rsrq, tti, earfcn, pci);
  };

  // MAC Interface for PHY
  uint16_t get_dl_sched_rnti(uint32_t tti) { return mac.get_dl_sched_rnti(tti); }
  uint16_t get_ul_sched_rnti(uint32_t tti) { return mac.get_ul_sched_rnti(tti); }

  void new_grant_ul(uint32_t cc_idx, mac_grant_ul_t grant, tb_action_ul_t* action)
  {
    mac.new_grant_ul(cc_idx, grant, action);
  }

  void new_grant_dl(uint32_t cc_idx, mac_grant_dl_t grant, tb_action_dl_t* action)
  {
    mac.new_grant_dl(cc_idx, grant, action);
  }

  void tb_decoded(uint32_t cc_idx, mac_grant_dl_t grant, bool ack[SRSLTE_MAX_CODEWORDS])
  {
    mac.tb_decoded(cc_idx, grant, ack);
  }

  void bch_decoded_ok(uint8_t* payload, uint32_t len) { mac.bch_decoded_ok(payload, len); }

  void mch_decoded(uint32_t len, bool crc) { mac.mch_decoded(len, crc); }

  void new_mch_dl(srslte_pdsch_grant_t phy_grant, tb_action_dl_t* action) { mac.new_mch_dl(phy_grant, action); }

  void set_mbsfn_config(uint32_t nof_mbsfn_services) { mac.set_mbsfn_config(nof_mbsfn_services); }

  void run_tti(uint32_t tti) final;

  // Interface for GW
  void write_sdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu, bool blocking) final;

  bool is_lcid_enabled(uint32_t lcid) final { return pdcp.is_lcid_enabled(lcid); }

  // Interface to upper MAC
  void process_pdus() final;
  void wait_ra_completion(uint16_t rnti) final;

  // Interface for RRC
  void start_cell_search() final;

private:
  void run_thread() final;
  void run_tti_impl(uint32_t tti);
  void stop_impl();

  bool                running;
  srsue::stack_args_t args;

  // timers
  srslte::timers timers;

  // UE stack logging
  srslte::logger*    logger = nullptr;
  srslte::log_filter mac_log;
  srslte::log_filter rlc_log;
  srslte::log_filter pdcp_log;
  srslte::log_filter rrc_log;
  srslte::log_filter nas_log;
  srslte::log_filter usim_log;
  srslte::log_filter pool_log;

  // stack components
  srsue::mac                 mac;
  srslte::mac_pcap           mac_pcap;
  srslte::nas_pcap           nas_pcap;
  srslte::rlc                rlc;
  srslte::pdcp               pdcp;
  srsue::rrc                 rrc;
  srsue::nas                 nas;
  std::unique_ptr<usim_base> usim;

  // RAT-specific interfaces
  phy_interface_stack_lte* phy = nullptr;
  gw_interface_stack*      gw  = nullptr;

  // Thread
  static const int STACK_MAIN_THREAD_PRIO = -1; // Use default high-priority below UHD

  // NOTE: we use this struct instead of a std::function bc lambdas can't capture by move in C++11
  struct task_t {
    std::function<void(task_t*)> func;
    srslte::unique_byte_buffer_t pdu;
    task_t() = default;
    explicit task_t(std::function<void(task_t*)> f_) : func(std::move(f_)) {}
    void operator()() { func(this); }
  };
  srslte::multiqueue_handler<task_t> pending_tasks;
  int sync_queue_id = -1, ue_queue_id = -1, gw_queue_id = -1, mac_queue_id = -1, background_queue_id = -1;
  srslte::task_thread_pool background_tasks; ///< Thread pool used for long, low-priority tasks
};

} // namespace srsue

#endif // SRSUE_UE_STACK_LTE_H
