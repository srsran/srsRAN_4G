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

#ifndef SRSLTE_SCHEDULER_CTRL_H
#define SRSLTE_SCHEDULER_CTRL_H

#include "scheduler.h"

namespace srsenb {

class sched::bc_sched_t
{
public:
  bc_sched_t(cell_cfg_t* cfg_);
  void init(rrc_interface_mac* rrc_);

  void dl_sched(tti_sched_t* tti_sched);
  void reset();

private:
  struct sched_sib_t {
    bool     is_in_window = false;
    uint32_t window_start = 0;
    uint32_t n_tx         = 0;
  };

  void update_si_windows(tti_sched_t* tti_sched);
  void alloc_sibs(tti_sched_t* tti_sched);
  void alloc_paging(tti_sched_t* tti_sched);

  // args
  cell_cfg_t*        cfg;
  rrc_interface_mac* rrc = nullptr;

  std::array<sched_sib_t, sched_interface::MAX_SIBS> pending_sibs;

  // TTI specific
  uint32_t current_sfn = 0, current_sf_idx = 0;
  uint32_t current_tti   = 0;
  uint32_t bc_aggr_level = 2;
};

} // namespace srsenb

#endif // SRSLTE_SCHEDULER_CTRL_H
