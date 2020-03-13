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

#ifndef SRSLTE_SCHEDULER_TEST_COMMON_H
#define SRSLTE_SCHEDULER_TEST_COMMON_H

#include "scheduler_test_utils.h"
#include "srsenb/hdr/stack/mac/scheduler.h"
#include <random>

namespace srsenb {

/***************************
 *     Random Utils
 **************************/

void                        set_randseed(uint64_t seed);
float                       randf();
std::default_random_engine& get_rand_gen();

// other helpers
int extract_dl_prbmask(const srslte_cell_t&               cell,
                       const srslte_dci_dl_t&             dci,
                       srslte::bounded_bitset<100, true>* alloc_mask);

/**************************
 *       Testers
 *************************/

class output_sched_tester
{
public:
  explicit output_sched_tester(const sched_cell_params_t& cell_params_) : cell_params(cell_params_) {}

  /* Check for collisions between RB allocations in the PUSCH and PUCCH */
  int test_pusch_collisions(const tti_params_t&                    tti_params,
                            const sched_interface::ul_sched_res_t& ul_result,
                            prbmask_t&                             ul_allocs) const;

  /* Check for collision between RB allocations in the PDSCH */
  int test_pdsch_collisions(const tti_params_t&                    tti_params,
                            const sched_interface::dl_sched_res_t& dl_result,
                            rbgmask_t&                             dl_mask) const;

  /* Check if SIBs are scheduled within their window */
  int test_sib_scheduling(const tti_params_t& tti_params, const sched_interface::dl_sched_res_t& dl_result) const;

  /* Check for collisions in the PDCCH */
  int test_pdcch_collisions(const sched_interface::dl_sched_res_t& dl_result,
                            const sched_interface::ul_sched_res_t& ul_result,
                            srslte::bounded_bitset<128, true>*     used_cce) const;

  /* Check DCI params correctness */
  int test_dci_values_consistency(const sched_interface::dl_sched_res_t& dl_result,
                                  const sched_interface::ul_sched_res_t& ul_result) const;

  int test_all(const tti_params_t&                    tti_params,
               const sched_interface::dl_sched_res_t& dl_result,
               const sched_interface::ul_sched_res_t& ul_result) const;

private:
  const sched_cell_params_t& cell_params;
};

class user_state_sched_tester
{
public:
  struct ue_state {
    tti_counter                       prach_tic, rar_tic, msg3_tic, msg4_tic;
    bool                              drb_cfg_flag = false;
    srsenb::sched_interface::ue_cfg_t user_cfg;
    uint32_t                          preamble_idx = 0;
    uint32_t                          msg3_riv     = 0;
    bool is_msg3_rx(const tti_counter& tti_rx) const { return msg3_tic.is_valid() and msg3_tic <= tti_rx; }
  };

  explicit user_state_sched_tester(const std::vector<srsenb::sched::cell_cfg_t>& cell_params_) :
    cell_params(cell_params_)
  {
  }

  void            new_tti(uint32_t tti_rx);
  bool            user_exists(uint16_t rnti) const { return users.find(rnti) != users.end(); }
  const ue_state* get_user_state(uint16_t rnti) const
  {
    return users.count(rnti) > 0 ? &users.find(rnti)->second : nullptr;
  }

  /* Config users */
  int  add_user(uint16_t rnti, uint32_t preamble_idx, const srsenb::sched_interface::ue_cfg_t& ue_cfg);
  int  user_reconf(uint16_t rnti, const srsenb::sched_interface::ue_cfg_t& ue_cfg);
  int  bearer_cfg(uint16_t rnti, uint32_t lcid, const srsenb::sched_interface::ue_bearer_cfg_t& bearer_cfg);
  void rem_user(uint16_t rnti);

  /* Test the timing of RAR, Msg3, Msg4 */
  int test_ra(uint32_t                               enb_cc_idx,
              const sched_interface::dl_sched_res_t& dl_result,
              const sched_interface::ul_sched_res_t& ul_result);

  /* Test allocs control content */
  int test_ctrl_info(uint32_t                               enb_cc_idx,
                     const sched_interface::dl_sched_res_t& dl_result,
                     const sched_interface::ul_sched_res_t& ul_result);

  /* Test correct activation of SCells */
  int test_scell_activation(uint32_t                               enb_cc_idx,
                            const sched_interface::dl_sched_res_t& dl_result,
                            const sched_interface::ul_sched_res_t& ul_result);

  int test_all(uint32_t                               enb_cc_idx,
               const sched_interface::dl_sched_res_t& dl_result,
               const sched_interface::ul_sched_res_t& ul_result);

private:
  const std::vector<srsenb::sched::cell_cfg_t>& cell_params;

  std::map<uint16_t, ue_state> users;
  tti_counter                  tic;
};

class sched_result_stats
{
public:
  explicit sched_result_stats(std::vector<srsenb::sched::cell_cfg_t> cell_params_) :
    cell_params(std::move(cell_params_))
  {
  }

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
  virtual int  add_user(uint16_t rnti, const ue_cfg_t& ue_cfg_);
  virtual void rem_user(uint16_t rnti);
  int          process_ack_txs();
  int          schedule_acks();
  virtual int  process_results();
  int          process_tti_events(const tti_ev& tti_ev);

  int test_next_ttis(const std::vector<tti_ev>& tti_events);
  int run_tti(const tti_ev& tti_events);

  // args
  sim_sched_args sim_args0; ///< arguments used to generate TTI events
  srslte::log*   tester_log = nullptr;

  // tti specific params
  tti_info_t  tti_info;
  tti_counter tic;

  // testers
  std::vector<output_sched_tester>         output_tester;
  std::unique_ptr<user_state_sched_tester> ue_tester;
  std::unique_ptr<sched_result_stats>      sched_stats;

protected:
  struct ack_info_t {
    uint16_t             rnti;
    uint32_t             tti;
    uint32_t             enb_cc_idx;
    uint32_t             ue_cc_idx;
    bool                 ack        = false;
    uint32_t             retx_delay = 0;
    srsenb::dl_harq_proc dl_harq;
  };
  struct ul_ack_info_t {
    uint16_t             rnti;
    uint32_t             tti_ack, tti_tx_ul;
    uint32_t             ue_cc_idx;
    uint32_t             enb_cc_idx;
    bool                 ack = false;
    srsenb::ul_harq_proc ul_harq;
  };

  virtual void new_test_tti();
  virtual void before_sched() {}

  // control params
  std::multimap<uint32_t, ack_info_t>    to_ack;
  std::multimap<uint32_t, ul_ack_info_t> to_ul_ack;
};

} // namespace srsenb

#endif // SRSLTE_SCHEDULER_TEST_COMMON_H
