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

#ifndef SRSRAN_RRC_NR_INTERFACE_TYPES_H
#define SRSRAN_RRC_NR_INTERFACE_TYPES_H

#include "srsran/config.h"
#include "srsran/srsran.h"
#include <string>

namespace srsran {

/***************************
 *      PHY Config
 **************************/

struct phy_cfg_nr_t {
  srsran_tdd_config_nr_t         tdd      = {};
  srsran_sch_hl_cfg_nr_t         pdsch    = {};
  srsran_sch_hl_cfg_nr_t         pusch    = {};
  srsran_pucch_nr_hl_cfg_t       pucch    = {};
  srsran_prach_cfg_t             prach    = {};
  srsran_pdcch_cfg_nr_t          pdcch    = {};
  srsran_ue_dl_nr_harq_ack_cfg_t harq_ack = {};
  srsran_csi_hl_cfg_t            csi      = {};
  srsran_carrier_nr_t            carrier  = {};

  phy_cfg_nr_t() {}

  /**
   * @param carrier
   */
  srsran_dci_cfg_nr_t get_dci_cfg() const
  {
    srsran_dci_cfg_nr_t dci_cfg = {};

    // Assume BWP bandwidth equals full channel bandwidth
    dci_cfg.coreset0_bw       = pdcch.coreset_present[0] ? srsran_coreset_get_bw(&pdcch.coreset[0]) : 0;
    dci_cfg.bwp_dl_initial_bw = carrier.nof_prb;
    dci_cfg.bwp_dl_active_bw  = carrier.nof_prb;
    dci_cfg.bwp_ul_initial_bw = carrier.nof_prb;
    dci_cfg.bwp_ul_active_bw  = carrier.nof_prb;

    // Iterate over all SS to select monitoring options
    for (uint32_t i = 0; i < SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE; i++) {
      // Skip not configured SS
      if (not pdcch.search_space_present[i]) {
        continue;
      }

      // Iterate all configured formats
      for (uint32_t j = 0; j < pdcch.search_space[i].nof_formats; j++) {
        if (pdcch.search_space[i].type == srsran_search_space_type_common_3 &&
            pdcch.search_space[i].formats[j] == srsran_dci_format_nr_0_0) {
          dci_cfg.monitor_common_0_0 = true;
        } else if (pdcch.search_space[i].type == srsran_search_space_type_ue &&
                   pdcch.search_space[i].formats[j] == srsran_dci_format_nr_0_0) {
          dci_cfg.monitor_0_0_and_1_0 = true;
        } else if (pdcch.search_space[i].type == srsran_search_space_type_ue &&
                   pdcch.search_space[i].formats[j] == srsran_dci_format_nr_0_1) {
          dci_cfg.monitor_0_1_and_1_1 = true;
        }
      }
    }

    // Set PUSCH parameters
    dci_cfg.enable_sul     = false;
    dci_cfg.enable_hopping = false;

    // Set Format 0_1 and 1_1 parameters
    dci_cfg.carrier_indicator_size = 0;
    dci_cfg.harq_ack_codebok       = harq_ack.harq_ack_codebook;
    dci_cfg.nof_rb_groups          = 0;

    // Format 0_1 specific configuration (for PUSCH only)
    dci_cfg.nof_ul_bwp      = 0;
    dci_cfg.nof_ul_time_res = (pusch.nof_dedicated_time_ra > 0)
                                  ? pusch.nof_dedicated_time_ra
                                  : (pusch.nof_common_time_ra > 0) ? pusch.nof_common_time_ra : SRSRAN_MAX_NOF_TIME_RA;
    dci_cfg.nof_srs                        = 1;
    dci_cfg.nof_ul_layers                  = 1;
    dci_cfg.pusch_nof_cbg                  = 0;
    dci_cfg.report_trigger_size            = 0;
    dci_cfg.enable_transform_precoding     = false;
    dci_cfg.dynamic_dual_harq_ack_codebook = false;
    dci_cfg.pusch_tx_config_non_codebook   = false;
    dci_cfg.pusch_dmrs_type2               = false;
    dci_cfg.pusch_dmrs_double              = false;
    dci_cfg.pusch_ptrs                     = false;
    dci_cfg.pusch_dynamic_betas            = false;
    dci_cfg.pusch_alloc_type               = pusch.alloc;

    // Format 1_1 specific configuration (for PDSCH only)
    dci_cfg.nof_dl_bwp      = 0;
    dci_cfg.nof_dl_time_res = (pdsch.nof_dedicated_time_ra > 0)
                                  ? pdsch.nof_dedicated_time_ra
                                  : (pdsch.nof_common_time_ra > 0) ? pdsch.nof_common_time_ra : SRSRAN_MAX_NOF_TIME_RA;
    dci_cfg.nof_aperiodic_zp       = 0;
    dci_cfg.pdsch_nof_cbg          = 0;
    dci_cfg.nof_dl_to_ul_ack       = harq_ack.nof_dl_data_to_ul_ack;
    dci_cfg.pdsch_inter_prb_to_prb = false;
    dci_cfg.pdsch_rm_pattern1      = false;
    dci_cfg.pdsch_rm_pattern2      = false;
    dci_cfg.pdsch_2cw              = false;
    dci_cfg.multiple_scell         = false;
    dci_cfg.pdsch_dmrs_type2       = false;
    dci_cfg.pdsch_dmrs_double      = false;
    dci_cfg.pdsch_tci              = false;
    dci_cfg.pdsch_cbg_flush        = false;
    dci_cfg.pdsch_dynamic_bundling = false;
    dci_cfg.pdsch_alloc_type       = pdsch.alloc;

    return dci_cfg;
  };
};
} // namespace srsran

#endif // SRSRAN_RRC_NR_INTERFACE_TYPES_H
