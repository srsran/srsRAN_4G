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

#ifndef SRSENB_SCHEDULER_METRIC_H
#define SRSENB_SCHEDULER_METRIC_H

#include "scheduler.h"

namespace srsenb {
  
class dl_metric_rr : public sched::metric_dl
{
  const static int MAX_RBG = 25;

public:
  void sched_users(std::map<uint16_t, sched_ue>& ue_db, rbgmask_t* dl_mask, uint32_t nof_ctrl_symbols, uint32_t tti);

private:
  bool          find_allocation(uint32_t nof_rbg, rbgmask_t* rbgmask);
  void          update_allocation(rbgmask_t new_mask);
  bool          allocation_is_valid(rbgmask_t mask);
  dl_harq_proc* allocate_user(sched_ue* user);

  uint32_t current_tti;
  //  rbgmask_t used_rbg_mask;
  rbgmask_t* used_rbg;
  uint32_t nof_ctrl_symbols;
  uint32_t available_rbg;
};

class ul_metric_rr : public sched::metric_ul
{
public:
  void sched_users(std::map<uint16_t, sched_ue>& ue_db, ul_mask_t* start_mask, uint32_t tti);

private:
  bool          find_allocation(uint32_t L, ul_harq_proc::ul_alloc_t* alloc);
  bool          allocation_is_valid(ul_harq_proc::ul_alloc_t alloc);
  bool          update_allocation(ul_harq_proc::ul_alloc_t alloc);
  ul_harq_proc* allocate_user_newtx_prbs(sched_ue* user);
  ul_harq_proc* allocate_user_retx_prbs(sched_ue *user);

  ul_mask_t* used_rb;
  uint32_t   current_tti;
};

  
}

#endif // SRSENB_SCHEDULER_METRIC_H

