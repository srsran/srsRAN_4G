/*
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
#include "srslte/phy/phch/pdsch_cfg_nr.h"

/**
 * @brief Determines the number of resource elements available for a given PDSCH transmission
 * @param pdsch_cfg PDSCH configuration provided by higher layers
 * @param grant The given PDSCH transmission grant
 * @return The number of resource elements if the provided configuration is valid, otherwise SRSLTE_ERROR code
 */
SRSLTE_API int srslte_ra_dl_nr_slot_nof_re(const srslte_pdsch_cfg_nr_t*   pdsch_cfg,
                                           const srslte_pdsch_grant_nr_t* grant);

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

SRSLTE_API int srslte_ra_nr_fill_tb(const srslte_pdsch_cfg_nr_t*   pdsch_cfg,
                                    const srslte_pdsch_grant_nr_t* grant,
                                    uint32_t                       mcs_idx,
                                    srslte_sch_tb_t*               tb);
#endif // SRSLTE_RA_NR_H
