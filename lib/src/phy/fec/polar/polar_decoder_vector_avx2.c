/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
 * \file polar_decoder_vector_avx2.c
 * \brief Definition of the polar decoder vectorizable functions using AVX2 instructions.
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#include "../utils_avx2.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef LV_HAVE_AVX2

#include <immintrin.h>

/*!
 * \brief Bit mask to extract the Most Significant Bit (MSB).
 */
#define MSB_MASK (-128) // 0b10000000

// General remarks
// We replace bits by {0, 128} (uint8_t) or {0, -128} (int8_t)

void srsran_vec_function_f_ccc_avx2(const int8_t* x, const int8_t* y, int8_t* z, const uint16_t len)
{

  for (int i = 0; i < len; i += SRSRAN_AVX2_B_SIZE) {
    __m256i m_x = _mm256_loadu_si256((__m256i*)&x[i]);
    __m256i m_y = _mm256_loadu_si256((__m256i*)&y[i]);

    __m256i m_sign            = _mm256_sign_epi8(m_x, m_y);
    __m256i m_abs_x           = _mm256_abs_epi8(m_x);
    __m256i m_abs_y           = _mm256_abs_epi8(m_y);
    __m256i m_min_abs_x_abs_y = _mm256_min_epi8(m_abs_x, m_abs_y);
    __m256i m_z               = _mm256_sign_epi8(m_min_abs_x_abs_y, m_sign);

    _mm256_storeu_si256((__m256i*)&z[i], m_z);
  }
}

void srsran_vec_function_g_bccc_avx2(const uint8_t* b, const int8_t* x, const int8_t* y, int8_t* z, const uint16_t len)
{

  const __m256i M_1      = _mm256_set1_epi8(1);
  const __m256i M_NEG127 = _mm256_set1_epi8(-127);

  for (int i = 0; i < len; i += SRSRAN_AVX2_B_SIZE) {

    __m256i m_x = _mm256_loadu_si256((__m256i*)&x[i]);
    __m256i m_y = _mm256_loadu_si256((__m256i*)&y[i]);
    __m256i m_b = _mm256_loadu_si256((__m256i*)&b[i]);

    __m256i m_b_or_1 =
        _mm256_or_si256(m_b, M_1); // avoids m_b being 0, in which case m_sign_x = 0 (in the next instruction)
    __m256i m_sign_x = _mm256_sign_epi8(m_x, m_b_or_1);
    __m256i m_z      = _mm256_adds_epi8(m_sign_x, m_y);
    __m256i m_sz     = _mm256_max_epi8(M_NEG127, m_z);

    _mm256_storeu_si256((__m256i*)&z[i], m_sz);
  }
}

void srsran_vec_xor_bbb_avx2(const uint8_t* x, const uint8_t* y, uint8_t* z, uint16_t len)
{

  for (int i = 0; i < len; i += SRSRAN_AVX2_B_SIZE) {
    __m256i m_x = _mm256_loadu_si256((__m256i*)&x[i]);
    __m256i m_y = _mm256_loadu_si256((__m256i*)&y[i]);

    __m256i m_z = _mm256_xor_si256(m_x, m_y);

    _mm256_storeu_si256((__m256i*)&z[i], m_z);
  }
}

void srsran_vec_hard_bit_cc_avx2(const int8_t* x, uint8_t* z, const uint16_t len)
{
  const __m256i M_MSB_MASK = _mm256_set1_epi8(MSB_MASK);

  for (int i = 0; i < len; i += SRSRAN_AVX2_B_SIZE) {
    __m256i m_x = _mm256_loadu_si256((__m256i*)&x[i]);

    __m256i m_z = _mm256_and_si256(m_x, M_MSB_MASK);

    _mm256_storeu_si256((__m256i*)&z[i], m_z);
  }
  // restore, by setting to 0, the memory positions between z + len and z + len + SRSRAN_AVX2_B_SIZE
  memset(z + len, 0, SRSRAN_AVX2_B_SIZE);
}

void srsran_vec_sign_to_bit_c_avx2(uint8_t* x, uint16_t len)
{
  const __m256i M_NEG1 = _mm256_set1_epi8(-1);

  int i = 0;
  for (; i < len - SRSRAN_AVX2_B_SIZE + 1; i += SRSRAN_AVX2_B_SIZE) {
    __m256i m_x = _mm256_loadu_si256((__m256i*)&x[i]);

    __m256i m_abs_x = _mm256_sign_epi8(M_NEG1, m_x);

    _mm256_storeu_si256((__m256i*)&x[i], m_abs_x);
  }

  // executed if code_size < 32, which is never the case in 5G
  for (; i < len; i++) {
    x[i] = x[i] >> 7U;
  }
}
#endif // LV_HAVE_AVX2
