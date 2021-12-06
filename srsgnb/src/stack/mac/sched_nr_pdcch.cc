/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/hdr/stack/mac/sched_nr_pdcch.h"
#include "srsran/common/string_helpers.h"

namespace srsenb {
namespace sched_nr_impl {

template <typename... Args>
void log_pdcch_alloc_failure(srslog::log_channel&       log_ch,
                             srsran_rnti_type_t         rnti_type,
                             uint32_t                   ss_id,
                             const ue_carrier_params_t* ue,
                             const char*                cause_fmt,
                             Args&&... args)
{
  if (not log_ch.enabled()) {
    return;
  }

  // Log PDCCH allocation failure
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, "SCHED: Failure to allocate PDCCH for {}-rnti", srsran_rnti_type_str_short(rnti_type));
  if (rnti_type == srsran_rnti_type_c or rnti_type == srsran_rnti_type_tc) {
    fmt::format_to(fmtbuf, "=0x{:x}", ue->rnti);
  }
  fmt::format_to(fmtbuf, ", ss_id={}. Cause:", ss_id);
  fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  log_ch("%s", srsran::to_c_str(fmtbuf));
}

coreset_region::coreset_region(const bwp_params_t& bwp_cfg_,
                               uint32_t            coreset_id_,
                               uint32_t            slot_idx_,
                               pdcch_dl_list_t&    dl_list_,
                               pdcch_ul_list_t&    ul_list_) :
  coreset_cfg(&bwp_cfg_.cfg.pdcch.coreset[coreset_id_]),
  coreset_id(coreset_id_),
  slot_idx(slot_idx_),
  pdcch_dl_list(dl_list_),
  pdcch_ul_list(ul_list_),
  rar_cce_list(bwp_cfg_.rar_cce_list),
  common_cce_list(bwp_cfg_.common_cce_list)
{
  const bool* res_active = &coreset_cfg->freq_resources[0];
  nof_freq_res           = std::count(res_active, res_active + SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE, true);
  srsran_assert(get_td_symbols() <= SRSRAN_CORESET_DURATION_MAX,
                "Possible number of time-domain OFDM symbols in CORESET must be within {1,2,3}");
  srsran_assert(nof_freq_res <= bwp_cfg_.cell_cfg.carrier.nof_prb,
                "The number of frequency resources=%d of CORESET#%d exceeds BWP bandwidth",
                nof_freq_res,
                coreset_id);
}

void coreset_region::reset()
{
  dfs_tree.clear();
  saved_dfs_tree.clear();
  dci_list.clear();
  pdcch_dl_list.clear();
  pdcch_ul_list.clear();
}

pdcch_dl_t* coreset_region::alloc_dl_pdcch(srsran_rnti_type_t         rnti_type,
                                           uint32_t                   aggr_idx,
                                           uint32_t                   search_space_id,
                                           const ue_carrier_params_t* user)
{
  // Add new DL PDCCH to sched result
  pdcch_dl_list.emplace_back();

  if (alloc_pdcch_common(rnti_type, true, aggr_idx, search_space_id, user)) {
    return &pdcch_dl_list.back();
  }

  // Remove failed PDCCH allocation
  pdcch_dl_list.pop_back();
  return nullptr;
}

pdcch_ul_t* coreset_region::alloc_ul_pdcch(uint32_t aggr_idx, uint32_t search_space_id, const ue_carrier_params_t* user)
{
  // Add new UL PDCCH to sched result
  pdcch_ul_list.emplace_back();

  if (alloc_pdcch_common(srsran_rnti_type_c, false, aggr_idx, search_space_id, user)) {
    return &pdcch_ul_list.back();
  }

  // Remove failed PDCCH allocation
  pdcch_ul_list.pop_back();
  return nullptr;
}

bool coreset_region::alloc_pdcch_common(srsran_rnti_type_t         rnti_type,
                                        bool                       is_dl,
                                        uint32_t                   aggr_idx,
                                        uint32_t                   search_space_id,
                                        const ue_carrier_params_t* user)
{
  saved_dfs_tree.clear();

  alloc_record record;
  record.ue        = user;
  record.aggr_idx  = aggr_idx;
  record.ss_id     = search_space_id;
  record.is_dl     = is_dl;
  record.rnti_type = rnti_type;
  record.idx       = record.is_dl ? pdcch_dl_list.size() - 1 : pdcch_ul_list.size() - 1;

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
  if (not dci_list.back().is_dl) {
    pdcch_ul_list.pop_back();
  } else {
    pdcch_dl_list.pop_back();
  }
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
    if (not record.is_dl) {
      pdcch_ul_t& pdcch_ul      = pdcch_ul_list[record.idx];
      pdcch_ul.dci.ctx.location = node.dci_pos;
    } else {
      pdcch_dl_t& pdcch_dl      = pdcch_dl_list[record.idx];
      pdcch_dl.dci.ctx.location = node.dci_pos;
    }
    return true;
  }

  return false;
}

srsran::span<const uint32_t> coreset_region::get_cce_loc_table(const alloc_record& record) const
{
  switch (record.rnti_type) {
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
      srsran_terminate("Invalid RNTI type=%d", record.rnti_type);
      break;
  }
  return {};
}

bwp_pdcch_allocator::bwp_pdcch_allocator(const bwp_params_t& bwp_cfg_,
                                         uint32_t            slot_idx_,
                                         pdcch_dl_list_t&    dl_pdcchs,
                                         pdcch_ul_list_t&    ul_pdcchs) :
  bwp_cfg(bwp_cfg_), pdcch_dl_list(dl_pdcchs), pdcch_ul_list(ul_pdcchs), slot_idx(slot_idx_), logger(bwp_cfg_.logger)
{
  for (uint32_t cs_idx = 0; cs_idx < SRSRAN_UE_DL_NR_MAX_NOF_CORESET; ++cs_idx) {
    if (bwp_cfg.cfg.pdcch.coreset_present[cs_idx]) {
      uint32_t cs_id = bwp_cfg.cfg.pdcch.coreset[cs_idx].id;
      coresets.emplace(cs_id, bwp_cfg, cs_id, slot_idx, pdcch_dl_list, pdcch_ul_list);
    }
  }
}

/// Helper function to verify valid inputs
bool bwp_pdcch_allocator::check_args_valid(srsran_rnti_type_t         rnti_type,
                                           uint32_t                   ss_id,
                                           uint32_t                   aggr_idx,
                                           const ue_carrier_params_t* user,
                                           bool                       is_dl) const
{
  srsran_assert(ss_id < SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE, "Invalid SearchSpace#%d", ss_id);
  srsran_assert(
      aggr_idx < SRSRAN_SEARCH_SPACE_NOF_AGGREGATION_LEVELS_NR, "Invalid aggregation level index=%d", aggr_idx);

  // Cell Configuration checks
  if (not bwp_cfg.slots[slot_idx].is_dl) {
    log_pdcch_alloc_failure(logger.error, rnti_type, ss_id, user, "DL is disabled for slot={}", slot_idx);
    return false;
  }

  // Coreset-specific checks
  const srsran_search_space_t* ss = (user == nullptr) ? bwp_cfg.get_ss(ss_id) : user->get_ss(ss_id);
  if (ss == nullptr) {
    log_pdcch_alloc_failure(logger.error, rnti_type, ss_id, user, "SearchSpace has not been configured");
    return false;
  }
  if (ss->nof_candidates[aggr_idx] == 0) {
    log_pdcch_alloc_failure(logger.warning, rnti_type, ss_id, user, "Chosen PDCCH doesn't have valid candidates");
    return false;
  }

  if (is_dl) {
    if (pdcch_dl_list.full()) {
      log_pdcch_alloc_failure(
          logger.warning, rnti_type, ss_id, user, "Maximum number of allocations={} reached", pdcch_dl_list.size());
      return false;
    }
    if (rnti_type == srsran_rnti_type_si or rnti_type == srsran_rnti_type_ra or rnti_type == srsran_rnti_type_p) {
      srsran_assert(user == nullptr, "No UE should be provided in case of non-UE PDCCH allocation");
      if (rnti_type == srsran_rnti_type_ra) {
        srsran_assert(ss_id == bwp_cfg.cfg.pdcch.ra_search_space.id and bwp_cfg.cfg.pdcch.ra_search_space_present,
                      "PDCCH grant type does not match search space");
      }
    } else {
      srsran_assert(user != nullptr, "UE object must be provided for UE-specific PDCCH allocations");
    }
  } else if (pdcch_ul_list.full()) {
    log_pdcch_alloc_failure(
        logger.warning, rnti_type, ss_id, user, "Maximum number of UL allocations={} reached", pdcch_ul_list.size());
    return false;
  }

  srsran_sanity_check(pdcch_dl_list.size() + pdcch_ul_list.size() == nof_allocations(), "Invalid PDCCH state");
  return true;
}

pdcch_dl_t* bwp_pdcch_allocator::alloc_dl_pdcch(srsran_rnti_type_t         rnti_type,
                                                uint32_t                   ss_id,
                                                uint32_t                   aggr_idx,
                                                const ue_carrier_params_t* user)
{
  if (not check_args_valid(rnti_type, ss_id, aggr_idx, user, true)) {
    return nullptr;
  }
  const srsran_search_space_t& ss = (user == nullptr) ? *bwp_cfg.get_ss(ss_id) : *user->get_ss(ss_id);

  pdcch_dl_t* pdcch = coresets[ss.coreset_id].alloc_dl_pdcch(rnti_type, aggr_idx, ss_id, user);

  if (pdcch == nullptr) {
    // Log PDCCH allocation failure
    srslog::log_channel& ch = user == nullptr ? logger.warning : logger.debug;
    log_pdcch_alloc_failure(ch, rnti_type, ss_id, user, "Failure to find available PDCCH position");
  }
  return pdcch;
}

pdcch_ul_t* bwp_pdcch_allocator::alloc_ul_pdcch(uint32_t ss_id, uint32_t aggr_idx, const ue_carrier_params_t* user)
{
  if (not check_args_valid(srsran_rnti_type_c, ss_id, aggr_idx, user, false)) {
    return nullptr;
  }
  const srsran_search_space_t& ss = *user->get_ss(ss_id);

  pdcch_ul_t* pdcch = coresets[ss.coreset_id].alloc_ul_pdcch(aggr_idx, ss_id, user);

  if (pdcch == nullptr) {
    // Log PDCCH allocation failure
    log_pdcch_alloc_failure(logger.debug, srsran_rnti_type_c, ss_id, user, "Failure to find available PDCCH position");
  }

  return pdcch;
}

void bwp_pdcch_allocator::rem_last_pdcch(uint32_t ss_id)
{
  const srsran_search_space_t& ss = bwp_cfg.cfg.pdcch.search_space[ss_id];

  uint32_t coreset_id = ss.coreset_id;
  coresets[coreset_id].rem_last_pdcch();
}

void bwp_pdcch_allocator::reset()
{
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

} // namespace sched_nr_impl
} // namespace srsenb
