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

#include <string.h>

#include "srsenb/hdr/stack/mac/scheduler.h"
#include "srsenb/hdr/stack/mac/scheduler_ue.h"
#include "srslte/common/log_helper.h"
#include "srslte/common/logmap.h"
#include "srslte/mac/pdu.h"
#include "srslte/srslte.h"

/******************************************************
 *                  UE class                          *
 ******************************************************/

namespace srsenb {

/******************************************************
 *                 Helper Functions                   *
 ******************************************************/

namespace sched_utils {

//! Obtains TB size *in bytes* for a given MCS and N_{PRB}
uint32_t get_tbs_bytes(uint32_t mcs, uint32_t nof_alloc_prb, bool use_tbs_index_alt, bool is_ul)
{
  int tbs_idx = srslte_ra_tbs_idx_from_mcs(mcs, use_tbs_index_alt, is_ul);
  if (tbs_idx < SRSLTE_SUCCESS) {
    tbs_idx = 0;
  }

  int tbs = srslte_ra_tbs_from_idx((uint32_t)tbs_idx, nof_alloc_prb);
  if (tbs < SRSLTE_SUCCESS) {
    tbs = 0;
  }

  return (uint32_t)tbs / 8U;
}

//! TS 36.321 sec 7.1.2 - MAC PDU subheader is 2 bytes if L<=128 and 3 otherwise
uint32_t get_mac_subheader_sdu_size(uint32_t sdu_bytes)
{
  return sdu_bytes == 0 ? 0 : (sdu_bytes > 128 ? 3 : 2);
}

/**
 * Count number of PRBs present in a DL RBG mask
 * @param bitmask DL RBG mask
 * @return number of prbs
 */
uint32_t count_prb_per_tb(const sched_cell_params_t& cell_params, const rbgmask_t& bitmask)
{
  uint32_t nof_prb = 0;
  for (uint32_t i = 0; i < bitmask.size(); i++) {
    if (bitmask.test(i)) {
      nof_prb += std::min(cell_params.cfg.cell.nof_prb - (i * cell_params.P), cell_params.P);
    }
  }
  return nof_prb;
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
  rnti             = rnti_;
  cell_params_list = &cell_list_params_;
  Info("SCHED: Added user rnti=0x%x\n", rnti);
}

void sched_ue::set_cfg(const sched_interface::ue_cfg_t& cfg_)
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
      // One carrier was added in the place of another
      carriers[ue_idx] = sched_ue_carrier{cfg, (*cell_params_list)[cc_cfg.enb_cc_idx], rnti, ue_idx};
      if (ue_idx == 0) {
        log_h->info("SCHED: PCell has changed for rnti=0x%x.\n", rnti);
      }
    } else {
      // The SCell internal configuration may have changed
      carriers[ue_idx].set_cfg(cfg);
    }
    scell_activation_state_changed |= carriers[ue_idx].is_active() != cc_cfg.active and ue_idx > 0;
  }
  if (scell_activation_state_changed) {
    pending_ces.emplace_back(srslte::dl_sch_lcid::SCELL_ACTIVATION);
    log_h->info("SCHED: Enqueueing SCell Activation CMD for rnti=0x%x\n", rnti);
  }
}

void sched_ue::reset()
{
  cfg                          = {};
  sr                           = false;
  next_tpc_pusch               = 1;
  next_tpc_pucch               = 1;
  phy_config_dedicated_enabled = false;
  cqi_request_tti              = 0;
  carriers.clear();

  // erase all bearers
  for (uint32_t i = 0; i < cfg.ue_bearers.size(); ++i) {
    set_bearer_cfg_unlocked(i, {});
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

void sched_ue::mac_buffer_state(uint32_t ce_code, uint32_t nof_cmds)
{
  auto cmd = (ce_cmd)ce_code;
  for (uint32_t i = 0; i < nof_cmds; ++i) {
    if (cmd == ce_cmd::CON_RES_ID) {
      pending_ces.push_front(cmd);
    } else {
      pending_ces.push_back(cmd);
    }
  }
  Info("SCHED: %s for rnti=0x%x needs to be scheduled\n", to_string(cmd), rnti);
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
  if (cfg.pucch_cfg.sr_configured && srslte_ue_ul_sr_send_tti(&cfg.pucch_cfg, current_tti)) {
    return (n_cce + cfg.pucch_cfg.N_pucch_1) == cfg.pucch_cfg.n_pucch_sr;
  }
  return false;
}

int sched_ue::set_ack_info(uint32_t tti_rx, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack)
{
  int  tbs_acked = -1;
  auto p         = get_cell_index(enb_cc_idx);
  if (p.first) {
    std::pair<uint32_t, int> p2 = carriers[p.second].harq_ent.set_ack_info(tti_rx, tb_idx, ack);
    tbs_acked                   = p2.second;
    if (tbs_acked > 0) {
      Debug("SCHED: Set ACK=%d for rnti=0x%x, pid=%d, tb=%d, tti=%d\n", ack, rnti, p2.first, tb_idx, tti_rx);
    } else {
      Warning("SCHED: Received ACK info for unknown TTI=%d\n", tti_rx);
    }
  } else {
    log_h->warning("Received DL ACK for invalid cell index %d\n", enb_cc_idx);
  }
  return tbs_acked;
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

void sched_ue::set_ul_crc(srslte::tti_point tti_rx, uint32_t enb_cc_idx, bool crc_res)
{
  auto p = get_cell_index(enb_cc_idx);
  if (p.first) {
    auto ret = carriers[p.second].harq_ent.set_ul_crc(tti_rx, 0, crc_res);
    if (not ret.first) {
      log_h->warning("Received UL CRC for invalid tti_rx=%d\n", (int)tti_rx.to_uint());
    }
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
    carriers[p.second].set_dl_cqi(tti, cqi);
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

constexpr uint32_t min_mac_sdu_size = 5; // accounts for MAC SDU subheader and RLC header

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
  auto     compute_subheader_size = [](uint32_t sdu_size) { return sdu_size > 128 ? 3 : 2; };
  uint32_t rem_tbs                = total_tbs;

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

/**
 * Allocate space for pending MAC CEs
 * @param data struct where the MAC CEs allocations are stored
 * @param total_tbs available space in bytes for allocations
 * @return number of bytes allocated
 */
uint32_t sched_ue::allocate_mac_ces(sched_interface::dl_sched_data_t* data, uint32_t total_tbs, uint32_t ue_cc_idx)
{
  if (ue_cc_idx != 0) {
    return 0;
  }

  int rem_tbs = total_tbs;
  while (not pending_ces.empty()) {
    int toalloc = srslte::ce_total_size(pending_ces.front());
    if (rem_tbs < toalloc) {
      break;
    }
    data->pdu[0][data->nof_pdu_elems[0]].lcid = (uint32_t)pending_ces.front();
    data->nof_pdu_elems[0]++;
    rem_tbs -= toalloc;
    Info("SCHED: Added a MAC %s CE for rnti=0x%x\n", srslte::to_string(pending_ces.front()), rnti);
    pending_ces.pop_front();
  }
  return total_tbs - rem_tbs;
}

/**
 * Allocate space
 * @param data
 * @param total_tbs
 * @param ue_cc_idx
 * @return pair with allocated tbs and mcs
 */
std::pair<int, int> sched_ue::allocate_new_dl_mac_pdu(sched::dl_sched_data_t* data,
                                                      dl_harq_proc*           h,
                                                      const rbgmask_t&        user_mask,
                                                      uint32_t                tti_tx_dl,
                                                      uint32_t                ue_cc_idx,
                                                      uint32_t                cfi,
                                                      uint32_t                tb,
                                                      const char*             dci_format)
{
  srslte_dci_dl_t* dci     = &data->dci;
  uint32_t         nof_prb = sched_utils::count_prb_per_tb(*carriers[ue_cc_idx].get_cell_cfg(), user_mask);
  auto             ret     = compute_mcs_and_tbs(ue_cc_idx, tti_tx_dl, nof_prb, cfi, *dci);
  int              mcs     = ret.first;
  int              tbs     = ret.second;

  /* Allocate MAC PDU (subheaders, CEs, and SDUS) */
  int rem_tbs = tbs;
  rem_tbs -= allocate_mac_ces(data, rem_tbs, ue_cc_idx);
  rem_tbs -= allocate_mac_sdus(data, rem_tbs, tb);

  /* Allocate DL UE Harq */
  if (rem_tbs != tbs) {
    h->new_tx(user_mask, tb, tti_tx_dl, mcs, tbs, data->dci.location.ncce);
    Debug("SCHED: Alloc DCI format%s new mcs=%d, tbs=%d, nof_prb=%d\n", dci_format, mcs, tbs, nof_prb);
  } else {
    Warning("SCHED: Failed to allocate DL harq pid=%d\n", h->get_id());
  }

  return std::make_pair(tbs, mcs);
}

int sched_ue::generate_dl_dci_format(uint32_t                          pid,
                                     sched_interface::dl_sched_data_t* data,
                                     uint32_t                          tti_tx_dl,
                                     uint32_t                          ue_cc_idx,
                                     uint32_t                          cfi,
                                     const rbgmask_t&                  user_mask)
{
  srslte_dci_format_t dci_format = get_dci_format();
  int                 tbs        = 0;

  switch (dci_format) {
    case SRSLTE_DCI_FORMAT1:
      tbs = generate_format1(pid, data, tti_tx_dl, ue_cc_idx, cfi, user_mask);
      break;
    case SRSLTE_DCI_FORMAT2:
      tbs = generate_format2(pid, data, tti_tx_dl, ue_cc_idx, cfi, user_mask);
      break;
    case SRSLTE_DCI_FORMAT2A:
      tbs = generate_format2a(pid, data, tti_tx_dl, ue_cc_idx, cfi, user_mask);
      break;
    default:
      Error("DCI format (%d) not implemented\n", dci_format);
  }
  return tbs;
}

// Generates a Format1 dci
// > return 0 if allocation is invalid
int sched_ue::generate_format1(uint32_t                          pid,
                               sched_interface::dl_sched_data_t* data,
                               uint32_t                          tti_tx_dl,
                               uint32_t                          ue_cc_idx,
                               uint32_t                          cfi,
                               const rbgmask_t&                  user_mask)
{
  dl_harq_proc*    h   = &carriers[ue_cc_idx].harq_ent.dl_harq_procs()[pid];
  srslte_dci_dl_t* dci = &data->dci;

  int mcs = 0;
  int tbs = 0;

  dci->alloc_type              = SRSLTE_RA_ALLOC_TYPE0;
  dci->type0_alloc.rbg_bitmask = (uint32_t)user_mask.to_uint64();

  if (h->is_empty(0)) {
    auto ret = allocate_new_dl_mac_pdu(data, h, user_mask, tti_tx_dl, ue_cc_idx, cfi, 0, "1");
    tbs      = ret.first;
    mcs      = ret.second;
  } else {
    h->new_retx(user_mask, 0, tti_tx_dl, &mcs, &tbs, data->dci.location.ncce);
    Debug("SCHED: Alloc format1 previous mcs=%d, tbs=%d\n", mcs, tbs);
  }

  if (tbs > 0) {
    dci->rnti          = rnti;
    dci->pid           = h->get_id();
    dci->ue_cc_idx     = ue_cc_idx;
    dci->tb[0].mcs_idx = (uint32_t)mcs;
    dci->tb[0].rv      = sched_utils::get_rvidx(h->nof_retx(0));
    dci->tb[0].ndi     = h->get_ndi(0);

    dci->tpc_pucch = (uint8_t)next_tpc_pucch;
    next_tpc_pucch = 1;
    data->tbs[0]   = (uint32_t)tbs;
    data->tbs[1]   = 0;

    dci->format = SRSLTE_DCI_FORMAT1;
  }
  return tbs;
}

/**
 * Based on the amount of tx data, allocated PRBs, DCI params, etc. compute a valid MCS and resulting TBS
 * @param ue_cc_idx user carrier index
 * @param tti_tx_dl tti when the tx will occur
 * @param nof_alloc_prbs number of PRBs that were allocated
 * @param cfi Number of control symbols in Subframe
 * @param dci contains the RBG mask, and alloc type
 * @return pair with MCS and TBS (in bytes)
 */
std::pair<int, int> sched_ue::compute_mcs_and_tbs(uint32_t               ue_cc_idx,
                                                  uint32_t               tti_tx_dl,
                                                  uint32_t               nof_alloc_prbs,
                                                  uint32_t               cfi,
                                                  const srslte_dci_dl_t& dci)
{
  int                           mcs = 0, tbs_bytes = 0;
  std::pair<uint32_t, uint32_t> req_bytes = get_requested_dl_bytes(ue_cc_idx);

  // Calculate exact number of RE for this PRB allocation
  srslte_pdsch_grant_t grant = {};
  srslte_dl_sf_cfg_t   dl_sf = {};
  dl_sf.cfi                  = cfi;
  dl_sf.tti                  = tti_tx_dl;
  srslte_ra_dl_grant_to_grant_prb_allocation(&dci, &grant, carriers[ue_cc_idx].get_cell_cfg()->nof_prb());
  uint32_t nof_re = srslte_ra_dl_grant_nof_re(&carriers[ue_cc_idx].get_cell_cfg()->cfg.cell, &dl_sf, &grant);

  // Compute MCS+TBS
  // Use a higher MCS for the Msg4 to fit in the 6 PRB case
  if (carriers[ue_cc_idx].fixed_mcs_dl < 0 or not carriers[ue_cc_idx].dl_cqi_rx) {
    // Dynamic MCS
    tbs_bytes = carriers[ue_cc_idx].alloc_tbs_dl(nof_alloc_prbs, nof_re, req_bytes.second, &mcs);
  } else {
    // Fixed MCS
    tbs_bytes = sched_utils::get_tbs_bytes(
        (uint32_t)carriers[ue_cc_idx].fixed_mcs_dl, nof_alloc_prbs, cfg.use_tbs_index_alt, false);
  }

  // If the number of prbs is not sufficient to fit minimum required bytes, increase the mcs
  // NOTE: this may happen during ConRes CE tx when DL-CQI is still not available
  while (tbs_bytes > 0 and (uint32_t) tbs_bytes < req_bytes.first and mcs < 28) {
    mcs++;
    tbs_bytes = sched_utils::get_tbs_bytes((uint32_t)mcs, nof_alloc_prbs, cfg.use_tbs_index_alt, false);
  }

  return {mcs, tbs_bytes};
}

// Generates a Format2a dci
int sched_ue::generate_format2a(uint32_t                          pid,
                                sched_interface::dl_sched_data_t* data,
                                uint32_t                          tti_tx_dl,
                                uint32_t                          ue_cc_idx,
                                uint32_t                          cfi,
                                const rbgmask_t&                  user_mask)
{
  dl_harq_proc* h                    = &carriers[ue_cc_idx].harq_ent.dl_harq_procs()[pid];
  bool          tb_en[SRSLTE_MAX_TB] = {false};

  srslte_dci_dl_t* dci         = &data->dci;
  dci->alloc_type              = SRSLTE_RA_ALLOC_TYPE0;
  dci->type0_alloc.rbg_bitmask = (uint32_t)user_mask.to_uint64();

  bool no_retx = true;

  if (carriers[ue_cc_idx].dl_ri == 0) {
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
    uint32_t req_bytes = get_pending_dl_new_data_total();
    int      mcs       = 0;
    int      tbs       = 0;

    if (!h->is_empty(tb)) {

      h->new_retx(user_mask, tb, tti_tx_dl, &mcs, &tbs, data->dci.location.ncce);
      Debug("SCHED: Alloc format2/2a previous mcs=%d, tbs=%d\n", mcs, tbs);

    } else if (tb_en[tb] && req_bytes > 0 && no_retx) {
      auto ret = allocate_new_dl_mac_pdu(data, h, user_mask, tti_tx_dl, ue_cc_idx, cfi, tb, "2/2a");
      tbs      = ret.first;
      mcs      = ret.second;
    }

    /* Fill DCI TB dedicated fields */
    if (tbs > 0 && tb_en[tb]) {
      dci->tb[tb].mcs_idx = (uint32_t)mcs;
      dci->tb[tb].rv      = sched_utils::get_rvidx(h->nof_retx(tb));
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
  dci->ue_cc_idx = ue_cc_idx;
  dci->pid       = h->get_id();
  dci->tpc_pucch = (uint8_t)next_tpc_pucch;
  next_tpc_pucch = 1;

  int ret = data->tbs[0] + data->tbs[1];
  return ret;
}

// Generates a Format2 dci
int sched_ue::generate_format2(uint32_t                          pid,
                               sched_interface::dl_sched_data_t* data,
                               uint32_t                          tti,
                               uint32_t                          cc_idx,
                               uint32_t                          cfi,
                               const rbgmask_t&                  user_mask)
{
  /* Call Format 2a (common) */
  int ret = generate_format2a(pid, data, tti, cc_idx, cfi, user_mask);

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
      tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(mcs, false, true), alloc.L) / 8;
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
    tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(mcs, false, true), alloc.L) / 8;
  }

  data->tbs = tbs;

  if (tbs > 0) {
    dci->rnti            = rnti;
    dci->format          = SRSLTE_DCI_FORMAT0;
    dci->ue_cc_idx       = cc_idx;
    dci->type2_alloc.riv = srslte_ra_type2_to_riv(alloc.L, alloc.RB_start, cell.nof_prb);
    dci->tb.rv           = sched_utils::get_rvidx(h->nof_retx(0));
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

bool sched_ue::bearer_is_ul(const ue_bearer_t* lch)
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

bool sched_ue::needs_cqi(uint32_t tti, uint32_t cc_idx, bool will_be_sent)
{
  return needs_cqi_unlocked(tti, cc_idx, will_be_sent);
}

// Private lock-free implemenentation
bool sched_ue::needs_cqi_unlocked(uint32_t tti, uint32_t cc_idx, bool will_be_sent)
{
  bool ret = false;
  if (phy_config_dedicated_enabled && cfg.aperiodic_cqi_period && get_pending_dl_new_data() > 0) {
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

/// Use this function in the dl-metric to get the bytes to be scheduled. It accounts for the UE data,
/// the RAR resources, and headers
/// \return number of bytes to be allocated
uint32_t sched_ue::get_pending_dl_new_data_total()
{
  uint32_t req_bytes = get_pending_dl_new_data();
  if (req_bytes > 0) {
    req_bytes += (req_bytes < 128) ? 2 : 3; // consider the header
  }
  return req_bytes;
}

/**
 * Compute the range of RBGs that avoids segmentation of TM and MAC subheader data. Always computed for highest CFI
 * @param ue_cc_idx carrier of the UE
 * @return range of number of RBGs that a UE can allocate in a given subframe
 */
rbg_range_t sched_ue::get_required_dl_rbgs(uint32_t ue_cc_idx)
{
  std::pair<uint32_t, uint32_t> req_bytes = get_requested_dl_bytes(ue_cc_idx);
  if (req_bytes.first == 0 and req_bytes.second == 0) {
    return {0, 0};
  }
  const auto* cellparams = carriers[ue_cc_idx].get_cell_cfg();
  int         pending_prbs =
      carriers[ue_cc_idx].get_required_prb_dl(req_bytes.first, cellparams->sched_cfg->max_nof_ctrl_symbols);
  if (pending_prbs < 0) {
    // Cannot fit allocation in given PRBs
    log_h->error("SCHED: DL CQI=%d does now allow fitting %d non-segmentable DL tx bytes into the cell bandwidth. "
                 "Consider increasing initial CQI value.\n",
                 carriers[ue_cc_idx].dl_cqi,
                 req_bytes.first);
    return {cellparams->nof_prb(), cellparams->nof_prb()};
  }
  uint32_t min_pending_rbg = cellparams->prb_to_rbg(pending_prbs);
  pending_prbs = carriers[ue_cc_idx].get_required_prb_dl(req_bytes.second, cellparams->sched_cfg->max_nof_ctrl_symbols);
  pending_prbs = (pending_prbs < 0) ? cellparams->nof_prb() : pending_prbs;
  uint32_t max_pending_rbg = cellparams->prb_to_rbg(pending_prbs);
  return {min_pending_rbg, max_pending_rbg};
}

/**
 * Returns the range (min,max) of possible MAC PDU sizes.
 * - the lower boundary value is set based on the following conditions:
 *   - if there is data in SRB0, the min value is the sum of:
 *     - SRB0 RLC data (Msg4) including MAC subheader and payload (no segmentation)
 *     - ConRes CE + MAC subheader (7 bytes)
 *   - elif there is data in other RBs, the min value is either:
 *     - first pending CE (subheader+CE payload) in queue, if it exists and we are in PCell. Or,
 *     - one subheader (2B) + one RLC header (<=3B) to allow one MAC PDU alloc
 * - the upper boundary is set as a sum of:
 *   - total data in all SRBs and DRBs including the MAC subheaders
 *   - All CEs (ConRes and others) including respective MAC subheaders
 * @ue_cc_idx carrier where allocation is being made
 * @return
 */
std::pair<uint32_t, uint32_t> sched_ue::get_requested_dl_bytes(uint32_t ue_cc_idx)
{
  const uint32_t min_alloc_bytes = 5; // 2 for subheader, and 3 for RLC header
  // Convenience function to compute the number of bytes allocated for a given SDU
  auto compute_sdu_total_bytes = [&min_alloc_bytes](uint32_t lcid, uint32_t buffer_bytes) {
    if (buffer_bytes == 0) {
      return 0u;
    }
    uint32_t subheader_and_sdu = buffer_bytes + sched_utils::get_mac_subheader_sdu_size(buffer_bytes);
    return (lcid == 0) ? subheader_and_sdu : std::max(subheader_and_sdu, min_alloc_bytes);
  };

  /* Set Maximum boundary */
  // Ensure there is space for ConRes and RRC Setup
  // SRB0 is a special case due to being RLC TM (no segmentation possible)
  if (not bearer_is_dl(&lch[0])) {
    log_h->error("SRB0 must always be activated for DL\n");
    return {0, 0};
  }
  if (not carriers[ue_cc_idx].is_active()) {
    return {0, 0};
  }

  uint32_t max_data = 0, min_data = 0;
  uint32_t srb0_data = 0, rb_data = 0, sum_ce_data = 0;
  bool     is_dci_format1 = get_dci_format() == SRSLTE_DCI_FORMAT1;
  if (is_dci_format1 and (lch[0].buf_tx > 0 or lch[0].buf_retx > 0)) {
    srb0_data = compute_sdu_total_bytes(0, lch[0].buf_retx);
    srb0_data += compute_sdu_total_bytes(0, lch[0].buf_tx);
  }
  // Add pending CEs
  if (ue_cc_idx == 0) {
    for (const auto& ce : pending_ces) {
      sum_ce_data += srslte::ce_total_size(ce);
    }
  }
  // Add pending data in remaining RLC buffers
  for (int i = 1; i < sched_interface::MAX_LC; i++) {
    if (bearer_is_dl(&lch[i])) {
      rb_data += compute_sdu_total_bytes(i, lch[i].buf_retx);
      rb_data += compute_sdu_total_bytes(i, lch[i].buf_tx);
    }
  }
  max_data = srb0_data + sum_ce_data + rb_data;

  /* Set Minimum boundary */
  min_data = srb0_data;
  if (not pending_ces.empty() and pending_ces.front() == ce_cmd::CON_RES_ID) {
    min_data += srslte::ce_total_size(pending_ces.front());
  }
  if (min_data == 0) {
    if (sum_ce_data > 0) {
      min_data = srslte::ce_total_size(pending_ces.front());
    } else if (rb_data > 0) {
      min_data = min_alloc_bytes;
    }
  }

  return {min_data, max_data};
}

/**
 * Get pending DL data in RLC buffers + CEs
 * @return
 */
uint32_t sched_ue::get_pending_dl_new_data()
{
  if (std::count_if(carriers.begin(), carriers.end(), [](const sched_ue_carrier& cc) { return cc.is_active(); }) == 0) {
    return 0;
  }

  uint32_t pending_data = 0;
  for (int i = 0; i < sched_interface::MAX_LC; i++) {
    if (bearer_is_dl(&lch[i])) {
      pending_data += lch[i].buf_retx + lch[i].buf_tx;
    }
  }
  for (auto& ce : pending_ces) {
    pending_data += srslte::ce_total_size(ce);
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
  uint32_t pending_data = 0;
  for (auto& h : carriers[cc_idx].harq_ent.ul_harq_procs()) {
    pending_data += h.get_pending_data();
  }
  return pending_data;
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

/* Gets HARQ process with oldest pending retx */
dl_harq_proc* sched_ue::get_pending_dl_harq(uint32_t tti_tx_dl, uint32_t ue_cc_idx)
{
  if (ue_cc_idx < carriers.size() and carriers[ue_cc_idx].is_active()) {
    return carriers[ue_cc_idx].harq_ent.get_pending_dl_harq(tti_tx_dl);
  }
  return nullptr;
}

dl_harq_proc* sched_ue::get_empty_dl_harq(uint32_t tti_tx_dl, uint32_t ue_cc_idx)
{
  if (ue_cc_idx < carriers.size() and carriers[ue_cc_idx].is_active()) {
    return carriers[ue_cc_idx].harq_ent.get_empty_dl_harq(tti_tx_dl);
  }
  return nullptr;
}

ul_harq_proc* sched_ue::get_ul_harq(uint32_t tti_tx_ul, uint32_t ue_cc_idx)
{
  if (ue_cc_idx < carriers.size() and carriers[ue_cc_idx].is_active()) {
    return carriers[ue_cc_idx].harq_ent.get_ul_harq(tti_tx_ul);
  }
  return nullptr;
}

const dl_harq_proc& sched_ue::get_dl_harq(uint32_t idx, uint32_t ue_cc_idx) const
{
  return carriers[ue_cc_idx].harq_ent.dl_harq_procs()[idx];
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

uint32_t sched_ue::get_aggr_level(uint32_t ue_cc_idx, uint32_t nof_bits)
{
  return carriers[ue_cc_idx].get_aggr_level(nof_bits);
}

void sched_ue::finish_tti(const tti_params_t& tti_params, uint32_t enb_cc_idx)
{
  auto p = get_cell_index(enb_cc_idx);
  if (not p.first) {
    return;
  }
  uint32_t ue_cc_idx = p.second;

  /* reset PIDs with pending data or blocked */
  carriers[ue_cc_idx].harq_ent.reset_pending_data(tti_params.tti_rx);
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

int sched_ue::cqi_to_tbs(uint32_t  cqi,
                         uint32_t  nof_prb,
                         uint32_t  nof_re,
                         uint32_t  max_mcs,
                         uint32_t  max_Qm,
                         bool      use_tbs_index_alt,
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
    uint32_t tbs_idx = srslte_ra_tbs_idx_from_mcs(sel_mcs, use_tbs_index_alt, is_ul);
    tbs              = srslte_ra_tbs_from_idx(tbs_idx, nof_prb);
    coderate         = srslte_coderate(tbs, nof_re);
    srslte_mod_t mod =
        (is_ul) ? srslte_ra_ul_mod_from_mcs(sel_mcs) : srslte_ra_dl_mod_from_mcs(sel_mcs, use_tbs_index_alt);
    Qm           = SRSLTE_MIN(max_Qm, srslte_mod_bits_x_symbol(mod));
    eff_coderate = coderate / Qm;
  } while ((sel_mcs > 0 && coderate > max_coderate) || eff_coderate > 0.930);

  if (mcs != nullptr) {
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
  ue_cc_idx(ue_cc_idx_),
  harq_ent(SCHED_MAX_HARQ_PROC, SCHED_MAX_HARQ_PROC)
{
  // only PCell starts active. Remaining ones wait for valid CQI
  active    = ue_cc_idx == 0;
  dl_cqi_rx = false;
  dl_cqi    = (ue_cc_idx == 0) ? cell_params->cfg.initial_dl_cqi : 0;

  // set max mcs
  max_mcs_ul = cell_params->sched_cfg->pusch_max_mcs >= 0 ? cell_params->sched_cfg->pusch_max_mcs : 28;
  max_mcs_dl = cell_params->sched_cfg->pdsch_max_mcs >= 0 ? cell_params->sched_cfg->pdsch_max_mcs : 28;
  max_mcs_dl_alt =
      cell_params->sched_cfg->pdsch_max_mcs >= 0 ? SRSLTE_MIN(cell_params->sched_cfg->pdsch_max_mcs, 27) : 27;
  max_aggr_level = cell_params->sched_cfg->max_aggr_level >= 0 ? cell_params->sched_cfg->max_aggr_level : 3;

  // set fixed mcs
  fixed_mcs_dl = cell_params->sched_cfg->pdsch_mcs;
  fixed_mcs_ul = cell_params->sched_cfg->pusch_mcs;

  // Generate allowed CCE locations
  for (int cfi = 0; cfi < 3; cfi++) {
    for (int sf_idx = 0; sf_idx < 10; sf_idx++) {
      sched_utils::generate_cce_location(cell_params->regs.get(), &dci_locations[cfi][sf_idx], cfi + 1, sf_idx, rnti);
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
  harq_ent.reset();
}

void sched_ue_carrier::set_cfg(const sched_interface::ue_cfg_t& cfg_)
{
  if (cfg != nullptr and cfg->maxharq_tx == cfg_.maxharq_tx) {
    // nothing changed
    return;
  }
  cfg = &cfg_;
  // Config HARQ processes
  harq_ent.set_cfg(cfg->maxharq_tx);
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
  uint32_t max_mcs = is_ul ? max_mcs_ul : (cfg->use_tbs_index_alt) ? max_mcs_dl_alt : max_mcs_dl;
  uint32_t max_Qm  = is_ul ? 4 : 6; // Allow 16-QAM in PUSCH Only

  // TODO: Compute real spectral efficiency based on PUSCH-UCI configuration
  int tbs_bytes =
      sched_ue::cqi_to_tbs(cqi, nof_prb, nof_re, max_mcs, max_Qm, cfg->use_tbs_index_alt, is_ul, &sel_mcs) / 8;

  /* If less bytes are requested, lower the MCS */
  if (tbs_bytes > (int)req_bytes && req_bytes > 0) {
    int req_tbs_idx = srslte_ra_tbs_to_table_idx(req_bytes * 8, nof_prb);
    int req_mcs     = srslte_ra_mcs_from_tbs_idx(req_tbs_idx, cfg->use_tbs_index_alt, is_ul);

    if (req_mcs < (int)sel_mcs) {
      sel_mcs   = req_mcs;
      tbs_bytes = srslte_ra_tbs_from_idx(req_tbs_idx, nof_prb) / 8;
    }
  }
  // Avoid the unusual case n_prb=1, mcs=6 tbs=328 (used in voip)
  if (nof_prb == 1 && sel_mcs == 6) {
    sel_mcs--;
    uint32_t tbs_idx = srslte_ra_tbs_idx_from_mcs(sel_mcs, cfg->use_tbs_index_alt, is_ul);
    tbs_bytes        = srslte_ra_tbs_from_idx(tbs_idx, nof_prb) / 8;
  }

  if (mcs != nullptr && tbs_bytes >= 0) {
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

int sched_ue_carrier::get_required_prb_dl(uint32_t req_bytes, uint32_t nof_ctrl_symbols)
{
  int      mcs    = 0;
  uint32_t nof_re = 0;
  int      tbs    = 0;

  uint32_t nbytes = 0;
  uint32_t n;
  for (n = 0; n < cell_params->nof_prb() and nbytes < req_bytes; ++n) {
    nof_re = srslte_ra_dl_approx_nof_re(&cell_params->cfg.cell, n + 1, nof_ctrl_symbols);
    if (fixed_mcs_dl < 0 or not dl_cqi_rx) {
      tbs = alloc_tbs_dl(n + 1, nof_re, 0, &mcs);
    } else {
      tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(fixed_mcs_dl, cfg->use_tbs_index_alt, false), n + 1) / 8;
    }
    if (tbs > 0) {
      nbytes = tbs;
    } else if (tbs < 0) {
      return 0;
    }
  }

  return (nbytes >= req_bytes) ? n : -1;
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
      tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(fixed_mcs_ul, false, true), n) / 8;
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

void sched_ue_carrier::set_dl_cqi(uint32_t tti_tx_dl, uint32_t dl_cqi_)
{
  dl_cqi     = dl_cqi_;
  dl_cqi_tti = tti_tx_dl;
  dl_cqi_rx  = dl_cqi_rx or dl_cqi > 0;
  if (ue_cc_idx > 0 and active != cfg->supported_cc_list[ue_cc_idx].active) {
    if (dl_cqi_rx) {
      active = cfg->supported_cc_list[ue_cc_idx].active;
      log_h->info("SCell index=%d is now %s\n", ue_cc_idx, active ? "active" : "inactive");
    }
  }
}

} // namespace srsenb
