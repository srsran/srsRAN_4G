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
#include "srslte/common/logmap.h"
#include "srslte/common/pdu.h"
#include "srslte/srslte.h"

#define Error(fmt, ...) log_h->error(fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) log_h->warning(fmt, ##__VA_ARGS__)
#define Info(fmt, ...) log_h->info(fmt, ##__VA_ARGS__)
#define Debug(fmt, ...) log_h->debug(fmt, ##__VA_ARGS__)

#define MCS_FIRST_DL 4

/******************************************************
 *                  UE class                          *
 ******************************************************/

namespace srsenb {

constexpr uint32_t conres_ce_size = 6;

/******************************************************
 *                 Helper Functions                   *
 ******************************************************/

namespace sched_utils {

uint32_t get_tbs_bytes(uint32_t mcs, uint32_t nof_alloc_prb, bool is_ul)
{
  return srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(mcs, is_ul), nof_alloc_prb);
}

} // namespace sched_utils

bool operator==(const sched_interface::ue_cfg_t::cc_cfg_t& lhs, const sched_interface::ue_cfg_t::cc_cfg_t& rhs)
{
  return lhs.enb_cc_idx == rhs.enb_cc_idx and lhs.active == rhs.active;
}

/*******************************************************
 *
 * Initialization and configuration functions
 *
 *******************************************************/

sched_ue::sched_ue() : log_h(srslte::logmap::get("MAC "))
{
  reset();
}

void sched_ue::init(uint16_t rnti_, const std::vector<sched_cell_params_t>& cell_list_params_)
{
  {
    rnti             = rnti_;
    cell_params_list = &cell_list_params_;
  }
  Info("SCHED: Added user rnti=0x%x\n", rnti);
}

void sched_ue::set_cfg(const sched_interface::ue_cfg_t& cfg_)
{
  {
    // for the first configured cc, set it as primary cc
    if (cfg.supported_cc_list.empty()) {
      uint32_t primary_cc_idx = 0;
      if (not cfg_.supported_cc_list.empty()) {
        primary_cc_idx = cfg_.supported_cc_list[0].enb_cc_idx;
      } else {
        Warning("Primary cc idx not provided in scheduler ue_cfg. Defaulting to cc_idx=0\n");
      }
      // setup primary cc
      main_cc_params = &(*cell_params_list)[primary_cc_idx];
      cell           = main_cc_params->cfg.cell;
      max_msg3retx   = main_cc_params->cfg.maxharq_msg3tx;
    }

    // update configuration
    std::vector<sched::ue_cfg_t::cc_cfg_t> prev_supported_cc_list = std::move(cfg.supported_cc_list);
    cfg                                                           = cfg_;

    // update bearer cfgs
    for (uint32_t i = 0; i < sched_interface::MAX_LC; ++i) {
      set_bearer_cfg_unlocked(i, cfg.ue_bearers[i]);
    }

    // either add a new carrier, or reconfigure existing one
    bool scell_activation_state_changed = false;
    for (uint32_t ue_idx = 0; ue_idx < cfg.supported_cc_list.size(); ++ue_idx) {
      auto& cc_cfg = cfg.supported_cc_list[ue_idx];

      if (ue_idx >= prev_supported_cc_list.size()) {
        // New carrier needs to be added
        carriers.emplace_back(cfg, (*cell_params_list)[cc_cfg.enb_cc_idx], rnti, ue_idx);
      } else if (cc_cfg.enb_cc_idx != prev_supported_cc_list[ue_idx].enb_cc_idx) {
        // TODO: Check if this will ever happen.
        // One carrier was added in the place of another
        carriers[ue_idx] = sched_ue_carrier{cfg, (*cell_params_list)[cc_cfg.enb_cc_idx], rnti, ue_idx};
      } else {
        // The SCell internal configuration may have changed
        carriers[ue_idx].set_cfg(cfg);
      }
      scell_activation_state_changed |= carriers[ue_idx].is_active() != cc_cfg.active and ue_idx > 0;
    }
    if (scell_activation_state_changed) {
      pending_ces.emplace_back(srslte::sch_subh::SCELL_ACTIVATION);
      log_h->info("SCHED: Enqueueing SCell Activation CMD for rnti=0x%x\n", rnti);
    }
  }
}

void sched_ue::reset()
{
  {
    cfg                          = {};
    sr                           = false;
    next_tpc_pusch               = 1;
    next_tpc_pucch               = 1;
    buf_mac                      = 0;
    buf_ul                       = 0;
    phy_config_dedicated_enabled = false;
    cqi_request_tti              = 0;
    conres_ce_pending            = true;
    carriers.clear();

    // erase all bearers
    for (uint32_t i = 0; i < cfg.ue_bearers.size(); ++i) {
      set_bearer_cfg_unlocked(i, {});
    }
  }
}

/*******************************************************
 *
 * FAPI-like main scheduler interface.
 *
 *******************************************************/

void sched_ue::set_bearer_cfg(uint32_t lc_id, sched_interface::ue_bearer_cfg_t* cfg_)
{
  cfg.ue_bearers[lc_id] = *cfg_;
  set_bearer_cfg_unlocked(lc_id, *cfg_);
}

void sched_ue::rem_bearer(uint32_t lc_id)
{
  cfg.ue_bearers[lc_id] = sched_interface::ue_bearer_cfg_t{};
  set_bearer_cfg_unlocked(lc_id, sched_interface::ue_bearer_cfg_t{});
}

void sched_ue::phy_config_enabled(uint32_t tti, bool enabled)
{
  for (sched_ue_carrier& c : carriers) {
    c.dl_cqi_tti = tti;
  }
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
  Debug("SCHED: bsr=%d, lcid=%d, bsr={%d,%d,%d,%d}\n", bsr, lc_id, lch[0].bsr, lch[1].bsr, lch[2].bsr, lch[3].bsr);
}

void sched_ue::ul_phr(int phr)
{
  power_headroom = phr;
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

void sched_ue::set_needs_ta_cmd(uint32_t nof_ta_cmd_)
{
  for (uint32_t i = 0; i < nof_ta_cmd_; ++i) {
    pending_ces.emplace_back(srslte::sch_subh::TA_CMD);
  }
  Info("SCHED: rnti=0x%x needs TA CMD\n", rnti);
}

bool sched_ue::pucch_sr_collision(uint32_t current_tti, uint32_t n_cce)
{
  if (!phy_config_dedicated_enabled) {
    return false;
  }
  if (cfg.pucch_cfg.sr_configured && srslte_ue_ul_sr_send_tti(&cfg.pucch_cfg, current_tti)) {
    return (n_cce + cfg.pucch_cfg.N_pucch_1) == cfg.pucch_cfg.n_pucch_sr;
  }
  return false;
}

int sched_ue::set_ack_info(uint32_t tti, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack)
{
  int  ret = -1;
  auto p   = get_cell_index(enb_cc_idx);
  if (p.first) {
    ret = carriers[p.second].set_ack_info(tti, tb_idx, ack);
  } else {
    log_h->warning("Received DL ACK for invalid cell index %d\n", enb_cc_idx);
  }
  return ret;
}

void sched_ue::ul_recv_len(uint32_t lcid, uint32_t len)
{
  // Remove PDCP header??
  if (len > 4) {
    len -= 4;
  }
  if (lcid < sched_interface::MAX_LC) {
    if (bearer_is_ul(&lch[lcid])) {
      if (lch[lcid].bsr > (int)len) {
        lch[lcid].bsr -= len;
      } else {
        lch[lcid].bsr = 0;
      }
    }
  }
  Debug("SCHED: recv_len=%d, lcid=%d, bsr={%d,%d,%d,%d}\n", len, lcid, lch[0].bsr, lch[1].bsr, lch[2].bsr, lch[3].bsr);
}

void sched_ue::set_ul_crc(uint32_t tti, uint32_t enb_cc_idx, bool crc_res)
{
  auto p = get_cell_index(enb_cc_idx);
  if (p.first) {
    get_ul_harq(tti, p.second)->set_ack(0, crc_res);
  } else {
    log_h->warning("Received UL CRC for invalid cell index %d\n", enb_cc_idx);
  }
}

void sched_ue::set_dl_ri(uint32_t tti, uint32_t enb_cc_idx, uint32_t ri)
{
  auto p = get_cell_index(enb_cc_idx);
  if (p.first) {
    carriers[p.second].dl_ri     = ri;
    carriers[p.second].dl_ri_tti = tti;
  } else {
    log_h->warning("Received DL RI for invalid cell index %d\n", enb_cc_idx);
  }
}

void sched_ue::set_dl_pmi(uint32_t tti, uint32_t enb_cc_idx, uint32_t pmi)
{
  auto p = get_cell_index(enb_cc_idx);
  if (p.first) {
    carriers[p.second].dl_pmi     = pmi;
    carriers[p.second].dl_pmi_tti = tti;
  } else {
    log_h->warning("Received DL PMI for invalid cell index %d\n", enb_cc_idx);
  }
}

void sched_ue::set_dl_cqi(uint32_t tti, uint32_t enb_cc_idx, uint32_t cqi)
{
  auto p = get_cell_index(enb_cc_idx);
  if (p.second != std::numeric_limits<uint32_t>::max()) {
    carriers[p.second].dl_cqi     = cqi;
    carriers[p.second].dl_cqi_tti = tti;
    carriers[p.second].update_cell_activity();
  } else {
    log_h->warning("Received DL CQI for invalid cell index %d\n", enb_cc_idx);
  }
}

void sched_ue::set_ul_cqi(uint32_t tti, uint32_t enb_cc_idx, uint32_t cqi, uint32_t ul_ch_code)
{
  auto p = get_cell_index(enb_cc_idx);
  if (p.first) {
    carriers[p.second].ul_cqi     = cqi;
    carriers[p.second].ul_cqi_tti = tti;
  } else {
    log_h->warning("Received SNR info for invalid cell index %d\n", enb_cc_idx);
  }
}

void sched_ue::tpc_inc()
{
  if (power_headroom > 0) {
    next_tpc_pusch = 3;
    next_tpc_pucch = 3;
  }
  log_h->info("SCHED: Set TCP=%d for rnti=0x%x\n", next_tpc_pucch, rnti);
}

void sched_ue::tpc_dec()
{
  next_tpc_pusch = 0;
  next_tpc_pucch = 0;
  log_h->info("SCHED: Set TCP=%d for rnti=0x%x\n", next_tpc_pucch, rnti);
}

/*******************************************************
 *
 * Functions used to generate DCI grants
 *
 *******************************************************/

/**
 * Allocate space for multiple MAC SDUs (i.e. RLC PDUs) and corresponding MAC SDU subheaders
 * @param data struct where the rlc pdu allocations are stored
 * @param total_tbs available TB size for allocations for a single UE
 * @param tbidx index of TB
 * @return allocated bytes, which is always equal or lower than total_tbs
 */
uint32_t sched_ue::allocate_mac_sdus(sched_interface::dl_sched_data_t* data, uint32_t total_tbs, uint32_t tbidx)
{
  // TS 36.321 sec 7.1.2 - MAC PDU subheader is 2 bytes if L<=128 and 3 otherwise
  auto      compute_subheader_size    = [](uint32_t sdu_size) { return sdu_size > 128 ? 3 : 2; };
  constexpr uint32_t min_mac_sdu_size = 5; // accounts for MAC SDU subheader and RLC header
  uint32_t           rem_tbs          = total_tbs;

  // if we do not have enough bytes to fit MAC subheader and RLC header, skip MAC SDU allocation
  while (rem_tbs >= min_mac_sdu_size) {
    uint32_t max_sdu_bytes   = rem_tbs - compute_subheader_size(rem_tbs - 2);
    uint32_t alloc_sdu_bytes = alloc_rlc_pdu(&data->pdu[tbidx][data->nof_pdu_elems[tbidx]], max_sdu_bytes);
    if (alloc_sdu_bytes == 0) {
      break;
    }
    rem_tbs -= (alloc_sdu_bytes + compute_subheader_size(alloc_sdu_bytes)); // account for MAC sub-header
    data->nof_pdu_elems[tbidx]++;
  }

  return total_tbs - rem_tbs;
}

// Generates a Format1 dci
// > return 0 if allocation is invalid
int sched_ue::generate_format1(dl_harq_proc*                     h,
                               sched_interface::dl_sched_data_t* data,
                               uint32_t                          tti_tx_dl,
                               uint32_t                          cc_idx,
                               uint32_t                          cfi,
                               const rbgmask_t&                  user_mask)
{
  srslte_dci_dl_t* dci = &data->dci;

  int mcs = 0;
  int tbs = 0;

  dci->alloc_type              = SRSLTE_RA_ALLOC_TYPE0;
  dci->type0_alloc.rbg_bitmask = (uint32_t)user_mask.to_uint64();

  if (h->is_empty(0)) {

    // Get total available data to transmit (includes MAC header)
    uint32_t req_bytes      = get_pending_dl_new_data_total_unlocked();
    bool     need_conres_ce = is_conres_ce_pending();

    uint32_t nof_prb = format1_count_prb(user_mask, cc_idx);

    // Calculate exact number of RE for this PRB allocation
    srslte_pdsch_grant_t grant = {};
    srslte_dl_sf_cfg_t   dl_sf = {};
    dl_sf.cfi                  = cfi;
    dl_sf.tti                  = tti_tx_dl;
    srslte_ra_dl_grant_to_grant_prb_allocation(dci, &grant, cell.nof_prb);
    uint32_t nof_re = srslte_ra_dl_grant_nof_re(&cell, &dl_sf, &grant);

    int mcs0 = carriers[cc_idx].fixed_mcs_dl;
    if (need_conres_ce and cell.nof_prb < 10) { // SRB0 Tx. Use a higher MCS for the PRACH to fit in 6 PRBs
      mcs0 = MCS_FIRST_DL;
    }
    if (mcs0 < 0) { // dynamic MCS
      tbs = carriers[cc_idx].alloc_tbs_dl(nof_prb, nof_re, req_bytes, &mcs);
    } else {
      mcs = mcs0;
      tbs = sched_utils::get_tbs_bytes(mcs, nof_prb, false);
    }

    // Allocate DL Harq
    h->new_tx(user_mask, 0, tti_tx_dl, mcs, tbs, data->dci.location.ncce);

    int rem_tbs = tbs;

    // Allocate MAC ConRes CE
    if (need_conres_ce) {
      data->pdu[0][data->nof_pdu_elems[0]].lcid = srslte::sch_subh::CON_RES_ID;
      data->nof_pdu_elems[0]++;
      conres_ce_pending = false;
      Info("SCHED: Added MAC Contention Resolution CE for rnti=0x%x\n", rnti);
    }

    // Allocate MAC CE CMDs
    while (cc_idx == 0 and not pending_ces.empty()) {
      int toalloc = pending_ces.front().get_req_bytes(cfg);
      if (rem_tbs < toalloc) {
        break;
      }
      data->pdu[0][data->nof_pdu_elems[0]].lcid = pending_ces.front().cetype;
      data->nof_pdu_elems[0]++;
      rem_tbs -= toalloc;
      Info("SCHED: Added a MAC %s CE for rnti=0x%x\n", pending_ces.front().to_string().c_str(), rnti);
      pending_ces.pop_front();
    }

    // Allocate MAC SDU and respective subheaders
    allocate_mac_sdus(data, rem_tbs, 0);

    Debug("SCHED: Alloc format1 new mcs=%d, tbs=%d, nof_prb=%d, req_bytes=%d\n", mcs, tbs, nof_prb, req_bytes);
  } else {
    h->new_retx(user_mask, 0, tti_tx_dl, &mcs, &tbs, data->dci.location.ncce);
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
int sched_ue::generate_format2a(dl_harq_proc*                     h,
                                sched_interface::dl_sched_data_t* data,
                                uint32_t                          tti,
                                uint32_t                          cc_idx,
                                uint32_t                          cfi,
                                const rbgmask_t&                  user_mask)
{
  int ret = generate_format2a_unlocked(h, data, tti, cc_idx, cfi, user_mask);
  return ret;
}

// Generates a Format2a dci
int sched_ue::generate_format2a_unlocked(dl_harq_proc*                     h,
                                         sched_interface::dl_sched_data_t* data,
                                         uint32_t                          tti,
                                         uint32_t                          cc_idx,
                                         uint32_t                          cfi,
                                         const rbgmask_t&                  user_mask)
{
  bool tb_en[SRSLTE_MAX_TB] = {false};

  srslte_dci_dl_t* dci = &data->dci;

  dci->alloc_type              = SRSLTE_RA_ALLOC_TYPE0;
  dci->type0_alloc.rbg_bitmask = (uint32_t)user_mask.to_uint64();

  uint32_t nof_prb = format1_count_prb(user_mask, cc_idx); // TODO: format1???

  // Calculate exact number of RE for this PRB allocation
  srslte_pdsch_grant_t grant = {};
  srslte_dl_sf_cfg_t   dl_sf = {};
  dl_sf.cfi                  = cfi;
  dl_sf.tti                  = tti;
  srslte_ra_dl_grant_to_grant_prb_allocation(dci, &grant, cell.nof_prb);
  uint32_t nof_re = srslte_ra_dl_grant_nof_re(&cell, &dl_sf, &grant);

  bool no_retx = true;

  if (carriers[cc_idx].dl_ri == 0) {
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
        no_retx   = false;
      }
    }
    /* Two layers, no retransmissions...  */
    if (no_retx) {
      tb_en[0] = true;
      tb_en[1] = true;
    }
  }

  for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
    uint32_t req_bytes = get_pending_dl_new_data_total_unlocked();
    int      mcs       = 0;
    int      tbs       = 0;

    if (!h->is_empty(tb)) {
      h->new_retx(user_mask, tb, tti, &mcs, &tbs, data->dci.location.ncce);
      Debug("SCHED: Alloc format2/2a previous mcs=%d, tbs=%d\n", mcs, tbs);
    } else if (tb_en[tb] && req_bytes && no_retx) {
      if (carriers[cc_idx].fixed_mcs_dl < 0) {
        tbs = carriers[cc_idx].alloc_tbs_dl(nof_prb, nof_re, req_bytes, &mcs);
      } else {
        tbs = srslte_ra_tbs_from_idx(
                  (uint32_t)srslte_ra_tbs_idx_from_mcs((uint32_t)carriers[cc_idx].fixed_mcs_dl, false), nof_prb) /
              8;
        mcs = carriers[cc_idx].fixed_mcs_dl;
      }

      h->new_tx(user_mask, tb, tti, mcs, tbs, data->dci.location.ncce);

      allocate_mac_sdus(data, tbs, tb);

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
  dci->tpc_pucch = (uint8_t)next_tpc_pucch;
  next_tpc_pucch = 1;

  int ret = data->tbs[0] + data->tbs[1];
  return ret;
}

// Generates a Format2 dci
int sched_ue::generate_format2(dl_harq_proc*                     h,
                               sched_interface::dl_sched_data_t* data,
                               uint32_t                          tti,
                               uint32_t                          cc_idx,
                               uint32_t                          cfi,
                               const rbgmask_t&                  user_mask)
{
  /* Call Format 2a (common) */
  int ret = generate_format2a_unlocked(h, data, tti, cc_idx, cfi, user_mask);

  /* Compute precoding information */
  data->dci.format = SRSLTE_DCI_FORMAT2;
  if ((SRSLTE_DCI_IS_TB_EN(data->dci.tb[0]) + SRSLTE_DCI_IS_TB_EN(data->dci.tb[1])) == 1) {
    data->dci.pinfo = (uint8_t)(carriers[cc_idx].dl_pmi + 1) % (uint8_t)5;
  } else {
    data->dci.pinfo = (uint8_t)(carriers[cc_idx].dl_pmi & 1u);
  }

  return ret;
}

int sched_ue::generate_format0(sched_interface::ul_sched_data_t* data,
                               uint32_t                          tti,
                               uint32_t                          cc_idx,
                               ul_harq_proc::ul_alloc_t          alloc,
                               bool                              needs_pdcch,
                               srslte_dci_location_t             dci_pos,
                               int                               explicit_mcs)
{
  ul_harq_proc*    h   = get_ul_harq(tti, cc_idx);
  srslte_dci_ul_t* dci = &data->dci;

  bool cqi_request = needs_cqi_unlocked(tti, true);

  // Set DCI position
  data->needs_pdcch = needs_pdcch;
  dci->location     = dci_pos;

  int mcs = (explicit_mcs >= 0) ? explicit_mcs : carriers[cc_idx].fixed_mcs_ul;
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
      tbs                = carriers[cc_idx].alloc_tbs_ul(alloc.L, nof_re, req_bytes, &mcs);
    }
    h->new_tx(tti, mcs, tbs, alloc, nof_retx);

    // Un-trigger SR
    unset_sr();
  } else {
    // retx
    h->new_retx(0, tti, &mcs, nullptr, alloc);
    tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(mcs, true), alloc.L) / 8;
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

bool sched_ue::bearer_is_ul(ue_bearer_t* lch)
{
  return lch->cfg.direction == sched_interface::ue_bearer_cfg_t::UL ||
         lch->cfg.direction == sched_interface::ue_bearer_cfg_t::BOTH;
}

bool sched_ue::bearer_is_dl(const ue_bearer_t* lch)
{
  return lch->cfg.direction == sched_interface::ue_bearer_cfg_t::DL ||
         lch->cfg.direction == sched_interface::ue_bearer_cfg_t::BOTH;
}

uint32_t sched_ue::get_max_retx()
{
  return cfg.maxharq_tx;
}

bool sched_ue::is_first_dl_tx()
{
  for (const sched_ue_carrier& c : carriers) {
    for (auto& h : c.dl_harq) {
      if (h.nof_tx(0) > 0) {
        return false;
      }
    }
  }
  return true;
}

bool sched_ue::needs_cqi(uint32_t tti, uint32_t cc_idx, bool will_be_sent)
{
  return needs_cqi_unlocked(tti, cc_idx, will_be_sent);
}

// Private lock-free implemenentation
bool sched_ue::needs_cqi_unlocked(uint32_t tti, uint32_t cc_idx, bool will_be_sent)
{
  bool ret = false;
  if (phy_config_dedicated_enabled && cfg.aperiodic_cqi_period && get_pending_dl_new_data_unlocked() > 0) {
    uint32_t interval = srslte_tti_interval(tti, carriers[cc_idx].dl_cqi_tti);
    bool     needscqi = interval >= cfg.aperiodic_cqi_period;
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

bool sched_ue::is_conres_ce_pending() const
{
  return conres_ce_pending and bearer_is_dl(&lch[0]) and (lch[0].buf_retx > 0 or lch[0].buf_tx > 0);
}

uint32_t sched_ue::get_pending_dl_new_data()
{
  return get_pending_dl_new_data_unlocked();
}

/// Use this function in the dl-metric to get the bytes to be scheduled. It accounts for the UE data,
/// the RAR resources, and headers
/// \return number of bytes to be allocated
uint32_t sched_ue::get_pending_dl_new_data_total()
{
  return get_pending_dl_new_data_total_unlocked();
}

uint32_t sched_ue::get_pending_dl_new_data_total_unlocked()
{
  uint32_t req_bytes = get_pending_dl_new_data_unlocked();
  if (is_conres_ce_pending()) {
    req_bytes += conres_ce_size; // Account for ConRes
  }
  if (req_bytes > 0) {
    req_bytes += (req_bytes < 128) ? 2 : 3; // consider the header
  }
  return req_bytes;
}

// Private lock-free implementation
uint32_t sched_ue::get_pending_dl_new_data_unlocked()
{
  uint32_t pending_data = 0;
  for (int i = 0; i < sched_interface::MAX_LC; i++) {
    if (bearer_is_dl(&lch[i])) {
      pending_data += lch[i].buf_retx + lch[i].buf_tx;
    }
  }
  for (auto& ce : pending_ces) {
    pending_data += ce.get_req_bytes(cfg);
  }
  return pending_data;
}

uint32_t sched_ue::get_pending_ul_new_data(uint32_t tti)
{
  return get_pending_ul_new_data_unlocked(tti);
}

uint32_t sched_ue::get_pending_ul_old_data(uint32_t cc_idx)
{
  return get_pending_ul_old_data_unlocked(cc_idx);
}

// Private lock-free implementation
uint32_t sched_ue::get_pending_ul_new_data_unlocked(uint32_t tti)
{
  uint32_t pending_data = 0;
  for (int i = 0; i < sched_interface::MAX_LC; i++) {
    if (bearer_is_ul(&lch[i])) {
      pending_data += lch[i].bsr;
    }
  }
  if (pending_data == 0) {
    if (is_sr_triggered()) {
      return 512;
    }
    for (uint32_t cc_idx = 0; cc_idx < carriers.size(); ++cc_idx) {
      if (needs_cqi_unlocked(tti, cc_idx)) {
        return 128;
      }
    }
  }

  // Subtract all the UL data already allocated in the UL harqs
  uint32_t pending_ul_data = 0;
  for (uint32_t cc_idx = 0; cc_idx < carriers.size(); ++cc_idx) {
    pending_ul_data += get_pending_ul_old_data_unlocked(cc_idx);
  }
  pending_data = (pending_data > pending_ul_data) ? pending_data - pending_ul_data : 0;

  if (pending_data > 0) {
    Debug("SCHED: pending_data=%d, pending_ul_data=%d, bsr={%d,%d,%d,%d}\n",
          pending_data,
          pending_ul_data,
          lch[0].bsr,
          lch[1].bsr,
          lch[2].bsr,
          lch[3].bsr);
  }
  return pending_data;
}

// Private lock-free implementation
uint32_t sched_ue::get_pending_ul_old_data_unlocked(uint32_t cc_idx)
{
  return carriers[cc_idx].get_pending_ul_old_data();
}

uint32_t sched_ue::get_required_prb_dl(uint32_t cc_idx, uint32_t req_bytes, uint32_t nof_ctrl_symbols)
{
  int      mcs    = 0;
  uint32_t nof_re = 0;
  int      tbs    = 0;

  uint32_t nbytes = 0;
  uint32_t n;
  int      mcs0 = (is_first_dl_tx() and cell.nof_prb == 6) ? MCS_FIRST_DL : carriers[cc_idx].fixed_mcs_dl;
  for (n = 0; n < cell.nof_prb && nbytes < req_bytes; ++n) {
    nof_re = srslte_ra_dl_approx_nof_re(&cell, n + 1, nof_ctrl_symbols);
    if (mcs0 < 0) {
      tbs = carriers[cc_idx].alloc_tbs_dl(n + 1, nof_re, 0, &mcs);
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

uint32_t sched_ue::get_required_prb_ul(uint32_t cc_idx, uint32_t req_bytes)
{
  return carriers[cc_idx].get_required_prb_ul(req_bytes);
}

void sched_ue::set_bearer_cfg_unlocked(uint32_t lc_id, const sched_interface::ue_bearer_cfg_t& cfg_)
{
  if (lc_id < sched_interface::MAX_LC) {
    bool is_idle   = lch[lc_id].cfg.direction == sched_interface::ue_bearer_cfg_t::IDLE;
    bool is_equal  = memcmp(&cfg_, &lch[lc_id].cfg, sizeof(cfg_)) == 0;
    lch[lc_id].cfg = cfg_;
    if (lch[lc_id].cfg.direction != sched_interface::ue_bearer_cfg_t::IDLE) {
      if (not is_equal) {
        Info("SCHED: Set bearer config lc_id=%d, direction=%d\n", lc_id, (int)lch[lc_id].cfg.direction);
      }
    } else if (not is_idle) {
      Info("SCHED: Removed bearer config lc_id=%d, direction=%d\n", lc_id, (int)lch[lc_id].cfg.direction);
    }
  }
}

bool sched_ue::is_sr_triggered()
{
  return sr;
}

void sched_ue::reset_pending_pids(uint32_t tti_rx, uint32_t cc_idx)
{
  carriers[cc_idx].reset_old_pending_pids(tti_rx);
}

/* Gets HARQ process with oldest pending retx */
dl_harq_proc* sched_ue::get_pending_dl_harq(uint32_t tti_tx_dl, uint32_t ue_cc_idx)
{
  if (ue_cc_idx < carriers.size() and carriers[ue_cc_idx].is_active()) {
    return carriers[ue_cc_idx].get_pending_dl_harq(tti_tx_dl);
  }
  return nullptr;
}

dl_harq_proc* sched_ue::get_empty_dl_harq(uint32_t tti_tx_dl, uint32_t ue_cc_idx)
{
  if (ue_cc_idx < carriers.size() and carriers[ue_cc_idx].is_active()) {
    return carriers[ue_cc_idx].get_empty_dl_harq(tti_tx_dl);
  }
  return nullptr;
}

ul_harq_proc* sched_ue::get_ul_harq(uint32_t tti_tx_ul, uint32_t ue_cc_idx)
{
  if (ue_cc_idx < carriers.size() and carriers[ue_cc_idx].is_active()) {
    return carriers[ue_cc_idx].get_ul_harq(tti_tx_ul);
  }
  return nullptr;
}

dl_harq_proc* sched_ue::find_dl_harq(uint32_t tti_rx, uint32_t ue_cc_idx)
{
  for (auto& h : carriers[ue_cc_idx].dl_harq) {
    if (h.get_tti() == tti_rx) {
      return &h;
    }
  }
  return nullptr;
}

dl_harq_proc* sched_ue::get_dl_harq(uint32_t idx, uint32_t ue_cc_idx)
{
  return &carriers[ue_cc_idx].dl_harq[idx];
}

std::pair<bool, uint32_t> sched_ue::get_cell_index(uint32_t enb_cc_idx) const
{
  auto it = std::find_if(
      cfg.supported_cc_list.begin(),
      cfg.supported_cc_list.end(),
      [enb_cc_idx](const sched_interface::ue_cfg_t::cc_cfg_t& u) { return u.enb_cc_idx == enb_cc_idx and u.active; });
  if (it != cfg.supported_cc_list.end()) {
    uint32_t ue_cc_idx = std::distance(cfg.supported_cc_list.begin(), it);
    return {carriers[ue_cc_idx].is_active(), ue_cc_idx};
  }
  return {false, std::numeric_limits<uint32_t>::max()};
}

void sched_ue::finish_tti(const tti_params_t& tti_params, uint32_t enb_cc_idx)
{
  auto p = get_cell_index(enb_cc_idx);
  if (not p.first) {
    return;
  }
  uint32_t ue_cc_idx = p.second;

  /* Reset pending ACKs and clean-up all the UL Harqs with maxretx == 0 */
  get_ul_harq(tti_params.tti_tx_ul, ue_cc_idx)->reset_pending_data();

  /* reset PIDs with pending data or blocked */
  reset_pending_pids(tti_params.tti_rx, ue_cc_idx);
}

srslte_dci_format_t sched_ue::get_dci_format()
{
  srslte_dci_format_t ret = SRSLTE_DCI_FORMAT1;

  if (phy_config_dedicated_enabled) {
    /* TODO: Assumes UE-Specific Search Space (Not common) */
    switch (cfg.dl_ant_info.tx_mode) {
      case sched_interface::ant_info_ded_t::tx_mode_t::tm1:
      case sched_interface::ant_info_ded_t::tx_mode_t::tm2:
        ret = SRSLTE_DCI_FORMAT1;
        break;
      case sched_interface::ant_info_ded_t::tx_mode_t::tm3:
        ret = SRSLTE_DCI_FORMAT2A;
        break;
      case sched_interface::ant_info_ded_t::tx_mode_t::tm4:
        ret = SRSLTE_DCI_FORMAT2;
        break;
      case sched_interface::ant_info_ded_t::tx_mode_t::tm5:
      case sched_interface::ant_info_ded_t::tx_mode_t::tm6:
      case sched_interface::ant_info_ded_t::tx_mode_t::tm7:
      case sched_interface::ant_info_ded_t::tx_mode_t::tm8_v920:
      default:
        Warning("Incorrect transmission mode (rnti=%04x; tm=%d)\n", rnti, static_cast<int>(cfg.dl_ant_info.tx_mode));
    }
  }

  return ret;
}

sched_dci_cce_t* sched_ue::get_locations(uint32_t enb_cc_idx, uint32_t cfi, uint32_t sf_idx)
{
  if (cfi > 0 && cfi <= 3) {
    return &carriers[get_cell_index(enb_cc_idx).second].dci_locations[cfi - 1][sf_idx];
  } else {
    Error("SCHED: Invalid CFI=%d\n", cfi);
    return &carriers[get_cell_index(enb_cc_idx).second].dci_locations[0][sf_idx];
  }
}

sched_ue_carrier* sched_ue::get_ue_carrier(uint32_t enb_cc_idx)
{
  auto p = get_cell_index(enb_cc_idx);
  if (not p.first) {
    return nullptr;
  }
  return &carriers[p.second];
}

/* Allocates first available RLC PDU */
int sched_ue::alloc_rlc_pdu(sched_interface::dl_sched_pdu_t* mac_sdu, int rem_tbs)
{
  // TODO: Implement lcid priority (now lowest index is lowest priority)
  int alloc_bytes = 0;
  int i           = 0;
  for (i = 0; i < sched_interface::MAX_LC and alloc_bytes == 0; i++) {
    if (lch[i].buf_retx > 0) {
      alloc_bytes = SRSLTE_MIN(lch[i].buf_retx, rem_tbs);
      lch[i].buf_retx -= alloc_bytes;
    } else if (lch[i].buf_tx > 0) {
      alloc_bytes = SRSLTE_MIN(lch[i].buf_tx, rem_tbs);
      lch[i].buf_tx -= alloc_bytes;
    }
  }
  if (alloc_bytes > 0) {
    mac_sdu->lcid   = i - 1;
    mac_sdu->nbytes = alloc_bytes;
    Debug("SCHED: Allocated lcid=%d, nbytes=%d, tbs_bytes=%d\n", mac_sdu->lcid, mac_sdu->nbytes, rem_tbs);
  }
  return alloc_bytes;
}

uint32_t sched_ue::format1_count_prb(const rbgmask_t& bitmask, uint32_t cc_idx)
{
  const sched_cell_params_t* cell_cfg = carriers[cc_idx].get_cell_cfg();
  uint32_t                   nof_prb  = 0;
  for (uint32_t i = 0; i < bitmask.size(); i++) {
    if (bitmask.test(i)) {
      nof_prb += std::min(cell_cfg->cfg.cell.nof_prb - (i * cell_cfg->P), cell_cfg->P);
    }
  }
  return nof_prb;
}

int sched_ue::cqi_to_tbs(uint32_t  cqi,
                         uint32_t  nof_prb,
                         uint32_t  nof_re,
                         uint32_t  max_mcs,
                         uint32_t  max_Qm,
                         bool      is_ul,
                         uint32_t* mcs)
{
  float    max_coderate = srslte_cqi_to_coderate(cqi);
  int      sel_mcs      = max_mcs + 1;
  float    coderate     = 99;
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
    eff_coderate     = coderate / Qm;
  } while ((sel_mcs > 0 && coderate > max_coderate) || eff_coderate > 0.930);
  if (mcs) {
    *mcs = (uint32_t)sel_mcs;
  }
  return tbs;
}

/************************************************************************************************
 *                                sched_ue::sched_ue_carrier
 ***********************************************************************************************/

sched_ue_carrier::sched_ue_carrier(const sched_interface::ue_cfg_t& cfg_,
                                   const sched_cell_params_t&       cell_cfg_,
                                   uint16_t                         rnti_,
                                   uint32_t                         ue_cc_idx_) :
  cell_params(&cell_cfg_),
  rnti(rnti_),
  log_h(srslte::logmap::get("MAC ")),
  ue_cc_idx(ue_cc_idx_)
{
  // only PCell starts active. Remaining ones wait for valid CQI
  active = ue_cc_idx == 0;

  // Init HARQ processes
  for (uint32_t i = 0; i < dl_harq.size(); ++i) {
    dl_harq[i].init(i);
    ul_harq[i].init(i);
  }

  // set max mcs
  max_mcs_ul     = cell_params->sched_cfg->pusch_max_mcs >= 0 ? cell_params->sched_cfg->pusch_max_mcs : 28;
  max_mcs_dl     = cell_params->sched_cfg->pdsch_max_mcs >= 0 ? cell_params->sched_cfg->pdsch_max_mcs : 28;
  max_aggr_level = cell_params->sched_cfg->max_aggr_level >= 0 ? cell_params->sched_cfg->max_aggr_level : 3;

  // set fixed mcs
  fixed_mcs_dl = cell_params->sched_cfg->pdsch_mcs;
  fixed_mcs_ul = cell_params->sched_cfg->pusch_mcs;

  // Generate allowed CCE locations
  for (int cfi = 0; cfi < 3; cfi++) {
    for (int sf_idx = 0; sf_idx < 10; sf_idx++) {
      sched::generate_cce_location(cell_params->regs.get(), &dci_locations[cfi][sf_idx], cfi + 1, sf_idx, rnti);
    }
  }

  set_cfg(cfg_);
}

void sched_ue_carrier::reset()
{
  dl_ri      = 0;
  dl_ri_tti  = 0;
  dl_pmi     = 0;
  dl_pmi_tti = 0;
  dl_cqi     = 1;
  dl_cqi_tti = 0;
  ul_cqi     = 1;
  ul_cqi_tti = 0;
  for (uint32_t i = 0; i < dl_harq.size(); ++i) {
    for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
      dl_harq[i].reset(tb);
      ul_harq[i].reset(tb);
    }
  }
}

void sched_ue_carrier::set_cfg(const sched_interface::ue_cfg_t& cfg_)
{
  if (cfg != nullptr and cfg->maxharq_tx == cfg_.maxharq_tx) {
    // nothing changed
    return;
  }
  cfg = &cfg_;
  // Config HARQ processes
  for (uint32_t i = 0; i < dl_harq.size(); ++i) {
    dl_harq[i].set_cfg(cfg->maxharq_tx);
    ul_harq[i].set_cfg(cfg->maxharq_tx);
  }
}

void sched_ue_carrier::reset_old_pending_pids(uint32_t tti_rx)
{
  uint32_t tti_tx_dl = TTI_TX(tti_rx), tti_tx_ul = TTI_RX_ACK(tti_rx);

  // UL Harqs
  get_ul_harq(tti_tx_ul)->reset_pending_data();

  // DL harqs
  for (auto& h : dl_harq) {
    h.reset_pending_data();
    if (not h.is_empty()) {
      uint32_t tti_diff = srslte_tti_interval(tti_tx_dl, h.get_tti());
      if (tti_diff > 50 and tti_diff < 10240 / 2) {
        log_h->info("SCHED: pid=%d is old. tti_pid=%d, now is %d, resetting\n", h.get_id(), h.get_tti(), tti_tx_dl);
        for (uint32_t tb = 0; tb < SRSLTE_MAX_TB; tb++) {
          h.reset(tb);
        }
      }
    }
  }
}

dl_harq_proc* sched_ue_carrier::get_pending_dl_harq(uint32_t tti_tx_dl)
{
  if (not ASYNC_DL_SCHED) {
    dl_harq_proc* h = &dl_harq[tti_tx_dl % SCHED_MAX_HARQ_PROC];
    return h->is_empty() ? nullptr : h;
  }

  int      oldest_idx = -1;
  uint32_t oldest_tti = 0;
  for (auto& h : dl_harq) {
    if (h.has_pending_retx(0, tti_tx_dl) or h.has_pending_retx(1, tti_tx_dl)) {
      uint32_t x = srslte_tti_interval(tti_tx_dl, h.get_tti());
      if (x > oldest_tti) {
        oldest_idx = h.get_id();
        oldest_tti = x;
      }
    }
  }
  dl_harq_proc* h = nullptr;
  if (oldest_idx >= 0) {
    h = &dl_harq[oldest_idx];
  }
  return h;
}

dl_harq_proc* sched_ue_carrier::get_empty_dl_harq(uint32_t tti_tx_dl)
{
  if (not ASYNC_DL_SCHED) {
    dl_harq_proc* h = &dl_harq[tti_tx_dl % SCHED_MAX_HARQ_PROC];
    return h->is_empty() ? nullptr : h;
  }

  auto it = std::find_if(dl_harq.begin(), dl_harq.end(), [](dl_harq_proc& h) { return h.is_empty(); });
  return it != dl_harq.end() ? &(*it) : nullptr;
}

int sched_ue_carrier::set_ack_info(uint32_t tti_rx, uint32_t tb_idx, bool ack)
{
  for (auto& h : dl_harq) {
    if (TTI_TX(h.get_tti()) == tti_rx) {
      Debug("SCHED: Set ACK=%d for rnti=0x%x, pid=%d, tb=%d, tti=%d\n", ack, rnti, h.get_id(), tb_idx, tti_rx);
      h.set_ack(tb_idx, ack);
      return h.get_tbs(tb_idx);
    }
  }
  Warning("SCHED: Received ACK info for unknown TTI=%d\n", tti_rx);
  return -1;
}

ul_harq_proc* sched_ue_carrier::get_ul_harq(uint32_t tti)
{
  return &ul_harq[tti % SCHED_MAX_HARQ_PROC];
}

uint32_t sched_ue_carrier::get_pending_ul_old_data()
{
  uint32_t pending_data = 0;
  for (auto& h : ul_harq) {
    pending_data += h.get_pending_data();
  }
  return pending_data;
}

/* Find lowest DCI aggregation level supported by the UE spectral efficiency */
uint32_t sched_ue_carrier::get_aggr_level(uint32_t nof_bits)
{
  uint32_t l            = 0;
  float    max_coderate = srslte_cqi_to_coderate(dl_cqi);
  float    coderate     = 99;
  float    factor       = 1.5;
  uint32_t l_max        = 3;
  if (cell_params->nof_prb() == 6) {
    factor = 1.0;
    l_max  = 2;
  }
  l_max = SRSLTE_MIN(max_aggr_level, l_max);
  do {
    coderate = srslte_pdcch_coderate(nof_bits, l);
    l++;
  } while (l < l_max && factor * coderate > max_coderate);
  Debug("SCHED: CQI=%d, l=%d, nof_bits=%d, coderate=%.2f, max_coderate=%.2f\n",
        dl_cqi,
        l,
        nof_bits,
        coderate,
        max_coderate);
  return l;
}

/* In this scheduler we tend to use all the available bandwidth and select the MCS
 * that approximates the minimum between the capacity and the requested rate
 */
int sched_ue_carrier::alloc_tbs(uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes, bool is_ul, int* mcs)
{
  uint32_t sel_mcs = 0;

  uint32_t cqi     = is_ul ? ul_cqi : dl_cqi;
  uint32_t max_mcs = is_ul ? max_mcs_ul : max_mcs_dl;
  uint32_t max_Qm  = is_ul ? 4 : 6; // Allow 16-QAM in PUSCH Only

  // TODO: Compute real spectral efficiency based on PUSCH-UCI configuration
  int tbs_bytes = sched_ue::cqi_to_tbs(cqi, nof_prb, nof_re, max_mcs, max_Qm, is_ul, &sel_mcs) / 8;

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
    *mcs = (int)sel_mcs;
  }

  return tbs_bytes;
}

int sched_ue_carrier::alloc_tbs_dl(uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes, int* mcs)
{
  return alloc_tbs(nof_prb, nof_re, req_bytes, false, mcs);
}

int sched_ue_carrier::alloc_tbs_ul(uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes, int* mcs)
{
  return alloc_tbs(nof_prb, nof_re, req_bytes, true, mcs);
}

uint32_t sched_ue_carrier::get_required_prb_ul(uint32_t req_bytes)
{
  int      mcs    = 0;
  uint32_t nbytes = 0;
  uint32_t N_srs  = 0;

  uint32_t n = 0;
  if (req_bytes == 0) {
    return 0;
  }

  for (n = 1; n < cell_params->nof_prb() && nbytes < req_bytes + 4; n++) {
    uint32_t nof_re = (2 * (SRSLTE_CP_NSYMB(cell_params->cfg.cell.cp) - 1) - N_srs) * n * SRSLTE_NRE;
    int      tbs    = 0;
    if (fixed_mcs_ul < 0) {
      tbs = alloc_tbs_ul(n, nof_re, 0, &mcs);
    } else {
      tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(fixed_mcs_ul, true), n) / 8;
    }
    if (tbs > 0) {
      nbytes = tbs;
    }
  }

  while (!srslte_dft_precoding_valid_prb(n) && n <= cell_params->nof_prb()) {
    n++;
  }

  return n;
}

void sched_ue_carrier::update_cell_activity()
{
  if (ue_cc_idx > 0 and active != cfg->supported_cc_list[ue_cc_idx].active) {
    if (dl_cqi > 0) {
      active = cfg->supported_cc_list[ue_cc_idx].active;
      log_h->info("SCell index=%d is now %s\n", ue_cc_idx, active ? "active" : "inactive");
    }
  }
}

/*******************************************************
 *                   MAC CE Command
 ******************************************************/

int sched_ue::ce_cmd::get_sdu_size(const sched_interface::ue_cfg_t& c) const
{
  // TS 36.321 Sec. 6.1.3 - MAC Control Elements
  switch (cetype) {
    case srslte::sch_subh::cetype::TA_CMD:
      return 1;
    case srslte::sch_subh::cetype::SCELL_ACTIVATION:
      // TS 36.321 Sec. 6.1.3.8 - Number of SDU octets is 4 if there are more than 7 Scells (or 8 cells including PCell)
      return c.supported_cc_list.size() > 8 ? 4 : 1;
    case srslte::sch_subh::cetype::CON_RES_ID:
      return 6;
    default:
      srslte::logmap::get("MAC ")->error("MAC CE not recognized\n");
      return 0;
  }
}

int sched_ue::ce_cmd::get_req_bytes(const sched_interface::ue_cfg_t& c) const
{
  // 36.321 Sec. 6.1.2 - CE subheader format is R/F2/E/LCID (1 octet) for fixed-size MAC CEs.
  return get_sdu_size(c) + 1;
}

std::string sched_ue::ce_cmd::to_string() const
{
  switch (cetype) {
    case srslte::sch_subh::cetype::SCELL_ACTIVATION:
      return "SCell Activation";
    case srslte::sch_subh::cetype::CON_RES_ID:
      return "ContentionResolution";
    case srslte::sch_subh::cetype::TA_CMD:
      return "Timing Advance";
    case srslte::sch_subh::cetype::DRX_CMD:
      return "DRX";
    default:
      srslte::logmap::get("MAC ")->error("MAC CE not recognized\n");
      return "";
  }
}

} // namespace srsenb
