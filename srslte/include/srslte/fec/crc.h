/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
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

#include "srslte/config.h"
#include <stdint.h>

typedef struct SRSLTE_API {
  unsigned long table[256];
  uint8_t byte;
  int polynom;
  int order;
  unsigned long crcinit; 
  unsigned long crcmask;
  unsigned long crchighbit;
  uint32_t srslte_crc_out;
} srslte_crc_t;

SRSLTE_API int srslte_crc_init(srslte_crc_t *h, 
                               uint32_t srslte_crc_poly, 
                               int srslte_crc_order);

SRSLTE_API int srslte_crc_set_init(srslte_crc_t *h, 
                                   unsigned long srslte_crc_init_value);

SRSLTE_API void srslte_crc_attach(srslte_crc_t *h, 
                                  uint8_t *data, 
                                  int len);

SRSLTE_API uint32_t srslte_crc_checksum(srslte_crc_t *h, 
                                        uint8_t *data, 
                                        int len);

#endif
