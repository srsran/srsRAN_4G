/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsenb/hdr/stack/mac/sched.h"
#include "srsenb/hdr/stack/mac/sched_carrier.h"
#include "srsenb/hdr/stack/mac/sched_ue.h"
#include <algorithm>
#include <chrono>
#include <random>
#include <set>
#include <unistd.h>

#include "srsran/interfaces/sched_interface.h"
#include "srsran/phy/utils/debug.h"

#include "sched_common_test_suite.h"
#include "sched_test_common.h"
#include "sched_test_utils.h"
#include "srsran/common/common_lte.h"
#include "srsran/common/test_common.h"

namespace srsenb {

uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();

struct ue_stats_t {
  uint32_t nof_dl_rbs   = 0;
  uint32_t nof_ul_rbs   = 0;
  uint64_t nof_dl_bytes = 0;
  uint64_t nof_ul_bytes = 0;
  uint32_t nof_ttis     = 0;
};
std::map<uint16_t, ue_stats_t> ue_stats;
ue_stats_t                     ue_tot_stats;

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
    logger.info("UE stats:");
    logger.info("all: {DL/UL RBs: %" PRIu32 "/%" PRIu32 ", DL/UL bitrates: %0.2f/%0.2f Mbps}",
                ue_tot_stats.nof_dl_rbs,
                ue_tot_stats.nof_ul_rbs,
                ue_tot_stats.nof_dl_bytes * 8 * 0.001 / ue_tot_stats.nof_ttis,
                ue_tot_stats.nof_ul_bytes * 8 * 0.001 / ue_tot_stats.nof_ttis);
    for (const auto& e : ue_stats) {
      logger.info("0x%x: {DL/UL RBs: %" PRIu32 "/%" PRIu32 ", DL/UL bitrates: %0.2f/%0.2f Mbps}",
                  e.first,
                  e.second.nof_dl_rbs,
                  e.second.nof_ul_rbs,
                  e.second.nof_dl_bytes * 8 * 0.001 / e.second.nof_ttis,
                  e.second.nof_ul_bytes * 8 * 0.001 / e.second.nof_ttis);
    }
    logger.info("Number of assertion warnings: %u", s.get_warning_counter());
    logger.info("Number of assertion errors: %u", s.get_error_counter());
    logger.info("This was the seed: %u", seed);
    srslog::flush();
  }

private:
  srsran::log_sink_spy& s;
};

/*******************
 *     Dummies     *
 *******************/

constexpr uint32_t CARRIER_IDX = 0;

// Designed for testing purposes
struct sched_tester : public srsenb::common_sched_tester {
  struct tester_user_results {
    srsenb::ul_harq_proc ul_harq;
  };
  struct sched_tti_data {
    std::map<uint16_t, tester_user_results> ue_data;   ///< stores buffer state of each user
    tester_user_results                     total_ues; ///< stores combined UL/DL buffer state
  };

  // sched results
  sched_tti_data tti_data;

  int rem_user(uint16_t rnti) override;
  int test_harqs();

private:
  void new_test_tti() override;
  void before_sched() override;
  int  process_results() override;
  int  update_ue_stats();
};

int sched_tester::rem_user(uint16_t rnti)
{
  tti_data.ue_data.erase(rnti);
  return common_sched_tester::rem_user(rnti);
}

void sched_tester::new_test_tti()
{
  common_sched_tester::new_test_tti();
  // NOTE: make a local copy, since some of these variables may be cleared during scheduling
  tti_data.ue_data.clear();
  tti_data.total_ues = tester_user_results();
}

void sched_tester::before_sched()
{
  // check pending data buffers
  for (auto& it : ue_db) {
    uint16_t            rnti = it.first;
    srsenb::sched_ue*   user = it.second.get();
    tester_user_results d;
    tti_data.ue_data.insert(std::make_pair(rnti, d));

    // NOTE: ACK might have just cleared the harq for tti_info.tti_params.tti_tx_ul
    tti_data.ue_data[rnti].ul_harq = *user->get_ul_harq(srsenb::to_tx_ul(tti_rx), CARRIER_IDX);
  }
}

int sched_tester::process_results()
{
  const srsenb::cc_sched_result* cc_result = sched_results.get_cc(tti_rx, CARRIER_IDX);
  srsenb::sf_output_res_t        sf_out{sched_cell_params, tti_rx, tti_info.ul_sched_result, tti_info.dl_sched_result};

  // Common tests
  TESTASSERT(test_pdcch_collisions(sf_out, CARRIER_IDX, &cc_result->pdcch_mask) == SRSRAN_SUCCESS);
  TESTASSERT(test_dci_content_common(sf_out, CARRIER_IDX) == SRSRAN_SUCCESS);
  TESTASSERT(test_sib_scheduling(sf_out, CARRIER_IDX) == SRSRAN_SUCCESS);
  TESTASSERT(test_pusch_collisions(sf_out, CARRIER_IDX, &cc_result->ul_mask) == SRSRAN_SUCCESS);
  TESTASSERT(test_pdsch_collisions(sf_out, CARRIER_IDX, &cc_result->dl_mask) == SRSRAN_SUCCESS);

  // UE dedicated tests
  TESTASSERT(run_ue_ded_tests_and_update_ctxt(sf_out) == SRSRAN_SUCCESS);
  TESTASSERT(test_harqs() == SRSRAN_SUCCESS);
  TESTASSERT(update_ue_stats() == SRSRAN_SUCCESS);

  return SRSRAN_SUCCESS;
}

int sched_tester::test_harqs()
{
  /* check consistency of DL harq procedures and allocations */
  for (uint32_t i = 0; i < tti_info.dl_sched_result[CARRIER_IDX].data.size(); ++i) {
    const auto&                 data = tti_info.dl_sched_result[CARRIER_IDX].data[i];
    uint32_t                    h_id = data.dci.pid;
    uint16_t                    rnti = data.dci.rnti;
    const srsenb::dl_harq_proc& h    = ue_db[rnti]->get_dl_harq(h_id, CARRIER_IDX);
    CONDERROR(h.get_tti() != srsenb::to_tx_dl(tti_rx),
              "The scheduled DL harq pid=%d does not a valid tti=%u",
              h_id,
              srsenb::to_tx_dl(tti_rx).to_uint());
    CONDERROR(h.get_n_cce() != data.dci.location.ncce, "Harq DCI location does not match with result");
  }

  /* Check PHICH allocations */
  for (uint32_t i = 0; i < tti_info.ul_sched_result[CARRIER_IDX].phich.size(); ++i) {
    const auto& phich = tti_info.ul_sched_result[CARRIER_IDX].phich[i];
    const auto& hprev = tti_data.ue_data[phich.rnti].ul_harq;
    const auto* h     = ue_db[phich.rnti]->get_ul_harq(srsenb::to_tx_ul(tti_rx), CARRIER_IDX);
    CONDERROR(not hprev.has_pending_phich(), "Alloc PHICH did not have any pending ack");
    bool maxretx_flag = hprev.nof_retx(0) + 1 >= hprev.max_nof_retx();
    if (phich.phich == sched_interface::ul_sched_phich_t::ACK) {
      // The harq can be either ACKed or Resumed
      if (not hprev.is_empty()) {
        // In case it was resumed
        CONDERROR(h == nullptr or h->is_empty(), "Cannot resume empty UL harq");
        for (uint32_t j = 0; j < tti_info.ul_sched_result[CARRIER_IDX].pusch.size(); ++j) {
          auto& pusch = tti_info.ul_sched_result[CARRIER_IDX].pusch[j];
          CONDERROR(pusch.dci.rnti == phich.rnti, "Cannot send PHICH::ACK for same harq that got UL grant.");
        }
      }
    } else {
      CONDERROR(h->get_pending_data() == 0 and !maxretx_flag, "NACKed harq has no pending data");
    }
  }

  return SRSRAN_SUCCESS;
}

int sched_tester::update_ue_stats()
{
  // update ue stats with number of allocated UL PRBs
  for (uint32_t i = 0; i < tti_info.ul_sched_result[CARRIER_IDX].pusch.size(); ++i) {
    const auto& pusch = tti_info.ul_sched_result[CARRIER_IDX].pusch[i];
    uint32_t    L, RBstart;
    srsran_ra_type2_from_riv(pusch.dci.type2_alloc.riv,
                             &L,
                             &RBstart,
                             sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb,
                             sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb);
    ue_stats[pusch.dci.rnti].nof_ul_rbs += L;
    ue_stats[pusch.dci.rnti].nof_ul_bytes += pusch.tbs;
    ue_tot_stats.nof_ul_rbs += L;
    ue_tot_stats.nof_ul_bytes += pusch.tbs;
  }

  // update ue stats with number of DL RB allocations
  srsran::bounded_bitset<100, true> alloc_mask(sched_cell_params[CARRIER_IDX].cfg.cell.nof_prb);
  for (uint32_t i = 0; i < tti_info.dl_sched_result[CARRIER_IDX].data.size(); ++i) {
    auto& data = tti_info.dl_sched_result[CARRIER_IDX].data[i];
    TESTASSERT(srsenb::extract_dl_prbmask(sched_cell_params[CARRIER_IDX].cfg.cell,
                                          tti_info.dl_sched_result[CARRIER_IDX].data[i].dci,
                                          alloc_mask) == SRSRAN_SUCCESS);
    ue_stats[data.dci.rnti].nof_dl_rbs += alloc_mask.count();
    ue_stats[data.dci.rnti].nof_dl_bytes += data.tbs[0] + data.tbs[1];
    ue_tot_stats.nof_dl_rbs += alloc_mask.count();
    ue_tot_stats.nof_dl_bytes += data.tbs[0] + data.tbs[1];
  }

  for (auto& u : ue_db) {
    ue_stats[u.first].nof_ttis++;
  }
  ue_tot_stats.nof_ttis++;

  return SRSRAN_SUCCESS;
}

int test_scheduler_rand(srsenb::sched_sim_events sim)
{
  // Create classes
  sched_tester  tester;
  srsenb::sched my_sched;

  tester.sim_cfg(std::move(sim.sim_args));

  TESTASSERT(tester.test_next_ttis(sim.tti_events) == SRSRAN_SUCCESS);
  return SRSRAN_SUCCESS;
}

template <typename T>
T pick_random_uniform(std::initializer_list<T> v)
{
  return *(v.begin() + std::uniform_int_distribution<size_t>{0, v.size() - 1}(srsenb::get_rand_gen()));
}

sched_sim_events rand_sim_params(uint32_t nof_ttis)
{
  auto             boolean_dist = []() { return std::uniform_int_distribution<>{0, 1}(srsenb::get_rand_gen()); };
  sched_sim_events sim_gen;
  uint32_t         max_conn_dur = 10000, min_conn_dur = 500;
  float            P_ul_sr = srsenb::randf() * 0.5, P_dl = srsenb::randf() * 0.5;
  float            P_prach        = 0.99f;  // 0.1f + randf()*0.3f;
  float            ul_sr_exps[]   = {1, 4}; // log rand
  float            dl_data_exps[] = {1, 4}; // log rand
  uint32_t         max_nof_users  = 5;
  std::uniform_int_distribution<>         connection_dur_dist(min_conn_dur, max_conn_dur);
  std::uniform_int_distribution<uint32_t> dist_prb_idx(0, 5);
  uint32_t                                prb_idx = dist_prb_idx(srsenb::get_rand_gen());
  uint32_t                                nof_prb = srsran::lte_cell_nof_prbs[prb_idx];
  printf("Number of PRBs is %u\n", nof_prb);

  sched_sim_event_generator generator;

  sim_gen.sim_args.cell_cfg                             = {generate_default_cell_cfg(nof_prb)};
  sim_gen.sim_args.cell_cfg[0].target_pucch_ul_sinr     = pick_random_uniform({10, 15, 20, -1});
  sim_gen.sim_args.cell_cfg[0].target_pusch_ul_sinr     = pick_random_uniform({10, 15, 20, -1});
  sim_gen.sim_args.cell_cfg[0].enable_phr_handling      = false;
  sim_gen.sim_args.default_ue_sim_cfg.ue_cfg            = generate_default_ue_cfg();
  sim_gen.sim_args.default_ue_sim_cfg.periodic_cqi      = true;
  sim_gen.sim_args.default_ue_sim_cfg.ue_cfg.maxharq_tx = std::uniform_int_distribution<>{1, 5}(srsenb::get_rand_gen());
  sim_gen.sim_args.default_ue_sim_cfg.prob_dl_ack_mask.resize(sim_gen.sim_args.default_ue_sim_cfg.ue_cfg.maxharq_tx,
                                                              0.5);
  sim_gen.sim_args.default_ue_sim_cfg.prob_dl_ack_mask.back() = 1;
  sim_gen.sim_args.default_ue_sim_cfg.prob_ul_ack_mask.resize(sim_gen.sim_args.default_ue_sim_cfg.ue_cfg.maxharq_tx,
                                                              0.5);
  sim_gen.sim_args.default_ue_sim_cfg.prob_ul_ack_mask.back() = 1;
  sim_gen.sim_args.default_ue_sim_cfg.ue_cfg.measgap_period   = pick_random_uniform({0, 40, 80});
  sim_gen.sim_args.default_ue_sim_cfg.ue_cfg.measgap_offset   = std::uniform_int_distribution<uint32_t>{
      0, sim_gen.sim_args.default_ue_sim_cfg.ue_cfg.measgap_period}(srsenb::get_rand_gen());
  sim_gen.sim_args.default_ue_sim_cfg.ue_cfg.pucch_cfg.n_pucch_sr =
      std::uniform_int_distribution<uint32_t>{0, 2047}(srsenb::get_rand_gen());

  sim_gen.sim_args.start_tti = 0;
  sim_gen.sim_args.sched_args.pdsch_mcs =
      boolean_dist() ? -1 : std::uniform_int_distribution<>{0, 24}(srsenb::get_rand_gen());
  sim_gen.sim_args.sched_args.pusch_mcs =
      boolean_dist() ? -1 : std::uniform_int_distribution<>{0, 24}(srsenb::get_rand_gen());

  generator.tti_events.resize(nof_ttis);

  for (uint32_t tti = 0; tti < nof_ttis; ++tti) {
    for (auto& u : generator.current_users) {
      uint32_t rnti = u.first;
      if (srsenb::randf() < P_ul_sr) {
        float exp = ul_sr_exps[0] + srsenb::randf() * (ul_sr_exps[1] - ul_sr_exps[0]);
        generator.add_ul_data(rnti, (uint32_t)pow(10, exp));
      }
      if (srsenb::randf() < P_dl) {
        float exp = dl_data_exps[0] + srsenb::randf() * (dl_data_exps[1] - dl_data_exps[0]);
        generator.add_dl_data(rnti, (uint32_t)pow(10, exp));
      }
    }

    // may add new user (For now, we only support one UE per PRACH)
    bool is_prach_tti =
        srsran_prach_tti_opportunity_config_fdd(sim_gen.sim_args.cell_cfg[CARRIER_IDX].prach_config, tti, -1);
    if (is_prach_tti and generator.current_users.size() < max_nof_users and srsenb::randf() < P_prach) {
      generator.add_new_default_user(connection_dur_dist(srsenb::get_rand_gen()), sim_gen.sim_args.default_ue_sim_cfg);
    }
    generator.step_tti();
  }

  sim_gen.tti_events = std::move(generator.tti_events);

  return sim_gen;
}

} // namespace srsenb

int main()
{
  // Setup seed
  srsenb::set_randseed(srsenb::seed);
  printf("This is the chosen seed: %u\n", srsenb::seed);

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
  mac_log.set_level(srslog::basic_levels::info);
  auto& test_log = srslog::fetch_basic_logger("TEST", *spy, false);
  test_log.set_level(srslog::basic_levels::info);

  // Start the log backend.
  srslog::init();

  uint32_t N_runs = 1, nof_ttis = 10240 + 10;

  srsenb::sched_diagnostic_printer printer(*spy);
  for (uint32_t n = 0; n < N_runs; ++n) {
    printf("Sim run number: %u\n", n + 1);
    srsenb::sched_sim_events sim = srsenb::rand_sim_params(nof_ttis);
    TESTASSERT(srsenb::test_scheduler_rand(std::move(sim)) == SRSRAN_SUCCESS);
  }

  return 0;
}
