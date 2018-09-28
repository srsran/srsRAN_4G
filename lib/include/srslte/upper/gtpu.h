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
 * 2      |         Message Type          |
 * 3      |     Length (1st Octet)        |
 * 4      |     Length (2nd Octet)        |
 * 5      |      TEID (1st Octet)         |
 * 6      |      TEID (2nd Octet)         |
 * 7      |      TEID (3rd Octet)         |
 * 8      |      TEID (4th Octet)         |
 * 9      |    Seq Number (1st Octet)     |
 * 10     |    Seq Number (2st Octet)     |
 * 11     |            N-PDU              |
 * 12     |  Next Extension Header Type   |
 ***************************************************************************/

#define GTPU_BASE_HEADER_LEN 8
#define GTPU_EXTENDED_HEADER_LEN 12

#define GTPU_VERSION_V1 1

#define GTP_PRIME_PROTO 0
#define GTP_PROTO 1

#define  GTPU_MSG_ECHO_REQUEST 0
#define  GTPU_MSG_ECHO_RESPONSE 1
#define  GTPU_MSG_ERROR_INDICATION 26
#define  GTPU_MSG_SUPPORTED_EXTENSION_HEADERS_NOTIFICATION 31
#define  GTPU_MSG_END_MARKER 254
#define  GTPU_MSG_DATA_PDU 255

typedef struct{
  uint8_t version : 3;
  uint8_t protocol_type : 1;
  uint8_t star : 1;
  uint8_t ext_header :1;
  uint8_t sequence :1;
  uint8_t pkt_number :1;
}gtpu_flags_t;

typedef struct{
  union{
    gtpu_flags_t flag_bits;
    uint8_t flags;
  } gtpu_flags;
  uint8_t   message_type;
  uint16_t  length;
  uint32_t  teid;
  uint16_t  seq_number;
  uint8_t   n_pdu;
  uint8_t   next_ext_hdr_type;
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
