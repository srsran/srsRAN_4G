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



#ifndef REFSIGNAL_
#define REFSIGNAL_


/* Object to manage reference signals for OFDM channel equalization.
 *
 * It generates the reference signals for LTE.
 *
 */

#include "liblte/config.h"
#include "liblte/phy/common/base.h"

typedef _Complex float cf_t;

typedef struct LIBLTE_API{
  int time_idx;
  int freq_idx;
  cf_t simbol;
  cf_t recv_simbol;
}ref_t;

typedef struct LIBLTE_API{
  int nof_refs;    // number of reference signals
  int *symbols_ref;   // symbols with at least one reference
  int nsymbols;    // number of symbols with at least one reference
  int voffset;    // offset of the first reference in the freq domain
  int nof_prb;
  ref_t *refs;
  cf_t *ch_est;
} refsignal_t;

LIBLTE_API int refsignal_init_LTEDL(refsignal_t *q, int port_id, int nslot,
    int cell_id, lte_cp_t cp, int nof_prb);
LIBLTE_API void refsignal_free(refsignal_t *q);

LIBLTE_API void refsignal_put(refsignal_t *q, cf_t *slot_symbols);

#endif
