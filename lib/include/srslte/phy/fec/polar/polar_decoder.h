/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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
 * \file polar_decoder.h
 * \brief Declaration of the polar decoder.
 * \author Jesus Gomez (CTTC)
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 * 5G uses a polar decoder with maximum sizes \f$2^n\f$ with \f$n = 5,...,10\f$.
 *
 */

#ifndef SRSLTE_POLARDECODER_H
#define SRSLTE_POLARDECODER_H
#include "srslte/config.h"
#include <stdbool.h>
#include <stdint.h>

/*!
 * Lists the different types of polar decoder.
 */
typedef enum {
  SRSLTE_POLAR_DECODER_SSC_F = 0, /*!< \brief Floating-point Simplified Successive Cancellation (SSC) decoder. */
  SRSLTE_POLAR_DECODER_SSC_S = 1, /*!< \brief Fixed-point (16 bit) Simplified Successive Cancellation (SSC) decoder. */
  SRSLTE_POLAR_DECODER_SSC_C = 2, /*!< \brief Fixed-point (8 bit) Simplified Successive Cancellation (SSC) decoder. */
  SRSLTE_POLAR_DECODER_SSC_C_AVX2 =
      3 /*!< \brief Fixed-point (8 bit, avx2) Simplified Successive Cancellation (SSC) decoder. */
} srslte_polar_decoder_type_t;

/*!
 * \brief Describes a polar decoder.
 */
typedef struct SRSLTE_API {
  void* ptr; /*!< \brief Pointer to the actual polar decoder structure. */
  int (*decode_f)(void*        ptr,
                  const float* symbols,
                  uint8_t*     data_decoded); /*!< \brief Pointer to the decoder function (float version). */
  int (*decode_s)(void*          ptr,
                  const int16_t* symbols,
                  uint8_t*       data_decoded); /*!< \brief Pointer to the decoder function (16-bit version). */
  int (*decode_c)(void*         ptr,
                  const int8_t* symbols,
                  uint8_t*      data_decoded); /*!< \brief Pointer to the decoder function (8-bit version). */
  void (*free)(void*);                    /*!< \brief Pointer to a "destructor". */
} srslte_polar_decoder_t;

/*!
 * Initializes all the polar decoder variables according to the selected decoding
 * algorithm and the given code size.
 * \param[out] q A pointer to the initialized polar decoder.
 * \param[in] polar_decoder_type Polar decoder type.
 * \param[in] code_size_log The \f$ log_2\f$ of the number of bits of the decoder input/output vector.
 * \param[in] frozen_set A pointer to the frozen–bit set (array of indices).
 * \param[in] frozen_set_size Number of frozen bits.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSLTE_API int srslte_polar_decoder_init(srslte_polar_decoder_t*     q,
                                         srslte_polar_decoder_type_t polar_decoder_type,
                                         uint16_t                    code_size_log,
                                         uint16_t*                   frozen_set,
                                         uint16_t                    frozen_set_size);

/*!
 * The polar decoder "destructor": it frees all the resources.
 * \param[in, out] q A pointer to the dismantled decoder.
 */
SRSLTE_API void srslte_polar_decoder_free(srslte_polar_decoder_t* q);

/*!
 * Decodes the input (float) codeword with the specified polar decoder.
 * \param[in] q A pointer to the desired polar decoder.
 * \param[in] input_llr The decoder LLR input vector.
 * \param[out] data_decoded The decoder output vector.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSLTE_API int srslte_polar_decoder_decode_f(srslte_polar_decoder_t* q, const float* input_llr, uint8_t* data_decoded);

/*!
 * Decodes the input (int16_t) codeword with the specified polar decoder.
 * \param[in] q A pointer to the desired polar decoder.
 * \param[in] input_llr The decoder LLR input vector.
 * \param[out] data_decoded The decoder output vector.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSLTE_API int
srslte_polar_decoder_decode_s(srslte_polar_decoder_t* q, const int16_t* input_llr, uint8_t* data_decoded);

/*!
 * Decodes the input (int8_t) codeword with the specified polar decoder.
 * \param[in] q A pointer to the desired polar decoder.
 * \param[in] input_llr The decoder LLR input vector.
 * \param[out] data_decoded The decoder output vector.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSLTE_API int srslte_polar_decoder_decode_c(srslte_polar_decoder_t* q, const int8_t* input_llr, uint8_t* data_decoded);

#endif // SRSLTE_POLARDECODER_H
