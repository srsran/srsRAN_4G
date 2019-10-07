/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#ifndef SRSLTE_CHEST_DL_H
#define SRSLTE_CHEST_DL_H

#include <stdio.h>

#include "srslte/config.h"

#include "srslte/phy/ch_estimation/chest_common.h"
#include "srslte/phy/ch_estimation/refsignal_dl.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/resampling/interp.h"
#include "srslte/phy/sync/pss.h"
#include "wiener_dl.h"

typedef struct SRSLTE_API {
  cf_t*    ce[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
  uint32_t nof_re;
  float    noise_estimate;
  float    noise_estimate_dbm;
  float    snr_db;
  float    snr_ant_port_db[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
  float    rsrp;
  float    rsrp_dbm;
  float    rsrp_neigh;
  float    rsrp_port_dbm[SRSLTE_MAX_PORTS];
  float    rsrp_ant_port_dbm[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
  float    rsrq;
  float    rsrq_db;
  float    rsrq_ant_port_db[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
  float    rssi_dbm;
  float    cfo;
  float    sync_error;
} srslte_chest_dl_res_t;

// Noise estimation algorithm
typedef enum SRSLTE_API {
  SRSLTE_NOISE_ALG_REFS = 0,
  SRSLTE_NOISE_ALG_PSS,
  SRSLTE_NOISE_ALG_EMPTY,
} srslte_chest_dl_noise_alg_t;

// Channel estimator algorithm
typedef enum SRSLTE_API {
  SRSLTE_ESTIMATOR_ALG_AVERAGE = 0,
  SRSLTE_ESTIMATOR_ALG_INTERPOLATE,
  SRSLTE_ESTIMATOR_ALG_WIENER,
} srslte_chest_dl_estimator_alg_t;

typedef struct SRSLTE_API {
  srslte_cell_t cell;
  uint32_t      nof_rx_antennas;

  srslte_refsignal_t   csr_refs;
  srslte_refsignal_t** mbsfn_refs;

  srslte_wiener_dl_t* wiener_dl;

  cf_t* pilot_estimates;
  cf_t* pilot_estimates_average;
  cf_t* pilot_recv_signal;
  cf_t* tmp_noise;
  cf_t* tmp_cfo_estimate;

#ifdef FREQ_SEL_SNR
  float snr_vector[12000];
  float pilot_power[12000];
#endif

  srslte_interp_linsrslte_vec_t srslte_interp_linvec;
  srslte_interp_lin_t           srslte_interp_lin;
  srslte_interp_lin_t           srslte_interp_lin_3;
  srslte_interp_lin_t           srslte_interp_lin_mbsfn;

  float rssi[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
  float rsrp[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
  float rsrp_corr[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
  float noise_estimate[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
  float sync_err[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
  float cfo;

  /* Use PSS for noise estimation in LS linear interpolation mode */
  cf_t pss_signal[SRSLTE_PSS_LEN];
  cf_t tmp_pss[SRSLTE_PSS_LEN];
  cf_t tmp_pss_noisy[SRSLTE_PSS_LEN];

} srslte_chest_dl_t;

typedef struct SRSLTE_API {

  srslte_chest_dl_estimator_alg_t estimator_alg;
  srslte_chest_dl_noise_alg_t     noise_alg;

  srslte_chest_filter_t       filter_type;
  float                       filter_coef[2];

  uint16_t mbsfn_area_id;
  bool     rsrp_neighbour;
  bool     cfo_estimate_enable;
  uint32_t cfo_estimate_sf_mask;
  bool     sync_error_enable;

} srslte_chest_dl_cfg_t;

SRSLTE_API int srslte_chest_dl_init(srslte_chest_dl_t* q, uint32_t max_prb, uint32_t nof_rx_antennas);

SRSLTE_API void srslte_chest_dl_free(srslte_chest_dl_t* q);

SRSLTE_API int srslte_chest_dl_res_init(srslte_chest_dl_res_t* q, uint32_t max_prb);

SRSLTE_API void srslte_chest_dl_res_set_identity(srslte_chest_dl_res_t* q);

SRSLTE_API void srslte_chest_dl_res_set_ones(srslte_chest_dl_res_t* q);

SRSLTE_API void srslte_chest_dl_res_free(srslte_chest_dl_res_t* q);

/* These functions change the internal object state */

SRSLTE_API int srslte_chest_dl_set_mbsfn_area_id(srslte_chest_dl_t* q, uint16_t mbsfn_area_id);

SRSLTE_API int srslte_chest_dl_set_cell(srslte_chest_dl_t* q, srslte_cell_t cell);

/* These functions do not change the internal state */

SRSLTE_API int srslte_chest_dl_estimate(srslte_chest_dl_t*     q,
                                        srslte_dl_sf_cfg_t*    sf,
                                        cf_t*                  input[SRSLTE_MAX_PORTS],
                                        srslte_chest_dl_res_t* res);

SRSLTE_API int srslte_chest_dl_estimate_cfg(srslte_chest_dl_t*     q,
                                            srslte_dl_sf_cfg_t*    sf,
                                            srslte_chest_dl_cfg_t* cfg,
                                            cf_t*                  input[SRSLTE_MAX_PORTS],
                                            srslte_chest_dl_res_t* res);

SRSLTE_API srslte_chest_dl_estimator_alg_t srslte_chest_dl_str2estimator_alg(const char* str);

#endif // SRSLTE_CHEST_DL_H
