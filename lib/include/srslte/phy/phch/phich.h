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
 *  File:         phich.h
 *
 *  Description:  Physical Hybrid ARQ indicator channel.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.9
 *****************************************************************************/

#ifndef SRSLTE_PHICH_H
#define SRSLTE_PHICH_H

#include "regs.h"
#include "srslte/config.h"
#include "srslte/phy/ch_estimation/chest_dl.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/mimo/layermap.h"
#include "srslte/phy/mimo/precoding.h"
#include "srslte/phy/modem/demod_soft.h"
#include "srslte/phy/modem/mod.h"
#include "srslte/phy/scrambling/scrambling.h"

#define SRSLTE_PHICH_NORM_NSEQUENCES 8
#define SRSLTE_PHICH_EXT_NSEQUENCES 4
#define SRSLTE_PHICH_NBITS 3

#define SRSLTE_PHICH_NORM_MSYMB SRSLTE_PHICH_NBITS * 4
#define SRSLTE_PHICH_EXT_MSYMB SRSLTE_PHICH_NBITS * 2
#define SRSLTE_PHICH_MAX_NSYMB SRSLTE_PHICH_NORM_MSYMB
#define SRSLTE_PHICH_NORM_C 1
#define SRSLTE_PHICH_EXT_C 2
#define SRSLTE_PHICH_NORM_NSF 4
#define SRSLTE_PHICH_EXT_NSF 2

/* phich object */
typedef struct SRSLTE_API {
  srslte_cell_t cell;

  uint32_t nof_rx_antennas;

  /* handler to REGs resource mapper */
  srslte_regs_t* regs;

  /* buffers */
  cf_t ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS][SRSLTE_PHICH_MAX_NSYMB];
  cf_t sf_symbols[SRSLTE_MAX_PORTS][SRSLTE_PHICH_MAX_NSYMB];
  cf_t x[SRSLTE_MAX_PORTS][SRSLTE_PHICH_MAX_NSYMB];
  cf_t d[SRSLTE_PHICH_MAX_NSYMB];
  cf_t d0[SRSLTE_PHICH_MAX_NSYMB];
  cf_t z[SRSLTE_PHICH_NBITS];

  /* bit message */
  uint8_t data[SRSLTE_PHICH_NBITS];
  float   data_rx[SRSLTE_PHICH_NBITS];

  /* tx & rx objects */
  srslte_modem_table_t mod;
  srslte_sequence_t    seq[SRSLTE_NOF_SF_X_FRAME];

} srslte_phich_t;

typedef struct SRSLTE_API {
  uint32_t ngroup;
  uint32_t nseq;
} srslte_phich_resource_t;

typedef struct SRSLTE_API {
  uint32_t n_prb_lowest;
  uint32_t n_dmrs;
  uint32_t I_phich;
} srslte_phich_grant_t;

typedef struct SRSLTE_API {
  bool  ack_value;
  float distance;
} srslte_phich_res_t;

SRSLTE_API int srslte_phich_init(srslte_phich_t* q, uint32_t nof_rx_antennas);

SRSLTE_API void srslte_phich_free(srslte_phich_t* q);

SRSLTE_API int srslte_phich_set_cell(srslte_phich_t* q, srslte_regs_t* regs, srslte_cell_t cell);

SRSLTE_API void srslte_phich_set_regs(srslte_phich_t* q, srslte_regs_t* regs);

SRSLTE_API void srslte_phich_calc(srslte_phich_t* q, srslte_phich_grant_t* grant, srslte_phich_resource_t* n_phich);

SRSLTE_API int srslte_phich_decode(srslte_phich_t*         q,
                                   srslte_dl_sf_cfg_t*     sf,
                                   srslte_chest_dl_res_t*  channel,
                                   srslte_phich_resource_t n_phich,
                                   cf_t*                   sf_symbols[SRSLTE_MAX_PORTS],
                                   srslte_phich_res_t*     result);

SRSLTE_API int srslte_phich_encode(srslte_phich_t*         q,
                                   srslte_dl_sf_cfg_t*     sf,
                                   srslte_phich_resource_t n_phich,
                                   uint8_t                 ack,
                                   cf_t*                   sf_symbols[SRSLTE_MAX_PORTS]);

SRSLTE_API void srslte_phich_reset(srslte_phich_t* q, cf_t* slot_symbols[SRSLTE_MAX_PORTS]);

SRSLTE_API uint32_t srslte_phich_ngroups(srslte_phich_t* q);

SRSLTE_API uint32_t srslte_phich_nsf(srslte_phich_t* q);

#endif // SRSLTE_PHICH_H
