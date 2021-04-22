/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsran/phy/fec/crc.h"
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/debug.h"

#ifdef LV_HAVE_SSE
#include <immintrin.h>
#endif // LV_HAVE_SSE

static void gen_crc_table(srsran_crc_t* h)
{
  uint32_t pad        = (h->order < 8) ? (8 - h->order) : 0;
  uint32_t ord        = h->order + pad - 8;
  uint32_t polynom    = h->polynom << pad;
  uint32_t crchighbit = h->crchighbit << pad;

  for (uint32_t i = 0; i < 256; i++) {
    uint64_t crc = ((uint64_t)i) << ord;
    for (uint32_t j = 0; j < 8; j++) {
      bool bit = crc & crchighbit;
      crc <<= 1U;
      if (bit) {
        crc ^= polynom;
      }
    }
    h->table[i] = (crc >> pad) & h->crcmask;
  }
}

uint64_t reversecrcbit(uint32_t crc, int nbits, srsran_crc_t* h)
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

int srsran_crc_set_init(srsran_crc_t* crc_par, uint64_t crc_init_value)
{
  crc_par->crcinit = crc_init_value;
  if (crc_par->crcinit != (crc_par->crcinit & crc_par->crcmask)) {
    printf("ERROR(invalid crcinit in crc_set_init().\n");
    return -1;
  }
  return 0;
}

int srsran_crc_init(srsran_crc_t* h, uint32_t crc_poly, int crc_order)
{
  // Set crc working default parameters
  h->polynom = crc_poly;
  h->order   = crc_order;
  h->crcinit = 0x00000000;

  // Compute bit masks for whole CRC and CRC high bit
  h->crcmask    = ((((uint64_t)1 << (h->order - 1)) - 1) << 1) | 1;
  h->crchighbit = (uint64_t)1 << (h->order - 1);

  if (srsran_crc_set_init(h, h->crcinit)) {
    ERROR("Error setting CRC init word");
    return -1;
  }

  // generate lookup table
  gen_crc_table(h);

  return 0;
}

uint32_t srsran_crc_checksum(srsran_crc_t* h, uint8_t* data, int len)
{
  int      i, k, len8, res8, a = 0;
  uint32_t crc = 0;
  uint8_t* pter;

  srsran_crc_set_init(h, 0);

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
#ifdef LV_HAVE_SSE
      // Get 8 Bit
      __m64 mask = _mm_cmpgt_pi8(*((__m64*)pter), _mm_set1_pi8(0));

      // Reverse
      mask = _mm_shuffle_pi8(mask, _mm_set_pi8(0, 1, 2, 3, 4, 5, 6, 7));

      // Get mask and write
      byte = (uint8_t)_mm_movemask_pi8(mask);
#else  /* LV_HAVE_SSE */
      byte = (uint8_t)(srsran_bit_pack(&pter, 8) & 0xFF);
#endif /* LV_HAVE_SSE */
    }
    srsran_crc_checksum_put_byte(h, byte);
  }
  crc = (uint32_t)srsran_crc_checksum_get(h);

  // Reverse CRC res8 positions
  if (a == 1) {
    crc = reversecrcbit(crc, 8 - res8, h);
  }

  // Return CRC value
  return crc;
}

// len is multiple of 8
uint32_t srsran_crc_checksum_byte(srsran_crc_t* h, const uint8_t* data, int len)
{
  int      i;
  uint32_t crc = 0;

  srsran_crc_set_init(h, 0);

  // Calculate CRC
  for (i = 0; i < len / 8; i++) {
    srsran_crc_checksum_put_byte(h, data[i]);
  }
  crc = (uint32_t)srsran_crc_checksum_get(h);

  return crc;
}

uint32_t srsran_crc_attach_byte(srsran_crc_t* h, uint8_t* data, int len)
{
  uint32_t checksum = srsran_crc_checksum_byte(h, data, len);

  // Add CRC
  for (int i = 0; i < h->order / 8; i++) {
    data[len / 8 + (h->order / 8 - i - 1)] = (checksum & (0xff << (8 * i))) >> (8 * i);
  }
  return checksum;
}

/** Appends crc_order checksum bits to the buffer data.
 * The buffer data must be len + crc_order bytes
 */
uint32_t srsran_crc_attach(srsran_crc_t* h, uint8_t* data, int len)
{
  uint32_t checksum = srsran_crc_checksum(h, data, len);

  // Add CRC
  uint8_t* ptr = &data[len];
  srsran_bit_unpack(checksum, &ptr, h->order);
  return checksum;
}

bool srsran_crc_match(srsran_crc_t* h, uint8_t* data, int len)
{
  uint8_t* ptr       = &data[len];
  uint32_t checksum1 = srsran_crc_checksum(h, data, len);
  uint32_t checksum2 = srsran_bit_pack(&ptr, h->order);
  return (checksum1 == checksum2);
}
