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

#ifndef SRSLTE_SCHEDULER_CARRIER_H
#define SRSLTE_SCHEDULER_CARRIER_H

#include "scheduler.h"

namespace srsenb {

class bc_sched;
class ra_sched;

class sched::carrier_sched
{
public:
  explicit carrier_sched(sched* sched_);

  class tti_sched_result_t : public dl_tti_sched_t, public ul_tti_sched_t
  {
  public:
    struct ctrl_alloc_t {
      size_t       dci_idx;
      rbg_range_t  rbg_range;
      uint16_t     rnti;
      uint32_t     req_bytes;
      alloc_type_t alloc_type;
    };
    struct rar_alloc_t : public ctrl_alloc_t {
      dl_sched_rar_t rar_grant;
      rar_alloc_t() = default;
      explicit rar_alloc_t(const ctrl_alloc_t& c) : ctrl_alloc_t(c) {}
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
    typedef std::pair<alloc_outcome_t, const rar_alloc_t*> rar_code_t;
    typedef std::pair<alloc_outcome_t, const ctrl_alloc_t> ctrl_code_t;

    // TTI scheduler result
    pdcch_mask_t                    pdcch_mask;
    sched_interface::dl_sched_res_t dl_sched_result;
    sched_interface::ul_sched_res_t ul_sched_result;

    void            init(carrier_sched* carrier_);
    void            new_tti(uint32_t tti_rx_, uint32_t start_cfi);
    alloc_outcome_t alloc_bc(uint32_t aggr_lvl, uint32_t sib_idx, uint32_t sib_ntx);
    alloc_outcome_t alloc_paging(uint32_t aggr_lvl, uint32_t paging_payload);
    rar_code_t      alloc_rar(uint32_t aggr_lvl, const dl_sched_rar_t& rar_grant, uint32_t rar_tti, uint32_t buf_rar);
    void            generate_dcis();
    // dl_tti_sched itf
    alloc_outcome_t  alloc_dl_user(sched_ue* user, const rbgmask_t& user_mask, uint32_t pid) final;
    uint32_t         get_tti_tx_dl() const final { return tti_alloc.get_tti_tx_dl(); }
    uint32_t         get_nof_ctrl_symbols() const final;
    const rbgmask_t& get_dl_mask() const final { return tti_alloc.get_dl_mask(); }
    // ul_tti_sched itf
    alloc_outcome_t  alloc_ul_user(sched_ue* user, ul_harq_proc::ul_alloc_t alloc) final;
    alloc_outcome_t  alloc_ul_msg3(sched_ue* user, ul_harq_proc::ul_alloc_t alloc, uint32_t mcs);
    const prbmask_t& get_ul_mask() const final { return tti_alloc.get_ul_mask(); }
    uint32_t         get_tti_tx_ul() const final { return tti_alloc.get_tti_tx_ul(); }

    // getters
    const pdcch_mask_t&            get_pdcch_mask() const { return pdcch_mask; }
    rbgmask_t&                     get_dl_mask() { return tti_alloc.get_dl_mask(); }
    prbmask_t&                     get_ul_mask() { return tti_alloc.get_ul_mask(); }
    const std::vector<ul_alloc_t>& get_ul_allocs() const { return ul_data_allocs; }
    uint32_t                       get_cfi() const { return tti_alloc.get_cfi(); }
    uint32_t                       get_tti_rx() const { return tti_alloc.get_tti_rx(); }
    uint32_t                       get_sfn() const { return tti_alloc.get_sfn(); }
    uint32_t                       get_sf_idx() const { return tti_alloc.get_sf_idx(); }

  private:
    bool            is_dl_alloc(sched_ue* user) const final;
    bool            is_ul_alloc(sched_ue* user) const final;
    ctrl_code_t     alloc_dl_ctrl(uint32_t aggr_lvl, uint32_t tbs_bytes, uint16_t rnti);
    alloc_outcome_t alloc_ul(sched_ue*                              user,
                             ul_harq_proc::ul_alloc_t               alloc,
                             tti_sched_result_t::ul_alloc_t::type_t alloc_type,
                             uint32_t                               msg3 = 0);
    int             generate_format1a(uint32_t         rb_start,
                                      uint32_t         l_crb,
                                      uint32_t         tbs,
                                      uint32_t         rv,
                                      uint16_t         rnti,
                                      srslte_dci_dl_t* dci);
    void            set_bc_sched_result(const pdcch_grid_t::alloc_result_t& dci_result);
    void            set_rar_sched_result(const pdcch_grid_t::alloc_result_t& dci_result);
    void            set_dl_data_sched_result(const pdcch_grid_t::alloc_result_t& dci_result);
    void            set_ul_sched_result(const pdcch_grid_t::alloc_result_t& dci_result);

    // consts
    carrier_sched*  parent_carrier = nullptr;
    srslte::log*    log_h          = nullptr;
    uint32_t        P              = 0;
    cell_cfg_sib_t* sibs_cfg       = nullptr;

    // internal state
    tti_grid_t               tti_alloc;
    std::vector<rar_alloc_t> rar_allocs;
    std::vector<bc_alloc_t>  bc_allocs;
    std::vector<dl_alloc_t>  data_allocs;
    std::vector<ul_alloc_t>  ul_data_allocs;
  };

  void                reset();
  void                carrier_cfg();
  void                set_metric(sched::metric_dl* dl_metric_, sched::metric_ul* ul_metric_);
  void                set_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs);
  tti_sched_result_t* generate_tti_result(uint32_t tti_rx);
  int                 dl_rach_info(dl_sched_rar_info_t rar_info);

  // private:
  void generate_phich(tti_sched_result_t* tti_sched);
  //! Compute DL scheduler result for given TTI
  void alloc_dl_users(tti_sched_result_t* tti_result);
  //! Compute UL scheduler result for given TTI
  int alloc_ul_users(tti_sched_result_t* tti_sched);

  // args
  sched*       sched_ptr = nullptr;
  srslte::log* log_h     = nullptr;
  cell_cfg_t*  cfg       = nullptr;
  metric_dl*   dl_metric = nullptr;
  metric_ul*   ul_metric = nullptr;

  // derived from args
  prbmask_t prach_mask;
  prbmask_t pucch_mask;

  // TTI result storage and management
  std::array<tti_sched_result_t, 10> tti_scheds;
  tti_sched_result_t*                get_tti_sched(uint32_t tti_rx) { return &tti_scheds[tti_rx % tti_scheds.size()]; }
  std::vector<uint8_t>               tti_dl_mask; ///< Some TTIs may be forbidden for DL sched due to MBMS

  std::unique_ptr<bc_sched> bc_sched_ptr;
  std::unique_ptr<ra_sched> ra_sched_ptr;

  // protects access to bc/ra schedulers and harqs
  std::mutex sched_mutex;
};

//! Broadcast (SIB + paging) scheduler
class bc_sched
{
public:
  explicit bc_sched(sched::cell_cfg_t* cfg_);
  void init(rrc_interface_mac* rrc_);

  void dl_sched(sched::carrier_sched::tti_sched_result_t* tti_sched);
  void reset();

private:
  struct sched_sib_t {
    bool     is_in_window = false;
    uint32_t window_start = 0;
    uint32_t n_tx         = 0;
  };

  void update_si_windows(sched::carrier_sched::tti_sched_result_t* tti_sched);
  void alloc_sibs(sched::carrier_sched::tti_sched_result_t* tti_sched);
  void alloc_paging(sched::carrier_sched::tti_sched_result_t* tti_sched);

  // args
  sched::cell_cfg_t* cfg;
  rrc_interface_mac* rrc = nullptr;

  std::array<sched_sib_t, sched_interface::MAX_SIBS> pending_sibs;

  // TTI specific
  uint32_t current_sfn = 0, current_sf_idx = 0;
  uint32_t current_tti   = 0;
  uint32_t bc_aggr_level = 2;
};

//! RAR/Msg3 scheduler
class ra_sched
{
public:
  using dl_sched_rar_info_t  = sched_interface::dl_sched_rar_info_t;
  using dl_sched_rar_t       = sched_interface::dl_sched_rar_t;
  using dl_sched_rar_grant_t = sched_interface::dl_sched_rar_grant_t;
  struct pending_msg3_t {
    bool     enabled = false;
    uint16_t rnti    = 0;
    uint32_t L       = 0;
    uint32_t n_prb   = 0;
    uint32_t mcs     = 0;
  };

  explicit ra_sched(sched::cell_cfg_t* cfg_);
  void                  init(srslte::log* log_, std::map<uint16_t, sched_ue>& ue_db_);
  void                  dl_sched(sched::carrier_sched::tti_sched_result_t* tti_sched);
  void                  ul_sched(sched::carrier_sched::tti_sched_result_t* tti_sched);
  int                   dl_rach_info(dl_sched_rar_info_t rar_info);
  void                  reset();
  const pending_msg3_t& find_pending_msg3(uint32_t tti);

private:
  // args
  srslte::log*                  log_h = nullptr;
  sched::cell_cfg_t*            cfg;
  std::map<uint16_t, sched_ue>* ue_db = nullptr;

  std::queue<dl_sched_rar_info_t>       pending_rars;
  std::array<pending_msg3_t, TTIMOD_SZ> pending_msg3;
  uint32_t                              tti_tx_dl      = 0;
  uint32_t                              rar_aggr_level = 2;
};

} // namespace srsenb

#endif // SRSLTE_SCHEDULER_CARRIER_H
