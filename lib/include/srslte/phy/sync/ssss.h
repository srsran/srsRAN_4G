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

/******************************************************************************
 *  File:         ssss.h
 *
 *  Description:  Secondary sidelink synchronization signal (SSSS) generation and detection.
 *
 *
 *  Reference:    3GPP TS 36.211 version 15.6.0 Release 15 Sec. 9.7.2
 *****************************************************************************/

#ifndef SRSLTE_SSSS_H
#define SRSLTE_SSSS_H

#include <stdbool.h>
#include <stdint.h>

#include "srslte/phy/common/phy_common_sl.h"
#include "srslte/phy/dft/dft.h"

#define SRSLTE_SSSS_NOF_SEQ 336
#define SRSLTE_SSSS_N 31
#define SRSLTE_SSSS_LEN 2 * SRSLTE_SSSS_N

typedef struct SRSLTE_API {

  float ssss_signal[SRSLTE_SSSS_NOF_SEQ][SRSLTE_SSSS_LEN]; // One sequence for each N_sl_id

  cf_t** ssss_sf_freq;

  cf_t* input_pad_freq;
  cf_t* input_pad_time;

  cf_t* dot_prod_output;
  cf_t* dot_prod_output_time;

  cf_t* shifted_output;

  float* shifted_output_abs;

  int32_t corr_peak_pos;
  float   corr_peak_value;

  uint32_t N_sl_id;

  srslte_dft_plan_t plan_input;
  srslte_dft_plan_t plan_out;

} srslte_ssss_t;

SRSLTE_API int srslte_ssss_init(srslte_ssss_t* q, uint32_t nof_prb, srslte_cp_t cp, srslte_sl_tm_t tm);

SRSLTE_API void srslte_ssss_generate(float* ssss_signal, uint32_t N_sl_id, srslte_sl_tm_t tm);

SRSLTE_API void srslte_ssss_put_sf_buffer(float* ssss_signal, cf_t* sf_buffer, uint32_t nof_prb, srslte_cp_t cp);

SRSLTE_API int srslte_ssss_find(srslte_ssss_t* q, cf_t* input, uint32_t nof_prb, uint32_t N_id_2, srslte_cp_t cp);

SRSLTE_API void srslte_ssss_free(srslte_ssss_t* q);

#endif