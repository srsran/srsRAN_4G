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

#include <string.h>

#include "srslte/srslte.h"
#include "srslte/common/pdu.h"
#include "srsenb/hdr/mac/scheduler.h"

#define Error(fmt, ...)   log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(fmt, ##__VA_ARGS__)

namespace srsenb {

  
/*******************************************************
 * 
 * Initialization and sched configuration functions 
 * 
 *******************************************************/
sched::sched() : bc_aggr_level(0), rar_aggr_level(0), avail_rbg(0), P(0), start_rbg(0), si_n_rbg(0), rar_n_rbg(0),
                 nof_rbg(0), sf_idx(0), sfn(0), current_cfi(0) {
  current_tti = 0;
  log_h = NULL;
  dl_metric = NULL;
  ul_metric = NULL;
  rrc = NULL;

  bzero(&cfg, sizeof(cfg));
  bzero(&regs, sizeof(regs));
  bzero(&used_cce, sizeof(used_cce));
  bzero(&sched_cfg, sizeof(sched_cfg));
  bzero(&common_locations, sizeof(common_locations));
  bzero(&pdsch_re, sizeof(pdsch_re));

  for (int i = 0; i < 3; i++) {
    bzero(rar_locations[i], sizeof(sched_ue::sched_dci_cce_t) * 10);
  }
  reset();

  pthread_rwlock_init(&rwlock, NULL);
}

sched::~sched()
{
  srslte_regs_free(&regs);
  pthread_rwlock_wrlock(&rwlock);
  pthread_rwlock_unlock(&rwlock);
  pthread_rwlock_destroy(&rwlock);
}

void sched::init(rrc_interface_mac *rrc_, srslte::log* log)
{
  sched_cfg.pdsch_max_mcs = 28; 
  sched_cfg.pdsch_mcs     = -1;
  sched_cfg.pusch_max_mcs = 28; 
  sched_cfg.pusch_mcs     = -1;
  sched_cfg.nof_ctrl_symbols = 3; 
  log_h = log;   
  rrc   = rrc_; 
  reset();
}

int sched::reset()
{
  bzero(pending_msg3, sizeof(pending_msg3_t)*10);
  bzero(pending_rar, sizeof(sched_rar_t)*SCHED_MAX_PENDING_RAR);
  bzero(pending_sibs, sizeof(sched_sib_t)*MAX_SIBS); 
  configured = false;
  pthread_rwlock_wrlock(&rwlock);
  ue_db.clear();
  pthread_rwlock_unlock(&rwlock);
  return 0;
}

void sched::set_sched_cfg(sched_interface::sched_args_t* sched_cfg_)
{
  if (sched_cfg_) {
    memcpy(&sched_cfg, sched_cfg_, sizeof(sched_args_t));
  }
}

void sched::set_metric(sched::metric_dl* dl_metric_, sched::metric_ul* ul_metric_)
{
  dl_metric = dl_metric_; 
  ul_metric = ul_metric_; 
}

int sched::cell_cfg(sched_interface::cell_cfg_t* cell_cfg)
{
  // Basic cell config checks
  if (cell_cfg->si_window_ms == 0) {
    Error("SCHED: Invalid si-window length 0 ms\n");
    return -1;
  }

  memcpy(&cfg, cell_cfg, sizeof(sched_interface::cell_cfg_t));
    
  // Get DCI locations 
  if (srslte_regs_init(&regs, cfg.cell)) {
    Error("Getting DCI locations\n");
    return SRSLTE_ERROR;
  }

  P = srslte_ra_type0_P(cfg.cell.nof_prb);
  si_n_rbg  = ceilf((float) 4/P);
  rar_n_rbg = ceilf((float) 3/P);
  nof_rbg = (uint32_t) ceil((float) cfg.cell.nof_prb/P);
      
  // Compute Common locations for DCI for each CFI
  for (uint32_t cfi=0;cfi<3;cfi++) {
    generate_cce_location(&regs, &common_locations[cfi], cfi+1);     
  }

  // Compute UE locations for RA-RNTI 
  for (int cfi=0;cfi<3;cfi++) {
    for (int sf_idx=0;sf_idx<10;sf_idx++) {
      uint16_t ra_rnti = 1+sf_idx; 
      generate_cce_location(&regs, &rar_locations[cfi][sf_idx], cfi+1, sf_idx);
    }
  }  
  configured = true;
  
  return 0;
}


/*******************************************************
 * 
 * FAPI-like main sched interface. Wrappers to UE object
 * 
 *******************************************************/

int sched::ue_cfg(uint16_t rnti, sched_interface::ue_cfg_t *ue_cfg)
{
   // Add or config user
  pthread_rwlock_rdlock(&rwlock);
  ue_db[rnti].set_cfg(rnti, ue_cfg, &cfg, &regs, log_h);
  ue_db[rnti].set_max_mcs(sched_cfg.pusch_max_mcs, sched_cfg.pdsch_max_mcs);
  ue_db[rnti].set_fixed_mcs(sched_cfg.pusch_mcs, sched_cfg.pdsch_mcs);
  pthread_rwlock_unlock(&rwlock);

  return 0;
}

int sched::ue_rem(uint16_t rnti)
{
  int ret = 0;
  pthread_rwlock_wrlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db.erase(rnti);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

bool sched::ue_exists(uint16_t rnti) 
{
  pthread_rwlock_rdlock(&rwlock);
  bool ret = (ue_db.count(rnti) == 1);
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

void sched::phy_config_enabled(uint16_t rnti, bool enabled)
{
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].phy_config_enabled(current_tti, enabled);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
}

int sched::bearer_ue_cfg(uint16_t rnti, uint32_t lc_id, sched_interface::ue_bearer_cfg_t *cfg)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].set_bearer_cfg(lc_id, cfg);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::bearer_ue_rem(uint16_t rnti, uint32_t lc_id)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].rem_bearer(lc_id);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

uint32_t sched::get_dl_buffer(uint16_t rnti)
{
  uint32_t ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ret = ue_db[rnti].get_pending_dl_new_data(current_tti);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

uint32_t sched::get_ul_buffer(uint16_t rnti)
{
  uint32_t ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ret = ue_db[rnti].get_pending_ul_new_data(current_tti);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::dl_rlc_buffer_state(uint16_t rnti, uint32_t lc_id, uint32_t tx_queue, uint32_t retx_queue)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].dl_buffer_state(lc_id, tx_queue, retx_queue);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::dl_mac_buffer_state(uint16_t rnti, uint32_t ce_code)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].mac_buffer_state(ce_code);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::dl_ant_info(uint16_t rnti, LIBLTE_RRC_ANTENNA_INFO_DEDICATED_STRUCT *dl_ant_info) {
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].set_dl_ant_info(dl_ant_info);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::dl_ack_info(uint32_t tti, uint16_t rnti, uint32_t tb_idx, bool ack)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ret = ue_db[rnti].set_ack_info(tti, tb_idx, ack);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::ul_crc_info(uint32_t tti, uint16_t rnti, bool crc)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].set_ul_crc(tti, crc);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::dl_ri_info(uint32_t tti, uint16_t rnti, uint32_t cqi_value)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].set_dl_ri(tti, cqi_value);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::dl_pmi_info(uint32_t tti, uint16_t rnti, uint32_t pmi_value)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].set_dl_pmi(tti, pmi_value);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::dl_cqi_info(uint32_t tti, uint16_t rnti, uint32_t cqi_value)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].set_dl_cqi(tti, cqi_value);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::dl_rach_info(uint32_t tti, uint32_t ra_id, uint16_t rnti, uint32_t estimated_size)
{
  for (int i=0;i<SCHED_MAX_PENDING_RAR;i++) {
    if (!pending_rar[i].buf_rar) {
      pending_rar[i].ra_id   = ra_id; 
      pending_rar[i].rnti    = rnti;
      pending_rar[i].rar_tti = tti; 
      pending_rar[i].buf_rar = estimated_size;       
      return 0; 
    }
  }
  Warning("SCHED: New RACH discarted because maximum number of pending RAR exceeded (%d)\n", 
          SCHED_MAX_PENDING_RAR);
  return -1; 
}

int sched::ul_cqi_info(uint32_t tti, uint16_t rnti, uint32_t cqi, uint32_t ul_ch_code)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].set_ul_cqi(tti, cqi, ul_ch_code);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::ul_bsr(uint16_t rnti, uint32_t lcid, uint32_t bsr, bool set_value)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].ul_buffer_state(lcid, bsr, set_value);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::ul_recv_len(uint16_t rnti, uint32_t lcid, uint32_t len)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].ul_recv_len(lcid, len);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::ul_phr(uint16_t rnti, int phr)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].ul_phr(phr);
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

int sched::ul_sr_info(uint32_t tti, uint16_t rnti)
{
  int ret = 0;
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].set_sr();;
  } else {
    Error("User rnti=0x%x not found\n", rnti);
    ret = -1;
  }
  pthread_rwlock_unlock(&rwlock);
  return ret;
}

void sched::tpc_inc(uint16_t rnti)
{
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].tpc_inc();
  } else {
    Error("User rnti=0x%x not found\n", rnti);    
  }
  pthread_rwlock_unlock(&rwlock);
}

void sched::tpc_dec(uint16_t rnti)
{
  pthread_rwlock_rdlock(&rwlock);
  if (ue_db.count(rnti)) {
    ue_db[rnti].tpc_dec();
  } else {
    Error("User rnti=0x%x not found\n", rnti);    
  }
  pthread_rwlock_unlock(&rwlock);
}

/*******************************************************
 * 
 * Main sched functions 
 * 
 *******************************************************/

// Schedules Broadcast messages (SIB)
int sched::dl_sched_bc(dl_sched_bc_t bc[MAX_BC_LIST]) 
{
  int nof_bc_elems = 0; 
  for (int i=0;i<MAX_SIBS;i++) {
    if (cfg.sibs[i].len) {
      if (!pending_sibs[i].is_in_window) {
        uint32_t sf = 5;
        uint32_t x  = 0; 
        if (i > 0) {
          x  = (i-1)*cfg.si_window_ms;
          sf = x%10; 
        }
        if ((sfn%(cfg.sibs[i].period_rf)) == x/10 && sf_idx == sf) {
          pending_sibs[i].is_in_window = true; 
          pending_sibs[i].window_start = current_tti;
          pending_sibs[i].n_tx = 0;
        }
      } else {
        if (i > 0) {
          if (srslte_tti_interval(current_tti, pending_sibs[i].window_start) > cfg.si_window_ms) {
            pending_sibs[i].is_in_window = false; 
            pending_sibs[i].window_start = 0;
          }
        } else {
          // SIB1 is always in window 
          if (pending_sibs[0].n_tx == 4) {
            pending_sibs[0].n_tx = 0; 
          }
        }
      }
    }
  }  
  
  for (int i=0;i<MAX_SIBS;i++) {
    if (cfg.sibs[i].len            && 
      pending_sibs[i].is_in_window && 
      pending_sibs[i].n_tx < 4     &&
      avail_rbg > si_n_rbg) 
    {
      uint32_t nof_tx = 4; 
      if (i > 0) {
        if (cfg.si_window_ms <= 10) {
          nof_tx = 1; 
        } else if (cfg.si_window_ms <= 20) {
          nof_tx = 2; 
        } else if (cfg.si_window_ms <= 30) {
          nof_tx = 3; 
        } else {
          nof_tx = 4; 
        }
      } 
      uint32_t n_sf = (current_tti-pending_sibs[i].window_start); 
      if ((i == 0 && (sfn%2) == 0 && sf_idx == 5) ||
          (i >  0 && n_sf >= (cfg.si_window_ms/nof_tx)*pending_sibs[i].n_tx && sf_idx==9)) 
      {
        uint32_t rv = get_rvidx(pending_sibs[i].n_tx);
        
        // Try to allocate DCI first 
        if (generate_dci(&bc[nof_bc_elems].dci_location, &common_locations[current_cfi-1], bc_aggr_level)) {
          int tbs = generate_format1a(start_rbg*P, si_n_rbg*P, cfg.sibs[i].len, rv, &bc[nof_bc_elems].dci);
          if (tbs >= (int) cfg.sibs[i].len) {
            bc[nof_bc_elems].index = i; 
            bc[nof_bc_elems].type  = sched_interface::dl_sched_bc_t::BCCH;
            bc[nof_bc_elems].tbs   = tbs;
            
            Debug("SCHED: SIB%d, start_rb=%d, n_rb=%d, rv=%d, len=%d, period=%d, mcs=%d\n",
                  i+1, start_rbg*P, si_n_rbg*P, rv, cfg.sibs[i].len, cfg.sibs[i].period_rf, bc[nof_bc_elems].dci.mcs_idx);
            
            pending_sibs[i].n_tx++;
            
            nof_bc_elems++;
            avail_rbg -= si_n_rbg;
            start_rbg += si_n_rbg;                   
          } else {
            Error("Could not allocate DCI Format1A for SIB%d, len=%d\n", i+1, cfg.sibs[i].len);
          }
        } else {
          Warning("SCHED: Could not schedule DCI for SIB=%d, L=%d\n", i+1, bc_aggr_level);              
        }
      }
    } 
  }
  
  // Schedule Paging 
  if (rrc) {
    uint32_t paging_payload = 0; 
    if (rrc->is_paging_opportunity(current_tti, &paging_payload)) {
      if (avail_rbg > si_n_rbg && paging_payload) 
      {
        if (generate_dci(&bc[nof_bc_elems].dci_location, &common_locations[current_cfi-1], bc_aggr_level)) {          
          int tbs = generate_format1a(start_rbg*P, si_n_rbg*P, paging_payload, 0, &bc[nof_bc_elems].dci);
          if (tbs > 0) {
            
            bc[nof_bc_elems].type = sched_interface::dl_sched_bc_t::PCCH;
            bc[nof_bc_elems].tbs  = tbs;
            nof_bc_elems++;
            
            Info("SCHED: PCH start_rb=%d, tbs=%d, mcs=%d\n", start_rbg, tbs, bc[nof_bc_elems].dci.mcs_idx);

            avail_rbg -= si_n_rbg;
            start_rbg += si_n_rbg;                   
            
          }
        }
      }    
    }
  }
  
  return nof_bc_elems;
}


// Schedules RAR 
int sched::dl_sched_rar(dl_sched_rar_t rar[MAX_RAR_LIST]) 
{

  int nof_rar_elems = 0; 
  for (uint32_t i=0;i<SCHED_MAX_PENDING_RAR;i++) 
  {
    if (pending_rar[i].buf_rar > 0 && avail_rbg >= rar_n_rbg)
    {
      /* Check if we are still within the RAR window, otherwise discard it */
      if (current_tti <= (pending_rar[i].rar_tti + cfg.prach_rar_window + 3)%10240 && current_tti >= pending_rar[i].rar_tti + 3)
      {                
        // Try to schedule DCI for this RAR 
        if (generate_dci(&rar[nof_rar_elems].dci_location, &rar_locations[current_cfi-1][sf_idx], rar_aggr_level)) {
          
          /* Find all pending RARs with same transmission TTI */        
          uint32_t tti        = pending_rar[i].rar_tti;
          uint32_t rar_sfidx  = (tti+1)%10; 
          uint32_t buf_rar    = 0;
          uint32_t nof_grants = 0; 
          for (int j=0;j<SCHED_MAX_PENDING_RAR;j++) {
            if (pending_rar[j].rar_tti == tti) {
              
              uint32_t L_prb = 3; 
              uint32_t n_prb = 2; 
              
              if (nof_grants == 0) {
                bzero(&rar[nof_rar_elems].grants[nof_grants], sizeof(srslte_dci_rar_grant_t));
                rar[nof_rar_elems].grants[nof_grants].grant.tpc_pusch = 3;
                rar[nof_rar_elems].grants[nof_grants].grant.trunc_mcs = 0; 
                rar[nof_rar_elems].grants[nof_grants].grant.rba       = srslte_ra_type2_to_riv(L_prb, n_prb, cfg.cell.nof_prb); 
                rar[nof_rar_elems].grants[nof_grants].ra_id           = pending_rar[j].ra_id;               
                buf_rar += pending_rar[i].buf_rar;
                pending_rar[j].buf_rar = 0; 
                pending_rar[j].rar_tti = 0;            
                
                // Save UL resources 
                uint32_t pending_tti=(current_tti+MSG3_DELAY_MS+HARQ_DELAY_MS)%10;
                pending_msg3[pending_tti].enabled = true; 
                pending_msg3[pending_tti].rnti    = pending_rar[j].rnti; 
                pending_msg3[pending_tti].L       = L_prb; 
                pending_msg3[pending_tti].n_prb   = n_prb; 
                pending_msg3[pending_tti].mcs     = rar[nof_rar_elems].grants[nof_grants].grant.trunc_mcs; 
            
                log_h->info("SCHED: RAR, ra_id=%d, rnti=0x%x, rarnti_idx=%d, start_rb=%d, n_rb=%d, rar_grant_rba=%d, rar_grant_mcs=%d\n",
                  pending_rar[j].ra_id, pending_rar[j].rnti, rar_sfidx, start_rbg*P, rar_n_rbg*P,
                  rar[nof_rar_elems].grants[nof_grants].grant.rba, 
                  rar[nof_rar_elems].grants[nof_grants].grant.trunc_mcs);
              } else {
                log_h->warning("Only 1 RA is responded at a time. Found %d for TTI=%d\n", nof_grants+1, tti);
              }
              nof_grants++;
            }
          }        
          
          rar[nof_rar_elems].nof_grants = nof_grants; 
          rar[nof_rar_elems].rarnti     = rar_sfidx; 
                  
          if (generate_format1a(start_rbg*P, rar_n_rbg*P, buf_rar, 0, &rar[nof_rar_elems].dci) >= 0) {
            rar[nof_rar_elems].tbs = buf_rar; 
            nof_rar_elems++;
            avail_rbg -= rar_n_rbg;
            start_rbg += rar_n_rbg;
          } else {
            Error("SCHED: Allocating Format1A grant\n");
          }
                              
        } else {
          log_h->warning("SCHED: Could not schedule DCI for RAR tti=%d, L=%d\n", pending_rar[i].rar_tti, rar_aggr_level);
        }
      } else {
        log_h->console("SCHED: Could not transmit RAR within the window (RA TTI=%d, Window=%d, Now=%d)\n", 
                pending_rar[i].rar_tti, cfg.prach_rar_window, current_tti);
        pending_rar[i].buf_rar = 0; 
        pending_rar[i].rar_tti = 0; 
      }
    }
  }  
  return nof_rar_elems; 
}

// Schedules data to users
int sched::dl_sched_data(dl_sched_data_t data[MAX_DATA_LIST]) 
{
  uint32_t nof_ctrl_symbols = (cfg.cell.nof_prb<10)?(current_cfi+1):current_cfi; 
  dl_metric->new_tti(ue_db, start_rbg, avail_rbg, nof_ctrl_symbols, current_tti); 
  
  int nof_data_elems = 0; 
  for(std::map<uint16_t, sched_ue>::iterator iter=ue_db.begin(); iter!=ue_db.end(); ++iter) {
    sched_ue *user = (sched_ue*) &iter->second;
    uint16_t rnti  = (uint16_t) iter->first;

    uint32_t data_before = user->get_pending_dl_new_data(current_tti);
    dl_harq_proc *h = dl_metric->get_user_allocation(user);
    srslte_dci_format_t dci_format = user->get_dci_format();
    data[nof_data_elems].dci_format = dci_format;

    uint32_t aggr_level = user->get_aggr_level(srslte_dci_format_sizeof(SRSLTE_DCI_FORMAT1, cfg.cell.nof_prb, cfg.cell.nof_ports));
    if (h) {
      // Try to schedule DCI first 
      if (generate_dci(&data[nof_data_elems].dci_location, 
                       user->get_locations(current_cfi, sf_idx),
                       aggr_level, user))
      {     
        bool is_newtx = h->is_empty(0) && h->is_empty(1) ;
        int tbs = 0;
        switch(dci_format) {
          case SRSLTE_DCI_FORMAT1:
            tbs = user->generate_format1(h, &data[nof_data_elems], current_tti, current_cfi);
            break;
          case SRSLTE_DCI_FORMAT2:
            tbs = user->generate_format2(h, &data[nof_data_elems], current_tti, current_cfi);
            break;
          case SRSLTE_DCI_FORMAT2A:
            tbs = user->generate_format2a(h, &data[nof_data_elems], current_tti, current_cfi);
            break;
          default:
            Error("DCI format (%d) not implemented\n", dci_format);
        }
        if (tbs > 0) {
          log_h->info("SCHED: DL %s rnti=0x%x, pid=%d, mask=0x%x, dci=%d,%d, n_rtx=%d, tbs=%d, buffer=%d/%d, tb_en={%s,%s}\n",
                      !is_newtx?"retx":"tx", rnti, h->get_id(), h->get_rbgmask(), 
                      data[nof_data_elems].dci_location.L, data[nof_data_elems].dci_location.ncce, h->nof_retx(0) + h->nof_retx(1),
                      tbs,
                      data_before, user->get_pending_dl_new_data(current_tti),
                      data[nof_data_elems].dci.tb_en[0]?"y":"n",
                      data[nof_data_elems].dci.tb_en[1]?"y":"n");
          nof_data_elems++;
        } else {
          log_h->warning("SCHED: Error DL %s rnti=0x%x, pid=%d, mask=0x%x, dci=%d,%d, tbs=%d, buffer=%d\n", 
                      !is_newtx?"retx":"tx", rnti, h->get_id(), h->get_rbgmask(), 
                      data[nof_data_elems].dci_location.L, data[nof_data_elems].dci_location.ncce,
                      tbs, user->get_pending_dl_new_data(current_tti));          
        }      
      } else {
        for(uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
          h->reset(tb);
        }
        Warning("SCHED: Could not schedule DL DCI for rnti=0x%x, pid=%d, cfi=%d\n", rnti, h->get_id(), current_cfi);
      }      
    }

    // Reset blocked PIDs
    user->reset_timeout_dl_harq(current_tti);

  }


  
  return nof_data_elems; 
} 

// Downlink Scheduler 
int sched::dl_sched(uint32_t tti, sched_interface::dl_sched_res_t* sched_result)
{
  if (!configured) {
    return 0; 
  }

  /* If ul_sched() not yet called this tti, reset CCE state */
  if (current_tti != tti) {
    bzero(used_cce, MAX_CCE*sizeof(bool));    
  }

  /* Initialize variables */
  current_tti = tti; 
  sfn = tti/10; 
  sf_idx = tti%10; 
  avail_rbg = nof_rbg; 
  start_rbg = 0;
  current_cfi = sched_cfg.nof_ctrl_symbols; 
  bc_aggr_level = 2; 
  rar_aggr_level = 2; 
  bzero(sched_result, sizeof(sched_interface::dl_sched_res_t));

  pthread_rwlock_rdlock(&rwlock);

  /* Schedule Broadcast data */
  sched_result->nof_bc_elems   += dl_sched_bc(sched_result->bc);
  
  /* Schedule RAR */
  sched_result->nof_rar_elems  += dl_sched_rar(sched_result->rar);

  /* Schedule pending RLC data */
  sched_result->nof_data_elems += dl_sched_data(sched_result->data);

  pthread_rwlock_unlock(&rwlock);

  /* Set CFI */
  sched_result->cfi = current_cfi; 
  
  return 0;
}

// Uplink sched 
int sched::ul_sched(uint32_t tti, srsenb::sched_interface::ul_sched_res_t* sched_result)
{
  typedef std::map<uint16_t, sched_ue>::iterator it_t;

  if (!configured) {
    return 0; 
  }

  if (cfg.prach_freq_offset + 6 > cfg.cell.nof_prb) {
    fprintf(stderr, "Invalid PRACH configuration: frequency offset=%d outside bandwidth limits\n", cfg.prach_freq_offset);
    return -1;
  }

  /* If dl_sched() not yet called this tti (this tti is +4ms advanced), reset CCE state */
  if (TTI_TX(current_tti) != tti) {
    bzero(used_cce, MAX_CCE*sizeof(bool));    
  }
  
  /* Initialize variables */
  current_tti = tti; 
  sfn = tti/10;
  if (tti > HARQ_DELAY_MS) {
    sf_idx = (tti-HARQ_DELAY_MS)%10;
  } else {
    sf_idx = (tti+10240-HARQ_DELAY_MS)%10;
  }
  int nof_dci_elems   = 0; 
  int nof_phich_elems = 0; 
    
  // current_cfi is set in dl_sched() 
  bzero(sched_result, sizeof(sched_interface::ul_sched_res_t));
  ul_metric->reset_allocation(cfg.cell.nof_prb);

  pthread_rwlock_rdlock(&rwlock);

  // Get HARQ process for this TTI 
  for(it_t iter=ue_db.begin(); iter!=ue_db.end(); ++iter) {
    sched_ue *user = (sched_ue*) &iter->second;
    uint16_t rnti  = (uint16_t) iter->first; 

    user->has_pucch = false;

    ul_harq_proc *h = user->get_ul_harq(current_tti);

    /* Indicate PHICH acknowledgment if needed */
    if (h->has_pending_ack()) {
      sched_result->phich[nof_phich_elems].phich = h->get_ack(0)?ul_sched_phich_t::ACK:ul_sched_phich_t::NACK;
      sched_result->phich[nof_phich_elems].rnti = rnti;
      nof_phich_elems++;
    }
  }

  // Update available allocation if there's a pending RAR
  if (pending_msg3[tti%10].enabled) {
    ul_harq_proc::ul_alloc_t msg3 = {pending_msg3[tti%10].n_prb, pending_msg3[tti%10].L};
    if(ul_metric->update_allocation(msg3)) {
      log_h->debug("SCHED: Allocated msg3 RBs within (%d,%d)\n", msg3.RB_start, msg3.RB_start + msg3.L);
    }
    else {
      log_h->warning("SCHED: Could not allocate msg3 within (%d,%d)\n", msg3.RB_start, msg3.RB_start + msg3.L);
    }
  }

  // Allocate PUCCH resources
  if (cfg.nrb_pucch >= 0) {
    ul_harq_proc::ul_alloc_t pucch = {0, (uint32_t) cfg.nrb_pucch};
    if(!ul_metric->update_allocation(pucch)) {
      log_h->warning("SCHED: Failed to allocate PUCCH\n");
    }
    pucch.RB_start = cfg.cell.nof_prb-cfg.nrb_pucch;
    pucch.L        = (uint32_t) cfg.nrb_pucch;
    if(!ul_metric->update_allocation(pucch)) {
      log_h->warning("SCHED: Failed to allocate PUCCH\n");
    } else {
      log_h->debug("Allocating PUCCH (%d,%d)\n", pucch.RB_start, pucch.RB_start+pucch.L);
    }
  } else {
    for(it_t iter=ue_db.begin(); iter!=ue_db.end(); ++iter) {
      sched_ue *user = (sched_ue*) &iter->second;
      uint16_t rnti  = (uint16_t) iter->first;
      uint32_t prb_idx[2] = {0, 0};
      if (user->get_pucch_sched(current_tti, prb_idx)) {
        user->has_pucch = true;
        // allocate PUCCH
        for (int i=0;i<2;i++) {
          ul_harq_proc::ul_alloc_t pucch = {prb_idx[i], 1};
          ul_metric->update_allocation(pucch);
        }
      }
    }
  }

  // reserve PRBs for PRACH
  if(srslte_prach_tti_opportunity_config(cfg.prach_config, tti, -1)) {
    ul_harq_proc::ul_alloc_t prach = {cfg.prach_freq_offset, 6};
    if(!ul_metric->update_allocation(prach)) {
      log_h->warning("SCHED: Failed to allocate PRACH RBs within (%d,%d)\n", prach.RB_start, prach.RB_start + prach.L);
    }
    else {
      log_h->debug("SCHED: Allocated PRACH RBs within (%d,%d)\n", prach.RB_start, prach.RB_start + prach.L);
    }
  }

  ul_metric->new_tti(ue_db, cfg.cell.nof_prb, current_tti);

  // Now allocate PUSCH 
  for(it_t iter=ue_db.begin(); iter!=ue_db.end(); ++iter) {
    sched_ue *user = (sched_ue*) &iter->second;
    uint16_t rnti  = (uint16_t) iter->first; 

    ul_harq_proc *h = NULL;

    // Check if there are pending Msg3 transmissions 
    bool is_rar = false; 
    if (pending_msg3[tti%10].enabled && pending_msg3[tti%10].rnti == rnti) {
      h = user->get_ul_harq(tti);
      if (h) {
        ul_harq_proc::ul_alloc_t alloc;
        alloc.L        = pending_msg3[tti%10].L;
        alloc.RB_start = pending_msg3[tti%10].n_prb;
        h->set_alloc(alloc);
        h->set_rar_mcs(pending_msg3[tti%10].mcs);
        is_rar = true; 
        pending_msg3[tti%10].enabled = false;
      } else {
        Warning("No HARQ pid available for transmission of Msg3\n");
      }
    } else {
      h = ul_metric->get_user_allocation(user);             
    }
    if (h) 
    {   
      ul_harq_proc::ul_alloc_t alloc = h->get_alloc(); 
      bool is_newtx = h->is_empty(0);
      bool needs_pdcch = !h->is_adaptive_retx() && !is_rar;

      // Set number of retx
      if (is_newtx) {
        if (is_rar) {
          h->set_max_retx(cfg.maxharq_msg3tx);
        } else {
          h->set_max_retx(user->get_max_retx()); 
        }
      }

      // Generate PDCCH except for RAR and non-adaptive retx
      if (needs_pdcch) {
        uint32_t aggr_level = user->get_aggr_level(srslte_dci_format_sizeof(SRSLTE_DCI_FORMAT0, cfg.cell.nof_prb, cfg.cell.nof_ports));
        if (!generate_dci(&sched_result->pusch[nof_dci_elems].dci_location, 
            user->get_locations(current_cfi, sf_idx),
            aggr_level)) 
        {
          h->reset(0);
          log_h->warning("SCHED: Could not schedule UL DCI rnti=0x%x, pid=%d, L=%d, sf_idx=%d\n",
                 rnti, h->get_id(), aggr_level, sf_idx);

          sched_result->pusch[nof_dci_elems].needs_pdcch = false;
        } else {
          sched_result->pusch[nof_dci_elems].needs_pdcch = true; 
        }
      } else {
        sched_result->pusch[nof_dci_elems].needs_pdcch = false; 
      }

      // Generate grant unless DCI could not be generated and was required 
      if (sched_result->pusch[nof_dci_elems].needs_pdcch == needs_pdcch) {
        uint32_t pending_data_before = user->get_pending_ul_new_data(current_tti); 
        if (user->generate_format0(h, &sched_result->pusch[nof_dci_elems], current_tti, user->needs_cqi(tti, true)) > 0) 
        {
          
          if (is_newtx) {
            // Un-trigger SR
            user->unset_sr();
          }

          log_h->info("SCHED: %s %s rnti=0x%x, pid=%d, dci=%d,%d, grant=(%d,%d), n_rtx=%d, tbs=%d, bsr=%d (%d-%d)\n",
                      is_rar?"RAR":"UL",
                      is_newtx?"tx":"retx",                
                      rnti, h->get_id(), 
                      sched_result->pusch[nof_dci_elems].dci_location.L, sched_result->pusch[nof_dci_elems].dci_location.ncce,
                      alloc.RB_start, alloc.RB_start+alloc.L, h->nof_retx(0), sched_result->pusch[nof_dci_elems].tbs,
                      user->get_pending_ul_new_data(current_tti),pending_data_before, user->get_pending_ul_old_data());

          nof_dci_elems++;          
        } else {
          log_h->warning("SCHED: Error %s %s rnti=0x%x, pid=%d, dci=%d,%d, grant=(%d,%d), tbs=%d, bsr=%d\n",
                      is_rar?"RAR":"UL",
                      is_newtx?"tx":"retx",                
                      rnti, h->get_id(), 
                      sched_result->pusch[nof_dci_elems].dci_location.L, sched_result->pusch[nof_dci_elems].dci_location.ncce,
                      alloc.RB_start, alloc.RB_start+alloc.L, sched_result->pusch[nof_dci_elems].tbs,
                      user->get_pending_ul_new_data(current_tti));
        }   
      }
    }
  }

  // Update pending data counters after this TTI
  for(it_t iter=ue_db.begin(); iter!=ue_db.end(); ++iter) {
    sched_ue *user = (sched_ue *) &iter->second;
    uint16_t rnti = (uint16_t) iter->first;

    user->get_ul_harq(current_tti)->reset_pending_data();
  }

  pthread_rwlock_unlock(&rwlock);

  sched_result->nof_dci_elems   = nof_dci_elems;
  sched_result->nof_phich_elems = nof_phich_elems;

  return SRSLTE_SUCCESS;
}


/*******************************************************
 * 
 * Helper functions 
 * 
 *******************************************************/

void sched::generate_cce_location(srslte_regs_t *regs_, sched_ue::sched_dci_cce_t* location, 
                                   uint32_t cfi, uint32_t sf_idx, uint16_t rnti)
{
  srslte_dci_location_t loc[64];
  uint32_t nloc = 0; 
  if (rnti == 0) {
    nloc = srslte_pdcch_common_locations_ncce(srslte_regs_pdcch_ncce(regs_, cfi), 
                                       loc, 64);       
  } else {
    nloc = srslte_pdcch_ue_locations_ncce(srslte_regs_pdcch_ncce(regs_, cfi), 
                                   loc, 64, sf_idx, rnti);           
  }

  for (uint32_t l=0;l<=3;l++) {
    int n=0;
    for (uint32_t i=0;i<nloc;i++) {
      if (loc[i].L == l) {
        location->cce_start[l][n] = loc[i].ncce;          
        n++;
      }
    }
    location->nof_loc[l] = n;     
  }
  
}


#define NCCE(L) (1<<L)

bool sched::generate_dci(srslte_dci_location_t *sched_location, sched_ue::sched_dci_cce_t *locations, uint32_t aggr_level, sched_ue *user) 
{
  if (!locations->nof_loc[aggr_level]) {
    Error("In generate_dci(): No locations for aggr_level=%d\n", aggr_level);
    return false;
  }
  uint32_t nof_cand  = 0;
  uint32_t test_cand = rand()%locations->nof_loc[aggr_level];
  bool allocated=false;

  while(nof_cand<locations->nof_loc[aggr_level] && !allocated) {
    uint32_t ncce = locations->cce_start[aggr_level][test_cand];
    bool used = false;
    if (user) {
      used = user->pucch_sr_collision(current_tti, ncce);
    }
    for (int j=0;j<NCCE(aggr_level) && !used;j++) {
      if (used_cce[ncce+j]) {
        used = true; 
      }
    }
    if (used) {
      test_cand++;
      if (test_cand==locations->nof_loc[aggr_level]) {
        test_cand = 0;
      }
      nof_cand++;
    } else {
      for (int j=0;j<NCCE(aggr_level) && !used;j++) {
        used_cce[ncce+j] = true; 
      }
      allocated = true; 
      Debug("SCHED: Allocated DCI L=%d, ncce=%d\n", aggr_level, ncce);
    }
  }
  
  if (allocated && sched_location) {
    sched_location->L = aggr_level; 
    sched_location->ncce = locations->cce_start[aggr_level][test_cand];
  }
    
  return allocated; 
}

int sched::generate_format1a(uint32_t rb_start, uint32_t l_crb, uint32_t tbs_bytes, uint32_t rv, srslte_ra_dl_dci_t *dci) 
{
  /* Calculate I_tbs for this TBS */
  int tbs = tbs_bytes*8;
  int i; 
  int mcs = -1; 
  for (i=0;i<27;i++) {
    if (srslte_ra_tbs_from_idx(i, 2) >= tbs) {
      dci->type2_alloc.n_prb1a = srslte_ra_type2_t::SRSLTE_RA_TYPE2_NPRB1A_2;
      mcs = i; 
      tbs = srslte_ra_tbs_from_idx(i, 2);
      break;
    } else if (srslte_ra_tbs_from_idx(i, 3) >= tbs) {
      dci->type2_alloc.n_prb1a = srslte_ra_type2_t::SRSLTE_RA_TYPE2_NPRB1A_3;
      mcs = i; 
      tbs = srslte_ra_tbs_from_idx(i, 3);
      break;
    }
  }
  if (i == 28) {
    Error("Can't allocate Format 1A for TBS=%d\n", tbs);
    return -1; 
  }

  Debug("ra_tbs=%d/%d, tbs_bytes=%d, tbs=%d, mcs=%d\n",
        srslte_ra_tbs_from_idx(mcs, 2),srslte_ra_tbs_from_idx(mcs, 3),tbs_bytes,tbs,mcs);
  
  dci->alloc_type = SRSLTE_RA_ALLOC_TYPE2; 
  dci->type2_alloc.mode = srslte_ra_type2_t::SRSLTE_RA_TYPE2_LOC;
  dci->type2_alloc.L_crb = l_crb; 
  dci->type2_alloc.RB_start = rb_start; 
  dci->harq_process = 0; 
  dci->mcs_idx = mcs; 
  dci->rv_idx = rv;   
  dci->tb_en[0] = true; 
  dci->tb_en[1] = false; 
  
  return tbs; 
}





}


