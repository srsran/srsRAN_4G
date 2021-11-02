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

#ifndef SRSRAN_SCHED_NR_WORKER_H
#define SRSRAN_SCHED_NR_WORKER_H

#include "sched_nr_bwp.h"
#include "sched_nr_cfg.h"
#include "sched_nr_grant_allocator.h"
#include "sched_nr_ue.h"
#include "srsran/adt/circular_array.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/pool/cached_alloc.h"
#include "srsran/adt/span.h"
#include <condition_variable>
#include <mutex>

namespace srsenb {

struct mac_metrics_t;

namespace sched_nr_impl {

class cc_worker
{
public:
  explicit cc_worker(const cell_params_t& params);

  void dl_rach_info(const sched_nr_interface::rar_info_t& rar_info);

  dl_sched_res_t* run_slot(slot_point pdcch_slot, ue_map_t& ue_db_);
  ul_sched_t*     get_ul_sched(slot_point sl);

  // const params
  const cell_params_t&  cfg;
  srslog::basic_logger& logger;

  // cc-specific resources
  srsran::bounded_vector<bwp_manager, SCHED_NR_MAX_BWP_PER_CELL> bwps;

private:
  void alloc_dl_ues(bwp_slot_allocator& bwp_alloc);
  void alloc_ul_ues(bwp_slot_allocator& bwp_alloc);
  void postprocess_decisions(bwp_slot_allocator& bwp_alloc);

  // {slot,cc} specific variables
  slot_ue_map_t slot_ues;
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_WORKER_H
