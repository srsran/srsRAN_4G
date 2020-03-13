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

#ifndef SRSLTE_CHEST_UL_H
#define SRSLTE_CHEST_UL_H

#include <stdio.h>

#include "srslte/config.h"

#include "srslte/phy/ch_estimation/chest_common.h"
#include "srslte/phy/ch_estimation/refsignal_ul.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/phch/pucch_cfg.h"
#include "srslte/phy/phch/pusch_cfg.h"
#include "srslte/phy/resampling/interp.h"

typedef struct SRSLTE_API {
  cf_t*    ce;
  uint32_t nof_re;
  float    noise_estimate;
  float    noise_estimate_dbm;
  float    snr;
  float    snr_db;
  float    cfo;
  float    ta_us;
} srslte_chest_ul_res_t;

typedef struct {
  srslte_cell_t cell;

  srslte_refsignal_ul_t             dmrs_signal;
  srslte_refsignal_ul_dmrs_pregen_t dmrs_pregen;
  bool                              dmrs_signal_configured;

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
  float    smooth_filter[SRSLTE_CHEST_MAX_SMOOTH_FIL_LEN];

  srslte_interp_linsrslte_vec_t srslte_interp_linvec;

} srslte_chest_ul_t;

SRSLTE_API int srslte_chest_ul_init(srslte_chest_ul_t* q, uint32_t max_prb);

SRSLTE_API void srslte_chest_ul_free(srslte_chest_ul_t* q);

SRSLTE_API int srslte_chest_ul_res_init(srslte_chest_ul_res_t* q, uint32_t max_prb);

SRSLTE_API void srslte_chest_ul_res_set_identity(srslte_chest_ul_res_t* q);

SRSLTE_API void srslte_chest_ul_res_free(srslte_chest_ul_res_t* q);

SRSLTE_API int srslte_chest_ul_set_cell(srslte_chest_ul_t* q, srslte_cell_t cell);

SRSLTE_API void srslte_chest_ul_pregen(srslte_chest_ul_t* q, srslte_refsignal_dmrs_pusch_cfg_t* cfg);

SRSLTE_API int srslte_chest_ul_estimate_pusch(srslte_chest_ul_t*     q,
                                              srslte_ul_sf_cfg_t*    sf,
                                              srslte_pusch_cfg_t*    cfg,
                                              cf_t*                  input,
                                              srslte_chest_ul_res_t* res);

SRSLTE_API int srslte_chest_ul_estimate_pucch(srslte_chest_ul_t*     q,
                                              srslte_ul_sf_cfg_t*    sf,
                                              srslte_pucch_cfg_t*    cfg,
                                              cf_t*                  input,
                                              srslte_chest_ul_res_t* res);

#endif // SRSLTE_CHEST_UL_H
