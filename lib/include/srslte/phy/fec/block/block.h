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

#ifndef SRSLTE_BLOCK_H
#define SRSLTE_BLOCK_H

#include "srslte/config.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Maximum number of bits that can be encoded
 */
#define SRSLTE_FEC_BLOCK_MAX_NOF_BITS 11U

/**
 * @brief Block coding output complete length
 */
#define SRSLTE_FEC_BLOCK_SIZE 32U

/**
 * @brief Encodes unpacked data using Reed–Muller code block channel coding.
 *
 * @remark Described by 3GPP 36.212 section 5.2.3.3 for 4G/LTE
 * @remark Described by 3GPP 38.212 section 5.3.3.3 for 5G/NR
 *
 * @param[in] input provides unpacked bits to encode
 * @param[in] input_len number of bits to encode, the maximum number of bits is SRSLTE_FEC_BLOCK_MAX_NOF_BITS
 * @param[out] output points to the unpacked encoded data
 * @param[in] output_len number of bits of encoded bits
 */
SRSLTE_API void srslte_block_encode(const uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t output_len);

/**
 * @brief Decodes 16-bit signed data using Reed–Muller code block channel coding.
 *
 * @param[in] llr Provides received LLRs
 * @param[in] nof_llr number of available LLRs
 * @param[out] data Data destination to store unpacked received bits
 * @param[in] data_len number of bits to decode, the maximum number of bits is SRSLTE_FEC_BLOCK_MAX_NOF_BITS
 * @return Decoded bits correlation if provided arguments are valid, otherwise SRSLTE_ERROR code
 */
SRSLTE_API int32_t srslte_block_decode_i16(const int16_t* llr, uint32_t nof_llr, uint8_t* data, uint32_t data_len);

/**
 * @brief Decodes 8-bit signed data using Reed–Muller code block channel coding.
 *
 * @param[in] llr Provides received LLRs
 * @param[in] nof_llr number of available LLRs
 * @param[out] data Data destination to store unpacked received bits
 * @param[in] data_len number of bits to decode, the maximum number of bits is SRSLTE_FEC_BLOCK_MAX_NOF_BITS
 * @return Decoded bits correlation if provided arguments are valid, otherwise SRSLTE_ERROR code
 */
SRSLTE_API int32_t srslte_block_decode_i8(const int8_t* llr, uint32_t nof_llr, uint8_t* data, uint32_t data_len);

#endif // SRSLTE_BLOCK_H
