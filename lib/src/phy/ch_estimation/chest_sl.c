/*
 * Copyright 2013-2019 Software Radio Systems Limited
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
#include <srslte/phy/common/phy_common.h>
#include <srslte/phy/mimo/precoding.h>
#include <srslte/phy/utils/debug.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srslte/config.h"
#include "srslte/phy/ch_estimation/chest_sl.h"
#include "srslte/phy/dft/dft_precoding.h"
#include "srslte/phy/utils/convolution.h"
#include "srslte/phy/utils/vector.h"
#include "ul_rs_tables.h"

#define NOF_PRIME_NUMBERS_SL 196
#define SRSLTE_SL_MAX_DMRS 4
#define SRSLTE_SL_MAX_M_SC_RS 100 * 12

static uint32_t get_q(uint32_t u, uint32_t v, uint32_t N_sz)
{
  float q;
  float q_hat;
  float n_sz = (float)N_sz;

  q_hat = n_sz * (u + 1) / 31;
  if ((((uint32_t)(2 * q_hat)) % 2) == 0) {
    q = q_hat + 0.5 + v;
  } else {
    q = q_hat + 0.5 - v;
  }
  return (uint32_t)q;
}

static void interpolate_pilots_sl_psbch(srslte_interp_linsrslte_vec_t* q, cf_t* ce, uint32_t n_prb)
{
  uint32_t L1 = SRSLTE_REFSIGNAL_UL_L(0, SRSLTE_CP_NORM);
  uint32_t L2 = SRSLTE_REFSIGNAL_UL_L(1, SRSLTE_CP_NORM);
  uint32_t NL = 2 * SRSLTE_CP_NSYMB(SRSLTE_CP_NORM);

  uint32_t nre = n_prb * SRSLTE_NRE;

  srslte_interp_linear_vector_resize(q, nre);

  uint32_t ce_l1 = SRSLTE_RE_IDX(n_prb, L1, 0 * SRSLTE_NRE);
  uint32_t ce_l2 = SRSLTE_RE_IDX(n_prb, L2, 0 * SRSLTE_NRE);

  srslte_interp_linear_vector3(q, &ce[ce_l2], &ce[ce_l1], &ce[ce_l1], &ce[ce_l1 - nre], (L2 - L1), L1, false, nre);
  srslte_interp_linear_vector3(q, &ce[ce_l1], &ce[ce_l2], NULL, &ce[ce_l1 + nre], (L2 - L1), (L2 - L1) - 1, true, nre);
  srslte_interp_linear_vector3(
      q, &ce[ce_l1], &ce[ce_l2], &ce[ce_l2], &ce[ce_l2 + nre], (L2 - L1), (NL - L2) - 1, true, nre);
}

// TS36.211 S9.8 Table 9.8-2: Reference signal parameters for PSCCH
int srslte_chest_sl_init_dmrs(srslte_chest_sl_t* q, uint32_t N_sa_id)
{
  srslte_interp_linear_vector_init(&q->lin_vec_sl, SRSLTE_MAX_PRB * SRSLTE_NRE);

  q->n_CS = srslte_vec_malloc(sizeof(int8_t) * SRSLTE_SL_MAX_DMRS);
  if (!q->n_CS) {
    ERROR("Error allocating memmory");
    return SRSLTE_ERROR;
  }
  q->alpha = srslte_vec_malloc(sizeof(float) * SRSLTE_SL_MAX_DMRS);
  if (!q->alpha) {
    ERROR("Error allocating memmory");
    return SRSLTE_ERROR;
  }
  q->w = srslte_vec_malloc(sizeof(uint8_t) * SRSLTE_SL_MAX_DMRS);
  if (!q->w) {
    ERROR("Error allocating memmory");
    return SRSLTE_ERROR;
  }
  q->q = srslte_vec_malloc(sizeof(uint32_t) * SRSLTE_SL_MAX_DMRS);
  if (!q->q) {
    ERROR("Error allocating memmory");
    return SRSLTE_ERROR;
  }
  q->r = (float**)srslte_vec_malloc(sizeof(float*) * SRSLTE_SL_MAX_DMRS);
  if (!q->r) {
    ERROR("Error allocating memmory");
    return SRSLTE_ERROR;
  }
  for (int i = 0; i < SRSLTE_SL_MAX_DMRS; i++) {
    q->r[i] = srslte_vec_malloc(sizeof(float) * SRSLTE_MAX_PRB * SRSLTE_NRE);
    if (!q->r[i]) {
      ERROR("Error allocating memmory");
      return SRSLTE_ERROR;
    }
  }
  q->r_uv = (cf_t**)srslte_vec_malloc(sizeof(cf_t*) * SRSLTE_SL_MAX_DMRS);
  if (!q->r_uv) {
    ERROR("Error allocating memmory");
    return SRSLTE_ERROR;
  }
  for (int i = 0; i < SRSLTE_SL_MAX_DMRS; i++) {
    q->r_uv[i] = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_MAX_PRB * SRSLTE_NRE);
    if (!q->r_uv[i]) {
      ERROR("Error allocating memmory");
      return SRSLTE_ERROR;
    }
  }
  q->r_sequence = (cf_t**)srslte_vec_malloc(sizeof(cf_t*) * SRSLTE_SL_MAX_DMRS);
  if (!q->r_sequence) {
    ERROR("Error allocating memmory");
    return SRSLTE_ERROR;
  }
  for (int i = 0; i < SRSLTE_SL_MAX_DMRS; i++) {
    q->r_sequence[i] = srslte_vec_malloc(sizeof(cf_t) * SRSLTE_MAX_PRB * SRSLTE_NRE);
    if (!q->r_sequence[i]) {
      ERROR("Error allocating memmory");
      return SRSLTE_ERROR;
    }
  }
  q->u = srslte_vec_malloc(sizeof(uint32_t) * 320 * SRSLTE_SL_MAX_DMRS);
  if (!q->u) {
    ERROR("Error allocating memmory");
    return SRSLTE_ERROR;
  }
  q->f_gh_pattern = srslte_vec_malloc(sizeof(uint32_t) * 320 * SRSLTE_SL_MAX_DMRS); // MAX PERIOD LENGTH 320
  if (!q->f_gh_pattern) {
    ERROR("Error allocating memmory");
    return SRSLTE_ERROR;
  }
  srslte_group_hopping_f_gh(q->f_gh_pattern, N_sa_id);

  q->ce = srslte_vec_malloc(sizeof(cf_t) * 100 * 12 * 14);
  if (!q->ce) {
    ERROR("Error allocating memmory");
    return SRSLTE_ERROR;
  }

  return SRSLTE_SUCCESS;
}

int srslte_chest_sl_init_psbch_dmrs(srslte_chest_sl_t* q)
{
  return srslte_chest_sl_init_dmrs(q, 0);
}

int srslte_chest_sl_gen_dmrs(srslte_chest_sl_t*   q,
                             srslte_sl_tm_t       txMode,
                             uint32_t             nof_prb,
                             srslte_sl_channels_t ch,
                             uint32_t             sf_idx,
                             uint32_t             N_sl_id,
                             uint32_t             available_pool_rbs,
                             uint32_t             N_sa_id)
{
  // M_sc_rs - Reference Signal Length
  switch (ch) {
    case SRSLTE_SIDELINK_PSBCH:
      q->M_sc_rs = 72;
      break;
    case SRSLTE_SIDELINK_PSCCH:
      if (txMode == SRSLTE_SIDELINK_TM1 || txMode == SRSLTE_SIDELINK_TM2) {
        q->M_sc_rs = 12;
      } else if (txMode == SRSLTE_SIDELINK_TM3 || txMode == SRSLTE_SIDELINK_TM4) {
        q->M_sc_rs = 24;
      }
      break;
    case SRSLTE_SIDELINK_PSSCH:
      q->M_sc_rs = available_pool_rbs;
      break;
    case SRSLTE_SIDELINK_PSDCH:
      printf("channel not supported yet\n\n");
      break;
    default:
      printf("channel not supported yet\n\n");
      return SRSLTE_ERROR;
  }

  // nr_DMRS_symbols
  switch (txMode) {
    case SRSLTE_SIDELINK_TM1:
    case SRSLTE_SIDELINK_TM2:
      q->nr_DMRS_symbols = 2;
      break;
    case SRSLTE_SIDELINK_TM3:
    case SRSLTE_SIDELINK_TM4:
      printf("transmission mode not supported yet\n");
      return SRSLTE_ERROR;
    default:
      printf("transmission mode not supported \n");
      return SRSLTE_ERROR;
  }

  // n_CS - Cyclic Shift
  switch (ch) {
    case SRSLTE_SIDELINK_PSBCH:
      for (int i = 0; i < q->nr_DMRS_symbols; i++) {
        q->n_CS[i] = (int)(N_sl_id / 2) % 8;
      }
      break;
    case SRSLTE_SIDELINK_PSCCH:
      if (txMode == SRSLTE_SIDELINK_TM1 || txMode == SRSLTE_SIDELINK_TM2) {
        for (int i = 0; i < q->nr_DMRS_symbols; i++) {
          q->n_CS[i] = 0;
        }
      } else if (txMode == SRSLTE_SIDELINK_TM3 || txMode == SRSLTE_SIDELINK_TM4) {
        for (int i = 0; i < q->nr_DMRS_symbols; i++) {
          q->n_CS[i] = i * 3;
        }
      }
      break;
    case SRSLTE_SIDELINK_PSSCH:
      if (txMode == SRSLTE_SIDELINK_TM1 || txMode == SRSLTE_SIDELINK_TM2) {
        for (int i = 0; i < q->nr_DMRS_symbols; i++) {
          q->n_CS[i] = (int)(N_sa_id / 2) % 8;
        }
      } else if (txMode == SRSLTE_SIDELINK_TM3 || txMode == SRSLTE_SIDELINK_TM4) {
      }
      break;
    case SRSLTE_SIDELINK_PSDCH:
      for (int i = 0; i < q->nr_DMRS_symbols; i++) {
        q->n_CS[i] = 0;
      }
      break;
  }

  // alpha - Reference Signal Cyclic Shift
  for (int i = 0; i < q->nr_DMRS_symbols; ++i) {
    q->alpha[i] = (2 * M_PI * q->n_CS[i]) / 12;
  }

  // Group Hopping
  // Base Sequence Number - always 0 for sidelink
  q->v = 0;
  switch (ch) {
    case SRSLTE_SIDELINK_PSBCH:
      q->f_gh = 0;
      q->f_ss = (N_sl_id / 16) % 30;
      for (int i = 0; i < q->nr_DMRS_symbols; ++i) {
        q->u[i] = (q->f_gh + q->f_ss) % 30;
      }
      break;
    case SRSLTE_SIDELINK_PSCCH:
      q->f_gh = 0;
      if (txMode == SRSLTE_SIDELINK_TM1 || txMode == SRSLTE_SIDELINK_TM2) {
        q->f_ss = 0;
      } else if (txMode == SRSLTE_SIDELINK_TM3 || txMode == SRSLTE_SIDELINK_TM4) {
        q->f_ss = 8;
      }
      for (int i = 0; i < q->nr_DMRS_symbols; ++i) {
        q->u[i] = (q->f_gh + q->f_ss) % 30;
      }
      break;
    case SRSLTE_SIDELINK_PSSCH:
      if (txMode == SRSLTE_SIDELINK_TM1 || txMode == SRSLTE_SIDELINK_TM2) {
        q->f_gh = 1;
        q->f_ss = N_sa_id % 30;

        uint8_t i = 0;
        for (uint32_t ns = 2 * sf_idx; ns < 2 * (sf_idx + 1); ns++) {
          uint32_t f_gh = q->f_gh_pattern[ns];

          uint32_t delta_ss = 0;
          q->u[i++]         = (f_gh + (N_sa_id % 30) + delta_ss) % 30;
        }
      } else if (txMode == SRSLTE_SIDELINK_TM3 || txMode == SRSLTE_SIDELINK_TM4) {
        q->f_gh = 1;
      }
      break;
    case SRSLTE_SIDELINK_PSDCH:
      q->f_gh = 0;
      q->f_ss = 0;
      for (int i = 0; i < q->nr_DMRS_symbols; ++i) {
        q->u[i] = (q->f_gh + q->f_ss) % 30;
      }
      break;
  }

  // N_zc - Zadoff Chu Sequence Length
  switch (q->M_sc_rs / SRSLTE_NRE) {
    case 1:
      for (int j = 0; j < q->nr_DMRS_symbols; ++j) {
        for (int i = 0; i < SRSLTE_NRE; i++) {
          q->r[j][i] = phi_M_sc_12[q->u[j]][i] * M_PI / 4;
        }
      }
      break;
    case 2:
      for (int j = 0; j < q->nr_DMRS_symbols; ++j) {
        for (int i = 0; i < q->M_sc_rs; i++) {
          q->r[j][i] = phi_M_sc_24[q->u[j]][i] * M_PI / 4;
        }
      }
      break;
    default:
      for (uint32_t i = NOF_PRIME_NUMBERS_SL - 1; i > 0; i--) {
        if (prime_numbers[i] < q->M_sc_rs) {
          q->N_zc = prime_numbers[i];
          break;
        }
      }
      for (int j = 0; j < q->nr_DMRS_symbols; ++j) {
        q->q[j]    = get_q(q->u[j], q->v, q->N_zc);
        float n_sz = (float)q->N_zc;
        for (uint32_t i = 0; i < q->M_sc_rs; i++) {
          float m    = (float)(i % q->N_zc);
          q->r[j][i] = -M_PI * q->q[j] * m * (m + 1) / n_sz;
        }
      }

      break;
  }
  // Do complex exponential and adjust amplitude
  for (int j = 0; j < q->nr_DMRS_symbols; ++j) {
    for (int i = 0; i < q->M_sc_rs; i++) {
      q->r_uv[j][i] = cexpf(I * (q->r[j][i] + q->alpha[0] * i));
    }
  }

  // w - Orthogonal Sequence
  switch (ch) {
    case SRSLTE_SIDELINK_PSBCH:
      if (txMode == SRSLTE_SIDELINK_TM1 || txMode == SRSLTE_SIDELINK_TM2) {
        if (N_sl_id % 2) {
          q->w[0] = 1;
          q->w[1] = -1;
        } else {
          q->w[0] = 1;
          q->w[1] = 1;
        }
      } else if (txMode == SRSLTE_SIDELINK_TM3 || txMode == SRSLTE_SIDELINK_TM4) {
        printf("transmission mode not supported \n");
        return SRSLTE_ERROR;
      }
      break;
    case SRSLTE_SIDELINK_PSCCH:
      if (txMode == SRSLTE_SIDELINK_TM1 || txMode == SRSLTE_SIDELINK_TM2) {
        q->w[0] = 1;
        q->w[1] = 1;
      } else if (txMode == SRSLTE_SIDELINK_TM3 || txMode == SRSLTE_SIDELINK_TM4) {
        printf("transmission mode not supported \n");
        return SRSLTE_ERROR;
      }
      break;
    case SRSLTE_SIDELINK_PSSCH:
      if (txMode == SRSLTE_SIDELINK_TM1 || txMode == SRSLTE_SIDELINK_TM2) {
        if (N_sa_id % 2 == 0) {
          q->w[0] = 1;
          q->w[1] = 1;
        } else {
          q->w[0] = 1;
          q->w[1] = -1;
        }
      } else if (txMode == SRSLTE_SIDELINK_TM3 || txMode == SRSLTE_SIDELINK_TM4) {
        printf("transmission mode not supported \n");
        return SRSLTE_ERROR;
      }
      break;
    default:
      return SRSLTE_ERROR;
  }
  for (int j = 0; j < q->nr_DMRS_symbols; j++) {
    for (int i = 0; i < q->M_sc_rs; i++) {
      q->r_sequence[j][i] = q->w[j] * q->r_uv[j][i];
    }
  }
  return SRSLTE_SUCCESS;
}

int srslte_chest_sl_gen_psbch_dmrs(
    srslte_chest_sl_t* q, srslte_sl_tm_t txMode, uint32_t nof_prb, uint32_t sf_idx, uint32_t N_sl_id)
{
  return srslte_chest_sl_gen_dmrs(q, txMode, nof_prb, SRSLTE_SIDELINK_PSBCH, 0, N_sl_id, 0, 0);
}

int srslte_chest_sl_put_psbch_dmrs(srslte_chest_sl_t* q, cf_t* sf_buffer, srslte_sl_tm_t tx_mode, uint32_t nof_prb)
{
  int sf_n_re = SRSLTE_CP_NSYMB(SRSLTE_CP_NORM) * SRSLTE_NRE * nof_prb * 2;
  if (tx_mode == SRSLTE_SIDELINK_TM1 || tx_mode == SRSLTE_SIDELINK_TM2) {
    int k = (SRSLTE_CP_NSYMB(SRSLTE_CP_NORM) - 4) * nof_prb * SRSLTE_NRE + nof_prb * SRSLTE_NRE / 2 - 36;
    memcpy(&sf_buffer[k], &q->r_sequence[0][0], q->M_sc_rs * sizeof(cf_t));
    memcpy(&sf_buffer[k + sf_n_re / 2], &q->r_sequence[1][0], q->M_sc_rs * sizeof(cf_t));
  } else if (tx_mode == SRSLTE_SIDELINK_TM3 || tx_mode == SRSLTE_SIDELINK_TM4) {
    printf("transmission mode not supported yet\n");
    return SRSLTE_ERROR;
  }
  return SRSLTE_SUCCESS;
}

void srslte_chest_sl_put_pscch_dmrs(
    srslte_chest_sl_t* q, uint32_t prb_idx, uint32_t nof_prb, cf_t* sf_buffer, srslte_sl_tm_t txMode)
{
  uint32_t l = 3;
  uint32_t k = prb_idx * SRSLTE_NRE;
  if (txMode == SRSLTE_SIDELINK_TM1 || txMode == SRSLTE_SIDELINK_TM2) {
    for (int i = 0; i < q->nr_DMRS_symbols; i++) {
      memcpy(&sf_buffer[k + l * SRSLTE_NRE * nof_prb], q->r_sequence[i], q->M_sc_rs * sizeof(cf_t));
      l += 7;
    }
  } else if (txMode == SRSLTE_SIDELINK_TM3 || txMode == SRSLTE_SIDELINK_TM4) {
    l = 2;
    for (int i = 0; i < q->nr_DMRS_symbols; i++) {
      memcpy(&sf_buffer[k + l * SRSLTE_NRE * nof_prb], q->r_sequence[i], q->M_sc_rs * sizeof(cf_t));
      l += 3;
    }
  }
}

void srslte_chest_sl_put_pssch_dmrs(srslte_chest_sl_t* q,
                                    uint32_t           prb_start,
                                    uint32_t           prb_end,
                                    uint32_t           prb_num,
                                    uint32_t           nof_prb,
                                    cf_t*              sf_buffer,
                                    srslte_sl_tm_t     txMode)
{
  uint32_t k = prb_start * SRSLTE_NRE;
  if (txMode == SRSLTE_SIDELINK_TM1 || txMode == SRSLTE_SIDELINK_TM2) {
    uint32_t l = 3;
    for (int i = 0; i < q->nr_DMRS_symbols; i++) {
      memcpy(&sf_buffer[k + l * nof_prb * SRSLTE_NRE], q->r_sequence[i], q->M_sc_rs / 2 * sizeof(cf_t));
      k = ((prb_end + 1) * SRSLTE_NRE) - (prb_num * SRSLTE_NRE);
      memcpy(
          &sf_buffer[k + l * nof_prb * SRSLTE_NRE], &q->r_sequence[i][q->M_sc_rs / 2], q->M_sc_rs / 2 * sizeof(cf_t));
      l += 7;
    }
  } else if (txMode == SRSLTE_SIDELINK_TM3 || txMode == SRSLTE_SIDELINK_TM4) {
    printf("Transmission Mode not supported\n");
  }
}

void srslte_chest_sl_psbch_ls_estimate_equalize(srslte_chest_sl_t* q, cf_t* sf_buffer, cf_t* output, uint32_t nof_prb)
{

  int sf_n_re = SRSLTE_CP_NSYMB(SRSLTE_CP_NORM) * SRSLTE_NRE * nof_prb * 2;

  // Get Demodulation Reference Signals from resource grid
  int k = (SRSLTE_CP_NSYMB(SRSLTE_CP_NORM) - 4) * nof_prb * SRSLTE_NRE + nof_prb * SRSLTE_NRE / 2 - 36;

  bzero(q->ce, sizeof(cf_t) * sf_n_re);

  // Get Pilot Estimates
  // Use the known DMRS signal to compute least-squares estimates
  srslte_vec_prod_conj_ccc(&sf_buffer[k], &q->r_sequence[0][0], &q->ce[k], q->M_sc_rs);
  srslte_vec_prod_conj_ccc(&sf_buffer[k + sf_n_re / 2], &q->r_sequence[1][0], &q->ce[k + sf_n_re / 2], q->M_sc_rs);

  interpolate_pilots_sl_psbch(&q->lin_vec_sl, q->ce, nof_prb);

  // Perform channel equalization
  srslte_predecoding_single(sf_buffer, q->ce, output, NULL, sf_n_re, 1, 0.0);
}

void srslte_chest_sl_free(srslte_chest_sl_t* q)
{
  srslte_interp_linear_vector_free(&q->lin_vec_sl);
  if (q->w) {
    free(q->w);
  }
  if (q->n_CS) {
    free(q->n_CS);
  }
  if (q->q) {
    free(q->q);
  }
  if (q->alpha) {
    free(q->alpha);
  }
  if (q->r) {
    for (int i = 0; i < SRSLTE_SL_MAX_DMRS; ++i) {
      free(q->r[i]);
    }
    free(q->r);
  }
  if (q->r_uv) {
    for (int i = 0; i < SRSLTE_SL_MAX_DMRS; ++i) {
      free(q->r_uv[i]);
    }
    free(q->r_uv);
  }
  if (q->r_sequence) {
    for (int i = 0; i < SRSLTE_SL_MAX_DMRS; ++i) {
      free(q->r_sequence[i]);
    }
    free(q->r_sequence);
  }
  if (q->u) {
    free(q->u);
  }
  if (q->f_gh_pattern) {
    free(q->f_gh_pattern);
  }
  if (q->ce) {
    free(q->ce);
  }
}