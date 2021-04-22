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
#include "srsue/hdr/phy/lte/worker_pool.h"

namespace srsue {
namespace lte {

worker_pool::worker_pool(uint32_t max_workers) : pool(max_workers) {}

bool worker_pool::init(phy_common* common, int prio)
{
  // Add workers to workers pool and start threads
  for (uint32_t i = 0; i < common->args->nof_phy_threads; i++) {
    srslog::basic_logger& log = srslog::fetch_basic_logger(fmt::format("PHY{}", i));
    log.set_level(srslog::str_to_basic_level(common->args->log.phy_level));
    log.set_hex_dump_max_size(common->args->log.phy_hex_limit);

    auto w = std::unique_ptr<lte::sf_worker>(new lte::sf_worker(SRSRAN_MAX_PRB, common, log));
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