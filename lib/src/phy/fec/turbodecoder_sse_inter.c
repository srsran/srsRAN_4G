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


#define NCB 8

#define INF 10000

#ifdef LV_HAVE_SSE
#include <smmintrin.h>

void sse_inter_extract_syst1(srslte_tdec_simd_inter_t *h, uint16_t *inter, uint32_t long_cb) 
{
  __m128i *llr1Ptr  = (__m128i*) h->llr1; 
  __m128i *wPtr     = (__m128i*) h->w; 
  __m128i *syst1Ptr = (__m128i*) h->syst1; 
  
  for (int i = 0; i < long_cb; i++) {    
    __m128i llr1 = _mm_load_si128(&llr1Ptr[inter[i]]);
    __m128i w    = _mm_load_si128(&wPtr[inter[i]]);
    _mm_store_si128(syst1Ptr++, _mm_sub_epi16(llr1, w));
  }
}

void sse_inter_update_w(srslte_tdec_simd_inter_t *h, uint16_t *deinter, uint32_t long_cb) 
{
  __m128i *llr1Ptr  = (__m128i*) h->llr1; 
  __m128i *llr2Ptr  = (__m128i*) h->llr2; 
  __m128i *wPtr     = (__m128i*) h->w; 
  __m128i *syst1Ptr = (__m128i*) h->syst1; 
  
  for (int i = 0; i < long_cb; i++) {    
    __m128i llr1 = _mm_load_si128(llr1Ptr++);
    __m128i w    = _mm_load_si128(wPtr++);
    __m128i llr2 = _mm_load_si128(&llr2Ptr[deinter[i]]);
    
    _mm_store_si128(syst1Ptr++, _mm_add_epi16(w, _mm_sub_epi16(llr2, llr1)));
  }
}

/* Computes beta values */
void map_sse_inter_beta(srslte_tdec_simd_inter_t * s, int16_t *input, int16_t *parity, int16_t * output, uint32_t long_cb)
{
  __m128i m_b[8], new[8], old[8], max1[8], max0[8];
  __m128i x, y, xy;
  __m128i m1, m0;
  uint32_t end = long_cb + 3;
  uint32_t i;

  __m128i *inputPtr  = (__m128i*) input;
  __m128i *parityPtr = (__m128i*) parity;
  __m128i *outputPtr = (__m128i*) output;
  __m128i *alphaPtr  = (__m128i*) s->alpha;

  for (int i = 0; i < 8; i++) {
    old[i] = _mm_set1_epi16(0);
  }

  for (int k = end - 1; k >= 0; k--) {
    x = _mm_load_si128(inputPtr++);
    y = _mm_load_si128(parityPtr++);

    xy = _mm_add_epi16(x,y);

    m_b[0] = _mm_add_epi16(old[4], xy);
    m_b[1] = old[4];
    m_b[2] = _mm_add_epi16(old[5], y);
    m_b[3] = _mm_add_epi16(old[5], x);
    m_b[4] = _mm_add_epi16(old[6], x);
    m_b[5] = _mm_add_epi16(old[6], y);
    m_b[6] = old[7];
    m_b[7] = _mm_add_epi16(old[7], xy);

    new[0] = old[0];
    new[1] = _mm_add_epi16(old[0], xy);
    new[2] = _mm_add_epi16(old[1], x);
    new[3] = _mm_add_epi16(old[1], y);
    new[4] = _mm_add_epi16(old[2], y);
    new[5] = _mm_add_epi16(old[2], x);
    new[6] = _mm_add_epi16(old[3], xy);
    new[7] = old[3];

    for (i = 0; i < 8; i++) {
      __m128i alpha = _mm_load_si128(alphaPtr++);
      max0[i] = _mm_add_epi16(alpha, m_b[i]);
      max1[i] = _mm_add_epi16(alpha, new[i]);
    }

    m1 = _mm_max_epi16(max1[0], max1[1]);
    m0 = _mm_max_epi16(max0[0], max0[1]);

    for (i = 2; i < 8; i++) {
      m1 = _mm_max_epi16(m1, max1[i]);
      m0 = _mm_max_epi16(m0, max0[i]);      
    }

    for (i = 0; i < 8; i++) {
      new[i] = _mm_max_epi16(m_b[i], new[i]);
      old[i] = new[i];
    }

    __m128i out = _mm_sub_epi16(m1, m0);
    _mm_store_si128(outputPtr++, out);

    // normalize 
    if ((k%4)==0) {
      for (int i=1;i<8;i++) {
        _mm_sub_epi16(old[i], old[0]);
      }
    }
  }
}

/* Computes alpha metrics */
void map_see_inter_alpha(srslte_tdec_simd_inter_t * s, int16_t *input, int16_t *parity, uint32_t long_cb)
{
  __m128i m_b[8], new[8], old[8];
  __m128i x, y, xy;
  uint32_t k;
  
  __m128i *inputPtr  = (__m128i*) input;
  __m128i *parityPtr = (__m128i*) parity;
  __m128i *alphaPtr  = (__m128i*) s->alpha;
  
  old[0] = _mm_set1_epi16(0);
  for (int i = 1; i < 8; i++) {
    old[i] = _mm_set1_epi16(-INF);
  }

  for (k = 0; k < long_cb; k++) {
    x = _mm_load_si128(inputPtr++);
    y = _mm_load_si128(parityPtr++);

    xy = _mm_add_epi16(x,y);

    m_b[0] = old[0];
    m_b[1] = _mm_add_epi16(old[3], y);
    m_b[2] = _mm_add_epi16(old[4], y);
    m_b[3] = old[7];
    m_b[4] = old[1];
    m_b[5] = _mm_add_epi16(old[2], y);
    m_b[6] = _mm_add_epi16(old[5], y);
    m_b[7] = old[6];

    new[0] = _mm_add_epi16(old[1], xy);
    new[1] = _mm_add_epi16(old[2], x);
    new[2] = _mm_add_epi16(old[5], x);
    new[3] = _mm_add_epi16(old[6], xy);
    new[4] = _mm_add_epi16(old[0], xy);
    new[5] = _mm_add_epi16(old[3], x);
    new[6] = _mm_add_epi16(old[4], x);
    new[7] = _mm_add_epi16(old[7], xy);    
    
    for (int i = 0; i < 8; i++) {
      new[i] = _mm_max_epi16(m_b[i], new[i]);
      old[i] = new[i];
      _mm_store_si128(alphaPtr++, old[i]);
    }

    // normalize 
    if ((k%4)==0) {
      for (int i=1;i<8;i++) {
        _mm_sub_epi16(old[i], old[0]);
      }
    }
  }
}

#endif
