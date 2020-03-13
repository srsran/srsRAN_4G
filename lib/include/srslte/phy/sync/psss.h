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
 *  File:         psss.h
 *
 *  Description:  Primary sidelink synchronization signal (PSSS) generation and detection.
 *
 *
 *  Reference:    3GPP TS 36.211 version 15.6.0 Release 15 Sec. 9.7.1
 *****************************************************************************/

#ifndef SRSLTE_PSSS_H
#define SRSLTE_PSSS_H

#include <stdbool.h>
#include <stdint.h>

#include "srslte/config.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/dft/dft.h"

#define SRSLTE_PSSS_LEN 62

typedef struct SRSLTE_API {

  cf_t psss_signal[2][SRSLTE_PSSS_LEN]; // One sequence for each N_id_2

  cf_t** psss_sf_freq;

  cf_t* input_pad_freq;
  cf_t* input_pad_time;

  cf_t* dot_prod_output;
  cf_t* dot_prod_output_time;

  cf_t* shifted_output;

  float* shifted_output_abs;

  int32_t corr_peak_pos;
  float   corr_peak_value;

  uint32_t N_id_2;

  srslte_dft_plan_t plan_input;
  srslte_dft_plan_t plan_out;

} srslte_psss_t;

SRSLTE_API int srslte_psss_init(srslte_psss_t* q, uint32_t nof_prb, srslte_cp_t cp);

SRSLTE_API int srslte_psss_generate(cf_t* psss_signal, uint32_t N_id_2);

SRSLTE_API void srslte_psss_put_sf_buffer(cf_t* psss_signal, cf_t* sf_buffer, uint32_t nof_prb, srslte_cp_t cp);

SRSLTE_API int srslte_psss_find(srslte_psss_t* q, cf_t* input, uint32_t nof_prb, srslte_cp_t cp);

SRSLTE_API void srslte_psss_free(srslte_psss_t* q);

#endif // SRSLTE_PSSS_H
