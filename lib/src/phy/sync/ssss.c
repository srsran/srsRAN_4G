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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srslte/phy/sync/ssss.h"

#include "gen_sss.c"
#include "srslte/config.h"
#include "srslte/phy/dft/ofdm.h"
#include "srslte/phy/utils/vector.h"

// Generates the Sidelink sequences that are used to detect SSSS
int srslte_ssss_init(srslte_ssss_t* q, uint32_t nof_prb, srslte_cp_t cp, srslte_sl_tm_t tm)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q != NULL) {
    ret = SRSLTE_ERROR;

    uint32_t symbol_sz    = srslte_symbol_sz(nof_prb);
    int      cp_len       = SRSLTE_CP_SZ(symbol_sz, cp);
    uint32_t sf_n_re      = SRSLTE_CP_NSYMB(cp) * 12 * 2 * nof_prb;
    uint32_t sf_n_samples = symbol_sz * 15;

    uint32_t ssss_n_samples =
        (symbol_sz + cp_len) * 2 +
        cp_len; // We need an extra cp_len in order to get the peaks centered after the correlation

    // CP Normal: ssss (with its cp) starts at sample number: 11 symbol_sz + 2 CP_LEN_NORM_0 + 9 cp_len
    // CP Extended: ssss (with its cp) starts at sample number: 9 symbol_sz + 9 cp_len
    uint32_t ssss_start = (cp == SRSLTE_CP_NORM)
                              ? (symbol_sz * 11 + (SRSLTE_CP_LEN_NORM(0, symbol_sz) * 2 + cp_len * 9))
                              : ((symbol_sz + cp_len) * 9);

    for (int i = 0; i < SRSLTE_SSSS_NOF_SEQ; ++i) {
      srslte_ssss_generate(q->ssss_signal[i], i, tm);
    }

    if (srslte_dft_plan(&q->plan_input, ssss_n_samples, SRSLTE_DFT_FORWARD, SRSLTE_DFT_COMPLEX)) {
      return SRSLTE_ERROR;
    }
    srslte_dft_plan_set_mirror(&q->plan_input, true);
    srslte_dft_plan_set_norm(&q->plan_input, true);

    q->input_pad_freq = srslte_vec_cf_malloc(sf_n_re);
    if (!q->input_pad_freq) {
      return SRSLTE_ERROR;
    }
    q->input_pad_time = srslte_vec_cf_malloc(sf_n_samples);
    if (!q->input_pad_time) {
      return SRSLTE_ERROR;
    }

    srslte_ofdm_t ssss_ifft;
    if (srslte_ofdm_tx_init(&ssss_ifft, cp, q->input_pad_freq, q->input_pad_time, nof_prb)) {
      printf("Error creating iFFT object\n");
      return SRSLTE_ERROR;
    }
    srslte_ofdm_set_normalize(&ssss_ifft, true);
    srslte_ofdm_set_freq_shift(&ssss_ifft, 0.5);

    q->ssss_sf_freq = srslte_vec_malloc(sizeof(cf_t*) * SRSLTE_SSSS_NOF_SEQ);
    if (!q->ssss_sf_freq) {
      return SRSLTE_ERROR;
    }
    for (int i = 0; i < SRSLTE_SSSS_NOF_SEQ; ++i) {
      q->ssss_sf_freq[i] = srslte_vec_cf_malloc(ssss_n_samples);
      if (!q->ssss_sf_freq[i]) {
        return SRSLTE_ERROR;
      }
    }

    for (int i = 0; i < SRSLTE_SSSS_NOF_SEQ; ++i) {
      srslte_vec_cf_zero(q->input_pad_freq, ssss_n_samples);
      srslte_vec_cf_zero(q->input_pad_time, ssss_n_samples);
      srslte_vec_cf_zero(q->ssss_sf_freq[i], ssss_n_samples);

      srslte_ssss_put_sf_buffer(q->ssss_signal[i], q->input_pad_freq, nof_prb, cp);
      srslte_ofdm_tx_sf(&ssss_ifft);

      srslte_dft_run_c(&q->plan_input, &q->input_pad_time[ssss_start], q->ssss_sf_freq[i]);
      srslte_vec_conj_cc(q->ssss_sf_freq[i], q->ssss_sf_freq[i], ssss_n_samples);
    }

    srslte_ofdm_tx_free(&ssss_ifft);

    q->dot_prod_output = srslte_vec_cf_malloc(ssss_n_samples);
    if (!q->dot_prod_output) {
      return SRSLTE_ERROR;
    }

    q->dot_prod_output_time = srslte_vec_cf_malloc(ssss_n_samples);
    if (!q->dot_prod_output_time) {
      return SRSLTE_ERROR;
    }

    q->shifted_output = srslte_vec_cf_malloc(ssss_n_samples);
    if (!q->shifted_output) {
      return SRSLTE_ERROR;
    }
    memset(q->shifted_output, 0, sizeof(cf_t) * ssss_n_samples);

    q->shifted_output_abs = srslte_vec_f_malloc(ssss_n_samples);
    if (!q->shifted_output_abs) {
      return SRSLTE_ERROR;
    }
    memset(q->shifted_output_abs, 0, sizeof(float) * ssss_n_samples);

    if (srslte_dft_plan(&q->plan_out, ssss_n_samples, SRSLTE_DFT_BACKWARD, SRSLTE_DFT_COMPLEX)) {
      return SRSLTE_ERROR;
    }
    srslte_dft_plan_set_dc(&q->plan_out, true);
    srslte_dft_plan_set_norm(&q->plan_out, true);
    //    srslte_dft_plan_set_mirror(&q->plan_out, true);

    ret = SRSLTE_SUCCESS;
  }
  return ret;
}

/**
 * Reference: 3GPP TS 36.211 version 15.6.0 Release 15 Sec. 9.7.2.1
 */
void srslte_ssss_generate(float* ssss_signal, uint32_t N_sl_id, srslte_sl_tm_t tm)
{
  uint32_t id1 = N_sl_id % 168;
  uint32_t id2 = N_sl_id / 168;
  uint32_t m0;
  uint32_t m1;
  int      s_t[SRSLTE_SSSS_N], c_t[SRSLTE_SSSS_N], z_t[SRSLTE_SSSS_N];
  int      s0[SRSLTE_SSSS_N], s1[SRSLTE_SSSS_N], c0[SRSLTE_SSSS_N], c1[SRSLTE_SSSS_N], z1_0[SRSLTE_SSSS_N],
      z1_1[SRSLTE_SSSS_N];

  generate_m0m1(id1, &m0, &m1);
  generate_zsc_tilde(z_t, s_t, c_t);

  generate_s(s0, s_t, m0);
  generate_s(s1, s_t, m1);

  generate_c(c0, c_t, id2, 0);
  generate_c(c1, c_t, id2, 1);

  generate_z(z1_0, z_t, m0);
  generate_z(z1_1, z_t, m1);

  // Transmission mode 1 and 2 uses only "subframe 0 sequence" in two symbols on subframe 0 depending on cp
  // Transmission mode 3 and 4 uses only "subframe 5 sequence" in two symbols on subframe 0 depending on cp
  if (tm == SRSLTE_SIDELINK_TM1 || tm == SRSLTE_SIDELINK_TM2) {
    for (uint32_t i = 0; i < SRSLTE_SSSS_N; i++) {
      ssss_signal[2 * i]     = (float)(s0[i] * c0[i]);
      ssss_signal[2 * i + 1] = (float)(s1[i] * c1[i] * z1_0[i]);
    }
  } else if (tm == SRSLTE_SIDELINK_TM3 || tm == SRSLTE_SIDELINK_TM4) {
    for (uint32_t i = 0; i < SRSLTE_SSSS_N; i++) {
      ssss_signal[2 * i]     = (float)(s1[i] * c0[i]);
      ssss_signal[2 * i + 1] = (float)(s0[i] * c1[i] * z1_1[i]);
    }
  }
}

/**
 * Mapping SSSS to resource elements
 * Reference: 3GPP TS 36.211 version 15.6.0 Release 15 Sec. 9.7.2.2
 */
void srslte_ssss_put_sf_buffer(float* ssss_signal, cf_t* sf_buffer, uint32_t nof_prb, srslte_cp_t cp)
{
  // Normal cycle prefix l = 4,5
  // Extended cycle prefix l = 3,4
  uint32_t slot1_pos = SRSLTE_CP_NSYMB(cp) * SRSLTE_NRE * nof_prb;
  for (int i = 0; i < 2; i++) {
    uint32_t k = (SRSLTE_CP_NSYMB(cp) - 3 + i) * nof_prb * SRSLTE_NRE + nof_prb * SRSLTE_NRE / 2 - 31;
    srslte_vec_cf_zero(&sf_buffer[slot1_pos + k - 5], 5);
    for (int j = 0; j < SRSLTE_SSSS_LEN; j++) {
      __real__ sf_buffer[slot1_pos + k + j] = ssss_signal[j];
      __imag__ sf_buffer[slot1_pos + k + j] = 0;
    }
    srslte_vec_cf_zero(&sf_buffer[slot1_pos + k + SRSLTE_SSSS_LEN], 5);
  }
}

/**
 * Performs SSSS correlation.
 * Returns the index of the SSSS correlation peak in a subframe.
 * The subframe starts at corr_peak_pos-subframe_size.
 * The value of the correlation is stored in corr_peak_value.
 *
 */
int srslte_ssss_find(srslte_ssss_t* q, cf_t* input, uint32_t nof_prb, uint32_t N_id_2, srslte_cp_t cp)
{
  // One array for each sequence
  float    corr_peak_value[SRSLTE_SSSS_NOF_SEQ] = {};
  uint32_t corr_peak_pos[SRSLTE_SSSS_NOF_SEQ]   = {};

  uint32_t symbol_sz = srslte_symbol_sz(nof_prb);
  int      cp_len    = SRSLTE_CP_SZ(symbol_sz, cp);

  uint32_t ssss_n_samples = (symbol_sz + cp_len) * 2 +
                            cp_len; // We need an extra cp_len in order to get the peaks centered after the correlation

  srslte_vec_cf_zero(q->input_pad_freq, ssss_n_samples);
  srslte_vec_cf_zero(q->input_pad_time, ssss_n_samples);

  // CP Normal: ssss (with its cp) starts at sample number: 11 symbol_sz + 2 CP_LEN_NORM_0 + 9 cp_len
  // CP Extended: ssss (with its cp) starts at sample number: 9 symbol_sz + 9 cp_len
  uint32_t ssss_start = (cp == SRSLTE_CP_NORM) ? (symbol_sz * 11 + (SRSLTE_CP_LEN_NORM(0, symbol_sz) * 2 + cp_len * 9))
                                               : ((symbol_sz + cp_len) * 9);

  memcpy(q->input_pad_time, &input[ssss_start], sizeof(cf_t) * ssss_n_samples);

  srslte_dft_run_c(&q->plan_input, q->input_pad_time, q->input_pad_freq);

  // 0-167 or 168-335
  for (int i = (168 * N_id_2); i < (168 * (N_id_2 + 1)); i++) {
    srslte_vec_prod_ccc(q->ssss_sf_freq[i], q->input_pad_freq, q->dot_prod_output, ssss_n_samples);

    srslte_dft_run_c(&q->plan_out, q->dot_prod_output, q->dot_prod_output_time);

    memcpy(q->shifted_output, &q->dot_prod_output_time[ssss_n_samples / 2], sizeof(cf_t) * ssss_n_samples / 2);
    memcpy(&q->shifted_output[ssss_n_samples / 2], q->dot_prod_output_time, sizeof(cf_t) * ssss_n_samples / 2);

    q->corr_peak_pos = -1;
    srslte_vec_abs_square_cf(q->shifted_output, q->shifted_output_abs, ssss_n_samples);

    // Correlation output peaks:
    //
    //       |
    //       |                              |
    //       |                              |
    //       |                 |            |            |
    //       |...______________|____________|____________|______________... t (samples)
    //                     side peak    main peak    side peak

    // Find the main peak
    q->corr_peak_pos   = srslte_vec_max_fi(q->shifted_output_abs, ssss_n_samples);
    q->corr_peak_value = q->shifted_output_abs[q->corr_peak_pos];
    if ((q->corr_peak_pos < ssss_n_samples / 2) || (q->corr_peak_pos > ssss_n_samples / 2)) {
      q->corr_peak_pos = -1;
      continue;
    }
    srslte_vec_f_zero(&q->shifted_output_abs[q->corr_peak_pos - (symbol_sz / 2)], symbol_sz);

    // Find the first side peak
    uint32_t peak_1_pos   = srslte_vec_max_fi(q->shifted_output_abs, ssss_n_samples);
    float    peak_1_value = q->shifted_output_abs[peak_1_pos];
    if ((peak_1_pos >= (q->corr_peak_pos - (symbol_sz + cp_len) - 2)) &&
        (peak_1_pos <= (q->corr_peak_pos - (symbol_sz + cp_len) + 2))) {
    } else if ((peak_1_pos >= (q->corr_peak_pos + (symbol_sz + cp_len) - 2)) &&
               (peak_1_pos <= (q->corr_peak_pos + (symbol_sz + cp_len) + 2))) {
    } else {
      q->corr_peak_pos = -1;
      continue;
    }
    srslte_vec_f_zero(&q->shifted_output_abs[peak_1_pos - ((cp_len - 2) / 2)], cp_len - 2);

    // Find the second side peak
    uint32_t peak_2_pos   = srslte_vec_max_fi(q->shifted_output_abs, ssss_n_samples);
    float    peak_2_value = q->shifted_output_abs[peak_2_pos];
    if ((peak_2_pos >= (q->corr_peak_pos - (symbol_sz + cp_len) - 2)) &&
        (peak_2_pos <= (q->corr_peak_pos - (symbol_sz + cp_len) + 2))) {
    } else if ((peak_2_pos >= (q->corr_peak_pos + (symbol_sz + cp_len) - 2)) &&
               (peak_2_pos <= (q->corr_peak_pos + (symbol_sz + cp_len) + 2))) {
    } else {
      q->corr_peak_pos = -1;
      continue;
    }
    srslte_vec_f_zero(&q->shifted_output_abs[peak_2_pos - ((cp_len - 2) / 2)], cp_len - 2);

    float threshold_above = q->corr_peak_value / 2.0 * 1.6;
    if ((peak_1_value > threshold_above) || (peak_2_value > threshold_above)) {
      q->corr_peak_pos = -1;
      continue;
    }

    float threshold_below = q->corr_peak_value / 2.0 * 0.4;
    if ((peak_1_value < threshold_below) || (peak_2_value < threshold_below)) {
      q->corr_peak_pos = -1;
      continue;
    }

    corr_peak_value[i] = q->corr_peak_value;
    corr_peak_pos[i]   = q->corr_peak_pos;
  }

  q->N_sl_id = srslte_vec_max_fi(corr_peak_value, SRSLTE_SSSS_NOF_SEQ);
  if (corr_peak_value[q->N_sl_id] == 0.0) {
    return SRSLTE_ERROR;
  }

  q->corr_peak_pos   = corr_peak_pos[q->N_sl_id];
  q->corr_peak_value = corr_peak_value[q->N_sl_id];

  return SRSLTE_SUCCESS;
}

void srslte_ssss_free(srslte_ssss_t* q)
{
  if (q) {
    srslte_dft_plan_free(&q->plan_input);
    srslte_dft_plan_free(&q->plan_out);
    if (q->shifted_output) {
      free(q->shifted_output);
    }
    if (q->shifted_output_abs) {
      free(q->shifted_output_abs);
    }
    if (q->dot_prod_output) {
      free(q->dot_prod_output);
    }
    if (q->dot_prod_output_time) {
      free(q->dot_prod_output_time);
    }
    if (q->input_pad_freq) {
      free(q->input_pad_freq);
    }
    if (q->input_pad_time) {
      free(q->input_pad_time);
    }
    if (q->ssss_sf_freq) {
      for (int N_sl_id = 0; N_sl_id < SRSLTE_SSSS_NOF_SEQ; ++N_sl_id) {
        if (q->ssss_sf_freq[N_sl_id]) {
          free(q->ssss_sf_freq[N_sl_id]);
        }
      }
      free(q->ssss_sf_freq);
    }

    bzero(q, sizeof(srslte_ssss_t));
  }
}