/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <complex.h>

#include "srslte/common/phy_common.h"
#include "srslte/ch_estimation/refsignal_ul.h"
#include "srslte/utils/vector.h"
#include "srslte/utils/debug.h"
#include "srslte/common/sequence.h"
#include "srslte/dft/dft_precoding.h"

#include "ul_rs_tables.h"

// n_dmrs_2 table 5.5.2.1.1-1 from 36.211
uint32_t n_dmrs_2[8] = { 0, 6, 3, 4, 2, 8, 10, 9 };

// n_dmrs_1 table 5.5.2.1.1-2 from 36.211
uint32_t n_dmrs_1[8] = { 0, 2, 3, 4, 6, 8, 9, 10 };

/* Orthogonal sequences for PUCCH formats 1a, 1b and 1c. Table 5.5.2.2.1-2 
 */
float w_arg_pucch_format1_cpnorm[3][3] = {{0, 0, 0},
                                         {0, 2*M_PI/3, 4*M_PI/3},
                                         {0, 4*M_PI/3, 2*M_PI/3}};

float w_arg_pucch_format1_cpext[3][2]  = {{0, 0},
                                         {0, M_PI},
                                         {0, 0}};

float w_arg_pucch_format2_cpnorm[2]  = {0, 0};
float w_arg_pucch_format2_cpext[1]   = {0};

uint32_t pucch_dmrs_symbol_format1_cpnorm[3] = {2, 3, 4};
uint32_t pucch_dmrs_symbol_format1_cpext[2] = {2, 3};
uint32_t pucch_dmrs_symbol_format2_cpnorm[2] = {1, 5};
uint32_t pucch_dmrs_symbol_format2_cpext[1] = {3};

/* Table 5.5.3.3-1: Frame structure type 1 sounding reference signal subframe configuration. */
uint32_t T_sfc[15] = {1, 2, 2, 5, 5, 5, 5, 5, 5, 10, 10, 10, 10, 10, 10};
uint32_t Delta_sfc1[7] = {0, 0, 1, 0, 1, 2, 3}; 
uint32_t Delta_sfc2[4] = {0, 1, 2, 3}; 


uint32_t m_srs_b[4][4][8] = {{
                        /* m_srs for 6<n_rb<40. Table 5.5.3.2-1 */
                        {36, 32, 24, 20, 16, 12, 8, 4}, 
                        {12, 16,  4,  4,  4,  4, 4, 4}, 
                        { 4,  8,  4,  4,  4,  4, 4, 4},
                        { 4,  4,  4,  4,  4,  4, 4, 4}},
                        {
                        /* m_srs for 40<n_rb<60. Table 5.5.3.2-2 */
                        {48, 48, 40, 36, 32, 24, 20, 16}, 
                        {24, 16, 20, 12, 16,  4,  4,  4},
                        {12,  8,  4,  4,  8,  4,  4,  4},
                        { 4,  4,  4,  4,  4,  4,  4,  4}},
                        {
                        /* m_srs for 60<n_rb<80. Table 5.5.3.2-3 */
                        {72, 64, 60, 48, 48, 40, 36, 32},
                        {24, 32, 20, 24, 16, 20, 12, 16},
                        {12, 16,  4, 12,  8,  4,  4,  8},
                        { 4,  4,  4,  4,  4,  4,  4,  4}},
                        
                        {
                        /* m_srs for 80<n_rb<110. Table 5.5.3.2-4 */
                        {96, 96, 80, 72, 64, 60, 48, 48},
                        {48, 32, 40, 24, 32, 20, 24, 16},
                        {24, 16, 20, 12, 16,  4, 12,  8},
                        { 4,  4,  4,  4,  4,  4,  4,  4}}};
                        
/* Same tables for Nb */                        
uint32_t Nb[4][4][8] = {{
                        {1, 1, 1, 1, 1, 1, 1, 1},
                        {3, 2, 6, 5, 4, 3, 2, 1},
                        {3, 2, 1, 1, 1, 1, 1, 1},
                        {1, 2, 1, 1, 1, 1, 1, 1}},
                        {
                        {1, 1, 1, 1, 1, 1, 1, 1},
                        {2, 3, 2, 3, 2, 6, 5, 4},
                        {2, 2, 5, 3, 2, 1, 1, 1},
                        {3, 2, 1, 1, 2, 1, 1, 1}},
                        {
                        {1, 1, 1, 1, 1, 1, 1, 1},
                        {3, 2, 3, 2, 3, 2, 3, 2},
                        {2, 2, 5, 2, 2, 5, 3, 2},
                        {3, 4, 1, 3, 2, 1, 1, 2}},
                        {
                        {1, 1, 1, 1, 1, 1, 1, 1},
                        {2, 3, 2, 3, 2, 3, 2, 3},
                        {2, 2, 2, 2, 2, 5, 2, 2},
                        {6, 4, 5, 3, 4, 1, 3, 2}}};

                        
/** Computes n_prs values used to compute alpha as defined in 5.5.2.1.1 of 36.211 */
static int generate_n_prs(srslte_refsignal_ul_t * q) {
  /* Calculate n_prs */
  uint32_t c_init; 
  
  srslte_sequence_t seq; 
  bzero(&seq, sizeof(srslte_sequence_t));
    
  for (uint32_t delta_ss=0;delta_ss<SRSLTE_NOF_DELTA_SS;delta_ss++) {
    c_init = ((q->cell.id / 30) << 5) + (((q->cell.id % 30) + delta_ss) % 30);
    if (srslte_sequence_LTE_pr(&seq, 8 * SRSLTE_CP_NSYMB(q->cell.cp) * 20, c_init)) {
      return SRSLTE_ERROR;
    }
    for (uint32_t ns=0;ns<SRSLTE_NSLOTS_X_FRAME;ns++) {  
      uint32_t n_prs = 0;
      for (int i = 0; i < 8; i++) {
        n_prs += (seq.c[8 * SRSLTE_CP_NSYMB(q->cell.cp) * ns + i] << i);
      }
      q->n_prs_pusch[delta_ss][ns] = n_prs;
    }
    srslte_sequence_free(&seq);
  }

  return SRSLTE_SUCCESS; 
}

void srslte_refsignal_r_uv_arg_1prb(float *arg, uint32_t u) {
  for (int i = 0; i < SRSLTE_NRE; i++) {
    arg[i] = phi_M_sc_12[u][i] * M_PI / 4;
  }
}

static int generate_srslte_sequence_hopping_v(srslte_refsignal_ul_t *q) {
  srslte_sequence_t seq; 
  bzero(&seq, sizeof(srslte_sequence_t));
  
  for (uint32_t ns=0;ns<SRSLTE_NSLOTS_X_FRAME;ns++) {
    for (uint32_t delta_ss=0;delta_ss<SRSLTE_NOF_DELTA_SS;delta_ss++) {
      if (srslte_sequence_LTE_pr(&seq, 20, ((q->cell.id / 30) << 5) + ((q->cell.id%30)+delta_ss)%30)) {
        return SRSLTE_ERROR;
      }
      q->v_pusch[ns][delta_ss] = seq.c[ns];    
      srslte_sequence_free(&seq);
    }
  }
  return SRSLTE_SUCCESS;
}


/** Initializes srslte_refsignal_ul_t object according to 3GPP 36.211 5.5
 *
 */
int srslte_refsignal_ul_init(srslte_refsignal_ul_t * q, srslte_cell_t cell)
{

  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && srslte_cell_isvalid(&cell)) {

    ret = SRSLTE_ERROR; 
    
    bzero(q, sizeof(srslte_refsignal_ul_t));
    q->cell = cell; 
    
    // Allocate temporal buffer for computing signal argument
    q->tmp_arg = srslte_vec_malloc(SRSLTE_NRE * q->cell.nof_prb * sizeof(cf_t)); 
    if (!q->tmp_arg) {
      perror("malloc");
      goto free_and_exit;
    }
    
    srslte_pucch_cfg_default(&q->pucch_cfg);
    
    // Precompute n_prs
    if (generate_n_prs(q)) {
      goto free_and_exit;
    }
    
    // Precompute group hopping values u. 
    if (srslte_group_hopping_f_gh(q->f_gh, q->cell.id)) {
      goto free_and_exit;
    }
    
    // Precompute sequence hopping values v. Uses f_ss_pusch
    if (generate_srslte_sequence_hopping_v(q)) {
      goto free_and_exit;
    }

    if (srslte_pucch_n_cs_cell(q->cell, q->n_cs_cell)) {
      goto free_and_exit;
    }

    ret = SRSLTE_SUCCESS;
  }
free_and_exit:
  if (ret == SRSLTE_ERROR) {
    srslte_refsignal_ul_free(q);
  }
  return ret;
}

void srslte_refsignal_ul_free(srslte_refsignal_ul_t * q) {
  if (q->tmp_arg) {
    free(q->tmp_arg);
  }
  bzero(q, sizeof(srslte_refsignal_ul_t));
}

void srslte_refsignal_ul_set_cfg(srslte_refsignal_ul_t *q, 
                                  srslte_refsignal_dmrs_pusch_cfg_t *pusch_cfg,
                                  srslte_pucch_cfg_t *pucch_cfg, 
                                  srslte_refsignal_srs_cfg_t *srs_cfg) 
{
  if (pusch_cfg) {
    memcpy(&q->pusch_cfg, pusch_cfg, sizeof(srslte_refsignal_dmrs_pusch_cfg_t));        
  }
  if (pucch_cfg) {
    if (srslte_pucch_cfg_isvalid(pucch_cfg, q->cell.nof_prb)) {
      memcpy(&q->pucch_cfg, pucch_cfg, sizeof(srslte_pucch_cfg_t));    
    }
  }
  if (srs_cfg) {
    memcpy(&q->srs_cfg, srs_cfg, sizeof(srslte_refsignal_srs_cfg_t));
  }
}

uint32_t largest_prime_lower_than(uint32_t x) {
  /* get largest prime n_zc<len */
  for (uint32_t i = NOF_PRIME_NUMBERS - 1; i > 0; i--) {
    if (prime_numbers[i] < x) {
      return prime_numbers[i];
    }
  }
  return 0;
}

static void arg_r_uv_2prb(float *arg, uint32_t u) {
  for (int i = 0; i < 2*SRSLTE_NRE; i++) {
    arg[i] = phi_M_sc_24[u][i] * M_PI / 4;
  }  
}

static uint32_t get_q(uint32_t u, uint32_t v, uint32_t N_sz) {
  float q;
  float q_hat;
  float n_sz = (float) N_sz;
  
  q_hat = n_sz *(u + 1) / 31;
  if ((((uint32_t) (2 * q_hat)) % 2) == 0) {
    q = q_hat + 0.5 + v;
  } else {
    q = q_hat + 0.5 - v;
  }
  return (uint32_t) q; 
}

static void arg_r_uv_mprb(float *arg, uint32_t M_sc, uint32_t u, uint32_t v) {

  uint32_t N_sz = largest_prime_lower_than(M_sc);
  if (N_sz > 0) {
    float q = get_q(u,v,N_sz);
    float n_sz = (float) N_sz;
    for (uint32_t i = 0; i < M_sc; i++) {
      float m = (float) (i%N_sz);
      arg[i] =  -M_PI * q * m * (m + 1) / n_sz;
    }
  }
}

/* Computes argument of r_u_v signal */
static void compute_r_uv_arg(srslte_refsignal_ul_t *q, uint32_t nof_prb, uint32_t u, uint32_t v) {
  if (nof_prb == 1) {
    srslte_refsignal_r_uv_arg_1prb(q->tmp_arg, u);
  } else if (nof_prb == 2) {
    arg_r_uv_2prb(q->tmp_arg, u);
  } else {
    arg_r_uv_mprb(q->tmp_arg, SRSLTE_NRE*nof_prb, u, v);
  }
}

/* Calculates alpha according to 5.5.2.1.1 of 36.211 */
static float pusch_alpha(srslte_refsignal_ul_t *q, srslte_refsignal_dmrs_pusch_cfg_t *cfg, 
                           uint32_t cyclic_shift_for_dmrs, uint32_t ns) 
{
  uint32_t n_dmrs_2_val = n_dmrs_2[cyclic_shift_for_dmrs];  
  uint32_t n_cs = (n_dmrs_1[cfg->cyclic_shift] + n_dmrs_2_val + q->n_prs_pusch[cfg->delta_ss][ns]) % 12;
  
  return 2 * M_PI * (n_cs) / 12;

}

bool srslte_refsignal_dmrs_pusch_cfg_isvalid(srslte_refsignal_ul_t *q, srslte_refsignal_dmrs_pusch_cfg_t *cfg, 
                                             uint32_t nof_prb) {
  if (cfg->cyclic_shift          < SRSLTE_NOF_CSHIFT   && 
      cfg->delta_ss              < SRSLTE_NOF_DELTA_SS &&
      nof_prb                    <= q->cell.nof_prb) {
    return true; 
  } else {
    return false;
  }
}

void srslte_refsignal_dmrs_pusch_put(srslte_refsignal_ul_t *q, cf_t *r_pusch, uint32_t nof_prb, uint32_t n_prb[2], cf_t *sf_symbols) 
{
  for (uint32_t ns_idx=0;ns_idx<2;ns_idx++) {
    INFO("Putting DRMS to n_prb: %d, L: %d, ns_idx: %d\n", n_prb[ns_idx], nof_prb, ns_idx);
    uint32_t L = SRSLTE_REFSIGNAL_UL_L(ns_idx, q->cell.cp);
    memcpy(&sf_symbols[SRSLTE_RE_IDX(q->cell.nof_prb, L, n_prb[ns_idx]*SRSLTE_NRE)], 
           &r_pusch[ns_idx*SRSLTE_NRE*nof_prb], nof_prb*SRSLTE_NRE*sizeof(cf_t));    
  }
}

void srslte_refsignal_dmrs_pusch_get(srslte_refsignal_ul_t *q, cf_t *sf_symbols, uint32_t nof_prb, uint32_t n_prb[2], cf_t *r_pusch) 
{
  for (uint32_t ns_idx=0;ns_idx<2;ns_idx++) {
    INFO("Getting DRMS from n_prb: %d, L: %d, ns_idx: %d\n", n_prb[ns_idx], nof_prb, ns_idx);
    uint32_t L = SRSLTE_REFSIGNAL_UL_L(ns_idx, q->cell.cp);
    memcpy(&r_pusch[ns_idx*SRSLTE_NRE*nof_prb],
           &sf_symbols[SRSLTE_RE_IDX(q->cell.nof_prb, L, n_prb[ns_idx]*SRSLTE_NRE)], 
           nof_prb*SRSLTE_NRE*sizeof(cf_t));    
  }
}

/* Computes r sequence */
void compute_r(srslte_refsignal_ul_t *q, uint32_t nof_prb, uint32_t ns, uint32_t delta_ss) {
  // Get group hopping number u 
  uint32_t f_gh=0; 
  if (q->pusch_cfg.group_hopping_en) {
    f_gh = q->f_gh[ns];
  }
  uint32_t u = (f_gh + (q->cell.id%30)+delta_ss)%30;

  // Get sequence hopping number v 
  uint32_t v = 0; 
  if (nof_prb >= 6 && q->pusch_cfg.sequence_hopping_en) {
    v = q->v_pusch[ns][q->pusch_cfg.delta_ss];
  }

  // Compute signal argument 
  compute_r_uv_arg(q, nof_prb, u, v);

}

int srslte_refsignal_dmrs_pusch_pregen(srslte_refsignal_ul_t *q, srslte_refsignal_ul_dmrs_pregen_t *pregen)
{
  for (uint32_t sf_idx=0;sf_idx<SRSLTE_NSUBFRAMES_X_FRAME;sf_idx++) {
    for (uint32_t cs=0;cs<SRSLTE_NOF_CSHIFT;cs++) {
      pregen->r[cs][sf_idx] = (cf_t**) calloc(sizeof(cf_t*), q->cell.nof_prb + 1);
      if (pregen->r[cs][sf_idx]) {
        for (uint32_t n=0;n<=q->cell.nof_prb;n++) {
          if (srslte_dft_precoding_valid_prb(n)) {
            pregen->r[cs][sf_idx][n] = (cf_t*) srslte_vec_malloc(sizeof(cf_t)*n*2*SRSLTE_NRE);
            if (pregen->r[cs][sf_idx][n]) {
              if (srslte_refsignal_dmrs_pusch_gen(q, n, sf_idx, cs, pregen->r[cs][sf_idx][n])) {
                return SRSLTE_ERROR; 
              }
            } else {
              return SRSLTE_ERROR;
            }     
          }
        }
      } else {
        return SRSLTE_ERROR;
      }
    }
  }
  return SRSLTE_SUCCESS;
}

void srslte_refsignal_dmrs_pusch_pregen_free(srslte_refsignal_ul_t *q, srslte_refsignal_ul_dmrs_pregen_t *pregen)
{
  for (uint32_t sf_idx=0;sf_idx<SRSLTE_NSUBFRAMES_X_FRAME;sf_idx++) {
    for (uint32_t cs=0;cs<SRSLTE_NOF_CSHIFT;cs++) {
      if (pregen->r[cs][sf_idx]) {
        for (uint32_t n=0;n<=q->cell.nof_prb;n++) {
          if (srslte_dft_precoding_valid_prb(n)) {
            if (pregen->r[cs][sf_idx][n]) {
              free(pregen->r[cs][sf_idx][n]);
            }
          }
        }
        free(pregen->r[cs][sf_idx]);      
      }
    }
  }
}

int srslte_refsignal_dmrs_pusch_pregen_put(srslte_refsignal_ul_t *q, 
                                            srslte_refsignal_ul_dmrs_pregen_t *pregen, 
                                            uint32_t nof_prb, 
                                            uint32_t sf_idx,
                                            uint32_t cyclic_shift_for_dmrs,
                                            uint32_t n_prb[2], 
                                            cf_t *sf_symbols)
{
  if (srslte_dft_precoding_valid_prb(nof_prb) && 
      sf_idx < SRSLTE_NSUBFRAMES_X_FRAME      &&
      cyclic_shift_for_dmrs < SRSLTE_NOF_CSHIFT) 
  {
    srslte_refsignal_dmrs_pusch_put(q, pregen->r[cyclic_shift_for_dmrs][sf_idx][nof_prb], nof_prb, n_prb, sf_symbols);
    return SRSLTE_SUCCESS;
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}


/* Generate DRMS for PUSCH signal according to 5.5.2.1 of 36.211 */
int srslte_refsignal_dmrs_pusch_gen(srslte_refsignal_ul_t *q, uint32_t nof_prb, uint32_t sf_idx, 
                                    uint32_t cyclic_shift_for_dmrs, cf_t *r_pusch) 
{

  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (srslte_refsignal_dmrs_pusch_cfg_isvalid(q, &q->pusch_cfg, nof_prb)) {
    ret = SRSLTE_ERROR;
    
    for (uint32_t ns=2*sf_idx;ns<2*(sf_idx+1);ns++) {
      
      compute_r(q, nof_prb, ns, q->pusch_cfg.delta_ss);
      
      // Add cyclic prefix alpha
      float alpha = pusch_alpha(q, &q->pusch_cfg, cyclic_shift_for_dmrs, ns);

      // Do complex exponential and adjust amplitude
      for (int i=0;i<SRSLTE_NRE*nof_prb;i++) {
        r_pusch[(ns%2)*SRSLTE_NRE*nof_prb+i] = cexpf(I*(q->tmp_arg[i] + alpha*i));
      }      
    }
    ret = 0; 
  }
  return ret; 
}

/* Number of PUCCH demodulation reference symbols per slot N_rs_pucch tABLE 5.5.2.2.1-1 36.211 */
static uint32_t get_N_rs(srslte_pucch_format_t format, srslte_cp_t cp) {
  switch (format) {
    case SRSLTE_PUCCH_FORMAT_1:
    case SRSLTE_PUCCH_FORMAT_1A:
    case SRSLTE_PUCCH_FORMAT_1B:
      if (SRSLTE_CP_ISNORM(cp)) {
        return 3; 
      } else {
        return 2; 
      }
    case SRSLTE_PUCCH_FORMAT_2:
      if (SRSLTE_CP_ISNORM(cp)) {
        return 2; 
      } else {
        return 1; 
      }
    case SRSLTE_PUCCH_FORMAT_2A:
    case SRSLTE_PUCCH_FORMAT_2B:
      return 2; 
  }
  return 0; 
}

/* Table 5.5.2.2.2-1: Demodulation reference signal location for different PUCCH formats. 36.211 */
static uint32_t get_pucch_dmrs_symbol(uint32_t m, srslte_pucch_format_t format, srslte_cp_t cp) {
  switch (format) {
    case SRSLTE_PUCCH_FORMAT_1:
    case SRSLTE_PUCCH_FORMAT_1A:
    case SRSLTE_PUCCH_FORMAT_1B:
      if (SRSLTE_CP_ISNORM(cp)) {
        if (m < 3) {
          return pucch_dmrs_symbol_format1_cpnorm[m];           
        }
      } else {
        if (m < 2) {
          return pucch_dmrs_symbol_format1_cpext[m]; 
        }
      }
      break;
    case SRSLTE_PUCCH_FORMAT_2:
      if (SRSLTE_CP_ISNORM(cp)) {
        if (m < 2) {
          return pucch_dmrs_symbol_format2_cpnorm[m];           
        }
      } else {
        if (m < 1) {
          return pucch_dmrs_symbol_format2_cpext[m]; 
        }
      }
      break;
    case SRSLTE_PUCCH_FORMAT_2A:
    case SRSLTE_PUCCH_FORMAT_2B:
      if (m < 2) {
        return pucch_dmrs_symbol_format2_cpnorm[m]; 
      }
      break;
    default:
      fprintf(stderr, "Unsupported format %d\n", format);
      return 0; 
  }
  return 0; 
}

/* Generates DMRS for PUCCH according to 5.5.2.2 in 36.211 */
int srslte_refsignal_dmrs_pucch_gen(srslte_refsignal_ul_t *q, srslte_pucch_format_t format, uint32_t n_pucch, 
                                    uint32_t sf_idx, uint8_t pucch_bits[2], cf_t *r_pucch) 
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (q && r_pucch) {
    ret = SRSLTE_ERROR;
    
    uint32_t N_rs=get_N_rs(format, q->cell.cp); 
    
    cf_t z_m_1 = 1.0;     
    if (format == SRSLTE_PUCCH_FORMAT_2A || format == SRSLTE_PUCCH_FORMAT_2B) {
      srslte_pucch_format2ab_mod_bits(format, pucch_bits, &z_m_1);
    }
    
    for (uint32_t ns=2*sf_idx;ns<2*(sf_idx+1);ns++) {
      // Get group hopping number u 
      uint32_t f_gh=0; 
      if (q->pusch_cfg.group_hopping_en) {
        f_gh = q->f_gh[ns];
      }
      uint32_t u = (f_gh + (q->cell.id%30))%30;
      
      srslte_refsignal_r_uv_arg_1prb(q->tmp_arg, u); 
      
      for (uint32_t m=0;m<N_rs;m++) {
        uint32_t n_oc=0; 
        
        uint32_t l = get_pucch_dmrs_symbol(m, format, q->cell.cp);
        // Add cyclic prefix alpha
        float alpha = 0.0; 
        if (format < SRSLTE_PUCCH_FORMAT_2) {
          alpha = srslte_pucch_alpha_format1(q->n_cs_cell, &q->pucch_cfg, n_pucch, q->cell.cp, true, ns, l, &n_oc, NULL);
        } else {
          alpha = srslte_pucch_alpha_format2(q->n_cs_cell, &q->pucch_cfg, n_pucch, ns, l);
        }

        // Choose number of symbols and orthogonal sequence from Tables 5.5.2.2.1-1 to -3 
        float *w=NULL;
        switch (format) {
          case SRSLTE_PUCCH_FORMAT_1:
          case SRSLTE_PUCCH_FORMAT_1A:
          case SRSLTE_PUCCH_FORMAT_1B:
            if (SRSLTE_CP_ISNORM(q->cell.cp)) {
              w=w_arg_pucch_format1_cpnorm[n_oc];
            } else {
              w=w_arg_pucch_format1_cpext[n_oc];
            }
            break;
          case SRSLTE_PUCCH_FORMAT_2:
            if (SRSLTE_CP_ISNORM(q->cell.cp)) {
              w=w_arg_pucch_format2_cpnorm;
            } else {
              w=w_arg_pucch_format2_cpext;
            }
            break;
          case SRSLTE_PUCCH_FORMAT_2A:
          case SRSLTE_PUCCH_FORMAT_2B:
            w=w_arg_pucch_format2_cpnorm;
            break;
        }
        cf_t z_m = 1.0; 
        if (m == 1) {
          z_m = z_m_1; 
        }
        if (w) {
          for (uint32_t n=0;n<SRSLTE_NRE;n++) {
            r_pucch[(ns%2)*SRSLTE_NRE*N_rs+m*SRSLTE_NRE+n] = z_m*cexpf(I*(w[m]+q->tmp_arg[n]+alpha*n));
          }                                 
        } else {
          return SRSLTE_ERROR; 
        }          
      }
    }
    ret = SRSLTE_SUCCESS; 
  }
  return ret;   
}

/* Maps PUCCH DMRS to the physical resources as defined in 5.5.2.2.2 in 36.211 */
int srslte_refsignal_dmrs_pucch_put(srslte_refsignal_ul_t *q, srslte_pucch_format_t format, uint32_t n_pucch, cf_t *r_pucch, cf_t *output) 
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  if (q && output && r_pucch) {
    ret = SRSLTE_ERROR; 
    uint32_t nsymbols = SRSLTE_CP_ISNORM(q->cell.cp)?SRSLTE_CP_NORM_NSYMB:SRSLTE_CP_EXT_NSYMB;
    
    // Determine m 
    uint32_t m = srslte_pucch_m(&q->pucch_cfg, format, n_pucch, q->cell.cp); 
    
    uint32_t N_rs = get_N_rs(format, q->cell.cp);
    for (uint32_t ns=0;ns<2;ns++) {
      // Determine n_prb 
      uint32_t n_prb = m/2; 
      if ((m+ns)%2) {
        n_prb = q->cell.nof_prb-1-m/2; 
      }
      
      for (uint32_t i=0;i<N_rs;i++) {
        uint32_t l = get_pucch_dmrs_symbol(i, format, q->cell.cp);
        memcpy(&output[SRSLTE_RE_IDX(q->cell.nof_prb, l+ns*nsymbols, n_prb*SRSLTE_NRE)], 
               &r_pucch[ns*N_rs*SRSLTE_NRE+i*SRSLTE_NRE], 
               SRSLTE_NRE*sizeof(cf_t));
      }
    }
    
    ret = SRSLTE_SUCCESS; 
  }
  return ret;   
}


uint32_t T_srs_table(uint32_t I_srs) {
  uint32_t T_srs; 
  /* This is Table 8.2-1 */
  if (I_srs < 2) {
    T_srs   = 2; 
  } else if (I_srs < 7) {
    T_srs   = 5; 
  } else if (I_srs < 17) {
    T_srs   = 10; 
  } else if (I_srs < 37) {
    T_srs   = 20; 
  } else if (I_srs < 77) {
    T_srs   = 40; 
  } else if (I_srs < 157) {
    T_srs   = 80; 
  } else if (I_srs < 317) {
    T_srs   = 160; 
  } else if (I_srs < 637) {
    T_srs   = 320; 
  } else {
    T_srs = 0; 
  }
  return T_srs; 
}

/* Returns 1 if tti is a valid subframe for SRS transmission according to I_srs (UE-specific 
 * configuration index), as defined in Section 8.1 of 36.213. 
 * Returns 0 if no SRS shall be transmitted or a negative number if error. 
 */
int srslte_refsignal_srs_send_ue(uint32_t I_srs, uint32_t tti) {
  if (I_srs < 1024 && tti < 10240) {
    uint32_t Toffset = 0;
    /* This is Table 8.2-1 */
    if (I_srs < 2) {
      Toffset = I_srs;  
    } else if (I_srs < 7) {
      Toffset = I_srs-2;        
    } else if (I_srs < 17) {
      Toffset = I_srs-7;        
    } else if (I_srs < 37) {
      Toffset = I_srs-17;        
    } else if (I_srs < 77) {
      Toffset = I_srs-37;        
    } else if (I_srs < 157) {
      Toffset = I_srs-77;        
    } else if (I_srs < 317) {
      Toffset = I_srs-157;        
    } else if (I_srs < 637) {
      Toffset = I_srs-317;        
    } else {
      return 0; 
    }
    if (((tti-Toffset)%T_srs_table(I_srs)) == 0) {
      return 1; 
    } else {
      return 0; 
    }
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

/* Returns 1 if sf_idx is a valid subframe for SRS transmission according to subframe_config (cell-specific), 
 * as defined in Section 5.5.3.3 of 36.211. Returns 0 if no SRS shall be transmitted or a negative
 * number if error. 
 */
int srslte_refsignal_srs_send_cs(uint32_t subframe_config, uint32_t sf_idx) {
  if (subframe_config < 15 && sf_idx < 10) {
    uint32_t tsfc = T_sfc[subframe_config];
    if (subframe_config < 7) {
      if ((sf_idx%tsfc)==Delta_sfc1[subframe_config]) {
        return 1; 
      } else {
        return 0; 
      }
    } else if (subframe_config == 7) {
      if (((sf_idx%tsfc)==0) || ((sf_idx%tsfc)==1)){
        return 1; 
      } else {
        return 0; 
      }
    } else if (subframe_config == 8) {
      if (((sf_idx%tsfc)==2) || ((sf_idx%tsfc)==3)){
        return 1; 
      } else {
        return 0; 
      }
    } else if (subframe_config < 13) {
      if ((sf_idx%tsfc)==Delta_sfc2[subframe_config-9]) {
        return 1; 
      } else {
        return 0; 
      }
    } else if (subframe_config == 13) {
      if (((sf_idx%tsfc)==5) || ((sf_idx%tsfc)==7) || ((sf_idx%tsfc)==9)){
        return 0; 
      } else {
        return 1; 
      }
    } else if (subframe_config == 14) {
      if (((sf_idx%tsfc)==7) || ((sf_idx%tsfc)==9)) {
        return 0; 
      } else {
        return 1; 
      }
    } else {
      return 0; 
    }
  } else {
    return SRSLTE_ERROR_INVALID_INPUTS;
  }
}

uint32_t srsbwtable_idx(uint32_t nof_prb) {
  if (nof_prb <= 40) {
    return 0;
  } else if (nof_prb <= 60) {
    return 1; 
  } else if (nof_prb <= 80) {
    return 2; 
  } else {
    return 3; 
  }
}
                        
/* Returns start of common SRS BW region */
uint32_t srslte_refsignal_srs_rb_start_cs(uint32_t bw_cfg, uint32_t nof_prb) {
  if (bw_cfg < 8) {
    return nof_prb/2-m_srs_b[srsbwtable_idx(nof_prb)][0][bw_cfg]/2;
  }
  return 0; 
}

/* Returns number of RB defined for the cell-specific SRS */
uint32_t srslte_refsignal_srs_rb_L_cs(uint32_t bw_cfg, uint32_t nof_prb) {
  if (bw_cfg < 8) {
    return m_srs_b[srsbwtable_idx(nof_prb)][0][bw_cfg];
  } 
  return 0; 
}

uint32_t srs_Fb(srslte_refsignal_srs_cfg_t *cfg, uint32_t b, uint32_t nof_prb, uint32_t tti) {
  uint32_t Fb = 0;
  uint32_t T = T_srs_table(cfg->I_srs);
  if (T) {
    uint32_t n_srs = tti/T; 
    uint32_t N_b = Nb[srsbwtable_idx(nof_prb)][b][cfg->bw_cfg]; 
  
    uint32_t prod_1=1;
    for (uint32_t bp=cfg->b_hop+1;bp<b;bp++) {
      prod_1 *= Nb[srsbwtable_idx(nof_prb)][bp][cfg->bw_cfg];
    }
    uint32_t prod_2 = prod_1*Nb[srsbwtable_idx(nof_prb)][b][cfg->bw_cfg];
    if ((N_b%2) == 0) {
      Fb = (N_b/2)*((n_srs%prod_2)/prod_1)+((n_srs%prod_2)/prod_1/2);
    } else {
      Fb = (N_b/2)*(n_srs/prod_1);
    }
  }
  return Fb;
}

/* Returns k0: frequency-domain starting position for ue-specific SRS */
uint32_t srs_k0_ue(srslte_refsignal_srs_cfg_t *cfg, uint32_t nof_prb, uint32_t tti) {
  
  if (cfg->bw_cfg < 8 && cfg->B < 4 && cfg->k_tc < 2) {    
    uint32_t k0p = srslte_refsignal_srs_rb_start_cs(cfg->bw_cfg, nof_prb)*SRSLTE_NRE + cfg->k_tc; 
    uint32_t k0 = k0p;
    uint32_t nb = 0; 
    for (int b=0;b<=cfg->B;b++) {
      uint32_t m_srs = m_srs_b[srsbwtable_idx(nof_prb)][b][cfg->bw_cfg]; 
      uint32_t m_sc = m_srs*SRSLTE_NRE/2;
      if (b <= cfg->b_hop) {
        nb = (4*cfg->n_rrc/m_srs)%Nb[srsbwtable_idx(nof_prb)][b][cfg->bw_cfg]; 
      } else {
        uint32_t Fb=srs_Fb(cfg, b, nof_prb, tti);
        nb = ((4*cfg->n_rrc/m_srs)+Fb)%Nb[srsbwtable_idx(nof_prb)][b][cfg->bw_cfg]; 
      }
      k0 += 2*m_sc*nb;      

    }
    return k0;
  }
  return 0; 
}

uint32_t srslte_refsignal_srs_M_sc(srslte_refsignal_ul_t *q) {
  return m_srs_b[srsbwtable_idx(q->cell.nof_prb)][q->srs_cfg.B][q->srs_cfg.bw_cfg]*SRSLTE_NRE/2;
}

int srslte_refsignal_srs_pregen(srslte_refsignal_ul_t *q, srslte_refsignal_srs_pregen_t *pregen)
{  
  uint32_t M_sc = srslte_refsignal_srs_M_sc(q); 
  for (uint32_t sf_idx=0;sf_idx<SRSLTE_NSUBFRAMES_X_FRAME;sf_idx++) {
    pregen->r[sf_idx] = srslte_vec_malloc(2*M_sc*sizeof(cf_t));
    if (pregen->r[sf_idx]) {
      if (srslte_refsignal_srs_gen(q, sf_idx, pregen->r[sf_idx])) {
        return SRSLTE_ERROR; 
      }
    } else {
      return SRSLTE_ERROR;
    }
  }
  return SRSLTE_SUCCESS;
}

void srslte_refsignal_srs_pregen_free(srslte_refsignal_ul_t *q, srslte_refsignal_srs_pregen_t *pregen) 
{
  for (uint32_t sf_idx=0;sf_idx<SRSLTE_NSUBFRAMES_X_FRAME;sf_idx++) {
    if (pregen->r[sf_idx]) {
      free(pregen->r[sf_idx]);
    }
  }
}

int srslte_refsignal_srs_pregen_put(srslte_refsignal_ul_t *q, srslte_refsignal_srs_pregen_t *pregen, 
                                    uint32_t tti, cf_t *sf_symbols)
{
  return srslte_refsignal_srs_put(q, tti, pregen->r[tti%SRSLTE_NSUBFRAMES_X_FRAME], sf_symbols);  
}


/* Genearte SRS signal as defined in Section 5.5.3.1 */
int srslte_refsignal_srs_gen(srslte_refsignal_ul_t *q, uint32_t sf_idx, cf_t *r_srs) 
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (r_srs && q) {
    ret = SRSLTE_ERROR;
    
    uint32_t M_sc = srslte_refsignal_srs_M_sc(q); 
    for (uint32_t ns=2*sf_idx;ns<2*(sf_idx+1);ns++) {
      
      compute_r(q, M_sc/SRSLTE_NRE, ns, 0);
      float alpha = 2*M_PI*q->srs_cfg.n_srs/8; 

      // Do complex exponential and adjust amplitude
      for (int i=0;i<M_sc;i++) {
        r_srs[(ns%2)*M_sc+i] = cexpf(I*(q->tmp_arg[i] + alpha*i));
      }     
    }
    ret = SRSLTE_SUCCESS; 
  }
  return ret; 
}

int srslte_refsignal_srs_put(srslte_refsignal_ul_t *q, uint32_t tti, cf_t *r_srs, cf_t *sf_symbols) {
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (r_srs && q) {
    ret = SRSLTE_ERROR;
    
    uint32_t M_sc = srslte_refsignal_srs_M_sc(q); 
    uint32_t k0 = srs_k0_ue(&q->srs_cfg, q->cell.nof_prb, tti);
    for (int i=0;i<M_sc;i++) {
      sf_symbols[SRSLTE_RE_IDX(q->cell.nof_prb, 2*SRSLTE_CP_NSYMB(q->cell.cp)-1, k0 + 2*i)] = r_srs[i];
    }
    ret = SRSLTE_SUCCESS; 
  }
  return ret; 
}

