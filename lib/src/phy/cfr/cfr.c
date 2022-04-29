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

#include "srsran/phy/cfr/cfr.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

// Uncomment this to use a literal implementation of the CFR algorithm
// #define CFR_PEAK_EXTRACTION

// Uncomment this to filter by zeroing the FFT bins instead of applying a frequency window
#define CFR_LPF_WITH_ZEROS

static inline float cfr_symb_peak(float* in_abs, int len);

void srsran_cfr_process(srsran_cfr_t* q, cf_t* in, cf_t* out)
{
  if (q == NULL || in == NULL || out == NULL) {
    return;
  }
  if (!q->cfg.cfr_enable) {
    // If no processing, copy the input samples into the output buffer
    if (in != out) {
      srsran_vec_cf_copy(out, in, q->cfg.symbol_sz);
    }
    return;
  }

  const float    alpha     = q->cfg.alpha;
  const uint32_t symbol_sz = q->cfg.symbol_sz;
  float          beta      = 0.0f;

  // Calculate absolute input values
  srsran_vec_abs_cf(in, q->abs_buffer_in, symbol_sz);

  // In auto modes, the beta threshold is calculated based on the measured PAPR
  if (q->cfg.cfr_mode == SRSRAN_CFR_THR_MANUAL) {
    beta = q->cfg.manual_thr;
  } else {
    const float symb_peak     = cfr_symb_peak(q->abs_buffer_in, q->cfg.symbol_sz);
    const float pwr_symb_peak = symb_peak * symb_peak;
    const float pwr_symb_avg  = srsran_vec_avg_power_ff(q->abs_buffer_in, q->cfg.symbol_sz);
    float       symb_papr     = 0.0f;

    if (isnormal(pwr_symb_avg) && isnormal(pwr_symb_peak)) {
      if (q->cfg.cfr_mode == SRSRAN_CFR_THR_AUTO_CMA) {
        // Once cma_n reaches its max value, stop incrementing to prevent overflow.
        // This turns the averaging into a de-facto EMA with an extremely slow time constant
        q->pwr_avg_in = SRSRAN_VEC_CMA(pwr_symb_avg, q->pwr_avg_in, q->cma_n++);
        q->cma_n      = q->cma_n & UINT64_MAX ? q->cma_n : UINT64_MAX;
      } else if (q->cfg.cfr_mode == SRSRAN_CFR_THR_AUTO_EMA) {
        q->pwr_avg_in = SRSRAN_VEC_EMA(pwr_symb_avg, q->pwr_avg_in, q->cfg.ema_alpha);
      }

      symb_papr = pwr_symb_peak / q->pwr_avg_in;
    }
    float papr_reduction = symb_papr / q->max_papr_lin;
    beta                 = (papr_reduction > 1) ? symb_peak / sqrtf(papr_reduction) : 0;
  }

  // Clipping algorithm
  if (isnormal(beta)) {
#ifdef CFR_PEAK_EXTRACTION
    srsran_vec_cf_zero(q->peak_buffer, symbol_sz);
    cf_t clip_thr = 0;
    for (int i = 0; i < symbol_sz; i++) {
      if (q->abs_buffer_in[i] > beta) {
        clip_thr          = beta * (in[i] / q->abs_buffer_in[i]);
        q->peak_buffer[i] = in[i] - clip_thr;
      }
    }

    // Apply FFT filter to the peak signal
    srsran_dft_run_c(&q->fft_plan, q->peak_buffer, q->peak_buffer);
#ifdef CFR_LPF_WITH_ZEROS
    srsran_vec_cf_zero(q->peak_buffer + q->lpf_bw / 2 + q->cfg.dc_sc, symbol_sz - q->cfg.symbol_bw - q->cfg.dc_sc);
#else  /* CFR_LPF_WITH_ZEROS */
    srsran_vec_prod_cfc(q->peak_buffer, q->lpf_spectrum, q->peak_buffer, symbol_sz);
#endif /* CFR_LPF_WITH_ZEROS */
    srsran_dft_run_c(&q->ifft_plan, q->peak_buffer, q->peak_buffer);

    // Scale the peak signal according to alpha
    srsran_vec_sc_prod_cfc(q->peak_buffer, alpha, q->peak_buffer, symbol_sz);

    // Apply the filtered clipping
    srsran_vec_sub_ccc(in, q->peak_buffer, out, symbol_sz);
#else /* CFR_PEAK_EXTRACTION */

    // Generate a clipping envelope and clip the signal
    srsran_vec_gen_clip_env(q->abs_buffer_in, beta, alpha, q->abs_buffer_in, symbol_sz);
    srsran_vec_prod_cfc(in, q->abs_buffer_in, out, symbol_sz);

    // FFT filter
    srsran_dft_run_c(&q->fft_plan, out, out);
#ifdef CFR_LPF_WITH_ZEROS
    srsran_vec_cf_zero(out + q->lpf_bw / 2 + q->cfg.dc_sc, symbol_sz - q->cfg.symbol_bw - q->cfg.dc_sc);
#else  /* CFR_LPF_WITH_ZEROS */
    srsran_vec_prod_cfc(out, q->lpf_spectrum, out, symbol_sz);
#endif /* CFR_LPF_WITH_ZEROS */
    srsran_dft_run_c(&q->ifft_plan, out, out);
#endif /* CFR_PEAK_EXTRACTION */

  } else {
    // If no processing, copy the input samples into the output buffer
    if (in != out) {
      srsran_vec_cf_copy(out, in, symbol_sz);
    }
  }
  if (q->cfg.cfr_mode != SRSRAN_CFR_THR_MANUAL && q->cfg.measure_out_papr) {
    srsran_vec_abs_cf(in, q->abs_buffer_out, symbol_sz);

    const float symb_peak     = cfr_symb_peak(q->abs_buffer_out, q->cfg.symbol_sz);
    const float pwr_symb_peak = symb_peak * symb_peak;
    const float pwr_symb_avg  = srsran_vec_avg_power_ff(q->abs_buffer_out, q->cfg.symbol_sz);
    float       symb_papr     = 0.0f;

    if (isnormal(pwr_symb_avg) && isnormal(pwr_symb_peak)) {
      if (q->cfg.cfr_mode == SRSRAN_CFR_THR_AUTO_CMA) {
        // Do not increment cma_n here, as it is being done when calculating input PAPR
        q->pwr_avg_out = SRSRAN_VEC_CMA(pwr_symb_avg, q->pwr_avg_out, q->cma_n);
      }

      else if (q->cfg.cfr_mode == SRSRAN_CFR_THR_AUTO_EMA) {
        q->pwr_avg_out = SRSRAN_VEC_EMA(pwr_symb_avg, q->pwr_avg_out, q->cfg.ema_alpha);
      }

      symb_papr = pwr_symb_peak / q->pwr_avg_out;
    }

    const float papr_out_db = srsran_convert_power_to_dB(symb_papr);
    printf("Output  PAPR: %f dB\n", papr_out_db);
  }
}

int srsran_cfr_init(srsran_cfr_t* q, srsran_cfr_cfg_t* cfg)
{
  int ret = SRSRAN_ERROR;
  if (q == NULL || cfg == NULL) {
    ERROR("Error, invalid inputs");
    ret = SRSRAN_ERROR_INVALID_INPUTS;
    goto clean_exit;
  }
  if (!cfg->symbol_sz || !cfg->symbol_bw || cfg->alpha < 0 || cfg->alpha > 1) {
    ERROR("Error, invalid configuration");
    goto clean_exit;
  }
  if (cfg->cfr_mode == SRSRAN_CFR_THR_INVALID) {
    ERROR("Error, invalid CFR mode");
    goto clean_exit;
  }
  if (cfg->cfr_mode == SRSRAN_CFR_THR_MANUAL && cfg->manual_thr <= 0) {
    ERROR("Error, invalid configuration for manual threshold");
    goto clean_exit;
  }
  if (cfg->cfr_mode == SRSRAN_CFR_THR_AUTO_CMA && (cfg->max_papr_db <= 0)) {
    ERROR("Error, invalid configuration for CMA averaging");
    goto clean_exit;
  }
  if (cfg->cfr_mode == SRSRAN_CFR_THR_AUTO_EMA &&
      (cfg->max_papr_db <= 0 || (cfg->ema_alpha < 0 || cfg->ema_alpha > 1))) {
    ERROR("Error, invalid configuration for EMA averaging");
    goto clean_exit;
  }

  // Copy all the configuration parameters
  q->cfg          = *cfg;
  q->max_papr_lin = srsran_convert_dB_to_power(q->cfg.max_papr_db);
  q->pwr_avg_in   = CFR_EMA_INIT_AVG_PWR;
  q->cma_n        = 0;

  if (q->cfg.measure_out_papr) {
    q->pwr_avg_out = CFR_EMA_INIT_AVG_PWR;
  }

  if (q->abs_buffer_in) {
    free(q->abs_buffer_in);
  }
  q->abs_buffer_in = srsran_vec_f_malloc(q->cfg.symbol_sz);
  if (!q->abs_buffer_in) {
    ERROR("Error allocating abs_buffer_in");
    goto clean_exit;
  }

  if (q->abs_buffer_out) {
    free(q->abs_buffer_out);
  }
  q->abs_buffer_out = srsran_vec_f_malloc(q->cfg.symbol_sz);
  if (!q->abs_buffer_out) {
    ERROR("Error allocating abs_buffer_out");
    goto clean_exit;
  }

  if (q->peak_buffer) {
    free(q->peak_buffer);
  }
  q->peak_buffer = srsran_vec_cf_malloc(q->cfg.symbol_sz);
  if (!q->peak_buffer) {
    ERROR("Error allocating peak_buffer");
    goto clean_exit;
  }

  // Allocate the filter
  if (q->lpf_spectrum) {
    free(q->lpf_spectrum);
  }
  q->lpf_spectrum = srsran_vec_f_malloc(q->cfg.symbol_sz);
  if (!q->lpf_spectrum) {
    ERROR("Error allocating lpf_spectrum");
    goto clean_exit;
  }

  // The LPF bandwidth is exactly the OFDM symbol bandwidth, in number of FFT bins
  q->lpf_bw = q->cfg.symbol_bw;

  // Initialise the filter
  srsran_vec_f_zero(q->lpf_spectrum, q->cfg.symbol_sz);

  // DC subcarrier is in position 0, so the OFDM symbol can go from index 1 to q->lpf_bw / 2 + 1
  for (uint32_t i = 0; i < q->lpf_bw / 2 + q->cfg.dc_sc; i++) {
    q->lpf_spectrum[i] = 1;
  }
  for (uint32_t i = q->cfg.symbol_sz - q->lpf_bw / 2; i < q->cfg.symbol_sz; i++) {
    q->lpf_spectrum[i] = 1;
  }

  // FFT plans, for 1 OFDM symbol
  if (q->fft_plan.size) {
    // Replan if it was initialised previously with bigger FFT size
    if (q->fft_plan.size >= q->cfg.symbol_sz) {
      if (srsran_dft_replan(&q->fft_plan, q->cfg.symbol_sz)) {
        ERROR("Replaning DFT plan");
        goto clean_exit;
      }
    } else {
      srsran_dft_plan_free(&q->fft_plan);
      if (srsran_dft_plan_c(&q->fft_plan, q->cfg.symbol_sz, SRSRAN_DFT_FORWARD)) {
        ERROR("Creating DFT plan");
        goto clean_exit;
      }
    }
  } else {
    // Create plan from zero otherwise
    if (srsran_dft_plan_c(&q->fft_plan, q->cfg.symbol_sz, SRSRAN_DFT_FORWARD)) {
      ERROR("Creating DFT plan");
      goto clean_exit;
    }
  }

  if (q->ifft_plan.size) {
    if (q->ifft_plan.size >= q->cfg.symbol_sz) {
      // Replan if it was initialised previously with bigger FFT size
      if (srsran_dft_replan(&q->ifft_plan, q->cfg.symbol_sz)) {
        ERROR("Replaning DFT plan");
        goto clean_exit;
      }
    } else {
      srsran_dft_plan_free(&q->ifft_plan);
      if (srsran_dft_plan_c(&q->ifft_plan, q->cfg.symbol_sz, SRSRAN_DFT_BACKWARD)) {
        ERROR("Creating DFT plan");
        goto clean_exit;
      }
    }
  } else {
    // Create plan from zero otherwise
    if (srsran_dft_plan_c(&q->ifft_plan, q->cfg.symbol_sz, SRSRAN_DFT_BACKWARD)) {
      ERROR("Creating DFT plan");
      goto clean_exit;
    }
  }

  srsran_dft_plan_set_norm(&q->fft_plan, true);
  srsran_dft_plan_set_norm(&q->ifft_plan, true);

  srsran_vec_cf_zero(q->peak_buffer, q->cfg.symbol_sz);
  srsran_vec_f_zero(q->abs_buffer_in, q->cfg.symbol_sz);
  srsran_vec_f_zero(q->abs_buffer_out, q->cfg.symbol_sz);
  ret = SRSRAN_SUCCESS;

clean_exit:
  if (ret < SRSRAN_SUCCESS) {
    srsran_cfr_free(q);
  }
  return ret;
}

void srsran_cfr_free(srsran_cfr_t* q)
{
  if (q) {
    srsran_dft_plan_free(&q->fft_plan);
    srsran_dft_plan_free(&q->ifft_plan);
    if (q->abs_buffer_in) {
      free(q->abs_buffer_in);
    }
    if (q->abs_buffer_out) {
      free(q->abs_buffer_out);
    }
    if (q->peak_buffer) {
      free(q->peak_buffer);
    }
    if (q->lpf_spectrum) {
      free(q->lpf_spectrum);
    }
    SRSRAN_MEM_ZERO(q, srsran_cfr_t, 1);
  }
}

// Find the peak absolute value of an OFDM symbol
static inline float cfr_symb_peak(float* in_abs, int len)
{
  const uint32_t max_index = srsran_vec_max_fi(in_abs, len);
  return in_abs[max_index];
}

bool srsran_cfr_params_valid(srsran_cfr_cfg_t* cfr_conf)
{
  if (cfr_conf == NULL) {
    return false;
  }
  if (cfr_conf->cfr_mode == SRSRAN_CFR_THR_INVALID) {
    return false;
  }
  if (cfr_conf->alpha < 0 || cfr_conf->alpha > 1) {
    return false;
  }
  if (cfr_conf->cfr_mode == SRSRAN_CFR_THR_MANUAL && cfr_conf->manual_thr <= 0) {
    return false;
  }
  if (cfr_conf->cfr_mode == SRSRAN_CFR_THR_AUTO_CMA && (cfr_conf->max_papr_db <= 0)) {
    return false;
  }
  if (cfr_conf->cfr_mode == SRSRAN_CFR_THR_AUTO_EMA &&
      (cfr_conf->max_papr_db <= 0 || (cfr_conf->ema_alpha < 0 || cfr_conf->ema_alpha > 1))) {
    return false;
  }
  return true;
}

int srsran_cfr_set_threshold(srsran_cfr_t* q, float thres)
{
  if (q == NULL) {
    ERROR("Invalid CFR object");
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
  if (thres <= 0.0f) {
    ERROR("Invalid CFR threshold");
    return SRSRAN_ERROR;
  }
  q->cfg.manual_thr = thres;
  return SRSRAN_SUCCESS;
}

int srsran_cfr_set_papr(srsran_cfr_t* q, float papr)
{
  if (q == NULL) {
    ERROR("Invalid CFR object");
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
  if (papr <= 0.0f) {
    ERROR("Invalid CFR configuration");
    return SRSRAN_ERROR;
  }
  q->cfg.max_papr_db = papr;
  q->max_papr_lin    = srsran_convert_dB_to_power(q->cfg.max_papr_db);
  return SRSRAN_SUCCESS;
}

srsran_cfr_mode_t srsran_cfr_str2mode(const char* mode_str)
{
  srsran_cfr_mode_t ret;
  if (strcmp(mode_str, "")) {
    if (!strcmp(mode_str, "manual")) {
      ret = SRSRAN_CFR_THR_MANUAL;
    } else if (!strcmp(mode_str, "auto_cma")) {
      ret = SRSRAN_CFR_THR_AUTO_CMA;
    } else if (!strcmp(mode_str, "auto_ema")) {
      ret = SRSRAN_CFR_THR_AUTO_EMA;
    } else {
      ret = SRSRAN_CFR_THR_INVALID; // mode_str is not recognised
    }
  } else {
    ret = SRSRAN_CFR_THR_INVALID; // mode_str is empty
  }
  return ret;
}
