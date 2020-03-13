/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "srslte/phy/fec/crc.h"
#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/debug.h"

void gen_crc_table(srslte_crc_t* h)
{

  int      i, j, ord = (h->order - 8);
  uint64_t bit, crc;

  for (i = 0; i < 256; i++) {
    crc = ((uint64_t)i) << ord;
    for (j = 0; j < 8; j++) {
      bit = crc & h->crchighbit;
      crc <<= 1;
      if (bit)
        crc ^= h->polynom;
    }
    h->table[i] = crc & h->crcmask;
  }
}

uint64_t reversecrcbit(uint32_t crc, int nbits, srslte_crc_t* h)
{

  uint64_t m, rmask = 0x1;

  for (m = 0; m < nbits; m++) {
    if ((rmask & crc) == 0x01)
      crc = (crc ^ h->polynom) >> 1;
    else
      crc = crc >> 1;
  }
  return (crc & h->crcmask);
}

int srslte_crc_set_init(srslte_crc_t* crc_par, uint64_t crc_init_value)
{

  crc_par->crcinit = crc_init_value;
  if (crc_par->crcinit != (crc_par->crcinit & crc_par->crcmask)) {
    printf("ERROR(invalid crcinit in crc_set_init().\n");
    return -1;
  }
  return 0;
}

int srslte_crc_init(srslte_crc_t* h, uint32_t crc_poly, int crc_order)
{

  // Set crc working default parameters
  h->polynom = crc_poly;
  h->order   = crc_order;
  h->crcinit = 0x00000000;

  // Compute bit masks for whole CRC and CRC high bit
  h->crcmask    = ((((uint64_t)1 << (h->order - 1)) - 1) << 1) | 1;
  h->crchighbit = (uint64_t)1 << (h->order - 1);

  // check parameters
  if (h->order % 8 != 0) {
    ERROR("ERROR(invalid order=%d, it must be 8, 16, 24 or 32.\n", h->order);
    return -1;
  }

  if (srslte_crc_set_init(h, h->crcinit)) {
    ERROR("Error setting CRC init word\n");
    return -1;
  }

  // generate lookup table
  gen_crc_table(h);

  return 0;
}

uint32_t srslte_crc_checksum(srslte_crc_t* h, uint8_t* data, int len)
{
  int      i, k, len8, res8, a = 0;
  uint32_t crc = 0;
  uint8_t* pter;

  srslte_crc_set_init(h, 0);

  // Pack bits into bytes
  len8 = (len >> 3);
  res8 = (len - (len8 << 3));
  if (res8 > 0) {
    a = 1;
  }

  // Calculate CRC
  for (i = 0; i < len8 + a; i++) {
    pter = (uint8_t*)(data + 8 * i);
    uint8_t byte;
    if (i == len8) {
      byte = 0x00;
      for (k = 0; k < res8; k++) {
        byte |= ((uint8_t) * (pter + k)) << (7 - k);
      }
    } else {
      byte = (uint8_t)(srslte_bit_pack(&pter, 8) & 0xFF);
    }
    srslte_crc_checksum_put_byte(h, byte);
  }
  crc = (uint32_t)srslte_crc_checksum_get(h);

  // Reverse CRC res8 positions
  if (a == 1) {
    crc = reversecrcbit(crc, 8 - res8, h);
  }

  // Return CRC value
  return crc;
}

// len is multiple of 8
uint32_t srslte_crc_checksum_byte(srslte_crc_t* h, uint8_t* data, int len)
{
  int      i;
  uint32_t crc = 0;

  srslte_crc_set_init(h, 0);

  // Calculate CRC
  for (i = 0; i < len / 8; i++) {
    srslte_crc_checksum_put_byte(h, data[i]);
  }
  crc = (uint32_t)srslte_crc_checksum_get(h);

  return crc;
}

uint32_t srslte_crc_attach_byte(srslte_crc_t* h, uint8_t* data, int len)
{
  uint32_t checksum = srslte_crc_checksum_byte(h, data, len);

  // Add CRC
  for (int i = 0; i < h->order / 8; i++) {
    data[len / 8 + (h->order / 8 - i - 1)] = (checksum & (0xff << (8 * i))) >> (8 * i);
  }
  return checksum;
}

/** Appends crc_order checksum bits to the buffer data.
 * The buffer data must be len + crc_order bytes
 */
uint32_t srslte_crc_attach(srslte_crc_t* h, uint8_t* data, int len)
{
  uint32_t checksum = srslte_crc_checksum(h, data, len);

  // Add CRC
  uint8_t* ptr = &data[len];
  srslte_bit_unpack(checksum, &ptr, h->order);
  return checksum;
}
