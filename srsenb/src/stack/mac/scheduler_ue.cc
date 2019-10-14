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

#include <string.h>

#include "srsenb/hdr/stack/mac/scheduler.h"
#include "srsenb/hdr/stack/mac/scheduler_ue.h"
#include "srslte/common/pdu.h"
#include "srslte/srslte.h"

#define Error(fmt, ...)   log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...)    log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...)   log_h->debug(fmt, ##__VA_ARGS__)

#define MCS_FIRST_DL 4
#define MIN_DATA_TBS 4

/****************************************************** 
 *                  UE class                          *
 ******************************************************/

namespace srsenb {


/*******************************************************
 * 
 * Initialization and configuration functions 
 * 
 *******************************************************/

sched_ue::sched_ue() :
  has_pucch(false),
  power_headroom(0),
  rnti(0),
  max_mcs_dl(0),
  max_mcs_ul(0),
  fixed_mcs_ul(0),
  fixed_mcs_dl(0),
  phy_config_dedicated_enabled(false)
{
  log_h = NULL;

  bzero(&cell, sizeof(cell));
  bzero(&lch, sizeof(lch));
  bzero(&dci_locations, sizeof(dci_locations));
  bzero(&dl_harq, sizeof(dl_harq));
  bzero(&ul_harq, sizeof(ul_harq));
  bzero(&dl_ant_info, sizeof(dl_ant_info));

  reset();
}

void sched_ue::set_cfg(uint16_t                     rnti_,
                       sched_interface::ue_cfg_t*   cfg_,
                       sched_interface::cell_cfg_t* cell_cfg,
                       srslte_regs_t*               regs,
                       srslte::log*                 log_h_)
{
  reset();

  {
    std::lock_guard<std::mutex> lock(mutex);
    rnti  = rnti_;
    log_h = log_h_;
    memcpy(&cell, &cell_cfg->cell, sizeof(srslte_cell_t));
    P = srslte_ra_type0_P(cell.nof_prb);

    max_mcs_dl   = 28;
    max_mcs_ul   = 28;
    max_msg3retx = cell_cfg->maxharq_msg3tx;

    cfg = *cfg_;

    // Initialize TM
    cfg.dl_cfg.tm = SRSLTE_TM1;

    Info("SCHED: Added user rnti=0x%x\n", rnti);
    // Config HARQ processes
    for (int i = 0; i < SCHED_MAX_HARQ_PROC; i++) {
      dl_harq[i].config(i, cfg.maxharq_tx, log_h);
      ul_harq[i].config(i, cfg.maxharq_tx, log_h);
    }

    // Generate allowed CCE locations
    for (int cfi = 0; cfi < 3; cfi++) {
      for (int sf_idx = 0; sf_idx < 10; sf_idx++) {
        sched::generate_cce_location(regs, &dci_locations[cfi][sf_idx], cfi + 1, sf_idx, rnti);
      }
    }
  }

  for (int i=0;i<sched_interface::MAX_LC;i++) {
    set_bearer_cfg(i, &cfg.ue_bearers[i]);
  }

}

void sched_ue::reset()
{
  {
    std::lock_guard<std::mutex> lock(mutex);
    bzero(&cfg, sizeof(sched_interface::ue_cfg_t));
    sr                           = false;
    next_tpc_pusch               = 1;
    next_tpc_pucch               = 1;
    buf_mac                      = 0;
    buf_ul                       = 0;
    phy_config_dedicated_enabled = false;
    dl_cqi                       = 1;
    ul_cqi                       = 1;
    dl_cqi_tti                   = 0;
    ul_cqi_tti                   = 0;
    dl_ri                        = 0;
    dl_ri_tti                    = 0;
    dl_pmi                       = 0;
    dl_pmi_tti                   = 0;
    cqi_request_tti              = 0;
    for (int i = 0; i < SCHED_MAX_HARQ_PROC; i++) {
      for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
        dl_harq[i].reset(tb);
        ul_harq[i].reset(tb);
      }
    }
  }

  for (int i=0;i<sched_interface::MAX_LC; i++) {
    rem_bearer(i);
  }
}

void sched_ue::set_fixed_mcs(int mcs_ul, int mcs_dl) {
  std::lock_guard<std::mutex> lock(mutex);
  fixed_mcs_ul = mcs_ul;
  fixed_mcs_dl = mcs_dl;
}

void sched_ue::set_max_mcs(int mcs_ul, int mcs_dl) {
  std::lock_guard<std::mutex> lock(mutex);
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
  std::lock_guard<std::mutex> lock(mutex);
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
  std::lock_guard<std::mutex> lock(mutex);
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
  std::lock_guard<std::mutex> lock(mutex);
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
  power_headroom = phr;
}

void sched_ue::dl_buffer_state(uint8_t lc_id, uint32_t tx_queue, uint32_t retx_queue)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (lc_id < sched_interface::MAX_LC) {
    lch[lc_id].buf_retx = retx_queue;
    lch[lc_id].buf_tx   = tx_queue;
    Debug("SCHED: DL lcid=%d buffer_state=%d,%d\n", lc_id, tx_queue, retx_queue);
  }
}

void sched_ue::mac_buffer_state(uint32_t ce_code)
{
  std::lock_guard<std::mutex> lock(mutex);
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
  } else {
    if (cfg.pucch_cfg.sr_configured && srslte_ue_ul_sr_send_tti(&cfg.pucch_cfg, current_tti)) {
      return (n_cce + cfg.pucch_cfg.N_pucch_1) == cfg.pucch_cfg.n_pucch_sr;
    } else {
      return false;
    }
  }
}

bool sched_ue::get_pucch_sched(uint32_t current_tti, uint32_t prb_idx[2])
{
  bool ret = false;

  std::lock_guard<std::mutex> lock(mutex);

  if (phy_config_dedicated_enabled) {

    // Configure expected UCI for this TTI
    ZERO_OBJECT(cfg.pucch_cfg.uci_cfg);

    // SR
    cfg.pucch_cfg.uci_cfg.is_scheduling_request_tti = srslte_ue_ul_sr_send_tti(&cfg.pucch_cfg, current_tti);

    ret |= cfg.pucch_cfg.uci_cfg.is_scheduling_request_tti;

    // Pending ACKs
    for (int i = 0; i < SCHED_MAX_HARQ_PROC; i++) {
      if (TTI_TX(dl_harq[i].get_tti()) == current_tti) {
        cfg.pucch_cfg.uci_cfg.ack[0].ncce[0]  = dl_harq[i].get_n_cce();
        cfg.pucch_cfg.uci_cfg.ack[0].nof_acks = 1;
        ret                                = true;
      }
    }
    // Periodic CQI
    if (srslte_enb_dl_gen_cqi_periodic(&cell, &cfg.dl_cfg, current_tti, 1, &cfg.pucch_cfg.uci_cfg.cqi)) {
      ret = true;
    }

    // Compute PRB index
    if (prb_idx) {
      for (int j = 0; j < 2; j++) {
        prb_idx[j] = srslte_enb_ul_get_pucch_prb_idx(&cell, &cfg.pucch_cfg, j);
      }
      Debug("SCHED: Reserved %s PUCCH for rnti=0x%x, n_prb=%d,%d, n_pucch=%d, ncce=%d, has_sr=%d\n",
            srslte_pucch_format_text(cfg.pucch_cfg.format),
            rnti,
            prb_idx[0],
            prb_idx[1],
            cfg.pucch_cfg.n_pucch,
            cfg.pucch_cfg.uci_cfg.ack[0].ncce[0],
            cfg.pucch_cfg.uci_cfg.is_scheduling_request_tti);
    }
  }

  return ret;
}

int sched_ue::set_ack_info(uint32_t tti, uint32_t tb_idx, bool ack)
{
  std::lock_guard<std::mutex> lock(mutex);
  int ret = -1;
  for (int i=0;i<SCHED_MAX_HARQ_PROC;i++) {
    if (TTI_TX(dl_harq[i].get_tti()) == tti) {
      Debug("SCHED: Set ACK=%d for rnti=0x%x, pid=%d, tb=%d, tti=%d\n", ack, rnti, i, tb_idx, tti);
      dl_harq[i].set_ack(tb_idx, ack);
      ret = dl_harq[i].get_tbs(tb_idx);
      goto unlock;
    }
  }

  Warning("SCHED: Received ACK info for unknown TTI=%d\n", tti);
  ret = -1;

unlock:
  return ret;
}

void sched_ue::ul_recv_len(uint32_t lcid, uint32_t len)
{
  std::lock_guard<std::mutex> lock(mutex);

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
  std::lock_guard<std::mutex> lock(mutex);
  get_ul_harq(tti)->set_ack(0, crc_res);
}

void sched_ue::set_dl_ri(uint32_t tti, uint32_t ri)
{
  std::lock_guard<std::mutex> lock(mutex);
  dl_ri     = ri;
  dl_ri_tti = tti;
}

void sched_ue::set_dl_pmi(uint32_t tti, uint32_t pmi)
{
  std::lock_guard<std::mutex> lock(mutex);
  dl_pmi     = pmi;
  dl_pmi_tti = tti;
}

void sched_ue::set_dl_cqi(uint32_t tti, uint32_t cqi)
{
  std::lock_guard<std::mutex> lock(mutex);
  dl_cqi     = cqi;
  dl_cqi_tti = tti;
}

void sched_ue::set_dl_ant_info(asn1::rrc::phys_cfg_ded_s::ant_info_c_* d)
{
  std::lock_guard<std::mutex> lock(mutex);
  dl_ant_info = *d;
}

void sched_ue::set_ul_cqi(uint32_t tti, uint32_t cqi, uint32_t ul_ch_code)
{
  std::lock_guard<std::mutex> lock(mutex);
  ul_cqi     = cqi;
  ul_cqi_tti = tti;
}

void sched_ue::tpc_inc() {
  std::lock_guard<std::mutex> lock(mutex);
  if (power_headroom > 0) {
    next_tpc_pusch = 3;
    next_tpc_pucch = 3;    
  }
  log_h->info("SCHED: Set TCP=%d for rnti=0x%x\n", next_tpc_pucch, rnti);
}

void sched_ue::tpc_dec() {
  std::lock_guard<std::mutex> lock(mutex);
  next_tpc_pusch = 0;
  next_tpc_pucch = 0;
  log_h->info("SCHED: Set TCP=%d for rnti=0x%x\n", next_tpc_pucch, rnti);
}

/*******************************************************
 * 
 * Functions used to generate DCI grants 
 * 
 *******************************************************/

// Generates a Format1 dci
// > return 0 if TBS<MIN_DATA_TBS
int sched_ue::generate_format1(
    dl_harq_proc* h, sched_interface::dl_sched_data_t* data, uint32_t tti, uint32_t cfi, const rbgmask_t& user_mask)
{
  std::lock_guard<std::mutex> lock(mutex);

  srslte_dci_dl_t* dci = &data->dci;

  int mcs = 0;
  int tbs = 0;

  dci->alloc_type              = SRSLTE_RA_ALLOC_TYPE0;
  dci->type0_alloc.rbg_bitmask = (uint32_t)user_mask.to_uint64();

  // If this is the first transmission for this UE, make room for MAC Contention Resolution ID
  bool need_conres_ce = false;
  if (is_first_dl_tx()) {
    need_conres_ce = true;
  }
  if (h->is_empty(0)) {

    // Get total available data to transmit (includes MAC header)
    uint32_t req_bytes = get_pending_dl_new_data_total_unlocked(tti);

    uint32_t nof_prb = format1_count_prb((uint32_t)user_mask.to_uint64(), cell.nof_prb);

    // Calculate exact number of RE for this PRB allocation
    srslte_pdsch_grant_t grant = {};
    srslte_dl_sf_cfg_t   dl_sf = {};
    dl_sf.cfi                  = cfi;
    dl_sf.tti                  = tti;
    srslte_ra_dl_grant_to_grant_prb_allocation(dci, &grant, cell.nof_prb);
    uint32_t nof_re = srslte_ra_dl_grant_nof_re(&cell, &dl_sf, &grant);

    int mcs0 = fixed_mcs_dl;
    if (need_conres_ce and cell.nof_prb < 10) { // SRB0 Tx. Use a higher MCS for the PRACH to fit in 6 PRBs
      mcs0 = MCS_FIRST_DL;
    }
    if (mcs0 < 0) { // dynamic MCS
      tbs = alloc_tbs_dl(nof_prb, nof_re, req_bytes, &mcs);
    } else {
      tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(mcs0, false), nof_prb) / 8;
      mcs = mcs0;
    }

    if (tbs < MIN_DATA_TBS) {
      log_h->warning("SCHED: Allocation of TBS=%d that does not account header\n", tbs);
      return 0;
    }

    h->new_tx(user_mask, 0, tti, mcs, tbs, data->dci.location.ncce);

    // Allocate MAC ConRes CE
    if (need_conres_ce) {
      data->pdu[0][0].lcid = srslte::sch_subh::CON_RES_ID;
      data->nof_pdu_elems[0]++;
      Info("SCHED: Added MAC Contention Resolution CE for rnti=0x%x\n", rnti);
    }

    int rem_tbs = tbs;
    int x       = 0;
    do {
      x = alloc_pdu(rem_tbs, &data->pdu[0][data->nof_pdu_elems[0]]);
      if (x) {
        rem_tbs -= x + 2; // count 2-byte header
        data->nof_pdu_elems[0]++;
      }
    } while (rem_tbs >= MIN_DATA_TBS && x > 0);

    Debug("SCHED: Alloc format1 new mcs=%d, tbs=%d, nof_prb=%d, req_bytes=%d\n", mcs, tbs, nof_prb, req_bytes);
  } else {
    h->new_retx(user_mask, 0, tti, &mcs, &tbs, data->dci.location.ncce);
    Debug("SCHED: Alloc format1 previous mcs=%d, tbs=%d\n", mcs, tbs);
  }

  if (tbs > 0) {
    dci->rnti          = rnti;
    dci->pid           = h->get_id();
    dci->tb[0].mcs_idx = (uint32_t)mcs;
    dci->tb[0].rv      = sched::get_rvidx(h->nof_retx(0));
    dci->tb[0].ndi     = h->get_ndi(0);

    dci->tpc_pucch = (uint8_t)next_tpc_pucch;
    next_tpc_pucch = 1;
    data->tbs[0]   = (uint32_t)tbs;
    data->tbs[1]   = 0;

    dci->format = SRSLTE_DCI_FORMAT1;
  }
  return tbs;
}

// Generates a Format2a dci
int sched_ue::generate_format2a(
    dl_harq_proc* h, sched_interface::dl_sched_data_t* data, uint32_t tti, uint32_t cfi, const rbgmask_t& user_mask)
{
  std::lock_guard<std::mutex> lock(mutex);
  int ret = generate_format2a_unlocked(h, data, tti, cfi, user_mask);
  return ret;
}

// Generates a Format2a dci
int sched_ue::generate_format2a_unlocked(
    dl_harq_proc* h, sched_interface::dl_sched_data_t* data, uint32_t tti, uint32_t cfi, const rbgmask_t& user_mask)
{
  bool tb_en[SRSLTE_MAX_TB] = {false};

  srslte_dci_dl_t* dci = &data->dci;

  dci->alloc_type              = SRSLTE_RA_ALLOC_TYPE0;
  dci->type0_alloc.rbg_bitmask = (uint32_t)user_mask.to_uint64();

  uint32_t nof_prb = format1_count_prb((uint32_t)user_mask.to_uint64(), cell.nof_prb); // FIXME: format1???

  // Calculate exact number of RE for this PRB allocation
  srslte_pdsch_grant_t grant = {};
  srslte_dl_sf_cfg_t   dl_sf = {};
  dl_sf.cfi                  = cfi;
  dl_sf.tti                  = tti;
  srslte_ra_dl_grant_to_grant_prb_allocation(dci, &grant, cell.nof_prb);
  uint32_t nof_re = srslte_ra_dl_grant_nof_re(&cell, &dl_sf, &grant);

  bool no_retx = true;

  if (dl_ri == 0) {
    if (h->is_empty(1)) {
      /* One layer, tb1 buffer is empty, send tb0 only */
      tb_en[0] = true;
    } else {
      /* One layer, tb1 buffer is not empty, send tb1 only */
      tb_en[1] = true;
    }
  } else {
    /* Two layers, retransmit what TBs that have not been Acknowledged */
    for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
      if (!h->is_empty(tb)) {
        tb_en[tb] = true;
        no_retx = false;
      }
    }
    /* Two layers, no retransmissions...  */
    if (no_retx) {
      tb_en[0] = true;
      tb_en[1] = true;
    }
  }

  for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
    uint32_t req_bytes = get_pending_dl_new_data_total_unlocked(tti);
    int mcs = 0;
    int tbs = 0;

    if (!h->is_empty(tb)) {
      h->new_retx(user_mask, tb, tti, &mcs, &tbs, data->dci.location.ncce);
      Debug("SCHED: Alloc format2/2a previous mcs=%d, tbs=%d\n", mcs, tbs);
    } else if (tb_en[tb] && req_bytes && no_retx) {
      if (fixed_mcs_dl < 0) {
        tbs = alloc_tbs_dl(nof_prb, nof_re, req_bytes, &mcs);
      } else {
        tbs = srslte_ra_tbs_from_idx((uint32_t)srslte_ra_tbs_idx_from_mcs((uint32_t)fixed_mcs_dl, false), nof_prb) / 8;
        mcs = fixed_mcs_dl;
      }
      h->new_tx(user_mask, tb, tti, mcs, tbs, data->dci.location.ncce);

      int rem_tbs = tbs;
      int x       = 0;
      do {
        x = alloc_pdu(rem_tbs, &data->pdu[tb][data->nof_pdu_elems[tb]]);
        rem_tbs -= x;
        if (x) {
          data->nof_pdu_elems[tb]++;
        }
      } while (rem_tbs >= MIN_DATA_TBS && x > 0);

      Debug("SCHED: Alloc format2/2a new mcs=%d, tbs=%d, nof_prb=%d, req_bytes=%d\n", mcs, tbs, nof_prb, req_bytes);
    }

    /* Fill DCI TB dedicated fields */
    if (tbs > 0 && tb_en[tb]) {
      dci->tb[tb].mcs_idx = (uint32_t)mcs;
      dci->tb[tb].rv      = sched::get_rvidx(h->nof_retx(tb));
      if (!SRSLTE_DCI_IS_TB_EN(dci->tb[tb])) {
        dci->tb[tb].rv = 2;
      }
      dci->tb[tb].ndi    = h->get_ndi(tb);
      dci->tb[tb].cw_idx = tb;
      data->tbs[tb]      = (uint32_t)tbs;
    } else {
      SRSLTE_DCI_TB_DISABLE(dci->tb[tb]);
      data->tbs[tb] = 0;
    }
  }

  /* Fill common fields */
  dci->format    = SRSLTE_DCI_FORMAT2A;
  dci->rnti      = rnti;
  dci->pid       = h->get_id();
  dci->tpc_pucch = (uint8_t) next_tpc_pucch;
  next_tpc_pucch = 1;

  int ret = data->tbs[0] + data->tbs[1];
  return ret;
}

// Generates a Format2 dci
int sched_ue::generate_format2(
    dl_harq_proc* h, sched_interface::dl_sched_data_t* data, uint32_t tti, uint32_t cfi, const rbgmask_t& user_mask)
{

  std::lock_guard<std::mutex> lock(mutex);

  /* Call Format 2a (common) */
  int ret = generate_format2a_unlocked(h, data, tti, cfi, user_mask);

  /* Compute precoding information */
  data->dci.format = SRSLTE_DCI_FORMAT2;
  if ((SRSLTE_DCI_IS_TB_EN(data->dci.tb[0]) + SRSLTE_DCI_IS_TB_EN(data->dci.tb[1])) == 1) {
    data->dci.pinfo = (uint8_t) (dl_pmi + 1) % (uint8_t) 5;
  } else {
    data->dci.pinfo = (uint8_t) (dl_pmi & 1);
  }

  return ret;
}

int sched_ue::generate_format0(sched_interface::ul_sched_data_t* data,
                               uint32_t                          tti,
                               ul_harq_proc::ul_alloc_t          alloc,
                               bool                              needs_pdcch,
                               srslte_dci_location_t             dci_pos,
                               int                               explicit_mcs)
{
  std::lock_guard<std::mutex> lock(mutex);

  ul_harq_proc*    h   = get_ul_harq(tti);
  srslte_dci_ul_t* dci = &data->dci;

  bool cqi_request = needs_cqi_unlocked(tti, true);

  // Set DCI position
  data->needs_pdcch = needs_pdcch;
  dci->location     = dci_pos;

  int mcs = (explicit_mcs >= 0) ? explicit_mcs : fixed_mcs_ul;
  int tbs = 0;

  bool is_newtx = h->is_empty(0);
  if (is_newtx) {
    uint32_t nof_retx;

    // If Msg3 set different nof retx
    nof_retx = (data->needs_pdcch) ? get_max_retx() : max_msg3retx;

    if (mcs >= 0) {
      tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(mcs, true), alloc.L) / 8;
    } else {
      // dynamic mcs
      uint32_t req_bytes = get_pending_ul_new_data_unlocked(tti);
      uint32_t N_srs     = 0;
      uint32_t nof_re    = (2 * (SRSLTE_CP_NSYMB(cell.cp) - 1) - N_srs) * alloc.L * SRSLTE_NRE;
      tbs                = alloc_tbs_ul(alloc.L, nof_re, req_bytes, &mcs);
    }
    h->new_tx(tti, mcs, tbs, alloc, nof_retx);

  } else {
    // retx
    h->new_retx(0, tti, &mcs, NULL, alloc);
    tbs      = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(mcs, true), alloc.L) / 8;
  }

  data->tbs = tbs;

  if (tbs > 0) {
    dci->rnti            = rnti;
    dci->format          = SRSLTE_DCI_FORMAT0;
    dci->type2_alloc.riv = srslte_ra_type2_to_riv(alloc.L, alloc.RB_start, cell.nof_prb);
    dci->tb.rv           = sched::get_rvidx(h->nof_retx(0));
    if (!is_newtx && h->is_adaptive_retx()) {
      dci->tb.mcs_idx = 28 + dci->tb.rv;
    } else {
      dci->tb.mcs_idx = mcs;
    }
    dci->tb.ndi      = h->get_ndi(0);
    dci->cqi_request = cqi_request;
    dci->freq_hop_fl = srslte_dci_ul_t::SRSLTE_RA_PUSCH_HOP_DISABLED;
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
    if (dl_harq[i].nof_tx(0) > 0) {
      return false; 
    }
  }
  return true; 
}

bool sched_ue::needs_cqi(uint32_t tti, bool will_be_sent)
{
  std::lock_guard<std::mutex> lock(mutex);
  bool ret = needs_cqi_unlocked(tti, will_be_sent);
  return ret;
}

// Private lock-free implemenentation
bool sched_ue::needs_cqi_unlocked(uint32_t tti, bool will_be_sent)
{
  bool ret = false;
  if (phy_config_dedicated_enabled &&
      cfg.aperiodic_cqi_period     &&
      get_pending_dl_new_data_unlocked(tti) > 0)
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
  std::lock_guard<std::mutex> lock(mutex);
  uint32_t pending_data = get_pending_dl_new_data_unlocked(tti);
  return pending_data;
}

/// Use this function in the dl-metric to get the bytes to be scheduled. It accounts for the UE data,
/// the RAR resources, and headers
/// \param tti
/// \return number of bytes to be allocated
uint32_t sched_ue::get_pending_dl_new_data_total(uint32_t tti)
{
  std::lock_guard<std::mutex> lock(mutex);
  uint32_t req_bytes = get_pending_dl_new_data_total_unlocked(tti);
  return req_bytes;
}

uint32_t sched_ue::get_pending_dl_new_data_total_unlocked(uint32_t tti)
{
  uint32_t req_bytes = get_pending_dl_new_data_unlocked(tti);
  if(req_bytes>0) {
    req_bytes += (req_bytes < 128) ? 2 : 3; // consider the header
    if(is_first_dl_tx()) {
      req_bytes += 6; // count for RAR
    }
  }
  return req_bytes;
}

// Private lock-free implementation
uint32_t sched_ue::get_pending_dl_new_data_unlocked(uint32_t tti)
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
  std::lock_guard<std::mutex> lock(mutex);
  uint32_t pending_data = get_pending_ul_new_data_unlocked(tti);
  return pending_data;
}

uint32_t sched_ue::get_pending_ul_old_data()
{
  std::lock_guard<std::mutex> lock(mutex);
  uint32_t pending_data = get_pending_ul_old_data_unlocked();
  return pending_data;
}

// Private lock-free implementation
uint32_t sched_ue::get_pending_ul_new_data_unlocked(uint32_t tti)
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
  if (!pending_data && needs_cqi_unlocked(tti)) {
    return 128;
  }
  uint32_t pending_ul_data = get_pending_ul_old_data_unlocked();
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

// Private lock-free implementation
uint32_t sched_ue::get_pending_ul_old_data_unlocked()
{
  uint32_t pending_data = 0;
  for (int i=0;i<SCHED_MAX_HARQ_PROC;i++) {
    pending_data += ul_harq[i].get_pending_data();
  }
  return pending_data;
}

uint32_t sched_ue::prb_to_rbg(uint32_t nof_prb)
{
  return (uint32_t) ceil((float) nof_prb / P);
}

uint32_t sched_ue::rgb_to_prb(uint32_t nof_rbg)
{
  return P*nof_rbg;
}

uint32_t sched_ue::get_required_prb_dl(uint32_t req_bytes, uint32_t nof_ctrl_symbols)
{
  std::lock_guard<std::mutex> lock(mutex);

  int      mcs    = 0;
  uint32_t nof_re = 0;
  int      tbs    = 0;

  uint32_t nbytes = 0;
  uint32_t n;
  int      mcs0 = (is_first_dl_tx() and cell.nof_prb == 6) ? MCS_FIRST_DL : fixed_mcs_dl;
  for (n=0; n < cell.nof_prb && nbytes < req_bytes; ++n) {
    nof_re = srslte_ra_dl_approx_nof_re(&cell, n + 1, nof_ctrl_symbols);
    if (mcs0 < 0) {
      tbs = alloc_tbs_dl(n+1, nof_re, 0, &mcs);
    } else {
      tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(mcs0, false), n + 1) / 8;
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
  int      mcs    = 0;
  uint32_t nbytes = 0;
  uint32_t N_srs = 0;

  uint32_t n = 0;
  if (req_bytes == 0) {
    return 0; 
  }

  std::lock_guard<std::mutex> lock(mutex);

  for (n = 1; n < cell.nof_prb && nbytes < req_bytes + 4; n++) {
    uint32_t nof_re = (2*(SRSLTE_CP_NSYMB(cell.cp)-1) - N_srs)*n*SRSLTE_NRE;
    int tbs = 0; 
    if (fixed_mcs_ul < 0) {
      tbs = alloc_tbs_ul(n, nof_re, 0, &mcs);
    } else {
      tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(fixed_mcs_ul, true), n) / 8;
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

void sched_ue::reset_pending_pids(uint32_t tti_rx)
{
  uint32_t tti_tx_dl = TTI_TX(tti_rx), tti_tx_ul = TTI_RX_ACK(tti_rx);

  // UL harqs
  get_ul_harq(tti_tx_ul)->reset_pending_data();

  // DL harqs
  for (int i=0;i<SCHED_MAX_HARQ_PROC;i++) {
    dl_harq[i].reset_pending_data();
    if (not dl_harq[i].is_empty()) {
      uint32_t tti_diff = srslte_tti_interval(tti_tx_dl, dl_harq[i].get_tti());
      if (tti_diff > 50 and tti_diff < 10240 / 2) {
        log_h->info("SCHED: pid=%d is old. tti_pid=%d, now is %d, resetting\n", i, dl_harq[i].get_tti(), tti_tx_dl);
        dl_harq[i].reset(0);
        dl_harq[i].reset(1);
      }
    }
  }
}

/* Gets HARQ process with oldest pending retx */
dl_harq_proc* sched_ue::get_pending_dl_harq(uint32_t tti)
{
#if ASYNC_DL_SCHED

  std::lock_guard<std::mutex> lock(mutex);

  int oldest_idx=-1;
  uint32_t oldest_tti = 0; 
  for (int i=0;i<SCHED_MAX_HARQ_PROC;i++) {
    if (dl_harq[i].has_pending_retx(0, tti) || dl_harq[i].has_pending_retx(1, tti)) {
      uint32_t x = srslte_tti_interval(tti, dl_harq[i].get_tti());
      if (x > oldest_tti) {
        oldest_idx = i; 
        oldest_tti = x; 
      }
    }
  }
  dl_harq_proc *h = NULL;
  if (oldest_idx >= 0) {
    h = &dl_harq[oldest_idx];
  }

  return h;

#else
  return &dl_harq[tti%SCHED_MAX_HARQ_PROC];
#endif
}

dl_harq_proc* sched_ue::get_empty_dl_harq()
{
  std::lock_guard<std::mutex> lock(mutex);

  dl_harq_proc *h = NULL;
  for (int i=0;i<SCHED_MAX_HARQ_PROC && !h;i++) {
    if (dl_harq[i].is_empty(0) && dl_harq[i].is_empty(1)) {
      h = &dl_harq[i];
    }
  }
  return h;
}

ul_harq_proc* sched_ue::get_ul_harq(uint32_t tti)
{
  return &ul_harq[tti % SCHED_MAX_HARQ_PROC];
}

dl_harq_proc* sched_ue::find_dl_harq(uint32_t tti)
{
  for (uint32_t i = 0; i < SCHED_MAX_HARQ_PROC; ++i) {
    if (dl_harq[i].get_tti() == tti) {
      return &dl_harq[i];
    }
  }
  return nullptr;
}

dl_harq_proc* sched_ue::get_dl_harq(uint32_t idx)
{
  return &dl_harq[idx];
}

srslte_dci_format_t sched_ue::get_dci_format() {
  srslte_dci_format_t ret = SRSLTE_DCI_FORMAT1;

  if (phy_config_dedicated_enabled) {
    /* FIXME: Assumes UE-Specific Search Space (Not common) */
    switch (dl_ant_info.explicit_value().tx_mode) {
      case asn1::rrc::ant_info_ded_s::tx_mode_e_::tm1:
      case asn1::rrc::ant_info_ded_s::tx_mode_e_::tm2:
        ret = SRSLTE_DCI_FORMAT1;
        break;
      case asn1::rrc::ant_info_ded_s::tx_mode_e_::tm3:
        ret = SRSLTE_DCI_FORMAT2A;
        break;
      case asn1::rrc::ant_info_ded_s::tx_mode_e_::tm4:
        ret = SRSLTE_DCI_FORMAT2;
        break;
      case asn1::rrc::ant_info_ded_s::tx_mode_e_::tm5:
      case asn1::rrc::ant_info_ded_s::tx_mode_e_::tm6:
      case asn1::rrc::ant_info_ded_s::tx_mode_e_::tm7:
      case asn1::rrc::ant_info_ded_s::tx_mode_e_::tm8_v920:
      default:
        Warning("Incorrect transmission mode (rnti=%04x; tm=%s)\n",
                rnti,
                dl_ant_info.explicit_value().tx_mode.to_string().c_str());
    }
  }

  return ret;
}


/* Find lowest DCI aggregation level supported by the UE spectral efficiency */
uint32_t sched_ue::get_aggr_level(uint32_t nof_bits)
{
  std::lock_guard<std::mutex> lock(mutex);
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

int sched_ue::cqi_to_tbs(uint32_t cqi, uint32_t nof_prb, uint32_t nof_re, uint32_t max_mcs, uint32_t max_Qm, bool is_ul,
                         uint32_t* mcs)
{
  float max_coderate = srslte_cqi_to_coderate(cqi);
  int      sel_mcs      = max_mcs + 1;
  float coderate = 99;
  float    eff_coderate = 99;
  uint32_t Qm           = 1;
  int      tbs          = 0;

  do {
    sel_mcs--;
    uint32_t tbs_idx = srslte_ra_tbs_idx_from_mcs(sel_mcs, is_ul);
    tbs              = srslte_ra_tbs_from_idx(tbs_idx, nof_prb);
    coderate         = srslte_coderate(tbs, nof_re);
    srslte_mod_t mod = (is_ul) ? srslte_ra_ul_mod_from_mcs(sel_mcs) : srslte_ra_dl_mod_from_mcs(sel_mcs);
    Qm               = SRSLTE_MIN(max_Qm, srslte_mod_bits_x_symbol(mod));
    eff_coderate = coderate/Qm;
  } while((sel_mcs > 0 && coderate > max_coderate) || eff_coderate > 0.930);
  if (mcs) {
    *mcs = (uint32_t)sel_mcs;
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
  uint32_t max_Qm  = is_ul ? 4 : 6; // Allow 16-QAM in PUSCH Only

  // TODO: Compute real spectral efficiency based on PUSCH-UCI configuration
  int tbs_bytes = cqi_to_tbs(cqi, nof_prb, nof_re, max_mcs, max_Qm, is_ul, &sel_mcs) / 8;

  /* If less bytes are requested, lower the MCS */
  if (tbs_bytes > (int)req_bytes && req_bytes > 0) {
    int req_tbs_idx = srslte_ra_tbs_to_table_idx(req_bytes * 8, nof_prb);
    int req_mcs     = srslte_ra_mcs_from_tbs_idx(req_tbs_idx, is_ul);

    if (req_mcs < (int)sel_mcs) {
      sel_mcs   = req_mcs;
      tbs_bytes = srslte_ra_tbs_from_idx(req_tbs_idx, nof_prb) / 8;
    }
  }
  // Avoid the unusual case n_prb=1, mcs=6 tbs=328 (used in voip)
  if (nof_prb == 1 && sel_mcs == 6) {
    sel_mcs--;
    uint32_t tbs_idx = srslte_ra_tbs_idx_from_mcs(sel_mcs, is_ul);
    tbs_bytes        = srslte_ra_tbs_from_idx(tbs_idx, nof_prb) / 8;
  }

  if (mcs && tbs_bytes >= 0) {
    *mcs = (int) sel_mcs; 
  }

  return tbs_bytes;
}


}
