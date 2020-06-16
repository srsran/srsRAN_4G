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

#include "srsenb/hdr/stack/gnb_stack_nr.h"
#include "srslte/srslte.h"
#include <srslte/interfaces/enb_metrics_interface.h>

namespace srsenb {

gnb_stack_nr::gnb_stack_nr(srslte::logger* logger_) : logger(logger_), timers(128), thread("gNB"), background_tasks(1)
{
  m_mac.reset(new mac_nr());
  m_rlc.reset(new rlc_nr("RLC"));
  m_pdcp.reset(new pdcp_nr(this, "PDCP"));
  m_rrc.reset(new rrc_nr(&timers));
  m_sdap.reset(new sdap());
  m_gw.reset(new srsue::gw());
  //  m_gtpu.reset(new srsenb::gtpu());

  ue_queue_id         = pending_tasks.add_queue();
  sync_queue_id       = pending_tasks.add_queue();
  gw_queue_id         = pending_tasks.add_queue();
  mac_queue_id        = pending_tasks.add_queue();
  background_queue_id = pending_tasks.add_queue();

  background_tasks.start();
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
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
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
  mac_args.log_level     = args.log.mac_level;
  mac_args.log_hex_limit = args.log.mac_hex_limit;
  mac_args.pcap          = args.mac_pcap;
  mac_args.sched         = args.mac.sched;
  mac_args.rnti          = args.coreless.rnti;
  mac_args.drb_lcid      = args.coreless.drb_lcid;
  m_mac->init(mac_args, phy, this, m_rlc.get(), m_rrc.get());

  m_rlc->init(m_pdcp.get(), m_rrc.get(), m_mac.get(), &timers);

  pdcp_nr_args_t pdcp_args = {};
  pdcp_args.log_level      = args.log.pdcp_level;
  pdcp_args.log_hex_limit  = args.log.pdcp_hex_limit;
  m_pdcp->init(pdcp_args, m_rlc.get(), m_rrc.get(), m_sdap.get());

  m_rrc->init(rrc_cfg_, phy, m_mac.get(), m_rlc.get(), m_pdcp.get(), nullptr, nullptr);

  m_sdap->init(m_pdcp.get(), nullptr, m_gw.get());

  m_gw->init(args.coreless.gw_args, logger, this);
  char* err_str = nullptr;
  if (m_gw->setup_if_addr(args.coreless.drb_lcid,
                          LIBLTE_MME_PDN_TYPE_IPV4,
                          htonl(inet_addr(args.coreless.ip_addr.c_str())),
                          nullptr,
                          err_str)) {
    printf("Error configuring TUN interface\n");
  }

  // TODO: add NGAP
  //  m_gtpu->init(args.s1ap.gtp_bind_addr, args.s1ap.mme_addr,
  //      args.expert.m1u_multiaddr, args.expert.m1u_if_addr, nullptr, &gtpu_log,
  //      args.expert.enable_mbsfn);

  running = true;

  start(STACK_MAIN_THREAD_PRIO);

  return SRSLTE_SUCCESS;
}

void gnb_stack_nr::stop()
{
  if (running) {
    m_gw->stop();
    //    m_gtpu->stop();
    m_rrc->stop();
    m_pdcp->stop();
    m_mac->stop();

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
    srslte::move_task_t task{};
    pending_tasks.wait_pop(&task);
    if (running) {
      task();
    }
  }
}

void gnb_stack_nr::run_tti(uint32_t tti)
{
  current_tti = tti;
  pending_tasks.push(sync_queue_id, [this, tti]() { run_tti_impl(tti); });
}

void gnb_stack_nr::run_tti_impl(uint32_t tti)
{
  //  m_ngap->run_tti();
  timers.step_all();
}

void gnb_stack_nr::process_pdus()
{
  pending_tasks.push(mac_queue_id, [this]() { m_mac->process_pdus(); });
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

int gnb_stack_nr::sf_indication(const uint32_t tti)
{
  return m_mac->sf_indication(tti);
}

int gnb_stack_nr::rx_data_indication(rx_data_ind_t& grant)
{
  return m_mac->rx_data_indication(grant);
}

// Temporary GW interface
void gnb_stack_nr::write_sdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu, bool blocking)
{
  m_pdcp->write_sdu(args.coreless.rnti, lcid, std::move(sdu));
}

bool gnb_stack_nr::is_lcid_enabled(uint32_t lcid)
{
  return (lcid == args.coreless.drb_lcid);
}

/***************************
 * Task Handling Interface
 **************************/

void gnb_stack_nr::enqueue_background_task(std::function<void(uint32_t)> f)
{
  background_tasks.push_task(std::move(f));
}

void gnb_stack_nr::notify_background_task_result(srslte::move_task_t task)
{
  // run the notification in the stack thread
  pending_tasks.push(background_queue_id, std::move(task));
}

void gnb_stack_nr::defer_callback(uint32_t duration_ms, std::function<void()> func)
{
  timers.defer_callback(duration_ms, func);
}

void gnb_stack_nr::defer_task(srslte::move_task_t task)
{
  deferred_stack_tasks.push_back(std::move(task));
}

} // namespace srsenb
