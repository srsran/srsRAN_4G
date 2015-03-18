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

#include "srslte/config.h"

#include "srslte/phy/resampling/interp.h"
#include "srslte/phy/filter/filter2d.h"
#include "srslte/phy/ch_estimation/refsignal_dl.h"
#include "srslte/phy/common/phy_common.h"

/** 3GPP LTE Downlink channel estimator and equalizer. 
 * Estimates the channel in the resource elements transmitting references and interpolates for the rest
 * of the resource grid. 
 * 
 * The equalizer uses the channel estimates to produce an estimation of the transmitted symbol. 
 * 
 * This object depends on the refsignal_t object for creating the LTE CSR signal.  
*/

#define CHEST_MAX_FILTER_FREQ_LEN    10
#define CHEST_MAX_FILTER_TIME_LEN    4

typedef struct {
  lte_cell_t cell; 
  refsignal_cs_t csr_signal;
  cf_t *pilot_estimates[MAX_PORTS];
  cf_t *pilot_estimates_average[MAX_PORTS];
  cf_t *pilot_recv_signal[MAX_PORTS];
 
  uint32_t filter_freq_len;
  float filter_freq[CHEST_MAX_FILTER_FREQ_LEN];
  uint32_t filter_time_len;
  float filter_time[CHEST_MAX_FILTER_TIME_LEN];
  
  cf_t *tmp_noise; 
  cf_t *tmp_freqavg;
  cf_t *tmp_timeavg[CHEST_MAX_FILTER_TIME_LEN];
  cf_t *tmp_timeavg_mult; 

  interp_linvec_t interp_linvec; 
  interp_lin_t interp_lin; 
  
  float rssi[MAX_PORTS]; 
  float rsrp[MAX_PORTS]; 
  float noise_estimate[MAX_PORTS];
} chest_dl_t;


LIBLTE_API int chest_dl_init(chest_dl_t *q, 
                             lte_cell_t cell);

LIBLTE_API void chest_dl_free(chest_dl_t *q); 

LIBLTE_API int chest_dl_set_filter_freq(chest_dl_t *q, 
                                        float *filter, 
                                        uint32_t filter_len);

LIBLTE_API int chest_dl_set_filter_time(chest_dl_t *q, 
                                        float *filter, 
                                        uint32_t filter_len);

LIBLTE_API int chest_dl_estimate(chest_dl_t *q, 
                                 cf_t *input,
                                 cf_t *ce[MAX_PORTS],
                                 uint32_t sf_idx);

LIBLTE_API int chest_dl_estimate_port(chest_dl_t *q, 
                                      cf_t *input,
                                      cf_t *ce,
                                      uint32_t sf_idx, 
                                      uint32_t port_id);

LIBLTE_API float chest_dl_get_snr(chest_dl_t *q); 

LIBLTE_API float chest_dl_get_noise_estimate(chest_dl_t *q); 

LIBLTE_API float chest_dl_get_rssi(chest_dl_t *q);

LIBLTE_API float chest_dl_get_rsrq(chest_dl_t *q);

LIBLTE_API float chest_dl_get_rsrp(chest_dl_t *q);

#endif