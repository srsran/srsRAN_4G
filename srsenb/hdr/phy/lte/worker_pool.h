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

#ifndef SRSENB_LTE_WORKER_POOL_H
#define SRSENB_LTE_WORKER_POOL_H

#include "sf_worker.h"
#include "srslte/common/thread_pool.h"

namespace srsenb {
namespace lte {

class worker_pool
{
  srslte::thread_pool                      pool;
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
