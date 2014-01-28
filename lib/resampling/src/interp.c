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
#include <complex.h>
#include <math.h>
#include "resampling/interp.h"
#include "utils/debug.h"

/* Performs 1st order linear interpolation with out-of-bound interpolation */
void interp_linear_offset(cf_t *input, cf_t *output, int M, int len, int off_st, int off_end) {
	int i, j;
	float mag0, mag1, arg0, arg1, mag, arg;

	for (i=0;i<len-1;i++) {
		mag0 = cabsf(input[i]);
		mag1 = cabsf(input[i+1]);
		arg0 = cargf(input[i]);
		arg1 = cargf(input[i+1]);
		if (i==0) {
			for (j=0;j<off_st;j++) {
				mag = mag0 - (j+1)*(mag1-mag0)/M;
				arg = arg0 - (j+1)*(arg1-arg0)/M;
				output[j] = mag * cexpf(I * arg);
			}
		}
		for (j=0;j<M;j++) {
			mag = mag0 + j*(mag1-mag0)/M;
			arg = arg0 + j*(arg1-arg0)/M;
			output[i*M+j+off_st] = mag * cexpf(I * arg);
//			DEBUG("output[%d] = input[%d]+%d*(input[%d]-input[%d])/%d = %.3f+%.3f = %.3f delta=%.3f\n",
//					i*M+j, i, j, i+1, i, M, cabsf(input[i]), cabsf(j*(input[i+1] - input[i])/M),
//					cabsf(output[i*M+j]));
		}
	}
	for (j=0;j<off_end;j++) {
		mag = mag1 + j*(mag1-mag0)/M;
		arg = arg1 + j*(arg1-arg0)/M;
		output[i*M+j+off_st] = mag * cexpf(I * arg);
	}
}

/* Performs 1st order linear interpolation */
void interp_linear(cf_t *input, cf_t *output, int M, int len) {
	interp_linear_offset(input, output, M, len, 0, 0);
}
