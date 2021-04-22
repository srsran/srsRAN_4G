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

/******************************************************************************
 *  @file ue_dl_nr.h
 *
 *  Description:  NR UE downlink physical layer procedures for data
 *
 *                This module is a frontend to all the downlink data channel processing modules.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_RA_DL_NR_H
#define SRSRAN_RA_DL_NR_H

#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/phch/dci_nr.h"
#include "srsran/phy/phch/phch_cfg_nr.h"

/**
 * @brief Calculates the PDSCH time resource allocation and stores it in the provided PDSCH NR grant.
 *
 * @remark Defined by TS 38.214 V15.10.0 section 5.1.2.1.1 Determination of the resource allocation table to be used for
 * PDSCH
 *
 * @param cfg Flattened PDSCH configuration provided from higher layers
 * @param rnti_type Type of the RNTI of the corresponding DCI
 * @param ss_type Type of the SS for PDCCH
 * @param coreset_id CORESET identifier associated with the PDCCH transmission
 * @param m Time domain resource assignment field value m provided in DCI
 * @param[out] Provides grant pointer to fill
 * @return Returns SRSRAN_SUCCESS if the provided allocation is valid, otherwise it returns SRSRAN_ERROR code
 */
SRSRAN_API int srsran_ra_dl_nr_time(const srsran_sch_hl_cfg_nr_t*    cfg,
                                    const srsran_rnti_type_t         rnti_type,
                                    const srsran_search_space_type_t ss_type,
                                    const uint32_t                   coreset_id,
                                    const uint8_t                    m,
                                    srsran_sch_grant_nr_t*           grant);

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
 * @return Returns SRSRAN_SUCCESS if the provided allocation is valid, otherwise it returns SRSRAN_ERROR code
 */
SRSRAN_API int
srsran_ra_dl_nr_time_default_A(uint32_t m, srsran_dmrs_sch_typeA_pos_t dmrs_typeA_pos, srsran_sch_grant_nr_t* grant);
/**
 * @brief Calculates the number of PDSCH-DMRS CDM groups without data for DCI format 1_0
 *
 * @remark Defined by TS 38.214 V15.10.0 5.1.6.1.3 CSI-RS for mobility
 *
 * @param cfg PDSCH-DMRS NR configuration by upper layers
 * @param[out] grant Provides grant pointer to fill
 * @return Returns SRSRAN_SUCCESS if the provided data is valid, otherwise it returns SRSRAN_ERROR code
 */
SRSRAN_API int srsran_ra_dl_nr_nof_dmrs_cdm_groups_without_data_format_1_0(const srsran_dmrs_sch_cfg_t* cfg,
                                                                           srsran_sch_grant_nr_t*       grant);

/**
 * @brief Calculates the PDSCH frequency resource allocation and stores it in the provided PDSCH NR grant.
 *
 * @remark Defined by TS 38.214 V15.10.0 section 5.1.2.2
 * @param carrier Carrier information
 * @param cfg PDSCH NR configuration by upper layers
 * @param dci_dl Unpacked DCI used to schedule the PDSCH grant
 * @param[out] grant Provides grant pointer to fill
 * @return SRSRAN_SUCCESS if the provided data is valid, SRSRAN_ERROR code otherwise
 */
SRSRAN_API int srsran_ra_dl_nr_freq(const srsran_carrier_nr_t*    carrier,
                                    const srsran_sch_hl_cfg_nr_t* cfg,
                                    const srsran_dci_dl_nr_t*     dci_dl,
                                    srsran_sch_grant_nr_t*        grant);

#endif // SRSRAN_RA_DL_NR_H
