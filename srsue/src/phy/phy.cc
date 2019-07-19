/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include <string>
#include <sstream>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>

#include "srslte/srslte.h"
#include "srslte/common/threads.h"
#include "srslte/common/log.h"
#include "srsue/hdr/phy/phy.h"

#define Error(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) if (SRSLTE_DEBUG_ENABLED) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    if (SRSLTE_DEBUG_ENABLED) log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->debug(fmt, ##__VA_ARGS__)

using namespace std;
using namespace asn1::rrc;

namespace srsue {


static void srslte_phy_handler(phy_logger_level_t log_level, void *ctx, char *str) {
  phy *r = (phy *) ctx;
  r->srslte_phy_logger(log_level, str);
}

void phy::srslte_phy_logger(phy_logger_level_t log_level, char *str) {
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

void phy::set_default_args(phy_args_t *args)
{
  args->nof_rx_ant          = 1;
  args->ul_pwr_ctrl_en      = false; 
  args->prach_gain          = -1;
  args->cqi_max             = -1; 
  args->cqi_fixed           = -1; 
  args->snr_ema_coeff       = 0.1; 
  args->snr_estim_alg       = "refs";
  args->pdsch_max_its       = 4; 
  args->nof_phy_threads     = DEFAULT_WORKERS;
  args->equalizer_mode      = "mmse"; 
  args->cfo_integer_enabled = false; 
  args->cfo_correct_tol_hz  = 50; 
  args->sss_algorithm       = "full";
  args->estimator_fil_auto  = false;
  args->estimator_fil_stddev = 1.0f;
  args->estimator_fil_order  = 4;
}

bool phy::check_args(const phy_args_t& args)
{
  if (args.nof_phy_threads > MAX_WORKERS) {
    log_h->console("Error in PHY args: nof_phy_threads must be 1, 2 or 3\n");
    return false; 
  }
  if (args.snr_ema_coeff > 1.0) {
    log_h->console("Error in PHY args: snr_ema_coeff must be 0<=w<=1\n");
    return false; 
  }
  return true; 
}

int phy::init(const phy_args_t&            args_,
              stack_interface_phy_lte*     stack_,
              srslte::radio_interface_phy* radio_)
{
  stack         = stack_;
  radio         = radio_;

  init(args_);

  return SRSLTE_SUCCESS;
}

int phy::init(const phy_args_t& args_)
{
  mlockall(MCL_CURRENT | MCL_FUTURE);

  args = args_;

  set_earfcn(args.earfcn_list);

  // Force frequency if given as argument
  if (args.dl_freq > 0 && args.ul_freq > 0) {
    sfsync.force_freq(args.dl_freq, args.ul_freq);
  }

  // Create array of pointers to phy_logs
  for (int i = 0; i < args.nof_phy_threads; i++) {
    auto*               mylog = new srslte::log_filter;
    char                tmp[16];
    sprintf(tmp, "PHY%d", i);
    mylog->init(tmp, logger, true);
    mylog->set_level(args.log.phy_level);
    mylog->set_hex_limit(args.log.phy_hex_limit);
    log_vec.push_back(std::move(std::unique_ptr<srslte::log_filter>(mylog)));
  }

  // Add PHY lib log
  if (log_vec.at(0)->get_level_from_string(args.log.phy_lib_level) != srslte::LOG_LEVEL_NONE) {
    auto*               lib_log = new srslte::log_filter;
    char                tmp[16];
    sprintf(tmp, "PHY_LIB");
    lib_log->init(tmp, logger, true);
    lib_log->set_level(args.log.phy_lib_level);
    lib_log->set_hex_limit(args.log.phy_hex_limit);
    log_vec.push_back(std::move(std::unique_ptr<srslte::log_filter>(lib_log)));
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

  initiated = false;
  start();
  return true;
}

// Initializes PHY in a thread
void phy::run_thread()
{
  prach_buffer.init(SRSLTE_MAX_PRB, log_h);
  common.init(&args, (srslte::log*)log_vec[0].get(), radio, stack);

  // Add workers to workers pool and start threads
  for (uint32_t i=0;i<nof_workers;i++) {
    auto w = std::unique_ptr<sf_worker>(new sf_worker(
        SRSLTE_MAX_PRB, &common, (srslte::log*)log_vec[i].get(), (srslte::log*)log_vec[nof_workers].get(), &sfsync));
    workers_pool.init_worker(i, w.get(), WORKERS_THREAD_PRIO, args.worker_cpu_mask);
    workers.push_back(std::move(w));
  }

  // Load Asynchronous SCell objects
  for (int i = 0; i < (int)args.nof_radios - 1; i++) {
    auto t = scell::async_recv_ptr(new scell::async_scell_recv());
    t->init(radio, &common, log_h);
    scell_sync.push_back(std::move(t));
  }

  // Warning this must be initialized after all workers have been added to the pool
  sfsync.init(radio,
              stack,
              &prach_buffer,
              &workers_pool,
              &common,
              log_h,
              log_phy_lib_h,
              &scell_sync,
              SF_RECV_THREAD_PRIO,
              args.sync_cpu_affinity);

  // Disable UL signal pregeneration until the attachment 
  enable_pregen_signals(false);

  initiated = true;
}

void phy::wait_initialize() {
  wait_thread_finish();
}

bool phy::is_initiated()
{
  return initiated;
}

void phy::stop()
{
  if (initiated) {
    sfsync.stop();
    for (uint32_t i = 0; i < args.nof_radios - 1; i++) {
      scell_sync.at(i)->stop();
    }

    workers_pool.stop();
    prach_buffer.stop();

    initiated = false;
  }
}

void phy::get_metrics(phy_metrics_t* m)
{
  common.get_dl_metrics(m->dl);
  common.get_ul_metrics(m->ul);
  common.get_sync_metrics(m->sync);
  m->nof_active_cc = args.nof_carriers;
}

void phy::set_timeadv_rar(uint32_t ta_cmd) {
  n_ta = srslte_N_ta_new_rar(ta_cmd);
  sfsync.set_time_adv_sec(((float)n_ta) * SRSLTE_LTE_TS);
  Info("PHY:   Set TA RAR: ta_cmd: %d, n_ta: %d, ta_usec: %.1f\n", ta_cmd, n_ta, ((float) n_ta)*SRSLTE_LTE_TS*1e6);
}

void phy::set_timeadv(uint32_t ta_cmd) {
  uint32_t new_nta = srslte_N_ta_new(n_ta, ta_cmd);
  sfsync.set_time_adv_sec(((float)new_nta) * SRSLTE_LTE_TS);
  Info("PHY:   Set TA: ta_cmd: %d, n_ta: %d, old_n_ta: %d, ta_usec: %.1f\n",
       ta_cmd,
       new_nta,
       n_ta,
       ((float)new_nta) * SRSLTE_LTE_TS * 1e6);
  n_ta = new_nta;
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

void phy::meas_reset() {
  sfsync.meas_reset();
}

int phy::meas_start(uint32_t earfcn, int pci) {
  return sfsync.meas_start(earfcn, pci);
}

int phy::meas_stop(uint32_t earfcn, int pci) {
  return sfsync.meas_stop(earfcn, pci);
}

bool phy::cell_select(phy_cell_t *cell) {
  return sfsync.cell_select(cell);
}

phy_interface_rrc_lte::cell_search_ret_t phy::cell_search(phy_cell_t* cell)
{
  return sfsync.cell_search(cell);
}

bool phy::cell_is_camping() {
  return sfsync.cell_is_camping();
}

float phy::get_phr()
{
  float phr = radio->get_max_tx_power() - common.cur_pusch_power;
  return phr; 
}

float phy::get_pathloss_db()
{
  return common.cur_pathloss;
}

void phy::get_current_cell(srslte_cell_t* cell, uint32_t* current_earfcn)
{
  sfsync.get_current_cell(cell, current_earfcn);
}

uint32_t phy::get_current_pci() {
  srslte_cell_t cell;
  sfsync.get_current_cell(&cell);
  return cell.id;
}

uint32_t phy::get_current_earfcn() {
  uint32_t earfcn;
  sfsync.get_current_cell(nullptr, &earfcn);
  return earfcn;
}

void phy::prach_send(uint32_t preamble_idx, int allowed_subframe, float target_power_dbm)
{
  sfsync.set_time_adv_sec(0.0f);
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
  n_ta = 0;
  sfsync.set_time_adv_sec(0);
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

void phy::set_earfcn(vector< uint32_t > earfcns)
{
  sfsync.set_earfcn(earfcns);
}

void phy::set_rar_grant(uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN], uint16_t rnti)
{
  common.set_rar_grant(grant_payload, rnti, tdd_config);
}

void phy::set_crnti(uint16_t rnti)
{
  for(uint32_t i=0;i<nof_workers;i++) {
    workers[i]->set_crnti(rnti);
  }    
}

// Start GUI
void phy::start_plot() {
  workers[0]->start_plot();
}

void phy::enable_pregen_signals(bool enable)
{
  for (uint32_t i = 0; i < nof_workers; i++) {
    workers[i]->enable_pregen_signals(enable);
  }
}

void phy::set_config(phy_interface_rrc_lte::phy_cfg_t* phy_cfg)
{
  if (is_initiated()) {
    for (uint32_t i = 0; i < nof_workers; i++) {
      workers[i]->set_pcell_config(phy_cfg);
    }
  }
  // Save PRACH configuration
  prach_cfg.config_idx     = phy_cfg->common.prach_cnfg.prach_cfg_info.prach_cfg_idx;
  prach_cfg.root_seq_idx   = phy_cfg->common.prach_cnfg.root_seq_idx;
  prach_cfg.zero_corr_zone = phy_cfg->common.prach_cnfg.prach_cfg_info.zero_correlation_zone_cfg;
  prach_cfg.freq_offset    = phy_cfg->common.prach_cnfg.prach_cfg_info.prach_freq_offset;
  prach_cfg.hs_flag        = phy_cfg->common.prach_cnfg.prach_cfg_info.high_speed_flag;
}

void phy::set_config_scell(asn1::rrc::scell_to_add_mod_r10_s* scell_config)
{
  // Enable CSI request extra bit
  common.multiple_csi_request_enabled = true;

  uint32_t cc_idx = scell_config->s_cell_idx_r10;

  // Component carrier index zero should be reserved for PCell
  if (cc_idx != 0 && cc_idx < args.nof_carriers) {
    carrier_map_t* m      = &args.carrier_map[cc_idx];
    srslte_cell_t  cell   = {};
    uint32_t       earfcn = 0;

    // Initialise default parameters from primary cell
    sfsync.get_current_cell(&cell, &earfcn);

    // Parse identification
    if (scell_config->cell_identif_r10_present) {
      cell.id = scell_config->cell_identif_r10.pci_r10;
      earfcn  = scell_config->cell_identif_r10.dl_carrier_freq_r10;
    }

    // Parse radio resource
    if (scell_config->rr_cfg_common_scell_r10_present) {
      rr_cfg_common_scell_r10_s* rr_cfg = &scell_config->rr_cfg_common_scell_r10;
      cell.frame_type                   = (rr_cfg->tdd_cfg_v1130_present) ? SRSLTE_TDD : SRSLTE_FDD;
      cell.nof_prb                      = rr_cfg->non_ul_cfg_r10.dl_bw_r10.to_number();
      cell.nof_ports                    = rr_cfg->non_ul_cfg_r10.ant_info_common_r10.ant_ports_count.to_number();
      cell.phich_length                 = (srslte_phich_length_t)rr_cfg->non_ul_cfg_r10.phich_cfg_r10.phich_dur.value;
      cell.phich_resources              = (srslte_phich_r_t)rr_cfg->non_ul_cfg_r10.phich_cfg_r10.phich_res.value;
    }

    // Send configuration to workers
    for (uint32_t i = 0; i < nof_workers; i++) {
      workers[i]->set_cell(cc_idx, cell);
      workers[i]->set_scell_config(cc_idx, scell_config);
    }

    // If SCell does not share synchronism with PCell ...
    if (m->radio_idx > 0) {
      scell_sync.at(m->radio_idx - 1)->set_scell_cell(cc_idx, &cell, earfcn);
    } else {
      // Change frequency only if the earfcn was modified
      if (common.scell_cfg[cc_idx].earfcn != earfcn) {
        float dl_freq = srslte_band_fd(earfcn) * 1e6f;
        float ul_freq = srslte_band_fu(srslte_band_ul_earfcn(earfcn)) * 1e6f;
        for (uint32_t p = 0; p < common.args->nof_rx_ant; p++) {
          radio->set_rx_freq(m->radio_idx, m->channel_idx + p, dl_freq);
          radio->set_tx_freq(m->radio_idx, m->channel_idx + p, ul_freq);
        }
      }
    }

    // Store SCell earfcn and pci
    common.scell_cfg[cc_idx].earfcn     = earfcn;
    common.scell_cfg[cc_idx].pci        = cell.id;
    common.scell_cfg[cc_idx].configured = true;
    common.scell_cfg[cc_idx].enabled    = false;
  } else {
    log_h->console("Received SCell configuration for index %d but there are not enough CC workers available\n",
                   scell_config->s_cell_idx_r10);
  }
}

void phy::set_config_tdd(tdd_cfg_s* tdd)
{
  tdd_config.sf_config  = tdd->sf_assign.to_number();
  tdd_config.ss_config  = tdd->special_sf_patterns.to_number();
  tdd_config.configured = true;

  if (!tdd_config.configured) {
    log_h->console("Setting TDD-config: %d, SS config: %d\n", tdd_config.sf_config, tdd_config.ss_config);
  }

  for (uint32_t i = 0; i < nof_workers; i++) {
    workers[i]->set_tdd_config(tdd_config);
  }
}

void phy::set_config_mbsfn_sib2(sib_type2_s* sib2)
{
  if (sib2->mbsfn_sf_cfg_list_present and sib2->mbsfn_sf_cfg_list.size() > 1) {
    Warning("SIB2 has %d MBSFN subframe configs - only 1 supported\n", sib2->mbsfn_sf_cfg_list.size());
  }
  if (sib2->mbsfn_sf_cfg_list_present and sib2->mbsfn_sf_cfg_list.size() > 0) {
    common.mbsfn_config.mbsfn_subfr_cnfg = sib2->mbsfn_sf_cfg_list[0];
    common.build_mch_table();
  }
}

void phy::set_config_mbsfn_sib13(sib_type13_r9_s* sib13)
{
  common.mbsfn_config.mbsfn_notification_cnfg = sib13->notif_cfg_r9;
  if (sib13->mbsfn_area_info_list_r9.size() > 1) {
    Warning("SIB13 has %d MBSFN area info elements - only 1 supported\n", sib13->mbsfn_area_info_list_r9.size());
  }
  if (sib13->mbsfn_area_info_list_r9.size() > 0) {
    common.mbsfn_config.mbsfn_area_info = sib13->mbsfn_area_info_list_r9[0];
    common.build_mcch_table();
  }
}

void phy::set_config_mbsfn_mcch(mcch_msg_s* mcch)
{
  common.mbsfn_config.mcch = *mcch;
  stack->set_mbsfn_config(
      common.mbsfn_config.mcch.msg.c1().mbsfn_area_cfg_r9().pmch_info_list_r9[0].mbms_session_info_list_r9.size());
  common.set_mch_period_stop(
      common.mbsfn_config.mcch.msg.c1().mbsfn_area_cfg_r9().pmch_info_list_r9[0].pmch_cfg_r9.sf_alloc_end_r9);
  common.set_mcch();
}

void phy::set_mch_period_stop(uint32_t stop)
{
  common.set_mch_period_stop(stop);
}

}
