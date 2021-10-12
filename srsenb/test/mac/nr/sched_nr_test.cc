/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "sched_nr_cfg_generators.h"
#include "sched_nr_sim_ue.h"
#include "srsran/common/phy_cfg_nr_default.h"
#include "srsran/common/test_common.h"
#include "srsran/common/thread_pool.h"
#include <chrono>

namespace srsenb {

using dl_sched_t = sched_nr_interface::dl_sched_t;

static const srsran::phy_cfg_nr_t default_phy_cfg =
    srsran::phy_cfg_nr_default_t{srsran::phy_cfg_nr_default_t::reference_cfg_t{}};

class sched_nr_tester : public sched_nr_sim_parallel
{
public:
  using sched_nr_sim_parallel::sched_nr_sim_parallel;

  void process_cc_result(const cc_result_t& cc_result) override
  {
    tot_latency_sched_ns += cc_result.sched_latency_ns.count();
    result_count++;
    pdsch_count += cc_result.dl_res.dl_sched.pdcch_dl.size();

    TESTASSERT(cc_result.dl_res.dl_sched.pdcch_dl.size() <= 1);
    if (srsran_duplex_nr_is_dl(&cell_params[cc_result.cc].cfg.duplex, 0, current_slot_tx.slot_idx())) {
      TESTASSERT(cc_result.dl_res.dl_sched.pdcch_dl.size() == 1 or not cc_result.dl_res.dl_sched.ssb.empty());
    }
  }

  void print_results() const
  {
    test_logger.info("TESTER: %f PDSCH/{slot,cc} were allocated", pdsch_count / (double)result_count);
    srslog::flush();
  }

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  uint64_t tot_latency_sched_ns = 0;
  uint32_t result_count         = 0;
  uint32_t pdsch_count          = 0;
};

void sched_nr_cfg_serialized_test()
{
  uint32_t max_nof_ttis = 1000, nof_sectors = 4;
  uint16_t rnti = 0x4601;

  sched_nr_interface::sched_args_t cfg;
  cfg.auto_refill_buffer = true;

  std::vector<sched_nr_interface::cell_cfg_t> cells_cfg = get_default_cells_cfg(nof_sectors);

  sched_nr_tester tester(cfg, cells_cfg, "Serialized Test");

  sched_nr_interface::ue_cfg_t uecfg = get_default_ue_cfg(nof_sectors);
  tester.add_user(rnti, uecfg, slot_point{0, 0}, 0);

  for (uint32_t nof_slots = 0; nof_slots < max_nof_ttis; ++nof_slots) {
    slot_point slot_rx(0, nof_slots % 10240);
    slot_point slot_tx = slot_rx + TX_ENB_DELAY;
    tester.new_slot(slot_tx);
    for (uint32_t cc = 0; cc != cells_cfg.size(); ++cc) {
      tester.generate_cc_result(cc);
    }
  }

  tester.stop();
  tester.print_results();
  //  TESTASSERT(tasks.pdsch_count == (int)(max_nof_ttis * nof_sectors * 0.6));

  double final_avg_usec = tester.tot_latency_sched_ns;
  final_avg_usec        = final_avg_usec / 1000.0 / max_nof_ttis;
  printf("Total time taken per slot: %f usec\n", final_avg_usec);
}

void sched_nr_cfg_parallel_cc_test()
{
  uint32_t nof_sectors  = 4;
  uint32_t max_nof_ttis = 1000;
  uint16_t rnti         = 0x4601;

  // Initiate CC Workers
  std::vector<std::unique_ptr<srsran::task_worker> > cc_workers;
  cc_workers.reserve(nof_sectors - 1);
  for (uint32_t i = 0; i < nof_sectors - 1; ++i) {
    fmt::memory_buffer fmtbuf;
    fmt::format_to(fmtbuf, "worker{}", i);
    cc_workers.emplace_back(new srsran::task_worker{to_string(fmtbuf), 10});
  }

  sched_nr_interface::sched_args_t cfg;
  cfg.auto_refill_buffer = true;

  std::vector<sched_nr_interface::cell_cfg_t> cells_cfg = get_default_cells_cfg(nof_sectors);

  sched_nr_tester tester(cfg, cells_cfg, "Parallel CC Test");

  sched_nr_interface::ue_cfg_t uecfg = get_default_ue_cfg(cells_cfg.size());
  tester.add_user(rnti, uecfg, slot_point{0, 0}, 0);

  for (uint32_t nof_slots = 0; nof_slots < max_nof_ttis; ++nof_slots) {
    slot_point slot_rx(0, nof_slots % 10240);
    slot_point slot_tx = slot_rx + TX_ENB_DELAY;
    tester.new_slot(slot_tx);
    // Run scheduler in parallel for {0, cc-2} and in the same thread for last cc
    for (uint32_t cc = 0; cc != cells_cfg.size() - 1; ++cc) {
      cc_workers[cc]->push_task([cc, &tester]() { tester.generate_cc_result(cc); });
    }
    tester.generate_cc_result(cells_cfg.size() - 1);
  }

  // Wait for all jobs to finish
  tester.stop();

  tester.print_results();

  //  TESTASSERT(tasks.pdsch_count == (int)(max_nof_ttis * nof_sectors * 0.6));

  double final_avg_usec = tester.tot_latency_sched_ns;
  final_avg_usec        = final_avg_usec / 1000.0 / max_nof_ttis / nof_sectors;
  printf("Total time taken per slot [usec]: %f\n", final_avg_usec);
}

} // namespace srsenb

int main()
{
  auto& test_logger = srslog::fetch_basic_logger("TEST");
  test_logger.set_level(srslog::basic_levels::error);
  auto& mac_nr_logger = srslog::fetch_basic_logger("MAC-NR");
  mac_nr_logger.set_level(srslog::basic_levels::error);
  auto& pool_logger = srslog::fetch_basic_logger("POOL");
  pool_logger.set_level(srslog::basic_levels::debug);

  // Start the log backend.
  srslog::init();

  srsenb::sched_nr_cfg_serialized_test();
  srsenb::sched_nr_cfg_parallel_cc_test();
}
