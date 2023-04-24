/**
 * Copyright 2013-2023 Software Radio Systems Limited
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
 * \file polar_encoder.h
 * \brief Declaration of the polar encoder.
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 * 5G uses a polar encoder with maximum sizes \f$2^n\f$ with \f$n = 5,...,10\f$.
 *
 */

#ifndef SRSRAN_POLAR_ENCODER_H
#define SRSRAN_POLAR_ENCODER_H

#include "srsran/config.h"
#include <stdint.h>

/*!
 * Lists the different types of polar decoder.
 */
typedef enum {
  SRSRAN_POLAR_ENCODER_PIPELINED = 0, /*!< \brief Non-optimized version of the pipelined polar encoder*/
  SRSRAN_POLAR_ENCODER_AVX2      = 1, /*!< \brief SIMD implementation of the polar encoder */
} srsran_polar_encoder_type_t;

/*!
 * \brief Describes a polar encoder.
 */
typedef struct SRSRAN_API {
  void* ptr; /*!< \brief Pointer to the actual polar encoder structure. */
  int (*encode)(void*          ptr,
                const uint8_t* input,
                uint8_t*       output,
                const uint8_t  code_size_log); /*!< \brief Pointer to the encoder function. */
  void (*free)(void*);                        /*!< \brief Pointer to a "destructor". */
} srsran_polar_encoder_t;

/*!
 * Initializes all the polar encoder variables according to the given code size.
 * \param[out] q A pointer to the initialized polar encoder.
 * \param[in] polar_encoder_type Polar encoder type.
 * \param[in] code_size_log The \f$ log_2\f$ of the number of bits of the encoder input/output vector.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSRAN_API int srsran_polar_encoder_init(srsran_polar_encoder_t*     q,
                                         srsran_polar_encoder_type_t polar_encoder_type,
                                         uint8_t                     code_size_log);

/*!
 * The polar encoder "destructor": it frees all the resources.
 * \param[in, out] q A pointer to the dismantled encoder.
 */
SRSRAN_API void srsran_polar_encoder_free(srsran_polar_encoder_t* q);

/*!
 * Encodes the input vector into a codeword with the specified polar encoder.
 * \param[in] q A pointer to the desired polar encoder.
 * \param[in] input The encoder input vector.
 * \param[in] code_size_log The \f$ log_2\f$ of the number of bits of the encoder input/output vector.
 *     It cannot be larger than the maximum code_size_log specified in q.code_size_log of
 *     the srsran_polar_encoder_t structure.
 * \param[out] output The encoder output vector.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSRAN_API int
srsran_polar_encoder_encode(srsran_polar_encoder_t* q, const uint8_t* input, uint8_t* output, uint8_t code_size_log);

#endif // SRSRAN_POLAR_ENCODER_H
