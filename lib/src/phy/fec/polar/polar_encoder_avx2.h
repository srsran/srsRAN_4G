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
 * \file polar_encoder_avx2.h
 * \brief Declaration of the AVX2 polar encoder.
 * \author Jesus Gomez \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#ifndef POLAR_ENCODER_AVX2_H
#define POLAR_ENCODER_AVX2_H

#include <stdint.h>

/*!
 * The AVX2 polar encoder "destructor": it frees all the resources allocated to the encoder.
 *
 * \param[in, out] p A pointer to the dismantled encoder.
 */
void delete_polar_encoder_avx2(void* p);

/*!
 * Encodes the input vector into a codeword with the specified polar encoder.
 * \param[in] p A void pointer used to declare a AVX2 polar encoder structure.
 * \param[in] input The encoder input vector.
 * \param[out] output The encoder output vector.
 * \param[in] code_size_log The \f$ log_2\f$ of the number of bits of the encoder input/output vector.
 * It can less or equal to the maximum code_size_log specified in q.code_size_log of the srslte_polar_encoder_t
 * structure \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int polar_encoder_encode_avx2(void* p, const uint8_t* input, uint8_t* output, uint8_t code_size_log);

/*!
 * Creates an AVX2 polar encoder structure of type pAVX2, and allocates memory for the encoding buffers.
 *
 * \param[in] code_size_log \f$log_2\f$ of the number of bits in the codeword.
 * \return A pointer to a pAVX2 structure if the function executes correctly, NULL otherwise.
 */
void* create_polar_encoder_avx2(uint8_t code_size_log);

#endif // POLAR_ENCODER_AVX2_H
