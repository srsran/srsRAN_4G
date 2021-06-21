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

void sched_nr_cfg_serialized_test()
{
  sched_nr_cfg cfg;
  cfg.cells.resize(1);

  sched_nr sched(cfg);

  sched_nr_ue_cfg uecfg;
  uecfg.carriers.resize(1);
  uecfg.carriers[0].active = true;
  sched.ue_cfg(0x46, uecfg);

  for (uint32_t nof_ttis = 0; nof_ttis < 1000; ++nof_ttis) {
    tti_point tti(nof_ttis % 10240);
    sched.new_tti(tti);
    for (uint32_t cc = 0; cc < cfg.cells.size(); ++cc) {
      sched_nr_res_t res;
      TESTASSERT(sched.generate_sched_result(tti, cc, res) == SRSRAN_SUCCESS);
    }
  }
}

void sched_nr_cfg_parallel_cc_test()
{
  std::atomic<int> tasks{0};

  sched_nr_cfg cfg;
  cfg.cells.resize(4);

  sched_nr sched(cfg);

  sched_nr_ue_cfg uecfg;
  uecfg.carriers.resize(cfg.cells.size());
  for (uint32_t cc = 0; cc < cfg.cells.size(); ++cc) {
    uecfg.carriers[cc].active = true;
  }
  sched.ue_cfg(0x46, uecfg);

  for (uint32_t nof_ttis = 0; nof_ttis < 1000; ++nof_ttis) {
    tti_point tti(nof_ttis % 10240);
    sched.new_tti(tti);
    ++tasks;
    srsran::get_background_workers().push_task([&cfg, &sched, tti, &tasks]() {
      for (uint32_t cc = 0; cc < cfg.cells.size(); ++cc) {
        sched_nr_res_t res;
        TESTASSERT(sched.generate_sched_result(tti, cc, res) == SRSRAN_SUCCESS);
      }
      --tasks;
    });
  }

  while (tasks > 0) {
    usleep(100);
  }
}

void sched_nr_cfg_parallel_sf_test()
{
  uint32_t         nof_sectors = 2;
  std::atomic<int> tasks{0};

  sched_nr_cfg cfg;
  cfg.nof_concurrent_subframes = 2;
  cfg.cells.resize(nof_sectors);

  sched_nr sched(cfg);

  sched_nr_ue_cfg uecfg;
  uecfg.carriers.resize(cfg.cells.size());
  for (uint32_t cc = 0; cc < cfg.cells.size(); ++cc) {
    uecfg.carriers[cc].active = true;
  }
  sched.ue_cfg(0x46, uecfg);

  for (uint32_t nof_ttis = 0; nof_ttis < 1000; ++nof_ttis) {
    tti_point tti(nof_ttis % 10240);
    sched.new_tti(tti);
    ++tasks;
    srsran::get_background_workers().push_task([&cfg, &sched, tti, &tasks]() {
      for (uint32_t cc = 0; cc < cfg.cells.size(); ++cc) {
        sched_nr_res_t res;
        TESTASSERT(sched.generate_sched_result(tti, cc, res) == SRSRAN_SUCCESS);
      }
      --tasks;
    });
  }

  while (tasks > 0) {
    usleep(100);
  }
}

} // namespace srsenb

int main()
{
  srsran::get_background_workers().set_nof_workers(4);

  srsenb::sched_nr_cfg_serialized_test();
  srsenb::sched_nr_cfg_parallel_cc_test();
  srsenb::sched_nr_cfg_parallel_sf_test();
}