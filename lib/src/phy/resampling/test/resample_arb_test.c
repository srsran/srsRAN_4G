/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srsran/phy/resampling/resample_arb.h"
#include "srsran/srsran.h"

int main(int argc, char** argv)
{
  int   N     = 100;  // Number of sinwave samples
  int   delay = 5;    // Delay of our resampling filter
  float down  = 25.0; // Downsampling rate

  for (int up = 1; up < down; up++) {
    float rate = up / down;
    printf("Testing rate %f\n", rate);

    cf_t* in = srsran_vec_cf_malloc(N);
    if (!in) {
      perror("malloc");
      exit(-1);
    }
    cf_t* out = srsran_vec_cf_malloc(N);
    if (!out) {
      perror("malloc");
      exit(-1);
    }

    for (int i = 0; i < N; i++)
      in[i] = sin(i * 2 * M_PI / N);

    // Resample
    srsran_resample_arb_t r;
    srsran_resample_arb_init(&r, rate, 0);
    int n_out = srsran_resample_arb_compute(&r, in, out, N);

    // Check interp values
    for (int i = delay + 1; i < n_out; i++) {
      float idx   = i / rate;
      int   pre   = floor(idx) - delay;
      int   post  = ceil(idx) - delay;
      int   round = roundf(idx) - delay;
      float diff  = fabs(creal(in[pre]) - creal(in[post]));
      float diff2 = fabs(creal(out[i]) - creal(in[round]));
      if (diff2 > diff && pre != post) {
        printf("Interpolation failed at index %f\n", idx);
        exit(-1);
      }
    }

    free(in);
    free(out);
  }

  printf("Ok\n");
  exit(0);
}
