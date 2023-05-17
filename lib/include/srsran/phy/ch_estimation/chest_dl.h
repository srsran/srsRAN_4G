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

/**********************************************************************************************
 *  File:         chest_dl.h
 *
 *  Description:  3GPP LTE Downlink channel estimator and equalizer.
 *                Estimates the channel in the resource elements transmitting references and
 *                interpolates for the rest of the resource grid.
 *                The equalizer uses the channel estimates to produce an estimation of the
 *                transmitted symbol.
 *                This object depends on the srsran_refsignal_t object for creating the LTE
 *                CSR signal.
 *
 *  Reference:
 *********************************************************************************************/

#ifndef SRSRAN_CHEST_DL_H
#define SRSRAN_CHEST_DL_H

#include <stdio.h>

#include "srsran/config.h"

#include "srsran/phy/ch_estimation/chest_common.h"
#include "srsran/phy/ch_estimation/refsignal_dl.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/resampling/interp.h"
#include "srsran/phy/sync/pss.h"
#include "wiener_dl.h"

typedef struct SRSRAN_API {
  cf_t*    ce[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS];
  uint32_t nof_re;
  float    noise_estimate;
  float    noise_estimate_dbm;
  float    snr_db;
  float    snr_ant_port_db[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS];
  float    rsrp;
  float    rsrp_dbm;
  float    rsrp_neigh;
  float    rsrp_port_dbm[SRSRAN_MAX_PORTS];
  float    rsrp_ant_port_dbm[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS];
  float    rsrq;
  float    rsrq_db;
  float    rsrq_ant_port_db[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS];
  float    rssi_dbm;
  float    cfo;
  float    sync_error;
} srsran_chest_dl_res_t;

// Noise estimation algorithm
typedef enum SRSRAN_API {
  SRSRAN_NOISE_ALG_REFS = 0,
  SRSRAN_NOISE_ALG_PSS,
  SRSRAN_NOISE_ALG_EMPTY,
} srsran_chest_dl_noise_alg_t;

// Channel estimator algorithm
typedef enum SRSRAN_API {
  SRSRAN_ESTIMATOR_ALG_AVERAGE = 0,
  SRSRAN_ESTIMATOR_ALG_INTERPOLATE,
  SRSRAN_ESTIMATOR_ALG_WIENER,
} srsran_chest_dl_estimator_alg_t;

typedef struct SRSRAN_API {
  srsran_cell_t cell;
  uint32_t      nof_rx_antennas;

  srsran_refsignal_t   csr_refs;
  srsran_refsignal_t** mbsfn_refs;

  srsran_wiener_dl_t* wiener_dl;

  cf_t* pilot_estimates;
  cf_t* pilot_estimates_average;
  cf_t* pilot_recv_signal;
  cf_t* tmp_noise;
  cf_t* tmp_cfo_estimate;

#ifdef FREQ_SEL_SNR
  float snr_vector[12000];
  float pilot_power[12000];
#endif

  srsran_interp_linsrsran_vec_t srsran_interp_linvec;
  srsran_interp_lin_t           srsran_interp_lin;
  srsran_interp_lin_t           srsran_interp_lin_3;
  srsran_interp_lin_t           srsran_interp_lin_mbsfn;

  float rssi[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS];
  float rsrp[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS];
  float rsrp_corr[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS];
  float noise_estimate[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS];
  float sync_err[SRSRAN_MAX_PORTS][SRSRAN_MAX_PORTS];
  float cfo;

  /* Use PSS for noise estimation in LS linear interpolation mode */
  cf_t pss_signal[SRSRAN_PSS_LEN];
  cf_t tmp_pss[SRSRAN_PSS_LEN];
  cf_t tmp_pss_noisy[SRSRAN_PSS_LEN];

} srsran_chest_dl_t;

typedef struct SRSRAN_API {
  srsran_chest_dl_estimator_alg_t estimator_alg;
  srsran_chest_dl_noise_alg_t     noise_alg;

  srsran_chest_filter_t       filter_type;
  float                       filter_coef[2];

  uint16_t mbsfn_area_id;
  bool     rsrp_neighbour;
  bool     cfo_estimate_enable;
  uint32_t cfo_estimate_sf_mask;
  bool     sync_error_enable;

} srsran_chest_dl_cfg_t;

SRSRAN_API int srsran_chest_dl_init(srsran_chest_dl_t* q, uint32_t max_prb, uint32_t nof_rx_antennas);

SRSRAN_API void srsran_chest_dl_free(srsran_chest_dl_t* q);

SRSRAN_API int srsran_chest_dl_res_init(srsran_chest_dl_res_t* q, uint32_t max_prb);
SRSRAN_API int srsran_chest_dl_res_init_re(srsran_chest_dl_res_t* q, uint32_t nof_re);

SRSRAN_API void srsran_chest_dl_res_set_identity(srsran_chest_dl_res_t* q);

SRSRAN_API void srsran_chest_dl_res_set_ones(srsran_chest_dl_res_t* q);

SRSRAN_API void srsran_chest_dl_res_free(srsran_chest_dl_res_t* q);

/* These functions change the internal object state */

SRSRAN_API int srsran_chest_dl_set_mbsfn_area_id(srsran_chest_dl_t* q, uint16_t mbsfn_area_id);

SRSRAN_API int srsran_chest_dl_set_cell(srsran_chest_dl_t* q, srsran_cell_t cell);

/* These functions do not change the internal state */

SRSRAN_API int srsran_chest_dl_estimate(srsran_chest_dl_t*     q,
                                        srsran_dl_sf_cfg_t*    sf,
                                        cf_t*                  input[SRSRAN_MAX_PORTS],
                                        srsran_chest_dl_res_t* res);

SRSRAN_API int srsran_chest_dl_estimate_cfg(srsran_chest_dl_t*     q,
                                            srsran_dl_sf_cfg_t*    sf,
                                            srsran_chest_dl_cfg_t* cfg,
                                            cf_t*                  input[SRSRAN_MAX_PORTS],
                                            srsran_chest_dl_res_t* res);

SRSRAN_API srsran_chest_dl_estimator_alg_t srsran_chest_dl_str2estimator_alg(const char* str);

#endif // SRSRAN_CHEST_DL_H
