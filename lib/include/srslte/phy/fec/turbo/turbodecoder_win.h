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

#include "srslte/config.h"

#define MAKE_FUNC(a) CONCAT2(CONCAT2(tdec_win, WINIMP), CONCAT2(_, a))
#define MAKE_TYPE CONCAT2(CONCAT2(tdec_win_, WINIMP), _t)

#ifdef WINIMP_IS_SSE16

#ifndef LV_HAVE_SSE
#error "Selected SSE window decoder but instruction set not supported"
#endif

#include <nmmintrin.h>

#define WINIMP sse16
#define nof_blocks 8

#define llr_t int16_t

#define simd_type_t __m128i
#define simd_load _mm_load_si128
#define simd_store _mm_store_si128
#define simd_add _mm_adds_epi16
#define simd_sub _mm_subs_epi16
#define simd_max _mm_max_epi16
#define simd_set1 _mm_set1_epi16
#define simd_insert _mm_insert_epi16
#define simd_shuffle _mm_shuffle_epi8
#define move_right _mm_set_epi8(15, 14, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2)
#define move_left _mm_set_epi8(13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 0)
#define simd_rb_shift _mm_srai_epi16

#define normalize_period 2
#define win_overlap_len 40

#define INF 10000

#else
#ifdef WINIMP_IS_AVX16

#ifndef LV_HAVE_AVX2
#error "Selected AVX2 window decoder but instruction set not supported"
#endif

#include <immintrin.h>

#define WINIMP avx16
#define nof_blocks 16

#define llr_t int16_t

#define simd_type_t __m256i
#define simd_load _mm256_load_si256
#define simd_store _mm256_store_si256
#define simd_add _mm256_adds_epi16
#define simd_sub _mm256_subs_epi16
#define simd_max _mm256_max_epi16
#define simd_set1 _mm256_set1_epi16
#define simd_insert _mm256_insert_epi16
#define simd_shuffle _mm256_shuffle_epi8
#define move_right                                                                                                     \
  _mm256_set_epi8(31,                                                                                                  \
                  30,                                                                                                  \
                  31,                                                                                                  \
                  30,                                                                                                  \
                  29,                                                                                                  \
                  28,                                                                                                  \
                  27,                                                                                                  \
                  26,                                                                                                  \
                  25,                                                                                                  \
                  24,                                                                                                  \
                  23,                                                                                                  \
                  22,                                                                                                  \
                  21,                                                                                                  \
                  20,                                                                                                  \
                  19,                                                                                                  \
                  18,                                                                                                  \
                  17,                                                                                                  \
                  16,                                                                                                  \
                  15,                                                                                                  \
                  14,                                                                                                  \
                  13,                                                                                                  \
                  12,                                                                                                  \
                  11,                                                                                                  \
                  10,                                                                                                  \
                  9,                                                                                                   \
                  8,                                                                                                   \
                  7,                                                                                                   \
                  6,                                                                                                   \
                  5,                                                                                                   \
                  4,                                                                                                   \
                  3,                                                                                                   \
                  2)
#define move_left                                                                                                      \
  _mm256_set_epi8(29,                                                                                                  \
                  28,                                                                                                  \
                  27,                                                                                                  \
                  26,                                                                                                  \
                  25,                                                                                                  \
                  24,                                                                                                  \
                  23,                                                                                                  \
                  22,                                                                                                  \
                  21,                                                                                                  \
                  20,                                                                                                  \
                  19,                                                                                                  \
                  18,                                                                                                  \
                  17,                                                                                                  \
                  16,                                                                                                  \
                  15,                                                                                                  \
                  14,                                                                                                  \
                  13,                                                                                                  \
                  12,                                                                                                  \
                  11,                                                                                                  \
                  10,                                                                                                  \
                  9,                                                                                                   \
                  8,                                                                                                   \
                  7,                                                                                                   \
                  6,                                                                                                   \
                  5,                                                                                                   \
                  4,                                                                                                   \
                  3,                                                                                                   \
                  2,                                                                                                   \
                  1,                                                                                                   \
                  0,                                                                                                   \
                  1,                                                                                                   \
                  0)

#define normalize_period 2
#define win_overlap_len 40

#define INF 10000
#else

#ifdef WINIMP_IS_SSE8

#ifndef LV_HAVE_SSE
#error "Selected SSE window decoder but instruction set not supported"
#endif

#include <nmmintrin.h>

#define WINIMP sse8
#define nof_blocks 16

#define llr_t int8_t

#define simd_type_t __m128i
#define simd_load _mm_load_si128
#define simd_store _mm_store_si128
#define simd_add _mm_adds_epi8
#define simd_sub _mm_subs_epi8
#define simd_max _mm_max_epi8
#define simd_set1 _mm_set1_epi8
#define simd_insert _mm_insert_epi8
#define simd_shuffle _mm_shuffle_epi8
#define move_right _mm_set_epi8(15, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define move_left _mm_set_epi8(14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0)
#define simd_rb_shift simd_rb_shift_128

#define normalize_max
#define normalize_period 1
#define win_overlap_len 40
#define use_saturated_add
#define divide_output 1

#define INF 0

inline static simd_type_t simd_rb_shift_128(simd_type_t v, const int l)
{
  __m128i low = _mm_srai_epi16(_mm_slli_epi16(v, 8), l + 8);
  __m128i hi  = _mm_srai_epi16(v, l);
  return _mm_blendv_epi8(hi, low, _mm_set1_epi32(0x00FF00FF));
}

#else

#ifdef WINIMP_IS_AVX8

#ifndef LV_HAVE_AVX2
#error "Selected AVX2 window decoder but instruction set not supported"
#endif

#include <immintrin.h>

#define WINIMP avx8
#define nof_blocks 32

#define llr_t int8_t

#define simd_type_t __m256i
#define simd_load _mm256_load_si256
#define simd_store _mm256_store_si256
#define simd_add _mm256_adds_epi8
#define simd_sub _mm256_subs_epi8
#define simd_max _mm256_max_epi8
#define simd_set1 _mm256_set1_epi8
#define simd_insert _mm256_insert_epi8
#define simd_shuffle _mm256_shuffle_epi8
#define move_right                                                                                                     \
  _mm256_set_epi8(31,                                                                                                  \
                  31,                                                                                                  \
                  30,                                                                                                  \
                  29,                                                                                                  \
                  28,                                                                                                  \
                  27,                                                                                                  \
                  26,                                                                                                  \
                  25,                                                                                                  \
                  24,                                                                                                  \
                  23,                                                                                                  \
                  22,                                                                                                  \
                  21,                                                                                                  \
                  20,                                                                                                  \
                  19,                                                                                                  \
                  18,                                                                                                  \
                  17,                                                                                                  \
                  16,                                                                                                  \
                  15,                                                                                                  \
                  14,                                                                                                  \
                  13,                                                                                                  \
                  12,                                                                                                  \
                  11,                                                                                                  \
                  10,                                                                                                  \
                  9,                                                                                                   \
                  8,                                                                                                   \
                  7,                                                                                                   \
                  6,                                                                                                   \
                  5,                                                                                                   \
                  4,                                                                                                   \
                  3,                                                                                                   \
                  2,                                                                                                   \
                  1)
#define move_left                                                                                                      \
  _mm256_set_epi8(30,                                                                                                  \
                  29,                                                                                                  \
                  28,                                                                                                  \
                  27,                                                                                                  \
                  26,                                                                                                  \
                  25,                                                                                                  \
                  24,                                                                                                  \
                  23,                                                                                                  \
                  22,                                                                                                  \
                  21,                                                                                                  \
                  20,                                                                                                  \
                  19,                                                                                                  \
                  18,                                                                                                  \
                  17,                                                                                                  \
                  16,                                                                                                  \
                  15,                                                                                                  \
                  14,                                                                                                  \
                  13,                                                                                                  \
                  12,                                                                                                  \
                  11,                                                                                                  \
                  10,                                                                                                  \
                  9,                                                                                                   \
                  8,                                                                                                   \
                  7,                                                                                                   \
                  6,                                                                                                   \
                  5,                                                                                                   \
                  4,                                                                                                   \
                  3,                                                                                                   \
                  2,                                                                                                   \
                  1,                                                                                                   \
                  0,                                                                                                   \
                  0)
#define simd_rb_shift simd_rb_shift_256

#define INF 0

#define normalize_max
#define normalize_period 1
#define win_overlap_len 40
#define use_saturated_add
#define divide_output 1

inline static simd_type_t simd_rb_shift_256(simd_type_t v, const int l)
{
  __m256i low = _mm256_srai_epi16(_mm256_slli_epi16(v, 8), l + 8);
  __m256i hi  = _mm256_srai_epi16(v, l);
  return _mm256_blendv_epi8(hi, low, _mm256_set1_epi32(0x00FF00FF));
}

#else
#ifdef WINIMP_IS_NEON16
#include <arm_neon.h>

#define WINIMP arm16
#define nof_blocks 8

#define llr_t int16_t

#define v_insert_s16(a, b, imm) ({ (vsetq_lane_s16((b), (a), (imm))); })

#define int8x16_to_8x8x2(v) ((int8x8x2_t){{vget_low_s8(v), vget_high_s8(v)}}) // TODO

static inline int movemask_neon(uint8x16_t movemask_low_in)
{

  uint8x8_t mask_and = vdup_n_u8(0x80);
  int8_t __attribute__((aligned(16))) xr[8];
  for (int i = 0; i < 8; i++)
    xr[i] = i - 7;

  int8x8_t  mask_shift = vld1_s8(xr);
  uint8x8_t lo         = vget_low_u8(movemask_low_in);
  uint8x8_t hi         = vget_high_u8(movemask_low_in);
  lo                   = vand_u8(lo, mask_and);
  lo                   = vshl_u8(lo, mask_shift);
  hi                   = vand_u8(hi, mask_and);
  hi                   = vshl_u8(hi, mask_shift);

  lo = vpadd_u8(lo, lo);
  lo = vpadd_u8(lo, lo);
  lo = vpadd_u8(lo, lo);

  hi = vpadd_u8(hi, hi);
  hi = vpadd_u8(hi, hi);
  hi = vpadd_u8(hi, hi);

  return ((hi[0] << 8) | (lo[0] & 0xFF));
}
inline static int16x8_t vshuff_s8(int16x8_t in, uint8x16_t mask)
{
  int8x8x2_t x  = int8x16_to_8x8x2((int8x16_t)in);
  int8x8_t   u  = (int8x8_t)vget_low_u8(mask);
  int8x8_t   eq = vtbl2_s8(x, u);

  int8x8x2_t x2  = int8x16_to_8x8x2((int8x16_t)in);
  int8x8_t   u2  = (int8x8_t)vget_high_u8(mask);
  int8x8_t   eq2 = vtbl2_s8(x2, u2);
  return (int16x8_t)vcombine_s8(eq, eq2);
}
static inline int16x8_t v_packs_s16(int16x8_t a, int16x8_t b)
{
  return (int16x8_t)(vcombine_s8(vqmovn_s16((a)), vqmovn_s16((b))));
}

inline static int16x8_t v_srai_s16(const int16x8_t a, const int count)
{
  int16x8_t b = vmovq_n_s16(-count);
  return vshlq_s16(a, b);
}
inline static uint8x16_t v_load_s8(int i15,
                                   int i14,
                                   int i13,
                                   int i12,
                                   int i11,
                                   int i10,
                                   int i9,
                                   int i8,
                                   int i7,
                                   int i6,
                                   int i5,
                                   int i4,
                                   int i3,
                                   int i2,
                                   int i1,
                                   int i0)
{
  uint8_t __attribute__((aligned(16)))
  data[16] = {i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15};
  return vld1q_u8(data);
}

#define simd_type_t int16x8_t
#define simd_load(x) vld1q_s16((int16_t*)x)
#define simd_store(x, y) vst1q_s16((int16_t*)x, y)
#define simd_add vaddq_s16
#define simd_sub vsubq_s16
#define simd_max vmaxq_s16
#define simd_set1 vdupq_n_s16
#define simd_insert v_insert_s16
#define simd_shuffle vshuff_s8
#define move_right v_load_s8(15, 14, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2)
#define move_left v_load_s8(13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 0)
#define simd_rb_shift v_srai_s16

#define normalize_period 2
#define win_overlap_len 40

#define INF 10000

#else
#error "Unknown WINIMP value"
#endif

#endif
#endif
#endif
#endif

typedef struct SRSLTE_API {
  uint32_t max_long_cb;
  llr_t*   beta;
} MAKE_TYPE;

#define long_sb (long_cb / nof_blocks)

#define debug_enabled_win 0

#if debug_enabled_win
#define debug_state(d)                                                                                                 \
  printf("k=%5d, in=%5d, pa=%3d, out=%5d, alpha=[",                                                                    \
         d* long_sb + k + 1,                                                                                           \
         MAKE_FUNC(get_simd)(x, d),                                                                                    \
         MAKE_FUNC(get_simd)(y, d),                                                                                    \
         MAKE_FUNC(get_simd)(out, d));                                                                                 \
  for (int j = 0; j < 8; j++)                                                                                          \
    printf("%5d, ", MAKE_FUNC(get_simd)(old[j], d));                                                                   \
  printf("], beta=[");                                                                                                 \
  for (int j = 0; j < 8; j++)                                                                                          \
    printf("%5d, ", MAKE_FUNC(get_simd)(beta_save[j], d));                                                             \
  printf("\n");

#define debug_state_pre(d)                                                                                             \
  printf("pre-window k=%5d, in=%5d, pa=%3d, alpha=[",                                                                  \
         (d + 1) * long_sb - loop_len + k + 1,                                                                         \
         MAKE_FUNC(get_simd)(x, d),                                                                                    \
         MAKE_FUNC(get_simd)(y, d));                                                                                   \
  for (int j = 0; j < 8; j++)                                                                                          \
    printf("%5d, ", MAKE_FUNC(get_simd)(old[j], d));                                                                   \
  printf("]\n");

#define debug_state_beta(d)                                                                                            \
  printf("k=%5d, in=%5d, pa=%3d, beta=[", d* long_sb + k, MAKE_FUNC(get_simd)(x, d), MAKE_FUNC(get_simd)(y, d));       \
  for (int j = 0; j < 8; j++)                                                                                          \
    printf("%5d, ", MAKE_FUNC(get_simd)(old[j], d));                                                                   \
  printf("\n");

static llr_t MAKE_FUNC(get_simd)(simd_type_t x, uint32_t pos)
{
  llr_t* s = (llr_t*)&x;
  return s[pos];
}

#else
#define debug_state(a)
#define debug_state_pre(a)
#define debug_state_beta(a)
#endif
/*
static void MAKE_FUNC(print_simd)(simd_type_t x) {
  llr_t *s = (llr_t*) &x;
  printf("[");
  for (int i=0;i<nof_blocks;i++) {
    printf("%4d, ", s[i]);
  }
  printf("]\n");
}*/

inline static llr_t MAKE_FUNC(sadd)(llr_t x, llr_t y)
{
#ifndef use_saturated_add
  return x + y;
#else
  int16_t z = (int16_t)x + y;
  return z > 127 ? 127 : (int8_t)z;
#endif
}

inline static void MAKE_FUNC(normalize)(uint32_t k, simd_type_t old[8])
{
  if ((k % normalize_period) == 0 && k != 0) {
#ifdef normalize_max
    simd_type_t m = simd_max(old[0], old[1]);
    for (int i = 2; i < 8; i++) {
      m = simd_max(m, old[i]);
    }
    for (int i = 0; i < 8; i++) {
      old[i] = simd_sub(old[i], m);
    }
#else
    for (int i = 1; i < 8; i++) {
      old[i] = simd_sub(old[i], old[0]);
    }
    old[0] = simd_set1(0);
#endif
  }
}

static void MAKE_FUNC(beta_trellis)(llr_t* input, llr_t* parity, uint32_t long_cb, llr_t old[8])
{
  llr_t m_b[8], new[8];
  llr_t x, y, xy;

  /* Calculate last state using Tail. No need to use SIMD here */
  old[0] = 0;
  for (int i = 1; i < 8; i++) {
    old[i] = -INF;
  }
  for (int k = long_cb + 2; k >= long_cb; k--) {
    x = input[k];
    y = parity[k];

    xy = MAKE_FUNC(sadd)(x, y);

    m_b[0] = MAKE_FUNC(sadd)(old[4], xy);
    m_b[1] = old[4];
    m_b[2] = MAKE_FUNC(sadd)(old[5], y);
    m_b[3] = MAKE_FUNC(sadd)(old[5], x);
    m_b[4] = MAKE_FUNC(sadd)(old[6], x);
    m_b[5] = MAKE_FUNC(sadd)(old[6], y);
    m_b[6] = old[7];
    m_b[7] = MAKE_FUNC(sadd)(old[7], xy);

    new[0] = old[0];
    new[1] = MAKE_FUNC(sadd)(old[0], xy);
    new[2] = MAKE_FUNC(sadd)(old[1], x);
    new[3] = MAKE_FUNC(sadd)(old[1], y);
    new[4] = MAKE_FUNC(sadd)(old[2], y);
    new[5] = MAKE_FUNC(sadd)(old[2], x);
    new[6] = MAKE_FUNC(sadd)(old[3], xy);
    new[7] = old[3];

#if debug_enabled_win
    printf("trellis: k=%d, in=%d, pa=%d, beta: ", k, x, y);
    for (int i = 0; i < 8; i++) {
      printf("%d,", old[i]);
    }
    printf("\n");
#endif

    for (int i = 0; i < 8; i++) {
      if (m_b[i] > new[i])
        new[i] = m_b[i];
      old[i] = new[i];
    }
  }
}

/* Computes beta values */
static void MAKE_FUNC(beta)(MAKE_TYPE* s, llr_t* input, llr_t* app, llr_t* parity, uint32_t long_cb)
{
  simd_type_t m_b[8], new[8], old[8];
  simd_type_t x, y, xy, ap;

  simd_type_t* inputPtr;
  simd_type_t* appPtr;
  simd_type_t* parityPtr;
  simd_type_t* betaPtr = (simd_type_t*)s->beta;

  // Inititialize old state
  for (int i = 0; i < 8; i++) {
    old[i] = simd_set1(0);
  }

  uint32_t loop_len;
  for (int j = 0; j < 2; j++) {

    // First run L states to find initial state for all sub-blocks after first
    if (j == 0) {
      loop_len = win_overlap_len;
    } else {
      loop_len = long_sb;
    }

    // When passing through all window pick estimated initial states (known state for sb=0)
    if (loop_len == long_sb) {

      // shuffle across 128-bit boundary manually
#ifdef WINIMP_IS_AVX16
      llr_t tmp[8];
      for (int i = 0; i < 8; i++) {
        tmp[i] = _mm256_extract_epi16(old[i], 8);
      }
#endif
#ifdef WINIMP_IS_AVX8
      llr_t tmp[8];
      for (int i = 0; i < 8; i++) {
        tmp[i] = _mm256_extract_epi8(old[i], 16);
      }
#endif

      for (int i = 0; i < 8; i++) {
        old[i] = simd_shuffle(old[i], move_right);
      }
      // last sub-block state is calculated from the trellis
      llr_t trellis_old[8];
      MAKE_FUNC(beta_trellis)(input, parity, long_cb, trellis_old);
      for (int i = 0; i < 8; i++) {
        old[i] = simd_insert(old[i], trellis_old[i], nof_blocks - 1);
      }

#ifdef WINIMP_IS_AVX16
      for (int i = 0; i < 8; i++) {
        old[i] = _mm256_insert_epi16(old[i], tmp[i], 7);
      }
#endif
#ifdef WINIMP_IS_AVX8
      for (int i = 0; i < 8; i++) {
        old[i] = _mm256_insert_epi8(old[i], tmp[i], 15);
      }
#endif

      inputPtr  = (simd_type_t*)&input[long_cb - nof_blocks];
      appPtr    = (simd_type_t*)&app[long_cb - nof_blocks];
      parityPtr = (simd_type_t*)&parity[long_cb - nof_blocks];

      for (int i = 0; i < 8; i++) {
        simd_store(&betaPtr[8 * long_sb + i], old[i]);
      }

    } else {
      // when estimating states, just set all to unknown
      for (int i = 0; i < 8; i++) {
        old[i] = simd_set1(-INF);
      }
      inputPtr  = (simd_type_t*)&input[nof_blocks * (loop_len - 1)];
      appPtr    = (simd_type_t*)&app[nof_blocks * (loop_len - 1)];
      parityPtr = (simd_type_t*)&parity[nof_blocks * (loop_len - 1)];
    }

    for (int k = loop_len - 1; k >= 0; k--) {
      x = simd_load(inputPtr--);
      y = simd_load(parityPtr--);

      if (app) {
        ap = simd_load(appPtr--);
        x  = simd_add(ap, x);
      }

      xy = simd_add(x, y);

      m_b[0] = simd_add(old[4], xy);
      m_b[1] = old[4];
      m_b[2] = simd_add(old[5], y);
      m_b[3] = simd_add(old[5], x);
      m_b[4] = simd_add(old[6], x);
      m_b[5] = simd_add(old[6], y);
      m_b[6] = old[7];
      m_b[7] = simd_add(old[7], xy);

      new[0] = old[0];
      new[1] = simd_add(old[0], xy);
      new[2] = simd_add(old[1], x);
      new[3] = simd_add(old[1], y);
      new[4] = simd_add(old[2], y);
      new[5] = simd_add(old[2], x);
      new[6] = simd_add(old[3], xy);
      new[7] = old[3];

      // Calculate maximum metric
      for (int i = 0; i < 8; i++) {
        old[i] = simd_max(m_b[i], new[i]);
      }
      // Store metric only when doing the final pass
      if (loop_len == long_sb) {
        for (int i = 0; i < 8; i++) {
          simd_store(&betaPtr[8 * k + i], old[i]);
        }
      }
      if (loop_len != long_sb) {
        debug_state_beta(0);
      } else {
        debug_state_beta(0);
      }

      // normalize
      MAKE_FUNC(normalize)(k, old);
    }
  }
}

/* Computes alpha metrics */
static void MAKE_FUNC(alpha)(MAKE_TYPE* s, llr_t* input, llr_t* app, llr_t* parity, llr_t* output, uint32_t long_cb)
{
  simd_type_t m_b[8], new[8], old[8], max1[8], max0[8];
  simd_type_t x, y, xy, ap;
  simd_type_t m1, m0;

  simd_type_t* inputPtr;
  simd_type_t* appPtr;
  simd_type_t* parityPtr;
  simd_type_t* betaPtr   = (simd_type_t*)s->beta;
  simd_type_t* outputPtr = (simd_type_t*)output;

#if debug_enabled_win
  simd_type_t beta_save[8];
#endif

  // Skip state 0
  betaPtr += 8;

  uint32_t loop_len;

  for (int j = 0; j < 2; j++) {

    // First run L states to find initial state for all sub-blocks after first
    if (j == 0) {
      loop_len = win_overlap_len;
    } else {
      loop_len = long_sb;
    }

    // When passing through all window pick estimated initial states (known state for sb=0)
    if (loop_len == long_sb) {

#ifdef WINIMP_IS_AVX16
      llr_t tmp[8];
      for (int i = 0; i < 8; i++) {
        tmp[i] = _mm256_extract_epi16(old[i], 7);
      }
#endif
#ifdef WINIMP_IS_AVX8
      llr_t tmp[8];
      for (int i = 0; i < 8; i++) {
        tmp[i] = _mm256_extract_epi8(old[i], 15);
      }
#endif
      for (int i = 0; i < 8; i++) {
        old[i] = simd_shuffle(old[i], move_left);
      }
#ifdef WINIMP_IS_AVX16
      for (int i = 0; i < 8; i++) {
        old[i] = _mm256_insert_epi16(old[i], tmp[i], 8);
      }
#endif
#ifdef WINIMP_IS_AVX8
      for (int i = 0; i < 8; i++) {
        old[i] = _mm256_insert_epi8(old[i], tmp[i], 16);
      }
#endif
      // 1st sub-block state is known
      old[0] = simd_insert(old[0], 0, 0);
      for (int i = 1; i < 8; i++) {
        old[i] = simd_insert(old[i], -INF, 0);
      }
    } else {
      // when estimating states, just set all to unknown
      for (int i = 0; i < 8; i++) {
        old[i] = simd_set1(-INF);
      }
    }

    inputPtr  = (simd_type_t*)&input[nof_blocks * (long_sb - loop_len)];
    appPtr    = (simd_type_t*)&app[nof_blocks * (long_sb - loop_len)];
    parityPtr = (simd_type_t*)&parity[nof_blocks * (long_sb - loop_len)];

    for (int k = 0; k < loop_len; k++) {
      x = simd_load(inputPtr++);
      y = simd_load(parityPtr++);

      if (app) {
        ap = simd_load(appPtr++);
        x  = simd_add(ap, x);
      }

      xy = simd_add(x, y);

      m_b[0] = old[0];
      m_b[1] = simd_add(old[3], y);
      m_b[2] = simd_add(old[4], y);
      m_b[3] = old[7];
      m_b[4] = old[1];
      m_b[5] = simd_add(old[2], y);
      m_b[6] = simd_add(old[5], y);
      m_b[7] = old[6];

      new[0] = simd_add(old[1], xy);
      new[1] = simd_add(old[2], x);
      new[2] = simd_add(old[5], x);
      new[3] = simd_add(old[6], xy);
      new[4] = simd_add(old[0], xy);
      new[5] = simd_add(old[3], x);
      new[6] = simd_add(old[4], x);
      new[7] = simd_add(old[7], xy);

      // Load beta and compute output only when passing through all window
      if (loop_len == long_sb) {
        simd_type_t beta;
        for (int i = 0; i < 8; i++) {
          beta    = simd_load(betaPtr++);
          max0[i] = simd_add(beta, m_b[i]);
          max1[i] = simd_add(beta, new[i]);

#if debug_enabled_win
          beta_save[i] = beta;
#endif
        }

        m1 = simd_max(max1[0], max1[1]);
        m0 = simd_max(max0[0], max0[1]);

        for (int i = 2; i < 8; i++) {
          m1 = simd_max(m1, max1[i]);
          m0 = simd_max(m0, max0[i]);
        }

        simd_type_t out = simd_sub(m1, m0);

        // Divide output when using 8-bit arithmetic
#ifdef divide_output
        out = simd_rb_shift(out, divide_output);
#endif

        simd_store(outputPtr++, out);

        debug_state(0);
      }

      for (int i = 0; i < 8; i++) {
        old[i] = simd_max(m_b[i], new[i]);
      }

      // normalize
      MAKE_FUNC(normalize)(k, old);

      if (loop_len != long_sb) {
        debug_state_pre(0);
      }
    }
  }
}

int MAKE_FUNC(init)(void** hh, uint32_t max_long_cb)
{
  *hh = calloc(1, sizeof(MAKE_TYPE));

  MAKE_TYPE* h = (MAKE_TYPE*)*hh;

  h->beta = srslte_vec_malloc(sizeof(llr_t) * 8 * max_long_cb * nof_blocks);
  if (!h->beta) {
    perror("srslte_vec_malloc");
    return -1;
  }
  h->max_long_cb = max_long_cb;
  return nof_blocks;
}

void MAKE_FUNC(free)(void* hh)
{
  MAKE_TYPE* h = (MAKE_TYPE*)hh;
  if (h) {
    if (h->beta) {
      free(h->beta);
    }
    free(h);
  }
}

void MAKE_FUNC(dec)(void* hh, llr_t* input, llr_t* app, llr_t* parity, llr_t* output, uint32_t long_cb)
{
  MAKE_TYPE* h = (MAKE_TYPE*)hh;
  MAKE_FUNC(beta)(h, input, app, parity, long_cb);
  MAKE_FUNC(alpha)(h, input, app, parity, output, long_cb);
#if debug_enabled_win
  printf("running win decoder: %s\n", STRING(WINIMP));
#endif
}

#define INSERT8_INPUT(reg, st, off)                                                                                    \
  reg = simd_insert(reg, input[3 * (i + (st + 0) * long_sb) + off], st + 0);                                           \
  reg = simd_insert(reg, input[3 * (i + (st + 1) * long_sb) + off], st + 1);                                           \
  reg = simd_insert(reg, input[3 * (i + (st + 2) * long_sb) + off], st + 2);                                           \
  reg = simd_insert(reg, input[3 * (i + (st + 3) * long_sb) + off], st + 3);                                           \
  reg = simd_insert(reg, input[3 * (i + (st + 4) * long_sb) + off], st + 4);                                           \
  reg = simd_insert(reg, input[3 * (i + (st + 5) * long_sb) + off], st + 5);                                           \
  reg = simd_insert(reg, input[3 * (i + (st + 6) * long_sb) + off], st + 6);                                           \
  reg = simd_insert(reg, input[3 * (i + (st + 7) * long_sb) + off], st + 7);

void MAKE_FUNC(
    extract_input)(llr_t* input, llr_t* systematic, llr_t* app2, llr_t* parity_0, llr_t* parity_1, uint32_t long_cb)
{
  simd_type_t* systPtr    = (simd_type_t*)systematic;
  simd_type_t* parity0Ptr = (simd_type_t*)parity_0;
  simd_type_t* parity1Ptr = (simd_type_t*)parity_1;

  simd_type_t syst    = simd_set1(0);
  simd_type_t parity0 = simd_set1(0);
  simd_type_t parity1 = simd_set1(0);

  for (int i = 0; i < long_sb; i++) {
    INSERT8_INPUT(syst, 0, 0);
    INSERT8_INPUT(parity0, 0, 1);
    INSERT8_INPUT(parity1, 0, 2);

#if nof_blocks >= 16
    INSERT8_INPUT(syst, 8, 0);
    INSERT8_INPUT(parity0, 8, 1);
    INSERT8_INPUT(parity1, 8, 2);
#endif

#if nof_blocks >= 32
    INSERT8_INPUT(syst, 16, 0);
    INSERT8_INPUT(parity0, 16, 1);
    INSERT8_INPUT(parity1, 16, 2);
    INSERT8_INPUT(syst, 24, 0);
    INSERT8_INPUT(parity0, 24, 1);
    INSERT8_INPUT(parity1, 24, 2);
#endif

    simd_store(systPtr++, syst);
    simd_store(parity0Ptr++, parity0);
    simd_store(parity1Ptr++, parity1);
  }

  for (int i = long_cb; i < long_cb + 3; i++) {
    systematic[i] = input[3 * long_cb + 2 * (i - long_cb)];
    parity_0[i]   = input[3 * long_cb + 2 * (i - long_cb) + 1];

    app2[i]     = input[3 * long_cb + 6 + 2 * (i - long_cb)];
    parity_1[i] = input[3 * long_cb + 6 + 2 * (i - long_cb) + 1];
  }
}

#define deinter(x, win) ((x % (long_cb / win)) * (win) + x / (long_cb / win))

#define reset_cnt(a, b)                                                                                                \
  if (!((a + 1) % b)) {                                                                                                \
    k += b * nof_blocks;                                                                                               \
    if (k >= long_cb) {                                                                                                \
      k -= (long_cb - 1);                                                                                              \
    }                                                                                                                  \
  }

#ifdef WINIMP_IS_NEON16
#define insert_bit(a, b)                                                                                               \
  ap = v_insert_s16(ap, app1[k + (a % b) * nof_blocks], 7 - a);                                                        \
  reset_cnt(a, b);
#else
#define insert_bit(a, b)                                                                                               \
  ap = _mm_insert_epi16(ap, app1[k + (a % b) * nof_blocks], 7 - a);                                                    \
  reset_cnt(a, b);
#endif

#ifndef WINIMP_IS_NEON16
#define decide_for(b)                                                                                                  \
  for (uint32_t i = 0; i < long_cb / 8; i++) {                                                                         \
    insert_bit(0, b);                                                                                                  \
    insert_bit(1, b);                                                                                                  \
    insert_bit(2, b);                                                                                                  \
    insert_bit(3, b);                                                                                                  \
    insert_bit(4, b);                                                                                                  \
    insert_bit(5, b);                                                                                                  \
    insert_bit(6, b);                                                                                                  \
    insert_bit(7, b);                                                                                                  \
    output[i] = (uint8_t)_mm_movemask_epi8(_mm_cmpgt_epi8(_mm_packs_epi16(ap, zeros), zeros));                         \
  }
#else
#define decide_for(b)                                                                                                  \
  for (uint32_t i = 0; i < long_cb / 8; i++) {                                                                         \
    insert_bit(0, b);                                                                                                  \
    insert_bit(1, b);                                                                                                  \
    insert_bit(2, b);                                                                                                  \
    insert_bit(3, b);                                                                                                  \
    insert_bit(4, b);                                                                                                  \
    insert_bit(5, b);                                                                                                  \
    insert_bit(6, b);                                                                                                  \
    insert_bit(7, b);                                                                                                  \
    output[i] = (uint8_t)movemask_neon((uint8x16_t)vcgtq_s8((int8x16_t)v_packs_s16(ap, (int16x8_t)zeros), zeros));     \
  }
#endif
/* No improvement to use AVX here */
void MAKE_FUNC(decision_byte)(llr_t* app1, uint8_t* output, uint32_t long_cb)
{
  uint32_t k = 0;
#ifdef WINIMP_IS_NEON16
  int8_t    z     = 0;
  int8x16_t zeros = vld1q_dup_s8(&z);
  int16x8_t ap;
#else
  __m128i zeros = _mm_setzero_si128();
  __m128i ap;
#endif
  if ((long_cb % (nof_blocks * 8)) == 0) {
    decide_for(8);
  } else if ((long_cb % (nof_blocks * 4)) == 0) {
    decide_for(4);
  } else if ((long_cb % (nof_blocks * 2)) == 0) {
    decide_for(2);
  } else {
    decide_for(1);
  }
}

#undef WINIMP
#undef nof_blocks
#undef llr_t
#undef normalize_period
#undef INF
#undef win_overlap_len
#undef simd_type_t
#undef simd_load
#undef simd_store
#undef simd_add
#undef simd_sub
#undef simd_max
#undef simd_set1
#undef simd_insert
#undef simd_shuffle
#undef move_right
#undef move_left
#undef debug_enabled_win

#ifdef normalize_max
#undef normalize_max
#endif

#ifdef use_saturated_add
#undef use_saturated_add
#endif

#ifdef simd_rb_shift
#undef simd_rb_shift
#endif

#ifdef divide_output
#undef divide_output
#endif