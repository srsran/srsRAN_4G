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

#ifndef SRSLTE_SCHEDULER_CARRIER_H
#define SRSLTE_SCHEDULER_CARRIER_H

#include "scheduler.h"

namespace srsenb {

class bc_sched;
class ra_sched;

class sched::carrier_sched
{
public:
  explicit carrier_sched(rrc_interface_mac* rrc_, std::map<uint16_t, sched_ue>* ue_db_, uint32_t enb_cc_idx_);
  ~carrier_sched();
  void                   reset();
  void                   carrier_cfg(const sched_cell_params_t& sched_params_);
  void                   set_dl_tti_mask(uint8_t* tti_mask, uint32_t nof_sfs);
  const sf_sched_result& generate_tti_result(uint32_t tti_rx);
  int                    dl_rach_info(dl_sched_rar_info_t rar_info);

  // getters
  const ra_sched* get_ra_sched() const { return ra_sched_ptr.get(); }
  //! Get a subframe result for a given tti
  const sf_sched_result& get_sf_result(uint32_t tti_rx) const;

private:
  //! Compute DL scheduler result for given TTI
  void alloc_dl_users(sf_sched* tti_result);
  //! Compute UL scheduler result for given TTI
  int alloc_ul_users(sf_sched* tti_sched);
  //! Get sf_sched for a given TTI
  sf_sched*        get_sf_sched(uint32_t tti_rx);
  sf_sched_result* get_next_sf_result(uint32_t tti_rx);

  // args
  const sched_cell_params_t*    cc_cfg = nullptr;
  srslte::log_ref               log_h;
  rrc_interface_mac*            rrc   = nullptr;
  std::map<uint16_t, sched_ue>* ue_db = nullptr;
  std::unique_ptr<metric_dl>    dl_metric;
  std::unique_ptr<metric_ul>    ul_metric;
  const uint32_t                enb_cc_idx;

  // derived from args
  prbmask_t prach_mask;
  prbmask_t pucch_mask;

  // TTI result storage and management
  std::array<sf_sched, TTIMOD_SZ>            sf_scheds;
  std::array<sf_sched_result, TTIMOD_SZ * 2> sf_sched_results;

  std::vector<uint8_t> sf_dl_mask; ///< Some TTIs may be forbidden for DL sched due to MBMS

  std::unique_ptr<bc_sched> bc_sched_ptr;
  std::unique_ptr<ra_sched> ra_sched_ptr;
};

//! Broadcast (SIB + paging) scheduler
class bc_sched
{
public:
  explicit bc_sched(const sched_cell_params_t& cfg_, rrc_interface_mac* rrc_);
  void dl_sched(sf_sched* tti_sched);
  void reset();

private:
  struct sched_sib_t {
    bool     is_in_window = false;
    uint32_t window_start = 0;
    uint32_t n_tx         = 0;
  };

  void update_si_windows(sf_sched* tti_sched);
  void alloc_sibs(sf_sched* tti_sched);
  void alloc_paging(sf_sched* tti_sched);

  // args
  const sched_cell_params_t* cc_cfg = nullptr;
  rrc_interface_mac*         rrc    = nullptr;

  std::array<sched_sib_t, sched_interface::MAX_SIBS> pending_sibs;

  // TTI specific
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

  explicit ra_sched(const sched_cell_params_t& cfg_, std::map<uint16_t, sched_ue>& ue_db_);
  void dl_sched(sf_sched* tti_sched);
  void ul_sched(sf_sched* sf_dl_sched, sf_sched* sf_msg3_sched);
  int  dl_rach_info(dl_sched_rar_info_t rar_info);
  void reset();

private:
  // args
  srslte::log_ref               log_h;
  const sched_cell_params_t*    cc_cfg = nullptr;
  std::map<uint16_t, sched_ue>* ue_db  = nullptr;

  std::deque<sf_sched::pending_rar_t> pending_rars;
  uint32_t                            rar_aggr_level = 2;
};

} // namespace srsenb

#endif // SRSLTE_SCHEDULER_CARRIER_H
