/**
 * Copyright 2013-2020 Software Radio Systems Limited
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
 *  Description:  NR UE downlink physical layer procedures for data
 *
 *                This module is a frontend to all the downlink data channel processing modules.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_UE_DL_NR_DATA_H
#define SRSLTE_UE_DL_NR_DATA_H

#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/phch/pdsch_cfg_nr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Calculates the PDSCH time resource provided by higher layers and stores it in the provided PDSCH NR grant.
 *
 * @remark Defined by TS 38.214 V15.10.0 section 5.1.2.1.1 Determination of the resource allocation table to be used for
 * PDSCH
 *
 * @param pdsch_alloc Flattened PHY PDSCH allocation configuration provided from higher layers
 * @param[out] grant PDSCH mapping type
 * @return Returns SRSLTE_SUCCESS if the provided allocation is valid, otherwise it returns SRSLTE_ERROR code
 */
SRSLTE_API int srslte_ue_dl_nr_pdsch_time_resource_hl(const srslte_pdsch_allocation_t* pdsch_alloc,
                                                      srslte_pdsch_grant_nr_t*         grant);

/**
 * @brief Calculates the PDSCH time resource default A and stores it in the provided PDSCH NR grant. This can be used by
 * SI-RNTI, RA-RNTI, P-RNTI and C-RNTI. See Table 5.1.2.1.1-1 for more details about the usage.
 *
 * @remark Defined by TS 38.214 V15.10.0 Table 5.1.2.1.1-2: Default PDSCH time domain resource allocation A for normal
 * CP
 *
 * @param m Time domain resource assignment field value m of the DCI
 * @param dmrs_typeA_pos DMRS TypeA position provided by higher layers
 * @param[out] grant PDSCH mapping type
 * @return Returns SRSLTE_SUCCESS if the provided allocation is valid, otherwise it returns SRSLTE_ERROR code
 */
SRSLTE_API int srslte_ue_dl_nr_pdsch_time_resource_default_A(uint32_t                      m,
                                                             srslte_dmrs_pdsch_typeA_pos_t dmrs_typeA_pos,
                                                             srslte_pdsch_grant_nr_t*      grant);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_UE_DL_NR_DATA_H
