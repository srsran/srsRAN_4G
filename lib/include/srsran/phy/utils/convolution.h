/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
