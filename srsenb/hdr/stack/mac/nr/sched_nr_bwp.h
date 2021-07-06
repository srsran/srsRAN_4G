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
  explicit ra_sched(const sched_cell_params& cell_cfg_);

  int    dl_rach_info(const dl_sched_rar_info_t& rar_info);
  void   run_slot(bwp_slot_allocator& slot_grid);
  size_t empty() const { return pending_rars.empty(); }

private:
  alloc_result
  allocate_pending_rar(bwp_slot_allocator& slot_grid, const pending_rar_t& rar, uint32_t& nof_grants_alloc);

  const sched_cell_params* cell_cfg = nullptr;
  srslog::basic_logger&    logger;

  srsran::deque<pending_rar_t> pending_rars;
};

class bwp_sched
{
public:
  explicit bwp_sched(const sched_cell_params& cell_cfg_, uint32_t bwp_id_);

  const sched_cell_params* cell_cfg;
  const uint32_t           bwp_id;

  // channel-specific schedulers
  ra_sched ra;

  // Pending allocations
  bwp_res_grid grid;
};

class cell_sched
{
public:
  srsran::bounded_vector<bwp_sched, SCHED_NR_MAX_BWP_PER_CELL> bwps;

  explicit cell_sched(const sched_cell_params& cell_cfg_);

  const sched_cell_params* cfg;
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_BWP_H
