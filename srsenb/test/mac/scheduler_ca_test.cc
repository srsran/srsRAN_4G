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

#include "lib/include/srslte/common/pdu.h"
#include "scheduler_test_common.h"
#include "scheduler_test_utils.h"
#include "srsenb/hdr/stack/mac/scheduler.h"

using namespace srsenb;

uint32_t const seed = std::chrono::system_clock::now().time_since_epoch().count();

/*******************
 *     Logging     *
 *******************/

class sched_test_log final : public srslte::test_log_filter
{
public:
  sched_test_log() : srslte::test_log_filter("TEST") { exit_on_error = true; }
  ~sched_test_log() override { log_diagnostics(); }

  void log_diagnostics() override
  {
    info("[TESTER] Number of assertion warnings: %u\n", warn_counter);
    info("[TESTER] Number of assertion errors: %u\n", error_counter);
    info("[TESTER] This was the seed: %u\n", seed);
  }
};
srslte::scoped_log<sched_test_log> log_global{};

/******************************
 *   Scheduler Tester for CA
 *****************************/

class sched_ca_tester : public common_sched_tester
{
public:
  int process_tti_events(const tti_ev& tti_events);
};

/******************************
 *      Scheduler Tests
 *****************************/

sim_sched_args generate_default_sim_args(uint32_t nof_prb, uint32_t nof_ccs)
{
  sim_sched_args sim_args;

  sim_args.P_retx = 0.1;

  sim_args.ue_cfg = generate_default_ue_cfg();

  // setup two cells
  std::vector<srsenb::sched_interface::cell_cfg_t> cell_cfg(nof_ccs, generate_default_cell_cfg(nof_prb));
  cell_cfg[0].scell_list.resize(1);
  cell_cfg[0].scell_list[0].enb_cc_idx               = 1;
  cell_cfg[0].scell_list[0].cross_carrier_scheduling = false;
  cell_cfg[0].scell_list[0].ul_allowed               = true;
  cell_cfg[1].cell.id                                = 2; // id=2
  cell_cfg[1].scell_list                             = cell_cfg[0].scell_list;
  cell_cfg[1].scell_list[0].enb_cc_idx               = 0;
  sim_args.cell_cfg                                  = std::move(cell_cfg);

  /* Setup Derived Params */
  sim_args.ue_cfg.supported_cc_list.resize(nof_ccs);
  for (uint32_t i = 0; i < sim_args.ue_cfg.supported_cc_list.size(); ++i) {
    sim_args.ue_cfg.supported_cc_list[i].active     = true;
    sim_args.ue_cfg.supported_cc_list[i].enb_cc_idx = i;
  }

  return sim_args;
}

int run_sim1()
{
  /* Simulation Configuration Arguments */
  uint32_t nof_prb   = 25;
  uint32_t nof_ccs   = 2;
  uint32_t start_tti = 0; // rand_int(0, 10240);

  /* Setup simulation arguments struct */
  sim_sched_args sim_args = generate_default_sim_args(nof_prb, nof_ccs);
  sim_args.sim_log        = log_global.get();
  sim_args.start_tti      = start_tti;

  /* Simulation Objects Setup */
  sched_sim_event_generator generator;
  // Setup scheduler
  sched_ca_tester tester;
  tester.init(nullptr);
  tester.sim_cfg(sim_args);

  /* Internal configurations. Do not touch */
  float          ul_sr_exps[]   = {1, 4}; // log rand
  float          dl_data_exps[] = {1, 4}; // log rand
  float          P_ul_sr = randf() * 0.5, P_dl = randf() * 0.5;
  const uint16_t rnti1     = 70;
  uint32_t       pcell_idx = 0;

  /* Setup Simulation */
  uint32_t prach_tti = 1, msg4_tot_delay = 10; // TODO: check correct value
  uint32_t msg4_size = 20;                     // TODO: Check
  uint32_t duration  = 1000;
  //  auto     process_ttis = [&generator, &tti_start, &tester]() {
  //    for (; tester.tti_counter <= generator.tti_counter;) {
  //      uint32_t tti = (tti_start + tester.tti_count) % 10240;
  //      log_global->step(tti);
  //      tester.run_tti(generator.tti_events[tester.tti_count]);
  //    }
  //  };

  /* Simulation */

  // Event PRACH: PRACH takes place for "rnti1", and carrier "pcell_idx"
  generator.step_until(prach_tti);
  tti_ev::user_cfg_ev* user                     = generator.add_new_default_user(duration);
  user->ue_cfg->supported_cc_list[0].enb_cc_idx = pcell_idx;
  user->rnti                                    = rnti1;
  tester.test_next_ttis(generator.tti_events);
  TESTASSERT(tester.ue_tester->user_exists(rnti1));

  // Event (TTI=prach_tti+msg4_tot_delay): First Tx (Msg4). Goes in SRB0 and contains ConRes
  generator.step_tti(msg4_tot_delay);
  generator.add_dl_data(rnti1, msg4_size);
  tester.test_next_ttis(generator.tti_events);

  // Event (20 TTIs): Data back and forth
  auto generate_data = [&](uint32_t nof_ttis, float prob_dl, float prob_ul) {
    for (uint32_t i = 0; i < nof_ttis; ++i) {
      generator.step_tti();
      bool ul_flag = randf() < prob_ul, dl_flag = randf() < prob_dl;
      if (dl_flag) {
        float exp = dl_data_exps[0] + randf() * (dl_data_exps[1] - dl_data_exps[0]);
        generator.add_dl_data(rnti1, pow(10, exp));
      }
      if (ul_flag) {
        float exp = ul_sr_exps[0] + randf() * (ul_sr_exps[1] - ul_sr_exps[0]);
        generator.add_ul_data(rnti1, pow(10, exp));
      }
    }
  };
  generate_data(20, P_dl, P_ul_sr);
  tester.test_next_ttis(generator.tti_events);

  // Event: Reconf Complete. Activate SCells. Check if CE correctly transmitted
  generator.step_tti();
  user          = generator.user_reconf(rnti1);
  *user->ue_cfg = *tester.get_ue_cfg(rnti1); // use current cfg as starting point, and add more supported ccs
  user->ue_cfg->supported_cc_list.resize(nof_ccs);
  for (uint32_t i = 0; i < user->ue_cfg->supported_cc_list.size(); ++i) {
    user->ue_cfg->supported_cc_list[i].active     = true;
    user->ue_cfg->supported_cc_list[i].enb_cc_idx = i;
  }
  tester.test_next_ttis(generator.tti_events);
  // When a new DL tx takes place, it should also encode the CE
  for (uint32_t i = 0; i < 100; ++i) {
    TESTASSERT(tester.tti_info.dl_sched_result[pcell_idx].nof_data_elems > 0);
    if (tester.tti_info.dl_sched_result[pcell_idx].data[0].nof_pdu_elems[0] > 0) {
      // it is a new DL tx
      TESTASSERT(tester.tti_info.dl_sched_result[pcell_idx].data[0].pdu[0][0].lcid ==
                 srslte::sch_subh::cetype::SCELL_ACTIVATION);
      break;
    }
    generator.step_tti();
    tester.test_next_ttis(generator.tti_events);
    // now we have two CCs
  }
  // now we have two CCs

  // Event: Generate a bit more data, now it should go through both cells
  generate_data(10, 1.0, 1.0);
  tester.test_next_ttis(generator.tti_events);
  TESTASSERT(tester.sched_stats->users[rnti1].tot_dl_sched_data[0] > 0);
  TESTASSERT(tester.sched_stats->users[rnti1].tot_dl_sched_data[1] > 0);
  TESTASSERT(tester.sched_stats->users[rnti1].tot_ul_sched_data[0] > 0);
  TESTASSERT(tester.sched_stats->users[rnti1].tot_ul_sched_data[1] > 0);

  log_global->info("[TESTER] Sim1 finished successfully\n");
  return SRSLTE_SUCCESS;
}

int main()
{
  // Setup rand seed
  set_randseed(seed);

  srslte::logmap::get_instance()->set_default_log_level(srslte::LOG_LEVEL_INFO);
  printf("[TESTER] This is the chosen seed: %u\n", seed);
  uint32_t N_runs = 1;
  for (uint32_t n = 0; n < N_runs; ++n) {
    printf("Sim run number: %u\n", n + 1);
    TESTASSERT(run_sim1() == SRSLTE_SUCCESS);
  }

  return 0;
}