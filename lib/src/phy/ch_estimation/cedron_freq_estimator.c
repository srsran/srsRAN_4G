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

#include <complex.h>
#include <math.h>
#include <stdio.h>

#include "srsran/config.h"
#include "srsran/phy/ch_estimation/cedron_freq_estimator.h"
#include "srsran/phy/utils/vector_simd.h"
#include "srsran/srsran.h"
#include <fftw3.h>

static pthread_mutex_t freq_est_fft_mutex = PTHREAD_MUTEX_INITIALIZER;

int srsran_cedron_freq_est_init(srsran_cedron_freq_est_t* q, uint32_t nof_prbs)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  int N   = SRSRAN_MAX_PRB * SRSRAN_NRE;
  if (q != NULL) {
    bzero(q, sizeof(srsran_cedron_freq_est_t));

    q->init_size = N;
    q->fft_size  = nof_prbs * SRSRAN_NRE;
    q->in        = fftwf_malloc(sizeof(fftwf_complex) * N);
    if (!q->in) {
      perror("fftwf_malloc");
      goto clean_exit;
    }
    q->out = fftwf_malloc(sizeof(fftwf_complex) * N);
    if (!q->out) {
      perror("fftwf_malloc");
      goto clean_exit;
    }

    pthread_mutex_lock(&freq_est_fft_mutex);
    q->p = fftwf_plan_dft_1d(q->fft_size, q->in, q->out, FFTW_FORWARD, 0U);
    pthread_mutex_unlock(&freq_est_fft_mutex);

    if (!q->p) {
      perror("fftwf_plan_dft_1d");
      goto clean_exit;
    }
    q->X = q->out;
  }

  ret = SRSRAN_SUCCESS;

clean_exit:
  if (ret != SRSRAN_SUCCESS) {
    srsran_cedron_freq_est_free(q);
  }
  return ret;
}

void srsran_cedron_freq_est_free(srsran_cedron_freq_est_t* q)
{
  if (!q) {
    return;
  }
  pthread_mutex_lock(&freq_est_fft_mutex);
  if (q->in) {
    fftwf_free(q->in);
  }
  if (q->out) {
    fftwf_free(q->out);
  }
  if (q->p) {
    fftwf_destroy_plan(q->p);
    q->p = NULL;
  }
  q->X = NULL;
  pthread_mutex_unlock(&freq_est_fft_mutex);
  bzero(q, sizeof(srsran_cedron_freq_est_t));
}

int srsran_cedron_freq_est_replan_c(srsran_cedron_freq_est_t* q, int new_dft_points)
{
  // No change in size, skip re-planning
  if (q->fft_size == new_dft_points) {
    return 0;
  }

  pthread_mutex_lock(&freq_est_fft_mutex);
  if (q->p) {
    fftwf_destroy_plan(q->p);
    q->p = NULL;
  }
  q->p = fftwf_plan_dft_1d(new_dft_points, q->in, q->out, FFTW_FORWARD, FFTW_MEASURE);
  pthread_mutex_unlock(&freq_est_fft_mutex);

  if (!q->p) {
    return -1;
  }
  q->fft_size = new_dft_points;
  return 0;
}

float srsran_cedron_freq_estimate(srsran_cedron_freq_est_t* q, const cf_t* x, int N)
{
  /*
   * Three Bin Exact Frequency Formulas for a Pure Complex Tone in a DFT
   * Cedron Dawg
   * https://www.dsprelated.com/showarticle/1043.php
   */
  const float TWOPI = 2.0f * (float)M_PI;
  cf_t        Z[3], R1, num, den, ratio;
  float       alpha, f_est;
  int32_t     k_max;

  if (N != q->fft_size) {
    srsran_cedron_freq_est_replan_c(q, N);
  }

  memcpy(q->in, x, sizeof(cf_t) * N);
  fftwf_execute(q->p);

  k_max = srsran_vec_max_ci_simd(q->X, N);
  if (k_max == 0) {
    Z[0] = q->X[N - 1];
    Z[1] = q->X[0];
    Z[2] = q->X[1];
  } else if (k_max == N - 1) {
    Z[0] = q->X[N - 2];
    Z[1] = q->X[N - 1];
    Z[2] = q->X[0];
  } else {
    Z[0] = q->X[k_max - 1];
    Z[1] = q->X[k_max];
    Z[2] = q->X[k_max + 1];
  }

  R1  = cexpf(-1.0 * _Complex_I * TWOPI / N);
  num = -R1 * Z[0] + (1 + R1) * Z[1] - Z[2];
  den = -Z[0] + (1 + R1) * Z[1] - R1 * Z[2];
  srsran_vec_div_ccc_simd(&num, &den, &ratio, 1);
  alpha = atan2f(__imag__(ratio), __real__(ratio));

  if (k_max > floor(N / 2)) {
    k_max = -(N - k_max);
  }
  f_est = 1.0 * k_max / N + alpha * M_1_PI * 0.5f;

  return -f_est;
}