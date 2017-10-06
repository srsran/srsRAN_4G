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
#include <boost/concept_check.hpp>
#include <srslte/interfaces/sched_interface.h>
#include <srslte/phy/phch/pucch.h>
#include <srslte/srslte.h>
#include <srslte/phy/common/phy_common.h>

#include "srslte/srslte.h"
#include "srslte/common/pdu.h"
#include "mac/scheduler_ue.h"
#include "mac/scheduler.h"

#define Error(fmt, ...)   log_h->error_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug_line(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

/****************************************************** 
 *                  UE class                          *
 ******************************************************/

namespace srsenb {


/*******************************************************
 * 
 * Initialization and configuration functions 
 * 
 *******************************************************/

sched_ue::sched_ue()
{
  reset(); 
}

void sched_ue::set_cfg(uint16_t rnti_, sched_interface::ue_cfg_t *cfg_, sched_interface::cell_cfg_t *cell_cfg, 
                            srslte_regs_t *regs, srslte::log *log_h_) 
{
  reset();
  
  rnti  = rnti_; 
  log_h = log_h_; 
  memcpy(&cell, &cell_cfg->cell, sizeof(srslte_cell_t));

  max_mcs_dl = 28; 
  max_mcs_ul = 28; 

  if (cfg_) {
    memcpy(&cfg, cfg_, sizeof(sched_interface::ue_cfg_t));
  }  
  
  Info("SCHED: Added user rnti=0x%x\n", rnti);
  for (int i=0;i<sched_interface::MAX_LC;i++) {
    set_bearer_cfg(i, &cfg.ue_bearers[i]);    
  }

  // Config HARQ processes 
  for (int i=0;i<SCHED_MAX_HARQ_PROC;i++) {
    dl_harq[i].config(i, cfg.maxharq_tx, log_h); 
    ul_harq[i].config(i, cfg.maxharq_tx, log_h);
  }
  
  // Generate allowed CCE locations   
  for (int cfi=0;cfi<3;cfi++) {
    for (int sf_idx=0;sf_idx<10;sf_idx++) {
      sched::generate_cce_location(regs, &dci_locations[cfi][sf_idx], cfi+1, sf_idx, rnti);
    }
  }    
}

void sched_ue::reset()
{
  bzero(&cfg, sizeof(sched_interface::ue_cfg_t));
  sr = false; 
  next_tpc_pusch = 1;
  next_tpc_pucch = 1; 
  buf_mac = 0; 
  buf_ul  = 0;
  phy_config_dedicated_enabled = false; 
  dl_cqi = 1; 
  ul_cqi = 1; 
  dl_cqi_tti = 0; 
  ul_cqi_tti = 0; 
  cqi_request_tti = 0; 
  for (int i=0;i<SCHED_MAX_HARQ_PROC;i++) {
    dl_harq[i].reset();
    ul_harq[i].reset();
  }
  for (int i=0;i<sched_interface::MAX_LC; i++) {
    rem_bearer(i);
  }
}

void sched_ue::set_fixed_mcs(int mcs_ul, int mcs_dl) {
  fixed_mcs_ul = mcs_ul; 
  fixed_mcs_dl = mcs_dl; 
}

void sched_ue::set_max_mcs(int mcs_ul, int mcs_dl) {
  if (mcs_ul < 0) {
    max_mcs_ul = 28;     
  } else {
    max_mcs_ul = mcs_ul;     
  }
  if (mcs_dl < 0) {
    max_mcs_dl = 28;     
  } else {
    max_mcs_dl = mcs_dl;     
  }
}


/*******************************************************
 * 
 * FAPI-like main scheduler interface. 
 * 
 *******************************************************/

void sched_ue::set_bearer_cfg(uint32_t lc_id, sched_interface::ue_bearer_cfg_t* cfg)
{
  if (lc_id < sched_interface::MAX_LC) {
    memcpy(&lch[lc_id].cfg, cfg, sizeof(sched_interface::ue_bearer_cfg_t));
    lch[lc_id].buf_tx = 0; 
    lch[lc_id].buf_retx = 0; 
    if (lch[lc_id].cfg.direction != sched_interface::ue_bearer_cfg_t::IDLE) {
      Info("SCHED: Set bearer config lc_id=%d, direction=%d\n", lc_id, (int) lch[lc_id].cfg.direction);
    }
  }
}

void sched_ue::rem_bearer(uint32_t lc_id)
{
  if (lc_id < sched_interface::MAX_LC) {
    bzero(&lch[lc_id], sizeof(ue_bearer_t));
  }
}

void sched_ue::phy_config_enabled(uint32_t tti, bool enabled)
{
  dl_cqi_tti = tti; 
  phy_config_dedicated_enabled = enabled; 
}

void sched_ue::ul_buffer_state(uint8_t lc_id, uint32_t bsr, bool set_value)
{
  if (lc_id < sched_interface::MAX_LC) {
    if (set_value) {
      lch[lc_id].bsr = bsr;
    } else {
      lch[lc_id].bsr += bsr;
    }
  }
  Debug("SCHED: bsr=%d, lcid=%d, bsr={%d,%d,%d,%d}\n", bsr, lc_id,
       lch[0].bsr, lch[1].bsr, lch[2].bsr, lch[3].bsr);
}

void sched_ue::ul_phr(int phr)
{ 
  power_headroom= phr; 
}

void sched_ue::dl_buffer_state(uint8_t lc_id, uint32_t tx_queue, uint32_t retx_queue)
{
  if (lc_id < sched_interface::MAX_LC) {
    lch[lc_id].buf_retx = retx_queue;
    lch[lc_id].buf_tx   = tx_queue;
    Debug("SCHED: DL lcid=%d buffer_state=%d,%d\n", lc_id, tx_queue, retx_queue);
  }
}

void sched_ue::mac_buffer_state(uint32_t ce_code)
{
  buf_mac++; 
}

void sched_ue::set_sr()
{
  sr = true; 
}

void sched_ue::unset_sr()
{
  sr = false; 
}

bool sched_ue::pucch_sr_collision(uint32_t current_tti, uint32_t n_cce)
{
  if (!phy_config_dedicated_enabled) {
    return false; 
  }
  srslte_pucch_sched_t pucch_sched; 
  pucch_sched.n_pucch_sr = cfg.sr_N_pucch;
  pucch_sched.n_pucch_2  = cfg.n_pucch_cqi;
  pucch_sched.N_pucch_1  = cfg.pucch_cfg.n1_pucch_an; 

  bool has_sr = cfg.sr_enabled && srslte_ue_ul_sr_send_tti(cfg.sr_I, current_tti);    
  if (!has_sr) {
    return false; 
  }
  uint32_t n_pucch_sr = srslte_pucch_get_npucch(n_cce, SRSLTE_PUCCH_FORMAT_1A, true, &pucch_sched);
  uint32_t n_pucch_nosr = srslte_pucch_get_npucch(n_cce, SRSLTE_PUCCH_FORMAT_1A, false, &pucch_sched);
  if (srslte_pucch_n_prb(&cfg.pucch_cfg, SRSLTE_PUCCH_FORMAT_1A, n_pucch_sr, cell.nof_prb, cell.cp, 0) == 
      srslte_pucch_n_prb(&cfg.pucch_cfg, SRSLTE_PUCCH_FORMAT_1A, n_pucch_nosr, cell.nof_prb, cell.cp, 0)) 
  {
    return true;       
  } else {
    return false; 
  }
}

bool sched_ue::get_pucch_sched(uint32_t current_tti, uint32_t prb_idx[2])
{
  if (!phy_config_dedicated_enabled) {
    return false; 
  }
  srslte_pucch_sched_t pucch_sched;
  pucch_sched.sps_enabled = false;
  pucch_sched.n_pucch_sr = cfg.sr_N_pucch;
  pucch_sched.n_pucch_2  = cfg.n_pucch_cqi;
  pucch_sched.N_pucch_1  = cfg.pucch_cfg.n1_pucch_an;
  
  bool has_sr = cfg.sr_enabled && srslte_ue_ul_sr_send_tti(cfg.sr_I, current_tti);    
  
  // First check if it has pending ACKs 
  for (int i=0;i<SCHED_MAX_HARQ_PROC;i++) {
    if (((dl_harq[i].get_tti()+4)%10240) == current_tti) {
      uint32_t n_pucch = srslte_pucch_get_npucch(dl_harq[i].get_n_cce(), SRSLTE_PUCCH_FORMAT_1A, has_sr, &pucch_sched);
      if (prb_idx) {
        for (int i=0;i<2;i++) {
          prb_idx[i] = srslte_pucch_n_prb(&cfg.pucch_cfg, SRSLTE_PUCCH_FORMAT_1A, n_pucch, cell.nof_prb, cell.cp, i);
        }
      }
      Debug("SCHED: Reserved Format1A PUCCH for rnti=0x%x, n_prb=%d,%d, n_pucch=%d, ncce=%d, has_sr=%d, n_pucch_1=%d\n",
           rnti, prb_idx[0], prb_idx[1], n_pucch, dl_harq[i].get_n_cce(), has_sr, pucch_sched.N_pucch_1);
      return true; 
    }
  }
  // If there is no Format1A/B, then check if it's expecting Format1
  if (has_sr) {
    if (prb_idx) {        
      for (int i=0;i<2;i++) {
        prb_idx[i] = srslte_pucch_n_prb(&cfg.pucch_cfg, SRSLTE_PUCCH_FORMAT_1, cfg.sr_N_pucch, cell.nof_prb, cell.cp, i); 
      }
    }
    Debug("SCHED: Reserved Format1 PUCCH for rnti=0x%x, n_prb=%d,%d, n_pucch=%d\n", rnti, prb_idx[0], prb_idx[1], cfg.sr_N_pucch);
    return true; 
  }
  // Finally check Format2 (periodic CQI)
  if (cfg.cqi_enabled && srslte_cqi_send(cfg.cqi_idx, current_tti)) {
    if (prb_idx) {
      for (int i=0;i<2;i++) {
        prb_idx[i] = srslte_pucch_n_prb(&cfg.pucch_cfg, SRSLTE_PUCCH_FORMAT_2, cfg.cqi_pucch, cell.nof_prb, cell.cp, i);
      }
    }
    Debug("SCHED: Reserved Format2 PUCCH for rnti=0x%x, n_prb=%d,%d, n_pucch=%d, pmi_idx=%d\n",
         rnti, prb_idx[0], prb_idx[1], cfg.cqi_pucch, cfg.cqi_idx);
    return true;
  }

  return false; 
}

int sched_ue::set_ack_info(uint32_t tti, bool ack)
{
  for (int i=0;i<SCHED_MAX_HARQ_PROC;i++) {
    if (((dl_harq[i].get_tti()+4)%10240) == tti) {
      Debug("SCHED: Set ACK=%d for rnti=0x%x, pid=%d, tti=%d\n", ack, rnti, i, tti);
      dl_harq[i].set_ack(ack); 
      return dl_harq[i].get_tbs();
    }
  }
  Warning("SCHED: Received ACK info for unknown TTI=%d\n", tti);
  return -1;
}

void sched_ue::ul_recv_len(uint32_t lcid, uint32_t len)
{
  // Remove PDCP header??
  if (len > 4) {
    len -= 4; 
  }
  if (lcid < sched_interface::MAX_LC) {
    if (bearer_is_ul(&lch[lcid])) {
      if (lch[lcid].bsr > (int) len) {
        lch[lcid].bsr -= len;
      } else {
        lch[lcid].bsr = 0; 
      }
    }
  }
  Debug("SCHED: recv_len=%d, lcid=%d, bsr={%d,%d,%d,%d}\n", len, lcid,
       lch[0].bsr, lch[1].bsr, lch[2].bsr, lch[3].bsr);
}

void sched_ue::set_ul_crc(uint32_t tti, bool crc_res)
{
  get_ul_harq(tti)->set_ack(crc_res);
}

void sched_ue::set_dl_cqi(uint32_t tti, uint32_t cqi)
{
  dl_cqi     = cqi; 
  dl_cqi_tti = tti; 
}

void sched_ue::set_ul_cqi(uint32_t tti, uint32_t cqi, uint32_t ul_ch_code)
{
  ul_cqi     = cqi; 
  ul_cqi_tti = tti; 
}

void sched_ue::tpc_inc() {
  if (power_headroom > 0) {
    next_tpc_pusch = 3;
    next_tpc_pucch = 3;    
  }
  log_h->info("SCHED: Set TCP=%d for rnti=0x%x\n", next_tpc_pucch, rnti);
}

void sched_ue::tpc_dec() {
  next_tpc_pusch = 0;
  next_tpc_pucch = 0;
  log_h->info("SCHED: Set TCP=%d for rnti=0x%x\n", next_tpc_pucch, rnti);
}

/*******************************************************
 * 
 * Functions used to generate DCI grants 
 * 
 *******************************************************/


// Generates a Format1 grant 
int sched_ue::generate_format1(dl_harq_proc *h, 
                         sched_interface::dl_sched_data_t *data, 
                         uint32_t tti, 
                         uint32_t cfi) 
{
  srslte_ra_dl_dci_t *dci = &data->dci;
  bzero(dci, sizeof(srslte_ra_dl_dci_t));
  
  uint32_t sf_idx = tti%10; 
  
  int mcs = 0; 
  int tbs = 0; 
  
  dci->alloc_type = SRSLTE_RA_ALLOC_TYPE0; 
  dci->type0_alloc.rbg_bitmask = h->get_rbgmask();
  
  
  // If this is the first transmission for this UE, make room for MAC Contention Resolution ID
  bool need_conres_ce = false; 
  if (is_first_dl_tx()) {
    need_conres_ce = true; 
  }
  if (h->is_empty()) {

    uint32_t req_bytes = get_pending_dl_new_data(tti); 
    
    uint32_t nof_prb = format1_count_prb(h->get_rbgmask(), cell.nof_prb);  
    srslte_ra_dl_grant_t grant; 
    srslte_ra_dl_dci_to_grant_prb_allocation(dci, &grant, cell.nof_prb);
    uint32_t nof_ctrl_symbols = cfi+(cell.nof_prb<10?1:0);
    uint32_t nof_re = srslte_ra_dl_grant_nof_re(&grant, cell, sf_idx, nof_ctrl_symbols);
    if (fixed_mcs_dl < 0) {
      tbs = alloc_tbs_dl(nof_prb, nof_re, req_bytes, &mcs);
    } else {
      tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(fixed_mcs_dl), nof_prb)/8;
      mcs = fixed_mcs_dl; 
    }

    h->new_tx(tti, mcs, tbs, data->dci_location.ncce);  
    
    Debug("SCHED: Alloc format1 new mcs=%d, tbs=%d, nof_prb=%d, req_bytes=%d\n", mcs, tbs, nof_prb, req_bytes);
  } else {
    h->new_retx(tti, &mcs, &tbs);  
    Debug("SCHED: Alloc format1 previous mcs=%d, tbs=%d\n", mcs, tbs);
  }
 
  // Allocate MAC ConRes CE
  if (need_conres_ce) {
    data->pdu[0].lcid = srslte::sch_subh::CON_RES_ID;
    data->nof_pdu_elems++;
    Info("SCHED: Added MAC Contention Resolution CE for rnti=0x%x\n", rnti);
  }
  
  int rem_tbs = tbs; 
  int x = 0; 
  do {
    x = alloc_pdu(rem_tbs, &data->pdu[data->nof_pdu_elems]); 
    rem_tbs -= x; 
    if (x) {
      data->nof_pdu_elems++;
    }
  } while(rem_tbs > 0 && x > 0);
  
  data->rnti    = rnti; 

  if (tbs > 0) {
    dci->harq_process = h->get_id(); 
    dci->mcs_idx      = mcs; 
    dci->rv_idx       = sched::get_rvidx(h->nof_retx()); 
    dci->ndi          = h->get_ndi(); 
    dci->tpc_pucch    = next_tpc_pucch; 
    next_tpc_pucch    = 1; 
    data->tbs         = tbs; 
    dci->tb_en[0]     = true; 
    dci->tb_en[1]     = false; 
  }  
  return tbs; 
}


int sched_ue::generate_format0(ul_harq_proc *h,  
                         sched_interface::ul_sched_data_t *data, 
                         uint32_t tti,
                         bool cqi_request) 
{
  srslte_ra_ul_dci_t *dci = &data->dci; 
  bzero(dci, sizeof(srslte_ra_ul_dci_t));
  
  int mcs = 0;   
  int tbs = 0; 
  
  ul_harq_proc::ul_alloc_t allocation = h->get_alloc();
  
  if (h->get_rar_mcs(&mcs)) {
    tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(mcs), allocation.L)/8;
    h->new_tx(tti, mcs, tbs); 
  } else if (h->is_empty()) {
    
    uint32_t req_bytes = get_pending_ul_new_data(tti); 
    
    uint32_t N_srs = 0; 
    uint32_t nof_re = (2*(SRSLTE_CP_NSYMB(cell.cp)-1) - N_srs)*allocation.L*SRSLTE_NRE;
    if (fixed_mcs_ul < 0) {
      tbs = alloc_tbs_ul(allocation.L, nof_re, req_bytes, &mcs);
    } else {
      tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(fixed_mcs_ul), allocation.L)/8;
      mcs = fixed_mcs_ul;
    }
    
    h->new_tx(tti, mcs, tbs);  

  } else {    
    h->new_retx(tti, &mcs, NULL);  
    tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(mcs), allocation.L)/8;
  }
  
  data->rnti = rnti; 
  data->tbs  = tbs; 
  
  if (tbs > 0) {
    dci->type2_alloc.L_crb = allocation.L;
    dci->type2_alloc.RB_start = allocation.RB_start;
    dci->mcs_idx     = mcs; 
    dci->rv_idx      = sched::get_rvidx(h->nof_retx()); 
    dci->ndi         = h->get_ndi(); 
    dci->cqi_request = cqi_request; 
    dci->freq_hop_fl = srslte_ra_ul_dci_t::SRSLTE_RA_PUSCH_HOP_DISABLED; 
    dci->tpc_pusch   = next_tpc_pusch; 
    next_tpc_pusch   = 1; 
  }
  
  return tbs; 
}

/*******************************************************
 * 
 * Functions used by scheduler or scheduler metric objects
 * 
 *******************************************************/

bool sched_ue::bearer_is_ul(ue_bearer_t *lch) {
  return lch->cfg.direction == sched_interface::ue_bearer_cfg_t::UL || lch->cfg.direction == sched_interface::ue_bearer_cfg_t::BOTH;
}

bool sched_ue::bearer_is_dl(ue_bearer_t *lch) {
  return lch->cfg.direction == sched_interface::ue_bearer_cfg_t::DL || lch->cfg.direction == sched_interface::ue_bearer_cfg_t::BOTH;
}

uint32_t sched_ue::get_max_retx() {
  return cfg.maxharq_tx; 
}

bool sched_ue::is_first_dl_tx()
{
  for (int i=0;i<SCHED_MAX_HARQ_PROC;i++) {
    if (dl_harq[i].nof_tx() > 0) {
      return false; 
    }
  }
  return true; 
}

bool sched_ue::needs_cqi(uint32_t tti, bool will_be_sent)
{  
  bool ret = false; 
  if (phy_config_dedicated_enabled && 
      cfg.aperiodic_cqi_period     && 
      get_pending_dl_new_data(tti) > 0) 
  {
    uint32_t interval = srslte_tti_interval(tti, dl_cqi_tti); 
    bool needscqi = interval >= cfg.aperiodic_cqi_period;
    if (needscqi) {    
      uint32_t interval_sent = srslte_tti_interval(tti, cqi_request_tti); 
      if (interval_sent >= 16) {
        if (will_be_sent) {
          cqi_request_tti = tti; 
        }
        Debug("SCHED: Needs_cqi, last_sent=%d, will_be_sent=%d\n", cqi_request_tti, will_be_sent);
        ret = true; 
      } 
    }
  }
  return ret; 
}

uint32_t sched_ue::get_pending_dl_new_data(uint32_t tti)
{
  uint32_t pending_data = 0; 
  for (int i=0;i<sched_interface::MAX_LC;i++) {
    if (bearer_is_dl(&lch[i])) {
      pending_data += lch[i].buf_retx + lch[i].buf_tx;
    }
  }
  return pending_data; 
}

uint32_t sched_ue::get_pending_ul_new_data(uint32_t tti)
{
  uint32_t pending_data = 0; 
  for (int i=0;i<sched_interface::MAX_LC;i++) {
    if (bearer_is_ul(&lch[i])) {
      pending_data += lch[i].bsr;
    }
  }
  if (!pending_data && is_sr_triggered()) {
    return 512; 
  }
  if (!pending_data && needs_cqi(tti)) {
    return 128; 
  }
  uint32_t pending_ul_data = get_pending_ul_old_data(); 
  if (pending_data > pending_ul_data) {
    pending_data -= pending_ul_data; 
  } else {
    pending_data = 0; 
  }
  if (pending_data) {
    Debug("SCHED: pending_data=%d, pending_ul_data=%d, bsr={%d,%d,%d,%d}\n", pending_data,pending_ul_data,
         lch[0].bsr, lch[1].bsr, lch[2].bsr, lch[3].bsr);
  }
  return pending_data; 
}

uint32_t sched_ue::get_pending_ul_old_data()
{
  uint32_t pending_data = 0; 
  for (int i=0;i<SCHED_MAX_HARQ_PROC;i++) {
    pending_data += ul_harq[i].get_pending_data();
  }
  return pending_data;
}


uint32_t sched_ue::get_required_prb_dl(uint32_t req_bytes, uint32_t nof_ctrl_symbols) 
{
  int mcs = 0; 
  uint32_t nbytes = 0; 
  uint32_t n = 0; 
  if (req_bytes == 0) {
    return 0; 
  }
  
  uint32_t nof_re = 0; 
  int tbs = 0; 
  for (n=1;n<=cell.nof_prb && nbytes < req_bytes;n++) {
    nof_re = srslte_ra_dl_approx_nof_re(cell, n, nof_ctrl_symbols);
    if (fixed_mcs_dl < 0) {
      tbs = alloc_tbs_dl(n, nof_re, 0, &mcs);
    } else {
      tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(fixed_mcs_dl), n)/8;
    }
    if (tbs > 0) {
      nbytes = tbs; 
    } else if (tbs < 0) {
      return 0; 
    }
  }
  return n; 
}

uint32_t sched_ue::get_required_prb_ul(uint32_t req_bytes) 
{
  int mcs = 0; 
  int tbs = 0; 
  uint32_t nbytes = 0; 
  uint32_t N_srs = 0; 
  
  uint32_t n = 0; 
  if (req_bytes == 0) {
    return 0; 
  }
  
  for (n=1;n<cell.nof_prb && nbytes < req_bytes + 4;n++) {
    uint32_t nof_re = (2*(SRSLTE_CP_NSYMB(cell.cp)-1) - N_srs)*n*SRSLTE_NRE;
    int tbs = 0; 
    if (fixed_mcs_ul < 0) {
      tbs = alloc_tbs_ul(n, nof_re, 0, &mcs);
    } else {
      tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(fixed_mcs_ul), n)/8;
    }
    if (tbs > 0) {
      nbytes = tbs; 
    }
  }

  while (!srslte_dft_precoding_valid_prb(n) && n<=cell.nof_prb) {
    n++;
  }
  
  return n; 
}

bool sched_ue::is_sr_triggered()
{
  return sr; 
}

/* Gets HARQ process with oldest pending retx */
dl_harq_proc* sched_ue::get_pending_dl_harq(uint32_t tti)
{
  int oldest_idx=-1; 
  uint32_t oldest_tti = 0; 
  for (int i=0;i<SCHED_MAX_HARQ_PROC;i++) {
    if (dl_harq[i].has_pending_retx(tti)) {
      uint32_t x = srslte_tti_interval(tti, dl_harq[i].get_tti()); 
      if (x > oldest_tti) {
        oldest_idx = i; 
        oldest_tti = x; 
      }
    }
  }
  if (oldest_idx >= 0) {
    return &dl_harq[oldest_idx]; 
  } else {
    return NULL; 
  }  
}

dl_harq_proc* sched_ue::get_empty_dl_harq()
{
  for (int i=0;i<SCHED_MAX_HARQ_PROC;i++) {
    if (dl_harq[i].is_empty()) {
      return &dl_harq[i]; 
    }
  }
  return NULL; 
}

ul_harq_proc* sched_ue::get_ul_harq(uint32_t tti)
{
  return &ul_harq[tti%SCHED_MAX_HARQ_PROC];
}

/* Find lowest DCI aggregation level supported by the UE spectral efficiency */
uint32_t sched_ue::get_aggr_level(uint32_t nof_bits)
{
  uint32_t l=0;
  float max_coderate = srslte_cqi_to_coderate(dl_cqi);
  float coderate = 99;
  float factor=1.5;
  uint32_t l_max = 3;
  if (cell.nof_prb == 6) {
    factor = 1.0;
    l_max  = 2;
  }
  do {
    coderate = srslte_pdcch_coderate(nof_bits, l);
    l++;
  } while(l<l_max && factor*coderate > max_coderate);
  Debug("SCHED: CQI=%d, l=%d, nof_bits=%d, coderate=%.2f, max_coderate=%.2f\n", dl_cqi, l, nof_bits, coderate, max_coderate);
  return l; 
}

sched_ue::sched_dci_cce_t* sched_ue::get_locations(uint32_t cfi, uint32_t sf_idx)
{
  if (cfi > 0 && cfi <= 3) {
    return &dci_locations[cfi-1][sf_idx];
  } else {
    Error("SCHED: Invalid CFI=%d\n", cfi);
    return &dci_locations[0][sf_idx]; 
  }    
}

/* Allocates first available RLC PDU */
int sched_ue::alloc_pdu(int tbs_bytes, sched_interface::dl_sched_pdu_t* pdu)
{
  // TODO: Implement lcid priority (now lowest index is lowest priority)
  int x = 0; 
  int i = 0; 
  for (i=0;i<sched_interface::MAX_LC && !x;i++) {
    if (lch[i].buf_retx) {
      x = SRSLTE_MIN(lch[i].buf_retx, tbs_bytes);
      lch[i].buf_retx -= x; 
    } else if (lch[i].buf_tx) {
      x = SRSLTE_MIN(lch[i].buf_tx, tbs_bytes);
      lch[i].buf_tx -= x; 
    }
  }
  if (x) {
    pdu->lcid   = i-1; 
    pdu->nbytes = x;  
    Debug("SCHED: Allocated lcid=%d, nbytes=%d, tbs_bytes=%d\n", pdu->lcid, pdu->nbytes, tbs_bytes);
  }
  return x; 
}

uint32_t sched_ue::format1_count_prb(uint32_t bitmask, uint32_t cell_nof_prb) {
  uint32_t P = srslte_ra_type0_P(cell_nof_prb);
  uint32_t nb = (int) ceilf((float) cell_nof_prb / P);
  
  uint32_t nof_prb = 0;   
  for (uint32_t i = 0; i < nb; i++) {
    if (bitmask & (1 << (nb - i - 1))) {
      for (uint32_t j = 0; j < P; j++) {
        if (i*P+j < cell_nof_prb) {
          nof_prb++;
        }
      }
    }
  }
  return nof_prb; 
}

int sched_ue::cqi_to_tbs(uint32_t cqi, uint32_t nof_prb, uint32_t nof_re, uint32_t max_mcs, uint32_t max_Qm, uint32_t *mcs) {
  float max_coderate = srslte_cqi_to_coderate(cqi);
  int sel_mcs = max_mcs+1; 
  float coderate = 99;
  float eff_coderate = 99;
  uint32_t Qm = 1;
  int tbs = 0; 

  do {
    sel_mcs--; 
    uint32_t tbs_idx = srslte_ra_tbs_idx_from_mcs(sel_mcs);
    tbs = srslte_ra_tbs_from_idx(tbs_idx, nof_prb);
    coderate = srslte_coderate(tbs, nof_re);
    Qm = SRSLTE_MIN(max_Qm, srslte_mod_bits_x_symbol(srslte_ra_mod_from_mcs(sel_mcs)));
    eff_coderate = coderate/Qm;
  } while((sel_mcs > 0 && coderate > max_coderate) || eff_coderate > 0.930);
  if (mcs) {
    *mcs = (uint32_t) sel_mcs; 
  }
  return tbs; 
}

int sched_ue::alloc_tbs_dl(uint32_t nof_prb,
                        uint32_t nof_re,
                        uint32_t req_bytes,
                        int *mcs)
{
  return alloc_tbs(nof_prb, nof_re, req_bytes, false, mcs);
}

int sched_ue::alloc_tbs_ul(uint32_t nof_prb,
                           uint32_t nof_re,
                           uint32_t req_bytes,
                           int *mcs)
{
  return alloc_tbs(nof_prb, nof_re, req_bytes, true, mcs);
}

  /* In this scheduler we tend to use all the available bandwidth and select the MCS
 * that approximates the minimum between the capacity and the requested rate 
 */
int sched_ue::alloc_tbs(uint32_t nof_prb,
                        uint32_t nof_re,
                        uint32_t req_bytes,
                        bool is_ul,
                        int *mcs)
{
  uint32_t sel_mcs = 0;

  uint32_t cqi     = is_ul?ul_cqi:dl_cqi;
  uint32_t max_mcs = is_ul?max_mcs_ul:max_mcs_dl;
  uint32_t max_Qm  = is_ul?4:6; // Allow 16-QAM in PUSCH Only

  // TODO: Compute real spectral efficiency based on PUSCH-UCI configuration
  if (has_pucch && is_ul) {
    cqi-=3;
  }

  int tbs = cqi_to_tbs(cqi, nof_prb, nof_re, max_mcs, max_Qm, &sel_mcs)/8;
  
  /* If less bytes are requested, lower the MCS */
  if (tbs > (int) req_bytes && req_bytes > 0) {
    uint32_t req_tbs_idx = srslte_ra_tbs_to_table_idx(req_bytes*8, nof_prb); 
    uint32_t req_mcs = srslte_ra_mcs_from_tbs_idx(req_tbs_idx);
    if (req_mcs < sel_mcs) {
      sel_mcs = req_mcs; 
      tbs = srslte_ra_tbs_from_idx(req_tbs_idx, nof_prb)/8;
    }
  }
  // Avoid the unusual case n_prb=1, mcs=6 tbs=328 (used in voip)
  if (nof_prb == 1 && sel_mcs == 6) {
    sel_mcs--;
  }

  if (mcs && tbs >= 0) {
    *mcs = (int) sel_mcs; 
  }
       
  return tbs; 
}


}
