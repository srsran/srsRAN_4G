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

/**********************************************************************************************
 *  File:         refsignal_dl.h
 *
 *  Description:  Object to manage downlink reference signals for channel estimation.
 *
 *  Reference:    3GPP TS 36.211 version 10.0.0 Release 10 Sec. 6.10
 *********************************************************************************************/

#ifndef SRSLTE_REFSIGNAL_DL_H
#define SRSLTE_REFSIGNAL_DL_H

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"

// Number of references in a subframe: there are 2 symbols for port_id=0,1 x 2 slots x 2 refs per prb
#define SRSLTE_REFSIGNAL_NUM_SF_MBSFN(nof_prb, port_id) ((2 + 18) * (nof_prb))

#define SRSLTE_REFSIGNAL_MAX_NUM_SF(nof_prb) (8 * nof_prb)
#define SRSLTE_REFSIGNAL_MAX_NUM_SF_MBSFN(nof_prb) SRSLTE_REFSIGNAL_NUM_SF_MBSFN(nof_prb, 0)

#define SRSLTE_REFSIGNAL_PILOT_IDX(i, l, cell) (2 * cell.nof_prb * (l) + (i))

#define SRSLTE_REFSIGNAL_PILOT_IDX_MBSFN(i, l, cell) ((6 * cell.nof_prb * (l) + (i)))

/** Cell-Specific Reference Signal */
typedef struct SRSLTE_API {
  srslte_cell_t cell;
  cf_t*         pilots[2][SRSLTE_NOF_SF_X_FRAME]; // Saves the reference signal per subframe for ports 0,1 and ports 2,3
  srslte_sf_t   type;
  uint16_t      mbsfn_area_id;
} srslte_refsignal_t;

SRSLTE_API int srslte_refsignal_cs_init(srslte_refsignal_t* q, uint32_t max_prb);

SRSLTE_API int srslte_refsignal_cs_set_cell(srslte_refsignal_t* q, srslte_cell_t cell);

SRSLTE_API void srslte_refsignal_free(srslte_refsignal_t* q);

SRSLTE_API int
srslte_refsignal_cs_put_sf(srslte_refsignal_t* q, srslte_dl_sf_cfg_t* sf, uint32_t port_id, cf_t* sf_symbols);

SRSLTE_API int srslte_refsignal_cs_get_sf(srslte_refsignal_t* q,
                                          srslte_dl_sf_cfg_t* sf,
                                          uint32_t            port_id,
                                          cf_t*               sf_symbols,
                                          cf_t*               pilots);

SRSLTE_API uint32_t srslte_refsignal_cs_fidx(srslte_cell_t cell, uint32_t l, uint32_t port_id, uint32_t m);

SRSLTE_API uint32_t srslte_refsignal_cs_nsymbol(uint32_t l, srslte_cp_t cp, uint32_t port_id);

SRSLTE_API uint32_t srslte_refsignal_cs_v(uint32_t port_id, uint32_t ref_symbol_idx);

SRSLTE_API uint32_t srslte_refsignal_cs_nof_symbols(srslte_refsignal_t* q, srslte_dl_sf_cfg_t* sf, uint32_t port_id);

SRSLTE_API uint32_t srslte_refsignal_cs_nof_pilots_x_slot(uint32_t nof_ports);

SRSLTE_API uint32_t srslte_refsignal_cs_nof_re(srslte_refsignal_t* q, srslte_dl_sf_cfg_t* sf, uint32_t port_id);

SRSLTE_API int srslte_refsignal_mbsfn_init(srslte_refsignal_t* q, uint32_t max_prb);

SRSLTE_API int srslte_refsignal_mbsfn_set_cell(srslte_refsignal_t* q, srslte_cell_t cell, uint16_t mbsfn_area_id);

SRSLTE_API int srslte_refsignal_mbsfn_get_sf(srslte_cell_t cell, uint32_t port_id, cf_t* sf_symbols, cf_t* pilots);

SRSLTE_API uint32_t srslte_refsignal_mbsfn_nsymbol(uint32_t l);

SRSLTE_API uint32_t srslte_refsignal_mbsfn_fidx(uint32_t l);

SRSLTE_API uint32_t srslte_refsignal_mbsfn_nof_symbols();

SRSLTE_API int srslte_refsignal_mbsfn_put_sf(srslte_cell_t cell,
                                             uint32_t      port_id,
                                             cf_t*         cs_pilots,
                                             cf_t*         mbsfn_pilots,
                                             cf_t*         sf_symbols);

SRSLTE_API int srslte_refsignal_mbsfn_gen_seq(srslte_refsignal_t* q, srslte_cell_t cell, uint32_t N_mbsfn_id);

#endif // SRSLTE_REFSIGNAL_DL_H
