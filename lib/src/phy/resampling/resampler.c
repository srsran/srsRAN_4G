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

#include <complex.h>
#include <srslte/phy/utils/debug.h>
#include <stdlib.h>
#include <string.h>

#include "srslte/phy/resampling/resampler.h"
#include "srslte/phy/utils/vector.h"

/**
 * Raised cosine filter Roll-off
 * 0: Frequency sharp, long in time
 * 1: Frequency relaxed, short in time
 */
#define RESAMPLER_BETA 0.45

/**
 * The FFT size power is determined from the ratio logarithm in base 2 plus the following parameter
 */
#define RESAMPLER_FILTER_SIZE_POW 2

/**
 * Lower bound of the filter size for ensuring a minimum of performance
 */
#define RESAMPLER_FILTER_SIZE_MIN 64

int srslte_resampler_fft_init(srslte_resampler_fft_t* q, srslte_resampler_mode_t mode, uint32_t ratio)
{
  if (q == NULL || ratio == 0) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Initialising the resampler is unnecessary
  if (ratio == 1) {
    q->ratio = 1;
    return SRSLTE_ERROR_OUT_OF_BOUNDS;
  }

  // Make sure interpolator is freed
  srslte_resampler_fft_free(q);

  // Initialise sizes
  uint32_t base_size =
      SRSLTE_MAX(RESAMPLER_FILTER_SIZE_MIN, (uint32_t)pow(2, ceilf(log2f(ratio) + RESAMPLER_FILTER_SIZE_POW)));
  uint32_t input_fft_size  = 0;
  uint32_t output_fft_size = 0;
  uint32_t high_size       = base_size * ratio;

  switch (mode) {
    case SRSLTE_RESAMPLER_MODE_INTERPOLATE:
      input_fft_size  = base_size;
      output_fft_size = high_size;
      break;
    case SRSLTE_RESAMPLER_MODE_DECIMATE:
    default:
      input_fft_size  = high_size;
      output_fft_size = base_size;
      break;
  }

  q->mode      = mode;
  q->ratio     = ratio;
  q->window_sz = input_fft_size / 4;

  q->in_buffer = srslte_vec_cf_malloc(high_size);
  if (q->in_buffer == NULL) {
    return SRSLTE_ERROR;
  }

  q->out_buffer = srslte_vec_cf_malloc(high_size);
  if (q->out_buffer == NULL) {
    return SRSLTE_ERROR;
  }

  int err =
      srslte_dft_plan_guru_c(&q->fft, input_fft_size, SRSLTE_DFT_FORWARD, q->in_buffer, q->out_buffer, 1, 1, 1, 1, 1);
  if (err != SRSLTE_SUCCESS) {
    ERROR("Initialising DFT\n");
    return err;
  }

  err = srslte_dft_plan_guru_c(
      &q->ifft, output_fft_size, SRSLTE_DFT_BACKWARD, q->in_buffer, q->out_buffer, 1, 1, 1, 1, 1);
  if (err != SRSLTE_SUCCESS) {
    ERROR("Initialising DFT\n");
    return err;
  }

  q->state = srslte_vec_cf_malloc(output_fft_size);
  if (q->state == NULL) {
    return SRSLTE_ERROR;
  }

  q->filter = srslte_vec_cf_malloc(high_size);
  if (q->filter == NULL) {
    return SRSLTE_ERROR;
  }

  // Compute time domain filter coefficients, see raised cosine formula in section "1.2 Impulse Response" of
  // https://dspguru.com/dsp/reference/raised-cosine-and-root-raised-cosine-formulas/
  double T = (double)1.0;
  for (int32_t i = 0; i < high_size; i++) {
    double t = ((double)i - (double)high_size / 2.0) / (double)ratio;
    double h = 1.0 / T;
    if (isnormal(t)) {
      h = sin(M_PI * t / T);
      h *= cos(M_PI * t * RESAMPLER_BETA / T);
      h /= M_PI * t;
      h /= 1.0 - 4.0 * pow(RESAMPLER_BETA, 2.0) * pow(t, 2.0) / pow(T, 2.0);
    }
    q->in_buffer[i] = (float)h;
  }

  // Compute frequency domain coefficients, since the filter is symmetrical, it does not matter whether FFT or iFFT
  if (mode == SRSLTE_RESAMPLER_MODE_INTERPOLATE) {
    srslte_dft_run_guru_c(&q->ifft);
  } else {
    srslte_dft_run_guru_c(&q->fft);
  }

  // Normalise filter
  float norm = 1.0f / (cabsf(q->out_buffer[0]) * (float)input_fft_size);
  srslte_vec_sc_prod_cfc(q->out_buffer, norm, q->filter, high_size);

  // Zero state
  q->state_len = 0;
  srslte_vec_cf_zero(q->state, output_fft_size);

  return SRSLTE_SUCCESS;
}

static void resampler_fft_interpolate(srslte_resampler_fft_t* q, const cf_t* input, cf_t* output, uint32_t nsamples)
{
  uint32_t count = 0;

  if (q == NULL || input == NULL || output == NULL) {
    return;
  }

  while (count < nsamples) {
    uint32_t n = SRSLTE_MIN(q->window_sz, nsamples - count);

    // Copy input samples
    srslte_vec_cf_copy(q->in_buffer, &input[count], q->window_sz);

    // Pad zeroes
    srslte_vec_cf_zero(&q->in_buffer[n], q->fft.size - n);

    // Execute FFT
    srslte_dft_run_guru_c(&q->fft);

    // Replicate input spectrum
    for (uint32_t i = 1; i < q->ratio; i++) {
      srslte_vec_cf_copy(&q->out_buffer[q->fft.size * i], q->out_buffer, q->fft.size);
    }

    // Apply filtering
    srslte_vec_prod_ccc(q->out_buffer, q->filter, q->in_buffer, q->ifft.size);

    // Execute iFFT
    srslte_dft_run_guru_c(&q->ifft);

    // Add previous state
    srslte_vec_sum_ccc(q->out_buffer, q->state, q->out_buffer, q->state_len);

    // Copy output
    srslte_vec_cf_copy(&output[count * q->ratio], q->out_buffer, n * q->ratio);

    // Save current state
    q->state_len = q->ifft.size - n * q->ratio;
    srslte_vec_cf_copy(q->state, &q->out_buffer[n * q->ratio], q->state_len);

    // Increment count
    count += n;
  }
}

static void resampler_fft_decimate(srslte_resampler_fft_t* q, const cf_t* input, cf_t* output, uint32_t nsamples)
{
  uint32_t count = 0;

  if (q == NULL || input == NULL || output == NULL) {
    return;
  }

  while (count < nsamples) {
    uint32_t n = SRSLTE_MIN(q->window_sz, nsamples - count);

    // Copy input samples
    srslte_vec_cf_copy(q->in_buffer, &input[count], q->window_sz);

    // Pad zeroes
    srslte_vec_cf_zero(&q->in_buffer[n], q->fft.size - n);

    // Execute FFT
    srslte_dft_run_guru_c(&q->fft);

    // Apply filtering and cut
    srslte_vec_prod_ccc(q->out_buffer, q->filter, q->in_buffer, q->ifft.size / 2);
    srslte_vec_prod_ccc(&q->out_buffer[q->fft.size - q->ifft.size / 2],
                        &q->filter[q->fft.size - q->ifft.size / 2],
                        &q->in_buffer[q->ifft.size / 2],
                        q->ifft.size / 2);

    // Execute iFFT
    srslte_dft_run_guru_c(&q->ifft);

    // Add previous state
    srslte_vec_sum_ccc(q->out_buffer, q->state, q->out_buffer, q->state_len);

    // Copy output
    srslte_vec_cf_copy(&output[count / q->ratio], q->out_buffer, n / q->ratio);

    // Save current state
    q->state_len = q->ifft.size - n / q->ratio;
    srslte_vec_cf_copy(q->state, &q->out_buffer[n / q->ratio], q->state_len);

    // Increment count
    count += n;
  }
}

void srslte_resampler_fft_run(srslte_resampler_fft_t* q, const cf_t* input, cf_t* output, uint32_t nsamples)
{
  if (q == NULL) {
    return;
  }

  // If the ratio is unset (0) or 1, copy samples and return
  if (q->ratio < 2) {
    srslte_vec_cf_copy(output, input, nsamples);
    return;
  }

  switch (q->mode) {
    case SRSLTE_RESAMPLER_MODE_INTERPOLATE:
      resampler_fft_interpolate(q, input, output, nsamples);
      break;
    case SRSLTE_RESAMPLER_MODE_DECIMATE:
    default:
      resampler_fft_decimate(q, input, output, nsamples);
      break;
  }
}

void srslte_resampler_fft_free(srslte_resampler_fft_t* q)
{
  if (q == NULL) {
    return;
  }

  srslte_dft_plan_free(&q->fft);
  srslte_dft_plan_free(&q->ifft);

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

  memset(q, 0, sizeof(srslte_resampler_fft_t));
}

uint32_t srslte_resampler_fft_get_delay(srslte_resampler_fft_t* q)
{
  if (q == NULL) {
    return UINT32_MAX;
  }

  return q->ifft.size / 2;
}