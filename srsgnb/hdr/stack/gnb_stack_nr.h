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

/******************************************************************************
 * File:        gnb_stack_nr.h
 * Description: L2/L3 gNB stack class.
 *****************************************************************************/

#ifndef SRSRAN_GNB_STACK_NR_H
#define SRSRAN_GNB_STACK_NR_H

#include "srsenb/hdr/stack/upper/pdcp.h"
#include "srsenb/hdr/stack/upper/rlc.h"
#include "srsgnb/hdr/stack/mac/mac_nr.h"
#include "srsgnb/hdr/stack/rrc/rrc_nr.h"
#include "srsgnb/hdr/stack/sdap/sdap.h"

#include "srsenb/hdr/stack/enb_stack_base.h"
#include "srsran/interfaces/gnb_interfaces.h"

#include "srsran/common/ngap_pcap.h"

namespace srsenb {

class ngap;
class gtpu;
class enb_bearer_manager;
class gtpu_pdcp_adapter;

struct gnb_stack_args_t {
  stack_log_args_t log;
  mac_nr_args_t    mac;
  ngap_args_t      ngap;
  pcap_args_t      ngap_pcap;
};

class gnb_stack_nr final : public srsenb::enb_stack_base,
                           public stack_interface_phy_nr,
                           public stack_interface_mac,
                           public srsue::stack_interface_gw,
                           public rrc_nr_interface_rrc,
                           public pdcp_interface_gtpu, // for user-plane over X2
                           public srsran::thread
{
public:
  explicit gnb_stack_nr(srslog::sink& log_sink);
  ~gnb_stack_nr() final;

  int init(const gnb_stack_args_t& args_,
           const rrc_nr_cfg_t&     rrc_cfg_,
           phy_interface_stack_nr* phy_,
           x2_interface*           x2_);

  // eNB stack base interface
  void        stop() final;
  std::string get_type() final;
  bool        get_metrics(srsenb::stack_metrics_t* metrics) final;

  // GW srsue stack_interface_gw dummy interface
  bool is_registered() override { return true; };
  bool start_service_request() override { return true; };

  // Temporary GW interface
  void write_sdu(uint32_t lcid, srsran::unique_byte_buffer_t sdu) override;
  bool has_active_radio_bearer(uint32_t eps_bearer_id) override;
  bool switch_on();
  void tti_clock() override;

  // MAC interface to trigger processing of received PDUs
  void process_pdus() final;

  void toggle_padding() override {}

  int         slot_indication(const srsran_slot_cfg_t& slot_cfg) override;
  dl_sched_t* get_dl_sched(const srsran_slot_cfg_t& slot_cfg) override;
  ul_sched_t* get_ul_sched(const srsran_slot_cfg_t& slot_cfg) override;
  int         pucch_info(const srsran_slot_cfg_t& slot_cfg, const pucch_info_t& pucch_info) override;
  int         pusch_info(const srsran_slot_cfg_t& slot_cfg, pusch_info_t& pusch_info) override;
  void        rach_detected(const rach_info_t& rach_info) override;

  // X2 interface

  // control plane, i.e. rrc_nr_interface_rrc
  void sgnb_addition_request(uint16_t eutra_rnti, const sgnb_addition_req_params_t& params) final
  {
    x2_task_queue.push([this, eutra_rnti, params]() { rrc.sgnb_addition_request(eutra_rnti, params); });
  };
  void sgnb_reconfiguration_complete(uint16_t eutra_rnti, const asn1::dyn_octstring& reconfig_response) final
  {
    x2_task_queue.push(
        [this, eutra_rnti, reconfig_response]() { rrc.sgnb_reconfiguration_complete(eutra_rnti, reconfig_response); });
  };
  void sgnb_release_request(uint16_t nr_rnti) final
  {
    x2_task_queue.push([this, nr_rnti]() { return rrc.sgnb_release_request(nr_rnti); });
  }
  // X2 data interface
  void write_sdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t sdu, int pdcp_sn = -1) final
  {
    auto task = [this, rnti, lcid, pdcp_sn](srsran::unique_byte_buffer_t& sdu) {
      pdcp.write_sdu(rnti, lcid, std::move(sdu), pdcp_sn);
    };
    gtpu_task_queue.push(std::bind(task, std::move(sdu)));
  }
  std::map<uint32_t, srsran::unique_byte_buffer_t> get_buffered_pdus(uint16_t rnti, uint32_t lcid) final
  {
    // TODO: make it thread-safe. For now, this function is unused
    return pdcp.get_buffered_pdus(rnti, lcid);
  }

private:
  void run_thread() final;
  void tti_clock_impl();
  void stop_impl();

  // args
  gnb_stack_args_t        args = {};
  phy_interface_stack_nr* phy  = nullptr;

  srslog::basic_logger& rrc_logger;
  srslog::basic_logger& mac_logger;
  srslog::basic_logger& rlc_logger;
  srslog::basic_logger& pdcp_logger;
  srslog::basic_logger& ngap_logger;
  srslog::basic_logger& gtpu_logger;
  srslog::basic_logger& stack_logger;

  srsran::ngap_pcap ngap_pcap;

  // task scheduling
  static const int                      STACK_MAIN_THREAD_PRIO = 4;
  srsran::task_scheduler                task_sched;
  srsran::task_multiqueue::queue_handle sync_task_queue, gtpu_task_queue, metrics_task_queue, gnb_task_queue,
      x2_task_queue;

  // metrics waiting condition
  std::mutex              metrics_mutex;
  std::condition_variable metrics_cvar;

  // layers
  srsenb::mac_nr                mac;
  srsenb::rlc                   rlc;
  srsenb::pdcp                  pdcp;
  srsenb::rrc_nr                rrc;
  std::unique_ptr<srsenb::ngap> ngap;
  std::unique_ptr<srsenb::gtpu> gtpu;
  // std::unique_ptr<sdap> m_sdap;

  std::unique_ptr<enb_bearer_manager> bearer_manager;
  std::unique_ptr<gtpu_pdcp_adapter>  gtpu_adapter;

  // state
  std::atomic<bool> running = {false};
};

} // namespace srsenb

#endif // SRSRAN_GNB_STACK_NR_H
