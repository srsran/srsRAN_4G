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

#include "srslte/phy/fec/turbodecoder_simd.h"
#include "srslte/phy/utils/vector.h"

#include <inttypes.h>

#define NUMSTATES       8
#define NINPUTS         2
#define TAIL            3
#define TOTALTAIL       12

#define INF 10000
#define ZERO 0


#ifdef LV_HAVE_SSE
#include <smmintrin.h>

// Define SSE/AVX implementations 
void map_sse_beta(map_gen_t * s, int16_t * output, uint32_t long_cb);
void map_sse_alpha(map_gen_t * s, uint32_t long_cb);
void map_sse_gamma(map_gen_t * h, int16_t *input, int16_t *app, int16_t *parity, uint32_t long_cb);

#ifdef LV_HAVE_AVX2
void map_avx_beta(map_gen_t * s, int16_t * output[SRSLTE_TDEC_MAX_NPAR], uint32_t long_cb);
void map_avx_alpha(map_gen_t * s, uint32_t long_cb);
void map_avx_gamma(map_gen_t * h, int16_t *input, int16_t *app, int16_t *parity, uint32_t cbidx, uint32_t long_cb);
#endif


void map_simd_beta(map_gen_t * s, int16_t * output[SRSLTE_TDEC_MAX_NPAR], uint32_t nof_cb, uint32_t long_cb)
{
  if (nof_cb == 1) {
    map_sse_beta(s, output[0], long_cb);
  } 
#ifdef LV_HAVE_AVX2
  else if (nof_cb == 2) {
    map_avx_beta(s, output, long_cb);
  }
#endif
}

void map_simd_alpha(map_gen_t * s, uint32_t nof_cb, uint32_t long_cb)
{
  if (nof_cb == 1) {
    map_sse_alpha(s, long_cb);
  }
#ifdef LV_HAVE_AVX2
  else if (nof_cb == 2) {
    map_avx_alpha(s, long_cb);
  }
#endif
}
void map_simd_gamma(map_gen_t * s, int16_t *input, int16_t *app, int16_t *parity, uint32_t cbidx, uint32_t nof_cb, uint32_t long_cb) 
{
  if (nof_cb == 1) {
    map_sse_gamma(s, input, app, parity, long_cb);
  } 
#ifdef LV_HAVE_AVX2
  else if (nof_cb == 2) {
    map_avx_gamma(s, input, app, parity, cbidx, long_cb);
  }    
#endif
}

/* Inititalizes constituent decoder object */
int map_simd_init(map_gen_t * h, uint32_t max_par_cb, uint32_t max_long_cb)
{
  bzero(h, sizeof(map_gen_t));

  h->max_par_cb  = max_par_cb;
  h->max_long_cb = max_long_cb;

  h->alpha = srslte_vec_malloc(sizeof(int16_t) * (max_long_cb + SRSLTE_TCOD_TOTALTAIL + 1) * NUMSTATES * h->max_par_cb);
  if (!h->alpha) {
    perror("srslte_vec_malloc");
    return -1;
  }
  h->branch = srslte_vec_malloc(sizeof(int16_t) * (max_long_cb + SRSLTE_TCOD_TOTALTAIL + 1) * NUMSTATES * h->max_par_cb);
  if (!h->branch) {
    perror("srslte_vec_malloc");
    return -1;
  }
  return 0;
}

void map_simd_free(map_gen_t * h)
{
  if (h->alpha) {
    free(h->alpha);
  }
  if (h->branch) {
    free(h->branch);
  }
  bzero(h, sizeof(map_gen_t));
}

/* Runs one instance of a decoder */
void map_simd_dec(map_gen_t * h, int16_t * input[SRSLTE_TDEC_MAX_NPAR], int16_t *app[SRSLTE_TDEC_MAX_NPAR], int16_t * parity[SRSLTE_TDEC_MAX_NPAR],
                  int16_t *output[SRSLTE_TDEC_MAX_NPAR], uint32_t cb_mask, uint32_t long_cb)
{
  
  uint32_t nof_cb = 1;
  int16_t *outptr[SRSLTE_TDEC_MAX_NPAR] = { NULL, NULL };
  
  // Compute branch metrics
  switch(cb_mask) {
    case 1:
      nof_cb = 1; 
      outptr[0] = output[0];
      map_simd_gamma(h, input[0], app?app[0]:NULL, parity[0], 0, 1, long_cb);    
      break;
    case 2:
      nof_cb = 1; 
      outptr[0] = output[1];
      map_simd_gamma(h, input[1], app?app[1]:NULL, parity[1], 0, 1, long_cb);    
      break;
    case 3:
      nof_cb = 2; 
      for (int i=0;i<2;i++) {    
        outptr[i] = output[i];
        map_simd_gamma(h, input[i], app?app[i]:NULL, parity[i], i, 2, long_cb);    
      }
      break;
  }

  // Forward recursion
  map_simd_alpha(h, nof_cb, long_cb);

  // Backwards recursion + LLR computation
  map_simd_beta(h, outptr, nof_cb, long_cb);
}

/* Initializes the turbo decoder object */
int srslte_tdec_simd_init(srslte_tdec_simd_t * h, uint32_t max_par_cb, uint32_t max_long_cb)
{
  int ret = -1;
  bzero(h, sizeof(srslte_tdec_simd_t));
  uint32_t len = max_long_cb + SRSLTE_TCOD_TOTALTAIL;

  h->max_long_cb = max_long_cb;
  h->max_par_cb  = max_par_cb; 
  
  for (int i=0;i<h->max_par_cb;i++) {
    h->app1[i] = srslte_vec_malloc(sizeof(int16_t) * len);
    if (!h->app1[i]) {
      perror("srslte_vec_malloc");
      goto clean_and_exit;
    }
    h->app2[i] = srslte_vec_malloc(sizeof(int16_t) * len);
    if (!h->app2[i]) {
      perror("srslte_vec_malloc");
      goto clean_and_exit;
    }
    h->ext1[i] = srslte_vec_malloc(sizeof(int16_t) * len);
    if (!h->ext1[i]) {
      perror("srslte_vec_malloc");
      goto clean_and_exit;
    }
    h->ext2[i] = srslte_vec_malloc(sizeof(int16_t) * len);
    if (!h->ext2[i]) {
      perror("srslte_vec_malloc");
      goto clean_and_exit;
    }
    h->syst[i] = srslte_vec_malloc(sizeof(int16_t) * len);
    if (!h->syst[i]) {
      perror("srslte_vec_malloc");
      goto clean_and_exit;
    }
    h->parity0[i] = srslte_vec_malloc(sizeof(int16_t) * len);
    if (!h->parity0[i]) {
      perror("srslte_vec_malloc");
      goto clean_and_exit;
    }
    h->parity1[i] = srslte_vec_malloc(sizeof(int16_t) * len);
    if (!h->parity1[i]) {
      perror("srslte_vec_malloc");
      goto clean_and_exit;
    }
    
  }

  if (map_simd_init(&h->dec, h->max_par_cb, h->max_long_cb)) {
    goto clean_and_exit;
  }

  for (int i=0;i<SRSLTE_NOF_TC_CB_SIZES;i++) {
    if (srslte_tc_interl_init(&h->interleaver[i], srslte_cbsegm_cbsize(i)) < 0) {
      goto clean_and_exit;
    }
    srslte_tc_interl_LTE_gen(&h->interleaver[i], srslte_cbsegm_cbsize(i));
  }
  h->current_cbidx = -1; 
  h->cb_mask = 0; 
  ret = 0;
clean_and_exit:if (ret == -1) {
    srslte_tdec_simd_free(h);
  }
  return ret;
}

void srslte_tdec_simd_free(srslte_tdec_simd_t * h)
{
  for (int i=0;i<h->max_par_cb;i++) {
    if (h->app1[i]) {
      free(h->app1[i]);
    }
    if (h->app2[i]) {
      free(h->app2[i]);
    }
    if (h->ext1[i]) {
      free(h->ext1[i]);
    }
    if (h->ext2[i]) {
      free(h->ext2[i]);
    }
    if (h->syst[i]) {
      free(h->syst[i]);
    }
    if (h->parity0[i]) {
      free(h->parity0[i]);
    }
    if (h->parity1[i]) {
      free(h->parity1[i]);
    }    
  }

  map_simd_free(&h->dec);

  for (int i=0;i<SRSLTE_NOF_TC_CB_SIZES;i++) {
    srslte_tc_interl_free(&h->interleaver[i]);    
  }

  bzero(h, sizeof(srslte_tdec_simd_t));
}

/* Deinterleaves the 3 streams from the input (systematic and 2 parity bits) into 
 * 3 buffers ready to be used by compute_gamma() 
 */
void deinterleave_input_simd(srslte_tdec_simd_t *h, int16_t *input, uint32_t cbidx, uint32_t long_cb) {
  uint32_t i;
 
  __m128i *inputPtr = (__m128i*) input; 
  __m128i in0, in1, in2;
  __m128i s0, s1, s2, s;
  __m128i p00, p01, p02, p0;
  __m128i p10, p11, p12, p1;
  
  __m128i *sysPtr = (__m128i*) h->syst[cbidx]; 
  __m128i *pa0Ptr = (__m128i*) h->parity0[cbidx]; 
  __m128i *pa1Ptr = (__m128i*) h->parity1[cbidx]; 
  
  // pick bits 0, 3, 6 from 1st word
  __m128i s0_mask = _mm_set_epi8(0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,13,12,7,6,1,0);
  // pick bits 1, 4, 7 from 2st word
  __m128i s1_mask = _mm_set_epi8(0xff,0xff,0xff,0xff,15,14,9,8,3,2,0xff,0xff,0xff,0xff,0xff,0xff);
  // pick bits 2, 5 from 3rd word
  __m128i s2_mask = _mm_set_epi8(11,10,5,4,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff);

  // pick bits 1, 4, 7 from 1st word
  __m128i p00_mask = _mm_set_epi8(0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,15,14,9,8,3,2);
  // pick bits 2, 5, from 2st word
  __m128i p01_mask = _mm_set_epi8(0xff,0xff,0xff,0xff,0xff,0xff,11,10,5,4,0xff,0xff,0xff,0xff,0xff,0xff);
  // pick bits 0, 3, 6 from 3rd word
  __m128i p02_mask = _mm_set_epi8(13,12,7,6,1,0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff);
  
  // pick bits 2, 5 from 1st word
  __m128i p10_mask = _mm_set_epi8(0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,11,10,5,4);
  // pick bits 0, 3, 6, from 2st word
  __m128i p11_mask = _mm_set_epi8(0xff,0xff,0xff,0xff,0xff,0xff,13,12,7,6,1,0,0xff,0xff,0xff,0xff);
  // pick bits 1, 4, 7 from 3rd word
  __m128i p12_mask = _mm_set_epi8(15,14,9,8,3,2,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff);
      
  // Split systematic and parity bits
  for (i = 0; i < long_cb/8; i++) {
        
    in0 = _mm_load_si128(inputPtr); inputPtr++; 
    in1 = _mm_load_si128(inputPtr); inputPtr++;   
    in2 = _mm_load_si128(inputPtr); inputPtr++;
    
    /* Deinterleave Systematic bits */
    s0 = _mm_shuffle_epi8(in0, s0_mask);
    s1 = _mm_shuffle_epi8(in1, s1_mask);
    s2 = _mm_shuffle_epi8(in2, s2_mask);    
    s = _mm_or_si128(s0, s1);
    s = _mm_or_si128(s, s2);

    _mm_store_si128(sysPtr, s);
    sysPtr++;

    /* Deinterleave parity 0 bits */
    p00 = _mm_shuffle_epi8(in0, p00_mask);
    p01 = _mm_shuffle_epi8(in1, p01_mask);
    p02 = _mm_shuffle_epi8(in2, p02_mask);    
    p0 = _mm_or_si128(p00, p01);
    p0 = _mm_or_si128(p0, p02);
    
    _mm_store_si128(pa0Ptr, p0);
    pa0Ptr++;

    /* Deinterleave parity 1 bits */
    p10 = _mm_shuffle_epi8(in0, p10_mask);
    p11 = _mm_shuffle_epi8(in1, p11_mask);
    p12 = _mm_shuffle_epi8(in2, p12_mask);    
    p1 = _mm_or_si128(p10, p11);
    p1 = _mm_or_si128(p1, p12);

    _mm_store_si128(pa1Ptr, p1);
    pa1Ptr++;    

  }
  
  for (i = 0; i < 3; i++) {
    h->syst[cbidx][i+long_cb]    = input[3*long_cb + 2*i];
    h->parity0[cbidx][i+long_cb] = input[3*long_cb + 2*i + 1];
  }
  for (i = 0; i < 3; i++) {
    h->app2[cbidx][i+long_cb]    = input[3*long_cb + 6 + 2*i];
    h->parity1[cbidx][i+long_cb] = input[3*long_cb + 6 + 2*i + 1];
  }

}

/* Runs 1 turbo decoder iteration */
void srslte_tdec_simd_iteration(srslte_tdec_simd_t * h, int16_t * input[SRSLTE_TDEC_MAX_NPAR], uint32_t long_cb)
{

  int16_t *tmp_app[SRSLTE_TDEC_MAX_NPAR];

  if (h->current_cbidx >= 0) {
    uint16_t *inter   = h->interleaver[h->current_cbidx].forward;
    uint16_t *deinter = h->interleaver[h->current_cbidx].reverse;
    
#ifndef LV_HAVE_AVX2
    input[1] = NULL;
#endif

    h->cb_mask = (input[0]?1:0) | (input[1]?2:0);

    for (int i=0;i<h->max_par_cb;i++) {
      if (h->n_iter[i] == 0 && input[i]) {
        //printf("deinterleaveing %d\n",i);
        deinterleave_input_simd(h, input[i], i, long_cb);        
      }        
    }
    
    // Add apriori information to decoder 1 
    for (int i=0;i<h->max_par_cb;i++) {
      if (h->n_iter[i] > 0 && input[i]) {
        srslte_vec_sub_sss(h->app1[i], h->ext1[i], h->app1[i], long_cb);
      }
    }

    // Run MAP DEC #1
    for (int i=0;i<h->max_par_cb;i++) {
      if (input[i]) {
        tmp_app[i] = h->n_iter[i]?h->app1[i]:NULL;
      } else {
        tmp_app[i] = NULL; 
      }
    }
    map_simd_dec(&h->dec, h->syst, tmp_app, h->parity0, h->ext1, h->cb_mask, long_cb);            
    
    // Convert aposteriori information into extrinsic information    
    for (int i=0;i<h->max_par_cb;i++) {
      if (h->n_iter[i] > 0 && input[i]) {
        srslte_vec_sub_sss(h->ext1[i], h->app1[i], h->ext1[i], long_cb);
      }
    }
    
    // Interleave extrinsic output of DEC1 to form apriori info for decoder 2
    for (int i=0;i<h->max_par_cb;i++) {
      if (input[i]) {
        srslte_vec_lut_sss(h->ext1[i], deinter, h->app2[i], long_cb);
      }
    }

    // Run MAP DEC #2. 2nd decoder uses apriori information as systematic bits
    map_simd_dec(&h->dec, h->app2, NULL, h->parity1, h->ext2, h->cb_mask, long_cb);

    // Deinterleaved extrinsic bits become apriori info for decoder 1 
    for (int i=0;i<h->max_par_cb;i++) {
      if (input[i]) {
        srslte_vec_lut_sss(h->ext2[i], inter, h->app1[i], long_cb);
      }
    }

    for (int i=0;i<h->max_par_cb;i++) {
      if (input[i]) {
        h->n_iter[i]++;     
      }
    }
  } else {
    fprintf(stderr, "Error CB index not set (call srslte_tdec_simd_reset() first\n");    
  }
}

/* Resets the decoder and sets the codeblock length */
int srslte_tdec_simd_reset(srslte_tdec_simd_t * h, uint32_t long_cb)
{
  if (long_cb > h->max_long_cb) {
    fprintf(stderr, "TDEC was initialized for max_long_cb=%d\n",
            h->max_long_cb);
    return -1;
  }
  for (int i=0;i<h->max_par_cb;i++) {
    h->n_iter[i] = 0;     
  }
  h->cb_mask = 0; 
  h->current_cbidx = srslte_cbsegm_cbindex(long_cb);
  if (h->current_cbidx < 0) {
    fprintf(stderr, "Invalid CB length %d\n", long_cb);
    return -1; 
  }
  return 0;
}

int srslte_tdec_simd_reset_cb(srslte_tdec_simd_t * h, uint32_t cb_idx)
{
  h->n_iter[cb_idx] = 0; 
  return 0;
}

int srslte_tdec_simd_get_nof_iterations_cb(srslte_tdec_simd_t * h, uint32_t cb_idx)
{
  return h->n_iter[cb_idx];
}

void tdec_simd_decision(srslte_tdec_simd_t * h, uint8_t *output, uint32_t cbidx, uint32_t long_cb)
{
  __m128i zero     = _mm_set1_epi16(0);
  __m128i lsb_mask = _mm_set1_epi16(1);
  
  __m128i *appPtr = (__m128i*) h->app1[cbidx];
  __m128i *outPtr = (__m128i*) output;
  __m128i ap, out, out0, out1; 
    
  for (uint32_t i = 0; i < long_cb/16; i++) {
    ap   = _mm_load_si128(appPtr); appPtr++;    
    out0 = _mm_and_si128(_mm_cmpgt_epi16(ap, zero), lsb_mask);
    ap   = _mm_load_si128(appPtr); appPtr++;
    out1 = _mm_and_si128(_mm_cmpgt_epi16(ap, zero), lsb_mask);
    
    out  = _mm_packs_epi16(out0, out1);
    _mm_store_si128(outPtr, out);
    outPtr++;
  }
  if (long_cb%16) {
    for (int i=0;i<8;i++) {
      output[long_cb-8+i] = h->app1[cbidx][long_cb-8+i]>0?1:0;
    }
  }
}

void srslte_tdec_simd_decision(srslte_tdec_simd_t * h, uint8_t *output[SRSLTE_TDEC_MAX_NPAR], uint32_t long_cb)
{
  for (int i=0;i<h->max_par_cb;i++) {    
    tdec_simd_decision(h, output[i], i, long_cb);
  }
}

void srslte_tdec_simd_decision_byte_cb(srslte_tdec_simd_t * h, uint8_t *output, uint32_t cbidx, uint32_t long_cb)
{
  uint8_t mask[8] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};
  
  // long_cb is always byte aligned
  for (uint32_t i = 0; i < long_cb/8; i++) {
    uint8_t out0 = h->app1[cbidx][8*i+0]>0?mask[0]:0;
    uint8_t out1 = h->app1[cbidx][8*i+1]>0?mask[1]:0;
    uint8_t out2 = h->app1[cbidx][8*i+2]>0?mask[2]:0;
    uint8_t out3 = h->app1[cbidx][8*i+3]>0?mask[3]:0;
    uint8_t out4 = h->app1[cbidx][8*i+4]>0?mask[4]:0;
    uint8_t out5 = h->app1[cbidx][8*i+5]>0?mask[5]:0;
    uint8_t out6 = h->app1[cbidx][8*i+6]>0?mask[6]:0;
    uint8_t out7 = h->app1[cbidx][8*i+7]>0?mask[7]:0;
    
    output[i] = out0 | out1 | out2 | out3 | out4 | out5 | out6 | out7;
  }
}

void srslte_tdec_simd_decision_byte(srslte_tdec_simd_t * h, uint8_t *output[SRSLTE_TDEC_MAX_NPAR], uint32_t long_cb)
{
  for (int i=0;i<h->max_par_cb;i++) {
    if (output[i]) {
      srslte_tdec_simd_decision_byte_cb(h, output[i], i, long_cb);
    }
  }
}


/* Runs nof_iterations iterations and decides the output bits */
int srslte_tdec_simd_run_all(srslte_tdec_simd_t * h, int16_t * input[SRSLTE_TDEC_MAX_NPAR], uint8_t *output[SRSLTE_TDEC_MAX_NPAR],
                            uint32_t nof_iterations, uint32_t long_cb)
{
  if (srslte_tdec_simd_reset(h, long_cb)) {
    return SRSLTE_ERROR; 
  }

  do {
    srslte_tdec_simd_iteration(h, input, long_cb);
  } while (h->n_iter[0] < nof_iterations);

  srslte_tdec_simd_decision_byte(h, output, long_cb);
  
  return SRSLTE_SUCCESS;
}

#endif


