/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsran/upper/gtpu.h"
#include "srsran/common/int_helpers.h"
#include <arpa/inet.h>
#include <netinet/in.h>

namespace srsran {

const static size_t HEADER_PDCP_PDU_NUMBER_SIZE = 4;

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 29.281 v10.1.0 Section 5
 ***************************************************************************/
bool gtpu_write_header(gtpu_header_t* header, srsran::byte_buffer_t* pdu, srslog::basic_logger& logger)
{
  // flags
  if (!gtpu_supported_flags_check(header, logger)) {
    logger.error("gtpu_write_header - Unhandled GTP-U Flags. Flags: 0x%x", header->flags);
    return false;
  }

  // msg type
  if (!gtpu_supported_msg_type_check(header, logger)) {
    logger.error("gtpu_write_header - Unhandled GTP-U Message Type. Message Type: 0x%x", header->message_type);
    return false;
  }

  // If E, S or PN are set, the header is longer
  if (header->flags & (GTPU_FLAGS_EXTENDED_HDR | GTPU_FLAGS_SEQUENCE | GTPU_FLAGS_PACKET_NUM)) {
    if (pdu->get_headroom() < GTPU_EXTENDED_HEADER_LEN) {
      logger.error("gtpu_write_header - No room in PDU for header");
      return false;
    }
    pdu->msg -= GTPU_EXTENDED_HEADER_LEN;
    pdu->N_bytes += GTPU_EXTENDED_HEADER_LEN;
    header->length += GTPU_EXTENDED_HEADER_LEN - GTPU_BASE_HEADER_LEN;
    if (header->next_ext_hdr_type > 0) {
      pdu->msg -= header->ext_buffer.size();
      pdu->N_bytes += header->ext_buffer.size();
      header->length += header->ext_buffer.size();
    }
  } else {
    if (pdu->get_headroom() < GTPU_BASE_HEADER_LEN) {
      logger.error("gtpu_write_header - No room in PDU for header");
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
  ptr += 4;
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
      *ptr          = 0;
    }
    ptr++;
    // E
    if (header->flags & GTPU_FLAGS_EXTENDED_HDR) {
      *ptr = header->next_ext_hdr_type;
      ptr++;
      for (size_t i = 0; i < header->ext_buffer.size(); ++i) {
        *ptr = header->ext_buffer[i];
        ptr++;
      }
    } else {
      *ptr = 0;
      ptr++;
    }
  }
  return true;
}

bool gtpu_read_ext_header(srsran::byte_buffer_t* pdu,
                          uint8_t**              ptr,
                          gtpu_header_t*         header,
                          srslog::basic_logger&  logger)
{
  if ((header->flags & GTPU_FLAGS_EXTENDED_HDR) == 0 or header->next_ext_hdr_type == 0) {
    return true;
  }

  // TODO: Iterate over next headers until no more extension headers
  switch (header->next_ext_hdr_type) {
    case GTPU_EXT_HEADER_PDCP_PDU_NUMBER:
      pdu->msg += HEADER_PDCP_PDU_NUMBER_SIZE;
      pdu->N_bytes -= HEADER_PDCP_PDU_NUMBER_SIZE;
      header->ext_buffer.resize(HEADER_PDCP_PDU_NUMBER_SIZE);
      for (size_t i = 0; i < HEADER_PDCP_PDU_NUMBER_SIZE; ++i) {
        header->ext_buffer[i] = **ptr;
        (*ptr)++;
      }
      break;
    case GTPU_EXT_HEADER_PDU_SESSION_CONTAINER:
      pdu->msg += GTPU_EXT_HEADER_PDU_SESSION_CONTAINER_LEN;
      pdu->N_bytes -= GTPU_EXT_HEADER_PDU_SESSION_CONTAINER_LEN;
      // TODO: Save Header Extension
      break;
    default:
      logger.error("gtpu_read_header - Unhandled GTP-U Extension Header Type: 0x%x", header->next_ext_hdr_type);
      return false;
  }
  return true;
}

bool gtpu_read_header(srsran::byte_buffer_t* pdu, gtpu_header_t* header, srslog::basic_logger& logger)
{
  uint8_t* ptr = pdu->msg;

  header->flags = *ptr;
  ptr++;
  header->message_type = *ptr;
  ptr++;
  uint8_to_uint16(ptr, &header->length);
  ptr += 2;
  uint8_to_uint32(ptr, &header->teid);
  ptr += 4;

  // flags
  if (!gtpu_supported_flags_check(header, logger)) {
    logger.error("gtpu_read_header - Unhandled GTP-U Flags. Flags: 0x%x", header->flags);
    return false;
  }

  // message_type
  if (!gtpu_supported_msg_type_check(header, logger)) {
    logger.error("gtpu_read_header - Unhandled GTP-U Message Type. Flags: 0x%x", header->message_type);
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

    if (not gtpu_read_ext_header(pdu, &ptr, header, logger)) {
      return false;
    }
  } else {
    pdu->msg += GTPU_BASE_HEADER_LEN;
    pdu->N_bytes -= GTPU_BASE_HEADER_LEN;
  }

  return true;
}

// Helper function to return a string from IPv4 address for easy printing
void gtpu_ntoa(fmt::memory_buffer& buffer, uint32_t addr)
{
  char           tmp_str[INET_ADDRSTRLEN + 1] = {};
  struct in_addr tmp_addr                     = {};

  tmp_addr.s_addr     = addr;
  const char* tmp_ptr = inet_ntop(AF_INET, &tmp_addr, tmp_str, INET_ADDRSTRLEN);
  if (tmp_ptr == NULL) {
    fmt::format_to(buffer, "Invalid IPv4 address");
  } else {
    fmt::format_to(buffer, "{}", tmp_str);
  }
}

} // namespace srsran
