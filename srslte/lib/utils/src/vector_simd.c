/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
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

#include "srslte/utils/vector_simd.h"

#include <inttypes.h>
#include <stdio.h>

#include <xmmintrin.h>

void print128_num(__m128i var)
{
    int16_t *val = (int16_t*) &var;//can also use uint32_t instead of 16_t
    printf("Numerical: %d %d %d %d %d %d %d %d \n", 
           val[0], val[1], val[2], val[3], val[4], val[5], 
           val[6], val[7]);
}

void srslte_vec_sum_sss_simd(short *x, short *y, short *z, uint32_t len)
{
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
}

void srslte_vec_sub_sss_simd(short *x, short *y, short *z, uint32_t len)
{
  unsigned int number = 0;
  const unsigned int points = len / 8;

  const __m128i* xPtr = (const __m128i*) x;
  const __m128i* yPtr = (const __m128i*) y;
  __m128i* zPtr = (__m128i*) z;

  __m128i xVal, yVal, zVal;
  for(;number < points; number++){

    xVal = _mm_load_si128(xPtr);
    yVal = _mm_load_si128(yPtr);

    zVal = _mm_sub_epi16(xVal, yVal);

    _mm_store_si128(zPtr, zVal); 

    xPtr ++;
    yPtr ++;
    zPtr ++;
  }

  number = points * 8;
  for(;number < len; number++){
    z[number] = x[number] - y[number];
  }
}

void srslte_vec_sc_div2_sss_simd(short *x, int k, short *z, uint32_t len)
{
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
}

