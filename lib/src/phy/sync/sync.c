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

#include "srslte/srslte.h"
#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <strings.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/sync/cfo.h"
#include "srslte/phy/sync/sync.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#define CFO_EMA_ALPHA 0.1
#define CP_EMA_ALPHA 0.1

#define DEFAULT_CFO_TOL 0.0 // Hz

#define MAX_CFO_PSS_OFFSET 7000

static bool fft_size_isvalid(uint32_t fft_size)
{
  if (fft_size >= SRSLTE_SYNC_FFT_SZ_MIN && fft_size <= SRSLTE_SYNC_FFT_SZ_MAX && (fft_size % 64) == 0) {
    return true;
  } else {
    return false;
  }
}

int srslte_sync_init(srslte_sync_t* q, uint32_t frame_size, uint32_t max_offset, uint32_t fft_size)
{
  return srslte_sync_init_decim(q, frame_size, max_offset, fft_size, 1);
}
int srslte_sync_init_decim(srslte_sync_t* q, uint32_t frame_size, uint32_t max_offset, uint32_t fft_size, int decimate)
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && fft_size_isvalid(fft_size)) {
    ret = SRSLTE_ERROR;
    bzero(q, sizeof(srslte_sync_t));

    q->N_id_2 = 1000;
    q->N_id_1 = 1000;

    q->cfo_ema_alpha = CFO_EMA_ALPHA;
    q->sss_alg       = SSS_FULL;

    q->detect_cp             = true;
    q->sss_en                = true;
    q->cfo_pss_enable        = false;
    q->cfo_cp_enable         = false;
    q->cfo_i_initiated       = false;
    q->pss_filtering_enabled = false;
    q->detect_frame_type     = true;

    q->cfo_cp_nsymbols = 3;
    q->fft_size        = fft_size;
    q->frame_size      = frame_size;
    q->max_offset      = max_offset;
    q->max_frame_size  = frame_size;

    srslte_sync_cfo_reset(q, 0.0f);

    if (srslte_cfo_init(&q->cfo_corr_frame, q->frame_size)) {
      ERROR("Error initiating CFO\n");
      goto clean_exit;
    }

    if (srslte_cfo_init(&q->cfo_corr_symbol, q->fft_size)) {
      ERROR("Error initiating CFO\n");
      goto clean_exit;
    }

    // Set default CFO tolerance
    srslte_sync_set_cfo_tol(q, DEFAULT_CFO_TOL);

    for (int i = 0; i < 2; i++) {
      q->cfo_i_corr[i] = srslte_vec_cf_malloc(q->frame_size);
      if (!q->cfo_i_corr[i]) {
        perror("malloc");
        goto clean_exit;
      }
    }

    q->temp = srslte_vec_cf_malloc(2 * q->frame_size);
    if (!q->temp) {
      perror("malloc");
      goto clean_exit;
    }

    srslte_sync_set_cp(q, SRSLTE_CP_NORM);
    q->decimate = decimate;
    if (!decimate) {
      decimate = 1;
    }

    if (srslte_dft_plan(&q->idftp_sss, fft_size, SRSLTE_DFT_BACKWARD, SRSLTE_DFT_COMPLEX)) {
      ERROR("Error creating DFT plan \n");
      goto clean_exit;
    }
    srslte_dft_plan_set_mirror(&q->idftp_sss, true);
    srslte_dft_plan_set_dc(&q->idftp_sss, true);
    srslte_dft_plan_set_norm(&q->idftp_sss, false);

    if (srslte_pss_init_fft_offset_decim(&q->pss, max_offset, fft_size, 0, decimate)) {
      ERROR("Error initializing PSS object\n");
      goto clean_exit;
    }
    if (srslte_sss_init(&q->sss, fft_size)) {
      ERROR("Error initializing SSS object\n");
      goto clean_exit;
    }

    if (srslte_cp_synch_init(&q->cp_synch, fft_size)) {
      ERROR("Error initiating CFO\n");
      goto clean_exit;
    }

    DEBUG("SYNC init with frame_size=%d, max_offset=%d and fft_size=%d\n", frame_size, max_offset, fft_size);

    ret = SRSLTE_SUCCESS;
  } else {
    ERROR("Invalid parameters frame_size: %d, fft_size: %d\n", frame_size, fft_size);
  }

clean_exit:
  if (ret == SRSLTE_ERROR) {
    srslte_sync_free(q);
  }
  return ret;
}

void srslte_sync_free(srslte_sync_t* q)
{
  if (q) {
    srslte_pss_free(&q->pss);
    srslte_sss_free(&q->sss);
    srslte_cfo_free(&q->cfo_corr_frame);
    srslte_cfo_free(&q->cfo_corr_symbol);
    srslte_cp_synch_free(&q->cp_synch);

    srslte_dft_plan_free(&q->idftp_sss);

    for (int i = 0; i < 2; i++) {
      if (q->cfo_i_corr[i]) {
        free(q->cfo_i_corr[i]);
      }
      srslte_pss_free(&q->pss_i[i]);
    }

    if (q->temp) {
      free(q->temp);
    }
  }
}

int srslte_sync_resize(srslte_sync_t* q, uint32_t frame_size, uint32_t max_offset, uint32_t fft_size)
{

  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && fft_size_isvalid(fft_size)) {
    if (frame_size > q->max_frame_size) {
      ERROR("Error in sync_resize(): frame_size must be lower than initialized\n");
      return SRSLTE_ERROR;
    }

    q->fft_size   = fft_size;
    q->frame_size = frame_size;
    q->max_offset = max_offset;

    if (srslte_pss_resize(&q->pss, q->max_offset, q->fft_size, 0)) {
      ERROR("Error resizing PSS object\n");
      return SRSLTE_ERROR;
    }
    if (srslte_sss_resize(&q->sss, q->fft_size)) {
      ERROR("Error resizing SSS object\n");
      return SRSLTE_ERROR;
    }

    if (srslte_dft_replan(&q->idftp_sss, fft_size)) {
      ERROR("Error resizing DFT plan \n");
      return SRSLTE_ERROR;
    }

    if (srslte_cp_synch_resize(&q->cp_synch, q->fft_size)) {
      ERROR("Error resizing CFO\n");
      return SRSLTE_ERROR;
    }

    if (srslte_cfo_resize(&q->cfo_corr_frame, q->frame_size)) {
      ERROR("Error resizing CFO\n");
      return SRSLTE_ERROR;
    }

    if (srslte_cfo_resize(&q->cfo_corr_symbol, q->fft_size)) {
      ERROR("Error resizing CFO\n");
      return SRSLTE_ERROR;
    }

    if (q->cfo_i_initiated) {
      for (int i = 0; i < 2; i++) {
        int offset = (i == 0) ? -1 : 1;
        if (srslte_pss_resize(&q->pss_i[i], q->max_offset, q->fft_size, offset)) {
          ERROR("Error initializing PSS object\n");
        }
        for (int t = 0; t < q->frame_size; t++) {
          q->cfo_i_corr[i][t] = cexpf(-2 * _Complex_I * M_PI * offset * (float)t / q->fft_size);
        }
      }
    }

    // Update CFO tolerance
    srslte_sync_set_cfo_tol(q, q->current_cfo_tol);

    DEBUG("SYNC init with frame_size=%d, max_offset=%d and fft_size=%d\n", frame_size, max_offset, fft_size);

    ret = SRSLTE_SUCCESS;
  } else {
    ERROR("Invalid parameters frame_size: %d, fft_size: %d\n", frame_size, fft_size);
  }

  return ret;
}

void srslte_sync_set_frame_type(srslte_sync_t* q, srslte_frame_type_t frame_type)
{
  q->frame_type        = frame_type;
  q->detect_frame_type = false;
}

void srslte_sync_set_cfo_tol(srslte_sync_t* q, float tol)
{
  q->current_cfo_tol = tol;
  srslte_cfo_set_tol(&q->cfo_corr_frame, tol / (15000.0 * q->fft_size));
  srslte_cfo_set_tol(&q->cfo_corr_symbol, tol / (15000.0 * q->fft_size));
}

void srslte_sync_set_threshold(srslte_sync_t* q, float threshold)
{
  q->threshold = threshold;
}

void srslte_sync_sss_en(srslte_sync_t* q, bool enabled)
{
  q->sss_en = enabled;
}

bool srslte_sync_sss_detected(srslte_sync_t* q)
{
  return q->sss_detected;
}

float srslte_sync_sss_correlation_peak(srslte_sync_t* q)
{
  return q->sss_corr;
}

bool srslte_sync_sss_available(srslte_sync_t* q)
{
  return q->sss_available;
}

int srslte_sync_get_cell_id(srslte_sync_t* q)
{
  if (srslte_N_id_2_isvalid(q->N_id_2) && srslte_N_id_1_isvalid(q->N_id_1)) {
    return q->N_id_1 * 3 + q->N_id_2;
  } else {
    return -1;
  }
}

int srslte_sync_set_N_id_2(srslte_sync_t* q, uint32_t N_id_2)
{
  if (srslte_N_id_2_isvalid(N_id_2)) {
    q->N_id_2 = N_id_2;
    return SRSLTE_SUCCESS;
  } else {
    ERROR("Invalid N_id_2=%d\n", N_id_2);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

static void generate_freq_sss(srslte_sync_t* q, uint32_t N_id_1)
{
  float sf[2][SRSLTE_SSS_LEN];
  cf_t  symbol[SRSLTE_SYMBOL_SZ_MAX];

  q->N_id_1        = N_id_1;
  uint32_t cell_id = q->N_id_1 * 3 + q->N_id_2;
  srslte_sss_generate(sf[0], sf[1], cell_id);

  uint32_t k = q->fft_size / 2 - 31;

  for (int n = 0; n < 2; n++) {
    srslte_vec_cf_zero(symbol, q->fft_size);
    for (uint32_t i = 0; i < SRSLTE_SSS_LEN; i++) {
      __real__ symbol[k + i] = sf[n][i];
      __imag__ symbol[k + i] = 0;
    }
    // Get freq-domain version of the SSS signal
    srslte_dft_run_c(&q->idftp_sss, symbol, q->sss_signal[n]);
  }
  q->sss_generated = true;
  DEBUG("Generated SSS for N_id_1=%d, cell_id=%d\n", N_id_1, cell_id);
}

int srslte_sync_set_N_id_1(srslte_sync_t* q, uint32_t N_id_1)
{
  if (srslte_N_id_1_isvalid(N_id_1)) {
    generate_freq_sss(q, N_id_1);
    return SRSLTE_SUCCESS;
  } else {
    ERROR("Invalid N_id_2=%d\n", N_id_1);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

uint32_t srslte_sync_get_sf_idx(srslte_sync_t* q)
{
  return q->sf_idx;
}

float srslte_sync_get_cfo(srslte_sync_t* q)
{
  return q->cfo_cp_mean + q->cfo_pss_mean + q->cfo_i_value;
}

void srslte_sync_cfo_reset(srslte_sync_t* q, float init_cfo_hz)
{
  q->cfo_cp_mean    = init_cfo_hz / 15e3f;
  q->cfo_cp_is_set  = false;
  q->cfo_pss_mean   = 0;
  q->cfo_pss_is_set = false;
}

void srslte_sync_copy_cfo(srslte_sync_t* q, srslte_sync_t* src_obj)
{
  q->cfo_cp_mean    = src_obj->cfo_cp_mean;
  q->cfo_pss_mean   = src_obj->cfo_pss_mean;
  q->cfo_i_value    = src_obj->cfo_i_value;
  q->cfo_cp_is_set  = false;
  q->cfo_pss_is_set = false;
}

void srslte_sync_set_cfo_i_enable(srslte_sync_t* q, bool enable)
{
  q->cfo_i_enable = enable;
  if (q->cfo_i_enable && !q->cfo_i_initiated) {
    for (int i = 0; i < 2; i++) {
      int offset = (i == 0) ? -1 : 1;
      if (srslte_pss_init_fft_offset(&q->pss_i[i], q->max_offset, q->fft_size, offset)) {
        ERROR("Error initializing PSS object\n");
      }
      for (int t = 0; t < q->frame_size; t++) {
        q->cfo_i_corr[i][t] = cexpf(-2 * _Complex_I * M_PI * offset * (float)t / q->fft_size);
      }
    }
    q->cfo_i_initiated = true;
  }
}

void srslte_sync_set_sss_eq_enable(srslte_sync_t* q, bool enable)
{
  q->sss_channel_equalize = enable;
  if (enable) {
    q->pss_filtering_enabled = true;
    q->pss.chest_on_filter   = true;
  }
}

void srslte_sync_set_pss_filt_enable(srslte_sync_t* q, bool enable)
{
  q->pss_filtering_enabled = enable;
}

void srslte_sync_set_cfo_cp_enable(srslte_sync_t* q, bool enable, uint32_t nof_symbols)
{
  q->cfo_cp_enable   = enable;
  q->cfo_cp_nsymbols = nof_symbols;
}

void srslte_sync_set_cfo_pss_enable(srslte_sync_t* q, bool enable)
{
  q->cfo_pss_enable = enable;
}

void srslte_sync_set_cfo_ema_alpha(srslte_sync_t* q, float alpha)
{
  q->cfo_ema_alpha = alpha;
}

float srslte_sync_get_peak_value(srslte_sync_t* q)
{
  return q->peak_value;
}

void srslte_sync_cp_en(srslte_sync_t* q, bool enabled)
{
  q->detect_cp = enabled;
}

void srslte_sync_set_em_alpha(srslte_sync_t* q, float alpha)
{
  srslte_pss_set_ema_alpha(&q->pss, alpha);
}

srslte_cp_t srslte_sync_get_cp(srslte_sync_t* q)
{
  return q->cp;
}
void srslte_sync_set_cp(srslte_sync_t* q, srslte_cp_t cp)
{
  q->cp     = cp;
  q->cp_len = SRSLTE_CP_ISNORM(q->cp) ? SRSLTE_CP_LEN_NORM(1, q->fft_size) : SRSLTE_CP_LEN_EXT(q->fft_size);
  if (q->frame_size < q->fft_size) {
    q->nof_symbols = 1;
  } else {
    q->nof_symbols = q->frame_size / (q->fft_size + q->cp_len) - 1;
  }
}

void srslte_sync_set_sss_algorithm(srslte_sync_t* q, sss_alg_t alg)
{
  q->sss_alg = alg;
}

/* CP detection algorithm taken from:
 * "SSS Detection Method for Initial Cell Search in 3GPP LTE FDD/TDD Dual Mode Receiver"
 * by Jung-In Kim et al.
 */
srslte_cp_t srslte_sync_detect_cp(srslte_sync_t* q, const cf_t* input, uint32_t peak_pos)
{
  float R_norm = 0, R_ext = 0, C_norm = 0, C_ext = 0;
  float M_norm = 0, M_ext = 0;

  uint32_t cp_norm_len = SRSLTE_CP_LEN_NORM(7, q->fft_size);
  uint32_t cp_ext_len  = SRSLTE_CP_LEN_EXT(q->fft_size);

  uint32_t nof_symbols = peak_pos / (q->fft_size + cp_ext_len);

  if (nof_symbols > 3) {
    nof_symbols = 3;
  }

  if (nof_symbols > 0) {

    const cf_t* input_cp_norm = &input[peak_pos - nof_symbols * (q->fft_size + cp_norm_len)];
    const cf_t* input_cp_ext  = &input[peak_pos - nof_symbols * (q->fft_size + cp_ext_len)];

    for (int i = 0; i < nof_symbols; i++) {
      R_norm += crealf(srslte_vec_dot_prod_conj_ccc(&input_cp_norm[q->fft_size], input_cp_norm, cp_norm_len));
      C_norm += cp_norm_len * srslte_vec_avg_power_cf(input_cp_norm, cp_norm_len);
      input_cp_norm += q->fft_size + cp_norm_len;
    }
    if (C_norm > 0) {
      M_norm = R_norm / C_norm;
    }

    q->M_norm_avg = SRSLTE_VEC_EMA(M_norm / nof_symbols, q->M_norm_avg, CP_EMA_ALPHA);

    for (int i = 0; i < nof_symbols; i++) {
      R_ext += crealf(srslte_vec_dot_prod_conj_ccc(&input_cp_ext[q->fft_size], input_cp_ext, cp_ext_len));
      C_ext += cp_ext_len * srslte_vec_avg_power_cf(input_cp_ext, cp_ext_len);
      input_cp_ext += q->fft_size + cp_ext_len;
    }
    if (C_ext > 0) {
      M_ext = R_ext / C_ext;
    }

    q->M_ext_avg = SRSLTE_VEC_EMA(M_ext / nof_symbols, q->M_ext_avg, CP_EMA_ALPHA);

    if (q->M_norm_avg > q->M_ext_avg) {
      return SRSLTE_CP_NORM;
    } else if (q->M_norm_avg < q->M_ext_avg) {
      return SRSLTE_CP_EXT;
    } else {
      if (R_norm > R_ext) {
        return SRSLTE_CP_NORM;
      } else {
        return SRSLTE_CP_EXT;
      }
    }
  } else {
    return SRSLTE_CP_NORM;
  }
}

/* Returns 1 if the SSS is found, 0 if not and -1 if there is not enough space
 * to correlate
 */
static bool sync_sss_symbol(srslte_sync_t* q, const cf_t* input, uint32_t* sf_idx, uint32_t* N_id_1, float* corr)
{
  int ret;

  srslte_sss_set_N_id_2(&q->sss, q->N_id_2);

  // If N_Id_1 is set and SSS generated, correlate with sf0 and sf5 signals to find sf boundaries
  if (q->sss_generated) {
    bool c                 = q->pss.chest_on_filter;
    q->pss.chest_on_filter = false;
    srslte_pss_filter(&q->pss, input, q->sss_recv);
    q->pss.chest_on_filter = c;
    float res[2];
    for (int s = 0; s < 2; s++) {
      res[s] = cabsf(srslte_vec_dot_prod_conj_ccc(q->sss_signal[s], q->sss_recv, q->fft_size));
    }
    float ratio;
    if (res[0] > res[1]) {
      *sf_idx = 0;
      ratio   = res[0] / res[1];
    } else {
      *sf_idx = 5;
      ratio   = res[1] / res[0];
    }
    *N_id_1 = q->N_id_1;
    *corr   = ratio;
    DEBUG("SSS correlation with N_id_1=%d, sf0=%.2f, sf5=%.2f, sf_idx=%d, ratio=%.1f\n",
          q->N_id_1,
          res[0],
          res[1],
          *sf_idx,
          ratio);
    if (ratio > 1.2) {
      return true;
    } else {
      return false;
    }
  } else {
    switch (q->sss_alg) {
      case SSS_DIFF:
        srslte_sss_m0m1_diff(&q->sss, input, &q->m0, &q->m0_value, &q->m1, &q->m1_value);
        break;
      case SSS_PARTIAL_3:
        srslte_sss_m0m1_partial(&q->sss, input, 3, NULL, &q->m0, &q->m0_value, &q->m1, &q->m1_value);
        break;
      case SSS_FULL:
        srslte_sss_m0m1_partial(&q->sss, input, 1, NULL, &q->m0, &q->m0_value, &q->m1, &q->m1_value);
        break;
    }

    *corr   = q->m0_value + q->m1_value;
    *sf_idx = srslte_sss_subframe(q->m0, q->m1);
    ret     = srslte_sss_N_id_1(&q->sss, q->m0, q->m1, *corr);
    if (ret >= 0) {
      *N_id_1 = (uint32_t)ret;
      INFO("SSS detected N_id_1=%d (corr=%7.1f), sf_idx=%d, %s CP\n",
           *N_id_1,
           *corr,
           *sf_idx,
           SRSLTE_CP_ISNORM(q->cp) ? "Normal" : "Extended");
      return true;
    }
  }
  return false;
}

srslte_pss_t* srslte_sync_get_cur_pss_obj(srslte_sync_t* q)
{
  srslte_pss_t* pss_obj[3] = {&q->pss_i[0], &q->pss, &q->pss_i[1]};
  return pss_obj[q->cfo_i_value + 1];
}

static float cfo_cp_estimate(srslte_sync_t* q, const cf_t* input)
{
  uint32_t cp_offset = 0;
  cp_offset          = srslte_cp_synch(
      &q->cp_synch, input, q->max_offset, q->cfo_cp_nsymbols, (uint32_t)SRSLTE_CP_LEN_NORM(1, q->fft_size));
  cf_t  cp_corr_max = srslte_cp_synch_corr_output(&q->cp_synch, cp_offset);
  float cfo         = -cargf(cp_corr_max) / ((float)M_PI * 2.0f);
  return cfo;
}

static int cfo_i_estimate(srslte_sync_t* q, const cf_t* input, int find_offset, int* peak_pos, int* cfo_i)
{
  float         peak_value;
  float         max_peak_value = -99;
  int           max_cfo_i      = 0;
  srslte_pss_t* pss_obj[3]     = {&q->pss_i[0], &q->pss, &q->pss_i[1]};
  for (int cfo = 0; cfo < 3; cfo++) {
    srslte_pss_set_N_id_2(pss_obj[cfo], q->N_id_2);
    int p = srslte_pss_find_pss(pss_obj[cfo], &input[find_offset], &peak_value);
    if (p < 0) {
      return -1;
    }
    if (peak_value > max_peak_value) {
      max_peak_value = peak_value;
      if (peak_pos) {
        *peak_pos = p;
      }
      q->peak_value = peak_value;
      max_cfo_i     = cfo - 1;
    }
  }
  if (cfo_i) {
    *cfo_i = max_cfo_i;
  }
  return 0;
}

/** Finds the PSS sequence previously defined by a call to srslte_sync_set_N_id_2()
 * around the position find_offset in the buffer input.
 *
 * Returns 1 if the correlation peak exceeds the threshold set by srslte_sync_set_threshold()
 * or 0 otherwise. Returns a negative number on error (if N_id_2 has not been set)
 *
 * The input signal is not modified. Any CFO correction is done in internal buffers
 *
 * The maximum of the correlation peak is always stored in *peak_position
 */
srslte_sync_find_ret_t
srslte_sync_find(srslte_sync_t* q, const cf_t* input, uint32_t find_offset, uint32_t* peak_position)
{
  srslte_sync_find_ret_t ret      = SRSLTE_SYNC_ERROR;
  int                    peak_pos = 0;

  if (!q) {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  if (input != NULL && srslte_N_id_2_isvalid(q->N_id_2) && fft_size_isvalid(q->fft_size)) {

    q->sss_detected = false;

    if (peak_position) {
      *peak_position = 0;
    }

    const cf_t* input_ptr = input;

    /* First CFO estimation stage is integer.
     * Finds max PSS correlation for shifted +1/0/-1 integer versions.
     * This should only used once N_id_2 is set
     */
    if (q->cfo_i_enable) {
      if (cfo_i_estimate(q, input_ptr, find_offset, &peak_pos, &q->cfo_i_value) < 0) {
        ERROR("Error calling finding PSS sequence at : %d  \n", peak_pos);
        return SRSLTE_ERROR;
      }
      // Correct it using precomputed signal and store in buffer (don't modify input signal)
      if (q->cfo_i_value != 0) {
        srslte_vec_prod_ccc((cf_t*)input_ptr, q->cfo_i_corr[q->cfo_i_value < 0 ? 0 : 1], q->temp, q->frame_size);
        INFO("Compensating cfo_i=%d\n", q->cfo_i_value);
        input_ptr = q->temp;
      }
    }

    /* Second stage is coarse fractional CFO estimation using CP.
     * In case of multi-cell, this can lead to incorrect estimations if CFO from different cells is different
     */
    if (q->cfo_cp_enable) {
      float cfo_cp = cfo_cp_estimate(q, input_ptr);

      if (!q->cfo_cp_is_set) {
        q->cfo_cp_mean   = cfo_cp;
        q->cfo_cp_is_set = true;
      } else {
        /* compute exponential moving average CFO */
        q->cfo_cp_mean = SRSLTE_VEC_EMA(cfo_cp, q->cfo_cp_mean, q->cfo_ema_alpha);
      }

      DEBUG("CP-CFO: estimated=%f, mean=%f\n", cfo_cp, q->cfo_cp_mean);

      /* Correct CFO with the averaged CFO estimation */
      srslte_cfo_correct(&q->cfo_corr_frame, input_ptr, q->temp, -q->cfo_cp_mean / q->fft_size);
      input_ptr = q->temp;
    }

    /* Find maximum of PSS correlation. If Integer CFO is enabled, correlation is already done
     */
    if (!q->cfo_i_enable) {
      srslte_pss_set_N_id_2(&q->pss, q->N_id_2);
      peak_pos = srslte_pss_find_pss(&q->pss, &input_ptr[find_offset], q->threshold > 0 ? &q->peak_value : NULL);
      if (peak_pos < 0) {
        ERROR("Error calling finding PSS sequence at : %d  \n", peak_pos);
        return SRSLTE_ERROR;
      }
    }

    DEBUG("PSS: id=%d, peak_pos=%d, peak_value=%f\n", q->N_id_2, peak_pos, q->peak_value);

    // Save peak position
    if (peak_position) {
      *peak_position = (uint32_t)peak_pos;
    }

    // In case of decimation, this compensates for the constant time shift caused by the low pass filter
    if (q->decimate && peak_pos < 0) {
      peak_pos = 0; // peak_pos + q->decimate*(2);// replace 2 with q->filter_size -2;
    }

    /* If peak is over threshold, compute CFO and SSS */
    if (q->peak_value >= q->threshold || q->threshold == 0) {

      if (q->cfo_pss_enable && peak_pos >= q->fft_size) {

        // Filter central bands before PSS-based CFO estimation
        const cf_t* pss_ptr = &input_ptr[find_offset + peak_pos - q->fft_size];
        if (q->pss_filtering_enabled) {
          srslte_pss_filter(&q->pss, pss_ptr, q->pss_filt);
          pss_ptr = q->pss_filt;
        }

        // PSS-based CFO estimation
        q->cfo_pss = srslte_pss_cfo_compute(&q->pss, pss_ptr);
        if (!q->cfo_pss_is_set) {
          q->cfo_pss_mean   = q->cfo_pss;
          q->cfo_pss_is_set = true;
        } else if (15000 * fabsf(q->cfo_pss) < MAX_CFO_PSS_OFFSET) {
          q->cfo_pss_mean = SRSLTE_VEC_EMA(q->cfo_pss, q->cfo_pss_mean, q->cfo_ema_alpha);
        }

        DEBUG("PSS-CFO: filter=%s, estimated=%f, mean=%f\n",
              q->pss_filtering_enabled ? "yes" : "no",
              q->cfo_pss,
              q->cfo_pss_mean);
      }

      // If there is enough space for CP and SSS estimation
      if (peak_pos + find_offset >= 2 * (q->fft_size + SRSLTE_CP_LEN_EXT(q->fft_size))) {

        // If SSS search is enabled, correlate SSS sequence
        if (q->sss_en) {

          int                 sss_idx;
          uint32_t            nof_frame_type_trials;
          srslte_frame_type_t frame_type_trials[2];
          float               sss_corr[2] = {};
          uint32_t            sf_idx[2], N_id_1[2];

          if (q->detect_frame_type) {
            nof_frame_type_trials = 2;
            frame_type_trials[0]  = SRSLTE_FDD;
            frame_type_trials[1]  = SRSLTE_TDD;
          } else {
            frame_type_trials[0]  = q->frame_type;
            nof_frame_type_trials = 1;
          }

          q->sss_available = true;
          q->sss_detected  = false;
          for (uint32_t f = 0; f < nof_frame_type_trials; f++) {
            if (frame_type_trials[f] == SRSLTE_FDD) {
              sss_idx = (int)find_offset + peak_pos - 2 * SRSLTE_SYMBOL_SZ(q->fft_size, q->cp) +
                        SRSLTE_CP_SZ(q->fft_size, q->cp);
            } else {
              sss_idx = (int)find_offset + peak_pos - 4 * SRSLTE_SYMBOL_SZ(q->fft_size, q->cp) +
                        SRSLTE_CP_SZ(q->fft_size, q->cp);
              ;
            }

            if (sss_idx >= 0) {
              const cf_t* sss_ptr = &input_ptr[sss_idx];

              // Correct CFO if detected in PSS
              if (q->cfo_pss_enable) {
                srslte_cfo_correct(&q->cfo_corr_symbol, sss_ptr, q->sss_filt, -q->cfo_pss_mean / q->fft_size);
                // Equalize channel if estimated in PSS
                if (q->sss_channel_equalize && q->pss.chest_on_filter && q->pss_filtering_enabled) {
                  srslte_vec_prod_ccc(&q->sss_filt[q->fft_size / 2 - SRSLTE_PSS_LEN / 2],
                                      q->pss.tmp_ce,
                                      &q->sss_filt[q->fft_size / 2 - SRSLTE_PSS_LEN / 2],
                                      SRSLTE_PSS_LEN);
                }
                sss_ptr = q->sss_filt;
              }

              // Consider SSS detected if at least one trial found the SSS
              q->sss_detected |= sync_sss_symbol(q, sss_ptr, &sf_idx[f], &N_id_1[f], &sss_corr[f]);
            } else {
              q->sss_available = false;
            }
          }

          if (q->detect_frame_type) {
            if (sss_corr[0] > sss_corr[1]) {
              q->frame_type = SRSLTE_FDD;
              q->sf_idx     = sf_idx[0];
              q->N_id_1     = N_id_1[0];
              q->sss_corr   = sss_corr[0];
            } else {
              q->frame_type = SRSLTE_TDD;
              q->sf_idx     = sf_idx[1] + 1;
              q->N_id_1     = N_id_1[1];
              q->sss_corr   = sss_corr[1];
            }
            DEBUG("SYNC: Detected SSS %s, corr=%.2f/%.2f\n",
                  q->frame_type == SRSLTE_FDD ? "FDD" : "TDD",
                  sss_corr[0],
                  sss_corr[1]);
          } else if (q->sss_detected) {
            if (q->frame_type == SRSLTE_FDD) {
              q->sf_idx = sf_idx[0];
            } else {
              q->sf_idx = sf_idx[0] + 1;
            }
            q->N_id_1   = N_id_1[0];
            q->sss_corr = sss_corr[0];
          }
        }

        // Detect CP length
        if (q->detect_cp) {
          srslte_sync_set_cp(q, srslte_sync_detect_cp(q, input_ptr, peak_pos + find_offset));
        }

        ret = SRSLTE_SYNC_FOUND;
      } else {
        ret = SRSLTE_SYNC_FOUND_NOSPACE;
      }
    } else {
      ret = SRSLTE_SYNC_NOFOUND;
    }

    DEBUG("SYNC ret=%d N_id_2=%d find_offset=%d frame_len=%d, pos=%d peak=%.2f threshold=%.2f CFO=%.3f kHz\n",
          ret,
          q->N_id_2,
          find_offset,
          q->frame_size,
          peak_pos,
          q->peak_value,
          q->threshold,
          15 * (srslte_sync_get_cfo(q)));

  } else if (!srslte_N_id_2_isvalid(q->N_id_2)) {
    ERROR("Must call srslte_sync_set_N_id_2() first!\n");
  }

  return ret;
}

void srslte_sync_reset(srslte_sync_t* q)
{
  q->M_ext_avg  = 0;
  q->M_norm_avg = 0;
  srslte_pss_reset(&q->pss);
}
