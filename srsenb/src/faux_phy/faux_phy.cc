/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
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

#include "srslte/common/threads.h"
#include "srslte/common/log.h"

#include "faux_phy/faux_phy.h"
#include "faux_phy/faux_log.h"

using namespace std; 

namespace srsenb {

faux_phy::faux_phy() : workers_pool(MAX_WORKERS), 
                       workers(MAX_WORKERS), 
                       workers_common(txrx::MUTEX_X_WORKER*MAX_WORKERS)
{ }

void faux_phy::parse_config(phy_cfg_t* cfg)
{
  // PRACH configuration
  prach_cfg.config_idx     = cfg->prach_cnfg.prach_cnfg_info.prach_config_index;
  prach_cfg.hs_flag        = cfg->prach_cnfg.prach_cnfg_info.high_speed_flag;
  prach_cfg.root_seq_idx   = cfg->prach_cnfg.root_sequence_index;
  prach_cfg.zero_corr_zone = cfg->prach_cnfg.prach_cnfg_info.zero_correlation_zone_config;
  prach_cfg.freq_offset    = cfg->prach_cnfg.prach_cnfg_info.prach_freq_offset;
  
  // PUSCH DMRS configuration 
  workers_common.pusch_cfg.cyclic_shift        = cfg->pusch_cnfg.ul_rs.cyclic_shift;
  workers_common.pusch_cfg.delta_ss            = cfg->pusch_cnfg.ul_rs.group_assignment_pusch;
  workers_common.pusch_cfg.group_hopping_en    = cfg->pusch_cnfg.ul_rs.group_hopping_enabled;
  workers_common.pusch_cfg.sequence_hopping_en = cfg->pusch_cnfg.ul_rs.sequence_hopping_enabled;
  
  // PUSCH hopping configuration 
  workers_common.hopping_cfg.hop_mode       = cfg->pusch_cnfg.hopping_mode  == LIBLTE_RRC_HOPPING_MODE_INTRA_AND_INTER_SUBFRAME ? 
                                                srslte_pusch_hopping_cfg_t::SRSLTE_PUSCH_HOP_MODE_INTRA_SF : 
                                                srslte_pusch_hopping_cfg_t::SRSLTE_PUSCH_HOP_MODE_INTER_SF; ;
  workers_common.hopping_cfg.n_sb           = cfg->pusch_cnfg.n_sb; 
  workers_common.hopping_cfg.hopping_offset = cfg->pusch_cnfg.pusch_hopping_offset;
  
  // PUCCH configuration 
  workers_common.pucch_cfg.delta_pucch_shift  = liblte_rrc_delta_pucch_shift_num[cfg->pucch_cnfg.delta_pucch_shift%LIBLTE_RRC_DELTA_PUCCH_SHIFT_N_ITEMS];
  workers_common.pucch_cfg.N_cs               = cfg->pucch_cnfg.n_cs_an;
  workers_common.pucch_cfg.n_rb_2             = cfg->pucch_cnfg.n_rb_cqi;
  workers_common.pucch_cfg.srs_configured     = false;
  workers_common.pucch_cfg.n1_pucch_an        = cfg->pucch_cnfg.n1_pucch_an;; 
}

bool faux_phy::init(phy_args_t *_args, 
               phy_cfg_t *_cfg, 
               srslte::radio* _radio_handler_, 
               mac_interface_phy *_mac, 
               srslte::log* _log_h)
{
  std::vector<void*> log_vec;

  for (int i=0;i<_args->nof_phy_threads;i++) {
    log_vec.push_back((void*)_log_h);
  }

  return init(_args, _cfg, _radio_handler_, _mac, log_vec);
}

bool faux_phy::init(phy_args_t *_args, 
               phy_cfg_t *_cfg, 
               srslte::radio* _radio_handler_, 
               mac_interface_phy *_mac, 
               std::vector<void*> _log_vec)
{
  Debug("begin");

  mlockall(MCL_CURRENT | MCL_FUTURE);
  
  radio_handler = _radio_handler_;

  nof_workers = _args->nof_phy_threads; 
  
  workers_common.params = *_args; 

  workers_common.init(&_cfg->cell, radio_handler, _mac);
  
  parse_config(_cfg);
  
  // Add workers to workers pool and start threads
  for (uint32_t i=0;i<nof_workers;i++) {
    workers[i].init(&workers_common, (srslte::log*) _log_vec[i]);
    workers_pool.init_worker(i, &workers[i], WORKERS_THREAD_PRIO);    
  }
  
  prach.init(&_cfg->cell, &prach_cfg, _mac, (srslte::log*) _log_vec[0], PRACH_WORKER_THREAD_PRIO);

  prach.set_max_prach_offset_us(_args->max_prach_offset_us);
  
  // Warning this must be initialized after all workers have been added to the pool
  tx_rx.init(radio_handler, &workers_pool, &workers_common, &prach, (srslte::log*) _log_vec[0], SF_RECV_THREAD_PRIO);
    
  Debug("end_2");

  return true; 
}

void faux_phy::stop()
{  
  Debug("begin");

  tx_rx.stop();  

  workers_common.stop();

  for (uint32_t i=0;i<nof_workers;i++) {
    workers[i].stop();
  }

  workers_pool.stop();

  prach.stop();

  Debug("end");
}

uint32_t faux_phy::tti_to_SFN(uint32_t tti) 
{
  uint32_t sfn = tti/10;
  
  Debug("tti %u, sfn %u", tti, sfn);
 
  return sfn; 
}

uint32_t faux_phy::tti_to_subf(uint32_t tti) 
{
  uint32_t subf = tti%10;
  
  Debug("tti %u, subf %u", tti, subf);
 
  return subf; 
}

/***** MAC->PHY interface **********/
int faux_phy::add_rnti(uint16_t rnti)
{
  if (rnti >= SRSLTE_CRNTI_START && rnti <= SRSLTE_CRNTI_END) {
    workers_common.ack_add_rnti(rnti);
  }

  for (uint32_t i=0;i<nof_workers;i++) {
    if (workers[i].add_rnti(rnti)) {
      return SRSLTE_ERROR; 
    }
  }

  return SRSLTE_SUCCESS;
}

void faux_phy::rem_rnti(uint16_t rnti)
{
  if (rnti >= SRSLTE_CRNTI_START && rnti <= SRSLTE_CRNTI_END) {
    workers_common.ack_rem_rnti(rnti);
  }

  for (uint32_t i=0;i<nof_workers;i++) {
    workers[i].rem_rnti(rnti);
  }
}

void faux_phy::get_metrics(phy_metrics_t metrics[ENB_METRICS_MAX_USERS])
{
  phy_metrics_t metrics_tmp[ENB_METRICS_MAX_USERS];

  uint32_t nof_users = workers[0].get_nof_rnti(); 

  bzero(metrics, sizeof(phy_metrics_t)*ENB_METRICS_MAX_USERS);

  int n_tot = 0; 

  for (uint32_t i=0;i<nof_workers;i++) {
    workers[i].get_metrics(metrics_tmp);

    for (uint32_t j=0;j<nof_users;j++) {
      metrics[j].dl.n_samples   += metrics_tmp[j].dl.n_samples; 
      metrics[j].dl.mcs         += metrics_tmp[j].dl.n_samples*metrics_tmp[j].dl.mcs;
      
      metrics[j].ul.n_samples   += metrics_tmp[j].ul.n_samples; 
      metrics[j].ul.mcs         += metrics_tmp[j].ul.n_samples*metrics_tmp[j].ul.mcs;
      metrics[j].ul.n           += metrics_tmp[j].ul.n_samples*metrics_tmp[j].ul.n;
      metrics[j].ul.rssi        += metrics_tmp[j].ul.n_samples*metrics_tmp[j].ul.rssi;
      metrics[j].ul.sinr        += metrics_tmp[j].ul.n_samples*metrics_tmp[j].ul.sinr;
      metrics[j].ul.turbo_iters += metrics_tmp[j].ul.n_samples*metrics_tmp[j].ul.turbo_iters;
    }
  }

  for (uint32_t j=0;j<nof_users;j++) {
    metrics[j].dl.mcs         /= metrics[j].dl.n_samples;
    metrics[j].ul.mcs         /= metrics[j].ul.n_samples;
    metrics[j].ul.n           /= metrics[j].ul.n_samples;
    metrics[j].ul.rssi        /= metrics[j].ul.n_samples;
    metrics[j].ul.sinr        /= metrics[j].ul.n_samples;
    metrics[j].ul.turbo_iters /= metrics[j].ul.n_samples;
  }
}


/***** RRC->PHY interface **********/

void faux_phy::set_config_dedicated(uint16_t rnti, LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT* dedicated)
{
  // Parse RRC config 
  srslte_uci_cfg_t uci_cfg;
  srslte_pucch_sched_t pucch_sched;
  
  /* PUSCH UCI configuration */
  bzero(&uci_cfg, sizeof(srslte_uci_cfg_t));
  uci_cfg.I_offset_ack   = dedicated->pusch_cnfg_ded.beta_offset_ack_idx;
  uci_cfg.I_offset_cqi   = dedicated->pusch_cnfg_ded.beta_offset_cqi_idx;
  uci_cfg.I_offset_ri    = dedicated->pusch_cnfg_ded.beta_offset_ri_idx;
  
  /* PUCCH Scheduling configuration */
  bzero(&pucch_sched, sizeof(srslte_pucch_sched_t));
  pucch_sched.n_pucch_2  = dedicated->cqi_report_cnfg.report_periodic.pucch_resource_idx;
  pucch_sched.n_pucch_sr = dedicated->sched_request_cnfg.sr_pucch_resource_idx;
  
  for (uint32_t i=0;i<nof_workers;i++) {
    workers[i].set_config_dedicated(rnti, &uci_cfg, &pucch_sched, NULL, 
                                    dedicated->sched_request_cnfg.sr_cnfg_idx, 
                                    dedicated->cqi_report_cnfg.report_periodic_setup_present,
                                    dedicated->cqi_report_cnfg.report_periodic.pmi_cnfg_idx, 
                                    dedicated->cqi_report_cnfg.report_periodic.simult_ack_nack_and_cqi);
  }
}

// Start GUI 
void faux_phy::start_plot() {
  ((phch_worker) workers[0]).start_plot();
}

}
