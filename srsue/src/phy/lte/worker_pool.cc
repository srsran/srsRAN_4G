/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

void worker_pool::phy_cfg_stash_t::set_cfg(const srsran::phy_cfg_t& c)
{
  for (auto it = pending.begin(); it < pending.end(); it++) {
    *it = true;
  }

  cfg = c;
}

bool worker_pool::phy_cfg_stash_t::is_pending(uint32_t sf_idx)
{
  if (sf_idx >= (uint32_t)pending.size()) {
    return false;
  }
  return pending[sf_idx];
}

const srsran::phy_cfg_t& worker_pool::phy_cfg_stash_t::get_cfg(uint32_t sf_idx)
{
  if (sf_idx < (uint32_t)pending.size()) {
    pending[sf_idx] = false;
  }
  return cfg;
}

worker_pool::worker_pool(uint32_t max_workers) :
  pool(max_workers), phy_cfg_stash{{max_workers, max_workers, max_workers, max_workers, max_workers}}
{}

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
  sf_worker* w = (sf_worker*)pool.wait_worker(tti);
  if (w == nullptr) {
    return w;
  }

  // Protect configuration
  std::unique_lock<std::mutex> lock(phy_cfg_mutex);

  // Iterate all CC searching for a pending configuration
  uint32_t worker_id = w->get_id();
  for (uint32_t cc_idx = 0; cc_idx < SRSRAN_MAX_CARRIERS; cc_idx++) {
    if (phy_cfg_stash[cc_idx].is_pending(worker_id)) {
      w->set_config_nolock(cc_idx, phy_cfg_stash[cc_idx].get_cfg(worker_id));
    }
  }

  return w;
}

sf_worker* worker_pool::wait_worker_id(uint32_t id)
{
  return (sf_worker*)pool.wait_worker_id(id);
}

void worker_pool::stop()
{
  pool.stop();
}

void worker_pool::set_config(uint32_t cc_idx, const srsran::phy_cfg_t& phy_cfg)
{
  // Protect CC index bounds
  if (cc_idx >= SRSRAN_MAX_CARRIERS) {
    return;
  }

  // Protect configuration
  std::unique_lock<std::mutex> lock(phy_cfg_mutex);
  phy_cfg_stash[cc_idx].set_cfg(phy_cfg);
}
}; // namespace lte
}; // namespace srsue