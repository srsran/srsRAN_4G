/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

/******************************************************************************
 *  File:         convolution.h
 *
 *  Description:  Utility module for fast convolution using FFT.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_CONVOLUTION_H
#define SRSRAN_CONVOLUTION_H

#include "srsran/config.h"
#include "srsran/phy/dft/dft.h"
#include <stdint.h>

typedef struct SRSRAN_API {
  cf_t*             input_fft;
  cf_t*             filter_fft;
  cf_t*             output_fft;
  cf_t*             output_fft2;
  uint32_t          input_len;
  uint32_t          filter_len;
  uint32_t          output_len;
  uint32_t          max_input_len;
  uint32_t          max_filter_len;
  srsran_dft_plan_t input_plan;
  srsran_dft_plan_t filter_plan;
  srsran_dft_plan_t output_plan;
  // cf_t *pss_signal_time_fft[3]; // One sequence for each N_id_2
  // cf_t *pss_signal_time[3];

} srsran_conv_fft_cc_t;

SRSRAN_API int srsran_conv_fft_cc_init(srsran_conv_fft_cc_t* q, uint32_t input_len, uint32_t filter_len);

SRSRAN_API int srsran_conv_fft_cc_replan(srsran_conv_fft_cc_t* q, uint32_t input_len, uint32_t filter_len);

SRSRAN_API void srsran_conv_fft_cc_free(srsran_conv_fft_cc_t* q);

SRSRAN_API uint32_t srsran_corr_fft_cc_run(srsran_conv_fft_cc_t* q, cf_t* input, cf_t* filter, cf_t* output);

SRSRAN_API uint32_t srsran_corr_fft_cc_run_opt(srsran_conv_fft_cc_t* q, cf_t* input, cf_t* filter, cf_t* output);

SRSRAN_API uint32_t srsran_conv_fft_cc_run(srsran_conv_fft_cc_t* q,
                                           const cf_t*           input,
                                           const cf_t*           filter,
                                           cf_t*                 output);

SRSRAN_API uint32_t srsran_conv_fft_cc_run_opt(srsran_conv_fft_cc_t* q,
                                               const cf_t*           input,
                                               const cf_t*           filter_freq,
                                               cf_t*                 output);

SRSRAN_API uint32_t
srsran_conv_cc(const cf_t* input, const cf_t* filter, cf_t* output, uint32_t input_len, uint32_t filter_len);

SRSRAN_API uint32_t
srsran_conv_same_cf(cf_t* input, float* filter, cf_t* output, uint32_t input_len, uint32_t filter_len);

SRSRAN_API uint32_t
srsran_conv_same_cc(cf_t* input, cf_t* filter, cf_t* output, uint32_t input_len, uint32_t filter_len);

#endif // SRSRAN_CONVOLUTION_H
