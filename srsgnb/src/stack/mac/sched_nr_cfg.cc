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

#include "srsgnb/hdr/stack/mac/sched_nr_cfg.h"
#include "srsgnb/hdr/stack/mac/sched_nr_helpers.h"
#include "srsran/adt/optional_array.h"
#include "srsran/asn1/rrc_nr_utils.h"
extern "C" {
#include "srsran/phy/phch/ra_ul_nr.h"
}

namespace srsenb {
namespace sched_nr_impl {

void get_dci_locs(const srsran_coreset_t&      coreset,
                  const srsran_search_space_t& search_space,
                  uint16_t                     rnti,
                  bwp_cce_pos_list&            cce_locs)
{
  for (uint32_t sl = 0; sl < SRSRAN_NOF_SF_X_FRAME; ++sl) {
    for (uint32_t agg_idx = 0; agg_idx < MAX_NOF_AGGR_LEVELS; ++agg_idx) {
      pdcch_cce_pos_list pdcch_locs;
      cce_locs[sl][agg_idx].resize(pdcch_locs.capacity());
      uint32_t n =
          srsran_pdcch_nr_locations_coreset(&coreset, &search_space, rnti, agg_idx, sl, cce_locs[sl][agg_idx].data());
      cce_locs[sl][agg_idx].resize(n);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bwp_params_t::bwp_params_t(const cell_config_manager& cell, uint32_t bwp_id_, const sched_nr_bwp_cfg_t& bwp_cfg) :
  cell_cfg(cell),
  sched_cfg(cell.sched_args),
  cc(cell.cc),
  bwp_id(bwp_id_),
  cfg(bwp_cfg),
  nof_prb(cell_cfg.carrier.nof_prb),
  logger(srslog::fetch_basic_logger(sched_cfg.logger_name)),
  cached_empty_prb_mask(bwp_cfg.rb_width, bwp_cfg.start_rb, bwp_cfg.pdsch.rbg_size_cfg_1)
{
  srsran_assert(cfg.pdcch.ra_search_space_present, "BWPs without RA search space not supported");
  const uint32_t ra_coreset_id = cfg.pdcch.ra_search_space.coreset_id;

  P     = get_P(cfg.rb_width, cfg.pdsch.rbg_size_cfg_1);
  N_rbg = get_nof_rbgs(cfg.rb_width, cfg.start_rb, cfg.pdsch.rbg_size_cfg_1);

  for (const srsran_coreset_t& cs : view_active_coresets(cfg.pdcch)) {
    coresets.emplace(cs.id);
    uint32_t rb_start                         = srsran_coreset_start_rb(&cs);
    coresets[cs.id].prb_limits                = prb_interval{rb_start, rb_start + srsran_coreset_get_bw(&cs)};
    coresets[cs.id].usable_common_ss_prb_mask = cached_empty_prb_mask;

    // TS 38.214, 5.1.2.2 - For DCI format 1_0 and common search space, lowest RB of the CORESET is the RB index = 0
    coresets[cs.id].usable_common_ss_prb_mask |= prb_interval(0, rb_start);
    coresets[cs.id].dci_1_0_prb_limits = prb_interval{rb_start, cfg.rb_width};

    // TS 38.214, 5.1.2.2.2 - when DCI format 1_0, common search space and CORESET#0 is configured for the cell,
    // RA type 1 allocs shall be within the CORESET#0 region
    if (cfg.pdcch.coreset_present[0]) {
      coresets[cs.id].dci_1_0_prb_limits = coresets[cs.id].prb_limits;
      coresets[cs.id].usable_common_ss_prb_mask |= prb_interval(coresets[cs.id].prb_limits.stop(), cfg.rb_width);
    }
  }

  // Derive params of individual slots
  uint32_t nof_slots = SRSRAN_NSLOTS_PER_FRAME_NR(cfg.numerology_idx);
  for (size_t sl = 0; sl < nof_slots; ++sl) {
    slot_cfg sl_cfg{};
    sl_cfg.is_dl = srsran_duplex_nr_is_dl(&cell_cfg.duplex, cfg.numerology_idx, sl);
    sl_cfg.is_ul = srsran_duplex_nr_is_ul(&cell_cfg.duplex, cfg.numerology_idx, sl);
    slots.push_back(sl_cfg);
  }

  pusch_ra_list.resize(cfg.pusch.nof_common_time_ra);
  srsran_sch_grant_nr_t grant;
  for (uint32_t m = 0; m < cfg.pusch.nof_common_time_ra; ++m) {
    int ret =
        srsran_ra_ul_nr_time(&cfg.pusch, srsran_rnti_type_ra, srsran_search_space_type_rar, ra_coreset_id, m, &grant);
    srsran_assert(ret == SRSRAN_SUCCESS, "Failed to obtain RA config");
    pusch_ra_list[m].msg3_delay = grant.k;
    ret = srsran_ra_ul_nr_time(&cfg.pusch, srsran_rnti_type_c, srsran_search_space_type_ue, ra_coreset_id, m, &grant);
    pusch_ra_list[m].K = grant.k;
    pusch_ra_list[m].S = grant.S;
    pusch_ra_list[m].L = grant.L;
    srsran_assert(ret == SRSRAN_SUCCESS, "Failed to obtain RA config");
  }
  srsran_assert(not pusch_ra_list.empty(), "Time-Domain Resource Allocation not valid");

  for (uint32_t sl = 0; sl < SRSRAN_NOF_SF_X_FRAME; ++sl) {
    for (uint32_t agg_idx = 0; agg_idx < MAX_NOF_AGGR_LEVELS; ++agg_idx) {
      rar_cce_list[sl][agg_idx].resize(SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR);
      int n = srsran_pdcch_nr_locations_coreset(&cell_cfg.bwps[0].cfg.pdcch.coreset[ra_coreset_id],
                                                &cell_cfg.bwps[0].cfg.pdcch.ra_search_space,
                                                0,
                                                agg_idx,
                                                sl,
                                                rar_cce_list[sl][agg_idx].data());
      srsran_assert(n >= 0, "Failed to configure RAR DCI locations");
      rar_cce_list[sl][agg_idx].resize(n);
    }
  }

  for (uint32_t ss_id = 0; ss_id < SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE; ++ss_id) {
    if (not cell_cfg.bwps[0].cfg.pdcch.search_space_present[ss_id]) {
      continue;
    }
    auto& ss      = cell_cfg.bwps[0].cfg.pdcch.search_space[ss_id];
    auto& coreset = cell_cfg.bwps[0].cfg.pdcch.coreset[ss.coreset_id];
    common_cce_list.emplace(ss_id);
    bwp_cce_pos_list& ss_cce_list = common_cce_list[ss_id];
    for (uint32_t sl = 0; sl < SRSRAN_NOF_SF_X_FRAME; ++sl) {
      for (uint32_t agg_idx = 0; agg_idx < MAX_NOF_AGGR_LEVELS; ++agg_idx) {
        ss_cce_list[sl][agg_idx].resize(SRSRAN_SEARCH_SPACE_MAX_NOF_CANDIDATES_NR);
        int n = srsran_pdcch_nr_locations_coreset(
            &coreset, &ss, SRSRAN_SIRNTI, agg_idx, sl, ss_cce_list[sl][agg_idx].data());
        srsran_assert(n >= 0, "Failed to configure DCI locations of search space id=%d", ss_id);
        ss_cce_list[sl][agg_idx].resize(n);
      }
    }
  }
}

cell_config_manager::cell_config_manager(uint32_t                   cc_,
                                         const sched_nr_cell_cfg_t& cell,
                                         const sched_args_t&        sched_args_) :
  cc(cc_), sched_args(sched_args_), default_ue_phy_cfg(get_common_ue_phy_cfg(cell)), sibs(cell.sibs)
{
  carrier.pci                    = cell.pci;
  carrier.dl_center_frequency_hz = cell.dl_center_frequency_hz;
  carrier.ul_center_frequency_hz = cell.ul_center_frequency_hz;
  carrier.ssb_center_freq_hz     = cell.ssb_center_freq_hz;
  carrier.nof_prb                = cell.dl_cell_nof_prb;
  carrier.start                  = 0; // TODO: Check
  carrier.max_mimo_layers        = cell.nof_layers;
  carrier.offset_to_carrier      = cell.dl_cfg_common.freq_info_dl.scs_specific_carrier_list[0].offset_to_carrier;
  carrier.scs = (srsran_subcarrier_spacing_t)cell.dl_cfg_common.init_dl_bwp.generic_params.subcarrier_spacing.value;

  // TDD-UL-DL-ConfigCommon
  duplex.mode = SRSRAN_DUPLEX_MODE_FDD;
  if (cell.tdd_ul_dl_cfg_common.has_value()) {
    bool success = srsran::make_phy_tdd_cfg(*cell.tdd_ul_dl_cfg_common, &duplex);
    srsran_assert(success, "Failed to generate Cell TDD config");
  }

  // Set SSB params
  make_ssb_cfg(cell, &ssb);

  // MIB
  make_mib_cfg(cell, &mib);

  bwps.reserve(cell.bwps.size());
  for (uint32_t i = 0; i < cell.bwps.size(); ++i) {
    bwps.emplace_back(*this, i, cell.bwps[i]);
  }
  srsran_assert(not bwps.empty(), "No BWPs were configured");
}

sched_params_t::sched_params_t(const sched_args_t& sched_cfg_) : sched_cfg(sched_cfg_)
{
  srsran_assert(sched_cfg.fixed_ul_mcs >= 0, "Dynamic DL MCS not supported");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace sched_nr_impl
} // namespace srsenb