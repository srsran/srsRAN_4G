/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#include "srslte/phy/channel/fading.h"
#include "srslte/phy/utils/random.h"
#include "srslte/phy/utils/vector.h"

#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COEFF_A_MIN 100
#define COEFF_A_MAX 2000

/*
 * Tables provided in 36.104 R10 section B.2 Multi-path fading propagation conditions
 */
const static uint32_t nof_taps[4] = {1, 7, 9, 9};

const static float excess_tap_delay_ns[4][SRSLTE_CHANNEL_FADING_MAXTAPS] = {
    /* None */ {0, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
    /* EPA  */ {0, 30, 70, 90, 110, 190, 410, NAN, NAN},
    /* EVA  */ {0, 30, 150, 310, 370, 710, 1090, 1730, 2510},
    /* ETU  */ {0, 50, 120, 200, 230, 500, 1600, 2300, 5000}};

const static float relative_power_db[4][SRSLTE_CHANNEL_FADING_MAXTAPS] = {
    /* None */ {+0.0f, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN},
    /* EPA  */ {+0.0f, -1.0f, -2.0f, -3.0f, -8.0f, -17.2f, -20.8f, NAN, NAN},
    /* EVA  */ {+0.0f, -1.5f, -1.4f, -3.6f, -0.6f, -9.1f, -7.0f, -12.0f, -16.9f},
    /* ETU  */ {-1.0f, -1.0f, -1.0f, +0.0f, +0.0f, +0.0f, -3.0f, -5.0f, -7.0f},
};

static inline int parse_model(srslte_channel_fading_t* q, const char* str)
{
  int      ret    = SRSLTE_SUCCESS;
  uint32_t offset = 3;

  if (strncmp("none", str, 4) == 0) {
    q->model = srslte_channel_fading_model_none;
    offset   = 4;
  } else if (strncmp("epa", str, 3) == 0) {
    q->model = srslte_channel_fading_model_epa;
  } else if (strncmp("eva", str, 3) == 0) {
    q->model = srslte_channel_fading_model_eva;
  } else if (strncmp("etu", str, 3) == 0) {
    q->model = srslte_channel_fading_model_etu;
  } else {
    ret = SRSLTE_ERROR;
  }

  if (ret == SRSLTE_SUCCESS) {
    if (strlen(str) > offset) {
      q->doppler = (float)strtod(&str[offset], NULL);
      if (isnan(q->doppler) || isinf(q->doppler)) {
        q->doppler = 0.0f;
      }
    } else {
      ret = SRSLTE_ERROR;
    }
  }

  return ret;
}

static inline float get_doppler_dispersion(double t, double a, double w, double p)
{
  return (float)(a * sin(w * t + p));
}

static inline void
generate_tap(float delay_ns, float power_db, float srate, float phase, cf_t* buf, uint32_t N, uint32_t path_delay)
{
  float amplitude = powf(10.0f, power_db / 20.0f);
  float O         = (delay_ns * 1e-9f * srate + path_delay) / (float)N;
  cf_t  a0        = amplitude * cexpf(-_Complex_I * phase) / N;

  srslte_vec_gen_sine(a0, -O, buf, N);
}

static inline void generate_taps(srslte_channel_fading_t* q, double time)
{
  // Initialise freq response
  bzero(q->h_freq, sizeof(cf_t) * q->N);

  // Generate taps
  for (int i = 0; i < nof_taps[q->model]; i++) {
    // Compute phase for thee doppler dispersion
    float phase = get_doppler_dispersion(time, q->coeff_a[i], q->coeff_w[i], q->coeff_p[i]);

    // Generate tab
    generate_tap(excess_tap_delay_ns[q->model][i], relative_power_db[q->model][i], q->srate, phase, q->temp, q->N,
                 q->path_delay);

    // Add to frequency response
    srslte_vec_sum_ccc(q->h_freq, q->temp, q->h_freq, q->N);
  }

  // at this stage, q->h_freq should contain the frequency response
}

static void filter_segment(srslte_channel_fading_t* q, const cf_t* input, cf_t* output, uint32_t nsamples)
{
  // Fill Input vector
  memcpy(q->temp, input, sizeof(cf_t) * nsamples);
  bzero(&q->temp[nsamples], sizeof(cf_t) * (q->N - nsamples));

  // Do FFT
  srslte_dft_run_c_zerocopy(&q->fft, q->temp, q->y_freq);

  // Apply channel
  srslte_vec_prod_ccc(q->y_freq, q->h_freq, q->y_freq, q->N);

  // Do iFFT
  srslte_dft_run_c_zerocopy(&q->ifft, q->y_freq, q->temp);

  // Add state
  srslte_vec_sum_ccc(q->temp, q->state, q->temp, q->N);

  // Copy output
  memcpy(output, q->temp, sizeof(cf_t) * nsamples);

  // Copy state
  memcpy(q->state, &q->temp[nsamples], sizeof(cf_t) * (q->N - nsamples));
  bzero(&q->state[q->N - nsamples], sizeof(cf_t) * nsamples);
}

int srslte_channel_fading_init(srslte_channel_fading_t* q, double srate, const char* model, uint32_t seed)
{
  int ret = SRSLTE_ERROR;

  if (q) {
    // Parse model
    if (parse_model(q, model) != SRSLTE_SUCCESS) {
      fprintf(stderr, "Error: invalid channel model '%s'\n", model);
      goto clean_exit;
    }

    // Fill srate
    q->srate = (float)srate;

    // Populate internal parameters
    q->N          = SRSLTE_MAX((uint32_t)1 << (uint32_t)(
                          round(log2(excess_tap_delay_ns[q->model][nof_taps[q->model] - 1] * 1e-9 * srate)) + 3),
                      64);
    q->path_delay = q->N / 4;

    // Initialise random number
    srslte_random_t* random = srslte_random_init(seed);

    // Initialise values
    for (int i = 0; i < nof_taps[q->model]; i++) {
      q->coeff_a[i] = srslte_random_uniform_real_dist(random, COEFF_A_MIN, COEFF_A_MAX);
      q->coeff_w[i] = 2.0 * M_PI * q->doppler / q->coeff_a[i];
      q->coeff_p[i] = srslte_random_uniform_real_dist(random, 0, (float)M_PI / 2.0f);
    }

    // Free random
    srslte_random_free(random);

    // Plan FFT
    if (srslte_dft_plan_c(&q->fft, q->N, SRSLTE_DFT_FORWARD) != SRSLTE_SUCCESS) {
      fprintf(stderr, "Error: planning fft\n");
      goto clean_exit;
    }

    // Plan iFFT
    if (srslte_dft_plan_c(&q->ifft, q->N, SRSLTE_DFT_BACKWARD) != SRSLTE_SUCCESS) {
      fprintf(stderr, "Error: planning ifft\n");
      goto clean_exit;
    }

    // Allocate memory
    q->temp = srslte_vec_malloc(sizeof(cf_t) * q->N);
    if (!q->temp) {
      fprintf(stderr, "Error: allocating h_freq\n");
      goto clean_exit;
    }

    q->h_freq = srslte_vec_malloc(sizeof(cf_t) * q->N);
    if (!q->h_freq) {
      fprintf(stderr, "Error: allocating h_freq\n");
      goto clean_exit;
    }

    q->y_freq = srslte_vec_malloc(sizeof(cf_t) * q->N);
    if (!q->y_freq) {
      fprintf(stderr, "Error: allocating y_freq\n");
      goto clean_exit;
    }

    q->state = srslte_vec_malloc(sizeof(cf_t) * q->N);
    if (!q->state) {
      fprintf(stderr, "Error: allocating y_freq\n");
      goto clean_exit;
    }
    bzero(q->state, sizeof(cf_t) * q->N);
  }

  ret = SRSLTE_SUCCESS;

clean_exit:
  return ret;
}

void srslte_channel_fading_free(srslte_channel_fading_t* q)
{
  if (q) {
    srslte_dft_plan_free(&q->fft);
    srslte_dft_plan_free(&q->ifft);

    if (q->temp) {
      free(q->temp);
    }

    if (q->h_freq) {
      free(q->h_freq);
    }

    if (q->y_freq) {
      free(q->y_freq);
    }

    if (q->state) {
      free(q->state);
    }
  }
}

double srslte_channel_fading_execute(
    srslte_channel_fading_t* q, const cf_t* in, cf_t* out, uint32_t nsamples, double init_time)
{
  uint32_t counter = 0;

  if (q) {
    while (counter < nsamples) {
      // Generate taps
      generate_taps(q, init_time);

      // Do not process more than N / 4 samples
      uint32_t n = SRSLTE_MIN(q->N / 4, nsamples - counter);

      // Execute
      filter_segment(q, &in[counter], &out[counter], n);

      // Increment time
      init_time += n / q->srate;

      // Increment counter
      counter += n;
    }
  }

  // Return time
  return init_time;
}
