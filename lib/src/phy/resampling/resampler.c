/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include <complex.h>
#include <srsran/phy/utils/debug.h>
#include <stdlib.h>
#include <string.h>

#include "srsran/phy/resampling/resampler.h"
#include "srsran/phy/utils/vector.h"

/**
 * Raised cosine filter Roll-off
 * 0: Frequency sharp, long in time
 * 1: Frequency relaxed, short in time
 */
#define RESAMPLER_BETA 0.45

/**
 * Filter delay in multiples of ratio
 */
#define RESAMPLER_DELAY 7

/**
 * The FFT size power is determined from the ratio logarithm in base 2 plus the following parameter
 */
#define RESAMPLER_FILTER_SIZE_POW 2

/**
 * Lower bound of the filter size for ensuring a minimum of performance
 */
#define RESAMPLER_FILTER_SIZE_MIN 64

int srsran_resampler_fft_init(srsran_resampler_fft_t* q, srsran_resampler_mode_t mode, uint32_t ratio)
{
  if (q == NULL || ratio == 0) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Initialising the resampler is unnecessary
  if (ratio == 1) {
    q->ratio = 1;
    return SRSRAN_ERROR_OUT_OF_BOUNDS;
  }

  if (q->mode == mode && q->ratio == ratio) {
    return SRSRAN_SUCCESS;
  }

  // Make sure interpolator is freed
  srsran_resampler_fft_free(q);

  // Initialise sizes
  uint32_t base_size =
      SRSRAN_MAX(RESAMPLER_FILTER_SIZE_MIN, (uint32_t)pow(2, ceilf(log2f(ratio) + RESAMPLER_FILTER_SIZE_POW)));
  uint32_t input_fft_size  = 0;
  uint32_t output_fft_size = 0;
  uint32_t high_size       = base_size * ratio;

  // Select FFT/IFFT sizes filter delay and window size. For best performance and avoid aliasing, the window size shall
  // be as big as the input DFT subtracting the filter length at the input rate
  switch (mode) {
    case SRSRAN_RESAMPLER_MODE_INTERPOLATE:
      input_fft_size  = base_size;
      output_fft_size = high_size;
      q->delay        = RESAMPLER_DELAY * ratio;
      q->window_sz    = input_fft_size - 2 * RESAMPLER_DELAY;
      break;
    case SRSRAN_RESAMPLER_MODE_DECIMATE:
    default:
      input_fft_size  = high_size;
      output_fft_size = base_size;
      q->delay        = RESAMPLER_DELAY * ratio;
      q->window_sz    = input_fft_size - 2 * q->delay;
      break;
  }

  q->mode  = mode;
  q->ratio = ratio;

  q->in_buffer = srsran_vec_cf_malloc(high_size);
  if (q->in_buffer == NULL) {
    return SRSRAN_ERROR;
  }

  q->out_buffer = srsran_vec_cf_malloc(high_size);
  if (q->out_buffer == NULL) {
    return SRSRAN_ERROR;
  }

  int err =
      srsran_dft_plan_guru_c(&q->fft, input_fft_size, SRSRAN_DFT_FORWARD, q->in_buffer, q->out_buffer, 1, 1, 1, 1, 1);
  if (err != SRSRAN_SUCCESS) {
    ERROR("Initialising DFT");
    return err;
  }

  err = srsran_dft_plan_guru_c(
      &q->ifft, output_fft_size, SRSRAN_DFT_BACKWARD, q->in_buffer, q->out_buffer, 1, 1, 1, 1, 1);
  if (err != SRSRAN_SUCCESS) {
    ERROR("Initialising DFT");
    return err;
  }

  q->state = srsran_vec_cf_malloc(output_fft_size);
  if (q->state == NULL) {
    return SRSRAN_ERROR;
  }

  q->filter = srsran_vec_cf_malloc(high_size);
  if (q->filter == NULL) {
    return SRSRAN_ERROR;
  }

  // Calculate absolute filter delay
  double delay = (double)q->delay;
  if (mode == SRSRAN_RESAMPLER_MODE_INTERPOLATE) {
    delay = (double)(high_size - q->delay);
  }

  // Compute time domain filter coefficients, see raised cosine formula in section "1.2 Impulse Response" of
  // https://dspguru.com/dsp/reference/raised-cosine-and-root-raised-cosine-formulas/
  double T = (double)1.0;
  for (int32_t i = 0; i < high_size; i++) {
    // Convert to time
    double t = ((double)i - delay) / (double)ratio;

    // Compute coefficient
    double h = 1.0 / T;
    if (isnormal(t)) {
      h = sin(M_PI * t / T);
      h *= cos(M_PI * t * RESAMPLER_BETA / T);
      h /= M_PI * t;
      h /= 1.0 - 4.0 * pow(RESAMPLER_BETA, 2.0) * pow(t, 2.0) / pow(T, 2.0);
    }
    q->in_buffer[i] = (float)h;
  }

  if (get_srsran_verbose_level() >= SRSRAN_VERBOSE_INFO && !is_handler_registered()) {
    printf("h_%s=", q->mode == SRSRAN_RESAMPLER_MODE_INTERPOLATE ? "interp" : "decimate");
    srsran_vec_fprint_c(stdout, q->in_buffer, high_size);
  }

  // Compute frequency domain coefficients, since the filter is symmetrical, it does not matter whether FFT or iFFT
  if (mode == SRSRAN_RESAMPLER_MODE_INTERPOLATE) {
    srsran_dft_run_guru_c(&q->ifft);
  } else {
    srsran_dft_run_guru_c(&q->fft);
  }

  // Normalise filter
  float norm = 1.0f / (cabsf(q->out_buffer[0]) * (float)input_fft_size);
  srsran_vec_sc_prod_cfc(q->out_buffer, norm, q->filter, high_size);

  // reset state
  srsran_resampler_fft_reset_state(q);

  return SRSRAN_SUCCESS;
}

void srsran_resampler_fft_reset_state(srsran_resampler_fft_t* q)
{
  q->state_len = 0;
  srsran_vec_cf_zero(q->state, q->ifft.size);
}

static void resampler_fft_interpolate(srsran_resampler_fft_t* q, const cf_t* input, cf_t* output, uint32_t nsamples)
{
  uint32_t count = 0;

  if (q == NULL) {
    return;
  }

  while (count < nsamples) {
    uint32_t n = SRSRAN_MIN(q->window_sz, nsamples - count);

    if (input) {
      // Copy input samples
      srsran_vec_cf_copy(q->in_buffer, &input[count], n);

      // Pad zeroes
      srsran_vec_cf_zero(&q->in_buffer[n], q->fft.size - n);

      // Execute FFT
      srsran_dft_run_guru_c(&q->fft);

      // Replicate input spectrum and filter at same time
      for (uint32_t i = 0; i < q->ratio; i++) {
        srsran_vec_prod_ccc(q->out_buffer, &q->filter[q->fft.size * i], &q->in_buffer[q->fft.size * i], q->fft.size);
      }

      // Execute iFFT
      srsran_dft_run_guru_c(&q->ifft);
    } else {
      // Equivalent IFFT output of feeding zeroes
      srsran_vec_cf_zero(q->out_buffer, q->ifft.size);
    }

    // Add previous state
    srsran_vec_sum_ccc(q->out_buffer, q->state, q->out_buffer, q->state_len);

    // Copy output
    if (output) {
      srsran_vec_cf_copy(&output[count * q->ratio], q->out_buffer, n * q->ratio);
    }

    // Save current state
    q->state_len = q->ifft.size - n * q->ratio;
    srsran_vec_cf_copy(q->state, &q->out_buffer[n * q->ratio], q->state_len);

    // Increment count
    count += n;
  }
}

static void resampler_fft_decimate(srsran_resampler_fft_t* q, const cf_t* input, cf_t* output, uint32_t nsamples)
{
  uint32_t count = 0;

  if (q == NULL || input == NULL || output == NULL) {
    return;
  }

  while (count < nsamples) {
    uint32_t n = SRSRAN_MIN(q->window_sz, nsamples - count);

    // Copy input samples
    srsran_vec_cf_copy(q->in_buffer, &input[count], n);

    // Pad zeroes
    srsran_vec_cf_zero(&q->in_buffer[n], q->fft.size - n);

    // Execute FFT
    srsran_dft_run_guru_c(&q->fft);

    // Apply filter
    srsran_vec_prod_ccc(q->out_buffer, q->filter, q->out_buffer, q->fft.size);

    // Decimate
    srsran_vec_cf_copy(q->in_buffer, q->out_buffer, q->ifft.size);
    for (uint32_t i = 1; i < q->ratio; i++) {
      srsran_vec_sum_ccc(&q->out_buffer[q->ifft.size * i], q->in_buffer, q->in_buffer, q->ifft.size);
    }

    // Execute iFFT
    srsran_dft_run_guru_c(&q->ifft);

    // Add previous state
    srsran_vec_sum_ccc(q->out_buffer, q->state, q->out_buffer, q->state_len);

    // Copy output
    if (output) {
      srsran_vec_cf_copy(&output[count / q->ratio], q->out_buffer, n / q->ratio);
    }

    // Save current state
    q->state_len = q->ifft.size - n / q->ratio;
    srsran_vec_cf_copy(q->state, &q->out_buffer[n / q->ratio], q->state_len);

    // Increment count
    count += n;
  }
}

void srsran_resampler_fft_run(srsran_resampler_fft_t* q, const cf_t* input, cf_t* output, uint32_t nsamples)
{
  if (q == NULL) {
    return;
  }

  // If the ratio is unset (0) or 1, copy samples and return
  if (q->ratio < 2) {
    srsran_vec_cf_copy(output, input, nsamples);
    return;
  }

  switch (q->mode) {
    case SRSRAN_RESAMPLER_MODE_INTERPOLATE:
      resampler_fft_interpolate(q, input, output, nsamples);
      break;
    case SRSRAN_RESAMPLER_MODE_DECIMATE:
    default:
      resampler_fft_decimate(q, input, output, nsamples);
      break;
  }
}

void srsran_resampler_fft_free(srsran_resampler_fft_t* q)
{
  if (q == NULL) {
    return;
  }

  srsran_dft_plan_free(&q->fft);
  srsran_dft_plan_free(&q->ifft);

  if (q->state) {
    free(q->state);
  }
  if (q->in_buffer) {
    free(q->in_buffer);
  }
  if (q->out_buffer) {
    free(q->out_buffer);
  }
  if (q->filter) {
    free(q->filter);
  }

  memset(q, 0, sizeof(srsran_resampler_fft_t));
}

uint32_t srsran_resampler_fft_get_delay(srsran_resampler_fft_t* q)
{
  if (q == NULL) {
    return UINT32_MAX;
  }

  return q->delay;
}
