/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSENB_NR_WORKER_POOL_H
#define SRSENB_NR_WORKER_POOL_H

#include "phy_nr_state.h"
#include "sf_worker.h"
#include "srsenb/hdr/phy/phy_interfaces.h"
#include "srsran/common/thread_pool.h"

namespace srsenb {
namespace nr {

class worker_pool
{
  srsran::thread_pool                      pool;
  std::vector<std::unique_ptr<sf_worker> > workers;
  phy_nr_state                             phy_state;

public:
  struct args_t {
    uint32_t    nof_workers   = 3;
    uint32_t    prio          = 52;
    std::string log_level     = "info";
    uint32_t    log_hex_limit = 64;
  };
  sf_worker* operator[](std::size_t pos) { return workers.at(pos).get(); }

  worker_pool(const phy_cell_cfg_list_nr_t& cell_list,
              const args_t&                 args,
              srsran::phy_common_interface& common,
              stack_interface_phy_nr&       stack,
              srslog::sink&                 log_sink);
  sf_worker* wait_worker(uint32_t tti);
  sf_worker* wait_worker_id(uint32_t id);
  void       start_worker(sf_worker* w);
  void       stop();
};

} // namespace nr
} // namespace srsenb

#endif // SRSENB_NR_WORKER_POOL_H
