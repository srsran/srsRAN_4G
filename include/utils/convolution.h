/*
 * Copyright (c) 2013, Ismael Gomez-Miguelez <gomezi@tcd.ie>.
 * This file is part of OSLD-lib (http://https://github.com/ismagom/osld-lib)
 *
 * OSLD-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OSLD-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OSLD-lib.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONVOLUTION_H_
#define CONVOLUTION_H_

#include "utils/dft.h"

typedef struct  {
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

int conv_fft_cc_init(conv_fft_cc_t *state, int input_len, int filter_len);
void conv_fft_cc_free(conv_fft_cc_t *state);
int conv_fft_cc_run(conv_fft_cc_t *state, _Complex float *input, _Complex float *filter, _Complex float *output);

int conv_cc(_Complex float *input, _Complex float *filter, _Complex float *output, int input_len, int filter_len);

#endif
