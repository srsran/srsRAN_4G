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
#include "srsenb/hdr/stack/mac/nr/sched_nr.h"
#include "srsran/common/phy_cfg_nr_default.h"
#include "srsran/common/test_common.h"
#include "srsran/common/thread_pool.h"
#include <chrono>

namespace srsenb {

using dl_sched_t = sched_nr_interface::dl_sched_t;

static const srsran::phy_cfg_nr_t default_phy_cfg =
    srsran::phy_cfg_nr_default_t{srsran::phy_cfg_nr_default_t::reference_cfg_t{}};

struct task_job_manager {
  std::mutex            mutex;
  int                   res_count   = 0;
  int                   pdsch_count = 0;
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
  struct slot_guard {
    int                     count = 0;
    std::condition_variable cvar;
  };
  srsran::bounded_vector<slot_guard, 10> slot_counter{};

  explicit task_job_manager(int max_concurrent_slots = 4) : slot_counter(max_concurrent_slots) {}

  void start_slot(slot_point slot, int nof_sectors)
  {
    std::unique_lock<std::mutex> lock(mutex);
    auto&                        sl = slot_counter[slot.to_uint() % slot_counter.size()];
    while (sl.count > 0) {
      sl.cvar.wait(lock);
    }
    sl.count = nof_sectors;
  }
  void finish_cc(slot_point slot, const dl_sched_t& dl_res, const sched_nr_interface::ul_sched_t& ul_res)
  {
    std::unique_lock<std::mutex> lock(mutex);
    TESTASSERT(dl_res.pdcch_dl.size() <= 1);
    res_count++;
    pdsch_count += dl_res.pdcch_dl.size();
    auto& sl = slot_counter[slot.to_uint() % slot_counter.size()];
    if (--sl.count == 0) {
      sl.cvar.notify_one();
    }
  }
  void wait_task_finish()
  {
    std::unique_lock<std::mutex> lock(mutex);
    for (auto& sl : slot_counter) {
      while (sl.count > 0) {
        sl.cvar.wait(lock);
      }
      sl.count = 1;
    }
  }
  void print_results() const
  {
    test_logger.info("TESTER: %f PDSCH/{slot,cc} were allocated", pdsch_count / (double)res_count);
    srslog::flush();
  }
};

void sched_nr_cfg_serialized_test()
{
  uint32_t         max_nof_ttis = 1000, nof_sectors = 4;
  task_job_manager tasks;

  sched_nr_interface::sched_cfg_t             cfg;
  std::vector<sched_nr_interface::cell_cfg_t> cells_cfg = get_default_cells_cfg(nof_sectors);

  sched_nr_sim_base sched_tester(cfg, cells_cfg, "Serialized Test");

  sched_nr_interface::ue_cfg_t uecfg = get_default_ue_cfg(nof_sectors);
  uecfg.fixed_dl_mcs                 = 15;
  uecfg.fixed_ul_mcs                 = 15;
  sched_tester.add_user(0x46, uecfg, slot_point{0, 0}, 0);

  std::vector<long> count_per_cc(nof_sectors, 0);
  for (uint32_t nof_slots = 0; nof_slots < max_nof_ttis; ++nof_slots) {
    slot_point slot_rx(0, nof_slots % 10240);
    slot_point slot_tx = slot_rx + TX_ENB_DELAY;
    tasks.start_slot(slot_rx, nof_sectors);
    sched_tester.new_slot(slot_tx);
    for (uint32_t cc = 0; cc < cells_cfg.size(); ++cc) {
      sched_nr_interface::dl_sched_t dl_res;
      sched_nr_interface::ul_sched_t ul_res;
      auto                           tp1 = std::chrono::steady_clock::now();
      TESTASSERT(sched_tester.get_sched()->get_dl_sched(slot_tx, cc, dl_res) == SRSRAN_SUCCESS);
      TESTASSERT(sched_tester.get_sched()->get_ul_sched(slot_tx, cc, ul_res) == SRSRAN_SUCCESS);
      auto tp2 = std::chrono::steady_clock::now();
      count_per_cc[cc] += std::chrono::duration_cast<std::chrono::nanoseconds>(tp2 - tp1).count();
      sched_nr_cc_output_res_t out{slot_tx, cc, &dl_res, &ul_res};
      sched_tester.update(out);
      tasks.finish_cc(slot_rx, dl_res, ul_res);
      TESTASSERT(not srsran_tdd_nr_is_dl(&cells_cfg[cc].tdd, 0, (slot_tx).slot_idx()) or dl_res.pdcch_dl.size() == 1);
    }
  }

  tasks.print_results();
  TESTASSERT(tasks.pdsch_count == (int)(max_nof_ttis * nof_sectors * 0.6));

  double final_avg_usec = 0;
  for (uint32_t cc = 0; cc < cells_cfg.size(); ++cc) {
    final_avg_usec += count_per_cc[cc];
  }
  final_avg_usec = final_avg_usec / 1000.0 / max_nof_ttis;
  printf("Total time taken per slot: %f usec\n", final_avg_usec);
}

void sched_nr_cfg_parallel_cc_test()
{
  uint32_t         nof_sectors  = 4;
  uint32_t         max_nof_ttis = 1000;
  task_job_manager tasks;

  sched_nr_interface::sched_cfg_t             cfg;
  std::vector<sched_nr_interface::cell_cfg_t> cells_cfg = get_default_cells_cfg(nof_sectors);

  sched_nr_sim_base sched_tester(cfg, cells_cfg, "Parallel CC Test");

  sched_nr_interface::ue_cfg_t uecfg = get_default_ue_cfg(cells_cfg.size());
  uecfg.fixed_dl_mcs                 = 15;
  uecfg.fixed_ul_mcs                 = 15;
  sched_tester.add_user(0x46, uecfg, slot_point{0, 0}, 0);

  std::array<std::atomic<long>, SRSRAN_MAX_CARRIERS> nano_count{};
  for (uint32_t nof_slots = 0; nof_slots < max_nof_ttis; ++nof_slots) {
    slot_point slot_rx(0, nof_slots % 10240);
    slot_point slot_tx = slot_rx + TX_ENB_DELAY;
    tasks.start_slot(slot_tx, nof_sectors);
    sched_tester.new_slot(slot_tx);
    for (uint32_t cc = 0; cc < cells_cfg.size(); ++cc) {
      srsran::get_background_workers().push_task([cc, slot_tx, &tasks, &sched_tester, &nano_count]() {
        sched_nr_interface::dl_sched_t dl_res;
        sched_nr_interface::ul_sched_t ul_res;
        auto                           tp1 = std::chrono::steady_clock::now();
        TESTASSERT(sched_tester.get_sched()->get_dl_sched(slot_tx, cc, dl_res) == SRSRAN_SUCCESS);
        TESTASSERT(sched_tester.get_sched()->get_ul_sched(slot_tx, cc, ul_res) == SRSRAN_SUCCESS);
        auto tp2 = std::chrono::steady_clock::now();
        nano_count[cc].fetch_add(std::chrono::duration_cast<std::chrono::nanoseconds>(tp2 - tp1).count(),
                                 std::memory_order_relaxed);
        sched_nr_cc_output_res_t out{slot_tx, cc, &dl_res, &ul_res};
        sched_tester.update(out);
        tasks.finish_cc(slot_tx, dl_res, ul_res);
      });
    }
  }

  tasks.wait_task_finish();

  tasks.print_results();
  TESTASSERT(tasks.pdsch_count == (int)(max_nof_ttis * nof_sectors * 0.6));

  double final_avg_usec = 0;
  for (uint32_t i = 0; i < nof_sectors; ++i) {
    final_avg_usec += nano_count[i];
  }
  final_avg_usec = final_avg_usec / 1000.0 / max_nof_ttis / nof_sectors;
  printf("Total time taken per slot [usec]: %f\n", final_avg_usec);
}

} // namespace srsenb

int main()
{
  auto& test_logger = srslog::fetch_basic_logger("TEST");
  test_logger.set_level(srslog::basic_levels::warning);
  auto& mac_logger = srslog::fetch_basic_logger("MAC");
  mac_logger.set_level(srslog::basic_levels::warning);
  auto& pool_logger = srslog::fetch_basic_logger("POOL");
  pool_logger.set_level(srslog::basic_levels::info);

  // Start the log backend.
  srslog::init();

  srsran::get_background_workers().set_nof_workers(6);

  srsenb::sched_nr_cfg_serialized_test();
  srsenb::sched_nr_cfg_parallel_cc_test();
}
