/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsgnb/hdr/stack/mac/sched_nr_pdcch.h"
#include "srsgnb/hdr/stack/mac/sched_nr_helpers.h"
#include "srsran/common/string_helpers.h"

namespace srsenb {
namespace sched_nr_impl {

template <typename... Args>
void log_pdcch_alloc_failure(srslog::log_channel& log_ch,
                             srsran_rnti_type_t   rnti_type,
                             uint32_t             ss_id,
                             uint16_t             rnti,
                             const char*          cause_fmt,
                             Args&&... args)
{
  if (not log_ch.enabled()) {
    return;
  }

  // Log PDCCH allocation failure
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf,
                 "SCHED: Failure to allocate PDCCH for {}-rnti=0x{:x}, SS#{}. Cause: ",
                 srsran_rnti_type_str_short(rnti_type),
                 rnti,
                 ss_id);
  fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  log_ch("%s", srsran::to_c_str(fmtbuf));
}

void fill_dci_from_cfg(const bwp_params_t& bwp_cfg, srsran_dci_dl_nr_t& dci)
{
  dci.bwp_id      = bwp_cfg.bwp_id;
  dci.cc_id       = bwp_cfg.cc;
  dci.tpc         = 1;
  dci.coreset0_bw = bwp_cfg.cfg.pdcch.coreset_present[0] ? bwp_cfg.coreset_prb_range(0).length() : 0;
}

void fill_dci_from_cfg(const bwp_params_t& bwp_cfg, srsran_dci_ul_nr_t& dci)
{
  dci.bwp_id = bwp_cfg.bwp_id;
  dci.cc_id  = bwp_cfg.cc;
  dci.tpc    = 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

coreset_region::coreset_region(const bwp_params_t& bwp_cfg_, uint32_t coreset_id_, uint32_t slot_idx_) :
  coreset_cfg(&bwp_cfg_.cfg.pdcch.coreset[coreset_id_]),
  coreset_id(coreset_id_),
  slot_idx(slot_idx_),
  rar_cce_list(bwp_cfg_.rar_cce_list),
  common_cce_list(bwp_cfg_.common_cce_list)
{
  const bool* res_active = &coreset_cfg->freq_resources[0];
  nof_freq_res           = std::count(res_active, res_active + SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE, true);
  srsran_assert(get_td_symbols() <= SRSRAN_CORESET_DURATION_MAX,
                "Possible number of time-domain OFDM symbols in CORESET must be within {1,2,3}");
  srsran_assert(nof_freq_res <= bwp_cfg_.cell_cfg.nof_prb(),
                "The number of frequency resources=%d of CORESET#%d exceeds BWP bandwidth=%d",
                nof_freq_res,
                coreset_id,
                bwp_cfg_.cell_cfg.nof_prb());
}

void coreset_region::reset()
{
  dfs_tree.clear();
  saved_dfs_tree.clear();
  dci_list.clear();
}

bool coreset_region::alloc_pdcch(srsran_rnti_type_t         rnti_type,
                                 bool                       is_dl,
                                 uint32_t                   aggr_idx,
                                 uint32_t                   search_space_id,
                                 const ue_carrier_params_t* user,
                                 srsran_dci_ctx_t&          dci)
{
  saved_dfs_tree.clear();

  alloc_record record;
  record.dci            = &dci;
  record.ue             = user;
  record.aggr_idx       = aggr_idx;
  record.ss_id          = search_space_id;
  record.is_dl          = is_dl;
  record.dci->rnti_type = rnti_type;

  // Try to allocate grant. If it fails, attempt the same grant, but using a different permutation of past grant DCI
  // positions
  do {
    bool success = alloc_dfs_node(record, 0);
    if (success) {
      // DCI record allocation successful
      dci_list.push_back(record);
      return true;
    }
    if (saved_dfs_tree.empty()) {
      saved_dfs_tree = dfs_tree;
    }
  } while (get_next_dfs());

  // Revert steps to initial state, before dci record allocation was attempted
  dfs_tree.swap(saved_dfs_tree);
  return false;
}

void coreset_region::rem_last_pdcch()
{
  srsran_assert(not dci_list.empty(), "%s called when no PDCCH have yet been allocated", __FUNCTION__);

  // Remove DCI record
  dfs_tree.pop_back();
  dci_list.pop_back();
}

bool coreset_region::get_next_dfs()
{
  do {
    if (dfs_tree.empty()) {
      // If we reach root, the allocation failed
      return false;
    }
    // Attempt to re-add last tree node, but with a higher node child index
    uint32_t start_child_idx = dfs_tree.back().dci_pos_idx + 1;
    dfs_tree.pop_back();
    while (dfs_tree.size() < dci_list.size() and alloc_dfs_node(dci_list[dfs_tree.size()], start_child_idx)) {
      start_child_idx = 0;
    }
  } while (dfs_tree.size() < dci_list.size());

  // Finished computation of next DFS node
  return true;
}

bool coreset_region::alloc_dfs_node(const alloc_record& record, uint32_t start_dci_idx)
{
  alloc_tree_dfs_t& alloc_dfs = dfs_tree;
  // Get DCI Location Table
  auto cce_locs = get_cce_loc_table(record);
  if (start_dci_idx >= cce_locs.size()) {
    return false;
  }

  tree_node node;
  node.dci_pos_idx = start_dci_idx;
  node.dci_pos.L   = record.aggr_idx;
  node.rnti        = record.ue != nullptr ? record.ue->rnti : SRSRAN_INVALID_RNTI;
  node.current_mask.resize(nof_cces());
  // get cumulative pdcch bitmap
  if (not alloc_dfs.empty()) {
    node.total_mask = alloc_dfs.back().total_mask;
  } else {
    node.total_mask.resize(nof_cces());
  }

  for (; node.dci_pos_idx < cce_locs.size(); ++node.dci_pos_idx) {
    node.dci_pos.ncce = cce_locs[node.dci_pos_idx];

    node.current_mask.reset();
    node.current_mask.fill(node.dci_pos.ncce, node.dci_pos.ncce + (1U << record.aggr_idx));
    if ((node.total_mask & node.current_mask).any()) {
      // there is a PDCCH collision. Try another CCE position
      continue;
    }

    // Allocation successful
    node.total_mask |= node.current_mask;
    alloc_dfs.push_back(node);
    record.dci->location = node.dci_pos;
    return true;
  }

  return false;
}

srsran::span<const uint32_t> coreset_region::get_cce_loc_table(const alloc_record& record) const
{
  switch (record.dci->rnti_type) {
    case srsran_rnti_type_ra:
      return rar_cce_list[slot_idx][record.aggr_idx];
    case srsran_rnti_type_si:
      return common_cce_list[record.ss_id][slot_idx][record.aggr_idx];
    case srsran_rnti_type_c:
    case srsran_rnti_type_tc:
    case srsran_rnti_type_mcs_c:
    case srsran_rnti_type_sp_csi:
      return record.ue->cce_pos_list(record.ss_id, slot_idx, record.aggr_idx);
    default:
      srsran_terminate("Invalid RNTI type=%s", srsran_rnti_type_str(record.dci->rnti_type));
      break;
  }
  return {};
}

void coreset_region::print_allocations(fmt::memory_buffer& fmtbuf) const
{
  if (not dci_list.empty()) {
    fmt::format_to(fmtbuf, "CORESET#{} (#CCEs={}):\n", coreset_id, nof_cces());
  }
  for (const alloc_record& dci : dci_list) {
    fmt::format_to(fmtbuf,
                   "  {}-rnti=0x{:x}: ({}, {})\n",
                   srsran_rnti_type_str_short(dci.dci->rnti_type),
                   dci.dci->rnti,
                   dci.dci->location.ncce,
                   dci.dci->location.L);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bwp_pdcch_allocator::bwp_pdcch_allocator(const bwp_params_t& bwp_cfg_,
                                         uint32_t            slot_idx_,
                                         pdcch_dl_list_t&    dl_pdcchs,
                                         pdcch_ul_list_t&    ul_pdcchs) :
  bwp_cfg(bwp_cfg_), pdcch_dl_list(dl_pdcchs), pdcch_ul_list(ul_pdcchs), slot_idx(slot_idx_), logger(bwp_cfg_.logger)
{
  for (uint32_t cs_idx = 0; cs_idx < SRSRAN_UE_DL_NR_MAX_NOF_CORESET; ++cs_idx) {
    if (bwp_cfg.cfg.pdcch.coreset_present[cs_idx]) {
      uint32_t cs_id = bwp_cfg.cfg.pdcch.coreset[cs_idx].id;
      coresets.emplace(cs_id, bwp_cfg, cs_id, slot_idx);
    }
  }
}

void bwp_pdcch_allocator::fill_dci_ctx_common(srsran_dci_ctx_t&            dci,
                                              srsran_rnti_type_t           rnti_type,
                                              uint16_t                     rnti,
                                              const srsran_search_space_t& ss,
                                              srsran_dci_format_nr_t       dci_fmt,
                                              const ue_carrier_params_t*   ue)
{
  // Note: Location is filled by coreset_region class.
  dci.ss_type    = ss.type;
  dci.coreset_id = ss.coreset_id;
  const srsran_coreset_t* coreset =
      ue == nullptr ? &bwp_cfg.cfg.pdcch.coreset[ss.coreset_id] : &ue->phy().pdcch.coreset[ss.coreset_id];
  dci.coreset_start_rb = srsran_coreset_start_rb(coreset);
  dci.rnti_type        = rnti_type;
  dci.rnti             = rnti;
  dci.format           = dci_fmt;
}

pdcch_dl_alloc_result bwp_pdcch_allocator::alloc_rar_pdcch(uint16_t ra_rnti, uint32_t aggr_idx)
{
  srsran_assert(bwp_cfg.cfg.pdcch.ra_search_space_present, "Allocating RAR PDCCH in BWP without RA SearchSpace");
  return alloc_dl_pdcch_common(
      srsran_rnti_type_ra, ra_rnti, bwp_cfg.cfg.pdcch.ra_search_space.id, aggr_idx, srsran_dci_format_nr_1_0, nullptr);
}

pdcch_dl_alloc_result bwp_pdcch_allocator::alloc_si_pdcch(uint32_t ss_id, uint32_t aggr_idx)
{
  return alloc_dl_pdcch_common(srsran_rnti_type_si, SRSRAN_SIRNTI, ss_id, aggr_idx, srsran_dci_format_nr_1_0, nullptr);
}

pdcch_dl_alloc_result bwp_pdcch_allocator::alloc_dl_pdcch(srsran_rnti_type_t         rnti_type,
                                                          uint32_t                   ss_id,
                                                          uint32_t                   aggr_idx,
                                                          const ue_carrier_params_t& user)
{
  static const srsran_dci_format_nr_t dci_fmt = srsran_dci_format_nr_1_0; // TODO: make it configurable
  srsran_assert(rnti_type == srsran_rnti_type_c or rnti_type == srsran_rnti_type_tc,
                "Invalid RNTI type=%s for UE-specific PDCCH",
                srsran_rnti_type_str_short(rnti_type));
  return alloc_dl_pdcch_common(rnti_type, user.rnti, ss_id, aggr_idx, dci_fmt, &user);
}

pdcch_dl_alloc_result bwp_pdcch_allocator::alloc_dl_pdcch_common(srsran_rnti_type_t         rnti_type,
                                                                 uint16_t                   rnti,
                                                                 uint32_t                   ss_id,
                                                                 uint32_t                   aggr_idx,
                                                                 srsran_dci_format_nr_t     dci_fmt,
                                                                 const ue_carrier_params_t* user)
{
  alloc_result r = check_args_valid(rnti_type, rnti, ss_id, aggr_idx, dci_fmt, user, true);
  if (r != alloc_result::success) {
    return {r};
  }
  const srsran_search_space_t& ss =
      (user == nullptr)
          ? (rnti_type == srsran_rnti_type_ra ? bwp_cfg.cfg.pdcch.ra_search_space : *bwp_cfg.get_ss(ss_id))
          : *user->get_ss(ss_id);

  // Add new DL PDCCH to sched result
  pdcch_dl_list.emplace_back();

  bool success =
      coresets[ss.coreset_id].alloc_pdcch(rnti_type, true, aggr_idx, ss_id, user, pdcch_dl_list.back().dci.ctx);

  if (not success) {
    // Remove failed PDCCH allocation
    pdcch_dl_list.pop_back();

    // Log PDCCH allocation failure
    srslog::log_channel& ch = user == nullptr ? logger.warning : logger.debug;
    log_pdcch_alloc_failure(ch, rnti_type, ss_id, rnti, "No available PDCCH position");

    return {alloc_result::no_cch_space};
  }

  // PDCCH allocation was successful
  pdcch_dl_t& pdcch = pdcch_dl_list.back();

  // Fill DCI with semi-static config
  fill_dci_from_cfg(bwp_cfg, pdcch.dci);

  // Fill DCI context information
  fill_dci_ctx_common(pdcch.dci.ctx, rnti_type, rnti, ss, dci_fmt, user);

  // register last PDCCH coreset, in case it needs to be aborted
  pending_dci = &pdcch.dci.ctx;

  return {&pdcch};
}

pdcch_ul_alloc_result
bwp_pdcch_allocator::alloc_ul_pdcch(uint32_t ss_id, uint32_t aggr_idx, const ue_carrier_params_t& user)
{
  static const srsran_dci_format_nr_t dci_fmt = srsran_dci_format_nr_0_0; // TODO: make it configurable
  alloc_result r = check_args_valid(srsran_rnti_type_c, user.rnti, ss_id, aggr_idx, dci_fmt, &user, false);
  if (r != alloc_result::success) {
    return {r};
  }
  const srsran_search_space_t& ss = *user.get_ss(ss_id);

  // Add new UL PDCCH to sched result
  pdcch_ul_list.emplace_back();

  bool success = coresets[ss.coreset_id].alloc_pdcch(
      srsran_rnti_type_c, false, aggr_idx, ss_id, &user, pdcch_ul_list.back().dci.ctx);

  if (not success) {
    // Remove failed PDCCH allocation
    pdcch_ul_list.pop_back();

    // Log PDCCH allocation failure
    log_pdcch_alloc_failure(logger.debug, srsran_rnti_type_c, ss_id, user.rnti, "No available PDCCH position");

    return {alloc_result::no_cch_space};
  }

  // PDCCH allocation was successful
  pdcch_ul_t& pdcch = pdcch_ul_list.back();

  // Fill DCI with semi-static config
  fill_dci_from_cfg(bwp_cfg, pdcch.dci);

  // Fill DCI context information
  fill_dci_ctx_common(pdcch.dci.ctx, srsran_rnti_type_c, user.rnti, ss, dci_fmt, &user);

  // register last PDCCH coreset, in case it needs to be aborted
  pending_dci = &pdcch.dci.ctx;

  return {&pdcch};
}

void bwp_pdcch_allocator::cancel_last_pdcch()
{
  srsran_assert(pending_dci != nullptr, "Trying to abort PDCCH allocation that does not exist");
  uint32_t cs_id = pending_dci->coreset_id;

  if (&pdcch_dl_list.back().dci.ctx == pending_dci) {
    pdcch_dl_list.pop_back();
  } else if (&pdcch_ul_list.back().dci.ctx == pending_dci) {
    pdcch_ul_list.pop_back();
  } else {
    logger.error("Invalid DCI context provided to be removed");
    return;
  }
  coresets[cs_id].rem_last_pdcch();
  pending_dci = nullptr;
}

void bwp_pdcch_allocator::reset()
{
  pending_dci = nullptr;
  pdcch_dl_list.clear();
  pdcch_ul_list.clear();
  for (coreset_region& coreset : coresets) {
    coreset.reset();
  }
}

uint32_t bwp_pdcch_allocator::nof_allocations() const
{
  uint32_t count = 0;
  for (const coreset_region& coreset : coresets) {
    count += coreset.nof_allocs();
  }
  return count;
}

uint32_t bwp_pdcch_allocator::nof_cces(uint32_t coreset_id) const
{
  return coresets[coreset_id].nof_cces();
}

alloc_result bwp_pdcch_allocator::check_args_valid(srsran_rnti_type_t         rnti_type,
                                                   uint16_t                   rnti,
                                                   uint32_t                   ss_id,
                                                   uint32_t                   aggr_idx,
                                                   srsran_dci_format_nr_t     dci_fmt,
                                                   const ue_carrier_params_t* user,
                                                   bool                       is_dl) const
{
  srsran_assert(ss_id < SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE, "Invalid SearchSpace#%d", ss_id);
  srsran_assert(
      aggr_idx < SRSRAN_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR, "Invalid aggregation level index=%d", aggr_idx);

  // DL must be active in given slot
  if (not bwp_cfg.slots[slot_idx].is_dl) {
    log_pdcch_alloc_failure(logger.error, rnti_type, ss_id, rnti, "DL is disabled for slot={}", slot_idx);
    return alloc_result::no_cch_space;
  }

  // Verify SearchSpace validity
  const srsran_search_space_t* ss =
      (user == nullptr)
          ? (rnti_type == srsran_rnti_type_ra ? &bwp_cfg.cfg.pdcch.ra_search_space : bwp_cfg.get_ss(ss_id))
          : user->get_ss(ss_id);
  if (ss == nullptr) {
    // Couldn't find SearchSpace
    log_pdcch_alloc_failure(logger.error, rnti_type, ss_id, rnti, "SearchSpace has not been configured");
    return alloc_result::invalid_grant_params;
  }
  if (ss->nof_candidates[aggr_idx] == 0) {
    // No valid DCI position candidates given aggregation level
    log_pdcch_alloc_failure(
        logger.warning, rnti_type, ss_id, rnti, "Chosen SearchSpace doesn't have CCE candidates for L={}", aggr_idx);
    return alloc_result::invalid_grant_params;
  }
  if (not is_rnti_type_valid_in_search_space(rnti_type, ss->type)) {
    // RNTI type doesnt match SearchSpace type
    log_pdcch_alloc_failure(logger.warning,
                            rnti_type,
                            ss_id,
                            rnti,
                            "Chosen SearchSpace type \"{}\" does not match rnti_type.",
                            srsran_ss_type_str(ss->type));
    return alloc_result::invalid_grant_params;
  }
  auto dci_fmt_equal = [dci_fmt](srsran_dci_format_nr_t f) { return f == dci_fmt; };
  if (std::none_of(&ss->formats[0], &ss->formats[ss->nof_formats], dci_fmt_equal)) {
    log_pdcch_alloc_failure(logger.warning,
                            rnti_type,
                            ss_id,
                            rnti,
                            "Chosen SearchSpace does not support chosen dci format={}",
                            srsran_dci_format_nr_string(dci_fmt));
    return alloc_result::invalid_grant_params;
  }

  if (is_dl) {
    if (pdcch_dl_list.full()) {
      log_pdcch_alloc_failure(
          logger.warning, rnti_type, ss_id, rnti, "Maximum number of allocations={} reached", pdcch_dl_list.size());
      return alloc_result::no_cch_space;
    }
  } else if (pdcch_ul_list.full()) {
    log_pdcch_alloc_failure(
        logger.warning, rnti_type, ss_id, rnti, "Maximum number of UL allocations={} reached", pdcch_ul_list.size());
    return alloc_result::no_cch_space;
  }

  if (user != nullptr) {
    if (user->active_bwp().bwp_id != bwp_cfg.bwp_id) {
      log_pdcch_alloc_failure(logger.warning,
                              rnti_type,
                              ss_id,
                              rnti,
                              "Trying to allocate BWP#{} which is inactive for the UE.",
                              user->active_bwp().bwp_id);
      return alloc_result::no_rnti_opportunity;
    }
  }

  srsran_sanity_check(pdcch_dl_list.size() + pdcch_ul_list.size() == nof_allocations(), "Invalid PDCCH state");
  return alloc_result::success;
}

void bwp_pdcch_allocator::print_allocations(fmt::memory_buffer& fmtbuf) const
{
  fmt::format_to(
      fmtbuf, "PDCCH allocations: ({} active coresets):{}\n", coresets.size(), nof_allocations() == 0 ? " None" : "");
  for (const coreset_region& cs : coresets) {
    cs.print_allocations(fmtbuf);
  }
}

std::string bwp_pdcch_allocator::print_allocations() const
{
  fmt::memory_buffer fmtbuf;
  print_allocations(fmtbuf);
  return fmt::to_string(fmtbuf);
}

} // namespace sched_nr_impl
} // namespace srsenb
