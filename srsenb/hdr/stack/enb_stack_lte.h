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

/******************************************************************************
 * File:        enb_stack_lte.h
 * Description: L2/L3 LTE eNB stack class.
 *****************************************************************************/

#ifndef SRSRAN_ENB_STACK_LTE_H
#define SRSRAN_ENB_STACK_LTE_H

#include "mac/mac.h"
#include "rrc/rrc.h"
#include "s1ap/s1ap.h"
#include "srsran/common/task_scheduler.h"
#include "upper/gtpu.h"
#include "upper/pdcp.h"
#include "upper/rlc.h"

#include "enb_stack_base.h"
#include "srsran/common/bearer_manager.h"
#include "srsran/common/mac_pcap_net.h"
#include "srsran/interfaces/enb_interfaces.h"
#include "srsran/srslog/srslog.h"

namespace srsenb {

class gtpu_pdcp_adapter;

class enb_stack_lte final : public enb_stack_base,
                            public stack_interface_phy_lte,
                            public rrc_eutra_interface_rrc_nr,
                            public srsran::thread
{
public:
  enb_stack_lte(srslog::sink& log_sink);
  ~enb_stack_lte() final;

  // eNB stack base interface
  int  init(const stack_args_t& args_, const rrc_cfg_t& rrc_cfg_, phy_interface_stack_lte* phy_, x2_interface* x2_);
  void stop() final;
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
  int sb_cqi_info(uint32_t tti, uint16_t rnti, uint32_t cc_idx, uint32_t sb_idx, uint32_t cqi_value) final
  {
    return mac.sb_cqi_info(tti, rnti, cc_idx, sb_idx, cqi_value);
  }
  int snr_info(uint32_t tti_rx, uint16_t rnti, uint32_t cc_idx, float snr_db, ul_channel_t ch) final
  {
    return mac.snr_info(tti_rx, rnti, cc_idx, snr_db, ch);
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
  int push_pdu(uint32_t tti,
               uint16_t rnti,
               uint32_t enb_cc_idx,
               uint32_t nof_bytes,
               bool     crc_res,
               uint32_t grant_nof_prbs) final
  {
    return mac.push_pdu(tti, rnti, enb_cc_idx, nof_bytes, crc_res, grant_nof_prbs);
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
  void toggle_padding() override { mac.toggle_padding(); }
  void tti_clock() override;

  // rrc_eutra_interface_rrc_nr
  void sgnb_addition_ack(uint16_t eutra_rnti, sgnb_addition_ack_params_t params) final
  {
    x2_task_queue.push([this, eutra_rnti, params]() { rrc.sgnb_addition_ack(eutra_rnti, params); });
  }
  void sgnb_addition_reject(uint16_t eutra_rnti) final
  {
    x2_task_queue.push([this, eutra_rnti]() { rrc.sgnb_addition_reject(eutra_rnti); });
  }
  void sgnb_addition_complete(uint16_t eutra_rnti, uint16_t nr_rnti) final
  {
    x2_task_queue.push([this, eutra_rnti, nr_rnti]() { rrc.sgnb_addition_complete(eutra_rnti, nr_rnti); });
  }
  void sgnb_inactivity_timeout(uint16_t eutra_rnti) final
  {
    x2_task_queue.push([this, eutra_rnti]() { rrc.sgnb_inactivity_timeout(eutra_rnti); });
  }
  void set_activity_user(uint16_t eutra_rnti) final
  {
    // Note: RRC processes activity asynchronously, so there is no need to use x2_task_queue
    rrc.set_activity_user(eutra_rnti);
  }
  void sgnb_release_ack(uint16_t eutra_rnti) final
  {
    x2_task_queue.push([this, eutra_rnti]() { rrc.sgnb_release_ack(eutra_rnti); });
  }

  // gtpu_interface_pdcp
  void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu);

private:
  static const int STACK_MAIN_THREAD_PRIO = 4;
  // thread loop
  void run_thread() override;
  void stop_impl();
  void tti_clock_impl();

  // args
  stack_args_t args    = {};
  rrc_cfg_t    rrc_cfg = {};

  srslog::basic_logger& mac_logger;
  srslog::basic_logger& rlc_logger;
  srslog::basic_logger& pdcp_logger;
  srslog::basic_logger& rrc_logger;
  srslog::basic_logger& s1ap_logger;
  srslog::basic_logger& gtpu_logger;
  srslog::basic_logger& stack_logger;

  // PCAP and trace option
  srsran::mac_pcap     mac_pcap;
  srsran::mac_pcap_net mac_pcap_net;
  srsran::s1ap_pcap    s1ap_pcap;

  // task handling
  srsran::task_scheduler    task_sched;
  srsran::task_queue_handle enb_task_queue, sync_task_queue, metrics_task_queue, x2_task_queue;

  // bearer management
  enb_bearer_manager                 bearers; // helper to manage mapping between EPS and radio bearers
  std::unique_ptr<gtpu_pdcp_adapter> gtpu_adapter;

  srsenb::mac  mac;
  srsenb::rlc  rlc;
  srsenb::pdcp pdcp;
  srsenb::rrc  rrc;
  srsenb::gtpu gtpu;
  srsenb::s1ap s1ap;

  // RAT-specific interfaces
  phy_interface_stack_lte* phy = nullptr;

  // state
  std::atomic<bool> started{false};

  srsran::dyn_blocking_queue<stack_metrics_t> pending_stack_metrics;
};

} // namespace srsenb

#endif // SRSRAN_ENB_STACK_LTE_H
