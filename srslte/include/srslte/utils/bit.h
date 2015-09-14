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

/******************************************************************************
 *  File:         bit.h
 *
 *  Description:  Bit-level utilities.
 *
 *  Reference:
 *****************************************************************************/

#ifndef BIT_
#define BIT_

#include <stdint.h>
#include <stdio.h>

#include "srslte/config.h"

SRSLTE_API void srslte_bit_unpack_vector(uint8_t *packed,
                                         uint8_t *unpacked,
                                         int nof_bits);

SRSLTE_API void srslte_bit_pack_vector(uint8_t *unpacked,
                                       uint8_t *packed,
                                       int nof_bits);

SRSLTE_API uint32_t srslte_bit_pack(uint8_t **bits, 
                                      int nof_bits);

SRSLTE_API uint64_t srslte_bit_pack_l(uint8_t **bits, 
                                        int nof_bits);

SRSLTE_API void srslte_bit_unpack_l(uint64_t value, 
                                  uint8_t **bits, 
                                  int nof_bits);

SRSLTE_API void srslte_bit_unpack(uint32_t value, 
                                uint8_t **bits, 
                                int nof_bits);

SRSLTE_API void srslte_bit_fprint(FILE *stream, 
                                  uint8_t *bits, 
                                  int nof_bits);

SRSLTE_API uint32_t srslte_bit_diff(uint8_t *x, 
                                    uint8_t *y, 
                                    int nbits);

SRSLTE_API uint32_t srslte_bit_count(uint32_t n);

#endif // BIT_

