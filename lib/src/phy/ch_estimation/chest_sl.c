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

#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "srsran/phy/ch_estimation/chest_sl.h"
#include "srsran/phy/common/zc_sequence.h"
#include "srsran/phy/mimo/precoding.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

static int chest_sl_init(srsran_chest_sl_t* q, uint32_t nof_cyclic_shift_seq)
{
  q->sf_n_re = SRSRAN_CP_NSYMB(q->cell.cp) * SRSRAN_NRE * q->cell.nof_prb * 2;

  srsran_interp_linear_vector_init(&q->lin_vec_sl, SRSRAN_MAX_PRB * SRSRAN_NRE);

  for (int i = 0; i < SRSRAN_SL_MAX_DMRS_SYMB; i++) {
    for (int j = 0; j < nof_cyclic_shift_seq; j++) {
      q->r_sequence[i][j] = srsran_vec_cf_malloc(SRSRAN_MAX_PRB * SRSRAN_NRE);
      if (!q->r_sequence[i][j]) {
        ERROR("Error allocating memory");
        return SRSRAN_ERROR;
      }
    }

    q->r_sequence_rx[i] = srsran_vec_cf_malloc(SRSRAN_MAX_PRB * SRSRAN_NRE);
    if (!q->r_sequence_rx[i]) {
      ERROR("Error allocating memory");
      return SRSRAN_ERROR;
    }
  }

  q->f_gh_pattern = srsran_vec_u32_malloc(SRSRAN_SL_MAX_DMRS_PERIOD_LENGTH); // MAX PERIOD LENGTH 320
  if (!q->f_gh_pattern) {
    ERROR("Error allocating memory");
    return SRSRAN_ERROR;
  }

  q->ce = srsran_vec_cf_malloc(2 * SRSRAN_CP_NSYMB(SRSRAN_CP_NORM) * SRSRAN_NRE * SRSRAN_MAX_PRB);
  if (!q->ce) {
    ERROR("Error allocating memory");
    return SRSRAN_ERROR;
  }

  q->ce_average = srsran_vec_cf_malloc(2 * SRSRAN_CP_NSYMB(SRSRAN_CP_NORM) * SRSRAN_NRE * SRSRAN_MAX_PRB);
  if (!q->ce_average) {
    ERROR("Error allocating memory");
    return SRSRAN_ERROR;
  }

  q->noise_tmp = srsran_vec_cf_malloc(2 * SRSRAN_CP_NSYMB(SRSRAN_CP_NORM) * SRSRAN_NRE * SRSRAN_MAX_PRB);
  if (!q->noise_tmp) {
    ERROR("Error allocating memory");
    return SRSRAN_ERROR;
  }

  q->sync_error_enable = true;
  q->rsrp_enable       = true;

  return SRSRAN_SUCCESS;
}

static int chest_sl_psbch_gen(srsran_chest_sl_t* q)
{
  // M_sc_rs - Reference Signal Length
  q->M_sc_rs = SRSRAN_PSBCH_NOF_PRB * SRSRAN_NRE;

  // Number of DMRS symbols
  if (q->cell.tm <= SRSRAN_SIDELINK_TM2) {
    q->nof_dmrs_symbols = SRSRAN_SL_TM12_DEFAULT_NUM_DMRS_SYMBOLS;
  } else {
    q->nof_dmrs_symbols = SRSRAN_PSBCH_TM34_NUM_DMRS_SYMBOLS;
  }

  // Cyclic Shift follows 36.211, Section 9.8
  for (int i = 0; i < q->nof_dmrs_symbols; i++) {
    q->n_CS[i] = (int)(q->cell.N_sl_id / 2) % 8;
  }

  // alpha - Reference Signal Cyclic Shift
  for (int i = 0; i < q->nof_dmrs_symbols; ++i) {
    q->alpha[i] = (2 * M_PI * q->n_CS[i]) / 12;
  }

  // Group Hopping
  uint32_t f_gh                       = 0; // Group Hopping Flag
  uint32_t f_ss                       = 0;
  uint32_t u[SRSRAN_SL_MAX_DMRS_SYMB] = {}; // Sequence Group Number

  // 36.211, Section 10.1.4.1.3 Base Sequence Number - always 0 for sidelink
  f_gh = 0;
  f_ss = (q->cell.N_sl_id / 16) % SRSRAN_SL_N_RU_SEQ;
  for (int ns = 0; ns < q->nof_dmrs_symbols; ns++) {
    u[ns] = (f_gh + f_ss) % SRSRAN_SL_N_RU_SEQ;
  }

  // w - Orthogonal Sequence, 36.211 Section 9.8
  if (q->cell.tm <= SRSRAN_SIDELINK_TM2) {
    if (q->cell.N_sl_id % 2) {
      q->w[0] = 1;
      q->w[1] = -1;
    } else {
      q->w[0] = 1;
      q->w[1] = 1;
    }
  } else {
    // TM3/4
    if (q->cell.N_sl_id % 2) {
      q->w[0] = 1;
      q->w[1] = -1;
      q->w[2] = 1;
    } else {
      q->w[0] = 1;
      q->w[1] = 1;
      q->w[2] = 1;
    }
  }

  for (int j = 0; j < q->nof_dmrs_symbols; j++) {
    cf_t* seq = q->r_sequence[j][0];

    // Generate R_uv sequences
    srsran_zc_sequence_generate_lte(u[j], SRSRAN_SL_BASE_SEQUENCE_NUMBER, q->alpha[0], q->M_sc_rs / SRSRAN_NRE, seq);

    // Apply w
    srsran_vec_sc_prod_ccc(seq, q->w[j], seq, q->M_sc_rs);
  }

  return SRSRAN_SUCCESS;
}

static int chest_sl_psbch_put_dmrs(srsran_chest_sl_t* q, cf_t* sf_buffer)
{
  uint32_t dmrs_idx = 0;
  uint32_t k        = q->cell.nof_prb * SRSRAN_NRE / 2 - 36;

  // Mapping to physical resources
  for (uint32_t i = 0; i < srsran_sl_get_num_symbols(q->cell.tm, q->cell.cp); i++) {
    if (srsran_psbch_is_symbol(SRSRAN_SIDELINK_DMRS_SYMBOL, q->cell.tm, i, q->cell.cp)) {
      memcpy(&sf_buffer[k + i * q->cell.nof_prb * SRSRAN_NRE], q->r_sequence[dmrs_idx][0], q->M_sc_rs * sizeof(cf_t));
      dmrs_idx++;
    }
  }

  return SRSRAN_SUCCESS;
}

static int chest_sl_psbch_get_dmrs(srsran_chest_sl_t* q, cf_t* sf_buffer, cf_t** dmrs_received)
{
  uint32_t dmrs_idx = 0;
  uint32_t k        = q->cell.nof_prb * SRSRAN_NRE / 2 - 36;

  // Get PSBCH DMRS symbols
  for (uint32_t i = 0; i < srsran_sl_get_num_symbols(q->cell.tm, q->cell.cp); i++) {
    if (srsran_psbch_is_symbol(SRSRAN_SIDELINK_DMRS_SYMBOL, q->cell.tm, i, q->cell.cp)) {
      memcpy(dmrs_received[dmrs_idx], &sf_buffer[k + i * q->cell.nof_prb * SRSRAN_NRE], q->M_sc_rs * sizeof(cf_t));
      dmrs_idx++;
    }
  }
  return dmrs_idx;
}

static void interpolate_pilots_sl_psbch(srsran_chest_sl_t* q)
{
  uint32_t NL                            = srsran_sl_get_num_symbols(q->cell.tm, q->cell.cp);
  uint32_t ce_l[SRSRAN_SL_MAX_DMRS_SYMB] = {};
  uint32_t n_re                          = q->cell.nof_prb * SRSRAN_NRE;

  uint32_t l_idx                      = 0;
  uint32_t L[SRSRAN_SL_MAX_DMRS_SYMB] = {};
  uint32_t k                          = 0;

  srsran_interp_linear_vector_resize(&q->lin_vec_sl, n_re);

  for (int i = 0; i < NL; i++) {
    if (srsran_psbch_is_symbol(SRSRAN_SIDELINK_DMRS_SYMBOL, q->cell.tm, i, q->cell.cp)) {
      L[l_idx] = i;
      l_idx++;
    }
  }

  // make sure l_idx is at least 2 to avoid accessing array at negative index below
  if (l_idx < 2) {
    ERROR("Couldn't interpolate PSBCH pilots. Invalid number of reference symbols.");
    return;
  }

  for (uint32_t n = 0; n < l_idx; n++) {
    ce_l[n] = SRSRAN_RE_IDX(q->cell.nof_prb, L[n], 0 * SRSRAN_NRE);
  }

  k = q->cell.nof_prb * SRSRAN_NRE / 2 - 36;

  srsran_interp_linear_vector3(&q->lin_vec_sl,
                               &q->ce[ce_l[1] + k],
                               &q->ce[ce_l[0] + k],
                               &q->ce[ce_l[0] + k],
                               &q->ce[ce_l[0] - n_re + k],
                               (L[1] - L[0]),
                               L[0],
                               false,
                               q->M_sc_rs);

  for (uint32_t n = 1; n < l_idx; n++) {
    srsran_interp_linear_vector3(&q->lin_vec_sl,
                                 &q->ce[ce_l[n - 1] + k],
                                 &q->ce[ce_l[n] + k],
                                 NULL,
                                 &q->ce[ce_l[n - 1] + n_re + k],
                                 (L[n] - L[n - 1]),
                                 (L[n] - L[n - 1]) - 1,
                                 true,
                                 q->M_sc_rs);
  }

  srsran_interp_linear_vector3(&q->lin_vec_sl,
                               &q->ce[ce_l[l_idx - 2] + k],
                               &q->ce[ce_l[l_idx - 1] + k],
                               &q->ce[ce_l[l_idx - 1] + k],
                               &q->ce[ce_l[l_idx - 1] + n_re + k],
                               (L[l_idx - 1] - L[l_idx - 2]),
                               (NL - L[l_idx - 1]) - 1,
                               true,
                               q->M_sc_rs);
}

static void chest_sl_psbch_ls_estimate(srsran_chest_sl_t* q, cf_t* sf_buffer)
{
  uint32_t dmrs_index  = 0;
  uint32_t k           = q->cell.nof_prb * SRSRAN_NRE / 2 - 36;
  uint32_t sf_nsymbols = srsran_sl_get_num_symbols(q->cell.tm, q->cell.cp);
  uint32_t sf_n_re     = SRSRAN_SF_LEN_RE(q->cell.nof_prb, q->cell.cp);

  // Get Pilot Estimates
  // Use the known DMRS signal to compute least-squares estimates
  srsran_vec_cf_zero(q->ce, sf_n_re);
  for (uint32_t i = 0; i < sf_nsymbols; i++) {
    if (srsran_psbch_is_symbol(SRSRAN_SIDELINK_DMRS_SYMBOL, q->cell.tm, i, q->cell.cp)) {
      srsran_vec_prod_conj_ccc(&sf_buffer[i * q->cell.nof_prb * SRSRAN_NRE + k],
                               &q->r_sequence[dmrs_index][0][0],
                               &q->ce[i * q->cell.nof_prb * SRSRAN_NRE + k],
                               q->M_sc_rs);
      dmrs_index++;
    }
  }

  interpolate_pilots_sl_psbch(q);
}

static int chest_sl_pscch_gen(srsran_chest_sl_t* q, uint32_t cyclic_shift)
{
  // M_sc_rs - Reference Signal Length
  if (q->cell.tm <= SRSRAN_SIDELINK_TM2) {
    q->M_sc_rs = SRSRAN_NRE;
  } else {
    q->M_sc_rs = SRSRAN_PSCCH_TM34_NOF_PRB * SRSRAN_NRE;
  }

  // Number of DMRS symbols
  if (q->cell.tm <= SRSRAN_SIDELINK_TM2) {
    q->nof_dmrs_symbols = SRSRAN_SL_TM12_DEFAULT_NUM_DMRS_SYMBOLS;
  } else {
    q->nof_dmrs_symbols = SRSRAN_SL_TM34_DEFAULT_NUM_DMRS_SYMBOLS;
  }

  // Cyclic Shift follows 36.211, Section 9.8
  if (q->cell.tm <= SRSRAN_SIDELINK_TM2) {
    for (int i = 0; i < q->nof_dmrs_symbols; i++) {
      q->n_CS[i] = 0;
    }
  } else {
    for (int i = 0; i < q->nof_dmrs_symbols; i++) {
      q->n_CS[i] = cyclic_shift;
    }
  }

  // alpha - Reference Signal Cyclic Shift
  for (int i = 0; i < q->nof_dmrs_symbols; ++i) {
    q->alpha[i] = (2 * M_PI * q->n_CS[i]) / 12;
  }

  // Group Hopping
  uint32_t f_gh                       = 0; // Group Hopping Flag
  uint32_t f_ss                       = 0;
  uint32_t u[SRSRAN_SL_MAX_DMRS_SYMB] = {}; // Sequence Group Number

  // 36.211, Section 10.1.4.1.3 Base Sequence Number - always 0 for sidelink
  f_gh = 0;
  if (q->cell.tm <= SRSRAN_SIDELINK_TM2) {
    f_ss = 0;
  } else {
    f_ss = 8;
  }
  for (int ns = 0; ns < q->nof_dmrs_symbols; ns++) {
    u[ns] = (f_gh + f_ss) % SRSRAN_SL_N_RU_SEQ;
  }

  // w - Orthogonal Sequence, 36.211 Section 9.8
  if (q->cell.tm <= SRSRAN_SIDELINK_TM2) {
    q->w[0] = 1;
    q->w[1] = 1;
  } else {
    q->w[0] = 1;
    q->w[1] = 1;
    q->w[2] = 1;
    q->w[3] = 1;
  }

  for (int j = 0; j < q->nof_dmrs_symbols; j++) {
    cf_t* seq = q->r_sequence[j][cyclic_shift / 3];

    // Generate R_uv sequences
    srsran_zc_sequence_generate_lte(u[j], SRSRAN_SL_BASE_SEQUENCE_NUMBER, q->alpha[0], q->M_sc_rs / SRSRAN_NRE, seq);

    // Apply w
    srsran_vec_sc_prod_ccc(seq, q->w[j], seq, q->M_sc_rs);
  }

  return SRSRAN_SUCCESS;
}

static int chest_sl_pscch_put_dmrs(srsran_chest_sl_t* q, cf_t* sf_buffer)
{
  uint32_t dmrs_idx = 0;
  uint32_t k        = q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE;

  // Mapping to physical resources
  for (uint32_t i = 0; i < srsran_sl_get_num_symbols(q->cell.tm, q->cell.cp); i++) {
    if (srsran_pscch_is_symbol(SRSRAN_SIDELINK_DMRS_SYMBOL, q->cell.tm, i, q->cell.cp)) {
      memcpy(&sf_buffer[k + i * q->cell.nof_prb * SRSRAN_NRE],
             q->r_sequence[dmrs_idx][q->chest_sl_cfg.cyclic_shift / 3],
             q->M_sc_rs * sizeof(cf_t));
      dmrs_idx++;
    }
  }

  return SRSRAN_SUCCESS;
}

static int chest_sl_pscch_get_dmrs(srsran_chest_sl_t* q, cf_t* sf_buffer, cf_t** dmrs_received)
{
  uint32_t dmrs_idx = 0;
  uint32_t k        = q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE;

  // Get DMRSs
  for (uint32_t i = 0; i < srsran_sl_get_num_symbols(q->cell.tm, q->cell.cp); i++) {
    if (srsran_pscch_is_symbol(SRSRAN_SIDELINK_DMRS_SYMBOL, q->cell.tm, i, q->cell.cp)) {
      memcpy(dmrs_received[dmrs_idx], &sf_buffer[k + i * q->cell.nof_prb * SRSRAN_NRE], q->M_sc_rs * sizeof(cf_t));
      dmrs_idx++;
    }
  }
  return dmrs_idx;
}

static void interpolate_pilots_sl_pscch(srsran_chest_sl_t* q)
{
  uint32_t NL                            = srsran_sl_get_num_symbols(q->cell.tm, q->cell.cp);
  uint32_t n_re                          = q->cell.nof_prb * SRSRAN_NRE;
  uint32_t ce_l[SRSRAN_SL_MAX_DMRS_SYMB] = {};
  uint32_t l_idx                         = 0;
  uint32_t L[SRSRAN_SL_MAX_DMRS_SYMB]    = {};
  uint32_t k                             = 0;

  srsran_interp_linear_vector_resize(&q->lin_vec_sl, n_re);

  for (int i = 0; i < NL; i++) {
    if (srsran_pscch_is_symbol(SRSRAN_SIDELINK_DMRS_SYMBOL, q->cell.tm, i, q->cell.cp)) {
      L[l_idx] = i;
      l_idx++;
    }
  }

  // make sure l_idx is at least 2 to avoid accessing array at negative index below
  if (l_idx < 2) {
    ERROR("Couldn't interpolate PSCCH pilots. Invalid number of reference symbols.");
    return;
  }

  for (uint32_t n = 0; n < l_idx; n++) {
    ce_l[n] = SRSRAN_RE_IDX(q->cell.nof_prb, L[n], 0 * SRSRAN_NRE);
  }

  k = q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE;

  srsran_interp_linear_vector3(&q->lin_vec_sl,
                               &q->ce[ce_l[1] + k],
                               &q->ce[ce_l[0] + k],
                               &q->ce[ce_l[0] + k],
                               &q->ce[ce_l[0] - n_re + k],
                               (L[1] - L[0]),
                               L[0],
                               false,
                               q->M_sc_rs);

  for (uint32_t n = 1; n < l_idx; n++) {
    srsran_interp_linear_vector3(&q->lin_vec_sl,
                                 &q->ce[ce_l[n - 1] + k],
                                 &q->ce[ce_l[n] + k],
                                 NULL,
                                 &q->ce[ce_l[n - 1] + n_re + k],
                                 (L[n] - L[n - 1]),
                                 (L[n] - L[n - 1]) - 1,
                                 true,
                                 q->M_sc_rs);
  }

  srsran_interp_linear_vector3(&q->lin_vec_sl,
                               &q->ce[ce_l[l_idx - 2] + k],
                               &q->ce[ce_l[l_idx - 1] + k],
                               &q->ce[ce_l[l_idx - 1] + k],
                               &q->ce[ce_l[l_idx - 1] + n_re + k],
                               (L[l_idx - 1] - L[l_idx - 2]),
                               (NL - L[l_idx - 1]) - 1,
                               true,
                               q->M_sc_rs);
}

static void chest_sl_pscch_ls_estimate(srsran_chest_sl_t* q, cf_t* sf_buffer)
{
  // Get Pilot Estimates
  // Use the known DMRS signal to compute least-squares estimates
  srsran_vec_cf_zero(q->ce, q->sf_n_re);

  uint32_t dmrs_idx = 0;
  for (uint32_t i = 0; i < srsran_sl_get_num_symbols(q->cell.tm, q->cell.cp); i++) {
    if (srsran_pscch_is_symbol(SRSRAN_SIDELINK_DMRS_SYMBOL, q->cell.tm, i, q->cell.cp)) {
      srsran_vec_prod_conj_ccc(
          &sf_buffer[i * q->cell.nof_prb * SRSRAN_NRE + q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE],
          &q->r_sequence[dmrs_idx][q->chest_sl_cfg.cyclic_shift / 3][0],
          &q->ce[i * q->cell.nof_prb * SRSRAN_NRE + q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE],
          q->M_sc_rs);
      dmrs_idx++;
    }
  }

  interpolate_pilots_sl_pscch(q);
}

static int chest_sl_pssch_gen(srsran_chest_sl_t* q)
{
  // M_sc_rs - Reference Signal Length
  q->M_sc_rs = q->chest_sl_cfg.nof_prb * SRSRAN_NRE;

  // Number of DMRS symbols
  if (q->cell.tm <= SRSRAN_SIDELINK_TM2) {
    q->nof_dmrs_symbols = SRSRAN_SL_TM12_DEFAULT_NUM_DMRS_SYMBOLS;
  } else {
    q->nof_dmrs_symbols = SRSRAN_SL_TM34_DEFAULT_NUM_DMRS_SYMBOLS;
  }

  // Cyclic Shift follows 36.211, Section 9.8
  for (int i = 0; i < q->nof_dmrs_symbols; i++) {
    q->n_CS[i] = (int)(q->chest_sl_cfg.N_x_id / 2) % 8;
  }

  // alpha - Reference Signal Cyclic Shift
  for (int i = 0; i < q->nof_dmrs_symbols; ++i) {
    q->alpha[i] = (2 * M_PI * q->n_CS[i]) / 12;
  }

  // Group Hopping
  uint32_t f_gh                       = 0; // Group Hopping Flag
  uint32_t f_ss                       = 0;
  uint32_t u[SRSRAN_SL_MAX_DMRS_SYMB] = {}; // Sequence Group Number

  // 36.211, Section 10.1.4.1.3 Base Sequence Number - always 0 for sidelink
  srsran_sl_group_hopping_f_gh(q->f_gh_pattern, q->chest_sl_cfg.N_x_id);

  if (q->cell.tm <= SRSRAN_SIDELINK_TM2) {
    f_ss              = q->chest_sl_cfg.N_x_id % SRSRAN_SL_N_RU_SEQ;
    uint32_t delta_ss = 0;

    for (uint32_t ns = 0; ns < q->nof_dmrs_symbols; ns++) {
      f_gh  = q->f_gh_pattern[ns];
      u[ns] = (f_gh + f_ss + delta_ss) % SRSRAN_SL_N_RU_SEQ;
    }
  } else {
    // TM3/4
    f_ss              = (q->chest_sl_cfg.N_x_id / 16) % SRSRAN_SL_N_RU_SEQ;
    uint32_t delta_ss = 0;

    for (uint32_t ns = 0; ns < q->nof_dmrs_symbols; ns++) {
      f_gh  = q->f_gh_pattern[(2 * 2 * (q->chest_sl_cfg.sf_idx % 10)) + ns];
      u[ns] = (f_gh + f_ss + delta_ss) % SRSRAN_SL_N_RU_SEQ;
    }
  }

  // w - Orthogonal Sequence, 36.211 Section 9.8
  if (q->cell.tm <= SRSRAN_SIDELINK_TM2) {
    if (q->chest_sl_cfg.N_x_id % 2 == 0) {
      q->w[0] = 1;
      q->w[1] = 1;
    } else {
      q->w[0] = 1;
      q->w[1] = -1;
    }
  } else {
    // TM3/4
    if (q->chest_sl_cfg.N_x_id % 2 == 0) {
      q->w[0] = 1;
      q->w[1] = 1;
      q->w[2] = 1;
      q->w[3] = 1;
    } else {
      q->w[0] = 1;
      q->w[1] = -1;
      q->w[2] = 1;
      q->w[3] = -1;
    }
  }

  for (int j = 0; j < q->nof_dmrs_symbols; j++) {
    cf_t* seq = q->r_sequence[j][0];

    // Generate R_uv sequences
    srsran_zc_sequence_generate_lte(u[j], SRSRAN_SL_BASE_SEQUENCE_NUMBER, q->alpha[0], q->M_sc_rs / SRSRAN_NRE, seq);

    // Apply w
    srsran_vec_sc_prod_ccc(seq, q->w[j], seq, q->M_sc_rs);
  }

  return SRSRAN_SUCCESS;
}

static int chest_sl_pssch_put_dmrs(srsran_chest_sl_t* q, cf_t* sf_buffer)
{
  int      dmrs_idx = 0;
  uint32_t k        = q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE;

  // Mapping to physical resources
  for (int i = 0; i < srsran_sl_get_num_symbols(q->cell.tm, q->cell.cp); i++) {
    if (srsran_pssch_is_symbol(SRSRAN_SIDELINK_DMRS_SYMBOL, q->cell.tm, i, q->cell.cp)) {
      if (q->cell.tm == SRSRAN_SIDELINK_TM1 || q->cell.tm == SRSRAN_SIDELINK_TM2) {
        if (q->chest_sl_cfg.nof_prb <= q->sl_comm_resource_pool.prb_num) {
          memcpy(&sf_buffer[k + i * q->cell.nof_prb * SRSRAN_NRE],
                 q->r_sequence[dmrs_idx][0],
                 sizeof(cf_t) * (SRSRAN_NRE * q->chest_sl_cfg.nof_prb));
        } else {
          // First band
          k = q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE;
          memcpy(&sf_buffer[k + i * q->cell.nof_prb * SRSRAN_NRE],
                 q->r_sequence[dmrs_idx][0],
                 sizeof(cf_t) * (SRSRAN_NRE * q->sl_comm_resource_pool.prb_num));

          // Second band
          if ((q->sl_comm_resource_pool.prb_num * 2) >
              (q->sl_comm_resource_pool.prb_end - q->sl_comm_resource_pool.prb_start + 1)) {
            k = (q->sl_comm_resource_pool.prb_end + 1 - q->sl_comm_resource_pool.prb_num + 1) * SRSRAN_NRE;
          } else {
            k = (q->sl_comm_resource_pool.prb_end + 1 - q->sl_comm_resource_pool.prb_num) * SRSRAN_NRE;
          }
          memcpy(&sf_buffer[k + i * q->cell.nof_prb * SRSRAN_NRE],
                 &q->r_sequence[dmrs_idx][0][SRSRAN_NRE * q->sl_comm_resource_pool.prb_num],
                 sizeof(cf_t) * (SRSRAN_NRE * (q->chest_sl_cfg.nof_prb - q->sl_comm_resource_pool.prb_num)));
        }
      } else if (q->cell.tm == SRSRAN_SIDELINK_TM3 || q->cell.tm == SRSRAN_SIDELINK_TM4) {
        memcpy(&sf_buffer[k + i * q->cell.nof_prb * SRSRAN_NRE],
               q->r_sequence[dmrs_idx][0],
               sizeof(cf_t) * (SRSRAN_NRE * q->chest_sl_cfg.nof_prb));
      }
      dmrs_idx++;
    }
  }
  return SRSRAN_SUCCESS;
}

static int chest_sl_pssch_get_dmrs(srsran_chest_sl_t* q, cf_t* sf_buffer, cf_t** dmrs_received)
{
  int      dmrs_idx = 0;
  uint32_t k        = q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE;

  // Mapping to physical resources
  for (int i = 0; i < srsran_sl_get_num_symbols(q->cell.tm, q->cell.cp); i++) {
    if (srsran_pssch_is_symbol(SRSRAN_SIDELINK_DMRS_SYMBOL, q->cell.tm, i, q->cell.cp)) {
      if (q->cell.tm == SRSRAN_SIDELINK_TM1 || q->cell.tm == SRSRAN_SIDELINK_TM2) {
        if (q->chest_sl_cfg.nof_prb <= q->sl_comm_resource_pool.prb_num) {
          memcpy(dmrs_received[dmrs_idx],
                 &sf_buffer[k + i * q->cell.nof_prb * SRSRAN_NRE],
                 sizeof(cf_t) * (SRSRAN_NRE * q->chest_sl_cfg.nof_prb));
        } else {
          // First band
          k = q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE;
          memcpy(dmrs_received[dmrs_idx],
                 &sf_buffer[k + i * q->cell.nof_prb * SRSRAN_NRE],
                 sizeof(cf_t) * (SRSRAN_NRE * q->sl_comm_resource_pool.prb_num));

          // Second band
          if ((q->sl_comm_resource_pool.prb_num * 2) >
              (q->sl_comm_resource_pool.prb_end - q->sl_comm_resource_pool.prb_start + 1)) {
            k = (q->sl_comm_resource_pool.prb_end + 1 - q->sl_comm_resource_pool.prb_num + 1) * SRSRAN_NRE;
          } else {
            k = (q->sl_comm_resource_pool.prb_end + 1 - q->sl_comm_resource_pool.prb_num) * SRSRAN_NRE;
          }
          memcpy(&dmrs_received[dmrs_idx][SRSRAN_NRE * q->sl_comm_resource_pool.prb_num],
                 &sf_buffer[k + i * q->cell.nof_prb * SRSRAN_NRE],
                 sizeof(cf_t) * (SRSRAN_NRE * (q->chest_sl_cfg.nof_prb - q->sl_comm_resource_pool.prb_num)));
        }
      } else if (q->cell.tm == SRSRAN_SIDELINK_TM3 || q->cell.tm == SRSRAN_SIDELINK_TM4) {
        memcpy(dmrs_received[dmrs_idx],
               &sf_buffer[k + i * q->cell.nof_prb * SRSRAN_NRE],
               sizeof(cf_t) * (SRSRAN_NRE * q->chest_sl_cfg.nof_prb));
      }
      dmrs_idx++;
    }
  }
  return SRSRAN_SUCCESS;
}

static void interpolate_pilots_sl_pssch(srsran_chest_sl_t* q)
{
  uint32_t NL                            = srsran_sl_get_num_symbols(q->cell.tm, q->cell.cp);
  uint32_t n_re                          = q->cell.nof_prb * SRSRAN_NRE;
  uint32_t ce_l[SRSRAN_SL_MAX_DMRS_SYMB] = {};
  uint32_t l_idx                         = 0;
  uint32_t L[SRSRAN_SL_MAX_DMRS_SYMB]    = {};
  uint32_t k                             = 0;

  srsran_interp_linear_vector_resize(&q->lin_vec_sl, n_re);

  for (int i = 0; i < NL; i++) {
    if (srsran_pssch_is_symbol(SRSRAN_SIDELINK_DMRS_SYMBOL, q->cell.tm, i, q->cell.cp)) {
      L[l_idx] = i;
      l_idx++;
    }
  }

  // make sure l_idx is at least 2 to avoid accessing array at negative index below
  if (l_idx < 2) {
    ERROR("Couldn't interpolate PSSCH pilots. Invalid number of reference symbols.");
    return;
  }

  for (uint32_t n = 0; n < l_idx; n++) {
    ce_l[n] = SRSRAN_RE_IDX(q->cell.nof_prb, L[n], 0 * SRSRAN_NRE);
  }

  if (q->cell.tm == SRSRAN_SIDELINK_TM1 || q->cell.tm == SRSRAN_SIDELINK_TM2) {
    if (q->chest_sl_cfg.nof_prb <= q->sl_comm_resource_pool.prb_num) {
      k = q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE;

      srsran_interp_linear_vector3(&q->lin_vec_sl,
                                   &q->ce[ce_l[1] + k],
                                   &q->ce[ce_l[0] + k],
                                   &q->ce[ce_l[0] + k],
                                   &q->ce[ce_l[0] - n_re + k],
                                   (L[1] - L[0]),
                                   L[0],
                                   false,
                                   q->chest_sl_cfg.nof_prb * SRSRAN_NRE);

      for (uint32_t n = 1; n < l_idx; n++) {
        srsran_interp_linear_vector3(&q->lin_vec_sl,
                                     &q->ce[ce_l[n - 1] + k],
                                     &q->ce[ce_l[n] + k],
                                     NULL,
                                     &q->ce[ce_l[n - 1] + n_re + k],
                                     (L[n] - L[n - 1]),
                                     (L[n] - L[n - 1]) - 1,
                                     true,
                                     q->chest_sl_cfg.nof_prb * SRSRAN_NRE);
      }

      srsran_interp_linear_vector3(&q->lin_vec_sl,
                                   &q->ce[ce_l[l_idx - 2] + k],
                                   &q->ce[ce_l[l_idx - 1] + k],
                                   &q->ce[ce_l[l_idx - 1] + k],
                                   &q->ce[ce_l[l_idx - 1] + n_re + k],
                                   (L[l_idx - 1] - L[l_idx - 2]),
                                   (NL - L[l_idx - 1]) - 1,
                                   true,
                                   q->chest_sl_cfg.nof_prb * SRSRAN_NRE);

    } else {
      // First band
      k = q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE;

      srsran_interp_linear_vector3(&q->lin_vec_sl,
                                   &q->ce[ce_l[1] + k],
                                   &q->ce[ce_l[0] + k],
                                   &q->ce[ce_l[0] + k],
                                   &q->ce[ce_l[0] - n_re + k],
                                   (L[1] - L[0]),
                                   L[0],
                                   false,
                                   q->sl_comm_resource_pool.prb_num * SRSRAN_NRE);

      for (uint32_t n = 1; n < l_idx; n++) {
        srsran_interp_linear_vector3(&q->lin_vec_sl,
                                     &q->ce[ce_l[n - 1] + k],
                                     &q->ce[ce_l[n] + k],
                                     NULL,
                                     &q->ce[ce_l[n - 1] + n_re + k],
                                     (L[n] - L[n - 1]),
                                     (L[n] - L[n - 1]) - 1,
                                     true,
                                     q->sl_comm_resource_pool.prb_num * SRSRAN_NRE);
      }

      srsran_interp_linear_vector3(&q->lin_vec_sl,
                                   &q->ce[ce_l[l_idx - 2] + k],
                                   &q->ce[ce_l[l_idx - 1] + k],
                                   &q->ce[ce_l[l_idx - 1] + k],
                                   &q->ce[ce_l[l_idx - 1] + n_re + k],
                                   (L[l_idx - 1] - L[l_idx - 2]),
                                   (NL - L[l_idx - 1]) - 1,
                                   true,
                                   q->sl_comm_resource_pool.prb_num * SRSRAN_NRE);

      // Second band
      if ((q->sl_comm_resource_pool.prb_num * 2) >
          (q->sl_comm_resource_pool.prb_end - q->sl_comm_resource_pool.prb_start + 1)) {
        k = (q->sl_comm_resource_pool.prb_end + 1 - q->sl_comm_resource_pool.prb_num + 1) * SRSRAN_NRE;
      } else {
        k = (q->sl_comm_resource_pool.prb_end + 1 - q->sl_comm_resource_pool.prb_num) * SRSRAN_NRE;
      }

      srsran_interp_linear_vector3(&q->lin_vec_sl,
                                   &q->ce[ce_l[1] + k],
                                   &q->ce[ce_l[0] + k],
                                   &q->ce[ce_l[0] + k],
                                   &q->ce[ce_l[0] - n_re + k],
                                   (L[1] - L[0]),
                                   L[0],
                                   false,
                                   (q->chest_sl_cfg.nof_prb - q->sl_comm_resource_pool.prb_num) * SRSRAN_NRE);

      for (uint32_t n = 1; n < l_idx; n++) {
        srsran_interp_linear_vector3(&q->lin_vec_sl,
                                     &q->ce[ce_l[n - 1] + k],
                                     &q->ce[ce_l[n] + k],
                                     NULL,
                                     &q->ce[ce_l[n - 1] + n_re + k],
                                     (L[n] - L[n - 1]),
                                     (L[n] - L[n - 1]) - 1,
                                     true,
                                     (q->chest_sl_cfg.nof_prb - q->sl_comm_resource_pool.prb_num) * SRSRAN_NRE);
      }

      srsran_interp_linear_vector3(&q->lin_vec_sl,
                                   &q->ce[ce_l[l_idx - 2] + k],
                                   &q->ce[ce_l[l_idx - 1] + k],
                                   &q->ce[ce_l[l_idx - 1] + k],
                                   &q->ce[ce_l[l_idx - 1] + n_re + k],
                                   (L[l_idx - 1] - L[l_idx - 2]),
                                   (NL - L[l_idx - 1]) - 1,
                                   true,
                                   (q->chest_sl_cfg.nof_prb - q->sl_comm_resource_pool.prb_num) * SRSRAN_NRE);
    }

  } else if (q->cell.tm == SRSRAN_SIDELINK_TM3 || q->cell.tm == SRSRAN_SIDELINK_TM4) {
    k = q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE;

    srsran_interp_linear_vector3(&q->lin_vec_sl,
                                 &q->ce[ce_l[1] + k],
                                 &q->ce[ce_l[0] + k],
                                 &q->ce[ce_l[0] + k],
                                 &q->ce[ce_l[0] - n_re + k],
                                 (L[1] - L[0]),
                                 L[0],
                                 false,
                                 q->chest_sl_cfg.nof_prb * SRSRAN_NRE);

    for (uint32_t n = 1; n < l_idx; n++) {
      srsran_interp_linear_vector3(&q->lin_vec_sl,
                                   &q->ce[ce_l[n - 1] + k],
                                   &q->ce[ce_l[n] + k],
                                   NULL,
                                   &q->ce[ce_l[n - 1] + n_re + k],
                                   (L[n] - L[n - 1]),
                                   (L[n] - L[n - 1]) - 1,
                                   true,
                                   q->chest_sl_cfg.nof_prb * SRSRAN_NRE);
    }

    srsran_interp_linear_vector3(&q->lin_vec_sl,
                                 &q->ce[ce_l[l_idx - 2] + k],
                                 &q->ce[ce_l[l_idx - 1] + k],
                                 &q->ce[ce_l[l_idx - 1] + k],
                                 &q->ce[ce_l[l_idx - 1] + n_re + k],
                                 (L[l_idx - 1] - L[l_idx - 2]),
                                 (NL - L[l_idx - 1]) - 1,
                                 true,
                                 q->chest_sl_cfg.nof_prb * SRSRAN_NRE);
  }
}

static void chest_sl_pssch_ls_estimate(srsran_chest_sl_t* q, cf_t* sf_buffer)
{
  int      dmrs_idx = 0;
  uint32_t k        = q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE;

  srsran_vec_cf_zero(q->ce, q->sf_n_re);
  for (int i = 0; i < srsran_sl_get_num_symbols(q->cell.tm, q->cell.cp); i++) {
    if (srsran_pssch_is_symbol(SRSRAN_SIDELINK_DMRS_SYMBOL, q->cell.tm, i, q->cell.cp)) {
      if (q->cell.tm == SRSRAN_SIDELINK_TM1 || q->cell.tm == SRSRAN_SIDELINK_TM2) {
        if (q->chest_sl_cfg.nof_prb <= q->sl_comm_resource_pool.prb_num) {
          srsran_vec_prod_conj_ccc(&sf_buffer[k + i * q->cell.nof_prb * SRSRAN_NRE],
                                   &q->r_sequence[dmrs_idx][0][0],
                                   &q->ce[k + i * q->cell.nof_prb * SRSRAN_NRE],
                                   (SRSRAN_NRE * q->chest_sl_cfg.nof_prb));
        } else {
          // First band
          k = q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE;
          srsran_vec_prod_conj_ccc(&sf_buffer[k + i * q->cell.nof_prb * SRSRAN_NRE],
                                   &q->r_sequence[dmrs_idx][0][0],
                                   &q->ce[k + i * q->cell.nof_prb * SRSRAN_NRE],
                                   (SRSRAN_NRE * q->sl_comm_resource_pool.prb_num));

          // Second band
          if ((q->sl_comm_resource_pool.prb_num * 2) >
              (q->sl_comm_resource_pool.prb_end - q->sl_comm_resource_pool.prb_start + 1)) {
            k = (q->sl_comm_resource_pool.prb_end + 1 - q->sl_comm_resource_pool.prb_num + 1) * SRSRAN_NRE;
          } else {
            k = (q->sl_comm_resource_pool.prb_end + 1 - q->sl_comm_resource_pool.prb_num) * SRSRAN_NRE;
          }
          srsran_vec_prod_conj_ccc(&sf_buffer[k + i * q->cell.nof_prb * SRSRAN_NRE],
                                   &q->r_sequence[dmrs_idx][0][SRSRAN_NRE * q->sl_comm_resource_pool.prb_num],
                                   &q->ce[k + i * q->cell.nof_prb * SRSRAN_NRE],
                                   (SRSRAN_NRE * (q->chest_sl_cfg.nof_prb - q->sl_comm_resource_pool.prb_num)));
        }
      } else if (q->cell.tm == SRSRAN_SIDELINK_TM3 || q->cell.tm == SRSRAN_SIDELINK_TM4) {
        srsran_vec_prod_conj_ccc(&sf_buffer[k + i * q->cell.nof_prb * SRSRAN_NRE],
                                 &q->r_sequence[dmrs_idx][0][0],
                                 &q->ce[k + i * q->cell.nof_prb * SRSRAN_NRE],
                                 (SRSRAN_NRE * q->chest_sl_cfg.nof_prb));
      }
      dmrs_idx++;
    }
  }
  interpolate_pilots_sl_pssch(q);
}

static void get_subband_noise(srsran_chest_sl_t* q, uint32_t k_start, uint32_t k_end, uint32_t sf_nsymbols)
{
  for (int k = k_start; k < k_end; k++) {
    cf_t aux = 0.0;
    for (int l = 0; l < sf_nsymbols / 2; l++) {
      aux = aux + q->ce[k + l * q->cell.nof_prb * SRSRAN_NRE];
    }
    for (int l = 0; l < sf_nsymbols / 2; l++) {
      q->ce_average[k + l * q->cell.nof_prb * SRSRAN_NRE] = aux / (float)sf_nsymbols * 2;
    }

    aux = 0.0;
    for (int l = sf_nsymbols / 2; l < sf_nsymbols; l++) {
      aux = aux + q->ce[k + l * q->cell.nof_prb * SRSRAN_NRE];
    }
    for (int l = sf_nsymbols / 2; l < sf_nsymbols; l++) {
      q->ce_average[k + l * q->cell.nof_prb * SRSRAN_NRE] = aux / (float)sf_nsymbols * 2;
    }
  }

  for (int l = 0; l < sf_nsymbols; l++) {
    srsran_vec_sub_ccc(&q->ce_average[k_start + l * q->cell.nof_prb * SRSRAN_NRE],
                       &q->ce[k_start + l * q->cell.nof_prb * SRSRAN_NRE],
                       &q->noise_tmp[k_start + l * q->cell.nof_prb * SRSRAN_NRE],
                       k_end - k_start);
    q->noise_estimated +=
        srsran_vec_avg_power_cf(&q->noise_tmp[k_start + l * q->cell.nof_prb * SRSRAN_NRE], k_end - k_start);
  }
}

float srsran_chest_sl_estimate_noise(srsran_chest_sl_t* q)
{
  uint32_t sf_nsymbols = srsran_sl_get_num_symbols(q->cell.tm, q->cell.cp);
  if (sf_nsymbols == 0) {
    ERROR("Error estimating channel noise. Invalid number of OFDM symbols.");
    return SRSRAN_ERROR;
  }

  srsran_vec_cf_zero(q->ce_average, q->sf_n_re);
  q->noise_estimated = 0.0;

  uint32_t k_start = 0;
  uint32_t k_end   = 0;

  switch (q->channel) {
    case SRSRAN_SIDELINK_PSBCH:
      k_start = q->cell.nof_prb * SRSRAN_NRE / 2 - 36;
      k_end   = k_start + q->M_sc_rs;
      get_subband_noise(q, k_start, k_end, sf_nsymbols);
      break;
    case SRSRAN_SIDELINK_PSCCH:
      k_start = q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE;
      k_end   = k_start + q->M_sc_rs;
      get_subband_noise(q, k_start, k_end, sf_nsymbols);
      break;
    case SRSRAN_SIDELINK_PSSCH:
      if (q->cell.tm == SRSRAN_SIDELINK_TM1 || q->cell.tm == SRSRAN_SIDELINK_TM2) {
        if (q->chest_sl_cfg.nof_prb <= q->sl_comm_resource_pool.prb_num) {
          k_start = q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE;
          k_end   = (q->chest_sl_cfg.nof_prb + q->chest_sl_cfg.prb_start_idx) * SRSRAN_NRE;
          get_subband_noise(q, k_start, k_end, sf_nsymbols);
        } else {
          // First band
          k_start = q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE;
          k_end   = k_start + q->sl_comm_resource_pool.prb_num * SRSRAN_NRE;
          get_subband_noise(q, k_start, k_end, sf_nsymbols);

          // Second band
          if ((q->sl_comm_resource_pool.prb_num * 2) >
              (q->sl_comm_resource_pool.prb_end - q->sl_comm_resource_pool.prb_start + 1)) {
            k_start = (q->sl_comm_resource_pool.prb_end + 1 - q->sl_comm_resource_pool.prb_num + 1) * SRSRAN_NRE;
          } else {
            k_start = (q->sl_comm_resource_pool.prb_end + 1 - q->sl_comm_resource_pool.prb_num) * SRSRAN_NRE;
          }
          k_end = k_start + (q->chest_sl_cfg.nof_prb - q->sl_comm_resource_pool.prb_num) * SRSRAN_NRE;
          get_subband_noise(q, k_start, k_end, sf_nsymbols);
        }
      } else if (q->cell.tm == SRSRAN_SIDELINK_TM3 || q->cell.tm == SRSRAN_SIDELINK_TM4) {
        k_start = q->chest_sl_cfg.prb_start_idx * SRSRAN_NRE;
        k_end   = (q->chest_sl_cfg.nof_prb + q->chest_sl_cfg.prb_start_idx) * SRSRAN_NRE;
        get_subband_noise(q, k_start, k_end, sf_nsymbols);
      }
      break;
    default:
      ERROR("Invalid Sidelink channel");
      return SRSRAN_ERROR;
  }
  q->noise_estimated = q->noise_estimated / (float)sf_nsymbols;
  return q->noise_estimated;
}

int srsran_chest_sl_init(srsran_chest_sl_t*             q,
                         srsran_sl_channels_t           channel,
                         srsran_cell_sl_t               cell,
                         srsran_sl_comm_resource_pool_t sl_comm_resource_pool)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  if (q != NULL) {
    bzero(q, sizeof(srsran_chest_sl_t));

    q->channel               = channel;
    q->cell                  = cell;
    q->sl_comm_resource_pool = sl_comm_resource_pool;

    switch (channel) {
      case SRSRAN_SIDELINK_PSBCH:
        if (chest_sl_init(q, SRSRAN_SL_DEFAULT_NOF_DMRS_CYCLIC_SHIFTS) != SRSRAN_SUCCESS) {
          ERROR("Error initializing sl dmrs");
          return SRSRAN_ERROR;
        }
        if (chest_sl_psbch_gen(q) != SRSRAN_SUCCESS) {
          ERROR("Error in chest PSBCH gen");
          return SRSRAN_ERROR;
        }
        break;
      case SRSRAN_SIDELINK_PSCCH:
        if (q->cell.tm <= SRSRAN_SIDELINK_TM2) {
          if (chest_sl_init(q, SRSRAN_SL_DEFAULT_NOF_DMRS_CYCLIC_SHIFTS) != SRSRAN_SUCCESS) {
            ERROR("Error initializing sl dmrs");
            return SRSRAN_ERROR;
          }
          // Generate DMRS sequence for PSCCH cyclic shift: {0}
          if (chest_sl_pscch_gen(q, 0) != SRSRAN_SUCCESS) {
            ERROR("Error in chest PSCCH gen");
            return SRSRAN_ERROR;
          }
        } else {
          if (chest_sl_init(q, SRSRAN_SL_MAX_PSCCH_NOF_DMRS_CYCLIC_SHIFTS) != SRSRAN_SUCCESS) {
            ERROR("Error initializing sl dmrs");
            return SRSRAN_ERROR;
          }
          // Generate DMRS sequences for all PSCCH cyclic shifts: {0,3,6,9}
          for (int i = 0; i <= 9; i += 3) {
            if (chest_sl_pscch_gen(q, i) != SRSRAN_SUCCESS) {
              ERROR("Error in chest PSCCH gen");
              return SRSRAN_ERROR;
            }
          }
        }
        break;
      case SRSRAN_SIDELINK_PSSCH:
        if (chest_sl_init(q, SRSRAN_SL_DEFAULT_NOF_DMRS_CYCLIC_SHIFTS) != SRSRAN_SUCCESS) {
          ERROR("Error initializing sl dmrs");
          return SRSRAN_ERROR;
        }
        break;
      default:
        ERROR("Invalid Sidelink channel");
        return SRSRAN_ERROR;
    }
    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

float srsran_chest_sl_get_sync_error(srsran_chest_sl_t* q)
{
  // Estimate synchronization error
  if (q->sync_error_enable) {
    float k   = (float)srsran_symbol_sz(q->cell.nof_prb);
    float sum = 0.0f;
    srsran_chest_sl_get_dmrs(q, q->ce, q->r_sequence_rx);
    for (int i = 0; i < q->nof_dmrs_symbols; i++) {
      sum += srsran_vec_estimate_frequency(q->r_sequence_rx[i], q->M_sc_rs) * k;
    }
    q->sync_err = sum / q->nof_dmrs_symbols;
  } else {
    q->sync_err = NAN;
  }
  return q->sync_err;
}

float srsran_chest_sl_get_rsrp(srsran_chest_sl_t* q)
{
  // Compute RSRP for the channel estimates in this port
  if (q->rsrp_enable) {
    srsran_chest_sl_get_dmrs(q, q->ce, q->r_sequence_rx);
    for (int i = 0; i < q->nof_dmrs_symbols; i++) {
      cf_t  corr   = srsran_vec_acc_cc(q->r_sequence_rx[i], q->M_sc_rs) / q->M_sc_rs;
      float energy = __real__(corr * conjf(corr));
      q->rsrp_corr = energy;
    }
    q->rsrp_corr /= q->nof_dmrs_symbols;
  } else {
    q->rsrp_corr = NAN;
  }
  return q->rsrp_corr;
}

int srsran_chest_sl_set_cell(srsran_chest_sl_t* q, srsran_cell_sl_t cell)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  if (q != NULL) {
    q->cell = cell;
    if (q->channel == SRSRAN_SIDELINK_PSBCH) {
      if (chest_sl_psbch_gen(q) != SRSRAN_SUCCESS) {
        return SRSRAN_ERROR;
      }
    }
    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

int srsran_chest_sl_set_cfg(srsran_chest_sl_t* q, srsran_chest_sl_cfg_t chest_sl_cfg)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  if (q != NULL) {
    q->chest_sl_cfg = chest_sl_cfg;

    if (q->channel == SRSRAN_SIDELINK_PSSCH) {
      if (chest_sl_pssch_gen(q) != SRSRAN_SUCCESS) {
        return SRSRAN_ERROR;
      }
    }
    ret = SRSRAN_SUCCESS;
  }
  return ret;
}

int srsran_chest_sl_put_dmrs(srsran_chest_sl_t* q, cf_t* sf_buffer)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  if (q != NULL) {
    switch (q->channel) {
      case SRSRAN_SIDELINK_PSBCH:
        return chest_sl_psbch_put_dmrs(q, sf_buffer);
      case SRSRAN_SIDELINK_PSCCH:
        return chest_sl_pscch_put_dmrs(q, sf_buffer);
      case SRSRAN_SIDELINK_PSSCH:
        return chest_sl_pssch_put_dmrs(q, sf_buffer);
      default:
        ret = SRSRAN_ERROR;
        break;
    }
  }
  return ret;
}

int srsran_chest_sl_get_dmrs(srsran_chest_sl_t* q, cf_t* sf_buffer, cf_t** dmrs_received)
{
  int ret = SRSRAN_ERROR_INVALID_INPUTS;
  if (q != NULL) {
    switch (q->channel) {
      case SRSRAN_SIDELINK_PSBCH:
        return chest_sl_psbch_get_dmrs(q, sf_buffer, dmrs_received);
      case SRSRAN_SIDELINK_PSCCH:
        return chest_sl_pscch_get_dmrs(q, sf_buffer, dmrs_received);
      case SRSRAN_SIDELINK_PSSCH:
        return chest_sl_pssch_get_dmrs(q, sf_buffer, dmrs_received);
      default:
        ret = SRSRAN_ERROR;
        break;
    }
  }
  return ret;
}

void srsran_chest_sl_ls_estimate(srsran_chest_sl_t* q, cf_t* sf_buffer)
{
  switch (q->channel) {
    case SRSRAN_SIDELINK_PSBCH:
      return chest_sl_psbch_ls_estimate(q, sf_buffer);
    case SRSRAN_SIDELINK_PSCCH:
      return chest_sl_pscch_ls_estimate(q, sf_buffer);
    case SRSRAN_SIDELINK_PSSCH:
      return chest_sl_pssch_ls_estimate(q, sf_buffer);
    default:
      return;
  }
}

void srsran_chest_sl_ls_equalize(srsran_chest_sl_t* q, cf_t* sf_buffer, cf_t* equalized_sf_buffer)
{
  srsran_chest_sl_estimate_noise(q);

  // Perform channel equalization
  srsran_predecoding_single(sf_buffer, q->ce_average, equalized_sf_buffer, NULL, q->sf_n_re, 1.0, q->noise_estimated);
}

void srsran_chest_sl_ls_estimate_equalize(srsran_chest_sl_t* q, cf_t* sf_buffer, cf_t* equalized_sf_buffer)
{
  srsran_chest_sl_ls_estimate(q, sf_buffer);
  srsran_chest_sl_ls_equalize(q, sf_buffer, equalized_sf_buffer);
}

void srsran_chest_sl_free(srsran_chest_sl_t* q)
{
  if (q != NULL) {
    srsran_interp_linear_vector_free(&q->lin_vec_sl);

    for (int i = 0; i < SRSRAN_SL_MAX_DMRS_SYMB; i++) {
      for (int j = 0; j < SRSRAN_SL_MAX_PSCCH_NOF_DMRS_CYCLIC_SHIFTS; j++) {
        if (q->r_sequence[i][j]) {
          free(q->r_sequence[i][j]);
        }
      }
      if (q->r_sequence_rx[i]) {
        free(q->r_sequence_rx[i]);
      }
    }

    if (q->f_gh_pattern) {
      free(q->f_gh_pattern);
    }

    if (q->ce) {
      free(q->ce);
    }
    if (q->ce_average) {
      free(q->ce_average);
    }
    if (q->noise_tmp) {
      free(q->noise_tmp);
    }
  }
}
