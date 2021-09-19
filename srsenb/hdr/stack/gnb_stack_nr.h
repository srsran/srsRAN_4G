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
 * File:        gnb_stack_nr.h
 * Description: L2/L3 gNB stack class.
 *****************************************************************************/

#ifndef SRSRAN_GNB_STACK_NR_H
#define SRSRAN_GNB_STACK_NR_H

#include "s1ap/s1ap.h"
#include "srsenb/hdr/stack/mac/nr/mac_nr.h"
#include "srsenb/hdr/stack/rrc/rrc_nr.h"
#include "srsenb/hdr/stack/upper/pdcp_nr.h"
#include "srsenb/hdr/stack/upper/rlc_nr.h"
#include "upper/gtpu.h"
#include "upper/sdap.h"

#include "enb_stack_base.h"
#include "srsenb/hdr/enb.h"
#include "srsran/interfaces/gnb_interfaces.h"

// This is needed for GW
#include "srsran/interfaces/ue_interfaces.h"
#include "srsue/hdr/stack/upper/gw.h"

namespace srsenb {

class gnb_stack_nr final : public srsenb::enb_stack_base,
                           public stack_interface_phy_nr,
                           public stack_interface_mac,
                           public srsue::stack_interface_gw,
                           public srsran::thread
{
public:
  explicit gnb_stack_nr();
  ~gnb_stack_nr() final;

  int init(const srsenb::stack_args_t& args_, const rrc_nr_cfg_t& rrc_cfg_, phy_interface_stack_nr* phy_);
  int init(const srsenb::stack_args_t& args_, const rrc_nr_cfg_t& rrc_cfg_);

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
  void run_tti(uint32_t tti);

  // MAC interface to trigger processing of received PDUs
  void process_pdus() final;

  void toggle_padding() override { srsran::console("padding not available for NR\n"); }

  int  slot_indication(const srsran_slot_cfg_t& slot_cfg) override;
  int  get_dl_sched(const srsran_slot_cfg_t& slot_cfg, dl_sched_t& dl_sched) override;
  int  get_ul_sched(const srsran_slot_cfg_t& slot_cfg, ul_sched_t& ul_sched) override;
  int  pucch_info(const srsran_slot_cfg_t& slot_cfg, const pucch_info_t& pucch_info) override;
  int  pusch_info(const srsran_slot_cfg_t& slot_cfg, pusch_info_t& pusch_info) override;
  void rach_detected(const rach_info_t& rach_info) override;

private:
  void run_thread() final;
  void run_tti_impl(uint32_t tti);

  // args
  srsenb::stack_args_t    args = {};
  phy_interface_stack_nr* phy  = nullptr;

  srslog::basic_logger& rlc_logger;

  // task scheduling
  static const int                      STACK_MAIN_THREAD_PRIO = 4;
  srsran::task_scheduler                task_sched;
  srsran::task_multiqueue::queue_handle sync_task_queue, ue_task_queue, gw_task_queue, mac_task_queue;

  // derived
  std::unique_ptr<mac_nr>    m_mac;
  std::unique_ptr<rlc_nr>    m_rlc;
  std::unique_ptr<pdcp_nr>   m_pdcp;
  std::unique_ptr<sdap>      m_sdap;
  std::unique_ptr<rrc_nr>    m_rrc;
  std::unique_ptr<srsue::gw> m_gw;
  //  std::unique_ptr<ngap>      m_ngap;
  //  std::unique_ptr<srsenb::gtpu> m_gtpu;

  // state
  bool     running     = false;
  uint32_t current_tti = 10240;
};

} // namespace srsenb

#endif // SRSRAN_GNB_STACK_NR_H
