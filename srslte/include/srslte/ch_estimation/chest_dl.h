/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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
 *  File:         chest_dl.h
 *
 *  Description:  3GPP LTE Downlink channel estimator and equalizer.
 *                Estimates the channel in the resource elements transmitting references and
 *                interpolates for the rest of the resource grid.
 *                The equalizer uses the channel estimates to produce an estimation of the
 *                transmitted symbol.
 *                This object depends on the srslte_refsignal_t object for creating the LTE
 *                CSR signal.
 *
 *  Reference:
 *********************************************************************************************/

#ifndef CHEST_DL_
#define CHEST_DL_

#include <stdio.h>

#include "srslte/config.h"

#include "srslte/resampling/interp.h"
#include "srslte/ch_estimation/refsignal_dl.h"
#include "srslte/common/phy_common.h"

#define SRSLTE_CHEST_MAX_FILTER_FREQ_LEN    21
#define SRSLTE_CHEST_MAX_FILTER_TIME_LEN    40

typedef struct {
  srslte_cell_t cell; 
  srslte_refsignal_cs_t csr_signal;
  cf_t *pilot_estimates[SRSLTE_MAX_PORTS];
  cf_t *pilot_estimates_average[SRSLTE_MAX_PORTS];
  cf_t *pilot_recv_signal[SRSLTE_MAX_PORTS];
 
  uint32_t filter_freq_len;
  float filter_freq[SRSLTE_CHEST_MAX_FILTER_FREQ_LEN];
  uint32_t filter_time_len;
  float filter_time[SRSLTE_CHEST_MAX_FILTER_TIME_LEN];
  
  cf_t *tmp_noise; 
  cf_t *tmp_freqavg;
  cf_t *tmp_timeavg[SRSLTE_CHEST_MAX_FILTER_TIME_LEN];
  cf_t *tmp_timeavg_mult; 

  srslte_interp_linsrslte_vec_t srslte_interp_linvec; 
  srslte_interp_lin_t srslte_interp_lin; 
  
  float rssi[SRSLTE_MAX_PORTS]; 
  float rsrp[SRSLTE_MAX_PORTS]; 
  float noise_estimate[SRSLTE_MAX_PORTS];
  float filter_time_ema;
} srslte_chest_dl_t;


SRSLTE_API int srslte_chest_dl_init(srslte_chest_dl_t *q, 
                                    srslte_cell_t cell);

SRSLTE_API void srslte_chest_dl_free(srslte_chest_dl_t *q); 

SRSLTE_API int srslte_chest_dl_set_filter_freq(srslte_chest_dl_t *q, 
                                               float *filter, 
                                               uint32_t filter_len);

SRSLTE_API int srslte_chest_dl_set_filter_time(srslte_chest_dl_t *q, 
                                               float *filter, 
                                               uint32_t filter_len);

SRSLTE_API void srslte_chest_dl_set_filter_time_ema(srslte_chest_dl_t *q, 
                                                    float ema_coefficient); 

SRSLTE_API int srslte_chest_dl_estimate(srslte_chest_dl_t *q, 
                                        cf_t *input,
                                        cf_t *ce[SRSLTE_MAX_PORTS],
                                        uint32_t sf_idx);

SRSLTE_API int srslte_chest_dl_estimate_port(srslte_chest_dl_t *q, 
                                             cf_t *input,
                                             cf_t *ce,
                                             uint32_t sf_idx, 
                                             uint32_t port_id);

SRSLTE_API float srslte_chest_dl_get_noise_estimate(srslte_chest_dl_t *q); 

SRSLTE_API float srslte_chest_dl_get_snr(srslte_chest_dl_t *q);

SRSLTE_API float srslte_chest_dl_get_rssi(srslte_chest_dl_t *q);

SRSLTE_API float srslte_chest_dl_get_rsrq(srslte_chest_dl_t *q);

SRSLTE_API float srslte_chest_dl_get_rsrp(srslte_chest_dl_t *q);

#endif
