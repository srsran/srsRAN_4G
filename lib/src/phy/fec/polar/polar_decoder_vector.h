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
 * \file polar_decoder_vector.h
 * \brief Declaration of the polar decoder vectorizable functions.
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#ifndef POLAR_VECTOR_FUNCTIONS_H
#define POLAR_VECTOR_FUNCTIONS_H
#include "srsran/config.h"
#include <stdint.h>

/*!
 * Computes \f$ z = sign(x) \times sign(y) \times \min(abs(x), abs(y)) \f$ elementwise (box-plus operator).
 * \param[in] x A pointer to a vector of floats.
 * \param[in] y A pointer to a vector of floats.
 * \param[out] z A pointer to a vector of floats.
 * \param[in] len Length of vectors x, y and z.
 */
SRSRAN_API void srsran_vec_function_f_fff(const float* x, const float* y, float* z, uint16_t len);

/*!
 * Computes \f$ z = sign(x) \times sign(y) \times \min(abs(x), abs(y)) \f$ elementwise (box-plus operator).
 * \param[in] x A pointer to a vector of int16_t.
 * \param[in] y A pointer to a vector of int16_t.
 * \param[out] z A pointer to a vector of int16_t.
 * \param[in] len Length of vectors x, y and z.
 */
SRSRAN_API void srsran_vec_function_f_sss(const int16_t* x, const int16_t* y, int16_t* z, uint16_t len);

/*!
 * Computes \f$ z = sign(x) \times sign(y) \times \min(abs(x), abs(y)) \f$ elementwise (box-plus operator).
 * \param[in] x A pointer to a vector of int8_t.
 * \param[in] y A pointer to a vector of int8_t.
 * \param[out] z A pointer to a vector of int8_t.
 * \param[in] len Length of vectors x, y and z.
 */
SRSRAN_API void srsran_vec_function_f_ccc(const int8_t* x, const int8_t* y, int8_t* z, uint16_t len);

/*!
 * Returns \f$ z = x + y \f$ if \f$ (b = 1) \f$ and \f$ z= -x + y \f$ if \f$ (b = 0)\f$.
 * \param[in] b A pointer to a vectors of uint8_t with 0's and 1's.
 * \param[in] x A pointer to a vector of floats.
 * \param[in] y A pointer to a vector of floats.
 * \param[out] z A pointer to a vector of floats.
 * \param[in] len Length of vectors b, x, y and z.
 */
SRSRAN_API void srsran_vec_function_g_bfff(const uint8_t* b, const float* x, const float* y, float* z, uint16_t len);

/*!
 * Returns \f$ z = x + y \f$ if \f$ (b = 1) \f$ and \f$ z= -x + y \f$ if \f$ (b = 0)\f$.
 * \param[in] b A pointer to a vectors of uint8_t with 0's and 1's.
 * \param[in] x A pointer to a vector of int16_t.
 * \param[in] y A pointer to a vector of int16_t.
 * \param[out] z A pointer to a vector of int16_t.
 * \param[in] len Length of vectors b, x, y and z.
 */
SRSRAN_API void
srsran_vec_function_g_bsss(const uint8_t* b, const int16_t* x, const int16_t* y, int16_t* z, uint16_t len);

/*!
 * Returns \f$ z = x + y \f$ if \f$ (b = 1) \f$ and \f$ z= -x + y \f$ if \f$ (b = 0)\f$.
 * \param[in] b A pointer to a vectors of uint8_t with 0's and 1's.
 * \param[in] x A pointer to a vector of int8_t.
 * \param[in] y A pointer to a vector of int8_t.
 * \param[out] z A pointer to a vector of int8_t.
 * \param[in] len Length of vectors b, x, y and z.
 */
SRSRAN_API void srsran_vec_function_g_bccc(const uint8_t* b, const int8_t* x, const int8_t* y, int8_t* z, uint16_t len);

/*!
 * Returns 1 if \f$ (x < 0) \f$ and 0 if \f$ (x >= 0) \f$.
 * \param[in] x A pointer to a vector of floats.
 * \param[out] z A pointer to a vector of uint8_t with 0's and 1's.
 * \param[in] len Length of vectors x and z.
 */
SRSRAN_API void srsran_vec_hard_bit_fc(const float* x, uint8_t* z, uint16_t len);

/*!
 * Returns 1 if \f$ (x < 0) \f$ and 0 if \f$ (x >= 0) \f$.
 * \param[in] x A pointer to a vector of int16_t.
 * \param[out] z A pointer to a vector of uint8_t with 0's and 1's.
 * \param[in] len Length of vectors x and z.
 */
SRSRAN_API void srsran_vec_hard_bit_sc(const int16_t* x, uint8_t* z, uint16_t len);

/*!
 * Returns 1 if \f$ (x < 0) \f$ and 0 if \f$ (x >= 0) \f$.
 * \param[in] x A pointer to a vector of int8_t.
 * \param[out] z A pointer to a vector of uint8_t with 0's and 1's.
 * \param[in] len Length of vectors x and z.
 */
SRSRAN_API void srsran_vec_hard_bit_cc(const int8_t* x, uint8_t* z, uint16_t len);

#endif // POLAR_VECTOR_FUNCTIONS_H
