/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#ifndef SRSRAN_GTPU_H
#define SRSRAN_GTPU_H

#include "srsran/common/byte_buffer.h"
#include "srsran/common/common.h"
#include "srsran/srslog/srslog.h"
#include <stdint.h>

namespace srsran {

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

#define GTPU_EXT_NO_MORE_EXTENSION_HEADERS 0x00
#define GTPU_EXT_HEADER_PDCP_PDU_NUMBER 0b11000000
#define GTPU_EXT_HEADER_PDU_SESSION_CONTAINER 0x85

#define GTPU_EXT_HEADER_PDU_SESSION_CONTAINER_LEN 4
struct gtpu_header_t {
  uint8_t              flags             = 0;
  uint8_t              message_type      = 0;
  uint16_t             length            = 0;
  uint32_t             teid              = 0;
  uint16_t             seq_number        = 0;
  uint8_t              n_pdu             = 0;
  uint8_t              next_ext_hdr_type = 0;
  std::vector<uint8_t> ext_buffer;
};

bool gtpu_read_header(srsran::byte_buffer_t* pdu, gtpu_header_t* header, srslog::basic_logger& logger);
bool gtpu_write_header(gtpu_header_t* header, srsran::byte_buffer_t* pdu, srslog::basic_logger& logger);
void gtpu_ntoa(fmt::memory_buffer& buffer, uint32_t addr);

inline bool gtpu_supported_flags_check(gtpu_header_t* header, srslog::basic_logger& logger)
{
  // flags
  if ((header->flags & GTPU_FLAGS_VERSION_MASK) != GTPU_FLAGS_VERSION_V1) {
    logger.error("gtpu_header - Unhandled GTP-U Version. Flags: 0x%x", header->flags);
    return false;
  }
  if (!(header->flags & GTPU_FLAGS_GTP_PROTOCOL)) {
    logger.error("gtpu_header - Unhandled Protocol Type. Flags: 0x%x", header->flags);
    return false;
  }
  if (header->flags & GTPU_FLAGS_PACKET_NUM) {
    logger.error("gtpu_header - Unhandled Packet Number. Flags: 0x%x", header->flags);
    return false;
  }
  return true;
}

inline bool gtpu_supported_msg_type_check(gtpu_header_t* header, srslog::basic_logger& logger)
{
  // msg_tpye
  if (header->message_type != GTPU_MSG_DATA_PDU && header->message_type != GTPU_MSG_ECHO_REQUEST &&
      header->message_type != GTPU_MSG_ECHO_RESPONSE && header->message_type != GTPU_MSG_ERROR_INDICATION &&
      header->message_type != GTPU_MSG_END_MARKER) {
    logger.error("gtpu_header - Unhandled message type: 0x%x", header->message_type);
    return false;
  }
  return true;
}

} // namespace srsran

#endif
