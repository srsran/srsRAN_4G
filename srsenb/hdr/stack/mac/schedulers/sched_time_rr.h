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

#ifndef SRSENB_SCHEDULER_METRIC_H
#define SRSENB_SCHEDULER_METRIC_H

#include "sched_base.h"

namespace srsenb {

class sched_time_rr final : public sched_base
{
  const static int MAX_RBG = 25;

public:
  sched_time_rr(const sched_cell_params_t& cell_params_, const sched_interface::sched_args_t& sched_args);
  void sched_dl_users(sched_ue_list& ue_db, sf_sched* tti_sched) override;
  void sched_ul_users(sched_ue_list& ue_db, sf_sched* tti_sched) override;

private:
  void sched_dl_retxs(sched_ue_list& ue_db, sf_sched* tti_sched, size_t prio_idx);
  void sched_dl_newtxs(sched_ue_list& ue_db, sf_sched* tti_sched, size_t prio_idx);
  void sched_ul_retxs(sched_ue_list& ue_db, sf_sched* tti_sched, size_t prio_idx);
  void sched_ul_newtxs(sched_ue_list& ue_db, sf_sched* tti_sched, size_t prio_idx);

  const sched_cell_params_t* cc_cfg = nullptr;
};

} // namespace srsenb

#endif // SRSENB_SCHEDULER_METRIC_H