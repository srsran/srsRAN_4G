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

/******************************************************************************
 *  @file ue_dl_nr.h
 *
 *  Description:  NR UE uplink physical layer procedures for data
 *
 *                This module is a frontend to all the uplink data channel processing modules.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_UE_UL_DATA_H
#define SRSLTE_UE_UL_DATA_H

#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/phch/phch_cfg_nr.h"

/**
 * @brief Calculates the PUSCH time resource default A and stores it in the provided PUSCH NR grant.
 *
 * @remark Defined by TS 38.214 V15.10.0 Table 6.1.2.1.1-2: Default PUSCH time domain resource allocation A for normal
 * CP
 *
 * @param m Time domain resource assignment field value m of the DCI
 * @param[out] grant PUSCH grant
 * @return Returns SRSLTE_SUCCESS if the provided allocation is valid, otherwise it returns SRSLTE_ERROR code
 */
SRSLTE_API int srslte_ue_ul_nr_pdsch_time_resource_default_A(uint32_t m, srslte_sch_grant_nr_t* grant);

/**
 * @brief Calculates the number of PUSCH-DMRS CDM groups without data for DCI format 0_0
 *
 * @remark Defined by TS 38.214 V15.10.0 6.2.2 UE DM-RS transmission procedure
 *
 * @param cfg PUSCH NR configuration by upper layers
 * @param[out] grant Provides grant pointer to fill
 * @return Returns SRSLTE_SUCCESS if the provided data is valid, otherwise it returns SRSLTE_ERROR code
 */
SRSLTE_API int srslte_ue_ul_nr_nof_dmrs_cdm_groups_without_data_format_0_0(const srslte_sch_cfg_nr_t* cfg,
                                                                           srslte_sch_grant_nr_t*     grant);


#endif // SRSLTE_UE_UL_DATA_H
