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

#include "sched_nr_sim_ue.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr.h"
#include "srsran/common/test_common.h"
#include "srsran/common/thread_pool.h"

namespace srsenb {

sched_nr_interface::ue_cfg_t get_default_ue_cfg(uint32_t nof_cc)
{
  sched_nr_interface::ue_cfg_t uecfg{};
  uecfg.carriers.resize(nof_cc);
  for (uint32_t cc = 0; cc < nof_cc; ++cc) {
    uecfg.carriers[cc].active = true;
  }
  uecfg.phy_cfg.pdcch.coreset_present[0] = true;
  uecfg.phy_cfg.pdcch.coreset[0].id      = 0;
  for (uint32_t i = 0; i < 100 / 6; ++i) {
    uecfg.phy_cfg.pdcch.coreset[0].freq_resources[i] = true;
  }
  uecfg.phy_cfg.pdcch.coreset[0].duration               = 1;
  uecfg.phy_cfg.pdcch.search_space_present[0]           = true;
  uecfg.phy_cfg.pdcch.search_space[0].id                = 0;
  uecfg.phy_cfg.pdcch.search_space[0].coreset_id        = 0;
  uecfg.phy_cfg.pdcch.search_space[0].duration          = 1;
  uecfg.phy_cfg.pdcch.search_space[0].type              = srsran_search_space_type_common_0;
  uecfg.phy_cfg.pdcch.search_space[0].nof_candidates[0] = 1;
  uecfg.phy_cfg.pdcch.search_space[0].nof_candidates[1] = 1;
  uecfg.phy_cfg.pdcch.search_space[0].nof_candidates[2] = 1;
  uecfg.phy_cfg.pdcch.search_space[0].nof_candidates[3] = 1;
  uecfg.phy_cfg.pdcch.search_space[0].nof_formats       = 1;
  uecfg.phy_cfg.pdcch.search_space[0].formats[0]        = srsran_dci_format_nr_0_0;
  return uecfg;
}

struct task_job_manager {
  std::mutex              mutex;
  std::condition_variable cond_var;
  int                     tasks       = 0;
  int                     res_count   = 0;
  int                     pdsch_count = 0;
  int                     max_tasks   = std::numeric_limits<int>::max() / 2;
  srslog::basic_logger&   test_logger = srslog::fetch_basic_logger("TEST");

  void start_task()
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (tasks >= max_tasks) {
      cond_var.wait(lock);
    }
    tasks++;
  }
  void finish_task(const sched_nr_interface::tti_request_t& res)
  {
    std::unique_lock<std::mutex> lock(mutex);
    TESTASSERT(res.dl_res.pdschs.size() <= 1);
    res_count++;
    pdsch_count += res.dl_res.pdschs.size();
    if (tasks-- >= max_tasks or tasks == 0) {
      cond_var.notify_one();
    }
  }
  void wait_task_finish()
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (tasks > 0) {
      cond_var.wait(lock);
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
  auto& mac_logger = srslog::fetch_basic_logger("MAC");

  uint32_t         max_nof_ttis = 1000, nof_sectors = 2;
  task_job_manager tasks;

  sched_nr_interface::sched_cfg_t             cfg;
  std::vector<sched_nr_interface::cell_cfg_t> cells_cfg(nof_sectors);

  sched_nr_sim_base sched_tester(cfg, cells_cfg, "Serialized Test");

  sched_nr_interface::ue_cfg_t uecfg = get_default_ue_cfg(2);

  sched_tester.add_user(0x46, uecfg, 0);

  for (uint32_t nof_ttis = 0; nof_ttis < max_nof_ttis; ++nof_ttis) {
    tti_point tti(nof_ttis % 10240);
    sched_tester.slot_indication(tti);
    for (uint32_t cc = 0; cc < cells_cfg.size(); ++cc) {
      tasks.start_task();
      sched_nr_interface::tti_request_t res;
      TESTASSERT(sched_tester.get_sched()->generate_sched_result(tti, cc, res) == SRSRAN_SUCCESS);
      sched_nr_cc_output_res_t out{tti, cc, &res.dl_res, &res.ul_res};
      sched_tester.update(out);
      tasks.finish_task(res);
      TESTASSERT(res.dl_res.pdschs.size() == 1);
    }
  }

  tasks.print_results();
  TESTASSERT(tasks.pdsch_count == (int)(max_nof_ttis * nof_sectors));
}

void sched_nr_cfg_parallel_cc_test()
{
  auto& mac_logger = srslog::fetch_basic_logger("MAC");

  uint32_t         max_nof_ttis = 1000;
  task_job_manager tasks;

  sched_nr_interface::sched_cfg_t             cfg;
  std::vector<sched_nr_interface::cell_cfg_t> cells_cfg(4);

  sched_nr_sim_base sched_tester(cfg, cells_cfg, "Parallel CC Test");

  sched_nr_interface::ue_cfg_t uecfg = get_default_ue_cfg(cells_cfg.size());
  sched_tester.add_user(0x46, uecfg, 0);

  for (uint32_t nof_ttis = 0; nof_ttis < max_nof_ttis; ++nof_ttis) {
    tti_point tti(nof_ttis % 10240);
    sched_tester.slot_indication(tti);
    for (uint32_t cc = 0; cc < cells_cfg.size(); ++cc) {
      tasks.start_task();
      srsran::get_background_workers().push_task([cc, tti, &tasks, &sched_tester]() {
        sched_nr_interface::tti_request_t res;
        TESTASSERT(sched_tester.get_sched()->generate_sched_result(tti, cc, res) == SRSRAN_SUCCESS);
        sched_nr_cc_output_res_t out{tti, cc, &res.dl_res, &res.ul_res};
        sched_tester.update(out);
        tasks.finish_task(res);
      });
    }
  }

  tasks.wait_task_finish();

  tasks.print_results();
}

void sched_nr_cfg_parallel_sf_test()
{
  uint32_t         max_nof_ttis = 1000;
  uint32_t         nof_sectors  = 2;
  task_job_manager tasks;

  sched_nr_interface::sched_cfg_t cfg;
  cfg.nof_concurrent_subframes = 2;
  std::vector<sched_nr_interface::cell_cfg_t> cells_cfg;
  cells_cfg.resize(nof_sectors);

  sched_nr sched(cfg);
  sched.cell_cfg(cells_cfg);

  sched_nr_interface::ue_cfg_t uecfg = get_default_ue_cfg(cells_cfg.size());
  sched.ue_cfg(0x46, uecfg);

  for (uint32_t nof_ttis = 0; nof_ttis < max_nof_ttis; ++nof_ttis) {
    tti_point tti(nof_ttis % 10240);
    sched.slot_indication(tti);
    for (uint32_t cc = 0; cc < cells_cfg.size(); ++cc) {
      tasks.start_task();
      srsran::get_background_workers().push_task([cc, &sched, tti, &tasks]() {
        sched_nr_interface::tti_request_t res;
        TESTASSERT(sched.generate_sched_result(tti, cc, res) == SRSRAN_SUCCESS);
        tasks.finish_task(res);
      });
    }
  }

  tasks.wait_task_finish();

  tasks.print_results();
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