/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/random.h"
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "srsran/phy/ch_estimation/cedron_freq_estimator.h"
#include "srsran/phy/dft/dft.h"
#include "srsran/phy/utils/vector.h"

static bool verbose = false;
#define MAXIMUM_ERROR (1e-6f)

double elapsed_us(struct timeval* ts_start, struct timeval* ts_end)
{
  if (ts_end->tv_usec > ts_start->tv_usec) {
    return ((double)ts_end->tv_sec - (double)ts_start->tv_sec) * 1000000 + (double)ts_end->tv_usec -
           (double)ts_start->tv_usec;
  } else {
    return ((double)ts_end->tv_sec - (double)ts_start->tv_sec - 1) * 1000000 + ((double)ts_end->tv_usec + 1000000) -
           (double)ts_start->tv_usec;
  }
}

#define RUN_TEST(FUNCTION)                                                                                             \
  do {                                                                                                                 \
    int            nof_prb;                                                                                            \
    struct timeval start, end;                                                                                         \
    gettimeofday(&start, NULL);                                                                                        \
    bool passed_ = true;                                                                                               \
    for (nof_prb = 1; nof_prb < SRSRAN_MAX_PRB; nof_prb++) {                                                           \
      passed_ &= FUNCTION(nof_prb);                                                                                    \
    }                                                                                                                  \
    gettimeofday(&end, NULL);                                                                                          \
    if (verbose)                                                                                                       \
      printf("%32s: %s ... %6.2f us/call\n",                                                                           \
             #FUNCTION,                                                                                                \
             (passed_) ? "Pass" : "Fail",                                                                              \
             elapsed_us(&start, &end) / SRSRAN_MAX_PRB);                                                               \
    passed &= passed_;                                                                                                 \
  } while (false)

static bool test_cedron_estimate_frequency(int nof_prbs)
{
  srsran_cedron_freq_est_t srsran_cedron_freq_est;
  float                    freq_gold = 0.2f;
  float                    freq      = 0.0f;
  float                    mse       = 0.0f;
  uint32_t                 nof_sym   = nof_prbs * SRSRAN_NRE;

  cf_t* x = srsran_vec_malloc(sizeof(cf_t) * nof_sym);
  if (srsran_cedron_freq_est_init(&srsran_cedron_freq_est, nof_prbs)) {
    ERROR("Error initializing cedron freq estimation algorithm.");
    return false;
  }

  for (int i = 0; i < nof_sym; i++) {
    x[i] = cexpf(-I * 2.0f * M_PI * (float)i * freq_gold);
  }

  freq = srsran_cedron_freq_estimate(&srsran_cedron_freq_est, x, nof_sym);
  mse  = fabsf(freq - freq_gold);
  free(x);
  srsran_cedron_freq_est_free(&srsran_cedron_freq_est);
  if (verbose)
    printf("Nof PRBs %i, mse %f\n", nof_prbs, mse);

  return (mse < MAXIMUM_ERROR);
}

bool test_real_signal(void)
{
  uint32_t                 nof_prbs      = 4;
  uint32_t                 nrefs_sym     = nof_prbs * SRSRAN_NRE;
  float                    ta_err_cedron = 0;
  float                    ta_err_srs    = 0;
  srsran_cedron_freq_est_t srsran_cedron_freq_est;

  // Sniffed UL REF signal with low SNR.
  cf_t cp_pilots[48] = {22.162853 - 26.839521 * 1i,  -12.896494 + 3.750004 * 1i,  43.889961 + 7.452690 * 1i,
                        36.788181 + 3.699238 * 1i,   19.841988 + 2.327892 * 1i,   -8.030174 + 15.597110 * 1i,
                        23.685257 + 9.359170 * 1i,   -0.184066 + 14.776085 * 1i,  54.138931 + 14.602448 * 1i,
                        33.998699 + 11.438558 * 1i,  8.634534 + 23.158798 * 1i,   11.593168 + 14.001324 * 1i,
                        -4.070977 - 28.250189 * 1i,  18.821701 + 1.274709 * 1i,   -2.113699 - 2.322813 * 1i,
                        -1.980798 - 2.809317 * 1i,   -16.248312 + 16.282543 * 1i, 4.916372 - 8.317366 * 1i,
                        19.537739 + 5.440768 * 1i,   19.273443 + 21.419304 * 1i,  9.158796 - 14.670293 * 1i,
                        12.963399 + 16.209164 * 1i,  -10.091204 - 0.774263 * 1i,  52.113579 - 62.882523 * 1i,
                        -45.814278 - 3.351721 * 1i,  16.937546 + 32.659332 * 1i,  -2.446608 + 2.216692 * 1i,
                        -13.836332 + 19.213146 * 1i, -21.508173 + 43.013851 * 1i, -21.323523 + 21.740101 * 1i,
                        -2.203827 - 12.458035 * 1i,  0.313410 - 8.307796 * 1i,    -15.429630 + 14.476921 * 1i,
                        -8.512527 + 34.065918 * 1i,  -16.693293 + 31.356386 * 1i, -34.033825 + 5.859118 * 1i,
                        -11.836067 + 20.825031 * 1i, -24.690987 + 41.358925 * 1i, -11.794442 + 3.393625 * 1i,
                        -18.838444 + 9.678068 * 1i,  7.530683 + 42.732479 * 1i,   -17.050388 + 32.361870 * 1i,
                        -3.941456 + 13.747462 * 1i,  -19.360886 + 11.063116 * 1i, -16.969175 + 30.928513 * 1i,
                        -14.056345 - 35.506645 * 1i, -23.354206 - 9.430195 * 1i,  3.566646 - 14.499187 * 1i};

  if (srsran_cedron_freq_est_init(&srsran_cedron_freq_est, nof_prbs)) {
    ERROR("Error initializing cedron freq estimation algorithm.");
    return false;
  }

  ta_err_cedron = srsran_cedron_freq_estimate(&srsran_cedron_freq_est, cp_pilots, nrefs_sym);
  ta_err_srs    = srsran_vec_estimate_frequency(cp_pilots, nrefs_sym);
  if (verbose) {
    printf("Cedron ta_err =  %f \n", ta_err_cedron);
    printf("SRS ta_err =  %f \n", ta_err_srs);
  }

  ta_err_cedron /= 15e3f; // Convert from normalized frequency to seconds
  ta_err_cedron *= 1e6f;  // Convert to micro-seconds
  ta_err_srs /= 15e3f;    // Convert from normalized frequency to seconds
  ta_err_srs *= 1e6f;     // Convert to micro-seconds

  if (verbose) {
    printf("Cedron TA_err %.1f \n", ta_err_srs);
    printf("SRS TA_err %.1f \n", ta_err_srs);
  }

  srsran_cedron_freq_est_free(&srsran_cedron_freq_est);
  return true;
}

int main(int argc, char** argv)
{
  bool passed = true;
  int  ret    = SRSRAN_SUCCESS;

  RUN_TEST(test_cedron_estimate_frequency);
  passed &= test_real_signal();

  printf("%s!\n", (passed) ? "Ok" : "Failed");

  if (!passed) {
    ret = SRSRAN_ERROR;
  }

  return ret;
}
