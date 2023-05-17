/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsgnb/hdr/stack/mac/sched_nr_worker.h"
#include "srsenb/hdr/stack/mac/common/mac_metrics.h"
#include "srsgnb/hdr/stack/mac/sched_nr_signalling.h"
#include "srsran/common/string_helpers.h"

namespace srsenb {
namespace sched_nr_impl {

cc_worker::cc_worker(const cell_config_manager& params) :
  cfg(params), logger(srslog::fetch_basic_logger(params.sched_args.logger_name))
{
  for (uint32_t bwp_id = 0; bwp_id < cfg.bwps.size(); ++bwp_id) {
    bwps.emplace_back(cfg.bwps[bwp_id]);
  }

  // Pre-allocate HARQs in common pool of softbuffers
  harq_softbuffer_pool::get_instance().init_pool(cfg.nof_prb());
}

void cc_worker::dl_rach_info(const sched_nr_interface::rar_info_t& rar_info)
{
  bwps[0].ra.dl_rach_info(rar_info);
}

/// Called within a locked context, to generate {slot, cc} scheduling decision

dl_sched_res_t* cc_worker::run_slot(slot_point tx_sl, ue_map_t& ue_db)
{
  // Reset old sched outputs
  if (not last_tx_sl.valid()) {
    last_tx_sl = tx_sl;
  }
  while (last_tx_sl != tx_sl) {
    last_tx_sl++;
    slot_point old_slot = last_tx_sl - TX_ENB_DELAY - 1;
    for (bwp_manager& bwp : bwps) {
      bwp.grid[old_slot].reset();
    }
  }

  // Reserve UEs for this worker slot (select candidate UEs)
  for (auto& ue_pair : ue_db) {
    uint16_t rnti = ue_pair.first;
    ue&      u    = *ue_pair.second;
    if (u.carriers[cfg.cc] == nullptr) {
      continue;
    }

    // info for a given UE on a slot to be process
    slot_ues.insert(rnti, u.make_slot_ue(tx_sl, cfg.cc));
    if (slot_ues[rnti].empty()) {
      // Failed to generate slot UE because UE has no conditions for DL/UL tx
      slot_ues.erase(rnti);
      continue;
    }
    // UE acquired successfully for scheduling in this {slot, cc}
  }

  // Create an BWP allocator object that will passed along to RA, SI, Data schedulers
  bwp_slot_allocator bwp_alloc{bwps[0].grid, tx_sl, slot_ues};

  // Log UEs state for slot
  log_sched_slot_ues(logger, tx_sl, cfg.cc, slot_ues);

  const uint32_t ss_id    = 0;
  slot_point     sl_pdcch = bwp_alloc.get_pdcch_tti();

  prb_bitmap prbs_before = bwp_alloc.res_grid()[sl_pdcch].pdschs.occupied_prbs(ss_id, srsran_dci_format_nr_1_0);
  // Allocate cell DL signalling
  sched_dl_signalling(bwp_alloc);

  prb_bitmap prbs_after = bwp_alloc.res_grid()[sl_pdcch].pdschs.occupied_prbs(ss_id, srsran_dci_format_nr_1_0);

  // Allocate pending SIBs
  bwps[0].si.run_slot(bwp_alloc);

  // Allocate pending RARs
  bwps[0].ra.run_slot(bwp_alloc);

  // TODO: Prioritize PDCCH scheduling for DL and UL data in a Round-Robin fashion
  alloc_dl_ues(bwp_alloc);
  alloc_ul_ues(bwp_alloc);

  // Post-processing of scheduling decisions
  postprocess_decisions(bwp_alloc);

  // Log CC scheduler result
  log_sched_bwp_result(logger, bwp_alloc.get_pdcch_tti(), bwps[0].grid, slot_ues);

  // releases UE resources
  slot_ues.clear();

  return &bwp_alloc.tx_slot_grid().dl;
}

ul_sched_t* cc_worker::get_ul_sched(slot_point sl)
{
  return &bwps[0].grid[sl].ul;
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
      if (h_ack.res.rnti == ue->rnti) {
        ack.nof_cc = 1;

        srsran_harq_ack_m_t ack_m = {};
        ack_m.resource            = h_ack.res;
        ack_m.present             = true;
        srsran_harq_ack_insert_m(&ack, &ack_m);
      }
    }

    srsran_uci_cfg_nr_t uci_cfg = {};
    if (not ue->phy().get_uci_cfg(slot_cfg, ack, uci_cfg)) {
      logger.error("Error getting UCI configuration");
      continue;
    }

    if (uci_cfg.ack.count == 0 and uci_cfg.nof_csi == 0 and uci_cfg.o_sr == 0) {
      continue;
    }

    bool has_pusch = false;
    for (auto& pusch : bwp_slot.ul.pusch) {
      if (pusch.sch.grant.rnti == ue->rnti) {
        // Put UCI configuration in PUSCH config
        has_pusch = true;

        // If has PUSCH, no SR shall be received
        uci_cfg.o_sr = 0;

        if (not ue->phy().get_pusch_uci_cfg(slot_cfg, uci_cfg, pusch.sch)) {
          logger.error("Error setting UCI configuration in PUSCH");
          continue;
        }
        break;
      }
    }
    if (not has_pusch) {
      // If any UCI information is triggered, schedule PUCCH
      if (bwp_slot.ul.pucch.full()) {
        logger.warning("SCHED: Cannot fit pending UCI into PUCCH");
        continue;
      }
      bwp_slot.ul.pucch.emplace_back();
      mac_interface_phy_nr::pucch_t& pucch = bwp_slot.ul.pucch.back();

      uci_cfg.pucch.rnti = ue->rnti;
      pucch.candidates.emplace_back();
      pucch.candidates.back().uci_cfg = uci_cfg;
      if (not ue->phy().get_pucch_uci_cfg(slot_cfg, uci_cfg, pucch.pucch_cfg, pucch.candidates.back().resource)) {
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
        if (not ue->phy().get_pucch_uci_cfg(slot_cfg, uci_cfg, pucch.pucch_cfg, pucch.candidates.back().resource)) {
          logger.error("Error getting UCI CFG");
          continue;
        }
      }
    }
  }
}

} // namespace sched_nr_impl
} // namespace srsenb
