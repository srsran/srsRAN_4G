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

#include "srslte/fec/turbodecoder_sse.h"
#include "srslte/utils/vector.h"

#include <inttypes.h>

#ifdef LV_HAVE_SSE
#include <smmintrin.h>
#endif


#define NUMSTATES       8
#define NINPUTS         2
#define TAIL            3
#define TOTALTAIL       12

#define INF 10000
#define ZERO 0


#ifdef LV_HAVE_SSE

/* Computes the horizontal MAX from 8 16-bit integers using the minpos_epu16 SSE4.1 instruction */
static inline int16_t hMax(__m128i buffer)
{
  __m128i tmp1 = _mm_sub_epi8(_mm_set1_epi16(0x7FFF), buffer);
  __m128i tmp3 = _mm_minpos_epu16(tmp1);
  return (int16_t)(_mm_cvtsi128_si32(tmp3));
}

/* Computes beta values */
void map_gen_beta(map_gen_t * s, int16_t * output, uint32_t long_cb)
{
  int k;
  uint32_t end = long_cb + 3;
  const __m128i *alphaPtr = (const __m128i*) s->alpha;
 
  __m128i beta_k = _mm_set_epi16(-INF, -INF, -INF, -INF, -INF, -INF, -INF, 0);
  __m128i g, bp, bn, alpha_k; 
  
  /* Define the shuffle constant for the positive beta */
  __m128i shuf_bp = _mm_set_epi8(
    15, 14, // 7
    7,  6,  // 3
    5,  4,  // 2
    13, 12, // 6
    11, 10, // 5
    3,  2,  // 1
    1,  0,  // 0
    9,  8   // 4
  );

  /* Define the shuffle constant for the negative beta */
  __m128i shuf_bn = _mm_set_epi8(
    7,   6, // 3
    15, 14, // 7
    13, 12, // 6
    5,  4,  // 2
    3,  2,  // 1
    11, 10, // 5
    9,  8,  // 4
    1,  0   // 0
  );
 
  alphaPtr += long_cb-1;

  /* Define shuffle for branch costs */
  __m128i shuf_g[4];  
  shuf_g[3] = _mm_set_epi8(3,2,1,0,1,0,3,2,3,2,1,0,1,0,3,2);
  shuf_g[2] = _mm_set_epi8(7,6,5,4,5,4,7,6,7,6,5,4,5,4,7,6);
  shuf_g[1] = _mm_set_epi8(11,10,9,8,9,8,11,10,11,10,9,8,9,8,11,10);
  shuf_g[0] = _mm_set_epi8(15,14,13,12,13,12,15,14,15,14,13,12,13,12,15,14);
  __m128i gv;
  int16_t *b = &s->branch[2*long_cb-8];
  __m128i *gPtr = (__m128i*) b;
  /* Define shuffle for beta normalization */
  __m128i shuf_norm = _mm_set_epi8(1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0);
  
  /* This defines a beta computation step: 
   * Adds and substracts the branch metrics to the previous beta step, 
   * shuffles the states according to the trellis path and selects maximum state 
   */
#define BETA_STEP(g)     bp = _mm_add_epi16(beta_k, g);\
    bn = _mm_sub_epi16(beta_k, g);\
    bp = _mm_shuffle_epi8(bp, shuf_bp);\
    bn = _mm_shuffle_epi8(bn, shuf_bn);\
    beta_k = _mm_max_epi16(bp, bn);    

    /* Loads the alpha metrics from memory and adds them to the temporal bn and bp 
     * metrics. Then computes horizontal maximum of both metrics and computes difference
     */
#define BETA_STEP_CNT(c,d) g = _mm_shuffle_epi8(gv, shuf_g[c]);\
    BETA_STEP(g)\
    alpha_k = _mm_load_si128(alphaPtr);\
    alphaPtr--;\
    bp = _mm_add_epi16(bp, alpha_k);\
    bn = _mm_add_epi16(bn, alpha_k); output[k-d] = hMax(bn) - hMax(bp);

  /* The tail does not require to load alpha or produce outputs. Only update 
   * beta metrics accordingly */
  for (k=end-1; k>=long_cb; k--) {
    int16_t g0 = s->branch[2*k];
    int16_t g1 = s->branch[2*k+1];
    g = _mm_set_epi16(g1, g0, g0, g1, g1, g0, g0, g1);
  
    BETA_STEP(g);
  }  
  
  /* We inline 2 trelis steps for each normalization */
  __m128i norm;
  for (; k >= 0; k-=8) {    
    gv = _mm_load_si128(gPtr);
    gPtr--;
    BETA_STEP_CNT(0,0);
    BETA_STEP_CNT(1,1);
    BETA_STEP_CNT(2,2);
    BETA_STEP_CNT(3,3);
    norm = _mm_shuffle_epi8(beta_k, shuf_norm); 
    beta_k = _mm_sub_epi16(beta_k, norm);
    gv = _mm_load_si128(gPtr);
    gPtr--;
    BETA_STEP_CNT(0,4);
    BETA_STEP_CNT(1,5);
    BETA_STEP_CNT(2,6);
    BETA_STEP_CNT(3,7);
    norm = _mm_shuffle_epi8(beta_k, shuf_norm); 
    beta_k = _mm_sub_epi16(beta_k, norm);
  }  
}

/* Computes alpha metrics */
void map_gen_alpha(map_gen_t * s, uint32_t long_cb)
{
  uint32_t k;
  int16_t *alpha = s->alpha;
  uint32_t i;

  alpha[0] = 0; 
  for (i = 1; i < 8; i++) {
    alpha[i] = -INF;
  }
  
  /* Define the shuffle constant for the positive alpha */
  __m128i shuf_ap = _mm_set_epi8(
    15, 14, // 7
    9,  8,  // 4
    7,  6,  // 3
    1,  0,  // 0
    13, 12, // 6
    11, 10, // 5
    5,  4,  // 2
    3,  2   // 1
  );

  /* Define the shuffle constant for the negative alpha */
  __m128i shuf_an = _mm_set_epi8(
    13, 12, // 6
    11, 10, // 5
    5,  4,  // 2
    3,  2,  // 1
    15, 14, // 7
    9,  8,  // 4
    7,  6,  // 3
    1,  0   // 0
  );
  
  /* Define shuffle for branch costs */
  __m128i shuf_g[4];
  shuf_g[0] = _mm_set_epi8(3,2,3,2,1,0,1,0,1,0,1,0,3,2,3,2);
  shuf_g[1] = _mm_set_epi8(7,6,7,6,5,4,5,4,5,4,5,4,7,6,7,6);
  shuf_g[2] = _mm_set_epi8(11,10,11,10,9,8,9,8,9,8,9,8,11,10,11,10);
  shuf_g[3] = _mm_set_epi8(15,14,15,14,13,12,13,12,13,12,13,12,15,14,15,14);

  __m128i shuf_norm = _mm_set_epi8(1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0);
  
  __m128i* alphaPtr = (__m128i*) alpha;
  alphaPtr++;

  __m128i gv; 
  __m128i *gPtr = (__m128i*) s->branch;
  __m128i g, ap, an; 
    
  __m128i alpha_k = _mm_set_epi16(-INF, -INF, -INF, -INF, -INF, -INF, -INF, 0);
  
  /* This defines a alpha computation step: 
   * Adds and substracts the branch metrics to the previous alpha step, 
   * shuffles the states according to the trellis path and selects maximum state 
   */
#define ALPHA_STEP(c)  g = _mm_shuffle_epi8(gv, shuf_g[c]); \
  ap = _mm_add_epi16(alpha_k, g);\
  an = _mm_sub_epi16(alpha_k, g);\
  ap = _mm_shuffle_epi8(ap, shuf_ap);\
  an = _mm_shuffle_epi8(an, shuf_an);\
  alpha_k = _mm_max_epi16(ap, an);\
  _mm_store_si128(alphaPtr, alpha_k);\
  alphaPtr++;    \

  /* In this loop, we compute 8 steps and normalize twice for each branch metrics memory load */
  __m128i norm;
  for (k = 0; k < long_cb/8; k++) {
    gv = _mm_load_si128(gPtr);
    gPtr++;
    ALPHA_STEP(0);
    ALPHA_STEP(1);
    ALPHA_STEP(2);
    ALPHA_STEP(3);
    norm = _mm_shuffle_epi8(alpha_k, shuf_norm); 
    alpha_k = _mm_sub_epi16(alpha_k, norm);
    gv = _mm_load_si128(gPtr);
    gPtr++;
    ALPHA_STEP(0);
    ALPHA_STEP(1);
    ALPHA_STEP(2);
    ALPHA_STEP(3);
    norm = _mm_shuffle_epi8(alpha_k, shuf_norm); 
    alpha_k = _mm_sub_epi16(alpha_k, norm);
  }  
}

/* Compute branch metrics (gamma) */
void map_gen_gamma(map_gen_t * h, int16_t *input, int16_t *app, int16_t *parity, uint32_t long_cb) 
{
  __m128i res10, res20, res11, res21, res1, res2; 
  __m128i in, ap, pa, g1, g0;

  __m128i *inPtr  = (__m128i*) input;
  __m128i *appPtr = (__m128i*) app;
  __m128i *paPtr  = (__m128i*) parity;
  __m128i *resPtr = (__m128i*) h->branch;
  
  __m128i res10_mask = _mm_set_epi8(0xff,0xff,7,6,0xff,0xff,5,4,0xff,0xff,3,2,0xff,0xff,1,0);
  __m128i res20_mask = _mm_set_epi8(0xff,0xff,15,14,0xff,0xff,13,12,0xff,0xff,11,10,0xff,0xff,9,8);
  __m128i res11_mask = _mm_set_epi8(7,6,0xff,0xff,5,4,0xff,0xff,3,2,0xff,0xff,1,0,0xff,0xff);
  __m128i res21_mask = _mm_set_epi8(15,14,0xff,0xff,13,12,0xff,0xff,11,10,0xff,0xff,9,8,0xff,0xff);
  
  for (int i=0;i<long_cb/8;i++) {
    in = _mm_load_si128(inPtr);
    inPtr++;
    pa = _mm_load_si128(paPtr);
    paPtr++;
    
    if (appPtr) {
      ap = _mm_load_si128(appPtr);
      appPtr++;
      in = _mm_add_epi16(ap, in);
    }
    
    g1 = _mm_add_epi16(in, pa);
    g0 = _mm_sub_epi16(in, pa);

    g1 = _mm_srai_epi16(g1, 1);
    g0 = _mm_srai_epi16(g0, 1);
    
    res10 = _mm_shuffle_epi8(g0, res10_mask);
    res20 = _mm_shuffle_epi8(g0, res20_mask);
    res11 = _mm_shuffle_epi8(g1, res11_mask);
    res21 = _mm_shuffle_epi8(g1, res21_mask);

    res1  = _mm_or_si128(res10, res11);
    res2  = _mm_or_si128(res20, res21);

    _mm_store_si128(resPtr, res1);
    resPtr++;
    _mm_store_si128(resPtr, res2);    
    resPtr++;
  }

  for (int i=long_cb;i<long_cb+3;i++) {
    h->branch[2*i]   = (input[i] - parity[i])/2;
    h->branch[2*i+1] = (input[i] + parity[i])/2;
  }
}

/* Inititalizes constituent decoder object */
int map_gen_init(map_gen_t * h, int max_long_cb)
{
  bzero(h, sizeof(map_gen_t));
  h->alpha = srslte_vec_malloc(sizeof(int16_t) * (max_long_cb + SRSLTE_TCOD_TOTALTAIL + 1) * NUMSTATES);
  if (!h->alpha) {
    perror("srslte_vec_malloc");
    return -1;
  }
  h->branch = srslte_vec_malloc(sizeof(int16_t) * (max_long_cb + SRSLTE_TCOD_TOTALTAIL + 1) * NUMSTATES);
  if (!h->branch) {
    perror("srslte_vec_malloc");
    return -1;
  }
  h->max_long_cb = max_long_cb;
  return 0;
}

void map_gen_free(map_gen_t * h)
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
void map_gen_dec(map_gen_t * h, int16_t * input, int16_t *app, int16_t * parity, int16_t * output,
                 uint32_t long_cb)
{
 
  // Compute branch metrics
  map_gen_gamma(h, input, app, parity, long_cb);

  // Forward recursion
  map_gen_alpha(h, long_cb);

  // Backwards recursion + LLR computation
  map_gen_beta(h, output, long_cb);
  
}

/* Initializes the turbo decoder object */
int srslte_tdec_sse_init(srslte_tdec_sse_t * h, uint32_t max_long_cb)
{
  int ret = -1;
  bzero(h, sizeof(srslte_tdec_sse_t));
  uint32_t len = max_long_cb + SRSLTE_TCOD_TOTALTAIL;

  h->max_long_cb = max_long_cb;

  h->app1 = srslte_vec_malloc(sizeof(int16_t) * len);
  if (!h->app1) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->app2 = srslte_vec_malloc(sizeof(int16_t) * len);
  if (!h->app2) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->ext1 = srslte_vec_malloc(sizeof(int16_t) * len);
  if (!h->ext1) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->ext2 = srslte_vec_malloc(sizeof(int16_t) * len);
  if (!h->ext2) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->syst = srslte_vec_malloc(sizeof(int16_t) * len);
  if (!h->syst) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->parity0 = srslte_vec_malloc(sizeof(int16_t) * len);
  if (!h->parity0) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }
  h->parity1 = srslte_vec_malloc(sizeof(int16_t) * len);
  if (!h->parity1) {
    perror("srslte_vec_malloc");
    goto clean_and_exit;
  }

  if (map_gen_init(&h->dec, h->max_long_cb)) {
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
    srslte_tdec_sse_free(h);
  }
  return ret;
}

void srslte_tdec_sse_free(srslte_tdec_sse_t * h)
{
  if (h->app1) {
    free(h->app1);
  }
  if (h->app2) {
    free(h->app2);
  }
  if (h->ext1) {
    free(h->ext1);
  }
  if (h->ext2) {
    free(h->ext2);
  }
  if (h->syst) {
    free(h->syst);
  }
  if (h->parity0) {
    free(h->parity0);
  }
  if (h->parity1) {
    free(h->parity1);
  }

  map_gen_free(&h->dec);

  for (int i=0;i<SRSLTE_NOF_TC_CB_SIZES;i++) {
    srslte_tc_interl_free(&h->interleaver[i]);    
  }

  bzero(h, sizeof(srslte_tdec_sse_t));
}

/* Deinterleaves the 3 streams from the input (systematic and 2 parity bits) into 
 * 3 buffers ready to be used by compute_gamma() 
 */
void deinterleave_input(srslte_tdec_sse_t *h, int16_t *input, uint32_t long_cb) {
  uint32_t i;
 
  __m128i *inputPtr = (__m128i*) input; 
  __m128i in0, in1, in2;
  __m128i s0, s1, s2, s;
  __m128i p00, p01, p02, p0;
  __m128i p10, p11, p12, p1;
  
  __m128i *sysPtr = (__m128i*) h->syst; 
  __m128i *pa0Ptr = (__m128i*) h->parity0; 
  __m128i *pa1Ptr = (__m128i*) h->parity1; 
  
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
    h->syst[i+long_cb]    = input[3*long_cb + 2*i];
    h->parity0[i+long_cb] = input[3*long_cb + 2*i + 1];
  }
  for (i = 0; i < 3; i++) {
    h->app2[i+long_cb]    = input[3*long_cb + 6 + 2*i];
    h->parity1[i+long_cb] = input[3*long_cb + 6 + 2*i + 1];
  }

}

/* Runs 1 turbo decoder iteration */
void srslte_tdec_sse_iteration(srslte_tdec_sse_t * h, int16_t * input, uint32_t long_cb)
{

  if (h->current_cbidx >= 0) {
    uint16_t *inter   = h->interleaver[h->current_cbidx].forward;
    uint16_t *deinter = h->interleaver[h->current_cbidx].reverse;
    
    if (h->n_iter == 0) {
      deinterleave_input(h, input, long_cb);
    }
    
    // Add apriori information to decoder 1 
    if (h->n_iter > 0) {
      srslte_vec_sub_sss(h->app1, h->ext1, h->app1, long_cb);
    }
        
    // Run MAP DEC #1
    if (h->n_iter == 0) {
      map_gen_dec(&h->dec, h->syst, NULL, h->parity0, h->ext1, long_cb);            
    } else {
      map_gen_dec(&h->dec, h->syst, h->app1, h->parity0, h->ext1, long_cb);      
    }

    // Convert aposteriori information into extrinsic information    
    if (h->n_iter > 0) {
      srslte_vec_sub_sss(h->ext1, h->app1, h->ext1, long_cb);
    }
    
    // Interleave extrinsic output of DEC1 to form apriori info for decoder 2
    srslte_vec_lut_sss(h->ext1, deinter, h->app2, long_cb);

    // Run MAP DEC #2. 2nd decoder uses apriori information as systematic bits
    map_gen_dec(&h->dec, h->app2, NULL, h->parity1, h->ext2, long_cb);

    // Deinterleaved extrinsic bits become apriori info for decoder 1 
    srslte_vec_lut_sss(h->ext2, inter, h->app1, long_cb);
    
    h->n_iter++;
  } else {
    fprintf(stderr, "Error CB index not set (call srslte_tdec_sse_reset() first\n");    
  }
}

/* Resets the decoder and sets the codeblock length */
int srslte_tdec_sse_reset(srslte_tdec_sse_t * h, uint32_t long_cb)
{
  if (long_cb > h->max_long_cb) {
    fprintf(stderr, "TDEC was initialized for max_long_cb=%d\n",
            h->max_long_cb);
    return -1;
  }
  h->n_iter = 0; 
  h->current_cbidx = srslte_cbsegm_cbindex(long_cb);
  if (h->current_cbidx < 0) {
    fprintf(stderr, "Invalid CB length %d\n", long_cb);
    return -1; 
  }
  return 0;
}

void srslte_tdec_sse_decision(srslte_tdec_sse_t * h, uint8_t *output, uint32_t long_cb)
{
  __m128i zero     = _mm_set1_epi16(0);
  __m128i lsb_mask = _mm_set1_epi16(1);
  
  __m128i *appPtr = (__m128i*) h->app1;
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
      output[long_cb-8+i] = h->app1[long_cb-8+i]>0?1:0;
    }
  }
}

void srslte_tdec_sse_decision_byte(srslte_tdec_sse_t * h, uint8_t *output, uint32_t long_cb)
{
  uint8_t mask[8] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};
  
  // long_cb is always byte aligned
  for (uint32_t i = 0; i < long_cb/8; i++) {
    uint8_t out0 = h->app1[8*i+0]>0?mask[0]:0;
    uint8_t out1 = h->app1[8*i+1]>0?mask[1]:0;
    uint8_t out2 = h->app1[8*i+2]>0?mask[2]:0;
    uint8_t out3 = h->app1[8*i+3]>0?mask[3]:0;
    uint8_t out4 = h->app1[8*i+4]>0?mask[4]:0;
    uint8_t out5 = h->app1[8*i+5]>0?mask[5]:0;
    uint8_t out6 = h->app1[8*i+6]>0?mask[6]:0;
    uint8_t out7 = h->app1[8*i+7]>0?mask[7]:0;
    
    output[i] = out0 | out1 | out2 | out3 | out4 | out5 | out6 | out7; 
  }
}

/* Runs nof_iterations iterations and decides the output bits */
int srslte_tdec_sse_run_all(srslte_tdec_sse_t * h, int16_t * input, uint8_t *output,
                  uint32_t nof_iterations, uint32_t long_cb)
{
  if (srslte_tdec_sse_reset(h, long_cb)) {
    return SRSLTE_ERROR; 
  }

  do {
    srslte_tdec_sse_iteration(h, input, long_cb);
  } while (h->n_iter < nof_iterations);

  srslte_tdec_sse_decision_byte(h, output, long_cb);
  
  return SRSLTE_SUCCESS;
}

#endif


