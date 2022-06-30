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
 * \file polar_decoder_vector_avx2.h
 * \brief Declaration of the 8-bit AVX2 polar decoder vectorizable functions.
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#ifndef POLAR_VECTOR_FUNCTIONS_AVX2_H
#define POLAR_VECTOR_FUNCTIONS_AVX2_H
#include "../utils_avx2.h"
#include "srsran/config.h"
#include <stdint.h>

/*!
 * Transforms input uint8_t bits represented by {0, 128} to {0, 1} with AVX2 instructions,
 * the output must have size larger than \ref SRSRAN_AVX2_B_SIZE.
 * Specifically, the function returns 0 if x=0 and 1 if x<0, otherwise the output is not defined.
 * \param[in, out] x A pointer to a vector of uint8_t.
 * \param[in] len Length of vectors x, y and z.
 */
SRSRAN_API void srsran_vec_sign_to_bit_c_avx2(uint8_t* x, uint16_t len);

/*!
 * Computes \f$ z = sign(x) \times sign(y) \times \min(abs(x), abs(y)) \f$ elementwise
 * (box-plus operator) with AVX2 instructions,
 * the output must have size larger than \ref SRSRAN_AVX2_B_SIZE.
 * \param[in] x A pointer to a vector of int8_t.
 * \param[in] y A pointer to a vector of int8_t.
 * \param[out] z A pointer to a vector of int8_t.
 * \param[in] len Length of vectors x, y and z.
 */
SRSRAN_API void srsran_vec_function_f_ccc_avx2(const int8_t* x, const int8_t* y, int8_t* z, uint16_t len);

/*!
 * Returns \f$ z = x + y \f$ if \f$ (b = 1) \f$ and \f$ z= -x + y \f$ if \f$ (b = 0)\f$ with AVX2 instructions,
 * the output must have size larger than \ref SRSRAN_AVX2_B_SIZE.
 * \param[in] b A pointer to a vectors of uint8_t with 0's and 1's.
 * \param[in] x A pointer to a vector of int8_t.
 * \param[in] y A pointer to a vector of int8_t.
 * \param[out] z A pointer to a vector of int8_t.
 * \param[in] len Length of vectors b, x, y and z.
 */
SRSRAN_API void
srsran_vec_function_g_bccc_avx2(const uint8_t* b, const int8_t* x, const int8_t* y, int8_t* z, uint16_t len);

/*!
 * Computes \f$ z = x \oplus y \f$ elementwise with AVX2 instructions,
 * the output must have size larger than \ref SRSRAN_AVX2_B_SIZE.
 * \param[in] x A pointer to a vector of uint8_t with 0's and 1's.
 * \param[in] y A pointer to a vector of uint8_t with 0's and 1's.
 * \param[out] z A pointer to a vector of uint8_t with 0's and 1's.
 * \param[in] len Length of vectors x, y and z.
 */
SRSRAN_API void srsran_vec_xor_bbb_avx2(const uint8_t* x, const uint8_t* y, uint8_t* z, uint16_t len);

/*!
 * Returns 1 if \f$ (x < 0) \f$ and 0 if \f$ (x >= 0) \f$ with AVX2 instructions,
 * the output must have size larger that \ref SRSRAN_AVX2_B_SIZE.
 * \param[in] x A pointer to a vector of int8_t.
 * \param[out] z A pointer to a vector of uint8_t with 0's and 1's.
 * \param[in] len Length of vectors x and z.
 */
SRSRAN_API void srsran_vec_hard_bit_cc_avx2(const int8_t* x, uint8_t* z, uint16_t len);

#endif // POLAR_VECTOR_FUNCTIONS_H
