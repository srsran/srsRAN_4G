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


#ifndef CRC_
#define CRC_

#include "liblte/config.h"
#include <stdint.h>

typedef struct LIBLTE_API {
  unsigned long table[256];
  uint8_t byte;
  int polynom;
  int order;
  unsigned long crcinit; 
  unsigned long crcmask;
  unsigned long crchighbit;
  uint32_t crc_out;
} crc_t;

LIBLTE_API int crc_init(crc_t *h, uint32_t crc_poly, int crc_order);
LIBLTE_API int crc_set_init(crc_t *h, unsigned long crc_init_value);
LIBLTE_API void crc_attach(crc_t *h, uint8_t *data, int len);
LIBLTE_API uint32_t crc_checksum(crc_t *h, uint8_t *data, int len);

#endif
