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

#include "sched.h"

namespace srsenb {

class dl_metric_rr : public sched::metric_dl
{
  const static int MAX_RBG = 25;

public:
  void set_params(const sched_cell_params_t& cell_params_) final;
  void sched_users(std::map<uint16_t, sched_ue>& ue_db, dl_sf_sched_itf* tti_sched) final;

private:
  bool          find_allocation(uint32_t min_nof_rbg, uint32_t max_nof_rbg, rbgmask_t* rbgmask);
  dl_harq_proc* allocate_user(sched_ue* user);

  const sched_cell_params_t* cc_cfg = nullptr;
  srslte::log_ref            log_h;
  dl_sf_sched_itf*           tti_alloc = nullptr;
};

class ul_metric_rr : public sched::metric_ul
{
public:
  void set_params(const sched_cell_params_t& cell_params_) final;
  void sched_users(std::map<uint16_t, sched_ue>& ue_db, ul_sf_sched_itf* tti_sched) final;

private:
  bool          find_allocation(uint32_t L, prb_interval* alloc);
  ul_harq_proc* allocate_user_newtx_prbs(sched_ue* user);
  ul_harq_proc* allocate_user_retx_prbs(sched_ue* user);

  const sched_cell_params_t* cc_cfg = nullptr;
  srslte::log_ref            log_h;
  ul_sf_sched_itf*           tti_alloc   = nullptr;
  uint32_t                   current_tti = 0;
};

} // namespace srsenb

#endif // SRSENB_SCHEDULER_METRIC_H
