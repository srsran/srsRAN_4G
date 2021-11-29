/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsue/hdr/phy/phy_nr_sa.h"
#include "srsran/common/band_helper.h"
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
  workers(logger, 4),
  common(logger),
  prach_buffer(logger)
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

  if (!check_args(args)) {
    return false;
  }

  is_configured = false;
  std::thread t([this]() { init_background(); });
  init_thread = std::move(t);

  return true;
}

void phy_nr_sa::init_background()
{
  nr::sync_sa::args_t sync_args = {};
  if (not sync.init(sync_args, stack, radio)) {
    logger.error("Error initialising SYNC");
    return;
  }
  prach_buffer.init(SRSRAN_MAX_PRB);
  workers.init(args, sync, stack, WORKERS_THREAD_PRIO);

  is_configured = true;
}

void phy_nr_sa::stop()
{
  cmd_worker.stop();
  cmd_worker_cell.stop();
  if (is_configured) {
    sync.stop();
    workers.stop();
    prach_buffer.stop();
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

phy_interface_rrc_nr::phy_nr_state_t phy_nr_sa::get_state() const
{
  {
    switch (sync.get_state()) {
      case nr::sync_sa::STATE_IDLE:
        break;
      case nr::sync_sa::STATE_CELL_SEARCH:
        return phy_interface_rrc_nr::PHY_NR_STATE_CELL_SEARCH;
      case nr::sync_sa::STATE_CELL_SELECT:
        return phy_interface_rrc_nr::PHY_NR_STATE_CELL_SELECT;
    }
  }
  return phy_interface_rrc_nr::PHY_NR_STATE_IDLE;
}

void phy_nr_sa::reset_nr()
{
  sync.go_idle();
}

bool phy_nr_sa::start_cell_search(const cell_search_args_t& req)
{
  // Prepare cell search configuration from the request
  nr::cell_search::cfg_t cfg = {};
  cfg.srate_hz               = 0; // args.srate_hz;
  cfg.center_freq_hz         = req.center_freq_hz;
  cfg.ssb_freq_hz            = req.ssb_freq_hz;
  cfg.ssb_scs                = req.ssb_scs;
  cfg.ssb_pattern            = req.ssb_pattern;
  cfg.duplex_mode            = req.duplex_mode;

  // Request cell search to lower synchronization instance
  return sync.start_cell_search(cfg);
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
    srsran::srsran_band_helper band_helper;

    // tune radio
    for (uint32_t i = 0; i < common.args->nof_nr_carriers; i++) {
      logger.info("Tuning Rx channel %d to %.2f MHz",
                  i + common.args->nof_lte_carriers,
                  config_nr.carrier.dl_center_frequency_hz / 1e6);
      radio->set_rx_freq(i + common.args->nof_lte_carriers, config_nr.carrier.dl_center_frequency_hz);
      logger.info("Tuning Tx channel %d to %.2f MHz",
                  i + common.args->nof_lte_carriers,
                  config_nr.carrier.ul_center_frequency_hz / 1e6);
      radio->set_tx_freq(i + common.args->nof_lte_carriers, config_nr.carrier.ul_center_frequency_hz);
    }

    // Set UE configuration
    bool ret = workers.set_config(config_nr);

    // Notify PHY config completion
    if (stack != nullptr) {
      stack->set_phy_config_complete(ret);
    }

    return ret;
  });
  return true;
}

} // namespace srsue