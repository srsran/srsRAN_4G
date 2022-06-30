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
 * \file utils_avx512.h
 * \brief Declarations of AVX512-related quantities and functions.
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#ifndef SRSRAN_UTILS_AVX512_H
#define SRSRAN_UTILS_AVX512_H

#define SRSRAN_AVX512_B_SIZE 64    /*!< \brief Number of packed bytes in an AVX512 instruction. */
#define SRSRAN_AVX512_B_SIZE_LOG 6 /*!< \brief \f$\log_2\f$ of \ref SRSRAN_AVX512_B_SIZE. */

#ifdef LV_HAVE_AVX512

#include <immintrin.h>

static inline void fec_avx512_hard_decision_c(const int8_t* llr, uint8_t* message, int nof_llr)
{
  int k = 0;
  for (; k < nof_llr - (SRSRAN_AVX512_B_SIZE - 1); k += SRSRAN_AVX512_B_SIZE) {
    __mmask64 mask = _mm512_cmpge_epi8_mask(_mm512_load_si512((__m512i*)&llr[k]), _mm512_set1_epi8(0));
    _mm512_storeu_si512((__m512i*)&message[k], _mm512_mask_blend_epi8(mask, _mm512_set1_epi8(1), _mm512_set1_epi8(0)));
  }
  for (; k < nof_llr; k++) {
    message[k] = (llr[k] < 0);
  }
}
#endif // LV_HAVE_AVX512

#endif // SRSRAN_UTILS_AVX512_H
