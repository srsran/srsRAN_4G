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

#include "srsgnb/hdr/stack/mac/sched_nr_signalling.h"
#include "srsgnb/hdr/stack/mac/sched_nr_grant_allocator.h"

#define POS_IN_BURST_FIRST_BIT_IDX 0
#define POS_IN_BURST_SECOND_BIT_IDX 1
#define POS_IN_BURST_THIRD_BIT_IDX 2
#define POS_IN_BURST_FOURTH_BIT_IDX 3

#define DEFAULT_SSB_PERIODICITY 5
#define MAX_SIB_TX 8

namespace srsenb {
namespace sched_nr_impl {

void sched_nzp_csi_rs(srsran::const_span<srsran_csi_rs_nzp_set_t> nzp_csi_rs_sets_cfg,
                      const srsran_slot_cfg_t&                    slot_cfg,
                      nzp_csi_rs_list&                            csi_rs_list)
{
  for (const srsran_csi_rs_nzp_set_t& set : nzp_csi_rs_sets_cfg) {
    // For each NZP-CSI-RS resource available in the set
    for (uint32_t i = 0; i < set.count; ++i) {
      // Select resource
      const srsran_csi_rs_nzp_resource_t& nzp_csi_resource = set.data[i];

      // Check if the resource is scheduled for this slot
      if (srsran_csi_rs_send(&nzp_csi_resource.periodicity, &slot_cfg)) {
        if (csi_rs_list.full()) {
          srslog::fetch_basic_logger("MAC-NR").error("SCHED: Failed to allocate NZP-CSI RS");
          return;
        }
        csi_rs_list.push_back(nzp_csi_resource);
      }
    }
  }
}

void sched_ssb_basic(const slot_point&      sl_point,
                     uint32_t               ssb_periodicity,
                     const srsran_mib_nr_t& mib,
                     ssb_list&              ssb_list)
{
  if (ssb_list.full()) {
    srslog::fetch_basic_logger("MAC-NR").error("SCHED: Failed to allocate SSB");
    return;
  }
  // If the periodicity is 0, it means that the parameter was not passed by the upper layers.
  // In that case, we use default value of 5ms (see Clause 4.1, TS 38.213)
  if (ssb_periodicity == 0) {
    ssb_periodicity = DEFAULT_SSB_PERIODICITY;
  }

  uint32_t sl_cnt = sl_point.to_uint();
  // Perform mod operation of slot index by ssb_periodicity;
  // "ssb_periodicity * nof_slots_per_subframe" gives the number of slots in 1 ssb_periodicity time interval
  uint32_t sl_point_mod = sl_cnt % (ssb_periodicity * (uint32_t)sl_point.nof_slots_per_subframe());

  // code below is simplified, it assumes 15kHz subcarrier spacing and sub 3GHz carrier
  if (sl_point_mod == 0) {
    ssb_t           ssb_msg = {};
    srsran_mib_nr_t mib_msg = mib;
    mib_msg.sfn             = sl_point.sfn();
    mib_msg.hrf             = (sl_point.slot_idx() % SRSRAN_NSLOTS_PER_FRAME_NR(srsran_subcarrier_spacing_15kHz) >=
                   SRSRAN_NSLOTS_PER_FRAME_NR(srsran_subcarrier_spacing_15kHz) / 2);
    // This corresponds to "Position in Burst" = 1000
    mib_msg.ssb_idx = 0;
    // Remaining MIB parameters remain constant

    // Pack mib message to be sent to PHY
    int packing_ret_code = srsran_pbch_msg_nr_mib_pack(&mib_msg, &ssb_msg.pbch_msg);
    srsran_assert(packing_ret_code == SRSRAN_SUCCESS, "SSB packing returned en error");
    ssb_list.push_back(ssb_msg);
  }
}

void sched_dl_signalling(bwp_slot_allocator& bwp_alloc)
{
  const bwp_params_t& bwp_params = bwp_alloc.cfg;
  slot_point          sl_pdcch   = bwp_alloc.get_pdcch_tti();
  bwp_slot_grid&      sl_grid    = bwp_alloc.tx_slot_grid();

  srsran_slot_cfg_t cfg;
  cfg.idx = sl_pdcch.to_uint();

  // Schedule SSB
  sched_ssb_basic(sl_pdcch, bwp_params.cell_cfg.ssb.periodicity_ms, bwp_params.cell_cfg.mib, sl_grid.dl.phy.ssb);

  // Mark SSB region as occupied
  if (!sl_grid.dl.phy.ssb.empty()) {
    float ssb_offset_hz =
        bwp_params.cell_cfg.carrier.ssb_center_freq_hz - bwp_params.cell_cfg.carrier.dl_center_frequency_hz;
    int      ssb_offset_rb = ceil(ssb_offset_hz / (15000.0f * 12));
    int      ssb_start_rb  = bwp_params.cell_cfg.carrier.nof_prb / 2 + ssb_offset_rb - 10;
    uint32_t ssb_len_rb    = 20;
    assert(ssb_start_rb >= 0 && ssb_start_rb + ssb_len_rb < bwp_params.cell_cfg.carrier.nof_prb);
    sl_grid.reserve_pdsch(prb_grant({(uint32_t)ssb_start_rb, ssb_start_rb + ssb_len_rb}));
  }

  // Schedule NZP-CSI-RS
  sched_nzp_csi_rs(bwp_params.cfg.pdsch.nzp_csi_rs_sets, cfg, sl_grid.dl.phy.nzp_csi_rs);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

si_sched::si_sched(const bwp_params_t& bwp_cfg_) :
  bwp_cfg(&bwp_cfg_), logger(srslog::fetch_basic_logger(bwp_cfg_.sched_cfg.logger_name))
{
  for (uint32_t i = 0; i < bwp_cfg->cell_cfg.sibs.size(); ++i) {
    pending_sis.emplace_back();
    si_msg_ctxt_t& si = pending_sis.back();
    si.n              = i;
    si.len_bytes      = bwp_cfg->cell_cfg.sibs[i].len;
    si.period_frames  = bwp_cfg->cell_cfg.sibs[i].period_rf;
    si.win_len_slots  = bwp_cfg->cell_cfg.sibs[i].si_window_slots;
    si.si_softbuffer  = harq_softbuffer_pool::get_instance().get_tx(bwp_cfg->nof_prb);
  }
}

void si_sched::run_slot(bwp_slot_allocator& bwp_alloc)
{
  if (not bwp_alloc.cfg.cfg.pdcch.coreset_present[0]) {
    // CORESET#0 must be present, otherwise SIs are not allocated
    // TODO: provide proper config
    return;
  }
  const uint32_t si_aggr_level = 2;
  const uint32_t ss_id         = 0;
  slot_point     sl_pdcch      = bwp_alloc.get_pdcch_tti();
  prb_bitmap     prbs          = bwp_alloc.res_grid()[sl_pdcch].pdschs.occupied_prbs(ss_id, srsran_dci_format_nr_1_0);

  // Update SI windows
  uint32_t N = bwp_cfg->slots.size();
  for (si_msg_ctxt_t& si : pending_sis) {
    uint32_t x = (si.n - 1) * si.win_len_slots;

    if (not si.win_start.valid()) {
      bool start_window;
      if (si.n == 0) {
        // SIB1 (slot index zero of even frames)
        start_window = sl_pdcch.slot_idx() == 0 and sl_pdcch.sfn() % 2 == 0;
      } else {
        // 5.2.2.3.2 - Acquisition of SI message
        start_window =
            (sl_pdcch.sfn() % si.period_frames == x / N) and sl_pdcch.slot_idx() == x % bwp_cfg->slots.size();
      }
      if (start_window) {
        // If start of SI message window
        si.win_start = sl_pdcch;
        si.n_tx      = 0;
      }
    } else if (si.win_start + si.win_len_slots >= sl_pdcch) {
      // If end of SI message window
      srsran_always_assert(
          si.n == 0, "SCHED: Could not allocate SIB1, len=%d. Cause: %s", si.len_bytes, to_string(si.result));
      logger.warning(
          "SCHED: Could not allocate SI message idx=%d, len=%d. Cause: %s", si.n, si.len_bytes, to_string(si.result));
      si.win_start.clear();
    }
  }

  // Schedule pending SIBs
  if (not bwp_cfg->slots[sl_pdcch.slot_idx()].is_dl) {
    return;
  }
  for (si_msg_ctxt_t& si : pending_sis) {
    if (not si.win_start.valid() or si.n_tx >= MAX_SIB_TX) {
      continue;
    }

    // Attempt grants with increasing number of PRBs (if the number of PRBs is too low, the coderate is invalid)
    si.result          = alloc_result::invalid_coderate;
    uint32_t     nprbs = 8;
    prb_interval grant = find_empty_interval_of_length(prbs, nprbs, 0);
    if (grant.length() >= nprbs) {
      si.result = bwp_alloc.alloc_si(si_aggr_level, si.n, si.n_tx, grant, *si.si_softbuffer.get());
      if (si.result == alloc_result::success) {
        // SIB scheduled successfully
        si.win_start.clear();
        si.n_tx++;
        if (si.n == 0) {
          logger.debug("SCHED: Allocated SIB1, len=%d.", si.len_bytes);
        } else {
          logger.debug("SCHED: Allocated SI message idx=%d, len=%d.", si.n, si.len_bytes);
        }
      }
    }
    if (si.result != alloc_result::success) {
      logger.warning("SCHED: Failed to allocate SI%s%d ntx=%d", si.n == 0 ? "B" : " message idx=", si.n + 1, si.n_tx);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace sched_nr_impl
} // namespace srsenb
