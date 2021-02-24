/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsenb/hdr/stack/mac/sched_phy_ch/sf_cch_allocator.h"
#include "srsenb/hdr/stack/mac/sched_grid.h"

namespace srsenb {

void sf_cch_allocator::alloc_tree_t::reset()
{
  prev_start = 0;
  prev_end   = 0;
  dci_alloc_tree.clear();
}

void sf_cch_allocator::init(const sched_cell_params_t& cell_params_)
{
  cc_cfg = &cell_params_;

  // init alloc trees
  alloc_trees.reserve(cc_cfg->sched_cfg->max_nof_ctrl_symbols);
  for (uint32_t i = 0; i < cc_cfg->sched_cfg->max_nof_ctrl_symbols; ++i) {
    alloc_trees.emplace_back(cc_cfg->nof_cce_table[i]);
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

bool sf_cch_allocator::alloc_dci(alloc_type_t alloc_type, uint32_t aggr_idx, sched_ue* user)
{
  // TODO: Make the alloc tree update lazy
  alloc_record_t record{.user = user, .aggr_idx = aggr_idx, .alloc_type = alloc_type};

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
      ret |= add_tree_node_leaves(tree, (int)j, record, *dci_locs, to_tx_dl(tti_rx));
    }
  } else {
    ret = add_tree_node_leaves(tree, -1, record, *dci_locs, to_tx_dl(tti_rx));
  }

  if (ret) {
    tree.prev_start = tree.prev_end;
    tree.prev_end   = tree.dci_alloc_tree.size();
  }

  return ret;
}

//! Algorithm to compute a valid PDCCH allocation
bool sf_cch_allocator::add_tree_node_leaves(alloc_tree_t&                 tree,
                                            int                           parent_node_idx,
                                            const alloc_record_t&         dci_record,
                                            const cce_cfi_position_table& dci_locs,
                                            tti_point                     tti_tx_dl)
{
  bool ret = false;

  alloc_t alloc;
  alloc.rnti      = (dci_record.user != nullptr) ? dci_record.user->get_rnti() : (uint16_t)0u;
  alloc.dci_pos.L = dci_record.aggr_idx;

  // get cumulative pdcch mask
  pdcch_mask_t cum_mask;
  if (parent_node_idx >= 0) {
    cum_mask = tree.dci_alloc_tree[parent_node_idx].node.total_mask;
  } else {
    cum_mask.resize(tree.nof_cces);
  }

  for (uint32_t i = 0; i < dci_locs[dci_record.aggr_idx].size(); ++i) {
    uint32_t startpos = dci_locs[dci_record.aggr_idx][i];

    if (dci_record.alloc_type == alloc_type_t::DL_DATA and dci_record.user->pucch_sr_collision(tti_tx_dl, startpos)) {
      // will cause a collision in the PUCCH
      continue;
    }

    pdcch_mask_t alloc_mask(tree.nof_cces);
    alloc_mask.fill(startpos, startpos + (1u << dci_record.aggr_idx));
    if ((cum_mask & alloc_mask).any()) {
      // there is collision. Try another mask
      continue;
    }

    // Allocation successful
    alloc.current_mask = alloc_mask;
    alloc.total_mask   = cum_mask | alloc_mask;
    alloc.dci_pos.ncce = startpos;

    // Prune if repetition
    uint32_t j = tree.prev_end;
    for (; j < tree.dci_alloc_tree.size(); ++j) {
      if (tree.dci_alloc_tree[j].node.total_mask == alloc.total_mask) {
        break;
      }
    }
    if (j < tree.dci_alloc_tree.size()) {
      continue;
    }

    // Register allocation
    tree.dci_alloc_tree.emplace_back(parent_node_idx, alloc);
    ret = true;
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
  // TODO: The estimation of the number of required prbs in metric depends on CFI. Analyse the consequences
  return true;
}

void sf_cch_allocator::get_allocs(alloc_result_t* vec, pdcch_mask_t* tot_mask, size_t idx) const
{
  auto& tree = alloc_trees[current_cfix];
  // if alloc tree is empty
  if (tree.prev_start == tree.prev_end) {
    if (vec != nullptr) {
      vec->clear();
    }
    if (tot_mask != nullptr) {
      tot_mask->resize(nof_cces());
      tot_mask->reset();
    }
    return;
  }

  // set vector of allocations
  if (vec != nullptr) {
    vec->clear();
    size_t i = tree.prev_start + idx;
    while (tree.dci_alloc_tree[i].parent_idx >= 0) {
      vec->push_back(&tree.dci_alloc_tree[i].node);
      i = (size_t)tree.dci_alloc_tree[i].parent_idx;
    }
    vec->push_back(&tree.dci_alloc_tree[i].node);
    std::reverse(vec->begin(), vec->end());
  }

  // set final cce mask
  if (tot_mask != nullptr) {
    *tot_mask = tree.dci_alloc_tree[tree.prev_start + idx].node.total_mask;
  }
}

std::string sf_cch_allocator::result_to_string(bool verbose) const
{
  auto&             tree = alloc_trees[current_cfix];
  std::stringstream ss;
  ss << "cfi=" << get_cfi() << ", mask_size=" << nof_cces() << ", " << tree.prev_end - tree.prev_start
     << " DCI allocation combinations:\n";
  // get all the possible combinations of DCI allocations
  uint32_t count = 0;
  for (size_t i = tree.prev_start; i < tree.prev_end; ++i) {
    alloc_result_t vec;
    pdcch_mask_t   tot_mask;
    get_allocs(&vec, &tot_mask, i - tree.prev_start);

    ss << "                          combination " << count << ": mask=0x" << tot_mask.to_hex().c_str();
    if (verbose) {
      ss << ", DCI allocs:\n";
      for (const auto& dci_alloc : vec) {
        char hex[5];
        sprintf(hex, "%x", dci_alloc->rnti);
        ss << "                          > rnti=0x" << hex << ": " << dci_alloc->current_mask.to_hex().c_str() << " / "
           << dci_alloc->total_mask.to_hex().c_str() << "\n";
      }
    } else {
      ss << "\n";
    }
    count++;
  }

  return ss.str();
}

} // namespace srsenb