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

/*
static void print_128i(__m128i x) {
  int16_t *s = (int16_t*) &x; 
  printf("[%d", s[0]);
  for (int i=1;i<8;i++) {
    printf(",%d", s[i]);
  }
  printf("]\n");
}
*/
//#define use_beta_transposed_max

#ifndef use_beta_transposed_max

/* Computes the horizontal MAX from 8 16-bit integers using the minpos_epu16 SSE4.1 instruction */
static inline int16_t hMax(__m128i buffer)
{
  __m128i tmp1 = _mm_sub_epi16(_mm_set1_epi16(0x7FFF), buffer);
  __m128i tmp3 = _mm_minpos_epu16(tmp1);
  return (int16_t)(_mm_cvtsi128_si32(tmp3));
}

/* Computes beta values */
void map_sse_beta(map_gen_t * s, int16_t * output, uint32_t long_cb)
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
    bn = _mm_add_epi16(bn, alpha_k);\
    output[k-d] = hMax(bn)-hMax(bp);
    
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

#endif

/* Computes alpha metrics */
void map_sse_alpha(map_gen_t * s, uint32_t long_cb)
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
void map_sse_gamma(map_gen_t * h, int16_t *input, int16_t *app, int16_t *parity, uint32_t long_cb) 
{
  __m128i res00, res10, res01, res11, res0, res1; 
  __m128i in, ap, pa, g1, g0;

  __m128i *inPtr  = (__m128i*) input;
  __m128i *appPtr = (__m128i*) app;
  __m128i *paPtr  = (__m128i*) parity;
  __m128i *resPtr = (__m128i*) h->branch;
  
  __m128i res00_mask = _mm_set_epi8(0xff,0xff,7,6,0xff,0xff,5,4,0xff,0xff,3,2,0xff,0xff,1,0);
  __m128i res10_mask = _mm_set_epi8(0xff,0xff,15,14,0xff,0xff,13,12,0xff,0xff,11,10,0xff,0xff,9,8);
  __m128i res01_mask = _mm_set_epi8(7,6,0xff,0xff,5,4,0xff,0xff,3,2,0xff,0xff,1,0,0xff,0xff);
  __m128i res11_mask = _mm_set_epi8(15,14,0xff,0xff,13,12,0xff,0xff,11,10,0xff,0xff,9,8,0xff,0xff);
  
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
    
    res00 = _mm_shuffle_epi8(g0, res00_mask);
    res10 = _mm_shuffle_epi8(g0, res10_mask);
    res01 = _mm_shuffle_epi8(g1, res01_mask);
    res11 = _mm_shuffle_epi8(g1, res11_mask);

    res0  = _mm_or_si128(res00, res01);
    res1  = _mm_or_si128(res10, res11);

    _mm_store_si128(resPtr, res0);
    resPtr++;
    _mm_store_si128(resPtr, res1);    
    resPtr++;
  }

  for (int i=long_cb;i<long_cb+3;i++) {
    h->branch[2*i]   = (input[i] - parity[i])/2;
    h->branch[2*i+1] = (input[i] + parity[i])/2;
  }
}






/***********************
 * 
 * This is an attempt to parallelize the horizontal max 
 * by doing a 8x8 tranpose of the vectors and computing max 
 * in cascade. However since we need to store 16 registers 
 * for the positive and negative values the performance is not very good
 */


#ifdef use_beta_transposed_max

static inline __m128i transposed_max(__m128i a, __m128i b, __m128i c, __m128i d, 
                                     __m128i e, __m128i f, __m128i g, __m128i h)
{
  // Transpose 8 vectors 
  __m128i t0 = _mm_unpacklo_epi16(a, b);
  __m128i t1 = _mm_unpacklo_epi16(c, d);
  __m128i t2 = _mm_unpacklo_epi16(e, f);
  __m128i t3 = _mm_unpacklo_epi16(g, h);
  __m128i t4 = _mm_unpackhi_epi16(a, b);
  __m128i t5 = _mm_unpackhi_epi16(c, d);
  __m128i t6 = _mm_unpackhi_epi16(e, f);
  __m128i t7 = _mm_unpackhi_epi16(g, h);

  __m128i s0 = _mm_unpacklo_epi32(t0, t1);
  __m128i s1 = _mm_unpackhi_epi32(t0, t1);
  __m128i s2 = _mm_unpacklo_epi32(t2, t3);
  __m128i s3 = _mm_unpackhi_epi32(t2, t3);
  __m128i s4 = _mm_unpacklo_epi32(t4, t5);
  __m128i s5 = _mm_unpackhi_epi32(t4, t5);
  __m128i s6 = _mm_unpacklo_epi32(t6, t7);
  __m128i s7 = _mm_unpackhi_epi32(t6, t7);

  __m128i x0 = _mm_unpacklo_epi64(s0, s2);
  __m128i x1 = _mm_unpackhi_epi64(s0, s2);
  __m128i x2 = _mm_unpacklo_epi64(s1, s3);
  __m128i x3 = _mm_unpackhi_epi64(s1, s3);
  __m128i x4 = _mm_unpacklo_epi64(s4, s6);
  __m128i x5 = _mm_unpackhi_epi64(s4, s6);
  __m128i x6 = _mm_unpacklo_epi64(s5, s7);
  __m128i x7 = _mm_unpackhi_epi64(s5, s7);

  // Cascade max on the transposed vector 
  __m128i res = _mm_max_epi16(x0,
                _mm_max_epi16(x1,
                _mm_max_epi16(x2,
                _mm_max_epi16(x3,
                _mm_max_epi16(x4,
                _mm_max_epi16(x5,
                _mm_max_epi16(x6,
                              x7)))))));
                              
  return res;   
}

void map_sse_beta(map_gen_t * s, int16_t * output, uint32_t long_cb)
{
  int k;
  uint32_t end = long_cb + 3;
  const __m128i *alphaPtr = (const __m128i*) s->alpha;
 
  __m128i beta_k = _mm_set_epi16(-INF, -INF, -INF, -INF, -INF, -INF, -INF, 0);
  __m128i g, alpha_k; 
  __m128i bn, bn_0, bn_1, bn_2, bn_3, bn_4, bn_5, bn_6, bn_7;
  __m128i bp, bp_0, bp_1, bp_2, bp_3, bp_4, bp_5, bp_6, bp_7;
  
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
     * metrics. 
     */
#define BETA_STEP_CNT(c,d) g = _mm_shuffle_epi8(gv, shuf_g[c]);\
    BETA_STEP(g)\
    alpha_k = _mm_load_si128(alphaPtr);\
    alphaPtr--;\
    bp_##d = _mm_add_epi16(bp, alpha_k);\
    bn_##d = _mm_add_epi16(bn, alpha_k);\

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
  __m128i *outPtr = (__m128i*) &output[long_cb-8];
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
    
    __m128i bn_transp = transposed_max(bn_7, bn_6, bn_5, bn_4, bn_3, bn_2, bn_1, bn_0);
    __m128i bp_transp = transposed_max(bp_7, bp_6, bp_5, bp_4, bp_3, bp_2, bp_1, bp_0);
    __m128i outval = _mm_sub_epi16(bp_transp,bn_transp);
    _mm_store_si128(outPtr, outval);
    outPtr--;    
  }  
}
#endif




#endif


