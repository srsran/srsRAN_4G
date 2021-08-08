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

#ifndef SRSRAN_SCHED_NR_WORKER_H
#define SRSRAN_SCHED_NR_WORKER_H

#include "sched_nr_cell.h"
#include "sched_nr_cfg.h"
#include "sched_nr_rb_grid.h"
#include "sched_nr_ue.h"
#include "srsran/adt/circular_array.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/pool/cached_alloc.h"
#include "srsran/adt/span.h"
#include <condition_variable>
#include <mutex>

namespace srsenb {
namespace sched_nr_impl {

using dl_sched_t = sched_nr_interface::dl_sched_t;
using ul_sched_t = sched_nr_interface::ul_sched_t;

class slot_cc_worker
{
public:
  using feedback_callback_t = srsran::move_callback<void(ue_carrier&)>;

  explicit slot_cc_worker(serv_cell_manager& sched);

  void start(slot_point pdcch_slot, ue_map_t& ue_db_);
  void run();
  void finish();
  bool running() const { return slot_rx.valid(); }

  void enqueue_cc_event(srsran::move_callback<void()> ev);

  /// Enqueue feedback directed at a given UE in a given cell
  void enqueue_cc_feedback(uint16_t rnti, feedback_callback_t fdbk);

private:
  /// Run all pending feedback. This should be called at the beginning of a TTI
  void run_feedback(ue_map_t& ue_db);

  void alloc_dl_ues();
  void alloc_ul_ues();
  void log_result() const;

  const sched_cell_params& cfg;
  serv_cell_manager&       cell;
  srslog::basic_logger&    logger;

  slot_point         slot_rx;
  bwp_slot_allocator bwp_alloc;

  // Process of UE cell-specific feedback
  struct feedback_t {
    uint16_t            rnti;
    feedback_callback_t fdbk;
  };
  std::mutex                                    feedback_mutex;
  srsran::deque<feedback_t>                     pending_feedback, tmp_feedback_to_run;
  srsran::deque<srsran::move_callback<void()> > pending_events, tmp_events_to_run;

  srsran::static_circular_map<uint16_t, slot_ue, SCHED_NR_MAX_USERS> slot_ues;
};

class sched_worker_manager
{
  struct slot_worker_ctxt {
    std::mutex                  slot_mutex; // lock of all workers of the same slot.
    std::condition_variable     cvar;
    slot_point                  slot_rx;
    int                         nof_workers_waiting = 0;
    std::atomic<int>            worker_count{0}; // variable shared across slot_cc_workers
    std::vector<slot_cc_worker> workers;
  };

public:
  explicit sched_worker_manager(ue_map_t&                                         ue_db_,
                                const sched_params&                               cfg_,
                                srsran::span<std::unique_ptr<serv_cell_manager> > cells_);
  sched_worker_manager(const sched_worker_manager&) = delete;
  sched_worker_manager(sched_worker_manager&&)      = delete;
  ~sched_worker_manager();

  void run_slot(slot_point slot_tx, uint32_t cc, dl_sched_t& dl_res, ul_sched_t& ul_res);

  void enqueue_event(uint16_t rnti, srsran::move_callback<void()> ev);
  void enqueue_cc_event(uint32_t cc, srsran::move_callback<void()> ev);
  void enqueue_cc_feedback(uint16_t rnti, uint32_t cc, slot_cc_worker::feedback_callback_t fdbk)
  {
    cc_worker_list[cc]->worker.enqueue_cc_feedback(rnti, std::move(fdbk));
  }

private:
  bool save_sched_result(slot_point pdcch_slot, uint32_t cc, dl_sched_t& dl_res, ul_sched_t& ul_res);

  const sched_params&                               cfg;
  ue_map_t&                                         ue_db;
  srsran::span<std::unique_ptr<serv_cell_manager> > cells;
  srslog::basic_logger&                             logger;

  struct ue_event_t {
    uint16_t                      rnti;
    srsran::move_callback<void()> callback;
  };
  std::mutex                event_mutex;
  srsran::deque<ue_event_t> next_slot_events, slot_events;

  std::vector<std::unique_ptr<slot_worker_ctxt> > slot_worker_ctxts;
  struct cc_context {
    std::condition_variable cvar;
    int                     waiting = 0;
    slot_cc_worker          worker;

    cc_context(serv_cell_manager& sched) : worker(sched) {}
  };

  std::mutex                                slot_mutex;
  std::condition_variable                   cvar;
  slot_point                                current_slot;
  std::atomic<int>                          worker_count{0}; // variable shared across slot_cc_workers
  std::vector<std::unique_ptr<cc_context> > cc_worker_list;
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_WORKER_H
