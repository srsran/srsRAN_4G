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
 * \file polar_encoder_pipelined.h
 * \brief Declaration of the pipelined polar encoder.
 * \author Jesus Gomez \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 */

#ifndef POLAR_ENCODER_PIPELINED_H
#define POLAR_ENCODER_PIPELINED_H

#include <stdint.h>

/*!
 * The pipelined polar encoder "destructor": it frees all the resources allocated to the encoder.
 *
 * \param[in, out] p A pointer to the dismantled encoder.
 */
void delete_polar_encoder_pipelined(void* p);

/*!
 * Encodes the input vector into a codeword with the specified polar encoder.
 * \param[in] p A void pointer used to declare a pPIPELINED structure.
 * \param[in] input The encoder input vector.
 * \param[out] output The encoder output vector.
 * \param[in] code_size_log The \f$\log_2\f$ of the number of bits of the encoder input/output vector.
 * It can less or equal to the maximum code_size_log specified in q.code_size_log of the srsran_polar_encoder_t
 * structure \return An integer: 0 if the function executes correctly, -1 otherwise.
 */
int polar_encoder_encode_pipelined(void* p, const uint8_t* input, uint8_t* output, uint8_t code_size_log);

/*!
 * Creates a pipelined polar encoder structure of type pPIPELINED, and allocates memory for the encoding buffers.
 *
 * \param[in] code_size_log \f$\log_2\f$ of the number of bits in the codeword.
 * \return A pointer to a pPIPELINED structure if the function executes correctly, NULL otherwise.
 */
void* create_polar_encoder_pipelined(uint8_t code_size_log);

#endif // POLAR_ENCODER_PIPELINED_H
