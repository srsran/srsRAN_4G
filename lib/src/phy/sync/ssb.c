/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/sync/ssb.h"
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

int srsran_ssb_init(srsran_ssb_t* q, const srsran_ssb_args_t* args)
{
  // Verify input parameters
  if (q == NULL || args == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Copy arguments
  q->args = *args;

  // Check if the maximum sampling rate is in range, force default otherwise
  if (!isnormal(q->args.max_srate_hz) || q->args.max_srate_hz < 0.0) {
    q->args.max_srate_hz = SRSRAN_SSB_DEFAULT_MAX_SRATE_HZ;
  }

  q->scs_hz        = (float)SRSRAN_SUBC_SPACING_NR(q->args.min_scs);
  q->max_symbol_sz = (uint32_t)round(q->args.max_srate_hz / q->scs_hz);

  // Allocate temporal data
  q->tmp_time = srsran_vec_cf_malloc(q->max_symbol_sz);
  q->tmp_freq = srsran_vec_cf_malloc(q->max_symbol_sz);
  if (q->tmp_time == NULL || q->tmp_time == NULL) {
    ERROR("Malloc");
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

  srsran_dft_plan_free(&q->ifft);
  srsran_dft_plan_free(&q->fft);

  SRSRAN_MEM_ZERO(q, srsran_ssb_t, 1);
}

int srsran_ssb_set_cfg(srsran_ssb_t* q, const srsran_ssb_cfg_t* cfg)
{
  // Verify input parameters
  if (q == NULL || cfg == NULL) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Calculate subcarrier spacing in Hz
  q->scs_hz = (double)SRSRAN_SUBC_SPACING_NR(cfg->scs);

  // Calculate SSB symbol size and integer offset
  uint32_t symbol_sz = (uint32_t)round(cfg->srate_hz / q->scs_hz);
  q->offset          = (uint32_t)(cfg->freq_offset_hz / q->scs_hz);
  q->cp0_sz          = (160U * symbol_sz) / 2048U;
  q->cp_sz           = (144U * symbol_sz) / 2048U;

  // Calculate SSB sampling error and check
  double ssb_srate_error_Hz = ((double)symbol_sz * q->scs_hz) - cfg->srate_hz;
  if (fabs(ssb_srate_error_Hz) > SSB_SRATE_MAX_ERROR_HZ) {
    ERROR("Invalid sampling rate (%.2f MHz)", cfg->srate_hz / 1e6);
    return SRSRAN_ERROR;
  }

  // Calculate SSB offset error and check
  double ssb_offset_error_Hz = ((double)q->offset * q->scs_hz) - cfg->freq_offset_hz;
  if (fabs(ssb_offset_error_Hz) > SSB_FREQ_OFFSET_MAX_ERROR_HZ) {
    ERROR("SSB Offset error exceeds maximum allowed");
    return SRSRAN_ERROR;
  }

  // Verify symbol size
  if (q->max_symbol_sz < symbol_sz) {
    ERROR("New symbol size (%d) exceeds maximum symbol size (%d)", symbol_sz, q->max_symbol_sz);
  }

  // Replan iFFT
  if ((q->args.enable_encode) && q->symbol_sz != symbol_sz) {
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
  if ((q->args.enable_measure || q->args.enable_decode) && q->symbol_sz != symbol_sz) {
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
    q->cfg.beta_pbch = SRSRAN_SSB_DEFAULT_BETA;
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

  uint32_t N_id_1                      = SRSRAN_NID_1_NR(N_id);
  uint32_t N_id_2                      = SRSRAN_NID_2_NR(N_id);
  cf_t     ssb_grid[SRSRAN_SSB_NOF_RE] = {};

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
  // ...

  // Put PBCH payload
  // ...

  // Initialise frequency domain
  srsran_vec_cf_zero(q->tmp_freq, q->symbol_sz);

  // Modulate
  const cf_t* in_ptr  = in;
  cf_t*       out_ptr = out;
  for (uint32_t l = 0; l < SRSRAN_SSB_DURATION_NSYMB; l++) {
    // Get CP length
    uint32_t cp_len = (l == 0) ? q->cp0_sz : q->cp_sz;

    // Select symbol in grid
    cf_t* ptr = &ssb_grid[l * SRSRAN_SSB_BW_SUBC];

    // Map grid into frequency domain symbol
    if (q->offset >= SRSRAN_SSB_BW_SUBC / 2) {
      srsran_vec_cf_copy(&q->tmp_freq[q->offset - SRSRAN_SSB_BW_SUBC / 2], ptr, SRSRAN_SSB_BW_SUBC);
    } else if (q->offset <= -SRSRAN_SSB_BW_SUBC / 2) {
      srsran_vec_cf_copy(&q->tmp_freq[q->symbol_sz + q->offset - SRSRAN_SSB_BW_SUBC / 2], ptr, SRSRAN_SSB_BW_SUBC);
    } else {
      srsran_vec_cf_copy(&q->tmp_freq[0], &ptr[SRSRAN_SSB_BW_SUBC / 2 - q->offset], SRSRAN_SSB_BW_SUBC / 2 + q->offset);
      srsran_vec_cf_copy(
          &q->tmp_freq[q->symbol_sz - SRSRAN_SSB_BW_SUBC / 2 + q->offset], &ptr[0], SRSRAN_SSB_BW_SUBC / 2 - q->offset);
    }

    // Convert to time domain
    srsran_dft_run_guru_c(&q->ifft);

    // Normalise output
    float norm = sqrtf((float)q->symbol_sz);
    if (isnormal(norm)) {
      srsran_vec_sc_prod_cfc(q->tmp_time, 1.0f / norm, q->tmp_time, q->symbol_sz);
    }

    // Add cyclic prefix to input;
    srsran_vec_sum_ccc(in_ptr, &q->tmp_time[q->symbol_sz - cp_len], out_ptr, cp_len);
    in_ptr += cp_len;
    out_ptr += cp_len;

    // Add symbol to the input baseband
    srsran_vec_sum_ccc(in_ptr, q->tmp_time, out_ptr, q->symbol_sz);
    in_ptr += q->symbol_sz;
    out_ptr += q->symbol_sz;
  }

  return SRSRAN_SUCCESS;
}

int srsran_ssb_csi_measure(srsran_ssb_t* q, uint32_t N_id, const cf_t* in, srsran_csi_trs_measurements_t* meas)
{
  // Verify inputs
  if (q == NULL || N_id >= SRSRAN_NOF_NID_NR || in == NULL || meas == NULL || !isnormal(q->scs_hz)) {
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  if (!q->args.enable_measure) {
    ERROR("SSB is not configured for measure");
    return SRSRAN_ERROR;
  }

  uint32_t N_id_1                      = SRSRAN_NID_1_NR(N_id);
  uint32_t N_id_2                      = SRSRAN_NID_2_NR(N_id);
  cf_t     ssb_grid[SRSRAN_SSB_NOF_RE] = {};

  // Demodulate
  const cf_t* in_ptr = in;
  for (uint32_t l = 0; l < SRSRAN_SSB_DURATION_NSYMB; l++) {
    // Get CP length
    uint32_t cp_len = (l == 0) ? q->cp0_sz : q->cp_sz;

    // Advance half CP, to avoid inter symbol interference
    in_ptr += SRSRAN_FLOOR(cp_len, 2);

    // Copy FFT window in temporal time domain buffer
    srsran_vec_cf_copy(q->tmp_time, in_ptr, q->symbol_sz);
    in_ptr += q->symbol_sz + SRSRAN_CEIL(cp_len, 2);

    // Convert to frequency domain
    srsran_dft_run_guru_c(&q->fft);

    // Compensate half CP delay
    srsran_vec_apply_cfo(q->tmp_freq, SRSRAN_CEIL(cp_len, 2) / (float)(q->symbol_sz), q->tmp_freq, q->symbol_sz);

    // Select symbol in grid
    cf_t* ptr = &ssb_grid[l * SRSRAN_SSB_BW_SUBC];

    // Map frequency domain symbol into the SSB grid
    if (q->offset >= SRSRAN_SSB_BW_SUBC / 2) {
      srsran_vec_cf_copy(ptr, &q->tmp_freq[q->offset - SRSRAN_SSB_BW_SUBC / 2], SRSRAN_SSB_BW_SUBC);
    } else if (q->offset <= -SRSRAN_SSB_BW_SUBC / 2) {
      srsran_vec_cf_copy(ptr, &q->tmp_freq[q->symbol_sz + q->offset - SRSRAN_SSB_BW_SUBC / 2], SRSRAN_SSB_BW_SUBC);
    } else {
      srsran_vec_cf_copy(&ptr[SRSRAN_SSB_BW_SUBC / 2 - q->offset], &q->tmp_freq[0], SRSRAN_SSB_BW_SUBC / 2 + q->offset);
      srsran_vec_cf_copy(
          &ptr[0], &q->tmp_freq[q->symbol_sz - SRSRAN_SSB_BW_SUBC / 2 + q->offset], SRSRAN_SSB_BW_SUBC / 2 - q->offset);
    }

    // Normalize
    float norm = sqrtf((float)q->symbol_sz);
    if (isnormal(norm)) {
      srsran_vec_sc_prod_cfc(ptr, 1.0f / norm, ptr, SRSRAN_SSB_BW_SUBC);
    }
  }

  // Extract PSS LSE
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
  for (uint32_t l = 0; l < SRSRAN_SSB_DURATION_NSYMB; l++) {
    srsran_vec_apply_cfo(
        &ssb_grid[SRSRAN_SSB_BW_SUBC * l], delay_avg_norm, &ssb_grid[SRSRAN_SSB_BW_SUBC * l], SRSRAN_SSB_BW_SUBC);
  }

  // Extract LSE again
  if (srsran_pss_nr_extract_lse(ssb_grid, N_id_2, pss_lse) < SRSRAN_SUCCESS ||
      srsran_sss_nr_extract_lse(ssb_grid, N_id_1, N_id_2, sss_lse) < SRSRAN_SUCCESS) {
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
  float cfo_hz     = cargf(corr_pss * conjf(corr_sss)) / (2.0f * M_PI) * cfo_hz_max;

  // Compute average RSRP
  float rsrp = (SRSRAN_CSQABS(corr_pss) + SRSRAN_CSQABS(corr_sss)) / 2.0f;

  // Compute Noise
  float n0 = 1e-9; // Almost 0
  if (epre > rsrp) {
    n0 = epre - rsrp;
  }

  // Put measurements together
  meas->epre       = epre;
  meas->epre_dB    = srsran_convert_power_to_dB(epre);
  meas->rsrp       = rsrp;
  meas->epre_dB    = srsran_convert_power_to_dB(rsrp);
  meas->n0         = n0;
  meas->n0_dB      = srsran_convert_power_to_dB(n0);
  meas->snr_dB     = meas->rsrp_dB - meas->n0_dB;
  meas->cfo_hz     = cfo_hz;
  meas->cfo_hz_max = cfo_hz_max;
  meas->delay_us   = delay_avg_us; // Convert the delay to microseconds
  meas->nof_re     = SRSRAN_PSS_NR_LEN + SRSRAN_SSS_NR_LEN;

  return SRSRAN_SUCCESS;
}