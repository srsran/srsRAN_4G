/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
#include "srslte/config.h"
#include <stdint.h>

/*!
 * Transforms input uint8_t bits represented by {0, 128} to {0, 1} with AVX2 instructions,
 * the output must have size larger than \ref SRSLTE_AVX2_B_SIZE.
 * Specifically, the function returns 0 if x=0 and 1 if x<0, otherwise the output is not defined.
 * \param[in, out] x A pointer to a vector of uint8_t.
 * \param[in] len Length of vectors x, y and z.
 */
SRSLTE_API void srslte_vec_sign_to_bit_c_avx2(uint8_t* x, uint16_t len);

/*!
 * Computes \f$ z = sign(x) \times sign(y) \times \min(abs(x), abs(y)) \f$ elementwise
 * (box-plus operator) with AVX2 instructions,
 * the output must have size larger than \ref SRSLTE_AVX2_B_SIZE.
 * \param[in] x A pointer to a vector of int8_t.
 * \param[in] y A pointer to a vector of int8_t.
 * \param[out] z A pointer to a vector of int8_t.
 * \param[in] len Length of vectors x, y and z.
 */
SRSLTE_API void srslte_vec_function_f_ccc_avx2(const int8_t* x, const int8_t* y, int8_t* z, uint16_t len);

/*!
 * Returns \f$ z = x + y \f$ if \f$ (b = 1) \f$ and \f$ z= -x + y \f$ if \f$ (b = 0)\f$ with AVX2 instructions,
 * the output must have size larger than \ref SRSLTE_AVX2_B_SIZE.
 * \param[in] b A pointer to a vectors of uint8_t with 0's and 1's.
 * \param[in] x A pointer to a vector of int8_t.
 * \param[in] y A pointer to a vector of int8_t.
 * \param[out] z A pointer to a vector of int8_t.
 * \param[in] len Length of vectors b, x, y and z.
 */
SRSLTE_API void
srslte_vec_function_g_bccc_avx2(const uint8_t* b, const int8_t* x, const int8_t* y, int8_t* z, uint16_t len);

/*!
 * Computes \f$ z = x \oplus y \f$ elementwise with AVX2 instructions,
 * the output must have size larger than \ref SRSLTE_AVX2_B_SIZE.
 * \param[in] x A pointer to a vector of uint8_t with 0's and 1's.
 * \param[in] y A pointer to a vector of uint8_t with 0's and 1's.
 * \param[out] z A pointer to a vector of uint8_t with 0's and 1's.
 * \param[in] len Length of vectors x, y and z.
 */
SRSLTE_API void srslte_vec_xor_bbb_avx2(const uint8_t* x, const uint8_t* y, uint8_t* z, uint16_t len);

/*!
 * Returns 1 if \f$ (x < 0) \f$ and 0 if \f$ (x >= 0) \f$ with AVX2 instructions,
 * the output must have size larger that \ref SRSLTE_AVX2_B_SIZE.
 * \param[in] x A pointer to a vector of int8_t.
 * \param[out] z A pointer to a vector of uint8_t with 0's and 1's.
 * \param[in] len Length of vectors x and z.
 */
SRSLTE_API void srslte_vec_hard_bit_cc_avx2(const int8_t* x, uint8_t* z, uint16_t len);

#endif // POLAR_VECTOR_FUNCTIONS_H
