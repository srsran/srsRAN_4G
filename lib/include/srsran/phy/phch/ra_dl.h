/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSRAN_RA_DL_H
#define SRSRAN_RA_DL_H

#include <stdbool.h>
#include <stdint.h>

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/phch/dci.h"
#include "srsran/phy/phch/pdsch_cfg.h"
#include "srsran/phy/phch/ra.h"

/**************************************************
 * Structures used for Downlink Resource Allocation
 **************************************************/

/** Functions to generate a grant from a received DCI */
SRSRAN_API int srsran_ra_dl_dci_to_grant(const srsran_cell_t*   cell,
                                         srsran_dl_sf_cfg_t*    sf,
                                         srsran_tm_t            tm,
                                         bool                   pdsch_use_tbs_index_alt,
                                         const srsran_dci_dl_t* dci,
                                         srsran_pdsch_grant_t*  grant);

SRSRAN_API int
srsran_ra_dl_grant_to_grant_prb_allocation(const srsran_dci_dl_t* dci, srsran_pdsch_grant_t* grant, uint32_t nof_prb);

/** Functions used by the eNodeB scheduler */
SRSRAN_API uint32_t srsran_ra_dl_approx_nof_re(const srsran_cell_t* cell, uint32_t nof_prb, uint32_t nof_ctrl_symbols);

SRSRAN_API uint32_t ra_re_x_prb(const srsran_cell_t* cell, srsran_dl_sf_cfg_t* sf, uint32_t slot, uint32_t prb_idx);

SRSRAN_API uint32_t srsran_ra_dl_grant_nof_re(const srsran_cell_t*  cell,
                                              srsran_dl_sf_cfg_t*   sf,
                                              srsran_pdsch_grant_t* grant);

/** Others */
SRSRAN_API int srsran_dl_fill_ra_mcs(srsran_ra_tb_t* tb, int last_tbs, uint32_t nprb, bool pdsch_use_tbs_index_alt);

SRSRAN_API void
srsran_ra_dl_compute_nof_re(const srsran_cell_t* cell, srsran_dl_sf_cfg_t* sf, srsran_pdsch_grant_t* grant);

SRSRAN_API uint32_t srsran_ra_dl_info(srsran_pdsch_grant_t* grant, char* info_str, uint32_t len);

#endif // SRSRAN_RA_DL_H
