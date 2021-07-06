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

#include "slot_worker.h"
#include "srsenb/hdr/phy/phy_interfaces.h"
#include "srsran/common/thread_pool.h"
#include "srsran/interfaces/gnb_interfaces.h"

namespace srsenb {
namespace nr {

class worker_pool
{
  srsran::phy_common_interface&              common;
  stack_interface_phy_nr&                    stack;
  srslog::sink&                              log_sink;
  srsran::thread_pool                        pool;
  std::vector<std::unique_ptr<slot_worker> > workers;

public:
  struct args_t {
    uint32_t               nof_phy_threads    = 3;
    uint32_t               prio               = 52;
    uint32_t               pusch_max_nof_iter = 10;
    srsran::phy_log_args_t log                = {};
  };
  slot_worker* operator[](std::size_t pos) { return workers.at(pos).get(); }

  worker_pool(srsran::phy_common_interface& common,
              stack_interface_phy_nr&       stack,
              srslog::sink&                 log_sink,
              uint32_t                      max_workers);
  bool         init(const args_t& args, const phy_cell_cfg_list_nr_t& cell_list);
  slot_worker* wait_worker(uint32_t tti);
  slot_worker* wait_worker_id(uint32_t id);
  void         start_worker(slot_worker* w);
  void         stop();
};

} // namespace nr
} // namespace srsenb

#endif // SRSENB_NR_WORKER_POOL_H
