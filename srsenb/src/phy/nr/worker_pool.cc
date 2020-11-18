/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */
#include "srsenb/hdr/phy/nr/worker_pool.h"

namespace srsenb {
namespace nr {

worker_pool::worker_pool(uint32_t max_workers) : pool(max_workers) {}

bool worker_pool::init(const phy_args_t& args, phy_common* common, srslte::logger* logger, int prio)
{
  // Create logs
  // Create array of pointers to phy_logs
  for (uint32_t i = 0; i < args.nof_phy_threads; i++) {
    auto* mylog = new srslte::log_filter;
    char  tmp[16];
    sprintf(tmp, "PHY%d", i);
    mylog->init(tmp, logger, true);
    mylog->set_level(args.log.phy_level);
    mylog->set_hex_limit(args.log.phy_hex_limit);
    log_vec.push_back(std::unique_ptr<srslte::log_filter>(mylog));
  }

  // Add workers to workers pool and start threads
  for (uint32_t i = 0; i < args.nof_phy_threads; i++) {
    auto w = std::unique_ptr<sf_worker>(new sf_worker(common, &phy_state, (srslte::log*)log_vec[i].get()));
    pool.init_worker(i, w.get(), prio);
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

}; // namespace nr
}; // namespace srsenb