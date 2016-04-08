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

#include "srslte/common/phy_common.h"
#include "srslte/mimo/precoding.h"
#include "srslte/utils/vector.h"

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
#include <pmmintrin.h>
int srslte_predecoding_single_sse(cf_t *y, cf_t *h, cf_t *x, int nof_symbols, float noise_estimate);
int srslte_predecoding_diversity2_sse(cf_t *y, cf_t *h[SRSLTE_MAX_PORTS], cf_t *x[SRSLTE_MAX_LAYERS], int nof_symbols);
#endif

#ifdef LV_HAVE_AVX
#include <immintrin.h>
int srslte_predecoding_single_avx(cf_t *y, cf_t *h, cf_t *x, int nof_symbols, float noise_estimate);
#endif



/************************************************
 * 
 * RECEIVER SIDE FUNCTIONS
 * 
 **************************************************/

#ifdef LV_HAVE_SSE

#define PROD(a,b) _mm_addsub_ps(_mm_mul_ps(a,_mm_moveldup_ps(b)),_mm_mul_ps(_mm_shuffle_ps(a,a,0xB1),_mm_movehdup_ps(b)))

int srslte_predecoding_single_sse(cf_t *y, cf_t *h, cf_t *x, int nof_symbols, float noise_estimate) {
  
  float *xPtr = (float*) x;
  const float *hPtr = (const float*) h;
  const float *yPtr = (const float*) y;

  __m128 conjugator = _mm_setr_ps(0, -0.f, 0, -0.f);
  
  __m128 noise = _mm_set1_ps(noise_estimate);
  __m128 h1Val, h2Val, y1Val, y2Val, h12square, h1square, h2square, h1conj, h2conj, x1Val, x2Val;
  for (int i=0;i<nof_symbols/4;i++) {
    y1Val = _mm_load_ps(yPtr); yPtr+=4;
    y2Val = _mm_load_ps(yPtr); yPtr+=4;
    h1Val = _mm_load_ps(hPtr); hPtr+=4;
    h2Val = _mm_load_ps(hPtr); hPtr+=4;
    
    h12square = _mm_hadd_ps(_mm_mul_ps(h1Val, h1Val), _mm_mul_ps(h2Val, h2Val)); 
    if (noise_estimate > 0) {
      h12square  = _mm_add_ps(h12square, noise);
    }
    
    h1square  = _mm_shuffle_ps(h12square, h12square, _MM_SHUFFLE(1, 1, 0, 0));
    h2square  = _mm_shuffle_ps(h12square, h12square, _MM_SHUFFLE(3, 3, 2, 2));
    
    /* Conjugate channel */
    h1conj = _mm_xor_ps(h1Val, conjugator); 
    h2conj = _mm_xor_ps(h2Val, conjugator); 

    /* Complex product */      
    x1Val = PROD(y1Val, h1conj);
    x2Val = PROD(y2Val, h2conj);

    x1Val = _mm_div_ps(x1Val, h1square);
    x2Val = _mm_div_ps(x2Val, h2square);
    
    _mm_store_ps(xPtr, x1Val); xPtr+=4;
    _mm_store_ps(xPtr, x2Val); xPtr+=4;
  }
  for (int i=8*(nof_symbols/8);i<nof_symbols;i++) {
    x[i] = y[i]*conj(h[i])/(conj(h[i])*h[i]+noise_estimate);
  }
  return nof_symbols;
}

#endif

#ifdef LV_HAVE_AVX

#define PROD_AVX(a,b) _mm256_addsub_ps(_mm256_mul_ps(a,_mm256_moveldup_ps(b)),_mm256_mul_ps(_mm256_shuffle_ps(a,a,0xB1),_mm256_movehdup_ps(b)))



int srslte_predecoding_single_avx(cf_t *y, cf_t *h, cf_t *x, int nof_symbols, float noise_estimate) {
  
  float *xPtr = (float*) x;
  const float *hPtr = (const float*) h;
  const float *yPtr = (const float*) y;

  __m256 conjugator = _mm256_setr_ps(0, -0.f, 0, -0.f, 0, -0.f, 0, -0.f);
  
  __m256 noise = _mm256_set1_ps(noise_estimate);
  __m256 h1Val, h2Val, y1Val, y2Val, h12square, h1square, h2square, h1_p, h2_p, h1conj, h2conj, x1Val, x2Val;
  
  for (int i=0;i<nof_symbols/8;i++) {
    y1Val = _mm256_load_ps(yPtr); yPtr+=8;
    y2Val = _mm256_load_ps(yPtr); yPtr+=8;
    h1Val = _mm256_load_ps(hPtr); hPtr+=8;
    h2Val = _mm256_load_ps(hPtr); hPtr+=8;
    
    __m256 t1 = _mm256_mul_ps(h1Val, h1Val);
    __m256 t2 = _mm256_mul_ps(h2Val, h2Val);
    h12square = _mm256_hadd_ps(_mm256_permute2f128_ps(t1, t2, 0x20), _mm256_permute2f128_ps(t1, t2, 0x31)); 
    if (noise_estimate > 0) {
      h12square  = _mm256_add_ps(h12square, noise);
    }
    h1_p     = _mm256_permute_ps(h12square, _MM_SHUFFLE(1, 1, 0, 0));
    h2_p     = _mm256_permute_ps(h12square, _MM_SHUFFLE(3, 3, 2, 2));
    h1square = _mm256_permute2f128_ps(h1_p, h2_p, 2<<4);
    h2square = _mm256_permute2f128_ps(h1_p, h2_p, 3<<4 | 1);
    
    /* Conjugate channel */
    h1conj = _mm256_xor_ps(h1Val, conjugator); 
    h2conj = _mm256_xor_ps(h2Val, conjugator); 

    /* Complex product */      
    x1Val = PROD_AVX(y1Val, h1conj);
    x2Val = PROD_AVX(y2Val, h2conj);

    x1Val = _mm256_div_ps(x1Val, h1square);
    x2Val = _mm256_div_ps(x2Val, h2square);
    
    _mm256_store_ps(xPtr, x1Val); xPtr+=8;
    _mm256_store_ps(xPtr, x2Val); xPtr+=8;
  }
  for (int i=16*(nof_symbols/16);i<nof_symbols;i++) {
    x[i] = y[i]*conj(h[i])/(conj(h[i])*h[i]+noise_estimate);
  }
  return nof_symbols;
}

#endif

int srslte_predecoding_single_gen(cf_t *y, cf_t *h, cf_t *x, int nof_symbols, float noise_estimate) {
  for (int i=0;i<nof_symbols;i++) {
    x[i] = y[i]*conj(h[i])/(conj(h[i])*h[i]+noise_estimate);
  }
  return nof_symbols;
}

/* ZF/MMSE SISO equalizer x=y(h'h+no)^(-1)h' (ZF if n0=0.0)*/
int srslte_predecoding_single(cf_t *y, cf_t *h, cf_t *x, int nof_symbols, float noise_estimate) {
#ifdef LV_HAVE_AVX
  if (nof_symbols > 32) {
    return srslte_predecoding_single_avx(y, h, x, nof_symbols, noise_estimate);
  } else {
    return srslte_predecoding_single_gen(y, h, x, nof_symbols, noise_estimate);
  }
#else
  #ifdef LV_HAVE_SSE
    if (nof_symbols > 32) {
      return srslte_predecoding_single_sse(y, h, x, nof_symbols, noise_estimate);
    } else {
      return srslte_predecoding_single_gen(y, h, x, nof_symbols, noise_estimate);      
    }
  #else
    return srslte_predecoding_single_gen(y, h, x, nof_symbols, noise_estimate);
  #endif
#endif
}

/* C implementatino of the SFBC equalizer */
int srslte_predecoding_diversity_gen_(cf_t *y, cf_t *h[SRSLTE_MAX_PORTS], cf_t *x[SRSLTE_MAX_LAYERS], 
                          int nof_ports, int nof_symbols, int symbol_start) 
{
  int i;
  if (nof_ports == 2) {
    cf_t h00, h01, h10, h11, r0, r1;
    float hh;

    for (i = symbol_start/2; i < nof_symbols / 2; i++) {
      h00 = h[0][2 * i];
      h01 = h[0][2 * i+1];
      h10 = h[1][2 * i];
      h11 = h[1][2 * i+1];
      hh = crealf(h00) * crealf(h00) + cimagf(h00) * cimagf(h00)
          + crealf(h11) * crealf(h11) + cimagf(h11) * cimagf(h11);
      r0 = y[2 * i];
      r1 = y[2 * i + 1];
      if (hh == 0) {
        hh = 1e-4;
      }
      x[0][i] = (conjf(h00) * r0 + h11 * conjf(r1)) / hh * sqrt(2);
      x[1][i] = (-h10 * conj(r0) + conj(h01) * r1) / hh * sqrt(2);
    }
    return i;
  } else if (nof_ports == 4) {
    cf_t h0, h1, h2, h3, r0, r1, r2, r3;
    float hh02, hh13;

    int m_ap = (nof_symbols % 4) ? ((nof_symbols - 2) / 4) : nof_symbols / 4;
    for (i = symbol_start; i < m_ap; i++) {
      h0 = h[0][4 * i];
      h1 = h[1][4 * i + 2];
      h2 = h[2][4 * i];
      h3 = h[3][4 * i + 2];
      hh02 = crealf(h0) * crealf(h0) + cimagf(h0) * cimagf(h0)
          + crealf(h2) * crealf(h2) + cimagf(h2) * cimagf(h2);
      hh13 = crealf(h1) * crealf(h1) + cimagf(h1) * cimagf(h1)
          + crealf(h3) * crealf(h3) + cimagf(h3) * cimagf(h3);
      r0 = y[4 * i];
      r1 = y[4 * i + 1];
      r2 = y[4 * i + 2];
      r3 = y[4 * i + 3];

      x[0][i] = (conjf(h0) * r0 + h2 * conjf(r1)) / hh02 * sqrt(2);
      x[1][i] = (-h2 * conjf(r0) + conjf(h0) * r1) / hh02 * sqrt(2);
      x[2][i] = (conjf(h1) * r2 + h3 * conjf(r3)) / hh13 * sqrt(2);
      x[3][i] = (-h3 * conjf(r2) + conjf(h1) * r3) / hh13 * sqrt(2);

    }
    return i;
  } else {
    fprintf(stderr, "Number of ports must be 2 or 4 for transmit diversity (nof_ports=%d)\n", nof_ports);
    return -1;
  }
}

int srslte_predecoding_diversity_gen(cf_t *y, cf_t *h[SRSLTE_MAX_PORTS], cf_t *x[SRSLTE_MAX_LAYERS], 
                          int nof_ports, int nof_symbols) {
  return srslte_predecoding_diversity_gen_(y, h, x, nof_ports, nof_symbols, 0);
}

/* SSE implementation of the 2-port SFBC equalizer */
#ifdef LV_HAVE_SSE
int srslte_predecoding_diversity2_sse(cf_t *y, cf_t *h[SRSLTE_MAX_PORTS], cf_t *x[SRSLTE_MAX_LAYERS], int nof_symbols) 
{
  float *x0Ptr = (float*) x[0];
  float *x1Ptr = (float*) x[1];
  const float *h0Ptr = (const float*) h[0];
  const float *h1Ptr = (const float*) h[1];
  const float *yPtr = (const float*) y;

  __m128 conjugator = _mm_setr_ps(0, -0.f, 0, -0.f);
  __m128 sqrt2      = _mm_setr_ps(sqrt(2), sqrt(2), sqrt(2), sqrt(2));
  
  __m128 h0Val_0, h0Val_1, h1Val_0, h1Val_1, h00, h00conj, h01, h01conj, h10, h11, hh, hhshuf, hhsum, hhadd; 
  __m128 r0Val, r1Val, r0, r1, r0conj, r1conj; 
  __m128 x0, x1; 
  
  for (int i=0;i<nof_symbols/4;i++) {
  
    h0Val_0 = _mm_load_ps(h0Ptr); h0Ptr+=4; h0Val_1 = _mm_load_ps(h0Ptr); h0Ptr+=4;    
    h1Val_0 = _mm_load_ps(h1Ptr); h1Ptr+=4; h1Val_1 = _mm_load_ps(h1Ptr); h1Ptr+=4;

    h00 = _mm_shuffle_ps(h0Val_0, h0Val_1, _MM_SHUFFLE(1, 0, 1, 0));
    h01 = _mm_shuffle_ps(h0Val_0, h0Val_1, _MM_SHUFFLE(3, 2, 3, 2));
    
    h10 = _mm_shuffle_ps(h1Val_0, h1Val_1, _MM_SHUFFLE(1, 0, 1, 0));
    h11 = _mm_shuffle_ps(h1Val_0, h1Val_1, _MM_SHUFFLE(3, 2, 3, 2));

    r0Val = _mm_load_ps(yPtr); yPtr+=4;
    r1Val = _mm_load_ps(yPtr); yPtr+=4;
    r0 = _mm_shuffle_ps(r0Val, r1Val, _MM_SHUFFLE(1, 0, 1, 0));
    r1 = _mm_shuffle_ps(r0Val, r1Val, _MM_SHUFFLE(3, 2, 3, 2));

    /* Compute channel gain */
    hhadd  = _mm_hadd_ps(_mm_mul_ps(h00, h00), _mm_mul_ps(h11, h11)); 
    hhshuf = _mm_shuffle_ps(hhadd, hhadd, _MM_SHUFFLE(3, 1, 2, 0));
    hhsum  = _mm_hadd_ps(hhshuf, hhshuf);
    hh     = _mm_shuffle_ps(hhsum, hhsum, _MM_SHUFFLE(1, 1, 0, 0)); // h00^2+h11^2 
    
    // Conjugate value 
    h00conj = _mm_xor_ps(h00, conjugator);
    h01conj = _mm_xor_ps(h01, conjugator); 
    r0conj  = _mm_xor_ps(r0, conjugator);
    r1conj  = _mm_xor_ps(r1, conjugator); 
    
    // Multiply by channel matrix
    x0 = _mm_add_ps(PROD(h00conj, r0), PROD(h11, r1conj));
    x1 = _mm_sub_ps(PROD(h01conj, r1), PROD(h10, r0conj));
        
    x0 = _mm_mul_ps(_mm_div_ps(x0, hh), sqrt2);
    x1 = _mm_mul_ps(_mm_div_ps(x1, hh), sqrt2);
    
    _mm_store_ps(x0Ptr, x0); x0Ptr+=4;
    _mm_store_ps(x1Ptr, x1); x1Ptr+=4;    
  }
  // Compute remaining symbols using generic implementation
  srslte_predecoding_diversity_gen_(y, h, x, 2, nof_symbols, 4*(nof_symbols/4));
  return nof_symbols;
}
#endif

int srslte_predecoding_diversity(cf_t *y, cf_t *h[SRSLTE_MAX_PORTS], cf_t *x[SRSLTE_MAX_LAYERS], 
                          int nof_ports, int nof_symbols) 
{
#ifdef LV_HAVE_SSE
  if (nof_symbols > 32 && nof_ports == 2) {
    return srslte_predecoding_diversity2_sse(y, h, x, nof_symbols);
  } else {
    return srslte_predecoding_diversity_gen(y, h, x, nof_ports, nof_symbols);      
  }
#else
  return srslte_predecoding_diversity_gen(y, h, x, nof_ports, nof_symbols);
#endif   
}

/* 36.211 v10.3.0 Section 6.3.4 */
int srslte_predecoding_type(cf_t *y, cf_t *h[SRSLTE_MAX_PORTS], cf_t *x[SRSLTE_MAX_LAYERS],
    int nof_ports, int nof_layers, int nof_symbols, srslte_mimo_type_t type, float noise_estimate) {

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
  case SRSLTE_MIMO_TYPE_SINGLE_ANTENNA:
    if (nof_ports == 1 && nof_layers == 1) {
      return srslte_predecoding_single(y, h[0], x[0], nof_symbols, noise_estimate);              
    } else {
      fprintf(stderr,
          "Number of ports and layers must be 1 for transmission on single antenna ports\n");
      return -1;
    }
    break;
  case SRSLTE_MIMO_TYPE_TX_DIVERSITY:
    if (nof_ports == nof_layers) {
      return srslte_predecoding_diversity(y, h, x, nof_ports, nof_symbols);
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

