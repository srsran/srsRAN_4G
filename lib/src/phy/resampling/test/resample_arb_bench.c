/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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
