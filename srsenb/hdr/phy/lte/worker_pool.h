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

#ifndef SRSENB_LTE_WORKER_POOL_H
#define SRSENB_LTE_WORKER_POOL_H

#include "sf_worker.h"
#include "srsran/common/thread_pool.h"

namespace srsenb {
namespace lte {

class worker_pool
{
  srsran::thread_pool                      pool;
  std::vector<std::unique_ptr<sf_worker> > workers;

public:
  sf_worker* operator[](std::size_t pos) { return workers.at(pos).get(); }
  uint32_t   get_nof_workers() { return (uint32_t)workers.size(); }

  worker_pool(uint32_t max_workers);
  bool       init(const phy_args_t& args, phy_common* common, srslog::sink& log_sink, int prio);
  sf_worker* wait_worker(uint32_t tti);
  sf_worker* wait_worker_id(uint32_t id);
  void       start_worker(sf_worker* w);
  void       stop();
};

} // namespace lte
} // namespace srsenb

#endif // SRSENB_LTE_WORKER_POOL_H
