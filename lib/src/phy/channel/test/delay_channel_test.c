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
#include <srslte/phy/channel/delay.h>
#include <srslte/phy/utils/debug.h>
#include <srslte/phy/utils/random.h>
#include <unistd.h>

static srslte_channel_delay_t delay = {};

static uint32_t delay_min_us      = 10;
static uint32_t delay_max_us      = 3333;
static float    delay_period_s    = 1;
static float    delay_init_time_s = 0;
static uint32_t srate_hz          = 1920000;
static uint32_t sim_time_periods  = 1;

#define INPUT_TYPE 0 /* 0: Dirac Delta; Otherwise: Random*/

static void usage(char* prog)
{
  printf("Usage: %s [mMtsT]\n", prog);
  printf("\t-m Minimum delay in microseconds [Default %d]\n", delay_min_us);
  printf("\t-M Maximum delay in microseconds [Default %d]\n", delay_max_us);
  printf("\t-t Delay period in seconds: [Default %.1f]\n", delay_period_s);
  printf("\t-s Sampling rate in Hz: [Default %d]\n", srate_hz);
  printf("\t-T Simulation Time in periods: [Default %d]\n", sim_time_periods);
}

static void parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "mMtsT")) != -1) {
    switch (opt) {
      case 'm':
        delay_min_us = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 'M':
        delay_max_us = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      case 't':
        delay_period_s = (uint32_t)strtol(argv[optind], NULL, 10);
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
  srslte_random_t    random_gen    = srslte_random_init(0x1234);
  struct timeval     t[3]          = {};

  // Parse arguments
  parse_args(argc, argv);

  // Initialise buffers
  uint32_t size = srate_hz / 1000;
  input_buffer  = srslte_vec_cf_malloc(size);
  output_buffer = srslte_vec_cf_malloc(size);
  if (!input_buffer || !output_buffer) {
    fprintf(stderr, "Error: Allocating memory\n");
    ret = SRSLTE_ERROR;
  }

  // Generate random samples
  srslte_random_uniform_complex_dist_vector(random_gen, input_buffer, size, -1.0f, +1.0f);

  // Initialise delay channel
  if (ret == SRSLTE_SUCCESS) {
    ret = srslte_channel_delay_init(&delay, delay_min_us, delay_max_us, delay_period_s, delay_init_time_s, srate_hz);
  }

  // Run actual test
  gettimeofday(&t[1], NULL);
  for (int i = 0; i < sim_time_periods && ret == SRSLTE_SUCCESS; i++) {
    for (int j = 0; j < 1000 * delay_period_s; j++) {
      // Run delay channel
      srslte_channel_delay_execute(&delay, input_buffer, output_buffer, size, &ts);

      // Increment timestamp 1ms
      srslte_timestamp_add(&ts, 0, 0.001);
    }
  }
  gettimeofday(&t[2], NULL);
  get_time_interval(t);

  // Free
  srslte_random_free(random_gen);
  srslte_channel_delay_free(&delay);

  if (input_buffer) {
    free(input_buffer);
  }

  if (output_buffer) {
    free(output_buffer);
  }

  uint64_t nof_samples = sim_time_periods * 1000 * delay_period_s * size;
  double   elapsed_us  = t[0].tv_sec * 1e6 + t[0].tv_usec;

  // Print result and exit
  printf("Test delay_min_us=%d; delay_max_us=%d; delay_period_s=%.1f; srate_hz=%d; periods=%d; %s ... %.1f MSps\n",
         delay_min_us,
         delay_max_us,
         delay_period_s,
         srate_hz,
         sim_time_periods,
         (ret == SRSLTE_SUCCESS) ? "Passed" : "Failed",
         (double)nof_samples / elapsed_us);
  exit(ret);
}
