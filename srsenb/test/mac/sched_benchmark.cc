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
#include "srsenb/hdr/stack/mac/sched.h"
#include "srsran/adt/accumulators.h"
#include "srsran/common/common_lte.h"
#include <chrono>

namespace srsenb {

struct run_params {
  uint32_t    nof_prbs;
  uint32_t    nof_ues;
  uint32_t    nof_ttis;
  uint32_t    cqi;
  const char* sched_policy;
};

struct run_params_range {
  std::vector<uint32_t>    nof_prbs{srsran::lte_cell_nof_prbs.begin(), srsran::lte_cell_nof_prbs.end()};
  std::vector<uint32_t>    nof_ues      = {1, 2, 5, 32};
  uint32_t                 nof_ttis     = 10000;
  std::vector<uint32_t>    cqi          = {5, 10, 15};
  std::vector<const char*> sched_policy = {"time_rr", "time_pf"};

  size_t     nof_runs() const { return nof_prbs.size() * nof_ues.size() * cqi.size() * sched_policy.size(); }
  run_params get_params(size_t idx) const
  {
    run_params r = {};
    r.nof_ttis   = nof_ttis;
    r.nof_prbs   = nof_prbs[idx % nof_prbs.size()];
    idx /= nof_prbs.size();
    r.nof_ues = nof_ues[idx % nof_ues.size()];
    idx /= nof_ues.size();
    r.cqi = cqi[idx % cqi.size()];
    idx /= cqi.size();
    r.sched_policy = sched_policy.at(idx);
    return r;
  }
};

class sched_tester : public sched_sim_base
{
  static std::vector<sched_interface::cell_cfg_t> get_cell_cfg(srsran::span<const sched_cell_params_t> cell_params)
  {
    std::vector<sched_interface::cell_cfg_t> cell_cfg_list;
    for (const auto& c : cell_params) {
      cell_cfg_list.push_back(c.cfg);
    }
    return cell_cfg_list;
  }

public:
  explicit sched_tester(sched*                                          sched_obj_,
                        const sched_interface::sched_args_t&            sched_args,
                        const std::vector<sched_interface::cell_cfg_t>& cell_cfg_list) :
    sched_sim_base(sched_obj_, sched_args, cell_cfg_list),
    sched_ptr(sched_obj_),
    dl_result(cell_cfg_list.size()),
    ul_result(cell_cfg_list.size())
  {}

  srslog::basic_logger& mac_logger = srslog::fetch_basic_logger("MAC");
  sched*                sched_ptr;
  uint32_t              dl_bytes_per_tti   = 100000;
  uint32_t              ul_bytes_per_tti   = 100000;
  run_params            current_run_params = {};

  std::vector<sched_interface::dl_sched_res_t> dl_result;
  std::vector<sched_interface::ul_sched_res_t> ul_result;

  struct throughput_stats {
    srsran::rolling_average<float>  mean_dl_tbs, mean_ul_tbs, avg_dl_mcs, avg_ul_mcs;
    srsran::rolling_average<double> avg_latency;
    std::vector<uint32_t>           latency_samples;
  };
  throughput_stats total_stats;

  int advance_tti()
  {
    tti_point tti_rx = get_tti_rx().is_valid() ? get_tti_rx() + 1 : tti_point(0);
    mac_logger.set_context(tti_rx.to_uint());
    new_tti(tti_rx);

    for (uint32_t cc = 0; cc < get_cell_params().size(); ++cc) {
      std::chrono::time_point<std::chrono::steady_clock> tp = std::chrono::steady_clock::now();
      TESTASSERT(sched_ptr->dl_sched(to_tx_dl(tti_rx).to_uint(), cc, dl_result[cc]) == SRSRAN_SUCCESS);
      TESTASSERT(sched_ptr->ul_sched(to_tx_ul(tti_rx).to_uint(), cc, ul_result[cc]) == SRSRAN_SUCCESS);
      std::chrono::time_point<std::chrono::steady_clock> tp2 = std::chrono::steady_clock::now();
      std::chrono::nanoseconds tdur = std::chrono::duration_cast<std::chrono::nanoseconds>(tp2 - tp);
      total_stats.avg_latency.push(tdur.count());
      total_stats.latency_samples.push_back(tdur.count());
    }

    sf_output_res_t sf_out{get_cell_params(), tti_rx, ul_result, dl_result};
    update(sf_out);
    process_stats(sf_out);

    return SRSRAN_SUCCESS;
  }

  void set_external_tti_events(const sim_ue_ctxt_t& ue_ctxt, ue_tti_events& pending_events) override
  {
    // do nothing
    if (ue_ctxt.conres_rx) {
      sched_ptr->ul_bsr(ue_ctxt.rnti, 1, dl_bytes_per_tti);
      sched_ptr->dl_rlc_buffer_state(ue_ctxt.rnti, 3, ul_bytes_per_tti, 0);

      if (get_tti_rx().to_uint() % 5 == 0) {
        for (auto& cc : pending_events.cc_list) {
          cc.dl_cqi = current_run_params.cqi;
          cc.ul_snr = 40;
        }
      }
    }
  }

  void process_stats(sf_output_res_t& sf_out)
  {
    for (uint32_t cc = 0; cc < get_cell_params().size(); ++cc) {
      uint32_t dl_tbs = 0, ul_tbs = 0, dl_mcs = 0, ul_mcs = 0;
      for (const auto& data : sf_out.dl_cc_result[cc].data) {
        dl_tbs += data.tbs[0];
        dl_tbs += data.tbs[1];
        dl_mcs = std::max(dl_mcs, data.dci.tb[0].mcs_idx);
      }
      total_stats.mean_dl_tbs.push(dl_tbs);
      if (not sf_out.dl_cc_result[cc].data.empty()) {
        total_stats.avg_dl_mcs.push(dl_mcs);
      }
      for (const auto& pusch : sf_out.ul_cc_result[cc].pusch) {
        ul_tbs += pusch.tbs;
        ul_mcs = std::max(ul_mcs, pusch.dci.tb.mcs_idx);
      }
      total_stats.mean_ul_tbs.push(ul_tbs);
      if (not sf_out.ul_cc_result[cc].pusch.empty()) {
        total_stats.avg_ul_mcs.push(ul_mcs);
      }
    }
  }
};

struct run_data {
  run_params                params;
  float                     avg_dl_throughput;
  float                     avg_ul_throughput;
  float                     avg_dl_mcs;
  float                     avg_ul_mcs;
  std::chrono::microseconds avg_latency;
  std::chrono::microseconds q0_9_latency;
};

int run_benchmark_scenario(run_params params, std::vector<run_data>& run_results)
{
  std::vector<sched_interface::cell_cfg_t> cell_list(1, generate_default_cell_cfg(params.nof_prbs));
  sched_interface::ue_cfg_t                ue_cfg_default = generate_default_ue_cfg();
  sched_interface::sched_args_t            sched_args     = {};
  sched_args.sched_policy                                 = params.sched_policy;

  sched     sched_obj;
  rrc_dummy rrc{};
  sched_obj.init(&rrc, sched_args);
  sched_tester tester(&sched_obj, sched_args, cell_list);

  tester.total_stats        = {};
  tester.current_run_params = params;

  for (uint32_t ue_idx = 0; ue_idx < params.nof_ues; ++ue_idx) {
    uint16_t rnti = 0x46 + ue_idx;
    // Add user (first need to advance to a PRACH TTI)
    while (not srsran_prach_tti_opportunity_config_fdd(
        tester.get_cell_params()[ue_cfg_default.supported_cc_list[0].enb_cc_idx].cfg.prach_config,
        tester.get_tti_rx().to_uint(),
        -1)) {
      TESTASSERT(tester.advance_tti() == SRSRAN_SUCCESS);
    }
    TESTASSERT(tester.add_user(rnti, ue_cfg_default, 16) == SRSRAN_SUCCESS);
    TESTASSERT(tester.advance_tti() == SRSRAN_SUCCESS);
  }

  // Ignore stats of the first TTIs until all UEs DRB1 are created
  auto ue_db_ctxt = tester.get_enb_ctxt().ue_db;
  while (not std::all_of(ue_db_ctxt.begin(), ue_db_ctxt.end(), [](std::pair<uint16_t, const sim_ue_ctxt_t*> p) {
    return p.second->conres_rx;
  })) {
    tester.advance_tti();
    ue_db_ctxt = tester.get_enb_ctxt().ue_db;
  }

  // Run benchmark
  tester.total_stats = {};
  tester.total_stats.latency_samples.reserve(params.nof_ttis);
  for (uint32_t count = 0; count < params.nof_ttis; ++count) {
    tester.advance_tti();
  }
  std::sort(tester.total_stats.latency_samples.begin(), tester.total_stats.latency_samples.end());

  run_data run_result          = {};
  run_result.params            = params;
  run_result.avg_dl_throughput = tester.total_stats.mean_dl_tbs.value() * 8.0F / 1e-3F;
  run_result.avg_ul_throughput = tester.total_stats.mean_ul_tbs.value() * 8.0F / 1e-3F;
  run_result.avg_dl_mcs        = tester.total_stats.avg_dl_mcs.value();
  run_result.avg_ul_mcs        = tester.total_stats.avg_ul_mcs.value();
  run_result.avg_latency  = std::chrono::microseconds(static_cast<int>(tester.total_stats.avg_latency.value() / 1000));
  run_result.q0_9_latency = std::chrono::microseconds(
      tester.total_stats.latency_samples[static_cast<size_t>(tester.total_stats.latency_samples.size() * 0.9)] / 1000);
  run_results.push_back(run_result);

  return SRSRAN_SUCCESS;
}

run_data expected_run_result(run_params params)
{
  assert(params.cqi == 15 && "only cqi=15 supported for now");
  run_data ret{};
  int      tbs_idx      = srsran_ra_tbs_idx_from_mcs(28, false, false);
  int      tbs          = srsran_ra_tbs_from_idx(tbs_idx, params.nof_prbs);
  ret.avg_dl_throughput = static_cast<float>(tbs) * 1e3F; // bps

  tbs_idx                 = srsran_ra_tbs_idx_from_mcs(24, false, true);
  uint32_t nof_pusch_prbs = params.nof_prbs - (params.nof_prbs == 6 ? 2 : 4);
  tbs                     = srsran_ra_tbs_from_idx(tbs_idx, nof_pusch_prbs);
  ret.avg_ul_throughput   = static_cast<float>(tbs) * 1e3F; // bps

  ret.avg_dl_mcs = 27;
  ret.avg_ul_mcs = 22;
  switch (params.nof_prbs) {
    case 6:
      ret.avg_dl_mcs = 25;
      ret.avg_dl_throughput *= 0.68;
      ret.avg_ul_throughput *= 0.75;
      break;
    case 15:
      ret.avg_dl_throughput *= 0.94;
      ret.avg_ul_throughput *= 0.7;
      break;
    default:
      ret.avg_dl_throughput *= 0.96;
      ret.avg_ul_throughput *= 0.84;
      break;
  }
  return ret;
}

void print_benchmark_results(const std::vector<run_data>& run_results)
{
  srslog::flush();
  fmt::print("run | Nprb | cqi | sched pol | Nue | DL/UL [Mbps] | DL/UL mcs | DL/UL OH [%] | latency | latency q0.9 "
             "[usec]\n");
  fmt::print("------------------------------------------------------------------------------------------------------"
             "------\n");
  for (uint32_t i = 0; i < run_results.size(); ++i) {
    const run_data& r = run_results[i];

    int   tbs_idx           = srsran_ra_tbs_idx_from_mcs(28, false, false);
    int   tbs               = srsran_ra_tbs_from_idx(tbs_idx, r.params.nof_prbs);
    float dl_rate_overhead  = 1.0F - r.avg_dl_throughput / (static_cast<float>(tbs) * 1e3F);
    tbs_idx                 = srsran_ra_tbs_idx_from_mcs(24, false, true);
    uint32_t nof_pusch_prbs = r.params.nof_prbs - (r.params.nof_prbs == 6 ? 2 : 4);
    tbs                     = srsran_ra_tbs_from_idx(tbs_idx, nof_pusch_prbs);
    float ul_rate_overhead  = 1.0F - r.avg_ul_throughput / (static_cast<float>(tbs) * 1e3F);

    fmt::print("{:>3d}{:>6d}{:>6d}{:>12}{:>6d}{:>9.2}/{:>4.2}{:>9.1f}/{:>4.1f}{:9.1f}/{:>4.1f}{:>9d}{:12d}\n",
               i,
               r.params.nof_prbs,
               r.params.cqi,
               r.params.sched_policy,
               r.params.nof_ues,
               r.avg_dl_throughput / 1e6,
               r.avg_ul_throughput / 1e6,
               r.avg_dl_mcs,
               r.avg_ul_mcs,
               dl_rate_overhead * 100,
               ul_rate_overhead * 100,
               r.avg_latency.count(),
               r.q0_9_latency.count());
  }
}

int run_rate_test()
{
  fmt::print("\n====== Scheduler Rate Test ======\n\n");
  run_params_range      run_param_list{};
  srslog::basic_logger& mac_logger = srslog::fetch_basic_logger("MAC");

  run_param_list.nof_ues = {1};
  run_param_list.cqi     = {15};

  std::vector<run_data> run_results;
  size_t                nof_runs = run_param_list.nof_runs();

  for (size_t r = 0; r < nof_runs; ++r) {
    run_params runparams = run_param_list.get_params(r);

    mac_logger.info("\n=== New run {} ===\n", r);
    TESTASSERT(run_benchmark_scenario(runparams, run_results) == SRSRAN_SUCCESS);
  }

  print_benchmark_results(run_results);

  bool success = true;
  for (auto& run : run_results) {
    run_data expected = expected_run_result(run.params);
    if (run.avg_dl_mcs < expected.avg_dl_mcs) {
      fmt::print(
          "Nprb={:>2d}: DL mcs below expected ({} < {})\n", run.params.nof_prbs, run.avg_dl_mcs, expected.avg_dl_mcs);
      success = false;
    }
    if (run.avg_dl_throughput < expected.avg_dl_throughput) {
      fmt::print("Nprb={:>2d}: DL rate below expected ({:.2} < {:.2}) Mbps\n",
                 run.params.nof_prbs,
                 run.avg_dl_throughput / 1e6,
                 expected.avg_dl_throughput / 1e6);
      success = false;
    }
    if (run.avg_ul_mcs < expected.avg_ul_mcs) {
      fmt::print(
          "Nprb={:>2d}: UL mcs below expected ({} < {})\n", run.params.nof_prbs, run.avg_ul_mcs, expected.avg_ul_mcs);
      success = false;
    }
    if (run.avg_ul_throughput < expected.avg_ul_throughput) {
      fmt::print("Nprb={:>2d}: UL rate below expected ({:.2} < {:.2}) Mbps\n",
                 run.params.nof_prbs,
                 run.avg_ul_throughput / 1e6,
                 expected.avg_ul_throughput / 1e6);
      success = false;
    }
  }
  return success ? SRSRAN_SUCCESS : SRSRAN_ERROR;
}

int run_all()
{
  run_params_range      run_param_list{};
  srslog::basic_logger& mac_logger = srslog::fetch_basic_logger("MAC");

  fmt::print("Running all param combinations\n");
  std::vector<run_data> run_results;
  size_t                nof_runs = run_param_list.nof_runs();
  for (size_t r = 0; r < nof_runs; ++r) {
    run_params runparams = run_param_list.get_params(r);

    mac_logger.info("\n### New run {} ###\n", r);
    TESTASSERT(run_benchmark_scenario(runparams, run_results) == SRSRAN_SUCCESS);
  }

  print_benchmark_results(run_results);

  return SRSRAN_SUCCESS;
}

int run_benchmark()
{
  run_params_range      run_param_list{};
  srslog::basic_logger& mac_logger = srslog::fetch_basic_logger("MAC");

  run_param_list.nof_ttis     = 1000000;
  run_param_list.nof_prbs     = {100};
  run_param_list.cqi          = {15};
  run_param_list.nof_ues      = {5};
  run_param_list.sched_policy = {"time_pf"};

  std::vector<run_data> run_results;
  size_t                nof_runs = run_param_list.nof_runs();
  fmt::print("Running Benchmark\n");
  for (size_t r = 0; r < nof_runs; ++r) {
    run_params runparams = run_param_list.get_params(r);

    mac_logger.info("\n### New run {} ###\n", r);
    TESTASSERT(run_benchmark_scenario(runparams, run_results) == SRSRAN_SUCCESS);
  }

  print_benchmark_results(run_results);

  return SRSRAN_SUCCESS;
}

} // namespace srsenb

int main(int argc, char* argv[])
{
  // Setup the log spy to intercept error and warning log entries.
  if (!srslog::install_custom_sink(
          srsran::log_sink_spy::name(),
          std::unique_ptr<srsran::log_sink_spy>(new srsran::log_sink_spy(srslog::get_default_log_formatter())))) {
    return SRSRAN_ERROR;
  }

  auto* spy = static_cast<srsran::log_sink_spy*>(srslog::find_sink(srsran::log_sink_spy::name()));
  if (spy == nullptr) {
    return SRSRAN_ERROR;
  }

  auto& mac_log = srslog::fetch_basic_logger("MAC");
  mac_log.set_level(srslog::basic_levels::warning);
  auto& test_log = srslog::fetch_basic_logger("TEST", *spy, false);
  test_log.set_level(srslog::basic_levels::warning);

  // Start the log backend.
  srslog::init();

  bool run_benchmark = false;

  if (argc == 1 or strcmp(argv[1], "test") == 0) {
    TESTASSERT(srsenb::run_rate_test() == SRSRAN_SUCCESS);
  } else if (strcmp(argv[1], "benchmark") == 0) {
    TESTASSERT(srsenb::run_benchmark() == SRSRAN_SUCCESS);
  } else {
    TESTASSERT(srsenb::run_all() == SRSRAN_SUCCESS);
  }

  return 0;
}