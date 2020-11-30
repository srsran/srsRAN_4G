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

#ifndef SRSLTE_POLAR_ENCODER_H
#define SRSLTE_POLAR_ENCODER_H

#include "srslte/config.h"
#include <stdint.h>

/*!
 * Lists the different types of polar decoder.
 */
typedef enum {
  SRSLTE_POLAR_ENCODER_PIPELINED = 0, /*!< \brief Non-optimized version of the pipelined polar encoder*/
  SRSLTE_POLAR_ENCODER_AVX2      = 1, /*!< \brief SIMD implementation of the polar encoder */
} srslte_polar_encoder_type_t;

/*!
 * \brief Describes a polar encoder.
 */
typedef struct SRSLTE_API {
  void* ptr; /*!< \brief Pointer to the actual polar encoder structure. */
  int (*encode)(void*          ptr,
                const uint8_t* input,
                uint8_t*       output,
                const uint8_t  code_size_log); /*!< \brief Pointer to the encoder function. */
  void (*free)(void*);                        /*!< \brief Pointer to a "destructor". */
} srslte_polar_encoder_t;

/*!
 * Initializes all the polar encoder variables according to the given code size.
 * \param[out] q A pointer to the initialized polar encoder.
 * \param[in] polar_encoder_type Polar encoder type.
 * \param[in] code_size_log The \f$ log_2\f$ of the number of bits of the encoder input/output vector.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSLTE_API int srslte_polar_encoder_init(srslte_polar_encoder_t*     q,
                                         srslte_polar_encoder_type_t polar_encoder_type,
                                         uint8_t                     code_size_log);

/*!
 * The polar encoder "destructor": it frees all the resources.
 * \param[in, out] q A pointer to the dismantled encoder.
 */
SRSLTE_API void srslte_polar_encoder_free(srslte_polar_encoder_t* q);

/*!
 * Encodes the input vector into a codeword with the specified polar encoder.
 * \param[in] q A pointer to the desired polar encoder.
 * \param[in] input The encoder input vector.
 * \param[in] code_size_log The \f$ log_2\f$ of the number of bits of the encoder input/output vector.
 *     It cannot be larger than the maximum code_size_log specified in q.code_size_log of
 *     the srslte_polar_encoder_t structure.
 * \param[out] output The encoder output vector.
 * \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
SRSLTE_API int
srslte_polar_encoder_encode(srslte_polar_encoder_t* q, const uint8_t* input, uint8_t* output, uint8_t code_size_log);

#endif // SRSLTE_POLAR_ENCODER_H
