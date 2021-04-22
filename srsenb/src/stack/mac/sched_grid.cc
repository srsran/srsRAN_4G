/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsenb/hdr/stack/mac/sched_grid.h"
#include "srsenb/hdr/stack/mac/sched_helpers.h"
#include "srsran/common/string_helpers.h"

namespace srsenb {

const char* to_string(alloc_result result)
{
  switch (result) {
    case alloc_result::success:
      return "success";
    case alloc_result::sch_collision:
      return "Collision with existing SCH allocations";
    case alloc_result::other_cause:
      return "error";
    case alloc_result::no_cch_space:
      return "No space available in PUCCH or PDCCH";
    case alloc_result::no_sch_space:
      return "Requested number of PRBs not available";
    case alloc_result::no_rnti_opportunity:
      return "rnti cannot be allocated (e.g. already allocated, no data, meas gap collision, carrier inactive, etc.)";
    case alloc_result::invalid_grant_params:
      return "invalid grant arguments (e.g. invalid prb mask)";
    case alloc_result::invalid_coderate:
      return "Effective coderate exceeds threshold";
    case alloc_result::no_grant_space:
      return "Max number of allocations reached";
    default:
      break;
  }
  return "unknown error";
}

void sf_sched_result::new_tti(tti_point tti_rx_)
{
  assert(tti_rx != tti_rx_);
  tti_rx = tti_rx_;
  for (auto& cc : enb_cc_list) {
    cc = {};
  }
}

bool sf_sched_result::is_ul_alloc(uint16_t rnti) const
{
  for (const auto& cc : enb_cc_list) {
    for (const auto& pusch : cc.ul_sched_result.pusch) {
      if (pusch.dci.rnti == rnti) {
        return true;
      }
    }
  }
  return false;
}
bool sf_sched_result::is_dl_alloc(uint16_t rnti) const
{
  for (const auto& cc : enb_cc_list) {
    for (const auto& data : cc.dl_sched_result.data) {
      if (data.dci.rnti == rnti) {
        return true;
      }
    }
  }
  return false;
}

void sched_result_ringbuffer::set_nof_carriers(uint32_t nof_carriers_)
{
  nof_carriers = nof_carriers_;
  for (auto& sf_res : results) {
    sf_res.enb_cc_list.resize(nof_carriers_);
  }
}

void sched_result_ringbuffer::new_tti(srsran::tti_point tti_rx)
{
  sf_sched_result* res = &results[tti_rx.to_uint()];
  res->new_tti(tti_rx);
}

/*******************************************************
 *          TTI resource Scheduling Methods
 *******************************************************/

void sf_grid_t::init(const sched_cell_params_t& cell_params_)
{
  cc_cfg   = &cell_params_;
  nof_rbgs = cc_cfg->nof_rbgs;

  dl_mask.resize(nof_rbgs);
  ul_mask.resize(cc_cfg->nof_prb());

  pdcch_alloc.init(*cc_cfg);

  // Compute reserved PRBs for CQI, SR and HARQ-ACK, and store it in a bitmask
  pucch_mask.resize(cc_cfg->nof_prb());
  pucch_nrb                    = (cc_cfg->cfg.nrb_pucch > 0) ? (uint32_t)cc_cfg->cfg.nrb_pucch : 0;
  srsran_pucch_cfg_t pucch_cfg = cell_params_.pucch_cfg_common;
  pucch_cfg.n_pucch            = cc_cfg->nof_cce_table[SRSRAN_NOF_CFI - 1] - 1 + cc_cfg->cfg.n1pucch_an;
  pucch_nrb                    = std::max(pucch_nrb, srsran_pucch_m(&pucch_cfg, cc_cfg->cfg.cell.cp) / 2 + 1);
  if (pucch_nrb > 0) {
    pucch_mask.fill(0, pucch_nrb);
    pucch_mask.fill(cc_cfg->nof_prb() - pucch_nrb, cc_cfg->nof_prb());
  }
}

void sf_grid_t::new_tti(tti_point tti_rx_)
{
  tti_rx = tti_rx_;

  dl_mask.reset();
  ul_mask.reset();

  // Reserve PRBs for PUCCH
  ul_mask |= pucch_mask;

  // Reserve PRBs for PRACH
  if (srsran_prach_tti_opportunity_config_fdd(cc_cfg->cfg.prach_config, to_tx_ul(tti_rx).to_uint(), -1)) {
    prbmask_t prach_mask{cc_cfg->nof_prb()};
    prach_mask.fill(cc_cfg->cfg.prach_freq_offset, cc_cfg->cfg.prach_freq_offset + 6);
    reserve_ul_prbs(prach_mask, false); // TODO: set to true once test sib.conf files are updated
    if (logger.debug.enabled()) {
      fmt::memory_buffer buffer;
      fmt::format_to(buffer, "SCHED: Allocated PRACH RBs mask={:x} for tti_tx_ul={}", prach_mask, to_tx_ul(tti_rx));
      logger.debug("%s", srsran::to_c_str(buffer));
    }
  }

  // internal state
  pdcch_alloc.new_tti(tti_rx);
}

//! Allocates CCEs and RBs for the given mask and allocation type (e.g. data, BC, RAR, paging)
alloc_result sf_grid_t::alloc_dl(uint32_t     aggr_idx,
                                 alloc_type_t alloc_type,
                                 rbgmask_t    alloc_mask,
                                 sched_ue*    user,
                                 bool         has_pusch_grant)
{
  // Check RBG collision
  if ((dl_mask & alloc_mask).any()) {
    logger.debug("SCHED: Provided RBG mask collides with allocation previously made.\n");
    return alloc_result::sch_collision;
  }

  // Allocate DCI in PDCCH
  if (not pdcch_alloc.alloc_dci(alloc_type, aggr_idx, user, has_pusch_grant)) {
    if (logger.debug.enabled()) {
      if (user != nullptr) {
        logger.debug("SCHED: No space in PDCCH for rnti=0x%x DL tx. Current PDCCH allocation:\n%s",
                     user->get_rnti(),
                     pdcch_alloc.result_to_string(true).c_str());
      } else {
        logger.debug("SCHED: No space in PDCCH for DL tx. Current PDCCH allocation:\n%s",
                     pdcch_alloc.result_to_string(true).c_str());
      }
    }
    return alloc_result::no_cch_space;
  }

  // Allocate RBGs
  dl_mask |= alloc_mask;

  return alloc_result::success;
}

/// Allocates CCEs and RBs for control allocs. It allocates RBs in a contiguous manner.
alloc_result sf_grid_t::alloc_dl_ctrl(uint32_t aggr_idx, rbg_interval rbg_range, alloc_type_t alloc_type)
{
  if (alloc_type != alloc_type_t::DL_RAR and alloc_type != alloc_type_t::DL_BC and
      alloc_type != alloc_type_t::DL_PCCH) {
    logger.error("SCHED: DL control allocations must be RAR/BC/PDCCH");
    return alloc_result::other_cause;
  }
  // Setup rbg_range starting from left
  if (rbg_range.stop() > nof_rbgs) {
    return alloc_result::sch_collision;
  }

  // allocate DCI and RBGs
  rbgmask_t new_mask(dl_mask.size());
  new_mask.fill(rbg_range.start(), rbg_range.stop());
  return alloc_dl(aggr_idx, alloc_type, new_mask);
}

//! Allocates CCEs and RBs for a user DL data alloc.
alloc_result sf_grid_t::alloc_dl_data(sched_ue* user, const rbgmask_t& user_mask, bool has_pusch_grant)
{
  srsran_dci_format_t dci_format = user->get_dci_format();
  uint32_t            nof_bits   = srsran_dci_format_sizeof(&cc_cfg->cfg.cell, nullptr, nullptr, dci_format);
  uint32_t            aggr_idx   = user->get_aggr_level(cc_cfg->enb_cc_idx, nof_bits);
  alloc_result        ret        = alloc_dl(aggr_idx, alloc_type_t::DL_DATA, user_mask, user, has_pusch_grant);

  return ret;
}

alloc_result sf_grid_t::alloc_ul_data(sched_ue* user, prb_interval alloc, bool needs_pdcch, bool strict)
{
  if (alloc.stop() > ul_mask.size()) {
    return alloc_result::no_sch_space;
  }

  prbmask_t newmask(ul_mask.size());
  newmask.fill(alloc.start(), alloc.stop());
  if (strict and (ul_mask & newmask).any()) {
    logger.debug("SCHED: Failed UL allocation. Cause: %s", to_string(alloc_result::sch_collision));
    return alloc_result::sch_collision;
  }

  // Generate PDCCH except for RAR and non-adaptive retx
  if (needs_pdcch) {
    uint32_t nof_bits = srsran_dci_format_sizeof(&cc_cfg->cfg.cell, nullptr, nullptr, SRSRAN_DCI_FORMAT0);
    uint32_t aggr_idx = user->get_aggr_level(cc_cfg->enb_cc_idx, nof_bits);
    if (not pdcch_alloc.alloc_dci(alloc_type_t::UL_DATA, aggr_idx, user)) {
      if (logger.debug.enabled()) {
        logger.debug("No space in PDCCH for rnti=0x%x UL tx. Current PDCCH allocation:\n%s",
                     user->get_rnti(),
                     pdcch_alloc.result_to_string(true).c_str());
      }
      return alloc_result::no_cch_space;
    }
  }

  ul_mask |= newmask;

  return alloc_result::success;
}

bool sf_grid_t::reserve_dl_rbgs(uint32_t start_rbg, uint32_t end_rbg)
{
  dl_mask.fill(start_rbg, end_rbg);
  return true;
}

void sf_grid_t::rem_last_alloc_dl(rbg_interval rbgs)
{
  if (pdcch_alloc.nof_allocs() == 0) {
    logger.error("Remove DL alloc called for empty Subframe RB grid");
    return;
  }

  pdcch_alloc.rem_last_dci();
  rbgmask_t rbgmask(dl_mask.size());
  rbgmask.fill(rbgs.start(), rbgs.stop());
  dl_mask &= ~rbgmask;
}

alloc_result sf_grid_t::reserve_ul_prbs(prb_interval alloc, bool strict)
{
  if (alloc.stop() > ul_mask.size()) {
    return alloc_result::invalid_grant_params;
  }

  prbmask_t newmask(ul_mask.size());
  newmask.fill(alloc.start(), alloc.stop());
  return reserve_ul_prbs(newmask, strict);
}

alloc_result sf_grid_t::reserve_ul_prbs(const prbmask_t& prbmask, bool strict)
{
  alloc_result ret = alloc_result::success;
  if (strict and (ul_mask & prbmask).any()) {
    if (logger.info.enabled()) {
      fmt::memory_buffer tmp_buffer;
      fmt::format_to(
          tmp_buffer, "There was a collision in the UL. Current mask=0x{:x}, new mask=0x{:x}", ul_mask, prbmask);
      logger.info("%s", srsran::to_c_str(tmp_buffer));
      ret = alloc_result::sch_collision;
    }
  }
  ul_mask |= prbmask;
  return ret;
}

/**
 * Finds a range of L contiguous PRBs that are empty
 * @param L Size of the requested UL allocation in PRBs
 * @param alloc Found allocation. It is guaranteed that 0 <= alloc->L <= L
 * @return true if the requested allocation of size L was strictly met
 */
bool sf_grid_t::find_ul_alloc(uint32_t L, prb_interval* alloc) const
{
  *alloc = {};
  for (uint32_t n = 0; n < ul_mask.size() && alloc->length() < L; n++) {
    if (not ul_mask.test(n) && alloc->length() == 0) {
      alloc->displace_to(n);
    }
    if (not ul_mask.test(n)) {
      alloc->resize_by(1);
    } else if (alloc->length() > 0) {
      // avoid edges
      if (n < 3) {
        *alloc = {};
      } else {
        break;
      }
    }
  }
  if (alloc->length() == 0) {
    return false;
  }

  // Make sure L is allowed by SC-FDMA modulation
  while (!srsran_dft_precoding_valid_prb(alloc->length())) {
    alloc->resize_by(-1);
  }
  return alloc->length() == L;
}

/*******************************************************
 *          TTI resource Scheduling Methods
 *******************************************************/

sf_sched::sf_sched() : logger(srslog::fetch_basic_logger("MAC")) {}

void sf_sched::init(const sched_cell_params_t& cell_params_)
{
  cc_cfg = &cell_params_;
  tti_alloc.init(*cc_cfg);
  max_msg3_prb = std::max(6U, cc_cfg->cfg.cell.nof_prb - tti_alloc.get_pucch_width());
}

void sf_sched::new_tti(tti_point tti_rx_, sf_sched_result* cc_results_)
{
  // reset internal state
  bc_allocs.clear();
  rar_allocs.clear();
  data_allocs.clear();
  ul_data_allocs.clear();

  tti_rx = tti_rx_;
  tti_alloc.new_tti(tti_rx_);
  cc_results = cc_results_;

  // setup first prb to be used for msg3 alloc. Account for potential PRACH alloc
  last_msg3_prb            = tti_alloc.get_pucch_width();
  tti_point tti_msg3_alloc = to_tx_ul(tti_rx) + MSG3_DELAY_MS;
  if (srsran_prach_tti_opportunity_config_fdd(cc_cfg->cfg.prach_config, tti_msg3_alloc.to_uint(), -1)) {
    last_msg3_prb = std::max(last_msg3_prb, cc_cfg->cfg.prach_freq_offset + 6);
  }
}

bool sf_sched::is_dl_alloc(uint16_t rnti) const
{
  return std::any_of(data_allocs.begin(), data_allocs.end(), [rnti](const dl_alloc_t& u) { return u.rnti == rnti; });
}

bool sf_sched::is_ul_alloc(uint16_t rnti) const
{
  return std::any_of(
      ul_data_allocs.begin(), ul_data_allocs.end(), [rnti](const ul_alloc_t& u) { return u.rnti == rnti; });
}

alloc_result sf_sched::alloc_sib(uint32_t aggr_lvl, uint32_t sib_idx, uint32_t sib_ntx, rbg_interval rbgs)
{
  if (bc_allocs.full()) {
    logger.warning("SCHED: Maximum number of Broadcast allocations reached");
    return alloc_result::no_grant_space;
  }
  bc_alloc_t bc_alloc;

  // Allocate SIB RBGs and PDCCH
  alloc_result ret = tti_alloc.alloc_dl_ctrl(aggr_lvl, rbgs, alloc_type_t::DL_BC);
  if (ret != alloc_result::success) {
    return ret;
  }

  // Generate DCI for SIB
  if (not generate_sib_dci(bc_alloc.bc_grant, get_tti_tx_dl(), sib_idx, sib_ntx, rbgs, *cc_cfg, tti_alloc.get_cfi())) {
    // Cancel on-going allocation
    tti_alloc.rem_last_alloc_dl(rbgs);
    return alloc_result::invalid_coderate;
  }

  // Allocation Successful
  bc_alloc.dci_idx   = tti_alloc.get_pdcch_grid().nof_allocs() - 1;
  bc_alloc.rbg_range = rbgs;
  bc_alloc.req_bytes = cc_cfg->cfg.sibs[sib_idx].len;
  bc_allocs.push_back(bc_alloc);

  return alloc_result::success;
}

alloc_result sf_sched::alloc_paging(uint32_t aggr_lvl, uint32_t paging_payload, rbg_interval rbgs)
{
  if (bc_allocs.full()) {
    logger.warning("SCHED: Maximum number of Broadcast allocations reached");
    return alloc_result::no_grant_space;
  }
  bc_alloc_t bc_alloc;

  // Allocate Paging RBGs and PDCCH
  alloc_result ret = tti_alloc.alloc_dl_ctrl(aggr_lvl, rbgs, alloc_type_t::DL_PCCH);
  if (ret != alloc_result::success) {
    return ret;
  }

  // Generate DCI for Paging message
  if (not generate_paging_dci(bc_alloc.bc_grant, get_tti_tx_dl(), paging_payload, rbgs, *cc_cfg, tti_alloc.get_cfi())) {
    // Cancel on-going allocation
    tti_alloc.rem_last_alloc_dl(rbgs);
    return alloc_result::invalid_coderate;
  }

  // Allocation Successful
  bc_alloc.dci_idx   = tti_alloc.get_pdcch_grid().nof_allocs() - 1;
  bc_alloc.rbg_range = rbgs;
  bc_alloc.req_bytes = paging_payload;
  bc_allocs.push_back(bc_alloc);

  return alloc_result::success;
}

alloc_result sf_sched::alloc_rar(uint32_t aggr_lvl, const pending_rar_t& rar, rbg_interval rbgs, uint32_t nof_grants)
{
  static const uint32_t msg3_nof_prbs = 3;
  if (rar_allocs.full()) {
    logger.info("SCHED: Maximum number of RAR allocations per TTI reached.");
    return alloc_result::no_grant_space;
  }

  uint32_t buf_rar           = 7 * nof_grants + 1; // 1+6 bytes per RAR subheader+body and 1 byte for Backoff
  uint32_t total_ul_nof_prbs = msg3_nof_prbs * nof_grants;

  // check if there is enough space for Msg3
  if (last_msg3_prb + total_ul_nof_prbs > max_msg3_prb) {
    return alloc_result::sch_collision;
  }

  // allocate RBGs and PDCCH
  alloc_result ret = tti_alloc.alloc_dl_ctrl(aggr_lvl, rbgs, alloc_type_t::DL_RAR);
  if (ret != alloc_result::success) {
    return ret;
  }

  // Generate DCI for RAR
  rar_alloc_t rar_alloc;
  if (not generate_rar_dci(
          rar_alloc.rar_grant, get_tti_tx_dl(), rar, rbgs, nof_grants, last_msg3_prb, *cc_cfg, tti_alloc.get_cfi())) {
    // Cancel on-going allocation
    tti_alloc.rem_last_alloc_dl(rbgs);
    return alloc_result::invalid_coderate;
  }

  // RAR allocation successful
  rar_alloc.alloc_data.dci_idx   = tti_alloc.get_pdcch_grid().nof_allocs() - 1;
  rar_alloc.alloc_data.rbg_range = rbgs;
  rar_alloc.alloc_data.req_bytes = buf_rar;
  rar_allocs.push_back(rar_alloc);
  last_msg3_prb += total_ul_nof_prbs * nof_grants;

  return ret;
}

bool is_periodic_cqi_expected(const sched_interface::ue_cfg_t& ue_cfg, tti_point tti_tx_ul)
{
  for (const sched_interface::ue_cfg_t::cc_cfg_t& cc : ue_cfg.supported_cc_list) {
    if (cc.dl_cfg.cqi_report.periodic_configured) {
      if (srsran_cqi_periodic_send(&cc.dl_cfg.cqi_report, tti_tx_ul.to_uint(), SRSRAN_FDD)) {
        return true;
      }
    }
  }
  return false;
}

alloc_result sf_sched::alloc_dl_user(sched_ue* user, const rbgmask_t& user_mask, uint32_t pid)
{
  if (data_allocs.full()) {
    logger.warning("SCHED: Maximum number of DL allocations reached");
    return alloc_result::no_grant_space;
  }

  if (is_dl_alloc(user->get_rnti())) {
    logger.warning("SCHED: Attempt to assign multiple harq pids to the same user rnti=0x%x", user->get_rnti());
    return alloc_result::no_rnti_opportunity;
  }

  auto* cc = user->find_ue_carrier(cc_cfg->enb_cc_idx);
  if (cc == nullptr or cc->cc_state() != cc_st::active) {
    return alloc_result::no_rnti_opportunity;
  }
  if (not user->pdsch_enabled(srsran::tti_point{get_tti_rx()}, cc_cfg->enb_cc_idx)) {
    return alloc_result::no_rnti_opportunity;
  }

  // Check if allocation would cause segmentation
  const dl_harq_proc& h = user->get_dl_harq(pid, cc_cfg->enb_cc_idx);
  if (h.is_empty()) {
    // It is newTx
    rbg_interval r = user->get_required_dl_rbgs(cc_cfg->enb_cc_idx);
    if (r.start() > user_mask.count()) {
      logger.debug("SCHED: The number of RBGs allocated to rnti=0x%x will force segmentation", user->get_rnti());
      return alloc_result::invalid_grant_params;
    }
  }

  srsran_dci_format_t dci_format = user->get_dci_format();
  if (dci_format == SRSRAN_DCI_FORMAT1A and not is_contiguous(user_mask)) {
    logger.warning("SCHED: Can't use distributed RBGs for DCI format 1A");
    return alloc_result::invalid_grant_params;
  }

  bool has_pusch_grant = is_ul_alloc(user->get_rnti()) or cc_results->is_ul_alloc(user->get_rnti());

  // Check if there is space in the PUCCH for HARQ ACKs
  const sched_interface::ue_cfg_t& ue_cfg    = user->get_ue_cfg();
  std::bitset<SRSRAN_MAX_CARRIERS> scells    = user->scell_activation_mask();
  uint32_t                         ue_cc_idx = cc->get_ue_cc_idx();
  if (user->nof_carriers_configured() > 1 and (ue_cc_idx == 0 or scells[ue_cc_idx]) and
      is_periodic_cqi_expected(ue_cfg, get_tti_tx_ul()) and not has_pusch_grant and
      user->get_ul_harq(get_tti_tx_ul(), get_enb_cc_idx())->is_empty()) {
    // Try to allocate small PUSCH grant, if there are no allocated PUSCH grants for this TTI yet
    prb_interval alloc = {};
    uint32_t L = user->get_required_prb_ul(cc_cfg->enb_cc_idx, srsran::ceil_div(SRSRAN_UCI_CQI_CODED_PUCCH_B + 2, 8));
    tti_alloc.find_ul_alloc(L, &alloc);
    has_pusch_grant = alloc.length() > 0 and alloc_ul_user(user, alloc) == alloc_result::success;
    if (ue_cc_idx != 0 and not has_pusch_grant) {
      // For SCells, if we can't allocate small PUSCH grant, abort DL allocation
      return alloc_result::no_cch_space;
    }
  }

  // Try to allocate RBGs, PDCCH, and PUCCH
  alloc_result ret = tti_alloc.alloc_dl_data(user, user_mask, has_pusch_grant);

  if (ret == alloc_result::no_cch_space and not has_pusch_grant and not data_allocs.empty() and
      user->get_ul_harq(get_tti_tx_ul(), get_enb_cc_idx())->is_empty()) {
    // PUCCH may be too full. Attempt small UL grant allocation for UCI-PUSCH
    uint32_t L = user->get_required_prb_ul(cc_cfg->enb_cc_idx, srsran::ceil_div(SRSRAN_UCI_CQI_CODED_PUCCH_B + 2, 8));
    prb_interval alloc = {};
    tti_alloc.find_ul_alloc(L, &alloc);
    has_pusch_grant = alloc.length() > 0 and alloc_ul_user(user, alloc) == alloc_result::success;
    if (has_pusch_grant) {
      ret = tti_alloc.alloc_dl_data(user, user_mask, has_pusch_grant);
    }
  }
  if (ret != alloc_result::success) {
    return ret;
  }

  // Allocation Successful
  dl_alloc_t alloc;
  alloc.dci_idx   = tti_alloc.get_pdcch_grid().nof_allocs() - 1;
  alloc.rnti      = user->get_rnti();
  alloc.user_mask = user_mask;
  alloc.pid       = pid;
  data_allocs.push_back(alloc);

  return alloc_result::success;
}

alloc_result
sf_sched::alloc_ul(sched_ue* user, prb_interval alloc, ul_alloc_t::type_t alloc_type, bool is_msg3, int msg3_mcs)
{
  if (ul_data_allocs.full()) {
    logger.debug("SCHED: Maximum number of UL allocations=%zd reached", ul_data_allocs.size());
    return alloc_result::no_grant_space;
  }

  if (is_ul_alloc(user->get_rnti())) {
    logger.warning("SCHED: Attempt to assign multiple UL grants to the same user rnti=0x%x", user->get_rnti());
    return alloc_result::no_rnti_opportunity;
  }

  // Check if there is no collision with measGap
  bool needs_pdcch = alloc_type == ul_alloc_t::ADAPT_RETX or (alloc_type == ul_alloc_t::NEWTX and not is_msg3);
  if (not user->pusch_enabled(get_tti_rx(), cc_cfg->enb_cc_idx, needs_pdcch)) {
    logger.debug("SCHED: PDCCH would collide with rnti=0x%x Measurement Gap", user->get_rnti());
    return alloc_result::no_rnti_opportunity;
  }

  // Allocate RBGs and DCI space
  bool         allow_pucch_collision = cc_cfg->nof_prb() == 6 and is_msg3;
  alloc_result ret                   = tti_alloc.alloc_ul_data(user, alloc, needs_pdcch, not allow_pucch_collision);
  if (ret != alloc_result::success) {
    return ret;
  }

  ul_data_allocs.emplace_back();
  ul_alloc_t& ul_alloc = ul_data_allocs.back();
  ul_alloc.type        = alloc_type;
  ul_alloc.is_msg3     = is_msg3;
  ul_alloc.dci_idx     = tti_alloc.get_pdcch_grid().nof_allocs() - 1;
  ul_alloc.rnti        = user->get_rnti();
  ul_alloc.alloc       = alloc;
  ul_alloc.msg3_mcs    = msg3_mcs;

  return alloc_result::success;
}

alloc_result sf_sched::alloc_ul_user(sched_ue* user, prb_interval alloc)
{
  // check whether adaptive/non-adaptive retx/newtx
  ul_alloc_t::type_t alloc_type;
  ul_harq_proc*      h        = user->get_ul_harq(get_tti_tx_ul(), cc_cfg->enb_cc_idx);
  bool               has_retx = h->has_pending_retx();
  if (not has_retx) {
    alloc_type = ul_alloc_t::NEWTX;
  } else if (h->retx_requires_pdcch(get_tti_tx_ul(), alloc)) {
    alloc_type = ul_alloc_t::ADAPT_RETX;
  } else {
    alloc_type = ul_alloc_t::NOADAPT_RETX;
  }

  return alloc_ul(user, alloc, alloc_type, h->is_msg3());
}

alloc_result sf_sched::alloc_phich(sched_ue* user)
{
  using phich_t = sched_interface::ul_sched_phich_t;

  auto* ul_sf_result = &cc_results->get_cc(cc_cfg->enb_cc_idx)->ul_sched_result;
  if (ul_sf_result->phich.full()) {
    logger.warning("SCHED: Maximum number of PHICH allocations has been reached");
    return alloc_result::no_grant_space;
  }

  auto p = user->get_active_cell_index(cc_cfg->enb_cc_idx);
  if (not p.first) {
    // user does not support this carrier
    return alloc_result::no_rnti_opportunity;
  }

  ul_harq_proc* h = user->get_ul_harq(get_tti_tx_ul(), cc_cfg->enb_cc_idx);

  /* Indicate PHICH acknowledgment if needed */
  if (h->has_pending_phich()) {
    ul_sf_result->phich.emplace_back();
    ul_sf_result->phich.back().rnti  = user->get_rnti();
    ul_sf_result->phich.back().phich = h->pop_pending_phich() ? phich_t::ACK : phich_t::NACK;
    return alloc_result::success;
  }
  return alloc_result::no_rnti_opportunity;
}

void sf_sched::set_dl_data_sched_result(const sf_cch_allocator::alloc_result_t& dci_result,
                                        sched_interface::dl_sched_res_t*        dl_result,
                                        sched_ue_list&                          ue_list)
{
  for (const auto& data_alloc : data_allocs) {
    dl_result->data.emplace_back();
    sched_interface::dl_sched_data_t* data = &dl_result->data.back();

    // Assign NCCE/L
    data->dci.location = dci_result[data_alloc.dci_idx]->dci_pos;

    // Generate DCI Format1/2/2A
    auto ue_it = ue_list.find(data_alloc.rnti);
    if (ue_it == ue_list.end()) {
      continue;
    }
    sched_ue*           user        = ue_it->second.get();
    uint32_t            data_before = user->get_requested_dl_bytes(cc_cfg->enb_cc_idx).stop();
    const dl_harq_proc& dl_harq     = user->get_dl_harq(data_alloc.pid, cc_cfg->enb_cc_idx);
    bool                is_newtx    = dl_harq.is_empty();

    int tbs = user->generate_dl_dci_format(
        data_alloc.pid, data, get_tti_tx_dl(), cc_cfg->enb_cc_idx, tti_alloc.get_cfi(), data_alloc.user_mask);

    if (tbs <= 0) {
      fmt::memory_buffer str_buffer;
      fmt::format_to(str_buffer,
                     "SCHED: DL {} failed rnti=0x{:x}, pid={}, mask={:x}, tbs={}, buffer={}",
                     is_newtx ? "tx" : "retx",
                     user->get_rnti(),
                     data_alloc.pid,
                     data_alloc.user_mask,
                     tbs,
                     user->get_requested_dl_bytes(cc_cfg->enb_cc_idx).stop());
      logger.warning("%s", srsran::to_c_str(str_buffer));
      continue;
    }

    // Print Resulting DL Allocation
    fmt::memory_buffer str_buffer;
    fmt::format_to(str_buffer,
                   "SCHED: DL {} rnti=0x{:x}, cc={}, pid={}, mask=0x{:x}, dci=({}, {}), n_rtx={}, tbs={}, "
                   "buffer={}/{}, tti_tx_dl={}",
                   is_newtx ? "tx" : "retx",
                   user->get_rnti(),
                   cc_cfg->enb_cc_idx,
                   data_alloc.pid,
                   data_alloc.user_mask,
                   data->dci.location.L,
                   data->dci.location.ncce,
                   dl_harq.nof_retx(0) + dl_harq.nof_retx(1),
                   tbs,
                   data_before,
                   user->get_requested_dl_bytes(cc_cfg->enb_cc_idx).stop(),
                   get_tti_tx_dl());
    logger.info("%s", srsran::to_c_str(str_buffer));
  }
}

//! Finds eNB CC Idex that currently holds UCI
uci_pusch_t is_uci_included(const sf_sched*        sf_sched,
                            const sf_sched_result& other_cc_results,
                            const sched_ue*        user,
                            uint32_t               current_enb_cc_idx)
{
  uci_pusch_t uci_alloc = UCI_PUSCH_NONE;

  if (not user->get_active_cell_index(current_enb_cc_idx).first) {
    return UCI_PUSCH_NONE;
  }

  // Check if UCI needs to be allocated
  const sched_interface::ue_cfg_t& ue_cfg = user->get_ue_cfg();
  for (uint32_t enbccidx = 0; enbccidx < other_cc_results.enb_cc_list.size() and uci_alloc != UCI_PUSCH_ACK_CQI;
       ++enbccidx) {
    auto p = user->get_active_cell_index(enbccidx);
    if (not p.first) {
      continue;
    }
    uint32_t ueccidx = p.second;

    // Check if CQI is pending for this CC
    const srsran_cqi_report_cfg_t& cqi_report = ue_cfg.supported_cc_list[ueccidx].dl_cfg.cqi_report;
    if (srsran_cqi_periodic_send(&cqi_report, sf_sched->get_tti_tx_ul().to_uint(), SRSRAN_FDD)) {
      if (uci_alloc == UCI_PUSCH_ACK) {
        uci_alloc = UCI_PUSCH_ACK_CQI;
      } else {
        uci_alloc = UCI_PUSCH_CQI;
      }
    }

    // Check if DL alloc is pending
    bool needs_ack_uci = false;
    if (enbccidx == current_enb_cc_idx) {
      needs_ack_uci = sf_sched->is_dl_alloc(user->get_rnti());
    } else {
      auto& dl_result = other_cc_results.enb_cc_list[enbccidx].dl_sched_result;
      for (uint32_t j = 0; j < dl_result.data.size(); ++j) {
        if (dl_result.data[j].dci.rnti == user->get_rnti()) {
          needs_ack_uci = true;
          break;
        }
      }
    }
    if (needs_ack_uci) {
      if (uci_alloc == UCI_PUSCH_CQI) {
        // Once we include ACK and CQI, stop the search
        uci_alloc = UCI_PUSCH_ACK_CQI;
      } else {
        uci_alloc = UCI_PUSCH_ACK;
      }
    }
  }
  if (uci_alloc == UCI_PUSCH_NONE) {
    return uci_alloc;
  }

  // If UL grant allocated in current carrier
  uint32_t ue_cc_idx      = other_cc_results.enb_cc_list.size();
  int      sel_enb_cc_idx = -1;
  if (sf_sched->is_ul_alloc(user->get_rnti())) {
    ue_cc_idx      = user->get_active_cell_index(current_enb_cc_idx).second;
    sel_enb_cc_idx = current_enb_cc_idx;
  }

  for (uint32_t enbccidx = 0; enbccidx < other_cc_results.enb_cc_list.size(); ++enbccidx) {
    for (uint32_t j = 0; j < other_cc_results.enb_cc_list[enbccidx].ul_sched_result.pusch.size(); ++j) {
      // Checks all the UL grants already allocated for the given rnti
      if (other_cc_results.enb_cc_list[enbccidx].ul_sched_result.pusch[j].dci.rnti == user->get_rnti()) {
        auto p = user->get_active_cell_index(enbccidx);
        // If the UE CC Idx is the lowest so far
        if (p.first and p.second < ue_cc_idx) {
          ue_cc_idx      = p.second;
          sel_enb_cc_idx = enbccidx;
        }
      }
    }
  }
  if (sel_enb_cc_idx == (int)current_enb_cc_idx) {
    return uci_alloc;
  } else {
    return UCI_PUSCH_NONE;
  }
}

void sf_sched::set_ul_sched_result(const sf_cch_allocator::alloc_result_t& dci_result,
                                   sched_interface::ul_sched_res_t*        ul_result,
                                   sched_ue_list&                          ue_list)
{
  /* Set UL data DCI locs and format */
  for (const auto& ul_alloc : ul_data_allocs) {
    auto ue_it = ue_list.find(ul_alloc.rnti);
    if (ue_it == ue_list.end()) {
      continue;
    }
    sched_ue* user = ue_it->second.get();

    srsran_dci_location_t cce_range = {0, 0};
    if (ul_alloc.needs_pdcch()) {
      cce_range = dci_result[ul_alloc.dci_idx]->dci_pos;
    }

    // If UCI is encoded in the current carrier
    uci_pusch_t uci_type = is_uci_included(this, *cc_results, user, cc_cfg->enb_cc_idx);

    /* Generate DCI Format1A */
    ul_result->pusch.emplace_back();
    sched_interface::ul_sched_data_t& pusch = ul_result->pusch.back();
    uint32_t total_data_before              = user->get_pending_ul_data_total(get_tti_tx_ul(), cc_cfg->enb_cc_idx);
    int      tbs                            = user->generate_format0(&pusch,
                                     get_tti_tx_ul(),
                                     cc_cfg->enb_cc_idx,
                                     ul_alloc.alloc,
                                     ul_alloc.needs_pdcch(),
                                     cce_range,
                                     ul_alloc.msg3_mcs,
                                     uci_type);

    ul_harq_proc* h                 = user->get_ul_harq(get_tti_tx_ul(), cc_cfg->enb_cc_idx);
    uint32_t      new_pending_bytes = user->get_pending_ul_new_data(get_tti_tx_ul(), cc_cfg->enb_cc_idx);
    // Allow TBS=0 in case of UCI-only PUSCH
    if (tbs < 0 || (tbs == 0 && pusch.dci.tb.mcs_idx != 29)) {
      fmt::memory_buffer str_buffer;
      fmt::format_to(str_buffer,
                     "SCHED: Error {} {} rnti=0x{:x}, pid={}, dci=({},{}), prb={}, bsr={}",
                     ul_alloc.is_msg3 ? "Msg3" : "UL",
                     ul_alloc.is_retx() ? "retx" : "tx",
                     user->get_rnti(),
                     h->get_id(),
                     pusch.dci.location.L,
                     pusch.dci.location.ncce,
                     ul_alloc.alloc,
                     new_pending_bytes);
      logger.warning("%s", srsran::to_c_str(str_buffer));
      ul_result->pusch.pop_back();
      continue;
    }

    // Print Resulting UL Allocation
    uint32_t old_pending_bytes = user->get_pending_ul_old_data();
    if (logger.info.enabled()) {
      fmt::memory_buffer str_buffer;
      fmt::format_to(str_buffer,
                     "SCHED: {} {} rnti=0x{:x}, cc={}, pid={}, dci=({},{}), prb={}, n_rtx={}, tbs={}, bsr={} ({}-{})",
                     ul_alloc.is_msg3 ? "Msg3" : "UL",
                     ul_alloc.is_retx() ? "retx" : "tx",
                     user->get_rnti(),
                     cc_cfg->enb_cc_idx,
                     h->get_id(),
                     pusch.dci.location.L,
                     pusch.dci.location.ncce,
                     ul_alloc.alloc,
                     h->nof_retx(0),
                     tbs,
                     new_pending_bytes,
                     total_data_before,
                     old_pending_bytes);
      logger.info("%s", srsran::to_c_str(str_buffer));
    }

    pusch.current_tx_nb = h->nof_retx(0);
  }
}

alloc_result sf_sched::alloc_msg3(sched_ue* user, const sched_interface::dl_sched_rar_grant_t& rargrant)
{
  // Derive PRBs from allocated RAR grants
  prb_interval msg3_alloc = prb_interval::riv_to_prbs(rargrant.grant.rba, cc_cfg->nof_prb());

  alloc_result ret = alloc_ul(user, msg3_alloc, sf_sched::ul_alloc_t::NEWTX, true, rargrant.grant.trunc_mcs);
  if (ret != alloc_result::success) {
    fmt::memory_buffer str_buffer;
    fmt::format_to(str_buffer, "{}", msg3_alloc);
    logger.warning("SCHED: Could not allocate msg3 within %s.", srsran::to_c_str(str_buffer));
  }
  return ret;
}

void sf_sched::generate_sched_results(sched_ue_list& ue_db)
{
  cc_sched_result* cc_result = cc_results->get_cc(cc_cfg->enb_cc_idx);

  /* Resume UL HARQs with pending retxs that did not get allocated */
  using phich_t    = sched_interface::ul_sched_phich_t;
  auto& phich_list = cc_result->ul_sched_result.phich;
  for (uint32_t i = 0; i < cc_result->ul_sched_result.phich.size(); ++i) {
    auto& phich = phich_list[i];
    if (phich.phich == phich_t::NACK) {
      auto&         ue = *ue_db[phich.rnti];
      ul_harq_proc* h  = ue.get_ul_harq(get_tti_tx_ul(), cc_cfg->enb_cc_idx);
      if (not is_ul_alloc(ue.get_rnti()) and h != nullptr and not h->is_empty()) {
        // There was a missed UL harq retx. Halt+Resume the HARQ
        phich.phich = phich_t::ACK;
        logger.debug("SCHED: rnti=0x%x UL harq pid=%d is being resumed", ue.get_rnti(), h->get_id());
      }
    }
  }

  /* Pick one of the possible DCI masks */
  sf_cch_allocator::alloc_result_t dci_result;
  //  tti_alloc.get_pdcch_grid().result_to_string();
  tti_alloc.get_pdcch_grid().get_allocs(&dci_result, &cc_result->pdcch_mask);

  /* Register final CFI */
  cc_result->dl_sched_result.cfi = tti_alloc.get_pdcch_grid().get_cfi();

  /* Generate DCI formats and fill sched_result structs */
  for (const auto& bc_alloc : bc_allocs) {
    cc_result->dl_sched_result.bc.emplace_back(bc_alloc.bc_grant);
    cc_result->dl_sched_result.bc.back().dci.location = dci_result[bc_alloc.dci_idx]->dci_pos;
    log_broadcast_allocation(cc_result->dl_sched_result.bc.back(), bc_alloc.rbg_range, *cc_cfg);
  }

  for (const auto& rar_alloc : rar_allocs) {
    cc_result->dl_sched_result.rar.emplace_back(rar_alloc.rar_grant);
    cc_result->dl_sched_result.rar.back().dci.location = dci_result[rar_alloc.alloc_data.dci_idx]->dci_pos;
    log_rar_allocation(cc_result->dl_sched_result.rar.back(), rar_alloc.alloc_data.rbg_range);
  }

  set_dl_data_sched_result(dci_result, &cc_result->dl_sched_result, ue_db);

  set_ul_sched_result(dci_result, &cc_result->ul_sched_result, ue_db);

  /* Store remaining sf_sched results for this TTI */
  cc_result->dl_mask   = tti_alloc.get_dl_mask();
  cc_result->ul_mask   = tti_alloc.get_ul_mask();
  cc_result->generated = true;
}

uint32_t sf_sched::get_nof_ctrl_symbols() const
{
  return tti_alloc.get_cfi() + ((cc_cfg->cfg.cell.nof_prb <= 10) ? 1 : 0);
}

} // namespace srsenb
