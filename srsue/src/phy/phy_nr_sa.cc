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

#include "srsue/hdr/phy/phy_nr_sa.h"
#include "srsran/common/standard_streams.h"
#include "srsran/srsran.h"

namespace srsue {

static void srsran_phy_handler(phy_logger_level_t log_level, void* ctx, char* str)
{
  phy_nr_sa* r = (phy_nr_sa*)ctx;
  r->srsran_phy_logger(log_level, str);
}

void phy_nr_sa::srsran_phy_logger(phy_logger_level_t log_level, char* str)
{
  switch (log_level) {
    case LOG_LEVEL_INFO_S:
      logger_phy_lib.info(" %s", str);
      break;
    case LOG_LEVEL_DEBUG_S:
      logger_phy_lib.debug(" %s", str);
      break;
    case LOG_LEVEL_ERROR_S:
      logger_phy_lib.error(" %s", str);
      break;
    default:
      break;
  }
}

void phy_nr_sa::set_default_args(phy_args_nr_t& args_)
{
  args_.nof_phy_threads = DEFAULT_WORKERS;
  // TODO
}

bool phy_nr_sa::check_args(const phy_args_nr_t& args_)
{
  if (args_.nof_phy_threads > MAX_WORKERS) {
    srsran::console("Error in PHY args: nof_phy_threads must be 1, 2 or 3\n");
    return false;
  }
  return true;
}

phy_nr_sa::phy_nr_sa(const char* logname) :
  logger(srslog::fetch_basic_logger(logname)),
  logger_phy_lib(srslog::fetch_basic_logger("PHY_LIB")),
  sync(logger, workers),
  workers(logger, 4)
{}

int phy_nr_sa::init(const phy_args_nr_t& args_, stack_interface_phy_nr* stack_, srsran::radio_interface_phy* radio_)
{
  args  = args_;
  stack = stack_;
  radio = radio_;

  // Add PHY lib log
  auto lib_log_level = srslog::str_to_basic_level(args.log.phy_lib_level);
  logger_phy_lib.set_level(lib_log_level);
  logger_phy_lib.set_hex_dump_max_size(args.log.phy_hex_limit);
  if (lib_log_level != srslog::basic_levels::none) {
    srsran_phy_log_register_handler(this, srsran_phy_handler);
  }

  // set default logger
  logger.set_level(srslog::str_to_basic_level(args.log.phy_level));
  logger.set_hex_dump_max_size(args.log.phy_hex_limit);

  logger.set_context(0);

  if (!check_args(args)) {
    return SRSRAN_ERROR;
  }

  is_configured = false;
  std::thread t([this]() { init_background(); });
  init_thread = std::move(t);

  return SRSRAN_SUCCESS;
}

void phy_nr_sa::init_background()
{
  nr::sync_sa::args_t sync_args = {};
  sync_args.srate_hz            = args.srate_hz;
  sync_args.thread_priority     = args.slot_recv_thread_prio;
  if (not sync.init(sync_args, stack, radio)) {
    logger.error("Error initialising SYNC");
    return;
  }
  workers.init(args, sync, stack);

  is_configured = true;
}

void phy_nr_sa::stop()
{
  cmd_worker.stop();
  cmd_worker_cell.stop();
  if (is_configured) {
    sync.stop();
    workers.stop();
    is_configured = false;
  }
}

bool phy_nr_sa::is_initialized()
{
  return is_configured;
}

void phy_nr_sa::wait_initialize()
{
  init_thread.join();
}

phy_interface_rrc_nr::phy_nr_state_t phy_nr_sa::get_state()
{
  {
    switch (sync.get_state()) {
      case sync_state::state_t::IDLE:
        return phy_interface_rrc_nr::PHY_NR_STATE_IDLE;
      case sync_state::state_t::CELL_SEARCH:
        return phy_interface_rrc_nr::PHY_NR_STATE_CELL_SEARCH;
      case sync_state::state_t::SFN_SYNC:
        return phy_interface_rrc_nr::PHY_NR_STATE_CELL_SELECT;
      case sync_state::state_t::CAMPING:
        return phy_interface_rrc_nr::PHY_NR_STATE_CAMPING;
    }
  }
  return phy_interface_rrc_nr::PHY_NR_STATE_IDLE;
}

void phy_nr_sa::reset_nr()
{
  sync.reset();

  sync.cell_go_idle();
}

// This function executes one part of the procedure immediately and returns to continue in the background.
// When it returns, the caller thread can expect the PHY to have switched to IDLE and have stopped all DL/UL/PRACH
// processing.
// It will perform cell search procedure in the background and will signal stack with function cell_search_found_cell()
// when finished
bool phy_nr_sa::start_cell_search(const cell_search_args_t& req)
{
  // TODO: verify arguments are valid before starting procedure

  cmd_worker_cell.add_cmd([this, req]() {
    logger.info("Cell Search: Going to IDLE");
    sync.cell_go_idle();

    // Prepare cell search configuration from the request
    nr::cell_search::cfg_t cfg = {};
    cfg.srate_hz               = args.srate_hz;
    cfg.center_freq_hz         = req.center_freq_hz;
    cfg.ssb_freq_hz            = req.ssb_freq_hz;
    cfg.ssb_scs                = req.ssb_scs;
    cfg.ssb_pattern            = req.ssb_pattern;
    cfg.duplex_mode            = req.duplex_mode;

    // Request cell search to lower synchronization instance.
    nr::cell_search::ret_t ret = sync.cell_search_run(cfg);

    // Pass result to stack
    rrc_interface_phy_nr::cell_search_result_t rrc_cs_ret = {};
    rrc_cs_ret.cell_found                                 = ret.result == nr::cell_search::ret_t::CELL_FOUND;
    if (rrc_cs_ret.cell_found) {
      rrc_cs_ret.pci          = ret.ssb_res.N_id;
      rrc_cs_ret.pbch_msg     = ret.ssb_res.pbch_msg;
      rrc_cs_ret.measurements = ret.ssb_res.measurements;
    }
    stack->cell_search_found_cell(rrc_cs_ret);
  });

  return true;
}

// This function executes one part of the procedure immediately and returns to continue in the background.
// When it returns, the caller thread can expect the PHY to have switched to IDLE and have stopped all DL/UL/PRACH
// processing.
// It will perform cell search procedure in the background and will signal stack with function cell_search_found_cell()
// when finished
bool phy_nr_sa::start_cell_select(const cell_select_args_t& req)
{
  // TODO: verify arguments are valid before starting procedure

  logger.info("Cell Select: Going to IDLE");
  sync.cell_go_idle();

  selected_cell = req.carrier;

  cmd_worker_cell.add_cmd([this, req]() {
    // Request cell search to lower synchronization instance and push the result directly to the stack
    stack->cell_select_completed(sync.cell_select_run(req));
  });

  return true;
}

bool phy_nr_sa::has_valid_sr_resource(uint32_t sr_id)
{
  return workers.has_valid_sr_resource(sr_id);
}

void phy_nr_sa::clear_pending_grants()
{
  workers.clear_pending_grants();
}

void phy_nr_sa::send_prach(const uint32_t prach_occasion,
                           const int      preamble_index,
                           const float    preamble_received_target_power,
                           const float    ta_base_sec)
{
  workers.send_prach(prach_occasion, preamble_index, preamble_received_target_power);
}

void phy_nr_sa::set_timeadv_rar(uint32_t tti, uint32_t ta_cmd)
{
  sync.add_ta_cmd_rar(tti, ta_cmd);
}

void phy_nr_sa::set_timeadv(uint32_t tti, uint32_t ta_cmd)
{
  sync.add_ta_cmd_new(tti, ta_cmd);
}

int phy_nr_sa::set_rar_grant(uint32_t                                       rar_slot_idx,
                             std::array<uint8_t, SRSRAN_RAR_UL_GRANT_NBITS> packed_ul_grant,
                             uint16_t                                       rnti,
                             srsran_rnti_type_t                             rnti_type)
{
  return workers.set_rar_grant(rar_slot_idx, packed_ul_grant, rnti, rnti_type);
}

bool phy_nr_sa::set_config(const srsran::phy_cfg_nr_t& cfg)
{
  // Stash NR configuration
  config_nr = cfg;

  // Setup carrier configuration asynchronously
  cmd_worker.add_cmd([this]() {
    // Set UE configuration
    bool ret = workers.set_config(config_nr);

    // Pass n_ta_offset to sync
    sync.add_ta_offset(config_nr.t_offset);

    // Notify PHY config completion
    if (stack != nullptr) {
      stack->set_phy_config_complete(ret);
    }

    return ret;
  });
  return true;
}

} // namespace srsue
