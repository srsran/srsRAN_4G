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

/******************************************************************************
 * File:        enb_stack_lte.h
 * Description: L2/L3 LTE eNB stack class.
 *****************************************************************************/

#ifndef SRSLTE_ENB_STACK_LTE_H
#define SRSLTE_ENB_STACK_LTE_H

#include "mac/mac.h"
#include "rrc/rrc.h"
#include "upper/gtpu.h"
#include "upper/pdcp.h"
#include "upper/rlc.h"
#include "upper/s1ap.h"

#include "enb_stack_base.h"
#include "srsenb/hdr/enb.h"
#include "srslte/common/multiqueue.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/enb_rrc_interface_types.h"

namespace srsenb {

class enb_stack_lte final : public enb_stack_base,
                            public stack_interface_phy_lte,
                            public stack_interface_s1ap_lte,
                            public stack_interface_gtpu_lte,
                            public stack_interface_mac_lte,
                            public srslte::thread
{
public:
  enb_stack_lte(srslte::logger* logger_);
  ~enb_stack_lte() final;

  // eNB stack base interface
  int         init(const stack_args_t& args_, const rrc_cfg_t& rrc_cfg_, phy_interface_stack_lte* phy_);
  int         init(const stack_args_t& args_, const rrc_cfg_t& rrc_cfg_);
  void        stop() final;
  std::string get_type() final;
  bool        get_metrics(stack_metrics_t* metrics) final;

  /* PHY-MAC interface */
  int  sr_detected(uint32_t tti, uint16_t rnti) final { return mac.sr_detected(tti, rnti); }
  void rach_detected(uint32_t tti, uint32_t primary_cc_idx, uint32_t preamble_idx, uint32_t time_adv) final
  {
    mac.rach_detected(tti, primary_cc_idx, preamble_idx, time_adv);
  }
  int ri_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t ri_value) final
  {
    return mac.ri_info(tti, rnti, cc_idx, ri_value);
  }
  int pmi_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t pmi_value) final
  {
    return mac.pmi_info(tti, rnti, cc_idx, pmi_value);
  }
  int cqi_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t cqi_value) final
  {
    return mac.cqi_info(tti, rnti, cc_idx, cqi_value);
  }
  int snr_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, float snr_db) final
  {
    return mac.snr_info(tti, rnti, cc_idx, snr_db);
  }
  int ta_info(uint32_t tti, uint16_t rnti, float ta_us) override { return mac.ta_info(tti, rnti, ta_us); }
  int ack_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack) final
  {
    return mac.ack_info(tti, rnti, enb_cc_idx, tb_idx, ack);
  }
  int crc_info(uint32_t tti, uint16_t rnti, uint32_t enb_cc_idx, uint32_t nof_bytes, bool crc_res) final
  {
    return mac.crc_info(tti, rnti, enb_cc_idx, nof_bytes, crc_res);
  }
  int get_dl_sched(uint32_t tti, dl_sched_list_t& dl_sched_res) final { return mac.get_dl_sched(tti, dl_sched_res); }
  int get_mch_sched(uint32_t tti, bool is_mcch, dl_sched_list_t& dl_sched_res) final
  {
    return mac.get_mch_sched(tti, is_mcch, dl_sched_res);
  }
  int  get_ul_sched(uint32_t tti, ul_sched_list_t& ul_sched_res) final { return mac.get_ul_sched(tti, ul_sched_res); }
  void set_sched_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs) final
  {
    mac.set_sched_dl_tti_mask(tti_mask, nof_sfs);
  }
  // Radio-Link status
  void rl_failure(uint16_t rnti) final { mac.rl_failure(rnti); }
  void rl_ok(uint16_t rnti) final { mac.rl_ok(rnti); }
  void tti_clock() override;

  /* STACK-S1AP interface*/
  void add_mme_socket(int fd) override;
  void remove_mme_socket(int fd) override;
  void add_gtpu_s1u_socket_handler(int fd) override;
  void add_gtpu_m1u_socket_handler(int fd) override;

  /* Stack-MAC interface */
  srslte::timer_handler::unique_timer    get_unique_timer() final;
  srslte::task_multiqueue::queue_handler make_task_queue() final;
  void                                   defer_callback(uint32_t duration_ms, std::function<void()> func) final;
  void                                   enqueue_background_task(std::function<void(uint32_t)> task) final;
  void                                   notify_background_task_result(srslte::move_task_t task) final;
  void                                   defer_task(srslte::move_task_t task) final;

private:
  static const int STACK_MAIN_THREAD_PRIO = -1; // Use default high-priority below UHD
  // thread loop
  void run_thread() override;
  void stop_impl();
  void tti_clock_impl();
  void handle_mme_rx_packet(srslte::unique_byte_buffer_t pdu,
                            const sockaddr_in&           from,
                            const sctp_sndrcvinfo&       sri,
                            int                          flags);

  // args
  stack_args_t args    = {};
  rrc_cfg_t    rrc_cfg = {};

  // components that layers depend on (need to be destroyed after layers)
  srslte::timer_handler                           timers;
  std::unique_ptr<srslte::rx_multisocket_handler> rx_sockets;

  srsenb::mac       mac;
  srslte::mac_pcap  mac_pcap;
  srsenb::rlc       rlc;
  srsenb::pdcp      pdcp;
  srsenb::rrc       rrc;
  srsenb::gtpu      gtpu;
  srsenb::s1ap      s1ap;
  srslte::s1ap_pcap s1ap_pcap;

  srslte::logger*           logger = nullptr;
  srslte::byte_buffer_pool* pool   = nullptr;

  // Radio and PHY log are in enb.cc
  srslte::log_ref mac_log{"MAC"};
  srslte::log_ref rlc_log{"RLC"};
  srslte::log_ref pdcp_log{"PDCP"};
  srslte::log_ref rrc_log{"RRC"};
  srslte::log_ref s1ap_log{"S1AP"};
  srslte::log_ref gtpu_log{"GTPU"};
  srslte::log_ref stack_log{"STCK"};

  // RAT-specific interfaces
  phy_interface_stack_lte* phy = nullptr;

  // state
  bool                    started = false;
  srslte::task_multiqueue pending_tasks;
  int enb_queue_id = -1, sync_queue_id = -1, mme_queue_id = -1, gtpu_queue_id = -1, mac_queue_id = -1,
      stack_queue_id = -1;
  std::vector<srslte::move_task_t>     deferred_stack_tasks; ///< enqueues stack tasks from within. Avoids locking
  srslte::block_queue<stack_metrics_t> pending_stack_metrics;
};

} // namespace srsenb

#endif // SRSLTE_ENB_STACK_LTE_H
