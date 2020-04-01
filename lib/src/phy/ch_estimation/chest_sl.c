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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 #include "srslte/phy/ch_estimation/chest_sl.h"
#include "srslte/phy/mimo/precoding.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"
#include "ul_rs_tables.h"

int srslte_chest_sl_init_dmrs(srslte_chest_sl_t* q)
{
  srslte_interp_linear_vector_init(&q->lin_vec_sl, SRSLTE_MAX_PRB * SRSLTE_NRE);

  for (int i = 0; i < SRSLTE_SL_MAX_DMRS_SYMB; i++) {
    q->r[i] = srslte_vec_f_malloc(SRSLTE_MAX_PRB * SRSLTE_NRE);
    if (!q->r[i]) {
      ERROR("Error allocating memory");
      return SRSLTE_ERROR;
    }

    q->r_uv[i] = srslte_vec_cf_malloc(SRSLTE_MAX_PRB * SRSLTE_NRE);
    if (!q->r_uv[i]) {
      ERROR("Error allocating memory");
      return SRSLTE_ERROR;
    }

    q->r_sequence[i] = srslte_vec_cf_malloc(SRSLTE_MAX_PRB * SRSLTE_NRE);
    if (!q->r_sequence[i]) {
      ERROR("Error allocating memory");
      return SRSLTE_ERROR;
    }

    q->dmrs_received[i] = srslte_vec_cf_malloc(SRSLTE_MAX_PRB * SRSLTE_NRE);
    if (!q->dmrs_received[i]) {
      ERROR("Error allocating memory");
      return SRSLTE_ERROR;
    }
  }

  q->f_gh_pattern = srslte_vec_u32_malloc(SRSLTE_SL_MAX_DMRS_PERIOD_LENGTH); // MAX PERIOD LENGTH 320
  if (!q->f_gh_pattern) {
    ERROR("Error allocating memory");
    return SRSLTE_ERROR;
  }

  q->ce = srslte_vec_cf_malloc(2 * SRSLTE_CP_NSYMB(SRSLTE_CP_NORM) * SRSLTE_NRE * SRSLTE_MAX_PRB);
  if (!q->ce) {
    ERROR("Error allocating memory");
    return SRSLTE_ERROR;
  }

  q->sync_error_enable = true;
  q->rsrp_enable       = true;

  return SRSLTE_SUCCESS;
}

static uint32_t get_q(uint32_t u, uint32_t v, uint32_t N_sz)
{
  float q;
  float n_sz = (float)N_sz;

  float q_hat = n_sz * (u + 1) / 31;
  if ((((uint32_t)(2 * q_hat)) % 2) == 0) {
    q = q_hat + 0.5 + v;
  } else {
    q = q_hat + 0.5 - v;
  }
  return (uint32_t)q;
}

int srslte_chest_sl_gen_dmrs(srslte_chest_sl_t*   q,
                             srslte_sl_tm_t       tm,
                             srslte_sl_channels_t ch,
                             uint32_t             sf_idx,
                             uint32_t             N_sl_id,
                             uint32_t             L_crb,
                             uint32_t             N_x_id,
                             uint32_t             cyclic_shift)
{
  // M_sc_rs - Reference Signal Length
  switch (ch) {
    case SRSLTE_SIDELINK_PSBCH:
      q->M_sc_rs = SRSLTE_PSBCH_NOF_PRB * SRSLTE_NRE;
      break;
    case SRSLTE_SIDELINK_PSCCH:
      q->M_sc_rs = SRSLTE_NRE;
      if (tm == SRSLTE_SIDELINK_TM3 || tm == SRSLTE_SIDELINK_TM4) {
        q->M_sc_rs = SRSLTE_PSCCH_TM34_NOF_PRB * SRSLTE_NRE;
      }
      break;
    case SRSLTE_SIDELINK_PSSCH:
      q->M_sc_rs = L_crb * SRSLTE_NRE;
      break;
    case SRSLTE_SIDELINK_PSDCH:
      printf("channel not supported yet\n\n");
      break;
    default:
      printf("channel not supported\n\n");
      return SRSLTE_ERROR;
  }

  // Number of DMRS symbols
  if (tm <= SRSLTE_SIDELINK_TM2) {
    q->nof_dmrs_symbols = SRSLTE_SL_TM12_DEFAULT_NUM_DMRS_SYMBOLS;
  } else {
    if (ch == SRSLTE_SIDELINK_PSBCH) {
      // only PSBCH has fewer symbols
      q->nof_dmrs_symbols = SRSLTE_PSBCH_TM34_NUM_DMRS_SYMBOLS;
    } else {
      // all other channels have 4 DMRS
      q->nof_dmrs_symbols = SRSLTE_SL_TM34_DEFAULT_NUM_DMRS_SYMBOLS;
    }
  }

  // Cyclic Shift
  // 36.211, Section 9.8
  switch (ch) {
    case SRSLTE_SIDELINK_PSBCH:
      for (int i = 0; i < q->nof_dmrs_symbols; i++) {
        q->n_CS[i] = (int)(N_sl_id / 2) % 8;
      }
      break;
    case SRSLTE_SIDELINK_PSCCH:
      if (tm <= SRSLTE_SIDELINK_TM2) {
        for (int i = 0; i < q->nof_dmrs_symbols; i++) {
          q->n_CS[i] = 0;
        }
      } else {
        for (int i = 0; i < q->nof_dmrs_symbols; i++) {
          // All DMRS symbols have the same cyclic shift
          q->n_CS[i] = cyclic_shift;
        }
      }
      break;
    case SRSLTE_SIDELINK_PSSCH:
      if (tm <= SRSLTE_SIDELINK_TM2) {
        for (int i = 0; i < q->nof_dmrs_symbols; i++) {
          q->n_CS[i] = ((int)floor(N_x_id / 2)) % 8;
        }
      } else {
        // TODO: both equation are the same here but spec says N_id_X for Mode3+4
        for (int i = 0; i < q->nof_dmrs_symbols; i++) {
          q->n_CS[i] = (int)(N_x_id / 2) % 8;
        }
      }
      break;
    case SRSLTE_SIDELINK_PSDCH:
      for (int i = 0; i < q->nof_dmrs_symbols; i++) {
        q->n_CS[i] = 0;
      }
      break;
  }

  // alpha - Reference Signal Cyclic Shift
  for (int i = 0; i < q->nof_dmrs_symbols; ++i) {
    q->alpha[i] = (2 * M_PI * q->n_CS[i]) / 12;
  }

  // Group Hopping
  uint32_t f_gh = 0; // Group Hopping Flag
  uint32_t f_ss = 0;
  uint32_t u[SRSLTE_SL_MAX_DMRS_SYMB]; // Sequence Group Number

  // 36.211, Section 10.1.4.1.3
  // Base Sequence Number - always 0 for sidelink
  switch (ch) {
    case SRSLTE_SIDELINK_PSBCH:
      f_ss = (N_sl_id / 16) % SRSLTE_SL_N_RU_SEQ;
      for (int ns = 0; ns < q->nof_dmrs_symbols; ns++) {
        u[ns] = (f_gh + f_ss) % SRSLTE_SL_N_RU_SEQ;
      }
      break;
    case SRSLTE_SIDELINK_PSCCH:
      if (tm <= SRSLTE_SIDELINK_TM2) {
        f_ss = 0;
      } else {
        f_ss = 8;
      }
      for (int ns = 0; ns < q->nof_dmrs_symbols; ns++) {
        u[ns] = (f_gh + f_ss) % SRSLTE_SL_N_RU_SEQ;
      }
      break;
    case SRSLTE_SIDELINK_PSSCH:
      srslte_group_hopping_f_gh(q->f_gh_pattern, N_x_id);

      if (tm <= SRSLTE_SIDELINK_TM2) {
        f_ss              = N_x_id % SRSLTE_SL_N_RU_SEQ;
        uint32_t delta_ss = 0;

        for (uint32_t ns = 0; ns < q->nof_dmrs_symbols; ns++) {
          f_gh = q->f_gh_pattern[ns];
          u[ns]        = (f_gh + f_ss + delta_ss) % SRSLTE_SL_N_RU_SEQ;
        }
      } else {
        // TM3/4
        f_ss              = (N_x_id / 16) % SRSLTE_SL_N_RU_SEQ;
        uint32_t delta_ss = 0;

        for (uint32_t ns = 0; ns < q->nof_dmrs_symbols; ns++) {
          f_gh  = q->f_gh_pattern[(2 * 2 * (sf_idx % 10)) + ns];
          u[ns] = (f_gh + f_ss + delta_ss) % SRSLTE_SL_N_RU_SEQ;
        }
      }
      break;
    case SRSLTE_SIDELINK_PSDCH:
      for (int ns = 0; ns < q->nof_dmrs_symbols; ns++) {
        u[ns] = (f_gh + f_ss) % SRSLTE_SL_N_RU_SEQ;
      }
      break;
  }

  // N_zc - Zadoff Chu Sequence Length
  int32_t N_zc = 0; // N_zc - Zadoff Chu Sequence Length
  // TODO: the refsignal_ul.c should be reused for this, code looks almost the same
  switch (q->M_sc_rs / SRSLTE_NRE) {
    case 1:
      for (int j = 0; j < q->nof_dmrs_symbols; ++j) {
        for (int i = 0; i < SRSLTE_NRE; i++) {
          q->r[j][i] = phi_M_sc_12[u[j]][i] * M_PI / 4;
        }
      }
      break;
    case 2:
      for (int j = 0; j < q->nof_dmrs_symbols; ++j) {
        for (int i = 0; i < q->M_sc_rs; i++) {
          q->r[j][i] = phi_M_sc_24[u[j]][i] * M_PI / 4;
        }
      }
      break;
    default:
      for (uint32_t i = NOF_PRIME_NUMBERS - 1; i > 0; i--) {
        if (prime_numbers[i] < q->M_sc_rs) {
          N_zc = prime_numbers[i];
          break;
        }
      }
      for (int j = 0; j < q->nof_dmrs_symbols; ++j) {
        q->q[j]    = get_q(u[j], SRSLTE_SL_BASE_SEQUENCE_NUMBER, N_zc);
        float n_sz = (float)N_zc;
        for (uint32_t i = 0; i < q->M_sc_rs; i++) {
          float m    = (float)(i % N_zc);
          q->r[j][i] = -M_PI * q->q[j] * m * (m + 1) / n_sz;
        }
      }
      break;
  }

  // Do complex exponential and adjust amplitude
  // 36.211, Section 5.5.1
  for (int j = 0; j < q->nof_dmrs_symbols; ++j) {
    for (int i = 0; i < q->M_sc_rs; i++) {
      q->r_uv[j][i] = cexpf(I * (q->r[j][i] + q->alpha[0] * i));
    }
  }

  // w - Orthogonal Sequence
  // 36.211, Section 9.8
  switch (ch) {
    case SRSLTE_SIDELINK_PSBCH:
    case SRSLTE_SIDELINK_PSDCH:
      if (tm <= SRSLTE_SIDELINK_TM2) {
        if (N_sl_id % 2) {
          q->w[0] = 1;
          q->w[1] = -1;
        } else {
          q->w[0] = 1;
          q->w[1] = 1;
        }
      } else {
        // TM3/4
        if (N_sl_id % 2) {
          q->w[0] = 1;
          q->w[1] = -1;
          q->w[2] = 1;
        } else {
          q->w[0] = 1;
          q->w[1] = 1;
          q->w[2] = 1;
        }
      }
      break;
    case SRSLTE_SIDELINK_PSCCH:
      if (tm <= SRSLTE_SIDELINK_TM2) {
        q->w[0] = 1;
        q->w[1] = 1;
      } else {
        q->w[0] = 1;
        q->w[1] = 1;
        q->w[2] = 1;
        q->w[3] = 1;
      }
      break;
    case SRSLTE_SIDELINK_PSSCH:
      if (tm <= SRSLTE_SIDELINK_TM2) {
        if (N_x_id % 2 == 0) {
          q->w[0] = 1;
          q->w[1] = 1;
        } else {
          q->w[0] = 1;
          q->w[1] = -1;
        }
      } else {
        // TM3/4
        if (N_x_id % 2 == 0) {
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
      break;
    default:
      return SRSLTE_ERROR;
  }

  for (int j = 0; j < q->nof_dmrs_symbols; j++) {
    for (int i = 0; i < q->M_sc_rs; i++) {
      q->r_sequence[j][i] = q->w[j] * q->r_uv[j][i];
    }
  }

  return SRSLTE_SUCCESS;
}


int srslte_chest_sl_init_psbch_dmrs(srslte_chest_sl_t* q)
{
  return srslte_chest_sl_init_dmrs(q);
}

int srslte_chest_sl_gen_psbch_dmrs(srslte_chest_sl_t* q, srslte_sl_tm_t tm, uint32_t N_sl_id)
{
  return srslte_chest_sl_gen_dmrs(q, tm, SRSLTE_SIDELINK_PSBCH, 0, N_sl_id, 0, 0, 0);
}

int srslte_chest_sl_put_psbch_dmrs(srslte_chest_sl_t* q,
                                   cf_t*              sf_buffer,
                                   srslte_sl_tm_t     tm,
                                   uint32_t           nof_prb,
                                   srslte_cp_t        cp)
{
  uint32_t sample_pos  = 0;
  uint32_t k           = nof_prb * SRSLTE_NRE / 2 - 36;
  uint32_t sf_nsymbols = 0;

  if (tm <= SRSLTE_SIDELINK_TM2) {
    sf_nsymbols = (cp == SRSLTE_CP_NORM) ? SRSLTE_CP_NORM_SF_NSYMB : SRSLTE_CP_EXT_SF_NSYMB;
  } else {
    // TM3/4
    sf_nsymbols = SRSLTE_CP_NORM_SF_NSYMB;
  }

  // Mapping to physical resources
  for (uint32_t i = 0; i < sf_nsymbols; i++) {
    if (srslte_psbch_is_symbol(SRSLTE_SIDELINK_DMRS_SYMBOL, tm, i)) {
      memcpy(&sf_buffer[k + i * nof_prb * SRSLTE_NRE], &q->r_sequence[sample_pos][0], q->M_sc_rs * sizeof(cf_t));
      sample_pos++;
    }
  }

  return SRSLTE_SUCCESS;
}

static void interpolate_pilots_sl_psbch(srslte_interp_linsrslte_vec_t* q,
                                        cf_t*                          ce,
                                        uint32_t                       n_prb,
                                        srslte_sl_tm_t                 tm,
                                        srslte_cp_t                    cp)
{
  uint32_t L1;
  uint32_t L2;
  uint32_t L3;
  uint32_t ce_l1;
  uint32_t ce_l2;
  uint32_t ce_l3;

  if (tm <= SRSLTE_SIDELINK_TM2) {
    if (cp == SRSLTE_CP_NORM) {
      L1 = 3;
      L2 = 10;
    } else {
      L1 = 2;
      L2 = 8;
    }
  } else {
    // TM3/4
    if (cp == SRSLTE_CP_NORM) {
      L1 = 4;
      L2 = 6;
      L3 = 9;
    } else {
      ERROR("Invalid CP");
      return;
    }
  }

  uint32_t NL = 2 * SRSLTE_CP_NSYMB(cp);

  uint32_t nre = n_prb * SRSLTE_NRE;

  srslte_interp_linear_vector_resize(q, nre);

  ce_l1 = SRSLTE_RE_IDX(n_prb, L1, 0 * SRSLTE_NRE);
  ce_l2 = SRSLTE_RE_IDX(n_prb, L2, 0 * SRSLTE_NRE);

  if (tm <= SRSLTE_SIDELINK_TM2) {
    srslte_interp_linear_vector3(q, &ce[ce_l2], &ce[ce_l1], &ce[ce_l1], &ce[ce_l1 - nre], (L2 - L1), L1, false, nre);
    srslte_interp_linear_vector3(
        q, &ce[ce_l1], &ce[ce_l2], NULL, &ce[ce_l1 + nre], (L2 - L1), (L2 - L1) - 1, true, nre);
    srslte_interp_linear_vector3(
        q, &ce[ce_l1], &ce[ce_l2], &ce[ce_l2], &ce[ce_l2 + nre], (L2 - L1), (NL - L2) - 1, true, nre);
  } else {
    // TM3/4
    ce_l3 = SRSLTE_RE_IDX(n_prb, L3, 0 * SRSLTE_NRE);
    srslte_interp_linear_vector3(q, &ce[ce_l2], &ce[ce_l1], &ce[ce_l1], &ce[ce_l1 - nre], (L2 - L1), L1, false, nre);
    srslte_interp_linear_vector3(
        q, &ce[ce_l1], &ce[ce_l2], NULL, &ce[ce_l1 + nre], (L2 - L1), (L2 - L1) - 1, true, nre);
    srslte_interp_linear_vector3(
        q, &ce[ce_l2], &ce[ce_l3], NULL, &ce[ce_l2 + nre], (L3 - L2), (L3 - L2) - 1, true, nre);
    srslte_interp_linear_vector3(
        q, &ce[ce_l2], &ce[ce_l3], &ce[ce_l3], &ce[ce_l3 + nre], (L3 - L2), (NL - L3) - 1, true, nre);
  }
}

void srslte_chest_sl_psbch_ls_estimate_equalize(srslte_chest_sl_t* q,
                                                cf_t*              sf_buffer,
                                                cf_t*              sf_buffer_rx,
                                                uint32_t           nof_prb,
                                                srslte_sl_tm_t     tm,
                                                srslte_cp_t        cp)
{
  int sf_n_re = SRSLTE_CP_NSYMB(cp) * SRSLTE_NRE * nof_prb * 2;

  uint32_t k = nof_prb * SRSLTE_NRE / 2 - 36;

  // Get Pilot Estimates
  // Use the known DMRS signal to compute least-squares estimates
  srslte_vec_cf_zero(q->ce, sf_n_re);

  if (tm <= SRSLTE_SIDELINK_TM2) {
    if (cp == SRSLTE_CP_NORM) {
      srslte_vec_prod_conj_ccc(&sf_buffer[3 * nof_prb * SRSLTE_NRE + k],
                               &q->r_sequence[0][0],
                               &q->ce[3 * nof_prb * SRSLTE_NRE + k],
                               q->M_sc_rs);
      srslte_vec_prod_conj_ccc(&sf_buffer[10 * nof_prb * SRSLTE_NRE + k],
                               &q->r_sequence[1][0],
                               &q->ce[10 * nof_prb * SRSLTE_NRE + k],
                               q->M_sc_rs);
    } else {
      srslte_vec_prod_conj_ccc(&sf_buffer[2 * nof_prb * SRSLTE_NRE + k],
                               &q->r_sequence[0][0],
                               &q->ce[2 * nof_prb * SRSLTE_NRE + k],
                               q->M_sc_rs);
      srslte_vec_prod_conj_ccc(&sf_buffer[8 * nof_prb * SRSLTE_NRE + k],
                               &q->r_sequence[1][0],
                               &q->ce[8 * nof_prb * SRSLTE_NRE + k],
                               q->M_sc_rs);
    }
  } else {
    // TM3/4
    if (cp == SRSLTE_CP_NORM) {
      srslte_vec_prod_conj_ccc(&sf_buffer[4 * nof_prb * SRSLTE_NRE + k],
                               &q->r_sequence[0][0],
                               &q->ce[4 * nof_prb * SRSLTE_NRE + k],
                               q->M_sc_rs);
      srslte_vec_prod_conj_ccc(&sf_buffer[6 * nof_prb * SRSLTE_NRE + k],
                               &q->r_sequence[1][0],
                               &q->ce[6 * nof_prb * SRSLTE_NRE + k],
                               q->M_sc_rs);
      srslte_vec_prod_conj_ccc(&sf_buffer[9 * nof_prb * SRSLTE_NRE + k],
                               &q->r_sequence[2][0],
                               &q->ce[9 * nof_prb * SRSLTE_NRE + k],
                               q->M_sc_rs);
    } else {
      ERROR("Invalid CP");
      return;
    }
  }

  interpolate_pilots_sl_psbch(&q->lin_vec_sl, q->ce, nof_prb, tm, cp);

  // Perform channel equalization
  srslte_predecoding_single(sf_buffer, q->ce, sf_buffer_rx, NULL, sf_n_re, 1, 0.0);
}

int srslte_chest_sl_get_psbch_dmrs(srslte_chest_sl_t* q,
                                   cf_t*              sf_buffer_rx,
                                   cf_t**             dmrs_received,
                                   srslte_sl_tm_t     tm,
                                   uint32_t           nof_prb,
                                   srslte_cp_t        cp)
{
  uint32_t sample_pos  = 0;
  uint32_t k           = nof_prb * SRSLTE_NRE / 2 - 36;
  uint32_t sf_nsymbols = (cp == SRSLTE_CP_NORM) ? SRSLTE_CP_NORM_SF_NSYMB : SRSLTE_CP_EXT_SF_NSYMB;

  // Get DMRSs
  for (uint32_t i = 0; i < sf_nsymbols; i++) {
    if (srslte_psbch_is_symbol(SRSLTE_SIDELINK_DMRS_SYMBOL, tm, i)) {
      memcpy(&q->dmrs_received[sample_pos][0], &sf_buffer_rx[k + i * nof_prb * SRSLTE_NRE], q->M_sc_rs * sizeof(cf_t));
      sample_pos++;
    }
  }

  for (uint32_t i = 0; i < q->nof_dmrs_symbols; i++) {
    memcpy(&dmrs_received[i][0], &q->dmrs_received[i][0], q->M_sc_rs * sizeof(cf_t));
  }

  return sample_pos;
}

int srslte_chest_sl_init_pscch_dmrs(srslte_chest_sl_t* q)
{
  return srslte_chest_sl_init_dmrs(q);
}

int srslte_chest_sl_gen_pscch_dmrs(srslte_chest_sl_t* q, uint32_t cyclic_shift, srslte_sl_tm_t tm)
{
  return srslte_chest_sl_gen_dmrs(q, tm, SRSLTE_SIDELINK_PSCCH, 0, 0, 0, 0, cyclic_shift);
}

int srslte_chest_sl_put_pscch_dmrs(srslte_chest_sl_t* q,
                                   cf_t*              sf_buffer,
                                   uint32_t           prb_idx,
                                   srslte_sl_tm_t     tm,
                                   uint32_t           nof_prb,
                                   srslte_cp_t        cp)
{

  uint32_t samplePos = 0;
  uint32_t k         = prb_idx * SRSLTE_NRE;

  // Mapping to physical resources
  for (uint32_t i = 0; i < srslte_sl_get_num_symbols(tm, cp); i++) {
    if (srslte_pscch_is_symbol(SRSLTE_SIDELINK_DMRS_SYMBOL, tm, i, cp)) {
      memcpy(&sf_buffer[k + i * nof_prb * SRSLTE_NRE], &q->r_sequence[samplePos][0], q->M_sc_rs * sizeof(cf_t));
      samplePos++;
    }
  }

  return SRSLTE_SUCCESS;
}

static void interpolate_pilots_sl_pscch(srslte_interp_linsrslte_vec_t* q,
                                        cf_t*                          ce,
                                        uint32_t                       n_prb,
                                        uint32_t                       prb_idx,
                                        srslte_sl_tm_t                 tm,
                                        srslte_cp_t                    cp)
{
  uint32_t l_idx                      = 0;
  uint32_t L[SRSLTE_SL_MAX_DMRS_SYMB] = {};
  for (int i = 0; i < srslte_sl_get_num_symbols(tm, cp); i++) {
    if (srslte_pscch_is_symbol(SRSLTE_SIDELINK_DMRS_SYMBOL, tm, i, cp)) {
      L[l_idx] = i;
      l_idx++;
    }
  }

  uint32_t NL = 2 * SRSLTE_CP_NSYMB(cp);
  uint32_t nre = n_prb * SRSLTE_NRE;

  srslte_interp_linear_vector_resize(q, nre);

  uint32_t ce_l1 = SRSLTE_RE_IDX(n_prb, L[0], 0 * SRSLTE_NRE);
  uint32_t ce_l2 = SRSLTE_RE_IDX(n_prb, L[1], 0 * SRSLTE_NRE);

  if ((tm == SRSLTE_SIDELINK_TM1) || (tm == SRSLTE_SIDELINK_TM2)) {
    srslte_interp_linear_vector3(q, &ce[ce_l2], &ce[ce_l1], &ce[ce_l1], &ce[ce_l1 - nre], (L[1] - L[0]), L[0], false, nre);
    srslte_interp_linear_vector3(
        q, &ce[ce_l1], &ce[ce_l2], NULL, &ce[ce_l1 + nre], (L[1] - L[0]), (L[1] - L[0]) - 1, true, nre);
    srslte_interp_linear_vector3(
        q, &ce[ce_l1], &ce[ce_l2], &ce[ce_l2], &ce[ce_l2 + nre], (L[1] - L[0]), (NL - L[1]) - 1, true, nre);
  } else {
    uint32_t ce_l3 = SRSLTE_RE_IDX(n_prb, L[2], 0 * SRSLTE_NRE);
    uint32_t ce_l4 = SRSLTE_RE_IDX(n_prb, L[3], 0 * SRSLTE_NRE);
    srslte_interp_linear_vector3(q, &ce[ce_l2], &ce[ce_l1], &ce[ce_l1], &ce[ce_l1 - nre], (L[1] - L[0]), L[0], false, nre);
    srslte_interp_linear_vector3(
        q, &ce[ce_l1], &ce[ce_l2], NULL, &ce[ce_l1 + nre], (L[1] - L[0]), (L[1] - L[0]) - 1, true, nre);
    srslte_interp_linear_vector3(
        q, &ce[ce_l2], &ce[ce_l3], NULL, &ce[ce_l2 + nre], (L[2] - L[1]), (L[2] - L[1]) - 1, true, nre);
    srslte_interp_linear_vector3(
        q, &ce[ce_l3], &ce[ce_l4], NULL, &ce[ce_l3 + nre], (L[3] - L[2]), (L[3] - L[2]) - 1, true, nre);
    srslte_interp_linear_vector3(
        q, &ce[ce_l3], &ce[ce_l4], &ce[ce_l4], &ce[ce_l4 + nre], (L[3] - L[2]), (NL - L[3]) - 1, true, nre);
  }
}

void srslte_chest_sl_pscch_ls_estimate_equalize(srslte_chest_sl_t* q,
                                                cf_t*              sf_buffer,
                                                uint32_t           prb_idx,
                                                cf_t*              sf_buffer_rx,
                                                uint32_t           nof_prb,
                                                srslte_sl_tm_t     tm,
                                                srslte_cp_t        cp)
{
  int sf_n_re = SRSLTE_CP_NSYMB(cp) * SRSLTE_NRE * nof_prb * 2;

  // Get Pilot Estimates
  // Use the known DMRS signal to compute least-squares estimates
  srslte_vec_cf_zero(q->ce, sf_n_re);

  if ((tm == SRSLTE_SIDELINK_TM1) || (tm == SRSLTE_SIDELINK_TM2)) {
    uint32_t dmrs_index = 0;
    for (uint32_t i = 0; i < srslte_sl_get_num_symbols(tm, cp); i++) {
      if (srslte_pscch_is_symbol(SRSLTE_SIDELINK_DMRS_SYMBOL, tm, i, cp)) {
        srslte_vec_prod_conj_ccc(&sf_buffer[i * nof_prb * SRSLTE_NRE + prb_idx * SRSLTE_NRE],
                                 &q->r_sequence[dmrs_index][0],
                                 &q->ce[i * nof_prb * SRSLTE_NRE + prb_idx * SRSLTE_NRE],
                                 q->M_sc_rs);
      }
    }
  } else if ((tm == SRSLTE_SIDELINK_TM3) || (tm == SRSLTE_SIDELINK_TM4)) {
    if (cp == SRSLTE_CP_NORM) {
      uint32_t dmrs_index = 0;
      for (uint32_t i = 0; i < srslte_sl_get_num_symbols(tm, cp); i++) {
        if (srslte_pscch_is_symbol(SRSLTE_SIDELINK_DMRS_SYMBOL, tm, i, cp)) {
          srslte_vec_prod_conj_ccc(&sf_buffer[i * nof_prb * SRSLTE_NRE + prb_idx * SRSLTE_NRE],
                                   &q->r_sequence[dmrs_index][0],
                                   &q->ce[i * nof_prb * SRSLTE_NRE + prb_idx * SRSLTE_NRE],
                                   q->M_sc_rs);
        }
      }
    } else {
      ERROR("Invalid CP");
      return;
    }
  } else {
    ERROR("Invalid TM");
    return;
  }

  // Estimate synchronization error
  if (q->sync_error_enable) {
    float k   = (float)srslte_symbol_sz(nof_prb);
    float sum = 0.0f;
    for (uint32_t i = 0; i < srslte_sl_get_num_symbols(tm, cp); i++) {
      if (srslte_pscch_is_symbol(SRSLTE_SIDELINK_DMRS_SYMBOL, tm, i, cp)) {
        sum += srslte_vec_estimate_frequency(&q->ce[i * nof_prb * SRSLTE_NRE + prb_idx * SRSLTE_NRE], q->M_sc_rs) * k;
      }
    }
    q->sync_err = sum / q->nof_dmrs_symbols;
  } else {
    q->sync_err = NAN;
  }

  // Compute RSRP for the channel estimates in this port
  if (q->rsrp_enable) {
    for (uint32_t i = 0; i < srslte_sl_get_num_symbols(tm, cp); i++) {
      if (srslte_pscch_is_symbol(SRSLTE_SIDELINK_DMRS_SYMBOL, tm, i, cp)) {
        cf_t corr = srslte_vec_acc_cc(&q->ce[i * nof_prb * SRSLTE_NRE + prb_idx * SRSLTE_NRE], q->M_sc_rs) / q->M_sc_rs;
        float energy = __real__(corr * conjf(corr));
        q->rsrp_corr = energy;
      }
    }
    q->rsrp_corr /= q->nof_dmrs_symbols;
  }

  interpolate_pilots_sl_pscch(&q->lin_vec_sl, q->ce, nof_prb, prb_idx, tm, cp);

  // Perform channel equalization
  srslte_predecoding_single(sf_buffer, q->ce, sf_buffer_rx, NULL, sf_n_re, 1, 0.0);
}

int srslte_chest_sl_get_pscch_dmrs(srslte_chest_sl_t* q,
                                   cf_t*              sf_buffer_rx,
                                   uint32_t           prb_idx,
                                   cf_t**             dmrs_received,
                                   srslte_sl_tm_t     tm,
                                   uint32_t           nof_prb,
                                   srslte_cp_t        cp)
{
  uint32_t samplePos = 0;
  uint32_t k         = prb_idx * SRSLTE_NRE;

  // Get DMRSs
  for (uint32_t i = 0; i < srslte_sl_get_num_symbols(tm, cp); i++) {
    if (srslte_pscch_is_symbol(SRSLTE_SIDELINK_DMRS_SYMBOL, tm, i, cp)) {
      memcpy(&q->dmrs_received[samplePos][0], &sf_buffer_rx[k + i * nof_prb * SRSLTE_NRE], q->M_sc_rs * sizeof(cf_t));
      samplePos++;
    }
  }

  for (uint32_t i = 0; i < q->nof_dmrs_symbols; i++) {
    memcpy(&dmrs_received[i][0], &q->dmrs_received[i][0], q->M_sc_rs * sizeof(cf_t));
  }

  return SRSLTE_SUCCESS;
}

void srslte_chest_sl_free(srslte_chest_sl_t* q)
{
  srslte_interp_linear_vector_free(&q->lin_vec_sl);

  for (int i = 0; i < SRSLTE_SL_MAX_DMRS_SYMB; i++) {
    if (q->r[i]) {
      free(q->r[i]);
    }

    if (q->r_uv[i]) {
      free(q->r_uv[i]);
    }

    if (q->r_sequence[i]) {
      free(q->r_sequence[i]);
    }

    if (q->dmrs_received[i]) {
      free(q->dmrs_received[i]);
    }
  }

  if (q->f_gh_pattern) {
    free(q->f_gh_pattern);
  }
  if (q->ce) {
    free(q->ce);
  }
}
