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

#ifndef SRSLTE_SCHEDULER_TEST_COMMON_H
#define SRSLTE_SCHEDULER_TEST_COMMON_H

#include "srsenb/hdr/stack/mac/scheduler.h"

namespace srsenb {

// helpers
int extract_dl_prbmask(const srslte_cell_t&               cell,
                       const srslte_dci_dl_t&             dci,
                       srslte::bounded_bitset<100, true>* alloc_mask);

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
    int                               prach_tti = -1, rar_tti = -1, msg3_tti = -1, msg4_tti = -1;
    bool                              drb_cfg_flag = false;
    srsenb::sched_interface::ue_cfg_t user_cfg;
    uint32_t                          dl_data      = 0;
    uint32_t                          ul_data      = 0;
    uint32_t                          preamble_idx = 0;
  };

  explicit user_state_sched_tester(const std::vector<srsenb::sched::cell_cfg_t>& cell_params_) :
    cell_params(cell_params_)
  {
  }

  void            new_tti(uint32_t tti_rx) { tti_params = tti_params_t{tti_rx}; }
  bool            user_exists(uint16_t rnti) const { return users.find(rnti) != users.end(); }
  const ue_state* get_user_state(uint16_t rnti) const
  {
    return users.count(rnti) > 0 ? &users.find(rnti)->second : nullptr;
  }

  /* Register new users */
  int add_user(uint16_t rnti, uint32_t preamble_idx, const srsenb::sched_interface::ue_cfg_t& ue_cfg);
  int user_reconf(uint16_t rnti, const srsenb::sched_interface::ue_cfg_t& ue_cfg);
  int bearer_cfg(uint16_t rnti, uint32_t lcid, const srsenb::sched_interface::ue_bearer_cfg_t& bearer_cfg);

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
  const std::vector<srsenb::sched::cell_cfg_t> cell_params;

  std::map<uint16_t, ue_state> users;
  tti_params_t                 tti_params{10241};
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

} // namespace srsenb

#endif // SRSLTE_SCHEDULER_TEST_COMMON_H
