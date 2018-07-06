/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
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


namespace srsue {

phy::phy() : workers_pool(MAX_WORKERS), 
             workers(MAX_WORKERS), 
             workers_common(phch_recv::MUTEX_X_WORKER*MAX_WORKERS),nof_coworkers(0)
{
}

static void srslte_phy_handler(phy_logger_level_t log_level, void *ctx, char *str) {
  phy *r = (phy *) ctx;
  r->srslte_phy_logger(log_level, str);
}

void phy::srslte_phy_logger(phy_logger_level_t log_level, char *str) {
  if (log_phy_lib_h) {
    switch(log_level){
      case LOG_LEVEL_INFO:
        log_phy_lib_h->info(" %s", str);
        break;
      case LOG_LEVEL_DEBUG:
        log_phy_lib_h->debug(" %s", str);
        break;
      case LOG_LEVEL_ERROR:
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
  args->attach_enable_64qam = false; 
  args->nof_phy_threads     = DEFAULT_WORKERS;
  args->equalizer_mode      = "mmse"; 
  args->cfo_integer_enabled = false; 
  args->cfo_correct_tol_hz  = 50; 
  args->sss_algorithm       = "full";
  args->estimator_fil_auto  = false;
  args->estimator_fil_stddev = 1.0f;
  args->estimator_fil_order  = 4;
}

bool phy::check_args(phy_args_t *args) 
{
  if (args->nof_phy_threads > MAX_WORKERS * 2) {
    log_h->console("Error in PHY args: nof_phy_threads must be 1, 2 or 3\n");
    return false; 
  }
  if (args->snr_ema_coeff > 1.0) {
    log_h->console("Error in PHY args: snr_ema_coeff must be 0<=w<=1\n");
    return false; 
  }
  return true; 
}

bool phy::init(srslte::radio_multi* radio_handler, mac_interface_phy *mac, rrc_interface_phy *rrc,
               std::vector<srslte::log_filter*> log_vec, phy_args_t *phy_args) {

  mlockall(MCL_CURRENT | MCL_FUTURE);

  n_ta = 0;
  this->log_vec       = log_vec;
  this->log_h         = (srslte::log*) log_vec[0];
  this->radio_handler = radio_handler;
  this->mac           = mac;
  this->rrc           = rrc;
 
  if (!phy_args) {
    args = &default_args;
    set_default_args(args);
  } else {
    args = phy_args;
  }

  if (!check_args(args)) {
    return false;
  }

  nof_workers = args->nof_phy_threads;
  if (nof_workers > MAX_WORKERS) {
    nof_coworkers = SRSLTE_MIN(nof_workers - MAX_WORKERS, MAX_WORKERS);
    nof_workers = MAX_WORKERS;
  }
  if (log_vec[nof_workers]) {
    this->log_phy_lib_h = (srslte::log*) log_vec[nof_workers];
    srslte_phy_log_register_handler(this, srslte_phy_handler);
  } else {
    this->log_phy_lib_h = NULL;
  }

  initiated = false;
  start();
  return true;
}

// Initializes PHY in a thread
void phy::run_thread() {

  prach_buffer.init(&config.common.prach_cnfg, SRSLTE_MAX_PRB, args, log_h);
  workers_common.init(&config, args, (srslte::log*) log_vec[0], radio_handler, rrc, mac);

  // Add workers to workers pool and start threads
  for (uint32_t i=0;i<nof_workers;i++) {
    workers[i].set_common(&workers_common);
    workers[i].init(SRSLTE_MAX_PRB, (srslte::log*) log_vec[i], (srslte::log*) log_vec[nof_workers], &sf_recv);
    workers_pool.init_worker(i, &workers[i], WORKERS_THREAD_PRIO, args->worker_cpu_mask);
  }

  for (uint32_t i=0;i<nof_coworkers;i++) {
    workers[i].enable_pdsch_coworker();
  }

  // Warning this must be initialized after all workers have been added to the pool
  sf_recv.init(radio_handler, mac, rrc, &prach_buffer, &workers_pool, &workers_common, log_h, log_phy_lib_h, args->nof_rx_ant, SF_RECV_THREAD_PRIO, args->sync_cpu_affinity);

  // Disable UL signal pregeneration until the attachment 
  enable_pregen_signals(false);

  initiated = true;
}

void phy::wait_initialize() {
  wait_thread_finish();
}

bool phy::is_initiated() {
  return initiated;
}

void phy::set_agc_enable(bool enabled)
{
  sf_recv.set_agc_enable(enabled);
}

void phy::start_trace()
{
  for (uint32_t i=0;i<nof_workers;i++) {
    workers[i].start_trace();
  }
}

void phy::write_trace(std::string filename)
{
  for (uint32_t i=0;i<nof_workers;i++) {
    string i_str = static_cast<ostringstream*>( &(ostringstream() << i) )->str();
    workers[i].write_trace(filename + "_" + i_str);
  }
}

void phy::stop()
{  
  sf_recv.stop();
  workers_pool.stop();
}

void phy::get_metrics(phy_metrics_t &m) {
  workers_common.get_dl_metrics(m.dl);
  workers_common.get_ul_metrics(m.ul);
  workers_common.get_sync_metrics(m.sync);
  int dl_tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(m.dl.mcs), workers_common.get_nof_prb());
  int ul_tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(m.ul.mcs), workers_common.get_nof_prb());
  m.dl.mabr_mbps = dl_tbs/1000.0; // TBS is bits/ms - convert to mbps
  m.ul.mabr_mbps = ul_tbs/1000.0; // TBS is bits/ms - convert to mbps
  Info("PHY:   MABR estimates. DL: %4.6f Mbps. UL: %4.6f Mbps.\n", m.dl.mabr_mbps, m.ul.mabr_mbps);
}

void phy::set_timeadv_rar(uint32_t ta_cmd) {
  n_ta = srslte_N_ta_new_rar(ta_cmd);
  sf_recv.set_time_adv_sec(((float) n_ta)*SRSLTE_LTE_TS);
  Info("PHY:   Set TA RAR: ta_cmd: %d, n_ta: %d, ta_usec: %.1f\n", ta_cmd, n_ta, ((float) n_ta)*SRSLTE_LTE_TS*1e6);
}

void phy::set_timeadv(uint32_t ta_cmd) {
  uint32_t new_nta = srslte_N_ta_new(n_ta, ta_cmd);
  sf_recv.set_time_adv_sec(((float) new_nta)*SRSLTE_LTE_TS);
  Info("PHY:   Set TA: ta_cmd: %d, n_ta: %d, old_n_ta: %d, ta_usec: %.1f\n", ta_cmd, new_nta, n_ta, ((float) new_nta)*SRSLTE_LTE_TS*1e6);
  n_ta = new_nta;
}

void phy::configure_prach_params()
{
  Debug("Configuring PRACH parameters\n");
  srslte_cell_t cell;
  sf_recv.get_current_cell(&cell);
  if (!prach_buffer.set_cell(cell)) {
    Error("Configuring PRACH parameters\n");
  }
}

void phy::configure_ul_params(bool pregen_disabled)
{
  Info("PHY:   Configuring UL parameters\n");
  if (is_initiated()) {
    for (uint32_t i=0;i<nof_workers;i++) {
      workers[i].set_ul_params(pregen_disabled);
    }
  }
}

void phy::meas_reset() {
  sf_recv.meas_reset();
}

int phy::meas_start(uint32_t earfcn, int pci) {
  return sf_recv.meas_start(earfcn, pci);
}

int phy::meas_stop(uint32_t earfcn, int pci) {
  return sf_recv.meas_stop(earfcn, pci);
}

bool phy::cell_select(phy_cell_t *cell) {
  return sf_recv.cell_select(cell);
}

phy_interface_rrc::cell_search_ret_t  phy::cell_search(phy_cell_t *cell) {
  return sf_recv.cell_search(cell);
}

bool phy::cell_is_camping() {
  return sf_recv.cell_is_camping();
}

float phy::get_phr()
{
  float phr = radio_handler->get_max_tx_power() - workers_common.cur_pusch_power; 
  return phr; 
}

float phy::get_pathloss_db()
{
  return workers_common.cur_pathloss;
}

void phy::pdcch_ul_search(srslte_rnti_type_t rnti_type, uint16_t rnti, int tti_start, int tti_end)
{
  workers_common.set_ul_rnti(rnti_type, rnti, tti_start, tti_end);
}

void phy::pdcch_dl_search(srslte_rnti_type_t rnti_type, uint16_t rnti, int tti_start, int tti_end)
{
  workers_common.set_dl_rnti(rnti_type, rnti, tti_start, tti_end);
}

void phy::pdcch_dl_search_reset()
{
  workers_common.set_dl_rnti(SRSLTE_RNTI_USER, 0);
}

void phy::pdcch_ul_search_reset()
{
  workers_common.set_ul_rnti(SRSLTE_RNTI_USER, 0);
}

void phy::get_current_cell(srslte_cell_t *cell, uint32_t *current_earfcn)
{
  sf_recv.get_current_cell(cell, current_earfcn);
}

uint32_t phy::get_current_pci() {
  srslte_cell_t cell;
  sf_recv.get_current_cell(&cell);
  return cell.id;
}

uint32_t phy::get_current_earfcn() {
  uint32_t earfcn;
  sf_recv.get_current_cell(NULL, &earfcn);
  return earfcn;
}

void phy::prach_send(uint32_t preamble_idx, int allowed_subframe, float target_power_dbm)
{
  
  if (!prach_buffer.prepare_to_send(preamble_idx, allowed_subframe, target_power_dbm)) {
    Error("Preparing PRACH to send\n");
  }
}

int phy::prach_tx_tti()
{
  return prach_buffer.tx_tti();
}

// Handle the case of a radio overflow. Resynchronise inmediatly
void phy::radio_overflow() {
  sf_recv.radio_overflow();
}

void phy::reset()
{
  Info("Resetting PHY\n");
  n_ta = 0;
  sf_recv.set_time_adv_sec(0);
  pdcch_dl_search_reset();
  for(uint32_t i=0;i<nof_workers;i++) {
    workers[i].reset();
  }
  workers_common.reset();
}

uint32_t phy::get_current_tti()
{
  return sf_recv.get_current_tti();
}


void phy::sr_send()
{
  workers_common.sr_enabled = true;
  workers_common.sr_last_tx_tti = -1;
}

int phy::sr_last_tx_tti()
{
  return workers_common.sr_last_tx_tti;
}

void phy::set_earfcn(vector< uint32_t > earfcns)
{
  sf_recv.set_earfcn(earfcns);
}

void phy::force_freq(float dl_freq, float ul_freq)
{
  sf_recv.force_freq(dl_freq, ul_freq);
}

void phy::set_rar_grant(uint32_t tti, uint8_t grant_payload[SRSLTE_RAR_GRANT_LEN])
{
  workers_common.set_rar_grant(tti, grant_payload);
}

void phy::set_crnti(uint16_t rnti) {
  for(uint32_t i=0;i<nof_workers;i++) {
    workers[i].set_crnti(rnti);
  }    
}

// Start GUI 
void phy::start_plot() {
  workers[0].start_plot();
}

void phy::enable_pregen_signals(bool enable)
{  
  for(uint32_t i=0;i<nof_workers;i++) {
    workers[i].enable_pregen_signals(enable);
  }
}

uint32_t phy::tti_to_SFN(uint32_t tti) {
  return tti/10; 
}

uint32_t phy::tti_to_subf(uint32_t tti) {
  return tti%10; 
}


void phy::get_config(phy_interface_rrc::phy_cfg_t* phy_cfg)
{
  memcpy(phy_cfg, &config, sizeof(phy_cfg_t));
}

void phy::set_config(phy_interface_rrc::phy_cfg_t* phy_cfg)
{
  memcpy(&config, phy_cfg, sizeof(phy_cfg_t));
}

void phy::set_config_64qam_en(bool enable)
{
  config.enable_64qam = enable; 
}

void phy::set_config_common(phy_interface_rrc::phy_cfg_common_t* common)
{
  memcpy(&config.common, common, sizeof(phy_cfg_common_t));
}

void phy::set_config_dedicated(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT* dedicated)
{
  memcpy(&config.dedicated, dedicated, sizeof(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT));
}

void phy::set_config_tdd(LIBLTE_RRC_TDD_CONFIG_STRUCT* tdd)
{
  memcpy(&config.common.tdd_cnfg, tdd, sizeof(LIBLTE_RRC_TDD_CONFIG_STRUCT));
}

void phy::set_config_mbsfn_sib2(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT *sib2)
{
  if(sib2->mbsfn_subfr_cnfg_list_size > 1) {
    Warning("SIB2 has %d MBSFN subframe configs - only 1 supported\n", sib2->mbsfn_subfr_cnfg_list_size);
  }
  if(sib2->mbsfn_subfr_cnfg_list_size > 0) {
    memcpy(&config.mbsfn.mbsfn_subfr_cnfg, &sib2->mbsfn_subfr_cnfg_list[0], sizeof(LIBLTE_RRC_MBSFN_SUBFRAME_CONFIG_STRUCT));
    workers_common.build_mch_table();
  }
}

void phy::set_config_mbsfn_sib13(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_13_STRUCT *sib13)
{
  memcpy(&config.mbsfn.mbsfn_notification_cnfg, &sib13->mbsfn_notification_config, sizeof(LIBLTE_RRC_MBSFN_NOTIFICATION_CONFIG_STRUCT));
  if(sib13->mbsfn_area_info_list_r9_size > 1) {
    Warning("SIB13 has %d MBSFN area info elements - only 1 supported\n", sib13->mbsfn_area_info_list_r9_size);
  }
  if(sib13->mbsfn_area_info_list_r9_size > 0) {
    memcpy(&config.mbsfn.mbsfn_area_info, &sib13->mbsfn_area_info_list_r9[0], sizeof(LIBLTE_RRC_MBSFN_AREA_INFO_STRUCT));
    workers_common.build_mcch_table();
  }
}

void phy::set_config_mbsfn_mcch(LIBLTE_RRC_MCCH_MSG_STRUCT *mcch)
{
  memcpy(&config.mbsfn.mcch, mcch, sizeof(LIBLTE_RRC_MCCH_MSG_STRUCT));
  mac->set_mbsfn_config(config.mbsfn.mcch.pmch_infolist_r9[0].mbms_sessioninfolist_r9_size);
  workers_common.set_mch_period_stop(config.mbsfn.mcch.pmch_infolist_r9[0].pmch_config_r9.sf_alloc_end_r9);
  workers_common.set_mcch(); 
}

void phy::set_mch_period_stop(uint32_t stop)
{
  workers_common.set_mch_period_stop(stop);
}

}
