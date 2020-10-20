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

using srslte::tti_point;

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
  lch_handler.set_cfg(cfg_);

  // in case carriers have been removed
  while (carriers.size() > cfg.supported_cc_list.size()) {
    // TODO: distinguish cell deactivation from reconfiguration
    carriers.pop_back();
  }
  // in case carriers have been added or modified
  bool scell_activation_state_changed = false;
  for (uint32_t ue_idx = 0; ue_idx < cfg.supported_cc_list.size(); ++ue_idx) {
    auto& cc_cfg = cfg.supported_cc_list[ue_idx];

    if (ue_idx >= prev_supported_cc_list.size()) {
      // New carrier needs to be added
      carriers.emplace_back(cfg, (*cell_params_list)[cc_cfg.enb_cc_idx], rnti, ue_idx, current_tti);
    } else if (cc_cfg.enb_cc_idx != prev_supported_cc_list[ue_idx].enb_cc_idx) {
      // One carrier was added in the place of another
      carriers[ue_idx] = cc_sched_ue{cfg, (*cell_params_list)[cc_cfg.enb_cc_idx], rnti, ue_idx, current_tti};
      if (ue_idx == 0) {
        log_h->info("SCHED: rnti=0x%x PCell is now enb_cc_idx=%d.\n", rnti, cc_cfg.enb_cc_idx);
      }
    } else {
      // The SCell internal configuration may have changed
      carriers[ue_idx].set_cfg(cfg);
    }
    scell_activation_state_changed |= ue_idx > 0 and (carriers[ue_idx].cc_state() == cc_st::activating or
                                                      carriers[ue_idx].cc_state() == cc_st::deactivating);
  }
  if (scell_activation_state_changed) {
    pending_ces.emplace_back(srslte::dl_sch_lcid::SCELL_ACTIVATION);
    log_h->info("SCHED: Enqueueing SCell Activation CMD for rnti=0x%x\n", rnti);
  }

  check_ue_cfg_correctness();
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
    lch_handler.config_lcid(i, {});
  }
}

void sched_ue::new_tti(srslte::tti_point new_tti)
{
  current_tti = new_tti;

  lch_handler.new_tti();
}

/// sanity check the UE CC configuration
void sched_ue::check_ue_cfg_correctness() const
{
  using cc_t             = sched::ue_cfg_t::cc_cfg_t;
  const auto& cc_list    = cfg.supported_cc_list;
  bool        has_scells = std::count_if(cc_list.begin(), cc_list.end(), [](const cc_t& c) { return c.active; }) > 1;

  if (has_scells) {
    // In case of CA, CQI configs must exist and cannot collide in the PUCCH
    for (uint32_t i = 0; i < cc_list.size(); ++i) {
      const auto& cc1 = cc_list[i];
      if (not cc1.active) {
        continue;
      }
      if (not cc1.dl_cfg.cqi_report.periodic_configured and not cc1.dl_cfg.cqi_report.aperiodic_configured) {
        log_h->warning("SCHED: No CQI configuration was provided for UE scell index=%d \n", i);
      } else if (cc1.dl_cfg.cqi_report.periodic_configured) {
        for (uint32_t j = i + 1; j < cc_list.size(); ++j) {
          if (cc_list[j].active and cc_list[j].dl_cfg.cqi_report.periodic_configured and
              cc_list[j].dl_cfg.cqi_report.pmi_idx == cc1.dl_cfg.cqi_report.pmi_idx) {
            log_h->warning(
                "SCHED: The provided CQI configurations for UE scells %d and %d collide in time resources.\n", i, j);
          }
        }
      }
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
  lch_handler.config_lcid(lc_id, *cfg_);
}

void sched_ue::rem_bearer(uint32_t lc_id)
{
  cfg.ue_bearers[lc_id] = sched_interface::ue_bearer_cfg_t{};
  lch_handler.config_lcid(lc_id, sched_interface::ue_bearer_cfg_t{});
}

void sched_ue::phy_config_enabled(uint32_t tti, bool enabled)
{
  for (cc_sched_ue& c : carriers) {
    c.dl_cqi_tti = tti;
  }
  phy_config_dedicated_enabled = enabled;
}

void sched_ue::ul_buffer_state(uint8_t lcg_id, uint32_t bsr)
{
  lch_handler.ul_bsr(lcg_id, bsr);
}

void sched_ue::ul_buffer_add(uint8_t lcid, uint32_t bytes)
{
  lch_handler.ul_buffer_add(lcid, bytes);
}

void sched_ue::ul_phr(int phr)
{
  power_headroom = phr;
}

void sched_ue::dl_buffer_state(uint8_t lc_id, uint32_t tx_queue, uint32_t retx_queue)
{
  lch_handler.dl_buffer_state(lc_id, tx_queue, retx_queue);
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

bool sched_ue::pucch_sr_collision(uint32_t tti, uint32_t n_cce)
{
  if (!phy_config_dedicated_enabled) {
    return false;
  }
  if (cfg.pucch_cfg.sr_configured && srslte_ue_ul_sr_send_tti(&cfg.pucch_cfg, tti)) {
    return (n_cce + cfg.pucch_cfg.N_pucch_1) == cfg.pucch_cfg.n_pucch_sr;
  }
  return false;
}

int sched_ue::set_ack_info(uint32_t tti_rx, uint32_t enb_cc_idx, uint32_t tb_idx, bool ack)
{
  int          tbs_acked = -1;
  cc_sched_ue* c         = find_ue_carrier(enb_cc_idx);
  if (c != nullptr and c->cc_state() != cc_st::idle) {
    std::pair<uint32_t, int> p2 = c->harq_ent.set_ack_info(tti_rx, tb_idx, ack);
    tbs_acked                   = p2.second;
    if (tbs_acked > 0) {
      Debug("SCHED: Set DL ACK=%d for rnti=0x%x, pid=%d, tb=%d, tti=%d\n", ack, rnti, p2.first, tb_idx, tti_rx);
    } else {
      Warning("SCHED: Received ACK info for unknown TTI=%d\n", tti_rx);
    }
  } else {
    log_h->warning("Received DL ACK for invalid cell index %d\n", enb_cc_idx);
  }
  return tbs_acked;
}

void sched_ue::set_ul_crc(srslte::tti_point tti_rx, uint32_t enb_cc_idx, bool crc_res)
{
  cc_sched_ue* c = find_ue_carrier(enb_cc_idx);
  if (c != nullptr and c->cc_state() != cc_st::idle) {
    auto ret = c->harq_ent.set_ul_crc(tti_rx, 0, crc_res);
    if (not ret.first) {
      log_h->warning("Received UL CRC for invalid tti_rx=%d\n", (int)tti_rx.to_uint());
    }
  } else {
    log_h->warning("Received UL CRC for invalid cell index %d\n", enb_cc_idx);
  }
}

void sched_ue::set_dl_ri(uint32_t tti, uint32_t enb_cc_idx, uint32_t ri)
{
  cc_sched_ue* c = find_ue_carrier(enb_cc_idx);
  if (c != nullptr and c->cc_state() != cc_st::idle) {
    c->dl_ri     = ri;
    c->dl_ri_tti = tti;
  } else {
    log_h->warning("Received DL RI for invalid cell index %d\n", enb_cc_idx);
  }
}

void sched_ue::set_dl_pmi(uint32_t tti, uint32_t enb_cc_idx, uint32_t pmi)
{
  cc_sched_ue* c = find_ue_carrier(enb_cc_idx);
  if (c != nullptr and c->cc_state() != cc_st::idle) {
    c->dl_pmi     = pmi;
    c->dl_pmi_tti = tti;
  } else {
    log_h->warning("Received DL PMI for invalid cell index %d\n", enb_cc_idx);
  }
}

void sched_ue::set_dl_cqi(uint32_t tti, uint32_t enb_cc_idx, uint32_t cqi)
{
  cc_sched_ue* c = find_ue_carrier(enb_cc_idx);
  if (c != nullptr and c->cc_state() != cc_st::idle) {
    c->set_dl_cqi(tti, cqi);
  } else {
    log_h->warning("Received DL CQI for invalid enb cell index %d\n", enb_cc_idx);
  }
}

void sched_ue::set_ul_cqi(uint32_t tti, uint32_t enb_cc_idx, uint32_t cqi, uint32_t ul_ch_code)
{
  cc_sched_ue* c = find_ue_carrier(enb_cc_idx);
  if (c != nullptr and c->cc_state() != cc_st::idle) {
    c->ul_cqi     = cqi;
    c->ul_cqi_tti = tti;
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
    uint32_t alloc_sdu_bytes = lch_handler.alloc_rlc_pdu(&data->pdu[tbidx][data->nof_pdu_elems[tbidx]], max_sdu_bytes);
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

  // If the size of Format1 and Format1A is ambiguous in the common SS, use Format1A since the UE assumes
  // Common SS when spaces collide
  if (cell.nof_prb == 15 && carriers.size() > 1) {
    dci->alloc_type       = SRSLTE_RA_ALLOC_TYPE2;
    dci->type2_alloc.mode = srslte_ra_type2_t::SRSLTE_RA_TYPE2_LOC;
    rbg_interval rbg_int  = rbg_interval::rbgmask_to_rbgs(user_mask);
    uint32_t     P        = srslte_ra_type0_P(15);
    prb_interval prb_int  = prb_interval::rbgs_to_prbs(rbg_int, P);
    dci->type2_alloc.riv =
        srslte_ra_type2_to_riv(SRSLTE_MIN(prb_int.length(), cell.nof_prb), prb_int.start(), cell.nof_prb);
    dci->format = SRSLTE_DCI_FORMAT1A;
    if (prb_int.length() != P * user_mask.count()) {
      // This happens if Type0 was using distributed allocation
      Warning("SCHED: Can't use distributed RA due to DCI size ambiguity\n");
    }
  } else {
    dci->alloc_type              = SRSLTE_RA_ALLOC_TYPE0;
    dci->type0_alloc.rbg_bitmask = (uint32_t)user_mask.to_uint64();
    dci->format                  = SRSLTE_DCI_FORMAT1;
  }

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
  int                        mcs = 0, tbs_bytes = 0;
  srslte::interval<uint32_t> req_bytes = get_requested_dl_bytes(ue_cc_idx);

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
    tbs_bytes = carriers[ue_cc_idx].alloc_tbs_dl(nof_alloc_prbs, nof_re, req_bytes.stop(), &mcs);
  } else {
    // Fixed MCS
    mcs       = carriers[ue_cc_idx].fixed_mcs_dl;
    tbs_bytes = sched_utils::get_tbs_bytes(
        (uint32_t)carriers[ue_cc_idx].fixed_mcs_dl, nof_alloc_prbs, cfg.use_tbs_index_alt, false);
  }

  // If the number of prbs is not sufficient to fit minimum required bytes, increase the mcs
  // NOTE: this may happen during ConRes CE tx when DL-CQI is still not available
  while (tbs_bytes > 0 and (uint32_t) tbs_bytes < req_bytes.start() and mcs < 28) {
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
                               uint32_t                          ue_cc_idx,
                               prb_interval                      alloc,
                               bool                              needs_pdcch,
                               srslte_dci_location_t             dci_pos,
                               int                               explicit_mcs,
                               uci_pusch_t                       uci_type)
{
  ul_harq_proc*    h   = get_ul_harq(tti, ue_cc_idx);
  srslte_dci_ul_t* dci = &data->dci;

  bool cqi_request = needs_cqi_unlocked(tti, true);

  // Set DCI position
  data->needs_pdcch = needs_pdcch;
  dci->location     = dci_pos;

  int mcs = (explicit_mcs >= 0) ? explicit_mcs : carriers[ue_cc_idx].fixed_mcs_ul;
  int tbs = 0;

  bool is_newtx = h->is_empty(0);
  if (is_newtx) {
    uint32_t nof_retx;

    // If Msg3 set different nof retx
    nof_retx = (data->needs_pdcch) ? get_max_retx() : max_msg3retx;

    if (mcs >= 0) {
      tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(mcs, false, true), alloc.length()) / 8;
    } else {
      // dynamic mcs
      uint32_t req_bytes = get_pending_ul_new_data(tti, ue_cc_idx);
      uint32_t N_srs     = 0;
      uint32_t nof_symb  = 2 * (SRSLTE_CP_NSYMB(cell.cp) - 1) - N_srs;
      uint32_t nof_re    = nof_symb * alloc.length() * SRSLTE_NRE;
      tbs                = carriers[ue_cc_idx].alloc_tbs_ul(alloc.length(), nof_re, req_bytes, &mcs);

      // Reduce MCS to fit UCI if transmitted in this grant
      if (uci_type != UCI_PUSCH_NONE) {
        // Calculate an approximation of the number of RE used by UCI
        uint32_t nof_uci_re = 0;
        // Add the RE for ACK
        if (uci_type == UCI_PUSCH_ACK || uci_type == UCI_PUSCH_ACK_CQI) {
          float beta = srslte_sch_beta_ack(cfg.uci_offset.I_offset_ack);
          nof_uci_re += srslte_qprime_ack_ext(alloc.length(), nof_symb, 8 * tbs, carriers.size(), beta);
        }
        // Add the RE for CQI report (RI reports are transmitted on CQI slots. We do a conservative estimate here)
        if (uci_type == UCI_PUSCH_CQI || uci_type == UCI_PUSCH_ACK_CQI || cqi_request) {
          float beta = srslte_sch_beta_cqi(cfg.uci_offset.I_offset_cqi);
          nof_uci_re += srslte_qprime_cqi_ext(alloc.length(), nof_symb, 8 * tbs, beta);
        }
        // Recompute again the MCS and TBS with the new spectral efficiency (based on the available RE for data)
        if (nof_re >= nof_uci_re) {
          tbs = carriers[ue_cc_idx].alloc_tbs_ul(alloc.length(), nof_re - nof_uci_re, req_bytes, &mcs);
        }
        // NOTE: if (nof_re < nof_uci_re) we should set TBS=0
      }
    }
    h->new_tx(tti, mcs, tbs, alloc, nof_retx);
    // Un-trigger the SR if data is allocated
    if (tbs > 0) {
      unset_sr();
    }
  } else {
    // retx
    h->new_retx(0, tti, &mcs, nullptr, alloc);
    tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(mcs, false, true), alloc.length()) / 8;
  }

  if (tbs >= 0) {
    data->tbs        = tbs;
    dci->rnti        = rnti;
    dci->format      = SRSLTE_DCI_FORMAT0;
    dci->ue_cc_idx   = ue_cc_idx;
    dci->tb.ndi      = h->get_ndi(0);
    dci->cqi_request = cqi_request;
    dci->freq_hop_fl = srslte_dci_ul_t::SRSLTE_RA_PUSCH_HOP_DISABLED;
    dci->tpc_pusch   = next_tpc_pusch;
    next_tpc_pusch   = 1;

    dci->type2_alloc.riv = srslte_ra_type2_to_riv(alloc.length(), alloc.start(), cell.nof_prb);

    // If there are no RE available for ULSCH but there is UCI to transmit, allocate PUSCH becuase
    // resources have been reserved already and in CA it will be used to ACK other carriers
    if (tbs == 0 && (cqi_request || uci_type != UCI_PUSCH_NONE)) {
      // 8.6.1 and 8.6.2 36.213 second paragraph
      dci->cqi_request = true;
      dci->tb.mcs_idx  = 29;
      dci->tb.rv       = 0; // No data is being transmitted

      // Empty TBS PUSCH only accepts a maximum of 4 PRB. Resize the grant. This doesn't affect the MCS selection
      // because there is no TB in this grant
      if (alloc.length() > 4) {
        alloc.set(alloc.start(), alloc.start() + 4);
      }
    } else if (tbs > 0) {
      dci->tb.rv = sched_utils::get_rvidx(h->nof_retx(0));
      if (!is_newtx && h->is_adaptive_retx()) {
        dci->tb.mcs_idx = 28 + dci->tb.rv;
      } else {
        dci->tb.mcs_idx = mcs;
      }
    } else if (tbs == 0) {
      log_h->warning("SCHED: No space for ULSCH while allocating format0. Discarding grant.\n");
    } else {
      log_h->error("SCHED: Unkown error while allocating format0\n");
    }
  }

  return tbs;
}

/*******************************************************
 *
 * Functions used by scheduler or scheduler metric objects
 *
 *******************************************************/

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
  if (phy_config_dedicated_enabled && cfg.supported_cc_list[0].aperiodic_cqi_period && get_pending_dl_new_data() > 0) {
    uint32_t interval = srslte_tti_interval(tti, carriers[cc_idx].dl_cqi_tti);
    bool     needscqi = interval >= cfg.supported_cc_list[0].aperiodic_cqi_period;
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
rbg_interval sched_ue::get_required_dl_rbgs(uint32_t ue_cc_idx)
{
  srslte::interval<uint32_t> req_bytes = get_requested_dl_bytes(ue_cc_idx);
  if (req_bytes == srslte::interval<uint32_t>{0, 0}) {
    return {0, 0};
  }
  const auto* cellparams = carriers[ue_cc_idx].get_cell_cfg();
  int         pending_prbs =
      carriers[ue_cc_idx].get_required_prb_dl(req_bytes.start(), cellparams->sched_cfg->max_nof_ctrl_symbols);
  if (pending_prbs < 0) {
    // Cannot fit allocation in given PRBs
    log_h->error("SCHED: DL CQI=%d does now allow fitting %d non-segmentable DL tx bytes into the cell bandwidth. "
                 "Consider increasing initial CQI value.\n",
                 carriers[ue_cc_idx].dl_cqi,
                 req_bytes.start());
    return {cellparams->nof_prb(), cellparams->nof_prb()};
  }
  uint32_t min_pending_rbg = cellparams->prb_to_rbg(pending_prbs);
  pending_prbs = carriers[ue_cc_idx].get_required_prb_dl(req_bytes.stop(), cellparams->sched_cfg->max_nof_ctrl_symbols);
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
srslte::interval<uint32_t> sched_ue::get_requested_dl_bytes(uint32_t ue_cc_idx)
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
  if (not lch_handler.is_bearer_dl(0)) {
    log_h->error("SRB0 must always be activated for DL\n");
    return {};
  }
  if (carriers[ue_cc_idx].cc_state() != cc_st::active) {
    return {};
  }

  uint32_t max_data = 0, min_data = 0;
  uint32_t srb0_data = 0, rb_data = 0, sum_ce_data = 0;
  bool     is_dci_format1 = get_dci_format() == SRSLTE_DCI_FORMAT1;
  if (is_dci_format1) {
    srb0_data += compute_sdu_total_bytes(0, lch_handler.get_dl_retx(0));
    srb0_data += compute_sdu_total_bytes(0, lch_handler.get_dl_tx(0));
  }
  // Add pending CEs
  if (ue_cc_idx == 0) {
    if (srb0_data == 0 and not pending_ces.empty() and pending_ces.front() == srslte::dl_sch_lcid::CON_RES_ID) {
      // Wait for SRB0 data to be available for Msg4 before scheduling the ConRes CE
      return {};
    }
    for (const ce_cmd& ce : pending_ces) {
      sum_ce_data += srslte::ce_total_size(ce);
    }
  }
  // Add pending data in remaining RLC buffers
  for (int i = 1; i < sched_interface::MAX_LC; i++) {
    if (lch_handler.is_bearer_dl(i)) {
      rb_data += compute_sdu_total_bytes(i, lch_handler.get_dl_retx(i));
      rb_data += compute_sdu_total_bytes(i, lch_handler.get_dl_tx(i));
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
  if (std::none_of(
          carriers.begin(), carriers.end(), [](const cc_sched_ue& cc) { return cc.cc_state() == cc_st::active; })) {
    return 0;
  }

  uint32_t pending_data = 0;
  for (int i = 0; i < sched_interface::MAX_LC; i++) {
    if (lch_handler.is_bearer_dl(i)) {
      pending_data += lch_handler.get_dl_tx(i) + lch_handler.get_dl_retx(i);
    }
  }
  for (auto& ce : pending_ces) {
    pending_data += srslte::ce_total_size(ce);
  }
  return pending_data;
}

uint32_t sched_ue::get_pending_ul_old_data(uint32_t cc_idx)
{
  return get_pending_ul_old_data_unlocked(cc_idx);
}

uint32_t sched_ue::get_pending_ul_new_data(uint32_t tti, int this_ue_cc_idx)
{
  static constexpr uint32_t lbsr_size = 4, sbsr_size = 2;

  // Note: If there are no active bearers, scheduling requests are also ignored.
  uint32_t pending_data = 0;
  uint32_t active_lcgs = 0, pending_lcgs = 0;
  for (int i = 0; i < sched_interface::MAX_LC_GROUP; i++) {
    if (lch_handler.is_bearer_ul(i)) {
      int bsr = lch_handler.get_bsr(i);
      pending_data += bsr;
      active_lcgs++;
      pending_lcgs += (bsr > 0) ? 1 : 0;
    }
  }
  if (pending_data > 0) {
    // The scheduler needs to account for the possibility of BSRs being allocated in the UL grant.
    // Otherwise, the UL grants allocated for very small RRC messages (e.g. rrcReconfigurationComplete)
    // may be fully occupied by a BSR, and RRC the message transmission needs to be postponed.
    pending_data += (pending_lcgs <= 1) ? sbsr_size : lbsr_size;
  } else {
    if (is_sr_triggered() and active_lcgs > 0 and this_ue_cc_idx >= 0) {
      // Check if this_cc_idx is the carrier with highest CQI
      uint32_t max_cqi = 0, max_cc_idx = 0;
      for (uint32_t cc_idx = 0; cc_idx < carriers.size(); ++cc_idx) {
        uint32_t sum_cqi = carriers[cc_idx].dl_cqi + carriers[cc_idx].ul_cqi;
        if (carriers[cc_idx].cc_state() == cc_st::active and sum_cqi > max_cqi) {
          max_cqi    = sum_cqi;
          max_cc_idx = cc_idx;
        }
      }
      if ((int)max_cc_idx == this_ue_cc_idx) {
        return 512;
      }
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
    Debug("SCHED: pending_data=%d, pending_ul_data=%d, bsr=%s\n",
          pending_data,
          pending_ul_data,
          lch_handler.get_bsr_text().c_str());
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

bool sched_ue::is_sr_triggered()
{
  return sr;
}

/* Gets HARQ process with oldest pending retx */
dl_harq_proc* sched_ue::get_pending_dl_harq(uint32_t tti_tx_dl, uint32_t ue_cc_idx)
{
  if (ue_cc_idx < carriers.size() and carriers[ue_cc_idx].cc_state() == cc_st::active) {
    return carriers[ue_cc_idx].harq_ent.get_pending_dl_harq(tti_tx_dl);
  }
  return nullptr;
}

dl_harq_proc* sched_ue::get_empty_dl_harq(uint32_t tti_tx_dl, uint32_t ue_cc_idx)
{
  if (ue_cc_idx < carriers.size() and carriers[ue_cc_idx].cc_state() == cc_st::active) {
    return carriers[ue_cc_idx].harq_ent.get_empty_dl_harq(tti_tx_dl);
  }
  return nullptr;
}

ul_harq_proc* sched_ue::get_ul_harq(uint32_t tti_tx_ul, uint32_t ue_cc_idx)
{
  if (ue_cc_idx < carriers.size() and carriers[ue_cc_idx].cc_state() == cc_st::active) {
    return carriers[ue_cc_idx].harq_ent.get_ul_harq(tti_tx_ul);
  }
  return nullptr;
}

const dl_harq_proc& sched_ue::get_dl_harq(uint32_t idx, uint32_t ue_cc_idx) const
{
  return carriers[ue_cc_idx].harq_ent.dl_harq_procs()[idx];
}

std::pair<bool, uint32_t> sched_ue::get_active_cell_index(uint32_t enb_cc_idx) const
{
  auto it = std::find_if(
      cfg.supported_cc_list.begin(),
      cfg.supported_cc_list.end(),
      [enb_cc_idx](const sched_interface::ue_cfg_t::cc_cfg_t& u) { return u.enb_cc_idx == enb_cc_idx and u.active; });
  if (it != cfg.supported_cc_list.end()) {
    uint32_t ue_cc_idx = std::distance(cfg.supported_cc_list.begin(), it);
    return {carriers[ue_cc_idx].cc_state() == cc_st::active, ue_cc_idx};
  }
  return {false, std::numeric_limits<uint32_t>::max()};
}

uint32_t sched_ue::get_aggr_level(uint32_t ue_cc_idx, uint32_t nof_bits)
{
  return carriers[ue_cc_idx].get_aggr_level(nof_bits);
}

void sched_ue::finish_tti(const tti_params_t& tti_params, uint32_t enb_cc_idx)
{
  cc_sched_ue* c = find_ue_carrier(enb_cc_idx);

  if (c != nullptr) {
    // Check that scell state needs to change
    c->finish_tti(current_tti);
  }
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
    return &carriers[get_active_cell_index(enb_cc_idx).second].dci_locations[cfi - 1][sf_idx];
  } else {
    Error("SCHED: Invalid CFI=%d\n", cfi);
    return &carriers[get_active_cell_index(enb_cc_idx).second].dci_locations[0][sf_idx];
  }
}

cc_sched_ue* sched_ue::find_ue_carrier(uint32_t enb_cc_idx)
{
  auto it = std::find_if(carriers.begin(), carriers.end(), [enb_cc_idx](const cc_sched_ue& c) {
    return c.get_cell_cfg()->enb_cc_idx == enb_cc_idx;
  });
  return it != carriers.end() ? &(*it) : nullptr;
}

int cc_sched_ue::cqi_to_tbs(uint32_t nof_prb, uint32_t nof_re, bool use_tbs_index_alt, bool is_ul, uint32_t* mcs)
{

  uint32_t cqi     = is_ul ? ul_cqi : dl_cqi;
  uint32_t max_mcs = is_ul ? max_mcs_ul : (cfg->use_tbs_index_alt) ? max_mcs_dl_alt : max_mcs_dl;
  uint32_t max_Qm  = is_ul and not ul_64qam_enabled ? 4 : (not is_ul and use_tbs_index_alt ? 8 : 6);

  // Take the upper bound code-rate
  float    max_coderate = srslte_cqi_to_coderate(std::min(cqi + 1u, 15u), use_tbs_index_alt);
  int      sel_mcs      = max_mcs + 1;
  float    coderate     = 99;
  int      tbs          = 0;
  uint32_t Qm           = 0;

  do {
    sel_mcs--;
    uint32_t tbs_idx = srslte_ra_tbs_idx_from_mcs(sel_mcs, use_tbs_index_alt, is_ul);
    tbs              = srslte_ra_tbs_from_idx(tbs_idx, nof_prb);
    coderate         = srslte_coderate(tbs, nof_re);
    srslte_mod_t mod =
        (is_ul) ? srslte_ra_ul_mod_from_mcs(sel_mcs) : srslte_ra_dl_mod_from_mcs(sel_mcs, use_tbs_index_alt);
    Qm = SRSLTE_MIN(max_Qm, srslte_mod_bits_x_symbol(mod));
  } while (sel_mcs > 0 && coderate > SRSLTE_MIN(max_coderate, 0.930 * Qm));

  if (mcs != nullptr) {
    *mcs = (uint32_t)sel_mcs;
  }

  // If coderate > SRSLTE_MIN(max_coderate, 0.930 * Qm) we should set TBS=0. We don't because it's not correctly
  // handled by the scheduler, but we might be scheduling undecodable codewords at very low SNR

  return tbs;
}

/************************************************************************************************
 *                                sched_ue::sched_ue_carrier
 ***********************************************************************************************/

cc_sched_ue::cc_sched_ue(const sched_interface::ue_cfg_t& cfg_,
                         const sched_cell_params_t&       cell_cfg_,
                         uint16_t                         rnti_,
                         uint32_t                         ue_cc_idx_,
                         tti_point                        current_tti) :
  cell_params(&cell_cfg_),
  rnti(rnti_),
  log_h(srslte::logmap::get("MAC ")),
  ue_cc_idx(ue_cc_idx_),
  last_tti(current_tti),
  harq_ent(SCHED_MAX_HARQ_PROC, SCHED_MAX_HARQ_PROC)
{
  dl_cqi_rx = false;
  dl_cqi    = (ue_cc_idx == 0) ? cell_params->cfg.initial_dl_cqi : 0;
  set_cfg(cfg_);

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
}

void cc_sched_ue::reset()
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

void cc_sched_ue::set_cfg(const sched_interface::ue_cfg_t& cfg_)
{
  cfg     = &cfg_;
  cfg_tti = last_tti;

  // Config HARQ processes
  harq_ent.set_cfg(cfg->maxharq_tx);

  if (ue_cc_idx == 0) {
    // PCell is always active
    cc_state_ = cc_st::active;
  } else {
    switch (cc_state()) {
      case cc_st::activating:
      case cc_st::active:
        if (not cfg->supported_cc_list[ue_cc_idx].active) {
          cc_state_ = cc_st::deactivating;
          log_h->info("SCHED: Deactivating rnti=0x%x, SCellIndex=%d...\n", rnti, ue_cc_idx);
        }
        break;
      case cc_st::deactivating:
      case cc_st::idle:
        if (cfg->supported_cc_list[ue_cc_idx].active) {
          cc_state_ = cc_st::activating;
          dl_cqi_rx = false;
          dl_cqi    = 0;
          log_h->info("SCHED: Activating rnti=0x%x, SCellIndex=%d...\n", rnti, ue_cc_idx);
        }
        break;
      default:
        break;
    }
  }
}

void cc_sched_ue::finish_tti(srslte::tti_point tti_rx)
{
  last_tti = tti_point{tti_rx};

  // reset PIDs with pending data or blocked
  harq_ent.reset_pending_data(last_tti);

  // Check if cell state needs to be updated
  if (ue_cc_idx > 0 and cc_state_ == cc_st::deactivating) {
    // wait for all ACKs to be received before completely deactivating SCell
    if (last_tti > srslte::to_tx_dl_ack(cfg_tti)) {
      cc_state_ = cc_st::idle;
      reset();
    }
  }
}

/* Find lowest DCI aggregation level supported by the UE spectral efficiency */
uint32_t cc_sched_ue::get_aggr_level(uint32_t nof_bits)
{
  uint32_t l            = 0;
  float    max_coderate = srslte_cqi_to_coderate(dl_cqi, cfg->use_tbs_index_alt);
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
int cc_sched_ue::alloc_tbs(uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes, bool is_ul, int* mcs)
{
  uint32_t sel_mcs = 0;

  // TODO: Compute real spectral efficiency based on PUSCH-UCI configuration
  int tbs_bytes = cqi_to_tbs(nof_prb, nof_re, cfg->use_tbs_index_alt, is_ul, &sel_mcs) / 8;

  /* If less bytes are requested, lower the MCS */
  if (tbs_bytes > (int)req_bytes && req_bytes > 0) {
    int req_tbs_idx = srslte_ra_tbs_to_table_idx(req_bytes * 8, nof_prb);
    int req_mcs     = srslte_ra_mcs_from_tbs_idx(req_tbs_idx, cfg->use_tbs_index_alt, is_ul);
    while (cfg->use_tbs_index_alt and req_mcs < 0 and req_tbs_idx < 33) {
      // some tbs_idx are invalid for 256QAM. See TS 36.213 - Table 7.1.7.1-1A
      req_mcs = srslte_ra_mcs_from_tbs_idx(++req_tbs_idx, cfg->use_tbs_index_alt, is_ul);
    }

    if (req_mcs >= 0 and req_mcs < (int)sel_mcs) {
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

int cc_sched_ue::alloc_tbs_dl(uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes, int* mcs)
{
  return alloc_tbs(nof_prb, nof_re, req_bytes, false, mcs);
}

int cc_sched_ue::alloc_tbs_ul(uint32_t nof_prb, uint32_t nof_re, uint32_t req_bytes, int* mcs)
{
  return alloc_tbs(nof_prb, nof_re, req_bytes, true, mcs);
}

int cc_sched_ue::get_required_prb_dl(uint32_t req_bytes, uint32_t nof_ctrl_symbols)
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

uint32_t cc_sched_ue::get_required_prb_ul(uint32_t req_bytes)
{
  int      mcs    = 0;
  uint32_t nbytes = 0;
  uint32_t N_srs  = 0;

  uint32_t n = 0;
  if (req_bytes == 0) {
    return 0;
  }

  uint32_t last_valid_n = 0;
  for (n = 1; n < cell_params->nof_prb() && nbytes < req_bytes + 4; n++) {
    uint32_t nof_re = (2 * (SRSLTE_CP_NSYMB(cell_params->cfg.cell.cp) - 1) - N_srs) * n * SRSLTE_NRE;
    int      tbs    = 0;
    if (fixed_mcs_ul < 0) {
      tbs = alloc_tbs_ul(n, nof_re, 0, &mcs);
    } else {
      tbs = srslte_ra_tbs_from_idx(srslte_ra_tbs_idx_from_mcs(fixed_mcs_ul, false, true), n) / 8;
    }
    if (tbs > 0) {
      nbytes       = tbs;
      last_valid_n = n;
    }
  }

  if (last_valid_n > 0) {
    if (n != last_valid_n) {
      n = last_valid_n;
    }
    while (!srslte_dft_precoding_valid_prb(n) && n <= cell_params->nof_prb()) {
      n++;
    }
    return n;
  } else {
    // This should never happen. Just in case, return 0 PRB and handle it later
    log_h->error("SCHED: Could not obtain any valid number of PRB for an uplink allocation\n");
    return 0;
  }
}

void cc_sched_ue::set_dl_cqi(uint32_t tti_tx_dl, uint32_t dl_cqi_)
{
  dl_cqi     = dl_cqi_;
  dl_cqi_tti = tti_tx_dl;
  dl_cqi_rx  = dl_cqi_rx or dl_cqi > 0;
  if (ue_cc_idx > 0 and cc_state_ == cc_st::activating and dl_cqi_rx) {
    // Wait for SCell to receive a positive CQI before activating it
    cc_state_ = cc_st::active;
    log_h->info("SCHED: SCell index=%d is now active\n", ue_cc_idx);
  }
}

/*******************************************************
 *
 *         Logical Channel Management
 *
 *******************************************************/

const char* to_string(sched_interface::ue_bearer_cfg_t::direction_t dir)
{
  switch (dir) {
    case sched_interface::ue_bearer_cfg_t::IDLE:
      return "idle";
    case sched_interface::ue_bearer_cfg_t::BOTH:
      return "bi-dir";
    case sched_interface::ue_bearer_cfg_t::DL:
      return "DL";
    case sched_interface::ue_bearer_cfg_t::UL:
      return "UL";
    default:
      return "unrecognized direction";
  }
}

void lch_manager::set_cfg(const sched_interface::ue_cfg_t& cfg)
{
  for (uint32_t lcid = 0; lcid < sched_interface::MAX_LC; lcid++) {
    config_lcid(lcid, cfg.ue_bearers[lcid]);
  }
}

void lch_manager::new_tti()
{
  prio_idx++;
  for (uint32_t lcid = 0; lcid < sched_interface::MAX_LC; ++lcid) {
    if (is_bearer_active(lcid)) {
      if (lch[lcid].cfg.pbr != pbr_infinity) {
        lch[lcid].Bj = std::min(lch[lcid].Bj + (int)(lch[lcid].cfg.pbr * tti_duration_ms), lch[lcid].bucket_size);
      }
    }
  }
}

void lch_manager::config_lcid(uint32_t lc_id, const sched_interface::ue_bearer_cfg_t& bearer_cfg)
{
  if (lc_id >= sched_interface::MAX_LC) {
    Warning("Adding bearer with invalid logical channel id=%d\n", lc_id);
    return;
  }
  if (bearer_cfg.group >= sched_interface::MAX_LC_GROUP) {
    Warning("Adding bearer with invalid logical channel group id=%d\n", bearer_cfg.group);
    return;
  }

  // update bearer config
  bool is_equal = memcmp(&bearer_cfg, &lch[lc_id].cfg, sizeof(bearer_cfg)) == 0;

  if (not is_equal) {
    lch[lc_id].cfg = bearer_cfg;
    if (lch[lc_id].cfg.pbr == pbr_infinity) {
      lch[lc_id].bucket_size = std::numeric_limits<int>::max();
      lch[lc_id].Bj          = std::numeric_limits<int>::max();
    } else {
      lch[lc_id].bucket_size = lch[lc_id].cfg.bsd * lch[lc_id].cfg.pbr;
      lch[lc_id].Bj          = 0;
    }
    Info("SCHED: bearer configured: lc_id=%d, mode=%s, prio=%d\n",
         lc_id,
         to_string(lch[lc_id].cfg.direction),
         lch[lc_id].cfg.priority);
  }
}

void lch_manager::ul_bsr(uint8_t lcg_id, uint32_t bsr)
{
  if (lcg_id >= sched_interface::MAX_LC_GROUP) {
    Warning("The provided logical channel group id=%d is not valid\n", lcg_id);
    return;
  }
  lcg_bsr[lcg_id] = bsr;
  Debug("SCHED: bsr=%d, lcg_id=%d, bsr=%s\n", bsr, lcg_id, get_bsr_text().c_str());
}

void lch_manager::ul_buffer_add(uint8_t lcid, uint32_t bytes)
{
  if (lcid >= sched_interface::MAX_LC) {
    Warning("The provided lcid=%d is not valid\n", lcid);
    return;
  }
  lcg_bsr[lch[lcid].cfg.group] += bytes;
  Debug("SCHED: UL buffer update=%d, lcg_id=%d, bsr=%s\n", bytes, lch[lcid].cfg.group, get_bsr_text().c_str());
}

void lch_manager::dl_buffer_state(uint8_t lcid, uint32_t tx_queue, uint32_t retx_queue)
{
  if (lcid >= sched_interface::MAX_LC) {
    Warning("The provided lcid=%d is not valid\n", lcid);
    return;
  }
  lch[lcid].buf_retx = retx_queue;
  lch[lcid].buf_tx   = tx_queue;
  Debug("SCHED: DL lcid=%d buffer_state=%d,%d\n", lcid, tx_queue, retx_queue);
}

int lch_manager::get_max_prio_lcid() const
{
  int min_prio_val = std::numeric_limits<int>::max(), prio_lcid = -1;

  // Prioritize retxs
  for (uint32_t lcid = 0; lcid < MAX_LC; ++lcid) {
    if (get_dl_retx(lcid) > 0 and lch[lcid].cfg.priority < min_prio_val) {
      min_prio_val = lch[lcid].cfg.priority;
      prio_lcid    = lcid;
    }
  }
  if (prio_lcid >= 0) {
    return prio_lcid;
  }

  // Select lcid with new txs using Bj
  for (uint32_t lcid = 0; lcid < MAX_LC; ++lcid) {
    if (get_dl_tx(lcid) > 0 and lch[lcid].Bj > 0 and lch[lcid].cfg.priority < min_prio_val) {
      min_prio_val = lch[lcid].cfg.priority;
      prio_lcid    = lcid;
    }
  }
  if (prio_lcid >= 0) {
    return prio_lcid;
  }

  // Disregard Bj
  size_t                       nof_lcids    = 0;
  std::array<uint32_t, MAX_LC> chosen_lcids = {};
  for (uint32_t lcid = 0; lcid < MAX_LC; ++lcid) {
    if (get_dl_tx_total(lcid) > 0) {
      if (lch[lcid].cfg.priority < min_prio_val) {
        min_prio_val    = lch[lcid].cfg.priority;
        chosen_lcids[0] = lcid;
        nof_lcids       = 1;
      } else if (lch[lcid].cfg.priority == min_prio_val) {
        chosen_lcids[nof_lcids++] = lcid;
      }
    }
  }
  // logical chanels with equal priority should be served equally
  if (nof_lcids > 0) {
    prio_lcid = chosen_lcids[prio_idx % nof_lcids];
  }

  return prio_lcid;
}

/// Allocates first available RLC PDU
int lch_manager::alloc_rlc_pdu(sched_interface::dl_sched_pdu_t* rlc_pdu, int rem_bytes)
{
  int alloc_bytes = 0;
  int lcid        = get_max_prio_lcid();
  if (lcid < 0) {
    return alloc_bytes;
  }

  // try first to allocate retxs
  alloc_bytes = alloc_retx_bytes(lcid, rem_bytes);

  // if no retx alloc, try newtx
  if (alloc_bytes == 0) {
    alloc_bytes = alloc_tx_bytes(lcid, rem_bytes);
  }

  if (alloc_bytes > 0) {
    rlc_pdu->nbytes = alloc_bytes;
    rlc_pdu->lcid   = lcid;
    Debug("SCHED: Allocated lcid=%d, nbytes=%d, tbs_bytes=%d\n", rlc_pdu->lcid, rlc_pdu->nbytes, rem_bytes);
  }
  return alloc_bytes;
}

int lch_manager::alloc_retx_bytes(uint8_t lcid, uint32_t rem_bytes)
{
  int alloc = std::min((int)rem_bytes, get_dl_retx(lcid));
  lch[lcid].buf_retx -= alloc;
  return alloc;
}

int lch_manager::alloc_tx_bytes(uint8_t lcid, uint32_t rem_bytes)
{
  int alloc = std::min((int)rem_bytes, get_dl_tx(lcid));
  lch[lcid].buf_tx -= alloc;
  if (alloc > 0 and lch[lcid].cfg.pbr != pbr_infinity) {
    // Update Bj
    lch[lcid].Bj -= alloc;
  }
  return alloc;
}

bool lch_manager::is_bearer_active(uint32_t lcid) const
{
  return lch[lcid].cfg.direction != sched_interface::ue_bearer_cfg_t::IDLE;
}

bool lch_manager::is_bearer_ul(uint32_t lcid) const
{
  return is_bearer_active(lcid) and lch[lcid].cfg.direction != sched_interface::ue_bearer_cfg_t::DL;
}

bool lch_manager::is_bearer_dl(uint32_t lcid) const
{
  return is_bearer_active(lcid) and lch[lcid].cfg.direction != sched_interface::ue_bearer_cfg_t::UL;
}

int lch_manager::get_dl_tx(uint32_t lcid) const
{
  return is_bearer_dl(lcid) ? lch[lcid].buf_tx : 0;
}
int lch_manager::get_dl_retx(uint32_t lcid) const
{
  return is_bearer_dl(lcid) ? lch[lcid].buf_retx : 0;
}
int lch_manager::get_bsr(uint32_t lcid) const
{
  return is_bearer_ul(lcid) ? lcg_bsr[lch[lcid].cfg.group] : 0;
}

std::string lch_manager::get_bsr_text() const
{
  std::stringstream ss;
  ss << "{" << lcg_bsr[0] << ", " << lcg_bsr[1] << ", " << lcg_bsr[2] << ", " << lcg_bsr[3] << "}";
  return ss.str();
}

} // namespace srsenb
