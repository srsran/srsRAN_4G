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

#include <string>

#include "srsran/common/band_helper.h"
#include "srsran/common/standard_streams.h"
#include "srsran/srsran.h"
#include "srsue/hdr/phy/phy.h"

#define Error(fmt, ...)                                                                                                \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger_phy.error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...)                                                                                              \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger_phy.warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)                                                                                                 \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger_phy.info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)                                                                                                \
  if (SRSRAN_DEBUG_ENABLED)                                                                                            \
  logger_phy.debug(fmt, ##__VA_ARGS__)

using namespace std;

namespace srsue {

static void srsran_phy_handler(phy_logger_level_t log_level, void* ctx, char* str)
{
  phy* r = (phy*)ctx;
  r->srsran_phy_logger(log_level, str);
}

void phy::srsran_phy_logger(phy_logger_level_t log_level, char* str)
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

void phy::set_default_args(phy_args_t& args_)
{
  args_.nof_rx_ant           = 1;
  args_.ul_pwr_ctrl_en       = false;
  args_.prach_gain           = -1;
  args_.cqi_max              = -1;
  args_.cqi_fixed            = -1;
  args_.snr_ema_coeff        = 0.1;
  args_.snr_estim_alg        = "refs";
  args_.pdsch_max_its        = 4;
  args_.nof_phy_threads      = DEFAULT_WORKERS;
  args_.equalizer_mode       = "mmse";
  args_.cfo_integer_enabled  = false;
  args_.cfo_correct_tol_hz   = 50;
  args_.sss_algorithm        = "full";
  args_.estimator_fil_auto   = false;
  args_.estimator_fil_stddev = 1.0f;
  args_.estimator_fil_order  = 4;
}

bool phy::check_args(const phy_args_t& args_)
{
  if (args_.nof_phy_threads > MAX_WORKERS) {
    srsran::console("Error in PHY args: nof_phy_threads must be 1, 2 or 3\n");
    return false;
  }
  if (args_.snr_ema_coeff > 1.0) {
    srsran::console("Error in PHY args: snr_ema_coeff must be 0<=w<=1\n");
    return false;
  }
  return true;
}

int phy::init(const phy_args_t& args_, stack_interface_phy_lte* stack_, srsran::radio_interface_phy* radio_)
{
  std::unique_lock<std::mutex> lock(config_mutex);

  stack = stack_;
  radio = radio_;

  args = args_;

  // Force frequency if given as argument
  if (args.dl_freq > 0 && args.ul_freq > 0) {
    sfsync.force_freq(args.dl_freq, args.ul_freq);
  }

  // Add PHY lib log
  auto lib_log_level = srslog::str_to_basic_level(args.log.phy_lib_level);
  logger_phy_lib.set_level(lib_log_level);
  logger_phy_lib.set_hex_dump_max_size(args.log.phy_hex_limit);
  if (lib_log_level != srslog::basic_levels::none) {
    srsran_phy_log_register_handler(this, srsran_phy_handler);
  }

  // set default logger
  logger_phy.set_level(srslog::str_to_basic_level(args.log.phy_level));
  logger_phy.set_hex_dump_max_size(args.log.phy_hex_limit);

  if (!check_args(args)) {
    return SRSRAN_ERROR;
  }

  is_configured = false;
  start();
  return SRSRAN_SUCCESS;
}

// Initializes PHY in a thread
void phy::run_thread()
{
  std::unique_lock<std::mutex> lock(config_mutex);
  prach_buffer.init(SRSRAN_MAX_PRB);
  common.init(&args, radio, stack, &sfsync);

  // Initialise workers
  lte_workers.init(&common, WORKERS_THREAD_PRIO);

  // Warning this must be initialized after all workers have been added to the pool
  sfsync.init(
      radio, stack, &prach_buffer, &lte_workers, &nr_workers, &common, SF_RECV_THREAD_PRIO, args.sync_cpu_affinity);

  is_configured = true;
  config_cond.notify_all();
}

void phy::wait_initialize()
{
  // wait until PHY is configured
  std::unique_lock<std::mutex> lock(config_mutex);
  while (!is_configured) {
    config_cond.wait(lock);
  }
}

bool phy::is_initialized()
{
  return is_configured;
}

void phy::stop()
{
  std::unique_lock<std::mutex> lock(config_mutex);
  cmd_worker.stop();
  cmd_worker_cell.stop();
  if (is_configured) {
    sfsync.stop();
    lte_workers.stop();
    nr_workers.stop();
    prach_buffer.stop();
    wait_thread_finish();

    is_configured = false;
  }
}

void phy::get_metrics(const srsran::srsran_rat_t& rat, phy_metrics_t* m)
{
  // Zero structure by default
  *m = {};

  // Get LTE metrics
  if (rat == srsran::srsran_rat_t::lte && args.nof_lte_carriers > 0) {
    uint32_t      dl_earfcn = 0;
    srsran_cell_t cell      = {};
    sfsync.get_current_cell(&cell, &dl_earfcn);
    m->info[0].pci       = cell.id;
    m->info[0].dl_earfcn = dl_earfcn;

    for (uint32_t i = 1; i < args.nof_lte_carriers; i++) {
      m->info[i].dl_earfcn = common.cell_state.get_earfcn(i);
      m->info[i].pci       = common.cell_state.get_pci(i);
    }

    common.get_ch_metrics(m->ch);
    common.get_dl_metrics(m->dl);
    common.get_ul_metrics(m->ul);
    common.get_sync_metrics(m->sync);
    m->nof_active_cc = args.nof_lte_carriers;
    return;
  }

  // Get NR metrics
  if (rat == srsran::srsran_rat_t::nr && args.nof_nr_carriers > 0) {
    nr_workers.get_metrics(*m);
    return;
  }

  // Add other RAT here
  // ...
}

void phy::set_timeadv_rar(uint32_t tti, uint32_t ta_cmd)
{
  common.ta.add_ta_cmd_rar(tti, ta_cmd);
}

void phy::set_timeadv(uint32_t tti, uint32_t ta_cmd)
{
  common.ta.add_ta_cmd_new(tti, ta_cmd);
}

void phy::deactivate_scells()
{
  common.cell_state.deactivate_all();
}

void phy::set_activation_deactivation_scell(uint32_t cmd, uint32_t tti)
{
  common.cell_state.set_activation_deactivation(cmd, tti);
}

void phy::configure_prach_params()
{
  Debug("Configuring PRACH parameters");

  if (!prach_buffer.set_cell(selected_cell, prach_cfg)) {
    Error("Configuring PRACH parameters");
  }
}

void phy::set_cells_to_meas(uint32_t earfcn, const std::set<uint32_t>& pci)
{
  uint32_t pcell_earfcn = selected_earfcn;
  // As the SCell configuration is performed asynchronously through the cmd_worker, append the command adding the
  // measurements to avoid a concurrency issue
  cmd_worker.add_cmd([this, earfcn, pci, pcell_earfcn]() {
    // Check if the EARFCN matches with serving cell
    bool available = (pcell_earfcn == earfcn);

    // Find if there is secondary serving cell configured with the specified EARFCN
    uint32_t cc_empty = 0;
    for (uint32_t cc = 1; cc < args.nof_lte_carriers and not available; cc++) {
      // If it is configured...
      if (common.cell_state.is_configured(cc)) {
        // ... Check if the EARFCN match
        logger_phy.info(
            "Setting new SCell measurement cc=%d is configured and earfcn=%d", cc, common.cell_state.get_earfcn(cc));
        if (common.cell_state.get_earfcn(cc) == earfcn) {
          available = true;
        }
      } else {
        logger_phy.info("Setting new SCell measurement cc=%d is not configured", cc);
        if (cc_empty == 0) {
          // ... otherwise, save the CC as non-configured
          cc_empty = cc;
        }
      }
    }

    // If not available and a non-configured carrier is available, configure it.
    if (not available and cc_empty != 0) {
      // Copy all attributes from serving cell
      srsran_cell_t cell = selected_cell;

      // Select the first PCI in the list
      if (not pci.empty()) {
        cell.id = *pci.begin();
      }

      // Configure a the empty carrier as it was CA
      logger_phy.info("Setting new SCell measurement cc_idx=%d, earfcn=%d, pci=%d...", cc_empty, earfcn, cell.id);
      set_scell(cell, cc_empty, earfcn, false);
    }

    // Finally, set the serving cell measure
    sfsync.set_cells_to_meas(earfcn, pci);
  });
}

void phy::meas_stop()
{
  if (is_configured) {
    sfsync.meas_stop();
  }
}

// This function executes one part of the procedure immediatly and returns to continue in the background.
// When it returns, the caller thread can expect the PHY to have switched to IDLE and have stopped all DL/UL/PRACH
// processing.
bool phy::cell_select(phy_cell_t cell)
{
  sfsync.scell_sync_stop();
  if (sfsync.cell_select_init(cell)) {
    // Update PCI before starting the background command to make sure PRACH gets the updated value
    selected_cell.id = cell.pci;

    // Update EARCN before starting the background task to make sure is taken into account when finding carriers to
    // measure inter-frequency neighbours (see set_cells_to_meas)
    selected_earfcn = cell.earfcn;

    // Indicate workers that cell selection is in progress
    common.cell_is_selecting = true;

    // Update EARCN before starting the background task to make sure is taken into account when finding carriers to
    // measure inter-frequency neighbours (see set_cells_to_meas)
    selected_earfcn = cell.earfcn;

    cmd_worker_cell.add_cmd([this, cell]() {
      // Wait SYNC transitions to IDLE
      sfsync.wait_idle();

      // Reset worker once SYNC is IDLE to flush any PHY state including measurements, pending ACKs and pending grants
      reset();

      bool ret = sfsync.cell_select_start(cell);
      if (ret) {
        srsran_cell_t sync_cell;
        sfsync.get_current_cell(&sync_cell);
        selected_cell = sync_cell;
      }
      stack->cell_select_complete(ret);

      // Indicate workers that cell selection has finished
      common.cell_is_selecting = false;
    });
    return true;
  } else {
    logger_phy.warning("Could not start Cell Selection procedure");
    return false;
  }
}

// This function executes one part of the procedure immediatly and returns to continue in the background.
// When it returns, the caller thread can expect the PHY to have switched to IDLE and have stopped all DL/UL/PRACH
// processing. If a valid EARFCN (>0) is given, this is used for cell search.
bool phy::cell_search(int earfcn)
{
  sfsync.scell_sync_stop();
  if (sfsync.cell_search_init()) {
    cmd_worker_cell.add_cmd([this, earfcn]() {
      // Wait SYNC transitions to IDLE
      sfsync.wait_idle();

      // Reset worker once SYNC is IDLE to flush any PHY state including measurements, pending ACKs and pending grants
      reset();

      phy_cell_t                               found_cell = {};
      rrc_interface_phy_lte::cell_search_ret_t ret        = sfsync.cell_search_start(&found_cell, earfcn);
      stack->cell_search_complete(ret, found_cell);
    });
  } else {
    logger_phy.warning("Could not start Cell Search procedure");
  }
  return true;
}

bool phy::cell_is_camping()
{
  return sfsync.cell_is_camping();
}

float phy::get_phr()
{
  float phr = radio->get_info()->max_tx_gain - common.get_pusch_power();
  return phr;
}

float phy::get_pathloss_db()
{
  return common.get_pathloss();
}

void phy::prach_send(uint32_t preamble_idx, int allowed_subframe, float target_power_dbm, float ta_base_sec)
{
  common.ta.set_base_sec(ta_base_sec);
  common.reset_radio();
  if (!prach_buffer.prepare_to_send(preamble_idx, allowed_subframe, target_power_dbm)) {
    Error("Preparing PRACH to send");
  }
}

phy_interface_mac_lte::prach_info_t phy::prach_get_info()
{
  return prach_buffer.get_info();
}

// Handle the case of a radio overflow. Resynchronise immediatly
void phy::radio_overflow()
{
  sfsync.radio_overflow();
}

void phy::radio_failure()
{
  // TODO: handle failure
  Error("Radio failure.");
}

void phy::reset()
{
  Info("Resetting PHY...");
  common.ta.set_base_sec(0);
  common.reset();

  // Release mapping of secondary cells
  if (radio != nullptr) {
    for (uint32_t i = 1; i < args.nof_lte_carriers; i++) {
      radio->release_freq(i);
    }
  }
}

uint32_t phy::get_current_tti()
{
  return sfsync.get_current_tti();
}

void phy::sr_send()
{
  common.sr.trigger();
  Debug("SR is triggered");
}

int phy::sr_last_tx_tti()
{
  return common.sr.get_last_tx_tti();
}

void phy::set_rar_grant(uint8_t grant_payload[SRSRAN_RAR_GRANT_LEN], uint16_t rnti)
{
  common.set_rar_grant(grant_payload, rnti, tdd_config);
}

// Start GUI
void phy::start_plot()
{
  lte_workers[0]->start_plot();
  if (args.nof_nr_carriers > 0) {
    nr_workers[0]->start_plot();
  }
}

bool phy::set_config(const srsran::phy_cfg_t& config_, uint32_t cc_idx)
{
  if (!is_initialized()) {
    fprintf(stderr, "Error calling set_config(): PHY not initialized\n");
    return false;
  }

  // Check parameters are valid
  if (cc_idx >= args.nof_lte_carriers) {
    srsran::console("Received SCell configuration for index %d but there are not enough CC workers available\n",
                    cc_idx);
    return true;
  }

  Info("Setting configuration");

  // Apply configurations asynchronously to avoid race conditions
  cmd_worker.add_cmd([this, config_, cc_idx]() {
    // The PRACH configuration shall be updated only if:
    // - The new configuration belongs to the primary cell
    // - The PRACH configuration is present
    if (!cc_idx && config_.prach_cfg_present) {
      prach_cfg            = config_.prach_cfg;
      prach_cfg.tdd_config = tdd_config;
    }

    logger_phy.info("Setting new PHY configuration cc_idx=%d...", cc_idx);
    lte_workers.set_config(cc_idx, config_);

    // It is up to the PRACH component to detect whether the cell or the configuration have changed to reconfigure
    configure_prach_params();
    stack->set_config_complete(true);
  });
  return true;
}

bool phy::set_scell(srsran_cell_t cell_info, uint32_t cc_idx, uint32_t earfcn)
{
  return set_scell(cell_info, cc_idx, earfcn, true);
}

bool phy::set_scell(srsran_cell_t cell_info, uint32_t cc_idx, uint32_t earfcn, bool run_in_background)
{
  if (!is_initialized()) {
    fprintf(stderr, "Error calling set_config(): PHY not initialized\n");
    return false;
  }

  if (cc_idx == 0) {
    logger_phy.error("Received SCell configuration for invalid cc_idx=0");
    return false;
  }

  // Check parameters are valid
  if (cc_idx >= args.nof_lte_carriers) {
    srsran::console("Received SCell configuration for index %d but there are not enough CC workers available\n",
                    cc_idx);
    return false;
  }

  // First of all check validity of parameters
  if (!srsran_cell_isvalid(&cell_info)) {
    logger_phy.error("Received SCell configuration for an invalid cell");
    return false;
  }

  bool earfcn_is_different = common.cell_state.get_earfcn(cc_idx) != earfcn;

  // Set inter-frequency measurement
  sfsync.set_inter_frequency_measurement(cc_idx, earfcn, cell_info);

  // Reset secondary serving cell state, prevents this component carrier from executing any new PHY processing. It does
  // not stop any current work
  common.cell_state.reset(cc_idx);

  // Component carrier index zero should be reserved for PCell
  // Send configuration to workers
  if (run_in_background) {
    cmd_worker.add_cmd([this, cell_info, cc_idx, earfcn, earfcn_is_different]() {
      set_scell_cmd(cell_info, cc_idx, earfcn, earfcn_is_different);
    });
  } else {
    set_scell_cmd(cell_info, cc_idx, earfcn, earfcn_is_different);
  }
  return true;
}

void phy::set_scell_cmd(srsran_cell_t cell_info, uint32_t cc_idx, uint32_t earfcn, bool earfcn_is_different)
{
  logger_phy.info("Setting new SCell configuration cc_idx=%d, earfcn=%d, pci=%d...", cc_idx, earfcn, cell_info.id);
  for (uint32_t i = 0; i < args.nof_phy_threads; i++) {
    // set_cell is not protected so run when worker has finished to ensure no PHY processing is done at the time of
    // cell setting
    lte::sf_worker* w = lte_workers.wait_worker_id(i);
    if (w) {
      // Reset secondary serving cell configuration, this needs to be done when the sf_worker is reserved to prevent
      // resetting the cell while it is working
      w->reset_cell_nolock(cc_idx);

      // Set the new cell
      w->set_cell_nolock(cc_idx, cell_info);

      // Release the new worker, it should not start processing until the SCell state is set to configured
      w->release();
    }
  }

  // Reset measurements for the given CC after all workers finished processing and have been configured to ensure the
  // measurements are not overwritten
  common.reset_measurements(cc_idx);

  // Change frequency only if the earfcn was modified
  if (earfcn_is_different) {
    double dl_freq = srsran_band_fd(earfcn) * 1e6;
    double ul_freq = srsran_band_fu(common.get_ul_earfcn(earfcn)) * 1e6;
    radio->set_rx_freq(cc_idx, dl_freq);
    radio->set_tx_freq(cc_idx, ul_freq);
  }

  // Set secondary serving cell synchronization
  sfsync.scell_sync_set(cc_idx, cell_info);

  logger_phy.info(
      "Finished setting new SCell configuration cc_idx=%d, earfcn=%d, pci=%d", cc_idx, earfcn, cell_info.id);

  // Configure secondary serving cell, allows this component carrier to execute PHY processing
  common.cell_state.configure(cc_idx, earfcn, cell_info.id);

  stack->set_scell_complete(true);
}

void phy::set_config_tdd(srsran_tdd_config_t& tdd_config_)
{
  tdd_config = tdd_config_;

  if (!tdd_config.configured) {
    srsran::console("Setting TDD-config: %d, SS config: %d\n", tdd_config.sf_config, tdd_config.ss_config);
  }
  tdd_config.configured = true;

  // Apply config when worker is finished
  cmd_worker.add_cmd([this]() {
    for (uint32_t i = 0; i < args.nof_phy_threads; i++) {
      // set_tdd_config is not protected so run when worker is finished
      lte::sf_worker* w = lte_workers.wait_worker_id(i);
      if (w) {
        w->set_tdd_config_nolock(tdd_config);
        w->release();
      }
    }
  });
}

void phy::set_config_mbsfn_sib2(srsran::mbsfn_sf_cfg_t* cfg_list, uint32_t nof_cfgs)
{
  if (nof_cfgs > 1) {
    Warning("SIB2 has %d MBSFN subframe configs - only 1 supported", nof_cfgs);
  }
  if (nof_cfgs > 0) {
    common.mbsfn_config.mbsfn_subfr_cnfg = cfg_list[0];
    common.build_mch_table();
  }
}

void phy::set_config_mbsfn_sib13(const srsran::sib13_t& sib13)
{
  common.mbsfn_config.mbsfn_notification_cnfg = sib13.notif_cfg;
  if (sib13.nof_mbsfn_area_info > 1) {
    Warning("SIB13 has %d MBSFN area info elements - only 1 supported", sib13.nof_mbsfn_area_info);
  }
  if (sib13.nof_mbsfn_area_info > 0) {
    common.mbsfn_config.mbsfn_area_info = sib13.mbsfn_area_info_list[0];
    common.build_mcch_table();
  }
}

void phy::set_config_mbsfn_mcch(const srsran::mcch_msg_t& mcch)
{
  common.mbsfn_config.mcch = mcch;
  stack->set_mbsfn_config(common.mbsfn_config.mcch.pmch_info_list[0].nof_mbms_session_info);
  common.set_mch_period_stop(common.mbsfn_config.mcch.pmch_info_list[0].sf_alloc_end);
  common.set_mcch();
}

void phy::set_mch_period_stop(uint32_t stop)
{
  common.set_mch_period_stop(stop);
}

int phy::init(const phy_args_nr_t& args_, stack_interface_phy_nr* stack_, srsran::radio_interface_phy* radio_)
{
  stack_nr = stack_;
  if (!nr_workers.init(args_, common, stack_)) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int phy::set_rar_grant(uint32_t                                       rar_slot_idx,
                       std::array<uint8_t, SRSRAN_RAR_UL_GRANT_NBITS> packed_ul_grant,
                       uint16_t                                       rnti,
                       srsran_rnti_type_t                             rnti_type)
{
  return nr_workers.set_rar_grant(rar_slot_idx, packed_ul_grant, rnti, rnti_type);
}

void phy::send_prach(const uint32_t prach_occasion,
                     const int      preamble_index,
                     const float    preamble_received_target_power,
                     const float    ta_base_sec)
{
  nr_workers.send_prach(prach_occasion, preamble_index, preamble_received_target_power);
}

void phy::set_earfcn(std::vector<uint32_t> earfcns)
{
  // Do nothing
}

bool phy::set_config(const srsran::phy_cfg_nr_t& cfg)
{
  // Stash NR configuration
  config_nr = cfg;

  // Setup carrier configuration asynchronously
  cmd_worker.add_cmd([this]() {
    srsran::srsran_band_helper band_helper;

    // tune radio
    for (uint32_t i = 0; i < common.args->nof_nr_carriers; i++) {
      logger_phy.info("Tuning Rx channel %d to %.2f MHz",
                      i + common.args->nof_lte_carriers,
                      config_nr.carrier.dl_center_frequency_hz / 1e6);
      radio->set_rx_freq(i + common.args->nof_lte_carriers, config_nr.carrier.dl_center_frequency_hz);
      logger_phy.info("Tuning Tx channel %d to %.2f MHz",
                      i + common.args->nof_lte_carriers,
                      config_nr.carrier.ul_center_frequency_hz / 1e6);
      radio->set_tx_freq(i + common.args->nof_lte_carriers, config_nr.carrier.ul_center_frequency_hz);
    }

    // Set UE configuration
    bool ret = nr_workers.set_config(config_nr);

    // Notify PHY config completion
    if (stack_nr != nullptr) {
      stack_nr->set_phy_config_complete(ret);
    }

    return ret;
  });
  return true;
}

bool phy::has_valid_sr_resource(uint32_t sr_id)
{
  return nr_workers.has_valid_sr_resource(sr_id);
}

void phy::clear_pending_grants()
{
  nr_workers.clear_pending_grants();
}

} // namespace srsue
