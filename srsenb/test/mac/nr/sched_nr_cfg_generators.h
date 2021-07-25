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

#ifndef SRSRAN_SCHED_NR_CFG_GENERATORS_H
#define SRSRAN_SCHED_NR_CFG_GENERATORS_H

#include "srsenb/hdr/stack/mac/nr/sched_nr_interface.h"
#include "srsran/common/phy_cfg_nr_default.h"

namespace srsenb {

srsran_coreset_t get_default_coreset0()
{
  srsran_coreset_t coreset{};
  coreset.id                   = 0;
  coreset.duration             = 1;
  coreset.precoder_granularity = srsran_coreset_precoder_granularity_reg_bundle;
  for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; ++i) {
    coreset.freq_resources[i] = i < 8;
  }
  return coreset;
}

sched_nr_interface::cell_cfg_t get_default_cell_cfg(const srsran::phy_cfg_nr_t& phy_cfg)
{
  sched_nr_interface::cell_cfg_t cell_cfg{};

  cell_cfg.carrier = phy_cfg.carrier;
  cell_cfg.tdd     = phy_cfg.tdd;

  cell_cfg.bwps.resize(1);
  cell_cfg.bwps[0].pdcch    = phy_cfg.pdcch;
  cell_cfg.bwps[0].pdsch    = phy_cfg.pdsch;
  cell_cfg.bwps[0].pusch    = phy_cfg.pusch;
  cell_cfg.bwps[0].rb_width = phy_cfg.carrier.nof_prb;

  cell_cfg.bwps[0].pdcch.coreset_present[0]      = true;
  cell_cfg.bwps[0].pdcch.coreset[0]              = get_default_coreset0();
  cell_cfg.bwps[0].pdcch.search_space_present[0] = true;
  auto& ss                                       = cell_cfg.bwps[0].pdcch.search_space[0];
  ss.id                                          = 0;
  ss.coreset_id                                  = 0;
  ss.duration                                    = 1;
  ss.type                                        = srsran_search_space_type_common_0;
  ss.nof_candidates[0]                           = 1;
  ss.nof_candidates[1]                           = 1;
  ss.nof_candidates[2]                           = 1;
  ss.nof_candidates[3]                           = 0;
  ss.nof_candidates[4]                           = 0;
  ss.nof_formats                                 = 1;
  ss.formats[0]                                  = srsran_dci_format_nr_1_0;
  cell_cfg.bwps[0].pdcch.ra_search_space_present = true;
  cell_cfg.bwps[0].pdcch.ra_search_space         = cell_cfg.bwps[0].pdcch.search_space[1];

  return cell_cfg;
}

std::vector<sched_nr_interface::cell_cfg_t> get_default_cells_cfg(
    uint32_t                    nof_sectors,
    const srsran::phy_cfg_nr_t& phy_cfg = srsran::phy_cfg_nr_default_t{srsran::phy_cfg_nr_default_t::reference_cfg_t{}})
{
  std::vector<sched_nr_interface::cell_cfg_t> cells;
  cells.reserve(nof_sectors);
  for (uint32_t i = 0; i < nof_sectors; ++i) {
    cells.push_back(get_default_cell_cfg(phy_cfg));
  }
  return cells;
}

sched_nr_interface::ue_cfg_t get_default_ue_cfg(uint32_t                    nof_cc,
                                                const srsran::phy_cfg_nr_t& phy_cfg = srsran::phy_cfg_nr_default_t{
                                                    srsran::phy_cfg_nr_default_t::reference_cfg_t{}})
{
  sched_nr_interface::ue_cfg_t uecfg{};
  uecfg.carriers.resize(nof_cc);
  for (uint32_t cc = 0; cc < nof_cc; ++cc) {
    uecfg.carriers[cc].cc     = cc;
    uecfg.carriers[cc].active = true;
  }
  uecfg.phy_cfg = phy_cfg;

  return uecfg;
}

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_CFG_GENERATORS_H
