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

#ifndef SRSUE_NR_WORKER_POOL_H
#define SRSUE_NR_WORKER_POOL_H

#include "sf_worker.h"
#include "srslte/common/thread_pool.h"
#include "srsue/hdr/phy/prach.h"

namespace srsue {
namespace nr {

class worker_pool
{
private:
  srslte::thread_pool                      pool;
  std::vector<std::unique_ptr<sf_worker> > workers;
  state                                    phy_state;
  std::unique_ptr<prach>                   prach_buffer = nullptr;

public:
  sf_worker* operator[](std::size_t pos) { return workers.at(pos).get(); }

  worker_pool(uint32_t max_workers);
  bool       init(phy_common* common, srslog::sink& log_sink, int prio);
  sf_worker* wait_worker(uint32_t tti);
  void       start_worker(sf_worker* w);
  void       stop();
};

} // namespace nr
} // namespace srsue

#endif // SRSUE_NR_WORKER_POOL_H
