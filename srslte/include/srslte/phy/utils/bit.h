/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#ifndef BIT_
#define BIT_

#include <stdint.h>
#include <stdio.h>

#include "srslte/config.h"

LIBLTE_API void bit_pack_vector(uint8_t *bit_unpacked, 
                     uint8_t *bits_packed, 
                     int nof_bits);

LIBLTE_API void bit_unpack_vector(uint8_t *bits_packed, 
                                  uint8_t *bit_unpacked, 
                                  int nof_bits);

LIBLTE_API uint32_t bit_unpack(uint8_t **bits, 
                               int nof_bits);

LIBLTE_API void bit_pack(uint32_t value, 
                         uint8_t **bits, 
                         int nof_bits);

LIBLTE_API void bit_fprint(FILE *stream, 
                           uint8_t *bits, 
                           int nof_bits);

LIBLTE_API uint32_t bit_diff(uint8_t *x, 
                             uint8_t *y, 
                             int nbits);

LIBLTE_API uint32_t bit_count(uint32_t n);

#endif // BIT_

