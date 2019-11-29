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
  explicit carrier_sched(sched* sched_, uint32_t cc_idx_);
  void                reset();
  void                carrier_cfg();
  void                set_metric(sched::metric_dl* dl_metric_, sched::metric_ul* ul_metric_);
  void                set_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs);
  tti_sched_result_t* generate_tti_result(uint32_t tti_rx);
  int                 dl_rach_info(dl_sched_rar_info_t rar_info);

  // getters
  const ra_sched*           get_ra_sched() const { return ra_sched_ptr.get(); }
  const tti_sched_result_t* get_tti_sched_view(uint32_t tti_rx) const
  {
    return &tti_scheds[tti_rx % tti_scheds.size()];
  }

private:
  void generate_phich(tti_sched_result_t* tti_sched);
  //! Compute DL scheduler result for given TTI
  void alloc_dl_users(tti_sched_result_t* tti_result);
  //! Compute UL scheduler result for given TTI
  int alloc_ul_users(tti_sched_result_t* tti_sched);

  // args
  sched*                sched_ptr    = nullptr;
  const sched_params_t* sched_params = nullptr;
  srslte::log*          log_h        = nullptr;
  metric_dl*            dl_metric    = nullptr;
  metric_ul*            ul_metric    = nullptr;
  const uint32_t        cc_idx;

  // derived from args
  prbmask_t prach_mask;
  prbmask_t pucch_mask;

  // TTI result storage and management
  std::array<tti_sched_result_t, TTIMOD_SZ> tti_scheds;
  tti_sched_result_t*  get_tti_sched(uint32_t tti_rx) { return &tti_scheds[tti_rx % tti_scheds.size()]; }
  std::vector<uint8_t> tti_dl_mask; ///< Some TTIs may be forbidden for DL sched due to MBMS

  std::unique_ptr<bc_sched> bc_sched_ptr;
  std::unique_ptr<ra_sched> ra_sched_ptr;

  // protects access to bc/ra schedulers and harqs
  std::mutex carrier_mutex;
};

//! Broadcast (SIB + paging) scheduler
class bc_sched
{
public:
  explicit bc_sched(const sched::cell_cfg_t& cfg_, rrc_interface_mac* rrc_);
  void dl_sched(tti_sched_result_t* tti_sched);
  void reset();

private:
  struct sched_sib_t {
    bool     is_in_window = false;
    uint32_t window_start = 0;
    uint32_t n_tx         = 0;
  };

  void update_si_windows(tti_sched_result_t* tti_sched);
  void alloc_sibs(tti_sched_result_t* tti_sched);
  void alloc_paging(tti_sched_result_t* tti_sched);

  // args
  const sched::cell_cfg_t* cfg;
  rrc_interface_mac*       rrc = nullptr;

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

  explicit ra_sched(const sched::cell_cfg_t& cfg_, srslte::log* log_, std::map<uint16_t, sched_ue>& ue_db_);
  void                  dl_sched(tti_sched_result_t* tti_sched);
  void                  ul_sched(tti_sched_result_t* tti_sched);
  int                   dl_rach_info(dl_sched_rar_info_t rar_info);
  void                  reset();
  const pending_msg3_t& find_pending_msg3(uint32_t tti) const;

private:
  // args
  srslte::log*                  log_h = nullptr;
  const sched::cell_cfg_t*      cfg   = nullptr;
  std::map<uint16_t, sched_ue>* ue_db = nullptr;

  std::queue<dl_sched_rar_info_t>       pending_rars;
  std::array<pending_msg3_t, TTIMOD_SZ> pending_msg3;
  uint32_t                              tti_tx_dl      = 0;
  uint32_t                              rar_aggr_level = 2;
};

} // namespace srsenb

#endif // SRSLTE_SCHEDULER_CARRIER_H
