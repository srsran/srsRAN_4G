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
#include "srsue/hdr/phy/nr/worker_pool.h"

namespace srsue {
namespace nr {

worker_pool::worker_pool(uint32_t max_workers) : pool(max_workers) {}

bool worker_pool::init(phy_common* common, srslog::sink& log_sink, int prio)
{
  // Set carrier attributes
  phy_state.carrier.id      = 500;
  phy_state.carrier.nof_prb = common->args->nr_nof_prb;

  // Set NR arguments
  phy_state.args.nof_carriers   = common->args->nof_nr_carriers;
  phy_state.args.dl.nof_max_prb = common->args->nr_nof_prb;

  // Skip init of workers if no NR carriers
  if (phy_state.args.nof_carriers == 0) {
    return true;
  }

  // Add workers to workers pool and start threads
  for (uint32_t i = 0; i < common->args->nof_phy_threads; i++) {
    auto& log = srslog::fetch_basic_logger(fmt::format("PHY{}", i), log_sink);
    log.set_level(srslog::str_to_basic_level(common->args->log.phy_level));
    log.set_hex_dump_max_size(common->args->log.phy_hex_limit);

    auto w = new sf_worker(common, &phy_state, log);
    pool.init_worker(i, w, prio, common->args->worker_cpu_mask);
    workers.push_back(std::unique_ptr<sf_worker>(w));

    if (not w->set_carrier_unlocked(0, &phy_state.carrier)) {
      return false;
    }
  }

  // Initialise PRACH
  auto& prach_log = srslog::fetch_basic_logger("NR-PRACH", log_sink);
  prach_log.set_level(srslog::str_to_basic_level(common->args->log.phy_level));
  prach_buffer = std::unique_ptr<prach>(new prach(prach_log));
  prach_buffer->init(phy_state.args.dl.nof_max_prb);

  // Set PRACH hard-coded cell
  srslte_cell_t cell = {};
  cell.nof_prb       = 50;
  cell.id            = phy_state.carrier.id;
  if (not prach_buffer->set_cell(cell, phy_state.cfg.prach)) {
    prach_log.error("Setting PRACH cell");
    return false;
  }

  return true;
}

void worker_pool::start_worker(sf_worker* w)
{
  pool.start_worker(w);
}

sf_worker* worker_pool::wait_worker(uint32_t tti)
{
  sf_worker* worker = (sf_worker*)pool.wait_worker(tti);

  // Prepare PRACH, send always sequence 0
  prach_buffer->prepare_to_send(0);

  // Generate PRACH if ready
  if (prach_buffer->is_ready_to_send(tti, phy_state.carrier.id)) {
    uint32_t nof_prach_sf       = 0;
    float    prach_target_power = 0.0f;
    cf_t*    prach_ptr          = prach_buffer->generate(0.0f, &nof_prach_sf, &prach_target_power);
    worker->set_prach(prach_ptr, prach_target_power);
  }

  return worker;
}

void worker_pool::stop()
{
  pool.stop();
}

} // namespace nr
} // namespace srsue