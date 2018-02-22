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


#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>

#include <srslte/config.h>
#include "srslte/phy/utils/vector_simd.h"
#include "srslte/phy/utils/simd.h"


void srslte_vec_xor_bbb_simd(const int8_t *x, const int8_t *y, int8_t *z, const int len) {
  int i = 0;
#if SRSLTE_SIMD_B_SIZE
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_B_SIZE + 1; i += SRSLTE_SIMD_B_SIZE) {
      simd_b_t a = srslte_simd_b_load(&x[i]);
      simd_b_t b = srslte_simd_b_load(&y[i]);

      simd_b_t r = srslte_simd_b_xor(a, b);

      srslte_simd_b_store(&z[i], r);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_B_SIZE + 1; i += SRSLTE_SIMD_B_SIZE) {
      simd_b_t a = srslte_simd_b_loadu(&x[i]);
      simd_b_t b = srslte_simd_b_loadu(&y[i]);

      simd_s_t r = srslte_simd_b_xor(a, b);

      srslte_simd_b_storeu(&z[i], r);
    }
  }
#endif /* SRSLTE_SIMD_B_SIZE */
  
    for(; i < len; i++){
    z[i] = x[i] ^ y[i];
  }
}

int srslte_vec_dot_prod_sss_simd(const int16_t *x, const int16_t *y, const int len) {
  int i = 0;
  int result = 0;
#if SRSLTE_SIMD_S_SIZE
  simd_s_t simd_dotProdVal = srslte_simd_s_zero();
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y)) {
    for (; i < len - SRSLTE_SIMD_S_SIZE + 1; i += SRSLTE_SIMD_S_SIZE) {
      simd_s_t a = srslte_simd_s_load(&x[i]);
      simd_s_t b = srslte_simd_s_load(&y[i]);

      simd_s_t z = srslte_simd_s_mul(a, b);

      simd_dotProdVal = srslte_simd_s_add(simd_dotProdVal, z);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_S_SIZE + 1; i += SRSLTE_SIMD_S_SIZE) {
      simd_s_t a = srslte_simd_s_loadu(&x[i]);
      simd_s_t b = srslte_simd_s_loadu(&y[i]);

      simd_s_t z = srslte_simd_s_mul(a, b);

      simd_dotProdVal = srslte_simd_s_add(simd_dotProdVal, z);
    }
  }
  __attribute__ ((aligned (SRSLTE_SIMD_S_SIZE*2))) short dotProdVector[SRSLTE_SIMD_S_SIZE];
  srslte_simd_s_store(dotProdVector, simd_dotProdVal);
  for (int k = 0; k < SRSLTE_SIMD_S_SIZE; k++) {
    result += dotProdVector[k];
  }
#endif /* SRSLTE_SIMD_S_SIZE */

  for(; i < len; i++){
    result += (x[i] * y[i]);
  }

  return result; 
}

void srslte_vec_sum_sss_simd(const int16_t *x, const int16_t *y, int16_t *z, const int len) {
  int i = 0;
#if SRSLTE_SIMD_S_SIZE
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_S_SIZE + 1; i += SRSLTE_SIMD_S_SIZE) {
      simd_s_t a = srslte_simd_s_load(&x[i]);
      simd_s_t b = srslte_simd_s_load(&y[i]);

      simd_s_t r = srslte_simd_s_add(a, b);

      srslte_simd_s_store(&z[i], r);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_S_SIZE + 1; i += SRSLTE_SIMD_S_SIZE) {
      simd_s_t a = srslte_simd_s_loadu(&x[i]);
      simd_s_t b = srslte_simd_s_loadu(&y[i]);

      simd_s_t r = srslte_simd_s_add(a, b);

      srslte_simd_s_storeu(&z[i], r);
    }
  }
#endif /* SRSLTE_SIMD_S_SIZE */

  for(; i < len; i++){
    z[i] = x[i] + y[i];
  }
}

void srslte_vec_sub_sss_simd(const int16_t *x, const int16_t *y, int16_t *z, const int len) {
  int i = 0;
#if SRSLTE_SIMD_S_SIZE
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_S_SIZE + 1; i += SRSLTE_SIMD_S_SIZE) {
      simd_s_t a = srslte_simd_s_load(&x[i]);
      simd_s_t b = srslte_simd_s_load(&y[i]);

      simd_s_t r = srslte_simd_s_sub(a, b);

      srslte_simd_s_store(&z[i], r);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_S_SIZE + 1; i += SRSLTE_SIMD_S_SIZE) {
      simd_s_t a = srslte_simd_s_loadu(&x[i]);
      simd_s_t b = srslte_simd_s_loadu(&y[i]);

      simd_s_t r = srslte_simd_s_sub(a, b);

      srslte_simd_s_storeu(&z[i], r);
    }
  }
#endif /* SRSLTE_SIMD_S_SIZE */

  for(; i < len; i++){
    z[i] = x[i] - y[i];
  }
}

void srslte_vec_prod_sss_simd(const int16_t *x, const int16_t *y, int16_t *z, const int len) {
  int i = 0;
#if SRSLTE_SIMD_S_SIZE
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_S_SIZE + 1; i += SRSLTE_SIMD_S_SIZE) {
      simd_s_t a = srslte_simd_s_load(&x[i]);
      simd_s_t b = srslte_simd_s_load(&y[i]);

      simd_s_t r = srslte_simd_s_mul(a, b);

      srslte_simd_s_store(&z[i], r);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_S_SIZE + 1; i += SRSLTE_SIMD_S_SIZE) {
      simd_s_t a = srslte_simd_s_loadu(&x[i]);
      simd_s_t b = srslte_simd_s_loadu(&y[i]);

      simd_s_t r = srslte_simd_s_mul(a, b);

      srslte_simd_s_storeu(&z[i], r);
    }
  }
#endif /* SRSLTE_SIMD_S_SIZE */

  for(; i < len; i++){
    z[i] = x[i] * y[i];
  }
}

/* No improvement with AVX */
void srslte_vec_lut_sss_simd(const short *x, const unsigned short *lut, short *y, const int len) {
  int i = 0;
#ifdef LV_HAVE_SSE
#if CMAKE_BUILD_TYPE!=Debug

  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(lut)) {
    for (; i < len - 7; i += 8) {
      __m128i xVal = _mm_load_si128((__m128i *) &x[i]);
      __m128i lutVal = _mm_load_si128((__m128i *) &lut[i]);

      for (int k = 0; k < 8; k++) {
        int16_t x = (int16_t) _mm_extract_epi16(xVal, k);
        uint16_t l = (uint16_t) _mm_extract_epi16(lutVal, k);
        y[l] = (short) x;
      }
    }
  } else {
    for (; i < len - 7; i += 8) {
      __m128i xVal = _mm_loadu_si128((__m128i *) &x[i]);
      __m128i lutVal = _mm_loadu_si128((__m128i *) &lut[i]);

      for (int k = 0; k < 8; k++) {
        int16_t x = (int16_t) _mm_extract_epi16(xVal, k);
        uint16_t l = (uint16_t) _mm_extract_epi16(lutVal, k);
        y[l] = (short) x;
      }
    }
  }
#endif
#endif

  for (; i < len; i++) {
    y[lut[i]] = x[i];
  }
}

void srslte_vec_convert_if_simd(const int16_t *x, float *z, const float scale, const int len) {
  int i = 0;
  const float gain = 1.0f / scale;

#ifdef LV_HAVE_SSE
  __m128 s = _mm_set1_ps(gain);
  if (SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - 3; i += 4) {
      __m64 *ptr = (__m64 *) &x[i];
      __m128 fl = _mm_cvtpi16_ps(*ptr);
      __m128 v = _mm_mul_ps(fl, s);

      _mm_store_ps(&z[i], v);
    }
  } else {
    for (; i < len - 3; i += 4) {
      __m64 *ptr = (__m64 *) &x[i];
      __m128 fl = _mm_cvtpi16_ps(*ptr);
      __m128 v = _mm_mul_ps(fl, s);

      _mm_storeu_ps(&z[i], v);
    }
  }
#endif /* LV_HAVE_SSE */

  for (; i < len; i++) {
    z[i] = ((float) x[i]) * gain;
  }
}

void srslte_vec_convert_fi_simd(const float *x, int16_t *z, const float scale, const int len) {
  int i = 0;

#if SRSLTE_SIMD_F_SIZE && SRSLTE_SIMD_S_SIZE
  simd_f_t s = srslte_simd_f_set1(scale);
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_S_SIZE + 1; i += SRSLTE_SIMD_S_SIZE) {
      simd_f_t a = srslte_simd_f_load(&x[i]);
      simd_f_t b = srslte_simd_f_load(&x[i + SRSLTE_SIMD_F_SIZE]);

      simd_f_t sa = srslte_simd_f_mul(a, s);
      simd_f_t sb = srslte_simd_f_mul(b, s);

      simd_s_t i16 = srslte_simd_convert_2f_s(sa, sb);

      srslte_simd_s_store(&z[i], i16);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_S_SIZE + 1; i += SRSLTE_SIMD_S_SIZE) {
      simd_f_t a = srslte_simd_f_loadu(&x[i]);
      simd_f_t b = srslte_simd_f_loadu(&x[i + SRSLTE_SIMD_F_SIZE]);

      simd_f_t sa = srslte_simd_f_mul(a, s);
      simd_f_t sb = srslte_simd_f_mul(b, s);

      simd_s_t i16 = srslte_simd_convert_2f_s(sa, sb);

      srslte_simd_s_storeu(&z[i], i16);
    }
  }
#endif /* SRSLTE_SIMD_F_SIZE && SRSLTE_SIMD_S_SIZE */

  for(; i < len; i++){
    z[i] = (int16_t) (x[i] * scale);
  }
}

float srslte_vec_acc_ff_simd(const float *x, const int len) {
  int i = 0;
  float acc_sum = 0.0f;

#if SRSLTE_SIMD_F_SIZE
  simd_f_t simd_sum = srslte_simd_f_zero();

  if (SRSLTE_IS_ALIGNED(x)) {
    for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
      simd_f_t a = srslte_simd_f_load(&x[i]);

      simd_sum = srslte_simd_f_add(simd_sum, a);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
      simd_f_t a = srslte_simd_f_loadu(&x[i]);

      simd_sum = srslte_simd_f_add(simd_sum, a);
    }
  }

  __attribute__((aligned(SRSLTE_SIMD_F_SIZE*4))) float sum[SRSLTE_SIMD_F_SIZE];
  srslte_simd_f_store(sum, simd_sum);
  for (int k = 0; k < SRSLTE_SIMD_F_SIZE; k++) {
    acc_sum += sum[k];
  }
#endif

  for (; i<len; i++) {
    acc_sum += x[i];
  }

  return acc_sum;
}

cf_t srslte_vec_acc_cc_simd(const cf_t *x, const int len) {
  int i = 0;
  cf_t acc_sum = 0.0f;

#if SRSLTE_SIMD_F_SIZE
  simd_f_t simd_sum = srslte_simd_f_zero();

  if (SRSLTE_IS_ALIGNED(x)) {
    for (; i < len - SRSLTE_SIMD_F_SIZE / 2 + 1; i += SRSLTE_SIMD_F_SIZE / 2) {
      simd_f_t a = srslte_simd_f_load((float *) &x[i]);

      simd_sum = srslte_simd_f_add(simd_sum, a);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_F_SIZE / 2 + 1; i += SRSLTE_SIMD_F_SIZE / 2) {
      simd_f_t a = srslte_simd_f_loadu((float *) &x[i]);

      simd_sum = srslte_simd_f_add(simd_sum, a);
    }
  }

  __attribute__((aligned(64))) cf_t sum[SRSLTE_SIMD_F_SIZE/2];
  srslte_simd_f_store((float*)&sum, simd_sum);
  for (int k = 0; k < SRSLTE_SIMD_F_SIZE/2; k++) {
    acc_sum += sum[k];
  }
#endif

  for (; i<len; i++) {
    acc_sum += x[i];
  }
  return acc_sum;
}

void srslte_vec_add_fff_simd(const float *x, const float *y, float *z, const int len) {
  int i = 0;

#if SRSLTE_SIMD_F_SIZE
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
      simd_f_t a = srslte_simd_f_load(&x[i]);
      simd_f_t b = srslte_simd_f_load(&y[i]);

      simd_f_t r = srslte_simd_f_add(a, b);

      srslte_simd_f_store(&z[i], r);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
      simd_f_t a = srslte_simd_f_loadu(&x[i]);
      simd_f_t b = srslte_simd_f_loadu(&y[i]);

      simd_f_t r = srslte_simd_f_add(a, b);

      srslte_simd_f_storeu(&z[i], r);
    }
  }
#endif

  for (; i<len; i++) {
    z[i] = x[i] + y[i];
  }
}

void srslte_vec_sub_fff_simd(const float *x, const float *y, float *z, const int len) {
  int i = 0;

#if SRSLTE_SIMD_F_SIZE
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
      simd_f_t a = srslte_simd_f_load(&x[i]);
      simd_f_t b = srslte_simd_f_load(&y[i]);

      simd_f_t r = srslte_simd_f_sub(a, b);

      srslte_simd_f_store(&z[i], r);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
      simd_f_t a = srslte_simd_f_loadu(&x[i]);
      simd_f_t b = srslte_simd_f_loadu(&y[i]);

      simd_f_t r = srslte_simd_f_sub(a, b);

      srslte_simd_f_storeu(&z[i], r);
    }
  }
#endif

  for (; i < len; i++) {
    z[i] = x[i] - y[i];
  }
}

cf_t srslte_vec_dot_prod_ccc_simd(const cf_t *x, const cf_t *y, const int len) {
  int i = 0;
  cf_t result = 0;

#if SRSLTE_SIMD_CF_SIZE
  __attribute__((aligned(64))) cf_t simd_dotProdVector[SRSLTE_SIMD_CF_SIZE];

  simd_cf_t avx_result = srslte_simd_cf_zero();
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y)) {
    for (; i < len - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
      simd_cf_t xVal = srslte_simd_cfi_load(&x[i]);
      simd_cf_t yVal = srslte_simd_cfi_load(&y[i]);

      avx_result = srslte_simd_cf_add(srslte_simd_cf_prod(xVal, yVal), avx_result);
      srslte_simd_cfi_store(simd_dotProdVector, avx_result);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
      simd_cf_t xVal = srslte_simd_cfi_loadu(&x[i]);
      simd_cf_t yVal = srslte_simd_cfi_loadu(&y[i]);

      avx_result = srslte_simd_cf_add(srslte_simd_cf_prod(xVal, yVal), avx_result);
      srslte_simd_cfi_storeu(simd_dotProdVector, avx_result);
    }
  }

  srslte_simd_cfi_store(simd_dotProdVector, avx_result);
  for (int k = 0; k < SRSLTE_SIMD_CF_SIZE; k++) {
    result += simd_dotProdVector[k];
  }
#endif

  for (; i < len; i++) {
    result += (x[i] * y[i]);
  }

  return result;
}

#ifdef ENABLE_C16
c16_t srslte_vec_dot_prod_ccc_c16i_simd(const c16_t *x, const c16_t *y, const int len) {
  int i = 0;
  c16_t result = 0;

#if SRSLTE_SIMD_C16_SIZE
  simd_c16_t avx_result = srslte_simd_c16_zero();

  for (; i < len - SRSLTE_SIMD_C16_SIZE + 1; i += SRSLTE_SIMD_C16_SIZE) {
    simd_c16_t xVal = srslte_simd_c16i_load(&x[i]);
    simd_c16_t yVal = srslte_simd_c16i_load(&y[i]);

    avx_result = srslte_simd_c16_add(srslte_simd_c16_prod(xVal, yVal), avx_result);
  }

  __attribute__((aligned(256))) c16_t avx_dotProdVector[16] = {0};
  srslte_simd_c16i_store(avx_dotProdVector, avx_result);
  for (int k = 0; k < 16; k++) {
    result += avx_dotProdVector[k];
  }
#endif

  for(;i < len; i++){
    result += (x[i] * y[i])/(1<<14);
  }

  return result;
}
#endif /* ENABLE_C16 */

cf_t srslte_vec_dot_prod_conj_ccc_simd(const cf_t *x, const cf_t *y, const int len)
{
  int i = 0;
  cf_t result = 0;

#if SRSLTE_SIMD_CF_SIZE
  __attribute__((aligned(256))) cf_t simd_dotProdVector[SRSLTE_SIMD_CF_SIZE];

  simd_cf_t simd_result = srslte_simd_cf_zero();
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y)) {
    for (; i < len - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
      simd_cf_t xVal = srslte_simd_cfi_load(&x[i]);
      simd_cf_t yVal = srslte_simd_cfi_load(&y[i]);

      simd_result = srslte_simd_cf_add(srslte_simd_cf_conjprod(xVal, yVal), simd_result);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
      simd_cf_t xVal = srslte_simd_cfi_loadu(&x[i]);
      simd_cf_t yVal = srslte_simd_cfi_loadu(&y[i]);

      simd_result = srslte_simd_cf_add(srslte_simd_cf_conjprod(xVal, yVal), simd_result);
    }
  }

  srslte_simd_cfi_store(simd_dotProdVector, simd_result);
  for (int k = 0; k < SRSLTE_SIMD_CF_SIZE; k++) {
    result += simd_dotProdVector[k];
  }
#endif

  for (; i < len; i++) {
    result += x[i] * conjf(y[i]);
  }

  return result;
}

void srslte_vec_prod_cfc_simd(const cf_t *x, const float *y, cf_t *z, const int len) {
  int i = 0;

#if SRSLTE_SIMD_CF_SIZE
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
      simd_f_t s = srslte_simd_f_load(&y[i]);

      simd_cf_t a = srslte_simd_cfi_load(&x[i]);
      simd_cf_t r = srslte_simd_cf_mul(a, s);
      srslte_simd_cfi_store(&z[i], r);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
      simd_f_t s = srslte_simd_f_loadu(&y[i]);

      simd_cf_t a = srslte_simd_cfi_loadu(&x[i]);
      simd_cf_t r = srslte_simd_cf_mul(a, s);
      srslte_simd_cfi_storeu(&z[i], r);
    }
  }
#endif

  for (; i<len; i++) {
    z[i] = x[i] * y[i];
  }
}

void srslte_vec_prod_fff_simd(const float *x, const float *y, float *z, const int len) {
  int i = 0;

#if SRSLTE_SIMD_F_SIZE
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
      simd_f_t a = srslte_simd_f_load(&x[i]);
      simd_f_t b = srslte_simd_f_load(&y[i]);

      simd_f_t r = srslte_simd_f_mul(a, b);

      srslte_simd_f_store(&z[i], r);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
      simd_f_t a = srslte_simd_f_loadu(&x[i]);
      simd_f_t b = srslte_simd_f_loadu(&y[i]);

      simd_f_t r = srslte_simd_f_mul(a, b);

      srslte_simd_f_storeu(&z[i], r);
    }
  }
#endif

  for (; i<len; i++) {
    z[i] = x[i] * y[i];
  }
}

void srslte_vec_prod_ccc_simd(const cf_t *x, const cf_t *y, cf_t *z, const int len) {
  int i = 0;

#if SRSLTE_SIMD_CF_SIZE
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
      simd_cf_t a = srslte_simd_cfi_load(&x[i]);
      simd_cf_t b = srslte_simd_cfi_load(&y[i]);

      simd_cf_t r = srslte_simd_cf_prod(a, b);

      srslte_simd_cfi_store(&z[i], r);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
      simd_cf_t a = srslte_simd_cfi_loadu(&x[i]);
      simd_cf_t b = srslte_simd_cfi_loadu(&y[i]);

      simd_cf_t r = srslte_simd_cf_prod(a, b);

      srslte_simd_cfi_storeu(&z[i], r);
    }
  }
#endif

  for (; i<len; i++) {
    z[i] = x[i] * y[i];
  }
}

void srslte_vec_prod_ccc_split_simd(const float *a_re, const float *a_im, const float *b_re, const float *b_im,
                                    float *r_re, float *r_im, const int len) {
  int i = 0;

#if SRSLTE_SIMD_F_SIZE
  if (SRSLTE_IS_ALIGNED(a_re) && SRSLTE_IS_ALIGNED(a_im) && SRSLTE_IS_ALIGNED(b_re) && SRSLTE_IS_ALIGNED(b_im) &&
      SRSLTE_IS_ALIGNED(r_re) && SRSLTE_IS_ALIGNED(r_im)) {
    for (; i < len - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
      simd_cf_t a = srslte_simd_cf_load(&a_re[i], &a_im[i]);
      simd_cf_t b = srslte_simd_cf_load(&b_re[i], &b_im[i]);

      simd_cf_t r = srslte_simd_cf_prod(a, b);

      srslte_simd_cf_store(&r_re[i], &r_im[i], r);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
      simd_cf_t a = srslte_simd_cf_loadu(&a_re[i], &a_im[i]);
      simd_cf_t b = srslte_simd_cf_loadu(&b_re[i], &b_im[i]);

      simd_cf_t r = srslte_simd_cf_prod(a, b);

      srslte_simd_cf_storeu(&r_re[i], &r_im[i], r);
    }
  }
#endif

  for (; i<len; i++) {
    r_re[i] = a_re[i]*b_re[i] - a_im[i]*b_im[i];
    r_im[i] = a_re[i]*b_im[i] + a_im[i]*b_re[i];
  }
}

#ifdef ENABLE_C16
void srslte_vec_prod_ccc_c16_simd(const int16_t *a_re, const int16_t *a_im, const int16_t *b_re, const int16_t *b_im,
                                  int16_t *r_re, int16_t *r_im, const int len) {
  int i = 0;

#if SRSLTE_SIMD_C16_SIZE
  if (SRSLTE_IS_ALIGNED(a_re) && SRSLTE_IS_ALIGNED(a_im) && SRSLTE_IS_ALIGNED(b_re) && SRSLTE_IS_ALIGNED(b_im) &&
      SRSLTE_IS_ALIGNED(r_re) && SRSLTE_IS_ALIGNED(r_im)) {
    for (; i < len - SRSLTE_SIMD_C16_SIZE + 1; i += SRSLTE_SIMD_C16_SIZE) {
      simd_c16_t a = srslte_simd_c16_load(&a_re[i], &a_im[i]);
      simd_c16_t b = srslte_simd_c16_load(&b_re[i], &b_im[i]);

      simd_c16_t r = srslte_simd_c16_prod(a, b);

      srslte_simd_c16_store(&r_re[i], &r_im[i], r);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_C16_SIZE + 1; i += SRSLTE_SIMD_C16_SIZE) {
      simd_c16_t a = srslte_simd_c16_loadu(&a_re[i], &a_im[i]);
      simd_c16_t b = srslte_simd_c16_loadu(&b_re[i], &b_im[i]);

      simd_c16_t r = srslte_simd_c16_prod(a, b);

      srslte_simd_c16_storeu(&r_re[i], &r_im[i], r);
    }
  }
#endif

  for (; i<len; i++) {
    r_re[i] = a_re[i]*b_re[i] - a_im[i]*b_im[i];
    r_im[i] = a_re[i]*b_im[i] + a_im[i]*b_re[i];
  }
}
#endif /* ENABLE_C16 */

void srslte_vec_prod_conj_ccc_simd(const cf_t *x, const cf_t *y, cf_t *z, const int len) {
  int i = 0;

#if SRSLTE_SIMD_CF_SIZE
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
      simd_cf_t a = srslte_simd_cfi_load(&x[i]);
      simd_cf_t b = srslte_simd_cfi_load(&y[i]);

      simd_cf_t r = srslte_simd_cf_conjprod(a, b);

      srslte_simd_cfi_store(&z[i], r);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
      simd_cf_t a = srslte_simd_cfi_loadu(&x[i]);
      simd_cf_t b = srslte_simd_cfi_loadu(&y[i]);

      simd_cf_t r = srslte_simd_cf_conjprod(a, b);

      srslte_simd_cfi_storeu(&z[i], r);
    }
  }
#endif

  for (; i<len; i++) {
    z[i] = x[i] * conjf(y[i]);
  }
}

void srslte_vec_div_ccc_simd(const cf_t *x, const cf_t *y, cf_t *z, const int len) {
  int i = 0;

#if SRSLTE_SIMD_CF_SIZE
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
      simd_cf_t a = srslte_simd_cfi_load(&x[i]);
      simd_cf_t b = srslte_simd_cfi_load(&y[i]);

      simd_cf_t rcpb = srslte_simd_cf_rcp(b);
      simd_cf_t r = srslte_simd_cf_prod(a, rcpb);

      srslte_simd_cfi_store(&z[i], r);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
      simd_cf_t a = srslte_simd_cfi_loadu(&x[i]);
      simd_cf_t b = srslte_simd_cfi_loadu(&y[i]);

      simd_cf_t rcpb = srslte_simd_cf_rcp(b);
      simd_cf_t r = srslte_simd_cf_prod(a, rcpb);

      srslte_simd_cfi_storeu(&z[i], r);
    }
  }
#endif

  for (; i < len; i++) {
    z[i] = x[i] / y[i];
  }
}


void srslte_vec_div_cfc_simd(const cf_t *x, const float *y, cf_t *z, const int len) {
  int i = 0;

#if SRSLTE_SIMD_CF_SIZE && SRSLTE_SIMD_CF_SIZE == SRSLTE_SIMD_F_SIZE
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
      simd_cf_t a = srslte_simd_cfi_load(&x[i]);
      simd_f_t b = srslte_simd_f_load(&y[i]);

      simd_f_t rcpb = srslte_simd_f_rcp(b);
      simd_cf_t r = srslte_simd_cf_mul(a, rcpb);

      srslte_simd_cfi_store(&z[i], r);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
      simd_cf_t a = srslte_simd_cfi_loadu(&x[i]);
      simd_f_t b = srslte_simd_f_loadu(&y[i]);

      simd_f_t rcpb = srslte_simd_f_rcp(b);
      simd_cf_t r = srslte_simd_cf_mul(a, rcpb);

      srslte_simd_cfi_storeu(&z[i], r);
    }
  }
#endif

  for (; i < len; i++) {
    z[i] = x[i] / y[i];
  }
}

void srslte_vec_div_fff_simd(const float *x, const float *y, float *z, const int len) {
  int i = 0;

#if SRSLTE_SIMD_F_SIZE
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
      simd_f_t a = srslte_simd_f_load(&x[i]);
      simd_f_t b = srslte_simd_f_load(&y[i]);

      simd_f_t rcpb = srslte_simd_f_rcp(b);
      simd_f_t r = srslte_simd_f_mul(a, rcpb);

      srslte_simd_f_store(&z[i], r);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
      simd_f_t a = srslte_simd_f_loadu(&x[i]);
      simd_f_t b = srslte_simd_f_loadu(&y[i]);

      simd_f_t rcpb = srslte_simd_f_rcp(b);
      simd_f_t r = srslte_simd_f_mul(a, rcpb);

      srslte_simd_f_storeu(&z[i], r);
    }
  }
#endif

  for (; i < len; i++) {
    z[i] = x[i] / y[i];
  }
}



int  srslte_vec_sc_prod_ccc_simd2(const cf_t *x, const cf_t h, cf_t *z, const int len)
{     
   int i = 0;
   const unsigned int loops = len / 4;
#ifdef HAVE_NEON
  simd_cf_t h_vec; 
    h_vec.val[0] = srslte_simd_f_set1(__real__ h);
    h_vec.val[1] = srslte_simd_f_set1(__imag__ h);
  for (; i < loops; i++) {

    simd_cf_t in =  srslte_simd_cfi_load(&x[i*4]);
    simd_cf_t temp =  srslte_simd_cf_prod(in, h_vec);
    srslte_simd_cfi_store(&z[i*4], temp);
  }
          
#endif  
  i = loops * 4;
return i;
}

void srslte_vec_sc_prod_ccc_simd(const cf_t *x, const cf_t h, cf_t *z, const int len) {
  int i = 0;

#if SRSLTE_SIMD_F_SIZE
  

#ifdef HAVE_NEON
  i = srslte_vec_sc_prod_ccc_simd2(x, h, z, len);
#else
  const simd_f_t hre = srslte_simd_f_set1(__real__ h);
  const simd_f_t him = srslte_simd_f_set1(__imag__ h);

  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_F_SIZE / 2 + 1; i += SRSLTE_SIMD_F_SIZE / 2) {
      simd_f_t temp = srslte_simd_f_load((float *) &x[i]);

      simd_f_t m1 = srslte_simd_f_mul(hre, temp);
      simd_f_t sw = srslte_simd_f_swap(temp);
      simd_f_t m2 = srslte_simd_f_mul(him, sw);
      simd_f_t r = srslte_simd_f_addsub(m1, m2);
      srslte_simd_f_store((float *) &z[i], r);

    }
  } else {
    for (; i < len - SRSLTE_SIMD_F_SIZE / 2 + 1; i += SRSLTE_SIMD_F_SIZE / 2) {
      simd_f_t temp = srslte_simd_f_loadu((float *) &x[i]);

      simd_f_t m1 = srslte_simd_f_mul(hre, temp);
      simd_f_t sw = srslte_simd_f_swap(temp);
      simd_f_t m2 = srslte_simd_f_mul(him, sw);
      simd_f_t r = srslte_simd_f_addsub(m1, m2);

      srslte_simd_f_storeu((float *) &z[i], r);
    }
  }
#endif
#endif
  for (; i < len; i++) {
    z[i] = x[i] * h;
  }
  
}

void srslte_vec_sc_prod_fff_simd(const float *x, const float h, float *z, const int len) {
  int i = 0;

#if SRSLTE_SIMD_F_SIZE
  const simd_f_t hh = srslte_simd_f_set1(h);
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
      simd_f_t xx = srslte_simd_f_load(&x[i]);

      simd_f_t zz = srslte_simd_f_mul(xx, hh);

      srslte_simd_f_store(&z[i], zz);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
      simd_f_t xx = srslte_simd_f_loadu(&x[i]);

      simd_f_t zz = srslte_simd_f_mul(xx, hh);

      srslte_simd_f_storeu(&z[i], zz);
    }
  }
#endif

  for (; i < len; i++) {
    z[i] = x[i] * h;
  }
}

void srslte_vec_abs_cf_simd(const cf_t *x, float *z, const int len) {
  int i = 0;

#if SRSLTE_SIMD_F_SIZE
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
      simd_f_t x1 = srslte_simd_f_load((float *) &x[i]);
      simd_f_t x2 = srslte_simd_f_load((float *) &x[i + SRSLTE_SIMD_F_SIZE / 2]);

      simd_f_t mul1 = srslte_simd_f_mul(x1, x1);
      simd_f_t mul2 = srslte_simd_f_mul(x2, x2);

      simd_f_t z1 = srslte_simd_f_hadd(mul1, mul2);
      z1 = srslte_simd_f_sqrt(z1);
      srslte_simd_f_store(&z[i], z1);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
      simd_f_t x1 = srslte_simd_f_loadu((float *) &x[i]);
      simd_f_t x2 = srslte_simd_f_loadu((float *) &x[i + SRSLTE_SIMD_F_SIZE / 2]);

      simd_f_t mul1 = srslte_simd_f_mul(x1, x1);
      simd_f_t mul2 = srslte_simd_f_mul(x2, x2);

      simd_f_t z1 = srslte_simd_f_hadd(mul1, mul2);
      z1 = srslte_simd_f_sqrt(z1);

      srslte_simd_f_storeu(&z[i], z1);
    }
  }
#endif

  for (; i < len; i++) {
    z[i] = sqrtf(__real__(x[i]) * __real__(x[i]) + __imag__(x[i]) * __imag__(x[i]));
  }
}

void srslte_vec_abs_square_cf_simd(const cf_t *x, float *z, const int len) {
  int i = 0;

#if SRSLTE_SIMD_F_SIZE
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
      simd_f_t x1 = srslte_simd_f_load((float *) &x[i]);
      simd_f_t x2 = srslte_simd_f_load((float *) &x[i + SRSLTE_SIMD_F_SIZE / 2]);

      simd_f_t mul1 = srslte_simd_f_mul(x1, x1);
      simd_f_t mul2 = srslte_simd_f_mul(x2, x2);

      simd_f_t z1 = srslte_simd_f_hadd(mul1, mul2);

      srslte_simd_f_store(&z[i], z1);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_F_SIZE + 1; i += SRSLTE_SIMD_F_SIZE) {
      simd_f_t x1 = srslte_simd_f_loadu((float *) &x[i]);
      simd_f_t x2 = srslte_simd_f_loadu((float *) &x[i + SRSLTE_SIMD_F_SIZE / 2]);

      simd_f_t mul1 = srslte_simd_f_mul(x1, x1);
      simd_f_t mul2 = srslte_simd_f_mul(x2, x2);

      simd_f_t z1 = srslte_simd_f_hadd(mul1, mul2);

      srslte_simd_f_storeu(&z[i], z1);
    }
  }
#endif

  for (; i < len; i++) {
    z[i] = __real__(x[i]) * __real__(x[i]) + __imag__(x[i]) * __imag__(x[i]);
  }
}


void srslte_vec_sc_prod_cfc_simd(const cf_t *x, const float h, cf_t *z, const int len) {
  int i = 0;

#if SRSLTE_SIMD_F_SIZE
  const simd_f_t tap = srslte_simd_f_set1(h);

  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - SRSLTE_SIMD_F_SIZE / 2 + 1; i += SRSLTE_SIMD_F_SIZE / 2) {
      simd_f_t temp = srslte_simd_f_load((float *) &x[i]);

      temp = srslte_simd_f_mul(tap, temp);

      srslte_simd_f_store((float *) &z[i], temp);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_F_SIZE / 2 + 1; i += SRSLTE_SIMD_F_SIZE / 2) {
      simd_f_t temp = srslte_simd_f_loadu((float *) &x[i]);

      temp = srslte_simd_f_mul(tap, temp);

      srslte_simd_f_storeu((float *) &z[i], temp);
    }
  }
#endif

  for (; i < len; i++) {
    z[i] = x[i] * h;
  }
}

void srslte_vec_cp_simd(const cf_t *src, cf_t *dst, const int len) {
  uint32_t i = 0;

#if SRSLTE_SIMD_F_SIZE
  if (SRSLTE_IS_ALIGNED(src) && SRSLTE_IS_ALIGNED(dst)) {
    for (; i < len - SRSLTE_SIMD_F_SIZE / 2 + 1; i += SRSLTE_SIMD_F_SIZE / 2) {
      simd_f_t temp = srslte_simd_f_load((float *) &src[i]);

      srslte_simd_f_store((float *) &dst[i], temp);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_F_SIZE / 2 + 1; i += SRSLTE_SIMD_F_SIZE / 2) {
      simd_f_t temp = srslte_simd_f_loadu((float *) &src[i]);

      srslte_simd_f_storeu((float *) &dst[i], temp);
    }
  }
#endif

  for (; i < len; i++) {
    dst[i] = src[i];
  }
}

uint32_t srslte_vec_max_fi_simd(const float *x, const int len) {
  int i = 0;

  float max_value = -INFINITY;
  uint32_t max_index = 0;

#if SRSLTE_SIMD_I_SIZE
  __attribute__ ((aligned (SRSLTE_SIMD_I_SIZE*sizeof(int)))) int indexes_buffer[SRSLTE_SIMD_I_SIZE] = {0};
  __attribute__ ((aligned (SRSLTE_SIMD_I_SIZE*sizeof(float)))) float values_buffer[SRSLTE_SIMD_I_SIZE] = {0};

  for (int k = 0; k < SRSLTE_SIMD_I_SIZE; k++) indexes_buffer[k] = k;
  simd_i_t simd_inc = srslte_simd_i_set1(SRSLTE_SIMD_I_SIZE);
  simd_i_t simd_indexes = srslte_simd_i_load(indexes_buffer);
  simd_i_t simd_max_indexes = srslte_simd_i_set1(0);

  simd_f_t simd_max_values = srslte_simd_f_set1(-INFINITY);

  if (SRSLTE_IS_ALIGNED(x)) {
    for (; i < len - SRSLTE_SIMD_I_SIZE + 1; i += SRSLTE_SIMD_I_SIZE) {
      simd_f_t a = srslte_simd_f_load(&x[i]);
      simd_sel_t res = srslte_simd_f_max(a, simd_max_values);
      simd_max_indexes = srslte_simd_i_select(simd_max_indexes, simd_indexes, res);
      simd_max_values = (simd_f_t) srslte_simd_i_select((simd_i_t) simd_max_values, (simd_i_t) a, res);
      simd_indexes = srslte_simd_i_add(simd_indexes, simd_inc);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_I_SIZE + 1; i += SRSLTE_SIMD_I_SIZE) {
      simd_f_t a = srslte_simd_f_loadu(&x[i]);
      simd_sel_t res = srslte_simd_f_max(a, simd_max_values);
      simd_max_indexes = srslte_simd_i_select(simd_max_indexes, simd_indexes, res);
      simd_max_values = (simd_f_t) srslte_simd_i_select((simd_i_t) simd_max_values, (simd_i_t) a, res);
      simd_indexes = srslte_simd_i_add(simd_indexes, simd_inc);
    }
  }

  srslte_simd_i_store(indexes_buffer, simd_max_indexes);
  srslte_simd_f_store(values_buffer, simd_max_values);

  for (int k = 0; k < SRSLTE_SIMD_I_SIZE; k++) {
    if (values_buffer[k] > max_value) {
      max_value = values_buffer[k];
      max_index = (uint32_t) indexes_buffer[k];
    }
  }
#endif /* SRSLTE_SIMD_I_SIZE */

  for (; i < len; i++) {
    if (x[i] > max_value) {
      max_value = x[i];
      max_index = (uint32_t)i;
    }
  }

  return max_index;
}

uint32_t srslte_vec_max_ci_simd(const cf_t *x, const int len) {
  int i = 0;

  float max_value = -INFINITY;
  uint32_t max_index = 0;

#if SRSLTE_SIMD_I_SIZE
  __attribute__ ((aligned (SRSLTE_SIMD_I_SIZE*sizeof(int)))) int indexes_buffer[SRSLTE_SIMD_I_SIZE] = {0};
  __attribute__ ((aligned (SRSLTE_SIMD_I_SIZE*sizeof(float)))) float values_buffer[SRSLTE_SIMD_I_SIZE] = {0};

  for (int k = 0; k < SRSLTE_SIMD_I_SIZE; k++) indexes_buffer[k] = k;
  simd_i_t simd_inc = srslte_simd_i_set1(SRSLTE_SIMD_I_SIZE);
  simd_i_t simd_indexes = srslte_simd_i_load(indexes_buffer);
  simd_i_t simd_max_indexes = srslte_simd_i_set1(0);

  simd_f_t simd_max_values = srslte_simd_f_set1(-INFINITY);

  if (SRSLTE_IS_ALIGNED(x)) {
    for (; i < len - SRSLTE_SIMD_I_SIZE + 1; i += SRSLTE_SIMD_I_SIZE) {
      simd_f_t x1 = srslte_simd_f_load((float *) &x[i]);
      simd_f_t x2 = srslte_simd_f_load((float *) &x[i + SRSLTE_SIMD_F_SIZE / 2]);

      simd_f_t mul1 = srslte_simd_f_mul(x1, x1);
      simd_f_t mul2 = srslte_simd_f_mul(x2, x2);

      simd_f_t z1 = srslte_simd_f_hadd(mul1, mul2);

      simd_sel_t res = srslte_simd_f_max(z1, simd_max_values);

      simd_max_indexes = srslte_simd_i_select(simd_max_indexes, simd_indexes, res);
      simd_max_values = (simd_f_t) srslte_simd_i_select((simd_i_t) simd_max_values, (simd_i_t) z1, res);
      simd_indexes = srslte_simd_i_add(simd_indexes, simd_inc);
    }
  } else {
    for (; i < len - SRSLTE_SIMD_I_SIZE + 1; i += SRSLTE_SIMD_I_SIZE) {
      simd_f_t x1 = srslte_simd_f_loadu((float *) &x[i]);
      simd_f_t x2 = srslte_simd_f_loadu((float *) &x[i + SRSLTE_SIMD_F_SIZE / 2]);

      simd_f_t mul1 = srslte_simd_f_mul(x1, x1);
      simd_f_t mul2 = srslte_simd_f_mul(x2, x2);

      simd_f_t z1 = srslte_simd_f_hadd(mul1, mul2);

      simd_sel_t res = srslte_simd_f_max(z1, simd_max_values);

      simd_max_indexes = srslte_simd_i_select(simd_max_indexes, simd_indexes, res);
      simd_max_values = (simd_f_t) srslte_simd_i_select((simd_i_t) simd_max_values, (simd_i_t) z1, res);
      simd_indexes = srslte_simd_i_add(simd_indexes, simd_inc);
    }
  }

  srslte_simd_i_store(indexes_buffer, simd_max_indexes);
  srslte_simd_f_store(values_buffer, simd_max_values);

  for (int k = 0; k < SRSLTE_SIMD_I_SIZE; k++) {
    if (values_buffer[k] > max_value) {
      max_value = values_buffer[k];
      max_index = (uint32_t) indexes_buffer[k];
    }
  }
#endif /* SRSLTE_SIMD_I_SIZE */

  for (; i < len; i++) {
    cf_t a = x[i];
    float abs2 = __real__ a * __real__ a + __imag__ a * __imag__ a;
    if (abs2 > max_value) {
      max_value = abs2;
      max_index = (uint32_t)i;
    }
  }

  return max_index;
}

void srslte_vec_interleave_simd(const cf_t *x, const cf_t *y, cf_t *z, const int len) {
  uint32_t i = 0, k = 0;

#ifdef LV_HAVE_SSE
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - 2 + 1; i += 2) {
      __m128i a = _mm_load_si128((__m128i *) &x[i]);
      __m128i b = _mm_load_si128((__m128i *) &y[i]);

      __m128i r1 = _mm_unpacklo_epi64(a, b);
      _mm_store_si128((__m128i *) &z[k], r1);
      k += 2;

      __m128i r2 = _mm_unpackhi_epi64(a, b);
      _mm_store_si128((__m128i *) &z[k], r2);
      k += 2;
    }
  } else {
    for (; i < len - 2 + 1; i += 2) {
      __m128i a = _mm_loadu_si128((__m128i *) &x[i]);
      __m128i b = _mm_loadu_si128((__m128i *) &y[i]);

      __m128i r1 = _mm_unpacklo_epi64(a, b);
      _mm_storeu_si128((__m128i *) &z[k], r1);
      k += 2;

      __m128i r2 = _mm_unpackhi_epi64(a, b);
      _mm_storeu_si128((__m128i *) &z[k], r2);
      k += 2;
    }
  }
#endif /* LV_HAVE_SSE */

  for (;i < len; i++) {
    z[k++] = x[i];
    z[k++] = y[i];
  }
}

void srslte_vec_interleave_add_simd(const cf_t *x, const cf_t *y, cf_t *z, const int len) {
  uint32_t i = 0, k = 0;

#ifdef LV_HAVE_SSE
  if (SRSLTE_IS_ALIGNED(x) && SRSLTE_IS_ALIGNED(y) && SRSLTE_IS_ALIGNED(z)) {
    for (; i < len - 2 + 1; i += 2) {
      __m128i a = _mm_load_si128((__m128i *) &x[i]);
      __m128i b = _mm_load_si128((__m128i *) &y[i]);

      __m128 r1 = (__m128) _mm_unpacklo_epi64(a, b);
      __m128 z1 = _mm_load_ps((float *) &z[k]);
      r1 = _mm_add_ps((__m128) r1, z1);
      _mm_store_ps((float *) &z[k], r1);
      k += 2;

      __m128 r2 = (__m128) _mm_unpackhi_epi64(a, b);
      __m128 z2 = _mm_load_ps((float *) &z[k]);
      r2 = _mm_add_ps((__m128) r2, z2);
      _mm_store_ps((float *) &z[k], r2);
      k += 2;
    }
  } else {
    for (; i < len - 2 + 1; i += 2) {
      __m128i a = _mm_loadu_si128((__m128i *) &x[i]);
      __m128i b = _mm_loadu_si128((__m128i *) &y[i]);

      __m128 r1 = (__m128) _mm_unpacklo_epi64(a, b);
      __m128 z1 = _mm_loadu_ps((float *) &z[k]);
      r1 = _mm_add_ps((__m128) r1, z1);
      _mm_storeu_ps((float *) &z[k], r1);
      k += 2;

      __m128 r2 = (__m128) _mm_unpackhi_epi64(a, b);
      __m128 z2 = _mm_loadu_ps((float *) &z[k]);
      r2 = _mm_add_ps((__m128) r2, z2);
      _mm_storeu_ps((float *) &z[k], r2);
      k += 2;
    }
  }
#endif /* LV_HAVE_SSE */

  for (;i < len; i++) {
    z[k++] += x[i];
    z[k++] += y[i];
  }
}
