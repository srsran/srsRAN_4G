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
#include "srslte/phy/resampling/decim.h"
#include "srslte/phy/utils/debug.h"


/* Performs integer linear decimation by a factor of M */
void decim_c(cf_t *input, cf_t *output, int M, int len) {
  int i;
  for (i=0;i<len/M;i++) {
    output[i] = input[i*M];
  }
}


/* Performs integer linear decimation by a factor of M */
void decim_f(float *input, float *output, int M, int len) {
  int i;
  for (i=0;i<len/M;i++) {
    output[i] = input[i*M];
  }
}
