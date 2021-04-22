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
 * \file ldpc_avx512_consts.h
 * \brief Declaration of constants and masks for the AVX512-based implementation
 *   of the LDPC encoder and decoder.
 *
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#ifndef LDPC_AVX512_CONSTS_H
#define LDPC_AVX512_CONSTS_H

#include <immintrin.h>

#include "../utils_avx512.h"

/*!
 * \brief Packed 8-bit zeros.
 */
static const __m512i _mm512_zero_epi8 = {0, 0, 0, 0, 0, 0, 0, 0}; // VECTOR OF 8 0's, is each occupying 64 bits

/*!
 * \brief Packed 8-bit ones.  // there are 16 x 4 bits = 64 for a LongLong (LL)
 */
static const __m512i _mm512_one_epi8 = {0x0101010101010101LL,
                                        0x0101010101010101LL,
                                        0x0101010101010101LL,
                                        0x0101010101010101LL,
                                        0x0101010101010101LL,
                                        0x0101010101010101LL,
                                        0x0101010101010101LL,
                                        0x0101010101010101LL};

/*!
 * \brief Packed 8-bit 127 (that is \f$2^7 - 1\f$, or 0111 1111).
 */
static const __m512i _mm512_infty8_epi8 = {0x7F7F7F7F7F7F7F7FLL,
                                           0x7F7F7F7F7F7F7F7FLL,
                                           0x7F7F7F7F7F7F7F7FLL,
                                           0x7F7F7F7F7F7F7F7FLL,
                                           0x7F7F7F7F7F7F7F7FLL,
                                           0x7F7F7F7F7F7F7F7FLL,
                                           0x7F7F7F7F7F7F7F7FLL,
                                           0x7F7F7F7F7F7F7F7FLL};
/*!
 * \brief Packed 8-bit --127 (that is \f$-2^7 + 1\f$, i.e. 1000 0001).
 */
static const __m512i _mm512_neg_infty8_epi8 = {0x8181818181818181LL,  // NOLINT
                                               0x8181818181818181LL,  // NOLINT
                                               0x8181818181818181LL,  // NOLINT
                                               0x8181818181818181LL,  // NOLINT
                                               0x8181818181818181LL,  // NOLINT
                                               0x8181818181818181LL,  // NOLINT
                                               0x8181818181818181LL,  // NOLINT
                                               0x8181818181818181LL}; // NOLINT

/*!
 * \brief Packed 8-bit 63 (that is \f$2^6 - 1\f$).
 */
static const __m512i _mm512_infty7_epi8 = {0x3F3F3F3F3F3F3F3FLL,
                                           0x3F3F3F3F3F3F3F3FLL,
                                           0x3F3F3F3F3F3F3F3FLL,
                                           0x3F3F3F3F3F3F3F3FLL,
                                           0x3F3F3F3F3F3F3F3FLL,
                                           0x3F3F3F3F3F3F3F3FLL,
                                           0x3F3F3F3F3F3F3F3FLL,
                                           0x3F3F3F3F3F3F3F3FLL};
/*!
 * \brief Packed 8-bit --63 (that is \f$-2^6 + 1\f$).
 */
static const __m512i _mm512_neg_infty7_epi8 = {0xC1C1C1C1C1C1C1C1LL,  // NOLINT
                                               0xC1C1C1C1C1C1C1C1LL,  // NOLINT
                                               0xC1C1C1C1C1C1C1C1LL,  // NOLINT
                                               0xC1C1C1C1C1C1C1C1LL,  // NOLINT
                                               0xC1C1C1C1C1C1C1C1LL,  // NOLINT
                                               0xC1C1C1C1C1C1C1C1LL,  // NOLINT
                                               0xC1C1C1C1C1C1C1C1LL,  // NOLINT
                                               0xC1C1C1C1C1C1C1C1LL}; // NOLINT

/*!
 * \brief Identifies even-indexed 8-bit packets.
 */
static const __m512i _mm512_mask_even_epi8 = {0x00FF00FF00FF00FF,
                                              0x00FF00FF00FF00FF,
                                              0x00FF00FF00FF00FF,
                                              0x00FF00FF00FF00FF,
                                              0x00FF00FF00FF00FF,
                                              0x00FF00FF00FF00FF,
                                              0x00FF00FF00FF00FF,
                                              0x00FF00FF00FF00FF}; // NOLINT
#endif
