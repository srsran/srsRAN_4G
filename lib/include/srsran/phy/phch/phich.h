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
 *  File:         phich.h
 *
 *  Description:  Physical Hybrid ARQ indicator channel.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.9
 *****************************************************************************/

#ifndef SRSRAN_PHICH_H
#define SRSRAN_PHICH_H

#include "regs.h"
#include "srsran/config.h"
#include "srsran/phy/ch_estimation/chest_dl.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/mimo/layermap.h"
#include "srsran/phy/mimo/precoding.h"
#include "srsran/phy/modem/demod_soft.h"
#include "srsran/phy/modem/mod.h"
#include "srsran/phy/scrambling/scrambling.h"

#define SRSRAN_PHICH_NORM_NSEQUENCES 8
#define SRSRAN_PHICH_EXT_NSEQUENCES 4
#define SRSRAN_PHICH_NBITS 3

#define SRSRAN_PHICH_NORM_MSYMB SRSRAN_PHICH_NBITS * 4
#define SRSRAN_PHICH_EXT_MSYMB SRSRAN_PHICH_NBITS * 2
#define SRSRAN_PHICH_MAX_NSYMB SRSRAN_PHICH_NORM_MSYMB
#define SRSRAN_PHICH_NORM_C 1
#define SRSRAN_PHICH_EXT_C 2
#define SRSRAN_PHICH_NORM_NSF 4
#define SRSRAN_PHICH_EXT_NSF 2

/* phich object */
typedef struct SRSRAN_API {
  srsran_cell_t cell;

  uint32_t nof_rx_antennas;

  /* handler to REGs resource mapper */
  srsran_regs_t* regs;

  /* buffers */
  cf_t ce[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS][SRSRAN_PHICH_MAX_NSYMB];
  cf_t sf_symbols[SRSRAN_MAX_PORTS][SRSRAN_PHICH_MAX_NSYMB];
  cf_t x[SRSRAN_MAX_PORTS][SRSRAN_PHICH_MAX_NSYMB];
  cf_t d[SRSRAN_PHICH_MAX_NSYMB];
  cf_t d0[SRSRAN_PHICH_MAX_NSYMB];
  cf_t z[SRSRAN_PHICH_NBITS];

  /* bit message */
  uint8_t data[SRSRAN_PHICH_NBITS];
  float   data_rx[SRSRAN_PHICH_NBITS];

  /* tx & rx objects */
  srsran_modem_table_t mod;
  srsran_sequence_t    seq[SRSRAN_NOF_SF_X_FRAME];

} srsran_phich_t;

typedef struct SRSRAN_API {
  uint32_t ngroup;
  uint32_t nseq;
} srsran_phich_resource_t;

typedef struct SRSRAN_API {
  uint32_t n_prb_lowest;
  uint32_t n_dmrs;
  uint32_t I_phich;
} srsran_phich_grant_t;

typedef struct SRSRAN_API {
  bool  ack_value;
  float distance;
} srsran_phich_res_t;

SRSRAN_API int srsran_phich_init(srsran_phich_t* q, uint32_t nof_rx_antennas);

SRSRAN_API void srsran_phich_free(srsran_phich_t* q);

SRSRAN_API int srsran_phich_set_cell(srsran_phich_t* q, srsran_regs_t* regs, srsran_cell_t cell);

SRSRAN_API void srsran_phich_set_regs(srsran_phich_t* q, srsran_regs_t* regs);

SRSRAN_API void srsran_phich_calc(srsran_phich_t* q, srsran_phich_grant_t* grant, srsran_phich_resource_t* n_phich);

SRSRAN_API int srsran_phich_decode(srsran_phich_t*         q,
                                   srsran_dl_sf_cfg_t*     sf,
                                   srsran_chest_dl_res_t*  channel,
                                   srsran_phich_resource_t n_phich,
                                   cf_t*                   sf_symbols[SRSRAN_MAX_PORTS],
                                   srsran_phich_res_t*     result);

SRSRAN_API int srsran_phich_encode(srsran_phich_t*         q,
                                   srsran_dl_sf_cfg_t*     sf,
                                   srsran_phich_resource_t n_phich,
                                   uint8_t                 ack,
                                   cf_t*                   sf_symbols[SRSRAN_MAX_PORTS]);

SRSRAN_API void srsran_phich_reset(srsran_phich_t* q, cf_t* slot_symbols[SRSRAN_MAX_PORTS]);

SRSRAN_API uint32_t srsran_phich_ngroups(srsran_phich_t* q);

SRSRAN_API uint32_t srsran_phich_nsf(srsran_phich_t* q);

#endif // SRSRAN_PHICH_H
