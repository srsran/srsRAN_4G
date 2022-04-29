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

#include "../sched_lte_common.h"
#include "sched_result.h"

#ifndef SRSRAN_PDCCH_SCHED_H
#define SRSRAN_PDCCH_SCHED_H

namespace srsenb {

class sched_ue;

/// Class responsible for managing a PDCCH CCE grid, namely CCE allocs, and avoid collisions.
class sf_cch_allocator
{
public:
  const static uint32_t MAX_CFI = 3;
  struct tree_node {
    int8_t                pucch_n_prb = -1; ///< this PUCCH resource identifier
    uint16_t              rnti        = SRSRAN_INVALID_RNTI;
    uint32_t              record_idx  = 0;
    uint32_t              dci_pos_idx = 0;
    srsran_dci_location_t dci_pos     = {0, 0};
    /// Accumulation of all PDCCH masks for the current solution (DFS path)
    pdcch_mask_t total_mask, current_mask;
    prbmask_t    total_pucch_mask;
  };
  using alloc_result_t = srsran::bounded_vector<const tree_node*, 16>;

  sf_cch_allocator() : logger(srslog::fetch_basic_logger("MAC")) {}

  void init(const sched_cell_params_t& cell_params_);
  void new_tti(tti_point tti_rx_);
  /**
   * Allocates DCI space in PDCCH and PUCCH, avoiding in the process collisions with other users
   * @param alloc_type allocation type (e.g. DL data, UL data, ctrl)
   * @param aggr_idx Aggregation level index (0..3)
   * @param user UE object or null in case of broadcast/RAR/paging allocation
   * @param has_pusch_grant If the UE has already an PUSCH grant for UCI allocated
   * @return if the allocation was successful
   */
  bool alloc_dci(alloc_type_t alloc_type, uint32_t aggr_idx, sched_ue* user = nullptr, bool has_pusch_grant = false);

  void rem_last_dci();

  // getters
  uint32_t    get_cfi() const { return current_cfix + 1; }
  void        get_allocs(alloc_result_t* vec = nullptr, pdcch_mask_t* tot_mask = nullptr, size_t idx = 0) const;
  uint32_t    nof_cces() const { return cc_cfg->nof_cce_table[current_cfix]; }
  size_t      nof_allocs() const { return dci_record_list.size(); }
  std::string result_to_string(bool verbose = false) const;

private:
  /// DCI allocation parameters
  struct alloc_record {
    bool         pusch_uci;
    uint32_t     aggr_idx;
    alloc_type_t alloc_type;
    sched_ue*    user;
  };
  const cce_cfi_position_table* get_cce_loc_table(alloc_type_t alloc_type, sched_ue* user, uint32_t cfix) const;

  // PDCCH allocation algorithm
  bool alloc_dfs_node(const alloc_record& record, uint32_t start_child_idx);
  bool get_next_dfs();

  // consts
  const sched_cell_params_t* cc_cfg = nullptr;
  srslog::basic_logger&      logger;
  srsran_pucch_cfg_t         pucch_cfg_common = {};

  // tti vars
  tti_point                 tti_rx;
  uint32_t                  current_cfix     = 0;
  uint32_t                  current_max_cfix = 0;
  std::vector<tree_node>    last_dci_dfs, temp_dci_dfs;
  std::vector<alloc_record> dci_record_list; ///< Keeps a record of all the PDCCH allocations done so far
};

// Helper methods
bool is_pucch_sr_collision(const srsran_pucch_cfg_t& ue_pucch_cfg, tti_point tti_tx_dl_ack, uint32_t n1_pucch);

} // namespace srsenb

#endif // SRSRAN_PDCCH_SCHED_H
