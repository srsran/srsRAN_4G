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

typedef struct LIBLTE_API {
  _Complex float *input_fft;
  _Complex float *filter_fft;
  _Complex float *output_fft;
  _Complex float *output_fft2;
  int input_len;
  int filter_len;
  int output_len;
  dft_plan_t input_plan;
  dft_plan_t filter_plan;
  dft_plan_t output_plan;
}conv_fft_cc_t;

LIBLTE_API int conv_fft_cc_init(conv_fft_cc_t *state, int input_len, int filter_len);
LIBLTE_API void conv_fft_cc_free(conv_fft_cc_t *state);
LIBLTE_API int conv_fft_cc_run(conv_fft_cc_t *state, _Complex float *input, _Complex float *filter, _Complex float *output);

LIBLTE_API int conv_cc(_Complex float *input, _Complex float *filter, _Complex float *output, int input_len, int filter_len);

#endif // CONVOLUTION_H_
