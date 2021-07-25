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

#include "srsenb/hdr/stack/gnb_stack_nr.h"
#include "srsran/srsran.h"
#include <srsran/interfaces/enb_metrics_interface.h>

namespace srsenb {

gnb_stack_nr::gnb_stack_nr() : task_sched{512, 128}, thread("gNB"), rlc_logger(srslog::fetch_basic_logger("RLC-NR"))
{
  m_mac.reset(new mac_nr(&task_sched));
  m_rlc.reset(new rlc_nr("RLC-NR"));
  m_pdcp.reset(new pdcp_nr(&task_sched, "PDCP-NR"));
  m_rrc.reset(new rrc_nr(&task_sched));
  m_sdap.reset(new sdap());
  m_gw.reset(new srsue::gw(srslog::fetch_basic_logger("GW")));
  //  m_gtpu.reset(new srsenb::gtpu());

  ue_task_queue   = task_sched.make_task_queue();
  sync_task_queue = task_sched.make_task_queue();
  gw_task_queue   = task_sched.make_task_queue();
  mac_task_queue  = task_sched.make_task_queue();
}

gnb_stack_nr::~gnb_stack_nr()
{
  stop();
}

std::string gnb_stack_nr::get_type()
{
  return "nr";
}

int gnb_stack_nr::init(const srsenb::stack_args_t& args_, const rrc_nr_cfg_t& rrc_cfg_, phy_interface_stack_nr* phy_)
{
  phy = phy_;
  if (init(args_, rrc_cfg_)) {
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

int gnb_stack_nr::init(const srsenb::stack_args_t& args_, const rrc_nr_cfg_t& rrc_cfg_)
{
  args = args_;

  // verify configuration correctness
  //  gtpu_log.init("GTPU", logger);
  //  gtpu_log.set_level(args.log.gtpu_level);
  //  gtpu_log.set_hex_limit(args.log.gtpu_hex_limit);

  // Init all layers
  mac_nr_args_t mac_args = {};
  mac_args.pcap          = args.mac_pcap;
  m_mac->init(mac_args, phy, this, nullptr, m_rrc.get());

  rlc_logger.set_level(srslog::str_to_basic_level(args.log.rlc_level));
  rlc_logger.set_hex_dump_max_size(args.log.rlc_hex_limit);
  m_rlc->init(m_pdcp.get(), m_rrc.get(), m_mac.get(), task_sched.get_timer_handler());

  pdcp_nr_args_t pdcp_args = {};
  pdcp_args.log_level      = args.log.pdcp_level;
  pdcp_args.log_hex_limit  = args.log.pdcp_hex_limit;
  m_pdcp->init(pdcp_args, m_rlc.get(), m_rrc.get(), m_sdap.get());

  m_rrc->init(rrc_cfg_, phy, m_mac.get(), nullptr, nullptr, nullptr, nullptr, nullptr);

  m_sdap->init(m_pdcp.get(), nullptr, m_gw.get());

  srsue::gw_args_t gw_args = {};
  m_gw->init(gw_args, this);

  // TODO: add NGAP
  //  m_gtpu->init(args.s1ap.gtp_bind_addr, args.s1ap.mme_addr,
  //      args.expert.m1u_multiaddr, args.expert.m1u_if_addr, nullptr, &gtpu_log,
  //      args.expert.enable_mbsfn);

  running = true;

  start(STACK_MAIN_THREAD_PRIO);

  return SRSRAN_SUCCESS;
}

void gnb_stack_nr::stop()
{
  if (running) {
    m_gw->stop();
    //    m_gtpu->stop();
    m_rrc->stop();
    m_pdcp->stop();
    m_mac->stop();

    srsran::get_background_workers().stop();
    running = false;
  }
}

bool gnb_stack_nr::switch_on()
{
  // Nothing to be done here
  return true;
}

void gnb_stack_nr::run_thread()
{
  while (running) {
    task_sched.run_next_task();
  }
}

void gnb_stack_nr::run_tti(uint32_t tti)
{
  current_tti = tti;
  sync_task_queue.push([this, tti]() { run_tti_impl(tti); });
}

void gnb_stack_nr::run_tti_impl(uint32_t tti)
{
  //  m_ngap->run_tti();
  task_sched.tic();
}

void gnb_stack_nr::process_pdus()
{
}

/********************************************************
 *
 * Interface for upper layer timers
 *
 *******************************************************/

bool gnb_stack_nr::get_metrics(srsenb::stack_metrics_t* metrics)
{
  m_mac->get_metrics(metrics->mac);
  m_rrc->get_metrics(metrics->rrc);
  return true;
}

int gnb_stack_nr::rx_data_indication(rx_data_ind_t& grant)
{
  return m_mac->rx_data_indication(grant);
}

// Temporary GW interface
void gnb_stack_nr::write_sdu(uint32_t lcid, srsran::unique_byte_buffer_t sdu)
{
  // not implemented
}

bool gnb_stack_nr::has_active_radio_bearer(uint32_t eps_bearer_id)
{
  return false;
}
int gnb_stack_nr::slot_indication(const srsran_slot_cfg_t& slot_cfg)
{
  return m_mac->slot_indication(slot_cfg);
}
int gnb_stack_nr::get_dl_sched(const srsran_slot_cfg_t& slot_cfg, dl_sched_t& dl_sched)
{
  return m_mac->get_dl_sched(slot_cfg, dl_sched);
}
int gnb_stack_nr::get_ul_sched(const srsran_slot_cfg_t& slot_cfg, ul_sched_t& ul_sched)
{
  return m_mac->get_ul_sched(slot_cfg, ul_sched);
}
int gnb_stack_nr::pucch_info(const srsran_slot_cfg_t& slot_cfg, const mac_interface_phy_nr::pucch_info_t& pucch_info)
{
  return m_mac->pucch_info(slot_cfg, pucch_info);
}
int gnb_stack_nr::pusch_info(const srsran_slot_cfg_t& slot_cfg, const mac_interface_phy_nr::pusch_info_t& pusch_info)
{
  return m_mac->pusch_info(slot_cfg, pusch_info);
}

void gnb_stack_nr::rach_detected(const rach_info_t& rach_info) {}

} // namespace srsenb
