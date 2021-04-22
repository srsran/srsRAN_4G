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

#ifndef SRSUE_NR_WORKER_POOL_H
#define SRSUE_NR_WORKER_POOL_H

#include "sf_worker.h"
#include "srsran/common/thread_pool.h"
#include "srsue/hdr/phy/prach.h"

namespace srsue {
namespace nr {

class worker_pool
{
private:
  srslog::basic_logger&                    logger;
  srsran::thread_pool                      pool;
  std::vector<std::unique_ptr<sf_worker> > workers;
  state                                    phy_state;
  std::unique_ptr<prach>                   prach_buffer = nullptr;

public:
  sf_worker* operator[](std::size_t pos) { return workers.at(pos).get(); }

  worker_pool(uint32_t max_workers);
  bool       init(const phy_args_nr_t& args_, phy_common* common, stack_interface_phy_nr* stack_, int prio);
  sf_worker* wait_worker(uint32_t tti);
  void       start_worker(sf_worker* w);
  void       stop();
  void       send_prach(uint32_t prach_occasion, uint32_t preamble_index, int preamble_received_target_power);
  int  set_ul_grant(std::array<uint8_t, SRSRAN_RAR_UL_GRANT_NBITS> array, uint16_t rnti, srsran_rnti_type_t rnti_type);
  bool set_config(const srsran::phy_cfg_nr_t& cfg);
  bool       has_valid_sr_resource(uint32_t sr_id);
  void       clear_pending_grants();
  void       get_metrics(phy_metrics_t& m);
};

} // namespace nr
} // namespace srsue

#endif // SRSUE_NR_WORKER_POOL_H
