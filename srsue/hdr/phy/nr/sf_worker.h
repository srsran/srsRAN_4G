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

#ifndef SRSUE_NR_PHCH_WORKER_H
#define SRSUE_NR_PHCH_WORKER_H

#include "../phy_common.h"
#include "cc_worker.h"
#include "srsran/common/thread_pool.h"

namespace srsue {
namespace nr {

/**
 * The sf_worker class handles the PHY processing, UL and DL procedures associated with 1 subframe.
 * It contains multiple cc_worker objects, one for each component carrier which may be executed in
 * one or multiple threads.
 *
 * A sf_worker object is executed by a thread within the thread_pool.
 */

class sf_worker final : public srsran::thread_pool::worker
{
public:
  sf_worker(phy_common* phy, state* phy_state_, srslog::basic_logger& logger);
  ~sf_worker() = default;

  bool update_cfg(uint32_t cc_idx);

  /* Functions used by main PHY thread */
  cf_t*    get_buffer(uint32_t cc_idx, uint32_t antenna_idx);
  uint32_t get_buffer_len();
  void     set_tti(uint32_t tti);
  int      read_pdsch_d(cf_t* pdsch_d);
  void     start_plot();

  void set_prach(cf_t* prach_ptr, float prach_power);

private:
  /* Inherited from thread_pool::worker. Function called every subframe to run the DL/UL processing */
  void work_imp() override;

  std::vector<std::unique_ptr<cc_worker> > cc_workers;

  phy_common*           phy       = nullptr;
  state*                phy_state = nullptr;
  srslog::basic_logger& logger;

  uint32_t tti_rx      = 0;
  cf_t*    prach_ptr   = nullptr;
  float    prach_power = 0;
};

} // namespace nr
} // namespace srsue

#endif // SRSUE_NR_PHCH_WORKER_H
