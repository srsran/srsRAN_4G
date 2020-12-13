/**
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

#ifndef SRSUE_NR_WORKER_POOL_H
#define SRSUE_NR_WORKER_POOL_H

#include "sf_worker.h"
#include "srslte/common/thread_pool.h"

namespace srsue {
namespace nr {

class worker_pool
{

private:
  std::vector<std::unique_ptr<srslte::log_filter> > log_vec;

  srslte::thread_pool                      pool;
  std::vector<std::unique_ptr<sf_worker> > workers;
  phy_nr_state                             phy_state;

public:
  sf_worker* operator[](std::size_t pos) { return workers.at(pos).get(); }

  worker_pool(uint32_t max_workers);
  bool       init(phy_common* common, srslte::logger* logger, int prio);
  sf_worker* wait_worker(uint32_t tti);
  sf_worker* wait_worker_id(uint32_t id);
  void       start_worker(sf_worker* w);
  void       stop();
};

} // namespace nr
} // namespace srsue

#endif // SRSUE_NR_WORKER_POOL_H
