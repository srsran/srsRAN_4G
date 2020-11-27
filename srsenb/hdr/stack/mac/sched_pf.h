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

#ifndef SRSLTE_SCHED_PF_H
#define SRSLTE_SCHED_PF_H

#include "sched.h"
#include <queue>

namespace srsenb {

class sched_dl_pf : public sched::metric_dl
{
  using ue_cit_t = std::map<uint16_t, sched_ue>::const_iterator;

public:
  void set_params(const sched_cell_params_t& cell_params_) final;
  void sched_users(std::map<uint16_t, sched_ue>& ue_db, dl_sf_sched_itf* tti_sched) final;

private:
  const sched_cell_params_t* cc_cfg = nullptr;
  srslte::log_ref            log_h;

  struct ue_ctxt {
    ue_ctxt(uint16_t rnti_) : rnti(rnti_) {}
    float    avg_rate() const { return nof_samples == 0 ? 0 : rate; }
    uint32_t count() const { return nof_samples; }
    void     new_tti(const sched_cell_params_t& cell, sched_ue& ue, dl_sf_sched_itf* tti_sched);
    void     save_history(bool alloc, float alpha);

    const uint16_t rnti;

    uint32_t      ue_cc_idx = 0;
    bool          is_retx   = false;
    float         prio      = 0;
    dl_harq_proc* h         = nullptr;

  private:
    float    rate        = 0;
    uint32_t nof_samples = 0;
  };
  std::map<uint16_t, ue_ctxt> ue_history_db;
  struct ue_prio_compare {
    bool operator()(const ue_ctxt* lhs, const ue_ctxt* rhs) const;
  };
  std::priority_queue<ue_ctxt*, std::vector<ue_ctxt*>, ue_prio_compare> ue_queue;

  bool try_dl_alloc(ue_ctxt& ue_ctxt, sched_ue& ue, dl_sf_sched_itf* tti_sched);
};

class sched_ul_pf : public sched::metric_ul
{
  using ue_cit_t = std::map<uint16_t, sched_ue>::const_iterator;

public:
  void set_params(const sched_cell_params_t& cell_params_) final;
  void sched_users(std::map<uint16_t, sched_ue>& ue_db, ul_sf_sched_itf* tti_sched) final;

private:
  const sched_cell_params_t* cc_cfg = nullptr;
  srslte::log_ref            log_h;
  struct ue_ctxt {
    ue_ctxt(uint16_t rnti_) : rnti(rnti_) {}
    float    avg_rate() const { return nof_samples == 0 ? 0 : rate; }
    uint32_t count() const { return nof_samples; }
    void     new_tti(const sched_cell_params_t& cell, sched_ue& ue, ul_sf_sched_itf* tti_sched);
    void     save_history(bool alloc, float alpha);

    const uint16_t rnti;

    uint32_t      ue_cc_idx = 0;
    float         prio      = 0;
    ul_harq_proc* h         = nullptr;

  private:
    float    rate        = 0;
    uint32_t nof_samples = 0;
  };
  std::map<uint16_t, ue_ctxt> ue_history_db;
  struct ue_prio_compare {
    bool operator()(const ue_ctxt* lhs, const ue_ctxt* rhs) const;
  };
  std::priority_queue<ue_ctxt*, std::vector<ue_ctxt*>, ue_prio_compare> ue_queue;

  bool try_ul_alloc(ue_ctxt& ue_ctxt, sched_ue& ue, ul_sf_sched_itf* tti_sched);
};

} // namespace srsenb

#endif // SRSLTE_SCHED_PF_H
