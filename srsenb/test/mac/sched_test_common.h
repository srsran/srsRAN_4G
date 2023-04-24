/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_SCHED_TEST_COMMON_H
#define SRSRAN_SCHED_TEST_COMMON_H

#include "sched_sim_ue.h"
#include "sched_test_utils.h"
#include "srsenb/hdr/stack/mac/sched.h"
#include "srsran/interfaces/enb_rrc_interface_mac.h"
#include "srsran/srslog/srslog.h"
#include <random>

namespace srsenb {

/***************************
 *     Random Utils
 **************************/

void                        set_randseed(uint64_t seed);
float                       randf();
std::default_random_engine& get_rand_gen();

struct rrc_dummy : public rrc_interface_mac {
public:
  int      add_user(uint16_t rnti, const sched_interface::ue_cfg_t& init_ue_cfg) { return SRSRAN_SUCCESS; }
  void     upd_user(uint16_t new_rnti, uint16_t old_rnti) {}
  void     set_activity_user(uint16_t rnti) {}
  void     set_radiolink_ul_state(uint16_t rnti, bool crc_res) {}
  void     set_radiolink_dl_state(uint16_t rnti, bool crc_res) {}
  bool     is_paging_opportunity(uint32_t tti, uint32_t* payload_len) { return false; }
  uint8_t* read_pdu_bcch_dlsch(const uint8_t enb_cc_idx, const uint32_t sib_index) { return nullptr; }
  void     read_pdu_pcch(uint32_t tti_tx_dl, uint8_t* payload, uint32_t n_bytes) {}
};

/**************************
 *       Testers
 *************************/

using dl_sched_res_list = std::vector<sched_interface::dl_sched_res_t>;
using ul_sched_res_list = std::vector<sched_interface::ul_sched_res_t>;

class sched_sim_random : public sched_sim_base
{
public:
  using sched_sim_base::sched_sim_base;
  void set_external_tti_events(const sim_ue_ctxt_t& ue_ctxt, ue_tti_events& pending_events) override;
  std::map<uint16_t, ue_ctxt_test_cfg> ue_sim_cfg_map;
};

class sched_result_stats
{
public:
  explicit sched_result_stats(std::vector<srsenb::sched::cell_cfg_t> cell_params_) :
    cell_params(std::move(cell_params_))
  {}

  void process_results(tti_point                                           tti_rx,
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
    uint32_t                                     nof_prachs = 0;
    std::vector<sched_interface::dl_sched_res_t> dl_sched_result;
    std::vector<sched_interface::ul_sched_res_t> ul_sched_result;
  };

  common_sched_tester();
  ~common_sched_tester() override;

  const ue_cfg_t* get_current_ue_cfg(uint16_t rnti) const;

  int         sim_cfg(sim_sched_args args);
  virtual int add_user(uint16_t rnti, const ue_ctxt_test_cfg& ue_cfg_);
  virtual int reconf_user(uint16_t rnti, const sched_interface::ue_cfg_t& ue_cfg_);
  virtual int rem_user(uint16_t rnti);
  virtual int process_results();
  int         process_tti_events(const tti_ev& tti_ev);

  int test_next_ttis(const std::vector<tti_ev>& tti_events);
  int run_tti(const tti_ev& tti_events);

  int run_ue_ded_tests_and_update_ctxt(const sf_output_res_t& sf_out);

  // args
  sim_sched_args        sim_args0; ///< arguments used to generate TTI events
  srslog::basic_logger& logger;

  // tti specific params
  tti_info_t tti_info;
  tti_point  tti_rx;
  uint32_t   tti_count = 0;

  // eNB+UE state handlers
  std::unique_ptr<sched_sim_random> sched_sim;

  // statistics
  std::unique_ptr<sched_result_stats> sched_stats;

protected:
  virtual void new_test_tti();
  virtual void before_sched() {}

  rrc_dummy rrc_ptr;
};

} // namespace srsenb

#endif // SRSRAN_SCHED_TEST_COMMON_H
