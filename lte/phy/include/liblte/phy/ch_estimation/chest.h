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



#ifndef CHEST_
#define CHEST_

#include <stdio.h>

#include "liblte/config.h"

#include "liblte/phy/resampling/interp.h"
#include "liblte/phy/ch_estimation/refsignal.h"
#include "liblte/phy/common/phy_common.h"

typedef _Complex float cf_t; /* this is only a shortcut */

typedef void (*interpolate_fnc_t) (cf_t *input, 
                                   cf_t *output, 
                                   uint32_t M, 
                                   uint32_t len, 
                                   uint32_t off_st, 
                                   uint32_t off_end);

/** This is an OFDM channel estimator.
 * It works with any reference signal pattern, provided by the object
 * refsignal_t
 * A 2-D filter is used for freq and time channel interpolation.
 *
 */

/* Low-level API */
typedef struct LIBLTE_API {
  uint32_t nof_ports;
  uint32_t nof_re;
  uint32_t nof_symbols;
  
  refsignal_t refsignal[MAX_PORTS][NSLOTS_X_FRAME];
  interp_t interp_time[MAX_PORTS]; 
  interp_t interp_freq[MAX_PORTS]; 
  
}chest_t;

LIBLTE_API int chest_init(chest_t *q, 
                          uint32_t nof_re, 
                          uint32_t nof_symbols, 
                          uint32_t nof_ports);

LIBLTE_API void chest_free(chest_t *q);

LIBLTE_API int chest_set_nof_ports(chest_t *q, 
                                    uint32_t nof_ports);

LIBLTE_API int chest_ce_ref(chest_t *q, 
                             cf_t *input, 
                             uint32_t nslot, 
                             uint32_t port_id, 
                             uint32_t nref);

LIBLTE_API int chest_ce_slot_port(chest_t *q, 
                                   cf_t *input, 
                                   cf_t *ce, 
                                   uint32_t nslot, 
                                   uint32_t port_id);

LIBLTE_API int chest_ce_sf_port(chest_t *q, 
                                 cf_t *input, 
                                 cf_t *ce, 
                                 uint32_t sf_idx, 
                                 uint32_t port_id);

LIBLTE_API int chest_ce_slot(chest_t *q, 
                              cf_t *input, 
                              cf_t *ce[MAX_PORTS], 
                              uint32_t nslot);

LIBLTE_API int chest_ce_sf(chest_t *q, 
                            cf_t *input, 
                            cf_t *ce[MAX_PORTS], 
                            uint32_t sf_idx);

LIBLTE_API void chest_fprint(chest_t *q, 
                             FILE *stream, 
                             uint32_t nslot, 
                             uint32_t port_id);

LIBLTE_API void chest_ref_fprint(chest_t *q, 
                                 FILE *stream, 
                                 uint32_t nslot, 
                                 uint32_t port_id);

LIBLTE_API void chest_recvsig_fprint(chest_t *q, 
                                     FILE *stream, 
                                     uint32_t nslot, 
                                     uint32_t port_id);

LIBLTE_API void chest_ce_fprint(chest_t *q, 
                                FILE *stream, 
                                uint32_t nslot, 
                                uint32_t port_id);

LIBLTE_API int chest_ref_get_symbols(chest_t *q, 
                                     uint32_t port_id, 
                                     uint32_t nslot, 
                                     uint32_t l[2]);




/*********************************************************
 * 
 * Downlink Channel Estimator 
 * 
 *********************************************************/
LIBLTE_API int chest_init_LTEDL(chest_t *q, 
                                lte_cell_t cell);

LIBLTE_API int chest_ref_set_LTEDL_slot_port(chest_t *q, 
                                             uint32_t nslot, 
                                             uint32_t port_id, 
                                             lte_cell_t cell);

LIBLTE_API int chest_ref_set_LTEDL_slot(chest_t *q, 
                                        uint32_t nslot, 
                                        lte_cell_t cell);

LIBLTE_API int chest_ref_set_LTEDL(chest_t *q, 
                                   lte_cell_t cell);


/*********************************************************
 * 
 * Uplink Channel Estimator 
 * 
 *********************************************************/
LIBLTE_API int chest_init_LTEUL(chest_t *q, 
                                lte_cell_t cell);

LIBLTE_API int chest_ref_set_LTEUL_slot(chest_t *q, 
                                        uint32_t nslot, 
                                        lte_cell_t cell);

LIBLTE_API int chest_ref_set_LTEUL(chest_t *q, 
                                   lte_cell_t cell);


/* High-level API */

/** TODO: The high-level API has N interfaces, one for each port */

typedef struct LIBLTE_API{
  chest_t obj;
  struct chest_init {
    int nof_symbols;     // 7 for normal cp, 6 for extended
    int nof_ports;
    int nof_prb;
    int cell_id;  // set to -1 to init at runtime
  } init;
  cf_t *input;
  int in_len;
  struct chest_ctrl_in {
    int sf_idx;  // subframe id in the 10ms frame
  } ctrl_in;
  cf_t *output[MAX_PORTS];
  int out_len[MAX_PORTS];
}chest_hl;

#define DEFAULT_FRAME_SIZE    2048

LIBLTE_API int chest_initialize(chest_hl* h);
LIBLTE_API int chest_work(chest_hl* hl);
LIBLTE_API int chest_stop(chest_hl* hl);

#endif








