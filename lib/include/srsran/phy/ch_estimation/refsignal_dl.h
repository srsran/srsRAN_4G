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

/**********************************************************************************************
 *  File:         refsignal_dl.h
 *
 *  Description:  Object to manage downlink reference signals for channel estimation.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.10
 *********************************************************************************************/

#ifndef SRSRAN_REFSIGNAL_DL_H
#define SRSRAN_REFSIGNAL_DL_H

#include "srsran/config.h"
#include "srsran/phy/common/phy_common.h"

// Number of references in a subframe: there are 2 symbols for port_id=0,1 x 2 slots x 2 refs per prb
#define SRSRAN_REFSIGNAL_NUM_SF_MBSFN(nof_prb, port_id) ((2 + 18) * (nof_prb))

#define SRSRAN_REFSIGNAL_MAX_NUM_SF(nof_prb) (8 * nof_prb)
#define SRSRAN_REFSIGNAL_MAX_NUM_SF_MBSFN(nof_prb) SRSRAN_REFSIGNAL_NUM_SF_MBSFN(nof_prb, 0)

#define SRSRAN_REFSIGNAL_PILOT_IDX(i, l, cell) (2 * cell.nof_prb * (l) + (i))

#define SRSRAN_REFSIGNAL_PILOT_IDX_MBSFN(i, l, cell) ((6 * cell.nof_prb * (l) + (i)))

/** Cell-Specific Reference Signal */
typedef struct SRSRAN_API {
  srsran_cell_t cell;
  cf_t*         pilots[2][SRSRAN_NOF_SF_X_FRAME]; // Saves the reference signal per subframe for ports 0,1 and ports 2,3
  srsran_sf_t   type;
  uint16_t      mbsfn_area_id;
} srsran_refsignal_t;

SRSRAN_API int srsran_refsignal_cs_init(srsran_refsignal_t* q, uint32_t max_prb);

SRSRAN_API int srsran_refsignal_cs_set_cell(srsran_refsignal_t* q, srsran_cell_t cell);

SRSRAN_API void srsran_refsignal_free(srsran_refsignal_t* q);

SRSRAN_API int
srsran_refsignal_cs_put_sf(srsran_refsignal_t* q, srsran_dl_sf_cfg_t* sf, uint32_t port_id, cf_t* sf_symbols);

SRSRAN_API int srsran_refsignal_cs_get_sf(srsran_refsignal_t* q,
                                          srsran_dl_sf_cfg_t* sf,
                                          uint32_t            port_id,
                                          cf_t*               sf_symbols,
                                          cf_t*               pilots);

SRSRAN_API uint32_t srsran_refsignal_cs_fidx(srsran_cell_t cell, uint32_t l, uint32_t port_id, uint32_t m);

SRSRAN_API uint32_t srsran_refsignal_cs_nsymbol(uint32_t l, srsran_cp_t cp, uint32_t port_id);

SRSRAN_API uint32_t srsran_refsignal_cs_v(uint32_t port_id, uint32_t ref_symbol_idx);

SRSRAN_API uint32_t srsran_refsignal_cs_nof_symbols(srsran_refsignal_t* q, srsran_dl_sf_cfg_t* sf, uint32_t port_id);

SRSRAN_API uint32_t srsran_refsignal_cs_nof_pilots_x_slot(uint32_t nof_ports);

SRSRAN_API uint32_t srsran_refsignal_cs_nof_re(srsran_refsignal_t* q, srsran_dl_sf_cfg_t* sf, uint32_t port_id);

SRSRAN_API int srsran_refsignal_mbsfn_init(srsran_refsignal_t* q, uint32_t max_prb);

SRSRAN_API int srsran_refsignal_mbsfn_set_cell(srsran_refsignal_t* q, srsran_cell_t cell, uint16_t mbsfn_area_id);

SRSRAN_API int srsran_refsignal_mbsfn_get_sf(srsran_cell_t cell, uint32_t port_id, cf_t* sf_symbols, cf_t* pilots);

SRSRAN_API uint32_t srsran_refsignal_mbsfn_nsymbol(uint32_t l);

SRSRAN_API uint32_t srsran_refsignal_mbsfn_fidx(uint32_t l);

SRSRAN_API uint32_t srsran_refsignal_mbsfn_nof_symbols();

SRSRAN_API int srsran_refsignal_mbsfn_put_sf(srsran_cell_t cell,
                                             uint32_t      port_id,
                                             cf_t*         cs_pilots,
                                             cf_t*         mbsfn_pilots,
                                             cf_t*         sf_symbols);

SRSRAN_API int srsran_refsignal_mbsfn_gen_seq(srsran_refsignal_t* q, srsran_cell_t cell, uint32_t N_mbsfn_id);

#endif // SRSRAN_REFSIGNAL_DL_H
