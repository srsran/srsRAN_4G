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
#include <math.h>
#include <string.h>

#include "srslte/phy/sync/sss.h"
#include "srslte/phy/utils/vector.h"

#define MAX_M 3

static void corr_all_zs(cf_t z[SRSLTE_SSS_N], float s[SRSLTE_SSS_N][SRSLTE_SSS_N - 1], float output[SRSLTE_SSS_N])
{
  uint32_t m;
  cf_t     tmp[SRSLTE_SSS_N];

  for (m = 0; m < SRSLTE_SSS_N; m++) {
    tmp[m] = srslte_vec_dot_prod_cfc(z, s[m], SRSLTE_SSS_N - 1);
  }
  srslte_vec_abs_square_cf(tmp, output, SRSLTE_SSS_N);
}

static void
corr_all_sz_partial(cf_t z[SRSLTE_SSS_N], float s[SRSLTE_SSS_N][SRSLTE_SSS_N], uint32_t M, float output[SRSLTE_SSS_N])
{
  uint32_t Nm = SRSLTE_SSS_N / M;
  cf_t     tmp[SRSLTE_SSS_N];
  float    tmp_abs[MAX_M - 1][SRSLTE_SSS_N];
  int      j, m;
  float*   ptr;

  for (j = 0; j < M; j++) {
    for (m = 0; m < SRSLTE_SSS_N; m++) {
      tmp[m] = srslte_vec_dot_prod_cfc(&z[j * Nm], &s[m][j * Nm], Nm);
    }
    if (j == 0) {
      ptr = output;
    } else {
      ptr = tmp_abs[j - 1];
    }
    srslte_vec_abs_square_cf(tmp, ptr, SRSLTE_SSS_N);
  }
  for (j = 1; j < M; j++) {
    srslte_vec_sum_fff(tmp_abs[j - 1], output, output, SRSLTE_SSS_N);
  }
}

static void extract_pair_sss(srslte_sss_t* q, const cf_t* input, cf_t* ce, cf_t y[2][SRSLTE_SSS_N])
{
  cf_t input_fft[SRSLTE_SYMBOL_SZ_MAX];

  // Run FFT
  srslte_dft_run_c(&q->dftp_input, input, input_fft);

  // Equalize if available channel estimate
  if (ce) {
    srslte_vec_div_ccc(
        &input_fft[q->fft_size / 2 - SRSLTE_SSS_N], ce, &input_fft[q->fft_size / 2 - SRSLTE_SSS_N], 2 * SRSLTE_SSS_N);
  }

  // Extract FFT Data
  for (int i = 0; i < SRSLTE_SSS_N; i++) {
    y[0][i] = input_fft[q->fft_size / 2 - SRSLTE_SSS_N + 2 * i];
    y[1][i] = input_fft[q->fft_size / 2 - SRSLTE_SSS_N + 2 * i + 1];
  }

  // Normalize
  for (int i = 0; i < 2; i++) {
    float avg_pow = srslte_vec_avg_power_cf(y[i], SRSLTE_SSS_N);
    float rms     = (avg_pow != 0.0f) ? sqrtf(avg_pow) : 1.0f;

    srslte_vec_sc_prod_cfc(y[i], 1.0 / rms, y[i], SRSLTE_SSS_N);
  }

  // Unmask signal with sequence generated from NID2
  srslte_vec_prod_cfc(y[0], q->fc_tables[q->N_id_2].c[0], y[0], SRSLTE_SSS_N);
  srslte_vec_prod_cfc(y[1], q->fc_tables[q->N_id_2].c[1], y[1], SRSLTE_SSS_N);
}

int srslte_sss_m0m1_diff(srslte_sss_t* q,
                         const cf_t*   input,
                         uint32_t*     m0,
                         float*        m0_value,
                         uint32_t*     m1,
                         float*        m1_value)
{
  return srslte_sss_m0m1_diff_coh(q, input, NULL, m0, m0_value, m1, m1_value);
}

/* Differential SSS estimation.
 * Returns m0 and m1 estimates
 *
 * Source: "SSS Detection Method for Initial Cell Search in 3GPP LTE FDD/TDD Dual Mode Receiver"
 *       Jung-In Kim, Jung-Su Han, Hee-Jin Roh and Hyung-Jin Choi

 *
 */
int srslte_sss_m0m1_diff_coh(srslte_sss_t* q,
                             const cf_t*   input,
                             cf_t          ce[2 * SRSLTE_SSS_N],
                             uint32_t*     m0,
                             float*        m0_value,
                             uint32_t*     m1,
                             float*        m1_value)
{

  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && input != NULL && m0 != NULL && m1 != NULL) {

    cf_t yprod[SRSLTE_SSS_N];
    cf_t y[2][SRSLTE_SSS_N];

    extract_pair_sss(q, input, ce, y);

    srslte_vec_prod_conj_ccc(&y[0][1], y[0], yprod, SRSLTE_SSS_N - 1);
    corr_all_zs(yprod, q->fc_tables[q->N_id_2].sd, q->corr_output_m0);
    *m0 = srslte_vec_max_fi(q->corr_output_m0, SRSLTE_SSS_N);
    if (m0_value) {
      *m0_value = q->corr_output_m0[*m0];
    }

    srslte_vec_prod_cfc(y[1], q->fc_tables[q->N_id_2].z1[*m0], y[1], SRSLTE_SSS_N);
    srslte_vec_prod_conj_ccc(&y[1][1], y[1], yprod, SRSLTE_SSS_N - 1);
    corr_all_zs(yprod, q->fc_tables[q->N_id_2].sd, q->corr_output_m1);
    *m1 = srslte_vec_max_fi(q->corr_output_m1, SRSLTE_SSS_N);
    if (m1_value) {
      *m1_value = q->corr_output_m1[*m1];
    }
    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

/* Partial correlation SSS estimation.
 * Returns m0 and m1 estimates
 *
 * Source: "SSS Detection Method for Initial Cell Search in 3GPP LTE FDD/TDD Dual Mode Receiver"
 *       Jung-In Kim, Jung-Su Han, Hee-Jin Roh and Hyung-Jin Choi

 */
int srslte_sss_m0m1_partial(srslte_sss_t* q,
                            const cf_t*   input,
                            uint32_t      M,
                            cf_t          ce[2 * SRSLTE_SSS_N],
                            uint32_t*     m0,
                            float*        m0_value,
                            uint32_t*     m1,
                            float*        m1_value)
{

  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && input != NULL && m0 != NULL && m1 != NULL && M <= MAX_M) {
    cf_t y[2][SRSLTE_SSS_N];

    extract_pair_sss(q, input, ce, y);

    corr_all_sz_partial(y[0], q->fc_tables[q->N_id_2].s, M, q->corr_output_m0);
    *m0 = srslte_vec_max_fi(q->corr_output_m0, SRSLTE_SSS_N);
    if (m0_value) {
      *m0_value = q->corr_output_m0[*m0];
    }
    srslte_vec_prod_cfc(y[1], q->fc_tables[q->N_id_2].z1[*m0], y[1], SRSLTE_SSS_N);
    corr_all_sz_partial(y[1], q->fc_tables[q->N_id_2].s, M, q->corr_output_m1);
    *m1 = srslte_vec_max_fi(q->corr_output_m1, SRSLTE_SSS_N);
    if (m1_value) {
      *m1_value = q->corr_output_m1[*m1];
    }
    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

void convert_tables(srslte_sss_fc_tables_t* fc_tables, srslte_sss_tables_t* in)
{
  uint32_t i, j;

  for (i = 0; i < SRSLTE_SSS_N; i++) {
    for (j = 0; j < SRSLTE_SSS_N; j++) {
      fc_tables->z1[i][j] = (float)in->z1[i][j];
    }
  }
  for (i = 0; i < SRSLTE_SSS_N; i++) {
    for (j = 0; j < SRSLTE_SSS_N; j++) {
      fc_tables->s[i][j] = (float)in->s[i][j];
    }
  }
  for (i = 0; i < SRSLTE_SSS_N; i++) {
    for (j = 0; j < SRSLTE_SSS_N - 1; j++) {
      fc_tables->sd[i][j] = (float)in->s[i][j + 1] * in->s[i][j];
    }
  }
  for (i = 0; i < 2; i++) {
    for (j = 0; j < SRSLTE_SSS_N; j++) {
      fc_tables->c[i][j] = (float)in->c[i][j];
    }
  }
}
