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



#ifndef CHEST_DL_
#define CHEST_DL_

#include <stdio.h>

#include "liblte/config.h"

#include "liblte/phy/resampling/interp.h"
#include "liblte/phy/filter/filter2d.h"
#include "liblte/phy/ch_estimation/refsignal_dl.h"
#include "liblte/phy/common/phy_common.h"

#define CHEST_RS_AVERAGE_TIME   0
#define CHEST_RS_AVERAGE_FREQ   3



/** 3GPP LTE Downlink channel estimator and equalizer. 
 * Estimates the channel in the resource elements transmitting references and interpolates for the rest
 * of the resource grid. 
 * 
 * The equalizer uses the channel estimates to produce an estimation of the transmitted symbol. 
 * 
 * This object depends on the refsignal_t object for creating the LTE CSR signal.  
*/

typedef struct {
  lte_cell_t cell; 
  refsignal_cs_t csr_signal;
  cf_t *pilot_estimates[MAX_PORTS];
  cf_t *pilot_recv_signal[MAX_PORTS];
  cf_t *pilot_symbol_avg;
  
  interp_t interp_time[MAX_PORTS]; 
  interp_t interp_freq[MAX_PORTS]; 

  float rssi; 
  float rsrq; 
  float rsrp; 
} chest_dl_t;


LIBLTE_API int chest_dl_init(chest_dl_t *q, 
                             lte_cell_t cell);

LIBLTE_API void chest_dl_free(chest_dl_t *q); 

LIBLTE_API int chest_dl_estimate(chest_dl_t *q, 
                                 cf_t *input,
                                 cf_t *ce[MAX_PORTS],
                                 uint32_t sf_idx);

LIBLTE_API int chest_dl_estimate_port(chest_dl_t *q, 
                                      cf_t *input,
                                      cf_t *ce,
                                      uint32_t sf_idx, 
                                      uint32_t port_id);

LIBLTE_API int chest_dl_equalize_zf(chest_dl_t *q, 
                                    cf_t *input,
                                    cf_t *ce[MAX_PORTS],
                                    cf_t *output);

LIBLTE_API int chest_dl_equalize_mmse(chest_dl_t *q, 
                                      cf_t *input,
                                      cf_t *ce[MAX_PORTS],
                                      float *noise_estimate,
                                      cf_t *output);

LIBLTE_API float chest_dl_get_rssi(chest_dl_t *q);

LIBLTE_API float chest_dl_get_rsrq(chest_dl_t *q);

LIBLTE_API float chest_dl_get_rsrp(chest_dl_t *q);

#endif