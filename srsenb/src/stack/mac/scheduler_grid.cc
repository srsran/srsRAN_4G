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

#include "srsenb/hdr/stack/mac/scheduler_grid.h"
#include "srsenb/hdr/stack/mac/scheduler.h"
#include <srslte/interfaces/sched_interface.h>

namespace srsenb {

const char* alloc_outcome_t::to_string() const
{
  switch (result) {
    case SUCCESS:
      return "success";
    case DCI_COLLISION:
      return "dci_collision";
    case RB_COLLISION:
      return "rb_collision";
    case ERROR:
      return "error";
  }
  return "unknown error";
}

/*******************************************************
 *             PDCCH Allocation Methods
 *******************************************************/

void pdcch_grid_t::init(srslte::log*               log_,
                        srslte_regs_t*             regs,
                        sched_ue::sched_dci_cce_t* common_locs,
                        sched_ue::sched_dci_cce_t (*rar_locs)[10])
{
  log_h            = log_;
  common_locations = common_locs;
  for (uint32_t cfix = 0; cfix < 3; ++cfix) {
    rar_locations[cfix] = rar_locs[cfix];
  }

  // precompute nof_cces
  for (uint32_t cfix = 0; cfix < nof_cfis; ++cfix) {
    int ret = srslte_regs_pdcch_ncce(regs, cfix + 1);
    if (ret < 0) {
      log_h->error("SCHED: Failed to calculate the number of CCEs in the PDCCH\n");
    }
    cce_size_array[cfix] = (uint32_t)ret;
  }

  reset();
}

void pdcch_grid_t::new_tti(uint32_t tti_rx_, uint32_t start_cfi)
{
  tti_rx       = tti_rx_;
  sf_idx       = TTI_TX(tti_rx) % 10;
  current_cfix = start_cfi - 1;
  reset();
}

const sched_ue::sched_dci_cce_t* pdcch_grid_t::get_cce_loc_table(alloc_type_t alloc_type, sched_ue* user) const
{
  switch (alloc_type) {
    case alloc_type_t::DL_BC:
      return &common_locations[current_cfix];
    case alloc_type_t::DL_PCCH:
      return &common_locations[current_cfix];
    case alloc_type_t::DL_RAR:
      return &rar_locations[current_cfix][sf_idx];
    case alloc_type_t::DL_DATA:
      return user->get_locations(current_cfix + 1, sf_idx);
    case alloc_type_t::UL_DATA:
      return user->get_locations(current_cfix + 1, sf_idx);
  }
  return NULL;
}

bool pdcch_grid_t::alloc_dci(alloc_type_t alloc_type, uint32_t aggr_idx, sched_ue* user)
{
  // FIXME: Make the alloc tree update lazy

  /* Get DCI Location Table */
  const sched_ue::sched_dci_cce_t* dci_locs = get_cce_loc_table(alloc_type, user);
  if (!dci_locs) {
    return false;
  }

  /* Search for potential DCI positions */
  if (prev_end > 0) {
    for (size_t j = prev_start; j < prev_end; ++j) {
      update_alloc_tree((int)j, aggr_idx, user, alloc_type, dci_locs);
    }
  } else {
    update_alloc_tree(-1, aggr_idx, user, alloc_type, dci_locs);
  }

  // if no pdcch space was available
  if (dci_alloc_tree.size() == prev_end) {
    return false;
  }

  prev_start = prev_end;
  prev_end   = dci_alloc_tree.size();

  nof_dci_allocs++;

  return true;
}

void pdcch_grid_t::update_alloc_tree(int                              parent_node_idx,
                                     uint32_t                         aggr_idx,
                                     sched_ue*                        user,
                                     alloc_type_t                     alloc_type,
                                     const sched_ue::sched_dci_cce_t* dci_locs)
{
  alloc_t alloc;
  alloc.rnti     = (user != nullptr) ? user->get_rnti() : (uint16_t)0u;
  alloc.dci_pos.L = aggr_idx;

  // get cumulative pdcch mask
  pdcch_mask_t cum_mask;
  if (parent_node_idx >= 0) {
    cum_mask = dci_alloc_tree[parent_node_idx].second.total_mask;
  } else {
    cum_mask.resize(nof_cces());
  }

  uint32_t nof_locs = dci_locs->nof_loc[aggr_idx];
  for (uint32_t i = 0; i < nof_locs; ++i) {
    uint32_t startpos = dci_locs->cce_start[aggr_idx][i];

    if (alloc_type == alloc_type_t::DL_DATA and user->pucch_sr_collision(TTI_TX(tti_rx), startpos)) {
      // will cause a collision in the PUCCH
      continue;
    }

    pdcch_mask_t alloc_mask(nof_cces());
    alloc_mask.fill(startpos, startpos + (1u << aggr_idx));
    if ((cum_mask & alloc_mask).any()) {
      // there is collision. Try another mask
      continue;
    }

    // Allocation successful
    alloc.current_mask = alloc_mask;
    alloc.total_mask   = cum_mask | alloc_mask;
    alloc.dci_pos.ncce = startpos;

    // Prune if repetition
    uint32_t j = prev_end;
    for (; j < dci_alloc_tree.size(); ++j) {
      if (dci_alloc_tree[j].second.total_mask == alloc.total_mask) {
        break;
      }
    }
    if (j < dci_alloc_tree.size()) {
      continue;
    }

    // Register allocation
    dci_alloc_tree.emplace_back(parent_node_idx, alloc);
  }
}

bool pdcch_grid_t::set_cfi(uint32_t cfi)
{
  current_cfix = cfi - 1;
  // FIXME: use this function for dynamic cfi
  // FIXME: The estimation of the number of required prbs in metric depends on CFI. Analyse the consequences
  return true;
}

void pdcch_grid_t::reset()
{
  prev_start = 0;
  prev_end   = 0;
  dci_alloc_tree.clear();
  nof_dci_allocs = 0;
}

void pdcch_grid_t::get_allocs(alloc_result_t* vec, pdcch_mask_t* tot_mask, size_t idx) const
{
  // if alloc tree is empty
  if (prev_start == prev_end) {
    if (vec)
      vec->clear();
    if (tot_mask) {
      tot_mask->reset();
    }
    return;
  }

  // set vector of allocations
  if (vec) {
    vec->clear();
    size_t i = prev_start + idx;
    while (dci_alloc_tree[i].first >= 0) {
      vec->push_back(&dci_alloc_tree[i].second);
      i = (size_t)dci_alloc_tree[i].first;
    }
    vec->push_back(&dci_alloc_tree[i].second);
    std::reverse(vec->begin(), vec->end());
  }

  // set final cce mask
  if (tot_mask) {
    *tot_mask = dci_alloc_tree[prev_start + idx].second.total_mask;
  }
}

void pdcch_grid_t::print_result(bool verbose) const
{
  if (prev_start == prev_end) {
    log_h->info("SCHED: No DCI allocations\n");
  }

  std::stringstream ss;
  ss << "SCHED: cfi=" << get_cfi() << ", " << prev_end - prev_start << " DCI allocation combinations:\n";
  // get all the possible combinations of DCI allocations
  uint32_t count = 0;
  for (size_t i = prev_start; i < prev_end; ++i) {
    alloc_result_t vec;
    pdcch_mask_t   tot_mask;
    get_allocs(&vec, &tot_mask, i - prev_start);

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

  log_h->info("%s", ss.str().c_str());
}

/*******************************************************
 *          TTI resource Scheduling Methods
 *******************************************************/

void tti_grid_t::init(srslte::log* log_, sched_interface::cell_cfg_t* cell_, const pdcch_grid_t& pdcch_grid)
{
  log_h      = log_;
  cell_cfg   = cell_;
  nof_prbs   = cell_cfg->cell.nof_prb;
  uint32_t P = srslte_ra_type0_P(cell_cfg->cell.nof_prb);
  nof_rbgs   = srslte::ceil_div(cell_cfg->cell.nof_prb, P);
  si_n_rbg   = srslte::ceil_div(4, P);
  rar_n_rbg  = srslte::ceil_div(3, P);

  pdcch_alloc = pdcch_grid;
}

void tti_grid_t::new_tti(uint32_t tti_rx_, uint32_t start_cfi)
{
  tti_rx = tti_rx_;

  // derived
  tti_tx_dl = TTI_TX(tti_rx);
  tti_tx_ul = TTI_RX_ACK(tti_rx);
  sfn       = tti_tx_dl / 10;

  // internal state
  avail_rbg = nof_rbgs;
  dl_mask.reset();
  dl_mask.resize(nof_rbgs);
  ul_mask.reset();
  ul_mask.resize(nof_prbs);
  pdcch_alloc.new_tti(tti_rx, start_cfi);
}

alloc_outcome_t tti_grid_t::alloc_dl(uint32_t aggr_lvl, alloc_type_t alloc_type, rbgmask_t alloc_mask, sched_ue* user)
{
  // Check RBG collision
  if ((dl_mask & alloc_mask).any()) {
    return alloc_outcome_t::RB_COLLISION;
  }

  // Allocate DCI in PDCCH
  if (not pdcch_alloc.alloc_dci(alloc_type, aggr_lvl, user)) {
    return alloc_outcome_t::DCI_COLLISION;
  }

  // Allocate RBGs
  dl_mask |= alloc_mask;
  avail_rbg -= alloc_mask.count();

  return alloc_outcome_t::SUCCESS;
}

tti_grid_t::ctrl_alloc_t tti_grid_t::alloc_dl_ctrl(uint32_t aggr_lvl, alloc_type_t alloc_type)
{
  rbg_range_t range;
  range.rbg_start = nof_rbgs - avail_rbg;
  range.rbg_end   = range.rbg_start + ((alloc_type == alloc_type_t::DL_RAR) ? rar_n_rbg : si_n_rbg);

  if (alloc_type != alloc_type_t::DL_RAR and alloc_type != alloc_type_t::DL_BC and
      alloc_type != alloc_type_t::DL_PCCH) {
    log_h->error("SCHED: DL control allocations must be RAR/BC/PDCCH\n");
    return {alloc_outcome_t::ERROR, range};
  }
  // Setup range starting from left
  if (range.rbg_end > nof_rbgs) {
    return {alloc_outcome_t::RB_COLLISION, range};
  }

  // allocate DCI and RBGs
  rbgmask_t new_mask(dl_mask.size());
  new_mask.fill(range.rbg_start, range.rbg_end);
  return {alloc_dl(aggr_lvl, alloc_type, new_mask), range};
}

alloc_outcome_t tti_grid_t::alloc_dl_data(sched_ue* user, const rbgmask_t& user_mask)
{
  srslte_dci_format_t dci_format = user->get_dci_format();
  uint32_t aggr_level = user->get_aggr_level(srslte_dci_format_sizeof(&cell_cfg->cell, NULL, NULL, dci_format));
  return alloc_dl(aggr_level, alloc_type_t::DL_DATA, user_mask, user);
}

alloc_outcome_t tti_grid_t::alloc_ul_data(sched_ue* user, ul_harq_proc::ul_alloc_t alloc, bool needs_pdcch)
{
  if (alloc.RB_start + alloc.L > ul_mask.size()) {
    return alloc_outcome_t::ERROR;
  }

  prbmask_t newmask(ul_mask.size());
  newmask.fill(alloc.RB_start, alloc.RB_start + alloc.L);
  if ((ul_mask & newmask).any()) {
    return alloc_outcome_t::RB_COLLISION;
  }

  // Generate PDCCH except for RAR and non-adaptive retx
  if (needs_pdcch) {
    uint32_t aggr_idx = user->get_aggr_level(srslte_dci_format_sizeof(&cell_cfg->cell, NULL, NULL, SRSLTE_DCI_FORMAT0));
    if (not pdcch_alloc.alloc_dci(alloc_type_t::UL_DATA, aggr_idx, user)) {
      return alloc_outcome_t::DCI_COLLISION;
    }
  }

  ul_mask |= newmask;

  return alloc_outcome_t::SUCCESS;
}

} // namespace srsenb