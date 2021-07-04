/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_SCHED_NR_SIM_UE_H
#define SRSRAN_SCHED_NR_SIM_UE_H

#include "../sched_sim_ue.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr.h"
#include <condition_variable>

namespace srsenb {

struct sched_nr_cc_output_res_t {
  tti_point                             tti_rx;
  uint32_t                              cc;
  sched_nr_interface::dl_tti_request_t* dl_cc_result;
  sched_nr_interface::ul_tti_request_t* ul_cc_result;
};

struct ue_nr_cc_ctxt_t {
  std::array<ue_harq_ctxt_t, SCHED_NR_MAX_HARQ> dl_harqs;
  std::array<ue_harq_ctxt_t, SCHED_NR_MAX_HARQ> ul_harqs;
};

struct sim_nr_ue_ctxt_t {
  uint16_t                     rnti;
  uint32_t                     preamble_idx;
  srsran::tti_point            prach_tti_rx;
  sched_nr_interface::ue_cfg_t ue_cfg;
  std::vector<ue_nr_cc_ctxt_t> cc_list;

  bool is_last_dl_retx(uint32_t ue_cc_idx, uint32_t pid) const
  {
    auto& h = cc_list.at(ue_cc_idx).dl_harqs[pid];
    return h.nof_retxs + 1 >= ue_cfg.maxharq_tx;
  }
};

class sched_nr_ue_sim
{
public:
  sched_nr_ue_sim(uint16_t                            rnti_,
                  const sched_nr_interface::ue_cfg_t& ue_cfg_,
                  tti_point                           prach_tti_rx,
                  uint32_t                            preamble_idx);

  int update(const sched_nr_cc_output_res_t& cc_out);

  const sim_nr_ue_ctxt_t& get_ctxt() const { return ctxt; }
  sim_nr_ue_ctxt_t&       get_ctxt() { return ctxt; }

private:
  void update_dl_harqs(const sched_nr_cc_output_res_t& sf_out);

  srslog::basic_logger& logger;
  sim_nr_ue_ctxt_t      ctxt;
};

class sched_nr_sim_base
{
public:
  sched_nr_sim_base(const sched_nr_interface::sched_cfg_t&             sched_args,
                    const std::vector<sched_nr_interface::cell_cfg_t>& cell_params_,
                    std::string                                        test_name);
  virtual ~sched_nr_sim_base();

  int add_user(uint16_t rnti, const sched_nr_interface::ue_cfg_t& ue_cfg_, uint32_t preamble_idx);

  void slot_indication(srsran::tti_point tti_rx);
  void update(sched_nr_cc_output_res_t& cc_out);

  sched_nr_ue_sim&       at(uint16_t rnti) { return ue_db.at(rnti); }
  const sched_nr_ue_sim& at(uint16_t rnti) const { return ue_db.at(rnti); }
  sched_nr_ue_sim*       find_rnti(uint16_t rnti)
  {
    auto it = ue_db.find(rnti);
    return it != ue_db.end() ? &it->second : nullptr;
  }
  const sched_nr_ue_sim* find_rnti(uint16_t rnti) const
  {
    auto it = ue_db.find(rnti);
    return it != ue_db.end() ? &it->second : nullptr;
  }
  bool                                user_exists(uint16_t rnti) const { return ue_db.count(rnti) > 0; }
  const sched_nr_interface::ue_cfg_t* get_user_cfg(uint16_t rnti) const
  {
    const sched_nr_ue_sim* ret = find_rnti(rnti);
    return ret == nullptr ? nullptr : &ret->get_ctxt().ue_cfg;
  }
  sched_nr*                                            get_sched() { return sched_ptr.get(); }
  srsran::const_span<sched_nr_impl::sched_cell_params> get_cell_params() { return cell_params; }
  tti_point                                            get_tti_rx() const
  {
    std::lock_guard<std::mutex> lock(mutex);
    return current_tti_rx;
  }

  std::map<uint16_t, sched_nr_ue_sim>::iterator begin() { return ue_db.begin(); }
  std::map<uint16_t, sched_nr_ue_sim>::iterator end() { return ue_db.end(); }

  // configurable by simulator concrete implementation
  virtual void set_external_tti_events(const sim_nr_ue_ctxt_t& ue_ctxt, ue_tti_events& pending_events) {}

private:
  int set_default_tti_events(const sim_nr_ue_ctxt_t& ue_ctxt, ue_tti_events& pending_events);
  int apply_tti_events(sim_nr_ue_ctxt_t& ue_ctxt, const ue_tti_events& events);

  std::string                                   test_name;
  srslog::basic_logger&                         logger;
  srslog::basic_logger&                         mac_logger;
  std::unique_ptr<sched_nr>                     sched_ptr;
  std::vector<sched_nr_impl::sched_cell_params> cell_params;

  srsran::tti_point                   current_tti_rx;
  std::map<uint16_t, sched_nr_ue_sim> ue_db;

  mutable std::mutex      mutex;
  std::condition_variable cond_var;
};

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_SIM_UE_H
