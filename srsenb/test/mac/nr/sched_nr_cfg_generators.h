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

#ifndef SRSRAN_SCHED_NR_CFG_GENERATORS_H
#define SRSRAN_SCHED_NR_CFG_GENERATORS_H

#include "srsenb/hdr/stack/mac/nr/sched_nr_interface.h"
#include "srsran/common/phy_cfg_nr_default.h"

namespace srsenb {

inline srsran_coreset_t get_default_coreset0(uint32_t nof_prb)
{
  srsran_coreset_t coreset{};
  coreset.id                   = 0;
  coreset.duration             = 1;
  coreset.precoder_granularity = srsran_coreset_precoder_granularity_reg_bundle;
  for (uint32_t i = 0; i < SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE; ++i) {
    coreset.freq_resources[i] = i < (nof_prb / 6);
  }
  return coreset;
}

inline sched_nr_interface::cell_cfg_t get_default_cell_cfg(
    const srsran::phy_cfg_nr_t& phy_cfg = srsran::phy_cfg_nr_default_t{srsran::phy_cfg_nr_default_t::reference_cfg_t{}})
{
  sched_nr_interface::cell_cfg_t cell_cfg{};

  cell_cfg.carrier = phy_cfg.carrier;
  cell_cfg.duplex  = phy_cfg.duplex;
  cell_cfg.ssb     = phy_cfg.ssb;

  cell_cfg.bwps.resize(1);
  cell_cfg.bwps[0].pdcch    = phy_cfg.pdcch;
  cell_cfg.bwps[0].pdsch    = phy_cfg.pdsch;
  cell_cfg.bwps[0].pusch    = phy_cfg.pusch;
  cell_cfg.bwps[0].rb_width = phy_cfg.carrier.nof_prb;

  cell_cfg.bwps[0].pdcch.coreset_present[0]      = true;
  cell_cfg.bwps[0].pdcch.coreset[0]              = get_default_coreset0(phy_cfg.carrier.nof_prb);
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

inline std::vector<sched_nr_interface::cell_cfg_t> get_default_cells_cfg(
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

inline sched_nr_interface::ue_cfg_t get_rach_ue_cfg(uint32_t cc)
{
  sched_nr_interface::ue_cfg_t uecfg{};

  // set Pcell
  uecfg.carriers.resize(1);
  uecfg.carriers[0].active = true;
  uecfg.carriers[0].cc     = cc;

  // set SRB0 as active
  uecfg.ue_bearers[0].direction = mac_lc_ch_cfg_t::BOTH;

  // set basic PHY config
  uecfg.phy_cfg     = srsran::phy_cfg_nr_default_t{srsran::phy_cfg_nr_default_t::reference_cfg_t{}};
  uecfg.phy_cfg.csi = {};

  return uecfg;
}

inline sched_nr_interface::ue_cfg_t get_default_ue_cfg(
    uint32_t                    nof_cc,
    const srsran::phy_cfg_nr_t& phy_cfg = srsran::phy_cfg_nr_default_t{srsran::phy_cfg_nr_default_t::reference_cfg_t{}})
{
  sched_nr_interface::ue_cfg_t uecfg{};
  uecfg.carriers.resize(nof_cc);
  for (uint32_t cc = 0; cc < nof_cc; ++cc) {
    uecfg.carriers[cc].cc     = cc;
    uecfg.carriers[cc].active = true;
  }
  uecfg.phy_cfg                 = phy_cfg;
  uecfg.ue_bearers[0].direction = mac_lc_ch_cfg_t::BOTH;

  return uecfg;
}

} // namespace srsenb

#endif // SRSRAN_SCHED_NR_CFG_GENERATORS_H
