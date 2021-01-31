/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */
#include "srslte/phy/ue/ue_ul_nr_data.h"
#include "srslte/phy/utils/debug.h"

typedef struct {
  srslte_sch_mapping_type_t mapping;
  uint32_t                  K2;
  uint32_t                  S;
  uint32_t                  L;
} ue_ul_time_resource_t;

static const ue_ul_time_resource_t ue_ul_default_A_lut[16] = {{srslte_sch_mapping_type_A, 0, 0, 14},
                                                              {srslte_sch_mapping_type_A, 0, 0, 12},
                                                              {srslte_sch_mapping_type_A, 0, 0, 10},
                                                              {srslte_sch_mapping_type_B, 0, 2, 10},
                                                              {srslte_sch_mapping_type_B, 0, 4, 10},
                                                              {srslte_sch_mapping_type_B, 0, 4, 8},
                                                              {srslte_sch_mapping_type_B, 0, 4, 6},
                                                              {srslte_sch_mapping_type_A, 1, 0, 14},
                                                              {srslte_sch_mapping_type_A, 1, 0, 12},
                                                              {srslte_sch_mapping_type_A, 1, 0, 10},
                                                              {srslte_sch_mapping_type_A, 2, 0, 14},
                                                              {srslte_sch_mapping_type_A, 2, 0, 12},
                                                              {srslte_sch_mapping_type_A, 2, 0, 10},
                                                              {srslte_sch_mapping_type_B, 0, 8, 6},
                                                              {srslte_sch_mapping_type_A, 3, 0, 14},
                                                              {srslte_sch_mapping_type_A, 3, 0, 10}};

int srslte_ue_ul_nr_pdsch_time_resource_default_A(uint32_t m, srslte_sch_grant_nr_t* grant)
{
  if (grant == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (m >= 16) {
    ERROR("m (%d) is out-of-range\n", m);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Select mapping
  grant->mapping = ue_ul_default_A_lut[m].mapping;
  grant->k2      = ue_ul_default_A_lut[m].K2;
  grant->S       = ue_ul_default_A_lut[m].S;
  grant->L       = ue_ul_default_A_lut[m].L;

  return SRSLTE_SUCCESS;
}

int srslte_ue_ul_nr_nof_dmrs_cdm_groups_without_data_format_0_0(const srslte_sch_cfg_nr_t* cfg,
                                                                srslte_sch_grant_nr_t*     grant)
{
  if (cfg == NULL || grant == NULL) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  /* According to TS 38.214 V15.10.0 6.2.2 UE DM-RS transmission procedure:
   * For PUSCH scheduled by DCI format 0_0 or by activation DCI format 0_0 with CRC scrambled by CS-RNTI, the UE
   * shall assume the number of DM-RS CDM groups without data is 1 which corresponds to CDM group 0 for the case of
   * PUSCH with allocation duration of 2 or less OFDM symbols with transform precoding disabled, the UE shall assume
   * that the number of DM-RS CDM groups without data is 3 which corresponds to CDM group {0,1,2} for the case of PUSCH
   * scheduled by activation DCI format 0_0 and the dmrs-Type in cg-DMRS-Configuration equal to 'type2' and the PUSCH
   * allocation duration being more than 2 OFDM symbols, and the UE shall assume that the number of DM-RS CDM groups
   * without data is 2 which corresponds to CDM group {0,1} for all other cases.
   */
  if (grant->L <= 2 && !cfg->enable_transform_precoder) {
    grant->nof_dmrs_cdm_groups_without_data = 1;
    //  } else if (grant->L > 2 && cfg->dmrs_cg.type == srslte_dmrs_sch_type_2){
    //    grant->nof_dmrs_cdm_groups_without_data = 3;
  } else {
    grant->nof_dmrs_cdm_groups_without_data = 2;
  }

  return SRSLTE_SUCCESS;
}