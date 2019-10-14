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

#include "srslte/common/threads.h"
#include "srslte/common/log.h"
#include "srsenb/hdr/phy/phy.h"

#define Error(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) if (SRSLTE_DEBUG_ENABLED) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    if (SRSLTE_DEBUG_ENABLED) log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->debug(fmt, ##__VA_ARGS__)

using namespace std;
using namespace asn1::rrc;

namespace srsenb {

phy::phy(srslte::logger* logger_) :
  logger(logger_),
  workers_pool(MAX_WORKERS),
  workers(MAX_WORKERS),
  workers_common(MAX_WORKERS),
  nof_workers(0)
{
}

phy::~phy()
{
  stop();
}

void phy::parse_config(const phy_cfg_t& cfg)
{
  // PRACH configuration
  ZERO_OBJECT(prach_cfg);
  prach_cfg.config_idx     = cfg.prach_cnfg.prach_cfg_info.prach_cfg_idx;
  prach_cfg.hs_flag        = cfg.prach_cnfg.prach_cfg_info.high_speed_flag;
  prach_cfg.root_seq_idx   = cfg.prach_cnfg.root_seq_idx;
  prach_cfg.zero_corr_zone = cfg.prach_cnfg.prach_cfg_info.zero_correlation_zone_cfg;
  prach_cfg.freq_offset    = cfg.prach_cnfg.prach_cfg_info.prach_freq_offset;

  // Uplink Physical common configuration
  ZERO_OBJECT(workers_common.ul_cfg_com);

  // DMRS
  workers_common.ul_cfg_com.dmrs.cyclic_shift        = cfg.pusch_cnfg.ul_ref_sigs_pusch.cyclic_shift;
  workers_common.ul_cfg_com.dmrs.delta_ss            = cfg.pusch_cnfg.ul_ref_sigs_pusch.group_assign_pusch;
  workers_common.ul_cfg_com.dmrs.group_hopping_en    = cfg.pusch_cnfg.ul_ref_sigs_pusch.group_hop_enabled;
  workers_common.ul_cfg_com.dmrs.sequence_hopping_en = cfg.pusch_cnfg.ul_ref_sigs_pusch.seq_hop_enabled;

  // Hopping
  workers_common.ul_cfg_com.hopping.hop_mode =
      cfg.pusch_cnfg.pusch_cfg_basic.hop_mode ==
              asn1::rrc::pusch_cfg_common_s::pusch_cfg_basic_s_::hop_mode_e_::intra_and_inter_sub_frame
          ? srslte_pusch_hopping_cfg_t::SRSLTE_PUSCH_HOP_MODE_INTRA_SF
          : srslte_pusch_hopping_cfg_t::SRSLTE_PUSCH_HOP_MODE_INTER_SF;
  ;
  workers_common.ul_cfg_com.hopping.n_sb             = cfg.pusch_cnfg.pusch_cfg_basic.n_sb;
  workers_common.ul_cfg_com.hopping.hopping_offset   = cfg.pusch_cnfg.pusch_cfg_basic.pusch_hop_offset;
  workers_common.ul_cfg_com.pusch.max_nof_iterations = workers_common.params.pusch_max_its;
  workers_common.ul_cfg_com.pusch.csi_enable         = false;
  workers_common.ul_cfg_com.pusch.meas_time_en       = true;

  // PUCCH
  workers_common.ul_cfg_com.pucch.delta_pucch_shift = cfg.pucch_cnfg.delta_pucch_shift.to_number();
  workers_common.ul_cfg_com.pucch.N_cs              = cfg.pucch_cnfg.n_cs_an;
  workers_common.ul_cfg_com.pucch.n_rb_2            = cfg.pucch_cnfg.n_rb_cqi;
  workers_common.ul_cfg_com.pucch.N_pucch_1         = cfg.pucch_cnfg.n1_pucch_an;
  workers_common.ul_cfg_com.pucch.threshold_format1 = 0.8;

  // PDSCH configuration
  ZERO_OBJECT(workers_common.dl_cfg_com);
  workers_common.dl_cfg_com.tm                 = SRSLTE_TM1;
  workers_common.dl_cfg_com.pdsch.rs_power     = cfg.pdsch_cnfg.ref_sig_pwr;
  workers_common.dl_cfg_com.pdsch.p_b          = cfg.pdsch_cnfg.p_b;
  workers_common.dl_cfg_com.pdsch.meas_time_en = true;
}

int phy::init(const phy_args_t&            args,
              const phy_cfg_t&             cfg,
              srslte::radio_interface_phy* radio_,
              stack_interface_phy_lte*     stack_)
{
  mlockall(MCL_CURRENT | MCL_FUTURE);

  // Create array of pointers to phy_logs
  for (int i = 0; i < args.nof_phy_threads; i++) {
    auto mylog   = std::unique_ptr<srslte::log_filter>(new srslte::log_filter);
    char tmp[16] = {};
    sprintf(tmp, "PHY%d", i);
    mylog->init(tmp, logger, true);
    mylog->set_level(args.log.phy_level);
    mylog->set_hex_limit(args.log.phy_hex_limit);
    log_vec.push_back(std::move(mylog));
  }

  // Add PHY lib log
  if (log_vec.at(0)->get_level_from_string(args.log.phy_lib_level) != srslte::LOG_LEVEL_NONE) {
    auto lib_log = std::unique_ptr<srslte::log_filter>(new srslte::log_filter);
    char tmp[16] = {};
    sprintf(tmp, "PHY_LIB");
    lib_log->init(tmp, logger, true);
    lib_log->set_level(args.log.phy_lib_level);
    lib_log->set_hex_limit(args.log.phy_hex_limit);
    log_vec.push_back(std::move(lib_log));
  } else {
    log_vec.push_back(nullptr);
  }

  radio       = radio_;
  nof_workers = args.nof_phy_threads;

  workers_common.params = args;

  workers_common.init(cfg.cell, radio, stack_);

  parse_config(cfg);
  
  // Add workers to workers pool and start threads
  for (uint32_t i=0;i<nof_workers;i++) {
    workers[i].init(&workers_common, log_vec.at(i).get());
    workers_pool.init_worker(i, &workers[i], WORKERS_THREAD_PRIO);    
  }

  prach.init(cfg.cell, prach_cfg, stack_, log_vec.at(0).get(), PRACH_WORKER_THREAD_PRIO);
  prach.set_max_prach_offset_us(args.max_prach_offset_us);

  // Warning this must be initialized after all workers have been added to the pool
  tx_rx.init(radio, &workers_pool, &workers_common, &prach, log_vec.at(0).get(), SF_RECV_THREAD_PRIO);

  initialized = true;

  return SRSLTE_SUCCESS;
}

void phy::stop()
{
  if (initialized) {
    tx_rx.stop();
    for (uint32_t i = 0; i < nof_workers; i++) {
      workers[i].stop();
    }
    workers_common.stop();
    workers_pool.stop();
    prach.stop();

    initialized = false;
  }
}

uint32_t phy::tti_to_SFN(uint32_t tti) {
  return tti/10; 
}

uint32_t phy::tti_to_subf(uint32_t tti) {
  return tti%10; 
}

/***** MAC->PHY interface **********/
int phy::add_rnti(uint16_t rnti, bool is_temporal)
{
  if (SRSLTE_RNTI_ISUSER(rnti)) {
    workers_common.ue_db_add_rnti(rnti);
  }

  for (uint32_t i=0;i<nof_workers;i++) {
    if (workers[i].add_rnti(rnti, is_temporal)) {
      return SRSLTE_ERROR; 
    }
  }
  return SRSLTE_SUCCESS;
}

void phy::rem_rnti(uint16_t rnti)
{
  if (SRSLTE_RNTI_ISUSER(rnti)) {
    workers_common.ue_db_rem_rnti(rnti);
  }
  for (uint32_t i=0;i<nof_workers;i++) {
    workers[i].rem_rnti(rnti);
  }
}

void phy::set_mch_period_stop(uint32_t stop)
{
  workers_common.set_mch_period_stop(stop);
}

void phy::get_metrics(phy_metrics_t metrics[ENB_METRICS_MAX_USERS])
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

void phy::set_config_dedicated(uint16_t rnti, phys_cfg_ded_s* dedicated)
{
  for (uint32_t i=0;i<nof_workers;i++) {
    workers[i].set_config_dedicated(rnti, dedicated);
  }
}

void phy::configure_mbsfn(sib_type2_s* sib2, sib_type13_r9_s* sib13, mcch_msg_s mcch)
{
  if (sib2->mbsfn_sf_cfg_list_present) {
    if (sib2->mbsfn_sf_cfg_list.size() == 0) {
      Warning("SIB2 does not have any MBSFN config although it was set as present\n");
    } else {
      if (sib2->mbsfn_sf_cfg_list.size() > 1) {
        Warning("SIB2 has %d MBSFN subframe configs - only 1 supported\n", sib2->mbsfn_sf_cfg_list.size());
      }
      phy_rrc_config.mbsfn.mbsfn_subfr_cnfg = sib2->mbsfn_sf_cfg_list[0];
    }
  } else {
    fprintf(stderr, "SIB2 has no MBSFN subframe config specified\n");
    return;
  }

  phy_rrc_config.mbsfn.mbsfn_notification_cnfg = sib13->notif_cfg_r9;
  if (sib13->mbsfn_area_info_list_r9.size() > 0) {
    if (sib13->mbsfn_area_info_list_r9.size() > 1) {
      Warning("SIB13 has %d MBSFN area info elements - only 1 supported\n", sib13->mbsfn_area_info_list_r9.size());
    }
    phy_rrc_config.mbsfn.mbsfn_area_info = sib13->mbsfn_area_info_list_r9[0];
  }

  phy_rrc_config.mbsfn.mcch = mcch;

  workers_common.configure_mbsfn(&phy_rrc_config.mbsfn);
}

// Start GUI
void phy::start_plot()
{
  workers[0].start_plot();
}

} // namespace srsenb
