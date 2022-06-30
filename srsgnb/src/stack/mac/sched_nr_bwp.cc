/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsgnb/hdr/stack/mac/sched_nr_bwp.h"
#include "srsran/common/standard_streams.h"
#include "srsran/common/string_helpers.h"

namespace srsenb {
namespace sched_nr_impl {

ra_sched::ra_sched(const bwp_params_t& bwp_cfg_) :
  bwp_cfg(&bwp_cfg_), logger(srslog::fetch_basic_logger(bwp_cfg_.sched_cfg.logger_name))
{}

alloc_result
ra_sched::allocate_pending_rar(bwp_slot_allocator& slot_grid, const pending_rar_t& rar, uint32_t& nof_grants_alloc)
{
  const uint32_t rar_aggr_level = 2;
  prb_bitmap     prbs           = slot_grid.res_grid()[slot_grid.get_pdcch_tti()].pdschs.occupied_prbs(
      bwp_cfg->cfg.pdcch.ra_search_space.id, srsran_dci_format_nr_1_0);

  alloc_result                            ret = alloc_result::other_cause;
  srsran::const_span<dl_sched_rar_info_t> msg3_grants{rar.msg3_grant};
  for (nof_grants_alloc = rar.msg3_grant.size(); nof_grants_alloc > 0; nof_grants_alloc--) {
    ret                    = alloc_result::invalid_coderate;
    uint32_t start_prb_idx = 0;
    for (uint32_t nprb = 4; nprb < bwp_cfg->cfg.rb_width and ret == alloc_result::invalid_coderate; ++nprb) {
      prb_interval interv = find_empty_interval_of_length(prbs, nprb, start_prb_idx);
      start_prb_idx       = interv.start();
      if (interv.length() == nprb) {
        ret =
            slot_grid.alloc_rar_and_msg3(rar.ra_rnti, rar_aggr_level, interv, msg3_grants.subspan(0, nof_grants_alloc));
      } else {
        ret = alloc_result::no_sch_space;
      }
    }

    // If allocation was not successful because there were not enough RBGs, try allocating fewer Msg3 grants
    if (ret != alloc_result::invalid_coderate and ret != alloc_result::no_sch_space) {
      break;
    }
  }
  if (ret != alloc_result::success) {
    logger.info("SCHED: RAR allocation for L=%d was postponed. Cause=%s", rar_aggr_level, to_string(ret));
  }
  return ret;
}

void ra_sched::run_slot(bwp_slot_allocator& slot_alloc)
{
  slot_point pdcch_slot = slot_alloc.get_pdcch_tti();
  slot_point msg3_slot  = pdcch_slot + bwp_cfg->pusch_ra_list[0].msg3_delay;
  if (not bwp_cfg->slots[pdcch_slot.slot_idx()].is_dl or not bwp_cfg->slots[msg3_slot.slot_idx()].is_ul) {
    // RAR only allowed if PDCCH is available and respective Msg3 slot is available for UL
    return;
  }

  for (auto it = pending_rars.begin(); it != pending_rars.end();) {
    pending_rar_t& rar = *it;

    // In case of RAR outside RAR window:
    // - if window has passed, discard RAR
    // - if window hasn't started, stop loop, as RARs are ordered by TTI
    if (not rar.rar_win.contains(pdcch_slot)) {
      if (pdcch_slot >= rar.rar_win.stop()) {
        fmt::memory_buffer str_buffer;
        fmt::format_to(str_buffer,
                       "SCHED: Could not transmit RAR within the window={}, PRACH={}, RAR={}",
                       rar.rar_win,
                       rar.prach_slot,
                       pdcch_slot);
        srsran::console("%s\n", srsran::to_c_str(str_buffer));
        logger.warning("%s", srsran::to_c_str(str_buffer));
        it = pending_rars.erase(it);
        continue;
      }
      return;
    }

    // Try to schedule DCIs + RBGs for RAR Grants
    uint32_t     nof_rar_allocs = 0;
    alloc_result ret            = allocate_pending_rar(slot_alloc, rar, nof_rar_allocs);

    if (ret == alloc_result::success) {
      // If RAR allocation was successful:
      // - in case all Msg3 grants were allocated, remove pending RAR, and continue with following RAR
      // - otherwise, erase only Msg3 grants that were allocated, and stop iteration

      if (nof_rar_allocs == rar.msg3_grant.size()) {
        it = pending_rars.erase(it);
      } else {
        std::copy(rar.msg3_grant.begin() + nof_rar_allocs, rar.msg3_grant.end(), rar.msg3_grant.begin());
        rar.msg3_grant.resize(rar.msg3_grant.size() - nof_rar_allocs);
        break;
      }
    } else {
      // If RAR allocation was not successful:
      // - in case of unavailable PDCCH space, try next pending RAR allocation
      // - otherwise, stop iteration
      if (ret != alloc_result::no_cch_space) {
        break;
      }
      ++it;
    }
  }
}

/// See TS 38.321, 5.1.3 - RAP transmission
int ra_sched::dl_rach_info(const dl_sched_rar_info_t& rar_info)
{
  // RA-RNTI = 1 + s_id + 14 × t_id + 14 × 80 × f_id + 14 × 80 × 8 × ul_carrier_id
  // s_id = index of the first OFDM symbol (0 <= s_id < 14)
  // t_id = index of first slot of the PRACH (0 <= t_id < 80)
  // f_id = index of the PRACH in the freq domain (0 <= f_id < 8) (for FDD, f_id=0)
  // ul_carrier_id = 0 for NUL and 1 for SUL carrier
  uint16_t ra_rnti = 1 + rar_info.ofdm_symbol_idx + 14 * rar_info.prach_slot.slot_idx() + 14 * 80 * rar_info.freq_idx;

  logger.info("SCHED: New PRACH slot=%d, preamble=%d, ra-rnti=0x%x, temp_crnti=0x%x, ta_cmd=%d, msg3_size=%d",
              rar_info.prach_slot.to_uint(),
              rar_info.preamble_idx,
              ra_rnti,
              rar_info.temp_crnti,
              rar_info.ta_cmd,
              rar_info.msg3_size);

  // find pending rar with same RA-RNTI
  for (pending_rar_t& r : pending_rars) {
    if (r.prach_slot == rar_info.prach_slot and ra_rnti == r.ra_rnti) {
      if (r.msg3_grant.full()) {
        logger.warning("PRACH ignored, as the the maximum number of RAR grants per tti has been reached");
        return SRSRAN_ERROR;
      }
      r.msg3_grant.push_back(rar_info);
      return SRSRAN_SUCCESS;
    }
  }

  // create new RAR
  pending_rar_t p;
  p.ra_rnti                            = ra_rnti;
  p.prach_slot                         = rar_info.prach_slot;
  const static uint32_t prach_duration = 1;
  for (slot_point t = rar_info.prach_slot + prach_duration; t < rar_info.prach_slot + bwp_cfg->slots.size(); ++t) {
    if (bwp_cfg->slots[t.slot_idx()].is_dl) {
      p.rar_win = {t, t + bwp_cfg->cfg.rar_window_size};
      break;
    }
  }
  p.msg3_grant.push_back(rar_info);
  pending_rars.push_back(p);

  return SRSRAN_SUCCESS;
}

bwp_manager::bwp_manager(const bwp_params_t& bwp_cfg) :
  cfg(&bwp_cfg), ra(bwp_cfg), si(bwp_cfg), grid(bwp_cfg), data_sched(new sched_nr_time_rr())
{}

} // namespace sched_nr_impl
} // namespace srsenb
