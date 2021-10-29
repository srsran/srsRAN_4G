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
#include "srsenb/hdr/stack/mac/common/mac_metrics.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_signalling.h"
#include "srsran/common/string_helpers.h"

namespace srsenb {
namespace sched_nr_impl {

carrier_feedback_manager::carrier_feedback_manager(const cell_params_t& cell_cfg) :
  cfg(cell_cfg), logger(srslog::fetch_basic_logger(cell_cfg.sched_args.logger_name))
{}

void carrier_feedback_manager::enqueue_common_event(srsran::move_callback<void()> ev)
{
  std::lock_guard<std::mutex> lock(feedback_mutex);
  pending_events.emplace_back(std::move(ev));
}

void carrier_feedback_manager::enqueue_ue_feedback(uint16_t rnti, feedback_callback_t fdbk)
{
  std::lock_guard<std::mutex> lock(feedback_mutex);
  pending_feedback.emplace_back();
  pending_feedback.back().rnti = rnti;
  pending_feedback.back().fdbk = std::move(fdbk);
}

void carrier_feedback_manager::run(ue_map_t& ue_db)
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cc_worker::cc_worker(const cell_params_t& params) :
  cfg(params), logger(srslog::fetch_basic_logger(params.sched_args.logger_name)), pending_feedback(params)
{
  for (uint32_t bwp_id = 0; bwp_id < cfg.cfg.bwps.size(); ++bwp_id) {
    bwps.emplace_back(cfg.bwps[bwp_id]);
  }

  // Pre-allocate HARQs in common pool of softbuffers
  harq_softbuffer_pool::get_instance().init_pool(cfg.nof_prb());
}

bool cc_worker::save_sched_result(dl_sched_res_t& dl_res, ul_sched_t& ul_res, slot_point slot_tx)
{
  auto& bwp_slot = bwps[0].grid[slot_tx];

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

void cc_worker::dl_rach_info(const sched_nr_interface::rar_info_t& rar_info)
{
  bwps[0].ra.dl_rach_info(rar_info);
}

/// Called within a locked context, to generate {slot, cc} scheduling decision
void cc_worker::run_slot(slot_point pdcch_slot, ue_map_t& ue_db, dl_sched_res_t& dl_res, ul_sched_t& ul_res)
{
  // Create an BWP allocator object that will passed along to RA, SI, Data schedulers
  bwp_slot_allocator bwp_alloc{bwps[0].grid, pdcch_slot, slot_ues};

  // Run pending cell feedback (process feedback)
  pending_feedback.run(ue_db);

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

  // Log UEs state for slot
  log_sched_slot_ues(logger, pdcch_slot, cfg.cc, slot_ues);

  // Allocate cell DL signalling
  bwp_slot_grid& bwp_pdcch_slot = bwps[0].grid[pdcch_slot];
  sched_dl_signalling(*bwps[0].cfg, pdcch_slot, bwp_pdcch_slot.ssb, bwp_pdcch_slot.nzp_csi_rs);

  // Allocate pending RARs
  bwps[0].ra.run_slot(bwp_alloc);

  // TODO: Prioritize PDCCH scheduling for DL and UL data in a Round-Robin fashion
  alloc_dl_ues(bwp_alloc);
  alloc_ul_ues(bwp_alloc);

  // Post-processing of scheduling decisions
  postprocess_decisions(bwp_alloc);

  // Log CC scheduler result
  log_sched_bwp_result(logger, bwp_alloc.get_pdcch_tti(), bwps[0].grid, slot_ues);

  // Post-process and copy results to intermediate buffer
  save_sched_result(dl_res, ul_res, pdcch_slot);

  // releases UE resources
  slot_ues.clear();
}

void cc_worker::alloc_dl_ues(bwp_slot_allocator& bwp_alloc)
{
  if (not cfg.sched_args.pdsch_enabled) {
    return;
  }
  bwps[0].data_sched->sched_dl_users(slot_ues, bwp_alloc);
}

void cc_worker::alloc_ul_ues(bwp_slot_allocator& bwp_alloc)
{
  if (not cfg.sched_args.pusch_enabled) {
    return;
  }
  bwps[0].data_sched->sched_ul_users(slot_ues, bwp_alloc);
}

void cc_worker::postprocess_decisions(bwp_slot_allocator& bwp_alloc)
{
  auto&             bwp_slot = bwps[0].grid[bwp_alloc.get_pdcch_tti()];
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

} // namespace sched_nr_impl
} // namespace srsenb
