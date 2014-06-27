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

#include <complex.h>
#include <math.h>
#include "liblte/phy/resampling/interp.h"
#include "liblte/phy/utils/debug.h"

/* Performs 1st order linear interpolation with out-of-bound interpolation */
void interp_linear_offset(cf_t *input, cf_t *output, int M, int len, int off_st, int off_end) {
  int i, j;
  float mag0=0, mag1=0, arg0=0, arg1=0, mag=0, arg=0;

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
    }
  }
  if (len > 1) {
    for (j=0;j<off_end;j++) {
      mag = mag1 + j*(mag1-mag0)/M;
      arg = arg1 + j*(arg1-arg0)/M;
      output[i*M+j+off_st] = mag * cexpf(I * arg);
    }
  }
}

/* Performs 1st order linear interpolation */
void interp_linear_c(cf_t *input, cf_t *output, int M, int len) {
  interp_linear_offset(input, output, M, len, 0, 0);
}


/* Performs 1st order integer linear interpolation */
void interp_linear_f(float *input, float *output, int M, int len) {
  int i, j;
  for (i=0;i<len-1;i++) {
    for (j=0;j<M;j++) {
      output[i*M+j] = input[i] + j * (input[i+1]-input[i]) / M;
    }
  }
}
