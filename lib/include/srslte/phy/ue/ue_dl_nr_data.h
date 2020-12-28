/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
#include "srslte/phy/phch/phch_cfg_nr.h"

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
                                                      srslte_sch_grant_nr_t*           grant);

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
SRSLTE_API int srslte_ue_dl_nr_pdsch_time_resource_default_A(uint32_t                    m,
                                                             srslte_dmrs_sch_typeA_pos_t dmrs_typeA_pos,
                                                             srslte_sch_grant_nr_t*      grant);
/**
 * @brief Calculates the number of PDSCH-DMRS CDM groups without data for DCI format 1_0
 *
 * @remark Defined by TS 38.214 V15.10.0 5.1.6.1.3 CSI-RS for mobility
 *
 * @param pdsch_cfg PDSCH NR configuration by upper layers
 * @param[out] grant Provides grant pointer to fill
 * @return Returns SRSLTE_SUCCESS if the provided data is valid, otherwise it returns SRSLTE_ERROR code
 */
SRSLTE_API int srslte_ue_dl_nr_nof_dmrs_cdm_groups_without_data_format_1_0(const srslte_sch_cfg_nr_t* pdsch_cfg,
                                                                           srslte_sch_grant_nr_t*     grant);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_UE_DL_NR_DATA_H
