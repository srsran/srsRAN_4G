/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
