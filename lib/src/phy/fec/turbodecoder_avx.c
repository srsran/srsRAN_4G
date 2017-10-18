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


#ifdef LV_HAVE_AVX2

#include <smmintrin.h>
#include <immintrin.h>


// Number of CB processed in parllel in AVX
#define NCB 2 

/*
static void print_256i(__m256i x) {
  int16_t *s = (int16_t*) &x; 
  printf("[%d", s[0]);
  for (int i=1;i<16;i++) {
    printf(",%d", s[i]);
  }
  printf("]\n");
}
*/

/* Computes the horizontal MAX from 8 16-bit integers using the minpos_epu16 SSE4.1 instruction */
static inline int16_t hMax0(__m256i masked_value)
{
  __m128i tmp1 = _mm256_extractf128_si256(masked_value, 0); 
  __m128i tmp3 = _mm_minpos_epu16(tmp1);
  return (int16_t)(_mm_cvtsi128_si32(tmp3));
}

static inline int16_t hMax1(__m256i masked_value)
{
  __m128i tmp1 = _mm256_extractf128_si256(masked_value, 1); 
  __m128i tmp3 = _mm_minpos_epu16(tmp1);
  return (int16_t)(_mm_cvtsi128_si32(tmp3));
}

/* Computes beta values */
void map_avx_beta(map_gen_t * s, int16_t * output[SRSLTE_TDEC_MAX_NPAR], uint32_t long_cb)
{
  int k;
  uint32_t end = long_cb + 3;
  const __m256i *alphaPtr = (const __m256i*) s->alpha;
 
  __m256i beta_k = _mm256_set_epi16(-INF, -INF, -INF, -INF, -INF, -INF, -INF, 0, -INF, -INF, -INF, -INF, -INF, -INF, -INF, 0);
  __m256i g, bp, bn, alpha_k; 
  
  /* Define the shuffle constant for the positive beta */
  __m256i shuf_bp = _mm256_set_epi8(
    // 1st CB
    15+16, 14+16, // 7
    7+16,  6+16,  // 3
    5+16,  4+16,  // 2
    13+16, 12+16, // 6
    11+16, 10+16, // 5
    3+16,  2+16,  // 1
    1+16,  0+16,  // 0
    9+16,  8+16,  // 4

    // 2nd CB
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
  __m256i shuf_bn = _mm256_set_epi8(
    7+16, 6+16,   // 3
    15+16, 14+16, // 7
    13+16, 12+16, // 6
    5+16,  4+16,  // 2
    3+16,  2+16,  // 1
    11+16, 10+16, // 5
    9+16,  8+16,  // 4
    1+16,  0+16,  // 0
    
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
  __m256i shuf_g[4];
  shuf_g[3] = _mm256_set_epi8(3+16,2+16,1+16,0+16,1+16,0+16,3+16,2+16,3+16,2+16,1+16,0+16,1+16,0+16,3+16,2+16, 
                              3,2,1,0,1,0,3,2,3,2,1,0,1,0,3,2);
  shuf_g[2] = _mm256_set_epi8(7+16,6+16,5+16,4+16,5+16,4+16,7+16,6+16,7+16,6+16,5+16,4+16,5+16,4+16,7+16,6+16,
                              7,6,5,4,5,4,7,6,7,6,5,4,5,4,7,6);
  shuf_g[1] = _mm256_set_epi8(11+16,10+16,9+16,8+16,9+16,8+16,11+16,10+16,11+16,10+16,9+16,8+16,9+16,8+16,11+16,10+16,
                              11,10,9,8,9,8,11,10,11,10,9,8,9,8,11,10);
  shuf_g[0] = _mm256_set_epi8(15+16,14+16,13+16,12+16,13+16,12+16,15+16,14+16,15+16,14+16,13+16,12+16,13+16,12+16,15+16,14+16,
                              15,14,13,12,13,12,15,14,15,14,13,12,13,12,15,14);

  /* Define shuffle for beta normalization */
  __m256i shuf_norm = _mm256_set_epi8(17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0);

  __m256i gv;
  int16_t *b = &s->branch[2*NCB*long_cb-16];
  __m256i *gPtr = (__m256i*) b;
  
  /* This defines a beta computation step: 
   * Adds and substracts the branch metrics to the previous beta step, 
   * shuffles the states according to the trellis path and selects maximum state 
   */
#define BETA_STEP(g)     bp = _mm256_add_epi16(beta_k, g);\
    bn = _mm256_sub_epi16(beta_k, g);\
    bp = _mm256_shuffle_epi8(bp, shuf_bp);\
    bn = _mm256_shuffle_epi8(bn, shuf_bn);\
    beta_k = _mm256_max_epi16(bp, bn);    

    /* Loads the alpha metrics from memory and adds them to the temporal bn and bp 
     * metrics. Then computes horizontal maximum of both metrics and computes difference
     */
#define BETA_STEP_CNT(c,d) g = _mm256_shuffle_epi8(gv, shuf_g[c]);\
    BETA_STEP(g)\
    alpha_k = _mm256_load_si256(alphaPtr);\
    alphaPtr--;\
    bp = _mm256_add_epi16(bp, alpha_k);\
    bn = _mm256_add_epi16(bn, alpha_k);\
    bn = _mm256_sub_epi16(_mm256_set1_epi16(0x7FFF), bn);\
    bp = _mm256_sub_epi16(_mm256_set1_epi16(0x7FFF), bp);\
    output[0][k-d] = hMax0(bn) - hMax0(bp);\
    output[1][k-d] = hMax1(bn) - hMax1(bp);

  /* The tail does not require to load alpha or produce outputs. Only update 
   * beta metrics accordingly */
  for (k=end-1; k>=long_cb; k--) {
    int16_t g0_1 = s->branch[2*NCB*k];
    int16_t g1_1 = s->branch[2*NCB*k+1];
    int16_t g0_2 = s->branch[2*NCB*k+6];
    int16_t g1_2 = s->branch[2*NCB*k+6+1];
    g = _mm256_set_epi16(g1_2, g0_2, g0_2, g1_2, g1_2, g0_2, g0_2, g1_2, g1_1, g0_1, g0_1, g1_1, g1_1, g0_1, g0_1, g1_1);
    BETA_STEP(g);
  }  
  
  /* We inline 2 trelis steps for each normalization */
  __m256i norm;
  for (; k >= 0; k-=8) {    
    gv = _mm256_load_si256(gPtr);
    gPtr--;
    BETA_STEP_CNT(0,0);
    BETA_STEP_CNT(1,1);
    BETA_STEP_CNT(2,2);
    BETA_STEP_CNT(3,3);
    norm = _mm256_shuffle_epi8(beta_k, shuf_norm); 
    beta_k = _mm256_sub_epi16(beta_k, norm);
    gv = _mm256_load_si256(gPtr);
    gPtr--;
    BETA_STEP_CNT(0,4);
    BETA_STEP_CNT(1,5);
    BETA_STEP_CNT(2,6);
    BETA_STEP_CNT(3,7);
    norm = _mm256_shuffle_epi8(beta_k, shuf_norm); 
    beta_k = _mm256_sub_epi16(beta_k, norm);
  }  
}

/* Computes alpha metrics */
void map_avx_alpha(map_gen_t * s, uint32_t long_cb)
{
  uint32_t k;
  int16_t *alpha1 = s->alpha;
  int16_t *alpha2 = &s->alpha[8];
  uint32_t i;

  alpha1[0] = 0; 
  alpha2[0] = 0; 
  for (i = 1; i < 8; i++) {
    alpha1[i] = -INF;
    alpha2[i] = -INF;
  }
    
  /* Define the shuffle constant for the positive alpha */
  __m256i shuf_ap = _mm256_set_epi8(

    // 1st CB 
    31, 30, // 7
    25, 24, // 4
    23, 22, // 3
    17, 16, // 0
    29, 28, // 6
    27, 26, // 5
    21, 20, // 2
    19, 18, // 1

    // 2nd CB
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
  __m256i shuf_an = _mm256_set_epi8(

    // 1nd CB 
    29, 28, // 6
    27, 26, // 5
    21, 20, // 2
    19, 18, // 1
    31, 30, // 7
    25, 24, // 4
    23, 22, // 3
    17, 16, // 0

    // 2nd CB 
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
  __m256i shuf_g[4];
  shuf_g[0] = _mm256_set_epi8(3+16,2+16,3+16,2+16,1+16,0+16,1+16,0+16,1+16,0+16,1+16,0+16,3+16,2+16,3+16,2+16,       
                              3,2,3,2,1,0,1,0,1,0,1,0,3,2,3,2);
  shuf_g[1] = _mm256_set_epi8(7+16,6+16,7+16,6+16,5+16,4+16,5+16,4+16,5+16,4+16,5+16,4+16,7+16,6+16,7+16,6+16,    
                              7,6,7,6,5,4,5,4,5,4,5,4,7,6,7,6);
  shuf_g[2] = _mm256_set_epi8(11+16,10+16,11+16,10+16,9+16,8+16,9+16,8+16,9+16,8+16,9+16,8+16,11+16,10+16,11+16,10+16,       
                              11,10,11,10,9,8,9,8,9,8,9,8,11,10,11,10);
  shuf_g[3] = _mm256_set_epi8(15+16,14+16,15+16,14+16,13+16,12+16,13+16,12+16,13+16,12+16,13+16,12+16,15+16,14+16,15+16,14+16,
                              15,14,15,14,13,12,13,12,13,12,13,12,15,14,15,14);

  __m256i shuf_norm = _mm256_set_epi8(17,16,17,16,17,16,17,16,17,16,17,16,17,16,17,16,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0);
  
  __m256i* alphaPtr = (__m256i*) s->alpha;
  alphaPtr++;

  __m256i gv; 
  __m256i *gPtr = (__m256i*) s->branch;
  __m256i g, ap, an; 
    
  __m256i alpha_k = _mm256_set_epi16(-INF, -INF, -INF, -INF, -INF, -INF, -INF, 0, -INF, -INF, -INF, -INF, -INF, -INF, -INF, 0);
  
  /* This defines a alpha computation step: 
   * Adds and substracts the branch metrics to the previous alpha step, 
   * shuffles the states according to the trellis path and selects maximum state 
   */
#define ALPHA_STEP(c)  g = _mm256_shuffle_epi8(gv, shuf_g[c]); \
  ap = _mm256_add_epi16(alpha_k, g);\
  an = _mm256_sub_epi16(alpha_k, g);\
  ap = _mm256_shuffle_epi8(ap, shuf_ap);\
  an = _mm256_shuffle_epi8(an, shuf_an);\
  alpha_k = _mm256_max_epi16(ap, an);\
  _mm256_store_si256(alphaPtr, alpha_k);\
  alphaPtr++;\


  /* In this loop, we compute 8 steps and normalize twice for each branch metrics memory load */
  __m256i norm;
  for (k = 0; k < long_cb/8; k++) {
    gv = _mm256_load_si256(gPtr);
    
    gPtr++;
    ALPHA_STEP(0);
    ALPHA_STEP(1);
    ALPHA_STEP(2);
    ALPHA_STEP(3);
    norm = _mm256_shuffle_epi8(alpha_k, shuf_norm); 
    alpha_k = _mm256_sub_epi16(alpha_k, norm);
    gv = _mm256_load_si256(gPtr);
    gPtr++;
    ALPHA_STEP(0);
    ALPHA_STEP(1);
    ALPHA_STEP(2);
    ALPHA_STEP(3);
    norm = _mm256_shuffle_epi8(alpha_k, shuf_norm); 
    alpha_k = _mm256_sub_epi16(alpha_k, norm);
  }  
}

void map_sse_gamma_single(int16_t *output, int16_t *input, int16_t *app, int16_t *parity) 
{
  __m128i res00, res10, res01, res11, res0, res1; 
  __m128i in, ap, pa, g1, g0;

  __m128i *inPtr  = (__m128i*) input;
  __m128i *appPtr = (__m128i*) app;
  __m128i *paPtr  = (__m128i*) parity;
  __m128i *resPtr = (__m128i*) output;
  
  __m128i res00_mask = _mm_set_epi8(0xff,0xff,7,6,0xff,0xff,5,4,0xff,0xff,3,2,0xff,0xff,1,0);
  __m128i res10_mask = _mm_set_epi8(0xff,0xff,15,14,0xff,0xff,13,12,0xff,0xff,11,10,0xff,0xff,9,8);
  __m128i res01_mask = _mm_set_epi8(7,6,0xff,0xff,5,4,0xff,0xff,3,2,0xff,0xff,1,0,0xff,0xff);
  __m128i res11_mask = _mm_set_epi8(15,14,0xff,0xff,13,12,0xff,0xff,11,10,0xff,0xff,9,8,0xff,0xff);
  
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


/* Compute branch metrics (gamma) */
void map_avx_gamma(map_gen_t * h, int16_t *input, int16_t *app, int16_t *parity, uint32_t cbidx, uint32_t long_cb) 
{
  __m128i res10, res20, res11, res21, res1, res2; 
  __m256i in, ap, pa, g1, g0;

  __m256i *inPtr  = (__m256i*) input;
  __m256i *appPtr = (__m256i*) app;
  __m256i *paPtr  = (__m256i*) parity;
  __m128i *resPtr = (__m128i*) h->branch;
  
  if (cbidx) {
    resPtr++;
  }
  
  __m128i res10_mask = _mm_set_epi8(0xff,0xff,7,6,0xff,0xff,5,4,0xff,0xff,3,2,0xff,0xff,1,0);
  __m128i res11_mask = _mm_set_epi8(7,6,0xff,0xff,5,4,0xff,0xff,3,2,0xff,0xff,1,0,0xff,0xff);

  __m128i res20_mask = _mm_set_epi8(0xff,0xff,15,14,0xff,0xff,13,12,0xff,0xff,11,10,0xff,0xff,9,8);
  __m128i res21_mask = _mm_set_epi8(15,14,0xff,0xff,13,12,0xff,0xff,11,10,0xff,0xff,9,8,0xff,0xff);

  for (int i=0;i<long_cb/16;i++) {
    in = _mm256_load_si256(inPtr);
    inPtr++;
    pa = _mm256_load_si256(paPtr);
    paPtr++;
    
    if (appPtr) {
      ap = _mm256_load_si256(appPtr);
      appPtr++;
      in = _mm256_add_epi16(ap, in);
    }
    
    g0 = _mm256_sub_epi16(in, pa);
    g1 = _mm256_add_epi16(in, pa);
 
    g0 = _mm256_srai_epi16(g0, 1);
    g1 = _mm256_srai_epi16(g1, 1);
    
    __m128i g0_t = _mm256_extractf128_si256(g0, 0);
    __m128i g1_t = _mm256_extractf128_si256(g1, 0);
    
    res10 = _mm_shuffle_epi8(g0_t, res10_mask);
    res11 = _mm_shuffle_epi8(g1_t, res11_mask);

    res20 = _mm_shuffle_epi8(g0_t, res20_mask);
    res21 = _mm_shuffle_epi8(g1_t, res21_mask);

    res1  = _mm_or_si128(res10, res11);
    res2  = _mm_or_si128(res20, res21);

    _mm_store_si128(resPtr, res1);
    resPtr++;
    resPtr++;
    _mm_store_si128(resPtr, res2);    
    resPtr++;
    resPtr++;          
    
    g0_t = _mm256_extractf128_si256(g0, 1);
    g1_t = _mm256_extractf128_si256(g1, 1);
    
    res10 = _mm_shuffle_epi8(g0_t, res10_mask);
    res11 = _mm_shuffle_epi8(g1_t, res11_mask);

    res20 = _mm_shuffle_epi8(g0_t, res20_mask);
    res21 = _mm_shuffle_epi8(g1_t, res21_mask);

    res1  = _mm_or_si128(res10, res11);
    res2  = _mm_or_si128(res20, res21);

    _mm_store_si128(resPtr, res1);
    resPtr++;
    resPtr++;
    _mm_store_si128(resPtr, res2);    
    resPtr++;
    resPtr++;          

  }
  
  if (long_cb%16) {
    map_sse_gamma_single((int16_t*) resPtr, (int16_t*) inPtr, (int16_t*) appPtr, (int16_t*) paPtr);
  }
    
  for (int i=long_cb;i<long_cb+3;i++) {
    h->branch[2*i*NCB+cbidx*6]   = (input[i] - parity[i])/2;
    h->branch[2*i*NCB+cbidx*6+1] = (input[i] + parity[i])/2;
  }
}


#endif


