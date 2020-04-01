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

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "srslte/phy/fec/turbodecoder_sse.h"
#include "srslte/phy/utils/vector.h"

#include <inttypes.h>

#ifdef LV_HAVE_SSE
#include <smmintrin.h>

#endif

#define NUMSTATES 8
#define NINPUTS 2
#define TAIL 3
#define TOTALTAIL 12

#define INF 10000

#ifdef LV_HAVE_SSE

#define debug_enabled 0

#if debug_enabled
#define debug_state(c, d)                                                                                              \
  printf("k=%5d, in=%5d, pa=%5d, out=%5d, alpha=",                                                                     \
         k - d,                                                                                                        \
         s->branch[2 * (k - d)] + s->branch[2 * (k - d) + 1],                                                          \
         -s->branch[2 * (k - d)] + s->branch[2 * (k - d) + 1],                                                         \
         output[k - d]);                                                                                               \
  print_128i(alpha_k);                                                                                                 \
  printf(", beta=");                                                                                                   \
  print_128i(beta_k);                                                                                                  \
  printf("\n");

static void print_128i(__m128i x)
{
  int16_t* s = (int16_t*)&x;
  printf("[%5d", s[0]);
  for (int i = 1; i < 8; i++) {
    printf(",%5d", s[i]);
  }
  printf("]");
}

static uint32_t max_128i(__m128i x)
{
  int16_t* s   = (int16_t*)&x;
  int16_t  m   = -INF;
  uint32_t max = 0;
  for (int i = 1; i < 8; i++) {
    if (s[i] > m) {
      max = i;
      m   = s[i];
    }
  }
  return max;
}

#else
#define debug_state(c, d)
#endif

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
void tdec_sse_beta(tdec_sse_t* s, int16_t* output, uint32_t long_cb)
{
  int            k;
  uint32_t       end      = long_cb + 3;
  const __m128i* alphaPtr = (const __m128i*)s->alpha;

  __m128i beta_k = _mm_set_epi16(-INF, -INF, -INF, -INF, -INF, -INF, -INF, 0);
  __m128i g, bp, bn, alpha_k;

  /* Define the shuffle constant for the positive beta */
  __m128i shuf_bp = _mm_set_epi8(15,
                                 14, // 7
                                 7,
                                 6, // 3
                                 5,
                                 4, // 2
                                 13,
                                 12, // 6
                                 11,
                                 10, // 5
                                 3,
                                 2, // 1
                                 1,
                                 0, // 0
                                 9,
                                 8 // 4
  );

  /* Define the shuffle constant for the negative beta */
  __m128i shuf_bn = _mm_set_epi8(7,
                                 6, // 3
                                 15,
                                 14, // 7
                                 13,
                                 12, // 6
                                 5,
                                 4, // 2
                                 3,
                                 2, // 1
                                 11,
                                 10, // 5
                                 9,
                                 8, // 4
                                 1,
                                 0 // 0
  );

  alphaPtr += long_cb - 1;

  /* Define shuffle for branch costs */
  __m128i shuf_g[4];
  shuf_g[3] = _mm_set_epi8(3, 2, 1, 0, 1, 0, 3, 2, 3, 2, 1, 0, 1, 0, 3, 2);
  shuf_g[2] = _mm_set_epi8(7, 6, 5, 4, 5, 4, 7, 6, 7, 6, 5, 4, 5, 4, 7, 6);
  shuf_g[1] = _mm_set_epi8(11, 10, 9, 8, 9, 8, 11, 10, 11, 10, 9, 8, 9, 8, 11, 10);
  shuf_g[0] = _mm_set_epi8(15, 14, 13, 12, 13, 12, 15, 14, 15, 14, 13, 12, 13, 12, 15, 14);
  __m128i  gv;
  int16_t* b    = &s->branch[2 * long_cb - 8];
  __m128i* gPtr = (__m128i*)b;
  /* Define shuffle for beta normalization */
  __m128i shuf_norm = _mm_set_epi8(1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0);

  /* This defines a beta computation step:
   * Adds and substracts the branch metrics to the previous beta step,
   * shuffles the states according to the trellis path and selects maximum state
   */
#define BETA_STEP(g)                                                                                                   \
  bp     = _mm_add_epi16(beta_k, g);                                                                                   \
  bn     = _mm_sub_epi16(beta_k, g);                                                                                   \
  bp     = _mm_shuffle_epi8(bp, shuf_bp);                                                                              \
  bn     = _mm_shuffle_epi8(bn, shuf_bn);                                                                              \
  beta_k = _mm_max_epi16(bp, bn);

  /* Loads the alpha metrics from memory and adds them to the temporal bn and bp
   * metrics. Then computes horizontal maximum of both metrics and computes difference
   */
#define BETA_STEP_CNT(c, d)                                                                                            \
  g = _mm_shuffle_epi8(gv, shuf_g[c]);                                                                                 \
  BETA_STEP(g)                                                                                                         \
  alpha_k = _mm_load_si128(alphaPtr);                                                                                  \
  alphaPtr--;                                                                                                          \
  bp            = _mm_add_epi16(bp, alpha_k);                                                                          \
  bn            = _mm_add_epi16(bn, alpha_k);                                                                          \
  output[k - d] = hMax(bn) - hMax(bp);                                                                                 \
  debug_state(c, d);

  /* The tail does not require to load alpha or produce outputs. Only update
   * beta metrics accordingly */
  for (k = end - 1; k >= long_cb; k--) {
    int16_t g0 = s->branch[2 * k];
    int16_t g1 = s->branch[2 * k + 1];
    g          = _mm_set_epi16(g1, g0, g0, g1, g1, g0, g0, g1);
    BETA_STEP(g);
  }

  /* We inline 2 trelis steps for each normalization */
  __m128i norm;
  for (; k >= 0; k -= 8) {
    gv = _mm_load_si128(gPtr);
    gPtr--;

    BETA_STEP_CNT(0, 0);
    BETA_STEP_CNT(1, 1);
    BETA_STEP_CNT(2, 2);
    BETA_STEP_CNT(3, 3);
    norm   = _mm_shuffle_epi8(beta_k, shuf_norm);
    beta_k = _mm_sub_epi16(beta_k, norm);
    gv     = _mm_load_si128(gPtr);
    gPtr--;
    BETA_STEP_CNT(0, 4);
    BETA_STEP_CNT(1, 5);
    BETA_STEP_CNT(2, 6);
    BETA_STEP_CNT(3, 7);

    norm   = _mm_shuffle_epi8(beta_k, shuf_norm);
    beta_k = _mm_sub_epi16(beta_k, norm);
  }
}

#endif

/* Computes alpha metrics */
void tdec_sse_alpha(tdec_sse_t* s, uint32_t long_cb)
{
  uint32_t k;
  int16_t* alpha = s->alpha;
  uint32_t i;

  alpha[0] = 0;
  for (i = 1; i < 8; i++) {
    alpha[i] = -INF;
  }

  /* Define the shuffle constant for the positive alpha */
  __m128i shuf_ap = _mm_set_epi8(15,
                                 14, // 7
                                 9,
                                 8, // 4
                                 7,
                                 6, // 3
                                 1,
                                 0, // 0
                                 13,
                                 12, // 6
                                 11,
                                 10, // 5
                                 5,
                                 4, // 2
                                 3,
                                 2 // 1
  );

  /* Define the shuffle constant for the negative alpha */
  __m128i shuf_an = _mm_set_epi8(13,
                                 12, // 6
                                 11,
                                 10, // 5
                                 5,
                                 4, // 2
                                 3,
                                 2, // 1
                                 15,
                                 14, // 7
                                 9,
                                 8, // 4
                                 7,
                                 6, // 3
                                 1,
                                 0 // 0
  );

  /* Define shuffle for branch costs */
  __m128i shuf_g[4];
  shuf_g[0] = _mm_set_epi8(3, 2, 3, 2, 1, 0, 1, 0, 1, 0, 1, 0, 3, 2, 3, 2);
  shuf_g[1] = _mm_set_epi8(7, 6, 7, 6, 5, 4, 5, 4, 5, 4, 5, 4, 7, 6, 7, 6);
  shuf_g[2] = _mm_set_epi8(11, 10, 11, 10, 9, 8, 9, 8, 9, 8, 9, 8, 11, 10, 11, 10);
  shuf_g[3] = _mm_set_epi8(15, 14, 15, 14, 13, 12, 13, 12, 13, 12, 13, 12, 15, 14, 15, 14);

  __m128i shuf_norm = _mm_set_epi8(1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0);

  __m128i* alphaPtr = (__m128i*)alpha;
  alphaPtr++;

  __m128i  gv;
  __m128i* gPtr = (__m128i*)s->branch;
  __m128i  g, ap, an;

  __m128i alpha_k = _mm_set_epi16(-INF, -INF, -INF, -INF, -INF, -INF, -INF, 0);

  /* This defines a alpha computation step:
   * Adds and substracts the branch metrics to the previous alpha step,
   * shuffles the states according to the trellis path and selects maximum state
   */
#define ALPHA_STEP(c)                                                                                                  \
  g       = _mm_shuffle_epi8(gv, shuf_g[c]);                                                                           \
  ap      = _mm_add_epi16(alpha_k, g);                                                                                 \
  an      = _mm_sub_epi16(alpha_k, g);                                                                                 \
  ap      = _mm_shuffle_epi8(ap, shuf_ap);                                                                             \
  an      = _mm_shuffle_epi8(an, shuf_an);                                                                             \
  alpha_k = _mm_max_epi16(ap, an);                                                                                     \
  _mm_store_si128(alphaPtr, alpha_k);                                                                                  \
  alphaPtr++;

  /* In this loop, we compute 8 steps and normalize twice for each branch metrics memory load */
  __m128i norm;
  for (k = 0; k < long_cb / 8; k++) {
    gv = _mm_load_si128(gPtr);
    gPtr++;
    ALPHA_STEP(0);
    ALPHA_STEP(1);
    ALPHA_STEP(2);
    ALPHA_STEP(3);
    norm    = _mm_shuffle_epi8(alpha_k, shuf_norm);
    alpha_k = _mm_sub_epi16(alpha_k, norm);
    gv      = _mm_load_si128(gPtr);
    gPtr++;
    ALPHA_STEP(0);
    ALPHA_STEP(1);
    ALPHA_STEP(2);
    ALPHA_STEP(3);
    norm    = _mm_shuffle_epi8(alpha_k, shuf_norm);
    alpha_k = _mm_sub_epi16(alpha_k, norm);
  }
}

/* Compute branch metrics (gamma) */
void tdec_sse_gamma(tdec_sse_t* h, int16_t* input, int16_t* app, int16_t* parity, uint32_t long_cb)
{
  __m128i res00, res10, res01, res11, res0, res1;
  __m128i in, ap, pa, g1, g0;

  __m128i* inPtr  = (__m128i*)input;
  __m128i* appPtr = (__m128i*)app;
  __m128i* paPtr  = (__m128i*)parity;
  __m128i* resPtr = (__m128i*)h->branch;

  __m128i res00_mask = _mm_set_epi8(0xff, 0xff, 7, 6, 0xff, 0xff, 5, 4, 0xff, 0xff, 3, 2, 0xff, 0xff, 1, 0);
  __m128i res10_mask = _mm_set_epi8(0xff, 0xff, 15, 14, 0xff, 0xff, 13, 12, 0xff, 0xff, 11, 10, 0xff, 0xff, 9, 8);
  __m128i res01_mask = _mm_set_epi8(7, 6, 0xff, 0xff, 5, 4, 0xff, 0xff, 3, 2, 0xff, 0xff, 1, 0, 0xff, 0xff);
  __m128i res11_mask = _mm_set_epi8(15, 14, 0xff, 0xff, 13, 12, 0xff, 0xff, 11, 10, 0xff, 0xff, 9, 8, 0xff, 0xff);

  for (int i = 0; i < long_cb / 8; i++) {
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

    res0 = _mm_or_si128(res00, res01);
    res1 = _mm_or_si128(res10, res11);

    _mm_store_si128(resPtr, res0);
    resPtr++;
    _mm_store_si128(resPtr, res1);
    resPtr++;

    // printf("k=%d, in=%d, pa=%d, g0=%d, g1=%d\n", i, input[i], parity[i], h->branch[2*i], h->branch[2*i+1]);
  }

  for (int i = long_cb; i < long_cb + 3; i++) {
    h->branch[2 * i]     = (input[i] - parity[i]) / 2;
    h->branch[2 * i + 1] = (input[i] + parity[i]) / 2;
  }
}

/* Inititalizes constituent decoder object */
int tdec_sse_init(void** hh, uint32_t max_long_cb)
{
  *hh = calloc(1, sizeof(tdec_sse_t));

  tdec_sse_t* h = (tdec_sse_t*)*hh;

  h->max_long_cb = max_long_cb;

  h->alpha = srslte_vec_i16_malloc((max_long_cb + TOTALTAIL + 1) * NUMSTATES);
  if (!h->alpha) {
    perror("srslte_vec_malloc");
    return -1;
  }
  h->branch = srslte_vec_i16_malloc((max_long_cb + TOTALTAIL + 1) * NUMSTATES);
  if (!h->branch) {
    perror("srslte_vec_malloc");
    return -1;
  }
  return 1;
}

void tdec_sse_free(void* hh)
{
  tdec_sse_t* h = (tdec_sse_t*)hh;

  if (h) {
    if (h->alpha) {
      free(h->alpha);
    }
    if (h->branch) {
      free(h->branch);
    }
    free(h);
  }
}

/* Runs one instance of a decoder */
void tdec_sse_dec(void* hh, int16_t* input, int16_t* app, int16_t* parity, int16_t* output, uint32_t long_cb)
{
  tdec_sse_t* h = (tdec_sse_t*)hh;

  // Compute branch metrics
  tdec_sse_gamma(h, input, app, parity, long_cb);

  // Forward recursion
  tdec_sse_alpha(h, long_cb);

  // Backwards recursion + LLR computation
  tdec_sse_beta(h, output, long_cb);
}

/* Deinterleaves the 3 streams from the input (systematic and 2 parity bits) into
 * 3 buffers ready to be used by compute_gamma()
 */
void tdec_sse_extract_input(int16_t* input,
                            int16_t* syst0,
                            int16_t* app2,
                            int16_t* parity0,
                            int16_t* parity1,
                            uint32_t long_cb)
{
  uint32_t i;

  __m128i* inputPtr = (__m128i*)input;
  __m128i  in0, in1, in2;
  __m128i  s0, s1, s2, s;
  __m128i  p00, p01, p02, p0;
  __m128i  p10, p11, p12, p1;

  __m128i* sysPtr = (__m128i*)syst0;
  __m128i* pa0Ptr = (__m128i*)parity0;
  __m128i* pa1Ptr = (__m128i*)parity1;

  // pick bits 0, 3, 6 from 1st word
  __m128i s0_mask = _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 13, 12, 7, 6, 1, 0);
  // pick bits 1, 4, 7 from 2st word
  __m128i s1_mask = _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 15, 14, 9, 8, 3, 2, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
  // pick bits 2, 5 from 3rd word
  __m128i s2_mask = _mm_set_epi8(11, 10, 5, 4, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);

  // pick bits 1, 4, 7 from 1st word
  __m128i p00_mask = _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 15, 14, 9, 8, 3, 2);
  // pick bits 2, 5, from 2st word
  __m128i p01_mask = _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 11, 10, 5, 4, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
  // pick bits 0, 3, 6 from 3rd word
  __m128i p02_mask = _mm_set_epi8(13, 12, 7, 6, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);

  // pick bits 2, 5 from 1st word
  __m128i p10_mask = _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 11, 10, 5, 4);
  // pick bits 0, 3, 6, from 2st word
  __m128i p11_mask = _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 13, 12, 7, 6, 1, 0, 0xff, 0xff, 0xff, 0xff);
  // pick bits 1, 4, 7 from 3rd word
  __m128i p12_mask = _mm_set_epi8(15, 14, 9, 8, 3, 2, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);

  // Split systematic and parity bits
  for (i = 0; i < long_cb / 8; i++) {

    in0 = _mm_load_si128(inputPtr);
    inputPtr++;
    in1 = _mm_load_si128(inputPtr);
    inputPtr++;
    in2 = _mm_load_si128(inputPtr);
    inputPtr++;

    /* Deinterleave Systematic bits */
    s0 = _mm_shuffle_epi8(in0, s0_mask);
    s1 = _mm_shuffle_epi8(in1, s1_mask);
    s2 = _mm_shuffle_epi8(in2, s2_mask);
    s  = _mm_or_si128(s0, s1);
    s  = _mm_or_si128(s, s2);

    _mm_store_si128(sysPtr, s);
    sysPtr++;

    /* Deinterleave parity 0 bits */
    p00 = _mm_shuffle_epi8(in0, p00_mask);
    p01 = _mm_shuffle_epi8(in1, p01_mask);
    p02 = _mm_shuffle_epi8(in2, p02_mask);
    p0  = _mm_or_si128(p00, p01);
    p0  = _mm_or_si128(p0, p02);

    _mm_store_si128(pa0Ptr, p0);
    pa0Ptr++;

    /* Deinterleave parity 1 bits */
    p10 = _mm_shuffle_epi8(in0, p10_mask);
    p11 = _mm_shuffle_epi8(in1, p11_mask);
    p12 = _mm_shuffle_epi8(in2, p12_mask);
    p1  = _mm_or_si128(p10, p11);
    p1  = _mm_or_si128(p1, p12);

    _mm_store_si128(pa1Ptr, p1);
    pa1Ptr++;
  }

  for (i = 0; i < 3; i++) {
    syst0[i + long_cb]   = input[3 * long_cb + 2 * i];
    parity0[i + long_cb] = input[3 * long_cb + 2 * i + 1];
  }
  for (i = 0; i < 3; i++) {
    app2[i + long_cb]    = input[3 * long_cb + 6 + 2 * i];
    parity1[i + long_cb] = input[3 * long_cb + 6 + 2 * i + 1];
  }
}

void tdec_sse_decision_byte(int16_t* app1, uint8_t* output, uint32_t long_cb)
{
  uint8_t mask[8] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};

  // long_cb is always byte aligned
  for (uint32_t i = 0; i < long_cb / 8; i++) {
    uint8_t out0 = app1[8 * i + 0] > 0 ? mask[0] : 0;
    uint8_t out1 = app1[8 * i + 1] > 0 ? mask[1] : 0;
    uint8_t out2 = app1[8 * i + 2] > 0 ? mask[2] : 0;
    uint8_t out3 = app1[8 * i + 3] > 0 ? mask[3] : 0;
    uint8_t out4 = app1[8 * i + 4] > 0 ? mask[4] : 0;
    uint8_t out5 = app1[8 * i + 5] > 0 ? mask[5] : 0;
    uint8_t out6 = app1[8 * i + 6] > 0 ? mask[6] : 0;
    uint8_t out7 = app1[8 * i + 7] > 0 ? mask[7] : 0;

    output[i] = out0 | out1 | out2 | out3 | out4 | out5 | out6 | out7;
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

static inline __m128i
transposed_max(__m128i a, __m128i b, __m128i c, __m128i d, __m128i e, __m128i f, __m128i g, __m128i h)
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
  __m128i res = _mm_max_epi16(
      x0,
      _mm_max_epi16(x1,
                    _mm_max_epi16(x2, _mm_max_epi16(x3, _mm_max_epi16(x4, _mm_max_epi16(x5, _mm_max_epi16(x6, x7)))))));

  return res;
}

void tdec_sse_beta(tdec_sse_t* s, int16_t* output, uint32_t long_cb)
{
  int            k;
  uint32_t       end      = long_cb + 3;
  const __m128i* alphaPtr = (const __m128i*)s->alpha;

  __m128i beta_k = _mm_set_epi16(-INF, -INF, -INF, -INF, -INF, -INF, -INF, 0);
  __m128i g, alpha_k;
  __m128i bn, bn_0, bn_1, bn_2, bn_3, bn_4, bn_5, bn_6, bn_7;
  __m128i bp, bp_0, bp_1, bp_2, bp_3, bp_4, bp_5, bp_6, bp_7;

  /* Define the shuffle constant for the positive beta */
  __m128i shuf_bp = _mm_set_epi8(15,
                                 14, // 7
                                 7,
                                 6, // 3
                                 5,
                                 4, // 2
                                 13,
                                 12, // 6
                                 11,
                                 10, // 5
                                 3,
                                 2, // 1
                                 1,
                                 0, // 0
                                 9,
                                 8 // 4
  );

  /* Define the shuffle constant for the negative beta */
  __m128i shuf_bn = _mm_set_epi8(7,
                                 6, // 3
                                 15,
                                 14, // 7
                                 13,
                                 12, // 6
                                 5,
                                 4, // 2
                                 3,
                                 2, // 1
                                 11,
                                 10, // 5
                                 9,
                                 8, // 4
                                 1,
                                 0 // 0
  );

  alphaPtr += long_cb - 1;

  /* Define shuffle for branch costs */
  __m128i shuf_g[4];
  shuf_g[3] = _mm_set_epi8(3, 2, 1, 0, 1, 0, 3, 2, 3, 2, 1, 0, 1, 0, 3, 2);
  shuf_g[2] = _mm_set_epi8(7, 6, 5, 4, 5, 4, 7, 6, 7, 6, 5, 4, 5, 4, 7, 6);
  shuf_g[1] = _mm_set_epi8(11, 10, 9, 8, 9, 8, 11, 10, 11, 10, 9, 8, 9, 8, 11, 10);
  shuf_g[0] = _mm_set_epi8(15, 14, 13, 12, 13, 12, 15, 14, 15, 14, 13, 12, 13, 12, 15, 14);
  __m128i  gv;
  int16_t* b    = &s->branch[2 * long_cb - 8];
  __m128i* gPtr = (__m128i*)b;
  /* Define shuffle for beta normalization */
  __m128i shuf_norm = _mm_set_epi8(1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0);

  /* This defines a beta computation step:
   * Adds and substracts the branch metrics to the previous beta step,
   * shuffles the states according to the trellis path and selects maximum state
   */
#define BETA_STEP(g)                                                                                                   \
  bp     = _mm_add_epi16(beta_k, g);                                                                                   \
  bn     = _mm_sub_epi16(beta_k, g);                                                                                   \
  bp     = _mm_shuffle_epi8(bp, shuf_bp);                                                                              \
  bn     = _mm_shuffle_epi8(bn, shuf_bn);                                                                              \
  beta_k = _mm_max_epi16(bp, bn);

  /* Loads the alpha metrics from memory and adds them to the temporal bn and bp
   * metrics.
   */
#define BETA_STEP_CNT(c, d)                                                                                            \
  g = _mm_shuffle_epi8(gv, shuf_g[c]);                                                                                 \
  BETA_STEP(g)                                                                                                         \
  alpha_k = _mm_load_si128(alphaPtr);                                                                                  \
  alphaPtr--;                                                                                                          \
  bp_##d = _mm_add_epi16(bp, alpha_k);                                                                                 \
  bn_##d = _mm_add_epi16(bn, alpha_k);

  /* The tail does not require to load alpha or produce outputs. Only update
   * beta metrics accordingly */
  for (k = end - 1; k >= long_cb; k--) {
    int16_t g0 = s->branch[2 * k];
    int16_t g1 = s->branch[2 * k + 1];
    g          = _mm_set_epi16(g1, g0, g0, g1, g1, g0, g0, g1);
    BETA_STEP(g);
  }

  /* We inline 2 trelis steps for each normalization */
  __m128i  norm;
  __m128i* outPtr = (__m128i*)&output[long_cb - 8];
  for (; k >= 0; k -= 8) {
    gv = _mm_load_si128(gPtr);
    gPtr--;

    BETA_STEP_CNT(0, 0);
    BETA_STEP_CNT(1, 1);
    BETA_STEP_CNT(2, 2);
    BETA_STEP_CNT(3, 3);
    norm   = _mm_shuffle_epi8(beta_k, shuf_norm);
    beta_k = _mm_sub_epi16(beta_k, norm);
    gv     = _mm_load_si128(gPtr);
    gPtr--;
    BETA_STEP_CNT(0, 4);
    BETA_STEP_CNT(1, 5);
    BETA_STEP_CNT(2, 6);
    BETA_STEP_CNT(3, 7);
    norm   = _mm_shuffle_epi8(beta_k, shuf_norm);
    beta_k = _mm_sub_epi16(beta_k, norm);

    __m128i bn_transp = transposed_max(bn_7, bn_6, bn_5, bn_4, bn_3, bn_2, bn_1, bn_0);
    __m128i bp_transp = transposed_max(bp_7, bp_6, bp_5, bp_4, bp_3, bp_2, bp_1, bp_0);
    __m128i outval    = _mm_sub_epi16(bp_transp, bn_transp);
    _mm_store_si128(outPtr, outval);
    outPtr--;
  }
}
#endif

#endif
