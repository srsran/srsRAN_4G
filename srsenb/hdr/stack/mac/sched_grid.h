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

#ifndef SRSLTE_SCHED_GRID_H
#define SRSLTE_SCHED_GRID_H

#include "lib/include/srslte/interfaces/sched_interface.h"
#include "sched_phy_ch/sf_cch_allocator.h"
#include "sched_ue.h"
#include "srslte/adt/bounded_bitset.h"
#include "srslte/common/log.h"
#include "srslte/srslog/srslog.h"
#include <deque>
#include <vector>

namespace srsenb {

/// Error code of alloc attempt
struct alloc_outcome_t {
  enum result_enum {
    SUCCESS,
    DCI_COLLISION,
    RB_COLLISION,
    ERROR,
    NOF_RB_INVALID,
    PUCCH_COLLISION,
    MEASGAP_COLLISION,
    ALREADY_ALLOC,
    NO_DATA,
    INVALID_PRBMASK,
    INVALID_CARRIER
  };
  result_enum result = ERROR;
  alloc_outcome_t()  = default;
  alloc_outcome_t(result_enum e) : result(e) {}
              operator result_enum() { return result; }
              operator bool() { return result == SUCCESS; }
  const char* to_string() const;
};

//! Result of a Subframe sched computation
struct cc_sched_result {
  tti_point                       tti_rx;
  sched_interface::dl_sched_res_t dl_sched_result = {};
  sched_interface::ul_sched_res_t ul_sched_result = {};
  rbgmask_t                       dl_mask         = {}; ///< Accumulation of all DL RBG allocations
  prbmask_t                       ul_mask         = {}; ///< Accumulation of all UL PRB allocations
  pdcch_mask_t                    pdcch_mask      = {}; ///< Accumulation of all CCE allocations

  bool is_generated(tti_point tti_rx_) const { return tti_rx == tti_rx_; }
};

struct sf_sched_result {
  srslte::tti_point            tti_rx;
  std::vector<cc_sched_result> enb_cc_list;

  cc_sched_result*       new_cc(uint32_t enb_cc_idx);
  const cc_sched_result* get_cc(uint32_t enb_cc_idx) const
  {
    return enb_cc_idx < enb_cc_list.size() ? &enb_cc_list[enb_cc_idx] : nullptr;
  }
  cc_sched_result* get_cc(uint32_t enb_cc_idx)
  {
    return enb_cc_idx < enb_cc_list.size() ? &enb_cc_list[enb_cc_idx] : nullptr;
  }
  bool is_ul_alloc(uint16_t rnti) const;
  bool is_dl_alloc(uint16_t rnti) const;
};

struct sched_result_list {
public:
  sf_sched_result*       new_tti(srslte::tti_point tti_rx);
  sf_sched_result*       get_sf(srslte::tti_point tti_rx);
  const sf_sched_result* get_sf(srslte::tti_point tti_rx) const;
  const cc_sched_result* get_cc(srslte::tti_point tti_rx, uint32_t enb_cc_idx) const;
  cc_sched_result*       get_cc(srslte::tti_point tti_rx, uint32_t enb_cc_idx);

private:
  std::array<sf_sched_result, TTIMOD_SZ> results;
};

/// manages a subframe grid resources, namely CCE and DL/UL RB allocations
class sf_grid_t
{
public:
  struct dl_ctrl_alloc_t {
    alloc_outcome_t outcome;
    rbg_interval    rbg_range;
  };

  sf_grid_t() : logger(srslog::fetch_basic_logger("MAC")) {}

  void            init(const sched_cell_params_t& cell_params_);
  void            new_tti(tti_point tti_rx);
  dl_ctrl_alloc_t alloc_dl_ctrl(uint32_t aggr_lvl, alloc_type_t alloc_type);
  alloc_outcome_t alloc_dl_data(sched_ue* user, const rbgmask_t& user_mask, bool has_pusch_grant);
  bool            reserve_dl_rbgs(uint32_t start_rbg, uint32_t end_rbg);
  alloc_outcome_t alloc_ul_data(sched_ue* user, prb_interval alloc, bool needs_pdcch);
  alloc_outcome_t reserve_ul_prbs(const prbmask_t& prbmask, bool strict);
  alloc_outcome_t reserve_ul_prbs(prb_interval alloc, bool strict);
  bool            find_ul_alloc(uint32_t L, prb_interval* alloc) const;

  // getters
  const rbgmask_t&        get_dl_mask() const { return dl_mask; }
  const prbmask_t&        get_ul_mask() const { return ul_mask; }
  uint32_t                get_cfi() const { return pdcch_alloc.get_cfi(); }
  const sf_cch_allocator& get_pdcch_grid() const { return pdcch_alloc; }
  uint32_t                get_pucch_width() const { return pucch_nrb; }

private:
  alloc_outcome_t alloc_dl(uint32_t     aggr_lvl,
                           alloc_type_t alloc_type,
                           rbgmask_t    alloc_mask,
                           sched_ue*    user            = nullptr,
                           bool         has_pusch_grant = false);

  // consts
  const sched_cell_params_t* cc_cfg = nullptr;
  srslog::basic_logger&      logger;
  uint32_t                   nof_rbgs = 0;
  uint32_t                   si_n_rbg = 0, rar_n_rbg = 0;
  uint32_t                   pucch_nrb = 0;
  prbmask_t                  pucch_mask;

  // derived
  sf_cch_allocator pdcch_alloc = {};

  // internal state
  tti_point tti_rx;
  uint32_t  avail_rbg = 0;
  rbgmask_t dl_mask   = {};
  prbmask_t ul_mask   = {};
};

/** Description: Stores the RAR, broadcast, paging, DL data, UL data allocations for the given subframe
 *               Converts the stored allocations' metadata to the scheduler DL/UL result
 *               Handles the generation of DCI formats
 */
class sf_sched
{
public:
  struct ctrl_alloc_t {
    size_t       dci_idx;
    rbg_interval rbg_range;
    uint16_t     rnti;
    uint32_t     req_bytes;
    alloc_type_t alloc_type;
  };
  struct rar_alloc_t {
    sf_sched::ctrl_alloc_t          alloc_data;
    sched_interface::dl_sched_rar_t rar_grant;
    rar_alloc_t(const sf_sched::ctrl_alloc_t& c, const sched_interface::dl_sched_rar_t& r) : alloc_data(c), rar_grant(r)
    {}
  };
  struct bc_alloc_t : public ctrl_alloc_t {
    uint32_t rv      = 0;
    uint32_t sib_idx = 0;
    bc_alloc_t()     = default;
    explicit bc_alloc_t(const ctrl_alloc_t& c) : ctrl_alloc_t(c) {}
  };
  struct dl_alloc_t {
    size_t    dci_idx;
    uint16_t  rnti;
    rbgmask_t user_mask;
    uint32_t  pid;
  };
  struct ul_alloc_t {
    enum type_t { NEWTX, NOADAPT_RETX, ADAPT_RETX, MSG3, MSG3_RETX };
    size_t       dci_idx;
    type_t       type;
    uint16_t     rnti;
    prb_interval alloc;
    int          msg3_mcs = -1;
    bool         is_retx() const { return type == NOADAPT_RETX or type == ADAPT_RETX; }
    bool         is_msg3() const { return type == MSG3; }
    bool         needs_pdcch() const { return type == NEWTX or type == ADAPT_RETX; }
  };
  struct pending_msg3_t {
    uint16_t rnti  = 0;
    uint32_t L     = 0;
    uint32_t n_prb = 0;
    uint32_t mcs   = 0;
  };
  struct pending_rar_t {
    uint16_t                             ra_rnti = 0;
    tti_point                            prach_tti{};
    uint32_t                             nof_grants                                = 0;
    sched_interface::dl_sched_rar_info_t msg3_grant[sched_interface::MAX_RAR_LIST] = {};
  };
  typedef std::pair<alloc_outcome_t, const ctrl_alloc_t> ctrl_code_t;

  // Control/Configuration Methods
  sf_sched();
  void init(const sched_cell_params_t& cell_params_);
  void new_tti(srslte::tti_point tti_rx_, sf_sched_result* cc_results);

  // DL alloc methods
  alloc_outcome_t                      alloc_bc(uint32_t aggr_lvl, uint32_t sib_idx, uint32_t sib_ntx);
  alloc_outcome_t                      alloc_paging(uint32_t aggr_lvl, uint32_t paging_payload);
  std::pair<alloc_outcome_t, uint32_t> alloc_rar(uint32_t aggr_lvl, const pending_rar_t& rar_grant);
  bool reserve_dl_rbgs(uint32_t rbg_start, uint32_t rbg_end) { return tti_alloc.reserve_dl_rbgs(rbg_start, rbg_end); }
  const std::vector<rar_alloc_t>& get_allocated_rars() const { return rar_allocs; }

  // UL alloc methods
  alloc_outcome_t alloc_msg3(sched_ue* user, const sched_interface::dl_sched_rar_grant_t& rargrant);
  alloc_outcome_t alloc_ul(sched_ue* user, prb_interval alloc, ul_alloc_t::type_t alloc_type, int msg3_mcs = -1);
  bool reserve_ul_prbs(const prbmask_t& ulmask, bool strict) { return tti_alloc.reserve_ul_prbs(ulmask, strict); }
  bool alloc_phich(sched_ue* user, sched_interface::ul_sched_res_t* ul_sf_result);

  // compute DCIs and generate dl_sched_result/ul_sched_result for a given TTI
  void generate_sched_results(sched_ue_list& ue_db);

  alloc_outcome_t  alloc_dl_user(sched_ue* user, const rbgmask_t& user_mask, uint32_t pid);
  tti_point        get_tti_tx_dl() const { return to_tx_dl(tti_rx); }
  uint32_t         get_nof_ctrl_symbols() const;
  const rbgmask_t& get_dl_mask() const { return tti_alloc.get_dl_mask(); }
  alloc_outcome_t  alloc_ul_user(sched_ue* user, prb_interval alloc);
  const prbmask_t& get_ul_mask() const { return tti_alloc.get_ul_mask(); }
  tti_point        get_tti_tx_ul() const { return to_tx_ul(tti_rx); }

  // getters
  tti_point get_tti_rx() const { return tti_rx; }
  bool      is_dl_alloc(uint16_t rnti) const;
  bool      is_ul_alloc(uint16_t rnti) const;
  uint32_t  get_enb_cc_idx() const { return cc_cfg->enb_cc_idx; }

private:
  ctrl_code_t alloc_dl_ctrl(uint32_t aggr_lvl, uint32_t tbs_bytes, uint16_t rnti);
  int         generate_format1a(prb_interval prb_range, uint32_t tbs, uint32_t rv, uint16_t rnti, srslte_dci_dl_t* dci);
  void        set_bc_sched_result(const sf_cch_allocator::alloc_result_t& dci_result,
                                  sched_interface::dl_sched_res_t*        dl_result);
  void        set_rar_sched_result(const sf_cch_allocator::alloc_result_t& dci_result,
                                   sched_interface::dl_sched_res_t*        dl_result);
  void        set_dl_data_sched_result(const sf_cch_allocator::alloc_result_t& dci_result,
                                       sched_interface::dl_sched_res_t*        dl_result,
                                       sched_ue_list&                          ue_list);
  void        set_ul_sched_result(const sf_cch_allocator::alloc_result_t& dci_result,
                                  sched_interface::ul_sched_res_t*        ul_result,
                                  sched_ue_list&                          ue_list);

  // consts
  const sched_cell_params_t* cc_cfg = nullptr;
  srslog::basic_logger&      logger;
  sf_sched_result*           cc_results; ///< Results of other CCs for the same Subframe

  // internal state
  sf_grid_t                tti_alloc;
  std::vector<bc_alloc_t>  bc_allocs;
  std::vector<rar_alloc_t> rar_allocs;
  std::vector<dl_alloc_t>  data_allocs;
  std::vector<ul_alloc_t>  ul_data_allocs;
  uint32_t                 last_msg3_prb = 0, max_msg3_prb = 0;

  // Next TTI state
  tti_point tti_rx;
};

} // namespace srsenb

#endif // SRSLTE_SCHED_GRID_H
