/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
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

#include "srslte/phy/resampling/resample_arb.h"
#include "srslte/srslte.h"

#define ITERATIONS 10000
int main(int argc, char** argv)
{
  int   N    = 9000;
  float rate = 24.0 / 25.0;
  cf_t* in   = srslte_vec_cf_malloc(N);
  cf_t* out  = srslte_vec_cf_malloc(N);

  for (int i = 0; i < N; i++)
    in[i] = sin(i * 2 * M_PI / 100);

  srslte_resample_arb_t r;
  srslte_resample_arb_init(&r, rate, 0);

  clock_t start = clock(), diff;
  for (int xx = 0; xx < ITERATIONS; xx++) {
    srslte_resample_arb_compute(&r, in, out, N);
  }
  diff = clock() - start;

  diff       = diff / ITERATIONS;
  int   msec = diff * 1000 / CLOCKS_PER_SEC;
  float thru = (CLOCKS_PER_SEC / (float)diff) * (N / 1e6);
  printf("Time taken %d seconds %d milliseconds\n", msec / 1000, msec % 1000);
  printf("Rate = %f MS/sec\n", thru);

  free(in);
  free(out);
  printf("Done\n");
  exit(0);
}
