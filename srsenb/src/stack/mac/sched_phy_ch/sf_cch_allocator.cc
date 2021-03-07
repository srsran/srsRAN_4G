/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsenb/hdr/stack/mac/sched_phy_ch/sf_cch_allocator.h"
#include "srsenb/hdr/stack/mac/sched_grid.h"
#include "srslte/srslog/bundled/fmt/format.h"

namespace srsenb {

void sf_cch_allocator::init(const sched_cell_params_t& cell_params_)
{
  cc_cfg           = &cell_params_;
  pucch_cfg_common = cc_cfg->pucch_cfg_common;

  // init alloc trees
  alloc_trees.reserve(cc_cfg->sched_cfg->max_nof_ctrl_symbols);
  for (uint32_t i = 0; i < cc_cfg->sched_cfg->max_nof_ctrl_symbols; ++i) {
    alloc_trees.emplace_back(i + 1, *cc_cfg, pucch_cfg_common);
  }
}

void sf_cch_allocator::new_tti(tti_point tti_rx_)
{
  tti_rx = tti_rx_;

  // Reset back all CFIs
  for (auto& t : alloc_trees) {
    t.reset();
  }
  dci_record_list.clear();
  current_cfix = cc_cfg->sched_cfg->min_nof_ctrl_symbols - 1;
}

const cce_cfi_position_table*
sf_cch_allocator::get_cce_loc_table(alloc_type_t alloc_type, sched_ue* user, uint32_t cfix) const
{
  switch (alloc_type) {
    case alloc_type_t::DL_BC:
      return &cc_cfg->common_locations[cfix];
    case alloc_type_t::DL_PCCH:
      return &cc_cfg->common_locations[cfix];
    case alloc_type_t::DL_RAR:
      return &cc_cfg->rar_locations[to_tx_dl(tti_rx).sf_idx()][cfix];
    case alloc_type_t::DL_DATA:
      return user->get_locations(cc_cfg->enb_cc_idx, cfix + 1, to_tx_dl(tti_rx).sf_idx());
    case alloc_type_t::UL_DATA:
      return user->get_locations(cc_cfg->enb_cc_idx, cfix + 1, to_tx_dl(tti_rx).sf_idx());
    default:
      break;
  }
  return nullptr;
}

bool sf_cch_allocator::alloc_dci(alloc_type_t alloc_type, uint32_t aggr_idx, sched_ue* user, bool has_pusch_grant)
{
  // TODO: Make the alloc tree update lazy
  alloc_record_t record{.user = user, .aggr_idx = aggr_idx, .alloc_type = alloc_type, .pusch_uci = has_pusch_grant};

  // Try to allocate user in PDCCH for given CFI. If it fails, increment CFI.
  uint32_t first_cfi = get_cfi();
  bool     success;
  do {
    success = alloc_dci_record(record, get_cfi() - 1);
  } while (not success and get_cfi() < cc_cfg->sched_cfg->max_nof_ctrl_symbols and set_cfi(get_cfi() + 1));

  if (not success) {
    // DCI allocation failed. go back to original CFI
    if (get_cfi() != first_cfi and not set_cfi(first_cfi)) {
      logger.error("SCHED: Failed to return back to original PDCCH state");
    }
    return false;
  }

  // DCI record allocation successful
  dci_record_list.push_back(record);
  return true;
}

bool sf_cch_allocator::alloc_dci_record(const alloc_record_t& record, uint32_t cfix)
{
  bool  ret  = false;
  auto& tree = alloc_trees[cfix];

  // Get DCI Location Table
  const cce_cfi_position_table* dci_locs = get_cce_loc_table(record.alloc_type, record.user, cfix);
  if (dci_locs == nullptr or (*dci_locs)[record.aggr_idx].empty()) {
    return ret;
  }

  if (tree.prev_end > 0) {
    for (size_t j = tree.prev_start; j < tree.prev_end; ++j) {
      ret |= tree.add_tree_node_leaves((int)j, record, *dci_locs, tti_rx);
    }
  } else {
    ret = tree.add_tree_node_leaves(-1, record, *dci_locs, tti_rx);
  }

  if (ret) {
    tree.prev_start = tree.prev_end;
    tree.prev_end   = tree.dci_alloc_tree.size();
  }

  return ret;
}

bool sf_cch_allocator::set_cfi(uint32_t cfi)
{
  if (cfi < cc_cfg->sched_cfg->min_nof_ctrl_symbols or cfi > cc_cfg->sched_cfg->max_nof_ctrl_symbols) {
    logger.error("Invalid CFI value. Defaulting to current CFI.");
    return false;
  }

  uint32_t new_cfix = cfi - 1;
  if (new_cfix == current_cfix) {
    return true;
  }

  // setup new PDCCH alloc tree
  auto& new_tree = alloc_trees[new_cfix];
  new_tree.reset();

  if (not dci_record_list.empty()) {
    // there are already PDCCH allocations

    // Rebuild Allocation Tree
    bool ret = true;
    for (const auto& old_record : dci_record_list) {
      ret &= alloc_dci_record(old_record, new_cfix);
    }

    if (not ret) {
      // Fail to rebuild allocation tree. Go back to previous CFI
      return false;
    }
  }

  current_cfix = new_cfix;
  return true;
}

void sf_cch_allocator::get_allocs(alloc_result_t* vec, pdcch_mask_t* tot_mask, size_t idx) const
{
  alloc_trees[current_cfix].get_allocs(vec, tot_mask, idx);
}

std::string sf_cch_allocator::result_to_string(bool verbose) const
{
  return alloc_trees[current_cfix].result_to_string(verbose);
}

sf_cch_allocator::alloc_tree_t::alloc_tree_t(uint32_t                   this_cfi,
                                             const sched_cell_params_t& cc_params,
                                             srslte_pucch_cfg_t&        pucch_cfg) :
  cfi(this_cfi), cc_cfg(&cc_params), pucch_cfg(&pucch_cfg), nof_cces(cc_params.nof_cce_table[this_cfi - 1])
{
  dci_alloc_tree.reserve(8);
}

void sf_cch_allocator::alloc_tree_t::reset()
{
  prev_start = 0;
  prev_end   = 0;
  dci_alloc_tree.clear();
}

bool is_pucch_sr_collision(const srslte_pucch_cfg_t& pucch_cfg, tti_point tti_tx_dl, uint32_t n1_pucch)
{
  if (pucch_cfg.sr_configured && srslte_ue_ul_sr_send_tti(&pucch_cfg, tti_tx_dl.to_uint())) {
    return n1_pucch == pucch_cfg.n_pucch_sr;
  }
  return false;
}

/// Algorithm to compute a valid PDCCH allocation
bool sf_cch_allocator::alloc_tree_t::add_tree_node_leaves(int                           parent_node_idx,
                                                          const alloc_record_t&         dci_record,
                                                          const cce_cfi_position_table& dci_locs,
                                                          tti_point                     tti_rx_)
{
  bool ret = false;

  alloc_t alloc;
  alloc.rnti      = (dci_record.user != nullptr) ? dci_record.user->get_rnti() : SRSLTE_INVALID_RNTI;
  alloc.dci_pos.L = dci_record.aggr_idx;

  // get cumulative pdcch & pucch masks
  pdcch_mask_t parent_total_mask;
  prbmask_t    parent_pucch_mask;
  if (parent_node_idx >= 0) {
    parent_total_mask = dci_alloc_tree[parent_node_idx].node.total_mask;
    parent_pucch_mask = dci_alloc_tree[parent_node_idx].node.total_pucch_mask;
  } else {
    parent_total_mask.resize(nof_cces);
    parent_pucch_mask.resize(cc_cfg->nof_prb());
  }

  for (uint32_t i = 0; i < dci_locs[dci_record.aggr_idx].size(); ++i) {
    int8_t   pucch_prbidx = -1;
    uint32_t ncce_pos     = dci_locs[dci_record.aggr_idx][i];

    if (dci_record.alloc_type == alloc_type_t::DL_DATA and not dci_record.pusch_uci) {
      // The UE needs to allocate space in PUCCH for HARQ-ACK
      pucch_cfg->n_pucch = ncce_pos + pucch_cfg->N_pucch_1;

      if (is_pucch_sr_collision(*pucch_cfg, to_tx_dl_ack(tti_rx_), pucch_cfg->n_pucch)) {
        // avoid collision of HARQ-ACK with own SR n(1)_pucch
        continue;
      }

      pucch_prbidx = srslte_pucch_n_prb(&cc_cfg->cfg.cell, pucch_cfg, 0);
      if (not cc_cfg->sched_cfg->pucch_mux_enabled and parent_pucch_mask.test(pucch_prbidx)) {
        // PUCCH allocation would collide with other PUCCH/PUSCH grants. Try another CCE position
        continue;
      }
    }

    pdcch_mask_t alloc_mask(nof_cces);
    alloc_mask.fill(ncce_pos, ncce_pos + (1u << dci_record.aggr_idx));
    if ((parent_total_mask & alloc_mask).any()) {
      // there is a PDCCH collision. Try another CCE position
      continue;
    }

    // Allocation successful
    alloc.current_mask     = alloc_mask;
    alloc.total_mask       = parent_total_mask | alloc_mask;
    alloc.dci_pos.ncce     = ncce_pos;
    alloc.pucch_n_prb      = pucch_prbidx;
    alloc.total_pucch_mask = parent_pucch_mask;
    if (pucch_prbidx >= 0) {
      alloc.total_pucch_mask.set(pucch_prbidx);
    }

    // Prune if repetition of total_masks
    uint32_t j = prev_end;
    for (; j < dci_alloc_tree.size(); ++j) {
      if (dci_alloc_tree[j].node.total_mask == alloc.total_mask) {
        // leave nested for-loop
        break;
      }
    }
    if (j < dci_alloc_tree.size()) {
      continue;
    }

    // Register allocation
    dci_alloc_tree.emplace_back(parent_node_idx, alloc);
    ret = true;
  }

  return ret;
}

void sf_cch_allocator::alloc_tree_t::get_allocs(alloc_result_t* vec, pdcch_mask_t* tot_mask, size_t idx) const
{
  // if alloc tree is empty
  if (prev_start == prev_end) {
    if (vec != nullptr) {
      vec->clear();
    }
    if (tot_mask != nullptr) {
      tot_mask->resize(nof_cces);
      tot_mask->reset();
    }
    return;
  }

  // set vector of allocations
  if (vec != nullptr) {
    vec->clear();
    size_t i = prev_start + idx;
    while (dci_alloc_tree[i].parent_idx >= 0) {
      vec->push_back(&dci_alloc_tree[i].node);
      i = (size_t)dci_alloc_tree[i].parent_idx;
    }
    vec->push_back(&dci_alloc_tree[i].node);
    std::reverse(vec->begin(), vec->end());
  }

  // set final cce mask
  if (tot_mask != nullptr) {
    *tot_mask = dci_alloc_tree[prev_start + idx].node.total_mask;
  }
}

std::string sf_cch_allocator::alloc_tree_t::result_to_string(bool verbose) const
{
  // get all the possible combinations of DCI pos allocations
  fmt::basic_memory_buffer<char, 1024> strbuf;
  fmt::format_to(strbuf,
                 "SCHED: PDCCH allocations cfi={}, nof_cce={}, {} possible combinations:\n",
                 cfi,
                 nof_cces,
                 prev_end - prev_start);
  uint32_t count = 0;
  for (size_t i = prev_start; i < prev_end; ++i) {
    alloc_result_t vec;
    pdcch_mask_t   tot_mask;
    get_allocs(&vec, &tot_mask, i - prev_start);

    fmt::format_to(strbuf, "[{}]: total mask=0x{}", count, tot_mask.to_hex().c_str());
    if (verbose) {
      fmt::format_to(strbuf, ", allocations:\n");
      for (const auto& dci_alloc : vec) {
        fmt::format_to(strbuf,
                       "  > rnti=0x{:0x}: 0x{} / 0x{}\n",
                       dci_alloc->rnti,
                       dci_alloc->current_mask.to_hex().c_str(),
                       dci_alloc->total_mask.to_hex().c_str());
      }
    } else {
      fmt::format_to(strbuf, "\n");
    }
    count++;
  }
  return fmt::to_string(strbuf);
}

} // namespace srsenb
