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

#include "srsgnb/hdr/stack/mac/sched_nr_interface_utils.h"

namespace srsenb {

srsran::phy_cfg_nr_t get_common_ue_phy_cfg(const sched_nr_interface::cell_cfg_t& cfg)
{
  srsran::phy_cfg_nr_t ue_phy_cfg;

  ue_phy_cfg.csi = {}; // disable CSI until RA is complete

  ue_phy_cfg.carrier  = cfg.carrier;
  ue_phy_cfg.duplex   = cfg.duplex;
  ue_phy_cfg.ssb      = cfg.ssb;
  ue_phy_cfg.pdcch    = cfg.bwps[0].pdcch;
  ue_phy_cfg.pdsch    = cfg.bwps[0].pdsch;
  ue_phy_cfg.pusch    = cfg.bwps[0].pusch;
  ue_phy_cfg.pucch    = cfg.bwps[0].pucch;
  ue_phy_cfg.prach    = cfg.bwps[0].prach;
  ue_phy_cfg.harq_ack = cfg.bwps[0].harq_ack;

  // remove UE-specific SearchSpaces (they will be added later via RRC)
  for (uint32_t i = 0; i < SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE; ++i) {
    if (ue_phy_cfg.pdcch.search_space_present[i] and
        ue_phy_cfg.pdcch.search_space[i].type == srsran_search_space_type_ue) {
      ue_phy_cfg.pdcch.search_space_present[i] = false;
      ue_phy_cfg.pdcch.search_space[i]         = {};
    }
  }

  return ue_phy_cfg;
}

} // namespace srsenb