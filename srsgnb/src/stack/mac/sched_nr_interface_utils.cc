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

#include "srsgnb/hdr/stack/mac/sched_nr_interface_utils.h"
#include "srsran/asn1/rrc_nr_utils.h"
#include "srsran/common/band_helper.h"

namespace srsenb {

uint32_t coreset_nof_cces(const srsran_coreset_t& coreset)
{
  const bool* res_active   = &coreset.freq_resources[0];
  uint32_t    nof_freq_res = std::count(res_active, res_active + SRSRAN_CORESET_FREQ_DOMAIN_RES_SIZE, true);
  return nof_freq_res * coreset.duration;
}

void make_mib_cfg(const sched_nr_cell_cfg_t& cfg, srsran_mib_nr_t* mib)
{
  *mib            = {};
  mib->scs_common = (srsran_subcarrier_spacing_t)cfg.dl_cfg_common.init_dl_bwp.generic_params.subcarrier_spacing.value;
  mib->ssb_offset = cfg.ssb_offset;
  mib->dmrs_typeA_pos         = (srsran_dmrs_sch_typeA_pos_t)cfg.dmrs_type_a_position.value;
  mib->coreset0_idx           = cfg.pdcch_cfg_sib1.ctrl_res_set_zero;
  mib->ss0_idx                = cfg.pdcch_cfg_sib1.search_space_zero;
  mib->cell_barred            = false;
  mib->intra_freq_reselection = true;
}

void make_ssb_cfg(const sched_nr_cell_cfg_t& cfg, srsran::phy_cfg_nr_t::ssb_cfg_t* ssb)
{
  ssb->periodicity_ms    = cfg.ssb_periodicity_ms;
  ssb->position_in_burst = {};
  uint32_t N             = cfg.ssb_positions_in_burst.in_one_group.length();
  for (uint32_t i = 0; i < N; ++i) {
    ssb->position_in_burst[i] = cfg.ssb_positions_in_burst.in_one_group.get(i);
  }
  if (cfg.ssb_positions_in_burst.group_presence_present) {
    for (uint32_t i = 1; i < cfg.ssb_positions_in_burst.group_presence.length(); ++i) {
      if (cfg.ssb_positions_in_burst.group_presence.get(i)) {
        std::copy(
            ssb->position_in_burst.begin(), ssb->position_in_burst.begin() + N, ssb->position_in_burst.begin() + i * N);
      }
    }
  }
  ssb->scs     = (srsran_subcarrier_spacing_t)cfg.ssb_scs.value;
  ssb->pattern = SRSRAN_SSB_PATTERN_A;
  if (cfg.dl_cfg_common.freq_info_dl.freq_band_list.size() > 0 and
      cfg.dl_cfg_common.freq_info_dl.freq_band_list[0].freq_band_ind_nr_present) {
    uint32_t band = cfg.dl_cfg_common.freq_info_dl.freq_band_list[0].freq_band_ind_nr;
    ssb->pattern  = srsran::srsran_band_helper::get_ssb_pattern(band, ssb->scs);
  }
}

srsran::phy_cfg_nr_t get_common_ue_phy_cfg(const sched_nr_cell_cfg_t& cfg)
{
  srsran::phy_cfg_nr_t ue_phy_cfg;

  // TDD UL-DL config
  ue_phy_cfg.duplex.mode = SRSRAN_DUPLEX_MODE_FDD;
  if (cfg.tdd_ul_dl_cfg_common.has_value()) {
    bool success = srsran::make_phy_tdd_cfg(*cfg.tdd_ul_dl_cfg_common, &ue_phy_cfg.duplex);
    srsran_sanity_check(success, "Failed to convert Cell TDDConfig to UEPHYConfig");
  }

  ue_phy_cfg.pdcch = cfg.bwps[0].pdcch;
  ue_phy_cfg.pdsch = cfg.bwps[0].pdsch;
  ue_phy_cfg.pusch = cfg.bwps[0].pusch;
  ue_phy_cfg.pucch = cfg.bwps[0].pucch;
  srsran::make_phy_rach_cfg(cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common.setup(),
                            cfg.tdd_ul_dl_cfg_common.has_value() ? SRSRAN_DUPLEX_MODE_TDD : SRSRAN_DUPLEX_MODE_FDD,
                            &ue_phy_cfg.prach);
  ue_phy_cfg.harq_ack                       = cfg.bwps[0].harq_ack;
  ue_phy_cfg.csi                            = {}; // disable CSI until RA is complete
  ue_phy_cfg.carrier.pci                    = cfg.pci;
  ue_phy_cfg.carrier.dl_center_frequency_hz = cfg.dl_center_frequency_hz;
  ue_phy_cfg.carrier.ul_center_frequency_hz = cfg.ul_center_frequency_hz;
  ue_phy_cfg.carrier.ssb_center_freq_hz     = cfg.ssb_center_freq_hz;
  ue_phy_cfg.carrier.offset_to_carrier = cfg.dl_cfg_common.freq_info_dl.scs_specific_carrier_list[0].offset_to_carrier;
  ue_phy_cfg.carrier.scs =
      (srsran_subcarrier_spacing_t)cfg.dl_cfg_common.init_dl_bwp.generic_params.subcarrier_spacing.value;
  ue_phy_cfg.carrier.nof_prb         = cfg.dl_cell_nof_prb;
  ue_phy_cfg.carrier.max_mimo_layers = cfg.nof_layers;
  make_ssb_cfg(cfg, &ue_phy_cfg.ssb);

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