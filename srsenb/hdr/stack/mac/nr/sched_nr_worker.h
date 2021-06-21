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

class bwp_worker
{
public:
  explicit bwp_worker(uint32_t cc_, ue_map_t& ue_db_) : cc(cc_), ue_db(ue_db_) {}

  void start(tti_point tti_rx_);
  void run();
  void end_tti();
  bool running() const { return tti_rx.is_valid(); }

private:
  ue_map_t& ue_db;

  tti_point tti_rx;
  uint32_t  cc;

  srsran::circular_array<bwp_ue, SCHED_NR_MAX_USERS> bwp_ues;
};

class sched_worker_manager
{
public:
  explicit sched_worker_manager(ue_map_t& ue_db_, const sched_nr_cfg& cfg_);
  sched_worker_manager(const sched_worker_manager&) = delete;
  sched_worker_manager(sched_worker_manager&&)      = delete;
  ~sched_worker_manager();

  void reserve_workers(tti_point tti_rx, srsran::span<sched_nr_res_t> sf_result_);
  void start_tti(tti_point tti_rx);
  bool run_tti(tti_point tti_rx, uint32_t cc);
  void end_tti(tti_point tti_rx);

private:
  const sched_nr_cfg& cfg;

  struct sf_worker_ctxt {
    sem_t                        sf_sem;
    tti_point                    tti_rx;
    srsran::span<sched_nr_res_t> sf_result;
    int                          worker_count = 0;
    std::vector<bwp_worker>      workers;
  };
  std::vector<std::unique_ptr<sf_worker_ctxt> > sf_ctxts;

  sf_worker_ctxt& get_sf(tti_point tti_rx);
};

} // namespace sched_nr_impl
} // namespace srsenb

#endif // SRSRAN_SCHED_NR_WORKER_H
