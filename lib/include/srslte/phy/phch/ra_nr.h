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
#include "srslte/phy/phch/dci_nr.h"
#include "srslte/phy/phch/phch_cfg_nr.h"

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
/**
 * @brief Converts an unpacked DL DCI message to a PDSCH grant structure.
 * Implements the procedures defined in Section 5 of 38.214 to compute the resource allocation (5.1.2)
 * and modulation order, target rate, redundancy version and TBS (5.1.3)
 *
 * Note: Only TypeA PDSCH mapping type is supported
 *
 * @param carrier Carrier information struct
 * @param pdsch_cfg PDSCH configuration indicated by higher layers
 * @param dci_dl DCI downlink (format 1_0 or 1_1)
 * @param pdsch_grant Generated PDSCH grant
 * @return 0 on success, -1 on error
 */
SRSLTE_API int srslte_ra_dl_dci_to_grant_nr(const srslte_carrier_nr_t* carrier,
                                            const srslte_sch_cfg_nr_t* pdsch_cfg,
                                            const srslte_dci_dl_nr_t*  dci_dl,
                                            srslte_sch_grant_nr_t*     pdsch_grant);

#endif // SRSLTE_RA_NR_H
