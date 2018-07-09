/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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
 *  File:         pdsch_cfg.h
 *
 *  Description:  Physical downlink shared channel configuration
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.4
 *****************************************************************************/

#ifndef SRSLTE_PDSCH_CFG_H
#define SRSLTE_PDSCH_CFG_H

#include "srslte/phy/phch/ra.h"
#include "srslte/phy/fec/softbuffer.h"
#include "srslte/phy/fec/cbsegm.h"

/* 3GPP 36.213 Table 5.2-1: The cell-specific ratio rho_B / rho_A for 1, 2, or 4 cell specific antenna ports */
static const float pdsch_cfg_cell_specific_ratio_table[2][4] =
    { /* One antenna port         */ {1.0f / 1.0f, 4.0f / 5.0f, 3.0f / 5.0f, 2.0f / 5.0f},
      /* Two or more antenna port */ {5.0f / 4.0f, 1.0f / 1.0f, 3.0f / 4.0f, 1.0f / 2.0f}
    };


typedef struct SRSLTE_API {
  srslte_cbsegm_t cb_segm[SRSLTE_MAX_CODEWORDS];
  srslte_ra_dl_grant_t grant;
  srslte_ra_nbits_t nbits[SRSLTE_MAX_CODEWORDS];
  uint32_t rv[SRSLTE_MAX_CODEWORDS];
  uint32_t sf_idx;
  uint32_t nof_layers;
  uint32_t codebook_idx;
  srslte_mimo_type_t mimo_type;
  bool tb_cw_swap;
} srslte_pdsch_cfg_t;

#endif // SRSLTE_PDSCH_CFG_H

