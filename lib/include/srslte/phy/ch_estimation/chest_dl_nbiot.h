/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_CHEST_DL_NBIOT_H
#define SRSLTE_CHEST_DL_NBIOT_H

#include <stdio.h>

#include "srslte/config.h"

#include "srslte/phy/ch_estimation/chest_common.h"
#include "srslte/phy/ch_estimation/chest_dl.h"
#include "srslte/phy/ch_estimation/refsignal_dl_nbiot.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/resampling/interp.h"

/*! \brief Downlink channel estimation for NB-IoT
 *
 * Estimates the channel in the resource elements transmitting references and
 * interpolates for the rest of the resource grid.
 * The equalizer uses the channel estimates to produce an estimation of the
 * transmitted symbol.
 * This object depends on the srslte_refsignal_t object for creating the LTE CSR signal.
 */
typedef struct {
  srslte_nbiot_cell_t         cell;
  srslte_refsignal_dl_nbiot_t nrs_signal;

  cf_t* pilot_estimates;
  cf_t* pilot_estimates_average;
  cf_t* pilot_recv_signal;
  cf_t* tmp_noise;

  uint32_t smooth_filter_len;
  float    smooth_filter[SRSLTE_CHEST_MAX_SMOOTH_FIL_LEN];

  srslte_interp_linsrslte_vec_t srslte_interp_linvec;
  srslte_interp_lin_t           srslte_interp_lin;

  float rssi[SRSLTE_MAX_PORTS];
  float rsrp[SRSLTE_MAX_PORTS];
  float noise_estimate[SRSLTE_MAX_PORTS];

  srslte_chest_dl_noise_alg_t noise_alg;

} srslte_chest_dl_nbiot_t;

SRSLTE_API int srslte_chest_dl_nbiot_init(srslte_chest_dl_nbiot_t* q, uint32_t max_prb);

SRSLTE_API void srslte_chest_dl_nbiot_free(srslte_chest_dl_nbiot_t* q);

SRSLTE_API int srslte_chest_dl_nbiot_set_cell(srslte_chest_dl_nbiot_t* q, srslte_nbiot_cell_t cell);

SRSLTE_API void srslte_chest_dl_nbiot_set_smooth_filter(srslte_chest_dl_nbiot_t* q, float* filter, uint32_t filter_len);

SRSLTE_API void srslte_chest_dl_nbiot_set_smooth_filter3_coeff(srslte_chest_dl_nbiot_t* q, float w);

SRSLTE_API void srslte_chest_dl_nbiot_set_noise_alg(srslte_chest_dl_nbiot_t*    q,
                                                    srslte_chest_dl_noise_alg_t noise_estimation_alg);

SRSLTE_API int srslte_chest_dl_nbiot_estimate(srslte_chest_dl_nbiot_t* q, cf_t* input, cf_t** ce, uint32_t sf_idx);

SRSLTE_API int srslte_chest_dl_nbiot_estimate_port(srslte_chest_dl_nbiot_t* q,
                                                   cf_t*                    input,
                                                   cf_t*                    ce,
                                                   uint32_t                 sf_idx,
                                                   uint32_t                 port_id);

SRSLTE_API float srslte_chest_dl_nbiot_get_noise_estimate(srslte_chest_dl_nbiot_t* q);

SRSLTE_API float srslte_chest_dl_nbiot_get_snr(srslte_chest_dl_nbiot_t* q);

SRSLTE_API float srslte_chest_dl_nbiot_get_rssi(srslte_chest_dl_nbiot_t* q);

SRSLTE_API float srslte_chest_dl_nbiot_get_rsrq(srslte_chest_dl_nbiot_t* q);

SRSLTE_API float srslte_chest_dl_nbiot_get_rsrp(srslte_chest_dl_nbiot_t* q);

#endif // SRSLTE_CHEST_DL_NBIOT_H
