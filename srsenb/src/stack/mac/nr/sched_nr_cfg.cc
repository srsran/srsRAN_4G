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

#include "srsenb/hdr/stack/mac/nr/sched_nr_cfg.h"
#include "srsenb/hdr/stack/mac/nr/sched_nr_phy.h"

namespace srsenb {
namespace sched_nr_impl {

bwp_params::bwp_params(const cell_cfg_t& cell, const sched_cfg_t& sched_cfg_, uint32_t cc_, uint32_t bwp_id_) :
  cell_cfg(cell), sched_cfg(sched_cfg_), cc(cc_), bwp_id(bwp_id_), cfg(cell.bwps[bwp_id_])
{
  P     = get_P(cfg.rb_width, cfg.pdsch.rbg_size_cfg_1);
  N_rbg = get_nof_rbgs(cfg.rb_width, cfg.start_rb, cfg.pdsch.rbg_size_cfg_1);

  srsran_assert(bwp_id != 0 or cfg.pdcch.coreset_present[0], "CORESET#0 has to be active for initial BWP");
}

sched_cell_params::sched_cell_params(uint32_t cc_, const cell_cfg_t& cell, const sched_cfg_t& sched_cfg_) :
  cc(cc_), cell_cfg(cell), sched_cfg(sched_cfg_)
{
  bwps.reserve(cell.bwps.size());
  for (uint32_t i = 0; i < cell.bwps.size(); ++i) {
    bwps.emplace_back(cell, sched_cfg_, cc, i);
  }
  srsran_assert(not bwps.empty(), "No BWPs were configured");
}

sched_params::sched_params(const sched_cfg_t& sched_cfg_) : sched_cfg(sched_cfg_) {}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

bwp_ue_cfg::bwp_ue_cfg(uint16_t rnti_, const bwp_params& bwp_cfg_, const ue_cfg_t& uecfg_) :
  rnti(rnti_), cfg_(&uecfg_), bwp_cfg(&bwp_cfg_)
{
  std::fill(ss_id_to_cce_idx.begin(), ss_id_to_cce_idx.end(), SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE);
  const auto& pdcch = phy().pdcch;
  for (uint32_t i = 0; i < SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE; ++i) {
    if (pdcch.search_space_present[i]) {
      const auto& ss = pdcch.search_space[i];
      srsran_assert(pdcch.coreset_present[ss.coreset_id],
                    "Invalid mapping search space id=%d to coreset id=%d",
                    ss.id,
                    ss.coreset_id);
      const auto& coreset = pdcch.coreset[ss.coreset_id];
      cce_positions_list.emplace_back();
      get_dci_locs(coreset, ss, rnti, cce_positions_list.back());
      ss_id_to_cce_idx[ss.id] = cce_positions_list.size() - 1;
    }
  }
}

ue_cfg_extended::ue_cfg_extended(uint16_t rnti_, const ue_cfg_t& uecfg) : ue_cfg_t(uecfg), rnti(rnti_)
{
  cc_params.resize(carriers.size());
  for (uint32_t cc = 0; cc < cc_params.size(); ++cc) {
    cc_params[cc].bwps.resize(1);
    auto& bwp = cc_params[cc].bwps[0];
    for (uint32_t ssid = 0; ssid < SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE; ++ssid) {
      if (phy_cfg.pdcch.search_space_present[ssid]) {
        auto& ss = phy_cfg.pdcch.search_space[ssid];
        bwp.ss_list[ss.id].emplace();
        bwp.ss_list[ss.id]->cfg = &ss;
        get_dci_locs(phy_cfg.pdcch.coreset[ss.coreset_id], ss, rnti, bwp.ss_list[ss.id]->cce_positions);
      }
    }
    for (uint32_t idx = 0; idx < SRSRAN_UE_DL_NR_MAX_NOF_CORESET; ++idx) {
      if (phy_cfg.pdcch.coreset_present[idx]) {
        bwp.coresets.emplace_back();
        auto& coreset = bwp.coresets.back();
        coreset.cfg   = &phy_cfg.pdcch.coreset[idx];
        for (auto& ss : bwp.ss_list) {
          if (ss.has_value() and ss->cfg->coreset_id == coreset.cfg->id) {
            coreset.ss_list.push_back(ss->cfg->id);
          }
        }
      }
    }
  }
}

} // namespace sched_nr_impl
} // namespace srsenb