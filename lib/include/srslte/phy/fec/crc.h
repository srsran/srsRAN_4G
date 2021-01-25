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

/**********************************************************************************************
 *  File:         convcoder.h
 *
 *  Description:  Cyclic Redundancy Check
 *                LTE requires CRC lengths 8, 16, 24A and 24B, each with it's own generator
 *                polynomial.
 *
 *  Reference:    3GPP TS 36.212 version 10.0.0 Release 10 Sec. 5.1.1
 *********************************************************************************************/

#ifndef SRSLTE_CRC_H
#define SRSLTE_CRC_H

#include "srslte/config.h"
#include <stdint.h>

typedef struct SRSLTE_API {
  uint64_t table[256];
  int      polynom;
  int      order;
  uint64_t crcinit;
  uint64_t crcmask;
  uint64_t crchighbit;
  uint32_t srslte_crc_out;
} srslte_crc_t;

SRSLTE_API int srslte_crc_init(srslte_crc_t* h, uint32_t srslte_crc_poly, int srslte_crc_order);

SRSLTE_API int srslte_crc_set_init(srslte_crc_t* h, uint64_t init_value);

SRSLTE_API uint32_t srslte_crc_attach(srslte_crc_t* h, uint8_t* data, int len);

SRSLTE_API uint32_t srslte_crc_attach_byte(srslte_crc_t* h, uint8_t* data, int len);

static inline void srslte_crc_checksum_put_byte(srslte_crc_t* h, uint8_t byte)
{

  uint64_t crc = h->crcinit;

  uint32_t idx;
  if (h->order > 8) {
    // For more than 8 bits
    uint32_t ord = h->order - 8U;
    idx          = ((crc >> (ord)) & 0xffU) ^ byte;
  } else {
    // For 8 bits or less
    uint32_t ord = 8U - h->order;
    idx          = ((crc << (ord)) & 0xffU) ^ byte;
  }

  crc        = (crc << 8U) ^ h->table[idx];
  h->crcinit = crc;
}

static inline uint64_t srslte_crc_checksum_get(srslte_crc_t* h)
{
  return (h->crcinit & h->crcmask);
}

SRSLTE_API uint32_t srslte_crc_checksum_byte(srslte_crc_t* h, const uint8_t* data, int len);

SRSLTE_API uint32_t srslte_crc_checksum(srslte_crc_t* h, uint8_t* data, int len);

#endif // SRSLTE_CRC_H
