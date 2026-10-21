// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srsran/phy/resampling/resample_arb.h"
#include "srsran/srsran.h"

#define ITERATIONS 10000
int main(int argc, char** argv)
{
  int   N    = 9000;
  float rate = 24.0 / 25.0;
  cf_t* in   = srsran_vec_cf_malloc(N);
  cf_t* out  = srsran_vec_cf_malloc(N);

  for (int i = 0; i < N; i++)
    in[i] = sin(i * 2 * M_PI / 100);

  srsran_resample_arb_t r;
  srsran_resample_arb_init(&r, rate, 0);

  clock_t start = clock(), diff;
  for (int xx = 0; xx < ITERATIONS; xx++) {
    srsran_resample_arb_compute(&r, in, out, N);
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
