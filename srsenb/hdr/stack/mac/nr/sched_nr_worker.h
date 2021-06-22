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

#include "sched_nr_common.h"
#include "sched_nr_rb_grid.h"
#include "sched_nr_ue.h"
#include "srsran/adt/circular_array.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/pool/cached_alloc.h"
#include "srsran/adt/span.h"
#include <condition_variable>
#include <mutex>
#include <semaphore.h>

namespace srsenb {
namespace sched_nr_impl {

class slot_cc_worker
{
public:
  explicit slot_cc_worker(const sched_cell_params& cell_params) : cfg(cell_params), res_grid(cfg) {}

  void start(tti_point tti_rx_, sched_nr_res_t& bwp_result, ue_map_t& ue_db_);
  void run();
  void end_tti();
  bool running() const { return tti_rx.is_valid(); }

private:
  void alloc_dl_ues();
  void alloc_ul_ues();

  const sched_cell_params& cfg;

  tti_point tti_rx;
  slot_grid res_grid;

  srsran::static_circular_map<uint16_t, slot_ue, SCHED_NR_MAX_USERS> slot_ues;
};

class sched_worker_manager
{
public:
  explicit sched_worker_manager(ue_map_t& ue_db_, const sched_params& cfg_);
  sched_worker_manager(const sched_worker_manager&) = delete;
  sched_worker_manager(sched_worker_manager&&)      = delete;
  ~sched_worker_manager();

  void reserve_workers(tti_point tti_rx, srsran::span<sched_nr_res_t> sf_result_);
  void start_tti(tti_point tti_rx);
  bool run_tti(tti_point tti_rx, uint32_t cc, sched_nr_res_t& result);
  void end_tti(tti_point tti_rx);

private:
  const sched_params& cfg;
  ue_map_t&           ue_db;

  struct slot_worker_ctxt {
    sem_t                        sf_sem; // lock of all workers of the same slot. unlocked by last slot_cc_worker
    tti_point                    tti_rx;
    srsran::span<sched_nr_res_t> sf_result;
    std::atomic<int>             worker_count{0}; // variable shared across slot_cc_workers
    std::vector<slot_cc_worker>  workers;
  };
  std::vector<std::unique_ptr<slot_worker_ctxt> > slot_ctxts;

  slot_worker_ctxt& get_sf(tti_point tti_rx);
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_WORKER_H
