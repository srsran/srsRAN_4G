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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <math.h>

#include "srslte/phy/fec/turbodecoder_simd_inter.h"
#include "srslte/phy/utils/vector.h"

#define TOTALTAIL       12

#ifdef LV_HAVE_SSE
#include <smmintrin.h>

void map_see_inter_alpha(srslte_tdec_simd_inter_t * s, int16_t *input, int16_t *parity, uint32_t long_cb);
void map_sse_inter_beta(srslte_tdec_simd_inter_t * s, int16_t *input, int16_t *parity, int16_t * output, uint32_t long_cb);
void sse_inter_update_w(srslte_tdec_simd_inter_t *h, uint16_t *deinter, uint32_t long_cb); 
void sse_inter_extract_syst1(srslte_tdec_simd_inter_t *h, uint16_t *inter, uint32_t long_cb);


static void map_sse_inter_dec(srslte_tdec_simd_inter_t * h, int16_t * input, int16_t * parity, int16_t * output,
                 uint32_t long_cb)
{
  map_see_inter_alpha(h, input, parity, long_cb);
  map_sse_inter_beta(h, input, parity, output, long_cb);
}

/************************************************
 *
 *  TURBO DECODER INTERFACE
 *
 ************************************************/
int srslte_tdec_simd_inter_init(srslte_tdec_simd_inter_t * h, uint32_t max_par_cb, uint32_t max_long_cb)
{
  int ret = -1;
  bzero(h, sizeof(srslte_tdec_simd_inter_t));
  uint32_t len = max_long_cb + 12;

  h->max_long_cb = max_long_cb;
  h->max_par_cb  = max_par_cb;

  h->llr1 = srslte_vec_malloc(sizeof(int16_t) * len * h->max_par_cb);
  if (!h->llr1) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->llr2 = srslte_vec_malloc(sizeof(int16_t) * len * h->max_par_cb);
  if (!h->llr2) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->w = srslte_vec_malloc(sizeof(int16_t) * len * h->max_par_cb);
  if (!h->w) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->syst0 = srslte_vec_malloc(sizeof(int16_t) * len * h->max_par_cb);
  if (!h->syst0) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->syst1 = srslte_vec_malloc(sizeof(int16_t) * len * h->max_par_cb);
  if (!h->syst1) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->parity0 = srslte_vec_malloc(sizeof(int16_t) * len * h->max_par_cb);
  if (!h->parity0) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->parity1 = srslte_vec_malloc(sizeof(int16_t) * len * h->max_par_cb);
  if (!h->parity1) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->alpha = srslte_vec_malloc(sizeof(int16_t) * 8*(len+12) * h->max_par_cb);
  if (!h->alpha) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }

  for (int i=0;i<SRSLTE_NOF_TC_CB_SIZES;i++) {
    if (srslte_tc_interl_init(&h->interleaver[i], srslte_cbsegm_cbsize(i)) < 0) {
      goto clean_and_exit;
    }
    srslte_tc_interl_LTE_gen(&h->interleaver[i], srslte_cbsegm_cbsize(i));
  }
  h->current_cbidx = -1; 
  ret = 0;
clean_and_exit:if (ret == -1) {
    srslte_tdec_simd_inter_free(h);
  }
  return ret;
}

void srslte_tdec_simd_inter_free(srslte_tdec_simd_inter_t * h)
{
  if (h->llr1) {
    free(h->llr1);
  }
  if (h->llr2) {
    free(h->llr2);
  }
  if (h->w) {
    free(h->w);
  }
  if (h->syst0) {
    free(h->syst0);
  }
  if (h->syst1) {
    free(h->syst1);
  }
  if (h->parity0) {
    free(h->parity0);
  }
  if (h->parity1) {
    free(h->parity1);
  }
  if (h->alpha) {
    free(h->alpha);
  }

  for (int i=0;i<SRSLTE_NOF_TC_CB_SIZES;i++) {
    srslte_tc_interl_free(&h->interleaver[i]);    
  }

  bzero(h, sizeof(srslte_tdec_simd_inter_t));
}


/* Deinterleave for inter-frame parallelization */
void extract_input(srslte_tdec_simd_inter_t *h, int16_t *input, uint32_t cbidx, uint32_t long_cb) 
{  
  for (int i=0;i<long_cb;i++) {
    h->syst0[h->max_par_cb*i+cbidx]    = input[3*i+0];
    h->parity0[h->max_par_cb*i+cbidx] = input[3*i+1];
    h->parity1[h->max_par_cb*i+cbidx] = input[3*i+2];
  }
  for (int i = long_cb; i < long_cb + 3; i++) {
    h->syst0[h->max_par_cb*i+cbidx] = input[3*long_cb + 2*(i - long_cb)];
    h->syst1[h->max_par_cb*i+cbidx] = input[3*long_cb + 2*(i - long_cb)];
    h->parity0[h->max_par_cb*i+cbidx] = input[3*long_cb + 2*(i - long_cb) + 1];
    h->parity0[h->max_par_cb*i+cbidx] = input[3*long_cb + 2*(i - long_cb) + 2];
  }
}

void srslte_tdec_simd_inter_iteration(srslte_tdec_simd_inter_t * h, int16_t *input[SRSLTE_TDEC_MAX_NPAR], uint32_t nof_cb, uint32_t long_cb)
{

  if (h->current_cbidx >= 0) {

    uint16_t *inter = h->interleaver[h->current_cbidx].forward;
    uint16_t *deinter = h->interleaver[h->current_cbidx].reverse;
    
    // Prepare systematic and parity bits for MAP DEC #1
    for (int i=0;i<nof_cb;i++) {
      if (h->n_iter[i] == 0) {
        extract_input(h, input[i], i, long_cb);
      }
      srslte_vec_sum_sss(h->syst0, h->w, h->syst0, long_cb*h->max_par_cb);      
    }
    
    // Run MAP DEC #1
    map_sse_inter_dec(h, h->syst0, h->parity0, h->llr1, long_cb);

    // Prepare systematic and parity bits for MAP DEC #1
    sse_inter_extract_syst1(h, inter, long_cb);

    // Run MAP DEC #2
    map_sse_inter_dec(h, h->syst1, h->parity1, h->llr2, long_cb);
    
    // Update a-priori LLR from the last iteration
    sse_inter_update_w(h, deinter, long_cb);
    
  } else {
    fprintf(stderr, "Error CB index not set (call srslte_tdec_simd_inter_reset() first\n");    
  }
}

int srslte_tdec_simd_inter_reset_cb(srslte_tdec_simd_inter_t * h, uint32_t cb_idx)
{
  for (int i=0;i<h->current_long_cb;i++) {
    h->w[h->max_par_cb*i+cb_idx] = 0; 
  }
  return 0;
}

int srslte_tdec_simd_inter_reset(srslte_tdec_simd_inter_t * h, uint32_t long_cb)
{
  if (long_cb > h->max_long_cb) {
    fprintf(stderr, "TDEC was initialized for max_long_cb=%d\n",
            h->max_long_cb);
    return -1;
  }
  h->current_long_cb = long_cb; 
  h->current_cbidx   = srslte_cbsegm_cbindex(long_cb);
  if (h->current_cbidx < 0) {
    fprintf(stderr, "Invalid CB length %d\n", long_cb);
    return -1; 
  }
  memset(h->w, 0, sizeof(int16_t) * long_cb * h->max_par_cb);
  return 0; 
}

void srslte_tdec_simd_inter_decision_cb(srslte_tdec_simd_inter_t * h, uint8_t *output, uint32_t cb_idx, uint32_t long_cb)
{
  uint16_t *deinter = h->interleaver[h->current_cbidx].reverse;
  uint32_t i;
  for (i = 0; i < long_cb; i++) {
    output[i] = (h->llr2[h->max_par_cb*deinter[i]+cb_idx] > 0) ? 1 : 0;    
  }
}

void srslte_tdec_simd_inter_decision(srslte_tdec_simd_inter_t * h, uint8_t *output[SRSLTE_TDEC_MAX_NPAR], uint32_t nof_cb, uint32_t long_cb)
{
  for (int i=0;i<nof_cb;i++) {
    srslte_tdec_simd_inter_decision_cb(h, output[i], i, long_cb);
  }
}

void srslte_tdec_simd_inter_decision_byte_cb(srslte_tdec_simd_inter_t * h, uint8_t *output, uint32_t cb_idx, uint32_t long_cb)
{
  uint32_t i;
  uint8_t mask[8] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};
  uint16_t *deinter = h->interleaver[h->current_cbidx].reverse;
  
#define indexOf_cb(idx, cb) (h->max_par_cb*(deinter[8*i+idx])+cb)
  
  // long_cb is always byte aligned
  for (i = 0; i < long_cb/8; i++) {
    uint8_t out0 = h->llr2[indexOf_cb(0, cb_idx)]>0?mask[0]:0;
    uint8_t out1 = h->llr2[indexOf_cb(1, cb_idx)]>0?mask[1]:0;
    uint8_t out2 = h->llr2[indexOf_cb(2, cb_idx)]>0?mask[2]:0;
    uint8_t out3 = h->llr2[indexOf_cb(3, cb_idx)]>0?mask[3]:0;
    uint8_t out4 = h->llr2[indexOf_cb(4, cb_idx)]>0?mask[4]:0;
    uint8_t out5 = h->llr2[indexOf_cb(5, cb_idx)]>0?mask[5]:0;
    uint8_t out6 = h->llr2[indexOf_cb(6, cb_idx)]>0?mask[6]:0;
    uint8_t out7 = h->llr2[indexOf_cb(7, cb_idx)]>0?mask[7]:0;
    
    output[i] = out0 | out1 | out2 | out3 | out4 | out5 | out6 | out7; 
  }
}

void srslte_tdec_simd_inter_decision_byte(srslte_tdec_simd_inter_t * h, uint8_t *output[SRSLTE_TDEC_MAX_NPAR], uint32_t nof_cb, uint32_t long_cb)
{
  for (int i=0;i<nof_cb;i++) {
    srslte_tdec_simd_inter_decision_byte_cb(h, output[i], i, long_cb);
  }
}

int srslte_tdec_simd_inter_run_all(srslte_tdec_simd_inter_t * h, 
                                  int16_t *input[SRSLTE_TDEC_MAX_NPAR], uint8_t *output[SRSLTE_TDEC_MAX_NPAR],
                                  uint32_t nof_iterations, uint32_t nof_cb, uint32_t long_cb)
{
  uint32_t iter = 0;

  if (srslte_tdec_simd_inter_reset(h, long_cb)) {
    return SRSLTE_ERROR; 
  }

  do {
    srslte_tdec_simd_inter_iteration(h, input, nof_cb, long_cb);
    iter++;
  } while (iter < nof_iterations);

  srslte_tdec_simd_inter_decision_byte(h, output, nof_cb, long_cb);
  
  return SRSLTE_SUCCESS;
}

#endif
