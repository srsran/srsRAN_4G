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
  srsran_assert(cfg.coresets[0].has_value(), "At least one coreset has to be active per BWP");
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

ue_cfg_extended::ue_cfg_extended(uint16_t rnti_, const ue_cfg_t& uecfg) : ue_cfg_t(uecfg), rnti(rnti_)
{
  cc_params.resize(carriers.size());
  for (uint32_t cc = 0; cc < cc_params.size(); ++cc) {
    cc_params[cc].bwps.resize(1);
    auto& bwp = cc_params[cc].bwps[0];
    for (uint32_t ssid = 0; ssid < SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE; ++ssid) {
      if (phy_cfg.pdcch.search_space_present[ssid]) {
        bwp.search_spaces.emplace_back();
        bwp.search_spaces.back().cfg = &phy_cfg.pdcch.search_space[ssid];
      }
    }
    for (uint32_t csid = 0; csid < SRSRAN_UE_DL_NR_MAX_NOF_CORESET; ++csid) {
      if (phy_cfg.pdcch.coreset_present[csid]) {
        bwp.coresets.emplace_back();
        auto& coreset = bwp.coresets.back();
        coreset.cfg   = &phy_cfg.pdcch.coreset[csid];
        for (auto& ss : bwp.search_spaces) {
          if (ss.cfg->coreset_id == csid + 1) {
            coreset.ss_list.push_back(&ss);
            get_dci_locs(*coreset.cfg, *coreset.ss_list.back()->cfg, rnti, coreset.cce_positions);
          }
        }
      }
    }
  }
}

} // namespace sched_nr_impl
} // namespace srsenb