/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsran/phy/utils/vector.h"
#include <srsran/phy/channel/delay.h>
#include <srsran/phy/utils/debug.h>
#include <srsran/phy/utils/random.h>
#include <unistd.h>

static srsran_channel_delay_t delay = {};

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

static int parse_args(int argc, char** argv)
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
        return SRSRAN_ERROR;
    }
  }
  return SRSRAN_SUCCESS;
}

int main(int argc, char** argv)
{
  int                ret           = SRSRAN_ERROR;
  cf_t*              input_buffer  = NULL;
  cf_t*              output_buffer = NULL;
  srsran_timestamp_t ts            = {}; // Initialised to zero
  srsran_random_t    random_gen    = srsran_random_init(0x1234);
  struct timeval     t[3]          = {};

  // Parse arguments
  if (parse_args(argc, argv) < SRSRAN_SUCCESS) {
    goto clean_exit;
  }

  // Initialise buffers
  uint32_t size = srate_hz / 1000;
  input_buffer  = srsran_vec_cf_malloc(size);
  output_buffer = srsran_vec_cf_malloc(size);
  if (!input_buffer || !output_buffer) {
    ERROR("Error: Allocating memory");
    goto clean_exit;
  }

  // Generate random samples
  srsran_random_uniform_complex_dist_vector(random_gen, input_buffer, size, -1.0f, +1.0f);

  // Initialise delay channel
  if (srsran_channel_delay_init(&delay, delay_min_us, delay_max_us, delay_period_s, delay_init_time_s, srate_hz) <
      SRSRAN_SUCCESS) {
    ERROR("Error initialising delay channel");
    goto clean_exit;
  }

  // Run actual test
  gettimeofday(&t[1], NULL);
  for (int i = 0; i < sim_time_periods; i++) {
    for (int j = 0; j < 1000 * delay_period_s; j++) {
      // Run delay channel
      srsran_channel_delay_execute(&delay, input_buffer, output_buffer, size, &ts);

      // Increment timestamp 1ms
      srsran_timestamp_add(&ts, 0, 0.001);
    }
  }
  gettimeofday(&t[2], NULL);
  get_time_interval(t);

  uint64_t nof_samples = sim_time_periods * 1000 * delay_period_s * size;
  double   elapsed_us  = t[0].tv_sec * 1e6 + t[0].tv_usec;

  double msps          = 0;
  if (isnormal(elapsed_us)) {
    msps = (double)nof_samples / elapsed_us;
    ret  = SRSRAN_SUCCESS;
  } else {
    ERROR("Error in Msps calculation: undefined division");
  }

  // Print result and exit
  printf("Test delay_min_us=%d; delay_max_us=%d; delay_period_s=%.1f; srate_hz=%d; periods=%d; %s ... %.1f MSps\n",
         delay_min_us,
         delay_max_us,
         delay_period_s,
         srate_hz,
         sim_time_periods,
         (ret == SRSRAN_SUCCESS) ? "Passed" : "Failed",
         msps);

clean_exit:
  srsran_random_free(random_gen);
  srsran_channel_delay_free(&delay);

  if (input_buffer) {
    free(input_buffer);
  }

  if (output_buffer) {
    free(output_buffer);
  }

  return (ret);
}
