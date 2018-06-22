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

#include "srslte/common/threads.h"
#include "srslte/common/log.h"
#include <sstream>
#include "srsenb/hdr/phy/txrx.h"

#include <assert.h>

#define Error(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) if (SRSLTE_DEBUG_ENABLED) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    if (SRSLTE_DEBUG_ENABLED) log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   if (SRSLTE_DEBUG_ENABLED) log_h->debug(fmt, ##__VA_ARGS__)

using namespace std; 


namespace srsenb {

void phch_common::set_nof_mutex(uint32_t nof_mutex_) {
  nof_mutex = nof_mutex_; 
  assert(nof_mutex <= max_mutex);
}

void phch_common::reset() {
  bzero(ul_grants, sizeof(mac_interface_phy::ul_sched_t)*TTIMOD_SZ);
  bzero(dl_grants, sizeof(mac_interface_phy::dl_sched_t)*TTIMOD_SZ);
}

bool phch_common::init(srslte_cell_t *cell_, srslte::radio* radio_h_, mac_interface_phy *mac_)
{
  radio = radio_h_;
  mac   = mac_; 
  memcpy(&cell, cell_, sizeof(srslte_cell_t));

  pthread_mutex_init(&user_mutex, NULL);
  
  is_first_of_burst = true; 
  is_first_tx = true; 
  for (uint32_t i=0;i<max_mutex;i++) {
    pthread_mutex_init(&tx_mutex[i], NULL);
  }
  reset(); 
  return true; 
}

void phch_common::stop() {
  for (uint32_t i=0;i<nof_mutex;i++) {
    pthread_mutex_trylock(&tx_mutex[i]);
    pthread_mutex_unlock(&tx_mutex[i]);
  }
}

void phch_common::worker_end(uint32_t tx_mutex_cnt, cf_t* buffer[SRSLTE_MAX_PORTS], uint32_t nof_samples, srslte_timestamp_t tx_time)
{

  // Wait previous TTIs to be transmitted 
  if (is_first_tx) {
    is_first_tx = false; 
  } else {
    pthread_mutex_lock(&tx_mutex[tx_mutex_cnt%nof_mutex]);
  }

  radio->set_tti(tx_mutex_cnt);
  radio->tx((void **) buffer, nof_samples, tx_time);
  
  // Trigger next transmission 
  pthread_mutex_unlock(&tx_mutex[(tx_mutex_cnt+1)%nof_mutex]);

  // Trigger MAC clock
  mac->tti_clock();
}

void phch_common::ue_db_clear(uint32_t sf_idx)
{
  for(std::map<uint16_t,common_ue>::iterator iter=common_ue_db.begin(); iter!=common_ue_db.end(); ++iter) {
    pending_ack_t *p = &((common_ue*)&iter->second)->pending_ack;
    for (uint32_t tb_idx = 0; tb_idx < SRSLTE_MAX_TB; tb_idx++) {
      p->is_pending[sf_idx][tb_idx] = false;
    }
  }
}

void phch_common::ue_db_add_rnti(uint16_t rnti)
{
  pthread_mutex_lock(&user_mutex);
  add_rnti(rnti);
  pthread_mutex_unlock(&user_mutex);
}

// Private function not mutexed
void phch_common::add_rnti(uint16_t rnti)
{
  for (int sf_idx=0;sf_idx<TTIMOD_SZ;sf_idx++) {
    for (uint32_t tb_idx = 0; tb_idx < SRSLTE_MAX_TB; tb_idx++) {
      common_ue_db[rnti].pending_ack.is_pending[sf_idx][tb_idx] = false;
    }
  }
}

void phch_common::ue_db_rem_rnti(uint16_t rnti)
{
  pthread_mutex_lock(&user_mutex);
  common_ue_db.erase(rnti);
  pthread_mutex_unlock(&user_mutex);
}

void phch_common::ue_db_set_ack_pending(uint32_t sf_idx, uint16_t rnti, uint32_t tb_idx, uint32_t last_n_pdcch)
{
  pthread_mutex_lock(&user_mutex);
  if (common_ue_db.count(rnti)) {
    common_ue_db[rnti].pending_ack.is_pending[sf_idx][tb_idx] = true;
    common_ue_db[rnti].pending_ack.n_pdcch[sf_idx]            = (uint16_t) last_n_pdcch;
  }
  pthread_mutex_unlock(&user_mutex);
}

bool phch_common::ue_db_is_ack_pending(uint32_t sf_idx, uint16_t rnti, uint32_t tb_idx, uint32_t *last_n_pdcch)
{
  bool ret = false;
  pthread_mutex_lock(&user_mutex);
  if (common_ue_db.count(rnti)) {
    ret = common_ue_db[rnti].pending_ack.is_pending[sf_idx][tb_idx];
    common_ue_db[rnti].pending_ack.is_pending[sf_idx][tb_idx] = false;

    if (ret && last_n_pdcch) {
      *last_n_pdcch = common_ue_db[rnti].pending_ack.n_pdcch[sf_idx];
    }
  }
  pthread_mutex_unlock(&user_mutex);
  return ret;
}

void phch_common::ue_db_set_ri(uint16_t rnti, uint8_t ri) {
  pthread_mutex_lock(&user_mutex);
  if (common_ue_db.count(rnti)) {
    common_ue_db[rnti].ri = ri;
  }
  pthread_mutex_unlock(&user_mutex);
}

uint8_t phch_common::ue_db_get_ri(uint16_t rnti)
{
  pthread_mutex_lock(&user_mutex);
  uint8_t ret = 0;
  if (common_ue_db.count(rnti)) {
    ret = common_ue_db[rnti].ri;
  }
  pthread_mutex_unlock(&user_mutex);
  return ret;
}

void phch_common::ue_db_set_last_ul_mod(uint16_t rnti, uint32_t tti, srslte_mod_t mcs)
{
  pthread_mutex_lock(&user_mutex);
  if (!common_ue_db.count(rnti)) {
    add_rnti(rnti);
  }
  common_ue_db[rnti].last_ul_mod[TTI_RX(tti)%(2*HARQ_DELAY_MS)] = mcs;
  pthread_mutex_unlock(&user_mutex);
}

srslte_mod_t phch_common::ue_db_get_last_ul_mod(uint16_t rnti, uint32_t tti)
{
  pthread_mutex_lock(&user_mutex);
  srslte_mod_t ret = SRSLTE_MOD_BPSK;
  if (common_ue_db.count(rnti)) {
    ret = common_ue_db[rnti].last_ul_mod[TTI_RX(tti)%(2*HARQ_DELAY_MS)];
  }
  pthread_mutex_unlock(&user_mutex);
  return ret;
}

void phch_common::ue_db_set_last_ul_tbs(uint16_t rnti, uint32_t tti, int tbs)
{
  pthread_mutex_lock(&user_mutex);
  if (!common_ue_db.count(rnti)) {
    add_rnti(rnti);
  }
  common_ue_db[rnti].last_ul_tbs[TTI_RX(tti)%(2*HARQ_DELAY_MS)] = tbs;
  pthread_mutex_unlock(&user_mutex);
}

int phch_common::ue_db_get_last_ul_tbs(uint16_t rnti, uint32_t tti) {
  int ret = -1;
  pthread_mutex_lock(&user_mutex);
  if (common_ue_db.count(rnti)) {
    ret = common_ue_db[rnti].last_ul_tbs[TTI_RX(tti)%(2*HARQ_DELAY_MS)];
  }
  pthread_mutex_unlock(&user_mutex);
  return ret;
}

void phch_common::configure_mbsfn(phy_interface_rrc::phy_cfg_mbsfn_t *cfg)
{
  memcpy(&mbsfn, cfg, sizeof(phy_interface_rrc::phy_cfg_mbsfn_t));
   
  build_mch_table();
  build_mcch_table();
  sib13_configured = true;
  mcch_configured = true;
}

void phch_common::build_mch_table()
{
   // First reset tables
  bzero(&mch_table[0], sizeof(uint8_t)*40);
  // 40 element table represents 4 frames (40 subframes)
  generate_mch_table(&mch_table[0], mbsfn.mbsfn_subfr_cnfg.subfr_alloc,(LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_ONE == mbsfn.mbsfn_subfr_cnfg.subfr_alloc_num_frames)?1:4);
  // Debug
  std::stringstream ss;
  ss << "|";
  for(uint32_t j=0; j<40; j++) {
    ss << (int) mch_table[j] << "|";
  }
}

void phch_common::build_mcch_table()
{
  bzero(&mcch_table[0], sizeof(uint8_t)*10);

  generate_mcch_table(&mcch_table[0], mbsfn.mbsfn_area_info.sf_alloc_info_r9);

  std::stringstream ss;
  ss << "|";
  for(uint32_t j=0; j<10; j++) {
    ss << (int) mcch_table[j] << "|";
  }
}

bool phch_common::is_mcch_subframe(subframe_cfg_t *cfg, uint32_t phy_tti)
{
  uint32_t sfn;   // System Frame Number
  uint8_t  sf;    // Subframe
  uint8_t  offset;
  uint8_t  period;

  sfn = phy_tti/10;
  sf  = phy_tti%10;

  if(sib13_configured) {
    LIBLTE_RRC_MBSFN_SUBFRAME_CONFIG_STRUCT *subfr_cnfg = &mbsfn.mbsfn_subfr_cnfg;
    LIBLTE_RRC_MBSFN_AREA_INFO_STRUCT *area_info = &mbsfn.mbsfn_area_info;

    offset = area_info->mcch_offset_r9;
    period = liblte_rrc_mcch_repetition_period_r9_num[area_info->mcch_repetition_period_r9];

    if((sfn%period == offset) && mcch_table[sf] > 0) {
      cfg->mbsfn_area_id = area_info->mbsfn_area_id_r9;
      cfg->non_mbsfn_region_length = liblte_rrc_non_mbsfn_region_length_num[area_info->non_mbsfn_region_length];
      cfg->mbsfn_mcs = liblte_rrc_mcch_signalling_mcs_r9_num[area_info->signalling_mcs_r9];
      cfg->mbsfn_encode = true;
      cfg->is_mcch = true;
      return true;
    }
  }
  return false;
}

bool phch_common::is_mch_subframe(subframe_cfg_t *cfg, uint32_t phy_tti)
{
  uint32_t sfn;   // System Frame Number
  uint8_t  sf;    // Subframe
  uint8_t  offset;
  uint8_t  period;

  sfn = phy_tti/10;
  sf  = phy_tti%10;

  // Set some defaults
  cfg->mbsfn_area_id            = 0;
  cfg->non_mbsfn_region_length  = 1;
  cfg->mbsfn_mcs                = 2;
  cfg->mbsfn_encode             = false;
  cfg->is_mcch = false;
  // Check for MCCH
  if (is_mcch_subframe(cfg, phy_tti)) {
    return true;
  }

  // Not MCCH, check for MCH
  LIBLTE_RRC_MBSFN_SUBFRAME_CONFIG_STRUCT *subfr_cnfg = &mbsfn.mbsfn_subfr_cnfg;
  LIBLTE_RRC_MBSFN_AREA_INFO_STRUCT *area_info = &mbsfn.mbsfn_area_info;

  offset = subfr_cnfg->radio_fr_alloc_offset;
  period = liblte_rrc_radio_frame_allocation_period_num[subfr_cnfg->radio_fr_alloc_period];

  if (LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_ONE == subfr_cnfg->subfr_alloc_num_frames) {
    if ((sfn%period == offset) && (mch_table[sf] > 0)) {
      if (sib13_configured) {
        cfg->mbsfn_area_id = area_info->mbsfn_area_id_r9;
        cfg->non_mbsfn_region_length = liblte_rrc_non_mbsfn_region_length_num[area_info->non_mbsfn_region_length];
        if (mcch_configured) {
          // Iterate through PMCH configs to see which one applies in the current frame
          LIBLTE_RRC_MCCH_MSG_STRUCT *mcch = &mbsfn.mcch;
          uint32_t sf_alloc_idx = sfn%liblte_rrc_mbsfn_common_sf_alloc_period_r9_num[mcch->commonsf_allocperiod_r9];
          for (uint32_t i=0; i<mcch->pmch_infolist_r9_size; i++) {
            //if(sf_alloc_idx < mch_period_stop) {
              cfg->mbsfn_mcs = mcch->pmch_infolist_r9[i].pmch_config_r9.datamcs_r9;
              cfg->mbsfn_encode = true;
            //}
          }
        
        }
      }
      return true;
    }
  } else if (LIBLTE_RRC_SUBFRAME_ALLOCATION_NUM_FRAMES_FOUR == subfr_cnfg->subfr_alloc_num_frames) {
    uint8_t idx = sfn%period;
    if ((idx >= offset) && (idx < offset+4)) {
      if (mch_table[(idx*10)+sf] > 0){
        if (sib13_configured) {
          cfg->mbsfn_area_id = area_info->mbsfn_area_id_r9;
          cfg->non_mbsfn_region_length = liblte_rrc_non_mbsfn_region_length_num[area_info->non_mbsfn_region_length];
         // TODO: check for MCCH configuration, set MCS and decode

        }
        return true;
      }
    }
  }

  return false;
}

void phch_common::get_sf_config(subframe_cfg_t *cfg, uint32_t phy_tti)
{
  if(is_mch_subframe(cfg, phy_tti)) {
    cfg->sf_type = SUBFRAME_TYPE_MBSFN;
  }else{
    cfg->sf_type = SUBFRAME_TYPE_REGULAR;
  }
}
}