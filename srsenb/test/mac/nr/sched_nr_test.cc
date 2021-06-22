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

#include "srsenb/hdr/stack/mac/nr/sched_nr.h"
#include "srsran/common/test_common.h"
#include "srsran/common/thread_pool.h"

namespace srsenb {

struct task_job_manager {
  std::mutex              mutex;
  std::condition_variable cond_var;
  int                     tasks       = 0;
  int                     res_count   = 0;
  int                     pdsch_count = 0;
  int                     max_tasks   = std::numeric_limits<int>::max() / 2;

  void start_task()
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (tasks >= max_tasks) {
      cond_var.wait(lock);
    }
    tasks++;
  }
  void finish_task(const sched_nr_res_t& res)
  {
    std::unique_lock<std::mutex> lock(mutex);
    TESTASSERT(res.dl_res.data.size() <= 1);
    res_count++;
    pdsch_count += res.dl_res.data.size();
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
  void print_results() const { printf("TESTER: %f PDSCH/{slot,cc} were allocated\n", pdsch_count / (double)res_count); }
};

void sched_nr_cfg_serialized_test()
{
  uint32_t         max_nof_ttis = 1000;
  task_job_manager tasks;

  sched_nr_cfg                   cfg;
  std::vector<sched_nr_cell_cfg> cells_cfg;
  cells_cfg.resize(1);

  sched_nr sched(cfg);
  sched.cell_cfg(cells_cfg);

  sched_nr_ue_cfg uecfg;
  uecfg.carriers.resize(1);
  uecfg.carriers[0].active = true;
  sched.ue_cfg(0x46, uecfg);

  for (uint32_t nof_ttis = 0; nof_ttis < max_nof_ttis; ++nof_ttis) {
    tti_point tti(nof_ttis % 10240);
    sched.new_tti(tti);
    for (uint32_t cc = 0; cc < cells_cfg.size(); ++cc) {
      tasks.start_task();
      sched_nr_res_t res;
      TESTASSERT(sched.generate_sched_result(tti, cc, res) == SRSRAN_SUCCESS);
      tasks.finish_task(res);
    }
  }

  tasks.print_results();
}

void sched_nr_cfg_parallel_cc_test()
{
  uint32_t         max_nof_ttis = 1000;
  task_job_manager tasks;

  sched_nr_cfg                   cfg;
  std::vector<sched_nr_cell_cfg> cells_cfg;
  cells_cfg.resize(4);

  sched_nr sched(cfg);
  sched.cell_cfg(cells_cfg);

  sched_nr_ue_cfg uecfg;
  uecfg.carriers.resize(cells_cfg.size());
  for (uint32_t cc = 0; cc < cells_cfg.size(); ++cc) {
    uecfg.carriers[cc].active = true;
  }
  sched.ue_cfg(0x46, uecfg);

  for (uint32_t nof_ttis = 0; nof_ttis < max_nof_ttis; ++nof_ttis) {
    tti_point tti(nof_ttis % 10240);
    sched.new_tti(tti);
    for (uint32_t cc = 0; cc < cells_cfg.size(); ++cc) {
      tasks.start_task();
      srsran::get_background_workers().push_task([cc, &sched, tti, &tasks]() {
        sched_nr_res_t res;
        TESTASSERT(sched.generate_sched_result(tti, cc, res) == SRSRAN_SUCCESS);
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

  sched_nr_cfg cfg;
  cfg.nof_concurrent_subframes = 2;
  std::vector<sched_nr_cell_cfg> cells_cfg;
  cells_cfg.resize(nof_sectors);

  sched_nr sched(cfg);
  sched.cell_cfg(cells_cfg);

  sched_nr_ue_cfg uecfg;
  uecfg.carriers.resize(cells_cfg.size());
  for (uint32_t cc = 0; cc < cells_cfg.size(); ++cc) {
    uecfg.carriers[cc].active = true;
  }
  sched.ue_cfg(0x46, uecfg);

  for (uint32_t nof_ttis = 0; nof_ttis < max_nof_ttis; ++nof_ttis) {
    tti_point tti(nof_ttis % 10240);
    sched.new_tti(tti);
    for (uint32_t cc = 0; cc < cells_cfg.size(); ++cc) {
      tasks.start_task();
      srsran::get_background_workers().push_task([cc, &sched, tti, &tasks]() {
        sched_nr_res_t res;
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
  auto& mac_logger = srslog::fetch_basic_logger("MAC");
  mac_logger.set_level(srslog::basic_levels::debug);
  auto& pool_logger = srslog::fetch_basic_logger("POOL");
  pool_logger.set_level(srslog::basic_levels::debug);

  srsran::get_background_workers().set_nof_workers(8);

  srsenb::sched_nr_cfg_serialized_test();
  srsenb::sched_nr_cfg_parallel_cc_test();
  srsenb::sched_nr_cfg_parallel_sf_test();
}