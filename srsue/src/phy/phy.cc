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

#include <pthread.h>
#include <sstream>
#include <string.h>
#include <string>
#include <strings.h>
#include <sys/mman.h>
#include <unistd.h>

#include "srslte/common/log.h"
#include "srslte/common/threads.h"
#include "srslte/srslte.h"
#include "srsue/hdr/phy/phy.h"

#define Error(fmt, ...)                                                                                                \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...)                                                                                              \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)                                                                                                 \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)                                                                                                \
  if (SRSLTE_DEBUG_ENABLED)                                                                                            \
  log_h->debug(fmt, ##__VA_ARGS__)

using namespace std;

namespace srsue {

static void srslte_phy_handler(phy_logger_level_t log_level, void* ctx, char* str)
{
  phy* r = (phy*)ctx;
  r->srslte_phy_logger(log_level, str);
}

void phy::srslte_phy_logger(phy_logger_level_t log_level, char* str)
{
  if (log_phy_lib_h) {
    switch (log_level) {
      case LOG_LEVEL_INFO_S:
        log_phy_lib_h->info(" %s", str);
        break;
      case LOG_LEVEL_DEBUG_S:
        log_phy_lib_h->debug(" %s", str);
        break;
      case LOG_LEVEL_ERROR_S:
        log_phy_lib_h->error(" %s", str);
        break;
      default:
        break;
    }
  } else {
    printf("[PHY_LIB]: %s\n", str);
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
    log_h->console("Error in PHY args: nof_phy_threads must be 1, 2 or 3\n");
    return false;
  }
  if (args_.snr_ema_coeff > 1.0) {
    log_h->console("Error in PHY args: snr_ema_coeff must be 0<=w<=1\n");
    return false;
  }
  return true;
}

int phy::init(const phy_args_t& args_, stack_interface_phy_lte* stack_, srslte::radio_interface_phy* radio_)
{
  stack = stack_;
  radio = radio_;

  init(args_);

  return SRSLTE_SUCCESS;
}

int phy::init(const phy_args_t& args_)
{
  std::unique_lock<std::mutex> lock(config_mutex);
  mlockall(MCL_CURRENT | MCL_FUTURE);

  args = args_;

  // Force frequency if given as argument
  if (args.dl_freq > 0 && args.ul_freq > 0) {
    sfsync.force_freq(args.dl_freq, args.ul_freq);
  }

  // Create array of pointers to phy_logs
  for (int i = 0; i < args.nof_phy_threads; i++) {
    auto* mylog = new srslte::log_filter;
    char  tmp[16];
    sprintf(tmp, "PHY%d", i);
    mylog->init(tmp, logger, true);
    mylog->set_level(args.log.phy_level);
    mylog->set_hex_limit(args.log.phy_hex_limit);
    log_vec.push_back(std::unique_ptr<srslte::log_filter>(mylog));
  }

  // Add PHY lib log
  if (log_vec.at(0)->get_level_from_string(args.log.phy_lib_level) != srslte::LOG_LEVEL_NONE) {
    auto* lib_log = new srslte::log_filter;
    char  tmp[16];
    sprintf(tmp, "PHY_LIB");
    lib_log->init(tmp, logger, true);
    lib_log->set_level(args.log.phy_lib_level);
    lib_log->set_hex_limit(args.log.phy_hex_limit);
    log_vec.push_back(std::unique_ptr<srslte::log_filter>(lib_log));
  } else {
    log_vec.push_back(nullptr);
  }

  // set default logger
  log_h = log_vec.at(0).get();

  if (!check_args(args)) {
    return false;
  }

  nof_workers = args.nof_phy_threads;
  if (log_vec[nof_workers]) {
    this->log_phy_lib_h = (srslte::log*)log_vec[0].get();
    srslte_phy_log_register_handler(this, srslte_phy_handler);
  } else {
    this->log_phy_lib_h = nullptr;
  }

  is_configured = false;
  start();
  return true;
}

// Initializes PHY in a thread
void phy::run_thread()
{
  std::unique_lock<std::mutex> lock(config_mutex);
  prach_buffer.init(SRSLTE_MAX_PRB, log_h);
  common.init(&args, (srslte::log*)log_vec[0].get(), radio, stack);

  // Add workers to workers pool and start threads
  for (uint32_t i = 0; i < nof_workers; i++) {
    auto w = std::unique_ptr<sf_worker>(new sf_worker(
        SRSLTE_MAX_PRB, &common, (srslte::log*)log_vec[i].get(), (srslte::log*)log_vec[nof_workers].get(), &sfsync));
    workers_pool.init_worker(i, w.get(), WORKERS_THREAD_PRIO, args.worker_cpu_mask);
    workers.push_back(std::move(w));
  }

  // Warning this must be initialized after all workers have been added to the pool
  sfsync.init(radio,
              stack,
              &prach_buffer,
              &workers_pool,
              &common,
              log_h,
              log_phy_lib_h,
              SF_RECV_THREAD_PRIO,
              args.sync_cpu_affinity);

  // Disable UL signal pregeneration until the attachment
  enable_pregen_signals(false);

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

bool phy::is_initiated()
{
  return is_configured;
}

void phy::stop()
{
  std::unique_lock<std::mutex> lock(config_mutex);
  if (is_configured) {
    sfsync.stop();
    workers_pool.stop();
    prach_buffer.stop();

    is_configured = false;
  }
}

void phy::get_metrics(phy_metrics_t* m)
{
  uint32_t      dl_earfcn = 0;
  srslte_cell_t cell      = {};
  sfsync.get_current_cell(&cell, &dl_earfcn);
  m->info[0].pci       = cell.id;
  m->info[0].dl_earfcn = dl_earfcn;

  for (uint32_t i = 1; i < args.nof_carriers; i++) {
    m->info[i].dl_earfcn = common.scell_cfg[i].earfcn;
    m->info[i].pci       = common.scell_cfg[i].pci;
  }

  common.get_dl_metrics(m->dl);
  common.get_ul_metrics(m->ul);
  common.get_sync_metrics(m->sync);
  m->nof_active_cc = args.nof_carriers;
}

void phy::set_timeadv_rar(uint32_t ta_cmd)
{
  common.ta.add_ta_cmd_rar(ta_cmd);
}

void phy::set_timeadv(uint32_t ta_cmd)
{
  common.ta.add_ta_cmd_new(ta_cmd);
}

void phy::set_activation_deactivation_scell(uint32_t cmd)
{
  Info("Received SCell Activation / Deactivation command: 0x%x\n", cmd);

  /* Implements 3GPP 36.321 section 6.1.3.8. Activation/Deactivation MAC Control Element*/
  log_h->console("SCELL Activation / Deactivation CMD: %x\n", cmd);

  for (uint32_t i = 1; i < SRSLTE_MAX_CARRIERS; i++) {
    bool activated = ((cmd >> i) & 0x1) == 0x1;

    /* Enable actual cell */
    common.enable_scell(i, activated);
  }
}

void phy::configure_prach_params()
{
  Debug("Configuring PRACH parameters\n");
  srslte_cell_t cell;
  sfsync.get_current_cell(&cell);

  prach_cfg.tdd_config = tdd_config;

  if (!prach_buffer.set_cell(cell, prach_cfg)) {
    Error("Configuring PRACH parameters\n");
  }
}

void phy::set_cells_to_meas(uint32_t earfcn, const std::set<uint32_t>& pci)
{
  sfsync.set_cells_to_meas(earfcn, pci);
}

void phy::meas_stop()
{
  sfsync.meas_stop();
}

bool phy::cell_select(const phy_cell_t* cell)
{
  return sfsync.cell_select(cell);
}

phy_interface_rrc_lte::cell_search_ret_t phy::cell_search(phy_cell_t* cell)
{
  return sfsync.cell_search(cell);
}

bool phy::cell_is_camping()
{
  return sfsync.cell_is_camping();
}

float phy::get_phr()
{
  float phr = radio->get_info()->max_tx_gain - common.cur_pusch_power;
  return phr;
}

float phy::get_pathloss_db()
{
  return common.cur_pathloss;
}

void phy::prach_send(uint32_t preamble_idx, int allowed_subframe, float target_power_dbm, float ta_base_sec)
{
  common.ta.set_base_sec(ta_base_sec);
  common.reset_radio();
  if (!prach_buffer.prepare_to_send(preamble_idx, allowed_subframe, target_power_dbm)) {
    Error("Preparing PRACH to send\n");
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
  Error("Radio failure.\n");
}

void phy::reset()
{
  Info("Resetting PHY\n");
  common.ta.set_base_sec(0);
  for (uint32_t i = 0; i < nof_workers; i++) {
    workers[i]->reset();
  }
  common.reset();
}

uint32_t phy::get_current_tti()
{
  return sfsync.get_current_tti();
}

void phy::sr_send()
{
  common.sr_enabled     = true;
  common.sr_last_tx_tti = -1;
  Debug("sr_send(): sr_enabled=%d, last_tx_tti=%d\n", common.sr_enabled, common.sr_last_tx_tti);
}

int phy::sr_last_tx_tti()
{
  return common.sr_last_tx_tti;
}

void phy::set_rar_grant(uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN], uint16_t rnti)
{
  common.set_rar_grant(grant_payload, rnti, tdd_config);
}

void phy::set_crnti(uint16_t rnti)
{
  // set_crnti() is an operation that takes time, apply asynrhonously with processing
  for (uint32_t i = 0; i < nof_workers; i++) {
    sf_worker* w = (sf_worker*)workers_pool.wait_worker_id(i);
    w->set_crnti(rnti);
    w->release();
  }
}

// Start GUI
void phy::start_plot()
{
  workers[0]->start_plot();
}

void phy::enable_pregen_signals(bool enable)
{
  for (uint32_t i = 0; i < nof_workers; i++) {
    workers[i]->enable_pregen_signals(enable);
  }
}

void phy::set_config(srslte::phy_cfg_t& config_, uint32_t cc_idx, uint32_t earfcn, srslte_cell_t* cell_info)
{
  if (!is_initiated()) {
    fprintf(stderr, "Error calling set_config(): PHY not initialized\n");
    return;
  }

  // Disable cell_info if configuration has not been set
  if (cell_info) {
    if (!srslte_cell_isvalid(cell_info)) {
      cell_info = nullptr;
    }
  }

  // Component carrier index zero should be reserved for PCell
  if (cc_idx < args.nof_carriers) {
    // Send configuration to workers
    for (uint32_t i = 0; i < nof_workers; i++) {
      if (cell_info) {
        // set_cell() is an operation that takes time, apply asynrhonously with processing
        sf_worker* w = (sf_worker*)workers_pool.wait_worker_id(i);
        w->set_cell(cc_idx, *cell_info);
        w->release();
      }
      // set_config() is just a memcpy
      workers[i]->set_config(cc_idx, config_);
    }

    // Set inter-frequency measurement primary cell
    if (cell_info) {
      sfsync.set_inter_frequency_measurement(cc_idx, earfcn, *cell_info);
    }

    if (cc_idx == 0) {
      prach_cfg = config_.prach_cfg;
    } else if (cell_info) {
      // Change frequency only if the earfcn was modified
      if (common.scell_cfg[cc_idx].earfcn != earfcn) {
        double dl_freq = srslte_band_fd(earfcn) * 1e6;
        double ul_freq = srslte_band_fu(common.get_ul_earfcn(earfcn)) * 1e6;
        radio->set_rx_freq(cc_idx, dl_freq);
        radio->set_tx_freq(cc_idx, ul_freq);
      }

      // Store SCell earfcn and pci
      common.scell_cfg[cc_idx].earfcn     = earfcn;
      common.scell_cfg[cc_idx].pci        = cell_info->id;
      common.scell_cfg[cc_idx].configured = true;
      common.scell_cfg[cc_idx].enabled    = false;
    } else {
      Error("Configuring Scell index %d but cell_info not provided\n", cc_idx);
    }

  } else {
    log_h->console("Received SCell configuration for index %d but there are not enough CC workers available\n", cc_idx);
  }
}

void phy::set_config_tdd(srslte_tdd_config_t& tdd_config_)
{
  tdd_config = tdd_config_;

  if (!tdd_config.configured) {
    log_h->console("Setting TDD-config: %d, SS config: %d\n", tdd_config.sf_config, tdd_config.ss_config);
  }
  tdd_config.configured = true;

  for (uint32_t i = 0; i < nof_workers; i++) {
    workers[i]->set_tdd_config(tdd_config);
  }
}

void phy::set_config_mbsfn_sib2(srslte::mbsfn_sf_cfg_t* cfg_list, uint32_t nof_cfgs)
{
  if (nof_cfgs > 1) {
    Warning("SIB2 has %d MBSFN subframe configs - only 1 supported\n", nof_cfgs);
  }
  if (nof_cfgs > 0) {
    common.mbsfn_config.mbsfn_subfr_cnfg = cfg_list[0];
    common.build_mch_table();
  }
}

void phy::set_config_mbsfn_sib13(const srslte::sib13_t& sib13)
{
  common.mbsfn_config.mbsfn_notification_cnfg = sib13.notif_cfg;
  if (sib13.nof_mbsfn_area_info > 1) {
    Warning("SIB13 has %d MBSFN area info elements - only 1 supported\n", sib13.nof_mbsfn_area_info);
  }
  if (sib13.nof_mbsfn_area_info > 0) {
    common.mbsfn_config.mbsfn_area_info = sib13.mbsfn_area_info_list[0];
    common.build_mcch_table();
  }
}

void phy::set_config_mbsfn_mcch(const srslte::mcch_msg_t& mcch)
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

} // namespace srsue
