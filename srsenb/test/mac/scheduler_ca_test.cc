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
  int run_tti(const tti_ev& tti_events) override;
};

int sched_ca_tester::process_tti_events(const tti_ev& tti_ev)
{
  for (const tti_ev::user_cfg_ev& ue_ev : tti_ev.user_updates) {
    // There is a new configuration
    if (ue_ev.ue_cfg != nullptr) {
      if (not ue_tester->user_exists(ue_ev.rnti)) {
        // new user
        TESTASSERT(add_user(ue_ev.rnti, *ue_ev.ue_cfg) == SRSLTE_SUCCESS);
      } else {
        // reconfiguration
        TESTASSERT(ue_cfg(ue_ev.rnti, *ue_ev.ue_cfg) == SRSLTE_SUCCESS);
        ue_tester->user_reconf(ue_ev.rnti, *ue_ev.ue_cfg);
      }
    }

    // There is a user to remove
    if (ue_ev.rem_user) {
      //        bearer_ue_rem(ue_ev.rnti, 0);
      ue_rem(ue_ev.rnti);
      ue_tester->rem_user(ue_ev.rnti);
      log_global->info("[TESTER] Removing user rnti=0x%x\n", ue_ev.rnti);
    }

    // configure carriers
    if (ue_ev.bearer_cfg != nullptr) {
      CONDERROR(not ue_tester->user_exists(ue_ev.rnti), "User rnti=0x%x does not exist\n", ue_ev.rnti);
      // TODO: Instantiate more bearers
      bearer_ue_cfg(ue_ev.rnti, 0, ue_ev.bearer_cfg.get());
    }

    // push UL SRs and DL packets
    if (ue_ev.buffer_ev != nullptr) {
      auto* user = ue_tester->get_user_state(ue_ev.rnti);
      CONDERROR(user == nullptr, "TESTER ERROR: Trying to schedule data for user that does not exist\n");

      if (ue_ev.buffer_ev->dl_data > 0) {
        // If Msg3 has already been received
        if (user->msg3_tic.is_valid() and user->msg3_tic <= tic) {
          // If Msg4 not yet sent, allocate data in SRB0 buffer
          uint32_t lcid                = (user->msg4_tic.is_valid()) ? 2 : 0;
          uint32_t pending_dl_new_data = ue_db[ue_ev.rnti].get_pending_dl_new_data();
          if (lcid == 2 and not user->drb_cfg_flag) {
            // If RRCSetup finished
            if (pending_dl_new_data == 0) {
              // setup lcid==2 bearer
              sched::ue_bearer_cfg_t cfg = {};
              cfg.direction              = ue_bearer_cfg_t::BOTH;
              ue_tester->bearer_cfg(ue_ev.rnti, 2, cfg);
              bearer_ue_cfg(ue_ev.rnti, 2, &cfg);
            } else {
              // Let SRB0 get emptied
              continue;
            }
          }
          // Update DL buffer
          uint32_t tot_dl_data = pending_dl_new_data + ue_ev.buffer_ev->dl_data; // TODO: derive pending based on rx
          dl_rlc_buffer_state(ue_ev.rnti, lcid, tot_dl_data, 0);                 // TODO: Check retx_queue
        }
      }

      if (ue_ev.buffer_ev->sr_data > 0 and user->drb_cfg_flag) {
        uint32_t tot_ul_data =
            ue_db[ue_ev.rnti].get_pending_ul_new_data(tti_info.tti_params.tti_tx_ul) + ue_ev.buffer_ev->sr_data;
        uint32_t lcid = 2;
        ul_bsr(ue_ev.rnti, lcid, tot_ul_data, true);
      }
    }
  }
  return SRSLTE_SUCCESS;
}

int sched_ca_tester::run_tti(const tti_ev& tti_events)
{
  new_test_tti();
  log_global->info("[TESTER] ---- tti=%u | nof_ues=%zd ----\n", tic.tti_rx(), ue_db.size());

  process_tti_events(tti_events);
  process_ack_txs();
  //  before_sched();

  // Call scheduler for all carriers
  tti_info.dl_sched_result.resize(sched_cell_params.size());
  for (uint32_t i = 0; i < sched_cell_params.size(); ++i) {
    dl_sched(tti_info.tti_params.tti_tx_dl, i, tti_info.dl_sched_result[i]);
  }
  tti_info.ul_sched_result.resize(sched_cell_params.size());
  for (uint32_t i = 0; i < sched_cell_params.size(); ++i) {
    ul_sched(tti_info.tti_params.tti_tx_ul, i, tti_info.ul_sched_result[i]);
  }

  process_results();
  schedule_acks();
  return SRSLTE_SUCCESS;
}

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

  sim_args.bearer_cfg           = {};
  sim_args.bearer_cfg.direction = srsenb::sched_interface::ue_bearer_cfg_t::BOTH;

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