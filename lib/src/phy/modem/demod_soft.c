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
#include <stdlib.h>
#include <strings.h>

#include "srslte/phy/modem/demod_soft.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#ifdef HAVE_NEONv8
#include <arm_neon.h>
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

#define int8x16_to_8x8x2(v) ((int8x8x2_t){{vget_low_s8(v), vget_high_s8(v)}})

inline static void vshuff_s32_even(int32x4_t a, int imm, int32x4_t* res)
{
  *res = vsetq_lane_s32(vgetq_lane_s32((a), ((imm) >> 2) & 0x3), *res, 1);
  *res = vsetq_lane_s32(vgetq_lane_s32((a), ((imm) >> 6) & 0x3), *res, 3);
}
inline static void vshuff_s32_odd(int32x4_t a, int imm, int32x4_t* res)
{
  *res = vsetq_lane_s32(vgetq_lane_s32((a), (imm)&0x3), *res, 0);
  *res = vsetq_lane_s32(vgetq_lane_s32((a), ((imm) >> 4) & 0x3), *res, 2);
}

inline static void vshuff_s32_idx(int32x4_t a, int imm, int32x4_t* res, int idx)
{
  *res = vsetq_lane_s32(vgetq_lane_s32((a), ((imm) >> idx * 2) & 0x3), *res, idx);
}

inline static void vshuff_s16_idx(int16x8_t a, int imm, int16x8_t* res, int idx)
{
  *res = vsetq_lane_s16(vgetq_lane_s16((a), ((imm) >> (idx * 4)) & 0xF), *res, idx);
}

inline static void vshuff_s16_even(int16x8_t a, int imm, int16x8_t* res)
{
  *res = vsetq_lane_s16(vgetq_lane_s16((a), ((imm) >> 4) & 0xF), *res, 1);
  *res = vsetq_lane_s16(vgetq_lane_s16((a), ((imm) >> 12) & 0xF), *res, 3);
  *res = vsetq_lane_s16(vgetq_lane_s16((a), ((imm) >> 20) & 0xF), *res, 5);
  *res = vsetq_lane_s16(vgetq_lane_s16((a), ((imm) >> 28) & 0xF), *res, 7);
}

inline static void vshuff_s16_odd(int16x8_t a, int imm, int16x8_t* res)
{
  *res = vsetq_lane_s16(vgetq_lane_s16((a), ((imm)) & 0xF), *res, 0);
  *res = vsetq_lane_s16(vgetq_lane_s16((a), ((imm) >> 8) & 0xF), *res, 2);
  *res = vsetq_lane_s16(vgetq_lane_s16((a), ((imm) >> 16) & 0xF), *res, 4);
  *res = vsetq_lane_s16(vgetq_lane_s16((a), ((imm) >> 24) & 0xF), *res, 6);
}

#endif

#ifdef LV_HAVE_SSE
#include <smmintrin.h>
void demod_16qam_lte_s_sse(const cf_t* symbols, short* llr, int nsymbols);
#endif

#define SCALE_SHORT_CONV_QPSK 100
#define SCALE_SHORT_CONV_QAM16 400
#define SCALE_SHORT_CONV_QAM64 700
#define SCALE_SHORT_CONV_QAM256 1000

#define SCALE_BYTE_CONV_QPSK 20
#define SCALE_BYTE_CONV_QAM16 30
#define SCALE_BYTE_CONV_QAM64 40
#define SCALE_BYTE_CONV_QAM256 50

void demod_bpsk_lte_b(const cf_t* symbols, int8_t* llr, int nsymbols)
{
  for (int i = 0; i < nsymbols; i++) {
    llr[i] = (int8_t)(-SCALE_BYTE_CONV_QPSK * (crealf(symbols[i]) + cimagf(symbols[i])) * M_SQRT1_2);
  }
}

void demod_bpsk_lte_s(const cf_t* symbols, short* llr, int nsymbols)
{
  for (int i = 0; i < nsymbols; i++) {
    llr[i] = (short)(-SCALE_SHORT_CONV_QPSK * (crealf(symbols[i]) + cimagf(symbols[i])) * M_SQRT1_2);
  }
}

void demod_bpsk_lte(const cf_t* symbols, float* llr, int nsymbols)
{
  for (int i = 0; i < nsymbols; i++) {
    llr[i] = -(crealf(symbols[i]) + cimagf(symbols[i])) * M_SQRT1_2;
  }
}

void demod_qpsk_lte_b(const cf_t* symbols, int8_t* llr, int nsymbols)
{
  srslte_vec_convert_fb((const float*)symbols, -SCALE_BYTE_CONV_QPSK * M_SQRT2, llr, nsymbols * 2);
}

void demod_qpsk_lte_s(const cf_t* symbols, short* llr, int nsymbols)
{
  srslte_vec_convert_fi((const float*)symbols, -SCALE_SHORT_CONV_QPSK * M_SQRT2, llr, nsymbols * 2);
}

void demod_qpsk_lte(const cf_t* symbols, float* llr, int nsymbols)
{
  srslte_vec_sc_prod_fff((const float*)symbols, -M_SQRT2, llr, nsymbols * 2);
}

void demod_16qam_lte(const cf_t* symbols, float* llr, int nsymbols)
{
  for (int i = 0; i < nsymbols; i++) {
    float yre = crealf(symbols[i]);
    float yim = cimagf(symbols[i]);

    llr[4 * i + 0] = -yre;
    llr[4 * i + 1] = -yim;
    llr[4 * i + 2] = fabsf(yre) - 2 / sqrtf(10);
    llr[4 * i + 3] = fabsf(yim) - 2 / sqrtf(10);
  }
}

#ifdef HAVE_NEONv8

void demod_16qam_lte_s_neon(const cf_t* symbols, short* llr, int nsymbols)
{
  float*      symbolsPtr = (float*)symbols;
  int16x8_t*  resultPtr  = (int16x8_t*)llr;
  float32x4_t symbol1, symbol2;
  int32x4_t   symbol_i1, symbol_i2;
  int16x8_t   symbol_i, symbol_abs;
  int8x16_t   result11, result21;
  result11            = vdupq_n_s8(0);
  result21            = vdupq_n_s8(0);
  int16x8_t   offset  = vdupq_n_s16(2 * SCALE_SHORT_CONV_QAM16 / sqrtf(10));
  float32x4_t scale_v = vdupq_n_f32(-SCALE_SHORT_CONV_QAM16);

  for (int i = 0; i < nsymbols / 4; i++) {
    symbol1 = vld1q_f32(symbolsPtr);
    symbolsPtr += 4;
    symbol2 = vld1q_f32(symbolsPtr);
    symbolsPtr += 4;

    symbol_i1 = vcvtnq_s32_f32(vmulq_f32(symbol1, scale_v));
    symbol_i2 = vcvtnq_s32_f32(vmulq_f32(symbol2, scale_v));
    symbol_i  = vcombine_s16(vqmovn_s32(symbol_i1), vqmovn_s32(symbol_i2));

    symbol_abs = vqabsq_s16(symbol_i);
    symbol_abs = vsubq_s16(symbol_abs, offset);

    vshuff_s32_odd((int32x4_t)symbol_i, 16, (int32x4_t*)&result11);
    vshuff_s32_even((int32x4_t)symbol_abs, 64, (int32x4_t*)&result11);

    vshuff_s32_odd((int32x4_t)symbol_i, 50, (int32x4_t*)&result21);
    vshuff_s32_even((int32x4_t)symbol_abs, 200, (int32x4_t*)&result21);

    vst1q_s8((int8_t*)resultPtr, result11);
    resultPtr++;
    vst1q_s8((int8_t*)resultPtr, result21);
    resultPtr++;
  }
  // Demodulate last symbols
  for (int i = 4 * (nsymbols / 4); i < nsymbols; i++) {
    short yre = (short)(SCALE_SHORT_CONV_QAM16 * crealf(symbols[i]));
    short yim = (short)(SCALE_SHORT_CONV_QAM16 * cimagf(symbols[i]));

    llr[4 * i + 0] = -yre;
    llr[4 * i + 1] = -yim;
    llr[4 * i + 2] = abs(yre) - 2 * SCALE_SHORT_CONV_QAM16 / sqrtf(10);
    llr[4 * i + 3] = abs(yim) - 2 * SCALE_SHORT_CONV_QAM16 / sqrtf(10);
  }
}

void demod_16qam_lte_b_neon(const cf_t* symbols, int8_t* llr, int nsymbols)
{
  float*      symbolsPtr = (float*)symbols;
  int8x16_t*  resultPtr  = (int8x16_t*)llr;
  float32x4_t symbol1, symbol2, symbol3, symbol4;
  int8x16_t   symbol_i, symbol_abs;
  int16x8_t   symbol_12, symbol_34;
  int32x4_t   symbol_i1, symbol_i2, symbol_i3, symbol_i4;
  int8x16_t   offset = vdupq_n_s8(2 * SCALE_BYTE_CONV_QAM16 / sqrtf(10));
  int8x16_t   result1n, result2n;
  float32x4_t scale_v = vdupq_n_f32(-SCALE_BYTE_CONV_QAM16);

  result1n = vdupq_n_s8(0);
  result2n = vdupq_n_s8(0);
  for (int i = 0; i < nsymbols / 8; i++) {

    symbol1 = vld1q_f32(symbolsPtr);
    symbolsPtr += 4;
    symbol2 = vld1q_f32(symbolsPtr);
    symbolsPtr += 4;
    symbol3 = vld1q_f32(symbolsPtr);
    symbolsPtr += 4;
    symbol4 = vld1q_f32(symbolsPtr);
    symbolsPtr += 4;
    symbol_i1 = vcvtnq_s32_f32(vmulq_f32(symbol1, scale_v));
    symbol_i2 = vcvtnq_s32_f32(vmulq_f32(symbol2, scale_v));
    symbol_i3 = vcvtnq_s32_f32(vmulq_f32(symbol3, scale_v));
    symbol_i4 = vcvtnq_s32_f32(vmulq_f32(symbol4, scale_v));

    symbol_12  = (int16x8_t)vcombine_s16(vqmovn_s32(symbol_i1), vqmovn_s32(symbol_i2));
    symbol_34  = (int16x8_t)vcombine_s16(vqmovn_s32(symbol_i3), vqmovn_s32(symbol_i4));
    symbol_i   = (int8x16_t)vcombine_s8(vqmovn_s16(symbol_12), vqmovn_s16(symbol_34));
    symbol_abs = vqabsq_s8(symbol_i);
    symbol_abs = vsubq_s8(symbol_abs, offset);

    vshuff_s16_odd((int16x8_t)symbol_i, 0x3020100, (int16x8_t*)&result1n);
    vshuff_s16_even((int16x8_t)symbol_abs, 0x30201000, (int16x8_t*)&result1n);

    vshuff_s16_odd((int16x8_t)symbol_i, 0x07060504, (int16x8_t*)&result2n);
    vshuff_s16_even((int16x8_t)symbol_abs, 0x70605040, (int16x8_t*)&result2n);

    vst1q_s8((int8_t*)resultPtr, result1n);
    resultPtr++;
    vst1q_s8((int8_t*)resultPtr, result2n);
    resultPtr++;
  }
  // Demodulate last symbols
  for (int i = 8 * (nsymbols / 8); i < nsymbols; i++) {
    short yre = (int8_t)(SCALE_BYTE_CONV_QAM16 * crealf(symbols[i]));
    short yim = (int8_t)(SCALE_BYTE_CONV_QAM16 * cimagf(symbols[i]));

    llr[4 * i + 0] = -yre;
    llr[4 * i + 1] = -yim;
    llr[4 * i + 2] = abs(yre) - 2 * SCALE_BYTE_CONV_QAM16 / sqrtf(10);
    llr[4 * i + 3] = abs(yim) - 2 * SCALE_BYTE_CONV_QAM16 / sqrtf(10);
  }
}

#endif

#ifdef LV_HAVE_SSE

void demod_16qam_lte_s_sse(const cf_t* symbols, short* llr, int nsymbols)
{
  float*   symbolsPtr = (float*)symbols;
  __m128i* resultPtr  = (__m128i*)llr;
  __m128   symbol1, symbol2;
  __m128i  symbol_i1, symbol_i2, symbol_i, symbol_abs;
  __m128i  offset = _mm_set1_epi16(2 * SCALE_SHORT_CONV_QAM16 / sqrtf(10));
  __m128i  result11, result12, result22, result21;
  __m128   scale_v           = _mm_set1_ps(-SCALE_SHORT_CONV_QAM16);
  __m128i  shuffle_negated_1 = _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 7, 6, 5, 4, 0xff, 0xff, 0xff, 0xff, 3, 2, 1, 0);
  __m128i  shuffle_abs_1     = _mm_set_epi8(7, 6, 5, 4, 0xff, 0xff, 0xff, 0xff, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff);

  __m128i shuffle_negated_2 =
      _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 15, 14, 13, 12, 0xff, 0xff, 0xff, 0xff, 11, 10, 9, 8);
  __m128i shuffle_abs_2 = _mm_set_epi8(15, 14, 13, 12, 0xff, 0xff, 0xff, 0xff, 11, 10, 9, 8, 0xff, 0xff, 0xff, 0xff);

  for (int i = 0; i < nsymbols / 4; i++) {
    symbol1 = _mm_load_ps(symbolsPtr);
    symbolsPtr += 4;
    symbol2 = _mm_load_ps(symbolsPtr);
    symbolsPtr += 4;
    symbol_i1 = _mm_cvtps_epi32(_mm_mul_ps(symbol1, scale_v));
    symbol_i2 = _mm_cvtps_epi32(_mm_mul_ps(symbol2, scale_v));
    symbol_i  = _mm_packs_epi32(symbol_i1, symbol_i2);

    symbol_abs = _mm_abs_epi16(symbol_i);
    symbol_abs = _mm_sub_epi16(symbol_abs, offset);

    result11 = _mm_shuffle_epi8(symbol_i, shuffle_negated_1);
    result12 = _mm_shuffle_epi8(symbol_abs, shuffle_abs_1);

    result21 = _mm_shuffle_epi8(symbol_i, shuffle_negated_2);
    result22 = _mm_shuffle_epi8(symbol_abs, shuffle_abs_2);

    _mm_store_si128(resultPtr, _mm_or_si128(result11, result12));
    resultPtr++;
    _mm_store_si128(resultPtr, _mm_or_si128(result21, result22));
    resultPtr++;
  }
  // Demodulate last symbols
  for (int i = 4 * (nsymbols / 4); i < nsymbols; i++) {
    short yre = (short)(SCALE_SHORT_CONV_QAM16 * crealf(symbols[i]));
    short yim = (short)(SCALE_SHORT_CONV_QAM16 * cimagf(symbols[i]));

    llr[4 * i + 0] = -yre;
    llr[4 * i + 1] = -yim;
    llr[4 * i + 2] = abs(yre) - 2 * SCALE_SHORT_CONV_QAM16 / sqrtf(10);
    llr[4 * i + 3] = abs(yim) - 2 * SCALE_SHORT_CONV_QAM16 / sqrtf(10);
  }
}

void demod_16qam_lte_b_sse(const cf_t* symbols, int8_t* llr, int nsymbols)
{
  float*   symbolsPtr = (float*)symbols;
  __m128i* resultPtr  = (__m128i*)llr;
  __m128   symbol1, symbol2, symbol3, symbol4;
  __m128i  symbol_i1, symbol_i2, symbol_i3, symbol_i4, symbol_i, symbol_abs, symbol_12, symbol_34;
  __m128i  offset = _mm_set1_epi8(2 * SCALE_BYTE_CONV_QAM16 / sqrtf(10));
  __m128i  result1n, result1a, result2n, result2a;
  __m128   scale_v = _mm_set1_ps(-SCALE_BYTE_CONV_QAM16);

  __m128i shuffle_negated_1 = _mm_set_epi8(0xff, 0xff, 7, 6, 0xff, 0xff, 5, 4, 0xff, 0xff, 3, 2, 0xff, 0xff, 1, 0);
  __m128i shuffle_abs_1     = _mm_set_epi8(7, 6, 0xff, 0xff, 5, 4, 0xff, 0xff, 3, 2, 0xff, 0xff, 1, 0, 0xff, 0xff);

  __m128i shuffle_negated_2 =
      _mm_set_epi8(0xff, 0xff, 15, 14, 0xff, 0xff, 13, 12, 0xff, 0xff, 11, 10, 0xff, 0xff, 9, 8);
  __m128i shuffle_abs_2 = _mm_set_epi8(15, 14, 0xff, 0xff, 13, 12, 0xff, 0xff, 11, 10, 0xff, 0xff, 9, 8, 0xff, 0xff);

  for (int i = 0; i < nsymbols / 8; i++) {
    symbol1 = _mm_load_ps(symbolsPtr);
    symbolsPtr += 4;
    symbol2 = _mm_load_ps(symbolsPtr);
    symbolsPtr += 4;
    symbol3 = _mm_load_ps(symbolsPtr);
    symbolsPtr += 4;
    symbol4 = _mm_load_ps(symbolsPtr);
    symbolsPtr += 4;
    symbol_i1 = _mm_cvtps_epi32(_mm_mul_ps(symbol1, scale_v));
    symbol_i2 = _mm_cvtps_epi32(_mm_mul_ps(symbol2, scale_v));
    symbol_i3 = _mm_cvtps_epi32(_mm_mul_ps(symbol3, scale_v));
    symbol_i4 = _mm_cvtps_epi32(_mm_mul_ps(symbol4, scale_v));
    symbol_12 = _mm_packs_epi32(symbol_i1, symbol_i2);
    symbol_34 = _mm_packs_epi32(symbol_i3, symbol_i4);
    symbol_i  = _mm_packs_epi16(symbol_12, symbol_34);

    symbol_abs = _mm_abs_epi8(symbol_i);
    symbol_abs = _mm_sub_epi8(symbol_abs, offset);

    result1n = _mm_shuffle_epi8(symbol_i, shuffle_negated_1);
    result1a = _mm_shuffle_epi8(symbol_abs, shuffle_abs_1);

    result2n = _mm_shuffle_epi8(symbol_i, shuffle_negated_2);
    result2a = _mm_shuffle_epi8(symbol_abs, shuffle_abs_2);

    _mm_store_si128(resultPtr, _mm_or_si128(result1n, result1a));
    resultPtr++;
    _mm_store_si128(resultPtr, _mm_or_si128(result2n, result2a));
    resultPtr++;
  }
  // Demodulate last symbols
  for (int i = 8 * (nsymbols / 8); i < nsymbols; i++) {
    short yre = (int8_t)(SCALE_BYTE_CONV_QAM16 * crealf(symbols[i]));
    short yim = (int8_t)(SCALE_BYTE_CONV_QAM16 * cimagf(symbols[i]));

    llr[4 * i + 0] = -yre;
    llr[4 * i + 1] = -yim;
    llr[4 * i + 2] = abs(yre) - 2 * SCALE_BYTE_CONV_QAM16 / sqrtf(10);
    llr[4 * i + 3] = abs(yim) - 2 * SCALE_BYTE_CONV_QAM16 / sqrtf(10);
  }
}

#endif

void demod_16qam_lte_s(const cf_t* symbols, short* llr, int nsymbols)
{
#ifdef LV_HAVE_SSE
  demod_16qam_lte_s_sse(symbols, llr, nsymbols);
#else
#ifdef HAVE_NEONv8
  demod_16qam_lte_s_neon(symbols, llr, nsymbols);
#else
  for (int i = 0; i < nsymbols; i++) {
    short yre = (short)(SCALE_SHORT_CONV_QAM16 * crealf(symbols[i]));
    short yim = (short)(SCALE_SHORT_CONV_QAM16 * cimagf(symbols[i]));

    llr[4 * i + 0] = -yre;
    llr[4 * i + 1] = -yim;
    llr[4 * i + 2] = abs(yre) - 2 * SCALE_SHORT_CONV_QAM16 / sqrtf(10);
    llr[4 * i + 3] = abs(yim) - 2 * SCALE_SHORT_CONV_QAM16 / sqrtf(10);
  }
#endif
#endif
}

void demod_16qam_lte_b(const cf_t* symbols, int8_t* llr, int nsymbols)
{
#ifdef LV_HAVE_SSE
  demod_16qam_lte_b_sse(symbols, llr, nsymbols);
#else
#ifdef HAVE_NEONv8
  demod_16qam_lte_b_neon(symbols, llr, nsymbols);
#else
  for (int i = 0; i < nsymbols; i++) {
    int8_t yre = (int8_t)(SCALE_BYTE_CONV_QAM16 * crealf(symbols[i]));
    int8_t yim = (int8_t)(SCALE_BYTE_CONV_QAM16 * cimagf(symbols[i]));

    llr[4 * i + 0] = -yre;
    llr[4 * i + 1] = -yim;
    llr[4 * i + 2] = abs(yre) - 2 * SCALE_BYTE_CONV_QAM16 / sqrtf(10);
    llr[4 * i + 3] = abs(yim) - 2 * SCALE_BYTE_CONV_QAM16 / sqrtf(10);
  }
#endif
#endif
}

void demod_64qam_lte(const cf_t* symbols, float* llr, int nsymbols)
{
  for (int i = 0; i < nsymbols; i++) {
    float yre = crealf(symbols[i]);
    float yim = cimagf(symbols[i]);

    llr[6 * i + 0] = -yre;
    llr[6 * i + 1] = -yim;
    llr[6 * i + 2] = fabsf(yre) - 4 / sqrtf(42);
    llr[6 * i + 3] = fabsf(yim) - 4 / sqrtf(42);
    llr[6 * i + 4] = fabsf(llr[6 * i + 2]) - 2 / sqrtf(42);
    llr[6 * i + 5] = fabsf(llr[6 * i + 3]) - 2 / sqrtf(42);
  }
}
#ifdef HAVE_NEONv8

void demod_64qam_lte_s_neon(const cf_t* symbols, short* llr, int nsymbols)
{
  float*      symbolsPtr = (float*)symbols;
  uint16x8_t* resultPtr  = (uint16x8_t*)llr;
  float32x4_t symbol1, symbol2;
  int16x8_t   symbol_i, symbol_abs, symbol_abs2;
  int32x4_t   symbol_i1, symbol_i2;
  int16x8_t   offset1 = vdupq_n_s16(4 * SCALE_SHORT_CONV_QAM64 / sqrtf(42));
  int16x8_t   offset2 = vdupq_n_s16(2 * SCALE_SHORT_CONV_QAM64 / sqrtf(42));
  float32x4_t scale_v = vdupq_n_f32(-SCALE_SHORT_CONV_QAM64);

  int16x8_t result11 = vdupq_n_s16(0);
  int16x8_t result21 = vdupq_n_s16(0);
  int16x8_t result31 = vdupq_n_s16(0);

  for (int i = 0; i < nsymbols / 4; i++) {

    symbol1 = vld1q_f32(symbolsPtr);
    symbolsPtr += 4;
    symbol2 = vld1q_f32(symbolsPtr);
    symbolsPtr += 4;
    symbol_i1   = vcvtnq_s32_f32(vmulq_f32(symbol1, scale_v));
    symbol_i2   = vcvtnq_s32_f32(vmulq_f32(symbol2, scale_v));
    symbol_i    = vcombine_s16(vqmovn_s32(symbol_i1), vqmovn_s32(symbol_i2));
    symbol_abs  = vqabsq_s16(symbol_i);
    symbol_abs  = vsubq_s16(symbol_abs, offset1);
    symbol_abs2 = vsubq_s16(vqabsq_s16(symbol_abs), offset2);

    vshuff_s32_idx((int32x4_t)symbol_i, 64, (int32x4_t*)&result11, 0);
    vshuff_s32_idx((int32x4_t)symbol_abs, 64, (int32x4_t*)&result11, 1);
    vshuff_s32_idx((int32x4_t)symbol_abs2, 64, (int32x4_t*)&result11, 2);
    vshuff_s32_idx((int32x4_t)symbol_i, 64, (int32x4_t*)&result11, 3);

    vshuff_s32_idx((int32x4_t)symbol_abs, 165, (int32x4_t*)&result21, 0);
    vshuff_s32_idx((int32x4_t)symbol_abs2, 165, (int32x4_t*)&result21, 1);
    vshuff_s32_idx((int32x4_t)symbol_i, 165, (int32x4_t*)&result21, 2);
    vshuff_s32_idx((int32x4_t)symbol_abs, 165, (int32x4_t*)&result21, 3);

    vshuff_s32_idx((int32x4_t)symbol_abs2, 254, (int32x4_t*)&result31, 0);
    vshuff_s32_idx((int32x4_t)symbol_i, 254, (int32x4_t*)&result31, 1);
    vshuff_s32_idx((int32x4_t)symbol_abs, 254, (int32x4_t*)&result31, 2);
    vshuff_s32_idx((int32x4_t)symbol_abs2, 254, (int32x4_t*)&result31, 3);

    vst1q_s16((int16_t*)resultPtr, result11);
    resultPtr++;
    vst1q_s16((int16_t*)resultPtr, result21);
    resultPtr++;
    vst1q_s16((int16_t*)resultPtr, result31);
    resultPtr++;
  }
  for (int i = 4 * (nsymbols / 4); i < nsymbols; i++) {
    float yre = (short)(SCALE_SHORT_CONV_QAM64 * crealf(symbols[i]));
    float yim = (short)(SCALE_SHORT_CONV_QAM64 * cimagf(symbols[i]));

    llr[6 * i + 0] = -yre;
    llr[6 * i + 1] = -yim;
    llr[6 * i + 2] = fabs(yre) - 4 * SCALE_SHORT_CONV_QAM64 / sqrtf(42);
    llr[6 * i + 3] = fabs(yim) - 4 * SCALE_SHORT_CONV_QAM64 / sqrtf(42);
    llr[6 * i + 4] = abs(llr[6 * i + 2]) - 2 * SCALE_SHORT_CONV_QAM64 / sqrtf(42);
    llr[6 * i + 5] = abs(llr[6 * i + 3]) - 2 * SCALE_SHORT_CONV_QAM64 / sqrtf(42);
  }
}

void demod_64qam_lte_b_neon(const cf_t* symbols, int8_t* llr, int nsymbols)
{
  float*      symbolsPtr = (float*)symbols;
  uint8x16_t* resultPtr  = (uint8x16_t*)llr;
  float32x4_t symbol1, symbol2, symbol3, symbol4;
  int8x16_t   symbol_i, symbol_abs, symbol_abs2;
  int16x8_t   symbol_12, symbol_34;
  int32x4_t   symbol_i1, symbol_i2, symbol_i3, symbol_i4;
  int8x16_t   offset1  = vdupq_n_s8(4 * SCALE_BYTE_CONV_QAM64 / sqrtf(42));
  int8x16_t   offset2  = vdupq_n_s8(2 * SCALE_BYTE_CONV_QAM64 / sqrtf(42));
  float32x4_t scale_v  = vdupq_n_f32(-SCALE_BYTE_CONV_QAM64);
  int8x16_t   result11 = vdupq_n_s8(0);
  int8x16_t   result21 = vdupq_n_s8(0);
  int8x16_t   result31 = vdupq_n_s8(0);

  for (int i = 0; i < nsymbols / 8; i++) {

    symbol1 = vld1q_f32(symbolsPtr);
    symbolsPtr += 4;
    symbol2 = vld1q_f32(symbolsPtr);
    symbolsPtr += 4;
    symbol3 = vld1q_f32(symbolsPtr);
    symbolsPtr += 4;
    symbol4 = vld1q_f32(symbolsPtr);
    symbolsPtr += 4;
    symbol_i1   = vcvtnq_s32_f32(vmulq_f32(symbol1, scale_v));
    symbol_i2   = vcvtnq_s32_f32(vmulq_f32(symbol2, scale_v));
    symbol_i3   = vcvtnq_s32_f32(vmulq_f32(symbol3, scale_v));
    symbol_i4   = vcvtnq_s32_f32(vmulq_f32(symbol4, scale_v));
    symbol_12   = vcombine_s16(vqmovn_s32(symbol_i1), vqmovn_s32(symbol_i2));
    symbol_34   = vcombine_s16(vqmovn_s32(symbol_i3), vqmovn_s32(symbol_i4));
    symbol_i    = vcombine_s8(vqmovn_s16(symbol_12), vqmovn_s16(symbol_34));
    symbol_abs  = vqabsq_s8(symbol_i);
    symbol_abs  = vsubq_s8(symbol_abs, offset1);
    symbol_abs2 = vsubq_s8(vqabsq_s8(symbol_abs), offset2);

    vshuff_s16_idx((int16x8_t)symbol_i, 0x22111000, (int16x8_t*)&result11, 0);
    vshuff_s16_idx((int16x8_t)symbol_abs, 0x22111000, (int16x8_t*)&result11, 1);
    vshuff_s16_idx((int16x8_t)symbol_abs2, 0x22111000, (int16x8_t*)&result11, 2);
    vshuff_s16_idx((int16x8_t)symbol_i, 0x22111000, (int16x8_t*)&result11, 3);
    vshuff_s16_idx((int16x8_t)symbol_abs, 0x22111000, (int16x8_t*)&result11, 4);
    vshuff_s16_idx((int16x8_t)symbol_abs2, 0x22111000, (int16x8_t*)&result11, 5);
    vshuff_s16_idx((int16x8_t)symbol_i, 0x22111000, (int16x8_t*)&result11, 6);
    vshuff_s16_idx((int16x8_t)symbol_abs, 0x22111000, (int16x8_t*)&result11, 7);

    vshuff_s16_idx((int16x8_t)symbol_abs2, 0x54443332, (int16x8_t*)&result21, 0);
    vshuff_s16_idx((int16x8_t)symbol_i, 0x54443332, (int16x8_t*)&result21, 1);
    vshuff_s16_idx((int16x8_t)symbol_abs, 0x54443332, (int16x8_t*)&result21, 2);
    vshuff_s16_idx((int16x8_t)symbol_abs2, 0x54443332, (int16x8_t*)&result21, 3);
    vshuff_s16_idx((int16x8_t)symbol_i, 0x54443332, (int16x8_t*)&result21, 4);
    vshuff_s16_idx((int16x8_t)symbol_abs, 0x54443332, (int16x8_t*)&result21, 5);
    vshuff_s16_idx((int16x8_t)symbol_abs2, 0x54443332, (int16x8_t*)&result21, 6);
    vshuff_s16_idx((int16x8_t)symbol_i, 0x54443332, (int16x8_t*)&result21, 7);

    vshuff_s16_idx((int16x8_t)symbol_abs, 0x77766655, (int16x8_t*)&result31, 0);
    vshuff_s16_idx((int16x8_t)symbol_abs2, 0x77766655, (int16x8_t*)&result31, 1);
    vshuff_s16_idx((int16x8_t)symbol_i, 0x77766655, (int16x8_t*)&result31, 2);
    vshuff_s16_idx((int16x8_t)symbol_abs, 0x77766655, (int16x8_t*)&result31, 3);
    vshuff_s16_idx((int16x8_t)symbol_abs2, 0x77766655, (int16x8_t*)&result31, 4);
    vshuff_s16_idx((int16x8_t)symbol_i, 0x77766655, (int16x8_t*)&result31, 5);
    vshuff_s16_idx((int16x8_t)symbol_abs, 0x77766655, (int16x8_t*)&result31, 6);
    vshuff_s16_idx((int16x8_t)symbol_abs2, 0x77766655, (int16x8_t*)&result31, 7);

    vst1q_s8((int8_t*)resultPtr, result11);
    resultPtr++;
    vst1q_s8((int8_t*)resultPtr, result21);
    resultPtr++;
    vst1q_s8((int8_t*)resultPtr, result31);
    resultPtr++;
  }
  for (int i = 8 * (nsymbols / 8); i < nsymbols; i++) {
    float yre = (int8_t)(SCALE_BYTE_CONV_QAM64 * crealf(symbols[i]));
    float yim = (int8_t)(SCALE_BYTE_CONV_QAM64 * cimagf(symbols[i]));

    llr[6 * i + 0] = -yre;
    llr[6 * i + 1] = -yim;
    llr[6 * i + 2] = fabs(yre) - 4 * SCALE_BYTE_CONV_QAM64 / sqrtf(42);
    llr[6 * i + 3] = fabs(yim) - 4 * SCALE_BYTE_CONV_QAM64 / sqrtf(42);
    llr[6 * i + 4] = abs(llr[6 * i + 2]) - 2 * SCALE_BYTE_CONV_QAM64 / sqrtf(42);
    llr[6 * i + 5] = abs(llr[6 * i + 3]) - 2 * SCALE_BYTE_CONV_QAM64 / sqrtf(42);
  }
}

#endif

#ifdef LV_HAVE_SSE

static void demod_64qam_lte_s_sse(const cf_t* symbols, int16_t* llr, int nsymbols)
{
  float*   symbolsPtr = (float*)symbols;
  __m128i* resultPtr  = (__m128i*)llr;
  __m128   symbol1, symbol2;
  __m128i  symbol_i1, symbol_i2, symbol_i, symbol_abs, symbol_abs2;
  __m128i  offset1 = _mm_set1_epi16(4 * SCALE_SHORT_CONV_QAM64 / sqrtf(42));
  __m128i  offset2 = _mm_set1_epi16(2 * SCALE_SHORT_CONV_QAM64 / sqrtf(42));
  __m128   scale_v = _mm_set1_ps(-SCALE_SHORT_CONV_QAM64);
  __m128i  result11, result12, result13, result22, result21, result23, result31, result32, result33;

  __m128i shuffle_negated_1 = _mm_set_epi8(7, 6, 5, 4, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 3, 2, 1, 0);
  __m128i shuffle_negated_2 =
      _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 11, 10, 9, 8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
  __m128i shuffle_negated_3 =
      _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 15, 14, 13, 12, 0xff, 0xff, 0xff, 0xff);

  __m128i shuffle_abs_1 =
      _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff);
  __m128i shuffle_abs_2 = _mm_set_epi8(11, 10, 9, 8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 7, 6, 5, 4);
  __m128i shuffle_abs_3 =
      _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 15, 14, 13, 12, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);

  __m128i shuffle_abs2_1 =
      _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
  __m128i shuffle_abs2_2 =
      _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 7, 6, 5, 4, 0xff, 0xff, 0xff, 0xff);
  __m128i shuffle_abs2_3 = _mm_set_epi8(15, 14, 13, 12, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 11, 10, 9, 8);

  for (int i = 0; i < nsymbols / 4; i++) {
    symbol1 = _mm_load_ps(symbolsPtr);
    symbolsPtr += 4;
    symbol2 = _mm_load_ps(symbolsPtr);
    symbolsPtr += 4;
    symbol_i1 = _mm_cvtps_epi32(_mm_mul_ps(symbol1, scale_v));
    symbol_i2 = _mm_cvtps_epi32(_mm_mul_ps(symbol2, scale_v));
    symbol_i  = _mm_packs_epi32(symbol_i1, symbol_i2);

    symbol_abs  = _mm_abs_epi16(symbol_i);
    symbol_abs  = _mm_sub_epi16(symbol_abs, offset1);
    symbol_abs2 = _mm_sub_epi16(_mm_abs_epi16(symbol_abs), offset2);

    result11 = _mm_shuffle_epi8(symbol_i, shuffle_negated_1);
    result12 = _mm_shuffle_epi8(symbol_abs, shuffle_abs_1);
    result13 = _mm_shuffle_epi8(symbol_abs2, shuffle_abs2_1);

    result21 = _mm_shuffle_epi8(symbol_i, shuffle_negated_2);
    result22 = _mm_shuffle_epi8(symbol_abs, shuffle_abs_2);
    result23 = _mm_shuffle_epi8(symbol_abs2, shuffle_abs2_2);

    result31 = _mm_shuffle_epi8(symbol_i, shuffle_negated_3);
    result32 = _mm_shuffle_epi8(symbol_abs, shuffle_abs_3);
    result33 = _mm_shuffle_epi8(symbol_abs2, shuffle_abs2_3);

    _mm_store_si128(resultPtr, _mm_or_si128(_mm_or_si128(result11, result12), result13));
    resultPtr++;
    _mm_store_si128(resultPtr, _mm_or_si128(_mm_or_si128(result21, result22), result23));
    resultPtr++;
    _mm_store_si128(resultPtr, _mm_or_si128(_mm_or_si128(result31, result32), result33));
    resultPtr++;
  }

  const int16_t threshold1 = 4 * SCALE_SHORT_CONV_QAM64 / sqrtf(42);
  const int16_t threshold2 = 2 * SCALE_SHORT_CONV_QAM64 / sqrtf(42);
  for (int i = 4 * (nsymbols / 4); i < nsymbols; i++) {
    int16_t yre = SCALE_SHORT_CONV_QAM64 * crealf(symbols[i]);
    int16_t yim = SCALE_SHORT_CONV_QAM64 * cimagf(symbols[i]);

    llr[6 * i + 0] = -yre;
    llr[6 * i + 1] = -yim;
    llr[6 * i + 2] = (int16_t)abs(yre) - threshold1;
    llr[6 * i + 3] = (int16_t)abs(yim) - threshold1;
    llr[6 * i + 4] = (int16_t)abs(llr[6 * i + 2]) - threshold2;
    llr[6 * i + 5] = (int16_t)abs(llr[6 * i + 3]) - threshold2;
  }
}

void demod_64qam_lte_b_sse(const cf_t* symbols, int8_t* llr, int nsymbols)
{
  float*   symbolsPtr = (float*)symbols;
  __m128i* resultPtr  = (__m128i*)llr;
  __m128   symbol1, symbol2, symbol3, symbol4;
  __m128i  symbol_i1, symbol_i2, symbol_i3, symbol_i4, symbol_i, symbol_abs, symbol_abs2, symbol_12, symbol_34;
  __m128i  offset1 = _mm_set1_epi8(4 * SCALE_BYTE_CONV_QAM64 / sqrtf(42));
  __m128i  offset2 = _mm_set1_epi8(2 * SCALE_BYTE_CONV_QAM64 / sqrtf(42));
  __m128   scale_v = _mm_set1_ps(-SCALE_BYTE_CONV_QAM64);
  __m128i  result11, result12, result13, result22, result21, result23, result31, result32, result33;

  __m128i shuffle_negated_1 =
      _mm_set_epi8(0xff, 0xff, 5, 4, 0xff, 0xff, 0xff, 0xff, 3, 2, 0xff, 0xff, 0xff, 0xff, 1, 0);
  __m128i shuffle_negated_2 =
      _mm_set_epi8(11, 10, 0xff, 0xff, 0xff, 0xff, 9, 8, 0xff, 0xff, 0xff, 0xff, 7, 6, 0xff, 0xff);
  __m128i shuffle_negated_3 =
      _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 15, 14, 0xff, 0xff, 0xff, 0xff, 13, 12, 0xff, 0xff, 0xff, 0xff);

  __m128i shuffle_abs_1 = _mm_set_epi8(5, 4, 0xff, 0xff, 0xff, 0xff, 3, 2, 0xff, 0xff, 0xff, 0xff, 1, 0, 0xff, 0xff);
  __m128i shuffle_abs_2 =
      _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 9, 8, 0xff, 0xff, 0xff, 0xff, 7, 6, 0xff, 0xff, 0xff, 0xff);
  __m128i shuffle_abs_3 =
      _mm_set_epi8(0xff, 0xff, 15, 14, 0xff, 0xff, 0xff, 0xff, 13, 12, 0xff, 0xff, 0xff, 0xff, 11, 10);

  __m128i shuffle_abs2_1 =
      _mm_set_epi8(0xff, 0xff, 0xff, 0xff, 3, 2, 0xff, 0xff, 0xff, 0xff, 1, 0, 0xff, 0xff, 0xff, 0xff);
  __m128i shuffle_abs2_2 = _mm_set_epi8(0xff, 0xff, 9, 8, 0xff, 0xff, 0xff, 0xff, 7, 6, 0xff, 0xff, 0xff, 0xff, 5, 4);
  __m128i shuffle_abs2_3 =
      _mm_set_epi8(15, 14, 0xff, 0xff, 0xff, 0xff, 13, 12, 0xff, 0xff, 0xff, 0xff, 11, 10, 0xff, 0xff);

  for (int i = 0; i < nsymbols / 8; i++) {
    symbol1 = _mm_load_ps(symbolsPtr);
    symbolsPtr += 4;
    symbol2 = _mm_load_ps(symbolsPtr);
    symbolsPtr += 4;
    symbol3 = _mm_load_ps(symbolsPtr);
    symbolsPtr += 4;
    symbol4 = _mm_load_ps(symbolsPtr);
    symbolsPtr += 4;
    symbol_i1 = _mm_cvtps_epi32(_mm_mul_ps(symbol1, scale_v));
    symbol_i2 = _mm_cvtps_epi32(_mm_mul_ps(symbol2, scale_v));
    symbol_i3 = _mm_cvtps_epi32(_mm_mul_ps(symbol3, scale_v));
    symbol_i4 = _mm_cvtps_epi32(_mm_mul_ps(symbol4, scale_v));
    symbol_12 = _mm_packs_epi32(symbol_i1, symbol_i2);
    symbol_34 = _mm_packs_epi32(symbol_i3, symbol_i4);
    symbol_i  = _mm_packs_epi16(symbol_12, symbol_34);

    symbol_abs  = _mm_abs_epi8(symbol_i);
    symbol_abs  = _mm_sub_epi8(symbol_abs, offset1);
    symbol_abs2 = _mm_sub_epi8(_mm_abs_epi8(symbol_abs), offset2);

    result11 = _mm_shuffle_epi8(symbol_i, shuffle_negated_1);
    result12 = _mm_shuffle_epi8(symbol_abs, shuffle_abs_1);
    result13 = _mm_shuffle_epi8(symbol_abs2, shuffle_abs2_1);

    result21 = _mm_shuffle_epi8(symbol_i, shuffle_negated_2);
    result22 = _mm_shuffle_epi8(symbol_abs, shuffle_abs_2);
    result23 = _mm_shuffle_epi8(symbol_abs2, shuffle_abs2_2);

    result31 = _mm_shuffle_epi8(symbol_i, shuffle_negated_3);
    result32 = _mm_shuffle_epi8(symbol_abs, shuffle_abs_3);
    result33 = _mm_shuffle_epi8(symbol_abs2, shuffle_abs2_3);

    _mm_store_si128(resultPtr, _mm_or_si128(_mm_or_si128(result11, result12), result13));
    resultPtr++;
    _mm_store_si128(resultPtr, _mm_or_si128(_mm_or_si128(result21, result22), result23));
    resultPtr++;
    _mm_store_si128(resultPtr, _mm_or_si128(_mm_or_si128(result31, result32), result33));
    resultPtr++;
  }

  const int8_t threshold1 = 4 * SCALE_BYTE_CONV_QAM64 / sqrtf(42);
  const int8_t threshold2 = 2 * SCALE_BYTE_CONV_QAM64 / sqrtf(42);
  for (int i = 8 * (nsymbols / 8); i < nsymbols; i++) {
    int8_t yre = SCALE_BYTE_CONV_QAM64 * crealf(symbols[i]);
    int8_t yim = SCALE_BYTE_CONV_QAM64 * cimagf(symbols[i]);

    llr[6 * i + 0] = -yre;
    llr[6 * i + 1] = -yim;
    llr[6 * i + 2] = (int8_t)abs(yre) - threshold1;
    llr[6 * i + 3] = (int8_t)abs(yim) - threshold1;
    llr[6 * i + 4] = (int8_t)abs(llr[6 * i + 2]) - threshold2;
    llr[6 * i + 5] = (int8_t)abs(llr[6 * i + 3]) - threshold2;
  }
}

#endif

void demod_64qam_lte_s(const cf_t* symbols, short* llr, int nsymbols)
{
#ifdef LV_HAVE_SSE
  demod_64qam_lte_s_sse(symbols, llr, nsymbols);
#else
#ifdef HAVE_NEONv8
  demod_64qam_lte_s_neon(symbols, llr, nsymbols);
#else
  for (int i = 0; i < nsymbols; i++) {
    float yre = (short)(SCALE_SHORT_CONV_QAM64 * crealf(symbols[i]));
    float yim = (short)(SCALE_SHORT_CONV_QAM64 * cimagf(symbols[i]));

    llr[6 * i + 0] = -yre;
    llr[6 * i + 1] = -yim;
    llr[6 * i + 2] = abs(yre) - 4 * SCALE_SHORT_CONV_QAM64 / sqrtf(42);
    llr[6 * i + 3] = abs(yim) - 4 * SCALE_SHORT_CONV_QAM64 / sqrtf(42);
    llr[6 * i + 4] = abs(llr[6 * i + 2]) - 2 * SCALE_SHORT_CONV_QAM64 / sqrtf(42);
    llr[6 * i + 5] = abs(llr[6 * i + 3]) - 2 * SCALE_SHORT_CONV_QAM64 / sqrtf(42);
  }
#endif
#endif
}

void demod_64qam_lte_b(const cf_t* symbols, int8_t* llr, int nsymbols)
{
#ifdef LV_HAVE_SSE
  demod_64qam_lte_b_sse(symbols, llr, nsymbols);
#else
#ifdef HAVE_NEONv8
  demod_64qam_lte_b_neon(symbols, llr, nsymbols);
#else
  for (int i = 0; i < nsymbols; i++) {
    float yre = (int8_t)(SCALE_BYTE_CONV_QAM64 * crealf(symbols[i]));
    float yim = (int8_t)(SCALE_BYTE_CONV_QAM64 * cimagf(symbols[i]));

    llr[6 * i + 0] = -yre;
    llr[6 * i + 1] = -yim;
    llr[6 * i + 2] = abs(yre) - 4 * SCALE_BYTE_CONV_QAM64 / sqrtf(42);
    llr[6 * i + 3] = abs(yim) - 4 * SCALE_BYTE_CONV_QAM64 / sqrtf(42);
    llr[6 * i + 4] = abs(llr[6 * i + 2]) - 2 * SCALE_BYTE_CONV_QAM64 / sqrtf(42);
    llr[6 * i + 5] = abs(llr[6 * i + 3]) - 2 * SCALE_BYTE_CONV_QAM64 / sqrtf(42);
  }
#endif
#endif
}

void demod_256qam_lte(const cf_t* symbols, float* llr, int nsymbols)
{
  for (int i = 0; i < nsymbols; i++) {
    float real = -__real__ symbols[i];
    float imag = -__imag__ symbols[i];
    *(llr++)   = real;
    *(llr++)   = imag;
    real       = fabsf(real) - 8.0f / sqrtf(170.0f);
    imag       = fabsf(imag) - 8.0f / sqrtf(170.0f);
    *(llr++)   = real;
    *(llr++)   = imag;
    real       = fabsf(real) - 4.0f / sqrtf(170.0f);
    imag       = fabsf(imag) - 4.0f / sqrtf(170.0f);
    *(llr++)   = real;
    *(llr++)   = imag;
    real       = fabsf(real) - 2.0f / sqrtf(170.0f);
    imag       = fabsf(imag) - 2.0f / sqrtf(170.0f);
    *(llr++)   = real;
    *(llr++)   = imag;
  }
}

void demod_256qam_lte_b(const cf_t* symbols, int8_t* llr, int nsymbols)
{
  for (int i = 0; i < nsymbols; i++) {
    float real = -__real__ symbols[i];
    float imag = -__imag__ symbols[i];
    *(llr++)   = SCALE_BYTE_CONV_QAM256 * real;
    *(llr++)   = SCALE_BYTE_CONV_QAM256 * imag;
    real       = fabsf(real) - 8.0f / sqrtf(170.0f);
    imag       = fabsf(imag) - 8.0f / sqrtf(170.0f);
    *(llr++)   = SCALE_BYTE_CONV_QAM256 * real;
    *(llr++)   = SCALE_BYTE_CONV_QAM256 * imag;
    real       = fabsf(real) - 4.0f / sqrtf(170.0f);
    imag       = fabsf(imag) - 4.0f / sqrtf(170.0f);
    *(llr++)   = SCALE_BYTE_CONV_QAM256 * real;
    *(llr++)   = SCALE_BYTE_CONV_QAM256 * imag;
    real       = fabsf(real) - 2.0f / sqrtf(170.0f);
    imag       = fabsf(imag) - 2.0f / sqrtf(170.0f);
    *(llr++)   = SCALE_BYTE_CONV_QAM256 * real;
    *(llr++)   = SCALE_BYTE_CONV_QAM256 * imag;
  }
}

void demod_256qam_lte_s(const cf_t* symbols, short* llr, int nsymbols)
{
  for (int i = 0; i < nsymbols; i++) {
    float real = -__real__ symbols[i];
    float imag = -__imag__ symbols[i];
    *(llr++)   = SCALE_SHORT_CONV_QAM256 * real;
    *(llr++)   = SCALE_SHORT_CONV_QAM256 * imag;
    real       = fabsf(real) - 8.0f / sqrtf(170.0f);
    imag       = fabsf(imag) - 8.0f / sqrtf(170.0f);
    *(llr++)   = SCALE_SHORT_CONV_QAM256 * real;
    *(llr++)   = SCALE_SHORT_CONV_QAM256 * imag;
    real       = fabsf(real) - 4.0f / sqrtf(170.0f);
    imag       = fabsf(imag) - 4.0f / sqrtf(170.0f);
    *(llr++)   = SCALE_SHORT_CONV_QAM256 * real;
    *(llr++)   = SCALE_SHORT_CONV_QAM256 * imag;
    real       = fabsf(real) - 2.0f / sqrtf(170.0f);
    imag       = fabsf(imag) - 2.0f / sqrtf(170.0f);
    *(llr++)   = SCALE_SHORT_CONV_QAM256 * real;
    *(llr++)   = SCALE_SHORT_CONV_QAM256 * imag;
  }
}

int srslte_demod_soft_demodulate(srslte_mod_t modulation, const cf_t* symbols, float* llr, int nsymbols)
{
  switch (modulation) {
    case SRSLTE_MOD_BPSK:
      demod_bpsk_lte(symbols, llr, nsymbols);
      break;
    case SRSLTE_MOD_QPSK:
      demod_qpsk_lte(symbols, llr, nsymbols);
      break;
    case SRSLTE_MOD_16QAM:
      demod_16qam_lte(symbols, llr, nsymbols);
      break;
    case SRSLTE_MOD_64QAM:
      demod_64qam_lte(symbols, llr, nsymbols);
      break;
    case SRSLTE_MOD_256QAM:
      demod_256qam_lte(symbols, llr, nsymbols);
      break;
    default:
      ERROR("Invalid modulation %d\n", modulation);
      return -1;
  }
  return 0;
}

int srslte_demod_soft_demodulate_s(srslte_mod_t modulation, const cf_t* symbols, short* llr, int nsymbols)
{
  switch (modulation) {
    case SRSLTE_MOD_BPSK:
      demod_bpsk_lte_s(symbols, llr, nsymbols);
      break;
    case SRSLTE_MOD_QPSK:
      demod_qpsk_lte_s(symbols, llr, nsymbols);
      break;
    case SRSLTE_MOD_16QAM:
      demod_16qam_lte_s(symbols, llr, nsymbols);
      break;
    case SRSLTE_MOD_64QAM:
      demod_64qam_lte_s(symbols, llr, nsymbols);
      break;
    case SRSLTE_MOD_256QAM:
      demod_256qam_lte_s(symbols, llr, nsymbols);
      break;
    default:
      ERROR("Invalid modulation %d\n", modulation);
      return -1;
  }
  return 0;
}

int srslte_demod_soft_demodulate_b(srslte_mod_t modulation, const cf_t* symbols, int8_t* llr, int nsymbols)
{
  switch (modulation) {
    case SRSLTE_MOD_BPSK:
      demod_bpsk_lte_b(symbols, llr, nsymbols);
      break;
    case SRSLTE_MOD_QPSK:
      demod_qpsk_lte_b(symbols, llr, nsymbols);
      break;
    case SRSLTE_MOD_16QAM:
      demod_16qam_lte_b(symbols, llr, nsymbols);
      break;
    case SRSLTE_MOD_64QAM:
      demod_64qam_lte_b(symbols, llr, nsymbols);
      break;
    case SRSLTE_MOD_256QAM:
      demod_256qam_lte_b(symbols, llr, nsymbols);
      break;
    default:
      ERROR("Invalid modulation %d\n", modulation);
      return -1;
  }
  return 0;
}
