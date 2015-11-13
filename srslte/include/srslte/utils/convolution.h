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

/******************************************************************************
 *  File:         convolution.h
 *
 *  Description:  Utility module for fast convolution using FFT.
 *
 *  Reference:
 *****************************************************************************/

#ifndef CONVOLUTION_H_
#define CONVOLUTION_H_

#include "srslte/config.h"
#include "srslte/dft/dft.h"

typedef struct SRSLTE_API {
  cf_t *input_fft;
  cf_t *filter_fft;
  cf_t *output_fft;
  cf_t *output_fft2;
  uint32_t input_len;
  uint32_t filter_len;
  uint32_t output_len;
  srslte_dft_plan_t input_plan;
  srslte_dft_plan_t filter_plan;
  srslte_dft_plan_t output_plan;
}srslte_conv_fft_cc_t;

SRSLTE_API int srslte_conv_fft_cc_init(srslte_conv_fft_cc_t *q, 
                                       uint32_t input_len, 
                                       uint32_t filter_len);

SRSLTE_API void srslte_conv_fft_cc_free(srslte_conv_fft_cc_t *q);

SRSLTE_API uint32_t srslte_conv_fft_cc_run(srslte_conv_fft_cc_t *q, 
                                           cf_t *input, 
                                           cf_t *filter, 
                                           cf_t *output);

SRSLTE_API uint32_t srslte_conv_cc(cf_t *input, 
                                   cf_t *filter, 
                                   cf_t *output, 
                                   uint32_t input_len, 
                                   uint32_t filter_len);

SRSLTE_API uint32_t srslte_conv_same_cf(cf_t *input, 
                                        float *filter, 
                                        cf_t *output, 
                                        uint32_t input_len, 
                                        uint32_t filter_len);

SRSLTE_API uint32_t srslte_conv_same_cc(cf_t *input, 
                                        cf_t *filter, 
                                        cf_t *output, 
                                        uint32_t input_len, 
                                        uint32_t filter_len);

#endif // CONVOLUTION_H_
