/*
 * Copyright 2013-2020 Software Radio Systems Limited
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
#include <deque>
#include <vector>

namespace srsenb {

//! Type of Allocation
enum class alloc_type_t { DL_BC, DL_PCCH, DL_RAR, DL_DATA, UL_DATA };

//! Result of alloc attempt
struct alloc_outcome_t {
  enum result_enum { SUCCESS, DCI_COLLISION, RB_COLLISION, ERROR, NOF_RB_INVALID };
  result_enum result = ERROR;
  alloc_outcome_t()  = default;
  alloc_outcome_t(result_enum e) : result(e) {}
              operator result_enum() { return result; }
              operator bool() { return result == SUCCESS; }
  const char* to_string() const;
};

//! Result of a Subframe sched computation
struct sf_sched_result {
  tti_params_t                    tti_params{10241};
  sched_interface::dl_sched_res_t dl_sched_result = {};
  sched_interface::ul_sched_res_t ul_sched_result = {};
  rbgmask_t                       dl_mask         = {}; ///< Accumulation of all DL RBG allocations
  prbmask_t                       ul_mask         = {}; ///< Accumulation of all UL PRB allocations
  pdcch_mask_t                    pdcch_mask      = {}; ///< Accumulation of all CCE allocations
};

//! Class responsible for managing a PDCCH CCE grid, namely cce allocs, and avoid collisions.
class pdcch_grid_t
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

  void init(const sched_cell_params_t& cell_params_);
  void new_tti(const tti_params_t& tti_params_);
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

  const alloc_tree_t&    get_alloc_tree() const { return alloc_trees[current_cfix]; }
  const sched_dci_cce_t* get_cce_loc_table(alloc_type_t alloc_type, sched_ue* user, uint32_t cfix) const;

  // PDCCH allocation algorithm
  bool        alloc_dci_record(const alloc_record_t& record, uint32_t cfix);
  static bool add_tree_node_leaves(alloc_tree_t&          tree,
                                   int                    node_idx,
                                   const alloc_record_t&  dci_record,
                                   const sched_dci_cce_t& dci_locs,
                                   uint32_t               tti_tx_dl);

  // consts
  const sched_cell_params_t* cc_cfg = nullptr;
  srslte::log_ref            log_h;

  // tti vars
  const tti_params_t*         tti_params   = nullptr;
  uint32_t                    current_cfix = 0;
  std::vector<alloc_tree_t>   alloc_trees;     ///< List of PDCCH alloc trees, where index is the cfi index
  std::vector<alloc_record_t> dci_record_list; ///< Keeps a record of all the PDCCH allocations done so far
};

//! manages a subframe grid resources, namely CCE and DL/UL RB allocations
class sf_grid_t
{
public:
  struct dl_ctrl_alloc_t {
    alloc_outcome_t outcome;
    rbg_range_t     rbg_range;
  };

  void            init(const sched_cell_params_t& cell_params_);
  void            new_tti(const tti_params_t& tti_params_);
  dl_ctrl_alloc_t alloc_dl_ctrl(uint32_t aggr_lvl, alloc_type_t alloc_type);
  alloc_outcome_t alloc_dl_data(sched_ue* user, const rbgmask_t& user_mask);
  bool            reserve_dl_rbgs(uint32_t start_rbg, uint32_t end_rbg);
  alloc_outcome_t alloc_ul_data(sched_ue* user, ul_harq_proc::ul_alloc_t alloc, bool needs_pdcch);
  bool            reserve_ul_prbs(const prbmask_t& prbmask, bool strict);

  // getters
  const rbgmask_t&    get_dl_mask() const { return dl_mask; }
  const prbmask_t&    get_ul_mask() const { return ul_mask; }
  uint32_t            get_cfi() const { return pdcch_alloc.get_cfi(); }
  const pdcch_grid_t& get_pdcch_grid() const { return pdcch_alloc; }

private:
  alloc_outcome_t alloc_dl(uint32_t aggr_lvl, alloc_type_t alloc_type, rbgmask_t alloc_mask, sched_ue* user = nullptr);

  // consts
  const sched_cell_params_t* cc_cfg = nullptr;
  srslte::log_ref            log_h;
  uint32_t                   nof_rbgs = 0;
  uint32_t                   si_n_rbg = 0, rar_n_rbg = 0;

  // tti const
  const tti_params_t* tti_params = nullptr;
  // derived
  pdcch_grid_t pdcch_alloc = {};

  // internal state
  uint32_t  avail_rbg = 0;
  rbgmask_t dl_mask   = {};
  prbmask_t ul_mask   = {};
};

//! generic interface used by DL scheduler algorithm
class dl_sf_sched_itf
{
public:
  virtual alloc_outcome_t  alloc_dl_user(sched_ue* user, const rbgmask_t& user_mask, uint32_t pid) = 0;
  virtual const rbgmask_t& get_dl_mask() const                                                     = 0;
  virtual uint32_t         get_tti_tx_dl() const                                                   = 0;
  virtual uint32_t         get_nof_ctrl_symbols() const                                            = 0;
  virtual bool             is_dl_alloc(sched_ue* user) const                                       = 0;
};

//! generic interface used by UL scheduler algorithm
class ul_sf_sched_itf
{
public:
  virtual alloc_outcome_t  alloc_ul_user(sched_ue* user, ul_harq_proc::ul_alloc_t alloc) = 0;
  virtual const prbmask_t& get_ul_mask() const                                           = 0;
  virtual uint32_t         get_tti_tx_ul() const                                         = 0;
  virtual bool             is_ul_alloc(sched_ue* user) const                             = 0;
};

/** Description: Stores the RAR, broadcast, paging, DL data, UL data allocations for the given subframe
 *               Converts the stored allocations' metadata to the scheduler DL/UL result
 *               Handles the generation of DCI formats
 */
class sf_sched : public dl_sf_sched_itf, public ul_sf_sched_itf
{
public:
  struct ctrl_alloc_t {
    size_t       dci_idx;
    rbg_range_t  rbg_range;
    uint16_t     rnti;
    uint32_t     req_bytes;
    alloc_type_t alloc_type;
  };
  struct rar_alloc_t {
    sf_sched::ctrl_alloc_t          alloc_data;
    sched_interface::dl_sched_rar_t rar_grant;
    rar_alloc_t(const sf_sched::ctrl_alloc_t& c, const sched_interface::dl_sched_rar_t& r) : alloc_data(c), rar_grant(r)
    {
    }
  };
  struct bc_alloc_t : public ctrl_alloc_t {
    uint32_t rv      = 0;
    uint32_t sib_idx = 0;
    bc_alloc_t()     = default;
    explicit bc_alloc_t(const ctrl_alloc_t& c) : ctrl_alloc_t(c) {}
  };
  struct dl_alloc_t {
    size_t    dci_idx;
    sched_ue* user_ptr;
    rbgmask_t user_mask;
    uint32_t  pid;
  };
  struct ul_alloc_t {
    enum type_t { NEWTX, NOADAPT_RETX, ADAPT_RETX, MSG3 };
    size_t                   dci_idx;
    type_t                   type;
    sched_ue*                user_ptr;
    ul_harq_proc::ul_alloc_t alloc;
    uint32_t                 mcs = 0;
    bool                     is_retx() const { return type == NOADAPT_RETX or type == ADAPT_RETX; }
    bool                     is_msg3() const { return type == MSG3; }
    bool                     needs_pdcch() const { return type == NEWTX or type == ADAPT_RETX; }
  };
  struct pending_msg3_t {
    uint16_t rnti  = 0;
    uint32_t L     = 0;
    uint32_t n_prb = 0;
    uint32_t mcs   = 0;
  };
  struct pending_rar_t {
    uint16_t                             ra_rnti                                   = 0;
    uint32_t                             prach_tti                                 = 0;
    uint32_t                             nof_grants                                = 0;
    sched_interface::dl_sched_rar_info_t msg3_grant[sched_interface::MAX_RAR_LIST] = {};
  };
  typedef std::pair<alloc_outcome_t, const ctrl_alloc_t> ctrl_code_t;

  // Control/Configuration Methods
  sf_sched();
  void init(const sched_cell_params_t& cell_params_);
  void new_tti(uint32_t tti_rx_);

  // DL alloc methods
  alloc_outcome_t                      alloc_bc(uint32_t aggr_lvl, uint32_t sib_idx, uint32_t sib_ntx);
  alloc_outcome_t                      alloc_paging(uint32_t aggr_lvl, uint32_t paging_payload);
  std::pair<alloc_outcome_t, uint32_t> alloc_rar(uint32_t aggr_lvl, const pending_rar_t& rar_grant);
  bool reserve_dl_rbgs(uint32_t rbg_start, uint32_t rbg_end) { return tti_alloc.reserve_dl_rbgs(rbg_start, rbg_end); }
  const std::vector<rar_alloc_t>& get_allocated_rars() const { return rar_allocs; }

  // UL alloc methods
  alloc_outcome_t alloc_msg3(sched_ue* user, const sched_interface::dl_sched_rar_grant_t& rargrant);
  alloc_outcome_t
       alloc_ul(sched_ue* user, ul_harq_proc::ul_alloc_t alloc, sf_sched::ul_alloc_t::type_t alloc_type, uint32_t mcs = 0);
  bool reserve_ul_prbs(const prbmask_t& ulmask, bool strict) { return tti_alloc.reserve_ul_prbs(ulmask, strict); }
  bool alloc_phich(sched_ue* user, sched_interface::ul_sched_res_t* ul_sf_result);

  // compute DCIs and generate dl_sched_result/ul_sched_result for a given TTI
  void generate_sched_results(sf_sched_result* sf_result);

  // dl_tti_sched itf
  alloc_outcome_t  alloc_dl_user(sched_ue* user, const rbgmask_t& user_mask, uint32_t pid) final;
  uint32_t         get_tti_tx_dl() const final { return tti_params.tti_tx_dl; }
  uint32_t         get_nof_ctrl_symbols() const final;
  const rbgmask_t& get_dl_mask() const final { return tti_alloc.get_dl_mask(); }
  // ul_tti_sched itf
  alloc_outcome_t  alloc_ul_user(sched_ue* user, ul_harq_proc::ul_alloc_t alloc) final;
  const prbmask_t& get_ul_mask() const final { return tti_alloc.get_ul_mask(); }
  uint32_t         get_tti_tx_ul() const final { return tti_params.tti_tx_ul; }

  // getters
  uint32_t            get_tti_rx() const { return tti_params.tti_rx; }
  const tti_params_t& get_tti_params() const { return tti_params; }

private:
  bool        is_dl_alloc(sched_ue* user) const final;
  bool        is_ul_alloc(sched_ue* user) const final;
  ctrl_code_t alloc_dl_ctrl(uint32_t aggr_lvl, uint32_t tbs_bytes, uint16_t rnti);
  int         generate_format1a(uint32_t         rb_start,
                                uint32_t         l_crb,
                                uint32_t         tbs,
                                uint32_t         rv,
                                uint16_t         rnti,
                                srslte_dci_dl_t* dci);
  void set_bc_sched_result(const pdcch_grid_t::alloc_result_t& dci_result, sched_interface::dl_sched_res_t* dl_result);
  void set_rar_sched_result(const pdcch_grid_t::alloc_result_t& dci_result, sched_interface::dl_sched_res_t* dl_result);
  void set_dl_data_sched_result(const pdcch_grid_t::alloc_result_t& dci_result,
                                sched_interface::dl_sched_res_t*    dl_result);
  void set_ul_sched_result(const pdcch_grid_t::alloc_result_t& dci_result, sched_interface::ul_sched_res_t* ul_result);

  // consts
  const sched_cell_params_t* cc_cfg = nullptr;
  srslte::log_ref            log_h;

  // internal state
  sf_grid_t                tti_alloc;
  std::vector<bc_alloc_t>  bc_allocs;
  std::vector<rar_alloc_t> rar_allocs;
  std::vector<dl_alloc_t>  data_allocs;
  std::vector<ul_alloc_t>  ul_data_allocs;
  uint32_t                 last_msg3_prb = 0, max_msg3_prb = 0;

  // Next TTI state
  tti_params_t tti_params{10241};
};

} // namespace srsenb

#endif // SRSLTE_SCHEDULER_GRID_H
