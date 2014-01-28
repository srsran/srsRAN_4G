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

#include <stdlib.h>
#include <string.h>

#include "utils/dft.h"
#include "utils/vector.h"
#include "utils/convolution.h"


int conv_fft_cc_init(conv_fft_cc_t *state, int input_len, int filter_len) {
	state->input_len = input_len;
	state->filter_len = filter_len;
	state->output_len = input_len+filter_len-1;
	state->input_fft = vec_malloc(sizeof(_Complex float)*state->output_len);
	state->filter_fft = vec_malloc(sizeof(_Complex float)*state->output_len);
	state->output_fft = vec_malloc(sizeof(_Complex float)*state->output_len);
	if (!state->input_fft || !state->filter_fft || !state->output_fft) {
		return -1;
	}
	if (dft_plan(state->output_len,COMPLEX_2_COMPLEX,FORWARD,&state->input_plan)) {
		return -2;
	}
	if (dft_plan(state->output_len,COMPLEX_2_COMPLEX,FORWARD,&state->filter_plan)) {
		return -3;
	}
	if (dft_plan(state->output_len,COMPLEX_2_COMPLEX,BACKWARD,&state->output_plan)) {
		return -4;
	}
	return 0;
}

void conv_fft_cc_free(conv_fft_cc_t *state) {
	if (state->input_fft) {
		free(state->input_fft);
	}
	if (state->filter_fft) {
		free(state->filter_fft);
	}
	if (state->output_fft) {
		free(state->output_fft);
	}
	dft_plan_free(&state->input_plan);
	dft_plan_free(&state->filter_plan);
	dft_plan_free(&state->output_plan);
}

int conv_fft_cc_run(conv_fft_cc_t *state, _Complex float *input, _Complex float *filter, _Complex float *output) {

	dft_run_c2c(&state->input_plan, input, state->input_fft);
	dft_run_c2c(&state->filter_plan, filter, state->filter_fft);

	vec_dot_prod(state->input_fft,state->filter_fft,state->output_fft,state->output_len);

	dft_run_c2c(&state->output_plan, state->output_fft, output);

	return state->output_len;

}

int conv_cc(_Complex float *input, _Complex float *filter, _Complex float *output, int input_len, int filter_len) {
	int i,j;
	int output_len;
	output_len=input_len+filter_len-1;
	memset(output,0,output_len*sizeof(_Complex float));
	for (i=0;i<input_len;i++) {
		for (j=0;j<filter_len;j++) {
			output[i+j]+=input[i]*filter[j];
		}
	}
	return output_len;
}
