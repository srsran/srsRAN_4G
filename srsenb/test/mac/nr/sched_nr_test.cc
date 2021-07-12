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

srsran_coreset_t get_default_coreset0()
{
  srsran_coreset_t coreset{};
  coreset.id                   = 0;
  coreset.duration             = 1;
  coreset.precoder_granularity = srsran_coreset_precoder_granularity_reg_bundle;
  for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; ++i) {
    coreset.freq_resources[i] = i < 8;
  }
  return coreset;
}

sched_nr_interface::cell_cfg_t get_default_cell_cfg()
{
  sched_nr_interface::cell_cfg_t cell_cfg{};

  cell_cfg.carrier = default_phy_cfg.carrier;
  cell_cfg.tdd     = default_phy_cfg.tdd;

  cell_cfg.bwps.resize(1);
  cell_cfg.bwps[0].pdcch    = default_phy_cfg.pdcch;
  cell_cfg.bwps[0].pdsch    = default_phy_cfg.pdsch;
  cell_cfg.bwps[0].rb_width = default_phy_cfg.carrier.nof_prb;

  cell_cfg.bwps[0].pdcch.coreset_present[0]      = true;
  cell_cfg.bwps[0].pdcch.coreset[0]              = get_default_coreset0();
  cell_cfg.bwps[0].pdcch.search_space_present[0] = true;
  auto& ss                                       = cell_cfg.bwps[0].pdcch.search_space[0];
  ss.id                                          = 0;
  ss.coreset_id                                  = 0;
  ss.duration                                    = 1;
  ss.type                                        = srsran_search_space_type_common_0;
  ss.nof_candidates[0]                           = 1;
  ss.nof_candidates[1]                           = 1;
  ss.nof_candidates[2]                           = 1;
  ss.nof_candidates[3]                           = 0;
  ss.nof_candidates[4]                           = 0;
  ss.nof_formats                                 = 1;
  ss.formats[0]                                  = srsran_dci_format_nr_1_0;
  cell_cfg.bwps[0].pdcch.ra_search_space_present = true;
  cell_cfg.bwps[0].pdcch.ra_search_space         = cell_cfg.bwps[0].pdcch.search_space[1];

  return cell_cfg;
}
std::vector<sched_nr_interface::cell_cfg_t> get_default_cells_cfg(uint32_t nof_sectors)
{
  std::vector<sched_nr_interface::cell_cfg_t> cells;
  cells.reserve(nof_sectors);
  for (uint32_t i = 0; i < nof_sectors; ++i) {
    cells.push_back(get_default_cell_cfg());
  }
  return cells;
}

sched_nr_interface::ue_cfg_t get_default_ue_cfg(uint32_t nof_cc)
{
  sched_nr_interface::ue_cfg_t uecfg{};
  uecfg.carriers.resize(nof_cc);
  for (uint32_t cc = 0; cc < nof_cc; ++cc) {
    uecfg.carriers[cc].active = true;
  }
  uecfg.phy_cfg = default_phy_cfg;

  return uecfg;
}

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

  void start_slot(tti_point tti, int nof_sectors)
  {
    std::unique_lock<std::mutex> lock(mutex);
    auto&                        sl = slot_counter[tti.to_uint() % slot_counter.size()];
    while (sl.count > 0) {
      sl.cvar.wait(lock);
    }
    sl.count = nof_sectors;
  }
  void finish_cc(tti_point tti, const dl_sched_t& dl_res, const sched_nr_interface::ul_sched_t& ul_res)
  {
    std::unique_lock<std::mutex> lock(mutex);
    TESTASSERT(dl_res.pdcch_dl.size() <= 1);
    res_count++;
    pdsch_count += dl_res.pdcch_dl.size();
    auto& sl = slot_counter[tti.to_uint() % slot_counter.size()];
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
  uint32_t         max_nof_ttis = 1000, nof_sectors = 2;
  task_job_manager tasks;

  sched_nr_interface::sched_cfg_t             cfg;
  std::vector<sched_nr_interface::cell_cfg_t> cells_cfg = get_default_cells_cfg(nof_sectors);

  sched_nr_sim_base sched_tester(cfg, cells_cfg, "Serialized Test");

  sched_nr_interface::ue_cfg_t uecfg = get_default_ue_cfg(2);

  sched_tester.add_user(0x46, uecfg, 0);

  auto tp1 = std::chrono::steady_clock::now();
  for (uint32_t nof_ttis = 0; nof_ttis < max_nof_ttis; ++nof_ttis) {
    tti_point tti_rx(nof_ttis % 10240);
    tti_point tti_tx = tti_rx + TX_ENB_DELAY;
    tasks.start_slot(tti_rx, nof_sectors);
    sched_tester.new_slot(tti_tx);
    for (uint32_t cc = 0; cc < cells_cfg.size(); ++cc) {
      sched_nr_interface::dl_sched_t dl_res;
      sched_nr_interface::ul_sched_t ul_res;
      TESTASSERT(sched_tester.get_sched()->get_dl_sched(tti_tx, cc, dl_res) == SRSRAN_SUCCESS);
      TESTASSERT(sched_tester.get_sched()->get_ul_sched(tti_tx, cc, ul_res) == SRSRAN_SUCCESS);
      sched_nr_cc_output_res_t out{tti_tx, cc, &dl_res, &ul_res};
      sched_tester.update(out);
      tasks.finish_cc(tti_rx, dl_res, ul_res);
      TESTASSERT(not srsran_tdd_nr_is_dl(&cells_cfg[cc].tdd, 0, (tti_tx).sf_idx()) or dl_res.pdcch_dl.size() == 1);
    }
  }
  auto tp2 = std::chrono::steady_clock::now();

  tasks.print_results();
  TESTASSERT(tasks.pdsch_count == (int)(max_nof_ttis * nof_sectors * 0.6));

  uint32_t microsecs = std::chrono::duration_cast<std::chrono::microseconds>(tp2 - tp1).count();
  printf("Total time taken per slot: %f\n", microsecs / (float)max_nof_ttis);
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
  sched_tester.add_user(0x46, uecfg, 0);

  auto tp1 = std::chrono::steady_clock::now();
  for (uint32_t nof_ttis = 0; nof_ttis < max_nof_ttis; ++nof_ttis) {
    tti_point tti_rx(nof_ttis % 10240);
    tti_point tti_tx = tti_rx + TX_ENB_DELAY;
    tasks.start_slot(tti_tx, nof_sectors);
    sched_tester.new_slot(tti_tx);
    for (uint32_t cc = 0; cc < cells_cfg.size(); ++cc) {
      srsran::get_background_workers().push_task([cc, tti_tx, &tasks, &sched_tester]() {
        sched_nr_interface::dl_sched_t dl_res;
        sched_nr_interface::ul_sched_t ul_res;
        TESTASSERT(sched_tester.get_sched()->get_dl_sched(tti_tx, cc, dl_res) == SRSRAN_SUCCESS);
        TESTASSERT(sched_tester.get_sched()->get_ul_sched(tti_tx, cc, ul_res) == SRSRAN_SUCCESS);
        sched_nr_cc_output_res_t out{tti_tx, cc, &dl_res, &ul_res};
        sched_tester.update(out);
        tasks.finish_cc(tti_tx, dl_res, ul_res);
      });
    }
  }
  auto tp2 = std::chrono::steady_clock::now();

  tasks.wait_task_finish();

  tasks.print_results();
  TESTASSERT(tasks.pdsch_count == (int)(max_nof_ttis * nof_sectors * 0.6));

  uint32_t microsecs = std::chrono::duration_cast<std::chrono::microseconds>(tp2 - tp1).count();
  printf("Total time taken per slot [usec]: %f\n", microsecs / (float)max_nof_ttis);
}

void sched_nr_cfg_parallel_sf_test()
{
  uint32_t         max_nof_ttis = 1000;
  uint32_t         nof_sectors  = 2;
  task_job_manager tasks;

  sched_nr_interface::sched_cfg_t cfg;
  cfg.nof_concurrent_subframes                          = 2;
  std::vector<sched_nr_interface::cell_cfg_t> cells_cfg = get_default_cells_cfg(nof_sectors);

  sched_nr sched(cfg);
  sched.cell_cfg(cells_cfg);

  sched_nr_interface::ue_cfg_t uecfg = get_default_ue_cfg(cells_cfg.size());
  sched.ue_cfg(0x46, uecfg);

  auto tp1 = std::chrono::steady_clock::now();
  for (uint32_t nof_ttis = 0; nof_ttis < max_nof_ttis; ++nof_ttis) {
    tti_point tti(nof_ttis % 10240);
    tasks.start_slot(tti, nof_sectors);
    for (uint32_t cc = 0; cc < cells_cfg.size(); ++cc) {
      srsran::get_background_workers().push_task([cc, &sched, tti, &tasks]() {
        sched_nr_interface::dl_sched_t dl_res;
        sched_nr_interface::ul_sched_t ul_res;
        TESTASSERT(sched.get_dl_sched(tti, cc, dl_res) == SRSRAN_SUCCESS);
        TESTASSERT(sched.get_ul_sched(tti, cc, ul_res) == SRSRAN_SUCCESS);
        tasks.finish_cc(tti, dl_res, ul_res);
      });
    }
  }
  auto tp2 = std::chrono::steady_clock::now();

  tasks.wait_task_finish();

  tasks.print_results();

  uint32_t microsecs = std::chrono::duration_cast<std::chrono::microseconds>(tp2 - tp1).count();
  printf("Total time taken per slot [usec]: %f\n", microsecs / (float)max_nof_ttis);
}

} // namespace srsenb

int main()
{
  auto& test_logger = srslog::fetch_basic_logger("TEST");
  test_logger.set_level(srslog::basic_levels::debug);
  auto& mac_logger = srslog::fetch_basic_logger("MAC");
  mac_logger.set_level(srslog::basic_levels::debug);
  auto& pool_logger = srslog::fetch_basic_logger("POOL");
  pool_logger.set_level(srslog::basic_levels::debug);

  // Start the log backend.
  srslog::init();

  srsran::get_background_workers().set_nof_workers(8);

  srsenb::sched_nr_cfg_serialized_test();
  srsenb::sched_nr_cfg_parallel_cc_test();
  srsenb::sched_nr_cfg_parallel_sf_test();
}