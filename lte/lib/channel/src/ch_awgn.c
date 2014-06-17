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
#include <stdlib.h>
#include <strings.h>

#include "gauss.h"
#include "lte/channel/ch_awgn.h"

void ch_awgn_c(const cf_t* x, cf_t* y, float variance, int buff_sz) {
  _Complex float tmp;
  int i;

  for (i=0;i<buff_sz;i++) {
    __real__ tmp = rand_gauss();
    __imag__ tmp = rand_gauss();
    tmp *= variance;
    y[i] = tmp + x[i];
  }
}
void ch_awgn_f(const float* x, float* y, float variance, int buff_sz) {
  int i;

  for (i=0;i<buff_sz;i++) {
    y[i] = x[i] + variance * rand_gauss();
  }
}

/* High-level API */
int ch_awgn_initialize(ch_awgn_hl* hl) {

  return 0;
}

int ch_awgn_work(ch_awgn_hl* hl) {
  ch_awgn_c(hl->input,hl->output,hl->ctrl_in.variance,hl->in_len);
  hl->out_len = hl->in_len;
  return 0;
}

int ch_awgn_stop(ch_awgn_hl* hl) {
  return 0;
}
