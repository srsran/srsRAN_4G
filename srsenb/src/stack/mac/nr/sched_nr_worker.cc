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

#include "srsenb/hdr/stack/mac/nr/sched_nr_worker.h"
#include "srsenb/hdr/stack/mac/common/mac_metrics.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_signalling.h"
#include "srsran/common/string_helpers.h"

namespace srsenb {
namespace sched_nr_impl {

slot_cc_worker::slot_cc_worker(serv_cell_manager& cc_sched) :
  cell(cc_sched),
  cfg(cc_sched.cfg),
  bwp_alloc(cc_sched.bwps[0].grid),
  logger(srslog::fetch_basic_logger(cc_sched.cfg.sched_args.logger_name))
{}

void slot_cc_worker::enqueue_cc_event(srsran::move_callback<void()> ev)
{
  std::lock_guard<std::mutex> lock(feedback_mutex);
  pending_events.emplace_back();
  pending_events.back() = std::move(ev);
}

void slot_cc_worker::enqueue_cc_feedback(uint16_t rnti, feedback_callback_t fdbk)
{
  std::lock_guard<std::mutex> lock(feedback_mutex);
  pending_feedback.emplace_back();
  pending_feedback.back().rnti = rnti;
  pending_feedback.back().fdbk = std::move(fdbk);
}

void slot_cc_worker::run_feedback(ue_map_t& ue_db)
{
  {
    std::lock_guard<std::mutex> lock(feedback_mutex);
    tmp_feedback_to_run.swap(pending_feedback);
    tmp_events_to_run.swap(pending_events);
  }

  for (srsran::move_callback<void()>& ev : tmp_events_to_run) {
    ev();
  }
  tmp_events_to_run.clear();

  for (feedback_t& f : tmp_feedback_to_run) {
    if (ue_db.contains(f.rnti) and ue_db[f.rnti]->carriers[cfg.cc] != nullptr) {
      f.fdbk(*ue_db[f.rnti]->carriers[cfg.cc]);
    } else {
      logger.info("SCHED: feedback received for rnti=0x%x, cc=%d that has been removed.", f.rnti, cfg.cc);
    }
  }
  tmp_feedback_to_run.clear();
}

/// Called within a locked context, to generate {slot, cc} scheduling decision
void slot_cc_worker::run(slot_point pdcch_slot, ue_map_t& ue_db)
{
  srsran_assert(not running(), "scheduler worker::start() called for active worker");
  slot_rx = pdcch_slot - TX_ENB_DELAY;

  // Run pending cell feedback (process feedback)
  run_feedback(ue_db);

  // Reserve UEs for this worker slot (select candidate UEs)
  for (auto& ue_pair : ue_db) {
    uint16_t rnti = ue_pair.first;
    ue&      u    = *ue_pair.second;
    if (u.carriers[cfg.cc] == nullptr) {
      continue;
    }

    // Update UE CC state
    u.carriers[cfg.cc]->new_slot(pdcch_slot);

    // info for a given UE on a slot to be process
    slot_ues.insert(rnti, u.try_reserve(pdcch_slot, cfg.cc));
    if (slot_ues[rnti].empty()) {
      // Failed to generate slot UE because UE has no conditions for DL/UL tx
      slot_ues.erase(rnti);
      continue;
    }
    // UE acquired successfully for scheduling in this {slot, cc}
  }

  // Create an BWP allocator object that will passed along to RA, SI, Data schedulers
  bwp_alloc.new_slot(slot_rx + TX_ENB_DELAY, slot_ues);

  // Log UEs state for slot
  log_sched_slot_ues(logger, bwp_alloc.get_pdcch_tti(), cfg.cc, slot_ues);

  // Allocate pending RARs
  cell.bwps[0].ra.run_slot(bwp_alloc);

  // TODO: Prioritize PDCCH scheduling for DL and UL data in a Round-Robin fashion
  alloc_dl_ues();
  alloc_ul_ues();

  // Post-processing of scheduling decisions
  postprocess_decisions();

  // Log CC scheduler result
  log_sched_bwp_result(logger, bwp_alloc.get_pdcch_tti(), cell.bwps[0].grid, slot_ues);

  // releases UE resources
  slot_ues.clear();
  slot_rx = {};
}

void slot_cc_worker::alloc_dl_ues()
{
  if (not cfg.sched_args.pdsch_enabled) {
    return;
  }
  cell.bwps[0].data_sched->sched_dl_users(slot_ues, bwp_alloc);
}

void slot_cc_worker::alloc_ul_ues()
{
  if (not cfg.sched_args.pusch_enabled) {
    return;
  }
  cell.bwps[0].data_sched->sched_ul_users(slot_ues, bwp_alloc);
}

void slot_cc_worker::postprocess_decisions()
{
  auto&             bwp_slot = cell.bwps[0].grid[bwp_alloc.get_pdcch_tti()];
  srsran_slot_cfg_t slot_cfg{};
  slot_cfg.idx = bwp_alloc.get_pdcch_tti().to_uint();

  for (auto& ue_pair : slot_ues) {
    auto& ue = ue_pair.second;
    // Group pending HARQ ACKs
    srsran_pdsch_ack_nr_t ack = {};

    for (auto& h_ack : bwp_slot.pending_acks) {
      if (h_ack.res.rnti == ue.rnti) {
        ack.nof_cc = 1;

        srsran_harq_ack_m_t ack_m = {};
        ack_m.resource            = h_ack.res;
        ack_m.present             = true;
        srsran_harq_ack_insert_m(&ack, &ack_m);
      }
    }

    srsran_uci_cfg_nr_t uci_cfg = {};
    if (not ue.cfg->phy().get_uci_cfg(slot_cfg, ack, uci_cfg)) {
      logger.error("Error getting UCI configuration");
      continue;
    }

    if (uci_cfg.ack.count == 0 and uci_cfg.nof_csi == 0 and uci_cfg.o_sr == 0) {
      continue;
    }

    bool has_pusch = false;
    for (auto& pusch : bwp_slot.puschs) {
      if (pusch.sch.grant.rnti == ue.rnti) {
        // Put UCI configuration in PUSCH config
        has_pusch = true;

        // If has PUSCH, no SR shall be received
        uci_cfg.o_sr = 0;

        if (not ue.cfg->phy().get_pusch_uci_cfg(slot_cfg, uci_cfg, pusch.sch)) {
          logger.error("Error setting UCI configuration in PUSCH");
          continue;
        }
        break;
      }
    }
    if (not has_pusch) {
      // If any UCI information is triggered, schedule PUCCH
      if (bwp_slot.pucch.full()) {
        logger.warning("SCHED: Cannot fit pending UCI into PUCCH");
        continue;
      }
      bwp_slot.pucch.emplace_back();
      mac_interface_phy_nr::pucch_t& pucch = bwp_slot.pucch.back();

      uci_cfg.pucch.rnti = ue.rnti;
      pucch.candidates.emplace_back();
      pucch.candidates.back().uci_cfg = uci_cfg;
      if (not ue.cfg->phy().get_pucch_uci_cfg(slot_cfg, uci_cfg, pucch.pucch_cfg, pucch.candidates.back().resource)) {
        logger.error("Error getting UCI CFG");
        continue;
      }

      // If this slot has a SR opportunity and the selected PUCCH format is 1, consider positive SR.
      if (uci_cfg.o_sr > 0 and uci_cfg.ack.count > 0 and
          pucch.candidates.back().resource.format == SRSRAN_PUCCH_NR_FORMAT_1) {
        // Set SR negative
        if (uci_cfg.o_sr > 0) {
          uci_cfg.sr_positive_present = false;
        }

        // Append new resource
        pucch.candidates.emplace_back();
        pucch.candidates.back().uci_cfg = uci_cfg;
        if (not ue.cfg->phy().get_pucch_uci_cfg(slot_cfg, uci_cfg, pucch.pucch_cfg, pucch.candidates.back().resource)) {
          logger.error("Error getting UCI CFG");
          continue;
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

sched_worker_manager::sched_worker_manager(ue_map_t&                                         ue_db_,
                                           const sched_params&                               cfg_,
                                           srsran::span<std::unique_ptr<serv_cell_manager> > cells_) :
  cfg(cfg_), ue_db(ue_db_), logger(srslog::fetch_basic_logger(cfg_.sched_cfg.logger_name)), cells(cells_)
{
  cc_worker_list.reserve(cfg.cells.size());
  for (uint32_t cc = 0; cc < cfg.cells.size(); ++cc) {
    cc_worker_list.emplace_back(new cc_context{*cells[cc]});
  }
}

sched_worker_manager::~sched_worker_manager() = default;

void sched_worker_manager::enqueue_event(uint16_t rnti, srsran::move_callback<void()> ev)
{
  std::lock_guard<std::mutex> lock(event_mutex);
  next_slot_events.push_back(ue_event_t{rnti, std::move(ev)});
}

void sched_worker_manager::enqueue_cc_event(uint32_t cc, srsran::move_callback<void()> ev)
{
  cc_worker_list[cc]->worker.enqueue_cc_event(std::move(ev));
}

/**
 * Update UEs state that is non-CC specific (e.g. SRs, buffer status, UE configuration)
 * @param slot_tx
 * @param locked_context to update only UEs with CA enabled or not
 */
void sched_worker_manager::update_ue_db(slot_point slot_tx, bool locked_context)
{
  // process non-cc specific feedback if pending (e.g. SRs, buffer updates, UE config)
  for (ue_event_t& ev : slot_events) {
    if ((locked_context and not ue_db.contains(ev.rnti)) or
        (ue_db.contains(ev.rnti) and ue_db[ev.rnti]->has_ca() == locked_context)) {
      ev.callback();
    }
  }

  // prepare UEs internal state for new slot
  for (auto& u : ue_db) {
    if (u.second->has_ca() == locked_context) {
      u.second->new_slot(slot_tx);
    }
  }
}

void sched_worker_manager::run_slot(slot_point slot_tx, uint32_t cc, dl_sched_res_t& dl_res, ul_sched_t& ul_res)
{
  // Fill DL signalling messages that do not depend on UEs state
  serv_cell_manager& serv_cell = *cells[cc];
  bwp_slot_grid&     bwp_slot  = serv_cell.bwps[0].grid[slot_tx];
  sched_dl_signalling(*serv_cell.bwps[0].cfg, slot_tx, bwp_slot.ssb, bwp_slot.nzp_csi_rs);

  // Synchronization point between CC workers, to avoid concurrency in UE state access
  srsran::bounded_vector<std::condition_variable*, SRSRAN_MAX_CARRIERS> waiting_cvars;
  {
    std::unique_lock<std::mutex> lock(slot_mutex);
    while (current_slot.valid() and current_slot != slot_tx) {
      // Wait for previous slot to finish
      cc_worker_list[cc]->waiting++;
      cc_worker_list[cc]->cvar.wait(lock);
      cc_worker_list[cc]->waiting--;
    }
    if (not current_slot.valid()) {
      /* First Worker to start slot */

      // process non-cc specific feedback if pending for UEs with CA
      // NOTE: there is no parallelism in these operations
      slot_events.clear();
      {
        std::lock_guard<std::mutex> ev_lock(event_mutex);
        next_slot_events.swap(slot_events);
      }
      update_ue_db(slot_tx, true);

      // mark the start of slot. awake remaining workers if locking on the mutex
      current_slot = slot_tx;
      worker_count.store(static_cast<int>(cc_worker_list.size()), std::memory_order_relaxed);
      for (auto& w : cc_worker_list) {
        if (w->waiting > 0) {
          waiting_cvars.push_back(&w->cvar);
        }
      }
      lock.unlock();
      for (auto& w : waiting_cvars) {
        w->notify_one();
      }
      waiting_cvars.clear();
    }
  }

  /* Parallel Region */

  // process non-cc specific feedback if pending (e.g. SRs, buffer updates, UE config) for UEs without CA
  update_ue_db(slot_tx, false);

  // process pending feedback, generate {slot, cc} scheduling decision
  cc_worker_list[cc]->worker.run(slot_tx, ue_db);

  // decrement the number of active workers
  int rem_workers = worker_count.fetch_sub(1, std::memory_order_release) - 1;
  srsran_assert(rem_workers >= 0, "invalid number of calls to run_slot(slot, cc)");
  if (rem_workers == 0) {
    /* Last Worker to finish slot */

    // Signal the release of slot if it is the last worker that finished its own generation
    std::unique_lock<std::mutex> lock(slot_mutex);
    current_slot = {};

    // All the workers of the same slot have finished. Synchronize scheduling decisions with UEs state
    for (auto& c : cc_worker_list) {
      if (c->waiting > 0) {
        waiting_cvars.push_back(&c->cvar);
      }
    }

    // Awake waiting workers
    lock.unlock();
    for (auto& c : waiting_cvars) {
      c->notify_one();
    }
  }

  // Post-process and copy results to intermediate buffer
  save_sched_result(slot_tx, cc, dl_res, ul_res);
}

void sched_worker_manager::get_metrics(mac_metrics_t& metrics)
{
  std::unique_lock<std::mutex> lock(slot_mutex);
  get_metrics_nolocking(metrics);
}

bool sched_worker_manager::save_sched_result(slot_point      pdcch_slot,
                                             uint32_t        cc,
                                             dl_sched_res_t& dl_res,
                                             ul_sched_t&     ul_res)
{
  // NOTE: Unlocked region
  auto& bwp_slot = cells[cc]->bwps[0].grid[pdcch_slot];

  dl_res.dl_sched.pdcch_dl   = bwp_slot.dl_pdcchs;
  dl_res.dl_sched.pdcch_ul   = bwp_slot.ul_pdcchs;
  dl_res.dl_sched.pdsch      = bwp_slot.pdschs;
  dl_res.rar                 = bwp_slot.rar;
  dl_res.dl_sched.ssb        = bwp_slot.ssb;
  dl_res.dl_sched.nzp_csi_rs = bwp_slot.nzp_csi_rs;
  ul_res.pusch               = bwp_slot.puschs;
  ul_res.pucch               = bwp_slot.pucch;

  // clear up BWP slot
  bwp_slot.reset();

  return true;
}

void sched_worker_manager::get_metrics_nolocking(mac_metrics_t& metrics)
{
  for (mac_ue_metrics_t& ue_metric : metrics.ues) {
    if (ue_db.contains(ue_metric.rnti) and ue_db[ue_metric.rnti]->carriers[0] != nullptr) {
      auto&                       ue_cc = *ue_db[ue_metric.rnti]->carriers[0];
      std::lock_guard<std::mutex> lock(ue_cc.metrics_mutex);
      ue_metric.tx_brate  = ue_cc.metrics.tx_brate;
      ue_metric.tx_errors = ue_cc.metrics.tx_errors;
      ue_metric.tx_pkts   = ue_cc.metrics.tx_pkts;
      ue_cc.metrics       = {};
    }
  }
}

} // namespace sched_nr_impl
} // namespace srsenb
