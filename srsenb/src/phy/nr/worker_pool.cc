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
#include "srsenb/hdr/phy/nr/worker_pool.h"

namespace srsenb {
namespace nr {

worker_pool::worker_pool(uint32_t max_workers) : pool(max_workers) {}

bool worker_pool::init(const phy_args_t& args, phy_common* common, srslog::sink& log_sink, int prio)
{
  // Add workers to workers pool and start threads
  srslog::basic_levels log_level = srslog::str_to_basic_level(args.log.phy_level);
  for (uint32_t i = 0; i < args.nof_phy_threads; i++) {
    auto& log = srslog::fetch_basic_logger(fmt::format("PHY{}-NR", i), log_sink);
    log.set_level(log_level);
    log.set_hex_dump_max_size(args.log.phy_hex_limit);

    auto w = new sf_worker(common, &phy_state, log);
    pool.init_worker(i, w, prio);
    workers.push_back(std::unique_ptr<sf_worker>(w));

    srsran_carrier_nr_t c = common->get_cell_nr(0);
    w->set_carrier_unlocked(0, &c);
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

} // namespace nr
} // namespace srsenb