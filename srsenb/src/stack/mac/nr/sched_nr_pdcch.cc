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

#include "srsenb/hdr/stack/mac/nr/sched_nr_pdcch.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_ue.h"

namespace srsenb {
namespace sched_nr_impl {

coreset_region::coreset_region(uint32_t         bwp_id_,
                               uint32_t         slot_idx_,
                               uint32_t         nof_td_symbols,
                               uint32_t         nof_freq_resources,
                               pdcch_dl_list_t& pdcch_list_) :
  bwp_id(bwp_id_),
  slot_idx(slot_idx_),
  nof_symbols(nof_td_symbols),
  nof_freq_res(nof_freq_resources),
  pdcch_dl_list(pdcch_list_)
{
  srsran_assert(nof_td_symbols <= SRSRAN_CORESET_DURATION_MAX,
                "Possible number of time-domain OFDM symbols in CORESET must be within {1,2,3}");
  srsran_assert(nof_freq_resources <= SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE,
                "Provided number of CORESET freq domain resources=%d is too high",
                nof_freq_resources);
}

void coreset_region::reset()
{
  dfs_tree.clear();
  saved_dfs_tree.clear();
  dci_list.clear();
  pdcch_dl_list.clear();
}

bool coreset_region::alloc_dci(pdcch_grant_type_t alloc_type, uint32_t aggr_idx, uint32_t coreset_id, slot_ue* user)
{
  srsran_assert(aggr_idx <= 4, "Invalid DCI aggregation level=%d", 1U << aggr_idx);
  srsran_assert((user == nullptr) xor
                    (alloc_type == pdcch_grant_type_t::dl_data or alloc_type == pdcch_grant_type_t::ul_data),
                "UE should be only provided for DL or UL data allocations");
  saved_dfs_tree.clear();

  alloc_record record;
  record.ue         = user;
  record.aggr_idx   = aggr_idx;
  record.alloc_type = alloc_type;
  record.idx        = pdcch_dl_list.size();
  record.coreset_id = coreset_id;
  pdcch_dl_list.emplace_back();

  // Try to allocate grant. If it fails, attempt the same grant, but using a different permutation of past grant DCI
  // positions
  do {
    bool success = alloc_dfs_node(record, 0);
    if (success) {
      // DCI record allocation successful
      dci_list.push_back(record);
      return true;
    }
    if (dfs_tree.empty()) {
      saved_dfs_tree = dfs_tree;
    }
  } while (get_next_dfs());

  // Revert steps to initial state, before dci record allocation was attempted
  dfs_tree = saved_dfs_tree;
  pdcch_dl_list.pop_back();
  return false;
}

void coreset_region::rem_last_dci()
{
  srsran_assert(not dci_list.empty(), "%s called when no PDCCH have yet been allocated", __FUNCTION__);

  // Remove DCI record
  dfs_tree.pop_back();
  dci_list.pop_back();
  pdcch_dl_list.pop_back();
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
    pdcch_dl_t& pdcch_dl      = pdcch_dl_list[record.idx];
    pdcch_dl.dci.ctx.location = node.dci_pos;
    return true;
  }

  return false;
}

srsran::span<const uint32_t> coreset_region::get_cce_loc_table(const alloc_record& record) const
{
  switch (record.alloc_type) {
    case pdcch_grant_type_t::dl_data:
      return record.ue->cfg->cc_params[record.ue->cc]
          .bwps[bwp_id]
          .coresets[record.coreset_id]
          .cce_positions[slot_idx][record.aggr_idx];
    default:
      break;
  }
  return {};
}

} // namespace sched_nr_impl
} // namespace srsenb