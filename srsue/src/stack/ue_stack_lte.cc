/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsue/hdr/stack/ue_stack_lte.h"
#include "srslte/common/logmap.h"
#include "srslte/srslte.h"
#include <algorithm>
#include <chrono>
#include <numeric>
#include <thread>

using namespace srslte;

namespace srsue {

ue_stack_lte::ue_stack_lte(srslog::sink& log_sink) :
  running(false),
  args(),
  stack_logger(srslog::fetch_basic_logger("STCK", log_sink, false)),
  mac_logger(srslog::fetch_basic_logger("MAC", log_sink)),
  rlc_logger(srslog::fetch_basic_logger("RLC", log_sink, false)),
  pdcp_logger(srslog::fetch_basic_logger("PDCP", log_sink, false)),
  rrc_logger(srslog::fetch_basic_logger("RRC", log_sink, false)),
  usim_logger(srslog::fetch_basic_logger("USIM", log_sink, false)),
  nas_logger(srslog::fetch_basic_logger("NAS", log_sink, false)),
  usim(nullptr),
  phy(nullptr),
  rlc("RLC"),
  mac("MAC", &task_sched),
  rrc(this, &task_sched),
  mac_nr(&task_sched),
  rrc_nr(&task_sched),
  pdcp(&task_sched, "PDCP"),
  nas(&task_sched),
  thread("STACK"),
  task_sched(512, 64),
  tti_tprof("tti_tprof", "STCK", TTI_STAT_PERIOD),
  mac_pcap(srslte_rat_t::lte)
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
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

int ue_stack_lte::init(const stack_args_t& args_, phy_interface_stack_lte* phy_, gw_interface_stack* gw_)
{
  phy = phy_;
  gw  = gw_;

  if (init(args_)) {
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
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

  // Set up pcap
  if (args.pcap.enable) {
    if (mac_pcap.open(args.pcap.filename.c_str()) == SRSLTE_SUCCESS) {
      mac.start_pcap(&mac_pcap);
    }
  }
  if (args.pcap.nas_enable) {
    nas_pcap.open(args.pcap.nas_filename.c_str());
    nas.start_pcap(&nas_pcap);
  }

  // Init USIM first to allow early exit in case reader couldn't be found
  usim = usim_base::get_instance(&args.usim, usim_logger);
  if (usim->init(&args.usim)) {
    srslte::console("Failed to initialize USIM.\n");
    return SRSLTE_ERROR;
  }

  // add sync queue
  sync_task_queue = task_sched.make_task_queue(args.sync_queue_size);

  mac.init(phy, &rlc, &rrc);
  rlc.init(&pdcp, &rrc, task_sched.get_timer_handler(), 0 /* RB_ID_SRB0 */);
  pdcp.init(&rlc, &rrc, gw);
  nas.init(usim.get(), &rrc, gw, args.nas);

  mac_nr_args_t mac_nr_args = {};
  mac_nr.init(mac_nr_args, phy_nr, &rlc);
  rrc_nr.init(phy_nr, &mac_nr, &rlc, &pdcp, gw, &rrc, usim.get(), task_sched.get_timer_handler(), nullptr, args.rrc_nr);
  rrc.init(phy, &mac, &rlc, &pdcp, &nas, usim.get(), gw, &rrc_nr, args.rrc);

  running = true;
  start(STACK_MAIN_THREAD_PRIO);

  return SRSLTE_SUCCESS;
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
  rrc.stop();

  rlc.stop();
  pdcp.stop();
  mac.stop();

  if (args.pcap.enable) {
    mac_pcap.close();
  }
  if (args.pcap.nas_enable) {
    nas_pcap.close();
  }

  task_sched.stop();
  get_background_workers().stop();
}

bool ue_stack_lte::switch_on()
{
  if (running) {
    ue_task_queue.try_push([this]() { nas.switch_on(); });
  }
  return true;
}

bool ue_stack_lte::switch_off()
{
  // generate detach request with switch-off flag
  nas.switch_off();

  // wait for max. 5s for it to be sent (according to TS 24.301 Sec 25.5.2.2)
  int cnt = 0, timeout_ms = 5000;
  while (not rrc.srbs_flushed() && ++cnt <= timeout_ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  bool detach_sent = true;
  if (not rrc.srbs_flushed()) {
    srslog::fetch_basic_logger("NAS").warning("Detach couldn't be sent after %dms.", timeout_ms);
    detach_sent = false;
  }

  return detach_sent;
}

bool ue_stack_lte::enable_data()
{
  // perform attach request
  srslte::console("Turning off airplane mode.\n");
  return nas.enable_data();
}

bool ue_stack_lte::disable_data()
{
  // generate detach request
  srslte::console("Turning on airplane mode.\n");
  return nas.disable_data();
}

bool ue_stack_lte::start_service_request()
{
  if (running) {
    ue_task_queue.try_push([this]() { nas.start_service_request(srslte::establishment_cause_t::mo_data); });
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
    rlc.get_metrics(metrics.rlc, metrics.mac[0].nof_tti);
    nas.get_metrics(&metrics.nas);
    rrc.get_metrics(metrics.rrc);
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
 *   GW Interface
 *******************/

/**
 * Push GW SDU to stack
 * @param lcid
 * @param sdu
 * @param blocking
 */
void ue_stack_lte::write_sdu(uint32_t lcid, srslte::unique_byte_buffer_t sdu)
{
  auto task = [this, lcid](srslte::unique_byte_buffer_t& sdu) { pdcp.write_sdu(lcid, std::move(sdu)); };
  bool ret  = gw_queue_id.try_push(std::bind(task, std::move(sdu))).first;
  if (not ret) {
    pdcp_logger.info("GW SDU with lcid=%d was discarded.", lcid);
    ul_dropped_sdus++;
  }
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
  current_tti = tti_point{tti};

  // perform tasks for the received TTI range
  for (uint32_t i = 0; i < tti_jump; ++i) {
    uint32_t next_tti = TTI_SUB(tti, (tti_jump - i - 1));
    mac.run_tti(next_tti);
    task_sched.tic();
  }
  rrc.run_tti();
  nas.run_tti();

  if (args.have_tti_time_stats) {
    std::chrono::nanoseconds dur = tti_tprof.stop();
    if (dur > TTI_WARN_THRESHOLD_MS) {
      mac_logger.warning("%s: detected long duration=%" PRId64 "ms",
                         "proc_time",
                         std::chrono::duration_cast<std::chrono::milliseconds>(dur).count());
    }
  }

  // print warning if PHY pushes new TTI messages faster than we process them
  if (sync_task_queue.size() > SYNC_QUEUE_WARN_THRESHOLD) {
    stack_logger.warning("Detected slow task processing (sync_queue_len=%zd).", sync_task_queue.size());
  }
}

} // namespace srsue
