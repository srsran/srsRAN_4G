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

#ifndef SRSRAN_BLOCK_H
#define SRSRAN_BLOCK_H

#include "srsran/config.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Maximum number of bits that can be encoded
 */
#define SRSRAN_FEC_BLOCK_MAX_NOF_BITS 11U

/**
 * @brief Block coding output complete length
 */
#define SRSRAN_FEC_BLOCK_SIZE 32U

/**
 * @brief Encodes unpacked data using Reed–Muller code block channel coding.
 *
 * @remark Described by 3GPP 36.212 section 5.2.3.3 for 4G/LTE
 * @remark Described by 3GPP 38.212 section 5.3.3.3 for 5G/NR
 *
 * @param[in] input provides unpacked bits to encode
 * @param[in] input_len number of bits to encode, the maximum number of bits is SRSRAN_FEC_BLOCK_MAX_NOF_BITS
 * @param[out] output points to the unpacked encoded data
 * @param[in] output_len number of bits of encoded bits
 */
SRSRAN_API void srsran_block_encode(const uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t output_len);

/**
 * @brief Decodes 16-bit signed data using Reed–Muller code block channel coding.
 *
 * @param[in] llr Provides received LLRs
 * @param[in] nof_llr number of available LLRs
 * @param[out] data Data destination to store unpacked received bits
 * @param[in] data_len number of bits to decode, the maximum number of bits is SRSRAN_FEC_BLOCK_MAX_NOF_BITS
 * @return Decoded bits correlation if provided arguments are valid, otherwise SRSRAN_ERROR code
 */
SRSRAN_API int32_t srsran_block_decode_i16(const int16_t* llr, uint32_t nof_llr, uint8_t* data, uint32_t data_len);

/**
 * @brief Decodes 8-bit signed data using Reed–Muller code block channel coding.
 *
 * @param[in] llr Provides received LLRs
 * @param[in] nof_llr number of available LLRs
 * @param[out] data Data destination to store unpacked received bits
 * @param[in] data_len number of bits to decode, the maximum number of bits is SRSRAN_FEC_BLOCK_MAX_NOF_BITS
 * @return Decoded bits correlation if provided arguments are valid, otherwise SRSRAN_ERROR code
 */
SRSRAN_API int32_t srsran_block_decode_i8(const int8_t* llr, uint32_t nof_llr, uint8_t* data, uint32_t data_len);

#endif // SRSRAN_BLOCK_H
