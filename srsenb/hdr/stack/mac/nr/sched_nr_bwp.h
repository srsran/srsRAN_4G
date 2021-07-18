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

#ifndef SRSRAN_SCHED_NR_BWP_H
#define SRSRAN_SCHED_NR_BWP_H

#include "sched_nr_cfg.h"
#include "sched_nr_rb_grid.h"
#include "srsran/adt/pool/cached_alloc.h"

namespace srsenb {
namespace sched_nr_impl {

using dl_sched_rar_info_t = sched_nr_interface::dl_sched_rar_info_t;

struct pending_rar_t {
  uint16_t                                                                   ra_rnti = 0;
  tti_point                                                                  prach_tti;
  srsran::bounded_vector<dl_sched_rar_info_t, sched_interface::MAX_RAR_LIST> msg3_grant;
};

/// RAR/Msg3 scheduler
class ra_sched
{
public:
  explicit ra_sched(const bwp_params& bwp_cfg_);

  int    dl_rach_info(const dl_sched_rar_info_t& rar_info);
  void   run_slot(bwp_slot_allocator& slot_grid);
  size_t empty() const { return pending_rars.empty(); }

private:
  alloc_result
  allocate_pending_rar(bwp_slot_allocator& slot_grid, const pending_rar_t& rar, uint32_t& nof_grants_alloc);

  const bwp_params*     bwp_cfg = nullptr;
  srslog::basic_logger& logger;

  srsran::deque<pending_rar_t> pending_rars;
};

class bwp_ctxt
{
public:
  explicit bwp_ctxt(const bwp_params& bwp_cfg);

  const bwp_params* cfg;

  // channel-specific schedulers
  ra_sched ra;

  // Stores pending allocations and PRB bitmaps
  bwp_res_grid grid;
};

class serv_cell_ctxt
{
public:
  srsran::bounded_vector<bwp_ctxt, SCHED_NR_MAX_BWP_PER_CELL> bwps;

  explicit serv_cell_ctxt(const sched_cell_params& cell_cfg_);

  const sched_cell_params* cfg;
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_BWP_H
