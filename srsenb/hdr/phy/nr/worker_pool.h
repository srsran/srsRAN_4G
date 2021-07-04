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
    uint32_t    nof_phy_threads    = 3;
    uint32_t    prio               = 52;
    std::string log_level          = "info";
    uint32_t    log_hex_limit      = 64;
    std::string log_id_preamble    = "";
    uint32_t    pusch_max_nof_iter = 10;
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
