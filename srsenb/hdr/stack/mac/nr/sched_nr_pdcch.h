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

#ifndef SRSRAN_SCHED_NR_PDCCH_H
#define SRSRAN_SCHED_NR_PDCCH_H

#include "srsenb/hdr/stack/mac/nr/sched_nr_cfg.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/bounded_vector.h"
#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/phch/dci.h"

namespace srsenb {

namespace sched_nr_impl {

using coreset_bitmap = srsran::bounded_bitset<SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE * SRSRAN_CORESET_DURATION_MAX, true>;

enum class pdcch_grant_type_t { sib, rar, dl_data, ul_data };

class slot_ue;

using bwp_cfg_t = sched_nr_interface::bwp_cfg_t;

class coreset_region
{
public:
  coreset_region(const bwp_params_t& bwp_cfg_,
                 uint32_t            coreset_id_,
                 uint32_t            slot_idx,
                 pdcch_dl_list_t&    pdcch_dl_list,
                 pdcch_ul_list_t&    pdcch_ul_list);
  void reset();

  /**
   * Allocates DCI space in PDCCH, avoiding in the process collisions with other users
   * @param pdcch_grant_type_t allocation type (e.g. DL data, UL data, SIB)
   * @param aggr_idx Aggregation level index (0..4)
   * @param user UE object or null in case of broadcast/RAR/paging allocation
   * @return if the allocation was successful
   */
  bool alloc_dci(pdcch_grant_type_t alloc_type, uint32_t aggr_idx, uint32_t search_space_id, slot_ue* user = nullptr);

  void rem_last_dci();

  uint32_t get_td_symbols() const { return coreset_cfg->duration; }
  uint32_t get_freq_resources() const { return nof_freq_res; }
  uint32_t nof_cces() const { return nof_freq_res * get_td_symbols(); }
  size_t   nof_allocs() const { return dfs_tree.size(); }

private:
  const srsran_coreset_t* coreset_cfg;
  uint32_t                coreset_id;
  uint32_t                slot_idx;
  uint32_t                nof_freq_res = 0;
  const bwp_cce_pos_list& rar_cce_list;

  // List of PDCCH grants
  struct alloc_record {
    uint32_t           aggr_idx;
    uint32_t           ss_id;
    uint32_t           idx;
    pdcch_grant_type_t alloc_type;
    slot_ue*           ue;
  };
  srsran::bounded_vector<alloc_record, 2 * MAX_GRANTS> dci_list;
  pdcch_dl_list_t&                                     pdcch_dl_list;
  pdcch_ul_list_t&                                     pdcch_ul_list;

  // DFS decision tree of PDCCH grants
  struct tree_node {
    uint16_t              rnti        = SRSRAN_INVALID_RNTI;
    uint32_t              record_idx  = 0;
    uint32_t              dci_pos_idx = 0;
    srsran_dci_location_t dci_pos     = {0, 0};
    /// Accumulation of all PDCCH masks for the current solution (DFS path)
    coreset_bitmap total_mask, current_mask;
  };
  using alloc_tree_dfs_t = std::vector<tree_node>;
  alloc_tree_dfs_t dfs_tree, saved_dfs_tree;

  srsran::span<const uint32_t> get_cce_loc_table(const alloc_record& record) const;
  bool                         alloc_dfs_node(const alloc_record& record, uint32_t dci_idx);
  bool                         get_next_dfs();
};

} // namespace sched_nr_impl

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_PDCCH_H
