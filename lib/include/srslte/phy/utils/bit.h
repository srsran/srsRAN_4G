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

/******************************************************************************
 *  File:         bit.h
 *
 *  Description:  Bit-level utilities.
 *
 *  Reference:
 *****************************************************************************/

#ifndef SRSLTE_BIT_H
#define SRSLTE_BIT_H

#include <stdint.h>
#include <stdio.h>

#include "srslte/config.h"

typedef struct {
  uint32_t  nof_bits;
  uint16_t* interleaver;
  uint16_t* byte_idx;
  uint8_t*  bit_mask;
  uint8_t   n_128;
} srslte_bit_interleaver_t;

SRSLTE_API void srslte_bit_interleaver_init(srslte_bit_interleaver_t* q, uint16_t* interleaver, uint32_t nof_bits);

SRSLTE_API void srslte_bit_interleaver_free(srslte_bit_interleaver_t* q);

SRSLTE_API void
srslte_bit_interleaver_run(srslte_bit_interleaver_t* q, uint8_t* input, uint8_t* output, uint16_t w_offset);

SRSLTE_API void srslte_bit_interleave(uint8_t* input, uint8_t* output, uint16_t* interleaver, uint32_t nof_bits);

SRSLTE_API void
srslte_bit_copy(uint8_t* dst, uint32_t dst_offset, uint8_t* src, uint32_t src_offset, uint32_t nof_bits);

SRSLTE_API void srslte_bit_interleave_i(uint8_t* input, uint8_t* output, uint32_t* interleaver, uint32_t nof_bits);

SRSLTE_API void srslte_bit_interleave_i_w_offset(uint8_t*  input,
                                                 uint8_t*  output,
                                                 uint32_t* interleaver,
                                                 uint32_t  nof_bits,
                                                 uint32_t  w_offset);

SRSLTE_API void srslte_bit_interleave_w_offset(uint8_t*  input,
                                               uint8_t*  output,
                                               uint16_t* interleaver,
                                               uint32_t  nof_bits,
                                               uint32_t  w_offset);

SRSLTE_API void srslte_bit_unpack_vector(const uint8_t* packed, uint8_t* unpacked, int nof_bits);

SRSLTE_API void srslte_bit_pack_vector(uint8_t* unpacked, uint8_t* packed, int nof_bits);

SRSLTE_API uint32_t srslte_bit_pack(uint8_t** bits, int nof_bits);

SRSLTE_API uint64_t srslte_bit_pack_l(uint8_t** bits, int nof_bits);

SRSLTE_API void srslte_bit_unpack_l(uint64_t value, uint8_t** bits, int nof_bits);

SRSLTE_API void srslte_bit_unpack(uint32_t value, uint8_t** bits, int nof_bits);

SRSLTE_API void srslte_bit_fprint(FILE* stream, uint8_t* bits, int nof_bits);

SRSLTE_API uint32_t srslte_bit_diff(const uint8_t* x, const uint8_t* y, int nbits);

SRSLTE_API uint32_t srslte_bit_count(uint32_t n);

#endif // SRSLTE_BIT_H
