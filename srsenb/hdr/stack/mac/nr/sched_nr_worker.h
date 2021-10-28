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

/// Class to manage the locking, storing and processing of carrier-specific feedback (UE-specific or common)
class carrier_feedback_manager
{
public:
  using feedback_callback_t = srsran::move_callback<void(ue_carrier&)>;
  struct feedback_t {
    uint16_t            rnti;
    feedback_callback_t fdbk;
  };

  explicit carrier_feedback_manager(const cell_params_t& cell_cfg);

  /// Enqueue cell-specific event not directly at a given UE (e.g. PRACH)
  void enqueue_common_event(srsran::move_callback<void()> ev);

  /// Enqueue feedback directed at a given UE in a given cell (e.g. ACKs, CQI)
  void enqueue_ue_feedback(uint16_t rnti, feedback_callback_t fdbk);

  /// Run all pending feedback. This should be called at the beginning of a TTI
  void run(ue_map_t& ue_db);

private:
  const cell_params_t&  cfg;
  srslog::basic_logger& logger;

  std::mutex                                    feedback_mutex;
  srsran::deque<feedback_t>                     pending_feedback, tmp_feedback_to_run;
  srsran::deque<srsran::move_callback<void()> > pending_events, tmp_events_to_run;
};

class cc_worker
{
public:
  using feedback_callback_t = srsran::move_callback<void(ue_carrier&)>;

  explicit cc_worker(const cell_params_t& params);

  void run_slot(slot_point pdcch_slot, ue_map_t& ue_db_, dl_sched_res_t& dl_res, ul_sched_t& ul_res);

  // const params
  const cell_params_t&  cfg;
  srslog::basic_logger& logger;

  carrier_feedback_manager pending_feedback;

  // cc-specific resources
  srsran::bounded_vector<bwp_manager, SCHED_NR_MAX_BWP_PER_CELL> bwps;

private:
  /// Derive the remaining scheduling parameters and save result
  bool save_sched_result(dl_sched_res_t& dl_res, ul_sched_t& ul_res, slot_point slot_tx);

  void alloc_dl_ues(bwp_slot_allocator& bwp_alloc);
  void alloc_ul_ues(bwp_slot_allocator& bwp_alloc);
  void postprocess_decisions(bwp_slot_allocator& bwp_alloc);

  // {slot,cc} specific variables
  slot_ue_map_t slot_ues;
};

class sched_worker_manager
{
public:
  void get_metrics(mac_metrics_t& metrics);

private:
  void get_metrics_nolocking(mac_metrics_t& metrics);
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_WORKER_H
