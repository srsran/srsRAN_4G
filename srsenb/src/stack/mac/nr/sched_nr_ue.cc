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

#include "srsenb/hdr/stack/mac/nr/sched_nr_ue.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_pdcch.h"
#include "srsran/common/string_helpers.h"

namespace srsenb {
namespace sched_nr_impl {

slot_ue::slot_ue(uint16_t rnti_, slot_point slot_rx_, uint32_t cc_) : rnti(rnti_), slot_rx(slot_rx_), cc(cc_) {}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ue_carrier::ue_carrier(uint16_t rnti_, const ue_cfg_t& uecfg_, const cell_params_t& cell_params_) :
  rnti(rnti_),
  cc(cell_params_.cc),
  bwp_cfg(rnti_, cell_params_.bwps[0], uecfg_),
  cell_params(cell_params_),
  harq_ent(rnti_, cell_params_.nof_prb(), SCHED_NR_MAX_HARQ, cell_params_.bwps[0].logger)
{}

void ue_carrier::set_cfg(const ue_cfg_t& ue_cfg)
{
  bwp_cfg = bwp_ue_cfg(rnti, cell_params.bwps[0], ue_cfg);
}

void ue_carrier::new_slot(slot_point slot_tx)
{
  harq_ent.new_slot(slot_tx - TX_ENB_DELAY);
}

slot_ue ue_carrier::try_reserve(slot_point pdcch_slot, uint32_t dl_pending_bytes, uint32_t ul_pending_bytes)
{
  slot_point slot_rx = pdcch_slot - TX_ENB_DELAY;

  // copy cc-specific parameters and find available HARQs
  slot_ue sfu(rnti, slot_rx, cc);
  sfu.cfg           = &bwp_cfg;
  sfu.pdcch_slot    = pdcch_slot;
  sfu.harq_ent      = &harq_ent;
  const uint32_t k0 = 0;
  sfu.pdsch_slot    = sfu.pdcch_slot + k0;
  uint32_t k1       = sfu.cfg->get_k1(sfu.pdsch_slot);
  sfu.uci_slot      = sfu.pdsch_slot + k1;
  uint32_t k2       = bwp_cfg.active_bwp().pusch_ra_list[0].K;
  sfu.pusch_slot    = sfu.pdcch_slot + k2;
  sfu.dl_cqi        = dl_cqi;
  sfu.ul_cqi        = ul_cqi;

  // set UE-common parameters
  sfu.dl_pending_bytes = dl_pending_bytes;
  sfu.ul_pending_bytes = ul_pending_bytes;

  const srsran_duplex_config_nr_t& tdd_cfg = cell_params.cfg.duplex;
  if (srsran_duplex_nr_is_dl(&tdd_cfg, 0, sfu.pdsch_slot.slot_idx())) {
    // If DL enabled
    sfu.h_dl = harq_ent.find_pending_dl_retx();
    if (sfu.h_dl == nullptr and sfu.dl_pending_bytes > 0) {
      sfu.h_dl = harq_ent.find_empty_dl_harq();
    }
  }
  if (srsran_duplex_nr_is_ul(&tdd_cfg, 0, sfu.pusch_slot.slot_idx())) {
    // If UL enabled
    sfu.h_ul = harq_ent.find_pending_ul_retx();
    if (sfu.h_ul == nullptr and sfu.ul_pending_bytes > 0) {
      sfu.h_ul = harq_ent.find_empty_ul_harq();
    }
  }

  return sfu;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ue::ue(uint16_t rnti_, const ue_cfg_t& cfg, const sched_params& sched_cfg_) :
  rnti(rnti_), sched_cfg(sched_cfg_), buffers(rnti_, srslog::fetch_basic_logger(sched_cfg_.sched_cfg.logger_name))
{
  set_cfg(cfg);
}

void ue::set_cfg(const ue_cfg_t& cfg)
{
  ue_cfg = cfg;
  for (auto& ue_cc_cfg : cfg.carriers) {
    if (ue_cc_cfg.active) {
      if (carriers[ue_cc_cfg.cc] == nullptr) {
        carriers[ue_cc_cfg.cc].reset(new ue_carrier(rnti, ue_cfg, sched_cfg.cells[ue_cc_cfg.cc]));
      } else {
        carriers[ue_cc_cfg.cc]->set_cfg(ue_cfg);
      }
    }
  }

  buffers.config_lcids(cfg.ue_bearers);
}

void ue::new_slot(slot_point pdcch_slot)
{
  last_pdcch_slot = pdcch_slot;

  // Compute pending DL/UL bytes for {rnti, pdcch_slot}
  if (sched_cfg.sched_cfg.auto_refill_buffer) {
    dl_pending_bytes = 1000000;
    ul_pending_bytes = 1000000;
  } else {
    dl_pending_bytes = buffers.get_dl_tx_total();
    ul_pending_bytes = buffers.get_bsr();
    for (auto& ue_cc_cfg : ue_cfg.carriers) {
      auto& cc = carriers[ue_cc_cfg.cc];
      if (cc != nullptr) {
        // Discount UL HARQ pending bytes to BSR
        for (uint32_t pid = 0; pid < cc->harq_ent.nof_ul_harqs(); ++pid) {
          if (not cc->harq_ent.ul_harq(pid).empty()) {
            ul_pending_bytes -= cc->harq_ent.ul_harq(pid).tbs();
            if (last_sr_slot.valid() and cc->harq_ent.ul_harq(pid).harq_slot_tx() > last_sr_slot) {
              last_sr_slot.clear();
            }
          }
        }
      }
    }
    ul_pending_bytes = std::max(0, ul_pending_bytes);
    if (ul_pending_bytes == 0 and last_sr_slot.valid()) {
      // If unanswered SR is pending
      ul_pending_bytes = 512;
    }
  }
}

slot_ue ue::try_reserve(slot_point pdcch_slot, uint32_t cc)
{
  srsran_assert(carriers[cc] != nullptr, "try_reserve() called for inexistent rnti=0x%x,cc=%d", rnti, cc);
  return carriers[cc]->try_reserve(pdcch_slot, dl_pending_bytes, ul_pending_bytes);
}

} // namespace sched_nr_impl
} // namespace srsenb
