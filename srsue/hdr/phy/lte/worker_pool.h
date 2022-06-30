/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#ifndef SRSUE_LTE_WORKER_POOL_H
#define SRSUE_LTE_WORKER_POOL_H

#include "sf_worker.h"
#include "srsran/common/thread_pool.h"

namespace srsue {
namespace lte {

class worker_pool
{
private:
  srsran::thread_pool                      pool;
  std::vector<std::unique_ptr<sf_worker> > workers;

  class phy_cfg_stash_t
  {
  private:
    std::vector<bool> pending; ///< Indicates for each SF worker if it has pending configuration
    srsran::phy_cfg_t cfg;     ///< Actual CC configuration

  public:
    phy_cfg_stash_t(uint32_t max_workers) : pending(max_workers) {}
    void                     set_cfg(const srsran::phy_cfg_t& c);
    bool                     is_pending(uint32_t sf_idx);
    const srsran::phy_cfg_t& get_cfg(uint32_t sf_idx);
  };
  std::mutex                                       phy_cfg_mutex; ///< Protects configuration stash
  std::array<phy_cfg_stash_t, SRSRAN_MAX_CARRIERS> phy_cfg_stash; ///< Stores the latest worker configuration

public:
  sf_worker* operator[](std::size_t pos) { return workers.at(pos).get(); }

  worker_pool(uint32_t max_workers);
  bool       init(phy_common* common, int prio);
  sf_worker* wait_worker(uint32_t tti);
  sf_worker* wait_worker_id(uint32_t id);
  void       start_worker(sf_worker* w);
  void       stop();

  /**
   * @brief Sets a new configuration for a given CC, it copies the new configuration into the stash and it will be
   * applied to the sf_worker at the time it is reserved.
   * @param cc_idx CC index
   * @param phy_cfg Actual PHY configuration
   */
  void set_config(uint32_t cc_idx, const srsran::phy_cfg_t& phy_cfg);
};

} // namespace lte
} // namespace srsue

#endif // SRSUE_LTE_WORKER_POOL_H
