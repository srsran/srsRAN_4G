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
 *  File:         chest_ul.h
 *
 *  Description:  3GPP LTE Uplink channel estimator and equalizer.
 *                Estimates the channel in the resource elements transmitting references and
 *                interpolates for the rest of the resource grid.
 *                The equalizer uses the channel estimates to produce an estimation of the
 *                transmitted symbol.
 *
 *  Reference:
 *********************************************************************************************/

#ifndef SRSRAN_CHEST_UL_H
#define SRSRAN_CHEST_UL_H

#include <stdio.h>

#include "srsran/config.h"

#include "srsran/phy/ch_estimation/chest_common.h"
#include "srsran/phy/ch_estimation/refsignal_ul.h"
#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/phch/pucch_cfg.h"
#include "srsran/phy/phch/pusch_cfg.h"
#include "srsran/phy/resampling/interp.h"

typedef struct SRSRAN_API {
  cf_t*    ce;
  uint32_t nof_re;
  float    noise_estimate;
  float    noise_estimate_dbFs;
  float    rsrp;
  float    rsrp_dBfs;
  float    epre;
  float    epre_dBfs;
  float    snr;
  float    snr_db;
  float    cfo_hz;
  float    ta_us;
} srsran_chest_ul_res_t;

typedef struct {
  srsran_cell_t cell;

  srsran_refsignal_ul_t             dmrs_signal;
  srsran_refsignal_ul_dmrs_pregen_t dmrs_pregen;
  bool                              dmrs_signal_configured;

  srsran_refsignal_srs_pregen_t srs_pregen;
  bool                          srs_signal_configured;

  cf_t* pilot_estimates;
  cf_t* pilot_estimates_tmp[4];
  cf_t* pilot_recv_signal;
  cf_t* pilot_known_signal;
  cf_t* tmp_noise;

#ifdef FREQ_SEL_SNR
  float snr_vector[12000];
  float pilot_power[12000];
#endif
  uint32_t smooth_filter_len;
  float    smooth_filter[SRSRAN_CHEST_MAX_SMOOTH_FIL_LEN];

  srsran_interp_linsrsran_vec_t srsran_interp_linvec;

} srsran_chest_ul_t;

SRSRAN_API int srsran_chest_ul_init(srsran_chest_ul_t* q, uint32_t max_prb);

SRSRAN_API void srsran_chest_ul_free(srsran_chest_ul_t* q);

SRSRAN_API int srsran_chest_ul_res_init(srsran_chest_ul_res_t* q, uint32_t max_prb);

SRSRAN_API void srsran_chest_ul_res_set_identity(srsran_chest_ul_res_t* q);

SRSRAN_API void srsran_chest_ul_res_free(srsran_chest_ul_res_t* q);

SRSRAN_API int srsran_chest_ul_set_cell(srsran_chest_ul_t* q, srsran_cell_t cell);

SRSRAN_API void srsran_chest_ul_pregen(srsran_chest_ul_t*                 q,
                                       srsran_refsignal_dmrs_pusch_cfg_t* cfg,
                                       srsran_refsignal_srs_cfg_t*        srs_cfg);

SRSRAN_API int srsran_chest_ul_estimate_pusch(srsran_chest_ul_t*     q,
                                              srsran_ul_sf_cfg_t*    sf,
                                              srsran_pusch_cfg_t*    cfg,
                                              cf_t*                  input,
                                              srsran_chest_ul_res_t* res);

SRSRAN_API int srsran_chest_ul_estimate_pucch(srsran_chest_ul_t*     q,
                                              srsran_ul_sf_cfg_t*    sf,
                                              srsran_pucch_cfg_t*    cfg,
                                              cf_t*                  input,
                                              srsran_chest_ul_res_t* res);

SRSRAN_API int srsran_chest_ul_estimate_srs(srsran_chest_ul_t*                 q,
                                            srsran_ul_sf_cfg_t*                sf,
                                            srsran_refsignal_srs_cfg_t*        cfg,
                                            srsran_refsignal_dmrs_pusch_cfg_t* pusch_cfg,
                                            cf_t*                              input,
                                            srsran_chest_ul_res_t*             res);

#endif // SRSRAN_CHEST_UL_H
