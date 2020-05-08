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

#ifndef SRSLTE_SIMD_H
#define SRSLTE_SIMD_H

#ifdef LV_HAVE_SSE /* AVX, AVX2, FMA, AVX512  are in this group */
#ifndef __OPTIMIZE__
#define __OPTIMIZE__
#endif
#include <immintrin.h>
#endif /* LV_HAVE_SSE */

#include "srslte/config.h"
#include <stdio.h>

#ifdef HAVE_NEON
#include <arm_neon.h>
#endif

/*
 * SSE Macros
 */
#ifdef LV_HAVE_SSE
#define _MM_SWAP(X) ((__m128)_mm_shuffle_ps(X, X, _MM_SHUFFLE(2, 3, 0, 1)))
#define _MM_PERM(X) ((__m128)_mm_shuffle_ps(X, X, _MM_SHUFFLE(2, 1, 3, 0)))
#define _MM_MULJ_PS(X) _MM_SWAP(_MM_CONJ_PS(X))
#define _MM_CONJ_PS(X) (_mm_xor_ps(X, _mm_set_ps(-0.0f, 0.0f, -0.0f, 0.0f)))
#define _MM_SQMOD_PS(X) _MM_PERM(_mm_hadd_ps(_mm_mul_ps(X, X), _mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f)))
#define _MM_PROD_PS(a, b)                                                                                              \
  _mm_addsub_ps(_mm_mul_ps(a, _mm_moveldup_ps(b)), _mm_mul_ps(_mm_shuffle_ps(a, a, 0xB1), _mm_movehdup_ps(b)))

#endif /* LV_HAVE_SSE */

/*
 * AVX Macros
 */
#ifdef LV_HAVE_AVX

#define _MM256_MULJ_PS(X) _mm256_permute_ps(_MM256_CONJ_PS(X), 0b10110001)
#define _MM256_CONJ_PS(X) (_mm256_xor_ps(X, _mm256_set_ps(-0.0f, 0.0f, -0.0f, 0.0f, -0.0f, 0.0f, -0.0f, 0.0f)))

#ifdef LV_HAVE_FMA
#define _MM256_SQMOD_PS(A, B)                                                                                          \
  _mm256_permute_ps(_mm256_hadd_ps(_mm256_fmadd_ps(A, A, _mm256_mul_ps(B, B)),                                         \
                                   _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f)),                     \
                    0b11011100)
#define _MM256_PROD_PS(a, b)                                                                                           \
  _mm256_fmaddsub_ps(a, _mm256_moveldup_ps(b), _mm256_mul_ps(_mm256_shuffle_ps(a, a, 0xB1), _mm256_movehdup_ps(b)))
#else
#define _MM256_SQMOD_PS(A, B)                                                                                          \
  _mm256_permute_ps(_mm256_hadd_ps(_mm256_add_ps(_mm256_mul_ps(A, A), _mm256_mul_ps(B, B)),                            \
                                   _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f)),                     \
                    0b11011100)
#define _MM256_PROD_PS(a, b)                                                                                           \
  _mm256_addsub_ps(_mm256_mul_ps(a, _mm256_moveldup_ps(b)),                                                            \
                   _mm256_mul_ps(_mm256_shuffle_ps(a, a, 0xB1), _mm256_movehdup_ps(b)))
#endif /* LV_HAVE_FMA */
#endif /* LV_HAVE_AVX */

/*
 * AVX extension with FMA Macros
 */
#ifdef LV_HAVE_FMA

#define _MM256_SQMOD_ADD_PS(A, B, C)                                                                                   \
  _mm256_permute_ps(_mm256_hadd_ps(_mm256_fmadd_ps(A, A, _mm256_fmadd_ps(B, B, C)),                                    \
                                   _mm256_set_ps(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f)),                     \
                    0b11011100)

#define _MM256_PROD_ADD_PS(A, B, C)                                                                                    \
  _mm256_fmaddsub_ps(                                                                                                  \
      A, _mm256_moveldup_ps(B), _mm256_fmaddsub_ps(_mm256_shuffle_ps(A, A, 0xB1), _mm256_movehdup_ps(B), C))

#define _MM256_PROD_SUB_PS(A, B, C)                                                                                    \
  _mm256_fmaddsub_ps(                                                                                                  \
      A, _mm256_moveldup_ps(B), _mm256_fmsubadd_ps(_mm256_shuffle_ps(A, A, 0xB1), _mm256_movehdup_ps(B), C))
#endif /* LV_HAVE_FMA */

/*
 * SIMD Vector bit alignment
 */
#ifdef LV_HAVE_AVX512
#define SRSLTE_SIMD_BIT_ALIGN 512
#define SRSLTE_IS_ALIGNED(PTR) (((size_t)(PTR)&0x3F) == 0)
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX
#define SRSLTE_SIMD_BIT_ALIGN 256
#define SRSLTE_IS_ALIGNED(PTR) (((size_t)(PTR)&0x1F) == 0)
#else /* LV_HAVE_AVX */
#ifdef LV_HAVE_SSE
#define SRSLTE_SIMD_BIT_ALIGN 128
#define SRSLTE_IS_ALIGNED(PTR) (((size_t)(PTR)&0x0F) == 0)
#else /* LV_HAVE_SSE */
#define SRSLTE_SIMD_BIT_ALIGN 64
#define SRSLTE_IS_ALIGNED(PTR) (1)
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX */
#endif /* LV_HAVE_AVX512 */

#define srslte_simd_aligned __attribute__((aligned(SRSLTE_SIMD_BIT_ALIGN / 8)))

/* Memory Sizes for Single Floating Point and fixed point */
#ifdef LV_HAVE_AVX512

#define SRSLTE_SIMD_F_SIZE 16
#define SRSLTE_SIMD_CF_SIZE 16

#define SRSLTE_SIMD_I_SIZE 16

#define SRSLTE_SIMD_B_SIZE 64
#define SRSLTE_SIMD_S_SIZE 32
#define SRSLTE_SIMD_C16_SIZE 0

#else
#ifdef LV_HAVE_AVX2

#define SRSLTE_SIMD_F_SIZE 8
#define SRSLTE_SIMD_CF_SIZE 8

#define SRSLTE_SIMD_I_SIZE 8

#define SRSLTE_SIMD_B_SIZE 32
#define SRSLTE_SIMD_S_SIZE 16
#define SRSLTE_SIMD_C16_SIZE 16

#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE

#define SRSLTE_SIMD_F_SIZE 4
#define SRSLTE_SIMD_CF_SIZE 4

#define SRSLTE_SIMD_I_SIZE 4

#define SRSLTE_SIMD_B_SIZE 16
#define SRSLTE_SIMD_S_SIZE 8
#define SRSLTE_SIMD_C16_SIZE 8

#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON

#define SRSLTE_SIMD_F_SIZE 4
#define SRSLTE_SIMD_CF_SIZE 4

#define SRSLTE_SIMD_I_SIZE 4
#define SRSLTE_SIMD_B_SIZE 16
#define SRSLTE_SIMD_S_SIZE 8
#define SRSLTE_SIMD_C16_SIZE 8

#else /* HAVE_NEON */
#define SRSLTE_SIMD_F_SIZE 0
#define SRSLTE_SIMD_CF_SIZE 0

#define SRSLTE_SIMD_I_SIZE 0
#define SRSLTE_SIMD_B_SIZE 0
#define SRSLTE_SIMD_S_SIZE 0
#define SRSLTE_SIMD_C16_SIZE 0

#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */

#ifndef ENABLE_C16
#undef SRSLTE_SIMD_C16_SIZE
#define SRSLTE_SIMD_C16_SIZE 0
#endif /* ENABLE_C16 */

#if SRSLTE_SIMD_F_SIZE

/* Data types */
#ifdef LV_HAVE_AVX512
typedef __m512 simd_f_t;
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
typedef __m256 simd_f_t;
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
typedef __m128 simd_f_t;
#else /* HAVE_NEON */
#ifdef HAVE_NEON
typedef float32x4_t simd_f_t;
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */

/* Single precision Floating point functions */
static inline simd_f_t srslte_simd_f_load(const float* ptr)
{
#ifdef LV_HAVE_AVX512
  return _mm512_load_ps(ptr);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_load_ps(ptr);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_load_ps(ptr);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vld1q_f32(ptr);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_f_t srslte_simd_f_loadu(const float* ptr)
{
#ifdef LV_HAVE_AVX512
  return _mm512_loadu_ps(ptr);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_loadu_ps(ptr);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_loadu_ps(ptr);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vld1q_f32(ptr);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline void srslte_simd_f_store(float* ptr, simd_f_t simdreg)
{
#ifdef LV_HAVE_AVX512
  _mm512_store_ps(ptr, simdreg);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  _mm256_store_ps(ptr, simdreg);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  _mm_store_ps(ptr, simdreg);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  vst1q_f32(ptr, simdreg);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline void srslte_simd_f_storeu(float* ptr, simd_f_t simdreg)
{
#ifdef LV_HAVE_AVX512
  _mm512_storeu_ps(ptr, simdreg);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  _mm256_storeu_ps(ptr, simdreg);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  _mm_storeu_ps(ptr, simdreg);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  vst1q_f32(ptr, simdreg);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_f_t srslte_simd_f_set1(float x)
{
#ifdef LV_HAVE_AVX512
  return _mm512_set1_ps(x);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_set1_ps(x);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_set1_ps(x);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vdupq_n_f32(x);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_f_t srslte_simd_f_mul(simd_f_t a, simd_f_t b)
{
#ifdef LV_HAVE_AVX512
  return _mm512_mul_ps(a, b);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_mul_ps(a, b);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_mul_ps(a, b);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vmulq_f32(a, b);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_f_t srslte_simd_f_rcp(simd_f_t a)
{
#ifdef LV_HAVE_AVX512
  return _mm512_rcp14_ps(a);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_rcp_ps(a);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_rcp_ps(a);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vmulq_f32(vrecpeq_f32(a), vrecpsq_f32(vrecpeq_f32(a), a));
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_f_t srslte_simd_f_addsub(simd_f_t a, simd_f_t b)
{
#ifdef LV_HAVE_AVX512
  __m512 r = _mm512_add_ps(a, b);
  return _mm512_mask_sub_ps(r, 0b0101010101010101, a, b);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_addsub_ps(a, b);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_addsub_ps(a, b);
#else            /* LV_HAVE_SSE */
#ifdef HAVE_NEON // CURRENTLY USES GENERIC IMPLEMENTATION FOR NEON
  float*   a_ptr = (float*)&a;
  float*   b_ptr = (float*)&b;
  simd_f_t ret;
  float*   c_ptr = (float*)&ret;
  for (int i = 0; i < 4; i++) {
    if (i % 2 == 0) {
      c_ptr[i] = a_ptr[i] - b_ptr[i];
    } else {
      c_ptr[i] = a_ptr[i] + b_ptr[i];
    }
  }

  return ret;
#endif           /* HAVE_NEON */
#endif           /* LV_HAVE_SSE */
#endif           /* LV_HAVE_AVX2 */
#endif           /* LV_HAVE_AVX512 */
}

static inline simd_f_t srslte_simd_f_sub(simd_f_t a, simd_f_t b)
{
#ifdef LV_HAVE_AVX512
  return _mm512_sub_ps(a, b);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_sub_ps(a, b);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_sub_ps(a, b);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vsubq_f32(a, b);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_f_t srslte_simd_f_add(simd_f_t a, simd_f_t b)
{
#ifdef LV_HAVE_AVX512
  return _mm512_add_ps(a, b);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_add_ps(a, b);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_add_ps(a, b);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vaddq_f32(a, b);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_f_t srslte_simd_f_zero(void)
{
#ifdef LV_HAVE_AVX512
  return _mm512_setzero_ps();
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_setzero_ps();
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_setzero_ps();
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vdupq_n_f32(0);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_f_t srslte_simd_f_swap(simd_f_t a)
{
#ifdef LV_HAVE_AVX512
  return _mm512_permute_ps(a, 0b10110001);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_permute_ps(a, 0b10110001);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_shuffle_ps(a, a, 0b10110001);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vcombine_f32(vrev64_f32(vget_low_f32(a)), vrev64_f32(vget_high_f32(a)));
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_f_t srslte_simd_f_hadd(simd_f_t a, simd_f_t b)
{
#ifdef LV_HAVE_AVX512
  const __m512i idx1 = _mm512_setr_epi32((0b00000),
                                         (0b00010),
                                         (0b00100),
                                         (0b00110),
                                         (0b01000),
                                         (0b01010),
                                         (0b01100),
                                         (0b01110),
                                         (0b10000),
                                         (0b10010),
                                         (0b10100),
                                         (0b10110),
                                         (0b11000),
                                         (0b11010),
                                         (0b11100),
                                         (0b11110));
  const __m512i idx2 = _mm512_or_epi32(idx1, _mm512_set1_epi32(1));

  simd_f_t a1 = _mm512_permutex2var_ps(a, idx1, b);
  simd_f_t b1 = _mm512_permutex2var_ps(a, idx2, b);
  return _mm512_add_ps(a1, b1);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  simd_f_t a1 = _mm256_permute2f128_ps(a, b, 0b00100000);
  simd_f_t b1 = _mm256_permute2f128_ps(a, b, 0b00110001);
  return _mm256_hadd_ps(a1, b1);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_hadd_ps(a, b);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vcombine_f32(vpadd_f32(vget_low_f32(a), vget_high_f32(a)), vpadd_f32(vget_low_f32(b), vget_high_f32(b)));
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_f_t srslte_simd_f_sqrt(simd_f_t a)
{
#ifdef LV_HAVE_AVX512
  return _mm512_sqrt_ps(a);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_sqrt_ps(a);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_sqrt_ps(a);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  float32x4_t sqrt_reciprocal = vrsqrteq_f32(a);
  sqrt_reciprocal    = vmulq_f32(vrsqrtsq_f32(vmulq_f32(a, sqrt_reciprocal), sqrt_reciprocal), sqrt_reciprocal);
  float32x4_t result = vmulq_f32(a, sqrt_reciprocal);

  /* Detect zeros in NEON 1/sqrtf for preventing NaN */
  float32x4_t zeros = vmovq_n_f32(0);      /* Zero vector */
  uint32x4_t  mask  = vceqq_f32(a, zeros); /* Zero vector mask */
  return vbslq_f32(mask, zeros, result);   /* Force zero results and return */
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_f_t srslte_simd_f_neg(simd_f_t a)
{
#ifdef LV_HAVE_AVX512
  return _mm512_xor_ps(_mm512_set1_ps(-0.0f), a);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_xor_ps(_mm256_set1_ps(-0.0f), a);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_xor_ps(_mm_set1_ps(-0.0f), a);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vnegq_f32(a);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_f_t srslte_simd_f_neg_mask(simd_f_t a, simd_f_t mask)
{
#ifdef LV_HAVE_AVX512
  return _mm512_xor_ps(mask, a);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_xor_ps(mask, a);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_xor_ps(mask, a);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return (float32x4_t)veorq_s32((int32x4_t)a, (int32x4_t)mask);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_f_t srslte_simd_f_abs(simd_f_t a)
{
#ifdef LV_HAVE_AVX512
  return _mm512_andnot_ps(_mm512_set1_ps(-0.0f), a);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_andnot_ps(_mm256_set1_ps(-0.0f), a);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_andnot_ps(_mm_set1_ps(-0.0f), a);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vabsq_f32(a);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline void srslte_simd_f_fprintf(FILE* stream, simd_f_t a)
{
  float x[SRSLTE_SIMD_F_SIZE];

  srslte_simd_f_storeu(x, a);

  fprintf(stream, "[");
  for (int i = 0; i < SRSLTE_SIMD_F_SIZE; i++) {
    fprintf(stream, "%+2.5f, ", x[i]);
  }
  fprintf(stream, "];\n");
}

#endif /* SRSLTE_SIMD_F_SIZE */

#if SRSLTE_SIMD_CF_SIZE

#ifdef HAVE_NEON
typedef float32x4x2_t simd_cf_t;
#else
typedef struct {
  simd_f_t re;
  simd_f_t im;
} simd_cf_t;
#endif

/* Complex Single precission Floating point functions */
static inline simd_cf_t srslte_simd_cfi_load(const cf_t* ptr)
{
  simd_cf_t ret;
#ifdef LV_HAVE_AVX512
  __m512 in1 = _mm512_load_ps((float*)(ptr));
  __m512 in2 = _mm512_load_ps((float*)(ptr + SRSLTE_SIMD_CF_SIZE / 2));
  ret.re     = _mm512_permutex2var_ps(
      in1,
      _mm512_setr_epi32(0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E),
      in2);
  ret.im = _mm512_permutex2var_ps(
      in1,
      _mm512_setr_epi32(0x01, 0x03, 0x05, 0x07, 0x09, 0x0B, 0x0D, 0x0F, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F),
      in2);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  __m256 in1  = _mm256_permute_ps(_mm256_load_ps((float*)(ptr)), 0b11011000);
  __m256 in2  = _mm256_permute_ps(_mm256_load_ps((float*)(ptr + 4)), 0b11011000);
  ret.re      = _mm256_unpacklo_ps(in1, in2);
  ret.im      = _mm256_unpackhi_ps(in1, in2);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  __m128 i1 = _mm_load_ps((float*)(ptr));
  __m128 i2 = _mm_load_ps((float*)(ptr + 2));
  ret.re    = _mm_shuffle_ps(i1, i2, _MM_SHUFFLE(2, 0, 2, 0));
  ret.im    = _mm_shuffle_ps(i1, i2, _MM_SHUFFLE(3, 1, 3, 1));
#else
#ifdef HAVE_NEON
  ret        = vld2q_f32((float*)(ptr));
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
  return ret;
}

/* Complex Single precission Floating point functions */
static inline simd_cf_t srslte_simd_cfi_loadu(const cf_t* ptr)
{
  simd_cf_t ret;
#ifdef LV_HAVE_AVX512
  __m512 in1 = _mm512_loadu_ps((float*)(ptr));
  __m512 in2 = _mm512_loadu_ps((float*)(ptr + SRSLTE_SIMD_CF_SIZE / 2));
  ret.re     = _mm512_permutex2var_ps(
      in1,
      _mm512_setr_epi32(0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E),
      in2);
  ret.im = _mm512_permutex2var_ps(
      in1,
      _mm512_setr_epi32(0x01, 0x03, 0x05, 0x07, 0x09, 0x0B, 0x0D, 0x0F, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F),
      in2);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  __m256 in1  = _mm256_permute_ps(_mm256_loadu_ps((float*)(ptr)), 0b11011000);
  __m256 in2  = _mm256_permute_ps(_mm256_loadu_ps((float*)(ptr + 4)), 0b11011000);
  ret.re      = _mm256_unpacklo_ps(in1, in2);
  ret.im      = _mm256_unpackhi_ps(in1, in2);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  __m128 i1 = _mm_loadu_ps((float*)(ptr));
  __m128 i2 = _mm_loadu_ps((float*)(ptr + 2));
  ret.re    = _mm_shuffle_ps(i1, i2, _MM_SHUFFLE(2, 0, 2, 0));
  ret.im    = _mm_shuffle_ps(i1, i2, _MM_SHUFFLE(3, 1, 3, 1));
#else
#ifdef HAVE_NEON
  ret        = vld2q_f32((float*)(ptr));
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
  return ret;
}

static inline simd_cf_t srslte_simd_cf_load(const float* re, const float* im)
{
  simd_cf_t ret;
#ifdef LV_HAVE_AVX512
  ret.re = _mm512_load_ps(re);
  ret.im = _mm512_load_ps(im);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  ret.re      = _mm256_load_ps(re);
  ret.im      = _mm256_load_ps(im);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  ret.re    = _mm_load_ps(re);
  ret.im    = _mm_load_ps(im);
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  ret.val[0] = vld1q_f32(re);
  ret.val[1] = vld1q_f32(im);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
  return ret;
}

static inline simd_cf_t srslte_simd_cf_loadu(const float* re, const float* im)
{
  simd_cf_t ret;
#ifdef LV_HAVE_AVX512
  ret.re = _mm512_loadu_ps(re);
  ret.im = _mm512_loadu_ps(im);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  ret.re      = _mm256_loadu_ps(re);
  ret.im      = _mm256_loadu_ps(im);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  ret.re    = _mm_loadu_ps(re);
  ret.im    = _mm_loadu_ps(im);
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  ret.val[0] = vld1q_f32(re);
  ret.val[1] = vld1q_f32(im);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
  return ret;
}

static inline void srslte_simd_cfi_store(cf_t* ptr, simd_cf_t simdreg)
{
#ifdef LV_HAVE_AVX512
  __m512 s1 = _mm512_permutex2var_ps(
      simdreg.re,
      _mm512_setr_epi32(0x00, 0x10, 0x01, 0x11, 0x02, 0x12, 0x03, 0x13, 0x04, 0x14, 0x05, 0x15, 0x06, 0x16, 0x07, 0x17),
      simdreg.im);
  __m512 s2 = _mm512_permutex2var_ps(
      simdreg.re,
      _mm512_setr_epi32(0x08, 0x18, 0x09, 0x19, 0x0A, 0x1A, 0x0B, 0x1B, 0x0C, 0x1C, 0x0D, 0x1D, 0x0E, 0x1E, 0x0F, 0x1F),
      simdreg.im);
  _mm512_store_ps((float*)(ptr), s1);
  _mm512_store_ps((float*)(ptr + 8), s2);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  __m256 out1 = _mm256_permute_ps(simdreg.re, 0b11011000);
  __m256 out2 = _mm256_permute_ps(simdreg.im, 0b11011000);
  _mm256_store_ps((float*)(ptr), _mm256_unpacklo_ps(out1, out2));
  _mm256_store_ps((float*)(ptr + 4), _mm256_unpackhi_ps(out1, out2));
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  _mm_store_ps((float*)(ptr), _mm_unpacklo_ps(simdreg.re, simdreg.im));
  _mm_store_ps((float*)(ptr + 2), _mm_unpackhi_ps(simdreg.re, simdreg.im));
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  vst2q_f32((float*)(ptr), simdreg);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline void srslte_simd_cfi_storeu(cf_t* ptr, simd_cf_t simdreg)
{
#ifdef LV_HAVE_AVX512
  __m512 s1 = _mm512_permutex2var_ps(
      simdreg.re,
      _mm512_setr_epi32(0x00, 0x10, 0x01, 0x11, 0x02, 0x12, 0x03, 0x13, 0x04, 0x14, 0x05, 0x15, 0x06, 0x16, 0x07, 0x17),
      simdreg.im);
  __m512 s2 = _mm512_permutex2var_ps(
      simdreg.re,
      _mm512_setr_epi32(0x08, 0x18, 0x09, 0x19, 0x0A, 0x1A, 0x0B, 0x1B, 0x0C, 0x1C, 0x0D, 0x1D, 0x0E, 0x1E, 0x0F, 0x1F),
      simdreg.im);
  _mm512_storeu_ps((float*)(ptr), s1);
  _mm512_storeu_ps((float*)(ptr + 8), s2);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  __m256 out1 = _mm256_permute_ps(simdreg.re, 0b11011000);
  __m256 out2 = _mm256_permute_ps(simdreg.im, 0b11011000);
  _mm256_storeu_ps((float*)(ptr), _mm256_unpacklo_ps(out1, out2));
  _mm256_storeu_ps((float*)(ptr + 4), _mm256_unpackhi_ps(out1, out2));
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  _mm_storeu_ps((float*)(ptr), _mm_unpacklo_ps(simdreg.re, simdreg.im));
  _mm_storeu_ps((float*)(ptr + 2), _mm_unpackhi_ps(simdreg.re, simdreg.im));
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  vst2q_f32((float*)(ptr), simdreg);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline void srslte_simd_cf_store(float* re, float* im, simd_cf_t simdreg)
{
#ifdef LV_HAVE_AVX512
  _mm512_store_ps(re, simdreg.re);
  _mm512_store_ps(im, simdreg.im);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  _mm256_store_ps(re, simdreg.re);
  _mm256_store_ps(im, simdreg.im);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_SSE
  _mm_store_ps((float*)re, simdreg.re);
  _mm_store_ps((float*)im, simdreg.im);
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  vst1q_f32((float*)re, simdreg.val[0]);
  vst1q_f32((float*)im, simdreg.val[1]);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline void srslte_simd_cf_storeu(float* re, float* im, simd_cf_t simdreg)
{
#ifdef LV_HAVE_AVX512
  _mm512_storeu_ps(re, simdreg.re);
  _mm512_storeu_ps(im, simdreg.im);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  _mm256_storeu_ps(re, simdreg.re);
  _mm256_storeu_ps(im, simdreg.im);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_SSE
  _mm_storeu_ps((float*)re, simdreg.re);
  _mm_storeu_ps((float*)im, simdreg.im);
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  vst1q_f32((float*)re, simdreg.val[0]);
  vst1q_f32((float*)im, simdreg.val[1]);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_f_t srslte_simd_cf_re(simd_cf_t in)
{
#ifdef HAVE_NEON
  simd_f_t out = in.val[0];
#else
  simd_f_t out      = in.re;
#endif /*HAVE_NEON*/
#ifndef LV_HAVE_AVX512
#ifdef LV_HAVE_AVX2
  /* Permute for AVX registers (mis SSE registers) */
  const __m256i idx = _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7);
  out               = _mm256_permutevar8x32_ps(out, idx);
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
  return out;
}

static inline simd_f_t srslte_simd_cf_im(simd_cf_t in)
{
#ifdef HAVE_NEON
  simd_f_t out = in.val[1];
#else
  simd_f_t out      = in.im;
#endif /*HAVE_NEON*/
#ifndef LV_HAVE_AVX512
#ifdef LV_HAVE_AVX2
  /* Permute for AVX registers (mis SSE registers) */
  const __m256i idx = _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7);
  out               = _mm256_permutevar8x32_ps(out, idx);
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
  return out;
}

static inline simd_cf_t srslte_simd_cf_set1(cf_t x)
{
  simd_cf_t ret;
#ifdef LV_HAVE_AVX512
  ret.re = _mm512_set1_ps(__real__ x);
  ret.im = _mm512_set1_ps(__imag__ x);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  ret.re            = _mm256_set1_ps(__real__ x);
  ret.im            = _mm256_set1_ps(__imag__ x);
#else
#ifdef LV_HAVE_SSE
  ret.re            = _mm_set1_ps(__real__ x);
  ret.im            = _mm_set1_ps(__imag__ x);
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  ret.val[0]        = vdupq_n_f32(__real__ x);
  ret.val[1]        = vdupq_n_f32(__imag__ x);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
  return ret;
}

static inline simd_cf_t srslte_simd_cf_prod(simd_cf_t a, simd_cf_t b)
{
  simd_cf_t ret;
#ifdef LV_HAVE_AVX512
  ret.re = _mm512_sub_ps(_mm512_mul_ps(a.re, b.re), _mm512_mul_ps(a.im, b.im));
  ret.im = _mm512_add_ps(_mm512_mul_ps(a.re, b.im), _mm512_mul_ps(a.im, b.re));
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
#ifdef LV_HAVE_FMA
  ret.re            = _mm256_fmsub_ps(a.re, b.re, _mm256_mul_ps(a.im, b.im));
  ret.im            = _mm256_fmadd_ps(a.re, b.im, _mm256_mul_ps(a.im, b.re));
#else  /* LV_HAVE_FMA */
  ret.re = _mm256_sub_ps(_mm256_mul_ps(a.re, b.re), _mm256_mul_ps(a.im, b.im));
  ret.im = _mm256_add_ps(_mm256_mul_ps(a.re, b.im), _mm256_mul_ps(a.im, b.re));
#endif /* LV_HAVE_FMA */
#else
#ifdef LV_HAVE_SSE
  ret.re            = _mm_sub_ps(_mm_mul_ps(a.re, b.re), _mm_mul_ps(a.im, b.im));
  ret.im            = _mm_add_ps(_mm_mul_ps(a.re, b.im), _mm_mul_ps(a.im, b.re));
#else
#ifdef HAVE_NEON
  ret.val[0]        = vsubq_f32(vmulq_f32(a.val[0], b.val[0]), vmulq_f32(a.val[1], b.val[1]));
  ret.val[1]        = vaddq_f32(vmulq_f32(a.val[0], b.val[1]), vmulq_f32(a.val[1], b.val[0]));
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
  return ret;
}

static inline simd_cf_t srslte_simd_cf_conjprod(simd_cf_t a, simd_cf_t b)
{
  simd_cf_t ret;
#ifdef LV_HAVE_AVX512
  ret.re = _mm512_add_ps(_mm512_mul_ps(a.re, b.re), _mm512_mul_ps(a.im, b.im));
  ret.im = _mm512_sub_ps(_mm512_mul_ps(a.im, b.re), _mm512_mul_ps(a.re, b.im));
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  ret.re            = _mm256_add_ps(_mm256_mul_ps(a.re, b.re), _mm256_mul_ps(a.im, b.im));
  ret.im            = _mm256_sub_ps(_mm256_mul_ps(a.im, b.re), _mm256_mul_ps(a.re, b.im));
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  ret.re            = _mm_add_ps(_mm_mul_ps(a.re, b.re), _mm_mul_ps(a.im, b.im));
  ret.im            = _mm_sub_ps(_mm_mul_ps(a.im, b.re), _mm_mul_ps(a.re, b.im));
#else
#ifdef HAVE_NEON
  ret.val[0]        = vaddq_f32(vmulq_f32(a.val[0], b.val[0]), vmulq_f32(a.val[1], b.val[1]));
  ret.val[1]        = vsubq_f32(vmulq_f32(a.val[1], b.val[0]), vmulq_f32(a.val[0], b.val[1]));
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
  return ret;
}

static inline simd_cf_t srslte_simd_cf_add(simd_cf_t a, simd_cf_t b)
{
  simd_cf_t ret;
#ifdef LV_HAVE_AVX512
  ret.re = _mm512_add_ps(a.re, b.re);
  ret.im = _mm512_add_ps(a.im, b.im);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  ret.re            = _mm256_add_ps(a.re, b.re);
  ret.im            = _mm256_add_ps(a.im, b.im);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  ret.re            = _mm_add_ps(a.re, b.re);
  ret.im            = _mm_add_ps(a.im, b.im);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  ret.val[0]        = vaddq_f32(a.val[0], b.val[0]);
  ret.val[1]        = vaddq_f32(a.val[1], b.val[1]);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
  return ret;
}

static inline simd_cf_t srslte_simd_cf_sub(simd_cf_t a, simd_cf_t b)
{
  simd_cf_t ret;
#ifdef LV_HAVE_AVX512
  ret.re = _mm512_sub_ps(a.re, b.re);
  ret.im = _mm512_sub_ps(a.im, b.im);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  ret.re            = _mm256_sub_ps(a.re, b.re);
  ret.im            = _mm256_sub_ps(a.im, b.im);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  ret.re            = _mm_sub_ps(a.re, b.re);
  ret.im            = _mm_sub_ps(a.im, b.im);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  ret.val[0]        = vsubq_f32(a.val[0], b.val[0]);
  ret.val[1]        = vsubq_f32(a.val[1], b.val[1]);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
  return ret;
}

static inline simd_cf_t srslte_simd_cf_mul(simd_cf_t a, simd_f_t b)
{
  simd_cf_t ret;
#ifdef LV_HAVE_AVX512
  ret.re = _mm512_mul_ps(a.re, b);
  ret.im = _mm512_mul_ps(a.im, b);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  b                 = _mm256_permutevar8x32_ps(b, _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7));
  ret.re            = _mm256_mul_ps(a.re, b);
  ret.im            = _mm256_mul_ps(a.im, b);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  ret.re            = _mm_mul_ps(a.re, b);
  ret.im            = _mm_mul_ps(a.im, b);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  ret.val[0]        = vmulq_f32(a.val[0], b);
  ret.val[1]        = vmulq_f32(a.val[1], b);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
  return ret;
}

static inline simd_cf_t srslte_simd_cf_rcp(simd_cf_t a)
{
  simd_cf_t ret;
#ifdef LV_HAVE_AVX512
  simd_f_t a2re     = _mm512_mul_ps(a.re, a.re);
  simd_f_t a2im     = _mm512_mul_ps(a.im, a.im);
  simd_f_t mod2     = _mm512_add_ps(a2re, a2im);
  simd_f_t rcp      = _mm512_rcp14_ps(mod2);
  simd_f_t neg_a_im = _mm512_xor_ps(_mm512_set1_ps(-0.0f), a.im);
  ret.re            = _mm512_mul_ps(a.re, rcp);
  ret.im            = _mm512_mul_ps(neg_a_im, rcp);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  simd_f_t a2re     = _mm256_mul_ps(a.re, a.re);
  simd_f_t a2im     = _mm256_mul_ps(a.im, a.im);
  simd_f_t mod2     = _mm256_add_ps(a2re, a2im);
  simd_f_t rcp      = _mm256_rcp_ps(mod2);
  simd_f_t neg_a_im = _mm256_xor_ps(_mm256_set1_ps(-0.0f), a.im);
  ret.re            = _mm256_mul_ps(a.re, rcp);
  ret.im            = _mm256_mul_ps(neg_a_im, rcp);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  simd_f_t a2re     = _mm_mul_ps(a.re, a.re);
  simd_f_t a2im     = _mm_mul_ps(a.im, a.im);
  simd_f_t mod2     = _mm_add_ps(a2re, a2im);
  simd_f_t rcp      = _mm_rcp_ps(mod2);
  simd_f_t neg_a_im = _mm_xor_ps(_mm_set1_ps(-0.0f), a.im);
  ret.re            = _mm_mul_ps(a.re, rcp);
  ret.im            = _mm_mul_ps(neg_a_im, rcp);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  simd_f_t a2re     = vmulq_f32(a.val[0], a.val[0]);
  simd_f_t a2im     = vmulq_f32(a.val[1], a.val[1]);
  simd_f_t mod2     = vaddq_f32(a2re, a2im);
  simd_f_t rcp      = vmulq_f32(vrecpeq_f32(mod2), vrecpsq_f32(vrecpeq_f32(mod2), mod2));
  simd_f_t neg_a_im = vnegq_f32(a.val[1]);
  ret.val[0]        = vmulq_f32(a.val[0], rcp);
  ret.val[1]        = vmulq_f32(neg_a_im, rcp);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
  return ret;
}

static inline simd_cf_t srslte_simd_cf_neg(simd_cf_t a)
{
  simd_cf_t ret;
#if HAVE_NEON
  ret.val[0] = srslte_simd_f_neg(a.val[0]);
  ret.val[1] = srslte_simd_f_neg(a.val[1]);
#else  /* HAVE_NEON */
  ret.re            = srslte_simd_f_neg(a.re);
  ret.im            = srslte_simd_f_neg(a.im);
#endif /* HAVE_NEON */
  return ret;
}

static inline simd_cf_t srslte_simd_cf_neg_mask(simd_cf_t a, simd_f_t mask)
{
  simd_cf_t ret;
#ifndef LV_HAVE_AVX512
#ifdef LV_HAVE_AVX2
  mask = _mm256_permutevar8x32_ps(mask, _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7));
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
#if HAVE_NEON
  ret.val[0] = srslte_simd_f_neg_mask(a.val[0], mask);
  ret.val[1] = srslte_simd_f_neg_mask(a.val[1], mask);
#else  /* HAVE_NEON */
  ret.re            = srslte_simd_f_neg_mask(a.re, mask);
  ret.im            = srslte_simd_f_neg_mask(a.im, mask);
#endif /* HAVE_NEON */
  return ret;
}

static inline simd_cf_t srslte_simd_cf_conj(simd_cf_t a)
{
  simd_cf_t ret;
#if HAVE_NEON
  ret.val[0] = a.val[0];
  ret.val[1] = srslte_simd_f_neg(a.val[1]);
#else  /* HAVE_NEON */
  ret.re            = a.re;
  ret.im            = srslte_simd_f_neg(a.im);
#endif /* HAVE_NEON */
  return ret;
}

static inline simd_cf_t srslte_simd_cf_mulj(simd_cf_t a)
{
  simd_cf_t ret;
#if HAVE_NEON
  ret.val[0] = srslte_simd_f_neg(a.val[1]);
  ret.val[1] = a.val[0];
#else  /* HAVE_NEON */
  ret.re            = srslte_simd_f_neg(a.im);
  ret.im            = a.re;
#endif /* HAVE_NEON */
  return ret;
}

static inline simd_cf_t srslte_simd_cf_zero(void)
{
  simd_cf_t ret;
#ifdef LV_HAVE_AVX512
  ret.re = _mm512_setzero_ps();
  ret.im = _mm512_setzero_ps();
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  ret.re            = _mm256_setzero_ps();
  ret.im            = _mm256_setzero_ps();
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  ret.re            = _mm_setzero_ps();
  ret.im            = _mm_setzero_ps();
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  ret.val[0]        = vdupq_n_f32(0);
  ret.val[1]        = vdupq_n_f32(0);
#endif /* HAVE_NEON */
#endif /* HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
  return ret;
}

static inline void srslte_simd_cf_fprintf(FILE* stream, simd_cf_t a)
{
  cf_t x[SRSLTE_SIMD_CF_SIZE];

  srslte_simd_cfi_storeu(x, a);

  fprintf(stream, "[");
  for (int i = 0; i < SRSLTE_SIMD_CF_SIZE; i++) {
    fprintf(stream, "%+2.5f%+2.5fi, ", __real__ x[i], __imag__ x[i]);
  }
  fprintf(stream, "];\n");
}

#endif /* SRSLTE_SIMD_CF_SIZE */

#if SRSLTE_SIMD_I_SIZE

#ifdef LV_HAVE_AVX512
typedef __m512i   simd_i_t;
typedef __mmask16 simd_sel_t;
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
typedef __m256i simd_i_t;
typedef __m256  simd_sel_t;
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
typedef __m128i simd_i_t;
typedef __m128  simd_sel_t;
#else /* LV_HAVE_AVX2 */
#ifdef HAVE_NEON
typedef int32x4_t simd_i_t;
typedef int32x4_t simd_sel_t;
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */

static inline simd_i_t srslte_simd_i_load(int* x)
{
#ifdef LV_HAVE_AVX512
  return _mm512_load_epi32((__m512i*)x);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_load_si256((__m256i*)x);
#else
#ifdef LV_HAVE_SSE
  return _mm_load_si128((__m128i*)x);
#else
#ifdef HAVE_NEON
  return vld1q_s32((int*)x);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline void srslte_simd_i_store(int* x, simd_i_t reg)
{
#ifdef LV_HAVE_AVX512
  _mm512_store_epi32((__m512i*)x, reg);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  _mm256_store_si256((__m256i*)x, reg);
#else
#ifdef LV_HAVE_SSE
  _mm_store_si128((__m128i*)x, reg);
#else
#ifdef HAVE_NEON
  vst1q_s32((int*)x, reg);
#endif /*HAVE_NEON*/
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_i_t srslte_simd_i_set1(int x)
{
#ifdef LV_HAVE_AVX512
  return _mm512_set1_epi32(x);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_set1_epi32(x);
#else
#ifdef LV_HAVE_SSE
  return _mm_set1_epi32(x);
#else
#ifdef HAVE_NEON
  return vdupq_n_s32(x);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_i_t srslte_simd_i_add(simd_i_t a, simd_i_t b)
{
#ifdef LV_HAVE_AVX512
  return _mm512_add_epi32(a, b);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_add_epi32(a, b);
#else
#ifdef LV_HAVE_SSE
  return _mm_add_epi32(a, b);
#else
#ifdef HAVE_NEON
  return vaddq_s32(a, b);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_i_t srslte_simd_i_mul(simd_i_t a, simd_i_t b)
{
#ifdef LV_HAVE_AVX512
  return _mm512_mullo_epi32(a, b);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_mullo_epi32(a, b);
#else
#ifdef LV_HAVE_SSE
  return _mm_mul_epi32(a, b);
#else
#ifdef HAVE_NEON
  return vmulq_s32(a, b);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_i_t srslte_simd_i_and(simd_i_t a, simd_i_t b)
{
#ifdef LV_HAVE_AVX512
  return _mm512_and_si512(a, b);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_and_si256(a, b);
#else
#ifdef LV_HAVE_SSE
  return _mm_and_si128(a, b);
#else
#ifdef HAVE_NEON
  return vandq_s32(a, b);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_sel_t srslte_simd_f_max(simd_f_t a, simd_f_t b)
{
#ifdef LV_HAVE_AVX512
  return _mm512_cmp_ps_mask(a, b, _CMP_GT_OS);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_cmp_ps(a, b, _CMP_GT_OS);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return (simd_sel_t)_mm_cmpgt_ps(a, b);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return (simd_sel_t)vcgtq_f32(a, b);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_sel_t srslte_simd_f_min(simd_f_t a, simd_f_t b)
{
#ifdef LV_HAVE_AVX512
  return _mm512_cmp_ps_mask(a, b, _CMP_LT_OS);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_cmp_ps(a, b, _CMP_LT_OS);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return (simd_sel_t)_mm_cmplt_ps(a, b);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return (simd_sel_t)vcltq_f32(a, b);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_f_t srslte_simd_f_select(simd_f_t a, simd_f_t b, simd_sel_t selector)
{
#ifdef LV_HAVE_AVX512
  return _mm512_mask_blend_ps(selector, (__m512)a, (__m512)b);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_blendv_ps(a, b, selector);
#else
#ifdef LV_HAVE_SSE
  return _mm_blendv_ps(a, b, selector);
#else            /* LV_HAVE_SSE */
#ifdef HAVE_NEON // CURRENTLY USES GENERIC IMPLEMENTATION FOR NEON
  float*   a_ptr = (float*)&a;
  float*   b_ptr = (float*)&b;
  simd_f_t ret;
  int*     sel   = (int*)&selector;
  float*   c_ptr = (float*)&ret;
  for (int i = 0; i < 4; i++) {
    if (sel[i] == -1) {
      c_ptr[i] = b_ptr[i];
    } else {
      c_ptr[i] = a_ptr[i];
    }
  }
  return ret;
#endif           /* HAVE_NEON */
#endif           /* LV_HAVE_SSE */
#endif           /* LV_HAVE_AVX2 */
#endif           /* LV_HAVE_AVX512 */
}

static inline simd_i_t srslte_simd_i_select(simd_i_t a, simd_i_t b, simd_sel_t selector)
{
#ifdef LV_HAVE_AVX512
  return (__m512i)_mm512_mask_blend_ps(selector, (__m512)a, (__m512)b);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return (__m256i)_mm256_blendv_ps((__m256)a, (__m256)b, selector);
#else
#ifdef LV_HAVE_SSE
  return (__m128i)_mm_blendv_ps((__m128)a, (__m128)b, selector);
#else            /* LV_HAVE_SSE */
#ifdef HAVE_NEON // CURRENTLY USES GENERIC IMPLEMENTATION FOR NEON
  int*     a_ptr = (int*)&a;
  int*     b_ptr = (int*)&b;
  simd_i_t ret;
  int*     sel   = (int*)&selector;
  int*     c_ptr = (int*)&ret;
  for (int i = 0; i < 4; i++) {
    if (sel[i] == -1) {
      c_ptr[i] = b_ptr[i];
    } else {
      c_ptr[i] = a_ptr[i];
    }
  }
  return ret;
#endif           /* HAVE_NEON */
#endif           /* LV_HAVE_SSE */
#endif           /* LV_HAVE_AVX2 */
#endif           /* LV_HAVE_AVX512 */
}

#endif /* SRSLTE_SIMD_I_SIZE*/

#if SRSLTE_SIMD_S_SIZE

#ifdef LV_HAVE_AVX512
typedef __m512i simd_s_t;
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
typedef __m256i simd_s_t;
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
typedef __m128i simd_s_t;
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
typedef int16x8_t simd_s_t;
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */

static inline simd_s_t srslte_simd_s_load(const int16_t* ptr)
{
#ifdef LV_HAVE_AVX512
  return _mm512_load_si512(ptr);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_load_si256((__m256i*)ptr);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_load_si128((__m128i*)ptr);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vld1q_s16(ptr);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_s_t srslte_simd_s_loadu(const int16_t* ptr)
{
#ifdef LV_HAVE_AVX512
  return _mm512_loadu_si512(ptr);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_loadu_si256((__m256i*)ptr);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_loadu_si128((__m128i*)ptr);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vld1q_s16(ptr);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline void srslte_simd_s_store(int16_t* ptr, simd_s_t simdreg)
{
#ifdef LV_HAVE_AVX512
  _mm512_store_si512(ptr, simdreg);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  _mm256_store_si256((__m256i*)ptr, simdreg);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  _mm_store_si128((__m128i*)ptr, simdreg);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  vst1q_s16(ptr, simdreg);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline void srslte_simd_s_storeu(int16_t* ptr, simd_s_t simdreg)
{
#ifdef LV_HAVE_AVX512
  _mm512_storeu_si512(ptr, simdreg);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  _mm256_storeu_si256((__m256i*)ptr, simdreg);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  _mm_storeu_si128((__m128i*)ptr, simdreg);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  vst1q_s16(ptr, simdreg);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}
static inline simd_s_t srslte_simd_s_zero(void)
{
#ifdef LV_HAVE_AVX512
  return _mm512_setzero_si512();
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_setzero_si256();
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_setzero_si128();
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vdupq_n_s16(0);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_s_t srslte_simd_s_mul(simd_s_t a, simd_s_t b)
{
#ifdef LV_HAVE_AVX512
  return _mm512_mullo_epi16(a, b);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_mullo_epi16(a, b);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_mullo_epi16(a, b);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vmulq_s16(a, b);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_s_t srslte_simd_s_neg(simd_s_t a, simd_s_t b)
{
#ifdef LV_HAVE_AVX512
  __m256i a0 = _mm512_extracti64x4_epi64(a, 0);
  __m256i a1 = _mm512_extracti64x4_epi64(a, 1);
  __m256i b0 = _mm512_extracti64x4_epi64(b, 0);
  __m256i b1 = _mm512_extracti64x4_epi64(b, 1);
  __m256i r0 = _mm256_sign_epi16(a0, b0);
  __m256i r1 = _mm256_sign_epi16(a1, b1);
  return _mm512_inserti64x4(_mm512_broadcast_i64x4(r0), r1, 1);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_sign_epi16(a, b);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_sign_epi16(a, b);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  /* Taken and modified from sse2neon.h licensed under MIT
   * Source: https://github.com/DLTcollab/sse2neon
   */
  int16x8_t zero = vdupq_n_s16(0);
  // signed shift right: faster than vclt
  // (b < 0) ? 0xFFFF : 0
  uint16x8_t ltMask = vreinterpretq_u16_s16(vshrq_n_s16(b, 15));
  // (b == 0) ? 0xFFFF : 0
  int16x8_t zeroMask = vreinterpretq_s16_u16(vceqq_s16(b, zero));
  // -a
  int16x8_t neg = vnegq_s16(a);
  // bitwise select either a or neg based on ltMask
  int16x8_t masked = vbslq_s16(ltMask, neg, a);
  // res = masked & (~zeroMask)
  int16x8_t res = vbicq_s16(masked, zeroMask);
  return res;
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_s_t srslte_simd_s_add(simd_s_t a, simd_s_t b)
{
#ifdef LV_HAVE_AVX512
  return _mm512_add_epi16(a, b);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_add_epi16(a, b);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_add_epi16(a, b);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vaddq_s16(a, b);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_s_t srslte_simd_s_sub(simd_s_t a, simd_s_t b)
{
#ifdef LV_HAVE_AVX512
  return _mm512_sub_epi16(a, b);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_sub_epi16(a, b);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_sub_epi16(a, b);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vsubq_s16(a, b);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

#endif /* SRSLTE_SIMD_S_SIZE */

#if SRSLTE_SIMD_C16_SIZE

typedef
#ifdef LV_HAVE_AVX512
    struct {
  union {
    __m512i m512;
    int16_t i16[32];
  } re;
  union {
    __m512i m512;
    int16_t i16[32];
  } im;
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
    struct {
  union {
    __m256i m256;
    int16_t i16[16];
  } re;
  union {
    __m256i m256;
    int16_t i16[16];
  } im;
#else
#ifdef LV_HAVE_SSE
    struct {
  union {
    __m128i m128;
    int16_t i16[8];
  } re;
  union {
    __m128i m128;
    int16_t i16[8];
  } im;
#else
#ifdef HAVE_NEON
    union {
  int16x8x2_t m128;
  int16_t     i16[16];
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
} simd_c16_t;

/* Fixed point precision (16-bit) functions */
static inline simd_c16_t srslte_simd_c16i_load(const c16_t* ptr)
{
  simd_c16_t ret;
#ifdef LV_HAVE_AVX512
  __m512i in1 = _mm512_load_si512((__m512i*)(ptr));
  __m512i in2 = _mm512_load_si512((__m512i*)(ptr + 8));
  ret.re.m512 = _mm512_mask_blend_epi16(
      0xAAAAAAAA, in1, _mm512_shufflelo_epi16(_mm512_shufflehi_epi16(in2, 0b10100000), 0b10100000));
  ret.im.m512 = _mm512_mask_blend_epi16(
      0xAAAAAAAA, _mm512_shufflelo_epi16(_mm512_shufflehi_epi16(in1, 0b11110101), 0b11110101), in2);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_AVX2
  __m256i in1 = _mm256_load_si256((__m256i*)(ptr));
  __m256i in2 = _mm256_load_si256((__m256i*)(ptr + 8));
  ret.re.m256 =
      _mm256_blend_epi16(in1, _mm256_shufflelo_epi16(_mm256_shufflehi_epi16(in2, 0b10100000), 0b10100000), 0b10101010);
  ret.im.m256 =
      _mm256_blend_epi16(_mm256_shufflelo_epi16(_mm256_shufflehi_epi16(in1, 0b11110101), 0b11110101), in2, 0b10101010);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  __m128i in1 = _mm_load_si128((__m128i*)(ptr));
  __m128i in2 = _mm_load_si128((__m128i*)(ptr + 8));
  ret.re.m128 = _mm_blend_epi16(in1, _mm_shufflelo_epi16(_mm_shufflehi_epi16(in2, 0b10100000), 0b10100000), 0b10101010);
  ret.im.m128 = _mm_blend_epi16(_mm_shufflelo_epi16(_mm_shufflehi_epi16(in1, 0b11110101), 0b11110101), in2, 0b10101010);
#else /* LV_HAVE_SSE*/
#ifdef HAVE_NEON
  ret.m128     = vld2q_s16((int16_t*)(ptr));
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
  return ret;
}

static inline simd_c16_t srslte_simd_c16_load(const int16_t* re, const int16_t* im)
{
  simd_c16_t ret;
#ifdef LV_HAVE_AVX2
  ret.re.m256 = _mm256_load_si256((__m256i*)(re));
  ret.im.m256 = _mm256_load_si256((__m256i*)(im));
#else
#ifdef LV_HAVE_SSE
  ret.re.m128   = _mm_load_si128((__m128i*)(re));
  ret.im.m128   = _mm_load_si128((__m128i*)(im));
#else /* LV_HAVE_SSE*/
#ifdef HAVE_NEON
  ret.m128.val[0] = vld1q_s16((int16_t*)(re));
  ret.m128.val[1] = vld1q_s16((int16_t*)(im));
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
  return ret;
}

static inline simd_c16_t srslte_simd_c16_loadu(const int16_t* re, const int16_t* im)
{
  simd_c16_t ret;
#ifdef LV_HAVE_AVX2
  ret.re.m256 = _mm256_loadu_si256((__m256i*)(re));
  ret.im.m256 = _mm256_loadu_si256((__m256i*)(im));
#else
#ifdef LV_HAVE_SSE
  ret.re.m128   = _mm_loadu_si128((__m128i*)(re));
  ret.im.m128   = _mm_loadu_si128((__m128i*)(im));
#else /* LV_HAVE_SSE*/
#ifdef HAVE_NEON
  ret.m128.val[0] = vld1q_s16((int16_t*)(re));
  ret.m128.val[1] = vld1q_s16((int16_t*)(im));
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
  return ret;
}

static inline void srslte_simd_c16i_store(c16_t* ptr, simd_c16_t simdreg)
{
#ifdef LV_HAVE_AVX2
  __m256i re_sw = _mm256_shufflelo_epi16(_mm256_shufflehi_epi16(simdreg.re.m256, 0b10110001), 0b10110001);
  __m256i im_sw = _mm256_shufflelo_epi16(_mm256_shufflehi_epi16(simdreg.im.m256, 0b10110001), 0b10110001);
  _mm256_store_si256((__m256i*)(ptr), _mm256_blend_epi16(simdreg.re.m256, im_sw, 0b10101010));
  _mm256_store_si256((__m256i*)(ptr + 8), _mm256_blend_epi16(re_sw, simdreg.im.m256, 0b10101010));
#else
#ifdef LV_HAVE_SSE
  __m128i re_sw = _mm_shufflelo_epi16(_mm_shufflehi_epi16(simdreg.re.m128, 0b10110001), 0b10110001);
  __m128i im_sw = _mm_shufflelo_epi16(_mm_shufflehi_epi16(simdreg.im.m128, 0b10110001), 0b10110001);
  _mm_store_si128((__m128i*)(ptr), _mm_blend_epi16(simdreg.re.m128, im_sw, 0b10101010));
  _mm_store_si128((__m128i*)(ptr + 8), _mm_blend_epi16(re_sw, simdreg.im.m128, 0b10101010));
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  vst2q_s16((int16_t*)(ptr), simdreg.m128);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
}

static inline void srslte_simd_c16i_storeu(c16_t* ptr, simd_c16_t simdreg)
{
#ifdef LV_HAVE_AVX2
  __m256i re_sw = _mm256_shufflelo_epi16(_mm256_shufflehi_epi16(simdreg.re.m256, 0b10110001), 0b10110001);
  __m256i im_sw = _mm256_shufflelo_epi16(_mm256_shufflehi_epi16(simdreg.im.m256, 0b10110001), 0b10110001);
  _mm256_storeu_si256((__m256i*)(ptr), _mm256_blend_epi16(simdreg.re.m256, im_sw, 0b10101010));
  _mm256_storeu_si256((__m256i*)(ptr + 8), _mm256_blend_epi16(re_sw, simdreg.im.m256, 0b10101010));
#else
#ifdef LV_HAVE_SSE
  __m128i re_sw = _mm_shufflelo_epi16(_mm_shufflehi_epi16(simdreg.re.m128, 0b10110001), 0b10110001);
  __m128i im_sw = _mm_shufflelo_epi16(_mm_shufflehi_epi16(simdreg.im.m128, 0b10110001), 0b10110001);
  _mm_storeu_si128((__m128i*)(ptr), _mm_blend_epi16(simdreg.re.m128, im_sw, 0b10101010));
  _mm_storeu_si128((__m128i*)(ptr + 8), _mm_blend_epi16(re_sw, simdreg.im.m128, 0b10101010));
#else /*HAVE_NEON*/
#ifdef HAVE_NEON
  vst2q_s16((int16_t*)(ptr), simdreg.m128);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
}

static inline void srslte_simd_c16_store(int16_t* re, int16_t* im, simd_c16_t simdreg)
{
#ifdef LV_HAVE_AVX2
  _mm256_store_si256((__m256i*)re, simdreg.re.m256);
  _mm256_store_si256((__m256i*)im, simdreg.im.m256);
#else
#ifdef LV_HAVE_SSE
  _mm_store_si128((__m128i*)re, simdreg.re.m128);
  _mm_store_si128((__m128i*)im, simdreg.im.m128);
#else
#ifdef HAVE_NEON
  vst1q_s16((int16_t*)re, simdreg.m128.val[0]);
  vst1q_s16((int16_t*)im, simdreg.m128.val[1]);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
}

static inline void srslte_simd_c16_storeu(int16_t* re, int16_t* im, simd_c16_t simdreg)
{
#ifdef LV_HAVE_AVX2
  _mm256_storeu_si256((__m256i*)re, simdreg.re.m256);
  _mm256_storeu_si256((__m256i*)im, simdreg.im.m256);
#else
#ifdef LV_HAVE_SSE
  _mm_storeu_si128((__m128i*)re, simdreg.re.m128);
  _mm_storeu_si128((__m128i*)im, simdreg.im.m128);
#else
#ifdef HAVE_NEON
  vst1q_s16((int16_t*)re, simdreg.m128.val[0]);
  vst1q_s16((int16_t*)im, simdreg.m128.val[1]);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
}

static inline simd_c16_t srslte_simd_c16_prod(simd_c16_t a, simd_c16_t b)
{
  simd_c16_t ret;
#ifdef LV_HAVE_AVX2
  ret.re.m256 = _mm256_sub_epi16(_mm256_mulhrs_epi16(a.re.m256, _mm256_slli_epi16(b.re.m256, 1)),
                                 _mm256_mulhrs_epi16(a.im.m256, _mm256_slli_epi16(b.im.m256, 1)));
  ret.im.m256 = _mm256_add_epi16(_mm256_mulhrs_epi16(a.re.m256, _mm256_slli_epi16(b.im.m256, 1)),
                                 _mm256_mulhrs_epi16(a.im.m256, _mm256_slli_epi16(b.re.m256, 1)));
#else
#ifdef LV_HAVE_SSE
  ret.re.m128 = _mm_sub_epi16(_mm_mulhrs_epi16(a.re.m128, _mm_slli_epi16(b.re.m128, 1)),
                              _mm_mulhrs_epi16(a.im.m128, _mm_slli_epi16(b.im.m128, 1)));
  ret.im.m128 = _mm_add_epi16(_mm_mulhrs_epi16(a.re.m128, _mm_slli_epi16(b.im.m128, 1)),
                              _mm_mulhrs_epi16(a.im.m128, _mm_slli_epi16(b.re.m128, 1)));
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
  return ret;
}

static inline simd_c16_t srslte_simd_c16_add(simd_c16_t a, simd_c16_t b)
{
  simd_c16_t ret;
#ifdef LV_HAVE_AVX2
  ret.re.m256 = _mm256_add_epi16(a.re.m256, b.re.m256);
  ret.im.m256 = _mm256_add_epi16(a.im.m256, b.im.m256);
#else
#ifdef LV_HAVE_SSE
  ret.re.m128 = _mm_add_epi16(a.re.m128, b.re.m128);
  ret.im.m128 = _mm_add_epi16(a.im.m128, b.im.m128);
#else
#ifdef HAVE_NEON
  ret.m128.val[0] = vaddq_s16(a.m128.val[0], a.m128.val[0]);
  ret.m128.val[1] = vaddq_s16(a.m128.val[1], a.m128.val[1]);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
  return ret;
}

static inline simd_c16_t srslte_simd_c16_zero(void)
{
  simd_c16_t ret;
#ifdef LV_HAVE_AVX2
  ret.re.m256 = _mm256_setzero_si256();
  ret.im.m256 = _mm256_setzero_si256();
#else
#ifdef LV_HAVE_SSE
  ret.re.m128 = _mm_setzero_si128();
  ret.im.m128 = _mm_setzero_si128();
#else
#ifdef HAVE_NEON
  ret.m128.val[0] = vdupq_n_s16(0);
  ret.m128.val[1] = vdupq_n_s16(0);
#endif /* HAVE_NEON    */
#endif /* LV_HAVE_SSE  */
#endif /* LV_HAVE_AVX2 */
  return ret;
}

#endif /* SRSLTE_SIMD_C16_SIZE */

#if SRSLTE_SIMD_F_SIZE && SRSLTE_SIMD_S_SIZE

static inline simd_s_t srslte_simd_convert_2f_s(simd_f_t a, simd_f_t b)
{
#ifdef LV_HAVE_AVX512
  __m512 aa = _mm512_permutex2var_ps(
      a,
      _mm512_setr_epi32(0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0A, 0x0B, 0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1A, 0x1B),
      b);
  __m512 bb = _mm512_permutex2var_ps(
      a,
      _mm512_setr_epi32(0x04, 0x05, 0x06, 0x07, 0x0C, 0x0D, 0x0E, 0x0F, 0x14, 0x15, 0x16, 0x17, 0x1C, 0x1D, 0x1E, 0x1F),
      b);
  __m512i ai = _mm512_cvttps_epi32(aa);
  __m512i bi = _mm512_cvttps_epi32(bb);
  return _mm512_packs_epi32(ai, bi);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  __m256  aa  = _mm256_permute2f128_ps(a, b, 0x20);
  __m256  bb  = _mm256_permute2f128_ps(a, b, 0x31);
  __m256i ai  = _mm256_cvttps_epi32(aa);
  __m256i bi  = _mm256_cvttps_epi32(bb);
  return _mm256_packs_epi32(ai, bi);
#else
#ifdef LV_HAVE_SSE
  __m128i ai      = _mm_cvttps_epi32(a);
  __m128i bi      = _mm_cvttps_epi32(b);
  return _mm_packs_epi32(ai, bi);
#else
#ifdef HAVE_NEON
  int32x4_t ai = vcvtq_s32_f32(a);
  int32x4_t bi = vcvtq_s32_f32(b);
  return (simd_s_t)vcombine_s16(vqmovn_s32(ai), vqmovn_s32(bi));
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

#endif /* SRSLTE_SIMD_F_SIZE && SRSLTE_SIMD_C16_SIZE */

#if SRSLTE_SIMD_B_SIZE
/* Data types */
#ifdef LV_HAVE_AVX512
typedef __m512i simd_b_t;
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
typedef __m256i simd_b_t;
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
typedef __m128i simd_b_t;
#else /* HAVE_NEON */
#ifdef HAVE_NEON
typedef int8x16_t simd_b_t;
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */

static inline simd_b_t srslte_simd_b_load(const int8_t* ptr)
{
#ifdef LV_HAVE_AVX512
  return _mm512_load_si512(ptr);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_load_si256((__m256i*)ptr);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_load_si128((__m128i*)ptr);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vld1q_s8(ptr);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_b_t srslte_simd_b_loadu(const int8_t* ptr)
{
#ifdef LV_HAVE_AVX512
  return _mm512_loadu_si512(ptr);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_loadu_si256((__m256i*)ptr);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_loadu_si128((__m128i*)ptr);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vld1q_s8(ptr);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline void srslte_simd_b_store(int8_t* ptr, simd_b_t simdreg)
{
#ifdef LV_HAVE_AVX512
  _mm512_store_si512(ptr, simdreg);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  _mm256_store_si256((__m256i*)ptr, simdreg);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  _mm_store_si128((__m128i*)ptr, simdreg);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  vst1q_s8(ptr, simdreg);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline void srslte_simd_b_storeu(int8_t* ptr, simd_b_t simdreg)
{
#ifdef LV_HAVE_AVX512
  _mm512_storeu_si512(ptr, simdreg);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  _mm256_storeu_si256((__m256i*)ptr, simdreg);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  _mm_storeu_si128((__m128i*)ptr, simdreg);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  vst1q_s8(ptr, simdreg);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_b_t srslte_simd_b_xor(simd_b_t a, simd_b_t b)
{

#ifdef LV_HAVE_AVX512
  return _mm512_xor_epi32(a, b);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_xor_si256(a, b);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_xor_si128(a, b);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return veorq_s8(a, b);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_b_t srslte_simd_b_sub(simd_b_t a, simd_b_t b)
{
#ifdef LV_HAVE_AVX512
  return _mm512_subs_epi8(a, b);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_subs_epi8(a, b);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_subs_epi8(a, b);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  return vqsubq_s8(a, b);
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

static inline simd_b_t srslte_simd_b_neg(simd_b_t a, simd_b_t b)
{
#ifdef LV_HAVE_AVX512
  __m256i a0 = _mm512_extracti64x4_epi64(a, 0);
  __m256i a1 = _mm512_extracti64x4_epi64(a, 1);
  __m256i b0 = _mm512_extracti64x4_epi64(b, 0);
  __m256i b1 = _mm512_extracti64x4_epi64(b, 1);
  __m256i r0 = _mm256_sign_epi8(a0, b0);
  __m256i r1 = _mm256_sign_epi8(a1, b1);
  return _mm512_inserti64x4(_mm512_broadcast_i64x4(r0), r1, 1);
#else /* LV_HAVE_AVX512 */
#ifdef LV_HAVE_AVX2
  return _mm256_sign_epi8(a, b);
#else /* LV_HAVE_AVX2 */
#ifdef LV_HAVE_SSE
  return _mm_sign_epi8(a, b);
#else /* LV_HAVE_SSE */
#ifdef HAVE_NEON
  /* Taken and modified from sse2neon.h licensed under MIT
   * Source: https://github.com/DLTcollab/sse2neon
   */
  int8x16_t zero = vdupq_n_s8(0);
  // signed shift right: faster than vclt
  // (b < 0) ? 0xFF : 0
  uint8x16_t ltMask = vreinterpretq_u8_s8(vshrq_n_s8(b, 7));
  // (b == 0) ? 0xFF : 0
  int8x16_t zeroMask = vreinterpretq_s8_u8(vceqq_s8(b, zero));
  // -a
  int8x16_t neg = vnegq_s8(a);
  // bitwise select either a or neg based on ltMask
  int8x16_t masked = vbslq_s8(ltMask, neg, a);
  // res = masked & (~zeroMask)
  int8x16_t res = vbicq_s8(masked, zeroMask);
  return res;
#endif /* HAVE_NEON */
#endif /* LV_HAVE_SSE */
#endif /* LV_HAVE_AVX2 */
#endif /* LV_HAVE_AVX512 */
}

#endif /*SRSLTE_SIMD_B_SIZE */

#endif // SRSLTE_SIMD_H
