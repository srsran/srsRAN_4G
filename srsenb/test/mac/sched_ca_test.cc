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

#include "sched_test_common.h"
#include "sched_test_utils.h"
#include "srsenb/hdr/stack/mac/sched.h"
#include "srsran/common/common_lte.h"
#include "srsran/mac/pdu.h"

using namespace srsenb;

uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();

/*******************
 *     Logging     *
 *******************/

/// RAII style class that prints the test diagnostic info on destruction.
class sched_diagnostic_printer
{
public:
  explicit sched_diagnostic_printer(srsran::log_sink_spy& s) : s(s) {}

  ~sched_diagnostic_printer()
  {
    auto& logger = srslog::fetch_basic_logger("TEST");
    logger.info("[TESTER] Number of assertion warnings: %u", s.get_warning_counter());
    logger.info("[TESTER] Number of assertion errors: %u", s.get_error_counter());
    logger.info("[TESTER] This was the seed: %u", seed);
    srslog::flush();
  }

private:
  srsran::log_sink_spy& s;
};

/******************************
 *      Scheduler Tests
 *****************************/

sim_sched_args generate_default_sim_args(uint32_t nof_prb, uint32_t nof_ccs)
{
  sim_sched_args sim_args;

  sim_args.default_ue_sim_cfg.ue_cfg = generate_default_ue_cfg2();

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
  sim_args.default_ue_sim_cfg.ue_cfg.supported_cc_list.resize(nof_ccs);
  for (uint32_t i = 0; i < sim_args.default_ue_sim_cfg.ue_cfg.supported_cc_list.size(); ++i) {
    sim_args.default_ue_sim_cfg.ue_cfg.supported_cc_list[i].active     = true;
    sim_args.default_ue_sim_cfg.ue_cfg.supported_cc_list[i].enb_cc_idx = i;
  }

  return sim_args;
}

struct test_scell_activation_params {
  uint32_t pcell_idx = 0;
};

int test_scell_activation(uint32_t sim_number, test_scell_activation_params params)
{
  /* Simulation Configuration Arguments */
  uint32_t nof_prb   = srsran::lte_cell_nof_prbs[std::uniform_int_distribution<uint32_t>{0, 5}(get_rand_gen())];
  uint32_t nof_ccs   = 2;
  uint32_t start_tti = 0; // rand_int(0, 10240);

  /* Internal configurations. Do not touch */
  float          ul_sr_exps[]   = {1, 4}; // log rand
  float          dl_data_exps[] = {1, 4}; // log rand
  float          P_ul_sr = randf() * 0.5, P_dl = randf() * 0.5;
  const uint16_t rnti1 = 70;

  /* Setup Simulation */
  uint32_t prach_tti = 1;
  uint32_t msg4_size = 40; // TODO: Check
  uint32_t duration  = 1000;
  // Generate Cell order
  std::vector<uint32_t> cc_idxs(nof_ccs);
  std::iota(cc_idxs.begin(), cc_idxs.end(), 0);
  std::shuffle(cc_idxs.begin(), cc_idxs.end(), get_rand_gen());
  std::iter_swap(cc_idxs.begin(), std::find(cc_idxs.begin(), cc_idxs.end(), params.pcell_idx));

  /* Setup simulation arguments struct */
  sim_sched_args sim_args = generate_default_sim_args(nof_prb, nof_ccs);
  sim_args.start_tti      = start_tti;
  sim_args.default_ue_sim_cfg.ue_cfg.supported_cc_list.resize(1);
  sim_args.default_ue_sim_cfg.ue_cfg.supported_cc_list[0].active                                = true;
  sim_args.default_ue_sim_cfg.ue_cfg.supported_cc_list[0].enb_cc_idx                            = cc_idxs[0];
  sim_args.default_ue_sim_cfg.ue_cfg.supported_cc_list[0].dl_cfg.cqi_report.periodic_configured = true;
  sim_args.default_ue_sim_cfg.ue_cfg.supported_cc_list[0].dl_cfg.cqi_report.pmi_idx             = 37;

  /* Simulation Objects Setup */
  sched_sim_event_generator generator;
  // Setup scheduler
  common_sched_tester tester;
  tester.sim_cfg(sim_args);

  /* Simulation */

  // Event PRACH: PRACH takes place for "rnti1", and carrier "pcell_idx"
  generator.step_until(prach_tti);
  tti_ev::user_cfg_ev* user = generator.add_new_default_user(duration, sim_args.default_ue_sim_cfg);
  user->rnti                = rnti1;
  tester.test_next_ttis(generator.tti_events);
  TESTASSERT(tester.sched_sim->user_exists(rnti1));

  // Event (TTI=prach_tti+msg4_tot_delay): First Tx (Msg4). Goes in SRB0 and contains ConRes
  while (not tester.sched_sim->find_rnti(rnti1)->get_ctxt().msg3_tti_rx.is_valid() or
         srsenb::to_tx_ul(tester.sched_sim->find_rnti(rnti1)->get_ctxt().msg3_tti_rx).to_uint() >
             generator.tti_counter) {
    generator.step_tti();
    tester.test_next_ttis(generator.tti_events);
  }
  generator.step_tti();
  generator.add_dl_data(rnti1, msg4_size);
  tester.test_next_ttis(generator.tti_events);
  while (not tester.sched_sim->find_rnti(rnti1)->get_ctxt().conres_rx) {
    generator.step_tti();
    tester.test_next_ttis(generator.tti_events);
  }

  // Event (20 TTIs): Data back and forth
  auto generate_data = [&](uint32_t nof_ttis, float prob_dl, float prob_ul, float rand_exp) {
    for (uint32_t i = 0; i < nof_ttis; ++i) {
      generator.step_tti();
      bool ul_flag = randf() < prob_ul, dl_flag = randf() < prob_dl;
      if (dl_flag) {
        float exp = dl_data_exps[0] + rand_exp * (dl_data_exps[1] - dl_data_exps[0]);
        generator.add_dl_data(rnti1, pow(10, exp));
      }
      if (ul_flag) {
        float exp = ul_sr_exps[0] + rand_exp * (ul_sr_exps[1] - ul_sr_exps[0]);
        generator.add_ul_data(rnti1, pow(10, exp));
      }
    }
  };
  generate_data(20, 1.0, P_ul_sr, randf());
  TESTASSERT(tester.test_next_ttis(generator.tti_events) == SRSRAN_SUCCESS);

  // Event: Reconf Complete. Activate SCells. Check if CE correctly transmitted
  generator.step_tti();
  user = generator.user_reconf(rnti1);
  user->ue_sim_cfg->ue_cfg =
      *tester.get_current_ue_cfg(rnti1); // use current cfg as starting point, and add more supported ccs
  user->ue_sim_cfg->ue_cfg.supported_cc_list.resize(nof_ccs);
  for (uint32_t i = 0; i < user->ue_sim_cfg->ue_cfg.supported_cc_list.size(); ++i) {
    user->ue_sim_cfg->ue_cfg.supported_cc_list[i].active     = true;
    user->ue_sim_cfg->ue_cfg.supported_cc_list[i].enb_cc_idx = cc_idxs[i];
  }
  TESTASSERT(tester.test_next_ttis(generator.tti_events) == SRSRAN_SUCCESS);
  auto activ_list = tester.get_enb_ue_cc_map(rnti1);
  for (uint32_t i = 0; i < cc_idxs.size(); ++i) {
    TESTASSERT(activ_list[i] >= 0);
  }

  // TEST: When a DL newtx takes place, it should also encode the CE
  for (uint32_t i = 0; i < 100; ++i) {
    if (not tester.tti_info.dl_sched_result[params.pcell_idx].data.empty()) {
      // DL data was allocated
      if (tester.tti_info.dl_sched_result[params.pcell_idx].data[0].nof_pdu_elems[0] > 0) {
        // it is a new DL tx
        TESTASSERT(tester.tti_info.dl_sched_result[params.pcell_idx].data[0].pdu[0][0].lcid ==
                   (uint32_t)srsran::dl_sch_lcid::SCELL_ACTIVATION);
        break;
      }
    }
    generator.step_tti();
    TESTASSERT(tester.test_next_ttis(generator.tti_events) == SRSRAN_SUCCESS);
  }

  // Event: Wait for UE to receive and ack CE. Send cqi==0, which should not activate the SCell
  uint32_t cqi = 0;
  for (uint32_t cidx = 1; cidx < cc_idxs.size(); ++cidx) {
    for (uint32_t i = 0; i < FDD_HARQ_DELAY_UL_MS; ++i) {
      tester.dl_cqi_info(tester.tti_rx.to_uint(), rnti1, cc_idxs[cidx], cqi);
      generator.step_tti();
    }
  }
  TESTASSERT(tester.test_next_ttis(generator.tti_events) == SRSRAN_SUCCESS);
  // The UE should now have received the CE

  // Event: Generate a bit more data, it should *not* go through SCells until we send a CQI
  generate_data(5, P_dl, P_ul_sr, randf());
  TESTASSERT(tester.test_next_ttis(generator.tti_events) == SRSRAN_SUCCESS);
  TESTASSERT(tester.sched_stats->users[rnti1].tot_dl_sched_data[params.pcell_idx] > 0);
  TESTASSERT(tester.sched_stats->users[rnti1].tot_ul_sched_data[params.pcell_idx] > 0);
  for (uint32_t i = 1; i < cc_idxs.size(); ++i) {
    TESTASSERT(tester.sched_stats->users[rnti1].tot_dl_sched_data[cc_idxs[i]] == 0);
    TESTASSERT(tester.sched_stats->users[rnti1].tot_ul_sched_data[cc_idxs[i]] == 0);
  }

  // Event: Scheduler receives dl_cqi for SCell. Data should go through SCells
  cqi = 14;
  for (uint32_t i = 1; i < cc_idxs.size(); ++i) {
    tester.dl_cqi_info(tester.tti_rx.to_uint(), rnti1, cc_idxs[i], cqi);
  }
  generate_data(10, 1.0, 1.0, 1.0);
  TESTASSERT(tester.test_next_ttis(generator.tti_events) == SRSRAN_SUCCESS);
  uint64_t tot_dl_sched_data = 0;
  uint64_t tot_ul_sched_data = 0;
  for (const auto& c : cc_idxs) {
    tot_dl_sched_data += tester.sched_stats->users[rnti1].tot_dl_sched_data[c];
    tot_ul_sched_data += tester.sched_stats->users[rnti1].tot_ul_sched_data[c];
  }

  TESTASSERT(tot_dl_sched_data > 0);
  TESTASSERT(tot_ul_sched_data > 0);

  srslog::flush();
  printf("[TESTER] Sim%d finished successfully\n\n", sim_number);
  return SRSRAN_SUCCESS;
}

int main()
{
  // Setup rand seed
  set_randseed(seed);

  // Setup the log spy to intercept error and warning log entries.
  if (!srslog::install_custom_sink(
          srsran::log_sink_spy::name(),
          std::unique_ptr<srsran::log_sink_spy>(new srsran::log_sink_spy(srslog::get_default_log_formatter())))) {
    return SRSRAN_ERROR;
  }

  auto* spy = static_cast<srsran::log_sink_spy*>(srslog::find_sink(srsran::log_sink_spy::name()));
  if (!spy) {
    return SRSRAN_ERROR;
  }

  auto& mac_log = srslog::fetch_basic_logger("MAC");
  mac_log.set_level(srslog::basic_levels::debug);
  auto& test_log = srslog::fetch_basic_logger("TEST", *spy, false);
  test_log.set_level(srslog::basic_levels::debug);

  // Start the log backend.
  srslog::init();

  sched_diagnostic_printer printer(*spy);

  printf("[TESTER] This is the chosen seed: %u\n", seed);
  uint32_t N_runs = 20;
  for (uint32_t n = 0; n < N_runs; ++n) {
    printf("[TESTER] Sim run number: %u\n", n);

    test_scell_activation_params p = {};
    p.pcell_idx                    = 0;
    TESTASSERT(test_scell_activation(n * 2, p) == SRSRAN_SUCCESS);

    p           = {};
    p.pcell_idx = 1;
    TESTASSERT(test_scell_activation(n * 2 + 1, p) == SRSRAN_SUCCESS);
  }

  srslog::flush();

  return 0;
}
