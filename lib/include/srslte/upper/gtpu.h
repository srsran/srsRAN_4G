/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_GTPU_H
#define SRSLTE_GTPU_H

#include <stdint.h>
#include "srslte/common/common.h"
#include "srslte/common/log.h"

namespace srslte {

/****************************************************************************
 * GTPU Header
 * Ref: 3GPP TS 29.281 v10.1.0 Section 5
 *
 *        | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 |
 *
 * 1      |  Version  |PT | * | E | S |PN |
 * 2      |           Message Type        |
 * 3      |         Length (1st Octet)    |
 * 4      |         Length (2nd Octet)    |
 * 5      |          TEID (1st Octet)     |
 * 6      |          TEID (2nd Octet)     |
 * 7      |          TEID (3rd Octet)     |
 * 8      |          TEID (4th Octet)     |
 ***************************************************************************/

#define GTPU_HEADER_LEN 8

typedef struct{
  uint8_t   flags;          // Only support 0x30 - v1, PT1 (GTP), no other flags
  uint8_t   message_type;   // Only support 0xFF - T-PDU type
  uint16_t  length;
  uint32_t  teid;
}gtpu_header_t;


bool gtpu_read_header(srslte::byte_buffer_t *pdu, gtpu_header_t *header, srslte::log *gtpu_log);
bool gtpu_write_header(gtpu_header_t *header, srslte::byte_buffer_t *pdu, srslte::log *gtpu_log);

inline void uint8_to_uint32(uint8_t *buf, uint32_t *i)
{
  *i =  (uint32_t)buf[0] << 24 |
    (uint32_t)buf[1] << 16 |
    (uint32_t)buf[2] << 8  |
    (uint32_t)buf[3];
}

inline void uint32_to_uint8(uint32_t i, uint8_t *buf)
{
  buf[0] = (i >> 24) & 0xFF;
  buf[1] = (i >> 16) & 0xFF;
  buf[2] = (i >> 8) & 0xFF;
  buf[3] = i & 0xFF;
}

inline void uint8_to_uint16(uint8_t *buf, uint16_t *i)
{
  *i =  (uint32_t)buf[0] << 8  |
    (uint32_t)buf[1];
}

inline void uint16_to_uint8(uint16_t i, uint8_t *buf)
{
  buf[0] = (i >> 8) & 0xFF;
  buf[1] = i & 0xFF;
}


}//namespace

#endif
