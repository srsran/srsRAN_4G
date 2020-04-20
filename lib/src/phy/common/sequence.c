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

#include "srslte/phy/common/sequence.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

#ifdef LV_HAVE_SSE
#include <immintrin.h>
#endif /* LV_HAVE_SSE */

/**
 * Length of the seed, used for the feedback delay. Do not change.
 */
#define SEQUENCE_SEED_LEN (31)

/**
 * Nc parameter defined in 3GPP. Do not change.
 */
#define SEQUENCE_NC (1600)

/*
 * Pseudo Random Sequence generation.
 * It follows the 3GPP Release 8 (LTE) 36.211
 * Section 7.2
 */

/**
 * Parallel bit generation for x1/x2 sequences parameters. Exploits the fact that the sequence generation is 31 chips
 * ahead and the maximum register shift is 3 (for x2).
 */
#define SEQUENCE_PAR_BITS (28U)
#define SEQUENCE_MASK ((1U << SEQUENCE_PAR_BITS) - 1U)

/**
 * Computes one step of the X1 sequence for SEQUENCE_PAR_BITS simultaneously
 * @param state 32 bit current state
 * @return new 32 bit state
 */
static inline uint32_t sequence_gen_LTE_pr_memless_step_par_x1(uint32_t state)
{
  // Perform XOR
  uint32_t f = state ^ (state >> 3U);

  // Prepare feedback
  f = ((f & SEQUENCE_MASK) << (SEQUENCE_SEED_LEN - SEQUENCE_PAR_BITS));

  // Insert feedback
  state = (state >> SEQUENCE_PAR_BITS) ^ f;

  return state;
}

/**
 * Computes one step of the X1 sequence for 1bit
 * @param state 32 bit current state
 * @return new 32 bit state
 */
static inline uint32_t sequence_gen_LTE_pr_memless_step_x1(uint32_t state)
{
  // Perform XOR
  uint32_t f = state ^ (state >> 3U);

  // Prepare feedback
  f = ((f & 1U) << (SEQUENCE_SEED_LEN - 1U));

  // Insert feedback
  state = (state >> 1U) ^ f;

  return state;
}

/**
 * Computes one step of the X2 sequence for SEQUENCE_PAR_BITS simultaneously
 * @param state 32 bit current state
 * @return new 32 bit state
 */
static inline uint32_t sequence_gen_LTE_pr_memless_step_par_x2(uint32_t state)
{
  // Perform XOR
  uint32_t f = state ^ (state >> 1U) ^ (state >> 2U) ^ (state >> 3U);

  // Prepare feedback
  f = ((f & SEQUENCE_MASK) << (SEQUENCE_SEED_LEN - SEQUENCE_PAR_BITS));

  // Insert feedback
  state = (state >> SEQUENCE_PAR_BITS) ^ f;

  return state;
}

/**
 * Computes one step of the X2 sequence for 1bit
 * @param state 32 bit current state
 * @return new 32 bit state
 */
static inline uint32_t sequence_gen_LTE_pr_memless_step_x2(uint32_t state)
{
  // Perform XOR
  uint32_t f = state ^ (state >> 1U) ^ (state >> 2U) ^ (state >> 3U);

  // Prepare feedback
  f = ((f & 1U) << (SEQUENCE_SEED_LEN - 1U));

  // Insert feedback
  state = (state >> 1U) ^ f;

  return state;
}

/**
 * Static precomputed x1 and x2 states after Nc shifts
 * -------------------------------------------------------
 *
 * The pre-computation of the Pseudo-Random sequences is based in their linearity properties.
 *
 * Having two seeds seed_1 and seed_2 generate x2_1 and x2_2 respectively:
 *     seed_1 -> x2_1
 *     seed_2 -> x2_2
 *
 * Then, the linearity property satisfies:
 *     seed_1 ^ seed_2 -> x2_1 ^ x2_2
 *
 * Because of this, a different x2 can be pre-computed for each bit of the seed.
 *
 */
static uint32_t sequence_x1_init                    = 0;
static uint32_t sequence_x2_init[SEQUENCE_SEED_LEN] = {};

/**
 * C constructor, pre-computes X1 and X2 initial states
 */
__attribute__((constructor)) __attribute__((unused)) static void srslte_lte_pr_pregen()
{

  // Compute transition step
  sequence_x1_init = 1;
  for (uint32_t n = 0; n < SEQUENCE_NC; n++) {
    sequence_x1_init = sequence_gen_LTE_pr_memless_step_x1(sequence_x1_init);
  }

  // For each bit of the seed
  for (uint32_t i = 0; i < SEQUENCE_SEED_LEN; i++) {
    // Compute transition step
    sequence_x2_init[i] = 1U << i;
    for (uint32_t n = 0; n < SEQUENCE_NC; n++) {
      sequence_x2_init[i] = sequence_gen_LTE_pr_memless_step_x2(sequence_x2_init[i]);
    }
  }
}

static uint32_t sequence_get_x2_init(uint32_t seed)
{
  uint32_t x2 = 0;

  for (uint32_t i = 0; i < SEQUENCE_SEED_LEN; i++) {
    if ((seed >> i) & 1U) {
      x2 ^= sequence_x2_init[i];
    }
  }

  return x2;
}

static void sequence_gen_LTE_pr(uint8_t* pr, uint32_t len, uint32_t seed)
{
  int      n  = 0;
  uint32_t x1 = sequence_x1_init;           // X1 initial state is fix
  uint32_t x2 = sequence_get_x2_init(seed); // loads x2 initial state

  // Parallel stage
  if (len >= SEQUENCE_PAR_BITS) {
    for (; n < len - (SEQUENCE_PAR_BITS - 1); n += SEQUENCE_PAR_BITS) {
      // XOR x1 and x2
      uint32_t c = (uint32_t)(x1 ^ x2);

      // Save state
      for (uint32_t i = 0; i < SEQUENCE_PAR_BITS; i++) {
        pr[n + i] = (uint8_t)((c >> i) & 1U);
      }

      // Parallel step
      x1 = sequence_gen_LTE_pr_memless_step_par_x1(x1);
      x2 = sequence_gen_LTE_pr_memless_step_par_x2(x2);
    }
  }

  // Single step
  for (; n < len; n++) {
    // Save current state
    pr[n] = (uint8_t)((x1 ^ x2) & 1U);

    // Single step
    x1 = sequence_gen_LTE_pr_memless_step_x1(x1);
    x2 = sequence_gen_LTE_pr_memless_step_x2(x2);
  }
}

// static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int srslte_sequence_set_LTE_pr(srslte_sequence_t* q, uint32_t len, uint32_t seed)
{
  if (len > q->max_len) {
    ERROR("Error generating pseudo-random sequence: len %d is greater than allocated len %d\n", len, q->max_len);
    return SRSLTE_ERROR;
  }

  sequence_gen_LTE_pr(q->c, len, seed);

  return SRSLTE_SUCCESS;
}

static inline void
sequence_generate_signed(const uint8_t* c_unpacked, int8_t* c_char, int16_t* c_short, float* c_float, uint32_t len)
{

  int i = 0;

#ifdef LV_HAVE_SSE
  __m128i* sse_c       = (__m128i*)c_unpacked;
  __m128i* sse_c_char  = (__m128i*)c_char;
  __m128i* sse_c_short = (__m128i*)c_short;
  float*   sse_c_float = c_float;

  if (len >= 16) {
    for (; i < len - 15; i += 16) {
      // Get bit mask
      __m128i m8 = _mm_cmpgt_epi8(_mm_load_si128(sse_c), _mm_set1_epi8(0));
      sse_c++;

      // Generate blend masks
      __m128i m16_1 = _mm_unpacklo_epi8(m8, m8);
      __m128i m16_2 = _mm_unpackhi_epi8(m8, m8);
      __m128  m32_1 = (__m128)_mm_unpacklo_epi8(m16_1, m16_1);
      __m128  m32_2 = (__m128)_mm_unpackhi_epi8(m16_1, m16_1);
      __m128  m32_3 = (__m128)_mm_unpacklo_epi8(m16_2, m16_2);
      __m128  m32_4 = (__m128)_mm_unpackhi_epi8(m16_2, m16_2);

      // Generate int8 values
      const __m128i bp = _mm_set1_epi8(+1);
      const __m128i bn = _mm_set1_epi8(-1);
      _mm_storeu_si128(sse_c_char, _mm_blendv_epi8(bp, bn, m8));
      sse_c_char++;

      // Generate int16 values
      const __m128i sp = _mm_set1_epi16(+1);
      const __m128i sn = _mm_set1_epi16(-1);
      _mm_store_si128(sse_c_short++, _mm_blendv_epi8(sp, sn, m16_1));
      _mm_store_si128(sse_c_short++, _mm_blendv_epi8(sp, sn, m16_2));

      // Generate float values
      const __m128 fp = _mm_set1_ps(+1);
      const __m128 fn = _mm_set1_ps(-1);
      _mm_store_ps(sse_c_float, _mm_blendv_ps(fp, fn, (__m128)m32_1));
      sse_c_float += 4;
      _mm_store_ps(sse_c_float, _mm_blendv_ps(fp, fn, (__m128)m32_2));
      sse_c_float += 4;
      _mm_store_ps(sse_c_float, _mm_blendv_ps(fp, fn, (__m128)m32_3));
      sse_c_float += 4;
      _mm_store_ps(sse_c_float, _mm_blendv_ps(fp, fn, (__m128)m32_4));
      sse_c_float += 4;
    }
  }
#endif /* LV_HAVE_SSE */

  for (; i < len; i++) {
    // Load signed
    int8_t tt = (int8_t)(c_unpacked[i] ? -1 : +1);

    // TYpecast conversion for each type
    c_char[i]  = tt;
    c_short[i] = (int16_t)tt;
    c_float[i] = (float)tt;
  }
}

int srslte_sequence_LTE_pr(srslte_sequence_t* q, uint32_t len, uint32_t seed)
{
  if (srslte_sequence_init(q, len)) {
    return SRSLTE_ERROR;
  }
  q->cur_len = len;

  // Generate sequence
  srslte_sequence_set_LTE_pr(q, len, seed);

  // Pack PR sequence
  srslte_bit_pack_vector(q->c, q->c_bytes, len);

  // Generate signed type values
  sequence_generate_signed(q->c, q->c_char, q->c_short, q->c_float, len);

  return SRSLTE_SUCCESS;
}

int srslte_sequence_init(srslte_sequence_t* q, uint32_t len)
{
  if (q->c && len > q->max_len) {
    srslte_sequence_free(q);
  }
  if (!q->c) {
    q->c = srslte_vec_u8_malloc(len);
    if (!q->c) {
      return SRSLTE_ERROR;
    }
    q->c_bytes = srslte_vec_u8_malloc(len / 8 + 8);
    if (!q->c_bytes) {
      return SRSLTE_ERROR;
    }
    q->c_float = srslte_vec_f_malloc(len);
    if (!q->c_float) {
      return SRSLTE_ERROR;
    }
    q->c_short = srslte_vec_i16_malloc(len);
    if (!q->c_short) {
      return SRSLTE_ERROR;
    }
    q->c_char = srslte_vec_i8_malloc(len);
    if (!q->c_char) {
      return SRSLTE_ERROR;
    }
    q->max_len = len;
  }
  return SRSLTE_SUCCESS;
}

void srslte_sequence_free(srslte_sequence_t* q)
{
  if (q->c) {
    free(q->c);
  }
  if (q->c_bytes) {
    free(q->c_bytes);
  }
  if (q->c_float) {
    free(q->c_float);
  }
  if (q->c_short) {
    free(q->c_short);
  }
  if (q->c_char) {
    free(q->c_char);
  }
  bzero(q, sizeof(srslte_sequence_t));
}

void srslte_sequence_apply_f(const float* in, float* out, uint32_t length, uint32_t seed)
{
  uint32_t x1 = sequence_x1_init;           // X1 initial state is fix
  uint32_t x2 = sequence_get_x2_init(seed); // loads x2 initial state

  uint32_t i = 0;

  if (length >= SEQUENCE_PAR_BITS) {
    for (; i < length - (SEQUENCE_PAR_BITS - 1); i += SEQUENCE_PAR_BITS) {
      uint32_t c = (uint32_t)(x1 ^ x2);

      uint32_t j = 0;
#ifdef LV_HAVE_SSE
      for (; j < SEQUENCE_PAR_BITS - 3; j += 4) {
        // Preloads bits of interest in the 4 LSB
        __m128i mask = _mm_set1_epi32(c >> j);

        // Masks each bit
        mask = _mm_and_si128(mask, _mm_setr_epi32(1, 2, 4, 8));

        // Get non zero mask
        mask = _mm_cmpgt_epi32(mask, _mm_set1_epi32(0));

        // And with MSB
        mask = _mm_and_si128(mask, (__m128i)_mm_set1_ps(-0.0F));

        // Load input
        __m128 v = _mm_loadu_ps(in + i + j);

        // Loads input and perform sign XOR
        v = _mm_xor_ps((__m128)mask, v);

        _mm_storeu_ps(out + i + j, v);
      }
#endif
      for (; j < SEQUENCE_PAR_BITS; j++) {
        ((uint32_t*)out)[i + j] = ((uint32_t*)in)[i] ^ (((c >> j) & 1U) << 31U);
      }

      // Step sequences
      x1 = sequence_gen_LTE_pr_memless_step_par_x1(x1);
      x2 = sequence_gen_LTE_pr_memless_step_par_x2(x2);
    }
  }

  for (; i < length; i++) {

    ((uint32_t*)out)[i] = ((uint32_t*)in)[i] ^ (((x1 ^ x2) & 1U) << 31U);

    // Step sequences
    x1 = sequence_gen_LTE_pr_memless_step_x1(x1);
    x2 = sequence_gen_LTE_pr_memless_step_x2(x2);
  }
}

void srslte_sequence_apply_s(const int16_t* in, int16_t* out, uint32_t length, uint32_t seed)
{
  uint32_t x1 = sequence_x1_init;           // X1 initial state is fix
  uint32_t x2 = sequence_get_x2_init(seed); // loads x2 initial state

  uint32_t i = 0;

  if (length >= SEQUENCE_PAR_BITS) {
    for (; i < length - (SEQUENCE_PAR_BITS - 1); i += SEQUENCE_PAR_BITS) {
      uint32_t c = (uint32_t)(x1 ^ x2);

      uint32_t j = 0;
#ifdef LV_HAVE_SSE
      for (; j < SEQUENCE_PAR_BITS - 7; j += 8) {
        // Preloads bits of interest in the 8 LSB
        __m128i mask = _mm_set1_epi16((c >> j) & 0xff);

        // Masks each bit
        mask = _mm_and_si128(mask, _mm_setr_epi16(0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80));

        // Get non zero mask
        mask = _mm_cmpgt_epi16(mask, _mm_set1_epi16(0));

        // Load input
        __m128i v = _mm_loadu_si128((__m128i*)(in + i + j));

        // Negate
        v = _mm_xor_si128(v, mask);

        // Add one
        mask = _mm_and_si128(mask, _mm_set1_epi16(1));
        v    = _mm_add_epi16(v, mask);

        _mm_storeu_si128((__m128i*)(out + i + j), v);
      }
#endif
      for (; j < SEQUENCE_PAR_BITS; j++) {
        out[i + j] = in[i + j] * (((c >> j) & 1U) ? -1 : +1);
      }

      // Step sequences
      x1 = sequence_gen_LTE_pr_memless_step_par_x1(x1);
      x2 = sequence_gen_LTE_pr_memless_step_par_x2(x2);
    }
  }

  for (; i < length; i++) {
    out[i] = in[i] * (((x1 ^ x2) & 1U) ? -1 : +1);

    // Step sequences
    x1 = sequence_gen_LTE_pr_memless_step_x1(x1);
    x2 = sequence_gen_LTE_pr_memless_step_x2(x2);
  }
}

void srslte_sequence_apply_c(const int8_t* in, int8_t* out, uint32_t length, uint32_t seed)
{
  uint32_t x1 = sequence_x1_init;           // X1 initial state is fix
  uint32_t x2 = sequence_get_x2_init(seed); // loads x2 initial state

  uint32_t i = 0;

  if (length >= SEQUENCE_PAR_BITS) {
    for (; i < length - (SEQUENCE_PAR_BITS - 1); i += SEQUENCE_PAR_BITS) {
      uint32_t c = (uint32_t)(x1 ^ x2);

      uint32_t j = 0;
#ifdef LV_HAVE_SSE
      if (SEQUENCE_PAR_BITS >= 16) {
        // Preloads bits of interest in the 16 LSB
        __m128i mask = _mm_set1_epi32(c);
        mask         = _mm_shuffle_epi8(mask, _mm_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1));

        // Masks each bit
        // mask = _mm_and_si128( mask, _mm_set_epi64x(0x0102040810204080, 0x0102040810204080));
        mask = _mm_and_si128(mask, _mm_set_epi64x(0x8040201008040201, 0x8040201008040201));

        // Get non zero mask
        mask = _mm_cmpeq_epi8(mask, _mm_set_epi64x(0x8040201008040201, 0x8040201008040201));

        // Load input
        __m128i v = _mm_loadu_si128((__m128i*)(in + i + j));

        // Negate
        v = _mm_xor_si128(mask, v);

        // Add one
        mask = _mm_and_si128(mask, _mm_set1_epi8(1));
        v    = _mm_add_epi8(v, mask);

        _mm_storeu_si128((__m128i*)(out + i + j), v);

        // Increment bit counter `j`
        j += 16;
      }
#endif
      for (; j < SEQUENCE_PAR_BITS; j++) {
        out[i + j] = in[i + j] * (((c >> j) & 1U) ? -1 : +1);
      }

      // Step sequences
      x1 = sequence_gen_LTE_pr_memless_step_par_x1(x1);
      x2 = sequence_gen_LTE_pr_memless_step_par_x2(x2);
    }
  }

  for (; i < length; i++) {
    out[i] = in[i] * (((x1 ^ x2) & 1U) ? -1 : +1);

    // Step sequences
    x1 = sequence_gen_LTE_pr_memless_step_x1(x1);
    x2 = sequence_gen_LTE_pr_memless_step_x2(x2);
  }
}