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

/*!
 * \file polar_encoder_avx2.c
 * \brief Definition of the AVX2 polar encoder.
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 * 5G uses a polar encoder with maximum sizes \f$2^n\f$ with \f$n = 5,...,10\f$.
 *
 */

#include "../utils_avx2.h"
#include "srsran/phy/utils/vector.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#ifdef LV_HAVE_AVX2

#include <emmintrin.h>
#include <immintrin.h>
#include <tmmintrin.h>

/*!
 * \brief Describes an AVX2 polar encoder.
 */
struct pAVX2 {
  uint8_t code_size_log; /*!< \brief The \f$ log_2\f$ of the maximum supported number of bits of the encoder
                            input/output vector. */
  uint8_t* tmp;          /*!< \brief Pointer to a temporary buffer. */
};

void delete_polar_encoder_avx2(void* o)
{
  struct pAVX2* q = o;

  if (q->tmp) {
    free(q->tmp);
  }
  free(q);
}

void* create_polar_encoder_avx2(const uint8_t code_size_log)
{
  struct pAVX2* q = NULL; // pointer to the polar encoder instance

  // allocate memory to the polar decoder instance
  if ((q = malloc(sizeof(struct pAVX2))) == NULL) {
    return NULL;
  }

  uint16_t code_size = 1U << code_size_log;

  if (code_size_log > SRSRAN_AVX2_B_SIZE_LOG) {
    q->tmp = srsran_vec_u8_malloc(code_size);
  } else {
    q->tmp = srsran_vec_u8_malloc(SRSRAN_AVX2_B_SIZE);
  }
  if (!q->tmp) {
    free(q);
    perror("malloc");
    return NULL;
  }

  q->code_size_log = code_size_log;

  return q;
}

/*!
 * Runs, in parallel, \f$ 2^{5-stage}\f$ polar encoders of size \f$ 2^{stage} \f$ each for s=1 to 5.
 */
static inline void srsran_vec_polar_encoder_32_avx2(const uint8_t* x, uint8_t* z, uint8_t stage)
{
  const __m256i MZERO = _mm256_set1_epi8(0);

  __m256i simd_x = _mm256_loadu_si256((__m256i*)x);
  __m256i simd_y;
  switch (stage) {
    case 5:
      // in 0x21, the  2 takes zeros, and the 1 takes the second half of simd_x
      simd_y = _mm256_permute2x128_si256(simd_x, MZERO, 0x21);
      simd_x = _mm256_xor_si256(simd_x, simd_y);
    case 4:
      simd_y = _mm256_srli_si256(simd_x, 8); // move each half 8-bytes= 64
      simd_x = _mm256_xor_si256(simd_x, simd_y);
    case 3: // stage 3
      simd_y = _mm256_srli_epi64(simd_x, 32);
      simd_x = _mm256_xor_si256(simd_x, simd_y);
    case 2: // stage 2
      simd_y = _mm256_srli_epi32(simd_x, 16);
      simd_x = _mm256_xor_si256(simd_x, simd_y);
    case 1: // stage 1
      simd_y = _mm256_srli_epi16(simd_x, 8);
      simd_x = _mm256_xor_si256(simd_x, simd_y);
      _mm256_storeu_si256((__m256i*)z, simd_x);
      break;
    default:
      printf("Wrong stage = %d\n", stage);
  }
}

/*!
 * Computes \f$ z = x \oplus y \f$ elementwise with AVX2 instructions.
 */
static inline void srsran_vec_xor_bbb_avx2(const uint8_t* x, const uint8_t* y, uint8_t* z, uint16_t len)
{
  for (int i = 0; i < len; i += SRSRAN_AVX2_B_SIZE) {
    __m256i simd_x = _mm256_loadu_si256((__m256i*)&x[i]);
    __m256i simd_y = _mm256_loadu_si256((__m256i*)&y[i]);

    __m256i simd_z = _mm256_xor_si256(simd_x, simd_y);

    _mm256_storeu_si256((__m256i*)&z[i], simd_z);
  }
}

int polar_encoder_encode_avx2(void* p, const uint8_t* input, uint8_t* output, const uint8_t code_size_log)
{
  struct pAVX2* q = p;

  if (q == NULL) {
    return -1;
  }

  uint8_t* tmp = q->tmp;

  uint8_t* x = NULL;
  uint8_t* y = NULL;
  uint8_t* z = NULL;

  // load data
  uint32_t code_size = 1U << code_size_log;

  memcpy(tmp, input, code_size * sizeof(uint8_t));

  if (code_size_log > q->code_size_log) {
    printf("ERROR: max code size log %d, current code size log %d.\n", q->code_size_log, code_size_log);
    return -1;
  }

  uint32_t code_size_stage      = 0;
  uint32_t code_half_size_stage = 0;
  uint32_t num_blocks           = 0;
  uint32_t s                    = code_size_log;
  for (; s > SRSRAN_AVX2_B_SIZE_LOG; s--) {
    code_size_stage      = 1U << s;
    code_half_size_stage = 1U << (s - 1);
    num_blocks           = 1U << (code_size_log - s);

    for (uint32_t b = 0; b < num_blocks; b++) {
      x = &tmp[b * code_size_stage];
      y = x + code_half_size_stage;
      z = x;
      srsran_vec_xor_bbb_avx2(x, y, z, code_half_size_stage);
    }
  }

  uint32_t num_simd_size_blocks = 1;
  if (code_size_log > SRSRAN_AVX2_B_SIZE_LOG) {
    num_simd_size_blocks = 1U << (code_size_log - SRSRAN_AVX2_B_SIZE_LOG);
  }

  for (uint32_t b = 0; b < num_simd_size_blocks; b++) {
    x = &tmp[b * SRSRAN_AVX2_B_SIZE];
    z = x;
    srsran_vec_polar_encoder_32_avx2(x, z, s);
  }

  memcpy(output, tmp, code_size * sizeof(uint8_t));

  return 0;
}

#endif // LV_HAVE_AVX2
