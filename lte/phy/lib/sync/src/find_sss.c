/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include <string.h>
#include <complex.h>
#include <math.h>

#include "liblte/phy/utils/vector.h"
#include "liblte/phy/sync/sss.h"

#define MAX_M 3


static void corr_all_zs(cf_t z[N_SSS], float s[N_SSS][N_SSS-1], float output[N_SSS]) {
  uint32_t m;
  cf_t tmp[N_SSS]; 
  
  for (m = 0; m < N_SSS; m++) {
    tmp[m] = vec_dot_prod_cfc(z, s[m], N_SSS - 1);    
  }
  vec_abs_square_cf(tmp, output, N_SSS);
}

static void corr_all_sz_partial(cf_t z[N_SSS], float s[N_SSS][N_SSS], uint32_t M, float output[N_SSS]) {
  uint32_t Nm = N_SSS/M; 
  cf_t tmp[N_SSS];
  float tmp_abs[MAX_M-1][N_SSS];
  int j, m; 
  float *ptr; 
  
  for (j=0;j<M;j++) {
    for (m = 0; m < N_SSS; m++) {      
      tmp[m] = vec_dot_prod_cfc(&z[j*Nm], &s[m][j*Nm], Nm);        
    }
    if (j == 0) {
      ptr = output; 
    } else {
      ptr = tmp_abs[j-1];
    }
    vec_abs_square_cf(tmp, ptr, N_SSS);   
  }
  for (j=1;j<M;j++) {
    vec_sum_fff(tmp_abs[j-1], output, output, N_SSS);
  }    
}

static void extract_pair_sss(sss_synch_t *q, cf_t *input, cf_t *ce, cf_t y[2][N_SSS]) {
  cf_t input_fft[SYMBOL_SZ_MAX];

  
  dft_run_c(&q->dftp_input, &input[CP_NORM(5, q->fft_size)], input_fft);
  
  if (ce) {
    vec_prod_conj_ccc(&input_fft[q->fft_size/2-N_SSS], ce, &input_fft[q->fft_size/2-N_SSS], 2*N_SSS);
  }
  
  for (int i = 0; i < N_SSS; i++) {
    y[0][i] = input_fft[q->fft_size/2-N_SSS + 2 * i];
    y[1][i] = input_fft[q->fft_size/2-N_SSS + 2 * i + 1];
  }

  vec_prod_cfc(y[0], q->fc_tables[q->N_id_2].c[0], y[0], N_SSS);
  vec_prod_cfc(y[1], q->fc_tables[q->N_id_2].c[1], y[1], N_SSS);

}    

int sss_synch_m0m1_diff(sss_synch_t *q, cf_t *input, uint32_t *m0, float *m0_value,
    uint32_t *m1, float *m1_value) {
  return sss_synch_m0m1_diff_coh(q, input, NULL, m0, m0_value, m1, m1_value);
}

/* Differential SSS estimation. 
 * Returns m0 and m1 estimates 
 *
 * Source: "SSS Detection Method for Initial Cell Search in 3GPP LTE FDD/TDD Dual Mode Receiver"
 *       Jung-In Kim, Jung-Su Han, Hee-Jin Roh and Hyung-Jin Choi

 *
 */
int sss_synch_m0m1_diff_coh(sss_synch_t *q, cf_t *input, cf_t ce[2*N_SSS], uint32_t *m0, float *m0_value,
    uint32_t *m1, float *m1_value) 
{

  int ret = LIBLTE_ERROR_INVALID_INPUTS; 

  if (q                 != NULL   &&
      input             != NULL   &&
      m0                != NULL   && 
      m1                != NULL)
  {
    
    cf_t yprod[N_SSS];
    cf_t y[2][N_SSS];

    extract_pair_sss(q, input, ce, y);
    
    vec_prod_conj_ccc(&y[0][1], y[0], yprod, N_SSS - 1);    
    corr_all_zs(yprod, q->fc_tables[q->N_id_2].sd, q->corr_output_m0);
    *m0 = vec_max_fi(q->corr_output_m0, N_SSS);
    if (m0_value) {
      *m0_value = q->corr_output_m0[*m0];
    }    
    
    vec_prod_cfc(y[1], q->fc_tables[q->N_id_2].z1[*m0], y[1], N_SSS);
    vec_prod_conj_ccc(&y[1][1], y[1], yprod, N_SSS - 1);
    corr_all_zs(yprod, q->fc_tables[q->N_id_2].sd, q->corr_output_m1);
    *m1 = vec_max_fi(q->corr_output_m1, N_SSS);
    if (m1_value) {
      *m1_value = q->corr_output_m1[*m1];
    }    
    ret = LIBLTE_SUCCESS;
  } 
  return ret; 
}

/* Partial correlation SSS estimation. 
 * Returns m0 and m1 estimates 
 *
 * Source: "SSS Detection Method for Initial Cell Search in 3GPP LTE FDD/TDD Dual Mode Receiver"
 *       Jung-In Kim, Jung-Su Han, Hee-Jin Roh and Hyung-Jin Choi

 */
int sss_synch_m0m1_partial(sss_synch_t *q, cf_t *input, uint32_t M, cf_t ce[2*N_SSS], uint32_t *m0, float *m0_value,
    uint32_t *m1, float *m1_value) 
{

  int ret = LIBLTE_ERROR_INVALID_INPUTS; 

  if (q                 != NULL   &&
      input             != NULL   &&
      m0                != NULL   && 
      m1                != NULL   && 
      M                 <= MAX_M)
  {
    cf_t y[2][N_SSS];
    
    extract_pair_sss(q, input, ce, y);
    
    corr_all_sz_partial(y[0], q->fc_tables[q->N_id_2].s, M, q->corr_output_m0);
    *m0 = vec_max_fi(q->corr_output_m0, N_SSS);
    if (m0_value) {
      *m0_value = q->corr_output_m0[*m0];
    }
    vec_prod_cfc(y[1], q->fc_tables[q->N_id_2].z1[*m0], y[1], N_SSS);
    corr_all_sz_partial(y[1], q->fc_tables[q->N_id_2].s, M, q->corr_output_m1);
    *m1 = vec_max_fi(q->corr_output_m1, N_SSS);
    if (m1_value) {
      *m1_value = q->corr_output_m1[*m1];
    }    
    ret = LIBLTE_SUCCESS;
  } 
  return ret; 
}

void convert_tables(struct fc_tables *fc_tables, struct sss_tables *in) {
  uint32_t i, j;

  for (i = 0; i < N_SSS; i++) {
    for (j = 0; j < N_SSS; j++) {
      fc_tables->z1[i][j] = (float) in->z1[i][j];
    }
  }
  for (i = 0; i < N_SSS; i++) {
    for (j = 0; j < N_SSS; j++) {
      fc_tables->s[i][j] = (float) in->s[i][j];
    }
  }
  for (i = 0; i < N_SSS; i++) {
    for (j = 0; j < N_SSS - 1; j++) {
      fc_tables->sd[i][j] = (float) in->s[i][j + 1] * in->s[i][j];
    }
  }
  for (i = 0; i < 2; i++) {
    for (j = 0; j < N_SSS; j++) {
      fc_tables->c[i][j] = (float) in->c[i][j];
    }
  }
}
