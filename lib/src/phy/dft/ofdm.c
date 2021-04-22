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

#include "srsran/srsran.h"
#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/dft/dft.h"
#include "srsran/phy/dft/ofdm.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

/* Uncomment next line for avoiding Guru DFT call */
//#define AVOID_GURU

static int ofdm_init_mbsfn_(srsran_ofdm_t* q, srsran_ofdm_cfg_t* cfg, srsran_dft_dir_t dir)
{
  // If the symbol size is not given, calculate in function of the number of resource blocks
  if (cfg->symbol_sz == 0) {
    int symbol_sz_err = srsran_symbol_sz(cfg->nof_prb);
    if (symbol_sz_err <= SRSRAN_SUCCESS) {
      ERROR("Invalid number of PRB %d", cfg->nof_prb);
      return SRSRAN_ERROR;
    }
    cfg->symbol_sz = (uint32_t)symbol_sz_err;
  }

  if (q->max_prb > 0) {
    // The object was already initialised, update only resizing params
    q->cfg.cp        = cfg->cp;
    q->cfg.nof_prb   = cfg->nof_prb;
    q->cfg.symbol_sz = cfg->symbol_sz;
  } else {
    // Otherwise copy all parameters
    q->cfg = *cfg;
  }

  uint32_t    symbol_sz = q->cfg.symbol_sz;
  srsran_cp_t cp        = q->cfg.cp;
  srsran_sf_t sf_type   = q->cfg.sf_type;

  // Set OFDM object attributes
  q->nof_symbols       = SRSRAN_CP_NSYMB(cp);
  q->nof_symbols_mbsfn = SRSRAN_CP_NSYMB(SRSRAN_CP_EXT);
  q->nof_re            = cfg->nof_prb * SRSRAN_NRE;
  q->nof_guards        = (q->cfg.symbol_sz - q->nof_re) / 2U;
  q->slot_sz           = (uint32_t)SRSRAN_SLOT_LEN(q->cfg.symbol_sz);
  q->sf_sz             = (uint32_t)SRSRAN_SF_LEN(q->cfg.symbol_sz);

  // Plan MBSFN
  if (q->fft_plan.size) {
    // Replan if it was initialised previously
    if (srsran_dft_replan(&q->fft_plan, q->cfg.symbol_sz)) {
      ERROR("Reeplaning DFT plan");
      return SRSRAN_ERROR;
    }
  } else {
    // Create plan from zero otherwise
    if (srsran_dft_plan_c(&q->fft_plan, symbol_sz, dir)) {
      ERROR("Creating DFT plan");
      return SRSRAN_ERROR;
    }
  }

  // Reallocate temporal buffer only if the new number of resource blocks is bigger than initial
  if (q->cfg.nof_prb > q->max_prb) {
    // Free before reallocating if allocted
    if (q->tmp) {
      free(q->tmp);
      free(q->shift_buffer);
    }

#ifdef AVOID_GURU
    q->tmp = srsran_vec_cf_malloc(symbol_sz);
#else
    q->tmp = srsran_vec_cf_malloc(q->sf_sz);
#endif /* AVOID_GURU */
    if (!q->tmp) {
      perror("malloc");
      return SRSRAN_ERROR;
    }

    q->shift_buffer = srsran_vec_cf_malloc(q->sf_sz);
    if (!q->shift_buffer) {
      perror("malloc");
      return SRSRAN_ERROR;
    }

    q->window_offset_buffer = srsran_vec_cf_malloc(q->sf_sz);
    if (!q->window_offset_buffer) {
      perror("malloc");
      return SRSRAN_ERROR;
    }

    q->max_prb = cfg->nof_prb;
  }

#ifdef AVOID_GURU
  srsran_vec_cf_zero(q->tmp, symbol_sz);
#else
  uint32_t nof_prb = q->cfg.nof_prb;
  cf_t* in_buffer = q->cfg.in_buffer;
  cf_t* out_buffer = q->cfg.out_buffer;
  int cp1 = SRSRAN_CP_ISNORM(cp) ? SRSRAN_CP_LEN_NORM(0, symbol_sz) : SRSRAN_CP_LEN_EXT(symbol_sz);
  int cp2 = SRSRAN_CP_ISNORM(cp) ? SRSRAN_CP_LEN_NORM(1, symbol_sz) : SRSRAN_CP_LEN_EXT(symbol_sz);

  // Slides DFT window a fraction of cyclic prefix, it does not apply for the inverse-DFT
  if (isnormal(cfg->rx_window_offset)) {
    cfg->rx_window_offset = SRSRAN_MAX(0, cfg->rx_window_offset);   // Needs to be positive
    cfg->rx_window_offset = SRSRAN_MIN(100, cfg->rx_window_offset); // Needs to be below 100
    q->window_offset_n = (uint32_t)roundf((float)cp2 * cfg->rx_window_offset);

    for (uint32_t i = 0; i < symbol_sz; i++) {
      q->window_offset_buffer[i] = cexpf(I * M_PI * 2.0f * (float)q->window_offset_n * (float)i / (float)symbol_sz);
    }
  }

  // Zero temporal and input buffers always
  srsran_vec_cf_zero(q->tmp, q->sf_sz);

  if (dir == SRSRAN_DFT_BACKWARD) {
    srsran_vec_cf_zero(in_buffer, SRSRAN_SF_LEN_RE(nof_prb, cp));
  } else {
    srsran_vec_cf_zero(in_buffer, q->sf_sz);
  }

  for (int slot = 0; slot < 2; slot++) {
    // If Guru DFT was allocated, free
    if (q->fft_plan_sf[slot].size) {
      srsran_dft_plan_free(&q->fft_plan_sf[slot]);
    }

    // Create Tx/Rx plans
    if (dir == SRSRAN_DFT_FORWARD) {
      if (srsran_dft_plan_guru_c(&q->fft_plan_sf[slot],
                                 symbol_sz,
                                 dir,
                                 in_buffer + cp1 + q->slot_sz * slot - q->window_offset_n,
                                 q->tmp,
                                 1,
                                 1,
                                 SRSRAN_CP_NSYMB(cp),
                                 symbol_sz + cp2,
                                 symbol_sz)) {
        ERROR("Creating Guru DFT plan (%d)", slot);
        return SRSRAN_ERROR;
      }
    } else {
      if (srsran_dft_plan_guru_c(&q->fft_plan_sf[slot],
                                 symbol_sz,
                                 dir,
                                 q->tmp,
                                 out_buffer + cp1 + q->slot_sz * slot,
                                 1,
                                 1,
                                 SRSRAN_CP_NSYMB(cp),
                                 symbol_sz,
                                 symbol_sz + cp2)) {
        ERROR("Creating Guru inverse-DFT plan (%d)", slot);
        return SRSRAN_ERROR;
      }
    }
  }
#endif

  srsran_dft_plan_set_mirror(&q->fft_plan, true);

  DEBUG("Init %s symbol_sz=%d, nof_symbols=%d, cp=%s, nof_re=%d, nof_guards=%d",
        dir == SRSRAN_DFT_FORWARD ? "FFT" : "iFFT",
        q->cfg.symbol_sz,
        q->nof_symbols,
        q->cfg.cp == SRSRAN_CP_NORM ? "Normal" : "Extended",
        q->nof_re,
        q->nof_guards);

  // MBSFN logic
  if (sf_type == SRSRAN_SF_MBSFN) {
    q->mbsfn_subframe   = true;
    q->non_mbsfn_region = 2; // default set to 2
  } else {
    q->mbsfn_subframe = false;
  }

  // Set other parameters
  srsran_ofdm_set_freq_shift(q, q->cfg.freq_shift_f);
  srsran_dft_plan_set_norm(&q->fft_plan, q->cfg.normalize);
  srsran_dft_plan_set_dc(&q->fft_plan, (!cfg->keep_dc) && (!isnormal(q->cfg.freq_shift_f)));

  return SRSRAN_SUCCESS;
}

void srsran_ofdm_set_non_mbsfn_region(srsran_ofdm_t* q, uint8_t non_mbsfn_region)
{
  q->non_mbsfn_region = non_mbsfn_region;
}

void srsran_ofdm_free_(srsran_ofdm_t* q)
{
  srsran_dft_plan_free(&q->fft_plan);

#ifndef AVOID_GURU
  for (int slot = 0; slot < 2; slot++) {
    if (q->fft_plan_sf[slot].init_size) {
      srsran_dft_plan_free(&q->fft_plan_sf[slot]);
    }
  }
#endif

  if (q->tmp) {
    free(q->tmp);
  }
  if (q->shift_buffer) {
    free(q->shift_buffer);
  }
  if (q->window_offset_buffer) {
    free(q->window_offset_buffer);
  }
  SRSRAN_MEM_ZERO(q, srsran_ofdm_t, 1);
}

int srsran_ofdm_rx_init(srsran_ofdm_t* q, srsran_cp_t cp, cf_t* in_buffer, cf_t* out_buffer, uint32_t max_prb)
{
  bzero(q, sizeof(srsran_ofdm_t));

  srsran_ofdm_cfg_t cfg = {};
  cfg.cp                = cp;
  cfg.in_buffer         = in_buffer;
  cfg.out_buffer        = out_buffer;
  cfg.nof_prb           = max_prb;
  cfg.sf_type           = SRSRAN_SF_NORM;

  return ofdm_init_mbsfn_(q, &cfg, SRSRAN_DFT_FORWARD);
}

int srsran_ofdm_rx_init_mbsfn(srsran_ofdm_t* q, srsran_cp_t cp, cf_t* in_buffer, cf_t* out_buffer, uint32_t max_prb)
{
  bzero(q, sizeof(srsran_ofdm_t));

  srsran_ofdm_cfg_t cfg = {};
  cfg.cp                = cp;
  cfg.in_buffer         = in_buffer;
  cfg.out_buffer        = out_buffer;
  cfg.nof_prb           = max_prb;
  cfg.sf_type           = SRSRAN_SF_MBSFN;

  return ofdm_init_mbsfn_(q, &cfg, SRSRAN_DFT_FORWARD);
}

int srsran_ofdm_tx_init(srsran_ofdm_t* q, srsran_cp_t cp, cf_t* in_buffer, cf_t* out_buffer, uint32_t max_prb)
{
  bzero(q, sizeof(srsran_ofdm_t));

  srsran_ofdm_cfg_t cfg = {};
  cfg.cp                = cp;
  cfg.in_buffer         = in_buffer;
  cfg.out_buffer        = out_buffer;
  cfg.nof_prb           = max_prb;
  cfg.sf_type           = SRSRAN_SF_NORM;

  return ofdm_init_mbsfn_(q, &cfg, SRSRAN_DFT_BACKWARD);
}

int srsran_ofdm_tx_init_cfg(srsran_ofdm_t* q, srsran_ofdm_cfg_t* cfg)
{
  return ofdm_init_mbsfn_(q, cfg, SRSRAN_DFT_BACKWARD);
}

int srsran_ofdm_rx_init_cfg(srsran_ofdm_t* q, srsran_ofdm_cfg_t* cfg)
{
  return ofdm_init_mbsfn_(q, cfg, SRSRAN_DFT_FORWARD);
}

int srsran_ofdm_tx_init_mbsfn(srsran_ofdm_t* q, srsran_cp_t cp, cf_t* in_buffer, cf_t* out_buffer, uint32_t nof_prb)
{
  bzero(q, sizeof(srsran_ofdm_t));

  srsran_ofdm_cfg_t cfg = {};
  cfg.cp                = cp;
  cfg.in_buffer         = in_buffer;
  cfg.out_buffer        = out_buffer;
  cfg.nof_prb           = nof_prb;
  cfg.sf_type           = SRSRAN_SF_MBSFN;

  return ofdm_init_mbsfn_(q, &cfg, SRSRAN_DFT_BACKWARD);
}

int srsran_ofdm_rx_set_prb(srsran_ofdm_t* q, srsran_cp_t cp, uint32_t nof_prb)
{
  srsran_ofdm_cfg_t cfg = {};
  cfg.cp                = cp;
  cfg.nof_prb           = nof_prb;
  return ofdm_init_mbsfn_(q, &cfg, SRSRAN_DFT_FORWARD);
}

int srsran_ofdm_tx_set_prb(srsran_ofdm_t* q, srsran_cp_t cp, uint32_t nof_prb)
{
  srsran_ofdm_cfg_t cfg = {};
  cfg.cp                = cp;
  cfg.nof_prb           = nof_prb;
  return ofdm_init_mbsfn_(q, &cfg, SRSRAN_DFT_BACKWARD);
}

void srsran_ofdm_rx_free(srsran_ofdm_t* q)
{
  srsran_ofdm_free_(q);
}

/* Shifts the signal after the iFFT or before the FFT.
 * Freq_shift is relative to inter-carrier spacing.
 * Caution: This function shall not be called during run-time
 */
int srsran_ofdm_set_freq_shift(srsran_ofdm_t* q, float freq_shift)
{
  q->cfg.freq_shift_f = freq_shift;

  // Check if fft shift is required
  if (!isnormal(q->cfg.freq_shift_f)) {
    srsran_dft_plan_set_dc(&q->fft_plan, true);
    return SRSRAN_SUCCESS;
  }

  uint32_t    symbol_sz = q->cfg.symbol_sz;
  srsran_cp_t cp        = q->cfg.cp;

  cf_t* ptr = q->shift_buffer;
  for (uint32_t n = 0; n < SRSRAN_NOF_SLOTS_PER_SF; n++) {
    for (uint32_t i = 0; i < q->nof_symbols; i++) {
      uint32_t cplen = SRSRAN_CP_ISNORM(cp) ? SRSRAN_CP_LEN_NORM(i, symbol_sz) : SRSRAN_CP_LEN_EXT(symbol_sz);
      for (uint32_t t = 0; t < symbol_sz + cplen; t++) {
        ptr[t] = cexpf(I * 2 * M_PI * ((float)t - (float)cplen) * freq_shift / symbol_sz);
      }
      ptr += symbol_sz + cplen;
    }
  }

  /* Disable DC carrier addition */
  srsran_dft_plan_set_dc(&q->fft_plan, false);

  return SRSRAN_SUCCESS;
}

void srsran_ofdm_tx_free(srsran_ofdm_t* q)
{
  srsran_ofdm_free_(q);
}

void srsran_ofdm_rx_slot_ng(srsran_ofdm_t* q, cf_t* input, cf_t* output)
{
  uint32_t    symbol_sz = q->cfg.symbol_sz;
  srsran_cp_t cp        = q->cfg.cp;

  for (uint32_t i = 0; i < q->nof_symbols; i++) {
    input += SRSRAN_CP_ISNORM(cp) ? SRSRAN_CP_LEN_NORM(i, symbol_sz) : SRSRAN_CP_LEN_EXT(symbol_sz);
    input -= q->window_offset_n;
    srsran_dft_run_c(&q->fft_plan, input, q->tmp);
    memcpy(output, &q->tmp[q->nof_guards], q->nof_re * sizeof(cf_t));
    input += symbol_sz;
    output += q->nof_re;
  }
}

/* Transforms input samples into output OFDM symbols.
 * Performs FFT on a each symbol and removes CP.
 */
static void ofdm_rx_slot(srsran_ofdm_t* q, int slot_in_sf)
{
#ifdef AVOID_GURU
  srsran_ofdm_rx_slot_ng(
      q, q->cfg.in_buffer + slot_in_sf * q->slot_sz, q->cfg.out_buffer + slot_in_sf * q->nof_re * q->nof_symbols);
#else
  uint32_t nof_symbols = q->nof_symbols;
  uint32_t nof_re = q->nof_re;
  cf_t* output = q->cfg.out_buffer + slot_in_sf * nof_re * nof_symbols;
  uint32_t symbol_sz = q->cfg.symbol_sz;
  float norm = 1.0f / sqrtf(q->fft_plan.size);
  cf_t* tmp = q->tmp;
  uint32_t dc = (q->fft_plan.dc) ? 1 : 0;

  srsran_dft_run_guru_c(&q->fft_plan_sf[slot_in_sf]);

  for (int i = 0; i < q->nof_symbols; i++) {
    // Apply frequency domain window offset
    if (q->window_offset_n) {
      srsran_vec_prod_ccc(tmp, q->window_offset_buffer, tmp, symbol_sz);
    }

    // Perform FFT shift
    memcpy(output, tmp + symbol_sz - nof_re / 2, sizeof(cf_t) * nof_re / 2);
    memcpy(output + nof_re / 2, &tmp[dc], sizeof(cf_t) * nof_re / 2);

    // Normalize output
    if (q->fft_plan.norm) {
      srsran_vec_sc_prod_cfc(output, norm, output, nof_re);
    }

    tmp += symbol_sz;
    output += nof_re;
  }
#endif
}

static void ofdm_rx_slot_mbsfn(srsran_ofdm_t* q, cf_t* input, cf_t* output)
{
  uint32_t i;
  for (i = 0; i < q->nof_symbols_mbsfn; i++) {
    if (i == q->non_mbsfn_region) {
      input += SRSRAN_NON_MBSFN_REGION_GUARD_LENGTH(q->non_mbsfn_region, q->cfg.symbol_sz);
    }
    input += (i >= q->non_mbsfn_region) ? SRSRAN_CP_LEN_EXT(q->cfg.symbol_sz) : SRSRAN_CP_LEN_NORM(i, q->cfg.symbol_sz);
    srsran_dft_run_c(&q->fft_plan, input, q->tmp);
    memcpy(output, &q->tmp[q->nof_guards], q->nof_re * sizeof(cf_t));
    input += q->cfg.symbol_sz;
    output += q->nof_re;
  }
}

void srsran_ofdm_rx_slot_zerocopy(srsran_ofdm_t* q, cf_t* input, cf_t* output)
{
  uint32_t i;
  for (i = 0; i < q->nof_symbols; i++) {
    input +=
        SRSRAN_CP_ISNORM(q->cfg.cp) ? SRSRAN_CP_LEN_NORM(i, q->cfg.symbol_sz) : SRSRAN_CP_LEN_EXT(q->cfg.symbol_sz);
    srsran_dft_run_c_zerocopy(&q->fft_plan, input, q->tmp);
    memcpy(output, &q->tmp[q->cfg.symbol_sz / 2 + q->nof_guards], sizeof(cf_t) * q->nof_re / 2);
    memcpy(&output[q->nof_re / 2], &q->tmp[1], sizeof(cf_t) * q->nof_re / 2);
    input += q->cfg.symbol_sz;
    output += q->nof_re;
  }
}

void srsran_ofdm_rx_sf(srsran_ofdm_t* q)
{
  if (isnormal(q->cfg.freq_shift_f)) {
    srsran_vec_prod_ccc(q->cfg.in_buffer, q->shift_buffer, q->cfg.in_buffer, q->sf_sz);
  }
  if (!q->mbsfn_subframe) {
    for (uint32_t n = 0; n < SRSRAN_NOF_SLOTS_PER_SF; n++) {
      ofdm_rx_slot(q, n);
    }
  } else {
    ofdm_rx_slot_mbsfn(q, q->cfg.in_buffer, q->cfg.out_buffer);
    ofdm_rx_slot(q, 1);
  }
}

void srsran_ofdm_rx_sf_ng(srsran_ofdm_t* q, cf_t* input, cf_t* output)
{
  uint32_t n;
  if (isnormal(q->cfg.freq_shift_f)) {
    srsran_vec_prod_ccc(input, q->shift_buffer, input, q->sf_sz);
  }
  if (!q->mbsfn_subframe) {
    for (n = 0; n < SRSRAN_NOF_SLOTS_PER_SF; n++) {
      srsran_ofdm_rx_slot_ng(q, &input[n * q->slot_sz], &output[n * q->nof_re * q->nof_symbols]);
    }
  } else {
    ofdm_rx_slot_mbsfn(q, q->cfg.in_buffer, q->cfg.out_buffer);
    ofdm_rx_slot(q, 1);
  }
}

/* Transforms input OFDM symbols into output samples.
 * Performs FFT on a each symbol and adds CP.
 */
static void ofdm_tx_slot(srsran_ofdm_t* q, int slot_in_sf)
{
  uint32_t    symbol_sz = q->cfg.symbol_sz;
  srsran_cp_t cp        = q->cfg.cp;

  cf_t* input  = q->cfg.in_buffer + slot_in_sf * q->nof_re * q->nof_symbols;
  cf_t* output = q->cfg.out_buffer + slot_in_sf * q->slot_sz;

#ifdef AVOID_GURU
  for (int i = 0; i < q->nof_symbols; i++) {
    int cp_len = SRSRAN_CP_ISNORM(cp) ? SRSRAN_CP_LEN_NORM(i, symbol_sz) : SRSRAN_CP_LEN_EXT(symbol_sz);
    memcpy(&q->tmp[q->nof_guards], input, q->nof_re * sizeof(cf_t));
    srsran_dft_run_c(&q->fft_plan, q->tmp, &output[cp_len]);
    input += q->nof_re;
    /* add CP */
    memcpy(output, &output[symbol_sz], cp_len * sizeof(cf_t));
    output += symbol_sz + cp_len;
  }
#else
  uint32_t nof_symbols = q->nof_symbols;
  uint32_t nof_re = q->nof_re;
  float norm = 1.0f / sqrtf(symbol_sz);
  cf_t* tmp = q->tmp;

  bzero(tmp, q->slot_sz);
  uint32_t dc = (q->fft_plan.dc) ? 1 : 0;

  for (int i = 0; i < nof_symbols; i++) {
    memcpy(&tmp[dc], &input[nof_re / 2], nof_re / 2 * sizeof(cf_t));
    memcpy(&tmp[symbol_sz - nof_re / 2], &input[0], nof_re / 2 * sizeof(cf_t));

    input += nof_re;
    tmp += symbol_sz;
  }

  srsran_dft_run_guru_c(&q->fft_plan_sf[slot_in_sf]);

  for (int i = 0; i < nof_symbols; i++) {
    int cp_len = SRSRAN_CP_ISNORM(cp) ? SRSRAN_CP_LEN_NORM(i, symbol_sz) : SRSRAN_CP_LEN_EXT(symbol_sz);

    if (q->fft_plan.norm) {
      srsran_vec_sc_prod_cfc(&output[cp_len], norm, &output[cp_len], symbol_sz);
    }

    /* add CP */
    memcpy(output, &output[symbol_sz], cp_len * sizeof(cf_t));
    output += symbol_sz + cp_len;
  }
#endif
}

void ofdm_tx_slot_mbsfn(srsran_ofdm_t* q, cf_t* input, cf_t* output)
{
  uint32_t symbol_sz = q->cfg.symbol_sz;

  for (uint32_t i = 0; i < q->nof_symbols_mbsfn; i++) {
    int cp_len = (i > (q->non_mbsfn_region - 1)) ? SRSRAN_CP_LEN_EXT(symbol_sz) : SRSRAN_CP_LEN_NORM(i, symbol_sz);
    memcpy(&q->tmp[q->nof_guards], input, q->nof_re * sizeof(cf_t));
    srsran_dft_run_c(&q->fft_plan, q->tmp, &output[cp_len]);
    input += q->nof_re;
    /* add CP */
    memcpy(output, &output[symbol_sz], cp_len * sizeof(cf_t));
    output += symbol_sz + cp_len;

    /*skip the small section between the non mbms region and the mbms region*/
    if (i == (q->non_mbsfn_region - 1))
      output += SRSRAN_NON_MBSFN_REGION_GUARD_LENGTH(q->non_mbsfn_region, symbol_sz);
  }
}

void srsran_ofdm_set_normalize(srsran_ofdm_t* q, bool normalize_enable)
{
  srsran_dft_plan_set_norm(&q->fft_plan, normalize_enable);
}

void srsran_ofdm_tx_sf(srsran_ofdm_t* q)
{
  uint32_t n;
  if (!q->mbsfn_subframe) {
    for (n = 0; n < SRSRAN_NOF_SLOTS_PER_SF; n++) {
      ofdm_tx_slot(q, n);
    }
  } else {
    ofdm_tx_slot_mbsfn(q, q->cfg.in_buffer, q->cfg.out_buffer);
    ofdm_tx_slot(q, 1);
  }
  if (isnormal(q->cfg.freq_shift_f)) {
    srsran_vec_prod_ccc(q->cfg.out_buffer, q->shift_buffer, q->cfg.out_buffer, q->sf_sz);
  }
}
