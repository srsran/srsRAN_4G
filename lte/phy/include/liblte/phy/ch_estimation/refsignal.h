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
#include "liblte/phy/common/phy_common.h"

typedef _Complex float cf_t;

typedef struct LIBLTE_API{
  uint32_t time_idx;
  uint32_t freq_idx;
  cf_t symbol;
}ref_t;

typedef struct LIBLTE_API{
  uint32_t nof_refs;    // number of reference signals
  uint32_t *symbols_ref;   // symbols with at least one reference
  uint32_t nsymbols;    // number of symbols with at least one reference
  uint32_t voffset;    // offset of the first reference in the freq domain
  uint32_t nof_prb;
  ref_t *refs;
  cf_t *ch_est;
  cf_t *recv_symbol;
} refsignal_t;


typedef struct LIBLTE_API {
  float beta;                     // amplitude scaling
  uint32_t delta_ss;              // Set to 0 for PUCCH
  uint32_t cyclic_shift;
  uint32_t cyclic_shift_for_drms; /* From DCI 0. Set to 0 if no PDCCH with DCI 0 for the same TB
                                    or if the initial PUSCH is semi-persisently scheduled or 
                                    if the initial PUSCH is scheduled by the RA response grant */                                  
  bool group_hopping_en;
  bool sequence_hopping_en; 
} refsignal_ul_cfg_t;


LIBLTE_API int refsignal_init_LTEDL(refsignal_t *q, 
                                    uint32_t port_id, 
                                    uint32_t nslot,
                                    lte_cell_t cell);

LIBLTE_API int refsignal_init_LTEUL_drms_pusch(refsignal_t *q, 
                                               uint32_t nof_prb, 
                                               uint32_t prb_start, 
                                               uint32_t nslot,
                                               lte_cell_t cell, 
                                               refsignal_ul_cfg_t *drms_cfg);

LIBLTE_API void refsignal_free(refsignal_t *q);

LIBLTE_API int refsignal_put(refsignal_t *q, 
                              cf_t *slot_symbols);

#endif
