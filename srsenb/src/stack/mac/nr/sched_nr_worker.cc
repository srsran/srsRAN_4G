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
#include "srsran/common/string_helpers.h"

namespace srsenb {
namespace sched_nr_impl {

slot_cc_worker::slot_cc_worker(serv_cell_manager& cc_sched) :
  cell(cc_sched), cfg(cc_sched.cfg), bwp_alloc(cc_sched.bwps[0].grid), logger(srslog::fetch_basic_logger("MAC"))
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
      logger.warning("SCHED: feedback received for invalid rnti=0x%x, cc=%d", f.rnti, cfg.cc);
    }
  }
  tmp_feedback_to_run.clear();
}

/// Called within a locked context, to generate {slot, cc} scheduling decision
void slot_cc_worker::run(slot_point pdcch_slot, ue_map_t& ue_db)
{
  srsran_assert(not running(), "scheduler worker::start() called for active worker");
  slot_rx = pdcch_slot - TX_ENB_DELAY;

  // Run pending cell feedback
  run_feedback(ue_db);

  // Reserve UEs for this worker slot
  for (auto& ue_pair : ue_db) {
    uint16_t rnti = ue_pair.first;
    ue&      u    = *ue_pair.second;
    if (u.carriers[cfg.cc] == nullptr) {
      continue;
    }

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

  // Allocate pending RARs
  cell.bwps[0].ra.run_slot(bwp_alloc);

  // TODO: Prioritize PDCCH scheduling for DL and UL data in a Round-Robin fashion
  alloc_dl_ues();
  alloc_ul_ues();

  // Log CC scheduler result
  log_sched_bwp_result(logger, bwp_alloc.get_pdcch_tti(), cell.bwps[0].grid, slot_ues);

  // releases UE resources
  slot_ues.clear();
  slot_rx = {};
}

void slot_cc_worker::alloc_dl_ues()
{
  if (not cfg.sched_cfg.pdsch_enabled) {
    return;
  }
  cell.bwps[0].data_sched->sched_dl_users(slot_ues, bwp_alloc);
}

void slot_cc_worker::alloc_ul_ues()
{
  if (not cfg.sched_cfg.pusch_enabled) {
    return;
  }
  cell.bwps[0].data_sched->sched_ul_users(slot_ues, bwp_alloc);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

sched_worker_manager::sched_worker_manager(ue_map_t&                                         ue_db_,
                                           const sched_params&                               cfg_,
                                           srsran::span<std::unique_ptr<serv_cell_manager> > cells_) :
  cfg(cfg_), ue_db(ue_db_), logger(srslog::fetch_basic_logger("MAC")), cells(cells_)
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

bool sched_worker_manager::save_sched_result(slot_point      pdcch_slot,
                                             uint32_t        cc,
                                             dl_sched_res_t& dl_res,
                                             ul_sched_t&     ul_res)
{
  // NOTE: Unlocked region
  auto& bwp_slot = cells[cc]->bwps[0].grid[pdcch_slot];

  dl_res.dl_sched.pdcch_dl = bwp_slot.dl_pdcchs;
  dl_res.dl_sched.pdcch_ul = bwp_slot.ul_pdcchs;
  dl_res.dl_sched.pdsch    = bwp_slot.pdschs;
  dl_res.rar               = bwp_slot.rar;
  ul_res.pusch             = bwp_slot.puschs;

  // Group pending HARQ ACKs
  srsran_pdsch_ack_nr_t ack           = {};
  ack.nof_cc                          = not bwp_slot.pending_acks.empty();
  const srsran::phy_cfg_nr_t* phy_cfg = nullptr;
  for (const harq_ack_t& pending_ack : bwp_slot.pending_acks) {
    srsran_harq_ack_m_t ack_m = {};
    ack_m.resource            = pending_ack.res;
    ack_m.present             = true;
    srsran_harq_ack_insert_m(&ack, &ack_m);
    phy_cfg = pending_ack.phy_cfg;
  }

  if (phy_cfg != nullptr) {
    srsran_slot_cfg_t slot_cfg{};
    slot_cfg.idx                = pdcch_slot.slot_idx();
    srsran_uci_cfg_nr_t uci_cfg = {};
    srsran_assert(phy_cfg->get_uci_cfg(slot_cfg, ack, uci_cfg), "Error getting UCI CFG");

    if (uci_cfg.ack.count > 0 || uci_cfg.nof_csi > 0 || uci_cfg.o_sr > 0) {
      if (not ul_res.pusch.empty()) {
        // Put UCI configuration in PUSCH config
        bool ret = phy_cfg->get_pusch_uci_cfg(slot_cfg, uci_cfg, ul_res.pusch[0].sch);
        srsran_assert(ret, "Error setting UCI configuration in PUSCH");
      } else {
        // Put UCI configuration in PUCCH config
        ul_res.pucch.emplace_back();
        pucch_t& pucch = ul_res.pucch.back();
        pucch.candidates.emplace_back();
        pucch.candidates.back().uci_cfg = uci_cfg;
        srsran_assert(phy_cfg->get_pucch_uci_cfg(
                          slot_cfg, pucch.candidates.back().uci_cfg, pucch.pucch_cfg, pucch.candidates.back().resource),
                      "Error getting PUCCH UCI cfg");

        // If this slot has a SR opportunity and the selected PUCCH format is 1, consider positive SR.
        if (uci_cfg.sr_positive_present and uci_cfg.ack.count > 0 and
            pucch.candidates.back().resource.format == SRSRAN_PUCCH_NR_FORMAT_1) {
          // Set SR negative
          if (uci_cfg.o_sr > 0) {
            uci_cfg.sr_positive_present = false;
          }

          // Append new resource
          pucch.candidates.emplace_back();
          pucch.candidates.back().uci_cfg = uci_cfg;
          srsran_assert(
              phy_cfg->get_pucch_uci_cfg(
                  slot_cfg, pucch.candidates.back().uci_cfg, pucch.pucch_cfg, pucch.candidates.back().resource),
              "Error getting PUCCH UCI cfg");
        }
      }
    }
  }

  // clear up BWP slot
  bwp_slot.reset();

  return true;
}

} // namespace sched_nr_impl
} // namespace srsenb
