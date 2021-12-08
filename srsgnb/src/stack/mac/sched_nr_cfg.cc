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

#include "srsgnb/hdr/stack/mac/sched_nr_cfg.h"
#include "srsgnb/hdr/stack/mac/sched_nr_helpers.h"
#include "srsran/adt/optional_array.h"
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

bwp_params_t::bwp_params_t(const cell_cfg_t& cell, const sched_args_t& sched_cfg_, uint32_t cc_, uint32_t bwp_id_) :
  cell_cfg(cell),
  sched_cfg(sched_cfg_),
  cc(cc_),
  bwp_id(bwp_id_),
  cfg(cell.bwps[bwp_id_]),
  logger(srslog::fetch_basic_logger(sched_cfg_.logger_name)),
  cached_empty_prb_mask(cell.bwps[bwp_id_].rb_width,
                        cell.bwps[bwp_id_].start_rb,
                        cell.bwps[bwp_id_].pdsch.rbg_size_cfg_1)
{
  srsran_assert(cfg.pdcch.ra_search_space_present, "BWPs without RA search space not supported");
  const uint32_t ra_coreset_id = cfg.pdcch.ra_search_space.coreset_id;

  P     = get_P(cfg.rb_width, cfg.pdsch.rbg_size_cfg_1);
  N_rbg = get_nof_rbgs(cfg.rb_width, cfg.start_rb, cfg.pdsch.rbg_size_cfg_1);

  for (const srsran_coreset_t& cs : view_active_coresets(cfg.pdcch)) {
    coresets.emplace(cs.id);
    auto& cached_coreset = coresets[cs.id];
    cached_coreset.bw    = srsran_coreset_get_bw(&cs);
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
      int n = srsran_pdcch_nr_locations_coreset(&cell_cfg.bwps[0].pdcch.coreset[ra_coreset_id],
                                                &cell_cfg.bwps[0].pdcch.ra_search_space,
                                                0,
                                                agg_idx,
                                                sl,
                                                rar_cce_list[sl][agg_idx].data());
      srsran_assert(n >= 0, "Failed to configure RAR DCI locations");
      rar_cce_list[sl][agg_idx].resize(n);
    }
  }

  for (uint32_t ss_id = 0; ss_id < SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE; ++ss_id) {
    if (not cell_cfg.bwps[0].pdcch.search_space_present[ss_id]) {
      continue;
    }
    auto& ss      = cell_cfg.bwps[0].pdcch.search_space[ss_id];
    auto& coreset = cell_cfg.bwps[0].pdcch.coreset[ss.coreset_id];
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

    if (SRSRAN_SEARCH_SPACE_IS_COMMON(ss.type)) {
      used_common_prb_masks.emplace(ss_id, cached_empty_prb_mask);
      uint32_t coreset_start = srsran_coreset_start_rb(&cfg.pdcch.coreset[ss.coreset_id]);
      used_common_prb_masks[ss_id] |= prb_interval(0, coreset_start);
      if (ss.coreset_id == 0) {
        uint32_t coreset0_bw = srsran_coreset_get_bw(&cfg.pdcch.coreset[0]);
        used_common_prb_masks[ss_id] |= prb_interval(coreset_start + coreset0_bw, cfg.rb_width);
      }
    }
  }
}

cell_params_t::cell_params_t(uint32_t cc_, const cell_cfg_t& cell, const sched_args_t& sched_cfg_) :
  cc(cc_), cfg(cell), sched_args(sched_cfg_)
{
  bwps.reserve(cell.bwps.size());
  for (uint32_t i = 0; i < cfg.bwps.size(); ++i) {
    bwps.emplace_back(cfg, sched_cfg_, cc, i);
  }
  srsran_assert(not bwps.empty(), "No BWPs were configured");
}

sched_params_t::sched_params_t(const sched_args_t& sched_cfg_) : sched_cfg(sched_cfg_)
{
  srsran_assert(sched_cfg.fixed_dl_mcs >= 0, "Dynamic DL MCS not supported");
  srsran_assert(sched_cfg.fixed_ul_mcs >= 0, "Dynamic DL MCS not supported");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ue_carrier_params_t::ue_carrier_params_t(uint16_t rnti_, const bwp_params_t& bwp_cfg_, const ue_cfg_t& uecfg_) :
  rnti(rnti_), cc(bwp_cfg_.cc), cfg_(&uecfg_), bwp_cfg(&bwp_cfg_), cached_dci_cfg(uecfg_.phy_cfg.get_dci_cfg())
{
  std::fill(ss_id_to_cce_idx.begin(), ss_id_to_cce_idx.end(), SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE);
  const auto& pdcch        = phy().pdcch;
  auto        ss_view      = srsran::make_optional_span(pdcch.search_space, pdcch.search_space_present);
  auto        coreset_view = srsran::make_optional_span(pdcch.coreset, pdcch.coreset_present);
  for (const auto& ss : ss_view) {
    srsran_assert(coreset_view.contains(ss.coreset_id),
                  "Invalid mapping search space id=%d to coreset id=%d",
                  ss.id,
                  ss.coreset_id);
    cce_positions_list.emplace_back();
    get_dci_locs(coreset_view[ss.coreset_id], ss, rnti, cce_positions_list.back());
    ss_id_to_cce_idx[ss.id] = cce_positions_list.size() - 1;
  }
}

int ue_carrier_params_t::find_ss_id(srsran_dci_format_nr_t dci_fmt) const
{
  static const uint32_t           aggr_idx  = 2;                  // TODO: Make it dynamic
  static const srsran_rnti_type_t rnti_type = srsran_rnti_type_c; // TODO: Use TC-RNTI for Msg4

  auto active_ss_lst = view_active_search_spaces(phy().pdcch);

  for (const srsran_search_space_t& ss : active_ss_lst) {
    // Prioritize UE-dedicated SearchSpaces
    if (ss.type == srsran_search_space_type_ue and ss.nof_candidates[aggr_idx] > 0 and
        contains_dci_format(ss, dci_fmt) and is_rnti_type_valid_in_search_space(rnti_type, ss.type)) {
      return ss.id;
    }
  }
  // Search Common SearchSpaces
  for (const srsran_search_space_t& ss : active_ss_lst) {
    if (SRSRAN_SEARCH_SPACE_IS_COMMON(ss.type) and ss.nof_candidates[aggr_idx] > 0 and
        contains_dci_format(ss, dci_fmt) and is_rnti_type_valid_in_search_space(rnti_type, ss.type)) {
      return ss.id;
    }
  }
  return -1;
}

} // namespace sched_nr_impl
} // namespace srsenb