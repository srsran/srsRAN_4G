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

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_UE_UL_DATA_H
