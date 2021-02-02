/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#include "srsue/hdr/phy/lte/worker_pool.h"

namespace srsue {
namespace lte {

worker_pool::worker_pool(uint32_t max_workers) : pool(max_workers) {}

bool worker_pool::init(phy_common* common, srslog::sink& log_sink, int prio)
{
  // Add workers to workers pool and start threads
  for (uint32_t i = 0; i < common->args->nof_phy_threads; i++) {
    srslog::basic_logger &log = srslog::fetch_basic_logger(fmt::format("PHY{}", i), log_sink);
    log.set_level(srslog::str_to_basic_level(common->args->log.phy_level));
    log.set_hex_dump_max_size(common->args->log.phy_hex_limit);

    auto w =
        std::unique_ptr<lte::sf_worker>(new lte::sf_worker(SRSLTE_MAX_PRB, common, log));
    pool.init_worker(i, w.get(), prio, common->args->worker_cpu_mask);
    workers.push_back(std::move(w));
  }

  return true;
}

void worker_pool::start_worker(sf_worker* w)
{
  pool.start_worker(w);
}

sf_worker* worker_pool::wait_worker(uint32_t tti)
{
  return (sf_worker*)pool.wait_worker(tti);
}

sf_worker* worker_pool::wait_worker_id(uint32_t id)
{
  return (sf_worker*)pool.wait_worker_id(id);
}

void worker_pool::stop()
{
  pool.stop();
}

}; // namespace lte
}; // namespace srsue