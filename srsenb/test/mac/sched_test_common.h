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

#ifndef SRSLTE_SCHED_TEST_COMMON_H
#define SRSLTE_SCHED_TEST_COMMON_H

#include "sched_sim_ue.h"
#include "sched_test_utils.h"
#include "srsenb/hdr/stack/mac/sched.h"
#include <random>

namespace srsenb {

/***************************
 *     Random Utils
 **************************/

void                        set_randseed(uint64_t seed);
float                       randf();
std::default_random_engine& get_rand_gen();

/**************************
 *       Testers
 *************************/

using dl_sched_res_list = std::vector<sched_interface::dl_sched_res_t>;
using ul_sched_res_list = std::vector<sched_interface::ul_sched_res_t>;

struct ue_ctxt_test {
  // args
  srslte::log_ref  log_h{"TEST"};
  ue_ctxt_test_cfg sim_cfg;

  // prach args
  uint16_t rnti;

  /* state */
  srsenb::sched_interface::ue_cfg_t user_cfg;
  srslte::tti_point                 current_tti_rx;

  struct cc_ue_ctxt_test {
    uint32_t ue_cc_idx  = 0;
    uint32_t enb_cc_idx = 0;
  };
  std::vector<cc_ue_ctxt_test> active_ccs;
  ue_sim*                      ue_ctxt;

  bool drb_cfg_flag = false;

  ue_ctxt_test(uint16_t rnti_, srslte::tti_point prach_tti, const ue_ctxt_test_cfg& cfg_, ue_sim& ue_ctxt_);

  int              set_cfg(const sched::ue_cfg_t& ue_cfg_);
  cc_ue_ctxt_test* get_cc_state(uint32_t enb_cc_idx);

  int new_tti(sched* sched_ptr, srslte::tti_point tti_rx);
  int test_sched_result(uint32_t                     enb_cc_idx,
                        const sched::dl_sched_res_t& dl_result,
                        const sched::ul_sched_res_t& ul_result);

private:
  int fwd_pending_acks(sched* sched_ptr);

  struct cc_result {
    uint32_t                     enb_cc_idx;
    const sched::dl_sched_res_t* dl_result;
    const sched::ul_sched_res_t* ul_result;
  };
  //! Test correct activation of SCells
  int test_scell_activation(cc_result result);
  int schedule_acks(cc_result result);

  struct pending_ack_t {
    srslte::tti_point tti_ack;
    uint32_t          cc_idx, ue_cc_idx, tb, pid;
    bool              ack;
    bool              operator<(const pending_ack_t& other) const { return tti_ack > other.tti_ack; }
  };
  std::priority_queue<pending_ack_t> pending_dl_acks, pending_ul_acks;
};

class user_state_sched_tester
{
public:
  explicit user_state_sched_tester(const std::vector<srsenb::sched::cell_cfg_t>& cell_params_) :
    cell_params(cell_params_)
  {}

  void                new_tti(sched* sched_ptr, uint32_t tti_rx);
  bool                user_exists(uint16_t rnti) const { return users.find(rnti) != users.end(); }
  const ue_ctxt_test* get_user_ctxt(uint16_t rnti) const
  {
    return users.count(rnti) > 0 ? &users.find(rnti)->second : nullptr;
  }
  const sched::ue_cfg_t* get_user_cfg(uint16_t rnti) const
  {
    return users.count(rnti) > 0 ? &users.find(rnti)->second.user_cfg : nullptr;
  }

  /* Config users */
  int  add_user(uint16_t rnti, uint32_t preamble_idx, const ue_ctxt_test_cfg& cfg);
  int  user_reconf(uint16_t rnti, const srsenb::sched_interface::ue_cfg_t& ue_cfg);
  int  bearer_cfg(uint16_t rnti, uint32_t lcid, const srsenb::sched_interface::ue_bearer_cfg_t& bearer_cfg);
  void rem_user(uint16_t rnti);

  int test_all(const sf_output_res_t& sf_out);

private:
  const std::vector<srsenb::sched::cell_cfg_t>& cell_params;

  ue_db_sim                        sim_users;
  std::map<uint16_t, ue_ctxt_test> users;
  srslte::tti_point                tic;
};

class sched_result_stats
{
public:
  explicit sched_result_stats(std::vector<srsenb::sched::cell_cfg_t> cell_params_) :
    cell_params(std::move(cell_params_))
  {}

  void process_results(const tti_params_t&                                 tti_params,
                       const std::vector<sched_interface::dl_sched_res_t>& dl_result,
                       const std::vector<sched_interface::ul_sched_res_t>& ul_result);

  struct user_stats {
    uint16_t              rnti;
    std::vector<uint64_t> tot_dl_sched_data; // includes retxs
    std::vector<uint64_t> tot_ul_sched_data;
  };

  std::map<uint16_t, user_stats> users;

private:
  user_stats* get_user(uint16_t rnti);

  const std::vector<srsenb::sched::cell_cfg_t> cell_params;
};

// Intrusive Scheduler Tester
class common_sched_tester : public sched
{
public:
  struct tti_info_t {
    tti_params_t                                 tti_params{10241};
    uint32_t                                     nof_prachs = 0;
    std::vector<sched_interface::dl_sched_res_t> dl_sched_result;
    std::vector<sched_interface::ul_sched_res_t> ul_sched_result;
  };

  ~common_sched_tester() override = default;

  const ue_cfg_t* get_current_ue_cfg(uint16_t rnti) const;

  int          sim_cfg(sim_sched_args args);
  virtual int  add_user(uint16_t rnti, const ue_ctxt_test_cfg& ue_cfg_);
  virtual int  reconf_user(uint16_t rnti, const sched_interface::ue_cfg_t& ue_cfg_);
  virtual void rem_user(uint16_t rnti);
  virtual int  process_results();
  int          process_tti_events(const tti_ev& tti_ev);

  int test_next_ttis(const std::vector<tti_ev>& tti_events);
  int run_tti(const tti_ev& tti_events);

  // args
  sim_sched_args sim_args0; ///< arguments used to generate TTI events
  srslte::log*   tester_log = nullptr;

  // tti specific params
  tti_info_t        tti_info;
  srslte::tti_point tic;
  uint32_t          tti_count = 0;

  // testers
  std::unique_ptr<user_state_sched_tester> ue_tester;
  std::unique_ptr<sched_result_stats>      sched_stats;

protected:
  virtual void new_test_tti();
  virtual void before_sched() {}
};

} // namespace srsenb

#endif // SRSLTE_SCHED_TEST_COMMON_H
