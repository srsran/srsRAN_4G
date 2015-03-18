/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef REFSIGNAL_DL_
#define REFSIGNAL_DL_

/* Object to manage Downlink reference signals for channel estimation.
 *
 */

#include "srslte/config.h"
#include "srslte/common/phy_common.h"

typedef _Complex float cf_t;

// Number of references in a subframe: there are 2 symbols for port_id=0,1 x 2 slots x 2 refs per prb
#define REFSIGNAL_NUM_SF(nof_prb, port_id)     (((port_id)<2?8:4)*(nof_prb))
#define REFSIGNAL_MAX_NUM_SF(nof_prb)     REFSIGNAL_NUM_SF(nof_prb, 0)

#define REFSIGNAL_PILOT_IDX(i,l,cell) (2*cell.nof_prb*(l)+(i))


/** Cell-Specific Reference Signal */
typedef struct SRSLTE_API {
  lte_cell_t cell; 
  cf_t *pilots[2][NSUBFRAMES_X_FRAME]; // Saves the reference signal per subframe for ports 0,1 and ports 2,3
} refsignal_cs_t;


SRSLTE_API int refsignal_cs_init(refsignal_cs_t *q, 
                                     lte_cell_t cell);

SRSLTE_API void refsignal_cs_free(refsignal_cs_t *q);

SRSLTE_API int refsignal_cs_put_sf(lte_cell_t cell, 
                                   uint32_t port_id, 
                                   cf_t *pilots,
                                   cf_t *sf_symbols);

SRSLTE_API int refsignal_cs_get_sf(lte_cell_t cell, 
                                   uint32_t port_id, 
                                   cf_t *sf_symbols, 
                                   cf_t *pilots);

SRSLTE_API uint32_t refsignal_cs_fidx(lte_cell_t cell, 
                                   uint32_t l, 
                                   uint32_t port_id, 
                                   uint32_t m);

SRSLTE_API uint32_t refsignal_cs_nsymbol(uint32_t l, 
                                      lte_cp_t cp, 
                                      uint32_t port_id);

SRSLTE_API uint32_t refsignal_cs_v(uint32_t port_id, 
                                   uint32_t ref_symbol_idx); 

SRSLTE_API uint32_t refsignal_cs_nof_symbols(uint32_t port_id);

#endif
