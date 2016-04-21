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

#ifndef CHEST_UL_
#define CHEST_UL_

#include <stdio.h>

#include "srslte/config.h"

#include "srslte/ch_estimation/chest_common.h"
#include "srslte/resampling/interp.h"
#include "srslte/ch_estimation/refsignal_ul.h"
#include "srslte/common/phy_common.h"

typedef struct {
  srslte_cell_t cell; 
  
  srslte_refsignal_ul_t             dmrs_signal;
  srslte_refsignal_ul_dmrs_pregen_t dmrs_pregen; 
  bool dmrs_signal_configured; 
  
  cf_t *pilot_estimates;
  cf_t *pilot_recv_signal; 
  cf_t *tmp_noise; 
  
#ifdef FREQ_SEL_SNR  
  float snr_vector[12000];
  float pilot_power[12000];
#endif
  uint32_t smooth_filter_len; 
  float smooth_filter[SRSLTE_CHEST_MAX_SMOOTH_FIL_LEN];

  srslte_interp_linsrslte_vec_t srslte_interp_linvec; 
  
  float pilot_power; 
  float noise_estimate;
  
} srslte_chest_ul_t;


SRSLTE_API int srslte_chest_ul_init(srslte_chest_ul_t *q, 
                                    srslte_cell_t cell);

SRSLTE_API void srslte_chest_ul_free(srslte_chest_ul_t *q); 

SRSLTE_API void srslte_chest_ul_set_cfg(srslte_chest_ul_t *q, 
                                        srslte_refsignal_dmrs_pusch_cfg_t *pusch_cfg,
                                        srslte_pucch_cfg_t *pucch_cfg, 
                                        srslte_refsignal_srs_cfg_t *srs_cfg);

SRSLTE_API void srslte_chest_ul_set_smooth_filter(srslte_chest_ul_t *q, 
                                                  float *filter, 
                                                  uint32_t filter_len); 

SRSLTE_API void srslte_chest_ul_set_smooth_filter3_coeff(srslte_chest_ul_t* q, 
                                                         float w); 

SRSLTE_API int srslte_chest_ul_estimate(srslte_chest_ul_t *q, 
                                        cf_t *input, 
                                        cf_t *ce, 
                                        uint32_t nof_prb, 
                                        uint32_t sf_idx, 
                                        uint32_t cyclic_shift_for_dmrs, 
                                        uint32_t n_prb[2]);

SRSLTE_API float srslte_chest_ul_get_noise_estimate(srslte_chest_ul_t *q); 

SRSLTE_API float srslte_chest_ul_get_snr(srslte_chest_ul_t *q);


#endif
