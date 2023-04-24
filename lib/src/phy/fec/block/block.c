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

#include "srsran/phy/fec/block/block.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/vector.h"

// The following MACRO enables/disables LUT for the decoder
#define USE_LUT 1

// The following type is used for selecting the algorithm precision
typedef int16_t block_llr_t;

/// Table 5.2.2.6.4-1: Basis sequence for (32, O) code compressed in uint16_t types
static const uint64_t M_basis_seq_b[SRSRAN_FEC_BLOCK_SIZE] = {
    0b10000000011, 0b11000000111, 0b11101001001, 0b10100001101, 0b10010001111, 0b10111010011, 0b11101010101,
    0b10110011001, 0b11010011011, 0b11001011101, 0b11011100101, 0b10101100111, 0b11110101001, 0b11010101011,
    0b10010110001, 0b11011110011, 0b01001110111, 0b00100111001, 0b00011111011, 0b00001100001, 0b10001000101,
    0b11000001011, 0b10110010001, 0b11100010111, 0b01111011111, 0b10011100011, 0b01100101101, 0b01110101111,
    0b00101110101, 0b00111111101, 0b11111111111, 0b00000000001,
};

static inline uint8_t encode_M_basis_seq_u16(uint16_t w, uint32_t bit_idx)
{
  // Apply mask
  uint64_t d = w & M_basis_seq_b[bit_idx % SRSRAN_FEC_BLOCK_SIZE];

  // Compute parity using Bit Twiddling
  d ^= d >> 8UL;
  d ^= d >> 4UL;
  d &= 0xFUL;
  d = (0x6996U >> d);

  return (uint8_t)(d & 1UL);
}

#if USE_LUT
// Encoded unpacked table
static uint8_t block_unpacked_lut[1U << SRSRAN_FEC_BLOCK_MAX_NOF_BITS][SRSRAN_FEC_BLOCK_SIZE];

// LLR signed table
static block_llr_t block_llr_lut[1U << SRSRAN_FEC_BLOCK_MAX_NOF_BITS][SRSRAN_FEC_BLOCK_SIZE];

// Initialization function, as the table is read-only after initialization, it can be initialised from constructor
__attribute__((constructor)) static void srsran_block_init()
{
  for (uint32_t word = 0; word < (1U << SRSRAN_FEC_BLOCK_MAX_NOF_BITS); word++) {
    for (uint32_t i = 0; i < SRSRAN_FEC_BLOCK_SIZE; i++) {
      uint8_t e = encode_M_basis_seq_u16(word, i);
      // Encoded unpacked byte
      block_unpacked_lut[word][i] = e;

      // Encoded LLR
      block_llr_lut[word][i] = (block_llr_t)e * 2 - 1;
    }
  }
}
#endif

void srsran_block_encode(const uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t output_len)
{
  if (!input || !output) {
    ERROR("Invalid inputs");
    return;
  }

  // Limit number of input bits
  input_len = SRSRAN_MIN(input_len, SRSRAN_FEC_BLOCK_MAX_NOF_BITS);

  // Pack input bits (reversed)
  uint16_t w = 0;
  for (uint32_t i = 0; i < input_len; i++) {
    w |= (input[i] & 1U) << i;
  }

  // Encode bits
#if USE_LUT
  uint32_t i = 0;
  for (; i < output_len / SRSRAN_FEC_BLOCK_SIZE; i++) {
    srsran_vec_u8_copy(&output[i * SRSRAN_FEC_BLOCK_SIZE], block_unpacked_lut[w], SRSRAN_FEC_BLOCK_SIZE);
  }
  srsran_vec_u8_copy(&output[i * SRSRAN_FEC_BLOCK_SIZE], block_unpacked_lut[w], output_len % SRSRAN_FEC_BLOCK_SIZE);
#else  // USE_LUT
  for (uint32_t i = 0; i < SRSRAN_MIN(output_len, SRSRAN_FEC_BLOCK_SIZE); i++) {
    output[i] = encode_M_basis_seq_u16(w, i);
  }

  // Avoid repeating operation by copying repeated sequence
  for (uint32_t i = SRSRAN_FEC_BLOCK_SIZE; i < output_len; i++) {
    output[i] = output[i % SRSRAN_FEC_BLOCK_SIZE];
  }
#endif // USE_LUT
}

static int32_t block_decode(const block_llr_t* llr, uint8_t* data, uint32_t data_len)
{
  int32_t  max_corr = 0; //< Stores maximum correlation
  uint32_t max_data = 0; //< Stores the word for maximum correlation

  // Limit data to maximum
  data_len = SRSRAN_MIN(data_len, SRSRAN_FEC_BLOCK_MAX_NOF_BITS);

  // Brute force all possible sequences
  uint16_t max_guess = (1U << data_len); //< Maximum guess bit combination (excluded)
  for (uint16_t guess = 0; guess < max_guess; guess++) {
    int32_t corr = 0;
#if USE_LUT
    // Load sequence from LUT
    // Dot product
    for (uint32_t i = 0; i < SRSRAN_FEC_BLOCK_SIZE; i++) {
      corr += llr[i] * block_llr_lut[guess][i];
    }
#else
    for (uint32_t i = 0; i < SRSRAN_FEC_BLOCK_SIZE; i++) {
      // On-the-fly sequence generation and product
      corr += llr[i] * (encode_M_basis_seq_u16(guess, i) * 2 - 1);
    }
#endif

    // Take decision
    if (corr > max_corr) {
      max_corr = corr;
      max_data = guess;
    }
  }

  // Bit unpack (reversed)
  for (uint32_t i = 0; i < data_len; i++) {
    data[i] = (uint8_t)((max_data >> i) & 1U);
  }

  // Return correlation
  return max_corr;
}

int32_t srsran_block_decode_i8(const int8_t* llr, uint32_t nof_llr, uint8_t* data, uint32_t data_len)
{
  block_llr_t llr_[SRSRAN_FEC_BLOCK_SIZE] = {};

  // Return invalid inputs if data is not provided
  if (!llr || !data) {
    ERROR("Invalid inputs");
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Accumulate all copies of the 32-length sequence
  for (uint32_t i = 0; i < nof_llr; i++) {
    llr_[i % SRSRAN_FEC_BLOCK_SIZE] += (block_llr_t)llr[i];
  }

  return block_decode(llr_, data, data_len);
}

int32_t srsran_block_decode_i16(const int16_t* llr, uint32_t nof_llr, uint8_t* data, uint32_t data_len)
{
  block_llr_t llr_[SRSRAN_FEC_BLOCK_SIZE] = {};

  // Return invalid inputs if data is not provided
  if (!llr || !data) {
    ERROR("Invalid inputs");
    return SRSRAN_ERROR_INVALID_INPUTS;
  }

  // Accumulate all copies of the 32-length sequence
  uint32_t i = 0;
  for (; i < nof_llr / SRSRAN_FEC_BLOCK_SIZE; i++) {
    srsran_vec_sum_sss(&llr[i * SRSRAN_FEC_BLOCK_SIZE], llr_, llr_, SRSRAN_FEC_BLOCK_SIZE);
  }
  srsran_vec_sum_sss(&llr[i * SRSRAN_FEC_BLOCK_SIZE], llr_, llr_, nof_llr % SRSRAN_FEC_BLOCK_SIZE);

  return block_decode(llr_, data, data_len);
}
