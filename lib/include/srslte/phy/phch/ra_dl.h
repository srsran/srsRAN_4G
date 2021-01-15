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
 *  File:         ra_dl.h
 *
 *  Description:  Implements Resource allocation Procedures for DL defined in Section 7
 *
 *  Reference:    3GPP TS 36.213 version 10.0.1 Release 10
 *****************************************************************************/

#ifndef SRSLTE_RA_DL_H
#define SRSLTE_RA_DL_H

#include <stdbool.h>
#include <stdint.h>

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/phch/dci.h"
#include "srslte/phy/phch/pdsch_cfg.h"
#include "srslte/phy/phch/ra.h"

/**************************************************
 * Structures used for Downlink Resource Allocation
 **************************************************/

/** Functions to generate a grant from a received DCI */
SRSLTE_API int srslte_ra_dl_dci_to_grant(const srslte_cell_t*   cell,
                                         srslte_dl_sf_cfg_t*    sf,
                                         srslte_tm_t            tm,
                                         bool                   pdsch_use_tbs_index_alt,
                                         const srslte_dci_dl_t* dci,
                                         srslte_pdsch_grant_t*  grant);

SRSLTE_API int
srslte_ra_dl_grant_to_grant_prb_allocation(const srslte_dci_dl_t* dci, srslte_pdsch_grant_t* grant, uint32_t nof_prb);

/** Functions used by the eNodeB scheduler */
SRSLTE_API uint32_t srslte_ra_dl_approx_nof_re(const srslte_cell_t* cell, uint32_t nof_prb, uint32_t nof_ctrl_symbols);

SRSLTE_API uint32_t ra_re_x_prb(const srslte_cell_t* cell, srslte_dl_sf_cfg_t* sf, uint32_t slot, uint32_t prb_idx);

SRSLTE_API uint32_t srslte_ra_dl_grant_nof_re(const srslte_cell_t*  cell,
                                              srslte_dl_sf_cfg_t*   sf,
                                              srslte_pdsch_grant_t* grant);

/** Others */
SRSLTE_API int srslte_dl_fill_ra_mcs(srslte_ra_tb_t* tb, int last_tbs, uint32_t nprb, bool pdsch_use_tbs_index_alt);

SRSLTE_API void
srslte_ra_dl_compute_nof_re(const srslte_cell_t* cell, srslte_dl_sf_cfg_t* sf, srslte_pdsch_grant_t* grant);

SRSLTE_API uint32_t srslte_ra_dl_info(srslte_pdsch_grant_t* grant, char* info_str, uint32_t len);

#endif // SRSLTE_RA_DL_H
