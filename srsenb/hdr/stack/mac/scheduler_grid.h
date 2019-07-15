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

#ifndef SRSLTE_SCHEDULER_GRID_H
#define SRSLTE_SCHEDULER_GRID_H

#include "lib/include/srslte/interfaces/sched_interface.h"
#include "scheduler_ue.h"
#include "srslte/common/bounded_bitset.h"
#include "srslte/common/log.h"
#include <vector>

namespace srsenb {

// Type of Allocation
enum class alloc_type_t { DL_BC, DL_PCCH, DL_RAR, DL_DATA, UL_DATA };

// Result of alloc attempt
struct alloc_outcome_t {
  enum result_enum { SUCCESS, DCI_COLLISION, RB_COLLISION, ERROR };
  result_enum result = ERROR;
  alloc_outcome_t()  = default;
  alloc_outcome_t(result_enum e) : result(e) {}
              operator result_enum() { return result; }
              operator bool() { return result == SUCCESS; }
  const char* to_string() const;
};

class pdcch_grid_t
{
public:
  struct alloc_t {
    uint16_t     rnti;
    srslte_dci_location_t dci_pos = {0, 0};
    pdcch_mask_t current_mask;
    pdcch_mask_t total_mask;
  };
  typedef std::vector<const alloc_t*> alloc_result_t;

  void init(srslte::log*               log_,
            srslte_regs_t*             regs,
            sched_ue::sched_dci_cce_t* common_locs,
            sched_ue::sched_dci_cce_t (*rar_locs)[10]);
  void new_tti(uint32_t tti_rx_, uint32_t start_cfi);
  bool alloc_dci(alloc_type_t alloc_type, uint32_t aggr_idx, sched_ue* user = NULL);
  bool set_cfi(uint32_t cfi);

  // getters
  uint32_t get_cfi() const { return current_cfix + 1; }
  void     get_allocs(alloc_result_t* vec = nullptr, pdcch_mask_t* tot_mask = nullptr, size_t idx = 0) const;
  uint32_t nof_cces() const { return cce_size_array[current_cfix]; }
  size_t   nof_allocs() const { return nof_dci_allocs; }
  size_t   nof_alloc_combinations() const { return prev_end - prev_start; }
  void     print_result(bool verbose = false) const;
  uint32_t get_sf_idx() const { return sf_idx; }

private:
  const static uint32_t           nof_cfis = 3;
  typedef std::pair<int, alloc_t> tree_node_t;

  void                             reset();
  const sched_ue::sched_dci_cce_t* get_cce_loc_table(alloc_type_t alloc_type, sched_ue* user) const;
  void                             update_alloc_tree(int                              node_idx,
                                                     uint32_t                         aggr_idx,
                                                     sched_ue*                        user,
                                                     alloc_type_t                     alloc_type,
                                                     const sched_ue::sched_dci_cce_t* dci_locs);

  // consts
  srslte::log*               log_h            = nullptr;
  sched_ue::sched_dci_cce_t* common_locations = nullptr;
  sched_ue::sched_dci_cce_t* rar_locations[10]        = {nullptr};
  uint32_t                   cce_size_array[nof_cfis] = {0};

  // tti vars
  uint32_t                 tti_rx       = 0;
  uint32_t                 sf_idx       = 0;
  uint32_t                 current_cfix = 0;
  size_t                   prev_start = 0, prev_end = 0;
  std::vector<tree_node_t> dci_alloc_tree;
  size_t                   nof_dci_allocs = 0;
};

class tti_grid_t
{
public:
  typedef std::pair<alloc_outcome_t, rbg_range_t> ctrl_alloc_t;

  void            init(srslte::log* log_, sched_interface::cell_cfg_t* cell_, const pdcch_grid_t& pdcch_grid);
  void            new_tti(uint32_t tti_rx_, uint32_t start_cfi);
  ctrl_alloc_t    alloc_dl_ctrl(uint32_t aggr_lvl, alloc_type_t alloc_type);
  alloc_outcome_t alloc_dl_data(sched_ue* user, const rbgmask_t& user_mask);
  alloc_outcome_t alloc_ul_data(sched_ue* user, ul_harq_proc::ul_alloc_t alloc, bool needs_pdcch);

  // getters
  uint32_t            get_avail_rbgs() const { return avail_rbg; }
  rbgmask_t&          get_dl_mask() { return dl_mask; }
  const rbgmask_t&    get_dl_mask() const { return dl_mask; }
  prbmask_t&          get_ul_mask() { return ul_mask; }
  const prbmask_t&    get_ul_mask() const { return ul_mask; }
  uint32_t            get_cfi() const { return pdcch_alloc.get_cfi(); }
  const pdcch_grid_t& get_pdcch_grid() const { return pdcch_alloc; }
  uint32_t            get_tti_rx() const { return tti_rx; }
  uint32_t            get_tti_tx_dl() const { return tti_tx_dl; }
  uint32_t            get_tti_tx_ul() const { return tti_tx_ul; }
  uint32_t            get_sfn() const { return sfn; }
  uint32_t            get_sf_idx() const { return pdcch_alloc.get_sf_idx(); }

private:
  alloc_outcome_t alloc_dl(uint32_t aggr_lvl, alloc_type_t alloc_type, rbgmask_t alloc_mask, sched_ue* user = NULL);

  // consts
  srslte::log*                 log_h    = nullptr;
  sched_interface::cell_cfg_t* cell_cfg = nullptr;
  uint32_t                     nof_prbs = 0;
  uint32_t                     nof_rbgs = 0;
  uint32_t                     si_n_rbg, rar_n_rbg = 0;

  // tti const
  uint32_t tti_rx = 10241;
  // derived
  uint32_t     tti_tx_dl = 0, tti_tx_ul = 0;
  uint32_t     sfn         = 0;
  pdcch_grid_t pdcch_alloc = {};

  // internal state
  uint32_t  avail_rbg = 0;
  rbgmask_t dl_mask   = {};
  prbmask_t ul_mask   = {};
};

} // namespace srsenb

#endif // SRSLTE_SCHEDULER_GRID_H
