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

#include "srsenb/hdr/stack/mac/nr/sched_nr_worker.h"

namespace srsenb {
namespace sched_nr_impl {

/// Called at the beginning of TTI in a locked context, to reserve available UE resources
void bwp_worker::start(tti_point tti_rx_)
{
  srsran_assert(not running(), "scheduler worker::start() called for active worker");
  // Try reserve UE cells for this worker
  for (auto& ue_pair : ue_db) {
    uint16_t rnti = ue_pair.first;
    ue&      u    = *ue_pair.second;

    bwp_ue sfu0 = u.try_reserve(tti_rx, cc);
    if (sfu0.empty()) {
      // Failed to synchronize because UE is being used by another worker
      continue;
    }
    // Synchronization of UE for this {tti, cc} was successful
    bwp_ues[rnti] = std::move(sfu0);
  }

  tti_rx = tti_rx_;
}

void bwp_worker::run()
{
  srsran_assert(running(), "scheduler worker::run() called for non-active worker");
}

void bwp_worker::end_tti()
{
  srsran_assert(running(), "scheduler worker::end() called for non-active worker");

  // releases UE resources
  for (bwp_ue& u : bwp_ues) {
    if (not u.empty()) {
      u = {};
    }
  }

  tti_rx = {};
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

sched_worker_manager::sched_worker_manager(ue_map_t& ue_db_, const sched_nr_cfg& cfg_) : cfg(cfg_)
{
  // Note: For now, we only allow parallelism at the sector level
  sf_ctxts.resize(cfg.nof_concurrent_subframes);
  for (size_t i = 0; i < cfg.nof_concurrent_subframes; ++i) {
    sf_ctxts[i].reset(new sf_worker_ctxt());
    sem_init(&sf_ctxts[i]->sf_sem, 0, 1);
    sf_ctxts[i]->workers.reserve(cfg.cells.size());
    for (uint32_t cc = 0; cc < cfg.cells.size(); ++cc) {
      sf_ctxts[i]->workers.emplace_back(cc, ue_db_);
    }
  }
}

sched_worker_manager::~sched_worker_manager()
{
  for (uint32_t sf = 0; sf < sf_ctxts.size(); ++sf) {
    sem_destroy(&sf_ctxts[sf]->sf_sem);
  }
}

sched_worker_manager::sf_worker_ctxt& sched_worker_manager::get_sf(tti_point tti_rx)
{
  return *sf_ctxts[tti_rx.to_uint() % sf_ctxts.size()];
}

void sched_worker_manager::reserve_workers(tti_point tti_rx_, srsran::span<sched_nr_res_t> sf_result_)
{
  // lock if slot worker is already being used
  auto& sf_worker_ctxt = get_sf(tti_rx_);
  sem_wait(&sf_worker_ctxt.sf_sem);

  sf_worker_ctxt.sf_result    = sf_result_;
  sf_worker_ctxt.tti_rx       = tti_rx_;
  sf_worker_ctxt.worker_count = sf_worker_ctxt.workers.size();
}

void sched_worker_manager::start_tti(tti_point tti_rx_)
{
  auto& sf_worker_ctxt = get_sf(tti_rx_);
  srsran_assert(sf_worker_ctxt.tti_rx == tti_rx_, "invalid run_tti(tti, cc) arguments");

  for (uint32_t cc = 0; cc < sf_worker_ctxt.workers.size(); ++cc) {
    sf_worker_ctxt.workers[cc].start(sf_worker_ctxt.tti_rx);
  }
}

bool sched_worker_manager::run_tti(tti_point tti_rx_, uint32_t cc)
{
  auto& sf_worker_ctxt = get_sf(tti_rx_);
  srsran_assert(sf_worker_ctxt.tti_rx == tti_rx_, "invalid run_tti(tti, cc) arguments");
  if (not sf_worker_ctxt.workers[cc].running()) {
    // run for this tti and cc was already called
    return false;
  }

  // Get {tti, cc} scheduling decision
  sf_worker_ctxt.workers[cc].run();

  // decrement the number of active workers
  --sf_worker_ctxt.worker_count;
  srsran_assert(sf_worker_ctxt.worker_count >= 0, "invalid number of calls to run_tti(tti, cc)");
  return sf_worker_ctxt.worker_count == 0;
}

void sched_worker_manager::end_tti(tti_point tti_rx_)
{
  auto& sf_worker_ctxt = get_sf(tti_rx_);
  srsran_assert(sf_worker_ctxt.tti_rx == tti_rx_, "invalid run_tti(tti, cc) arguments");
  srsran_assert(sf_worker_ctxt.worker_count == 0, "invalid number of calls to run_tti(tti, cc)");

  // All the workers of the same TTI have finished. Synchronize scheduling decisions with UEs state
  for (auto& worker : sf_worker_ctxt.workers) {
    worker.end_tti();
  }
  sem_post(&sf_worker_ctxt.sf_sem);
}

} // namespace sched_nr_impl
} // namespace srsenb