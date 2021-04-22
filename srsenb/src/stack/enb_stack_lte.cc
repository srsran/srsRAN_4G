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

#include "srsenb/hdr/stack/enb_stack_lte.h"
#include "srsenb/hdr/common/rnti_pool.h"
#include "srsenb/hdr/enb.h"
#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsran/srslog/event_trace.h"
#include "srsran/upper/bearer_mem_pool.h"

using namespace srsran;

namespace srsenb {

enb_stack_lte::enb_stack_lte(srslog::sink& log_sink) :
  thread("STACK"),
  mac_logger(srslog::fetch_basic_logger("MAC", log_sink)),
  rlc_logger(srslog::fetch_basic_logger("RLC", log_sink, false)),
  pdcp_logger(srslog::fetch_basic_logger("PDCP", log_sink, false)),
  rrc_logger(srslog::fetch_basic_logger("RRC", log_sink, false)),
  s1ap_logger(srslog::fetch_basic_logger("S1AP", log_sink, false)),
  gtpu_logger(srslog::fetch_basic_logger("GTPU", log_sink, false)),
  stack_logger(srslog::fetch_basic_logger("STCK", log_sink, false)),
  task_sched(512, 128),
  pdcp(&task_sched, pdcp_logger),
  mac(&task_sched, mac_logger),
  rlc(rlc_logger),
  gtpu(&task_sched, gtpu_logger, &rx_sockets),
  s1ap(&task_sched, s1ap_logger, &rx_sockets),
  rrc(&task_sched),
  mac_pcap(),
  pending_stack_metrics(64)
{
  get_background_workers().set_nof_workers(2);
  enb_task_queue  = task_sched.make_task_queue();
  mme_task_queue  = task_sched.make_task_queue();
  gtpu_task_queue = task_sched.make_task_queue();
  // sync_queue is added in init()
}

enb_stack_lte::~enb_stack_lte()
{
  stop();
}

std::string enb_stack_lte::get_type()
{
  return "lte";
}

int enb_stack_lte::init(const stack_args_t& args_, const rrc_cfg_t& rrc_cfg_, phy_interface_stack_lte* phy_)
{
  phy = phy_;
  if (init(args_, rrc_cfg_)) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int enb_stack_lte::init(const stack_args_t& args_, const rrc_cfg_t& rrc_cfg_)
{
  args    = args_;
  rrc_cfg = rrc_cfg_;

  // Init RNTI and bearer memory pools
  reserve_rnti_memblocks(args.mac.max_nof_ues);
  uint32_t min_nof_bearers_per_ue = 4;
  reserve_rlc_memblocks(args.mac.max_nof_ues * min_nof_bearers_per_ue);

  // setup logging for each layer
  mac_logger.set_level(srslog::str_to_basic_level(args.log.mac_level));
  mac_logger.set_hex_dump_max_size(args.log.mac_hex_limit);

  // Init logs
  rlc_logger.set_level(srslog::str_to_basic_level(args.log.rlc_level));
  pdcp_logger.set_level(srslog::str_to_basic_level(args.log.pdcp_level));
  rrc_logger.set_level(srslog::str_to_basic_level(args.log.rrc_level));
  gtpu_logger.set_level(srslog::str_to_basic_level(args.log.gtpu_level));
  s1ap_logger.set_level(srslog::str_to_basic_level(args.log.s1ap_level));
  stack_logger.set_level(srslog::str_to_basic_level(args.log.stack_level));

  rlc_logger.set_hex_dump_max_size(args.log.rlc_hex_limit);
  pdcp_logger.set_hex_dump_max_size(args.log.pdcp_hex_limit);
  rrc_logger.set_hex_dump_max_size(args.log.rrc_hex_limit);
  gtpu_logger.set_hex_dump_max_size(args.log.gtpu_hex_limit);
  s1ap_logger.set_hex_dump_max_size(args.log.s1ap_hex_limit);
  stack_logger.set_hex_dump_max_size(args.log.stack_hex_limit);

  // Set up pcap and trace
  if (args.mac_pcap.enable) {
    mac_pcap.open(args.mac_pcap.filename.c_str());
    mac.start_pcap(&mac_pcap);
  }

  if (args.mac_pcap_net.enable) {
    mac_pcap_net.open(args.mac_pcap_net.client_ip,
                      args.mac_pcap_net.bind_ip,
                      args.mac_pcap_net.client_port,
                      args.mac_pcap_net.bind_port);
    mac.start_pcap_net(&mac_pcap_net);
  }

  if (args.s1ap_pcap.enable) {
    s1ap_pcap.open(args.s1ap_pcap.filename.c_str());
    s1ap.start_pcap(&s1ap_pcap);
  }

  // add sync queue
  sync_task_queue = task_sched.make_task_queue(args.sync_queue_size);

  // Init all layers
  if (!mac.init(args.mac, rrc_cfg.cell_list, phy, &rlc, &rrc)) {
    stack_logger.error("Couldn't initialize MAC");
    return SRSRAN_ERROR;
  }
  rlc.init(&pdcp, &rrc, &mac, task_sched.get_timer_handler());
  pdcp.init(&rlc, &rrc, &gtpu);
  if (rrc.init(rrc_cfg, phy, &mac, &rlc, &pdcp, &s1ap, &gtpu) != SRSRAN_SUCCESS) {
    stack_logger.error("Couldn't initialize RRC");
    return SRSRAN_ERROR;
  }
  if (s1ap.init(args.s1ap, &rrc) != SRSRAN_SUCCESS) {
    stack_logger.error("Couldn't initialize S1AP");
    return SRSRAN_ERROR;
  }
  if (gtpu.init(args.s1ap.gtp_bind_addr,
                args.s1ap.mme_addr,
                args.embms.m1u_multiaddr,
                args.embms.m1u_if_addr,
                &pdcp,
                args.embms.enable)) {
    stack_logger.error("Couldn't initialize GTPU");
    return SRSRAN_ERROR;
  }

  started = true;
  start(STACK_MAIN_THREAD_PRIO);

  return SRSRAN_SUCCESS;
}

void enb_stack_lte::tti_clock()
{
  sync_task_queue.push([this]() { tti_clock_impl(); });
}

void enb_stack_lte::tti_clock_impl()
{
  trace_complete_event("enb_stack_lte::tti_clock_impl", "total_time");
  task_sched.tic();
  rrc.tti_clock();
}

void enb_stack_lte::stop()
{
  if (started) {
    enb_task_queue.push([this]() { stop_impl(); });
    wait_thread_finish();
  }
}

void enb_stack_lte::stop_impl()
{
  rx_sockets.stop();

  s1ap.stop();
  gtpu.stop();
  mac.stop();
  rlc.stop();
  pdcp.stop();
  rrc.stop();

  if (args.mac_pcap.enable) {
    mac_pcap.close();
  }

  if (args.mac_pcap_net.enable) {
    mac_pcap_net.close();
  }

  if (args.s1ap_pcap.enable) {
    s1ap_pcap.close();
  }

  task_sched.stop();
  get_background_workers().stop();

  started = false;
}

bool enb_stack_lte::get_metrics(stack_metrics_t* metrics)
{
  // use stack thread to query metrics
  auto ret = enb_task_queue.try_push([this]() {
    stack_metrics_t metrics{};
    mac.get_metrics(metrics.mac);
    if (not metrics.mac.ues.empty()) {
      rlc.get_metrics(metrics.rlc, metrics.mac.ues[0].nof_tti);
      pdcp.get_metrics(metrics.pdcp, metrics.mac.ues[0].nof_tti);
    }
    rrc.get_metrics(metrics.rrc);
    s1ap.get_metrics(metrics.s1ap);
    if (not pending_stack_metrics.try_push(metrics)) {
      stack_logger.error("Unable to push metrics to queue");
    }
  });

  if (ret.first) {
    // wait for result
    *metrics = pending_stack_metrics.pop_blocking();
    return true;
  }
  return false;
}

void enb_stack_lte::run_thread()
{
  while (started) {
    task_sched.run_next_task();
  }
}

} // namespace srsenb
