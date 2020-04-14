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

#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/mimo/precoding.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/mat.h"
#include "srslte/phy/utils/simd.h"
#include "srslte/phy/utils/vector.h"

#ifdef LV_HAVE_SSE
#include <immintrin.h>
int srslte_predecoding_single_sse(cf_t* y[SRSLTE_MAX_PORTS],
                                  cf_t* h[SRSLTE_MAX_PORTS],
                                  cf_t* x,
                                  int   nof_rxant,
                                  int   nof_symbols,
                                  float scaling,
                                  float noise_estimate);
int srslte_predecoding_diversity2_sse(cf_t* y[SRSLTE_MAX_PORTS],
                                      cf_t* h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                      cf_t* x[SRSLTE_MAX_LAYERS],
                                      int   nof_rxant,
                                      int   nof_symbols,
                                      float scaling);
#endif

#ifdef LV_HAVE_AVX
#include <immintrin.h>
int srslte_predecoding_single_avx(cf_t* y[SRSLTE_MAX_PORTS],
                                  cf_t* h[SRSLTE_MAX_PORTS],
                                  cf_t* x,
                                  int   nof_rxant,
                                  int   nof_symbols,
                                  float scaling,
                                  float noise_estimate);
#endif
#include "srslte/phy/utils/mat.h"

static srslte_mimo_decoder_t mimo_decoder = SRSLTE_MIMO_DECODER_MMSE;

/************************************************
 *
 * RECEIVER SIDE FUNCTIONS
 *
 **************************************************/

#ifdef LV_HAVE_SSE

#define PROD(a, b)                                                                                                     \
  _mm_addsub_ps(_mm_mul_ps(a, _mm_moveldup_ps(b)), _mm_mul_ps(_mm_shuffle_ps(a, a, 0xB1), _mm_movehdup_ps(b)))

int srslte_predecoding_single_sse(cf_t* y[SRSLTE_MAX_PORTS],
                                  cf_t* h[SRSLTE_MAX_PORTS],
                                  cf_t* x,
                                  int   nof_rxant,
                                  int   nof_symbols,
                                  float scaling,
                                  float noise_estimate)
{

  float*       xPtr  = (float*)x;
  const float* hPtr1 = (const float*)h[0];
  const float* yPtr1 = (const float*)y[0];
  const float* hPtr2 = (const float*)h[1];
  const float* yPtr2 = (const float*)y[1];

  __m128 conjugator = _mm_setr_ps(0, -0.f, 0, -0.f);

  __m128 noise = _mm_set1_ps(noise_estimate);
  __m128 h1Val1, h2Val1, y1Val1, y2Val1;
  __m128 h1Val2, h2Val2, y1Val2, y2Val2;
  __m128 hsquare, h1square, h2square, h1conj1, h2conj1, x1Val1, x2Val1;
  __m128 hsquare2, h1conj2, h2conj2, x1Val2, x2Val2;

  for (int i = 0; i < nof_symbols / 4; i++) {
    y1Val1 = _mm_load_ps(yPtr1);
    yPtr1 += 4;
    y2Val1 = _mm_load_ps(yPtr1);
    yPtr1 += 4;
    h1Val1 = _mm_load_ps(hPtr1);
    hPtr1 += 4;
    h2Val1 = _mm_load_ps(hPtr1);
    hPtr1 += 4;

    if (nof_rxant == 2) {
      y1Val2 = _mm_load_ps(yPtr2);
      yPtr2 += 4;
      y2Val2 = _mm_load_ps(yPtr2);
      yPtr2 += 4;
      h1Val2 = _mm_load_ps(hPtr2);
      hPtr2 += 4;
      h2Val2 = _mm_load_ps(hPtr2);
      hPtr2 += 4;
    }

    hsquare = _mm_hadd_ps(_mm_mul_ps(h1Val1, h1Val1), _mm_mul_ps(h2Val1, h2Val1));
    if (nof_rxant == 2) {
      hsquare2 = _mm_hadd_ps(_mm_mul_ps(h1Val2, h1Val2), _mm_mul_ps(h2Val2, h2Val2));
      hsquare  = _mm_add_ps(hsquare, hsquare2);
    }
    if (noise_estimate > 0) {
      hsquare = _mm_add_ps(hsquare, noise);
    }

    h1square = _mm_shuffle_ps(hsquare, hsquare, _MM_SHUFFLE(1, 1, 0, 0));
    h2square = _mm_shuffle_ps(hsquare, hsquare, _MM_SHUFFLE(3, 3, 2, 2));

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

    x1Val1 = _mm_mul_ps(x1Val1, _mm_set1_ps(1 / scaling));
    x2Val1 = _mm_mul_ps(x2Val1, _mm_set1_ps(1 / scaling));

    _mm_store_ps(xPtr, x1Val1);
    xPtr += 4;
    _mm_store_ps(xPtr, x2Val1);
    xPtr += 4;
  }
  for (int i = 8 * (nof_symbols / 8); i < nof_symbols; i++) {
    cf_t r  = 0;
    cf_t hh = 0;
    for (int p = 0; p < nof_rxant; p++) {
      r += y[p][i] * conjf(h[p][i]);
      hh += conjf(h[p][i]) * h[p][i];
    }
    x[i] = scaling * r / (hh + noise_estimate);
  }
  return nof_symbols;
}

#endif

#ifdef LV_HAVE_AVX

#define PROD_AVX(a, b)                                                                                                 \
  _mm256_addsub_ps(_mm256_mul_ps(a, _mm256_moveldup_ps(b)),                                                            \
                   _mm256_mul_ps(_mm256_shuffle_ps(a, a, 0xB1), _mm256_movehdup_ps(b)))

int srslte_predecoding_single_avx(cf_t* y[SRSLTE_MAX_PORTS],
                                  cf_t* h[SRSLTE_MAX_PORTS],
                                  cf_t* x,
                                  int   nof_rxant,
                                  int   nof_symbols,
                                  float scaling,
                                  float noise_estimate)
{

  float*       xPtr  = (float*)x;
  const float* hPtr1 = (const float*)h[0];
  const float* yPtr1 = (const float*)y[0];
  const float* hPtr2 = (const float*)h[1];
  const float* yPtr2 = (const float*)y[1];

  __m256 conjugator = _mm256_setr_ps(0, -0.f, 0, -0.f, 0, -0.f, 0, -0.f);

  __m256 noise = _mm256_set1_ps(noise_estimate);
  __m256 h1Val1, h2Val1, y1Val1, y2Val1, h12square, h1square, h2square, h1_p, h2_p, h1conj1, h2conj1, x1Val, x2Val;
  __m256 h1Val2, h2Val2, y1Val2, y2Val2, h1conj2, h2conj2;
  __m256 avx_scaling = _mm256_set1_ps(1 / scaling);

  for (int i = 0; i < nof_symbols / 8; i++) {
    y1Val1 = _mm256_load_ps(yPtr1);
    yPtr1 += 8;
    y2Val1 = _mm256_load_ps(yPtr1);
    yPtr1 += 8;
    h1Val1 = _mm256_load_ps(hPtr1);
    hPtr1 += 8;
    h2Val1 = _mm256_load_ps(hPtr1);
    hPtr1 += 8;

    if (nof_rxant == 2) {
      y1Val2 = _mm256_load_ps(yPtr2);
      yPtr2 += 8;
      y2Val2 = _mm256_load_ps(yPtr2);
      yPtr2 += 8;
      h1Val2 = _mm256_load_ps(hPtr2);
      hPtr2 += 8;
      h2Val2 = _mm256_load_ps(hPtr2);
      hPtr2 += 8;
    }

    __m256 t1 = _mm256_mul_ps(h1Val1, h1Val1);
    __m256 t2 = _mm256_mul_ps(h2Val1, h2Val1);
    h12square = _mm256_hadd_ps(_mm256_permute2f128_ps(t1, t2, 0x20), _mm256_permute2f128_ps(t1, t2, 0x31));

    if (nof_rxant == 2) {
      t1        = _mm256_mul_ps(h1Val2, h1Val2);
      t2        = _mm256_mul_ps(h2Val2, h2Val2);
      h12square = _mm256_add_ps(
          h12square, _mm256_hadd_ps(_mm256_permute2f128_ps(t1, t2, 0x20), _mm256_permute2f128_ps(t1, t2, 0x31)));
    }

    if (noise_estimate > 0) {
      h12square = _mm256_add_ps(h12square, noise);
    }

    h1_p     = _mm256_permute_ps(h12square, _MM_SHUFFLE(1, 1, 0, 0));
    h2_p     = _mm256_permute_ps(h12square, _MM_SHUFFLE(3, 3, 2, 2));
    h1square = _mm256_permute2f128_ps(h1_p, h2_p, 2 << 4);
    h2square = _mm256_permute2f128_ps(h1_p, h2_p, 3 << 4 | 1);

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

    x1Val = _mm256_mul_ps(x1Val, avx_scaling);
    x2Val = _mm256_mul_ps(x2Val, avx_scaling);

    _mm256_store_ps(xPtr, x1Val);
    xPtr += 8;
    _mm256_store_ps(xPtr, x2Val);
    xPtr += 8;
  }
  for (int i = 16 * (nof_symbols / 16); i < nof_symbols; i++) {
    cf_t r  = 0;
    cf_t hh = 0;
    for (int p = 0; p < nof_rxant; p++) {
      r += y[p][i] * conjf(h[p][i]);
      hh += conjf(h[p][i]) * h[p][i];
    }
    x[i] = r / ((hh + noise_estimate) * scaling);
  }
  return nof_symbols;
}

#endif

int srslte_predecoding_single_gen(cf_t* y[SRSLTE_MAX_PORTS],
                                  cf_t* h[SRSLTE_MAX_PORTS],
                                  cf_t* x,
                                  int   nof_rxant,
                                  int   nof_symbols,
                                  float scaling,
                                  float noise_estimate)
{
  for (int i = 0; i < nof_symbols; i++) {
    cf_t r  = 0;
    cf_t hh = 0;
    for (int p = 0; p < nof_rxant; p++) {
      r += y[p][i] * conjf(h[p][i]);
      hh += conjf(h[p][i]) * h[p][i];
    }
    x[i] = r / ((hh + noise_estimate) * scaling);
  }
  return nof_symbols;
}

int srslte_predecoding_single_csi(cf_t*  y[SRSLTE_MAX_PORTS],
                                  cf_t*  h[SRSLTE_MAX_PORTS],
                                  cf_t*  x,
                                  float* csi,
                                  int    nof_rxant,
                                  int    nof_symbols,
                                  float  scaling,
                                  float  noise_estimate)
{
  int i = 0;

#if SRSLTE_SIMD_CF_SIZE
  const simd_f_t _noise   = srslte_simd_f_set1(noise_estimate);
  const simd_f_t _scaling = srslte_simd_f_set1(1.0f / scaling);

  for (; i < nof_symbols - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
    simd_cf_t _r  = srslte_simd_cf_zero();
    simd_f_t  _hh = srslte_simd_f_zero();

    for (int p = 0; p < nof_rxant; p++) {
      simd_cf_t _y = srslte_simd_cfi_load(&y[p][i]);
      simd_cf_t _h = srslte_simd_cfi_load(&h[p][i]);

      _r  = srslte_simd_cf_add(_r, srslte_simd_cf_conjprod(_y, _h));
      _hh = srslte_simd_f_add(_hh, srslte_simd_cf_re(srslte_simd_cf_conjprod(_h, _h)));
    }

    simd_f_t  _csi = srslte_simd_f_add(_hh, _noise);
    simd_cf_t _x   = srslte_simd_cf_mul(srslte_simd_cf_mul(_r, _scaling), srslte_simd_f_rcp(_csi));

    srslte_simd_f_store(&csi[i], _csi);
    srslte_simd_cfi_store(&x[i], _x);
  }
#endif

  for (; i < nof_symbols; i++) {
    cf_t  r    = 0;
    float hh   = 0;
    float norm = 1.0f / scaling;
    for (int p = 0; p < nof_rxant; p++) {
      r += y[p][i] * conjf(h[p][i]);
      hh += (__real__ h[p][i] * __real__ h[p][i]) + (__imag__ h[p][i] * __imag__ h[p][i]);
    }
    csi[i] = hh + noise_estimate;
    x[i]   = r * norm / csi[i];
  }
  return nof_symbols;
}

/* ZF/MMSE SISO equalizer x=y(h'h+no)^(-1)h' (ZF if n0=0.0)*/
int srslte_predecoding_single(cf_t*  y_,
                              cf_t*  h_,
                              cf_t*  x,
                              float* csi,
                              int    nof_symbols,
                              float  scaling,
                              float  noise_estimate)
{

  cf_t* y[SRSLTE_MAX_PORTS];
  cf_t* h[SRSLTE_MAX_PORTS];
  y[0]          = y_;
  h[0]          = h_;
  int nof_rxant = 1;

  if (csi) {
    return srslte_predecoding_single_csi(y, h, x, csi, nof_rxant, nof_symbols, scaling, noise_estimate);
  }

#ifdef LV_HAVE_AVX
  if (nof_symbols > 32 && nof_rxant <= 2) {
    return srslte_predecoding_single_avx(y, h, x, nof_rxant, nof_symbols, scaling, noise_estimate);
  } else {
    return srslte_predecoding_single_gen(y, h, x, nof_rxant, nof_symbols, scaling, noise_estimate);
  }
#else
#ifdef LV_HAVE_SSE
  if (nof_symbols > 32 && nof_rxant <= 2) {
    return srslte_predecoding_single_sse(y, h, x, nof_rxant, nof_symbols, scaling, noise_estimate);
  } else {
    return srslte_predecoding_single_gen(y, h, x, nof_rxant, nof_symbols, scaling, noise_estimate);
  }
#else
  return srslte_predecoding_single_gen(y, h, x, nof_rxant, nof_symbols, scaling, noise_estimate);
#endif
#endif
}

/* ZF/MMSE SISO equalizer x=y(h'h+no)^(-1)h' (ZF if n0=0.0)*/
int srslte_predecoding_single_multi(cf_t*  y[SRSLTE_MAX_PORTS],
                                    cf_t*  h[SRSLTE_MAX_PORTS],
                                    cf_t*  x,
                                    float* csi[SRSLTE_MAX_CODEWORDS],
                                    int    nof_rxant,
                                    int    nof_symbols,
                                    float  scaling,
                                    float  noise_estimate)
{
  if (csi && csi[0]) {
    return srslte_predecoding_single_csi(y, h, x, csi[0], nof_rxant, nof_symbols, scaling, noise_estimate);
  }

#ifdef LV_HAVE_AVX
  if (nof_symbols > 32) {
    return srslte_predecoding_single_avx(y, h, x, nof_rxant, nof_symbols, scaling, noise_estimate);
  } else {
    return srslte_predecoding_single_gen(y, h, x, nof_rxant, nof_symbols, scaling, noise_estimate);
  }
#else
#ifdef LV_HAVE_SSE
  if (nof_symbols > 32) {
    return srslte_predecoding_single_sse(y, h, x, nof_rxant, nof_symbols, scaling, noise_estimate);
  } else {
    return srslte_predecoding_single_gen(y, h, x, nof_rxant, nof_symbols, scaling, noise_estimate);
  }
#else
  return srslte_predecoding_single_gen(y, h, x, nof_rxant, nof_symbols, scaling, noise_estimate);
#endif
#endif
}

/* C implementatino of the SFBC equalizer */
int srslte_predecoding_diversity_gen_(cf_t* y[SRSLTE_MAX_PORTS],
                                      cf_t* h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                      cf_t* x[SRSLTE_MAX_LAYERS],
                                      int   nof_rxant,
                                      int   nof_ports,
                                      int   nof_symbols,
                                      int   symbol_start,
                                      float scaling)
{
  int i;
  if (nof_ports == 2) {
    cf_t h00, h01, h10, h11, r0, r1;

    for (i = symbol_start / 2; i < nof_symbols / 2; i++) {
      float hh = 0;
      cf_t  x0 = 0;
      cf_t  x1 = 0;
      for (int p = 0; p < nof_rxant; p++) {
        h00 = h[0][p][2 * i];
        h01 = h[0][p][2 * i + 1];
        h10 = h[1][p][2 * i];
        h11 = h[1][p][2 * i + 1];
        hh += crealf(h00) * crealf(h00) + cimagf(h00) * cimagf(h00) + crealf(h11) * crealf(h11) +
              cimagf(h11) * cimagf(h11);
        r0 = y[p][2 * i];
        r1 = y[p][2 * i + 1];
        if (hh == 0) {
          hh = 1e-4;
        }
        x0 += (conjf(h00) * r0 + h11 * conjf(r1));
        x1 += (-h10 * conjf(r0) + conjf(h01) * r1);
      }
      hh *= scaling;
      x[0][i] = x0 / hh * M_SQRT2;
      x[1][i] = x1 / hh * M_SQRT2;
    }
    return i;
  } else if (nof_ports == 4) {
    cf_t h0, h1, h2, h3, r0, r1, r2, r3;

    int m_ap = (nof_symbols % 4) ? ((nof_symbols - 2) / 4) : nof_symbols / 4;
    for (i = symbol_start; i < m_ap; i++) {
      float hh02 = 0, hh13 = 0;
      cf_t  x0 = 0, x1 = 0, x2 = 0, x3 = 0;
      for (int p = 0; p < nof_rxant; p++) {
        h0 = h[0][p][4 * i];
        h1 = h[1][p][4 * i + 2];
        h2 = h[2][p][4 * i];
        h3 = h[3][p][4 * i + 2];
        hh02 += crealf(h0) * crealf(h0) + cimagf(h0) * cimagf(h0) + crealf(h2) * crealf(h2) + cimagf(h2) * cimagf(h2);
        hh13 += crealf(h1) * crealf(h1) + cimagf(h1) * cimagf(h1) + crealf(h3) * crealf(h3) + cimagf(h3) * cimagf(h3);
        r0 = y[p][4 * i];
        r1 = y[p][4 * i + 1];
        r2 = y[p][4 * i + 2];
        r3 = y[p][4 * i + 3];

        x0 += (conjf(h0) * r0 + h2 * conjf(r1));
        x1 += (-h2 * conjf(r0) + conjf(h0) * r1);
        x2 += (conjf(h1) * r2 + h3 * conjf(r3));
        x3 += (-h3 * conjf(r2) + conjf(h1) * r3);
      }

      hh02 *= scaling;
      hh13 *= scaling;

      x[0][i] = x0 / hh02 * M_SQRT2;
      x[1][i] = x1 / hh02 * M_SQRT2;
      x[2][i] = x2 / hh13 * M_SQRT2;
      x[3][i] = x3 / hh13 * M_SQRT2;
    }
    return i;
  } else {
    ERROR("Number of ports must be 2 or 4 for transmit diversity (nof_ports=%d)\n", nof_ports);
    return -1;
  }
}

int srslte_predecoding_diversity_gen(cf_t* y[SRSLTE_MAX_PORTS],
                                     cf_t* h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                     cf_t* x[SRSLTE_MAX_LAYERS],
                                     int   nof_rxant,
                                     int   nof_ports,
                                     int   nof_symbols,
                                     float scaling)
{
  return srslte_predecoding_diversity_gen_(y, h, x, nof_rxant, nof_ports, nof_symbols, 0, scaling);
}

/* SSE implementation of the 2-port SFBC equalizer */
#ifdef LV_HAVE_SSE
int srslte_predecoding_diversity2_sse(cf_t* y[SRSLTE_MAX_PORTS],
                                      cf_t* h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                      cf_t* x[SRSLTE_MAX_LAYERS],
                                      int   nof_rxant,
                                      int   nof_symbols,
                                      float scaling)
{
  float*       x0Ptr  = (float*)x[0];
  float*       x1Ptr  = (float*)x[1];
  const float* h0Ptr0 = (const float*)h[0][0];
  const float* h1Ptr0 = (const float*)h[1][0];
  const float* h0Ptr1 = (const float*)h[0][1];
  const float* h1Ptr1 = (const float*)h[1][1];
  const float* yPtr0  = (const float*)y[0];
  const float* yPtr1  = (const float*)y[1];

  __m128 conjugator = _mm_setr_ps(0, -0.f, 0, -0.f);
  __m128 sqrt2      = _mm_set1_ps(M_SQRT2 / scaling);

  __m128 h0Val_00, h0Val_10, h1Val_00, h1Val_10, h000, h00conj0, h010, h01conj0, h100, h110;
  __m128 h0Val_01, h0Val_11, h1Val_01, h1Val_11, h001, h00conj1, h011, h01conj1, h101, h111;
  __m128 hh, hhshuf, hhsum, hhadd;
  __m128 r0Val0, r1Val0, r00, r10, r0conj0, r1conj0;
  __m128 r0Val1, r1Val1, r01, r11, r0conj1, r1conj1;
  __m128 x0, x1;

  for (int i = 0; i < nof_symbols / 4; i++) {

    h0Val_00 = _mm_load_ps(h0Ptr0);
    h0Ptr0 += 4;
    h0Val_10 = _mm_load_ps(h0Ptr0);
    h0Ptr0 += 4;
    h1Val_00 = _mm_load_ps(h1Ptr0);
    h1Ptr0 += 4;
    h1Val_10 = _mm_load_ps(h1Ptr0);
    h1Ptr0 += 4;

    if (nof_rxant == 2) {
      h0Val_01 = _mm_load_ps(h0Ptr1);
      h0Ptr1 += 4;
      h0Val_11 = _mm_load_ps(h0Ptr1);
      h0Ptr1 += 4;
      h1Val_01 = _mm_load_ps(h1Ptr1);
      h1Ptr1 += 4;
      h1Val_11 = _mm_load_ps(h1Ptr1);
      h1Ptr1 += 4;
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

    r0Val0 = _mm_load_ps(yPtr0);
    yPtr0 += 4;
    r1Val0 = _mm_load_ps(yPtr0);
    yPtr0 += 4;
    r00 = _mm_shuffle_ps(r0Val0, r1Val0, _MM_SHUFFLE(1, 0, 1, 0));
    r10 = _mm_shuffle_ps(r0Val0, r1Val0, _MM_SHUFFLE(3, 2, 3, 2));

    if (nof_rxant == 2) {
      r0Val1 = _mm_load_ps(yPtr1);
      yPtr1 += 4;
      r1Val1 = _mm_load_ps(yPtr1);
      yPtr1 += 4;
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

    _mm_store_ps(x0Ptr, x0);
    x0Ptr += 4;
    _mm_store_ps(x1Ptr, x1);
    x1Ptr += 4;
  }
  // Compute remaining symbols using generic implementation
  srslte_predecoding_diversity_gen_(y, h, x, nof_rxant, 2, nof_symbols, 4 * (nof_symbols / 4), scaling);
  return nof_symbols;
}
#endif

int srslte_predecoding_diversity(cf_t* y_,
                                 cf_t* h_[SRSLTE_MAX_PORTS],
                                 cf_t* x[SRSLTE_MAX_LAYERS],
                                 int   nof_ports,
                                 int   nof_symbols,
                                 float scaling)
{
  cf_t*    h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS];
  cf_t*    y[SRSLTE_MAX_PORTS];
  uint32_t nof_rxant = 1;

  for (int i = 0; i < nof_ports; i++) {
    h[i][0] = h_[i];
  }
  y[0] = y_;

#ifdef LV_HAVE_SSE
  if (nof_symbols > 32 && nof_ports == 2) {
    return srslte_predecoding_diversity2_sse(y, h, x, nof_rxant, nof_symbols, scaling);
  } else {
    return srslte_predecoding_diversity_gen(y, h, x, nof_rxant, nof_ports, nof_symbols, scaling);
  }
#else
  return srslte_predecoding_diversity_gen(y, h, x, nof_rxant, nof_ports, nof_symbols, scaling);
#endif
}

int srslte_predecoding_diversity_csi(cf_t*  y[SRSLTE_MAX_PORTS],
                                     cf_t*  h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                     cf_t*  x[SRSLTE_MAX_LAYERS],
                                     float* csi[SRSLTE_MAX_CODEWORDS],
                                     int    nof_rxant,
                                     int    nof_ports,
                                     int    nof_symbols,
                                     float  scaling)
{
  int i;
  if (nof_ports == 2) {
    cf_t h00, h01, h10, h11, r0, r1;

    for (i = 0; i < nof_symbols / 2; i++) {
      float hh = 0;
      cf_t  x0 = 0;
      cf_t  x1 = 0;
      for (int p = 0; p < nof_rxant; p++) {
        h00 = h[0][p][2 * i];
        h01 = h[0][p][2 * i + 1];
        h10 = h[1][p][2 * i];
        h11 = h[1][p][2 * i + 1];
        hh += crealf(h00) * crealf(h00) + cimagf(h00) * cimagf(h00) + crealf(h11) * crealf(h11) +
              cimagf(h11) * cimagf(h11);
        r0 = y[p][2 * i];
        r1 = y[p][2 * i + 1];
        if (hh == 0) {
          hh = 1e-4;
        }
        x0 += (conjf(h00) * r0 + h11 * conjf(r1));
        x1 += (-h10 * conjf(r0) + conjf(h01) * r1);
      }

      csi[0][2 * i + 0] = hh;
      csi[0][2 * i + 1] = hh;

      hh *= scaling;
      x[0][i] = x0 / hh * M_SQRT2;
      x[1][i] = x1 / hh * M_SQRT2;
    }
    return i;
  } else if (nof_ports == 4) {
    int m_ap = (nof_symbols % 4) ? ((nof_symbols - 2) / 4) : nof_symbols / 4;
    for (i = 0; i < m_ap; i++) {
      cf_t  x0 = 0, x1 = 0, x2 = 0, x3 = 0;
      float a0 = 0, a1 = 0, a2 = 0, a3 = 0;
      cf_t  r0, r1, r2, r3;
      cf_t  h00, h01, h10, h11;

      for (int p = 0; p < nof_rxant; p++) {
        h00 = h[0][p][4 * i + 0];
        h01 = h[2][p][4 * i + 0];
        h10 = h[0][p][4 * i + 1];
        h11 = h[2][p][4 * i + 1];

        a0 += __real__ h00 * __real__ h00 + __imag__ h00 * __imag__ h00 + __real__ h11 * __real__ h11 +
              __imag__ h11 * __imag__ h11;

        a1 += __real__ h10 * __real__ h10 + __imag__ h10 * __imag__ h10 + __real__ h01 * __real__ h01 +
              __imag__ h01 * __imag__ h01;

        r0 = y[p][4 * i];
        r1 = y[p][4 * i + 1];

        x0 += (conjf(h00) * r0 + h11 * conjf(r1));
        x1 += (-h01 * conjf(r0) + conjf(h10) * r1);

        h00 = h[1][p][4 * i + 2];
        h01 = h[3][p][4 * i + 2];
        h10 = h[1][p][4 * i + 3];
        h11 = h[3][p][4 * i + 3];

        a2 += __real__ h00 * __real__ h00 + __imag__ h00 * __imag__ h00 + __real__ h11 * __real__ h11 +
              __imag__ h11 * __imag__ h11;

        a3 += __real__ h10 * __real__ h10 + __imag__ h10 * __imag__ h10 + __real__ h01 * __real__ h01 +
              __imag__ h01 * __imag__ h01;

        r2 = y[p][4 * i + 2];
        r3 = y[p][4 * i + 3];

        x2 += (conjf(h00) * r2 + h11 * conjf(r3));
        x3 += (-h01 * conjf(r2) + conjf(h10) * r3);
      }

      a0 *= scaling;
      a1 *= scaling;
      a2 *= scaling;
      a3 *= scaling;

      csi[0][4 * i + 0] = a0 / nof_rxant;
      csi[0][4 * i + 1] = a1 / nof_rxant;
      csi[0][4 * i + 2] = a2 / nof_rxant;
      csi[0][4 * i + 3] = a3 / nof_rxant;

      x[0][i] = x0 / a0 * M_SQRT2;
      x[1][i] = x1 / a1 * M_SQRT2;
      x[2][i] = x2 / a2 * M_SQRT2;
      x[3][i] = x3 / a3 * M_SQRT2;
    }
    return i;
  } else {
    ERROR("Number of ports must be 2 or 4 for transmit diversity (nof_ports=%d)\n", nof_ports);
    return -1;
  }
}

int srslte_predecoding_diversity_multi(cf_t*  y[SRSLTE_MAX_PORTS],
                                       cf_t*  h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                       cf_t*  x[SRSLTE_MAX_LAYERS],
                                       float* csi[SRSLTE_MAX_CODEWORDS],
                                       int    nof_rxant,
                                       int    nof_ports,
                                       int    nof_symbols,
                                       float  scaling)
{
  if (csi && csi[0]) {
    return srslte_predecoding_diversity_csi(y, h, x, csi, nof_rxant, nof_ports, nof_symbols, scaling);
  } else {
#ifdef LV_HAVE_SSE
    if (nof_symbols > 32 && nof_ports == 2) {
      return srslte_predecoding_diversity2_sse(y, h, x, nof_rxant, nof_symbols, scaling);
    } else {
      return srslte_predecoding_diversity_gen(y, h, x, nof_rxant, nof_ports, nof_symbols, scaling);
    }
#else
    return srslte_predecoding_diversity_gen(y, h, x, nof_rxant, nof_ports, nof_symbols, scaling);
#endif
  }
}

int srslte_precoding_mimo_2x2_gen(cf_t  W[2][2],
                                  cf_t* y[SRSLTE_MAX_PORTS],
                                  cf_t* h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                  cf_t* x[SRSLTE_MAX_LAYERS],
                                  int   nof_symbols,
                                  float scaling,
                                  float noise_estimate)
{

  cf_t G[2][2], Gx[2][2];

  for (int i = 0; i < nof_symbols; i++) {

    // G=H*W
    G[0][0] = h[0][0][i] * W[0][0] + h[0][1][i] * W[1][0];
    G[0][1] = h[0][0][i] * W[1][0] + h[0][1][i] * W[1][1];
    G[1][0] = h[1][0][i] * W[0][0] + h[1][1][i] * W[1][0];
    G[1][1] = h[1][0][i] * W[1][0] + h[1][1][i] * W[1][1];

    if (noise_estimate == 0) {
      // MF equalizer: Gx = G'
      Gx[0][0] = conjf(G[0][0]);
      Gx[0][1] = conjf(G[1][0]);
      Gx[1][0] = conjf(G[0][1]);
      Gx[1][1] = conjf(G[1][1]);
    } else {
      // MMSE equalizer: Gx = (G'G+I)
      ERROR("MMSE MIMO decoder not implemented\n");
      return -1;
    }

    // x=G*y
    x[0][i] = (Gx[0][0] * y[0][i] + Gx[0][1] * y[1][i]) * scaling;
    x[1][i] = (Gx[1][0] * y[0][i] + Gx[1][1] * y[1][i]) * scaling;
  }

  return SRSLTE_SUCCESS;
}

static int srslte_predecoding_ccd_2x2_zf_csi(cf_t*  y[SRSLTE_MAX_PORTS],
                                             cf_t*  h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                             cf_t*  x[SRSLTE_MAX_LAYERS],
                                             float* csi[SRSLTE_MAX_CODEWORDS],
                                             int    nof_symbols,
                                             float  scaling)
{
  uint32_t i    = 0;
  float    norm = 2.0f / scaling;

#if SRSLTE_SIMD_CF_SIZE != 0
#if SRSLTE_SIMD_CF_SIZE == 16
  float _mask1[SRSLTE_SIMD_CF_SIZE] = {
      +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f};
  float _mask2[SRSLTE_SIMD_CF_SIZE] = {
      -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f};
#elif SRSLTE_SIMD_CF_SIZE == 8
  float _mask1[SRSLTE_SIMD_CF_SIZE] = {+0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f};
  float _mask2[SRSLTE_SIMD_CF_SIZE] = {-0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f};
#elif SRSLTE_SIMD_CF_SIZE == 4
  float _mask1[SRSLTE_SIMD_CF_SIZE] = {+0.0f, -0.0f, +0.0f, -0.0f};
  float _mask2[SRSLTE_SIMD_CF_SIZE] = {-0.0f, +0.0f, -0.0f, +0.0f};
#endif

  simd_f_t mask1 = srslte_simd_f_loadu(_mask1);
  simd_f_t mask2 = srslte_simd_f_loadu(_mask2);

  for (; i < nof_symbols - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
    /* Load channel */
    simd_cf_t h00i = srslte_simd_cfi_load(&h[0][0][i]);
    simd_cf_t h01i = srslte_simd_cfi_load(&h[0][1][i]);
    simd_cf_t h10i = srslte_simd_cfi_load(&h[1][0][i]);
    simd_cf_t h11i = srslte_simd_cfi_load(&h[1][1][i]);

    /* Apply precoding */
    simd_cf_t h00, h01, h10, h11;
    h00 = srslte_simd_cf_add(h00i, srslte_simd_cf_neg_mask(h10i, mask1));
    h10 = srslte_simd_cf_add(h01i, srslte_simd_cf_neg_mask(h11i, mask1));
    h01 = srslte_simd_cf_add(h00i, srslte_simd_cf_neg_mask(h10i, mask2));
    h11 = srslte_simd_cf_add(h01i, srslte_simd_cf_neg_mask(h11i, mask2));

    simd_cf_t y0 = srslte_simd_cfi_load(&y[0][i]);
    simd_cf_t y1 = srslte_simd_cfi_load(&y[1][i]);

    simd_cf_t x0, x1;
    simd_f_t  csi0, csi1;

    srslte_mat_2x2_zf_csi_simd(y0, y1, h00, h01, h10, h11, &x0, &x1, &csi0, &csi1, norm);

    srslte_simd_cfi_store(&x[0][i], x0);
    srslte_simd_cfi_store(&x[1][i], x1);

    srslte_simd_f_store(&csi[0][i], csi0);
    srslte_simd_f_store(&csi[1][i], csi1);
  }
#endif /* SRSLTE_SIMD_CF_SIZE != 0 */

  cf_t h00, h01, h10, h11, det;
  for (; i < nof_symbols; i++) {

    // Even precoder
    h00 = +h[0][0][i] + h[1][0][i];
    h10 = +h[0][1][i] + h[1][1][i];
    h01 = +h[0][0][i] - h[1][0][i];
    h11 = +h[0][1][i] - h[1][1][i];
    det = (h00 * h11 - h01 * h10);
    det = conjf(det) * (norm / (crealf(det) * crealf(det) + cimagf(det) * cimagf(det)));

    x[0][i] = (+h11 * y[0][i] - h01 * y[1][i]) * det;
    x[1][i] = (-h10 * y[0][i] + h00 * y[1][i]) * det;

    csi[0][i] = 1.0f;
    csi[1][i] = 1.0f;

    i++;

    // Odd precoder
    h00 = h[0][0][i] - h[1][0][i];
    h10 = h[0][1][i] - h[1][1][i];
    h01 = h[0][0][i] + h[1][0][i];
    h11 = h[0][1][i] + h[1][1][i];
    det = (h00 * h11 - h01 * h10);
    det = conjf(det) * (norm / (crealf(det) * crealf(det) + cimagf(det) * cimagf(det)));

    x[0][i] = (+h11 * y[0][i] - h01 * y[1][i]) * det;
    x[1][i] = (-h10 * y[0][i] + h00 * y[1][i]) * det;

    csi[0][i] = 1.0f;
    csi[1][i] = 1.0f;
  }
  return SRSLTE_SUCCESS;
}

static int srslte_predecoding_ccd_2x2_zf(cf_t* y[SRSLTE_MAX_PORTS],
                                         cf_t* h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                         cf_t* x[SRSLTE_MAX_LAYERS],
                                         int   nof_symbols,
                                         float scaling)
{
  uint32_t i    = 0;
  float    norm = 2.0f / scaling;

#if SRSLTE_SIMD_CF_SIZE != 0
#if SRSLTE_SIMD_CF_SIZE == 16
  float _mask1[SRSLTE_SIMD_CF_SIZE] = {
      +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f};
  float _mask2[SRSLTE_SIMD_CF_SIZE] = {
      -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f};
#elif SRSLTE_SIMD_CF_SIZE == 8
  float _mask1[SRSLTE_SIMD_CF_SIZE] = {+0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f};
  float _mask2[SRSLTE_SIMD_CF_SIZE] = {-0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f};
#elif SRSLTE_SIMD_CF_SIZE == 4
  float _mask1[SRSLTE_SIMD_CF_SIZE] = {+0.0f, -0.0f, +0.0f, -0.0f};
  float _mask2[SRSLTE_SIMD_CF_SIZE] = {-0.0f, +0.0f, -0.0f, +0.0f};
#endif

  simd_f_t mask1 = srslte_simd_f_loadu(_mask1);
  simd_f_t mask2 = srslte_simd_f_loadu(_mask2);

  for (; i < nof_symbols - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
    /* Load channel */
    simd_cf_t h00i = srslte_simd_cfi_load(&h[0][0][i]);
    simd_cf_t h01i = srslte_simd_cfi_load(&h[0][1][i]);
    simd_cf_t h10i = srslte_simd_cfi_load(&h[1][0][i]);
    simd_cf_t h11i = srslte_simd_cfi_load(&h[1][1][i]);

    /* Apply precoding */
    simd_cf_t h00, h01, h10, h11;
    h00 = srslte_simd_cf_add(h00i, srslte_simd_cf_neg_mask(h10i, mask1));
    h10 = srslte_simd_cf_add(h01i, srslte_simd_cf_neg_mask(h11i, mask1));
    h01 = srslte_simd_cf_add(h00i, srslte_simd_cf_neg_mask(h10i, mask2));
    h11 = srslte_simd_cf_add(h01i, srslte_simd_cf_neg_mask(h11i, mask2));

    simd_cf_t y0 = srslte_simd_cfi_load(&y[0][i]);
    simd_cf_t y1 = srslte_simd_cfi_load(&y[1][i]);

    simd_cf_t x0, x1;

    srslte_mat_2x2_zf_simd(y0, y1, h00, h01, h10, h11, &x0, &x1, norm);

    srslte_simd_cfi_store(&x[0][i], x0);
    srslte_simd_cfi_store(&x[1][i], x1);
  }
#endif /* SRSLTE_SIMD_CF_SIZE != 0 */

  cf_t h00, h01, h10, h11, det;
  for (; i < nof_symbols; i++) {

    // Even precoder
    h00 = +h[0][0][i] + h[1][0][i];
    h10 = +h[0][1][i] + h[1][1][i];
    h01 = +h[0][0][i] - h[1][0][i];
    h11 = +h[0][1][i] - h[1][1][i];
    det = (h00 * h11 - h01 * h10);
    det = conjf(det) * (norm / (crealf(det) * crealf(det) + cimagf(det) * cimagf(det)));

    x[0][i] = (+h11 * y[0][i] - h01 * y[1][i]) * det;
    x[1][i] = (-h10 * y[0][i] + h00 * y[1][i]) * det;

    i++;

    // Odd precoder
    h00 = h[0][0][i] - h[1][0][i];
    h10 = h[0][1][i] - h[1][1][i];
    h01 = h[0][0][i] + h[1][0][i];
    h11 = h[0][1][i] + h[1][1][i];
    det = (h00 * h11 - h01 * h10);
    det = conjf(det) * (norm / (crealf(det) * crealf(det) + cimagf(det) * cimagf(det)));

    x[0][i] = (+h11 * y[0][i] - h01 * y[1][i]) * det;
    x[1][i] = (-h10 * y[0][i] + h00 * y[1][i]) * det;
  }
  return SRSLTE_SUCCESS;
}

static int srslte_predecoding_ccd_zf(cf_t*  y[SRSLTE_MAX_PORTS],
                                     cf_t*  h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                     cf_t*  x[SRSLTE_MAX_LAYERS],
                                     float* csi[SRSLTE_MAX_CODEWORDS],
                                     int    nof_rxant,
                                     int    nof_ports,
                                     int    nof_layers,
                                     int    nof_symbols,
                                     float  scaling)
{
  if (nof_ports == 2 && nof_rxant == 2) {
    if (nof_layers == 2) {
      if (csi && csi[0]) {
        return srslte_predecoding_ccd_2x2_zf_csi(y, h, x, csi, nof_symbols, scaling);
      } else {
        return srslte_predecoding_ccd_2x2_zf(y, h, x, nof_symbols, scaling);
      }
    } else {
      ERROR("Error predecoding CCD: Invalid number of layers %d\n", nof_layers);
      return -1;
    }
  } else if (nof_ports == 4) {
    ERROR("Error predecoding CCD: Only 2 ports supported\n");
  } else {
    ERROR("Error predecoding CCD: Invalid combination of ports %d and rx antennax %d\n", nof_ports, nof_rxant);
  }
  return SRSLTE_ERROR;
}

static int srslte_predecoding_ccd_2x2_mmse_csi(cf_t*  y[SRSLTE_MAX_PORTS],
                                               cf_t*  h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                               cf_t*  x[SRSLTE_MAX_LAYERS],
                                               float* csi[SRSLTE_MAX_CODEWORDS],
                                               int    nof_symbols,
                                               float  scaling,
                                               float  noise_estimate)
{
  int   i    = 0;
  float norm = 2.0f / scaling;

#if SRSLTE_SIMD_CF_SIZE != 0
#if SRSLTE_SIMD_CF_SIZE == 16
  float _mask1[SRSLTE_SIMD_CF_SIZE] = {
      +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f};
  float _mask2[SRSLTE_SIMD_CF_SIZE] = {
      -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f};
#elif SRSLTE_SIMD_CF_SIZE == 8
  float _mask1[SRSLTE_SIMD_CF_SIZE] = {+0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f};
  float _mask2[SRSLTE_SIMD_CF_SIZE] = {-0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f};
#elif SRSLTE_SIMD_CF_SIZE == 4
  float _mask1[SRSLTE_SIMD_CF_SIZE] = {+0.0f, -0.0f, +0.0f, -0.0f};
  float _mask2[SRSLTE_SIMD_CF_SIZE] = {-0.0f, +0.0f, -0.0f, +0.0f};
#endif

  simd_f_t mask1 = srslte_simd_f_loadu(_mask1);
  simd_f_t mask2 = srslte_simd_f_loadu(_mask2);

  for (; i < nof_symbols - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
    /* Load channel */
    simd_cf_t h00i = srslte_simd_cfi_load(&h[0][0][i]);
    simd_cf_t h01i = srslte_simd_cfi_load(&h[0][1][i]);
    simd_cf_t h10i = srslte_simd_cfi_load(&h[1][0][i]);
    simd_cf_t h11i = srslte_simd_cfi_load(&h[1][1][i]);

    /* Apply precoding */
    simd_cf_t h00, h01, h10, h11;
    h00 = srslte_simd_cf_add(h00i, srslte_simd_cf_neg_mask(h10i, mask1));
    h10 = srslte_simd_cf_add(h01i, srslte_simd_cf_neg_mask(h11i, mask1));
    h01 = srslte_simd_cf_add(h00i, srslte_simd_cf_neg_mask(h10i, mask2));
    h11 = srslte_simd_cf_add(h01i, srslte_simd_cf_neg_mask(h11i, mask2));

    simd_cf_t y0 = srslte_simd_cfi_load(&y[0][i]);
    simd_cf_t y1 = srslte_simd_cfi_load(&y[1][i]);

    simd_cf_t x0, x1;
    simd_f_t  csi0, csi1;

    srslte_mat_2x2_mmse_csi_simd(y0, y1, h00, h01, h10, h11, &x0, &x1, &csi0, &csi1, noise_estimate, norm);

    srslte_simd_cfi_store(&x[0][i], x0);
    srslte_simd_cfi_store(&x[1][i], x1);

    srslte_simd_f_store(&csi[0][i], csi0);
    srslte_simd_f_store(&csi[1][i], csi1);
  }
#endif /* SRSLTE_SIMD_CF_SIZE != 0 */

  cf_t h00, h01, h10, h11;
  for (; i < nof_symbols; i++) {

    // Even precoder
    h00 = +h[0][0][i] + h[1][0][i];
    h10 = +h[0][1][i] + h[1][1][i];
    h01 = +h[0][0][i] - h[1][0][i];
    h11 = +h[0][1][i] - h[1][1][i];
    srslte_mat_2x2_mmse_csi_gen(
        y[0][i], y[1][i], h00, h01, h10, h11, &x[0][i], &x[1][i], &csi[0][i], &csi[1][i], noise_estimate, norm);
    i++;

    // Odd precoder
    h00 = h[0][0][i] - h[1][0][i];
    h10 = h[0][1][i] - h[1][1][i];
    h01 = h[0][0][i] + h[1][0][i];
    h11 = h[0][1][i] + h[1][1][i];
    srslte_mat_2x2_mmse_csi_gen(
        y[0][i], y[1][i], h00, h01, h10, h11, &x[0][i], &x[1][i], &csi[0][i], &csi[1][i], noise_estimate, norm);
  }
  return SRSLTE_SUCCESS;
}

static int srslte_predecoding_ccd_2x2_mmse(cf_t* y[SRSLTE_MAX_PORTS],
                                           cf_t* h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                           cf_t* x[SRSLTE_MAX_LAYERS],
                                           int   nof_symbols,
                                           float scaling,
                                           float noise_estimate)
{
  int   i    = 0;
  float norm = 2.0f / scaling;

#if SRSLTE_SIMD_CF_SIZE != 0
#if SRSLTE_SIMD_CF_SIZE == 16
  float _mask1[SRSLTE_SIMD_CF_SIZE] = {
      +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f};
  float _mask2[SRSLTE_SIMD_CF_SIZE] = {
      -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f};
#elif SRSLTE_SIMD_CF_SIZE == 8
  float _mask1[SRSLTE_SIMD_CF_SIZE] = {+0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f};
  float _mask2[SRSLTE_SIMD_CF_SIZE] = {-0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f, -0.0f, +0.0f};
#elif SRSLTE_SIMD_CF_SIZE == 4
  float _mask1[SRSLTE_SIMD_CF_SIZE] = {+0.0f, -0.0f, +0.0f, -0.0f};
  float _mask2[SRSLTE_SIMD_CF_SIZE] = {-0.0f, +0.0f, -0.0f, +0.0f};
#endif

  simd_f_t mask1 = srslte_simd_f_loadu(_mask1);
  simd_f_t mask2 = srslte_simd_f_loadu(_mask2);

  for (; i < nof_symbols - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
    /* Load channel */
    simd_cf_t h00i = srslte_simd_cfi_load(&h[0][0][i]);
    simd_cf_t h01i = srslte_simd_cfi_load(&h[0][1][i]);
    simd_cf_t h10i = srslte_simd_cfi_load(&h[1][0][i]);
    simd_cf_t h11i = srslte_simd_cfi_load(&h[1][1][i]);

    /* Apply precoding */
    simd_cf_t h00, h01, h10, h11;
    h00 = srslte_simd_cf_add(h00i, srslte_simd_cf_neg_mask(h10i, mask1));
    h10 = srslte_simd_cf_add(h01i, srslte_simd_cf_neg_mask(h11i, mask1));
    h01 = srslte_simd_cf_add(h00i, srslte_simd_cf_neg_mask(h10i, mask2));
    h11 = srslte_simd_cf_add(h01i, srslte_simd_cf_neg_mask(h11i, mask2));

    simd_cf_t y0 = srslte_simd_cfi_load(&y[0][i]);
    simd_cf_t y1 = srslte_simd_cfi_load(&y[1][i]);

    simd_cf_t x0, x1;
    srslte_mat_2x2_mmse_simd(y0, y1, h00, h01, h10, h11, &x0, &x1, noise_estimate, norm);

    srslte_simd_cfi_store(&x[0][i], x0);
    srslte_simd_cfi_store(&x[1][i], x1);
  }
#endif /* SRSLTE_SIMD_CF_SIZE != 0 */

  cf_t h00, h01, h10, h11;
  for (; i < nof_symbols; i++) {

    // Even precoder
    h00 = +h[0][0][i] + h[1][0][i];
    h10 = +h[0][1][i] + h[1][1][i];
    h01 = +h[0][0][i] - h[1][0][i];
    h11 = +h[0][1][i] - h[1][1][i];
    srslte_mat_2x2_mmse_gen(y[0][i], y[1][i], h00, h01, h10, h11, &x[0][i], &x[1][i], noise_estimate, 2.0f / scaling);

    i++;

    // Odd precoder
    h00 = h[0][0][i] - h[1][0][i];
    h10 = h[0][1][i] - h[1][1][i];
    h01 = h[0][0][i] + h[1][0][i];
    h11 = h[0][1][i] + h[1][1][i];
    srslte_mat_2x2_mmse_gen(y[0][i], y[1][i], h00, h01, h10, h11, &x[0][i], &x[1][i], noise_estimate, 2.0f / scaling);
  }
  return SRSLTE_SUCCESS;
}

int srslte_predecoding_ccd_mmse(cf_t*  y[SRSLTE_MAX_PORTS],
                                cf_t*  h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                cf_t*  x[SRSLTE_MAX_LAYERS],
                                float* csi[SRSLTE_MAX_CODEWORDS],
                                int    nof_rxant,
                                int    nof_ports,
                                int    nof_layers,
                                int    nof_symbols,
                                float  scaling,
                                float  noise_estimate)
{
  if (nof_ports == 2 && nof_rxant == 2) {
    if (nof_layers == 2) {
      if (csi && csi[0])
        return srslte_predecoding_ccd_2x2_mmse_csi(y, h, x, csi, nof_symbols, scaling, noise_estimate);
      else {
        return srslte_predecoding_ccd_2x2_mmse(y, h, x, nof_symbols, scaling, noise_estimate);
      }
    } else {
      ERROR("Error predecoding CCD: Invalid number of layers %d\n", nof_layers);
      return -1;
    }
  } else if (nof_ports == 4) {
    ERROR("Error predecoding CCD: Only 2 ports supported\n");
  } else {
    ERROR("Error predecoding CCD: Invalid combination of ports %d and rx antennax %d\n", nof_ports, nof_rxant);
  }
  return SRSLTE_ERROR;
}

static int srslte_predecoding_multiplex_2x2_zf_csi(cf_t*  y[SRSLTE_MAX_PORTS],
                                                   cf_t*  h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                                   cf_t*  x[SRSLTE_MAX_LAYERS],
                                                   float* csi,
                                                   int    codebook_idx,
                                                   int    nof_symbols,
                                                   float  scaling)
{
  float norm = 1.0f;
  int   i    = 0;

  switch (codebook_idx) {
    case 0:
      norm = (float)M_SQRT2 / scaling;
      break;
    case 1:
    case 2:
      norm = 2.0f / scaling;
      break;
    default:
      ERROR("Wrong codebook_idx=%d", codebook_idx);
      return SRSLTE_ERROR;
  }

#if SRSLTE_SIMD_CF_SIZE != 0
  for (; i < nof_symbols - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
    simd_cf_t h00i = srslte_simd_cfi_load(&h[0][0][i]);
    simd_cf_t h01i = srslte_simd_cfi_load(&h[0][1][i]);
    simd_cf_t h10i = srslte_simd_cfi_load(&h[1][0][i]);
    simd_cf_t h11i = srslte_simd_cfi_load(&h[1][1][i]);

    simd_cf_t h00, h01, h10, h11;
    switch (codebook_idx) {
      case 0:
        h00 = h00i;
        h01 = h10i;
        h10 = h01i;
        h11 = h11i;
        break;
      case 1:
        h00 = srslte_simd_cf_add(h00i, h10i);
        h01 = srslte_simd_cf_sub(h00i, h10i);
        h10 = srslte_simd_cf_add(h01i, h11i);
        h11 = srslte_simd_cf_sub(h01i, h11i);
        break;
      case 2:
        h00 = srslte_simd_cf_add(h00i, srslte_simd_cf_mulj(h10i));
        h01 = srslte_simd_cf_sub(h00i, srslte_simd_cf_mulj(h10i));
        h10 = srslte_simd_cf_add(h01i, srslte_simd_cf_mulj(h11i));
        h11 = srslte_simd_cf_sub(h01i, srslte_simd_cf_mulj(h11i));
        break;
      default:
        ERROR("Wrong codebook_idx=%d\n", codebook_idx);
        return SRSLTE_ERROR;
    }

    simd_cf_t y0 = srslte_simd_cfi_load(&y[0][i]);
    simd_cf_t y1 = srslte_simd_cfi_load(&y[1][i]);

    simd_cf_t x0, x1;
    simd_f_t  csi0, csi1;
    srslte_mat_2x2_zf_csi_simd(y0, y1, h00, h01, h10, h11, &x0, &x1, &csi0, &csi1, norm);

    srslte_simd_cfi_store(&x[0][i], x0);
    srslte_simd_cfi_store(&x[1][i], x1);

    srslte_simd_f_store(&csi[i], csi0);
    srslte_simd_f_store(&csi[i], csi1);
  }
#endif /* SRSLTE_SIMD_CF_SIZE */

  for (; i < nof_symbols; i++) {
    cf_t h00, h01, h10, h11;

    switch (codebook_idx) {
      case 0:
        h00 = h[0][0][i];
        h01 = h[1][0][i];
        h10 = h[0][1][i];
        h11 = h[1][1][i];
        break;
      case 1:
        h00 = h[0][0][i] + h[1][0][i];
        h01 = h[0][0][i] - h[1][0][i];
        h10 = h[0][1][i] + h[1][1][i];
        h11 = h[0][1][i] - h[1][1][i];
        break;
      case 2:
        h00 = h[0][0][i] + _Complex_I * h[1][0][i];
        h01 = h[0][0][i] - _Complex_I * h[1][0][i];
        h10 = h[0][1][i] + _Complex_I * h[1][1][i];
        h11 = h[0][1][i] - _Complex_I * h[1][1][i];
        break;
      default:
        ERROR("Wrong codebook_idx=%d\n", codebook_idx);
        return SRSLTE_ERROR;
    }

    cf_t det = (h00 * h11 - h01 * h10);
    det      = conjf(det) * (norm / (crealf(det) * crealf(det) + cimagf(det) * cimagf(det)));

    x[0][i] = (+h11 * y[0][i] - h01 * y[1][i]) * det;
    x[1][i] = (-h10 * y[0][i] + h00 * y[1][i]) * det;

    csi[i] = 1.0f;
    csi[i] = 1.0f;
  }
  return SRSLTE_SUCCESS;
}

// Generic implementation of ZF 2x2 Spatial Multiplexity equalizer
static int srslte_predecoding_multiplex_2x2_zf(cf_t* y[SRSLTE_MAX_PORTS],
                                               cf_t* h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                               cf_t* x[SRSLTE_MAX_LAYERS],
                                               int   codebook_idx,
                                               int   nof_symbols,
                                               float scaling)
{
  float norm = 1.0f;
  int   i    = 0;

  switch (codebook_idx) {
    case 0:
      norm = (float)M_SQRT2 / scaling;
      break;
    case 1:
    case 2:
      norm = 2.0f / scaling;
      break;
    default:
      ERROR("Wrong codebook_idx=%d", codebook_idx);
      return SRSLTE_ERROR;
  }

#if SRSLTE_SIMD_CF_SIZE != 0
  for (; i < nof_symbols - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
    simd_cf_t h00i = srslte_simd_cfi_load(&h[0][0][i]);
    simd_cf_t h01i = srslte_simd_cfi_load(&h[0][1][i]);
    simd_cf_t h10i = srslte_simd_cfi_load(&h[1][0][i]);
    simd_cf_t h11i = srslte_simd_cfi_load(&h[1][1][i]);

    simd_cf_t h00, h01, h10, h11;
    switch (codebook_idx) {
      case 0:
        h00 = h00i;
        h01 = h10i;
        h10 = h01i;
        h11 = h11i;
        break;
      case 1:
        h00 = srslte_simd_cf_add(h00i, h10i);
        h01 = srslte_simd_cf_sub(h00i, h10i);
        h10 = srslte_simd_cf_add(h01i, h11i);
        h11 = srslte_simd_cf_sub(h01i, h11i);
        break;
      case 2:
        h00 = srslte_simd_cf_add(h00i, srslte_simd_cf_mulj(h10i));
        h01 = srslte_simd_cf_sub(h00i, srslte_simd_cf_mulj(h10i));
        h10 = srslte_simd_cf_add(h01i, srslte_simd_cf_mulj(h11i));
        h11 = srslte_simd_cf_sub(h01i, srslte_simd_cf_mulj(h11i));
        break;
      default:
        ERROR("Wrong codebook_idx=%d\n", codebook_idx);
        return SRSLTE_ERROR;
    }

    simd_cf_t y0 = srslte_simd_cfi_load(&y[0][i]);
    simd_cf_t y1 = srslte_simd_cfi_load(&y[1][i]);

    simd_cf_t x0, x1;
    simd_f_t  csi0, csi1;
    srslte_mat_2x2_zf_csi_simd(y0, y1, h00, h01, h10, h11, &x0, &x1, &csi0, &csi1, norm);

    srslte_simd_cfi_store(&x[0][i], x0);
    srslte_simd_cfi_store(&x[1][i], x1);
  }
#endif /* SRSLTE_SIMD_CF_SIZE */

  for (; i < nof_symbols; i++) {
    cf_t h00, h01, h10, h11;

    switch (codebook_idx) {
      case 0:
        h00 = h[0][0][i];
        h01 = h[1][0][i];
        h10 = h[0][1][i];
        h11 = h[1][1][i];
        break;
      case 1:
        h00 = h[0][0][i] + h[1][0][i];
        h01 = h[0][0][i] - h[1][0][i];
        h10 = h[0][1][i] + h[1][1][i];
        h11 = h[0][1][i] - h[1][1][i];
        break;
      case 2:
        h00 = h[0][0][i] + _Complex_I * h[1][0][i];
        h01 = h[0][0][i] - _Complex_I * h[1][0][i];
        h10 = h[0][1][i] + _Complex_I * h[1][1][i];
        h11 = h[0][1][i] - _Complex_I * h[1][1][i];
        break;
      default:
        ERROR("Wrong codebook_idx=%d\n", codebook_idx);
        return SRSLTE_ERROR;
    }

    srslte_mat_2x2_zf_gen(y[0][i], y[1][i], h00, h01, h10, h11, &x[0][i], &x[1][i], norm);
  }
  return SRSLTE_SUCCESS;
}

// Generic implementation of ZF 2x2 Spatial Multiplexity equalizer
static int srslte_predecoding_multiplex_2x2_mmse_csi(cf_t*  y[SRSLTE_MAX_PORTS],
                                                     cf_t*  h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                                     cf_t*  x[SRSLTE_MAX_LAYERS],
                                                     float* csi[SRSLTE_MAX_CODEWORDS],
                                                     int    codebook_idx,
                                                     int    nof_symbols,
                                                     float  scaling,
                                                     float  noise_estimate)
{
  float norm = 1.0f;
  int   i    = 0;

  switch (codebook_idx) {
    case 0:
      norm = (float)M_SQRT2 / scaling;
      break;
    case 1:
    case 2:
      norm = 2.0f / scaling;
      break;
    default:
      ERROR("Wrong codebook_idx=%d", codebook_idx);
      return SRSLTE_ERROR;
  }

#if SRSLTE_SIMD_CF_SIZE != 0
  for (; i < nof_symbols - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
    simd_cf_t h00i = srslte_simd_cfi_load(&h[0][0][i]);
    simd_cf_t h01i = srslte_simd_cfi_load(&h[0][1][i]);
    simd_cf_t h10i = srslte_simd_cfi_load(&h[1][0][i]);
    simd_cf_t h11i = srslte_simd_cfi_load(&h[1][1][i]);

    simd_cf_t h00, h01, h10, h11;
    switch (codebook_idx) {
      case 0:
        h00 = h00i;
        h01 = h10i;
        h10 = h01i;
        h11 = h11i;
        break;
      case 1:
        h00 = srslte_simd_cf_add(h00i, h10i);
        h01 = srslte_simd_cf_sub(h00i, h10i);
        h10 = srslte_simd_cf_add(h01i, h11i);
        h11 = srslte_simd_cf_sub(h01i, h11i);
        break;
      case 2:
        h00 = srslte_simd_cf_add(h00i, srslte_simd_cf_mulj(h10i));
        h01 = srslte_simd_cf_sub(h00i, srslte_simd_cf_mulj(h10i));
        h10 = srslte_simd_cf_add(h01i, srslte_simd_cf_mulj(h11i));
        h11 = srslte_simd_cf_sub(h01i, srslte_simd_cf_mulj(h11i));
        break;
      default:
        ERROR("Wrong codebook_idx=%d\n", codebook_idx);
        return SRSLTE_ERROR;
    }

    simd_cf_t y0 = srslte_simd_cfi_load(&y[0][i]);
    simd_cf_t y1 = srslte_simd_cfi_load(&y[1][i]);

    simd_cf_t x0, x1;
    simd_f_t  csi0, csi1;
    srslte_mat_2x2_mmse_csi_simd(y0, y1, h00, h01, h10, h11, &x0, &x1, &csi0, &csi1, noise_estimate, norm);

    srslte_simd_cfi_store(&x[0][i], x0);
    srslte_simd_cfi_store(&x[1][i], x1);

    srslte_simd_f_store(&csi[0][i], csi0);
    srslte_simd_f_store(&csi[1][i], csi1);
  }
#endif /* SRSLTE_SIMD_CF_SIZE */

  for (; i < nof_symbols; i++) {
    cf_t h00, h01, h10, h11;

    switch (codebook_idx) {
      case 0:
        h00 = h[0][0][i];
        h01 = h[1][0][i];
        h10 = h[0][1][i];
        h11 = h[1][1][i];
        break;
      case 1:
        h00 = h[0][0][i] + h[1][0][i];
        h01 = h[0][0][i] - h[1][0][i];
        h10 = h[0][1][i] + h[1][1][i];
        h11 = h[0][1][i] - h[1][1][i];
        break;
      case 2:
        h00 = h[0][0][i] + _Complex_I * h[1][0][i];
        h01 = h[0][0][i] - _Complex_I * h[1][0][i];
        h10 = h[0][1][i] + _Complex_I * h[1][1][i];
        h11 = h[0][1][i] - _Complex_I * h[1][1][i];
        break;
      default:
        ERROR("Wrong codebook_idx=%d\n", codebook_idx);
        return SRSLTE_ERROR;
    }

    srslte_mat_2x2_mmse_csi_gen(
        y[0][i], y[1][i], h00, h01, h10, h11, &x[0][i], &x[1][i], &csi[0][i], &csi[1][i], noise_estimate, norm);
  }
  return SRSLTE_SUCCESS;
}

static int srslte_predecoding_multiplex_2x2_mmse(cf_t* y[SRSLTE_MAX_PORTS],
                                                 cf_t* h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                                 cf_t* x[SRSLTE_MAX_LAYERS],
                                                 int   codebook_idx,
                                                 int   nof_symbols,
                                                 float scaling,
                                                 float noise_estimate)
{
  float norm = 1.0;
  int   i    = 0;

  switch (codebook_idx) {
    case 0:
      norm = (float)M_SQRT2 / scaling;
      break;
    case 1:
    case 2:
      norm = 2.0f / scaling;
      break;
    default:
      ERROR("Wrong codebook_idx=%d", codebook_idx);
      return SRSLTE_ERROR;
  }

#if SRSLTE_SIMD_CF_SIZE != 0
  for (; i < nof_symbols - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
    simd_cf_t h00i = srslte_simd_cfi_load(&h[0][0][i]);
    simd_cf_t h01i = srslte_simd_cfi_load(&h[0][1][i]);
    simd_cf_t h10i = srslte_simd_cfi_load(&h[1][0][i]);
    simd_cf_t h11i = srslte_simd_cfi_load(&h[1][1][i]);

    simd_cf_t h00, h01, h10, h11;
    switch (codebook_idx) {
      case 0:
        h00 = h00i;
        h01 = h10i;
        h10 = h01i;
        h11 = h11i;
        break;
      case 1:
        h00 = srslte_simd_cf_add(h00i, h10i);
        h01 = srslte_simd_cf_sub(h00i, h10i);
        h10 = srslte_simd_cf_add(h01i, h11i);
        h11 = srslte_simd_cf_sub(h01i, h11i);
        break;
      case 2:
        h00 = srslte_simd_cf_add(h00i, srslte_simd_cf_mulj(h10i));
        h01 = srslte_simd_cf_sub(h00i, srslte_simd_cf_mulj(h10i));
        h10 = srslte_simd_cf_add(h01i, srslte_simd_cf_mulj(h11i));
        h11 = srslte_simd_cf_sub(h01i, srslte_simd_cf_mulj(h11i));
        break;
      default:
        ERROR("Wrong codebook_idx=%d\n", codebook_idx);
        return SRSLTE_ERROR;
    }

    simd_cf_t y0 = srslte_simd_cfi_load(&y[0][i]);
    simd_cf_t y1 = srslte_simd_cfi_load(&y[1][i]);

    simd_cf_t x0, x1;
    simd_f_t  csi0, csi1;
    srslte_mat_2x2_mmse_csi_simd(y0, y1, h00, h01, h10, h11, &x0, &x1, &csi0, &csi1, noise_estimate, norm);

    srslte_simd_cfi_store(&x[0][i], x0);
    srslte_simd_cfi_store(&x[1][i], x1);
  }
#endif /* SRSLTE_SIMD_CF_SIZE */

  for (; i < nof_symbols; i++) {
    cf_t h00, h01, h10, h11;

    switch (codebook_idx) {
      case 0:
        h00 = h[0][0][i];
        h01 = h[1][0][i];
        h10 = h[0][1][i];
        h11 = h[1][1][i];
        break;
      case 1:
        h00 = h[0][0][i] + h[1][0][i];
        h01 = h[0][0][i] - h[1][0][i];
        h10 = h[0][1][i] + h[1][1][i];
        h11 = h[0][1][i] - h[1][1][i];
        break;
      case 2:
        h00 = h[0][0][i] + _Complex_I * h[1][0][i];
        h01 = h[0][0][i] - _Complex_I * h[1][0][i];
        h10 = h[0][1][i] + _Complex_I * h[1][1][i];
        h11 = h[0][1][i] - _Complex_I * h[1][1][i];
        break;
      default:
        ERROR("Wrong codebook_idx=%d\n", codebook_idx);
        return SRSLTE_ERROR;
    }

    srslte_mat_2x2_mmse_gen(y[0][i], y[1][i], h00, h01, h10, h11, &x[0][i], &x[1][i], noise_estimate, norm);
  }
  return SRSLTE_SUCCESS;
}

// Implementation of MRC 2x1 (two antennas into one layer) Spatial Multiplexing equalizer
static int srslte_predecoding_multiplex_2x1_mrc(cf_t* y[SRSLTE_MAX_PORTS],
                                                cf_t* h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                                cf_t* x[SRSLTE_MAX_LAYERS],
                                                int   codebook_idx,
                                                int   nof_symbols,
                                                float scaling)
{
  float norm = (float)M_SQRT2 / scaling;
  int   i    = 0;

#if SRSLTE_SIMD_CF_SIZE != 0
  simd_f_t _norm = srslte_simd_f_set1(norm);

  for (; i < nof_symbols - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
    simd_cf_t x0 = srslte_simd_cf_set1(0.0f);
    simd_f_t  hh = srslte_simd_f_set1(0.0f);

    for (int k = 0; k < 2; k++) {
      simd_cf_t h0xi = srslte_simd_cfi_load(&h[0][k][i]);
      simd_cf_t h1xi = srslte_simd_cfi_load(&h[1][k][i]);
      simd_cf_t yx   = srslte_simd_cfi_load(&y[k][i]);

      simd_cf_t hx;
      switch (codebook_idx) {
        case 0:
          hx = srslte_simd_cf_add(h0xi, h1xi);
          break;
        case 1:
          hx = srslte_simd_cf_sub(h0xi, h1xi);
          break;
        case 2:
          hx = srslte_simd_cf_add(h0xi, srslte_simd_cf_mulj(h1xi));
          break;
        case 3:
          hx = srslte_simd_cf_sub(h0xi, srslte_simd_cf_mulj(h1xi));
          break;
        default:
          ERROR("Wrong codebook_idx=%d\n", codebook_idx);
          return SRSLTE_ERROR;
      }

      hh = srslte_simd_f_add(srslte_simd_cf_re(srslte_simd_cf_conjprod(hx, hx)), hh);
      x0 = srslte_simd_cf_add(srslte_simd_cf_conjprod(yx, hx), x0);
    }

    hh = srslte_simd_f_mul(_norm, srslte_simd_f_rcp(hh));
    srslte_simd_cfi_store(&x[0][i], srslte_simd_cf_mul(x0, hh));
  }
#endif /* SRSLTE_SIMD_CF_SIZE */

  for (; i < nof_symbols; i += 1) {
    cf_t  h0, h1;
    float hh;

    switch (codebook_idx) {
      case 0:
        h0 = h[0][0][i] + h[1][0][i];
        h1 = h[0][1][i] + h[1][1][i];
        break;
      case 1:
        h0 = h[0][0][i] - h[1][0][i];
        h1 = h[0][1][i] - h[1][1][i];
        break;
      case 2:
        h0 = h[0][0][i] + _Complex_I * h[1][0][i];
        h1 = h[0][1][i] + _Complex_I * h[1][1][i];
        break;
      case 3:
        h0 = h[0][0][i] - _Complex_I * h[1][0][i];
        h1 = h[0][1][i] - _Complex_I * h[1][1][i];
        break;
      default:
        ERROR("Wrong codebook_idx=%d\n", codebook_idx);
        return SRSLTE_ERROR;
    }

    hh = norm / (crealf(h0) * crealf(h0) + cimagf(h0) * cimagf(h0) + crealf(h1) * crealf(h1) + cimagf(h1) * cimagf(h1));

    x[0][i] = (conjf(h0) * y[0][i] + conjf(h1) * y[1][i]) * hh;
  }
  return SRSLTE_SUCCESS;
}

// Generic implementation of MRC 2x1 (two antennas into one layer) Spatial Multiplexing equalizer
static int srslte_predecoding_multiplex_2x1_mrc_csi(cf_t*  y[SRSLTE_MAX_PORTS],
                                                    cf_t*  h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                                    cf_t*  x[SRSLTE_MAX_LAYERS],
                                                    float* csi,
                                                    int    codebook_idx,
                                                    int    nof_symbols,
                                                    float  scaling)
{
  float norm = (float)M_SQRT2 / scaling;
  int   i    = 0;

#if SRSLTE_SIMD_CF_SIZE != 0
  simd_f_t _norm = srslte_simd_f_set1(norm);

  for (; i < nof_symbols - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
    simd_cf_t x0 = srslte_simd_cf_set1(0.0f);
    simd_f_t  hh = srslte_simd_f_set1(0.0f);

    for (int k = 0; k < 2; k++) {
      simd_cf_t h0xi = srslte_simd_cfi_load(&h[0][k][i]);
      simd_cf_t h1xi = srslte_simd_cfi_load(&h[1][k][i]);
      simd_cf_t yx   = srslte_simd_cfi_load(&y[k][i]);

      simd_cf_t hx;
      switch (codebook_idx) {
        case 0:
          hx = srslte_simd_cf_add(h0xi, h1xi);
          break;
        case 1:
          hx = srslte_simd_cf_sub(h0xi, h1xi);
          break;
        case 2:
          hx = srslte_simd_cf_add(h0xi, srslte_simd_cf_mulj(h1xi));
          break;
        case 3:
          hx = srslte_simd_cf_sub(h0xi, srslte_simd_cf_mulj(h1xi));
          break;
        default:
          ERROR("Wrong codebook_idx=%d\n", codebook_idx);
          return SRSLTE_ERROR;
      }

      hh = srslte_simd_f_add(srslte_simd_cf_re(srslte_simd_cf_conjprod(hx, hx)), hh);
      x0 = srslte_simd_cf_add(srslte_simd_cf_conjprod(yx, hx), x0);
    }

    hh = srslte_simd_f_mul(_norm, srslte_simd_f_rcp(hh));
    srslte_simd_cfi_store(&x[0][i], srslte_simd_cf_mul(x0, hh));
    srslte_simd_f_store(&csi[i], srslte_simd_f_mul(srslte_simd_f_rcp(hh), srslte_simd_f_set1((float)M_SQRT1_2)));
  }
#endif /* SRSLTE_SIMD_CF_SIZE */

  for (; i < nof_symbols; i += 1) {
    cf_t  h0, h1;
    float hh, _csi;

    switch (codebook_idx) {
      case 0:
        h0 = h[0][0][i] + h[1][0][i];
        h1 = h[0][1][i] + h[1][1][i];
        break;
      case 1:
        h0 = h[0][0][i] - h[1][0][i];
        h1 = h[0][1][i] - h[1][1][i];
        break;
      case 2:
        h0 = h[0][0][i] + _Complex_I * h[1][0][i];
        h1 = h[0][1][i] + _Complex_I * h[1][1][i];
        break;
      case 3:
        h0 = h[0][0][i] - _Complex_I * h[1][0][i];
        h1 = h[0][1][i] - _Complex_I * h[1][1][i];
        break;
      default:
        ERROR("Wrong codebook_idx=%d\n", codebook_idx);
        return SRSLTE_ERROR;
    }

    _csi = crealf(h0) * crealf(h0) + cimagf(h0) * cimagf(h0) + crealf(h1) * crealf(h1) + cimagf(h1) * cimagf(h1);
    hh   = norm / _csi;

    x[0][i] = (conjf(h0) * y[0][i] + conjf(h1) * y[1][i]) * hh;
    csi[i]  = _csi / norm * (float)M_SQRT1_2;
  }
  return SRSLTE_SUCCESS;
}

static int srslte_predecoding_multiplex(cf_t*  y[SRSLTE_MAX_PORTS],
                                        cf_t*  h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                        cf_t*  x[SRSLTE_MAX_LAYERS],
                                        float* csi[SRSLTE_MAX_CODEWORDS],
                                        int    nof_rxant,
                                        int    nof_ports,
                                        int    nof_layers,
                                        int    codebook_idx,
                                        int    nof_symbols,
                                        float  scaling,
                                        float  noise_estimate)
{
  if (nof_ports == 2 && nof_rxant <= 2) {
    if (nof_layers == 2) {
      switch (mimo_decoder) {
        case SRSLTE_MIMO_DECODER_ZF:
          if (csi && csi[0]) {
            return srslte_predecoding_multiplex_2x2_zf_csi(y, h, x, csi[0], codebook_idx, nof_symbols, scaling);
          } else {
            return srslte_predecoding_multiplex_2x2_zf(y, h, x, codebook_idx, nof_symbols, scaling);
          }
          break;
        case SRSLTE_MIMO_DECODER_MMSE:
          if (csi && csi[0]) {
            return srslte_predecoding_multiplex_2x2_mmse_csi(
                y, h, x, csi, codebook_idx, nof_symbols, scaling, noise_estimate);
          } else {
            return srslte_predecoding_multiplex_2x2_mmse(y, h, x, codebook_idx, nof_symbols, scaling, noise_estimate);
          }
          break;
      }
    } else {
      if (csi && csi[0]) {
        return srslte_predecoding_multiplex_2x1_mrc_csi(y, h, x, csi[0], codebook_idx, nof_symbols, scaling);
      } else {
        return srslte_predecoding_multiplex_2x1_mrc(y, h, x, codebook_idx, nof_symbols, scaling);
      }
    }
  } else if (nof_ports == 4) {
    ERROR("Error predecoding multiplex: not implemented for %d Tx ports", nof_ports);
  } else {
    ERROR("Error predecoding multiplex: Invalid combination of ports %d and rx antennas %d\n", nof_ports, nof_rxant);
  }
  return SRSLTE_ERROR;
}

void srslte_predecoding_set_mimo_decoder(srslte_mimo_decoder_t _mimo_decoder)
{
  mimo_decoder = _mimo_decoder;
}

/* 36.211 v10.3.0 Section 6.3.4 */
int srslte_predecoding_type(cf_t*              y[SRSLTE_MAX_PORTS],
                            cf_t*              h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                            cf_t*              x[SRSLTE_MAX_LAYERS],
                            float*             csi[SRSLTE_MAX_CODEWORDS],
                            int                nof_rxant,
                            int                nof_ports,
                            int                nof_layers,
                            int                codebook_idx,
                            int                nof_symbols,
                            srslte_tx_scheme_t type,
                            float              scaling,
                            float              noise_estimate)
{

  if (nof_ports > SRSLTE_MAX_PORTS) {
    ERROR("Maximum number of ports is %d (nof_ports=%d)\n", SRSLTE_MAX_PORTS, nof_ports);
    return SRSLTE_ERROR;
  }
  if (nof_layers > SRSLTE_MAX_LAYERS) {
    ERROR("Maximum number of layers is %d (nof_layers=%d)\n", SRSLTE_MAX_LAYERS, nof_layers);
    return SRSLTE_ERROR;
  }

  switch (type) {
    case SRSLTE_TXSCHEME_CDD:
      if (nof_layers == 2) {
        switch (mimo_decoder) {
          case SRSLTE_MIMO_DECODER_ZF:
            return srslte_predecoding_ccd_zf(y, h, x, csi, nof_rxant, nof_ports, nof_layers, nof_symbols, scaling);
          case SRSLTE_MIMO_DECODER_MMSE:
            return srslte_predecoding_ccd_mmse(
                y, h, x, csi, nof_rxant, nof_ports, nof_layers, nof_symbols, scaling, noise_estimate);
        }
      } else {
        ERROR("Invalid number of layers %d\n", nof_layers);
        return SRSLTE_ERROR;
      }
      return SRSLTE_ERROR;
    case SRSLTE_TXSCHEME_PORT0:
      if (nof_ports == 1 && nof_layers == 1) {
        return srslte_predecoding_single_multi(y, h[0], x[0], csi, nof_rxant, nof_symbols, scaling, noise_estimate);
      } else {
        ERROR("Number of ports and layers must be 1 for transmission on single antenna ports (%d, %d)\n",
              nof_ports,
              nof_layers);
        return SRSLTE_ERROR;
      }
    case SRSLTE_TXSCHEME_DIVERSITY:
      if (nof_ports == nof_layers) {
        return srslte_predecoding_diversity_multi(y, h, x, csi, nof_rxant, nof_ports, nof_symbols, scaling);
      } else {
        ERROR("Error number of layers must equal number of ports in transmit diversity\n");
        return SRSLTE_ERROR;
      }
    case SRSLTE_TXSCHEME_SPATIALMUX:
      return srslte_predecoding_multiplex(
          y, h, x, csi, nof_rxant, nof_ports, nof_layers, codebook_idx, nof_symbols, scaling, noise_estimate);
    default:
      ERROR("Invalid Txscheme=%d\n", type);
      return SRSLTE_ERROR;
  }
}

/************************************************
 *
 * TRANSMITTER SIDE FUNCTIONS
 *
 **************************************************/

int srslte_precoding_single(cf_t* x, cf_t* y, int nof_symbols, float scaling)
{
  if (scaling == 1.0f) {
    memcpy(y, x, nof_symbols * sizeof(cf_t));
  } else {
    srslte_vec_sc_prod_cfc(x, scaling, y, (uint32_t)nof_symbols);
  }
  return nof_symbols;
}
int srslte_precoding_diversity(cf_t* x[SRSLTE_MAX_LAYERS],
                               cf_t* y[SRSLTE_MAX_PORTS],
                               int   nof_ports,
                               int   nof_symbols,
                               float scaling)
{
  int i;
  if (nof_ports == 2) {
    for (i = 0; i < nof_symbols; i++) {
      y[0][2 * i]     = x[0][i];
      y[1][2 * i]     = -conjf(x[1][i]);
      y[0][2 * i + 1] = x[1][i];
      y[1][2 * i + 1] = conjf(x[0][i]);
    }
    // normalize
    srslte_vec_sc_prod_cfc(y[0], scaling * M_SQRT1_2, y[0], 2 * nof_symbols);
    srslte_vec_sc_prod_cfc(y[1], scaling * M_SQRT1_2, y[1], 2 * nof_symbols);
    return 2 * i;
  } else if (nof_ports == 4) {
    scaling /= M_SQRT2;

    // int m_ap = (nof_symbols%4)?(nof_symbols*4-2):nof_symbols*4;
    int m_ap = 4 * nof_symbols;
    for (i = 0; i < m_ap / 4; i++) {
      y[0][4 * i] = x[0][i] * scaling;
      y[1][4 * i] = 0;
      y[2][4 * i] = -conjf(x[1][i]) * scaling;
      y[3][4 * i] = 0;

      y[0][4 * i + 1] = x[1][i] * scaling;
      y[1][4 * i + 1] = 0;
      y[2][4 * i + 1] = conjf(x[0][i]) * scaling;
      y[3][4 * i + 1] = 0;

      y[0][4 * i + 2] = 0;
      y[1][4 * i + 2] = x[2][i] * scaling;
      y[2][4 * i + 2] = 0;
      y[3][4 * i + 2] = -conjf(x[3][i]) * scaling;

      y[0][4 * i + 3] = 0;
      y[1][4 * i + 3] = x[3][i] * scaling;
      y[2][4 * i + 3] = 0;
      y[3][4 * i + 3] = conjf(x[2][i]) * scaling;
    }
    return 4 * i;
  } else {
    ERROR("Number of ports must be 2 or 4 for transmit diversity (nof_ports=%d)\n", nof_ports);
    return -1;
  }
}

#ifdef LV_HAVE_AVX

int srslte_precoding_cdd_2x2_avx(cf_t* x[SRSLTE_MAX_LAYERS], cf_t* y[SRSLTE_MAX_PORTS], int nof_symbols, float scaling)
{
  __m256 norm_avx = _mm256_set1_ps(0.5f * scaling);
  for (int i = 0; i < nof_symbols - 3; i += 4) {
    __m256 x0 = _mm256_load_ps((float*)&x[0][i]);
    __m256 x1 = _mm256_load_ps((float*)&x[1][i]);

    __m256 y0 = _mm256_mul_ps(norm_avx, _mm256_add_ps(x0, x1));

    x0 = _mm256_xor_ps(x0, _mm256_setr_ps(+0.0f, +0.0f, -0.0f, -0.0f, +0.0f, +0.0f, -0.0f, -0.0f));
    x1 = _mm256_xor_ps(x1, _mm256_set_ps(+0.0f, +0.0f, -0.0f, -0.0f, +0.0f, +0.0f, -0.0f, -0.0f));

    __m256 y1 = _mm256_mul_ps(norm_avx, _mm256_add_ps(x0, x1));

    _mm256_store_ps((float*)&y[0][i], y0);
    _mm256_store_ps((float*)&y[1][i], y1);
  }

  return 2 * nof_symbols;
}

#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE

int srslte_precoding_cdd_2x2_sse(cf_t* x[SRSLTE_MAX_LAYERS], cf_t* y[SRSLTE_MAX_PORTS], int nof_symbols, float scaling)
{
  __m128 norm_sse = _mm_set1_ps(0.5f * scaling);
  for (int i = 0; i < nof_symbols - 1; i += 2) {
    __m128 x0 = _mm_load_ps((float*)&x[0][i]);
    __m128 x1 = _mm_load_ps((float*)&x[1][i]);

    __m128 y0 = _mm_mul_ps(norm_sse, _mm_add_ps(x0, x1));

    x0 = _mm_xor_ps(x0, _mm_setr_ps(+0.0f, +0.0f, -0.0f, -0.0f));
    x1 = _mm_xor_ps(x1, _mm_set_ps(+0.0f, +0.0f, -0.0f, -0.0f));

    __m128 y1 = _mm_mul_ps(norm_sse, _mm_add_ps(x0, x1));

    _mm_store_ps((float*)&y[0][i], y0);
    _mm_store_ps((float*)&y[1][i], y1);
  }

  return 2 * nof_symbols;
}

#endif /* LV_HAVE_SSE */

int srslte_precoding_cdd_2x2_gen(cf_t* x[SRSLTE_MAX_LAYERS], cf_t* y[SRSLTE_MAX_PORTS], int nof_symbols, float scaling)
{
  scaling /= 2.0f;
  for (int i = 0; i < nof_symbols; i++) {
    y[0][i] = (x[0][i] + x[1][i]) * scaling;
    y[1][i] = (x[0][i] - x[1][i]) * scaling;
    i++;
    y[0][i] = (x[0][i] + x[1][i]) * scaling;
    y[1][i] = (-x[0][i] + x[1][i]) * scaling;
  }
  return 2 * nof_symbols;
}

int srslte_precoding_cdd(cf_t* x[SRSLTE_MAX_LAYERS],
                         cf_t* y[SRSLTE_MAX_PORTS],
                         int   nof_layers,
                         int   nof_ports,
                         int   nof_symbols,
                         float scaling)
{
  if (nof_ports == 2) {
    if (nof_layers != 2) {
      ERROR("Invalid number of layers %d for 2 ports\n", nof_layers);
      return -1;
    }
#ifdef LV_HAVE_AVX
    return srslte_precoding_cdd_2x2_avx(x, y, nof_symbols, scaling);
#else
#ifdef LV_HAVE_SSE
    return srslte_precoding_cdd_2x2_sse(x, y, nof_symbols, scaling);
#else
    return srslte_precoding_cdd_2x2_gen(x, y, nof_symbols, scaling);
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX */
  } else if (nof_ports == 4) {
    ERROR("Not implemented\n");
    return -1;
  } else {
    ERROR("Number of ports must be 2 or 4 for transmit diversity (nof_ports=%d)\n", nof_ports);
    return -1;
  }
}

int srslte_precoding_multiplex(cf_t*    x[SRSLTE_MAX_LAYERS],
                               cf_t*    y[SRSLTE_MAX_PORTS],
                               int      nof_layers,
                               int      nof_ports,
                               int      codebook_idx,
                               uint32_t nof_symbols,
                               float    scaling)
{
  int i = 0;
  if (nof_ports == 2) {
    if (nof_layers == 1) {
      scaling *= M_SQRT1_2;
      switch (codebook_idx) {
        case 0:
          srslte_vec_sc_prod_cfc(x[0], scaling, y[0], nof_symbols);
          srslte_vec_sc_prod_cfc(x[0], scaling, y[1], nof_symbols);
          break;
        case 1:
          srslte_vec_sc_prod_cfc(x[0], scaling, y[0], nof_symbols);
          srslte_vec_sc_prod_cfc(x[0], -scaling, y[1], nof_symbols);
          break;
        case 2:
          srslte_vec_sc_prod_cfc(x[0], scaling, y[0], nof_symbols);
          srslte_vec_sc_prod_ccc(x[0], _Complex_I * scaling, y[1], nof_symbols);
          break;
        case 3:
          srslte_vec_sc_prod_cfc(x[0], scaling, y[0], nof_symbols);
          srslte_vec_sc_prod_ccc(x[0], -_Complex_I * scaling, y[1], nof_symbols);
          break;
        default:
          ERROR("Invalid multiplex combination: codebook_idx=%d, nof_layers=%d, nof_ports=%d\n",
                codebook_idx,
                nof_layers,
                nof_ports);
          return SRSLTE_ERROR;
      }
    } else if (nof_layers == 2) {
      switch (codebook_idx) {
        case 0:
          scaling *= M_SQRT1_2;
          srslte_vec_sc_prod_cfc(x[0], scaling, y[0], nof_symbols);
          srslte_vec_sc_prod_cfc(x[1], scaling, y[1], nof_symbols);
          break;
        case 1:
          scaling /= 2.0f;
#ifdef LV_HAVE_AVX
          for (; i < nof_symbols - 3; i += 4) {
            __m256 x0 = _mm256_load_ps((float*)&x[0][i]);
            __m256 x1 = _mm256_load_ps((float*)&x[1][i]);

            __m256 y0 = _mm256_mul_ps(_mm256_set1_ps(scaling), _mm256_add_ps(x0, x1));
            __m256 y1 = _mm256_mul_ps(_mm256_set1_ps(scaling), _mm256_sub_ps(x0, x1));

            _mm256_store_ps((float*)&y[0][i], y0);
            _mm256_store_ps((float*)&y[1][i], y1);
          }
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE
          for (; i < nof_symbols - 1; i += 2) {
            __m128 x0 = _mm_load_ps((float*)&x[0][i]);
            __m128 x1 = _mm_load_ps((float*)&x[1][i]);

            __m128 y0 = _mm_mul_ps(_mm_set1_ps(scaling), _mm_add_ps(x0, x1));
            __m128 y1 = _mm_mul_ps(_mm_set1_ps(scaling), _mm_sub_ps(x0, x1));

            _mm_store_ps((float*)&y[0][i], y0);
            _mm_store_ps((float*)&y[1][i], y1);
          }
#endif /* LV_HAVE_SSE */

          for (; i < nof_symbols; i++) {
            y[0][i] = (x[0][i] + x[1][i]) * scaling;
            y[1][i] = (x[0][i] - x[1][i]) * scaling;
          }
          break;
        case 2:
          scaling /= 2.0f;
#ifdef LV_HAVE_AVX
          for (; i < nof_symbols - 3; i += 4) {
            __m256 x0 = _mm256_load_ps((float*)&x[0][i]);
            __m256 x1 = _mm256_load_ps((float*)&x[1][i]);

            __m256 y0 = _mm256_mul_ps(_mm256_set1_ps(scaling), _mm256_add_ps(x0, x1));
            __m256 y1 = _mm256_mul_ps(_mm256_set1_ps(scaling), _MM256_MULJ_PS(_mm256_sub_ps(x0, x1)));

            _mm256_store_ps((float*)&y[0][i], y0);
            _mm256_store_ps((float*)&y[1][i], y1);
          }
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_SSE
          for (; i < nof_symbols - 1; i += 2) {
            __m128 x0 = _mm_load_ps((float*)&x[0][i]);
            __m128 x1 = _mm_load_ps((float*)&x[1][i]);

            __m128 y0 = _mm_mul_ps(_mm_set1_ps(scaling), _mm_add_ps(x0, x1));
            __m128 y1 = _mm_mul_ps(_mm_set1_ps(scaling), _MM_MULJ_PS(_mm_sub_ps(x0, x1)));

            _mm_store_ps((float*)&y[0][i], y0);
            _mm_store_ps((float*)&y[1][i], y1);
          }
#endif /* LV_HAVE_SSE */

          for (; i < nof_symbols; i++) {
            y[0][i] = (x[0][i] + x[1][i]) * scaling;
            y[1][i] = (_Complex_I * x[0][i] - _Complex_I * x[1][i]) * scaling;
          }
          break;
        case 3:
        default:
          ERROR("Invalid multiplex combination: codebook_idx=%d, nof_layers=%d, nof_ports=%d\n",
                codebook_idx,
                nof_layers,
                nof_ports);
          return SRSLTE_ERROR;
      }
    } else {
      ERROR("Not implemented");
    }
  } else {
    ERROR("Not implemented");
  }
  return SRSLTE_SUCCESS;
}

/* 36.211 v10.3.0 Section 6.3.4 */
int srslte_precoding_type(cf_t*              x[SRSLTE_MAX_LAYERS],
                          cf_t*              y[SRSLTE_MAX_PORTS],
                          int                nof_layers,
                          int                nof_ports,
                          int                codebook_idx,
                          int                nof_symbols,
                          float              scaling,
                          srslte_tx_scheme_t type)
{

  if (nof_ports > SRSLTE_MAX_PORTS) {
    ERROR("Maximum number of ports is %d (nof_ports=%d)\n", SRSLTE_MAX_PORTS, nof_ports);
    return -1;
  }
  if (nof_layers > SRSLTE_MAX_LAYERS) {
    ERROR("Maximum number of layers is %d (nof_layers=%d)\n", SRSLTE_MAX_LAYERS, nof_layers);
    return -1;
  }

  switch (type) {
    case SRSLTE_TXSCHEME_CDD:
      return srslte_precoding_cdd(x, y, nof_layers, nof_ports, nof_symbols, scaling);
    case SRSLTE_TXSCHEME_PORT0:
      if (nof_ports == 1 && nof_layers == 1) {
        return srslte_precoding_single(x[0], y[0], nof_symbols, scaling);
      } else {
        ERROR("Number of ports and layers must be 1 for transmission on single antenna ports\n");
        return -1;
      }
      break;
    case SRSLTE_TXSCHEME_DIVERSITY:
      if (nof_ports == nof_layers) {
        return srslte_precoding_diversity(x, y, nof_ports, nof_symbols, scaling);
      } else {
        ERROR("Error number of layers must equal number of ports in transmit diversity\n");
        return -1;
      }
    case SRSLTE_TXSCHEME_SPATIALMUX:
      return srslte_precoding_multiplex(x, y, nof_layers, nof_ports, codebook_idx, (uint32_t)nof_symbols, scaling);
    default:
      return SRSLTE_ERROR;
  }
  return SRSLTE_ERROR;
}

#define PMI_SEL_PRECISION 24

/* PMI Select for 1 layer */
int srslte_precoding_pmi_select_1l_gen(cf_t*     h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                       uint32_t  nof_symbols,
                                       float     noise_estimate,
                                       uint32_t* pmi,
                                       float     sinr_list[SRSLTE_MAX_CODEBOOKS])
{

#define SQRT1_2 ((float)M_SQRT1_2)
  float    max_sinr = 0.0;
  uint32_t i, count;

  for (i = 0; i < 4; i++) {
    sinr_list[i] = 0;
    count        = 0;

    for (uint32_t j = 0; j < nof_symbols; j += PMI_SEL_PRECISION) {
      /* 0. Load channel matrix */
      cf_t h00 = h[0][0][j];
      cf_t h01 = h[1][0][j];
      cf_t h10 = h[0][1][j];
      cf_t h11 = h[1][1][j];

      /* 1. B = W'* H' */
      cf_t a0, a1;
      switch (i) {
        case 0:
          a0 = conjf(h00) + conjf(h01);
          a1 = conjf(h10) + conjf(h11);
          break;
        case 1:
          a0 = conjf(h00) - conjf(h01);
          a1 = conjf(h10) - conjf(h11);
          break;
        case 2:
          a0 = conjf(h00) - _Complex_I * conjf(h01);
          a1 = conjf(h10) - _Complex_I * conjf(h11);
          break;
        case 3:
          a0 = conjf(h00) + _Complex_I * conjf(h01);
          a1 = conjf(h10) + _Complex_I * conjf(h11);
          break;
      }
      a0 *= SQRT1_2;
      a1 *= SQRT1_2;

      /* 2. B = W' * H' * H = A * H */
      cf_t b0 = a0 * h00 + a1 * h10;
      cf_t b1 = a0 * h01 + a1 * h11;

      /* 3. C = W' * H' * H * W' = B * W */
      cf_t c;
      switch (i) {
        case 0:
          c = b0 + b1;
          break;
        case 1:
          c = b0 - b1;
          break;
        case 2:
          c = b0 + _Complex_I * b1;
          break;
        case 3:
          c = b0 - _Complex_I * b1;
          break;
        default:
          return SRSLTE_ERROR;
      }
      c *= SQRT1_2;

      /* Add for averaging */
      sinr_list[i] += crealf(c);

      count++;
    }

    /* Divide average by noise */
    sinr_list[i] /= noise_estimate * count;

    if (sinr_list[i] > max_sinr) {
      max_sinr = sinr_list[i];
      *pmi     = i;
    }
  }

  return i;
}

#ifdef SRSLTE_SIMD_CF_SIZE

/* PMI Select for 1 layer */
int srslte_precoding_pmi_select_1l_simd(cf_t*     h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                        uint32_t  nof_symbols,
                                        float     noise_estimate,
                                        uint32_t* pmi,
                                        float     sinr_list[SRSLTE_MAX_CODEBOOKS])
{
  float    max_sinr    = 0.0;
  simd_f_t simd_f_norm = srslte_simd_f_set1(0.5f);

  for (uint32_t i = 0; i < 4; i++) {
    float sinr_acc = 0;
    float count    = 0;

    for (uint32_t j = 0; j < nof_symbols - PMI_SEL_PRECISION * SRSLTE_SIMD_CF_SIZE + 1;
         j += PMI_SEL_PRECISION * SRSLTE_SIMD_CF_SIZE) {
      // 0. Load channel matrix
      srslte_simd_aligned cf_t h00_v[SRSLTE_SIMD_CF_SIZE];
      srslte_simd_aligned cf_t h01_v[SRSLTE_SIMD_CF_SIZE];
      srslte_simd_aligned cf_t h10_v[SRSLTE_SIMD_CF_SIZE];
      srslte_simd_aligned cf_t h11_v[SRSLTE_SIMD_CF_SIZE];

      for (uint32_t k = 0; k < SRSLTE_SIMD_CF_SIZE; k++) {
        h00_v[k] = h[0][0][j + PMI_SEL_PRECISION * k];
        h01_v[k] = h[1][0][j + PMI_SEL_PRECISION * k];
        h10_v[k] = h[0][1][j + PMI_SEL_PRECISION * k];
        h11_v[k] = h[1][1][j + PMI_SEL_PRECISION * k];
      }

      simd_cf_t h00 = srslte_simd_cfi_load(h00_v);
      simd_cf_t h01 = srslte_simd_cfi_load(h01_v);
      simd_cf_t h10 = srslte_simd_cfi_load(h10_v);
      simd_cf_t h11 = srslte_simd_cfi_load(h11_v);

      /* 1. B = W'* H' */
      simd_cf_t a0, a1;
      switch (i) {
        case 0:
          a0 = srslte_simd_cf_add(srslte_simd_cf_conj(h00), srslte_simd_cf_conj(h01));
          a1 = srslte_simd_cf_add(srslte_simd_cf_conj(h10), srslte_simd_cf_conj(h11));
          break;
        case 1:
          a0 = srslte_simd_cf_sub(srslte_simd_cf_conj(h00), srslte_simd_cf_conj(h01));
          a1 = srslte_simd_cf_sub(srslte_simd_cf_conj(h10), srslte_simd_cf_conj(h11));
          break;
        case 2:
          a0 = srslte_simd_cf_sub(srslte_simd_cf_conj(h00), srslte_simd_cf_mulj(srslte_simd_cf_conj(h01)));
          a1 = srslte_simd_cf_sub(srslte_simd_cf_conj(h10), srslte_simd_cf_mulj(srslte_simd_cf_conj(h11)));
          break;
        default:
          a0 = srslte_simd_cf_add(srslte_simd_cf_conj(h00), srslte_simd_cf_mulj(srslte_simd_cf_conj(h01)));
          a1 = srslte_simd_cf_add(srslte_simd_cf_conj(h10), srslte_simd_cf_mulj(srslte_simd_cf_conj(h11)));
          break;
      }

      /* 2. B = W' * H' * H = A * H */
      simd_cf_t b0 = srslte_simd_cf_add(srslte_simd_cf_prod(a0, h00), srslte_simd_cf_prod(a1, h10));
      simd_cf_t b1 = srslte_simd_cf_add(srslte_simd_cf_prod(a0, h01), srslte_simd_cf_prod(a1, h11));

      /* 3. C = W' * H' * H * W' = B * W */
      simd_cf_t c;
      switch (i) {
        case 0:
          c = srslte_simd_cf_add(b0, b1);
          break;
        case 1:
          c = srslte_simd_cf_sub(b0, b1);
          break;
        case 2:
          c = srslte_simd_cf_add(b0, srslte_simd_cf_mulj(b1));
          break;
        case 3:
          c = srslte_simd_cf_sub(b0, srslte_simd_cf_mulj(b1));
          break;
        default:
          return SRSLTE_ERROR;
      }

      simd_f_t gamma = srslte_simd_f_mul(srslte_simd_cf_re(c), simd_f_norm);

      // Horizontal accumulation
      for (int k = 1; k < SRSLTE_SIMD_F_SIZE; k *= 2) {
        gamma = srslte_simd_f_hadd(gamma, gamma);
      }

      // Temporal store accumulated values
      srslte_simd_aligned float v[SRSLTE_SIMD_F_SIZE];
      srslte_simd_f_store(v, gamma);

      // Average and accumulate SINR loop
      sinr_acc += (v[0] / SRSLTE_SIMD_CF_SIZE);

      // Increase loop counter
      count += 1;
    }

    // Average accumulated SINR
    if (count) {
      sinr_acc /= (noise_estimate * count);
    } else {
      sinr_acc = 1e+9f;
    }

    // Save SINR if available
    if (sinr_list) {
      sinr_list[i] = sinr_acc;
    }

    // Select maximum SINR Codebook
    if (pmi && sinr_acc > max_sinr) {
      max_sinr = sinr_acc;
      *pmi     = i;
    }
  }

  return 4;
}

#endif /* SRSLTE_SIMD_CF_SIZE */

int srslte_precoding_pmi_select_1l(cf_t*     h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                   uint32_t  nof_symbols,
                                   float     noise_estimate,
                                   uint32_t* pmi,
                                   float     sinr_list[SRSLTE_MAX_CODEBOOKS])
{
  int ret;
#ifdef SRSLTE_SIMD_CF_SIZE
  ret = srslte_precoding_pmi_select_1l_simd(h, nof_symbols, noise_estimate, pmi, sinr_list);
#else
  ret = srslte_precoding_pmi_select_1l_gen(h, nof_symbols, noise_estimate, pmi, sinr_list);
#endif /* SRSLTE_SIMD_CF_SIZE */
  INFO("Precoder PMI Select for 1 layer SINR=[%.1fdB; %.1fdB; %.1fdB; %.1fdB] PMI=%d\n",
       srslte_convert_power_to_dB(sinr_list[0]),
       srslte_convert_power_to_dB(sinr_list[1]),
       srslte_convert_power_to_dB(sinr_list[2]),
       srslte_convert_power_to_dB(sinr_list[3]),
       *pmi);

  return ret;
}

int srslte_precoding_pmi_select_2l_gen(cf_t*     h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                       uint32_t  nof_symbols,
                                       float     noise_estimate,
                                       uint32_t* pmi,
                                       float     sinr_list[SRSLTE_MAX_CODEBOOKS])
{

  float    max_sinr = 0.0;
  uint32_t i, count;

  for (i = 0; i < 2; i++) {
    sinr_list[i] = 0;
    count        = 0;

    for (uint32_t j = 0; j < nof_symbols; j += PMI_SEL_PRECISION) {
      /* 0. Load channel matrix */
      cf_t h00 = h[0][0][j];
      cf_t h01 = h[1][0][j];
      cf_t h10 = h[0][1][j];
      cf_t h11 = h[1][1][j];

      /* 1. B = W'* H' */
      cf_t a00, a01, a10, a11;
      switch (i) {
        case 0:
          a00 = conjf(h00) + conjf(h01);
          a01 = conjf(h10) + conjf(h11);
          a10 = conjf(h00) - conjf(h01);
          a11 = conjf(h10) - conjf(h11);
          break;
        case 1:
          a00 = conjf(h00) - _Complex_I * conjf(h01);
          a01 = conjf(h10) - _Complex_I * conjf(h11);
          a10 = conjf(h00) + _Complex_I * conjf(h01);
          a11 = conjf(h10) + _Complex_I * conjf(h11);
          break;
        default:
          return SRSLTE_ERROR;
      }

      /* 2. B = W' * H' * H = A * H */
      cf_t b00 = a00 * h00 + a01 * h10;
      cf_t b01 = a00 * h01 + a01 * h11;
      cf_t b10 = a10 * h00 + a11 * h10;
      cf_t b11 = a10 * h01 + a11 * h11;

      /* 3. C = W' * H' * H * W' = B * W */
      cf_t c00, c01, c10, c11;
      switch (i) {
        case 0:
          c00 = b00 + b01;
          c01 = b00 - b01;
          c10 = b10 + b11;
          c11 = b10 - b11;
          break;
        case 1:
          c00 = b00 + _Complex_I * b01;
          c01 = b00 - _Complex_I * b01;
          c10 = b10 + _Complex_I * b11;
          c11 = b10 - _Complex_I * b11;
          break;
        default:
          return SRSLTE_ERROR;
      }
      c00 *= 0.25;
      c01 *= 0.25;
      c10 *= 0.25;
      c11 *= 0.25;

      /* 4. C += noise * I */
      c00 += noise_estimate;
      c11 += noise_estimate;

      /* 5. detC */
      cf_t detC     = c00 * c11 - c01 * c10;
      cf_t inv_detC = conjf(detC) / (crealf(detC) * crealf(detC) + cimagf(detC) * cimagf(detC));

      cf_t den0 = noise_estimate * c00 * inv_detC;
      cf_t den1 = noise_estimate * c11 * inv_detC;

      float gamma0 = crealf((conjf(den0) / (crealf(den0) * crealf(den0) + cimagf(den0) * cimagf(den0))) - 1);
      float gamma1 = crealf((conjf(den1) / (crealf(den1) * crealf(den1) + cimagf(den1) * cimagf(den1))) - 1);

      /* Add for averaging */
      sinr_list[i] += (gamma0 + gamma1);

      count++;
    }

    /* Divide average by noise */
    if (count) {
      sinr_list[i] /= count;
    }

    if (sinr_list[i] > max_sinr) {
      max_sinr = sinr_list[i];
      *pmi     = i;
    }
  }

  return i;
}

#ifdef SRSLTE_SIMD_CF_SIZE

int srslte_precoding_pmi_select_2l_simd(cf_t*     h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                        int       nof_symbols,
                                        float     noise_estimate,
                                        uint32_t* pmi,
                                        float     sinr_list[SRSLTE_MAX_CODEBOOKS])
{
  // SIMD Constants
  const simd_cf_t simd_cf_noise_estimate = srslte_simd_cf_set1(noise_estimate);
  const simd_f_t  simd_f_noise_estimate  = srslte_simd_f_set1(noise_estimate);
  const simd_f_t  simd_f_norm            = srslte_simd_f_set1(0.25f);
  const simd_f_t  simd_f_ones            = srslte_simd_f_set1(1.0f);
  const simd_f_t  simd_f_det_min         = srslte_simd_f_set1(1e-10f);
  const simd_f_t  simd_f_gamma_min       = srslte_simd_f_set1(1e-9f);

  float max_sinr = 0.0f;

  for (uint32_t i = 0; i < 2; i++) {
    float count    = 0.0f;
    float sinr_acc = 0.0f;

    for (uint32_t j = 0; j < nof_symbols - PMI_SEL_PRECISION * SRSLTE_SIMD_CF_SIZE + 1;
         j += PMI_SEL_PRECISION * SRSLTE_SIMD_CF_SIZE) {
      // 0. Load channel matrix
      srslte_simd_aligned cf_t h00_v[SRSLTE_SIMD_CF_SIZE];
      srslte_simd_aligned cf_t h01_v[SRSLTE_SIMD_CF_SIZE];
      srslte_simd_aligned cf_t h10_v[SRSLTE_SIMD_CF_SIZE];
      srslte_simd_aligned cf_t h11_v[SRSLTE_SIMD_CF_SIZE];

      for (uint32_t k = 0; k < SRSLTE_SIMD_CF_SIZE; k++) {
        h00_v[k] = h[0][0][j + PMI_SEL_PRECISION * k];
        h01_v[k] = h[1][0][j + PMI_SEL_PRECISION * k];
        h10_v[k] = h[0][1][j + PMI_SEL_PRECISION * k];
        h11_v[k] = h[1][1][j + PMI_SEL_PRECISION * k];
      }

      simd_cf_t h00 = srslte_simd_cfi_load(h00_v);
      simd_cf_t h01 = srslte_simd_cfi_load(h01_v);
      simd_cf_t h10 = srslte_simd_cfi_load(h10_v);
      simd_cf_t h11 = srslte_simd_cfi_load(h11_v);

      // 1. B = W'* H'
      simd_cf_t a00, a01, a10, a11;
      switch (i) {
        case 0:
          a00 = srslte_simd_cf_add(srslte_simd_cf_conj(h00), srslte_simd_cf_conj(h01));
          a01 = srslte_simd_cf_add(srslte_simd_cf_conj(h10), srslte_simd_cf_conj(h11));
          a10 = srslte_simd_cf_sub(srslte_simd_cf_conj(h00), srslte_simd_cf_conj(h01));
          a11 = srslte_simd_cf_sub(srslte_simd_cf_conj(h10), srslte_simd_cf_conj(h11));
          break;
        case 1:
          a00 = srslte_simd_cf_sub(srslte_simd_cf_conj(h00), srslte_simd_cf_mulj(srslte_simd_cf_conj(h01)));
          a01 = srslte_simd_cf_sub(srslte_simd_cf_conj(h10), srslte_simd_cf_mulj(srslte_simd_cf_conj(h11)));
          a10 = srslte_simd_cf_add(srslte_simd_cf_conj(h00), srslte_simd_cf_mulj(srslte_simd_cf_conj(h01)));
          a11 = srslte_simd_cf_add(srslte_simd_cf_conj(h10), srslte_simd_cf_mulj(srslte_simd_cf_conj(h11)));
          break;
        default:
          return SRSLTE_ERROR;
      }

      // 2. B = W' * H' * H = A * H
      simd_cf_t b00 = srslte_simd_cf_add(srslte_simd_cf_prod(a00, h00), srslte_simd_cf_prod(a01, h10));
      simd_cf_t b01 = srslte_simd_cf_add(srslte_simd_cf_prod(a00, h01), srslte_simd_cf_prod(a01, h11));
      simd_cf_t b10 = srslte_simd_cf_add(srslte_simd_cf_prod(a10, h00), srslte_simd_cf_prod(a11, h10));
      simd_cf_t b11 = srslte_simd_cf_add(srslte_simd_cf_prod(a10, h01), srslte_simd_cf_prod(a11, h11));

      // 3. C = W' * H' * H * W' = B * W
      simd_cf_t c00, c01, c10, c11;
      switch (i) {
        case 0:
          c00 = srslte_simd_cf_add(b00, b01);
          c01 = srslte_simd_cf_sub(b00, b01);
          c10 = srslte_simd_cf_add(b10, b11);
          c11 = srslte_simd_cf_sub(b10, b11);
          break;
        case 1:
          c00 = srslte_simd_cf_add(b00, srslte_simd_cf_mulj(b01));
          c01 = srslte_simd_cf_sub(b00, srslte_simd_cf_mulj(b01));
          c10 = srslte_simd_cf_add(b10, srslte_simd_cf_mulj(b11));
          c11 = srslte_simd_cf_sub(b10, srslte_simd_cf_mulj(b11));
          break;
        default:
          return SRSLTE_ERROR;
      }
      c00 = srslte_simd_cf_mul(c00, simd_f_norm);
      c01 = srslte_simd_cf_mul(c01, simd_f_norm);
      c10 = srslte_simd_cf_mul(c10, simd_f_norm);
      c11 = srslte_simd_cf_mul(c11, simd_f_norm);

      // 4. C += noise * I
      c00 = srslte_simd_cf_add(c00, simd_cf_noise_estimate);
      c11 = srslte_simd_cf_add(c11, simd_cf_noise_estimate);

      // 5. detC
      simd_f_t detC = srslte_simd_cf_re(srslte_mat_2x2_det_simd(c00, c01, c10, c11));

      // Avoid zero determinant
      detC = srslte_simd_f_select(detC, simd_f_det_min, srslte_simd_f_min(detC, simd_f_det_min));

      simd_f_t inv_detC = srslte_simd_f_rcp(detC);
      inv_detC          = srslte_simd_f_mul(simd_f_noise_estimate, inv_detC);

      simd_f_t den0 = srslte_simd_f_mul(srslte_simd_cf_re(c00), inv_detC);
      simd_f_t den1 = srslte_simd_f_mul(srslte_simd_cf_re(c11), inv_detC);

      simd_f_t gamma0 = srslte_simd_f_sub(srslte_simd_f_rcp(den0), simd_f_ones);
      simd_f_t gamma1 = srslte_simd_f_sub(srslte_simd_f_rcp(den1), simd_f_ones);

      // Avoid negative gamma
      gamma0 = srslte_simd_f_select(gamma0, simd_f_gamma_min, srslte_simd_f_min(gamma0, simd_f_gamma_min));
      gamma1 = srslte_simd_f_select(gamma1, simd_f_gamma_min, srslte_simd_f_min(gamma1, simd_f_gamma_min));

      simd_f_t gamma_sum = srslte_simd_f_hadd(gamma0, gamma1);

      // Horizontal accumulation
      for (int k = 1; k < SRSLTE_SIMD_F_SIZE; k *= 2) {
        gamma_sum = srslte_simd_f_hadd(gamma_sum, gamma_sum);
      }

      // Temporal store accumulated values
      srslte_simd_aligned float v[SRSLTE_SIMD_F_SIZE];
      srslte_simd_f_store(v, gamma_sum);

      // Average and accumulate SINR loop
      sinr_acc += (v[0] / SRSLTE_SIMD_CF_SIZE);

      // Increase loop counter
      count += 1.0f;
    }

    // Average loop accumulator
    if (isnormal(count)) {
      sinr_acc /= count;
    } else {
      sinr_acc = 1e+9f;
    }

    // Set SINR if available
    if (sinr_list) {
      sinr_list[i] = sinr_acc;
    }

    // Set PMI if available
    if (pmi && sinr_acc > max_sinr) {
      max_sinr = sinr_acc;
      *pmi     = i;
    }
  }

  // Return number of codebooks
  return 2;
}

#endif /* SRSLTE_SIMD_CF_SIZE */

/* PMI Select for 2 layers */
int srslte_precoding_pmi_select_2l(cf_t*     h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                   uint32_t  nof_symbols,
                                   float     noise_estimate,
                                   uint32_t* pmi,
                                   float     sinr_list[SRSLTE_MAX_CODEBOOKS])
{

  int ret;
#ifdef SRSLTE_SIMD_CF_SIZE
  ret = srslte_precoding_pmi_select_2l_simd(h, nof_symbols, noise_estimate, pmi, sinr_list);
#else
  ret = srslte_precoding_pmi_select_2l_gen(h, nof_symbols, noise_estimate, pmi, sinr_list);
#endif /* SRSLTE_SIMD_CF_SIZE */

  INFO("Precoder PMI Select for 2 layers SINR=[%.1fdB; %.1fdB] PMI=%d\n",
       srslte_convert_power_to_dB(sinr_list[0]),
       srslte_convert_power_to_dB(sinr_list[1]),
       *pmi);

  return ret;
}

int srslte_precoding_pmi_select(cf_t*     h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                                uint32_t  nof_symbols,
                                float     noise_estimate,
                                int       nof_layers,
                                uint32_t* pmi,
                                float     sinr[SRSLTE_MAX_CODEBOOKS])
{
  int ret;

  // Bound noise estimate value
  if (!isnormal(noise_estimate) || noise_estimate < 1e-9f) {
    noise_estimate = 1e-9f;
  }

  if (nof_layers == 1) {
    ret = srslte_precoding_pmi_select_1l(h, nof_symbols, noise_estimate, pmi, sinr);
  } else if (nof_layers == 2) {
    ret = srslte_precoding_pmi_select_2l(h, nof_symbols, noise_estimate, pmi, sinr);
  } else {
    ERROR("Unsupported number of layers");
    ret = SRSLTE_ERROR_INVALID_INPUTS;
  }

  return ret;
}

/* PMI Select for 1 layer */
float srslte_precoding_2x2_cn_gen(cf_t* h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS], uint32_t nof_symbols)
{
  uint32_t count  = 0;
  float    cn_avg = 0.0f;

  for (uint32_t i = 0; i < nof_symbols; i += PMI_SEL_PRECISION) {
    /* 0. Load channel matrix */
    cf_t h00 = h[0][0][i];
    cf_t h01 = h[1][0][i];
    cf_t h10 = h[0][1][i];
    cf_t h11 = h[1][1][i];

    cn_avg += srslte_mat_2x2_cn(h00, h01, h10, h11);

    count++;
  }

  if (count) {
    cn_avg /= count;
  }

  return cn_avg;
}

/* Computes the condition number for a given number of antennas,
 * stores in the parameter *cn the Condition Number in dB */
int srslte_precoding_cn(cf_t*    h[SRSLTE_MAX_PORTS][SRSLTE_MAX_PORTS],
                        uint32_t nof_tx_antennas,
                        uint32_t nof_rx_antennas,
                        uint32_t nof_symbols,
                        float*   cn)
{
  if (nof_tx_antennas == 2 && nof_rx_antennas == 2) {
    *cn = srslte_precoding_2x2_cn_gen(h, nof_symbols);
    return SRSLTE_SUCCESS;
  } else {
    ERROR("MIMO Condition Number calculation not implemented for %d×%d\n", nof_tx_antennas, nof_rx_antennas);
    return SRSLTE_ERROR;
  }
}
