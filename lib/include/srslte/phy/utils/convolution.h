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

/******************************************************************************
 *  File:         convolution.h
 *
 *  Description:  Utility module for fast convolution using FFT.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_CONVOLUTION_H
#define SRSLTE_CONVOLUTION_H

#include "srslte/config.h"
#include "srslte/phy/dft/dft.h"
#include <stdint.h>

typedef struct SRSLTE_API {
  cf_t*             input_fft;
  cf_t*             filter_fft;
  cf_t*             output_fft;
  cf_t*             output_fft2;
  uint32_t          input_len;
  uint32_t          filter_len;
  uint32_t          output_len;
  uint32_t          max_input_len;
  uint32_t          max_filter_len;
  srslte_dft_plan_t input_plan;
  srslte_dft_plan_t filter_plan;
  srslte_dft_plan_t output_plan;
  // cf_t *pss_signal_time_fft[3]; // One sequence for each N_id_2
  // cf_t *pss_signal_time[3];

} srslte_conv_fft_cc_t;

SRSLTE_API int srslte_conv_fft_cc_init(srslte_conv_fft_cc_t* q, uint32_t input_len, uint32_t filter_len);

SRSLTE_API int srslte_conv_fft_cc_replan(srslte_conv_fft_cc_t* q, uint32_t input_len, uint32_t filter_len);

SRSLTE_API void srslte_conv_fft_cc_free(srslte_conv_fft_cc_t* q);

SRSLTE_API uint32_t srslte_corr_fft_cc_run(srslte_conv_fft_cc_t* q, cf_t* input, cf_t* filter, cf_t* output);

SRSLTE_API uint32_t srslte_corr_fft_cc_run_opt(srslte_conv_fft_cc_t* q, cf_t* input, cf_t* filter, cf_t* output);

SRSLTE_API uint32_t srslte_conv_fft_cc_run(srslte_conv_fft_cc_t* q,
                                           const cf_t*           input,
                                           const cf_t*           filter,
                                           cf_t*                 output);

SRSLTE_API uint32_t srslte_conv_fft_cc_run_opt(srslte_conv_fft_cc_t* q,
                                               const cf_t*           input,
                                               const cf_t*           filter_freq,
                                               cf_t*                 output);

SRSLTE_API uint32_t
srslte_conv_cc(const cf_t* input, const cf_t* filter, cf_t* output, uint32_t input_len, uint32_t filter_len);

SRSLTE_API uint32_t
srslte_conv_same_cf(cf_t* input, float* filter, cf_t* output, uint32_t input_len, uint32_t filter_len);

SRSLTE_API uint32_t
srslte_conv_same_cc(cf_t* input, cf_t* filter, cf_t* output, uint32_t input_len, uint32_t filter_len);

#endif // SRSLTE_CONVOLUTION_H
