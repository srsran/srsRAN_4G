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

#include <stdlib.h>
#include <assert.h>
#include <complex.h>
#include <string.h>
#include <math.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/mimo/precoding.h"
#include "srslte/phy/utils/vector.h"

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
#include <pmmintrin.h>
int srslte_predecoding_single_sse(cf_t *y[SRSLTE_MAX_PORTS], cf_t *h[SRSLTE_MAX_PORTS], cf_t *x, int nof_rxant, int nof_symbols, float noise_estimate);
int srslte_predecoding_diversity2_sse(cf_t *y[SRSLTE_MAX_PORTS], cf_t *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS], cf_t *x[SRSLTE_MAX_LAYERS], int nof_rxant, int nof_symbols);
#endif

#ifdef LV_HAVE_AVX
#include <immintrin.h>
int srslte_predecoding_single_avx(cf_t *y[SRSLTE_MAX_PORTS], cf_t *h[SRSLTE_MAX_PORTS], cf_t *x, int nof_rxant, int nof_symbols, float noise_estimate);
#endif



/************************************************
 * 
 * RECEIVER SIDE FUNCTIONS
 * 
 **************************************************/

#ifdef LV_HAVE_SSE

#define PROD(a,b) _mm_addsub_ps(_mm_mul_ps(a,_mm_moveldup_ps(b)),_mm_mul_ps(_mm_shuffle_ps(a,a,0xB1),_mm_movehdup_ps(b)))

int srslte_predecoding_single_sse(cf_t *y[SRSLTE_MAX_PORTS], cf_t *h[SRSLTE_MAX_PORTS], cf_t *x, int nof_rxant, int nof_symbols, float noise_estimate) {
  
  float *xPtr = (float*) x;
  const float *hPtr1 = (const float*) h[0];
  const float *yPtr1 = (const float*) y[0];
  const float *hPtr2 = (const float*) h[1];
  const float *yPtr2 = (const float*) y[1];

  __m128 conjugator = _mm_setr_ps(0, -0.f, 0, -0.f);
  
  __m128 noise = _mm_set1_ps(noise_estimate);
  __m128 h1Val1, h2Val1, y1Val1, y2Val1;
  __m128 h1Val2, h2Val2, y1Val2, y2Val2;
  __m128 hsquare, h1square, h2square, h1conj1, h2conj1, x1Val1, x2Val1;
  __m128 hsquare2, h1conj2, h2conj2, x1Val2, x2Val2;

  for (int i=0;i<nof_symbols/4;i++) {
    y1Val1 = _mm_load_ps(yPtr1); yPtr1+=4;
    y2Val1 = _mm_load_ps(yPtr1); yPtr1+=4;
    h1Val1 = _mm_load_ps(hPtr1); hPtr1+=4;
    h2Val1 = _mm_load_ps(hPtr1); hPtr1+=4;

    if (nof_rxant == 2) {
      y1Val2 = _mm_load_ps(yPtr2); yPtr2+=4;
      y2Val2 = _mm_load_ps(yPtr2); yPtr2+=4;
      h1Val2 = _mm_load_ps(hPtr2); hPtr2+=4;
      h2Val2 = _mm_load_ps(hPtr2); hPtr2+=4;      
    }
    
    hsquare = _mm_hadd_ps(_mm_mul_ps(h1Val1, h1Val1), _mm_mul_ps(h2Val1, h2Val1)); 
    if (nof_rxant == 2) {
      hsquare2 = _mm_hadd_ps(_mm_mul_ps(h1Val2, h1Val2), _mm_mul_ps(h2Val2, h2Val2)); 
      hsquare = _mm_add_ps(hsquare, hsquare2);
    }
    if (noise_estimate > 0) {
      hsquare  = _mm_add_ps(hsquare, noise);
    }
    
    h1square  = _mm_shuffle_ps(hsquare, hsquare, _MM_SHUFFLE(1, 1, 0, 0));
    h2square  = _mm_shuffle_ps(hsquare, hsquare, _MM_SHUFFLE(3, 3, 2, 2));
    
    /* Conjugate channel */
    h1conj1 = _mm_xor_ps(h1Val1, conjugator); 
    h2conj1 = _mm_xor_ps(h2Val1, conjugator); 

    if (nof_rxant == 2) {
      h1conj2 = _mm_xor_ps(h1Val2, conjugator); 
      h2conj2 = _mm_xor_ps(h2Val2, conjugator); 
    }
    
    /* Complex product */      
    x1Val1 = PROD(y1Val1, h1conj1);
    x2Val1 = PROD(y2Val1, h2conj1);

    if (nof_rxant == 2) {
      x1Val2 = PROD(y1Val2, h1conj2);
      x2Val2 = PROD(y2Val2, h2conj2);
      x1Val1 = _mm_add_ps(x1Val1, x1Val2);
      x2Val1 = _mm_add_ps(x2Val1, x2Val2);
    }
    
    x1Val1 = _mm_div_ps(x1Val1, h1square);
    x2Val1 = _mm_div_ps(x2Val1, h2square);
    
    _mm_store_ps(xPtr, x1Val1); xPtr+=4;
    _mm_store_ps(xPtr, x2Val1); xPtr+=4;
    
  }
  for (int i=8*(nof_symbols/8);i<nof_symbols;i++) {
    cf_t r  = 0; 
    cf_t hh = 0; 
    for (int p=0;p<nof_rxant;p++) {
      r  += y[p][i]*conj(h[p][i]);
      hh += conj(h[p][i])*h[p][i];
    }
    x[i] = r/(hh+noise_estimate);
  }
  return nof_symbols;
}

#endif

#ifdef LV_HAVE_AVX

#define PROD_AVX(a,b) _mm256_addsub_ps(_mm256_mul_ps(a,_mm256_moveldup_ps(b)),_mm256_mul_ps(_mm256_shuffle_ps(a,a,0xB1),_mm256_movehdup_ps(b)))



int srslte_predecoding_single_avx(cf_t *y[SRSLTE_MAX_PORTS], cf_t *h[SRSLTE_MAX_PORTS], cf_t *x, int nof_rxant, int nof_symbols, float noise_estimate) {
  
  float *xPtr = (float*) x;
  const float *hPtr1 = (const float*) h[0];
  const float *yPtr1 = (const float*) y[0];
  const float *hPtr2 = (const float*) h[1];
  const float *yPtr2 = (const float*) y[1];

  __m256 conjugator = _mm256_setr_ps(0, -0.f, 0, -0.f, 0, -0.f, 0, -0.f);
  
  __m256 noise = _mm256_set1_ps(noise_estimate);
  __m256 h1Val1, h2Val1, y1Val1, y2Val1, h12square, h1square, h2square, h1_p, h2_p, h1conj1, h2conj1, x1Val, x2Val;
  __m256 h1Val2, h2Val2, y1Val2, y2Val2, h1conj2, h2conj2;

  for (int i=0;i<nof_symbols/8;i++) {
    y1Val1 = _mm256_load_ps(yPtr1); yPtr1+=8;
    y2Val1 = _mm256_load_ps(yPtr1); yPtr1+=8;
    h1Val1 = _mm256_load_ps(hPtr1); hPtr1+=8;
    h2Val1 = _mm256_load_ps(hPtr1); hPtr1+=8;

    if (nof_rxant == 2) {
      y1Val2 = _mm256_load_ps(yPtr2); yPtr2+=8;
      y2Val2 = _mm256_load_ps(yPtr2); yPtr2+=8;
      h1Val2 = _mm256_load_ps(hPtr2); hPtr2+=8;
      h2Val2 = _mm256_load_ps(hPtr2); hPtr2+=8;      
    }
    
    __m256 t1 = _mm256_mul_ps(h1Val1, h1Val1);
    __m256 t2 = _mm256_mul_ps(h2Val1, h2Val1);
    h12square = _mm256_hadd_ps(_mm256_permute2f128_ps(t1, t2, 0x20), _mm256_permute2f128_ps(t1, t2, 0x31)); 

    if (nof_rxant == 2) {
      t1 = _mm256_mul_ps(h1Val2, h1Val2);
      t2 = _mm256_mul_ps(h2Val2, h2Val2);
      h12square = _mm256_add_ps(h12square, _mm256_hadd_ps(_mm256_permute2f128_ps(t1, t2, 0x20), _mm256_permute2f128_ps(t1, t2, 0x31)));   
    }

    if (noise_estimate > 0) {
      h12square  = _mm256_add_ps(h12square, noise);
    }
    
    h1_p     = _mm256_permute_ps(h12square, _MM_SHUFFLE(1, 1, 0, 0));
    h2_p     = _mm256_permute_ps(h12square, _MM_SHUFFLE(3, 3, 2, 2));
    h1square = _mm256_permute2f128_ps(h1_p, h2_p, 2<<4);
    h2square = _mm256_permute2f128_ps(h1_p, h2_p, 3<<4 | 1);
    
    /* Conjugate channel */
    h1conj1 = _mm256_xor_ps(h1Val1, conjugator); 
    h2conj1 = _mm256_xor_ps(h2Val1, conjugator); 

    if (nof_rxant == 2) {
      h1conj2 = _mm256_xor_ps(h1Val2, conjugator); 
      h2conj2 = _mm256_xor_ps(h2Val2, conjugator);       
    }
    
    /* Complex product */      
    x1Val = PROD_AVX(y1Val1, h1conj1);
    x2Val = PROD_AVX(y2Val1, h2conj1);

    if (nof_rxant == 2) {
      x1Val = _mm256_add_ps(x1Val, PROD_AVX(y1Val2, h1conj2));
      x2Val = _mm256_add_ps(x2Val, PROD_AVX(y2Val2, h2conj2));  
    }
    
    x1Val = _mm256_div_ps(x1Val, h1square);
    x2Val = _mm256_div_ps(x2Val, h2square);
    
    _mm256_store_ps(xPtr, x1Val); xPtr+=8;
    _mm256_store_ps(xPtr, x2Val); xPtr+=8;
  }
  for (int i=16*(nof_symbols/16);i<nof_symbols;i++) {
    cf_t r  = 0; 
    cf_t hh = 0; 
    for (int p=0;p<nof_rxant;p++) {
      r  += y[p][i]*conj(h[p][i]);
      hh += conj(h[p][i])*h[p][i];
    }
    x[i] = r/(hh+noise_estimate);
  }
  return nof_symbols;
}

#endif

int srslte_predecoding_single_gen(cf_t *y[SRSLTE_MAX_PORTS], cf_t *h[SRSLTE_MAX_PORTS], cf_t *x, int nof_rxant, int nof_symbols, float noise_estimate) {
  for (int i=0;i<nof_symbols;i++) {
    cf_t r  = 0; 
    cf_t hh = 0; 
    for (int p=0;p<nof_rxant;p++) {
      r  += y[p][i]*conj(h[p][i]);
      hh += conj(h[p][i])*h[p][i];
    }
    x[i] = r/(hh+noise_estimate);
  }
  return nof_symbols;
}

/* ZF/MMSE SISO equalizer x=y(h'h+no)^(-1)h' (ZF if n0=0.0)*/
int srslte_predecoding_single(cf_t *y_, cf_t *h_, cf_t *x, int nof_symbols, float noise_estimate) {
  
  cf_t *y[SRSLTE_MAX_PORTS]; 
  cf_t *h[SRSLTE_MAX_PORTS];
  y[0] = y_;
  h[0] = h_; 
  int nof_rxant = 1; 
  
#ifdef LV_HAVE_AVX
  if (nof_symbols > 32 && nof_rxant <= 2) {
    return srslte_predecoding_single_avx(y, h, x, nof_rxant, nof_symbols, noise_estimate);
  } else {
    return srslte_predecoding_single_gen(y, h, x, nof_rxant, nof_symbols, noise_estimate);
  }
#else
  #ifdef LV_HAVE_SSE
    if (nof_symbols > 32 && nof_rxant <= 2) {
      return srslte_predecoding_single_sse(y, h, x, nof_rxant, nof_symbols, noise_estimate);
    } else {
      return srslte_predecoding_single_gen(y, h, x, nof_rxant, nof_symbols, noise_estimate);      
    }
  #else
    return srslte_predecoding_single_gen(y, h, x, nof_rxant, nof_symbols, noise_estimate);
  #endif
#endif
}

/* ZF/MMSE SISO equalizer x=y(h'h+no)^(-1)h' (ZF if n0=0.0)*/
int srslte_predecoding_single_multi(cf_t *y[SRSLTE_MAX_PORTS], cf_t *h[SRSLTE_MAX_PORTS], cf_t *x, int nof_rxant, int nof_symbols, float noise_estimate) {
#ifdef LV_HAVE_AVX
  if (nof_symbols > 32) {
    return srslte_predecoding_single_avx(y, h, x, nof_rxant, nof_symbols, noise_estimate);
  } else {
    return srslte_predecoding_single_gen(y, h, x, nof_rxant, nof_symbols, noise_estimate);
  }
#else
  #ifdef LV_HAVE_SSE
    if (nof_symbols > 32) {
      return srslte_predecoding_single_sse(y, h, x, nof_rxant, nof_symbols, noise_estimate);
    } else {
      return srslte_predecoding_single_gen(y, h, x, nof_rxant, nof_symbols, noise_estimate);      
    }
  #else
    return srslte_predecoding_single_gen(y, h, x, nof_rxant, nof_symbols, noise_estimate);
  #endif
#endif
}

/* C implementatino of the SFBC equalizer */
int srslte_predecoding_diversity_gen_(cf_t *y[SRSLTE_MAX_PORTS], cf_t *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS], 
                                      cf_t *x[SRSLTE_MAX_LAYERS], 
                                      int nof_rxant, int nof_ports, int nof_symbols, int symbol_start) 
{
  int i;
  if (nof_ports == 2) {
    cf_t h00, h01, h10, h11, r0, r1;

    for (i = symbol_start/2; i < nof_symbols / 2; i++) {
      float hh = 0;
      cf_t x0 = 0; 
      cf_t x1 = 0; 
      for (int p=0;p<nof_rxant;p++) {
        h00 = h[0][p][2 * i];
        h01 = h[0][p][2 * i+1];
        h10 = h[1][p][2 * i];
        h11 = h[1][p][2 * i+1];
        hh += crealf(h00) * crealf(h00) + cimagf(h00) * cimagf(h00)
            + crealf(h11) * crealf(h11) + cimagf(h11) * cimagf(h11);
        r0 = y[p][2 * i];
        r1 = y[p][2 * i + 1];
        if (hh == 0) {
          hh = 1e-4;
        }
        x0 += (conjf(h00) * r0 + h11 * conjf(r1));
        x1 += (-h10 * conj(r0) + conj(h01) * r1);
      }
      x[0][i] = x0 / hh * sqrt(2);
      x[1][i] = x1 / hh * sqrt(2);
    }
    return i;
  } else if (nof_ports == 4) {
    cf_t h0, h1, h2, h3, r0, r1, r2, r3;
    
    int m_ap = (nof_symbols % 4) ? ((nof_symbols - 2) / 4) : nof_symbols / 4;
    for (i = symbol_start; i < m_ap; i++) {
      float hh02 = 0, hh13 = 0;
      cf_t x0 = 0, x1 = 0, x2 = 0, x3 = 0; 
      for (int p=0;p<nof_rxant;p++) {
        h0 = h[0][p][4 * i];
        h1 = h[1][p][4 * i + 2];
        h2 = h[2][p][4 * i];
        h3 = h[3][p][4 * i + 2];
        hh02 += crealf(h0) * crealf(h0) + cimagf(h0) * cimagf(h0)
            + crealf(h2) * crealf(h2) + cimagf(h2) * cimagf(h2);
        hh13 += crealf(h1) * crealf(h1) + cimagf(h1) * cimagf(h1)
            + crealf(h3) * crealf(h3) + cimagf(h3) * cimagf(h3);
        r0 = y[p][4 * i];
        r1 = y[p][4 * i + 1];
        r2 = y[p][4 * i + 2];
        r3 = y[p][4 * i + 3];

        x0 += (conjf(h0) * r0 + h2 * conjf(r1));
        x1 += (-h2 * conjf(r0) + conjf(h0) * r1);
        x2 += (conjf(h1) * r2 + h3 * conjf(r3));
        x3 += (-h3 * conjf(r2) + conjf(h1) * r3);
      }
      x[0][i] = x0 / hh02 * sqrt(2);
      x[1][i] = x1 / hh02 * sqrt(2);
      x[2][i] = x2 / hh13 * sqrt(2);
      x[3][i] = x3 / hh13 * sqrt(2);
    }
    return i;
  } else {
    fprintf(stderr, "Number of ports must be 2 or 4 for transmit diversity (nof_ports=%d)\n", nof_ports);
    return -1;
  }
}

int srslte_predecoding_diversity_gen(cf_t *y[SRSLTE_MAX_PORTS], cf_t *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS], 
                                     cf_t *x[SRSLTE_MAX_LAYERS], 
                                     int nof_rxant, int nof_ports, int nof_symbols) {
  return srslte_predecoding_diversity_gen_(y, h, x, nof_rxant, nof_ports, nof_symbols, 0);
}

/* SSE implementation of the 2-port SFBC equalizer */
#ifdef LV_HAVE_SSE
int srslte_predecoding_diversity2_sse(cf_t *y[SRSLTE_MAX_PORTS], cf_t *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS], 
                                      cf_t *x[SRSLTE_MAX_LAYERS], 
                                      int nof_rxant, int nof_symbols) 
{
  float *x0Ptr = (float*) x[0];
  float *x1Ptr = (float*) x[1];
  const float *h0Ptr0 = (const float*) h[0][0];
  const float *h1Ptr0 = (const float*) h[1][0];
  const float *h0Ptr1 = (const float*) h[0][1];
  const float *h1Ptr1 = (const float*) h[1][1];
  const float *yPtr0 = (const float*) y[0];
  const float *yPtr1 = (const float*) y[1];

  __m128 conjugator = _mm_setr_ps(0, -0.f, 0, -0.f);
  __m128 sqrt2      = _mm_setr_ps(sqrt(2), sqrt(2), sqrt(2), sqrt(2));
  
  __m128 h0Val_00, h0Val_10, h1Val_00, h1Val_10, h000, h00conj0, h010, h01conj0, h100, h110;
  __m128 h0Val_01, h0Val_11, h1Val_01, h1Val_11, h001, h00conj1, h011, h01conj1, h101, h111;
  __m128 hh, hhshuf, hhsum, hhadd; 
  __m128 r0Val0, r1Val0, r00, r10, r0conj0, r1conj0; 
  __m128 r0Val1, r1Val1, r01, r11, r0conj1, r1conj1; 
  __m128 x0, x1; 
  
  for (int i=0;i<nof_symbols/4;i++) {
  
    h0Val_00 = _mm_load_ps(h0Ptr0); h0Ptr0+=4; h0Val_10 = _mm_load_ps(h0Ptr0); h0Ptr0+=4;    
    h1Val_00 = _mm_load_ps(h1Ptr0); h1Ptr0+=4; h1Val_10 = _mm_load_ps(h1Ptr0); h1Ptr0+=4;

    if (nof_rxant == 2) {
      h0Val_01 = _mm_load_ps(h0Ptr1); h0Ptr1+=4; h0Val_11 = _mm_load_ps(h0Ptr1); h0Ptr1+=4;    
      h1Val_01 = _mm_load_ps(h1Ptr1); h1Ptr1+=4; h1Val_11 = _mm_load_ps(h1Ptr1); h1Ptr1+=4;
    }
    
    h000 = _mm_shuffle_ps(h0Val_00, h0Val_10, _MM_SHUFFLE(1, 0, 1, 0));
    h010 = _mm_shuffle_ps(h0Val_00, h0Val_10, _MM_SHUFFLE(3, 2, 3, 2));
    
    h100 = _mm_shuffle_ps(h1Val_00, h1Val_10, _MM_SHUFFLE(1, 0, 1, 0));
    h110 = _mm_shuffle_ps(h1Val_00, h1Val_10, _MM_SHUFFLE(3, 2, 3, 2));

    if (nof_rxant == 2) {
      h001 = _mm_shuffle_ps(h0Val_01, h0Val_11, _MM_SHUFFLE(1, 0, 1, 0));
      h011 = _mm_shuffle_ps(h0Val_01, h0Val_11, _MM_SHUFFLE(3, 2, 3, 2));
      
      h101 = _mm_shuffle_ps(h1Val_01, h1Val_11, _MM_SHUFFLE(1, 0, 1, 0));
      h111 = _mm_shuffle_ps(h1Val_01, h1Val_11, _MM_SHUFFLE(3, 2, 3, 2));      
    }
    
    r0Val0 = _mm_load_ps(yPtr0); yPtr0+=4;
    r1Val0 = _mm_load_ps(yPtr0); yPtr0+=4;
    r00 = _mm_shuffle_ps(r0Val0, r1Val0, _MM_SHUFFLE(1, 0, 1, 0));
    r10 = _mm_shuffle_ps(r0Val0, r1Val0, _MM_SHUFFLE(3, 2, 3, 2));

    if (nof_rxant == 2) {
      r0Val1 = _mm_load_ps(yPtr1); yPtr1+=4;
      r1Val1 = _mm_load_ps(yPtr1); yPtr1+=4;
      r01 = _mm_shuffle_ps(r0Val1, r1Val1, _MM_SHUFFLE(1, 0, 1, 0));
      r11 = _mm_shuffle_ps(r0Val1, r1Val1, _MM_SHUFFLE(3, 2, 3, 2));      
    }
    
    /* Compute channel gain */
    hhadd  = _mm_hadd_ps(_mm_mul_ps(h000, h000), _mm_mul_ps(h110, h110)); 
    hhshuf = _mm_shuffle_ps(hhadd, hhadd, _MM_SHUFFLE(3, 1, 2, 0));
    hhsum  = _mm_hadd_ps(hhshuf, hhshuf);
    hh     = _mm_shuffle_ps(hhsum, hhsum, _MM_SHUFFLE(1, 1, 0, 0)); // h00^2+h11^2 
    
    /* Add channel from 2nd antenna */
    if (nof_rxant == 2) {
      hhadd  = _mm_hadd_ps(_mm_mul_ps(h001, h001), _mm_mul_ps(h111, h111)); 
      hhshuf = _mm_shuffle_ps(hhadd, hhadd, _MM_SHUFFLE(3, 1, 2, 0));
      hhsum  = _mm_hadd_ps(hhshuf, hhshuf);
      hh     = _mm_add_ps(hh, _mm_shuffle_ps(hhsum, hhsum, _MM_SHUFFLE(1, 1, 0, 0))); // h00^2+h11^2       
    }
    
    // Conjugate value 
    h00conj0 = _mm_xor_ps(h000, conjugator);
    h01conj0 = _mm_xor_ps(h010, conjugator); 
    r0conj0  = _mm_xor_ps(r00, conjugator);
    r1conj0  = _mm_xor_ps(r10, conjugator); 
 
    if (nof_rxant == 2) {
      h00conj1 = _mm_xor_ps(h001, conjugator);
      h01conj1 = _mm_xor_ps(h011, conjugator); 
      r0conj1  = _mm_xor_ps(r01, conjugator);
      r1conj1  = _mm_xor_ps(r11, conjugator);       
    }
    
    // Multiply by channel matrix
    x0 = _mm_add_ps(PROD(h00conj0, r00), PROD(h110, r1conj0));
    x1 = _mm_sub_ps(PROD(h01conj0, r10), PROD(h100, r0conj0));

    // Add received symbol from 2nd antenna
    if (nof_rxant == 2) {
      x0 = _mm_add_ps(x0, _mm_add_ps(PROD(h00conj1, r01), PROD(h111, r1conj1)));
      x1 = _mm_add_ps(x1, _mm_sub_ps(PROD(h01conj1, r11), PROD(h101, r0conj1)));                
    }

    x0 = _mm_mul_ps(_mm_div_ps(x0, hh), sqrt2);
    x1 = _mm_mul_ps(_mm_div_ps(x1, hh), sqrt2);

    _mm_store_ps(x0Ptr, x0); x0Ptr+=4;
    _mm_store_ps(x1Ptr, x1); x1Ptr+=4;    
  }
  // Compute remaining symbols using generic implementation
  srslte_predecoding_diversity_gen_(y, h, x, nof_rxant, 2, nof_symbols, 4*(nof_symbols/4));
  return nof_symbols;
}
#endif

int srslte_predecoding_diversity(cf_t *y_, cf_t *h_[SRSLTE_MAX_PORTS], cf_t *x[SRSLTE_MAX_LAYERS], 
                          int nof_ports, int nof_symbols) 
{
  cf_t *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS]; 
  cf_t *y[SRSLTE_MAX_PORTS]; 
  uint32_t nof_rxant = 1; 
  
  for (int i=0;i<nof_ports;i++) {
    h[i][0] = h_[i];
  }
  y[0] = y_; 
  
#ifdef LV_HAVE_SSE
  if (nof_symbols > 32 && nof_ports == 2) {
    return srslte_predecoding_diversity2_sse(y, h, x, nof_rxant, nof_symbols);
  } else {
    return srslte_predecoding_diversity_gen(y, h, x, nof_rxant, nof_ports, nof_symbols);      
  }
#else
  return srslte_predecoding_diversity_gen(y, h, x, nof_rxant, nof_ports, nof_symbols);
#endif   
}

int srslte_predecoding_diversity_multi(cf_t *y[SRSLTE_MAX_PORTS], cf_t *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS], cf_t *x[SRSLTE_MAX_LAYERS], 
                          int nof_rxant, int nof_ports, int nof_symbols) 
{
#ifdef LV_HAVE_SSE
  if (nof_symbols > 32 && nof_ports == 2) {
    return srslte_predecoding_diversity2_sse(y, h, x, nof_rxant, nof_symbols);
  } else {
    return srslte_predecoding_diversity_gen(y, h, x, nof_rxant, nof_ports, nof_symbols);      
  }
#else
  return srslte_predecoding_diversity_gen(y, h, x, nof_rxant, nof_ports, nof_symbols);
#endif   
}


int srslte_predecoding_type(cf_t *y_, cf_t *h_[SRSLTE_MAX_PORTS], cf_t *x[SRSLTE_MAX_LAYERS],
    int nof_ports, int nof_layers, int nof_symbols, srslte_mimo_type_t type, float noise_estimate) 
{
  cf_t *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS]; 
  cf_t *y[SRSLTE_MAX_PORTS]; 
  uint32_t nof_rxant = 1; 
  
  for (int i=0;i<nof_ports;i++) {
    h[i][0] = h_[i];
  }
  y[0] = y_; 
  return srslte_predecoding_type_multi(y, h, x, nof_rxant, nof_ports, nof_layers, nof_symbols, type, noise_estimate);  
}

/* 36.211 v10.3.0 Section 6.3.4 */
int srslte_predecoding_type_multi(cf_t *y[SRSLTE_MAX_PORTS], cf_t *h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS], cf_t *x[SRSLTE_MAX_LAYERS],
    int nof_rxant, int nof_ports, int nof_layers, int nof_symbols, srslte_mimo_type_t type, float noise_estimate) {

  if (nof_ports > SRSLTE_MAX_PORTS) {
    fprintf(stderr, "Maximum number of ports is %d (nof_ports=%d)\n", SRSLTE_MAX_PORTS,
        nof_ports);
    return -1;
  }
  if (nof_layers > SRSLTE_MAX_LAYERS) {
    fprintf(stderr, "Maximum number of layers is %d (nof_layers=%d)\n",
        SRSLTE_MAX_LAYERS, nof_layers);
    return -1;
  }

  switch (type) {
  case SRSLTE_MIMO_TYPE_CDD:
    fprintf(stderr, "CCD not supported\n");
    return -1; 
  case SRSLTE_MIMO_TYPE_SINGLE_ANTENNA:
    if (nof_ports == 1 && nof_layers == 1) {
      return srslte_predecoding_single_multi(y, h[0], x[0], nof_rxant, nof_symbols, noise_estimate);              
    } else {
      fprintf(stderr,
          "Number of ports and layers must be 1 for transmission on single antenna ports\n");
      return -1;
    }
    break;
  case SRSLTE_MIMO_TYPE_TX_DIVERSITY:
    if (nof_ports == nof_layers) {
      return srslte_predecoding_diversity_multi(y, h, x, nof_rxant, nof_ports, nof_symbols);
    } else {
      fprintf(stderr,
          "Error number of layers must equal number of ports in transmit diversity\n");
      return -1;
    }
    break;
  case SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX:
    fprintf(stderr, "Spatial multiplexing not supported\n");
    return -1;
  }
  return 0;
}






/************************************************
 * 
 * TRANSMITTER SIDE FUNCTIONS
 * 
 **************************************************/

int srslte_precoding_single(cf_t *x, cf_t *y, int nof_symbols) {
  memcpy(y, x, nof_symbols * sizeof(cf_t));
  return nof_symbols;
}
int srslte_precoding_diversity(cf_t *x[SRSLTE_MAX_LAYERS], cf_t *y[SRSLTE_MAX_PORTS], int nof_ports,
    int nof_symbols) {
  int i;
  if (nof_ports == 2) {
    for (i = 0; i < nof_symbols; i++) {
      y[0][2 * i] = x[0][i];
      y[1][2 * i] = -conjf(x[1][i]);
      y[0][2 * i + 1] = x[1][i];
      y[1][2 * i + 1] = conjf(x[0][i]);
    }
    // normalize
    srslte_vec_sc_prod_cfc(y[0], 1.0/sqrtf(2), y[0], 2*nof_symbols);
    srslte_vec_sc_prod_cfc(y[1], 1.0/sqrtf(2), y[1], 2*nof_symbols);
    return 2 * i;
  } else if (nof_ports == 4) {
    //int m_ap = (nof_symbols%4)?(nof_symbols*4-2):nof_symbols*4;
    int m_ap = 4 * nof_symbols;
    for (i = 0; i < m_ap / 4; i++) {
      y[0][4 * i] = x[0][i] / sqrtf(2);
      y[1][4 * i] = 0;
      y[2][4 * i] = -conjf(x[1][i]) / sqrtf(2);
      y[3][4 * i] = 0;

      y[0][4 * i + 1] = x[1][i] / sqrtf(2);
      y[1][4 * i + 1] = 0;
      y[2][4 * i + 1] = conjf(x[0][i]) / sqrtf(2);
      y[3][4 * i + 1] = 0;

      y[0][4 * i + 2] = 0;
      y[1][4 * i + 2] = x[2][i] / sqrtf(2);
      y[2][4 * i + 2] = 0;
      y[3][4 * i + 2] = -conjf(x[3][i]) / sqrtf(2);

      y[0][4 * i + 3] = 0;
      y[1][4 * i + 3] = x[3][i] / sqrtf(2);
      y[2][4 * i + 3] = 0;
      y[3][4 * i + 3] = conjf(x[2][i]) / sqrtf(2);
    }
    return 4 * i;
  } else {
    fprintf(stderr, "Number of ports must be 2 or 4 for transmit diversity (nof_ports=%d)\n", nof_ports);
    return -1;
  }
}

int srslte_precoding_cdd(cf_t *x[SRSLTE_MAX_LAYERS], cf_t *y[SRSLTE_MAX_PORTS], int nof_layers, int nof_ports, int nof_symbols) 
{
  int i;
  if (nof_ports == 2) {
    if (nof_layers != 2) {
      fprintf(stderr, "Invalid number of layers %d for 2 ports\n", nof_layers);
      return -1; 
    }
    for (i = 0; i < nof_symbols; i++) {
      y[0][i] =  (x[0][i]+x[1][i])/2;
      y[1][i] =  (x[0][i]-x[1][i])/2;
      i++;
      y[0][i] =  (x[0][i]+x[1][i])/2;
      y[1][i] = (-x[0][i]+x[1][i])/2;
    }
    return 2 * i;
  } else if (nof_ports == 4) {
    fprintf(stderr, "Not implemented\n");
    return -1;
  } else {
    fprintf(stderr, "Number of ports must be 2 or 4 for transmit diversity (nof_ports=%d)\n", nof_ports);
    return -1;
  }
}

/* 36.211 v10.3.0 Section 6.3.4 */
int srslte_precoding_type(cf_t *x[SRSLTE_MAX_LAYERS], cf_t *y[SRSLTE_MAX_PORTS], int nof_layers,
    int nof_ports, int nof_symbols, srslte_mimo_type_t type) {

  if (nof_ports > SRSLTE_MAX_PORTS) {
    fprintf(stderr, "Maximum number of ports is %d (nof_ports=%d)\n", SRSLTE_MAX_PORTS,
        nof_ports);
    return -1;
  }
  if (nof_layers > SRSLTE_MAX_LAYERS) {
    fprintf(stderr, "Maximum number of layers is %d (nof_layers=%d)\n",
        SRSLTE_MAX_LAYERS, nof_layers);
    return -1;
  }

  switch (type) {
  case SRSLTE_MIMO_TYPE_CDD:
    return srslte_precoding_cdd(x, y, nof_layers, nof_ports, nof_symbols); 
  case SRSLTE_MIMO_TYPE_SINGLE_ANTENNA:
    if (nof_ports == 1 && nof_layers == 1) {
      return srslte_precoding_single(x[0], y[0], nof_symbols);
    } else {
      fprintf(stderr,
          "Number of ports and layers must be 1 for transmission on single antenna ports\n");
      return -1;
    }
    break;
  case SRSLTE_MIMO_TYPE_TX_DIVERSITY:
    if (nof_ports == nof_layers) {
      return srslte_precoding_diversity(x, y, nof_ports, nof_symbols);
    } else {
      fprintf(stderr,
          "Error number of layers must equal number of ports in transmit diversity\n");
      return -1;
    }
  case SRSLTE_MIMO_TYPE_SPATIAL_MULTIPLEX:
    fprintf(stderr, "Spatial multiplexing not supported\n");
    return -1;
  }
  return 0;
}

