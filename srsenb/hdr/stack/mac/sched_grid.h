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

#ifndef SRSRAN_SCHED_GRID_H
#define SRSRAN_SCHED_GRID_H

#include "lib/include/srsran/interfaces/sched_interface.h"
#include "sched_phy_ch/sched_result.h"
#include "sched_phy_ch/sf_cch_allocator.h"
#include "sched_ue.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/circular_array.h"
#include "srsran/srslog/srslog.h"
#include <vector>

namespace srsenb {

/// Error code of alloc attempt
enum class alloc_result {
  success,
  sch_collision,
  no_cch_space,
  no_sch_space,
  no_rnti_opportunity,
  invalid_grant_params,
  invalid_coderate,
  no_grant_space,
  other_cause
};
const char* to_string(alloc_result res);

struct sf_sched_result {
  tti_point                    tti_rx;
  std::vector<cc_sched_result> enb_cc_list;

  void new_tti(tti_point tti_rx);
  bool is_generated(uint32_t enb_cc_idx) const
  {
    return enb_cc_list.size() > enb_cc_idx and enb_cc_list[enb_cc_idx].generated;
  }
  const cc_sched_result* get_cc(uint32_t enb_cc_idx) const
  {
    assert(enb_cc_idx < enb_cc_list.size());
    return &enb_cc_list[enb_cc_idx];
  }
  cc_sched_result* get_cc(uint32_t enb_cc_idx)
  {
    assert(enb_cc_idx < enb_cc_list.size());
    return &enb_cc_list[enb_cc_idx];
  }
  bool is_ul_alloc(uint16_t rnti) const;
  bool is_dl_alloc(uint16_t rnti) const;
};

struct sched_result_ringbuffer {
public:
  void             set_nof_carriers(uint32_t nof_carriers);
  void             new_tti(srsran::tti_point tti_rx);
  bool             has_sf(srsran::tti_point tti_rx) const { return results[tti_rx.to_uint()].tti_rx == tti_rx; }
  sf_sched_result* get_sf(srsran::tti_point tti_rx)
  {
    assert(has_sf(tti_rx));
    return &results[tti_rx.to_uint()];
  }
  const sf_sched_result* get_sf(srsran::tti_point tti_rx) const
  {
    assert(has_sf(tti_rx));
    return &results[tti_rx.to_uint()];
  }
  const cc_sched_result* get_cc(srsran::tti_point tti_rx, uint32_t enb_cc_idx) const
  {
    return get_sf(tti_rx)->get_cc(enb_cc_idx);
  }
  cc_sched_result* get_cc(srsran::tti_point tti_rx, uint32_t enb_cc_idx) { return get_sf(tti_rx)->get_cc(enb_cc_idx); }

private:
  uint32_t                                           nof_carriers = 1;
  srsran::circular_array<sf_sched_result, TTIMOD_SZ> results;
};

/// manages a subframe grid resources, namely CCE and DL/UL RB allocations
class sf_grid_t
{
public:
  sf_grid_t() : logger(srslog::fetch_basic_logger("MAC")) {}

  void         init(const sched_cell_params_t& cell_params_);
  void         new_tti(tti_point tti_rx);
  alloc_result alloc_dl_ctrl(uint32_t aggr_lvl, rbg_interval rbg_range, alloc_type_t alloc_type);
  alloc_result alloc_dl_data(sched_ue* user, const rbgmask_t& user_mask, bool has_pusch_grant);
  bool         reserve_dl_rbgs(uint32_t start_rbg, uint32_t end_rbg);
  void         rem_last_alloc_dl(rbg_interval rbgs);

  alloc_result alloc_ul_data(sched_ue* user, prb_interval alloc, bool needs_pdcch, bool strict = true);
  alloc_result reserve_ul_prbs(const prbmask_t& prbmask, bool strict);
  alloc_result reserve_ul_prbs(prb_interval alloc, bool strict);
  bool         find_ul_alloc(uint32_t L, prb_interval* alloc) const;

  // getters
  const rbgmask_t&        get_dl_mask() const { return dl_mask; }
  const prbmask_t&        get_ul_mask() const { return ul_mask; }
  uint32_t                get_cfi() const { return pdcch_alloc.get_cfi(); }
  const sf_cch_allocator& get_pdcch_grid() const { return pdcch_alloc; }
  uint32_t                get_pucch_width() const { return pucch_nrb; }

private:
  alloc_result alloc_dl(uint32_t     aggr_lvl,
                        alloc_type_t alloc_type,
                        rbgmask_t    alloc_mask,
                        sched_ue*    user            = nullptr,
                        bool         has_pusch_grant = false);

  // consts
  const sched_cell_params_t* cc_cfg = nullptr;
  srslog::basic_logger&      logger;
  uint32_t                   nof_rbgs  = 0;
  uint32_t                   pucch_nrb = 0;
  prbmask_t                  pucch_mask;

  // derived
  sf_cch_allocator pdcch_alloc = {};

  // internal state
  tti_point tti_rx;
  rbgmask_t dl_mask = {};
  prbmask_t ul_mask = {};
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
    uint32_t     req_bytes;
  };
  struct rar_alloc_t {
    sf_sched::ctrl_alloc_t          alloc_data;
    sched_interface::dl_sched_rar_t rar_grant;
  };
  struct bc_alloc_t : public ctrl_alloc_t {
    sched_interface::dl_sched_bc_t bc_grant;
  };
  struct dl_alloc_t {
    size_t    dci_idx;
    uint16_t  rnti;
    rbgmask_t user_mask;
    uint32_t  pid;
  };
  struct ul_alloc_t {
    enum type_t { NEWTX, NOADAPT_RETX, ADAPT_RETX };
    bool         is_msg3 = false;
    size_t       dci_idx;
    type_t       type;
    uint16_t     rnti;
    prb_interval alloc;
    int          msg3_mcs = -1;
    bool         is_retx() const { return type == NOADAPT_RETX or type == ADAPT_RETX; }
    bool         needs_pdcch() const { return (type == NEWTX and not is_msg3) or type == ADAPT_RETX; }
  };
  struct pending_msg3_t {
    uint16_t rnti  = 0;
    uint32_t L     = 0;
    uint32_t n_prb = 0;
    uint32_t mcs   = 0;
  };

  // Control/Configuration Methods
  sf_sched();
  void init(const sched_cell_params_t& cell_params_);
  void new_tti(srsran::tti_point tti_rx_, sf_sched_result* cc_results);

  // DL alloc methods
  alloc_result alloc_sib(uint32_t aggr_lvl, uint32_t sib_idx, uint32_t sib_ntx, rbg_interval rbgs);
  alloc_result alloc_paging(uint32_t aggr_lvl, uint32_t paging_payload, rbg_interval rbgs);
  alloc_result alloc_rar(uint32_t aggr_lvl, const pending_rar_t& rar_grant, rbg_interval rbgs, uint32_t nof_grants);
  bool reserve_dl_rbgs(uint32_t rbg_start, uint32_t rbg_end) { return tti_alloc.reserve_dl_rbgs(rbg_start, rbg_end); }

  // UL alloc methods
  alloc_result alloc_msg3(sched_ue* user, const sched_interface::dl_sched_rar_grant_t& rargrant);
  alloc_result
               alloc_ul(sched_ue* user, prb_interval alloc, ul_alloc_t::type_t alloc_type, bool is_msg3 = false, int msg3_mcs = -1);
  alloc_result reserve_ul_prbs(const prbmask_t& ulmask, bool strict)
  {
    return tti_alloc.reserve_ul_prbs(ulmask, strict);
  }
  alloc_result alloc_phich(sched_ue* user);

  // compute DCIs and generate dl_sched_result/ul_sched_result for a given TTI
  void generate_sched_results(sched_ue_list& ue_db);

  alloc_result                    alloc_dl_user(sched_ue* user, const rbgmask_t& user_mask, uint32_t pid);
  tti_point                       get_tti_tx_dl() const { return to_tx_dl(tti_rx); }
  uint32_t                        get_nof_ctrl_symbols() const;
  const rbgmask_t&                get_dl_mask() const { return tti_alloc.get_dl_mask(); }
  alloc_result                    alloc_ul_user(sched_ue* user, prb_interval alloc);
  const prbmask_t&                get_ul_mask() const { return tti_alloc.get_ul_mask(); }
  tti_point                       get_tti_tx_ul() const { return to_tx_ul(tti_rx); }
  srsran::const_span<rar_alloc_t> get_allocated_rars() const { return rar_allocs; }

  // getters
  tti_point                  get_tti_rx() const { return tti_rx; }
  bool                       is_dl_alloc(uint16_t rnti) const;
  bool                       is_ul_alloc(uint16_t rnti) const;
  uint32_t                   get_enb_cc_idx() const { return cc_cfg->enb_cc_idx; }
  const sched_cell_params_t* get_cc_cfg() const { return cc_cfg; }

private:
  void set_dl_data_sched_result(const sf_cch_allocator::alloc_result_t& dci_result,
                                sched_interface::dl_sched_res_t*        dl_result,
                                sched_ue_list&                          ue_list);
  void set_ul_sched_result(const sf_cch_allocator::alloc_result_t& dci_result,
                           sched_interface::ul_sched_res_t*        ul_result,
                           sched_ue_list&                          ue_list);

  // consts
  const sched_cell_params_t* cc_cfg = nullptr;
  srslog::basic_logger&      logger;
  sf_sched_result*           cc_results; ///< Results of other CCs for the same Subframe

  // internal state
  sf_grid_t tti_alloc;

  srsran::bounded_vector<bc_alloc_t, sched_interface::MAX_BC_LIST>   bc_allocs;
  srsran::bounded_vector<rar_alloc_t, sched_interface::MAX_RAR_LIST> rar_allocs;
  srsran::bounded_vector<dl_alloc_t, sched_interface::MAX_DATA_LIST> data_allocs;
  srsran::bounded_vector<ul_alloc_t, sched_interface::MAX_DATA_LIST> ul_data_allocs;
  uint32_t                                                           last_msg3_prb = 0, max_msg3_prb = 0;

  // Next TTI state
  tti_point tti_rx;
};

} // namespace srsenb

#endif // SRSRAN_SCHED_GRID_H
