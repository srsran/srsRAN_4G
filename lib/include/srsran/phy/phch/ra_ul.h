/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
 *  File:         ra_ul.h
 *
 *  Description:  Implements Resource allocation Procedures for UL defined in Sections 8
 *
 *  Reference:    3GPP TS 36.213 version 10.0.1 Release 10
 *****************************************************************************/

#ifndef SRSRAN_RA_UL_H
#define SRSRAN_RA_UL_H

#include <stdbool.h>
#include <stdint.h>

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/phch/dci.h"
#include "srsran/phy/phch/pusch_cfg.h"

// Structure for PUSCH frequency hopping procedure
typedef struct SRSRAN_API {
  bool              initialized;
  srsran_cell_t     cell;
  srsran_sequence_t seq_type2_fo;
} srsran_ra_ul_pusch_hopping_t;

SRSRAN_API int srsran_ra_ul_pusch_hopping_init(srsran_ra_ul_pusch_hopping_t* q, srsran_cell_t cell);

SRSRAN_API void srsran_ra_ul_pusch_hopping_free(srsran_ra_ul_pusch_hopping_t* q);

SRSRAN_API void srsran_ra_ul_pusch_hopping(srsran_ra_ul_pusch_hopping_t* q,
                                           srsran_ul_sf_cfg_t*           sf,
                                           srsran_pusch_hopping_cfg_t*   hopping_cfg,
                                           srsran_pusch_grant_t*         grant);

/** Functions to generate a grant from a received DCI */
SRSRAN_API int srsran_ra_ul_dci_to_grant(srsran_cell_t*              cell,
                                         srsran_ul_sf_cfg_t*         sf,
                                         srsran_pusch_hopping_cfg_t* hopping_cfg,
                                         srsran_dci_ul_t*            dci,
                                         srsran_pusch_grant_t*       grant);

SRSRAN_API void srsran_ra_ul_compute_nof_re(srsran_pusch_grant_t* grant, srsran_cp_t cp, uint32_t N_srs);

/** Others */
SRSRAN_API uint32_t srsran_ra_ul_info(const srsran_pusch_grant_t* grant, char* info_str, uint32_t len);

#endif // SRSRAN_RA_UL_H
