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

#ifndef SRSENB_SCHEDULER_METRIC_H
#define SRSENB_SCHEDULER_METRIC_H

#include "sched_base.h"

namespace srsenb {

class sched_time_rr final : public sched_base
{
  const static int MAX_RBG = 25;

public:
  sched_time_rr(const sched_cell_params_t& cell_params_, const sched_interface::sched_args_t& sched_args);
  void sched_dl_users(std::map<uint16_t, sched_ue>& ue_db, sf_sched* tti_sched) override;
  void sched_ul_users(std::map<uint16_t, sched_ue>& ue_db, sf_sched* tti_sched) override;

private:
  void sched_dl_retxs(std::map<uint16_t, sched_ue>& ue_db, sf_sched* tti_sched, size_t prio_idx);
  void sched_dl_newtxs(std::map<uint16_t, sched_ue>& ue_db, sf_sched* tti_sched, size_t prio_idx);
  void sched_ul_retxs(std::map<uint16_t, sched_ue>& ue_db, sf_sched* tti_sched, size_t prio_idx);
  void sched_ul_newtxs(std::map<uint16_t, sched_ue>& ue_db, sf_sched* tti_sched, size_t prio_idx);

  const sched_cell_params_t* cc_cfg = nullptr;
};

} // namespace srsenb

#endif // SRSENB_SCHEDULER_METRIC_H