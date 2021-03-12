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
    int8_t                pucch_n_prb; ///< this PUCCH resource identifier
    uint16_t              rnti    = 0;
    srslte_dci_location_t dci_pos = {0, 0};
    pdcch_mask_t          current_mask;     ///< this allocation PDCCH mask
    pdcch_mask_t          total_mask;       ///< Accumulation of all PDCCH masks for the current solution (tree route)
    prbmask_t             total_pucch_mask; ///< Accumulation of all PUCCH masks for the current solution/tree route
  };
  using alloc_result_t = srslte::bounded_vector<const alloc_t*, 16>;

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

  // getters
  uint32_t    get_cfi() const { return current_cfix + 1; }
  void        get_allocs(alloc_result_t* vec = nullptr, pdcch_mask_t* tot_mask = nullptr, size_t idx = 0) const;
  uint32_t    nof_cces() const { return cc_cfg->nof_cce_table[current_cfix]; }
  size_t      nof_allocs() const { return dci_record_list.size(); }
  size_t      nof_alloc_combinations() const { return get_alloc_tree().nof_leaves(); }
  std::string result_to_string(bool verbose = false) const;

private:
  /// DCI allocation parameters
  struct alloc_record_t {
    sched_ue*    user;
    uint32_t     aggr_idx;
    alloc_type_t alloc_type;
    bool         pusch_uci;
  };
  /// Tree-based data structure to store possible DCI allocation decisions
  struct alloc_tree_t {
    struct node_t {
      int     parent_idx;
      alloc_t node;
      node_t(int i, const alloc_t& a) : parent_idx(i), node(a) {}
    };

    // args
    size_t                     nof_cces;
    const sched_cell_params_t* cc_cfg         = nullptr;
    srslte_pucch_cfg_t*        pucch_cfg_temp = nullptr;
    uint32_t                   cfi;
    // state
    std::vector<node_t> dci_alloc_tree;
    size_t              prev_start = 0, prev_end = 0;

    explicit alloc_tree_t(uint32_t this_cfi, const sched_cell_params_t& cc_params, srslte_pucch_cfg_t& pucch_cfg);
    size_t      nof_leaves() const { return prev_end - prev_start; }
    void        reset();
    void        get_allocs(alloc_result_t* vec, pdcch_mask_t* tot_mask, size_t idx) const;
    bool        add_tree_node_leaves(int                           node_idx,
                                     const alloc_record_t&         dci_record,
                                     const cce_cfi_position_table& dci_locs,
                                     tti_point                     tti_rx);
    std::string result_to_string(bool verbose) const;
  };

  const alloc_tree_t&           get_alloc_tree() const { return alloc_trees[current_cfix]; }
  const cce_cfi_position_table* get_cce_loc_table(alloc_type_t alloc_type, sched_ue* user, uint32_t cfix) const;

  // PDCCH allocation algorithm
  bool set_cfi(uint32_t cfi);
  bool alloc_dci_record(const alloc_record_t& record, uint32_t cfix);

  // consts
  const sched_cell_params_t* cc_cfg = nullptr;
  srslog::basic_logger&      logger;
  srslte_pucch_cfg_t         pucch_cfg_common = {};

  // tti vars
  tti_point                   tti_rx;
  uint32_t                    current_cfix = 0;
  std::vector<alloc_tree_t>   alloc_trees;     ///< List of PDCCH alloc trees, where index is the cfi index
  std::vector<alloc_record_t> dci_record_list; ///< Keeps a record of all the PDCCH allocations done so far
};

// Helper methods
bool is_pucch_sr_collision(const srslte_pucch_cfg_t& ue_pucch_cfg, tti_point tti_tx_dl_ack, uint32_t n1_pucch);

} // namespace srsenb

#endif // SRSLTE_PDCCH_SCHED_H
