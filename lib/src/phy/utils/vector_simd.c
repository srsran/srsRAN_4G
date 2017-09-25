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


int srslte_vec_dot_prod_sss_simd(int16_t *x, int16_t *y, int len) {
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

void srslte_vec_sum_sss_simd(int16_t *x, int16_t *y, int16_t *z, int len) {
  int i = 0;
#ifdef SRSLTE_SIMD_S_SIZE
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

void srslte_vec_sub_sss_simd(int16_t *x, int16_t *y, int16_t *z, int len) {
  int i = 0;
#ifdef SRSLTE_SIMD_S_SIZE
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

void srslte_vec_prod_sss_simd(int16_t *x, int16_t *y, int16_t *z, int len) {
  int i = 0;
#ifdef SRSLTE_SIMD_S_SIZE
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




#warning remove function if it is not used
/*
void srslte_vec_sc_div2_sss_sse(short *x, int k, short *z, uint32_t len)
{
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int points = len / 8;

  const __m128i* xPtr = (const __m128i*) x;
  __m128i* zPtr = (__m128i*) z;

  __m128i xVal, zVal;
  for(;number < points; number++){

    xVal = _mm_load_si128(xPtr);
    
    zVal = _mm_srai_epi16(xVal, k);                 
      
    _mm_store_si128(zPtr, zVal);

    xPtr ++;
    zPtr ++;
  }

  number = points * 8;
  short divn = (1<<k);
  for(;number < len; number++){
    z[number] = x[number] / divn;
  }
#endif
}*/

#warning remove function if it is not used
/*
void srslte_vec_sc_div2_sss_avx2(short *x, int k, short *z, uint32_t len)
{
#ifdef LV_HAVE_AVX2
  unsigned int number = 0;
  const unsigned int points = len / 16;

  const __m256i* xPtr = (const __m256i*) x;
  __m256i* zPtr = (__m256i*) z;

  __m256i xVal, zVal;
  for(;number < points; number++){

    xVal = _mm256_load_si256(xPtr);
    
    zVal = _mm256_srai_epi16(xVal, k);                 
      
    _mm256_store_si256(zPtr, zVal); 

    xPtr ++;
    zPtr ++;
  }

  number = points * 16;
  short divn = (1<<k);
  for(;number < len; number++){
    z[number] = x[number] / divn;
  }
#endif
}*/



/* No improvement with AVX */
void srslte_vec_lut_sss_sse(short *x, unsigned short *lut, short *y, uint32_t len)
{
#ifdef DEBUG_MODE
  for (int i=0;i<len;i++) {
    y[lut[i]] = x[i];
  }
#else
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int points = len / 8;

  const __m128i* xPtr = (const __m128i*) x;
  const __m128i* lutPtr = (__m128i*) lut;

  __m128i xVal, lutVal;
  for(;number < points; number++){

    xVal   = _mm_loadu_si128(xPtr);
    lutVal = _mm_loadu_si128(lutPtr);
    
    for (int i=0;i<8;i++) {
      int16_t x = (int16_t)   _mm_extract_epi16(xVal, i); 
      uint16_t l = (uint16_t) _mm_extract_epi16(lutVal, i);
      y[l] = x;
    }
    xPtr ++;
    lutPtr ++;
  }

  number = points * 8;
  for(;number < len; number++){
    y[lut[number]] = x[number];
  }
#endif  
#endif
}

/* Modified from volk_32f_s32f_convert_16i_a_simd2. Removed clipping */
void srslte_vec_convert_fi_sse(float *x, int16_t *z, float scale, uint32_t len)
{
#ifdef LV_HAVE_SSE
  unsigned int number = 0;

  const unsigned int eighthPoints = len / 8;

  const float* inputVectorPtr = (const float*)x;
  int16_t* outputVectorPtr = z;

  __m128 vScalar = _mm_set_ps1(scale);
  __m128 inputVal1, inputVal2;
  __m128i intInputVal1, intInputVal2;
  __m128 ret1, ret2;

  for(;number < eighthPoints; number++){
    inputVal1 = _mm_loadu_ps(inputVectorPtr); inputVectorPtr += 4;
    inputVal2 = _mm_loadu_ps(inputVectorPtr); inputVectorPtr += 4;

    ret1 = _mm_mul_ps(inputVal1, vScalar);
    ret2 = _mm_mul_ps(inputVal2, vScalar);

    intInputVal1 = _mm_cvtps_epi32(ret1);
    intInputVal2 = _mm_cvtps_epi32(ret2);

    intInputVal1 = _mm_packs_epi32(intInputVal1, intInputVal2);

    _mm_storeu_si128((__m128i*)outputVectorPtr, intInputVal1);
    outputVectorPtr += 8;
  }

  number = eighthPoints * 8;
  for(; number < len; number++){
    z[number] = (int16_t) (x[number] * scale);
  }
#endif
}


// for enb no-volk
void srslte_vec_sum_fff_sse(float *x, float *y, float *z, uint32_t len) {
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int points = len / 4;

  const float* xPtr = (const float*) x;
  const float* yPtr = (const float*) y;
  float* zPtr = (float*) z;

  __m128 xVal, yVal, zVal;
  for(;number < points; number++){

    xVal = _mm_loadu_ps(xPtr);
    yVal = _mm_loadu_ps(yPtr);

    zVal = _mm_add_ps(xVal, yVal);

    _mm_storeu_ps(zPtr, zVal);

    xPtr += 4;
    yPtr += 4;
    zPtr += 4;
  }

  number = points * 4;
  for(;number < len; number++){
    z[number] = x[number] + y[number];
  }
#endif
}

void srslte_vec_sum_fff_avx(float *x, float *y, float *z, uint32_t len) {
#ifdef LV_HAVE_AVX
  unsigned int number = 0;
  const unsigned int points = len / 8;

  const float* xPtr = (const float*) x;
  const float* yPtr = (const float*) y;
  float* zPtr = (float*) z;

  __m256 xVal, yVal, zVal;
  for(;number < points; number++){

    xVal = _mm256_loadu_ps(xPtr);
    yVal = _mm256_loadu_ps(yPtr);

    zVal = _mm256_add_ps(xVal, yVal);

    _mm256_storeu_ps(zPtr, zVal);

    xPtr += 8;
    yPtr += 8;
    zPtr += 8;
  }

  for(number = points * 8;number < len; number++){
    z[number] = x[number] + y[number];
  }
#endif
}

cf_t srslte_vec_acc_cc_simd(cf_t *x, int len) {
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

void srslte_vec_add_fff_simd(float *x, float *y, float *z, int len) {
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

void srslte_vec_sub_fff_simd(float *x, float *y, float *z, int len) {
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

cf_t srslte_vec_dot_prod_ccc_simd(cf_t *x, cf_t *y, int len) {
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

c16_t srslte_vec_dot_prod_ccc_c16i_simd(c16_t *x, c16_t *y, int len) {
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

cf_t srslte_vec_dot_prod_conj_ccc_simd(cf_t *x, cf_t *y, int len)
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

void srslte_vec_prod_fff_simd(float *x, float *y, float *z, int len) {
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

void srslte_vec_prod_ccc_simd(cf_t *x,cf_t *y, cf_t *z, int len) {
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

void srslte_vec_prod_ccc_cf_simd(float *a_re, float *a_im, float *b_re, float *b_im, float *r_re, float *r_im, int len) {
  int i = 0;

#if SRSLTE_SIMD_F_SIZE
  for (; i < len - SRSLTE_SIMD_CF_SIZE + 1; i += SRSLTE_SIMD_CF_SIZE) {
    simd_cf_t a = srslte_simd_cf_load(&a_re[i], &a_im[i]);
    simd_cf_t b = srslte_simd_cf_load(&b_re[i], &b_im[i]);

    simd_cf_t r = srslte_simd_cf_prod(a, b);

    srslte_simd_cf_store(&r_re[i], &r_im[i], r);
  }
#endif

  for (; i<len; i++) {
    r_re[i] = a_re[i]*b_re[i] - a_im[i]*b_im[i];
    r_im[i] = a_re[i]*b_im[i] + a_im[i]*b_re[i];
  }
}

void srslte_vec_prod_ccc_c16_simd(int16_t *a_re, int16_t *a_im, int16_t *b_re, int16_t *b_im, int16_t *r_re,
                                  int16_t *r_im, int len) {
  int i = 0;

#if SRSLTE_SIMD_C16_SIZE
  for (; i < len - SRSLTE_SIMD_C16_SIZE + 1; i += SRSLTE_SIMD_C16_SIZE) {
    simd_c16_t a = srslte_simd_c16_load(&a_re[i], &a_im[i]);
    simd_c16_t b = srslte_simd_c16_load(&b_re[i], &b_im[i]);

    simd_c16_t r = srslte_simd_c16_prod(a, b);

    srslte_simd_c16_store(&r_re[i], &r_im[i], r);
  }
#endif

  for (; i<len; i++) {
    r_re[i] = a_re[i]*b_re[i] - a_im[i]*b_im[i];
    r_im[i] = a_re[i]*b_im[i] + a_im[i]*b_re[i];
  }
}

void srslte_vec_prod_conj_ccc_simd(cf_t *x,cf_t *y, cf_t *z, int len) {
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

void srslte_vec_sc_prod_ccc_simd(cf_t *x, cf_t h, cf_t *z, int len) {
  int i = 0;

#if SRSLTE_SIMD_F_SIZE
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
      simd_f_t temp = srslte_simd_f_load((float *) &x[i]);

      simd_f_t m1 = srslte_simd_f_mul(hre, temp);
      simd_f_t sw = srslte_simd_f_swap(temp);
      simd_f_t m2 = srslte_simd_f_mul(him, sw);
      simd_f_t r = srslte_simd_f_addsub(m1, m2);

      srslte_simd_f_store((float *) &z[i], r);
    }
  }
#endif

  for (; i < len; i++) {
    z[i] = x[i] * h;
  }
}

void srslte_vec_sc_prod_fff_simd(float *x, float h, float *z, int len) {
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

void srslte_vec_abs_cf_simd(cf_t *x, float *z, int len) {
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

void srslte_vec_abs_square_cf_simd(cf_t *x, float *z, int len) {
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

void srslte_vec_cp_simd(cf_t *src, cf_t *dst, int len) {
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
