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
#include <complex.h>
#include <srsran/phy/channel/ch_awgn.h>
#include <srsran/phy/dft/dft.h>
#include <srsran/phy/utils/debug.h>
#include <unistd.h>

#undef ENABLE_GUI
#ifdef ENABLE_GUI
#include "srsgui/srsgui.h"
#endif /* ENABLE_GUI */

static srsran_channel_awgn_t awgn = {};

static uint32_t nof_samples = 1920 * 8;
static float    n0_min      = 0.0f;
static float    n0_max      = 6.0f;
static float    n0_step     = 0.25f;
static float    tolerance   = 0.05f;

static void usage(char* prog)
{
  printf("Usage: %s [nmMst]\n", prog);
  printf("\t-n number of samples to simulate [Default %d]\n", nof_samples);
  printf("\t-m Minimum n0 (in dBfs) to simulate [Default %.3f]\n", n0_min);
  printf("\t-M Maximum n0 (in dBfs) to simulate: [Default %.3f]\n", n0_max);
  printf("\t-s n0 step size: [Default %.3f]\n", n0_step);
  printf("\t-t tolerance: [Default %.3f]\n", tolerance);
}

static int parse_args(int argc, char** argv)
{
  int opt;
  while ((opt = getopt(argc, argv, "nmMst")) != -1) {
    switch (opt) {
      case 'm':
        n0_min = strtof(argv[optind], NULL);
        break;
      case 'M':
        n0_max = strtof(argv[optind], NULL);
        break;
      case 's':
        n0_step = strtof(argv[optind], NULL);
        break;
      case 't':
        tolerance = strtof(argv[optind], NULL);
        break;
      case 'n':
        nof_samples = (uint32_t)strtol(argv[optind], NULL, 10);
        break;
      default:
        usage(argv[0]);
        return SRSRAN_ERROR;
    }
  }
  return SRSRAN_SUCCESS;
}

static int compare_floats(const void* a, const void* b)
{
  float arg1 = *(const float*)a;
  float arg2 = *(const float*)b;

  if (arg1 < arg2) {
    return -1;
  }
  if (arg1 > arg2) {
    return 1;
  }
  return 0;
}

/*
 * Checks for Gaussianity with the Anderson--Darling test: if the returned statistic A2 is larger than 1
 * (and if the number of samples is larger than 100), then the Gaussianity hypothesis is rejected with a significance
 * level of approximately 1% (see https://en.wikipedia.org/wiki/Anderson%E2%80%93Darling_test).
 *
 * x points to the vector of samples (real values and imaginary values)
 * half_length is the number of complex samples
 * y is a pointer to a helper vector used for temporary computations
 */
static float anderson(const float* x, uint32_t half_length, float* y)
{
#define SQRT1_2 ((float)M_SQRT1_2)
#define CDF(a) ((1 + erff((a)*SQRT1_2)) * .5)

  uint32_t length   = 2 * half_length;
  float    length_f = (float)length;

  // estimate mean and variance (the test works better with estimated values than with nominal ones)
  float mean = srsran_vec_acc_ff(x, length);
  mean /= length_f;

  srsran_vec_sc_sum_fff(x, -mean, y, length);
  float variance = srsran_vec_dot_prod_fff(y, y, length);
  variance /= length_f - 1;

  // standardize samples
  srsran_vec_sc_prod_fff(y, 1 / sqrtf(variance), y, length);

  // sort standardized samples
  qsort(y, length, sizeof(float), compare_floats);

  // compute Anderson--Darling statistic
  float cdf1 = NAN;
  float cdf2 = NAN;
  float a2   = 0;
  for (uint32_t ii = 0; ii < nof_samples; ii++) {
    cdf1 = CDF(y[ii]);
    cdf2 = CDF(y[length - ii - 1]);
    a2 += (2.F * ii + 1) * (logf(cdf1) + log1pf(-cdf2)) + (2.F * (length - ii) - 1) * (logf(cdf2) + log1pf(-cdf1));
  }
  a2 = -length_f - a2 / length_f;
  a2 = a2 * (1 + (4 - 25 / length_f) / length_f);

  return a2;
}

int main(int argc, char** argv)
{
  int      ret           = SRSRAN_SUCCESS;
  cf_t*    input_buffer  = NULL;
  cf_t*    output_buffer = NULL;
  float*   help_buffer   = NULL;
  uint64_t count_samples = 0;
  uint64_t count_us      = 0;

#ifdef ENABLE_GUI
  cf_t* fft_out = NULL;
#endif

  // Parse arguments
  if (parse_args(argc, argv) < SRSRAN_SUCCESS) {
    ret = SRSRAN_ERROR;
    goto clean_exit;
  }

  // Initialise buffers
  input_buffer  = srsran_vec_cf_malloc(nof_samples);
  output_buffer = srsran_vec_cf_malloc(nof_samples);
  help_buffer   = srsran_vec_f_malloc(2 * nof_samples);

  if (!input_buffer || !output_buffer || !help_buffer) {
    ERROR("Error: Allocating memory");
    ret = SRSRAN_ERROR;
    goto clean_exit;
  }

  // Initialise input
  srsran_vec_cf_zero(input_buffer, nof_samples);

#ifdef ENABLE_GUI
  sdrgui_init();
  sdrgui_init_title("SRS AWGN Channel");
  plot_real_t    plot_fft     = NULL;
  plot_scatter_t plot_scatter = NULL;

  plot_real_init(&plot_fft);
  plot_real_setTitle(&plot_fft, "Spectrum");
  plot_real_addToWindowGrid(&plot_fft, (char*)"Spectrum", 0, 0);

  plot_scatter_init(&plot_scatter);
  plot_scatter_setTitle(&plot_fft, "IQ");
  plot_scatter_addToWindowGrid(&plot_fft, (char*)"IQ", 1, 0);

  fft_out               = srsran_vec_cf_malloc(nof_samples);
  srsran_dft_plan_t fft = {};
  if (srsran_dft_plan_c(&fft, nof_samples, SRSRAN_DFT_FORWARD)) {
    ERROR("Error: init DFT");
    ret = SRSRAN_ERROR;
    goto clean_exit;
  }
#endif /* ENABLE_GUI */

  // Initialise AWGN channel
  if (srsran_channel_awgn_init(&awgn, 0) < SRSRAN_SUCCESS) {
    ERROR("Error initialising AWGN channel");
    ret = SRSRAN_ERROR;
    goto clean_exit;
  }

  float n0 = n0_min;
  while (!isnan(n0) && !isinf(n0) && n0 < n0_max) {
    struct timeval t[3] = {};

    if (srsran_channel_awgn_set_n0(&awgn, n0) < SRSRAN_SUCCESS) {
      ERROR("Error setting AWGN n0");
      ret = SRSRAN_ERROR;
      goto clean_exit;
    }

    // Run actual test
    gettimeofday(&t[1], NULL);
    srsran_channel_awgn_run_c(&awgn, input_buffer, output_buffer, nof_samples);
    gettimeofday(&t[2], NULL);
    get_time_interval(t);

    float power    = srsran_vec_avg_power_cf(output_buffer, nof_samples);
    float power_dB = srsran_convert_power_to_dB(power);

    if ((n0 + tolerance) < power_dB || (n0 - tolerance) > power_dB) {
      printf("-- failed: %.3f<%.3f<%.3f\n", n0 - tolerance, power_dB, n0 + tolerance);
      ret = SRSRAN_ERROR;
    }

    // Check for Gaussianity
    float a2 = anderson((float*)output_buffer, nof_samples, help_buffer);
    if ((nof_samples > 100 && a2 > 1) || !isfinite(a2)) {
      printf("-- failed: A2 = %f > 1: not Gaussian\n", a2);
      // TODO: use proper RNG with gaussian behaviour
      // ret = SRSRAN_ERROR;
    }

#ifdef ENABLE_GUI
    plot_scatter_setNewData(&plot_scatter, output_buffer, nof_samples);

    srsran_dft_run_c(&fft, output_buffer, fft_out);

    float  min     = +INFINITY;
    float  max     = -INFINITY;
    float* fft_mag = (float*)fft_out;
    for (uint32_t i = 0; i < nof_samples; i++) {
      float mag = srsran_convert_amplitude_to_dB(cabsf(fft_out[i]));

      min = SRSRAN_MIN(min, mag);
      max = SRSRAN_MAX(max, mag);

      fft_mag[i] = mag;
    }

    plot_real_setYAxisScale(&plot_fft, min, max);

    plot_real_setNewData(&plot_fft, fft_mag, nof_samples);
    sleep(5);
#endif /* ENABLE_GUI */

    count_samples += nof_samples;
    count_us += t->tv_usec + t->tv_sec * 1000000;

    n0 += n0_step;
  }

  // Print result and exit
  double msps = 0;
  if (count_us) {
    msps = (double)nof_samples / (double)count_us;
  } else {
    ERROR("Error in Msps calculation: undefined division");
    ret = SRSRAN_ERROR;
  }
  printf("Test n0_min=%.3f; n0_max=%.3f; n0_step=%.3f; nof_samples=%d; %s ... %.1f MSps\n",
         n0_min,
         n0_max,
         n0_step,
         nof_samples,
         (ret == SRSRAN_SUCCESS) ? "Passed" : "Failed",
         msps);

clean_exit:
  srsran_channel_awgn_free(&awgn);

  if (input_buffer) {
    free(input_buffer);
  }
  if (output_buffer) {
    free(output_buffer);
  }
  if (help_buffer) {
    free(help_buffer);
  }

#ifdef ENABLE_GUI
  if (fft_out) {
    free(fft_out);
  }
  srsran_dft_plan_free(&fft);
#endif /* ENABLE_GUI */

  return ret;
}
