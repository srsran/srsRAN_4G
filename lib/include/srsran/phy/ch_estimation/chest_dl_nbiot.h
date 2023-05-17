/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_CHEST_DL_NBIOT_H
#define SRSRAN_CHEST_DL_NBIOT_H

#include <stdio.h>

#include "srsran/config.h"

#include "srsran/phy/ch_estimation/chest_common.h"
#include "srsran/phy/ch_estimation/chest_dl.h"
#include "srsran/phy/ch_estimation/refsignal_dl_nbiot.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/resampling/interp.h"

/*! \brief Downlink channel estimation for NB-IoT
 *
 * Estimates the channel in the resource elements transmitting references and
 * interpolates for the rest of the resource grid.
 * The equalizer uses the channel estimates to produce an estimation of the
 * transmitted symbol.
 * This object depends on the srsran_refsignal_t object for creating the LTE CSR signal.
 */
typedef struct {
  srsran_nbiot_cell_t         cell;
  srsran_refsignal_dl_nbiot_t nrs_signal;

  cf_t* pilot_estimates;
  cf_t* pilot_estimates_average;
  cf_t* pilot_recv_signal;
  cf_t* tmp_noise;

  uint32_t smooth_filter_len;
  float    smooth_filter[SRSRAN_CHEST_MAX_SMOOTH_FIL_LEN];

  srsran_interp_linsrsran_vec_t srsran_interp_linvec;
  srsran_interp_lin_t           srsran_interp_lin;

  float rssi[SRSRAN_MAX_PORTS];
  float rsrp[SRSRAN_MAX_PORTS];
  float noise_estimate[SRSRAN_MAX_PORTS];

  srsran_chest_dl_noise_alg_t noise_alg;

} srsran_chest_dl_nbiot_t;

SRSRAN_API int srsran_chest_dl_nbiot_init(srsran_chest_dl_nbiot_t* q, uint32_t max_prb);

SRSRAN_API void srsran_chest_dl_nbiot_free(srsran_chest_dl_nbiot_t* q);

SRSRAN_API int srsran_chest_dl_nbiot_set_cell(srsran_chest_dl_nbiot_t* q, srsran_nbiot_cell_t cell);

SRSRAN_API void srsran_chest_dl_nbiot_set_smooth_filter(srsran_chest_dl_nbiot_t* q, float* filter, uint32_t filter_len);

SRSRAN_API void srsran_chest_dl_nbiot_set_smooth_filter3_coeff(srsran_chest_dl_nbiot_t* q, float w);

SRSRAN_API void srsran_chest_dl_nbiot_set_noise_alg(srsran_chest_dl_nbiot_t*    q,
                                                    srsran_chest_dl_noise_alg_t noise_estimation_alg);

SRSRAN_API int srsran_chest_dl_nbiot_estimate(srsran_chest_dl_nbiot_t* q, cf_t* input, cf_t** ce, uint32_t sf_idx);

SRSRAN_API int srsran_chest_dl_nbiot_estimate_port(srsran_chest_dl_nbiot_t* q,
                                                   cf_t*                    input,
                                                   cf_t*                    ce,
                                                   uint32_t                 sf_idx,
                                                   uint32_t                 port_id);

SRSRAN_API float srsran_chest_dl_nbiot_get_noise_estimate(srsran_chest_dl_nbiot_t* q);

SRSRAN_API float srsran_chest_dl_nbiot_get_snr(srsran_chest_dl_nbiot_t* q);

SRSRAN_API float srsran_chest_dl_nbiot_get_rssi(srsran_chest_dl_nbiot_t* q);

SRSRAN_API float srsran_chest_dl_nbiot_get_rsrq(srsran_chest_dl_nbiot_t* q);

SRSRAN_API float srsran_chest_dl_nbiot_get_rsrp(srsran_chest_dl_nbiot_t* q);

#endif // SRSRAN_CHEST_DL_NBIOT_H
