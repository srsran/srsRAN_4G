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
 * \file polar_chanalloc.h
 * \brief Declaration of the subchannel allocation block.
 * \author Jesus Gomez
 * \date 2020
 *
 * \copyright Software Radio Systems Limited
 *
 *
 */

#ifndef SRSRAN_CHANALLOC_H
#define SRSRAN_CHANALLOC_H

#include "srsran/config.h"
#include "stdint.h"

/*!
 * Allocates message bits (data + CRC) to the encoder input bit vector at the
 * positions specified in \a K_set\\PC_set, computes and allocates the PC bits and
 * zeros to the remaining positions.
 * \param[in] message A pointer to the vector with the message bits (data and CRC).
 * \param[out] input_encoder A pointer to the encoder input bit vector.
 * \param[in] N The codeword length.
 * \param[in] K Number of data + CRC bits.
 * \param[in] nPC Number of parity check (PC) bits.
 * \param[in] K_set Pointer to the indices of the encoder input vector containing.
 * \param[in] PC_set Pointer to the indices of the parity check bits.
 */
void srsran_polar_chanalloc_tx(const uint8_t*  message,
                               uint8_t*        input_encoder,
                               const uint16_t  N,
                               const uint16_t  K,
                               const uint8_t   nPC,
                               const uint16_t* K_set,
                               const uint16_t* PC_set);

/*!
 * Extracts message bits (data + CRC) from the decoder output vector
 * according to the positions specified in \a K_set\\PC_set.
 * \param[in] output_decoder A pointer to the decoder output bit vector.
 * \param[out] message A pointer to the vector with the message bits (data and CRC).
 * \param[in] K Number of data + CRC bits.
 * \param[in] nPC Number of parity check (PC) bits.
 * \param[in] K_set Pointer to the indices of the encoder input vector containing.
 * \param[in] PC_set Pointer to the indices of the parity check bits.
 */
void srsran_polar_chanalloc_rx(const uint8_t*  output_decoder,
                               uint8_t*        message,
                               const uint16_t  K,
                               const uint8_t   nPC,
                               const uint16_t* K_set,
                               const uint16_t* PC_set);

#endif // SRSRAN_CHANALLOC_H
