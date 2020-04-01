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

#include "srslte/phy/utils/vector.h"
#include <srslte/phy/channel/hst.h>
#include <srslte/phy/utils/debug.h>
#include <unistd.h>

static srslte_channel_hst_t hst = {};

static float    fd_hz            = 750;
static float    period_s         = 7.2;
static float    init_time_s      = 0;
static uint32_t srate_hz         = 1920000;
static uint32_t sim_time_periods = 1;

#define INPUT_TYPE 0 /* 0: Dirac Delta; Otherwise: Random*/

static void usage(char* prog)
{
  printf("Usage: %s [mMtsT]\n", prog);
  printf("\t-f Doppler frequency [Default %.1f]\n", fd_hz);
  printf("\t-t Period in seconds: [Default %.1f]\n", period_s);
  printf("\t-i Initial time in seconds: [Default %.1f]\n", init_time_s);
  printf("\t-s Sampling rate in Hz: [Default %d]\n", srate_hz);
  printf("\t-T Simulation Time in periods: [Default %d]\n", sim_time_periods);
}

static void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "ftisT")) != -1) {
    switch (opt) {
      case 'f':
        fd_hz = strtof(argv[optind], NULL);
        break;
      case 't':
        period_s = strtof(argv[optind], NULL);
        break;
      case 'i':
        init_time_s = strtof(argv[optind], NULL);
        break;
      case 's':
        srate_hz = (uint32_t)strtof(argv[optind], NULL);
        break;
      case 'T':
        sim_time_periods = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
  int                ret           = SRSLTE_SUCCESS;
  cf_t*              input_buffer  = NULL;
  cf_t*              output_buffer = NULL;
  srslte_timestamp_t ts            = {}; // Initialised to zero
  struct timeval     t[3]          = {};

  // Parse arguments
  parse_args(argc, argv);

  // Initialise buffers
  uint32_t size = srate_hz / 1000; // 1 ms samples
  input_buffer  = srslte_vec_cf_malloc(size);
  output_buffer = srslte_vec_cf_malloc(size);
  if (!input_buffer || !output_buffer) {
    fprintf(stderr, "Error: Allocating memory\n");
    ret = SRSLTE_ERROR;
  }

  // Generate random samples
  if (ret == SRSLTE_SUCCESS) {
    srslte_vec_gen_sine(1.0f, 0.0f, input_buffer, size);
  }

  // Initialise delay channel
  if (ret == SRSLTE_SUCCESS) {
    ret = srslte_channel_hst_init(&hst, fd_hz, period_s, init_time_s);
    srslte_channel_hst_update_srate(&hst, srate_hz);
  }

  // Run actual test
  gettimeofday(&t[1], NULL);
  for (int i = 0; i < sim_time_periods && ret == SRSLTE_SUCCESS; i++) {
    for (int j = 0; j < 1000 * period_s; j++) {
      // Run delay channel
      srslte_channel_hst_execute(&hst, input_buffer, output_buffer, size, &ts);

      // Increment timestamp 1ms
      srslte_timestamp_add(&ts, 0, 0.001);

      float ideal_freq = hst.fs_hz;
      float meas_freq  = srslte_vec_estimate_frequency(output_buffer, size) * srate_hz;
      //      printf("[%03d.%03d] fs = [%6.1f | %6.1f] Hz\n", i, j, ideal_freq, meas_freq);
      if (fabsf(ideal_freq - meas_freq) > 0.5f) {
        printf("Error [%03d.%03d] fs = [%6.1f | %6.1f] Hz\n", i, j, ideal_freq, meas_freq);
        return SRSLTE_ERROR;
      }
    }
  }
  gettimeofday(&t[2], NULL);
  get_time_interval(t);

  // Free
  srslte_channel_hst_free(&hst);

  if (input_buffer) {
    free(input_buffer);
  }

  if (output_buffer) {
    free(output_buffer);
  }

  uint64_t nof_samples = sim_time_periods * (uint64_t)(1000 * period_s) * size;
  double   elapsed_us  = t[0].tv_sec * 1e6 + t[0].tv_usec;

  // Print result and exit
  printf("Test fd=%.1fHz; period=%.1fs; init_time=%.1fs; srate_hz=%d; periods=%d; %s ... %.1f MSps\n",
         fd_hz,
         period_s,
         init_time_s,
         srate_hz,
         sim_time_periods,
         (ret == SRSLTE_SUCCESS) ? "Passed" : "Failed",
         (double)nof_samples / elapsed_us);
  exit(ret);
}
