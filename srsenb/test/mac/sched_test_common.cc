/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "sched_test_common.h"
#include "srsenb/hdr/common/common_enb.h"
#include "srsenb/hdr/stack/mac/sched.h"

#include "sched_common_test_suite.h"
#include "sched_ue_ded_test_suite.h"
#include "srsran/common/test_common.h"

using namespace srsenb;

/***************************
 *     Random Utils
 **************************/

std::default_random_engine rand_gen;

float ::srsenb::randf()
{
  static std::uniform_real_distribution<float> unif_dist(0, 1.0);
  return unif_dist(rand_gen);
}

void ::srsenb::set_randseed(uint64_t seed)
{
  rand_gen = std::default_random_engine(seed);
}

std::default_random_engine& ::srsenb::get_rand_gen()
{
  return rand_gen;
}

/***********************
 *  User State Tester
 ***********************/

void sched_sim_random::set_external_tti_events(const sim_ue_ctxt_t& ue_ctxt, ue_tti_events& pending_events)
{
  const auto& sim_cfg = ue_sim_cfg_map.at(ue_ctxt.rnti);

  for (uint32_t enb_cc_idx = 0; enb_cc_idx < pending_events.cc_list.size(); ++enb_cc_idx) {
    auto& cc_feedback = pending_events.cc_list[enb_cc_idx];
    if (not cc_feedback.configured) {
      continue;
    }

    // ACK DL HARQs
    if (cc_feedback.dl_pid >= 0) {
      auto& h            = ue_ctxt.cc_list[cc_feedback.ue_cc_idx].dl_harqs[cc_feedback.dl_pid];
      cc_feedback.dl_ack = randf() < sim_cfg.prob_dl_ack_mask[h.nof_retxs % sim_cfg.prob_dl_ack_mask.size()];
    }

    // ACK UL HARQs
    if (cc_feedback.ul_pid >= 0) {
      auto& h            = ue_ctxt.cc_list[cc_feedback.ue_cc_idx].ul_harqs[cc_feedback.ul_pid];
      cc_feedback.ul_ack = randf() < sim_cfg.prob_ul_ack_mask[h.nof_retxs % sim_cfg.prob_ul_ack_mask.size()];
    }

    // DL CQI
    if (cc_feedback.dl_cqi >= 0) {
      cc_feedback.dl_cqi = std::uniform_int_distribution<uint32_t>{5, 24}(get_rand_gen());
    }

    // UL CQI
    if (cc_feedback.ul_snr >= 0) {
      cc_feedback.ul_snr = std::uniform_int_distribution<uint32_t>{5, 40}(get_rand_gen());
    }
  }
}

/***********************
 *  Sim Stats Storage
 **********************/

void sched_result_stats::process_results(tti_point                                           tti_rx,
                                         const std::vector<sched_interface::dl_sched_res_t>& dl_result,
                                         const std::vector<sched_interface::ul_sched_res_t>& ul_result)
{
  for (uint32_t ccidx = 0; ccidx < dl_result.size(); ++ccidx) {
    for (uint32_t i = 0; i < dl_result[ccidx].data.size(); ++i) {
      user_stats* user = get_user(dl_result[ccidx].data[i].dci.rnti);
      user->tot_dl_sched_data[ccidx] += dl_result[ccidx].data[i].tbs[0];
      user->tot_dl_sched_data[ccidx] += dl_result[ccidx].data[i].tbs[1];
    }
    for (uint32_t i = 0; i < ul_result[ccidx].pusch.size(); ++i) {
      user_stats* user = get_user(ul_result[ccidx].pusch[i].dci.rnti);
      user->tot_ul_sched_data[ccidx] += ul_result[ccidx].pusch[i].tbs;
    }
  }
}

sched_result_stats::user_stats* sched_result_stats::get_user(uint16_t rnti)
{
  if (users.count(rnti) != 0) {
    return &users[rnti];
  }
  users[rnti].rnti = rnti;
  users[rnti].tot_dl_sched_data.resize(cell_params.size(), 0);
  users[rnti].tot_ul_sched_data.resize(cell_params.size(), 0);
  return &users[rnti];
}

/***********************
 * Common Sched Tester
 **********************/

common_sched_tester::common_sched_tester() : logger(srslog::fetch_basic_logger("TEST")) {}

common_sched_tester::~common_sched_tester() {}

const sched::ue_cfg_t* common_sched_tester::get_current_ue_cfg(uint16_t rnti) const
{
  return sched_sim->get_user_cfg(rnti);
}

int common_sched_tester::sim_cfg(sim_sched_args args)
{
  sim_args0 = std::move(args);

  sched::init(&rrc_ptr, sim_args0.sched_args);

  sched_sim.reset(new sched_sim_random{this, sim_args0.sched_args, sim_args0.cell_cfg});
  sched_stats.reset(new sched_result_stats{sim_args0.cell_cfg});

  return SRSRAN_SUCCESS;
}

int common_sched_tester::add_user(uint16_t rnti, const ue_ctxt_test_cfg& ue_cfg_)
{
  logger.info("Adding user rnti=0x%x", rnti);
  sched_sim->ue_sim_cfg_map[rnti] = ue_cfg_;
  return sched_sim->add_user(rnti, ue_cfg_.ue_cfg, tti_info.nof_prachs++);
}

int common_sched_tester::reconf_user(uint16_t rnti, const sched_interface::ue_cfg_t& ue_cfg_)
{
  return sched_sim->ue_recfg(rnti, ue_cfg_);
}

int common_sched_tester::rem_user(uint16_t rnti)
{
  logger.info("Removing user rnti=0x%x", rnti);
  sched_sim->ue_sim_cfg_map.erase(rnti);
  return sched_sim->rem_user(rnti);
}

void common_sched_tester::new_test_tti()
{
  if (not tti_rx.is_valid()) {
    tti_rx = srsran::tti_point{sim_args0.start_tti};
  } else {
    tti_rx++;
  }

  tti_info.nof_prachs = 0;
  tti_info.dl_sched_result.clear();
  tti_info.ul_sched_result.clear();
  tti_info.dl_sched_result.resize(sched_cell_params.size());
  tti_info.ul_sched_result.resize(sched_cell_params.size());

  logger.set_context(tti_rx.to_uint());
}

int common_sched_tester::run_ue_ded_tests_and_update_ctxt(const sf_output_res_t& sf_out)
{
  // Perform UE-dedicated sched result tests
  sim_enb_ctxt_t enb_ctxt = sched_sim->get_enb_ctxt();
  TESTASSERT(test_all_ues(enb_ctxt, sf_out) == SRSRAN_SUCCESS);

  // Update Simulated UEs state
  sched_sim->update(sf_out);
  return SRSRAN_SUCCESS;
}

int common_sched_tester::process_results()
{
  // Perform common eNB result tests
  sf_output_res_t sf_out{sched_cell_params, tti_rx, tti_info.ul_sched_result, tti_info.dl_sched_result};
  TESTASSERT(test_all_common(sf_out) == SRSRAN_SUCCESS);
  TESTASSERT(run_ue_ded_tests_and_update_ctxt(sf_out) == SRSRAN_SUCCESS);

  sched_stats->process_results(tti_rx, tti_info.dl_sched_result, tti_info.ul_sched_result);

  return SRSRAN_SUCCESS;
}

int common_sched_tester::process_tti_events(const tti_ev& tti_ev)
{
  for (const tti_ev::user_cfg_ev& ue_ev : tti_ev.user_updates) {
    // There is a new configuration
    if (ue_ev.ue_sim_cfg != nullptr) {
      if (not sched_sim->user_exists(ue_ev.rnti)) {
        // new user
        TESTASSERT(add_user(ue_ev.rnti, *ue_ev.ue_sim_cfg) == SRSRAN_SUCCESS);
      } else {
        // reconfiguration
        TESTASSERT(reconf_user(ue_ev.rnti, ue_ev.ue_sim_cfg->ue_cfg) == SRSRAN_SUCCESS);
      }
    }

    // There is a user to remove
    if (ue_ev.rem_user) {
      TESTASSERT(rem_user(ue_ev.rnti) == SRSRAN_SUCCESS);
    }

    // configure bearers
    if (ue_ev.bearer_cfg != nullptr) {
      CONDERROR(not sched_sim->user_exists(ue_ev.rnti), "User rnti=0x%x does not exist", ue_ev.rnti);
      // TODO: Instantiate more bearers
      TESTASSERT(sched_sim->bearer_cfg(ue_ev.rnti, 0, *ue_ev.bearer_cfg) == SRSRAN_SUCCESS);
    }

    const ue_sim* user = sched_sim->find_rnti(ue_ev.rnti);

    // push UL SRs and DL packets
    if (ue_ev.buffer_ev != nullptr) {
      CONDERROR(user == nullptr, "TESTER ERROR: Trying to schedule data for user that does not exist");
      const auto& ue_sim_ctxt = user->get_ctxt();
      if (ue_ev.buffer_ev->dl_data > 0 and ue_sim_ctxt.conres_rx) {
        // If Msg4 has already been tx and there DL data to transmit
        uint32_t lcid                = drb_to_lcid(lte_drb::drb1);
        uint32_t pending_dl_new_data = ue_db[ue_ev.rnti]->get_pending_dl_rlc_data();
        // DRB is set. Update DL buffer
        uint32_t tot_dl_data = pending_dl_new_data + ue_ev.buffer_ev->dl_data; // TODO: derive pending based on rx
        dl_rlc_buffer_state(ue_ev.rnti, lcid, tot_dl_data, 0);                 // TODO: Check retx_queue
      }

      if (ue_ev.buffer_ev->sr_data > 0 and ue_sim_ctxt.conres_rx) {
        uint32_t tot_ul_data =
            ue_db[ue_ev.rnti]->get_pending_ul_new_data(to_tx_ul(tti_rx), -1) + ue_ev.buffer_ev->sr_data;
        uint32_t lcg = 1;
        ul_bsr(ue_ev.rnti, lcg, tot_ul_data);
      }
    }
  }
  return SRSRAN_SUCCESS;
}

int common_sched_tester::run_tti(const tti_ev& tti_events)
{
  new_test_tti();
  logger.info("---- tti=%u | nof_ues=%zd ----", tti_rx.to_uint(), ue_db.size());

  sched_sim->new_tti(tti_rx);
  process_tti_events(tti_events);
  before_sched();

  // Call scheduler for all carriers
  tti_info.dl_sched_result.resize(sched_cell_params.size());
  for (uint32_t i = 0; i < sched_cell_params.size(); ++i) {
    dl_sched(to_tx_dl(tti_rx).to_uint(), i, tti_info.dl_sched_result[i]);
  }
  tti_info.ul_sched_result.resize(sched_cell_params.size());
  for (uint32_t i = 0; i < sched_cell_params.size(); ++i) {
    ul_sched(to_tx_ul(tti_rx).to_uint(), i, tti_info.ul_sched_result[i]);
  }

  TESTASSERT(process_results() == SRSRAN_SUCCESS);
  tti_count++;
  return SRSRAN_SUCCESS;
}

int common_sched_tester::test_next_ttis(const std::vector<tti_ev>& tti_events)
{
  while (tti_count < tti_events.size()) {
    TESTASSERT(run_tti(tti_events[tti_count]) == SRSRAN_SUCCESS);
  }
  return SRSRAN_SUCCESS;
}
