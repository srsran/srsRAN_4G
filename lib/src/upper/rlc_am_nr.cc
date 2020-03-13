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

#include "srslte/upper/rlc_am_nr.h"
#include <sstream>

namespace srslte {

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 38.322 v15.3.0 Section 6.2.2.4
 ***************************************************************************/

uint32_t rlc_am_nr_read_data_pdu_header(const byte_buffer_t*      pdu,
                                        const rlc_am_nr_sn_size_t sn_size,
                                        rlc_am_nr_pdu_header_t*   header)
{
  return rlc_am_nr_read_data_pdu_header(pdu->msg, pdu->N_bytes, sn_size, header);
}

uint32_t rlc_am_nr_read_data_pdu_header(const uint8_t*            payload,
                                        const uint32_t            nof_bytes,
                                        const rlc_am_nr_sn_size_t sn_size,
                                        rlc_am_nr_pdu_header_t*   header)
{
  uint8_t* ptr = const_cast<uint8_t*>(payload);

  header->sn_size = sn_size;

  // Fixed part
  header->dc = (rlc_dc_field_t)((*ptr >> 7) & 0x01);    // 1 bit D/C field
  header->p  = (*ptr >> 6) & 0x01;                      // 1 bit P flag
  header->si = (rlc_nr_si_field_t)((*ptr >> 4) & 0x03); // 2 bits SI

  if (sn_size == rlc_am_nr_sn_size_t::size12bits) {
    header->sn = (*ptr & 0x0F) << 8; // first 4 bits SN
    ptr++;

    header->sn |= (*ptr & 0xFF); // last 8 bits SN
    ptr++;
  } else if (sn_size == rlc_am_nr_sn_size_t::size18bits) {
    // sanity check
    if ((*ptr & 0x0c) != 0) {
      fprintf(stderr, "Malformed PDU, reserved bits are set.\n");
      return 0;
    }
    header->sn = (*ptr & 0x03) << 16; // first 4 bits SN
    ptr++;
    header->sn |= (*ptr & 0xFF) << 8; // bit 2-10 of SN
    ptr++;
    header->sn |= (*ptr & 0xFF); // last 8 bits SN
    ptr++;
  } else {
    fprintf(stderr, "Unsupported SN length\n");
    return 0;
  }

  // Read optional part
  if (header->si == rlc_nr_si_field_t::last_segment ||
      header->si == rlc_nr_si_field_t::neither_first_nor_last_segment) {
    // read SO
    header->so = (*ptr & 0xFF) << 8;
    ptr++;
    header->so |= (*ptr & 0xFF);
    ptr++;
  }

  // return consumed bytes
  return (ptr - payload);
}

uint32_t rlc_am_nr_packed_length(const rlc_am_nr_pdu_header_t& header)
{
  uint32_t len = 0;
  if (header.si == rlc_nr_si_field_t::full_sdu || header.si == rlc_nr_si_field_t::first_segment) {
    len = 2;
    if (header.sn_size == rlc_am_nr_sn_size_t::size18bits) {
      len++;
    }
  } else {
    // PDU contains SO
    len = 4;
    if (header.sn_size == rlc_am_nr_sn_size_t::size18bits) {
      len++;
    }
  }
  return len;
}

uint32_t rlc_am_nr_write_data_pdu_header(const rlc_am_nr_pdu_header_t& header, byte_buffer_t* pdu)
{
  // Make room for the header
  uint32_t len = rlc_am_nr_packed_length(header);
  pdu->msg -= len;
  uint8_t* ptr = pdu->msg;

  // fixed header part
  *ptr = (header.dc & 0x01) << 7;  ///< 1 bit D/C field
  *ptr |= (header.p & 0x01) << 6;  ///< 1 bit P flag
  *ptr |= (header.si & 0x03) << 4; ///< 2 bits SI

  if (header.sn_size == rlc_am_nr_sn_size_t::size12bits) {
    // write first 4 bit of SN
    *ptr |= (header.sn >> 8) & 0x0f; // 4 bit SN
    ptr++;
    *ptr = header.sn & 0xff; // remaining 8 bit of SN
    ptr++;
  } else {
    // 18bit SN
    *ptr |= (header.sn >> 16) & 0x3; // 2 bit SN
    ptr++;
    *ptr = header.sn >> 8; // bit 3 - 10 of SN
    ptr++;
    *ptr = (header.sn & 0xff); // remaining 8 bit of SN
    ptr++;
  }

  if (header.so) {
    // write SO
    *ptr = header.so >> 8; // first part of SO
    ptr++;
    *ptr = (header.so & 0xff); // second part of SO
    ptr++;
  }

  pdu->N_bytes += ptr - pdu->msg;

  return len;
}

uint32_t
rlc_am_nr_read_status_pdu(const byte_buffer_t* pdu, const rlc_am_nr_sn_size_t sn_size, rlc_am_nr_status_pdu_t* status)
{
  return rlc_am_nr_read_status_pdu(pdu->msg, pdu->N_bytes, sn_size, status);
}

uint32_t rlc_am_nr_read_status_pdu(const uint8_t*            payload,
                                   const uint32_t            nof_bytes,
                                   const rlc_am_nr_sn_size_t sn_size,
                                   rlc_am_nr_status_pdu_t*   status)
{
  uint8_t* ptr = const_cast<uint8_t*>(payload);

  // fixed part
  status->cpt = (rlc_am_nr_control_pdu_type_t)((*ptr >> 4) & 0x07); // 3 bits CPT

  // sanity check
  if (status->cpt != rlc_am_nr_control_pdu_type_t::status_pdu) {
    fprintf(stderr, "Malformed PDU, reserved bits are set.\n");
    return 0;
  }

  if (sn_size == rlc_am_nr_sn_size_t::size12bits) {
    status->ack_sn = (*ptr & 0x0F) << 8; // first 4 bits SN
    ptr++;

    status->ack_sn |= (*ptr & 0xFF); // last 8 bits SN
    ptr++;

    // read E1 flag
    uint8_t e1 = *ptr & 0x80;

    // sanity check for reserved bits
    if ((*ptr & 0x7f) != 0) {
      fprintf(stderr, "Malformed PDU, reserved bits are set.\n");
      return 0;
    }

    // all good, continue with next byte depending on E1
    ptr++;

    // reset number of acks
    status->N_nack = 0;

    if (e1) {
      // E1 flag set, read a NACK_SN
      rlc_status_nack_t nack = {};
      nack.nack_sn           = (*ptr & 0xff) << 4;
      ptr++;
      // uint8_t len2 = (*ptr & 0xF0) >> 4;
      nack.nack_sn |= (*ptr & 0xF0) >> 4;
      status->nacks[status->N_nack] = nack;

      status->N_nack++;
    }
  }

  return SRSLTE_SUCCESS;
}

/**
 * Write a RLC AM NR status PDU to a PDU buffer and eets the length of the generate PDU accordingly
 * @param status_pdu The status PDU
 * @param pdu A pointer to a unique bytebuffer
 * @return SRSLTE_SUCCESS if PDU was written, SRSLTE_ERROR otherwise
 */
int32_t rlc_am_nr_write_status_pdu(const rlc_am_nr_status_pdu_t& status_pdu,
                                   const rlc_am_nr_sn_size_t     sn_size,
                                   byte_buffer_t*                pdu)
{
  uint8_t* ptr = pdu->msg;

  // fixed header part
  *ptr = 0; ///< 1 bit D/C field and 3bit CPT are all zero

  if (sn_size == rlc_am_nr_sn_size_t::size12bits) {
    // write first 4 bit of ACK_SN
    *ptr |= (status_pdu.ack_sn >> 8) & 0x0f; // 4 bit ACK_SN
    ptr++;
    *ptr = status_pdu.ack_sn & 0xff; // remaining 8 bit of SN
    ptr++;

    // write E1 flag in octet 3
    *ptr = (status_pdu.N_nack > 0) ? 0x80 : 0x00;
    ptr++;

    if (status_pdu.N_nack > 0) {
      // write first 8 bit of NACK_SN
      *ptr = (status_pdu.nacks[0].nack_sn >> 4) & 0xff;
      ptr++;

      // write remaining 4 bits of NACK_SN
      *ptr = status_pdu.nacks[0].nack_sn & 0xf0;
      ptr++;
    }
  } else {
    // 18bit SN
    *ptr |= (status_pdu.ack_sn >> 14) & 0x0f; // 4 bit ACK_SN
    ptr++;
    *ptr = status_pdu.ack_sn >> 8; // bit 3 - 10 of SN
    ptr++;
    *ptr = (status_pdu.ack_sn & 0xff); // remaining 6 bit of SN
    ptr++;
  }

  pdu->N_bytes = ptr - pdu->msg;

  return SRSLTE_SUCCESS;
}

} // namespace srslte
