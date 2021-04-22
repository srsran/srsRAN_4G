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

#include "srsenb/hdr/stack/mac/sched_phy_ch/sf_cch_allocator.h"
#include "srsenb/hdr/stack/mac/sched_grid.h"
#include "srsran/srslog/bundled/fmt/format.h"

namespace srsenb {

bool is_pucch_sr_collision(const srsran_pucch_cfg_t& ue_pucch_cfg, tti_point tti_tx_dl_ack, uint32_t n1_pucch)
{
  if (ue_pucch_cfg.sr_configured && srsran_ue_ul_sr_send_tti(&ue_pucch_cfg, tti_tx_dl_ack.to_uint())) {
    return n1_pucch == ue_pucch_cfg.n_pucch_sr;
  }
  return false;
}

void sf_cch_allocator::init(const sched_cell_params_t& cell_params_)
{
  cc_cfg           = &cell_params_;
  pucch_cfg_common = cc_cfg->pucch_cfg_common;
  dci_record_list.reserve(16);
  last_dci_dfs.reserve(16);
  temp_dci_dfs.reserve(16);
}

void sf_cch_allocator::new_tti(tti_point tti_rx_)
{
  tti_rx = tti_rx_;

  dci_record_list.clear();
  last_dci_dfs.clear();
  current_cfix     = cc_cfg->sched_cfg->min_nof_ctrl_symbols - 1;
  current_max_cfix = cc_cfg->sched_cfg->max_nof_ctrl_symbols - 1;
}

const cce_cfi_position_table*
sf_cch_allocator::get_cce_loc_table(alloc_type_t alloc_type, sched_ue* user, uint32_t cfix) const
{
  switch (alloc_type) {
    case alloc_type_t::DL_BC:
    case alloc_type_t::DL_PCCH:
      return &cc_cfg->common_locations[cfix];
    case alloc_type_t::DL_RAR:
      return &cc_cfg->rar_locations[to_tx_dl(tti_rx).sf_idx()][cfix];
    case alloc_type_t::DL_DATA:
    case alloc_type_t::UL_DATA:
      return user->get_locations(cc_cfg->enb_cc_idx, cfix + 1, to_tx_dl(tti_rx).sf_idx());
    default:
      break;
  }
  return nullptr;
}

bool sf_cch_allocator::alloc_dci(alloc_type_t alloc_type, uint32_t aggr_idx, sched_ue* user, bool has_pusch_grant)
{
  temp_dci_dfs.clear();
  uint32_t start_cfix = current_cfix;

  alloc_record record;
  record.user       = user;
  record.aggr_idx   = aggr_idx;
  record.alloc_type = alloc_type;
  record.pusch_uci  = has_pusch_grant;

  if (is_dl_ctrl_alloc(alloc_type) and nof_allocs() == 0 and cc_cfg->nof_prb() == 6 and
      current_max_cfix > current_cfix) {
    // Given that CFI is not currently dynamic for ctrl allocs, in case of SIB/RAR alloc and a low number of PRBs,
    // start with an CFI that maximizes nof potential CCE locs
    uint32_t nof_locs = 0, lowest_cfix = current_cfix;
    for (uint32_t cfix_tmp = current_max_cfix; cfix_tmp > lowest_cfix; --cfix_tmp) {
      const cce_cfi_position_table* dci_locs = get_cce_loc_table(record.alloc_type, record.user, cfix_tmp);
      if ((*dci_locs)[record.aggr_idx].size() > nof_locs) {
        nof_locs     = (*dci_locs)[record.aggr_idx].size();
        current_cfix = cfix_tmp;
      } else {
        break;
      }
    }
  }

  // Try to allocate grant. If it fails, attempt the same grant, but using a different permutation of past grant DCI
  // positions
  do {
    bool success = alloc_dfs_node(record, 0);
    if (success) {
      // DCI record allocation successful
      dci_record_list.push_back(record);

      if (is_dl_ctrl_alloc(alloc_type)) {
        // Dynamic CFI not yet supported for DL control allocations, as coderate can be exceeded
        current_max_cfix = current_cfix;
      }
      return true;
    }
    if (temp_dci_dfs.empty()) {
      temp_dci_dfs = last_dci_dfs;
    }
  } while (get_next_dfs());

  // Revert steps to initial state, before dci record allocation was attempted
  last_dci_dfs.swap(temp_dci_dfs);
  current_cfix = start_cfix;
  return false;
}

bool sf_cch_allocator::get_next_dfs()
{
  do {
    uint32_t start_child_idx = 0;
    if (last_dci_dfs.empty()) {
      // If we reach root, increase CFI
      current_cfix++;
      if (current_cfix > current_max_cfix) {
        return false;
      }
    } else {
      // Attempt to re-add last tree node, but with a higher node child index
      start_child_idx = last_dci_dfs.back().dci_pos_idx + 1;
      last_dci_dfs.pop_back();
    }
    while (last_dci_dfs.size() < dci_record_list.size() and
           alloc_dfs_node(dci_record_list[last_dci_dfs.size()], start_child_idx)) {
      start_child_idx = 0;
    }
  } while (last_dci_dfs.size() < dci_record_list.size());

  // Finished computation of next DFS node
  return true;
}

bool sf_cch_allocator::alloc_dfs_node(const alloc_record& record, uint32_t start_dci_idx)
{
  // Get DCI Location Table
  const cce_cfi_position_table* dci_locs = get_cce_loc_table(record.alloc_type, record.user, current_cfix);
  if (dci_locs == nullptr or (*dci_locs)[record.aggr_idx].empty()) {
    return false;
  }
  const cce_position_list& dci_pos_list = (*dci_locs)[record.aggr_idx];
  if (start_dci_idx >= dci_pos_list.size()) {
    return false;
  }

  tree_node node;
  node.dci_pos_idx = start_dci_idx;
  node.dci_pos.L   = record.aggr_idx;
  node.rnti        = record.user != nullptr ? record.user->get_rnti() : SRSRAN_INVALID_RNTI;
  node.current_mask.resize(nof_cces());
  // get cumulative pdcch & pucch masks
  if (not last_dci_dfs.empty()) {
    node.total_mask       = last_dci_dfs.back().total_mask;
    node.total_pucch_mask = last_dci_dfs.back().total_pucch_mask;
  } else {
    node.total_mask.resize(nof_cces());
    node.total_pucch_mask.resize(cc_cfg->nof_prb());
  }

  for (; node.dci_pos_idx < dci_pos_list.size(); ++node.dci_pos_idx) {
    node.dci_pos.ncce = dci_pos_list[node.dci_pos_idx];

    if (record.alloc_type == alloc_type_t::DL_DATA and not record.pusch_uci) {
      // The UE needs to allocate space in PUCCH for HARQ-ACK
      pucch_cfg_common.n_pucch = node.dci_pos.ncce + pucch_cfg_common.N_pucch_1;

      if (is_pucch_sr_collision(record.user->get_ue_cfg().pucch_cfg, to_tx_dl_ack(tti_rx), pucch_cfg_common.n_pucch)) {
        // avoid collision of HARQ-ACK with own SR n(1)_pucch
        continue;
      }

      node.pucch_n_prb = srsran_pucch_n_prb(&cc_cfg->cfg.cell, &pucch_cfg_common, 0);
      if (not cc_cfg->sched_cfg->pucch_mux_enabled and node.total_pucch_mask.test(node.pucch_n_prb)) {
        // PUCCH allocation would collide with other PUCCH/PUSCH grants. Try another CCE position
        continue;
      }
    }

    node.current_mask.reset();
    node.current_mask.fill(node.dci_pos.ncce, node.dci_pos.ncce + (1U << record.aggr_idx));
    if ((node.total_mask & node.current_mask).any()) {
      // there is a PDCCH collision. Try another CCE position
      continue;
    }

    // Allocation successful
    node.total_mask |= node.current_mask;
    if (node.pucch_n_prb >= 0) {
      node.total_pucch_mask.set(node.pucch_n_prb);
    }
    last_dci_dfs.push_back(node);
    return true;
  }

  return false;
}

void sf_cch_allocator::rem_last_dci()
{
  assert(not dci_record_list.empty());

  // Remove DCI record
  last_dci_dfs.pop_back();
  dci_record_list.pop_back();
}

void sf_cch_allocator::get_allocs(alloc_result_t* vec, pdcch_mask_t* tot_mask, size_t idx) const
{
  if (vec != nullptr) {
    vec->clear();

    vec->resize(last_dci_dfs.size());
    for (uint32_t i = 0; i < last_dci_dfs.size(); ++i) {
      (*vec)[i] = &last_dci_dfs[i];
    }
  }

  if (tot_mask != nullptr) {
    if (last_dci_dfs.empty()) {
      tot_mask->resize(nof_cces());
      tot_mask->reset();
    } else {
      *tot_mask = last_dci_dfs.back().total_mask;
    }
  }
}

std::string sf_cch_allocator::result_to_string(bool verbose) const
{
  fmt::basic_memory_buffer<char, 1024> strbuf;
  if (dci_record_list.empty()) {
    fmt::format_to(strbuf, "SCHED: PDCCH allocations cfi={}, nof_cce={}, No allocations.\n", get_cfi(), nof_cces());
  } else {
    fmt::format_to(strbuf,
                   "SCHED: PDCCH allocations cfi={}, nof_cce={}, nof_allocs={}, total PDCCH mask=0x{:x}",
                   get_cfi(),
                   nof_cces(),
                   nof_allocs(),
                   last_dci_dfs.back().total_mask);
    alloc_result_t vec;
    get_allocs(&vec);
    if (verbose) {
      fmt::format_to(strbuf, ", allocations:\n");
      for (const auto& dci_alloc : vec) {
        fmt::format_to(strbuf,
                       "  > rnti=0x{:0x}: 0x{:x} / 0x{:x}\n",
                       dci_alloc->rnti,
                       dci_alloc->current_mask,
                       dci_alloc->total_mask);
      }
    } else {
      fmt::format_to(strbuf, ".\n");
    }
  }
  return fmt::to_string(strbuf);
}

} // namespace srsenb
