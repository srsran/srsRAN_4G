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

#ifndef SRSLTE_GTPU_H
#define SRSLTE_GTPU_H

#include "srslte/common/common.h"
#include "srslte/common/logmap.h"
#include <stdint.h>

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

#define GTPU_FLAGS_VERSION_MASK 0xE0
#define GTPU_FLAGS_VERSION_V1 0x20
#define GTPU_FLAGS_GTP_PRIME_PROTOCOL 0x00
#define GTPU_FLAGS_GTP_PROTOCOL 0x10
#define GTPU_FLAGS_EXTENDED_HDR 0x04
#define GTPU_FLAGS_SEQUENCE 0x02
#define GTPU_FLAGS_PACKET_NUM 0x01

#define GTPU_MSG_ECHO_REQUEST 1
#define GTPU_MSG_ECHO_RESPONSE 2
#define GTPU_MSG_ERROR_INDICATION 26
#define GTPU_MSG_SUPPORTED_EXTENSION_HEADERS_NOTIFICATION 31
#define GTPU_MSG_END_MARKER 254
#define GTPU_MSG_DATA_PDU 255

typedef struct {
  uint8_t  flags;
  uint8_t  message_type;
  uint16_t length;
  uint32_t teid;
  uint16_t seq_number;
  uint8_t  n_pdu;
  uint8_t  next_ext_hdr_type;
} gtpu_header_t;

bool        gtpu_read_header(srslte::byte_buffer_t* pdu, gtpu_header_t* header, srslte::log_ref gtpu_log);
bool        gtpu_write_header(gtpu_header_t* header, srslte::byte_buffer_t* pdu, srslte::log_ref gtpu_log);
std::string gtpu_ntoa(uint32_t addr);

inline bool gtpu_supported_flags_check(gtpu_header_t* header, srslte::log_ref gtpu_log)
{
  // flags
  if ((header->flags & GTPU_FLAGS_VERSION_MASK) != GTPU_FLAGS_VERSION_V1) {
    gtpu_log->error("gtpu_header - Unhandled GTP-U Version. Flags: 0x%x\n", header->flags);
    return false;
  }
  if (!(header->flags & GTPU_FLAGS_GTP_PROTOCOL)) {
    gtpu_log->error("gtpu_header - Unhandled Protocol Type. Flags: 0x%x\n\n", header->flags);
    return false;
  }
  if (header->flags & GTPU_FLAGS_EXTENDED_HDR) {
    gtpu_log->error("gtpu_header - Unhandled Header Extensions. Flags: 0x%x\n\n", header->flags);
    return false;
  }
  if (header->flags & GTPU_FLAGS_PACKET_NUM) {
    gtpu_log->error("gtpu_header - Unhandled Packet Number. Flags: 0x%x\n\n", header->flags);
    return false;
  }
  return true;
}

inline bool gtpu_supported_msg_type_check(gtpu_header_t* header, srslte::log_ref gtpu_log)
{
  // msg_tpye
  if (header->message_type != GTPU_MSG_DATA_PDU && header->message_type != GTPU_MSG_ECHO_REQUEST &&
      header->message_type != GTPU_MSG_ECHO_RESPONSE) {
    gtpu_log->error("gtpu_header - Unhandled message type: 0x%x\n", header->message_type);
    return false;
  }
  return true;
}

} // namespace srslte

#endif
