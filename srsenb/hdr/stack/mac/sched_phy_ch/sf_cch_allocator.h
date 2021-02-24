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

#include "../sched_common.h"

#ifndef SRSLTE_PDCCH_SCHED_H
#define SRSLTE_PDCCH_SCHED_H

namespace srsenb {

class sched_ue;

/// Class responsible for managing a PDCCH CCE grid, namely CCE allocs, and avoid collisions.
class sf_cch_allocator
{
public:
  const static uint32_t MAX_CFI = 3;
  struct alloc_t {
    uint16_t              rnti    = 0;
    srslte_dci_location_t dci_pos = {0, 0};
    pdcch_mask_t          current_mask; ///< this PDCCH alloc mask
    pdcch_mask_t          total_mask;   ///< Accumulation of all PDCCH masks for the current solution (tree route)
  };
  using alloc_result_t = std::vector<const alloc_t*>;

  sf_cch_allocator() : logger(srslog::fetch_basic_logger("MAC")) {}

  void init(const sched_cell_params_t& cell_params_);
  void new_tti(tti_point tti_rx_);
  bool alloc_dci(alloc_type_t alloc_type, uint32_t aggr_idx, sched_ue* user = nullptr);
  bool set_cfi(uint32_t cfi);

  // getters
  uint32_t    get_cfi() const { return current_cfix + 1; }
  void        get_allocs(alloc_result_t* vec = nullptr, pdcch_mask_t* tot_mask = nullptr, size_t idx = 0) const;
  uint32_t    nof_cces() const { return cc_cfg->nof_cce_table[current_cfix]; }
  size_t      nof_allocs() const { return dci_record_list.size(); }
  size_t      nof_alloc_combinations() const { return get_alloc_tree().nof_leaves(); }
  std::string result_to_string(bool verbose = false) const;

private:
  struct alloc_tree_t {
    struct node_t {
      int     parent_idx;
      alloc_t node;
      node_t(int i, const alloc_t& a) : parent_idx(i), node(a) {}
    };
    // state
    size_t              nof_cces;
    std::vector<node_t> dci_alloc_tree;
    size_t              prev_start = 0, prev_end = 0;

    explicit alloc_tree_t(size_t nof_cces_) : nof_cces(nof_cces_) {}
    size_t nof_leaves() const { return prev_end - prev_start; }
    void   reset();
  };
  struct alloc_record_t {
    sched_ue*    user;
    uint32_t     aggr_idx;
    alloc_type_t alloc_type;
  };

  const alloc_tree_t&           get_alloc_tree() const { return alloc_trees[current_cfix]; }
  const cce_cfi_position_table* get_cce_loc_table(alloc_type_t alloc_type, sched_ue* user, uint32_t cfix) const;

  // PDCCH allocation algorithm
  bool        alloc_dci_record(const alloc_record_t& record, uint32_t cfix);
  static bool add_tree_node_leaves(alloc_tree_t&                 tree,
                                   int                           node_idx,
                                   const alloc_record_t&         dci_record,
                                   const cce_cfi_position_table& dci_locs,
                                   tti_point                     tti_tx_dl);

  // consts
  const sched_cell_params_t* cc_cfg = nullptr;
  srslog::basic_logger&      logger;

  // tti vars
  tti_point                   tti_rx;
  uint32_t                    current_cfix = 0;
  std::vector<alloc_tree_t>   alloc_trees;     ///< List of PDCCH alloc trees, where index is the cfi index
  std::vector<alloc_record_t> dci_record_list; ///< Keeps a record of all the PDCCH allocations done so far
};

} // namespace srsenb

#endif // SRSLTE_PDCCH_SCHED_H
