/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#ifndef CONVOLUTION_H_
#define CONVOLUTION_H_

#include "liblte/config.h"
#include "liblte/phy/utils/dft.h"

typedef _Complex float cf_t;

typedef struct LIBLTE_API {
  cf_t *input_fft;
  cf_t *filter_fft;
  cf_t *output_fft;
  cf_t *output_fft2;
  uint32_t input_len;
  uint32_t filter_len;
  uint32_t output_len;
  dft_plan_t input_plan;
  dft_plan_t filter_plan;
  dft_plan_t output_plan;
}conv_fft_cc_t;

LIBLTE_API int conv_fft_cc_init(conv_fft_cc_t *q, 
                                uint32_t input_len, 
                                uint32_t filter_len);

LIBLTE_API void conv_fft_cc_free(conv_fft_cc_t *q);

LIBLTE_API uint32_t conv_fft_cc_run(conv_fft_cc_t *q, 
                               cf_t *input, 
                               cf_t *filter, 
                               cf_t *output);

LIBLTE_API uint32_t conv_cc(cf_t *input, 
                       cf_t *filter, 
                       cf_t *output, 
                       uint32_t input_len, 
                       uint32_t filter_len);

LIBLTE_API uint32_t conv_same_cf(cf_t *input, 
                                 float *filter, 
                                 cf_t *output, 
                                 uint32_t input_len, 
                                 uint32_t filter_len);

LIBLTE_API uint32_t conv_same_cc(cf_t *input, 
                                 cf_t *filter, 
                                 cf_t *output, 
                                 uint32_t input_len, 
                                 uint32_t filter_len);

#endif // CONVOLUTION_H_
