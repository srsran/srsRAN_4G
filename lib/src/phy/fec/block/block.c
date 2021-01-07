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

#include "srslte/phy/fec/block/block.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/vector.h"

// Use carry-less multiplication for parity check calculation only if AVX2 is available
#ifdef LV_HAVE_AVX2
#include <immintrin.h>
#define USE_CARRYLESS_MULT 1
#else
#define USE_CARRYLESS_MULT 0
#endif // LV_HAVE_AVX2

// The following MACRO enables/disables LUT for the decoder
#define USE_LUT 1

// The following type is used for selecting the algorithm precision
typedef int16_t block_llr_t;

/// Table 5.2.2.6.4-1: Basis sequence for (32, O) code compressed in uint16_t types
static const uint64_t M_basis_seq_b[SRSLTE_FEC_BLOCK_SIZE] = {
    0b10000000011, 0b11000000111, 0b11101001001, 0b10100001101, 0b10010001111, 0b10111010011, 0b11101010101,
    0b10110011001, 0b11010011011, 0b11001011101, 0b11011100101, 0b10101100111, 0b11110101001, 0b11010101011,
    0b10010110001, 0b11011110011, 0b01001110111, 0b00100111001, 0b00011111011, 0b00001100001, 0b10001000101,
    0b11000001011, 0b10110010001, 0b11100010111, 0b01111011111, 0b10011100011, 0b01100101101, 0b01110101111,
    0b00101110101, 0b00111111101, 0b11111111111, 0b00000000001,
};

static inline uint8_t encode_M_basis_seq_u16(uint16_t w, uint32_t bit_idx)
{
  // Apply mask
  uint64_t d = w & M_basis_seq_b[bit_idx % SRSLTE_FEC_BLOCK_SIZE];

#if USE_CARRYLESS_MULT
  // Compute parity using carry-less multiplication
  const __m128i temp = _mm_clmulepi64_si128(_mm_set_epi64x(0, d), _mm_set_epi64x(0, 0xffffUL << 17UL), 0);
  d                  = _mm_extract_epi32(temp, 1);
#else
  // Compute parity using Bit Twiddling
  d ^= d >> 8UL;
  d ^= d >> 4UL;
  d &= 0xFUL;
  d = (0x6996U >> d);
#endif

  return (uint8_t)(d & 1UL);
}

#if USE_LUT
static block_llr_t M_basis_seq_b_lut[1U << SRSLTE_FEC_BLOCK_MAX_NOF_BITS][SRSLTE_FEC_BLOCK_SIZE];

// Initialization function, as the table does not change, it can be initialised as constructor
__attribute__((constructor)) static void srslte_block_init()
{
  for (uint32_t word = 0; word < (1U << SRSLTE_FEC_BLOCK_MAX_NOF_BITS); word++) {
    for (uint32_t i = 0; i < SRSLTE_FEC_BLOCK_SIZE; i++) {
      // Encode guess word
      M_basis_seq_b_lut[word][i] = encode_M_basis_seq_u16(word, i) * 2 - 1;
    }
  }
}
#endif

void srslte_block_encode(const uint8_t* input, uint32_t input_len, uint8_t* output, uint32_t output_len)
{
  // Limit number of input bits
  input_len = SRSLTE_MIN(input_len, SRSLTE_FEC_BLOCK_MAX_NOF_BITS);

  // Pack input bits
  uint16_t w = 0;
  for (uint32_t i = 0; i < input_len; i++) {
    w |= (input[i] & 1U) << i;
  }

  // Encode bits
  for (uint32_t i = 0; i < SRSLTE_MIN(output_len, SRSLTE_FEC_BLOCK_SIZE); i++) {
    output[i] = encode_M_basis_seq_u16(w, i);
  }

  // Avoid repeating operation by copying repeated sequence
  for (uint32_t i = SRSLTE_FEC_BLOCK_SIZE; i < output_len; i++) {
    output[i] = output[i % SRSLTE_FEC_BLOCK_SIZE];
  }
}

static int32_t srslte_block_decode(const block_llr_t llr[SRSLTE_FEC_BLOCK_SIZE], uint8_t* data, uint32_t data_len)
{
  int32_t  max_corr = 0; //< Stores maximum correlation
  uint32_t max_data = 0; //< Stores the word for maximum correlation

  // Limit data to maximum
  data_len = SRSLTE_MIN(data_len, SRSLTE_FEC_BLOCK_MAX_NOF_BITS);

  // Brute force all possible sequences
  uint16_t max_guess = (1U << data_len); //< Maximum guess bit combination (excluded)
  for (uint16_t guess = 0; guess < max_guess; guess++) {
    int32_t corr = 0;
#if USE_LUT
    // Load sequence from LUT
    block_llr_t* sequence = M_basis_seq_b_lut[guess];

    // Dot product
    for (uint32_t i = 0; i < SRSLTE_FEC_BLOCK_SIZE; i++) {
      corr += llr[i] * sequence[i];
    }
#else
    for (uint32_t i = 0; i < SRSLTE_FEC_BLOCK_SIZE; i++) {
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

  // Unpack
  for (uint32_t i = 0; i < data_len; i++) {
    data[i] = (uint8_t)((max_data >> i) & 1U);
  }

  // Return correlation
  return max_corr;
}

int32_t srslte_block_decode_i8(const int8_t* llr, uint32_t nof_llr, uint8_t* data, uint32_t data_len)
{
  block_llr_t llr_[SRSLTE_FEC_BLOCK_SIZE];

  // Return invalid inputs if data is not provided
  if (!llr || !data) {
    ERROR("Invalid inputs\n");
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Return invalid inputs if not enough LLR are provided
  if (nof_llr < SRSLTE_FEC_BLOCK_SIZE) {
    ERROR("Not enough LLR bits are provided %d. Required %d;\n", nof_llr, SRSLTE_FEC_BLOCK_SIZE);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Load the minimum of LLRs
  uint32_t i = 0;
  for (; i < SRSLTE_FEC_BLOCK_SIZE; i++) {
    llr_[i] = (block_llr_t)llr[i];
  }

  // Combine the rest of LLRs
  for (; i < nof_llr; i++) {
    llr_[i % SRSLTE_FEC_BLOCK_SIZE] += (block_llr_t)llr[i];
  }

  return srslte_block_decode(llr_, data, data_len);
}

int32_t srslte_block_decode_i16(const int16_t* llr, uint32_t nof_llr, uint8_t* data, uint32_t data_len)
{
  block_llr_t llr_[SRSLTE_FEC_BLOCK_SIZE];

  // Return invalid inputs if data is not provided
  if (!llr || !data) {
    ERROR("Invalid inputs\n");
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Return invalid inputs if not enough LLR are provided
  if (nof_llr < SRSLTE_FEC_BLOCK_SIZE) {
    ERROR("Not enough LLR bits are provided %d. Required %d;\n", nof_llr, SRSLTE_FEC_BLOCK_SIZE);
    return SRSLTE_ERROR_INVALID_INPUTS;
  }

  // Load the minimum of LLRs
  uint32_t i = 0;
  for (; i < SRSLTE_FEC_BLOCK_SIZE; i++) {
    llr_[i] = (block_llr_t)llr[i];
  }

  // Combine the rest of LLRs
  for (; i < nof_llr; i++) {
    llr_[i % SRSLTE_FEC_BLOCK_SIZE] += (block_llr_t)llr[i];
  }

  return srslte_block_decode(llr_, data, data_len);
}
