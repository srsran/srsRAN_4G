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

#ifndef SRSRAN_SCHED_TIME_PF_H
#define SRSRAN_SCHED_TIME_PF_H

#include "sched_base.h"
#include "srsenb/hdr/common/common_enb.h"
#include "srsran/adt/circular_map.h"
#include <queue>

namespace srsenb {

class sched_time_pf final : public sched_base
{
  using ue_cit_t = sched_ue_list::const_iterator;

public:
  sched_time_pf(const sched_cell_params_t& cell_params_, const sched_interface::sched_args_t& sched_args);
  void sched_dl_users(sched_ue_list& ue_db, sf_sched* tti_sched) override;
  void sched_ul_users(sched_ue_list& ue_db, sf_sched* tti_sched) override;

private:
  void new_tti(sched_ue_list& ue_db, sf_sched* tti_sched);

  const sched_cell_params_t* cc_cfg         = nullptr;
  float                      fairness_coeff = 1;

  srsran::tti_point current_tti_rx;

  struct ue_ctxt {
    ue_ctxt(uint16_t rnti_, float fairness_coeff_) : rnti(rnti_), fairness_coeff(fairness_coeff_) {}
    float    dl_avg_rate() const { return dl_nof_samples == 0 ? 0 : dl_avg_rate_; }
    float    ul_avg_rate() const { return ul_nof_samples == 0 ? 0 : ul_avg_rate_; }
    uint32_t dl_count() const { return dl_nof_samples; }
    uint32_t ul_count() const { return ul_nof_samples; }
    void     new_tti(const sched_cell_params_t& cell, sched_ue& ue, sf_sched* tti_sched);
    void     save_dl_alloc(uint32_t alloc_bytes, float alpha);
    void     save_ul_alloc(uint32_t alloc_bytes, float alpha);

    const uint16_t rnti;
    const float    fairness_coeff;

    int                 ue_cc_idx  = 0;
    float               dl_prio    = 0;
    float               ul_prio    = 0;
    const dl_harq_proc* dl_retx_h  = nullptr;
    const dl_harq_proc* dl_newtx_h = nullptr;
    const ul_harq_proc* ul_h       = nullptr;

  private:
    float    dl_avg_rate_   = 0;
    float    ul_avg_rate_   = 0;
    uint32_t dl_nof_samples = 0;
    uint32_t ul_nof_samples = 0;
  };

  rnti_map_t<ue_ctxt> ue_history_db;

  struct ue_dl_prio_compare {
    bool operator()(const ue_ctxt* lhs, const ue_ctxt* rhs) const;
  };
  struct ue_ul_prio_compare {
    bool operator()(const ue_ctxt* lhs, const ue_ctxt* rhs) const;
  };

  using ue_dl_queue_t = std::priority_queue<ue_ctxt*, std::vector<ue_ctxt*>, ue_dl_prio_compare>;
  using ue_ul_queue_t = std::priority_queue<ue_ctxt*, std::vector<ue_ctxt*>, ue_ul_prio_compare>;

  ue_dl_queue_t dl_queue;
  ue_ul_queue_t ul_queue;

  uint32_t try_dl_alloc(ue_ctxt& ue_ctxt, sched_ue& ue, sf_sched* tti_sched);
  uint32_t try_ul_alloc(ue_ctxt& ue_ctxt, sched_ue& ue, sf_sched* tti_sched);
};

} // namespace srsenb

#endif // SRSRAN_SCHED_TIME_PF_H