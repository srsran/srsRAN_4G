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

#include "srsue/hdr/stack/ue_stack_lte.h"
#include "srsran/common/standard_streams.h"
#include "srsran/interfaces/ue_phy_interfaces.h"
#include "srsran/srslog/event_trace.h"

#include <algorithm>
#include <chrono>
#include <numeric>
#include <thread>

using namespace srsran;

namespace srsue {

ue_stack_lte::ue_stack_lte() :
  args(),
  stack_logger(srslog::fetch_basic_logger("STCK", false)),
  mac_logger(srslog::fetch_basic_logger("MAC")),
  rlc_logger(srslog::fetch_basic_logger("RLC", false)),
  pdcp_logger(srslog::fetch_basic_logger("PDCP", false)),
  rrc_logger(srslog::fetch_basic_logger("RRC", false)),
  usim_logger(srslog::fetch_basic_logger("USIM", false)),
  nas_logger(srslog::fetch_basic_logger("NAS", false)),
  nas5g_logger(srslog::fetch_basic_logger("NAS5G", false)),
  mac_nr_logger(srslog::fetch_basic_logger("MAC-NR")),
  rrc_nr_logger(srslog::fetch_basic_logger("RRC-NR", false)),
  rlc_nr_logger(srslog::fetch_basic_logger("RLC-NR", false)),
  pdcp_nr_logger(srslog::fetch_basic_logger("PDCP-NR", false)),
  mac_pcap(),
  mac_nr_pcap(),
  rlc("RLC"),
  mac("MAC", &task_sched),
  rrc(this, &task_sched),
  rlc_nr("RLC-NR"),
  mac_nr(&task_sched),
  rrc_nr(&task_sched),
  pdcp(&task_sched, "PDCP"),
  pdcp_nr(&task_sched, "PDCP-NR"),
  sdap("SDAP-NR"),
  sdap_pdcp(&pdcp_nr, &sdap),
  nas(srslog::fetch_basic_logger("NAS", false), &task_sched),
  nas_5g(srslog::fetch_basic_logger("NAS5G", false), &task_sched),
  thread("STACK"),
  task_sched(512, 64),
  tti_tprof("tti_tprof", "STCK", TTI_STAT_PERIOD)
{
  get_background_workers().set_nof_workers(2);
  ue_task_queue  = task_sched.make_task_queue();
  gw_queue_id    = task_sched.make_task_queue();
  cfg_task_queue = task_sched.make_task_queue();
  // sync_queue is added in init()
}

ue_stack_lte::~ue_stack_lte()
{
  stop();
}

std::string ue_stack_lte::get_type()
{
  return "lte";
}

int ue_stack_lte::init(const stack_args_t&      args_,
                       phy_interface_stack_lte* phy_,
                       phy_interface_stack_nr*  phy_nr_,
                       gw_interface_stack*      gw_)
{
  phy_nr = phy_nr_;
  if (init(args_, phy_, gw_)) {
    return SRSRAN_ERROR;
  }
  return SRSRAN_SUCCESS;
}

int ue_stack_lte::init(const stack_args_t& args_, phy_interface_stack_lte* phy_, gw_interface_stack* gw_)
{
  phy = phy_;
  gw  = gw_;

  if (init(args_)) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int ue_stack_lte::init(const stack_args_t& args_)
{
  args = args_;

  // init own log
  stack_logger.set_level(srslog::str_to_basic_level(args.log.stack_level));
  stack_logger.set_hex_dump_max_size(args.log.stack_hex_limit);
  byte_buffer_pool::get_instance()->enable_logger(true);

  // init layer logs
  mac_logger.set_level(srslog::str_to_basic_level(args.log.mac_level));
  mac_logger.set_hex_dump_max_size(args.log.mac_hex_limit);
  rlc_logger.set_level(srslog::str_to_basic_level(args.log.rlc_level));
  rlc_logger.set_hex_dump_max_size(args.log.rlc_hex_limit);
  pdcp_logger.set_level(srslog::str_to_basic_level(args.log.pdcp_level));
  pdcp_logger.set_hex_dump_max_size(args.log.pdcp_hex_limit);
  rrc_logger.set_level(srslog::str_to_basic_level(args.log.rrc_level));
  rrc_logger.set_hex_dump_max_size(args.log.rrc_hex_limit);
  usim_logger.set_level(srslog::str_to_basic_level(args.log.usim_level));
  usim_logger.set_hex_dump_max_size(args.log.usim_hex_limit);
  nas_logger.set_level(srslog::str_to_basic_level(args.log.nas_level));
  nas_logger.set_hex_dump_max_size(args.log.nas_hex_limit);

  nas5g_logger.set_level(srslog::str_to_basic_level(args.log.nas_level));
  nas5g_logger.set_hex_dump_max_size(args.log.nas_hex_limit);
  mac_nr_logger.set_level(srslog::str_to_basic_level(args.log.mac_level));
  mac_nr_logger.set_hex_dump_max_size(args.log.mac_hex_limit);
  rrc_nr_logger.set_level(srslog::str_to_basic_level(args.log.rrc_level));
  rrc_nr_logger.set_hex_dump_max_size(args.log.rrc_hex_limit);
  pdcp_nr_logger.set_level(srslog::str_to_basic_level(args.log.pdcp_level));
  pdcp_nr_logger.set_hex_dump_max_size(args.log.pdcp_hex_limit);
  rlc_nr_logger.set_level(srslog::str_to_basic_level(args.log.rlc_level));
  rlc_nr_logger.set_hex_dump_max_size(args.log.rlc_hex_limit);

  // Set up pcap
  // parse pcap trace list
  std::vector<std::string> pcap_list;
  srsran::string_parse_list(args.pkt_trace.enable, ',', pcap_list);
  if (pcap_list.empty()) {
    stack_logger.error("PCAP enable list empty defaulting to disable all PCAPs");
    args.pkt_trace.mac_pcap.enable    = false;
    args.pkt_trace.mac_nr_pcap.enable = false;
    args.pkt_trace.mac_nr_pcap.enable = false;
  }

  for (auto& pcap : pcap_list) {
    // Remove white spaces
    pcap.erase(std::remove_if(pcap.begin(), pcap.end(), isspace), pcap.end());
    if (pcap == "mac" || pcap == "MAC") {
      args.pkt_trace.mac_pcap.enable = true;
    } else if (pcap == "mac_nr" || pcap == "MAC_NR") {
      args.pkt_trace.mac_nr_pcap.enable = true;
    } else if (pcap == "nas" || pcap == "NAS") {
      args.pkt_trace.nas_pcap.enable = true;
    } else if (pcap == "none" || pcap == "NONE") {
      args.pkt_trace.mac_pcap.enable    = false;
      args.pkt_trace.mac_nr_pcap.enable = false;
      args.pkt_trace.mac_nr_pcap.enable = false;
    } else {
      stack_logger.error("Unknown PCAP option %s", pcap.c_str());
    }
  }

  // If mac and mac_nr pcap option is enabled and if the filenames are the same,
  // mac and mac_nr should write in the same PCAP file.
  if (args.pkt_trace.mac_pcap.enable && args.pkt_trace.mac_nr_pcap.enable &&
      args.pkt_trace.mac_pcap.filename == args.pkt_trace.mac_nr_pcap.filename) {
    stack_logger.info("Using same MAC PCAP file %s for LTE and NR", args.pkt_trace.mac_pcap.filename.c_str());
    if (mac_pcap.open(args.pkt_trace.mac_pcap.filename.c_str()) == SRSRAN_SUCCESS) {
      mac.start_pcap(&mac_pcap);
      mac_nr.start_pcap(&mac_pcap);
      stack_logger.info("Open mac pcap file %s", args.pkt_trace.mac_pcap.filename.c_str());
    } else {
      stack_logger.error("Can not open pcap file %s", args.pkt_trace.mac_pcap.filename.c_str());
    }
  } else {
    if (args.pkt_trace.mac_pcap.enable) {
      if (mac_pcap.open(args.pkt_trace.mac_pcap.filename.c_str()) == SRSRAN_SUCCESS) {
        mac.start_pcap(&mac_pcap);
        stack_logger.info("Open mac pcap file %s", args.pkt_trace.mac_pcap.filename.c_str());
      } else {
        stack_logger.error("Can not open pcap file %s", args.pkt_trace.mac_pcap.filename.c_str());
      }
    }

    if (args.pkt_trace.mac_nr_pcap.enable) {
      if (mac_nr_pcap.open(args.pkt_trace.mac_nr_pcap.filename.c_str()) == SRSRAN_SUCCESS) {
        mac_nr.start_pcap(&mac_nr_pcap);
        stack_logger.info("Open mac nr pcap file %s", args.pkt_trace.mac_nr_pcap.filename.c_str());
      } else {
        stack_logger.error("Can not open pcap file %s", args.pkt_trace.mac_nr_pcap.filename.c_str());
      }
    }
  }

  if (args.pkt_trace.nas_pcap.enable) {
    if (nas_pcap.open(args.pkt_trace.nas_pcap.filename.c_str()) == SRSRAN_SUCCESS) {
      nas.start_pcap(&nas_pcap);
      nas_5g.start_pcap(&nas_pcap);
      stack_logger.info("Open nas pcap file %s", args.pkt_trace.nas_pcap.filename.c_str());
    } else {
      stack_logger.error("Can not open pcap file %s", args.pkt_trace.nas_pcap.filename.c_str());
    }
  }

  // Init USIM first to allow early exit in case reader couldn't be found
  usim = usim_base::get_instance(&args.usim, usim_logger);
  if (usim->init(&args.usim)) {
    srsran::console("Failed to initialize USIM.\n");
    return SRSRAN_ERROR;
  }

  // add sync queue
  sync_task_queue = task_sched.make_task_queue(args.sync_queue_size);

  mac.init(phy, &rlc, &rrc);
  rlc.init(&pdcp, &rrc, task_sched.get_timer_handler(), 0 /* RB_ID_SRB0 */);
  nas.init(usim.get(), &rrc, gw, args.nas);

  if (!args.sa_mode) {
    pdcp.init(&rlc, &rrc, gw);
  } else {
    pdcp.init(&rlc, &rrc, &sdap_pdcp);
    sdap.init(&sdap_pdcp, gw);
  }

  mac_nr_args_t mac_nr_args = {};
  mac_nr.init(mac_nr_args, phy_nr, &rlc_nr, &rrc_nr);
  rlc_nr.init(&pdcp_nr, &rrc_nr, task_sched.get_timer_handler(), 0 /* RB_ID_SRB0 */);
  pdcp_nr.init(&rlc_nr, &rrc_nr, gw);
  rrc_nr.init(phy_nr,
              &mac_nr,
              &rlc_nr,
              &pdcp_nr,
              &sdap,
              gw,
              &nas_5g,
              args.sa_mode ? nullptr : &rrc,
              usim.get(),
              task_sched.get_timer_handler(),
              this,
              args.rrc_nr);
  rrc.init(phy, &mac, &rlc, &pdcp, &nas, usim.get(), gw, &rrc_nr, args.rrc);

  if (args.sa_mode) {
    nas_5g.init(usim.get(), &rrc_nr, gw, args.nas_5g);
  }

  running = true;
  start(STACK_MAIN_THREAD_PRIO);

  return SRSRAN_SUCCESS;
}

void ue_stack_lte::stop()
{
  if (running) {
    ue_task_queue.try_push([this]() { stop_impl(); });
    wait_thread_finish();
  }
}

void ue_stack_lte::stop_impl()
{
  running = false;

  usim->stop();
  nas.stop();
  nas_5g.stop();
  rrc.stop();

  rlc.stop();
  pdcp.stop();
  mac.stop();

  if (args.pkt_trace.mac_pcap.enable) {
    mac_pcap.close();
  }
  if (args.pkt_trace.mac_nr_pcap.enable) {
    mac_nr_pcap.close();
  }
  if (args.pkt_trace.nas_pcap.enable) {
    nas_pcap.close();
  }

  task_sched.stop();
  get_background_workers().stop();
}

bool ue_stack_lte::switch_on()
{
  if (running) {
    stack_logger.info("Triggering NAS switch on");
    if (!ue_task_queue.try_push([this]() {
          if (args.sa_mode) {
            nas_5g.switch_on();
          } else {
            nas.switch_on();
          }
        })) {
      stack_logger.error("Triggering NAS switch on: ue_task_queue is full\n");
    }
  } else {
    stack_logger.error("Triggering NAS switch on: stack is not running\n");
  }
  return true;
}

bool ue_stack_lte::switch_off()
{
  if (running) {
    ue_task_queue.try_push([this]() {
      // generate detach request with switch-off flag
      nas.switch_off();
    });
  }
  return true;
}

bool ue_stack_lte::enable_data()
{
  if (running) {
    ue_task_queue.try_push([this]() {
      // perform attach request
      srsran::console("Turning off airplane mode.\n");
      nas.enable_data();
    });
  }
  return true;
}

bool ue_stack_lte::disable_data()
{
  if (running) {
    ue_task_queue.try_push([this]() {
      // generate detach request
      srsran::console("Turning on airplane mode.\n");
      nas.disable_data();
    });
  }
  return true;
}

bool ue_stack_lte::start_service_request()
{
  if (running) {
    ue_task_queue.try_push([this]() {
      if (args.sa_mode) {
        nas_5g.start_service_request();
      } else {
        nas.start_service_request(srsran::establishment_cause_t::mo_data);
      }
    });
  }
  return true;
}

bool ue_stack_lte::get_metrics(stack_metrics_t* metrics)
{
  // use stack thread to query metrics
  ue_task_queue.try_push([this]() {
    stack_metrics_t metrics{};
    metrics.ul_dropped_sdus = ul_dropped_sdus;
    mac.get_metrics(metrics.mac);
    mac_nr.get_metrics(metrics.mac_nr);
    rlc.get_metrics(metrics.rlc, metrics.mac[0].nof_tti);
    nas.get_metrics(&metrics.nas);
    rrc.get_metrics(metrics.rrc);
    rrc_nr.get_metrics(metrics.rrc_nr);
    pending_stack_metrics.push(metrics);
  });
  // wait for result
  *metrics = pending_stack_metrics.wait_pop();
  return (metrics->nas.state == emm_state_t::state_t::registered && metrics->rrc.state == RRC_STATE_CONNECTED);
}

void ue_stack_lte::run_thread()
{
  while (running) {
    task_sched.run_next_task();
  }
}

/***********************************************************************************************************************
 *                                                Stack Interfaces
 **********************************************************************************************************************/

/********************
 *   RRC Interface
 *******************/

void ue_stack_lte::add_eps_bearer(uint8_t eps_bearer_id, srsran::srsran_rat_t rat, uint32_t lcid)
{
  bearers.add_eps_bearer(eps_bearer_id, rat, lcid);
}

void ue_stack_lte::remove_eps_bearer(uint8_t eps_bearer_id)
{
  bearers.remove_eps_bearer(eps_bearer_id);
}

/********************
 *   GW Interface
 *******************/

/**
 * GW calls write_sdu() to push SDU for EPS bearer to stack.
 * If the EPS bearer ID is valid it will deliver the PDU to the
 * registered PDCP entity.
 *
 * @param eps_bearer_id
 * @param sdu
 */
void ue_stack_lte::write_sdu(uint32_t eps_bearer_id, srsran::unique_byte_buffer_t sdu)
{
  auto bearer = bearers.get_radio_bearer(eps_bearer_id);

  auto task = [this, eps_bearer_id, bearer](srsran::unique_byte_buffer_t& sdu) {
    // route SDU to PDCP entity
    if (bearer.rat == srsran_rat_t::lte) {
      pdcp.write_sdu(bearer.lcid, std::move(sdu));
    } else if (bearer.rat == srsran_rat_t::nr) {
      if (args.sa_mode) {
        sdap.write_sdu(bearer.lcid, std::move(sdu));
      } else {
        pdcp_nr.write_sdu(bearer.lcid, std::move(sdu));
      }
    } else {
      stack_logger.warning("Can't deliver SDU for EPS bearer %d. Dropping it.", eps_bearer_id);
    }
  };

  bool ret = gw_queue_id.try_push(std::bind(task, std::move(sdu))).has_value();
  if (not ret) {
    pdcp_logger.info("GW SDU with lcid=%d was discarded.", bearer.lcid);
    ul_dropped_sdus++;
  }
}

bool ue_stack_lte::has_active_radio_bearer(uint32_t eps_bearer_id)
{
  return bearers.has_active_radio_bearer(eps_bearer_id);
}

void ue_stack_lte::reset_eps_bearers()
{
  bearers.reset();
}

/**
 * Check whether nas is attached
 * @return bool wether NAS is in EMM_REGISTERED
 */
bool ue_stack_lte::is_registered()
{
  return nas.is_registered();
}

/********************
 *  PHY Interface
 *******************/

void ue_stack_lte::cell_search_complete(cell_search_ret_t ret, phy_cell_t found_cell)
{
  cfg_task_queue.push([this, ret, found_cell]() { rrc.cell_search_complete(ret, found_cell); });
}

void ue_stack_lte::cell_select_complete(bool status)
{
  cfg_task_queue.push([this, status]() { rrc.cell_select_complete(status); });
}

void ue_stack_lte::set_config_complete(bool status)
{
  cfg_task_queue.push([this, status]() { rrc.set_config_complete(status); });
}

void ue_stack_lte::set_scell_complete(bool status)
{
  cfg_task_queue.push([this, status]() { rrc.set_scell_complete(status); });
}

/********************
 *  SYNC Interface
 *******************/

/**
 * Sync thread signal that it is in sync
 */
void ue_stack_lte::in_sync()
{
  sync_task_queue.push([this]() { rrc.in_sync(); });
}

void ue_stack_lte::out_of_sync()
{
  sync_task_queue.push([this]() { rrc.out_of_sync(); });
}

void ue_stack_lte::run_tti(uint32_t tti, uint32_t tti_jump)
{
  if (running) {
    sync_task_queue.push([this, tti, tti_jump]() { run_tti_impl(tti, tti_jump); });
  }
}

void ue_stack_lte::run_tti_impl(uint32_t tti, uint32_t tti_jump)
{
  if (args.have_tti_time_stats) {
    tti_tprof.start();
  }

  trace_complete_event("ue_stack_lte::run_tti_impl", "total time");

  current_tti = tti_point{tti};

  // perform tasks for the received TTI range
  for (uint32_t i = 0; i < tti_jump; ++i) {
    uint32_t next_tti = TTI_SUB(tti, (tti_jump - i - 1));
    mac.run_tti(next_tti);
    mac_nr.run_tti(next_tti);
    task_sched.tic();
  }
  rrc.run_tti();
  rrc_nr.run_tti(tti);
  nas.run_tti();
  nas_5g.run_tti();

  if (args.have_tti_time_stats) {
    std::chrono::nanoseconds dur = tti_tprof.stop();
    if (dur > TTI_WARN_THRESHOLD_MS) {
      mac_logger.info("%s: detected long duration=%" PRId64 "ms",
                      "proc_time",
                      std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());
    }
  }

  // print warning if PHY pushes new TTI messages faster than we process them
  if (sync_task_queue.size() > SYNC_QUEUE_WARN_THRESHOLD) {
    stack_logger.warning("Detected slow task processing (sync_queue_len=%zd).", sync_task_queue.size());
  }
}
void ue_stack_lte::set_phy_config_complete(bool status)
{
  cfg_task_queue.push([this, status]() { rrc_nr.set_phy_config_complete(status); });
}

void ue_stack_lte::cell_search_found_cell(const cell_search_result_t& result)
{
  cfg_task_queue.push([this, result]() { rrc_nr.cell_search_found_cell(result); });
}
void ue_stack_lte::cell_select_completed(const rrc_interface_phy_nr::cell_select_result_t& result)
{
  cfg_task_queue.push([this, result]() { rrc_nr.cell_select_completed(result); });
}

} // namespace srsue
