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

#include "srslte/upper/gtpu.h"
#include "srslte/common/int_helpers.h"

namespace srslte {

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 29.281 v10.1.0 Section 5
 ***************************************************************************/
bool gtpu_write_header(gtpu_header_t* header, srslte::byte_buffer_t* pdu, srslte::log_ref gtpu_log)
{
  // flags
  if (!gtpu_supported_flags_check(header, gtpu_log)) {
    gtpu_log->error("gtpu_write_header - Unhandled GTP-U Flags. Flags: 0x%x\n", header->flags);
    return false;
  }

  // msg type
  if (!gtpu_supported_msg_type_check(header, gtpu_log)) {
    gtpu_log->error("gtpu_write_header - Unhandled GTP-U Message Type. Message Type: 0x%x\n", header->message_type);
    return false;
  }

  // If E, S or PN are set, the header is longer
  if (header->flags & (GTPU_FLAGS_EXTENDED_HDR | GTPU_FLAGS_SEQUENCE | GTPU_FLAGS_PACKET_NUM)) {
    if (pdu->get_headroom() < GTPU_EXTENDED_HEADER_LEN) {
      gtpu_log->error("gtpu_write_header - No room in PDU for header\n");
      return false;
    }
    pdu->msg -= GTPU_EXTENDED_HEADER_LEN;
    pdu->N_bytes += GTPU_EXTENDED_HEADER_LEN;
  } else {
    if (pdu->get_headroom() < GTPU_BASE_HEADER_LEN) {
      gtpu_log->error("gtpu_write_header - No room in PDU for header\n");
      return false;
    }
    pdu->msg -= GTPU_BASE_HEADER_LEN;
    pdu->N_bytes += GTPU_BASE_HEADER_LEN;
  }

  // write mandatory fields
  uint8_t* ptr = pdu->msg;
  *ptr         = header->flags;
  ptr++;
  *ptr = header->message_type;
  ptr++;
  uint16_to_uint8(header->length, ptr);
  ptr += 2;
  uint32_to_uint8(header->teid, ptr);
  // write optional fields, if E, S or PN are set.
  if (header->flags & (GTPU_FLAGS_EXTENDED_HDR | GTPU_FLAGS_SEQUENCE | GTPU_FLAGS_PACKET_NUM)) {
    // S
    if (header->flags & GTPU_FLAGS_SEQUENCE) {
      uint16_to_uint8(header->seq_number, ptr);
    } else {
      uint16_to_uint8(0, ptr);
    }
    ptr += 2;
    // PN
    if (header->flags & GTPU_FLAGS_PACKET_NUM) {
      *ptr = header->n_pdu;
    } else {
      header->n_pdu = 0;
    }
    ptr++;
    // E
    if (header->flags & GTPU_FLAGS_EXTENDED_HDR) {
      *ptr = header->next_ext_hdr_type;
    } else {
      *ptr = 0;
    }
    ptr++;
  }
  return true;
}

bool gtpu_read_header(srslte::byte_buffer_t* pdu, gtpu_header_t* header, srslte::log_ref gtpu_log)
{
  uint8_t* ptr = pdu->msg;

  header->flags = *ptr;
  ptr++;
  header->message_type = *ptr;
  ptr++;
  uint8_to_uint16(ptr, &header->length);
  ptr += 2;
  uint8_to_uint32(ptr, &header->teid);

  // flags
  if (!gtpu_supported_flags_check(header, gtpu_log)) {
    gtpu_log->error("gtpu_read_header - Unhandled GTP-U Flags. Flags: 0x%x\n", header->flags);
    return false;
  }

  // message_type
  if (!gtpu_supported_msg_type_check(header, gtpu_log)) {
    gtpu_log->error("gtpu_read_header - Unhandled GTP-U Message Type. Flags: 0x%x\n", header->message_type);
    return false;
  }

  // If E, S or PN are set, header is longer
  if (header->flags & (GTPU_FLAGS_EXTENDED_HDR | GTPU_FLAGS_SEQUENCE | GTPU_FLAGS_PACKET_NUM)) {
    pdu->msg += GTPU_EXTENDED_HEADER_LEN;
    pdu->N_bytes -= GTPU_EXTENDED_HEADER_LEN;

    uint8_to_uint16(ptr, &header->seq_number);
    ptr += 2;

    header->n_pdu = *ptr;
    ptr++;

    header->next_ext_hdr_type = *ptr;
    ptr++;
  } else {
    pdu->msg += GTPU_BASE_HEADER_LEN;
    pdu->N_bytes -= GTPU_BASE_HEADER_LEN;
  }

  return true;
}

// Helper function to return a string from IPv4 address for easy printing
std::string gtpu_ntoa(uint32_t addr)
{
  char           tmp_str[INET_ADDRSTRLEN + 1] = {};
  struct in_addr tmp_addr                     = {};

  tmp_addr.s_addr     = addr;
  const char* tmp_ptr = inet_ntop(AF_INET, &tmp_addr, tmp_str, INET_ADDRSTRLEN);
  if (tmp_ptr == NULL) {
    return std::string("Invalid IPv4 address");
  }
  return std::string(tmp_str);
}

} // namespace srslte
