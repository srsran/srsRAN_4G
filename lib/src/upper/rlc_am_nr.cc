/*
 * Copyright 2013-2019 Software Radio Systems Limited
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
    if (header->sn > 3) {
      fprintf(stderr, "Malformed PDU, reserved bits are set.\n");
      return 0;
    }
    header->sn |= (*ptr & 0xFF); // bit 2-10 of SN
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
    *ptr |= (header.sn & 0x3); // 2 bit SN
    ptr++;
    *ptr = (header.sn) >> 8; // bit 3 - 10 of SN
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

} // namespace srslte
