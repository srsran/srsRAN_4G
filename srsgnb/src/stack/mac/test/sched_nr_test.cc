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

#include "sched_nr_cfg_generators.h"
#include "sched_nr_sim_ue.h"
#include "srsran/common/phy_cfg_nr_default.h"
#include "srsran/common/test_common.h"
#include "srsran/support/emergency_handlers.h"
#include <boost/program_options.hpp>
#include <fstream>
#include <random>

// shorten boost program options namespace
namespace bpo = boost::program_options;

namespace srsenb {

std::default_random_engine rand_gen;

struct sim_args_t {
  uint32_t    rand_seed;
  uint32_t    fixed_cqi;
  std::string mac_log_level;
  std::string test_log_level;
};

class sched_tester : public sched_nr_base_test_bench
{
  sim_args_t args;

public:
  explicit sched_tester(sim_args_t                              args_,
                        const sched_nr_interface::sched_args_t& sched_args,
                        const std::vector<sched_nr_cell_cfg_t>& cell_params_,
                        std::string                             test_name) :
    sched_nr_base_test_bench(sched_args, cell_params_, test_name), args(args_)
  {}

  void process_slot_result(const sim_nr_enb_ctxt_t& enb_ctxt, srsran::const_span<cc_result_t> cc_out) override
  {
    for (auto& cc : cc_out) {
      for (auto& pdsch : cc.res.dl->phy.pdsch) {
        if (pdsch.sch.grant.rnti_type == srsran_rnti_type_c or pdsch.sch.grant.rnti_type == srsran_rnti_type_tc) {
          ue_metrics[pdsch.sch.grant.rnti].nof_dl_txs++;
          ue_metrics[pdsch.sch.grant.rnti].nof_dl_bytes += pdsch.sch.grant.tb[0].tbs / 8u;
        }
      }
      for (auto& pusch : cc.res.ul->pusch) {
        if (pusch.sch.grant.rnti_type == srsran_rnti_type_c or pusch.sch.grant.rnti_type == srsran_rnti_type_tc) {
          ue_metrics[pusch.sch.grant.rnti].nof_ul_txs++;
          ue_metrics[pusch.sch.grant.rnti].nof_ul_bytes += pusch.sch.grant.tb[0].tbs / 8u;
        }
      }
    }
  }

  void set_external_slot_events(const sim_nr_ue_ctxt_t& ue_ctxt, ue_nr_slot_events& pending_events) override
  {
    for (uint32_t cc = 0; cc < pending_events.cc_list.size(); ++cc) {
      auto& cc_events = pending_events.cc_list[cc];
      // if CQI is expected, set it to fixed value
      if (cc_events.cqi >= 0) {
        cc_events.cqi = args.fixed_cqi;
      }
    }
  }

  void print_results()
  {
    srslog::flush();
    fmt::print("SCHED UE metrics:\n");
    for (auto& u : ue_metrics) {
      fmt::print("  0x{:x}: nof_txs=({}, {}), nof_bytes=({}, {})\n",
                 u.first,
                 u.second.nof_dl_txs,
                 u.second.nof_ul_txs,
                 u.second.nof_dl_bytes,
                 u.second.nof_ul_bytes);
    }
  }

  struct sched_ue_metrics {
    uint32_t nof_dl_txs = 0, nof_ul_txs = 0;
    uint64_t nof_dl_bytes = 0, nof_ul_bytes = 0;
  };
  std::map<uint16_t, sched_ue_metrics> ue_metrics;
};

struct sched_event_t {
  uint32_t                                       slot_count;
  std::function<void(sched_nr_base_test_bench&)> run;
};

sched_event_t add_user(uint32_t slot_count, uint16_t rnti, uint32_t preamble_idx)
{
  auto task = [rnti, preamble_idx](sched_nr_base_test_bench& tester) {
    tester.rach_ind(rnti, 0, tester.get_slot_tx() - TX_ENB_DELAY, preamble_idx);
  };
  return sched_event_t{slot_count, task};
}

sched_event_t ue_cfg(uint32_t slot_count, uint16_t rnti, const sched_nr_ue_cfg_t& ue_cfg)
{
  auto task = [rnti, ue_cfg](sched_nr_base_test_bench& tester) { tester.user_cfg(rnti, ue_cfg); };
  return sched_event_t{slot_count, task};
}

sched_event_t add_rlc_dl_bytes(uint32_t slot_count, uint16_t rnti, uint32_t lcid, uint32_t pdu_size)
{
  auto task = [rnti, pdu_size, lcid](sched_nr_base_test_bench& tester) {
    tester.add_rlc_dl_bytes(rnti, lcid, pdu_size);
  };
  return sched_event_t{slot_count, task};
}

void test_sched_nr_no_data(sim_args_t args)
{
  uint32_t max_nof_ttis = 1000, nof_sectors = 1;
  uint16_t rnti = 0x4601;

  sched_nr_interface::sched_args_t cfg;
  cfg.auto_refill_buffer                     = false;
  std::vector<sched_nr_cell_cfg_t> cells_cfg = get_default_cells_cfg(nof_sectors);

  std::string  test_name = "Test with no data";
  sched_tester tester(args, cfg, cells_cfg, test_name);

  /* Set events */
  std::deque<sched_event_t> events;
  events.push_back(add_user(9, rnti, 0));
  sched_nr_interface::ue_cfg_t uecfg = get_default_ue_cfg(1);
  events.push_back(ue_cfg(20, rnti, uecfg));

  /* Run Test */
  for (uint32_t nof_slots = 0; nof_slots < max_nof_ttis; ++nof_slots) {
    slot_point slot_rx(0, nof_slots % 10240);
    slot_point slot_tx = slot_rx + TX_ENB_DELAY;

    // run events
    while (not events.empty() and events.front().slot_count <= nof_slots) {
      events.front().run(tester);
      events.pop_front();
    }

    // call sched
    tester.run_slot(slot_tx);
  }

  tester.print_results();

  // Since DL buffers were not externally updated, we should only see Msg4 as DL tx
  TESTASSERT_EQ(1, tester.ue_metrics[rnti].nof_dl_txs);
  // Since UL buffers were not externally updated, we should only see Msg3 as UL tx
  TESTASSERT_EQ(1, tester.ue_metrics[rnti].nof_ul_txs);
}

void test_sched_nr_data(sim_args_t args)
{
  uint32_t nof_sectors = 1;
  uint16_t rnti        = 0x4601;
  uint32_t nof_dl_bytes_to_tx =
      std::uniform_int_distribution<int>{1, 9}(rand_gen)*pow(10, std::uniform_int_distribution<int>{1, 7}(rand_gen));

  sched_nr_interface::sched_args_t cfg;
  cfg.auto_refill_buffer                     = false;
  std::vector<sched_nr_cell_cfg_t> cells_cfg = get_default_cells_cfg(nof_sectors);

  std::string  test_name = "Test with data";
  sched_tester tester(args, cfg, cells_cfg, test_name);

  /* Set events */
  std::deque<sched_event_t> events;
  events.push_back(add_user(9, rnti, 0));
  sched_nr_interface::ue_cfg_t uecfg = get_default_ue_cfg(1);
  events.push_back(ue_cfg(20, rnti, uecfg));
  events.push_back(add_rlc_dl_bytes(50, rnti, 0, nof_dl_bytes_to_tx));

  /* Run Test */
  uint32_t stop_tti         = std::numeric_limits<uint32_t>::max();
  auto     finish_condition = [&stop_tti, rnti, nof_dl_bytes_to_tx, &tester](uint32_t nof_slots) mutable {
    if (stop_tti == std::numeric_limits<uint32_t>::max() and
        tester.ue_metrics[rnti].nof_dl_bytes >= nof_dl_bytes_to_tx) {
      stop_tti = nof_slots + 10;
    }
    return nof_slots >= std::min(stop_tti, 100000u);
  };
  for (uint32_t nof_slots = 0; not finish_condition(nof_slots); ++nof_slots) {
    slot_point slot_rx(0, nof_slots % 10240);
    slot_point slot_tx = slot_rx + TX_ENB_DELAY;

    // run events
    while (not events.empty() and events.front().slot_count <= nof_slots) {
      events.front().run(tester);
      events.pop_front();
    }

    // call sched
    tester.run_slot(slot_tx);
  }
  // Run a few extra slots to ensure the scheduler does not allocate more DL bytes than necessary
  for (uint32_t nof_slots_extra = 0; nof_slots_extra < 40; ++nof_slots_extra) {
    tester.run_slot(tester.get_slot_tx() + 1);
    auto results = tester.get_slot_results();
    TESTASSERT(results[0].res.dl->phy.pdcch_dl.empty());
    TESTASSERT(results[0].res.dl->phy.pdcch_ul.empty());
  }

  srslog::flush();
  fmt::print("== Results ==\n");
  fmt::print("Enqueued RLC DL bytes: {}\n", nof_dl_bytes_to_tx);
  tester.print_results();

  TESTASSERT(tester.ue_metrics[rnti].nof_dl_txs > 0);
  TESTASSERT(tester.ue_metrics[rnti].nof_dl_bytes >= nof_dl_bytes_to_tx);
  TESTASSERT(tester.ue_metrics[rnti].nof_dl_bytes < nof_dl_bytes_to_tx + 20000);
  // Since UL buffers were not externally updated, we should only see Msg3 as UL tx
  TESTASSERT_EQ(1, tester.ue_metrics[rnti].nof_ul_txs);
}

sim_args_t handle_args(int argc, char** argv)
{
  sim_args_t args;

  std::string              config_file;
  bpo::options_description options;
  bpo::options_description options_sim("Test Sim options");
  bpo::options_description options_conf_file("Configuration file");

  // clang-format off
  options_sim.add_options()
      ("seed",            bpo::value<uint32_t>(&args.rand_seed)->default_value(std::chrono::system_clock::now().time_since_epoch().count()), "Simulation Random Seed")
      ("cqi",            bpo::value<uint32_t>(&args.fixed_cqi)->default_value(15), "UE DL CQI")
      ("log.mac_level",  bpo::value<std::string>(&args.mac_log_level)->default_value("info"), "MAC log level")
      ("log.test_level", bpo::value<std::string>(&args.test_log_level)->default_value("info"), "TEST log level")
      ;
  options_conf_file.add_options()
      ("config_file", bpo::value<std::string>(&config_file), "Configuration file")
      ;

  bpo::positional_options_description p;
  p.add("config_file", -1);

  options.add(options_sim).add(options_conf_file).add_options()
      ("help",                      "Show this message")
      ;
  // clang-format on

  bpo::variables_map vm;
  try {
    bpo::store(bpo::command_line_parser(argc, argv).options(options).positional(p).run(), vm);
    bpo::notify(vm);
  } catch (bpo::error& e) {
    srsran_terminate("%s", e.what());
  }

  // help option was given or error - print usage and exit
  if (vm.count("help")) {
    fmt::print("Usage: {} [OPTIONS] config_file\n\n", argv[0]);
    fmt::print("{}\n", options);
    exit(0);
  }

  // if config file given
  if (vm.count("config_file") != 0U) {
    fmt::print("Reading configuration file {}...\n", config_file);
    std::ifstream conf(config_file.c_str(), std::ios::in);
    if (conf.fail()) {
      fmt::print("Failed to read configuration file {} - exiting\n", config_file);
      exit(1);
    }

    // parse config file and handle errors gracefully
    try {
      bpo::store(bpo::parse_config_file(conf, options), vm);
      bpo::notify(vm);
    } catch (const boost::program_options::error& e) {
      srsran_terminate("%s\n", e.what());
    }
  }

  return args;
}

} // namespace srsenb

int main(int argc, char** argv)
{
  // Start the log backend.
  srslog::init();

  // Parse args
  srsenb::sim_args_t args = srsenb::handle_args(argc, argv);

  // Initialize Loggers
  auto& test_logger = srslog::fetch_basic_logger("TEST");
  test_logger.set_level(srslog::str_to_basic_level(args.test_log_level));
  auto& mac_nr_logger = srslog::fetch_basic_logger("MAC-NR");
  mac_nr_logger.set_level(srslog::str_to_basic_level(args.mac_log_level));
  auto& pool_logger = srslog::fetch_basic_logger("POOL");
  pool_logger.set_level(srslog::basic_levels::debug);

  // Setup Random Generator
  srsenb::rand_gen = std::default_random_engine(args.rand_seed);
  fmt::print("TEST: Random Seed is {}", args.rand_seed);
  add_emergency_cleanup_handler(
      [](void* args_void) {
        auto* args = (srsenb::sim_args_t*)args_void;
        fmt::print("TEST: Random Seed was {}", args->rand_seed);
      },
      (void*)&args);

  srsenb::test_sched_nr_no_data(args);
  srsenb::test_sched_nr_data(args);

  fmt::print("TEST: Random Seed was {}", args.rand_seed);
}
