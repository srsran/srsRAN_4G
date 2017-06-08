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


#include <float.h>
#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "srslte/phy/utils/vector_simd.h"

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <smmintrin.h>
#endif

#ifdef LV_HAVE_AVX
#include <immintrin.h>
#endif


int srslte_vec_dot_prod_sss_sse(short *x, short *y, uint32_t len)
{
  int result = 0; 
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int points = len / 8;

  const __m128i* xPtr = (const __m128i*) x;
  const __m128i* yPtr = (const __m128i*) y;
  
  __m128i dotProdVal = _mm_setzero_si128();

  __m128i xVal, yVal, zVal;
  for(;number < points; number++){

    xVal = _mm_load_si128(xPtr);
    yVal = _mm_loadu_si128(yPtr);

    zVal = _mm_mullo_epi16(xVal, yVal);

    dotProdVal = _mm_add_epi16(dotProdVal, zVal);

    xPtr ++;
    yPtr ++;
  }
  
  short dotProdVector[8];
  _mm_store_si128((__m128i*) dotProdVector, dotProdVal);
  for (int i=0;i<8;i++) {
    result += dotProdVector[i]; 
  }

  number = points * 8;
  for(;number < len; number++){
    result += (x[number] * y[number]);
  }
  
#endif
  return result; 
}


int srslte_vec_dot_prod_sss_avx(short *x, short *y, uint32_t len)
{
  int result = 0; 
#ifdef LV_HAVE_AVX
  unsigned int number = 0;
  const unsigned int points = len / 16;

  const __m256i* xPtr = (const __m256i*) x;
  const __m256i* yPtr = (const __m256i*) y;
  
  __m256i dotProdVal = _mm256_setzero_si256();

  __m256i xVal, yVal, zVal;
  for(;number < points; number++){

    xVal = _mm256_load_si256(xPtr);
    yVal = _mm256_loadu_si256(yPtr);
    zVal = _mm256_mullo_epi16(xVal, yVal);
    dotProdVal = _mm256_add_epi16(dotProdVal, zVal);
    xPtr ++;
    yPtr ++;
  }
  
  short dotProdVector[16];
  _mm256_store_si256((__m256i*) dotProdVector, dotProdVal);
  for (int i=0;i<16;i++) {
    result += dotProdVector[i]; 
  }

  number = points * 16;
  for(;number < len; number++){
    result += (x[number] * y[number]);
  }
  
#endif
  return result; 
}



void srslte_vec_sum_sss_sse(short *x, short *y, short *z, uint32_t len)
{
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int points = len / 8;

  const __m128i* xPtr = (const __m128i*) x;
  const __m128i* yPtr = (const __m128i*) y;
  __m128i* zPtr = (__m128i*) z;

  __m128i xVal, yVal, zVal;
  for(;number < points; number++){

    xVal = _mm_load_si128(xPtr);
    yVal = _mm_load_si128(yPtr);

    zVal = _mm_add_epi16(xVal, yVal);

    _mm_store_si128(zPtr, zVal); 

    xPtr ++;
    yPtr ++;
    zPtr ++;
  }

  number = points * 8;
  for(;number < len; number++){
    z[number] = x[number] + y[number];
  }
#endif

}

void srslte_vec_sum_sss_avx(short *x, short *y, short *z, uint32_t len)
{
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int points = len / 16;

  const __m256i* xPtr = (const __m256i*) x;
  const __m256i* yPtr = (const __m256i*) y;
  __m256i* zPtr = (__m256i*) z;

  __m256i xVal, yVal, zVal;
  for(;number < points; number++){

    xVal = _mm256_load_si256(xPtr);
    yVal = _mm256_loadu_si256(yPtr);

    zVal = _mm256_add_epi16(xVal, yVal);
    _mm256_store_si256(zPtr, zVal); 

    xPtr ++;
    yPtr ++;
    zPtr ++;
  }

  number = points * 16;
  for(;number < len; number++){
    z[number] = x[number] + y[number];
  }
#endif

}


void srslte_vec_sub_sss_sse(short *x, short *y, short *z, uint32_t len)
{
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int points = len / 8;

  const __m128i* xPtr = (const __m128i*) x;
  const __m128i* yPtr = (const __m128i*) y;
  __m128i* zPtr = (__m128i*) z;

  __m128i xVal, yVal, zVal;
  for(;number < points; number++){

    xVal = _mm_loadu_si128(xPtr);
    yVal = _mm_loadu_si128(yPtr);

    zVal = _mm_sub_epi16(xVal, yVal);

    _mm_storeu_si128(zPtr, zVal); 

    xPtr ++;
    yPtr ++;
    zPtr ++;
  }

  number = points * 8;
  for(;number < len; number++){
    z[number] = x[number] - y[number];
  }
#endif
}

void srslte_vec_sub_sss_avx(short *x, short *y, short *z, uint32_t len)
{
#ifdef LV_HAVE_AVX
  unsigned int number = 0;
  const unsigned int points = len / 16;

  const __m256i* xPtr = (const __m256i*) x;
  const __m256i* yPtr = (const __m256i*) y;
  __m256i* zPtr = (__m256i*) z;

  __m256i xVal, yVal, zVal;
  for(;number < points; number++){

    xVal = _mm256_load_si256(xPtr);
    yVal = _mm256_loadu_si256(yPtr);

    zVal = _mm256_sub_epi16(xVal, yVal);

    _mm256_store_si256(zPtr, zVal); 

    xPtr ++;
    yPtr ++;
    zPtr ++;
  }

  number = points * 16;
  for(;number < len; number++){
    z[number] = x[number] - y[number];
  }
  #endif
}




void srslte_vec_prod_sss_sse(short *x, short *y, short *z, uint32_t len)
{
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int points = len / 8;

  const __m128i* xPtr = (const __m128i*) x;
  const __m128i* yPtr = (const __m128i*) y;
  __m128i* zPtr = (__m128i*) z;

  __m128i xVal, yVal, zVal;
  for(;number < points; number++){

    xVal = _mm_loadu_si128(xPtr);
    yVal = _mm_loadu_si128(yPtr);

    zVal = _mm_mullo_epi16(xVal, yVal);

    _mm_storeu_si128(zPtr, zVal); 

    xPtr ++;
    yPtr ++;
    zPtr ++;
  }

  number = points * 8;
  for(;number < len; number++){
    z[number] = x[number] * y[number];
  }
#endif
}

void srslte_vec_prod_sss_avx(short *x, short *y, short *z, uint32_t len)
{
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int points = len / 16;

  const __m256i* xPtr = (const __m256i*) x;
  const __m256i* yPtr = (const __m256i*) y;
  __m256i* zPtr = (__m256i*) z;

  __m256i xVal, yVal, zVal;
  for(;number < points; number++){

    xVal = _mm256_loadu_si256(xPtr);
    yVal = _mm256_loadu_si256(yPtr);

    zVal = _mm256_mullo_epi16(xVal, yVal);

    _mm256_storeu_si256(zPtr, zVal); 

    xPtr ++;
    yPtr ++;
    zPtr ++;
  }

  number = points * 16;
  for(;number < len; number++){
    z[number] = x[number] * y[number];
  }
#endif
}






void srslte_vec_sc_div2_sss_sse(short *x, int k, short *z, uint32_t len)
{
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int points = len / 8;

  const __m128i* xPtr = (const __m128i*) x;
  __m128i* zPtr = (__m128i*) z;

  __m128i xVal, zVal;
  for(;number < points; number++){

    xVal = _mm_loadu_si128(xPtr);
    
    zVal = _mm_srai_epi16(xVal, k);                 
      
    _mm_storeu_si128(zPtr, zVal); 

    xPtr ++;
    zPtr ++;
  }

  number = points * 8;
  short divn = (1<<k);
  for(;number < len; number++){
    z[number] = x[number] / divn;
  }
#endif
}

void srslte_vec_sc_div2_sss_avx(short *x, int k, short *z, uint32_t len)
{
#ifdef LV_HAVE_AVX
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
}



/* No improvement with AVX */
void srslte_vec_lut_sss_sse(short *x, unsigned short *lut, short *y, uint32_t len)
{
#ifndef DEBUG_MODE
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
void srslte_vec_sum_fff_simd(float *x, float *y, float *z, uint32_t len) {
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

void srslte_vec_sub_fff_simd(float *x, float *y, float *z, uint32_t len) {
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

    zVal = _mm_sub_ps(xVal, yVal);

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


#ifdef LV_HAVE_SSE
static inline __m128 _mm_complexmul_ps(__m128 x, __m128 y) {
  __m128 yl, yh, tmp1, tmp2;
  yl = _mm_moveldup_ps(y); // Load yl with cr,cr,dr,dr
  yh = _mm_movehdup_ps(y); // Load yh with ci,ci,di,di
  tmp1 = _mm_mul_ps(x, yl); // tmp1 = ar*cr,ai*cr,br*dr,bi*dr
  x = _mm_shuffle_ps(x, x, 0xB1); // Re-arrange x to be ai,ar,bi,br
  tmp2 = _mm_mul_ps(x, yh); // tmp2 = ai*ci,ar*ci,bi*di,br*di
  return _mm_addsub_ps(tmp1, tmp2); // ar*cr-ai*ci, ai*cr+ar*ci, br*dr-bi*di, bi*dr+br*di
}
#endif


#ifdef LV_HAVE_SSE
static inline __m128 _mm_complexmulconj_ps(__m128 x, __m128 y) {
  const __m128 conjugator = _mm_setr_ps(0, -0.f, 0, -0.f);
  y = _mm_xor_ps(y, conjugator); 
  return _mm_complexmul_ps(x, y);
}
#endif

cf_t srslte_vec_dot_prod_ccc_simd(cf_t *x, cf_t *y, uint32_t len)
{
  cf_t result = 0; 
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int points = len / 2;

  const float* xPtr = (const float*) x;
  const float* yPtr = (const float*) y;
  
  __m128 dotProdVal = _mm_setzero_ps();

  __m128 xVal, yVal, zVal;
  for(;number < points; number++){

    xVal = _mm_loadu_ps(xPtr);
    yVal = _mm_loadu_ps(yPtr);

    zVal = _mm_complexmul_ps(xVal, yVal);

    dotProdVal = _mm_add_ps(dotProdVal, zVal);

    xPtr += 4;
    yPtr += 4;
  }
  
  cf_t dotProdVector[2];
  _mm_storeu_ps((float*) dotProdVector, dotProdVal);
  for (int i=0;i<2;i++) {
    result += dotProdVector[i]; 
  }

  number = points * 2;
  for(;number < len; number++){
    result += (x[number] * y[number]);
  }
  
#endif
  return result; 
}


cf_t srslte_vec_dot_prod_conj_ccc_simd(cf_t *x, cf_t *y, uint32_t len)
{
  cf_t result = 0; 
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int points = len / 2;

  const float* xPtr = (const float*) x;
  const float* yPtr = (const float*) y;
  
  __m128 dotProdVal = _mm_setzero_ps();

  __m128 xVal, yVal, zVal;
  for(;number < points; number++){

    xVal = _mm_loadu_ps(xPtr);
    yVal = _mm_loadu_ps(yPtr);

    zVal = _mm_complexmulconj_ps(xVal, yVal);

    dotProdVal = _mm_add_ps(dotProdVal, zVal);

    xPtr += 4;
    yPtr += 4;
  }
  
  cf_t dotProdVector[2];
  _mm_storeu_ps((float*) dotProdVector, dotProdVal);
  for (int i=0;i<2;i++) {
    result += dotProdVector[i]; 
  }

  number = points * 2;
  for(;number < len; number++){
    result += (x[number] * y[number]);
  }
  
#endif
  return result; 
}
void srslte_vec_prod_ccc_simd(cf_t *x,cf_t *y, cf_t *z, uint32_t len) 
{
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int halfPoints = len / 2;

  __m128 xVal, yVal, zVal;
  float* zPtr = (float*) z;
  const float* xPtr = (const float*) x;
  const float* yPtr = (const float*) y;

  for(; number < halfPoints; number++){
    xVal = _mm_loadu_ps(xPtr); 
    yVal = _mm_loadu_ps(yPtr); 
    zVal = _mm_complexmul_ps(xVal, yVal);
    _mm_storeu_ps(zPtr, zVal); 

    xPtr += 4;
    yPtr += 4;
    zPtr += 4;
  }

  number = halfPoints * 2;
  for(;number < len; number++){
    z[number] = x[number] * y[number];
  }
#endif
}


void srslte_vec_prod_conj_ccc_simd(cf_t *x,cf_t *y, cf_t *z, uint32_t len) {
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int halfPoints = len / 2;

  __m128 xVal, yVal, zVal;
  float* zPtr = (float*) z;
  const float* xPtr = (const float*) x;
  const float* yPtr = (const float*) y;

  for(; number < halfPoints; number++){
    xVal = _mm_loadu_ps(xPtr); 
    yVal = _mm_loadu_ps(yPtr); 
    zVal = _mm_complexmulconj_ps(xVal, yVal);
    _mm_storeu_ps(zPtr, zVal); 

    xPtr += 4;
    yPtr += 4;
    zPtr += 4;
  }

  number = halfPoints * 2;
  for(;number < len; number++){
    z[number] = x[number] * conjf(y[number]);
  }
#endif
}

void srslte_vec_sc_prod_ccc_simd(cf_t *x, cf_t h, cf_t *z, uint32_t len) {
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int halfPoints = len / 2;

  __m128 xVal, yl, yh, zVal, tmp1, tmp2;
  float* zPtr = (float*) z;
  const float* xPtr = (const float*) x;

  // Set up constant scalar vector
  yl = _mm_set_ps1(creal(h));
  yh = _mm_set_ps1(cimag(h));

  for(;number < halfPoints; number++){

    xVal = _mm_loadu_ps(xPtr); 
    tmp1 = _mm_mul_ps(xVal,yl); 
    xVal = _mm_shuffle_ps(xVal,xVal,0xB1); 
    tmp2 = _mm_mul_ps(xVal,yh); 
    zVal = _mm_addsub_ps(tmp1,tmp2); 
    _mm_storeu_ps(zPtr,zVal); 

    xPtr += 4;
    zPtr += 4;
  }

  number = halfPoints * 2;
  for(;number < len; number++){
    z[number] = x[number] * h;
  }
#endif
}


void srslte_vec_sc_prod_cfc_simd(cf_t *x, float h, cf_t *z, uint32_t len) {
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int halfPoints = len / 2;

  __m128 xVal, hVal, zVal;
  float* zPtr = (float*) z;
  const float* xPtr = (const float*) x;

  // Set up constant scalar vector
  hVal = _mm_set_ps1(h);
  
  for(;number < halfPoints; number++){

    xVal = _mm_loadu_ps(xPtr); 
    zVal = _mm_mul_ps(xVal,hVal); 
    _mm_storeu_ps(zPtr,zVal); 

    xPtr += 4;
    zPtr += 4;
  }

  number = halfPoints * 2;
  for(;number < len; number++){
    z[number] = x[number] * h;
  }

#endif
}



void srslte_vec_sc_prod_fff_simd(float *x, float h, float *z, uint32_t len) {
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int quarterPoints = len / 4;

  __m128 xVal, hVal, zVal;
  float* zPtr = (float*) z;
  const float* xPtr = (const float*) x;

  // Set up constant scalar vector
  hVal = _mm_set_ps1(h);
  
  for(;number < quarterPoints; number++){

    xVal = _mm_loadu_ps(xPtr); 
    zVal = _mm_mul_ps(xVal,hVal); 
    _mm_storeu_ps(zPtr,zVal); 

    xPtr += 4;
    zPtr += 4;
  }

  number = quarterPoints * 4;
  for(;number < len; number++){
    z[number] = x[number] * h;
  }

#endif
}


void srslte_vec_abs_square_cf_simd(cf_t *x, float *z, uint32_t len) {
#ifdef LV_HAVE_SSE
  unsigned int number = 0;
  const unsigned int quarterPoints = len / 4;

  const float* xPtr = (const float*) x;
  float* zPtr = z;

  __m128 xVal1, xVal2, zVal;
  for(; number < quarterPoints; number++){
    xVal1 = _mm_loadu_ps(xPtr);
    xPtr += 4;
    xVal2 = _mm_loadu_ps(xPtr);
    xPtr += 4;
    xVal1 = _mm_mul_ps(xVal1, xVal1); 
    xVal2 = _mm_mul_ps(xVal2, xVal2); 
    zVal = _mm_hadd_ps(xVal1, xVal2);
    _mm_storeu_ps(zPtr, zVal);
    zPtr += 4;
  }

  number = quarterPoints * 4;
  for(;number < len; number++){
    z[number] = creal(x[number]) * creal(x[number]) + cimag(x[number])*cimag(x[number]);
  }
#endif
}


//srslte_32fc_s32f_multiply_32fc_avx
 void srslte_vec_mult_scalar_cf_f_avx( cf_t *z,const cf_t *x,const float h,const uint32_t len)
{
#ifdef LV_HAVE_AVX
   
  unsigned int i = 0;
  const unsigned int loops = len/4;
  //__m256 outputVec;
    cf_t *xPtr = (cf_t*) x;
    cf_t *zPtr = (cf_t*) z;
  
  __m256 inputVec, outputVec;
    const __m256 tapsVec  = _mm256_set1_ps(h);
  for(;i < loops;i++)
  {
      inputVec  = _mm256_loadu_ps((float*)xPtr);
      //__builtin_prefetch(xPtr+4);
      outputVec = _mm256_mul_ps(inputVec,tapsVec);
      _mm256_storeu_ps((float*)zPtr,outputVec);
      xPtr += 4;
      zPtr += 4;
  }
  
  for(i = loops * 4;i < len;i++)
  {
      *zPtr++ = (*xPtr++) * h;
  }
#endif
}
