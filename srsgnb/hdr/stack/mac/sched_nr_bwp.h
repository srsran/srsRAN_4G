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

#ifndef SRSRAN_SCHED_NR_BWP_H
#define SRSRAN_SCHED_NR_BWP_H

#include "sched_nr_cfg.h"
#include "sched_nr_grant_allocator.h"
#include "sched_nr_signalling.h"
#include "sched_nr_time_rr.h"
#include "srsran/adt/pool/cached_alloc.h"

namespace srsenb {
namespace sched_nr_impl {

using dl_sched_rar_info_t = sched_nr_interface::rar_info_t;

/// RAR/Msg3 scheduler
class ra_sched
{
public:
  explicit ra_sched(const bwp_params_t& bwp_cfg_);

  /// Addition of detected PRACH into the queue
  int dl_rach_info(const dl_sched_rar_info_t& rar_info);

  /// Allocate pending RARs
  void run_slot(bwp_slot_allocator& slot_alloc);

  /// Check if there are pending RARs
  bool empty() const { return pending_rars.empty(); }

private:
  struct pending_rar_t {
    uint16_t                                                ra_rnti = 0;
    slot_point                                              prach_slot;
    slot_interval                                           rar_win;
    srsran::bounded_vector<dl_sched_rar_info_t, MAX_GRANTS> msg3_grant;
  };

  alloc_result
  allocate_pending_rar(bwp_slot_allocator& slot_grid, const pending_rar_t& rar, uint32_t& nof_grants_alloc);

  const bwp_params_t*   bwp_cfg = nullptr;
  srslog::basic_logger& logger;

  srsran::deque<pending_rar_t> pending_rars;
};

class bwp_manager
{
public:
  explicit bwp_manager(const bwp_params_t& bwp_cfg);

  const bwp_params_t* cfg;

  // channel-specific schedulers
  si_sched                       si;
  ra_sched                       ra;
  std::unique_ptr<sched_nr_base> data_sched;

  // Stores pending allocations and PRB bitmaps
  bwp_res_grid grid;
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_BWP_H
