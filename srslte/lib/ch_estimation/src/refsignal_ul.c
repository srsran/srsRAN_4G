/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
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
  }

  srslte_sequence_free(&seq);
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
    }
  }
  srslte_sequence_free(&seq);
  return SRSLTE_SUCCESS;
}


/** Initializes srslte_refsignal_ul_t object according to 3GPP 36.211 5.5
 *
 */
int srslte_refsignal_ul_init(srslte_refsignal_ul_t * q, srslte_cell_t cell)
{

  int ret = SRSLTE_ERROR_INVALID_INPUTS;

  if (q != NULL && srslte_cell_isvalid(&cell)) {

    bzero(q, sizeof(srslte_refsignal_ul_t));
    q->cell = cell; 
    
    // Allocate temporal buffer for computing signal argument
    q->tmp_arg = srslte_vec_malloc(SRSLTE_NRE * q->cell.nof_prb * sizeof(cf_t)); 
    if (!q->tmp_arg) {
      perror("malloc");
      goto free_and_exit;
    }
    
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
  float q = get_q(u,v,N_sz);
  float n_sz = (float) N_sz;
  
  for (uint32_t i = 0; i < M_sc; i++) {
    float m = (float) (i%N_sz);
    arg[i] =  -M_PI * q * m * (m + 1) / n_sz;
  }
}

/* Computes argument of r_u_v signal */
static void compute_pusch_r_uv_arg(srslte_refsignal_ul_t *q, srslte_refsignal_dmrs_pusch_cfg_t *cfg, uint32_t nof_prb, uint32_t u, uint32_t v) {
  if (nof_prb == 1) {
    srslte_refsignal_r_uv_arg_1prb(q->tmp_arg, u);
  } else if (nof_prb == 2) {
    arg_r_uv_2prb(q->tmp_arg, u);
  } else {
    arg_r_uv_mprb(q->tmp_arg, SRSLTE_NRE*nof_prb, u, v);
  }
}

/* Calculates alpha according to 5.5.2.1.1 of 36.211 */
static float pusch_alpha(srslte_refsignal_ul_t *q, srslte_refsignal_dmrs_pusch_cfg_t *cfg, uint32_t ns) {
  uint32_t n_dmrs_2_val = 0; 
  if (cfg->en_dmrs_2) {
    n_dmrs_2_val = n_dmrs_2[cfg->cyclic_shift_for_dmrs];
  }
  uint32_t n_cs = (n_dmrs_1[cfg->cyclic_shift] + n_dmrs_2_val + q->n_prs_pusch[cfg->delta_ss][ns]) % 12;
  
  return 2 * M_PI * (n_cs) / 12;

}

bool srslte_refsignal_dmrs_pusch_cfg_isvalid(srslte_refsignal_ul_t *q, srslte_refsignal_dmrs_pusch_cfg_t *cfg, uint32_t nof_prb) {
  if (cfg->cyclic_shift          < SRSLTE_NOF_CSHIFT   && 
      cfg->cyclic_shift_for_dmrs < SRSLTE_NOF_CSHIFT   &&
      cfg->delta_ss              < SRSLTE_NOF_DELTA_SS &&
      nof_prb                    < q->cell.nof_prb) {
    return true; 
  } else {
    return false;
  }
}

void srslte_refsignal_dmrs_pusch_put(srslte_refsignal_ul_t *q, srslte_refsignal_dmrs_pusch_cfg_t *cfg, 
                              cf_t *r_pusch, 
                              uint32_t nof_prb, 
                              uint32_t n_prb[2], 
                              cf_t *sf_symbols) 
{
  for (uint32_t ns_idx=0;ns_idx<2;ns_idx++) {
    DEBUG("Putting DRMS to n_prb: %d, L: %d, ns_idx: %d\n", n_prb[ns_idx], nof_prb, ns_idx);
    uint32_t L = (ns_idx+1)*SRSLTE_CP_NSYMB(q->cell.cp)-4;
    memcpy(&sf_symbols[SRSLTE_RE_IDX(q->cell.nof_prb, L, n_prb[ns_idx]*SRSLTE_NRE)], 
           &r_pusch[ns_idx*SRSLTE_NRE*nof_prb], nof_prb*SRSLTE_NRE*sizeof(cf_t));    
  }
}

/* Generate DRMS for PUSCH signal according to 5.5.2.1 of 36.211 */
int srslte_refsignal_dmrs_pusch_gen(srslte_refsignal_ul_t *q, srslte_refsignal_dmrs_pusch_cfg_t *cfg, uint32_t nof_prb, uint32_t sf_idx, cf_t *r_pusch) 
{

  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (srslte_refsignal_dmrs_pusch_cfg_isvalid(q, cfg, nof_prb)) {
    ret = SRSLTE_ERROR;
    
    for (uint32_t ns=2*sf_idx;ns<2*(sf_idx+1);ns++) {
      // Get group hopping number u 
      uint32_t f_gh=0; 
      if (cfg->group_hopping_en) {
        f_gh = q->f_gh[ns];
      }
      uint32_t u = (f_gh + (q->cell.id%30)+cfg->delta_ss)%30;
      
      // Get sequence hopping number v 
      uint32_t v = 0; 
      if (nof_prb >= 6 && cfg->sequence_hopping_en) {
        v = q->v_pusch[ns][cfg->delta_ss];
      }

      // Compute signal argument 
      compute_pusch_r_uv_arg(q, cfg, nof_prb, u, v);

      // Add cyclic prefix alpha
      float alpha = pusch_alpha(q, cfg, ns);

      if (srslte_verbose == SRSLTE_VERBOSE_DEBUG) {
        uint32_t N_sz = largest_prime_lower_than(nof_prb*SRSLTE_NRE);
        DEBUG("Generating PUSCH DRMS sequence with parameters:\n",0);
        DEBUG("\tbeta: %.1f, nof_prb: %d, u: %d, v: %d, alpha: %f, N_sc: %d, root q: %d, nprs: %d\n", 
              cfg->beta_pusch, nof_prb, u, v, alpha, N_sz, get_q(u,v,N_sz),q->n_prs_pusch[cfg->delta_ss][ns]);
      }

      // Do complex exponential and adjust amplitude
      for (int i=0;i<SRSLTE_NRE*nof_prb;i++) {
        r_pusch[(ns%2)*SRSLTE_NRE*nof_prb+i] = cfg->beta_pusch * cexpf(I*(q->tmp_arg[i] + alpha*i));
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
        if (m < 4) {
          return pucch_dmrs_symbol_format1_cpnorm[m];           
        }
      } else {
        if (m < 3) {
          return pucch_dmrs_symbol_format1_cpext[m]; 
        }
      }
    case SRSLTE_PUCCH_FORMAT_2:
      if (SRSLTE_CP_ISNORM(cp)) {
        if (m < 3) {
          return pucch_dmrs_symbol_format2_cpnorm[m];           
        }
      } else {
        if (m < 2) {
          return pucch_dmrs_symbol_format2_cpext[m]; 
        }
      }
    case SRSLTE_PUCCH_FORMAT_2A:
    case SRSLTE_PUCCH_FORMAT_2B:
      if (m < 3) {
        return pucch_dmrs_symbol_format2_cpnorm[m]; 
      }
  }
  return 0; 
}

/* Generates DMRS for PUCCH according to 5.5.2.2 in 36.211 */
int srslte_refsignal_dmrs_pucch_gen(srslte_refsignal_ul_t *q, srslte_pucch_cfg_t *cfg, uint32_t sf_idx, cf_t *r_pucch) 
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS;
  if (srslte_pucch_cfg_isvalid(cfg)) {
    ret = SRSLTE_ERROR;
    
    uint32_t N_rs=get_N_rs(cfg->format, q->cell.cp); 
    
    for (uint32_t ns=2*sf_idx;ns<2*(sf_idx+1);ns++) {
      // Get group hopping number u 
      uint32_t f_gh=0; 
      if (cfg->group_hopping_en) {
        f_gh = q->f_gh[ns];
      }
      uint32_t u = (f_gh + (q->cell.id%30))%30;
      
      srslte_refsignal_r_uv_arg_1prb(q->tmp_arg, u); 
      
      for (uint32_t m=0;m<N_rs;m++) {
        uint32_t n_oc=0; 
        
        uint32_t l = get_pucch_dmrs_symbol(m, cfg->format, q->cell.cp);
        // Add cyclic prefix alpha
        float alpha = srslte_pucch_alpha(q->n_cs_cell, cfg, q->cell.cp, true, ns, l, &n_oc, NULL);

        // Choose number of symbols and orthogonal sequence from Tables 5.5.2.2.1-1 to -3 
        float *w=NULL;
        switch (cfg->format) {
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

        if (w) {
          for (uint32_t n=0;n<SRSLTE_NRE;n++) {
            r_pucch[(ns%2)*SRSLTE_NRE*N_rs+m*SRSLTE_NRE+n] = cfg->beta_pucch*w[m]*cexpf(I*(q->tmp_arg[n]+alpha*n));
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
int srslte_refsignal_dmrs_pucch_put(srslte_refsignal_ul_t *q, srslte_pucch_cfg_t *cfg, cf_t *r_pucch, cf_t *output) 
{
  int ret = SRSLTE_ERROR_INVALID_INPUTS; 
  if (q && cfg && output) {
    ret = SRSLTE_ERROR; 

    // Determine m 
    uint32_t m = srslte_pucch_m(cfg, q->cell.cp); 
    
    uint32_t N_rs = get_N_rs(cfg->format, q->cell.cp);
    for (uint32_t ns=0;ns<2;ns++) {
      // Determine n_prb 
      uint32_t n_prb = m/2; 
      if ((m+ns)%2) {
        n_prb = q->cell.nof_prb-1-m/2; 
      }
      
      for (uint32_t i=0;i<N_rs;i++) {
        uint32_t l = get_pucch_dmrs_symbol(m, cfg->format, q->cell.cp);
        memcpy(&output[SRSLTE_RE_IDX(q->cell.nof_prb, l, n_prb*SRSLTE_NRE)], 
               &r_pucch[ns*N_rs*SRSLTE_NRE+i*SRSLTE_NRE], 
               SRSLTE_NRE*sizeof(cf_t));
      }
    }
    
    ret = SRSLTE_SUCCESS; 
  }
  return ret;   
}

void srslte_refsignal_srs_gen(srslte_refsignal_ul_t *q, srslte_refsignal_srs_cfg_t *cfg, uint32_t ns, cf_t *r_srs) 
{
  
}
