/**
 * Copyright 2013-2021 Software Radio Systems Limited
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
 *  File:         pmch.h
 *
 *  Description:  Physical multicast channel
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.5
 *****************************************************************************/

#ifndef SRSRAN_PMCH_H
#define SRSRAN_PMCH_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/common/sequence.h"
#include "srsran/phy/mimo/layermap.h"
#include "srsran/phy/mimo/precoding.h"
#include "srsran/phy/modem/demod_soft.h"
#include "srsran/phy/modem/mod.h"
#include "srsran/phy/phch/dci.h"
#include "srsran/phy/phch/pdsch.h"
#include "srsran/phy/phch/ra_dl.h"
#include "srsran/phy/phch/regs.h"
#include "srsran/phy/phch/sch.h"
#include "srsran/phy/scrambling/scrambling.h"
typedef struct {
  srsran_sequence_t seq[SRSRAN_NOF_SF_X_FRAME];
} srsran_pmch_seq_t;

typedef struct SRSRAN_API {
  srsran_pdsch_cfg_t pdsch_cfg;
  uint16_t           area_id;
} srsran_pmch_cfg_t;

/* PMCH object */
typedef struct SRSRAN_API {
  srsran_cell_t cell;

  uint32_t nof_rx_antennas;

  uint32_t max_re;

  /* buffers */
  // void buffers are shared for tx and rx
  cf_t* ce[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS];
  cf_t* symbols[SRSRAN_MAX_PORTS];
  cf_t* x[SRSRAN_MAX_PORTS];
  cf_t* d;
  void* e;

  /* tx & rx objects */
  srsran_modem_table_t mod[4];

  // This is to generate the scrambling seq for multiple MBSFN Area IDs
  srsran_pmch_seq_t** seqs;

  srsran_sch_t dl_sch;

} srsran_pmch_t;

SRSRAN_API int srsran_pmch_init(srsran_pmch_t* q, uint32_t max_prb, uint32_t nof_rx_antennas);

SRSRAN_API void srsran_pmch_free(srsran_pmch_t* q);

SRSRAN_API int srsran_pmch_set_cell(srsran_pmch_t* q, srsran_cell_t cell);

SRSRAN_API int srsran_pmch_set_area_id(srsran_pmch_t* q, uint16_t area_id);

SRSRAN_API void srsran_pmch_free_area_id(srsran_pmch_t* q, uint16_t area_id);

SRSRAN_API void srsran_configure_pmch(srsran_pmch_cfg_t* pmch_cfg, srsran_cell_t* cell, srsran_mbsfn_cfg_t* mbsfn_cfg);

SRSRAN_API int srsran_pmch_encode(srsran_pmch_t*      q,
                                  srsran_dl_sf_cfg_t* sf,
                                  srsran_pmch_cfg_t*  cfg,
                                  uint8_t*            data,
                                  cf_t*               sf_symbols[SRSRAN_MAX_PORTS]);

SRSRAN_API int srsran_pmch_decode(srsran_pmch_t*         q,
                                  srsran_dl_sf_cfg_t*    sf,
                                  srsran_pmch_cfg_t*     cfg,
                                  srsran_chest_dl_res_t* channel,
                                  cf_t*                  sf_symbols[SRSRAN_MAX_PORTS],
                                  srsran_pdsch_res_t*    data);

#endif // SRSRAN_PMCH_H
