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

#include "scheduler_test_utils.h"
#include "srsenb/hdr/stack/mac/scheduler.h"

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
 * Setup Scheduler Tester Args
 *****************************/

sched_sim_events generate_default_sim_events(uint32_t nof_prb, uint32_t nof_ccs)
{
  sched_sim_events sim_events;
  sim_sched_args&  sim_args = sim_events.sim_args;

  sim_args.nof_ttis = 10240 + 10;
  sim_args.P_retx   = 0.1;

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

  sim_args.bearer_cfg           = {};
  sim_args.bearer_cfg.direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;

  /* Setup Derived Params */
  sim_args.ue_cfg.supported_cc_list.resize(nof_ccs);
  for (uint32_t i = 0; i < sim_args.ue_cfg.supported_cc_list.size(); ++i) {
    sim_args.ue_cfg.supported_cc_list[i].active     = true;
    sim_args.ue_cfg.supported_cc_list[i].enb_cc_idx = i;
  }

  return sim_events;
}

sched_sim_events generate_sim1()
{
  /* Simulation Configuration */
  uint32_t nof_prb = 25;
  uint32_t nof_ccs = 2;

  sched_sim_events sim_events = generate_default_sim_events(nof_prb, nof_ccs);

  /* Internal configurations. Do not touch */
  float ul_sr_exps[]   = {1, 4}; // log rand
  float dl_data_exps[] = {1, 4}; // log rand
  float P_ul_sr = randf() * 0.5, P_dl = randf() * 0.5;

  sched_sim_event_generator generator;

  /* Setup Events */
  uint32_t prach_tti = 1, msg4_tot_delay = 10; // TODO: check correct value
  uint32_t msg4_size = 20;                     // TODO: Check
  uint32_t duration  = 1000;

  // Event PRACH: at prach_tti
  generator.step_until(prach_tti);
  tti_ev::user_cfg_ev* user = generator.add_new_default_user(duration);
  uint16_t             rnti = user->rnti;

  // Event (TTI=prach_tti+msg4_tot_delay): First Tx (Msg4)
  generator.step_tti(msg4_tot_delay);
  generator.add_dl_data(rnti, msg4_size);

  // Event (20 TTIs): Data back and forth
  auto generate_data = [&](uint32_t nof_ttis) {
    for (uint32_t i = 0; i < nof_ttis; ++i) {
      generator.step_tti();
      bool ul_flag = randf() < P_ul_sr, dl_flag = randf() < P_dl;
      if (dl_flag) {
        float exp = dl_data_exps[0] + randf() * (dl_data_exps[1] - dl_data_exps[0]);
        generator.add_dl_data(rnti, pow(10, exp));
      }
      if (ul_flag) {
        float exp = ul_sr_exps[0] + randf() * (ul_sr_exps[1] - ul_sr_exps[0]);
        generator.add_ul_data(rnti, pow(10, exp));
      }
    }
  };
  generate_data(20);

  // Event: Reconf Complete. Activate SCells
  user = generator.user_reconf(rnti);
  user->ue_cfg->supported_cc_list.resize(nof_ccs);
  for (uint32_t i = 0; i < user->ue_cfg->supported_cc_list.size(); ++i) {
    user->ue_cfg->supported_cc_list[i].active     = true;
    user->ue_cfg->supported_cc_list[i].enb_cc_idx = i;
  }
  // now we have two CCs

  // Generate a bit more data, now it should go through both cells
  generate_data(20);
  // We should have scheduled the SCell Activation by now

  sim_events.tti_events        = std::move(generator.tti_events);
  sim_events.sim_args.nof_ttis = sim_events.tti_events.size();

  return sim_events;
}

/******************************
 *   Scheduler Tester for CA
 *****************************/

class sched_ca_tester : public srsenb::sched
{
public:
  void run_tti(uint32_t tti);
};

void sched_ca_tester::run_tti(uint32_t tti_rx)
{
  //  new_test_tti(tti_rx);
  log_global->info("[TESTER] ---- tti=%u | nof_ues=%zd ----\n", tti_rx, ue_db.size());

  //  process_tti_args();
  //
  //  ack_txs();
  //  before_sched();
  //
  //  dl_sched(tti_data.tti_tx_dl, CARRIER_IDX, tti_data.sched_result_dl);
  //  ul_sched(tti_data.tti_tx_ul, CARRIER_IDX, tti_data.sched_result_ul);
  //
  //  process_results();
}

int test_scheduler_ca(const sched_sim_events& sim_events)
{
  // Create classes
  sched_ca_tester tester;

  tester.init(nullptr);
  tester.cell_cfg(sim_events.sim_args.cell_cfg);

  uint32_t tti_start = 0; // rand_int(0, 10240);
  for (uint32_t nof_ttis = 0; nof_ttis < sim_events.sim_args.nof_ttis; ++nof_ttis) {
    uint32_t tti = (tti_start + nof_ttis) % 10240;
    log_global->step(tti);
    tester.run_tti(tti);
  }

  return SRSLTE_SUCCESS;
}

int main()
{
  srslte::logmap::get_instance()->set_default_log_level(srslte::LOG_LEVEL_INFO);
  printf("[TESTER] This is the chosen seed: %u\n", seed);
  uint32_t N_runs = 1;
  for (uint32_t n = 0; n < N_runs; ++n) {
    printf("Sim run number: %u\n", n + 1);
    sched_sim_events sim_events = generate_sim1();

    TESTASSERT(test_scheduler_ca(sim_events) == SRSLTE_SUCCESS);
  }

  return 0;
}