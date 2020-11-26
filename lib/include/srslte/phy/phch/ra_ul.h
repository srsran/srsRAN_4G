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
 *  File:         ra_ul.h
 *
 *  Description:  Implements Resource allocation Procedures for UL defined in Sections 8
 *
 *  Reference:    3GPP TS 36.213 version 10.0.1 Release 10
 *****************************************************************************/

#ifndef SRSLTE_RA_UL_H
#define SRSLTE_RA_UL_H

#include <stdbool.h>
#include <stdint.h>

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/phch/dci.h"
#include "srslte/phy/phch/pusch_cfg.h"

// Structure for PUSCH frequency hopping procedure
typedef struct SRSLTE_API {
  bool              initialized;
  srslte_cell_t     cell;
  srslte_sequence_t seq_type2_fo;
} srslte_ra_ul_pusch_hopping_t;

SRSLTE_API int srslte_ra_ul_pusch_hopping_init(srslte_ra_ul_pusch_hopping_t* q, srslte_cell_t cell);

SRSLTE_API void srslte_ra_ul_pusch_hopping_free(srslte_ra_ul_pusch_hopping_t* q);

SRSLTE_API void srslte_ra_ul_pusch_hopping(srslte_ra_ul_pusch_hopping_t* q,
                                           srslte_ul_sf_cfg_t*           sf,
                                           srslte_pusch_hopping_cfg_t*   hopping_cfg,
                                           srslte_pusch_grant_t*         grant);

/** Functions to generate a grant from a received DCI */
SRSLTE_API int srslte_ra_ul_dci_to_grant(srslte_cell_t*              cell,
                                         srslte_ul_sf_cfg_t*         sf,
                                         srslte_pusch_hopping_cfg_t* hopping_cfg,
                                         srslte_dci_ul_t*            dci,
                                         srslte_pusch_grant_t*       grant);

SRSLTE_API void srslte_ra_ul_compute_nof_re(srslte_pusch_grant_t* grant, srslte_cp_t cp, uint32_t N_srs);

/** Others */
SRSLTE_API uint32_t srslte_ra_ul_info(const srslte_pusch_grant_t* grant, char* info_str, uint32_t len);

#endif // SRSLTE_RA_UL_H
