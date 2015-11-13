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

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "srslte/srslte.h"
#include "srslte/resampling/resample_arb.h"



int main(int argc, char **argv) {
  int N = 100;        // Number of sinwave samples
  int delay = 5;      // Delay of our resampling filter
  float down = 25.0;  // Downsampling rate

  for(int up=1;up<down;up++)
  {
    float rate = up/down;
    printf("Testing rate %f\n", rate);

    cf_t *in = malloc(N*sizeof(cf_t));
    if(!in) {
      perror("malloc");
      exit(-1);
    }
    cf_t *out = malloc(N*sizeof(cf_t));
    if(!out) {
      perror("malloc");
      exit(-1);
    }

    for(int i=0;i<N;i++)
      in[i] = sin(i*2*M_PI/N);

    // Resample
    srslte_resample_arb_t r;
    srslte_resample_arb_init(&r, rate);
    int n_out = srslte_resample_arb_compute(&r, in, out, N);

    // Check interp values
    for(int i=delay+1;i<n_out;i++){
      float idx = i/rate;
      int pre = floor(idx)-delay;
      int post = ceil(idx)-delay;
      int round = roundf(idx)-delay;
      float diff = fabs(creal(in[pre])-creal(in[post]));
      float diff2 = fabs(creal(out[i])-creal(in[round]));
      if(diff2 > diff && pre != post){
          printf("Interpolation failed at index %f", idx);
          exit(-1);
      }
    }


    free(in);
    free(out);
  }

  printf("Ok\n");
  exit(0);
}
