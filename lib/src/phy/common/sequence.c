/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/phy/common/sequence.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

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
 * ahead and the maximum register shift is 3 (for x2). The maximum number of parallel bits is 28, 16 is optimal for SSE.
 */
#define SEQUENCE_PAR_BITS (24U)
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
__attribute__((constructor)) __attribute__((unused)) static void srsran_lte_pr_pregen()
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

void srsran_sequence_state_init(srsran_sequence_state_t* s, uint32_t seed)
{
  s->x1 = sequence_x1_init;
  s->x2 = sequence_get_x2_init(seed);
}

#define FLOAT_U32_XOR(DST, SRC, U32_MASK)                                                                              \
  do {                                                                                                                 \
    uint32_t temp_u32;                                                                                                 \
    memcpy(&temp_u32, &(SRC), 4);                                                                                      \
    temp_u32 ^= (U32_MASK);                                                                                            \
    memcpy(&(DST), &temp_u32, 4);                                                                                      \
  } while (false)

void srsran_sequence_state_gen_f(srsran_sequence_state_t* s, float value, float* out, uint32_t length)
{
  uint32_t i = 0;

  if (length >= SEQUENCE_PAR_BITS) {
    for (; i < length - (SEQUENCE_PAR_BITS - 1); i += SEQUENCE_PAR_BITS) {
      uint32_t c = (uint32_t)(s->x1 ^ s->x2);

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
        __m128 v = _mm_set1_ps(value);

        // Loads input and perform sign XOR
        v = _mm_xor_ps((__m128)mask, v);

        _mm_storeu_ps(out + i + j, v);
      }
#endif
      // Finish the parallel bits with generic code
      for (; j < SEQUENCE_PAR_BITS; j++) {
        FLOAT_U32_XOR(out[i + j], value, (c << (31U - j)) & 0x80000000);
      }

      // Step sequences
      s->x1 = sequence_gen_LTE_pr_memless_step_par_x1(s->x1);
      s->x2 = sequence_gen_LTE_pr_memless_step_par_x2(s->x2);
    }
  }

  for (; i < length; i++) {
    FLOAT_U32_XOR(out[i], value, (s->x1 ^ s->x2) << 31U);

    // Step sequences
    s->x1 = sequence_gen_LTE_pr_memless_step_x1(s->x1);
    s->x2 = sequence_gen_LTE_pr_memless_step_x2(s->x2);
  }
}

void srsran_sequence_state_apply_f(srsran_sequence_state_t* s, const float* in, float* out, uint32_t length)
{
  uint32_t i = 0;

  if (length >= SEQUENCE_PAR_BITS) {
    for (; i < length - (SEQUENCE_PAR_BITS - 1); i += SEQUENCE_PAR_BITS) {
      uint32_t c = (uint32_t)(s->x1 ^ s->x2);

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
#endif // LV_HAVE_SSE
      // Finish the parallel bits with generic code
      for (; j < SEQUENCE_PAR_BITS; j++) {
        FLOAT_U32_XOR(out[i + j], in[i + j], (c << (31U - j)) & 0x80000000);
      }

      // Step sequences
      s->x1 = sequence_gen_LTE_pr_memless_step_par_x1(s->x1);
      s->x2 = sequence_gen_LTE_pr_memless_step_par_x2(s->x2);
    }
  }

  for (; i < length; i++) {
    FLOAT_U32_XOR(out[i], in[i], (s->x1 ^ s->x2) << 31U);

    // Step sequences
    s->x1 = sequence_gen_LTE_pr_memless_step_x1(s->x1);
    s->x2 = sequence_gen_LTE_pr_memless_step_x2(s->x2);
  }
}

void srsran_sequence_state_advance(srsran_sequence_state_t* s, uint32_t length)
{
  uint32_t i = 0;
  if (length >= SEQUENCE_PAR_BITS) {
    for (; i < length - (SEQUENCE_PAR_BITS - 1); i += SEQUENCE_PAR_BITS) {
      // Step sequences
      s->x1 = sequence_gen_LTE_pr_memless_step_par_x1(s->x1);
      s->x2 = sequence_gen_LTE_pr_memless_step_par_x2(s->x2);
    }
  }

  for (; i < length; i++) {
    // Step sequences
    s->x1 = sequence_gen_LTE_pr_memless_step_x1(s->x1);
    s->x2 = sequence_gen_LTE_pr_memless_step_x2(s->x2);
  }
}

// static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int srsran_sequence_set_LTE_pr(srsran_sequence_t* q, uint32_t len, uint32_t seed)
{
  if (len > q->max_len) {
    ERROR("Error generating pseudo-random sequence: len %d is greater than allocated len %d", len, q->max_len);
    return SRSRAN_ERROR;
  }

  sequence_gen_LTE_pr(q->c, len, seed);

  return SRSRAN_SUCCESS;
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

int srsran_sequence_LTE_pr(srsran_sequence_t* q, uint32_t len, uint32_t seed)
{
  if (srsran_sequence_init(q, len)) {
    return SRSRAN_ERROR;
  }
  q->cur_len = len;

  // Generate sequence
  srsran_sequence_set_LTE_pr(q, len, seed);

  // Pack PR sequence
  srsran_bit_pack_vector(q->c, q->c_bytes, len);

  // Generate signed type values
  sequence_generate_signed(q->c, q->c_char, q->c_short, q->c_float, len);

  return SRSRAN_SUCCESS;
}

int srsran_sequence_init(srsran_sequence_t* q, uint32_t len)
{
  if (q->c && len > q->max_len) {
    srsran_sequence_free(q);
  }
  if (!q->c) {
    q->c = srsran_vec_u8_malloc(len);
    if (!q->c) {
      return SRSRAN_ERROR;
    }
    q->c_bytes = srsran_vec_u8_malloc(len / 8 + 8);
    if (!q->c_bytes) {
      return SRSRAN_ERROR;
    }
    q->c_float = srsran_vec_f_malloc(len);
    if (!q->c_float) {
      return SRSRAN_ERROR;
    }
    q->c_short = srsran_vec_i16_malloc(len);
    if (!q->c_short) {
      return SRSRAN_ERROR;
    }
    q->c_char = srsran_vec_i8_malloc(len);
    if (!q->c_char) {
      return SRSRAN_ERROR;
    }
    q->max_len = len;
  }
  return SRSRAN_SUCCESS;
}

void srsran_sequence_free(srsran_sequence_t* q)
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
  bzero(q, sizeof(srsran_sequence_t));
}

void srsran_sequence_apply_f(const float* in, float* out, uint32_t length, uint32_t seed)
{
  srsran_sequence_state_t seq = {};
  srsran_sequence_state_init(&seq, seed);

  srsran_sequence_state_apply_f(&seq, in, out, length);
}

void srsran_sequence_apply_s(const int16_t* in, int16_t* out, uint32_t length, uint32_t seed)
{
  const int16_t s[2] = {+1, -1};
  uint32_t      x1   = sequence_x1_init;           // X1 initial state is fix
  uint32_t      x2   = sequence_get_x2_init(seed); // loads x2 initial state

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
#endif // LV_HAVE_SSE
      for (; j < SEQUENCE_PAR_BITS; j++) {
        out[i + j] = in[i + j] * s[(c >> j) & 1U];
      }

      // Step sequences
      x1 = sequence_gen_LTE_pr_memless_step_par_x1(x1);
      x2 = sequence_gen_LTE_pr_memless_step_par_x2(x2);
    }
  }

  for (; i < length; i++) {
    out[i] = in[i] * s[(x1 ^ x2) & 1U];

    // Step sequences
    x1 = sequence_gen_LTE_pr_memless_step_x1(x1);
    x2 = sequence_gen_LTE_pr_memless_step_x2(x2);
  }
}

void srsran_sequence_state_apply_c(srsran_sequence_state_t* s, const int8_t* in, int8_t* out, uint32_t length)
{
  uint32_t i = 0;

  if (length >= SEQUENCE_PAR_BITS) {
    for (; i < length - (SEQUENCE_PAR_BITS - 1); i += SEQUENCE_PAR_BITS) {
      uint32_t c = (uint32_t)(s->x1 ^ s->x2);

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
      s->x1 = sequence_gen_LTE_pr_memless_step_par_x1(s->x1);
      s->x2 = sequence_gen_LTE_pr_memless_step_par_x2(s->x2);
    }
  }

  for (; i < length; i++) {
    out[i] = in[i] * (((s->x1 ^ s->x2) & 1U) ? -1 : +1);

    // Step sequences
    s->x1 = sequence_gen_LTE_pr_memless_step_x1(s->x1);
    s->x2 = sequence_gen_LTE_pr_memless_step_x2(s->x2);
  }
}

void srsran_sequence_apply_c(const int8_t* in, int8_t* out, uint32_t length, uint32_t seed)
{
  srsran_sequence_state_t sequence_state;
  srsran_sequence_state_init(&sequence_state, seed);
  srsran_sequence_state_apply_c(&sequence_state, in, out, length);
}

void srsran_sequence_state_apply_bit(srsran_sequence_state_t* s, const uint8_t* in, uint8_t* out, uint32_t length)
{
  uint32_t i = 0;

  if (length >= SEQUENCE_PAR_BITS) {
    for (; i < length - (SEQUENCE_PAR_BITS - 1); i += SEQUENCE_PAR_BITS) {
      uint32_t c = (uint32_t)(s->x1 ^ s->x2);

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

        // Reduce to 1s and 0s
        mask = _mm_and_si128(mask, _mm_set1_epi8(1));

        // Load input
        __m128i v = _mm_loadu_si128((__m128i*)(in + i + j));

        // Apply XOR
        v = _mm_xor_si128(mask, v);

        _mm_storeu_si128((__m128i*)(out + i + j), v);

        // Increment bit counter `j`
        j += 16;
      }
#endif
      for (; j < SEQUENCE_PAR_BITS; j++) {
        out[i + j] = in[i + j] ^ ((c >> j) & 1U);
      }

      // Step sequences
      s->x1 = sequence_gen_LTE_pr_memless_step_par_x1(s->x1);
      s->x2 = sequence_gen_LTE_pr_memless_step_par_x2(s->x2);
    }
  }

  for (; i < length; i++) {
    out[i] = in[i] ^ ((s->x1 ^ s->x2) & 1U);

    // Step sequences
    s->x1 = sequence_gen_LTE_pr_memless_step_x1(s->x1);
    s->x2 = sequence_gen_LTE_pr_memless_step_x2(s->x2);
  }
}

void srsran_sequence_apply_bit(const uint8_t* in, uint8_t* out, uint32_t length, uint32_t seed)
{
  srsran_sequence_state_t sequence_state = {};
  srsran_sequence_state_init(&sequence_state, seed);
  srsran_sequence_state_apply_bit(&sequence_state, in, out, length);
}

void srsran_sequence_apply_packed(const uint8_t* in, uint8_t* out, uint32_t length, uint32_t seed)
{
  uint32_t x1 = sequence_x1_init;           // X1 initial state is fix
  uint32_t x2 = sequence_get_x2_init(seed); // loads x2 initial state

  const uint8_t reverse_lut[256] = {
      0b00000000, 0b10000000, 0b01000000, 0b11000000, 0b00100000, 0b10100000, 0b01100000, 0b11100000, 0b00010000,
      0b10010000, 0b01010000, 0b11010000, 0b00110000, 0b10110000, 0b01110000, 0b11110000, 0b00001000, 0b10001000,
      0b01001000, 0b11001000, 0b00101000, 0b10101000, 0b01101000, 0b11101000, 0b00011000, 0b10011000, 0b01011000,
      0b11011000, 0b00111000, 0b10111000, 0b01111000, 0b11111000, 0b00000100, 0b10000100, 0b01000100, 0b11000100,
      0b00100100, 0b10100100, 0b01100100, 0b11100100, 0b00010100, 0b10010100, 0b01010100, 0b11010100, 0b00110100,
      0b10110100, 0b01110100, 0b11110100, 0b00001100, 0b10001100, 0b01001100, 0b11001100, 0b00101100, 0b10101100,
      0b01101100, 0b11101100, 0b00011100, 0b10011100, 0b01011100, 0b11011100, 0b00111100, 0b10111100, 0b01111100,
      0b11111100, 0b00000010, 0b10000010, 0b01000010, 0b11000010, 0b00100010, 0b10100010, 0b01100010, 0b11100010,
      0b00010010, 0b10010010, 0b01010010, 0b11010010, 0b00110010, 0b10110010, 0b01110010, 0b11110010, 0b00001010,
      0b10001010, 0b01001010, 0b11001010, 0b00101010, 0b10101010, 0b01101010, 0b11101010, 0b00011010, 0b10011010,
      0b01011010, 0b11011010, 0b00111010, 0b10111010, 0b01111010, 0b11111010, 0b00000110, 0b10000110, 0b01000110,
      0b11000110, 0b00100110, 0b10100110, 0b01100110, 0b11100110, 0b00010110, 0b10010110, 0b01010110, 0b11010110,
      0b00110110, 0b10110110, 0b01110110, 0b11110110, 0b00001110, 0b10001110, 0b01001110, 0b11001110, 0b00101110,
      0b10101110, 0b01101110, 0b11101110, 0b00011110, 0b10011110, 0b01011110, 0b11011110, 0b00111110, 0b10111110,
      0b01111110, 0b11111110, 0b00000001, 0b10000001, 0b01000001, 0b11000001, 0b00100001, 0b10100001, 0b01100001,
      0b11100001, 0b00010001, 0b10010001, 0b01010001, 0b11010001, 0b00110001, 0b10110001, 0b01110001, 0b11110001,
      0b00001001, 0b10001001, 0b01001001, 0b11001001, 0b00101001, 0b10101001, 0b01101001, 0b11101001, 0b00011001,
      0b10011001, 0b01011001, 0b11011001, 0b00111001, 0b10111001, 0b01111001, 0b11111001, 0b00000101, 0b10000101,
      0b01000101, 0b11000101, 0b00100101, 0b10100101, 0b01100101, 0b11100101, 0b00010101, 0b10010101, 0b01010101,
      0b11010101, 0b00110101, 0b10110101, 0b01110101, 0b11110101, 0b00001101, 0b10001101, 0b01001101, 0b11001101,
      0b00101101, 0b10101101, 0b01101101, 0b11101101, 0b00011101, 0b10011101, 0b01011101, 0b11011101, 0b00111101,
      0b10111101, 0b01111101, 0b11111101, 0b00000011, 0b10000011, 0b01000011, 0b11000011, 0b00100011, 0b10100011,
      0b01100011, 0b11100011, 0b00010011, 0b10010011, 0b01010011, 0b11010011, 0b00110011, 0b10110011, 0b01110011,
      0b11110011, 0b00001011, 0b10001011, 0b01001011, 0b11001011, 0b00101011, 0b10101011, 0b01101011, 0b11101011,
      0b00011011, 0b10011011, 0b01011011, 0b11011011, 0b00111011, 0b10111011, 0b01111011, 0b11111011, 0b00000111,
      0b10000111, 0b01000111, 0b11000111, 0b00100111, 0b10100111, 0b01100111, 0b11100111, 0b00010111, 0b10010111,
      0b01010111, 0b11010111, 0b00110111, 0b10110111, 0b01110111, 0b11110111, 0b00001111, 0b10001111, 0b01001111,
      0b11001111, 0b00101111, 0b10101111, 0b01101111, 0b11101111, 0b00011111, 0b10011111, 0b01011111, 0b11011111,
      0b00111111, 0b10111111, 0b01111111, 0b11111111,
  };

  uint32_t i = 0;
#if SEQUENCE_PAR_BITS % 8 != 0
  uint64_t buffer = 0;
  uint32_t count  = 0;

  for (; i < length / 8; i++) {
    // Generate sequence bits
    while (count < 8) {
      uint32_t c = (uint32_t)(x1 ^ x2);
      buffer     = buffer | ((SEQUENCE_MASK & c) << count);

      // Step sequences
      x1 = sequence_gen_LTE_pr_memless_step_par_x1(x1);
      x2 = sequence_gen_LTE_pr_memless_step_par_x2(x2);

      // Increase count
      count += SEQUENCE_PAR_BITS;
    }

    // Apply XOR
    out[i] = in[i] ^ reverse_lut[buffer & 255UL];
    buffer = buffer >> 8UL;
    count -= 8;
  }

  // Process spare bits
  uint32_t rem8 = length % 8;
  if (rem8 != 0) {
    // Generate sequence bits
    while (count < rem8) {
      uint32_t c = (uint32_t)(x1 ^ x2);
      buffer     = buffer | ((SEQUENCE_MASK & c) << count);

      // Step sequences
      x1 = sequence_gen_LTE_pr_memless_step_par_x1(x1);
      x2 = sequence_gen_LTE_pr_memless_step_par_x2(x2);

      // Increase count
      count += SEQUENCE_PAR_BITS;
    }

    out[i] = in[i] ^ reverse_lut[buffer & ((1U << rem8) - 1U) & 255U];
  }
#else  // SEQUENCE_PAR_BITS % 8 == 0
  while (i < (length / 8 - (SEQUENCE_PAR_BITS - 1) / 8)) {
    uint32_t c = (uint32_t)(x1 ^ x2);

    for (uint32_t j = 0; j < SEQUENCE_PAR_BITS / 8; j++) {
      out[i] = in[i] ^ reverse_lut[c & 255U];
      c      = c >> 8U;
      i++;
    }

    // Step sequences
    x1 = sequence_gen_LTE_pr_memless_step_par_x1(x1);
    x2 = sequence_gen_LTE_pr_memless_step_par_x2(x2);
  }

  // Process spare bytes
  uint32_t c = (uint32_t)(x1 ^ x2);
  while (i < length / 8) {
    out[i] = in[i] ^ reverse_lut[c & 255U];
    c      = c >> 8U;
    i++;
  }

  // Process spare bits
  uint32_t rem8 = length % 8;
  if (rem8 != 0) {
    out[i] = in[i] ^ reverse_lut[c & ((1U << rem8) - 1U) & 255U];
  }
#endif // SEQUENCE_PAR_BITS % 8 == 0
}
