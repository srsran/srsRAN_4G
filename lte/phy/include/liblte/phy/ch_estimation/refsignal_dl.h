/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
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

#include "liblte/config.h"
#include "liblte/phy/common/phy_common.h"

typedef _Complex float cf_t;

// Number of references in a subframe: there are 2 symbols for port_id=0,1 x 2 slots x 2 refs per prb
#define REFSIGNAL_NUM_SF(nof_prb, port_id)     (((port_id)<2?8:4)*(nof_prb))
#define REFSIGNAL_MAX_NUM_SF(nof_prb)     REFSIGNAL_NUM_SF(nof_prb, 0)

#define REFSIGNAL_PILOT_IDX(i,l,ns,cell) (2*cell.nof_prb*((l)+2*((ns)%2))+(i))


/** Cell-Specific Reference Signal */
typedef struct LIBLTE_API {
  lte_cell_t cell; 
  cf_t *pilots[NSUBFRAMES_X_FRAME]; // save 2 reference symbols per slot
} refsignal_cs_t;


LIBLTE_API int refsignal_cs_generate(refsignal_cs_t *q, 
                                     lte_cell_t cell);

LIBLTE_API void refsignal_cs_free(refsignal_cs_t *q);

LIBLTE_API int refsignal_cs_put_sf(lte_cell_t cell, 
                                   uint32_t port_id, 
                                   uint32_t sf_idx,
                                   cf_t *pilots,
                                   cf_t *sf_symbols);

LIBLTE_API int refsignal_cs_get_sf(lte_cell_t cell, 
                                   uint32_t port_id, 
                                   uint32_t sf_idx, 
                                   cf_t *sf_symbols, 
                                   cf_t *pilots);

LIBLTE_API uint32_t refsignal_fidx(lte_cell_t cell, 
                                   uint32_t ns, 
                                   uint32_t l, 
                                   uint32_t port_id, 
                                   uint32_t m);

LIBLTE_API uint32_t refsignal_nsymbol(lte_cell_t cell, 
                                      uint32_t ns, 
                                      uint32_t l);

LIBLTE_API uint32_t refsignal_cs_v(uint32_t port_id, 
                                   uint32_t ns, 
                                   uint32_t symbol_id); 

LIBLTE_API uint32_t refsignal_cs_nof_symbols(uint32_t port_id);

#endif
