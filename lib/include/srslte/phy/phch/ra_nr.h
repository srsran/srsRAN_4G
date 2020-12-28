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
 *  File:         ra_nr.h
 *
 *  Description:  Implements Resource allocation Procedures common in for DL and UL
 *
 *  Reference:    3GPP TS 38.214 version 15.10.0
 *****************************************************************************/

#ifndef SRSLTE_RA_NR_H
#define SRSLTE_RA_NR_H

#include <stdbool.h>
#include <stdint.h>

#include "srslte/config.h"
#include "srslte/phy/common/phy_common_nr.h"
#include "srslte/phy/phch/phch_cfg_nr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Determines target rate
 * @param mcs_table Configured MCS table
 * @param dci_format DCI format used for the grant
 * @param search_space_type Search space type
 * @param rnti_type RNTI type
 * @param mcs_idx Desired Modulation Coding Scheme (MCS) index
 * @return The target rate if provided information is valid. Otherwise, it returns NAN
 */
SRSLTE_API double srslte_ra_nr_R_from_mcs(srslte_mcs_table_t         mcs_table,
                                          srslte_dci_format_nr_t     dci_format,
                                          srslte_search_space_type_t search_space_type,
                                          srslte_rnti_type_t         rnti_type,
                                          uint32_t                   mcs_idx);

/**
 * @brief Determines target rate
 * @param mcs_table Configured MCS table
 * @param dci_format DCI format used for the grant
 * @param search_space_type Search space type
 * @param rnti_type RNTI type
 * @param mcs_idx Desired Modulation Coding Scheme (MCS) index
 * @return The selected modulation if provided information is valid. Otherwise, it returns SRSLTE_MOD_NITEMS
 */
SRSLTE_API srslte_mod_t srslte_ra_nr_mod_from_mcs(srslte_mcs_table_t         mcs_table,
                                                  srslte_dci_format_nr_t     dci_format,
                                                  srslte_search_space_type_t search_space_type,
                                                  srslte_rnti_type_t         rnti_type,
                                                  uint32_t                   mcs_idx);

/**
 * @brief Determines the number of resource elements available for a given PDSCH transmission
 * @param pdsch_cfg PDSCH configuration provided by higher layers
 * @param grant The given PDSCH transmission grant
 * @return The number of resource elements if the provided configuration is valid, otherwise SRSLTE_ERROR code
 */
SRSLTE_API int srslte_ra_dl_nr_slot_nof_re(const srslte_sch_cfg_nr_t* pdsch_cfg, const srslte_sch_grant_nr_t* grant);

/**
 * @brief Calculates shared channel TBS
 * @param N_re Number of resource elements
 * @param S TB scaling, used in rare occasions. Set to 1, 0, NAN or INF if unused.
 * @param R Target Rate
 * @param Qm Modulation level
 * @param nof_layers Number of layers
 * @return The transport block size
 */
SRSLTE_API uint32_t srslte_ra_nr_tbs(uint32_t N_re, double S, double R, uint32_t Qm, uint32_t nof_layers);

SRSLTE_API int srslte_ra_nr_fill_tb(const srslte_sch_cfg_nr_t*   pdsch_cfg,
                                    const srslte_sch_grant_nr_t* grant,
                                    uint32_t                     mcs_idx,
                                    srslte_sch_tb_t*             tb);

#ifdef __cplusplus
}
#endif

#endif // SRSLTE_RA_NR_H
