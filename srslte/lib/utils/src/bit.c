/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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


#include <stdint.h>
#include <stdio.h>

#include "srslte/utils/bit.h"

void srslte_bit_unpack_vector(uint8_t *packed, uint8_t *unpacked, int nof_bits)
{
  uint32_t i, nbytes;
  nbytes = nof_bits/8;
  for (i=0;i<nbytes;i++) {
    srslte_bit_unpack(packed[i], &unpacked, 8);
  }
  if (nof_bits%8) {
    srslte_bit_unpack(packed[i], &unpacked, nof_bits%8);
  }
}

void srslte_bit_unpack_l(uint64_t value, uint8_t **bits, int nof_bits)
{
  int i;

  for(i=0; i<nof_bits; i++) {
      (*bits)[i] = (value >> (nof_bits-i-1)) & 0x1;
  }
  *bits += nof_bits;
}

void srslte_bit_unpack(uint32_t value, uint8_t **bits, int nof_bits)
{
    int i;

    for(i=0; i<nof_bits; i++) {
        (*bits)[i] = (value >> (nof_bits-i-1)) & 0x1;
    }
    *bits += nof_bits;
}

void srslte_bit_pack_vector(uint8_t *unpacked, uint8_t *packed, int nof_bits)
{
  uint32_t i, nbytes;
  nbytes = nof_bits/8;
  for (i=0;i<nbytes;i++) {
    packed[i] = srslte_bit_pack(&unpacked, 8);
  }
  if (nof_bits%8) {
    packed[i] = srslte_bit_pack(&unpacked, nof_bits%8);
  }
}

uint32_t srslte_bit_pack(uint8_t **bits, int nof_bits)
{
    int i;
    uint32_t value=0;

    for(i=0; i<nof_bits; i++) {
      value |= (uint32_t) (*bits)[i] << (nof_bits-i-1);
    }
    *bits += nof_bits;
    return value;
}

uint64_t srslte_bit_pack_l(uint8_t **bits, int nof_bits)
{
    int i;
    uint64_t value=0;

    for(i=0; i<nof_bits; i++) {
      value |= (uint64_t) (*bits)[i] << (nof_bits-i-1);
    }
    *bits += nof_bits;
    return value;
}

void srslte_bit_fprint(FILE *stream, uint8_t *bits, int nof_bits) {
  int i;

  fprintf(stream,"[");
  for (i=0;i<nof_bits-1;i++) {
    fprintf(stream,"%d,",bits[i]);
  }
  fprintf(stream,"%d]\n",bits[i]);
}

uint32_t srslte_bit_diff(uint8_t *x, uint8_t *y, int nbits) {
  uint32_t errors=0;
  for (int i=0;i<nbits;i++) {
    if (x[i] != y[i]) {
      //printf("%d, ",i);
      errors++;
    }
  }
  return errors;
}

// Counts the number of ones in a word. K&R book exercise 2.9
uint32_t srslte_bit_count(uint32_t n) {
  int c;
  for (c = 0; n; c++)
    n &= n - 1;
  return c;
}
