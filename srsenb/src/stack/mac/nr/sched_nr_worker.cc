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

slot_cc_worker::slot_cc_worker(serv_cell_ctxt& cc_sched) :
  cell(cc_sched), cfg(*cc_sched.cfg), bwp_alloc(cc_sched.bwps[0].grid), logger(srslog::fetch_basic_logger("MAC"))
{}

/// Called at the beginning of TTI in a locked context, to reserve available UE resources
void slot_cc_worker::start(tti_point tti_rx_, ue_map_t& ue_db)
{
  srsran_assert(not running(), "scheduler worker::start() called for active worker");
  tti_rx = tti_rx_;

  // Try reserve UE cells for this worker
  for (auto& ue_pair : ue_db) {
    uint16_t rnti = ue_pair.first;
    ue&      u    = *ue_pair.second;

    slot_ues.insert(rnti, u.try_reserve(tti_rx, cfg.cc));
    if (slot_ues[rnti].empty()) {
      // Failed to synchronize because UE is being used by another worker
      slot_ues.erase(rnti);
      continue;
    }
    // UE acquired successfully for scheduling in this {tti, cc}
  }
}

void slot_cc_worker::run()
{
  srsran_assert(running(), "scheduler worker::run() called for non-active worker");

  bwp_alloc.new_slot(tti_rx + TX_ENB_DELAY);

  // Allocate pending RARs
  cell.bwps[0].ra.run_slot(bwp_alloc);

  // TODO: Prioritize PDCCH scheduling for DL and UL data in a Round-Robin fashion
  alloc_dl_ues();
  alloc_ul_ues();

  // Log CC scheduler result
  log_result();
}

void slot_cc_worker::end_tti()
{
  srsran_assert(running(), "scheduler worker::end() called for non-active worker");

  // releases UE resources
  slot_ues.clear();

  tti_rx = {};
}

void slot_cc_worker::alloc_dl_ues()
{
  if (slot_ues.empty()) {
    return;
  }
  slot_ue& ue = slot_ues.begin()->second;
  if (ue.h_dl == nullptr) {
    return;
  }

  rbgmask_t dlmask(cfg.bwps[0].N_rbg);
  dlmask.fill(0, dlmask.size(), true);
  bwp_alloc.alloc_pdsch(ue, dlmask);
}

void slot_cc_worker::alloc_ul_ues()
{
  if (slot_ues.empty()) {
    return;
  }
  slot_ue& ue = slot_ues.begin()->second;
  if (ue.h_ul == nullptr) {
    return;
  }

  rbgmask_t ulmask(cfg.bwps[0].N_rbg);
  ulmask.fill(0, ulmask.size(), true);
  bwp_alloc.alloc_pusch(ue, ulmask);
}

void slot_cc_worker::log_result() const
{
  const bwp_slot_grid& bwp_slot = cell.bwps[0].grid[tti_rx + TX_ENB_DELAY];
  for (const pdcch_dl_t& pdcch : bwp_slot.dl_pdcchs) {
    fmt::memory_buffer fmtbuf;
    if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_c) {
      const slot_ue& ue = slot_ues[pdcch.dci.ctx.rnti];
      fmt::format_to(fmtbuf,
                     "SCHED: DL {}, cc={}, rnti=0x{:x}, pid={}, nrtx={}, dai={}, tti_pdsch={}, tti_ack={}",
                     ue.h_dl->nof_retx() == 0 ? "tx" : "retx",
                     cell.cfg->cc,
                     ue.rnti,
                     ue.h_dl->pid,
                     ue.h_dl->nof_retx(),
                     pdcch.dci.dai,
                     ue.pdsch_tti,
                     ue.uci_tti);
    } else if (pdcch.dci.ctx.rnti_type == srsran_rnti_type_ra) {
      fmt::format_to(fmtbuf, "SCHED: DL RAR, cc={}", cell.cfg->cc);
    } else {
      fmt::format_to(fmtbuf, "SCHED: unknown format");
    }

    logger.info("%s", srsran::to_c_str(fmtbuf));
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

sched_worker_manager::sched_worker_manager(ue_map_t& ue_db_, const sched_params& cfg_) :
  cfg(cfg_), ue_db(ue_db_), logger(srslog::fetch_basic_logger("MAC"))
{
  for (uint32_t cc = 0; cc < cfg.cells.size(); ++cc) {
    cell_grid_list.emplace_back(cfg.cells[cc]);
  }

  // Note: For now, we only allow parallelism at the sector level
  slot_worker_ctxts.resize(cfg.sched_cfg.nof_concurrent_subframes);
  for (size_t i = 0; i < cfg.sched_cfg.nof_concurrent_subframes; ++i) {
    slot_worker_ctxts[i].reset(new slot_worker_ctxt());
    slot_worker_ctxts[i]->workers.reserve(cfg.cells.size());
    for (uint32_t cc = 0; cc < cfg.cells.size(); ++cc) {
      slot_worker_ctxts[i]->workers.emplace_back(cell_grid_list[cc]);
    }
  }
}

sched_worker_manager::~sched_worker_manager() = default;

sched_worker_manager::slot_worker_ctxt& sched_worker_manager::get_sf(tti_point tti_rx)
{
  return *slot_worker_ctxts[tti_rx.to_uint() % slot_worker_ctxts.size()];
}

void sched_worker_manager::start_slot(tti_point tti_rx, srsran::move_callback<void()> process_feedback)
{
  auto& sf_worker_ctxt = get_sf(tti_rx);

  std::unique_lock<std::mutex> lock(sf_worker_ctxt.slot_mutex);
  while ((sf_worker_ctxt.tti_rx.is_valid() and sf_worker_ctxt.tti_rx != tti_rx)) {
    // wait for previous slot to finish
    sf_worker_ctxt.nof_workers_waiting++;
    sf_worker_ctxt.cvar.wait(lock);
    sf_worker_ctxt.nof_workers_waiting--;
  }
  if (sf_worker_ctxt.tti_rx == tti_rx) {
    // another worker with the same slot idx already started
    return;
  }

  {
    std::lock_guard<std::mutex> db_lock(ue_db_mutex);

    process_feedback();

    for (uint32_t cc = 0; cc < sf_worker_ctxt.workers.size(); ++cc) {
      sf_worker_ctxt.workers[cc].start(tti_rx, ue_db);
    }
  }

  sf_worker_ctxt.tti_rx = tti_rx;
  sf_worker_ctxt.worker_count.store(static_cast<int>(sf_worker_ctxt.workers.size()), std::memory_order_relaxed);
  if (sf_worker_ctxt.nof_workers_waiting > 0) {
    sf_worker_ctxt.cvar.notify_all();
  }
}

bool sched_worker_manager::run_slot(tti_point tti_rx_, uint32_t cc)
{
  auto& sf_worker_ctxt = get_sf(tti_rx_);
  srsran_assert(sf_worker_ctxt.tti_rx == tti_rx_, "invalid run_tti(tti, cc) arguments");

  // Get {tti, cc} scheduling decision
  sf_worker_ctxt.workers[cc].run();

  // decrement the number of active workers
  int rem_workers = sf_worker_ctxt.worker_count.fetch_sub(1, std::memory_order_release) - 1;
  srsran_assert(rem_workers >= 0, "invalid number of calls to run_tti(tti, cc)");

  return rem_workers == 0;
}

void sched_worker_manager::release_slot(tti_point tti_rx_)
{
  auto& sf_worker_ctxt = get_sf(tti_rx_);
  srsran_assert(sf_worker_ctxt.tti_rx == tti_rx_, "invalid run_tti(tti, cc) arguments");
  srsran_assert(sf_worker_ctxt.worker_count == 0, "invalid number of calls to run_tti(tti, cc)");

  {
    std::lock_guard<std::mutex> lock(ue_db_mutex);

    // All the workers of the same slot have finished. Synchronize scheduling decisions with UEs state
    for (slot_cc_worker& worker : sf_worker_ctxt.workers) {
      worker.end_tti();
    }
  }

  std::unique_lock<std::mutex> lock(sf_worker_ctxt.slot_mutex);
  sf_worker_ctxt.tti_rx = {};
  if (sf_worker_ctxt.nof_workers_waiting > 0) {
    lock.unlock();
    sf_worker_ctxt.cvar.notify_one();
  }
}

bool sched_worker_manager::save_sched_result(tti_point pdcch_tti, uint32_t cc, dl_sched_t& dl_res, ul_sched_t& ul_res)
{
  auto& bwp_slot = cell_grid_list[cc].bwps[0].grid[pdcch_tti];

  dl_res.pdcch_dl = bwp_slot.dl_pdcchs;
  dl_res.pdcch_ul = bwp_slot.ul_pdcchs;
  dl_res.pdsch    = bwp_slot.pdschs;
  ul_res.pusch    = bwp_slot.puschs;

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
    slot_cfg.idx                = pdcch_tti.sf_idx();
    srsran_uci_cfg_nr_t uci_cfg = {};
    srsran_assert(phy_cfg->get_uci_cfg(slot_cfg, ack, uci_cfg), "Error getting UCI CFG");

    if (uci_cfg.ack.count > 0 || uci_cfg.nof_csi > 0 || uci_cfg.o_sr > 0) {
      if (not ul_res.pusch.empty()) {
        // Put UCI configuration in PUSCH config
        srsran_assert(phy_cfg->get_pusch_uci_cfg(slot_cfg, uci_cfg, ul_res.pusch[0].sch),
                      "Error setting UCI configuration in PUSCH");
      } else {
        // Put UCI configuration in PUCCH config
        ul_res.pucch.emplace_back();
        pucch_t& pucch = ul_res.pucch.back();
        pucch.uci_cfg  = uci_cfg;
        srsran_assert(phy_cfg->get_pucch_uci_cfg(slot_cfg, pucch.uci_cfg, pucch.pucch_cfg, pucch.resource),
                      "Error getting PUCCH UCI cfg");
      }
    }
  }

  // clear up BWP slot
  bwp_slot.reset();

  return true;
}

} // namespace sched_nr_impl
} // namespace srsenb