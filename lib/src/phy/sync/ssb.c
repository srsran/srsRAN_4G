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

#include "srsran/phy/sync/ssb.h"
#include "srsran/phy/ch_estimation/dmrs_pbch.h"
#include "srsran/phy/sync/pss_nr.h"
#include "srsran/phy/sync/sss_nr.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"
#include <complex.h>

/*
 * Maximum allowed maximum sampling rate error in Hz
 */
#define SSB_SRATE_MAX_ERROR_HZ 0.01

/*
 * Maximum allowed maximum frequency error offset in Hz
 */
#define SSB_FREQ_OFFSET_MAX_ERROR_HZ 0.01

/*
 * Correlation size in function of the symbol size. It selects a power of two number at least 8 times bigger than the
 * given symbol size but not bigger than 2^13 points.
 */
#define SSB_CORR_SZ(SYMB_SZ) SRSRAN_MIN(1U << (uint32_t)ceil(log2((double)(SYMB_SZ)) + 3.0), 1U << 13U)

/*
 * Default NR-PBCH DMRS normalised correlation (RSRP/EPRE) threshold
 */
#define SSB_PBCH_DMRS_DEFAULT_CORR_THR 0.5f

static int ssb_init_corr(srsran_ssb_t* q)
{
  // Initialise correlation only if it is enabled
  if (!q->args.enable_search) {
    return SRSRAN_SUCCESS;
  }

  // For each PSS sequence allocate
  for (uint32_t N_id_2 = 0; N_id_2 < SRSRAN_NOF_NID_2_NR; N_id_2++) {
    // Allocate sequences
    q->pss_seq[N_id_2] = srsran_vec_cf_malloc(q->max_corr_sz);
    if (q->pss_seq[N_id_2] == NULL) {
      ERROR("Malloc");
      return SRSRAN_ERROR;
    }
  }

  q->sf_buffer = srsran_vec_cf_malloc(q->max_ssb_sz + q->max_sf_sz);
  if (q->sf_buffer == NULL) {
    ERROR("Malloc");
    return SRSRAN_ERROR;
  }
  srsran_vec_cf_zero(q->sf_buffer, q->max_ssb_sz + q->max_sf_sz);

  return SRSRAN_SUCCESS;
}

static int ssb_init_pbch(srsran_ssb_t* q)
{
  srsran_pbch_nr_args_t args = {};
  args.enable_encode         = q->args.enable_encode;
  args.enable_decode         = q->args.enable_decode;
  args.disable_simd          = q->args.disable_polar_simd;

  if (!args.enable_encode && !args.enable_decode) {
    return SRSRAN_SUCCESS;
  }

  if (srsran_pbch_nr_init(&q->pbch, &args) < SRSRAN_SUCCESS) {
    ERROR("Error init NR PBCH");
    return SRSRAN_SUCCESS;
  }

  return SRSRAN_SUCCESS;
}

int srsran_ssb_init(srsran_ssb_t* q, const srsran_ssb_args_t* args)
{
  // Verify input parameters
  if (q == NULL || args == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Copy arguments
  q->args = *args;

  // Check if the maximum sampling rate is in range, force default otherwise
  q->args.max_srate_hz  = (!isnormal(q->args.max_srate_hz)) ? SRSRAN_SSB_DEFAULT_MAX_SRATE_HZ : q->args.max_srate_hz;
  q->args.pbch_dmrs_thr = (!isnormal(q->args.pbch_dmrs_thr)) ? SSB_PBCH_DMRS_DEFAULT_CORR_THR : q->args.pbch_dmrs_thr;

  q->scs_hz        = (float)SRSRAN_SUBC_SPACING_NR(q->args.min_scs);
  q->max_sf_sz     = (uint32_t)round(1e-3 * q->args.max_srate_hz);
  q->max_symbol_sz = (uint32_t)round(q->args.max_srate_hz / q->scs_hz);
  q->max_corr_sz   = SSB_CORR_SZ(q->max_symbol_sz);
  q->max_ssb_sz    = SRSRAN_SSB_DURATION_NSYMB * (q->max_symbol_sz + (144 * q->max_symbol_sz) / 2048);

  // Allocate temporal data
  q->tmp_time = srsran_vec_cf_malloc(q->max_corr_sz);
  q->tmp_freq = srsran_vec_cf_malloc(q->max_corr_sz);
  q->tmp_corr = srsran_vec_cf_malloc(q->max_corr_sz);
  if (q->tmp_time == NULL || q->tmp_freq == NULL || q->tmp_corr == NULL) {
    ERROR("Malloc");
    return SRSRAN_ERROR;
  }

  // Allocate correlation buffers
  if (ssb_init_corr(q) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // PBCH
  if (ssb_init_pbch(q) < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

void srsran_ssb_free(srsran_ssb_t* q)
{
  if (q == NULL) {
    return;
  }

  if (q->tmp_time != NULL) {
    free(q->tmp_time);
  }

  if (q->tmp_freq != NULL) {
    free(q->tmp_freq);
  }

  if (q->tmp_corr != NULL) {
    free(q->tmp_corr);
  }

  // For each PSS sequence allocate
  for (uint32_t N_id_2 = 0; N_id_2 < SRSRAN_NOF_NID_2_NR; N_id_2++) {
    if (q->pss_seq[N_id_2] != NULL) {
      free(q->pss_seq[N_id_2]);
    }
  }

  if (q->sf_buffer != NULL) {
    free(q->sf_buffer);
  }

  srsran_dft_plan_free(&q->ifft);
  srsran_dft_plan_free(&q->fft);
  srsran_dft_plan_free(&q->fft_corr);
  srsran_dft_plan_free(&q->ifft_corr);
  srsran_pbch_nr_free(&q->pbch);

  SRSRAN_MEM_ZERO(q, srsran_ssb_t, 1);
}

static uint32_t ssb_first_symbol_caseA(const srsran_ssb_cfg_t* cfg, uint32_t indexes[SRSRAN_SSB_NOF_CANDIDATES])
{
  // Case A - 15 kHz SCS: the first symbols of the candidate SS/PBCH blocks have indexes of { 2 , 8 } + 14 ⋅ n . For
  // carrier frequencies smaller than or equal to 3 GHz, n = 0 , 1 . For carrier frequencies within FR1 larger than 3
  // GHz, n = 0 , 1 , 2 , 3 .
  uint32_t count           = 0;
  uint32_t base_indexes[2] = {2, 8};

  uint32_t N = 2;
  if (cfg->center_freq_hz > 3e9) {
    N = 4;
  }

  for (uint32_t n = 0; n < N; n++) {
    for (uint32_t i = 0; i < 2; i++) {
      indexes[count++] = base_indexes[i] + 14 * n;
    }
  }

  return count;
}

static uint32_t ssb_first_symbol_caseB(const srsran_ssb_cfg_t* cfg, uint32_t indexes[SRSRAN_SSB_NOF_CANDIDATES])
{
  // Case B - 30 kHz SCS: the first symbols of the candidate SS/PBCH blocks have indexes { 4 , 8 , 16 , 20 } + 28 ⋅ n .
  // For carrier frequencies smaller than or equal to 3 GHz, n = 0 . For carrier frequencies within FR1 larger than 3
  // GHz, n = 0 , 1 .
  uint32_t count           = 0;
  uint32_t base_indexes[4] = {4, 8, 16, 20};

  uint32_t N = 1;
  if (cfg->center_freq_hz > 3e9) {
    N = 2;
  }

  for (uint32_t n = 0; n < N; n++) {
    for (uint32_t i = 0; i < 4; i++) {
      indexes[count++] = base_indexes[i] + 28 * n;
    }
  }

  return count;
}

static uint32_t ssb_first_symbol_caseC(const srsran_ssb_cfg_t* cfg, uint32_t indexes[SRSRAN_SSB_NOF_CANDIDATES])
{
  // Case C - 30 kHz SCS: the first symbols of the candidate SS/PBCH blocks have indexes { 2 , 8 } +14 ⋅ n .
  // - For paired spectrum operation
  //   - For carrier frequencies smaller than or equal to 3 GHz, n = 0 , 1 . For carrier frequencies within FR1 larger
  //     than 3 GHz, n = 0 , 1 , 2 , 3 .
  // - For unpaired spectrum operation
  //   - For carrier frequencies smaller than or equal to 2.3 GHz, n = 0 , 1 . For carrier frequencies within FR1
  //     larger than 2.3 GHz, n = 0 , 1 , 2 , 3 .
  uint32_t count           = 0;
  uint32_t base_indexes[2] = {2, 8};

  uint32_t N = 4;
  if ((cfg->duplex_mode == SRSRAN_DUPLEX_MODE_FDD && cfg->center_freq_hz <= 3e9) ||
      (cfg->duplex_mode == SRSRAN_DUPLEX_MODE_TDD && cfg->center_freq_hz <= 2.3e9)) {
    N = 2;
  }

  for (uint32_t n = 0; n < N; n++) {
    for (uint32_t i = 0; i < 2; i++) {
      indexes[count++] = base_indexes[i] + 14 * n;
    }
  }

  return count;
}

static uint32_t ssb_first_symbol_caseD(const srsran_ssb_cfg_t* cfg, uint32_t indexes[SRSRAN_SSB_NOF_CANDIDATES])
{
  // Case D - 120 kHz SCS: the first symbols of the candidate SS/PBCH blocks have indexes { 4 , 8 , 16 , 20 } + 28 ⋅ n .
  // For carrier frequencies within FR2, n = 0 , 1 , 2 , 3 , 5 , 6 , 7 , 8 , 10 , 11 , 12 , 13 , 15 , 16 , 17 , 18 .
  uint32_t count           = 0;
  uint32_t base_indexes[4] = {4, 8, 16, 20};
  uint32_t n_indexes[16]   = {0, 1, 2, 3, 5, 6, 7, 8, 10, 11, 12, 13, 15, 16, 17, 18};

  for (uint32_t j = 0; j < 16; j++) {
    for (uint32_t i = 0; i < 4; i++) {
      indexes[count++] = base_indexes[i] + 28 * n_indexes[j];
    }
  }

  return count;
}

static uint32_t ssb_first_symbol_caseE(const srsran_ssb_cfg_t* cfg, uint32_t indexes[SRSRAN_SSB_NOF_CANDIDATES])
{
  // Case E - 240 kHz SCS: the first symbols of the candidate SS/PBCH blocks have indexes
  //{ 8 , 12 , 16 , 20 , 32 , 36 , 40 , 44 } + 56 ⋅ n . For carrier frequencies within FR2, n = 0 , 1 , 2 , 3 , 5 , 6 ,
  // 7 , 8 .
  uint32_t count           = 0;
  uint32_t base_indexes[8] = {8, 12, 16, 20, 32, 38, 40, 44};
  uint32_t n_indexes[8]    = {0, 1, 2, 3, 5, 6, 7, 8};

  for (uint32_t j = 0; j < 8; j++) {
    for (uint32_t i = 0; i < 8; i++) {
      indexes[count++] = base_indexes[i] + 56 * n_indexes[j];
    }
  }

  return count;
}

static uint32_t ssb_first_symbol(const srsran_ssb_cfg_t* cfg, uint32_t indexes[SRSRAN_SSB_NOF_CANDIDATES])
{
  uint32_t Lmax = 0;

  switch (cfg->pattern) {
    case SRSRAN_SSB_PATTERN_A:
      Lmax = ssb_first_symbol_caseA(cfg, indexes);
      break;
    case SRSRAN_SSB_PATTERN_B:
      Lmax = ssb_first_symbol_caseB(cfg, indexes);
      break;
    case SRSRAN_SSB_PATTERN_C:
      Lmax = ssb_first_symbol_caseC(cfg, indexes);
      break;
    case SRSRAN_SSB_PATTERN_D:
      Lmax = ssb_first_symbol_caseD(cfg, indexes);
      break;
    case SRSRAN_SSB_PATTERN_E:
      Lmax = ssb_first_symbol_caseE(cfg, indexes);
      break;
    case SRSRAN_SSB_PATTERN_INVALID:
      ERROR("Invalid case");
  }
  return Lmax;
}

// Modulates a given symbol l and stores the time domain signal in q->tmp_time
static void ssb_modulate_symbol(srsran_ssb_t* q, cf_t ssb_grid[SRSRAN_SSB_NOF_RE], uint32_t l)
{
  // Select symbol in grid
  cf_t* ptr = &ssb_grid[l * SRSRAN_SSB_BW_SUBC];

  // Initialise frequency domain
  srsran_vec_cf_zero(q->tmp_freq, q->symbol_sz);

  // Map grid into frequency domain symbol
  if (q->f_offset >= SRSRAN_SSB_BW_SUBC / 2) {
    srsran_vec_cf_copy(&q->tmp_freq[q->f_offset - SRSRAN_SSB_BW_SUBC / 2], ptr, SRSRAN_SSB_BW_SUBC);
  } else if (q->f_offset <= -SRSRAN_SSB_BW_SUBC / 2) {
    srsran_vec_cf_copy(&q->tmp_freq[q->symbol_sz + q->f_offset - SRSRAN_SSB_BW_SUBC / 2], ptr, SRSRAN_SSB_BW_SUBC);
  } else {
    srsran_vec_cf_copy(
        &q->tmp_freq[0], &ptr[SRSRAN_SSB_BW_SUBC / 2 - q->f_offset], SRSRAN_SSB_BW_SUBC / 2 + q->f_offset);
    srsran_vec_cf_copy(&q->tmp_freq[q->symbol_sz - SRSRAN_SSB_BW_SUBC / 2 + q->f_offset],
                       &ptr[0],
                       SRSRAN_SSB_BW_SUBC / 2 - q->f_offset);
  }

  // Convert to time domain
  srsran_dft_run_guru_c(&q->ifft);

  // Normalise output
  float norm = sqrtf((float)q->symbol_sz);
  if (isnormal(norm)) {
    srsran_vec_sc_prod_cfc(q->tmp_time, 1.0f / norm, q->tmp_time, q->symbol_sz);
  }
}

static int ssb_setup_corr(srsran_ssb_t* q)
{
  // Skip if disabled
  if (!q->args.enable_search) {
    return SRSRAN_SUCCESS;
  }

  // Compute new correlation size
  uint32_t corr_sz = SSB_CORR_SZ(q->symbol_sz);

  // Skip if the symbol size is unchanged
  if (q->corr_sz == corr_sz) {
    return SRSRAN_SUCCESS;
  }
  q->corr_sz = corr_sz;

  // Select correlation window, return error if the correlation window is smaller than a symbol
  if (corr_sz < 2 * q->symbol_sz) {
    ERROR("Correlation size (%d) is not sufficient (min. %d)", corr_sz, q->symbol_sz * 2);
    return SRSRAN_ERROR;
  }
  q->corr_window = corr_sz - q->symbol_sz;

  // Free correlation
  srsran_dft_plan_free(&q->fft_corr);
  srsran_dft_plan_free(&q->ifft_corr);

  // Prepare correlation FFT
  if (srsran_dft_plan_guru_c(&q->fft_corr, (int)corr_sz, SRSRAN_DFT_FORWARD, q->tmp_time, q->tmp_freq, 1, 1, 1, 1, 1) <
      SRSRAN_SUCCESS) {
    ERROR("Error planning correlation DFT");
    return SRSRAN_ERROR;
  }
  if (srsran_dft_plan_guru_c(
          &q->ifft_corr, (int)corr_sz, SRSRAN_DFT_BACKWARD, q->tmp_corr, q->tmp_time, 1, 1, 1, 1, 1) < SRSRAN_SUCCESS) {
    ERROR("Error planning correlation DFT");
    return SRSRAN_ERROR;
  }

  // Zero the time domain signal last samples
  srsran_vec_cf_zero(&q->tmp_time[q->symbol_sz], q->corr_window);

  // Temporal grid
  cf_t ssb_grid[SRSRAN_SSB_NOF_RE] = {};

  // Initialise correlation sequence
  for (uint32_t N_id_2 = 0; N_id_2 < SRSRAN_NOF_NID_2_NR; N_id_2++) {
    // Put the PSS in SSB grid
    if (srsran_pss_nr_put(ssb_grid, N_id_2, 1.0f) < SRSRAN_SUCCESS) {
      ERROR("Error putting PDD N_id_2=%d", N_id_2);
      return SRSRAN_ERROR;
    }

    // Modulate symbol with PSS
    ssb_modulate_symbol(q, ssb_grid, SRSRAN_PSS_NR_SYMBOL_IDX);

    // Convert to frequency domain
    srsran_dft_run_guru_c(&q->fft_corr);

    // Copy frequency domain sequence
    srsran_vec_cf_copy(q->pss_seq[N_id_2], q->tmp_freq, q->corr_sz);
  }

  return SRSRAN_SUCCESS;
}

static inline int ssb_get_t_offset(srsran_ssb_t* q, uint32_t ssb_idx)
{
  // Get baseband time offset from the begining of the half radio frame to the first symbol
  if (ssb_idx >= SRSRAN_SSB_NOF_CANDIDATES) {
    ERROR("Invalid SSB candidate index (%d)", ssb_idx);
    return SRSRAN_ERROR;
  }

  float t_offset_s = srsran_symbol_offset_s(q->l_first[ssb_idx], q->cfg.scs);
  if (isnan(t_offset_s) || isinf(t_offset_s) || t_offset_s < 0.0f) {
    ERROR("Invalid first symbol (l_first=%d)", q->l_first[ssb_idx]);
    return SRSRAN_ERROR;
  }

  return (int)round(t_offset_s * q->cfg.srate_hz);
}

int srsran_ssb_set_cfg(srsran_ssb_t* q, const srsran_ssb_cfg_t* cfg)
{
  // Verify input parameters
  if (q == NULL || cfg == NULL || cfg->pattern == SRSRAN_SSB_PATTERN_INVALID ||
      cfg->duplex_mode == SRSRAN_DUPLEX_MODE_INVALID) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Calculate subcarrier spacing in Hz
  q->scs_hz = (float)SRSRAN_SUBC_SPACING_NR(cfg->scs);

  // Get first symbol
  q->Lmax = ssb_first_symbol(cfg, q->l_first);

  // Calculate SSB symbol size and integer frequency offset
  double   freq_offset_hz = cfg->ssb_freq_hz - cfg->center_freq_hz;
  uint32_t symbol_sz      = (uint32_t)round(cfg->srate_hz / q->scs_hz);
  q->f_offset             = (int32_t)round(freq_offset_hz / q->scs_hz);

  // Calculate cyclic prefix
  q->cp_sz = (144U * symbol_sz) / 2048U;

  // Calculate SSB sampling error and check
  double ssb_srate_error_Hz = ((double)symbol_sz * q->scs_hz) - cfg->srate_hz;
  if (fabs(ssb_srate_error_Hz) > SSB_SRATE_MAX_ERROR_HZ) {
    ERROR("Invalid sampling rate (%.2f MHz)", cfg->srate_hz / 1e6);
    return SRSRAN_ERROR;
  }

  // Calculate SSB offset error and check
  double ssb_offset_error_Hz = ((double)q->f_offset * q->scs_hz) - freq_offset_hz;
  if (fabs(ssb_offset_error_Hz) > SSB_FREQ_OFFSET_MAX_ERROR_HZ) {
    ERROR("SSB Offset (%.1f kHz) error exceeds maximum allowed", freq_offset_hz / 1e3);
    return SRSRAN_ERROR;
  }

  // Verify symbol size
  if (q->max_symbol_sz < symbol_sz) {
    ERROR("New symbol size (%d) exceeds maximum symbol size (%d)", symbol_sz, q->max_symbol_sz);
    return SRSRAN_ERROR;
  }

  // Replan iFFT
  if ((q->args.enable_encode || q->args.enable_search) && q->symbol_sz != symbol_sz) {
    // free the current IFFT, it internally checks if the plan was created
    srsran_dft_plan_free(&q->ifft);

    // Creates DFT plan
    if (srsran_dft_plan_guru_c(&q->ifft, (int)symbol_sz, SRSRAN_DFT_BACKWARD, q->tmp_freq, q->tmp_time, 1, 1, 1, 1, 1) <
        SRSRAN_SUCCESS) {
      ERROR("Error creating iDFT");
      return SRSRAN_ERROR;
    }
  }

  // Replan FFT
  if ((q->args.enable_measure || q->args.enable_decode || q->args.enable_search) && q->symbol_sz != symbol_sz) {
    // free the current FFT, it internally checks if the plan was created
    srsran_dft_plan_free(&q->fft);

    // Creates DFT plan
    if (srsran_dft_plan_guru_c(&q->fft, (int)symbol_sz, SRSRAN_DFT_FORWARD, q->tmp_time, q->tmp_freq, 1, 1, 1, 1, 1) <
        SRSRAN_SUCCESS) {
      ERROR("Error creating iDFT");
      return SRSRAN_ERROR;
    }
  }

  // Finally, copy configuration
  q->cfg       = *cfg;
  q->symbol_sz = symbol_sz;
  q->sf_sz     = (uint32_t)round(1e-3 * cfg->srate_hz);
  q->ssb_sz    = SRSRAN_SSB_DURATION_NSYMB * (q->symbol_sz + q->cp_sz);

  // Initialise correlation
  if (ssb_setup_corr(q) < SRSRAN_SUCCESS) {
    ERROR("Error initialising correlation");
    return SRSRAN_ERROR;
  }

  if (!isnormal(q->cfg.beta_pss)) {
    q->cfg.beta_pss = SRSRAN_SSB_DEFAULT_BETA;
  }

  if (!isnormal(q->cfg.beta_sss)) {
    q->cfg.beta_sss = SRSRAN_SSB_DEFAULT_BETA;
  }

  if (!isnormal(q->cfg.beta_pbch)) {
    q->cfg.beta_pbch = SRSRAN_SSB_DEFAULT_BETA;
  }

  if (!isnormal(q->cfg.beta_pbch_dmrs)) {
    q->cfg.beta_pbch_dmrs = SRSRAN_SSB_DEFAULT_BETA;
  }

  return SRSRAN_SUCCESS;
}

bool srsran_ssb_send(srsran_ssb_t* q, uint32_t sf_idx)
{
  // Verify input
  if (q == NULL) {
    return false;
  }

  // Verify periodicity
  if (q->cfg.periodicity_ms == 0) {
    return false;
  }

  // Check periodicity
  return (sf_idx % q->cfg.periodicity_ms == 0);
}

static int ssb_encode(srsran_ssb_t* q, uint32_t N_id, const srsran_pbch_msg_nr_t* msg, cf_t ssb_grid[SRSRAN_SSB_NOF_RE])
{
  uint32_t N_id_1 = SRSRAN_NID_1_NR(N_id);
  uint32_t N_id_2 = SRSRAN_NID_2_NR(N_id);

  // Put PSS
  if (srsran_pss_nr_put(ssb_grid, N_id_2, q->cfg.beta_pss) < SRSRAN_SUCCESS) {
    ERROR("Error putting PSS");
    return SRSRAN_ERROR;
  }

  // Put SSS
  if (srsran_sss_nr_put(ssb_grid, N_id_1, N_id_2, q->cfg.beta_sss) < SRSRAN_SUCCESS) {
    ERROR("Error putting PSS");
    return SRSRAN_ERROR;
  }

  // Put PBCH DMRS
  srsran_dmrs_pbch_cfg_t pbch_dmrs_cfg = {};
  pbch_dmrs_cfg.N_id                   = N_id;
  pbch_dmrs_cfg.n_hf                   = msg->hrf ? 1 : 0;
  pbch_dmrs_cfg.ssb_idx                = msg->ssb_idx;
  pbch_dmrs_cfg.L_max                  = q->Lmax;
  pbch_dmrs_cfg.beta                   = 0.0f;
  pbch_dmrs_cfg.scs                    = q->cfg.scs;
  if (srsran_dmrs_pbch_put(&pbch_dmrs_cfg, ssb_grid) < SRSRAN_SUCCESS) {
    ERROR("Error putting PBCH DMRS");
    return SRSRAN_ERROR;
  }

  // Put PBCH payload
  srsran_pbch_nr_cfg_t pbch_cfg = {};
  pbch_cfg.N_id                 = N_id;
  pbch_cfg.n_hf                 = msg->hrf;
  pbch_cfg.ssb_idx              = msg->ssb_idx;
  pbch_cfg.Lmax                 = q->Lmax;
  pbch_cfg.beta                 = 0.0f;
  if (srsran_pbch_nr_encode(&q->pbch, &pbch_cfg, msg, ssb_grid) < SRSRAN_SUCCESS) {
    ERROR("Error encoding PBCH");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

SRSRAN_API int
srsran_ssb_put_grid(srsran_ssb_t* q, uint32_t N_id, const srsran_pbch_msg_nr_t* msg, cf_t* re_grid, uint32_t grid_bw_sc)
{
  // Verify input parameters
  if (q == NULL || N_id >= SRSRAN_NOF_NID_NR || msg == NULL || re_grid == NULL ||
      grid_bw_sc * SRSRAN_NRE < SRSRAN_SSB_BW_SUBC) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (!q->args.enable_encode) {
    ERROR("SSB is not configured for encode");
    return SRSRAN_ERROR;
  }

  // Put signals in SSB grid
  cf_t ssb_grid[SRSRAN_SSB_NOF_RE] = {};
  if (ssb_encode(q, N_id, msg, ssb_grid) < SRSRAN_SUCCESS) {
    ERROR("Putting SSB in grid");
    return SRSRAN_ERROR;
  }

  // First symbol in the half frame
  uint32_t l_first = q->l_first[msg->ssb_idx];

  // Frequency offset fom the bottom of the grid
  uint32_t f_offset = grid_bw_sc / 2 + q->f_offset - SRSRAN_SSB_BW_SUBC / 2;

  // Put SSB grid in the actual resource grid
  for (uint32_t l = 0; l < SRSRAN_SSB_DURATION_NSYMB; l++) {
    srsran_vec_cf_copy(
        &re_grid[grid_bw_sc * (l_first + l) + f_offset], &ssb_grid[SRSRAN_SSB_BW_SUBC * l], SRSRAN_SSB_BW_SUBC);
  }

  return SRSRAN_SUCCESS;
}

int srsran_ssb_add(srsran_ssb_t* q, uint32_t N_id, const srsran_pbch_msg_nr_t* msg, const cf_t* in, cf_t* out)
{
  // Verify input parameters
  if (q == NULL || N_id >= SRSRAN_NOF_NID_NR || msg == NULL || in == NULL || out == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (!q->args.enable_encode) {
    ERROR("SSB is not configured for encode");
    return SRSRAN_ERROR;
  }

  // Put signals in SSB grid
  cf_t ssb_grid[SRSRAN_SSB_NOF_RE] = {};
  if (ssb_encode(q, N_id, msg, ssb_grid) < SRSRAN_SUCCESS) {
    ERROR("Putting SSB in grid");
    return SRSRAN_ERROR;
  }

  // Select start symbol from SSB candidate index
  int t_offset = ssb_get_t_offset(q, msg->ssb_idx);
  if (t_offset < SRSRAN_SUCCESS) {
    ERROR("Invalid SSB candidate index");
    return SRSRAN_ERROR;
  }

  // Select input/ouput pointers considering the time offset in the slot
  const cf_t* in_ptr  = &in[t_offset];
  cf_t*       out_ptr = &out[t_offset];

  // For each SSB symbol, modulate
  for (uint32_t l = 0; l < SRSRAN_SSB_DURATION_NSYMB; l++) {
    // Map SSB in resource grid and perform IFFT
    ssb_modulate_symbol(q, ssb_grid, l);

    // Phase compensation
    cf_t phase_compensation = (cf_t)cexp(-I * 2.0 * M_PI * q->cfg.center_freq_hz * (double)t_offset / q->cfg.srate_hz);
    srsran_vec_sc_prod_ccc(q->tmp_time, phase_compensation, q->tmp_time, q->symbol_sz);
    t_offset += (int)(q->symbol_sz + q->cp_sz);

    // Add cyclic prefix to input;
    srsran_vec_sum_ccc(in_ptr, &q->tmp_time[q->symbol_sz - q->cp_sz], out_ptr, q->cp_sz);
    in_ptr += q->cp_sz;
    out_ptr += q->cp_sz;

    // Add symbol to the input baseband
    srsran_vec_sum_ccc(in_ptr, q->tmp_time, out_ptr, q->symbol_sz);
    in_ptr += q->symbol_sz;
    out_ptr += q->symbol_sz;
  }

  return SRSRAN_SUCCESS;
}

static int ssb_demodulate(srsran_ssb_t* q,
                          const cf_t*   in,
                          uint32_t      t_offset,
                          float         coarse_cfo_hz,
                          cf_t          ssb_grid[SRSRAN_SSB_NOF_RE])
{
  const cf_t* in_ptr = &in[t_offset];
  for (uint32_t l = 0; l < SRSRAN_SSB_DURATION_NSYMB; l++) {
    // Advance half CP, to avoid inter symbol interference
    in_ptr += SRSRAN_FLOOR(q->cp_sz, 2);

    // Copy FFT window in temporal time domain buffer
    if (isnormal(coarse_cfo_hz)) {
      srsran_vec_apply_cfo(in_ptr, (float)(-coarse_cfo_hz / q->cfg.srate_hz), q->tmp_time, q->symbol_sz);
    } else {
      srsran_vec_cf_copy(q->tmp_time, in_ptr, q->symbol_sz);
    }
    in_ptr += q->symbol_sz + SRSRAN_CEIL(q->cp_sz, 2);

    // Phase compensation
    cf_t phase_compensation =
        (cf_t)cexp(-I * 2.0 * M_PI * (q->cfg.center_freq_hz - coarse_cfo_hz) * (double)t_offset / q->cfg.srate_hz);
    t_offset += q->symbol_sz + q->cp_sz;

    // Convert to frequency domain
    srsran_dft_run_guru_c(&q->fft);

    // Compensate half CP delay
    srsran_vec_apply_cfo(q->tmp_freq, SRSRAN_CEIL(q->cp_sz, 2) / (float)(q->symbol_sz), q->tmp_freq, q->symbol_sz);

    // Select symbol in grid
    cf_t* ptr = &ssb_grid[l * SRSRAN_SSB_BW_SUBC];

    // Map frequency domain symbol into the SSB grid
    if (q->f_offset >= SRSRAN_SSB_BW_SUBC / 2) {
      srsran_vec_cf_copy(ptr, &q->tmp_freq[q->f_offset - SRSRAN_SSB_BW_SUBC / 2], SRSRAN_SSB_BW_SUBC);
    } else if (q->f_offset <= -SRSRAN_SSB_BW_SUBC / 2) {
      srsran_vec_cf_copy(ptr, &q->tmp_freq[q->symbol_sz + q->f_offset - SRSRAN_SSB_BW_SUBC / 2], SRSRAN_SSB_BW_SUBC);
    } else {
      srsran_vec_cf_copy(
          &ptr[SRSRAN_SSB_BW_SUBC / 2 - q->f_offset], &q->tmp_freq[0], SRSRAN_SSB_BW_SUBC / 2 + q->f_offset);
      srsran_vec_cf_copy(&ptr[0],
                         &q->tmp_freq[q->symbol_sz - SRSRAN_SSB_BW_SUBC / 2 + q->f_offset],
                         SRSRAN_SSB_BW_SUBC / 2 - q->f_offset);
    }

    // Normalize
    float norm = sqrtf((float)q->symbol_sz);
    if (isnormal(norm)) {
      srsran_vec_sc_prod_ccc(ptr, conjf(phase_compensation) / norm, ptr, SRSRAN_SSB_BW_SUBC);
    }
  }

  return SRSRAN_SUCCESS;
}

static int
ssb_measure(srsran_ssb_t* q, const cf_t ssb_grid[SRSRAN_SSB_NOF_RE], uint32_t N_id, srsran_csi_trs_measurements_t* meas)
{
  uint32_t N_id_1 = SRSRAN_NID_1_NR(N_id);
  uint32_t N_id_2 = SRSRAN_NID_2_NR(N_id);

  // Extract PSS and SSS LSE
  cf_t pss_lse[SRSRAN_PSS_NR_LEN];
  cf_t sss_lse[SRSRAN_SSS_NR_LEN];
  if (srsran_pss_nr_extract_lse(ssb_grid, N_id_2, pss_lse) < SRSRAN_SUCCESS ||
      srsran_sss_nr_extract_lse(ssb_grid, N_id_1, N_id_2, sss_lse) < SRSRAN_SUCCESS) {
    ERROR("Error extracting LSE");
    return SRSRAN_ERROR;
  }

  // Estimate average delay
  float delay_pss_norm = srsran_vec_estimate_frequency(pss_lse, SRSRAN_PSS_NR_LEN);
  float delay_sss_norm = srsran_vec_estimate_frequency(sss_lse, SRSRAN_SSS_NR_LEN);
  float delay_avg_norm = (delay_pss_norm + delay_sss_norm) / 2.0f;
  float delay_avg_us   = 1e6f * delay_avg_norm / q->scs_hz;

  // Pre-compensate delay
  cf_t ssb_grid_corrected[SRSRAN_SSB_NOF_RE];
  for (uint32_t l = 0; l < SRSRAN_SSB_DURATION_NSYMB; l++) {
    srsran_vec_apply_cfo(&ssb_grid[SRSRAN_SSB_BW_SUBC * l],
                         delay_avg_norm,
                         &ssb_grid_corrected[SRSRAN_SSB_BW_SUBC * l],
                         SRSRAN_SSB_BW_SUBC);
  }

  // Extract LSE again
  if (srsran_pss_nr_extract_lse(ssb_grid_corrected, N_id_2, pss_lse) < SRSRAN_SUCCESS ||
      srsran_sss_nr_extract_lse(ssb_grid_corrected, N_id_1, N_id_2, sss_lse) < SRSRAN_SUCCESS) {
    ERROR("Error extracting LSE");
    return SRSRAN_ERROR;
  }

  // Estimate average EPRE
  float epre_pss = srsran_vec_avg_power_cf(pss_lse, SRSRAN_PSS_NR_LEN);
  float epre_sss = srsran_vec_avg_power_cf(sss_lse, SRSRAN_SSS_NR_LEN);
  float epre     = (epre_pss + epre_sss) / 2.0f;

  // Compute correlation
  cf_t corr_pss = srsran_vec_acc_cc(pss_lse, SRSRAN_PSS_NR_LEN) / SRSRAN_PSS_NR_LEN;
  cf_t corr_sss = srsran_vec_acc_cc(sss_lse, SRSRAN_SSS_NR_LEN) / SRSRAN_SSS_NR_LEN;

  // Compute CFO in Hz
  float distance_s = srsran_symbol_distance_s(SRSRAN_PSS_NR_SYMBOL_IDX, SRSRAN_SSS_NR_SYMBOL_IDX, q->cfg.scs);
  float cfo_hz_max = 1.0f / distance_s;
  float cfo_hz     = cargf(corr_sss * conjf(corr_pss)) / (2.0f * M_PI) * cfo_hz_max;

  // Compute average RSRP
  float rsrp_pss = SRSRAN_CSQABS(corr_pss);
  float rsrp_sss = SRSRAN_CSQABS(corr_sss);
  float rsrp     = (rsrp_pss + rsrp_sss) / 2.0f;

  // Avoid taking log of 0 or another abnormal value
  if (!isnormal(rsrp)) {
    rsrp = 1e-9f;
  }

  // Estimate Noise:
  // - Infinite (1e9), if the EPRE or RSRP is zero
  // - EPRE-RSRP if EPRE > RSRP
  // - zero (1e-9), otherwise
  float n0_pss = 1e-9f;
  if (!isnormal(epre_pss) || !isnormal(rsrp_pss)) {
    n0_pss = 1e9f;
  } else if (epre_pss > rsrp_pss) {
    n0_pss = epre - rsrp_pss;
  }
  float n0_sss = 1e-9f;
  if (!isnormal(epre_sss) || !isnormal(rsrp_sss)) {
    n0_sss = 1e9f;
  } else if (epre_sss > rsrp_sss) {
    n0_sss = epre - rsrp_sss;
  }
  float n0 = (n0_pss + n0_sss) / 2.0f;

  // Put measurements together
  meas->epre       = epre;
  meas->epre_dB    = srsran_convert_power_to_dB(epre);
  meas->rsrp       = rsrp;
  meas->rsrp_dB    = srsran_convert_power_to_dB(rsrp);
  meas->n0         = n0;
  meas->n0_dB      = srsran_convert_power_to_dB(n0);
  meas->snr_dB     = meas->rsrp_dB - meas->n0_dB;
  meas->cfo_hz     = cfo_hz;
  meas->cfo_hz_max = cfo_hz_max;
  meas->delay_us   = delay_avg_us; // Convert the delay to microseconds
  meas->nof_re     = SRSRAN_PSS_NR_LEN + SRSRAN_SSS_NR_LEN;

  return SRSRAN_SUCCESS;
}

static void ssb_vec_prod_conj_circ_shift(const cf_t* a, const cf_t* b, cf_t* c, uint32_t n, int shift)
{
  uint32_t offset = (uint32_t)abs(shift);

  // Avoid negative number of samples
  if (offset > n) {
    srsran_vec_cf_zero(c, n);
    return;
  }

  // Shift is negative
  if (shift < 0) {
    srsran_vec_prod_conj_ccc(&a[offset], &b[0], &c[0], n - offset);
    srsran_vec_prod_conj_ccc(&a[0], &b[n - offset], &c[n - offset], offset);
    return;
  }

  // Shift is positive
  if (shift > 0) {
    srsran_vec_prod_conj_ccc(&a[0], &b[offset], &c[0], n - offset);
    srsran_vec_prod_conj_ccc(&a[n - offset], &b[0], &c[n - offset], offset);
    return;
  }

  // Shift is zero
  srsran_vec_prod_conj_ccc(a, b, c, n);
}

static int ssb_pss_search(srsran_ssb_t* q,
                          const cf_t*   in,
                          uint32_t      nof_samples,
                          uint32_t*     found_N_id_2,
                          uint32_t*     found_delay,
                          float*        coarse_cfo_hz)
{
  // verify it is initialised
  if (q->corr_sz == 0) {
    return SRSRAN_ERROR;
  }

  // Calculate correlation CFO coarse precision
  double coarse_cfo_ref_hz = (q->cfg.srate_hz / q->corr_sz);

  // Calculate shift integer range to detect the signal with a maximum CFO equal to the SSB subcarrier spacing
  int shift_range = (int)ceil(SRSRAN_SUBC_SPACING_NR(q->cfg.scs) / coarse_cfo_ref_hz);

  // Calculate the coarse shift increment for half of the subcarrier spacing
  int shift_coarse_inc = shift_range / 2;

  // Correlation best sequence
  float    best_corr   = 0;
  uint32_t best_delay  = 0;
  uint32_t best_N_id_2 = 0;
  int      best_shift  = 0;

  // Delay in correlation window
  uint32_t t_offset = 0;
  while ((t_offset + q->symbol_sz) < nof_samples) {
    // Number of samples taken in this iteration
    uint32_t n = q->corr_sz;

    // Detect if the correlation input exceeds the input length, take the maximum amount of samples
    if (t_offset + q->corr_sz > nof_samples) {
      n = nof_samples - t_offset;
    }

    // Copy the amount of samples
    srsran_vec_cf_copy(q->tmp_time, &in[t_offset], n);

    // Append zeros if there is space left
    if (n < q->corr_sz) {
      srsran_vec_cf_zero(&q->tmp_time[n], q->corr_sz - n);
    }

    // Convert to frequency domain
    srsran_dft_run_guru_c(&q->fft_corr);

    // Try each N_id_2 sequence
    for (uint32_t N_id_2 = 0; N_id_2 < SRSRAN_NOF_NID_2_NR; N_id_2++) {
      // Steer coarse frequency offset
      for (int shift = -shift_range; shift <= shift_range; shift += shift_coarse_inc) {
        // Actual correlation in frequency domain
        ssb_vec_prod_conj_circ_shift(q->tmp_freq, q->pss_seq[N_id_2], q->tmp_corr, q->corr_sz, shift);

        // Convert to time domain
        srsran_dft_run_guru_c(&q->ifft_corr);

        // Find maximum
        uint32_t peak_idx = srsran_vec_max_abs_ci(q->tmp_time, q->corr_window);

        // Average power, take total power of the frequency domain signal after filtering, skip correlation window if
        // value is invalid (0.0, nan or inf)
        float avg_pwr_corr = srsran_vec_avg_power_cf(q->tmp_corr, q->corr_sz);
        if (!isnormal(avg_pwr_corr)) {
          continue;
        }

        // Normalise correlation
        float corr = SRSRAN_CSQABS(q->tmp_time[peak_idx]) / avg_pwr_corr / sqrtf(SRSRAN_PSS_NR_LEN);

        // Update if the correlation is better than the current best
        if (best_corr < corr) {
          best_corr   = corr;
          best_delay  = peak_idx + t_offset;
          best_N_id_2 = N_id_2;
          best_shift  = shift;
        }
      }
    }

    // Advance time
    t_offset += q->corr_window;
  }

  // From the best sequence correlate in frequency domain
  {
    // Reset best correlation
    best_corr = 0.0f;

    // Number of samples taken in this iteration
    uint32_t n = q->corr_sz;

    // Detect if the correlation input exceeds the input length, take the maximum amount of samples
    if (best_delay + q->corr_sz > nof_samples) {
      n = nof_samples - best_delay;
    }

    // Copy the amount of samples
    srsran_vec_cf_copy(q->tmp_time, &in[best_delay], n);

    // Append zeros if there is space left
    if (n < q->corr_sz) {
      srsran_vec_cf_zero(&q->tmp_time[n], q->corr_sz - n);
    }

    // Convert to frequency domain
    srsran_dft_run_guru_c(&q->fft_corr);

    for (int shift = -shift_range; shift <= shift_range; shift++) {
      // Actual correlation in frequency domain
      ssb_vec_prod_conj_circ_shift(q->tmp_freq, q->pss_seq[best_N_id_2], q->tmp_corr, q->corr_sz, shift);

      // Calculate correlation assuming the peak is in the first sample
      float corr = SRSRAN_CSQABS(srsran_vec_acc_cc(q->tmp_corr, q->corr_sz));

      // Update if the correlation is better than the current best
      if (best_corr < corr) {
        best_corr  = corr;
        best_shift = shift;
      }
    }
  }

  // Save findings
  *found_delay   = best_delay;
  *found_N_id_2  = best_N_id_2;
  *coarse_cfo_hz = -(float)best_shift * coarse_cfo_ref_hz;

  return SRSRAN_SUCCESS;
}

int srsran_ssb_csi_search(srsran_ssb_t*                  q,
                          const cf_t*                    in,
                          uint32_t                       nof_samples,
                          uint32_t*                      N_id,
                          srsran_csi_trs_measurements_t* meas)
{
  // Verify inputs
  if (q == NULL || in == NULL || N_id == NULL || meas == NULL || !isnormal(q->scs_hz)) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (!q->args.enable_search) {
    ERROR("SSB is not configured for search");
    return SRSRAN_ERROR;
  }

  // Avoid finding a peak in a region that cannot be demodulated
  if (nof_samples < (q->symbol_sz + q->cp_sz) * SRSRAN_SSB_DURATION_NSYMB) {
    ERROR("Insufficient number of samples (%d/%d)", nof_samples, (q->symbol_sz + q->cp_sz) * SRSRAN_SSB_DURATION_NSYMB);
    return SRSRAN_ERROR;
  }
  nof_samples -= (q->symbol_sz + q->cp_sz) * SRSRAN_SSB_DURATION_NSYMB;

  // Search for PSS in time domain
  uint32_t N_id_2        = 0;
  uint32_t t_offset      = 0;
  float    coarse_cfo_hz = 0.0f;
  if (ssb_pss_search(q, in, nof_samples, &N_id_2, &t_offset, &coarse_cfo_hz) < SRSRAN_SUCCESS) {
    ERROR("Error searching for N_id_2");
    return SRSRAN_ERROR;
  }

  // Remove CP offset prior demodulation
  if (t_offset >= q->cp_sz) {
    t_offset -= q->cp_sz;
  } else {
    t_offset = 0;
  }

  // Make sure SSB time offset is in bounded in the input buffer
  if (t_offset + q->ssb_sz > nof_samples) {
    return SRSRAN_SUCCESS;
  }

  // Demodulate
  cf_t ssb_grid[SRSRAN_SSB_NOF_RE] = {};
  if (ssb_demodulate(q, in, t_offset, coarse_cfo_hz, ssb_grid) < SRSRAN_SUCCESS) {
    ERROR("Error demodulating");
    return SRSRAN_ERROR;
  }

  // Find best N_id_1
  uint32_t N_id_1   = 0;
  float    sss_corr = 0.0f;
  if (srsran_sss_nr_find(ssb_grid, N_id_2, &sss_corr, &N_id_1) < SRSRAN_SUCCESS) {
    ERROR("Error searching for N_id_2");
    return SRSRAN_ERROR;
  }

  // Select N_id
  *N_id = SRSRAN_NID_NR(N_id_1, N_id_2);

  // Measure selected N_id
  if (ssb_measure(q, ssb_grid, *N_id, meas)) {
    ERROR("Error measuring");
    return SRSRAN_ERROR;
  }

  // Add delay to measure
  meas->delay_us += (float)(1e6 * t_offset / q->cfg.srate_hz);
  meas->cfo_hz -= coarse_cfo_hz;

  return SRSRAN_SUCCESS;
}

int srsran_ssb_csi_measure(srsran_ssb_t*                  q,
                           uint32_t                       N_id,
                           uint32_t                       ssb_idx,
                           const cf_t*                    in,
                           srsran_csi_trs_measurements_t* meas)
{
  // Verify inputs
  if (q == NULL || N_id >= SRSRAN_NOF_NID_NR || in == NULL || meas == NULL || !isnormal(q->scs_hz)) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (!q->args.enable_measure) {
    ERROR("SSB is not configured to measure");
    return SRSRAN_ERROR;
  }

  // Select start symbol from SSB candidate index
  int t_offset = ssb_get_t_offset(q, ssb_idx);
  if (t_offset < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // Demodulate
  cf_t ssb_grid[SRSRAN_SSB_NOF_RE] = {};
  if (ssb_demodulate(q, in, (uint32_t)t_offset, 0.0f, ssb_grid) < SRSRAN_SUCCESS) {
    ERROR("Error demodulating");
    return SRSRAN_ERROR;
  }

  // Actual measurement
  if (ssb_measure(q, ssb_grid, N_id, meas)) {
    ERROR("Error measuring");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

static int ssb_select_pbch(srsran_ssb_t*            q,
                           uint32_t                 N_id,
                           const cf_t               ssb_grid[SRSRAN_SSB_NOF_RE],
                           uint32_t*                found_n_hf,
                           uint32_t*                found_ssb_idx_4lsb,
                           srsran_dmrs_pbch_meas_t* pbch_meas)
{
  // Prepare PBCH DMRS configuration
  srsran_dmrs_pbch_cfg_t pbch_dmrs_cfg = {};
  pbch_dmrs_cfg.N_id                   = N_id;
  pbch_dmrs_cfg.n_hf                   = 0; // Parameter to guess
  pbch_dmrs_cfg.ssb_idx                = 0; // Parameter to guess
  pbch_dmrs_cfg.L_max                  = q->Lmax;
  pbch_dmrs_cfg.beta                   = 0.0f;
  pbch_dmrs_cfg.scs                    = q->cfg.scs;

  // Initialise best values
  srsran_dmrs_pbch_meas_t best_meas    = {};
  uint32_t                best_n_hf    = 0;
  uint32_t                best_ssb_idx = 0;

  // Iterate over all the parameters to guess and select the most suitable
  for (uint32_t n_hf = 0; n_hf < 2; n_hf++) {
    for (uint32_t ssb_idx = 0; ssb_idx < SRSRAN_MIN(8, q->Lmax); ssb_idx++) {
      // Set parameters
      pbch_dmrs_cfg.n_hf    = n_hf;
      pbch_dmrs_cfg.ssb_idx = ssb_idx;

      // Measure
      srsran_dmrs_pbch_meas_t meas = {};
      if (srsran_dmrs_pbch_measure(&pbch_dmrs_cfg, ssb_grid, &meas) < SRSRAN_SUCCESS) {
        ERROR("Error measure for n_hf=%d ssb_idx=%d", n_hf, ssb_idx);
        return SRSRAN_ERROR;
      }

      // Select the result with highest correlation (most suitable)
      if (meas.corr > best_meas.corr) {
        best_meas    = meas;
        best_n_hf    = n_hf;
        best_ssb_idx = ssb_idx;
      }
    }
  }

  // Save findings
  *found_n_hf         = best_n_hf;
  *found_ssb_idx_4lsb = best_ssb_idx;
  *pbch_meas          = best_meas;

  return SRSRAN_SUCCESS;
}

static int ssb_decode_pbch(srsran_ssb_t*         q,
                           uint32_t              N_id,
                           uint32_t              n_hf,
                           uint32_t              ssb_idx,
                           const cf_t            ssb_grid[SRSRAN_SSB_NOF_RE],
                           srsran_pbch_msg_nr_t* msg)
{
  // Prepare PBCH DMRS configuration
  srsran_dmrs_pbch_cfg_t pbch_dmrs_cfg = {};
  pbch_dmrs_cfg.N_id                   = N_id;
  pbch_dmrs_cfg.n_hf                   = n_hf;
  pbch_dmrs_cfg.ssb_idx                = ssb_idx;
  pbch_dmrs_cfg.L_max                  = q->Lmax;
  pbch_dmrs_cfg.beta                   = 0.0f;
  pbch_dmrs_cfg.scs                    = q->cfg.scs;

  // Compute PBCH channel estimates
  cf_t ce[SRSRAN_SSB_NOF_RE] = {};
  if (srsran_dmrs_pbch_estimate(&pbch_dmrs_cfg, ssb_grid, ce) < SRSRAN_SUCCESS) {
    ERROR("Error estimating channel");
    return SRSRAN_ERROR;
  }

  // Prepare PBCH configuration
  srsran_pbch_nr_cfg_t pbch_cfg = {};
  pbch_cfg.N_id                 = N_id;
  pbch_cfg.n_hf                 = n_hf;
  pbch_cfg.ssb_idx              = ssb_idx;
  pbch_cfg.Lmax                 = q->Lmax;
  pbch_cfg.beta                 = 0.0f;

  // Decode
  if (srsran_pbch_nr_decode(&q->pbch, &pbch_cfg, ssb_grid, ce, msg) < SRSRAN_SUCCESS) {
    ERROR("Error decoding PBCH");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_ssb_decode_grid(srsran_ssb_t*         q,
                           uint32_t              N_id,
                           uint32_t              n_hf,
                           uint32_t              ssb_idx,
                           const cf_t*           re_grid,
                           uint32_t              grid_bw_sc,
                           srsran_pbch_msg_nr_t* msg)
{
  // Verify input parameters
  if (q == NULL || N_id >= SRSRAN_NOF_NID_NR || msg == NULL || re_grid == NULL || grid_bw_sc < SRSRAN_SSB_BW_SUBC) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (!q->args.enable_encode) {
    ERROR("SSB is not configured for encode");
    return SRSRAN_ERROR;
  }

  // First symbol in the half frame
  uint32_t l_first = q->l_first[ssb_idx];

  // Frequency offset fom the bottom of the grid
  uint32_t f_offset = grid_bw_sc / 2 + q->f_offset - SRSRAN_SSB_BW_SUBC / 2;

  // Get SSB grid from resource grid
  cf_t ssb_grid[SRSRAN_SSB_NOF_RE] = {};
  for (uint32_t l = 0; l < SRSRAN_SSB_DURATION_NSYMB; l++) {
    srsran_vec_cf_copy(
        &ssb_grid[SRSRAN_SSB_BW_SUBC * l], &re_grid[grid_bw_sc * (l_first + l) + f_offset], SRSRAN_SSB_BW_SUBC);
  }

  // Decode PBCH
  if (ssb_decode_pbch(q, N_id, n_hf, ssb_idx, ssb_grid, msg) < SRSRAN_SUCCESS) {
    ERROR("Error decoding");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_ssb_decode_pbch(srsran_ssb_t*         q,
                           uint32_t              N_id,
                           uint32_t              n_hf,
                           uint32_t              ssb_idx,
                           const cf_t*           in,
                           srsran_pbch_msg_nr_t* msg)
{
  // Verify inputs
  if (q == NULL || N_id >= SRSRAN_NOF_NID_NR || in == NULL || msg == NULL || !isnormal(q->scs_hz)) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (!q->args.enable_decode) {
    ERROR("SSB is not configured to decode");
    return SRSRAN_ERROR;
  }

  // Select start symbol from SSB candidate index
  int t_offset = ssb_get_t_offset(q, ssb_idx);
  if (t_offset < SRSRAN_SUCCESS) {
    return SRSRAN_ERROR;
  }

  // Demodulate
  cf_t ssb_grid[SRSRAN_SSB_NOF_RE] = {};
  if (ssb_demodulate(q, in, (uint32_t)t_offset, 0.0f, ssb_grid) < SRSRAN_SUCCESS) {
    ERROR("Error demodulating");
    return SRSRAN_ERROR;
  }

  // Decode PBCH
  if (ssb_decode_pbch(q, N_id, n_hf, ssb_idx, ssb_grid, msg) < SRSRAN_SUCCESS) {
    ERROR("Error decoding");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int srsran_ssb_search(srsran_ssb_t* q, const cf_t* in, uint32_t nof_samples, srsran_ssb_search_res_t* res)
{
  // Verify inputs
  if (q == NULL || in == NULL || res == NULL || !isnormal(q->scs_hz)) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (!q->args.enable_search || !q->args.enable_decode) {
    ERROR("SSB is not configured to search (%c) and decode (%c)",
          q->args.enable_search ? 'y' : 'n',
          q->args.enable_decode ? 'y' : 'n');
    return SRSRAN_ERROR;
  }

  // Set the SSB search result with default value with PBCH CRC unmatched, meaning no cell is found
  SRSRAN_MEM_ZERO(res, srsran_ssb_search_res_t, 1);

  // Search for PSS in time domain
  uint32_t N_id_2        = 0;
  uint32_t t_offset      = 0;
  float    coarse_cfo_hz = 0.0f;
  if (ssb_pss_search(q, in, nof_samples, &N_id_2, &t_offset, &coarse_cfo_hz) < SRSRAN_SUCCESS) {
    ERROR("Error searching for N_id_2");
    return SRSRAN_ERROR;
  }

  // Remove CP offset prior demodulation
  if (t_offset >= q->cp_sz) {
    t_offset -= q->cp_sz;
  } else {
    t_offset = 0;
  }

  // Make sure SSB time offset is in bounded in the input buffer
  if (t_offset + q->ssb_sz > nof_samples) {
    return SRSRAN_SUCCESS;
  }

  // Demodulate
  cf_t ssb_grid[SRSRAN_SSB_NOF_RE] = {};
  if (ssb_demodulate(q, in, t_offset, coarse_cfo_hz, ssb_grid) < SRSRAN_SUCCESS) {
    ERROR("Error demodulating");
    return SRSRAN_ERROR;
  }

  // Find best N_id_1
  uint32_t N_id_1   = 0;
  float    sss_corr = 0.0f;
  if (srsran_sss_nr_find(ssb_grid, N_id_2, &sss_corr, &N_id_1) < SRSRAN_SUCCESS) {
    ERROR("Error searching for N_id_2");
    return SRSRAN_ERROR;
  }

  // Select N_id
  uint32_t N_id = SRSRAN_NID_NR(N_id_1, N_id_2);

  // Select the most suitable SSB candidate
  uint32_t                n_hf      = 0;
  uint32_t                ssb_idx   = 0;
  srsran_dmrs_pbch_meas_t pbch_meas = {};
  if (ssb_select_pbch(q, N_id, ssb_grid, &n_hf, &ssb_idx, &pbch_meas) < SRSRAN_SUCCESS) {
    ERROR("Error selecting PBCH");
    return SRSRAN_ERROR;
  }

  // Avoid decoding if the selected PBCH DMRS do not reach the minimum threshold
  if (pbch_meas.corr < q->args.pbch_dmrs_thr) {
    return SRSRAN_SUCCESS;
  }

  // Decode PBCH
  srsran_pbch_msg_nr_t pbch_msg = {};
  if (ssb_decode_pbch(q, N_id, n_hf, ssb_idx, ssb_grid, &pbch_msg) < SRSRAN_SUCCESS) {
    ERROR("Error decoding PBCH");
    return SRSRAN_ERROR;
  }

  // If PBCH was not decoded, skip measurements
  if (!pbch_msg.crc) {
    return SRSRAN_SUCCESS;
  }

  // Perform measurements from PSS and SSS
  srsran_csi_trs_measurements_t measurements = {};
  if (ssb_measure(q, ssb_grid, N_id, &measurements) < SRSRAN_SUCCESS) {
    ERROR("Error measuring");
    return SRSRAN_ERROR;
  }

  // Save result
  res->N_id         = N_id;
  res->t_offset     = t_offset;
  res->pbch_msg     = pbch_msg;
  res->measurements = measurements;
  res->measurements.cfo_hz += coarse_cfo_hz;

  return SRSRAN_SUCCESS;
}

static int ssb_pss_find(srsran_ssb_t* q, const cf_t* in, uint32_t nof_samples, uint32_t N_id_2, uint32_t* found_delay)
{
  // verify it is initialised
  if (q->corr_sz == 0) {
    return SRSRAN_ERROR;
  }

  // Correlation best sequence
  float    best_corr  = 0;
  uint32_t best_delay = 0;

  // Delay in correlation window
  uint32_t t_offset = 0;
  while ((t_offset + q->symbol_sz) < nof_samples) {
    // Number of samples taken in this iteration
    uint32_t n = q->corr_sz;

    // Detect if the correlation input exceeds the input length, take the maximum amount of samples
    if (t_offset + q->corr_sz > nof_samples) {
      n = nof_samples - t_offset;
    }

    // Copy the amount of samples
    srsran_vec_cf_copy(q->tmp_time, &in[t_offset], n);

    // Append zeros if there is space left
    if (n < q->corr_sz) {
      srsran_vec_cf_zero(&q->tmp_time[n], q->corr_sz - n);
    }

    // Convert to frequency domain
    srsran_dft_run_guru_c(&q->fft_corr);

    // Actual correlation in frequency domain
    srsran_vec_prod_conj_ccc(q->tmp_freq, q->pss_seq[N_id_2], q->tmp_corr, q->corr_sz);

    // Convert to time domain
    srsran_dft_run_guru_c(&q->ifft_corr);

    // Find maximum
    uint32_t peak_idx = srsran_vec_max_abs_ci(q->tmp_time, q->corr_window);

    // Average power, skip window if value is invalid (0.0, nan or inf)
    float avg_pwr_corr = srsran_vec_avg_power_cf(&q->tmp_time[peak_idx], q->symbol_sz);
    if (!isnormal(avg_pwr_corr)) {
      // Advance time
      t_offset += q->corr_window;
      continue;
    }

    // Normalise correlation
    float corr = SRSRAN_CSQABS(q->tmp_time[peak_idx]) / avg_pwr_corr / sqrtf(SRSRAN_PSS_NR_LEN);

    // Update if the correlation is better than the current best
    if (best_corr < corr) {
      best_corr  = corr;
      best_delay = peak_idx + t_offset;
    }

    // Advance time
    t_offset += q->corr_window;
  }

  // Save findings
  *found_delay = best_delay;

  return SRSRAN_SUCCESS;
}

int srsran_ssb_find(srsran_ssb_t*                  q,
                    const cf_t*                    sf_buffer,
                    uint32_t                       N_id,
                    srsran_csi_trs_measurements_t* meas,
                    srsran_pbch_msg_nr_t*          pbch_msg)
{
  // Verify inputs
  if (q == NULL || sf_buffer == NULL || meas == NULL || !isnormal(q->scs_hz)) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (!q->args.enable_search) {
    ERROR("SSB is not configured for search");
    return SRSRAN_ERROR;
  }

  // Set the PBCH message result with default value (CRC unmatched), meaning no cell is found
  SRSRAN_MEM_ZERO(pbch_msg, srsran_pbch_msg_nr_t, 1);

  // Copy tail from previous execution into the start of this
  srsran_vec_cf_copy(q->sf_buffer, &q->sf_buffer[q->sf_sz], q->ssb_sz);

  // Append new samples
  srsran_vec_cf_copy(&q->sf_buffer[q->ssb_sz], sf_buffer, q->sf_sz);

  // Search for PSS in time domain
  uint32_t t_offset = 0;
  if (ssb_pss_find(q, q->sf_buffer, q->sf_sz + q->ssb_sz, SRSRAN_NID_2_NR(N_id), &t_offset) < SRSRAN_SUCCESS) {
    ERROR("Error searching for N_id_2");
    return SRSRAN_ERROR;
  }

  // Remove CP offset prior demodulation
  if (t_offset >= q->cp_sz) {
    t_offset -= q->cp_sz;
  } else {
    t_offset = 0;
  }

  // Make sure SSB time offset is in bounded in the input buffer
  if (t_offset > q->sf_sz) {
    return SRSRAN_SUCCESS;
  }

  // Demodulate
  cf_t ssb_grid[SRSRAN_SSB_NOF_RE] = {};
  if (ssb_demodulate(q, q->sf_buffer, t_offset, 0.0f, ssb_grid) < SRSRAN_SUCCESS) {
    ERROR("Error demodulating");
    return SRSRAN_ERROR;
  }

  // Measure selected N_id
  if (ssb_measure(q, ssb_grid, N_id, meas)) {
    ERROR("Error measuring");
    return SRSRAN_ERROR;
  }

  // Select the most suitable SSB candidate
  uint32_t                n_hf      = 0;
  uint32_t                ssb_idx   = 0; // SSB candidate index
  srsran_dmrs_pbch_meas_t pbch_meas = {};
  if (ssb_select_pbch(q, N_id, ssb_grid, &n_hf, &ssb_idx, &pbch_meas) < SRSRAN_SUCCESS) {
    ERROR("Error selecting PBCH");
    return SRSRAN_ERROR;
  }

  // Avoid decoding if the selected PBCH DMRS do not reach the minimum threshold
  if (pbch_meas.corr < q->args.pbch_dmrs_thr) {
    return SRSRAN_SUCCESS;
  }

  // Calculate the SSB offset in the subframe
  uint32_t ssb_offset = srsran_ssb_candidate_sf_offset(q, ssb_idx);

  // Compute PBCH channel estimates
  if (ssb_decode_pbch(q, N_id, n_hf, ssb_idx, ssb_grid, pbch_msg) < SRSRAN_SUCCESS) {
    ERROR("Error decoding PBCH");
    return SRSRAN_ERROR;
  }

  // SSB delay in SF
  float ssb_delay_us = (float)(1e6 * (((double)t_offset - (double)q->ssb_sz - (double)ssb_offset) / q->cfg.srate_hz));

  // Add delay to measure
  meas->delay_us += ssb_delay_us;

  return SRSRAN_SUCCESS;
}

int srsran_ssb_track(srsran_ssb_t*                  q,
                     const cf_t*                    sf_buffer,
                     uint32_t                       N_id,
                     uint32_t                       ssb_idx,
                     uint32_t                       n_hf,
                     srsran_csi_trs_measurements_t* meas,
                     srsran_pbch_msg_nr_t*          pbch_msg)
{
  // Verify inputs
  if (q == NULL || sf_buffer == NULL || meas == NULL || !isnormal(q->scs_hz)) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (!q->args.enable_search) {
    ERROR("SSB is not configured for search");
    return SRSRAN_ERROR;
  }

  // Calculate SSB offset
  uint32_t t_offset = srsran_ssb_candidate_sf_offset(q, ssb_idx);

  // Demodulate
  cf_t ssb_grid[SRSRAN_SSB_NOF_RE] = {};
  if (ssb_demodulate(q, sf_buffer, t_offset, 0.0f, ssb_grid) < SRSRAN_SUCCESS) {
    ERROR("Error demodulating");
    return SRSRAN_ERROR;
  }

  // Measure selected N_id
  if (ssb_measure(q, ssb_grid, N_id, meas)) {
    ERROR("Error measuring");
    return SRSRAN_ERROR;
  }

  // Compute PBCH channel estimates
  if (ssb_decode_pbch(q, N_id, n_hf, ssb_idx, ssb_grid, pbch_msg) < SRSRAN_SUCCESS) {
    ERROR("Error decoding PBCH");
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

uint32_t srsran_ssb_candidate_sf_idx(const srsran_ssb_t* q, uint32_t ssb_idx, bool half_frame)
{
  if (q == NULL) {
    return 0;
  }

  uint32_t nof_symbols_subframe = SRSRAN_NSYMB_PER_SLOT_NR * SRSRAN_NSLOTS_PER_SF_NR(q->cfg.scs);

  return q->l_first[ssb_idx] / nof_symbols_subframe + (half_frame ? (SRSRAN_NOF_SF_X_FRAME / 2) : 0);
}

uint32_t srsran_ssb_candidate_sf_offset(const srsran_ssb_t* q, uint32_t ssb_idx)
{
  if (q == NULL) {
    return 0;
  }

  uint32_t nof_symbols_subframe = SRSRAN_NSYMB_PER_SLOT_NR * SRSRAN_NSLOTS_PER_SF_NR(q->cfg.scs);
  uint32_t l                    = q->l_first[ssb_idx] % nof_symbols_subframe;

  uint32_t cp_sz_0 = (16U * q->symbol_sz) / 2048U;

  return cp_sz_0 + l * (q->symbol_sz + q->cp_sz);
}

uint32_t srsran_ssb_cfg_to_str(const srsran_ssb_cfg_t* cfg, char* str, uint32_t str_len)
{
  uint32_t n = 0;

  n = srsran_print_check(str,
                         str_len,
                         n,
                         "srate=%.2f MHz; c-freq=%.3f MHz; ss-freq=%.3f MHz; scs=%s; pattern=%s; duplex=%s;",
                         cfg->srate_hz / 1e6,
                         cfg->center_freq_hz / 1e6,
                         cfg->ssb_freq_hz / 1e6,
                         srsran_subcarrier_spacing_to_str(cfg->scs),
                         srsran_ssb_pattern_to_str(cfg->pattern),
                         cfg->duplex_mode == SRSRAN_DUPLEX_MODE_FDD ? "fdd" : "tdd");

  if (cfg->periodicity_ms > 0) {
    n = srsran_print_check(str, str_len, n, " period=%d ms;", cfg->periodicity_ms);
  }

  return n;
}
