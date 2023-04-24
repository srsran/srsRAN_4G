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

#include "srsran/rlc/rlc_am_nr_packing.h"
#include <sstream>

namespace srsran {

/****************************************************************************
 * Container implementation for pack/unpack functions
 ***************************************************************************/

rlc_am_nr_status_pdu_t::rlc_am_nr_status_pdu_t(rlc_am_nr_sn_size_t sn_size) :
  sn_size(sn_size), mod_nr(cardinality(sn_size))
{
  nacks_.reserve(RLC_AM_NR_TYP_NACKS);
}

void rlc_am_nr_status_pdu_t::reset()
{
  cpt    = rlc_am_nr_control_pdu_type_t::status_pdu;
  ack_sn = INVALID_RLC_SN;
  nacks_.clear();
  packed_size_ = rlc_am_nr_status_pdu_sizeof_header_ack_sn;
}

bool rlc_am_nr_status_pdu_t::is_continuous_sequence(const rlc_status_nack_t& left, const rlc_status_nack_t& right) const
{
  // SN must be continuous
  if (right.nack_sn != ((left.has_nack_range ? left.nack_sn + left.nack_range : (left.nack_sn + 1)) % mod_nr)) {
    return false;
  }

  // Segments on left side (if present) must reach the end of sdu
  if (left.has_so && left.so_end != rlc_status_nack_t::so_end_of_sdu) {
    return false;
  }

  // Segments on right side (if present) must start from the beginning
  if (right.has_so && right.so_start != 0) {
    return false;
  }

  return true;
}

void rlc_am_nr_status_pdu_t::push_nack(const rlc_status_nack_t& nack)
{
  if (nacks_.size() == 0) {
    nacks_.push_back(nack);
    packed_size_ += nack_size(nack);
    return;
  }

  rlc_status_nack_t& prev = nacks_.back();
  if (is_continuous_sequence(prev, nack) == false) {
    nacks_.push_back(nack);
    packed_size_ += nack_size(nack);
    return;
  }

  // expand previous NACK
  // subtract size of previous NACK (add updated size later)
  packed_size_ -= nack_size(prev);

  // enable and update NACK range
  if (nack.has_nack_range == true) {
    if (prev.has_nack_range == true) {
      // [NACK range][NACK range]
      prev.nack_range += nack.nack_range;
    } else {
      // [NACK SDU][NACK range]
      prev.nack_range     = nack.nack_range + 1;
      prev.has_nack_range = true;
    }
  } else {
    if (prev.has_nack_range == true) {
      // [NACK range][NACK SDU]
      prev.nack_range++;
    } else {
      // [NACK SDU][NACK SDU]
      prev.nack_range     = 2;
      prev.has_nack_range = true;
    }
  }

  // enable and update segment offsets (if required)
  if (nack.has_so == true) {
    if (prev.has_so == false) {
      // [NACK SDU][NACK segm]
      prev.has_so   = true;
      prev.so_start = 0;
    }
    // [NACK SDU][NACK segm] or [NACK segm][NACK segm]
    prev.so_end = nack.so_end;
  } else {
    if (prev.has_so == true) {
      // [NACK segm][NACK SDU]
      prev.so_end = rlc_status_nack_t::so_end_of_sdu;
    }
    // [NACK segm][NACK SDU] or [NACK SDU][NACK SDU]
  }

  // add updated size
  packed_size_ += nack_size(prev);
}

bool rlc_am_nr_status_pdu_t::trim(uint32_t max_packed_size)
{
  if (max_packed_size >= packed_size_) {
    // no trimming required
    return true;
  }
  if (max_packed_size < rlc_am_nr_status_pdu_sizeof_header_ack_sn) {
    // too little space for smallest possible status PDU (only header + ACK).
    return false;
  }

  // remove NACKs (starting from the back) until it fits into given space
  // note: when removing a NACK for a segment, we have to remove all other NACKs with the same SN as well,
  // see TS 38.322 Sec. 5.3.4:
  //   "set the ACK_SN to the SN of the next not received RLC SDU
  //   which is not indicated as missing in the resulting STATUS PDU."
  while (nacks_.size() > 0 && (max_packed_size < packed_size_ || nacks_.back().nack_sn == ack_sn)) {
    packed_size_ -= nack_size(nacks_.back());
    ack_sn = nacks_.back().nack_sn;
    nacks_.pop_back();
  }
  return true;
}

void rlc_am_nr_status_pdu_t::refresh_packed_size()
{
  packed_size_ = rlc_am_nr_status_pdu_sizeof_header_ack_sn;
  for (auto nack : nacks_) {
    packed_size_ += nack_size(nack);
  }
}

uint32_t rlc_am_nr_status_pdu_t::nack_size(const rlc_status_nack_t& nack) const
{
  uint32_t result = sn_size == rlc_am_nr_sn_size_t::size12bits ? rlc_am_nr_status_pdu_sizeof_nack_sn_ext_12bit_sn
                                                               : rlc_am_nr_status_pdu_sizeof_nack_sn_ext_18bit_sn;
  if (nack.has_so) {
    result += rlc_am_nr_status_pdu_sizeof_nack_so;
  }
  if (nack.has_nack_range) {
    result += rlc_am_nr_status_pdu_sizeof_nack_range;
  }
  return result;
}

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

uint32_t rlc_am_nr_write_data_pdu_header(const rlc_am_nr_pdu_header_t& header, uint8_t* payload)
{
  uint8_t* ptr = payload;

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
  return rlc_am_nr_packed_length(header);
}

uint32_t rlc_am_nr_write_data_pdu_header(const rlc_am_nr_pdu_header_t& header, byte_buffer_t* pdu)
{
  // Make room for the header
  uint32_t len = rlc_am_nr_packed_length(header);
  pdu->msg -= len;
  pdu->N_bytes += len;
  rlc_am_nr_write_data_pdu_header(header, pdu->msg);
  return len;
}

/****************************************************************************
 * Status PDU pack/unpack helper functions
 * Ref: 3GPP TS 38.322 v16.2.0 Section 6.2.2.5
 ***************************************************************************/

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
  if (sn_size == rlc_am_nr_sn_size_t::size12bits) {
    return rlc_am_nr_read_status_pdu_12bit_sn(payload, nof_bytes, status);
  } else { // 18bit SN
    return rlc_am_nr_read_status_pdu_18bit_sn(payload, nof_bytes, status);
  }
}

uint32_t
rlc_am_nr_read_status_pdu_12bit_sn(const uint8_t* payload, const uint32_t nof_bytes, rlc_am_nr_status_pdu_t* status)
{
  uint8_t* ptr = const_cast<uint8_t*>(payload);
  status->reset();

  // fixed part
  status->cpt = (rlc_am_nr_control_pdu_type_t)((*ptr >> 4) & 0x07); // 3 bits CPT

  // sanity check
  if (status->cpt != rlc_am_nr_control_pdu_type_t::status_pdu) {
    fprintf(stderr, "Malformed PDU, reserved bits are set.\n");
    return 0;
  }

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

  while (e1 != 0) {
    // check buffer headroom
    if (uint32_t(ptr - payload) >= nof_bytes) {
      fprintf(stderr, "Malformed PDU, trying to read more bytes than it is available\n");
      return 0;
    }

    // E1 flag set, read a NACK_SN
    rlc_status_nack_t nack = {};
    nack.nack_sn           = (*ptr & 0xff) << 4;
    ptr++;

    e1         = *ptr & 0x08; // 1 = further NACKs follow
    uint8_t e2 = *ptr & 0x04; // 1 = set of {so_start, so_end} follows
    uint8_t e3 = *ptr & 0x02; // 1 = NACK range follows (i.e. NACK across multiple SNs)

    // sanity check for reserved bits
    if ((*ptr & 0x01) != 0) {
      fprintf(stderr, "Malformed PDU, reserved bits are set.\n");
      return 0;
    }
    nack.nack_sn |= (*ptr & 0xF0) >> 4;

    ptr++;
    if (e2 != 0) {
      nack.has_so   = true;
      nack.so_start = (*ptr) << 8;
      ptr++;
      nack.so_start |= (*ptr);
      ptr++;
      nack.so_end = (*ptr) << 8;
      ptr++;
      nack.so_end |= (*ptr);
      ptr++;
    }
    if (e3 != 0) {
      nack.has_nack_range = true;
      nack.nack_range     = (*ptr);
      ptr++;
    }
    status->push_nack(nack);
  }

  return SRSRAN_SUCCESS;
}

uint32_t
rlc_am_nr_read_status_pdu_18bit_sn(const uint8_t* payload, const uint32_t nof_bytes, rlc_am_nr_status_pdu_t* status)
{
  uint8_t* ptr = const_cast<uint8_t*>(payload);
  status->reset();

  // fixed part
  status->cpt = (rlc_am_nr_control_pdu_type_t)((*ptr >> 4) & 0x07); // 3 bits CPT

  // sanity check
  if (status->cpt != rlc_am_nr_control_pdu_type_t::status_pdu) {
    fprintf(stderr, "Malformed PDU, reserved bits are set.\n");
    return 0;
  }

  status->ack_sn = (*ptr & 0x0F) << 14; // upper 4 bits of SN
  ptr++;

  status->ack_sn |= (*ptr & 0xFF) << 6; // center 8 bits of SN
  ptr++;

  status->ack_sn |= (*ptr & 0xFC) >> 2; // lower 6 bits of SN

  // read E1 flag
  uint8_t e1 = *ptr & 0x02;

  // sanity check for reserved bits
  if ((*ptr & 0x01) != 0) {
    fprintf(stderr, "Malformed PDU, reserved bit is set.\n");
    return 0;
  }

  // all good, continue with next byte depending on E1
  ptr++;

  while (e1 != 0) {
    // check buffer headroom
    if (uint32_t(ptr - payload) >= nof_bytes) {
      fprintf(stderr, "Malformed PDU, trying to read more bytes than it is available\n");
      return 0;
    }

    // E1 flag set, read a NACK_SN
    rlc_status_nack_t nack = {};

    nack.nack_sn = (*ptr & 0xFF) << 10; // upper 8 bits of SN
    ptr++;
    nack.nack_sn |= (*ptr & 0xFF) << 2; // center 8 bits of SN
    ptr++;
    nack.nack_sn |= (*ptr & 0xC0) >> 6; // lower 2 bits of SN

    e1         = *ptr & 0x20; // 1 = further NACKs follow
    uint8_t e2 = *ptr & 0x10; // 1 = set of {so_start, so_end} follows
    uint8_t e3 = *ptr & 0x08; // 1 = NACK range follows (i.e. NACK across multiple SNs)

    // sanity check for reserved bits
    if ((*ptr & 0x07) != 0) {
      fprintf(stderr, "Malformed PDU, reserved bits are set.\n");
      return 0;
    }

    ptr++;
    if (e2 != 0) {
      nack.has_so   = true;
      nack.so_start = (*ptr) << 8;
      ptr++;
      nack.so_start |= (*ptr);
      ptr++;
      nack.so_end = (*ptr) << 8;
      ptr++;
      nack.so_end |= (*ptr);
      ptr++;
    }
    if (e3 != 0) {
      nack.has_nack_range = true;
      nack.nack_range     = (*ptr);
      ptr++;
    }
    status->push_nack(nack);
  }

  return SRSRAN_SUCCESS;
}

/**
 * Write a RLC AM NR status PDU to a PDU buffer and eets the length of the generate PDU accordingly
 * @param status_pdu The status PDU
 * @param pdu A pointer to a unique bytebuffer
 * @return SRSRAN_SUCCESS if PDU was written, SRSRAN_ERROR otherwise
 */
int32_t rlc_am_nr_write_status_pdu(const rlc_am_nr_status_pdu_t& status_pdu,
                                   const rlc_am_nr_sn_size_t     sn_size,
                                   byte_buffer_t*                pdu)
{
  if (sn_size == rlc_am_nr_sn_size_t::size12bits) {
    return rlc_am_nr_write_status_pdu_12bit_sn(status_pdu, pdu);
  } else { // 18bit SN
    return rlc_am_nr_write_status_pdu_18bit_sn(status_pdu, pdu);
  }
}

int32_t rlc_am_nr_write_status_pdu_12bit_sn(const rlc_am_nr_status_pdu_t& status_pdu, byte_buffer_t* pdu)
{
  uint8_t* ptr = pdu->msg;

  // fixed header part
  *ptr = 0; ///< 1 bit D/C field and 3bit CPT are all zero

  // write first 4 bit of ACK_SN
  *ptr |= (status_pdu.ack_sn >> 8) & 0x0f; // 4 bit ACK_SN
  ptr++;
  *ptr = status_pdu.ack_sn & 0xff; // remaining 8 bit of SN
  ptr++;

  // write E1 flag in octet 3
  if (status_pdu.nacks.size() > 0) {
    *ptr = 0x80;
  } else {
    *ptr = 0x00;
  }
  ptr++;

  if (status_pdu.nacks.size() > 0) {
    for (uint32_t i = 0; i < status_pdu.nacks.size(); i++) {
      // write first 8 bit of NACK_SN
      *ptr = (status_pdu.nacks[i].nack_sn >> 4) & 0xff;
      ptr++;

      // write remaining 4 bits of NACK_SN
      *ptr = (status_pdu.nacks[i].nack_sn & 0x0f) << 4;
      // Set E1 if necessary
      if (i < (uint32_t)(status_pdu.nacks.size() - 1)) {
        *ptr |= 0x08;
      }

      if (status_pdu.nacks[i].has_so) {
        // Set E2
        *ptr |= 0x04;
      }

      if (status_pdu.nacks[i].has_nack_range) {
        // Set E3
        *ptr |= 0x02;
      }

      ptr++;
      if (status_pdu.nacks[i].has_so) {
        (*ptr) = status_pdu.nacks[i].so_start >> 8;
        ptr++;
        (*ptr) = status_pdu.nacks[i].so_start;
        ptr++;
        (*ptr) = status_pdu.nacks[i].so_end >> 8;
        ptr++;
        (*ptr) = status_pdu.nacks[i].so_end;
        ptr++;
      }
      if (status_pdu.nacks[i].has_nack_range) {
        (*ptr) = status_pdu.nacks[i].nack_range;
        ptr++;
      }
    }
  }

  pdu->N_bytes = ptr - pdu->msg;

  return SRSRAN_SUCCESS;
}

int32_t rlc_am_nr_write_status_pdu_18bit_sn(const rlc_am_nr_status_pdu_t& status_pdu, byte_buffer_t* pdu)
{
  uint8_t* ptr = pdu->msg;

  // fixed header part
  *ptr = 0; ///< 1 bit D/C field and 3bit CPT are all zero

  *ptr |= (status_pdu.ack_sn >> 14) & 0x0F; // upper 4 bits of SN
  ptr++;
  *ptr = (status_pdu.ack_sn >> 6) & 0xFF; // center 8 bits of SN
  ptr++;
  *ptr = (status_pdu.ack_sn << 2) & 0xFC; // lower 6 bits of SN

  // set E1 flag if necessary
  if (status_pdu.nacks.size() > 0) {
    *ptr |= 0x02;
  }
  ptr++;

  if (status_pdu.nacks.size() > 0) {
    for (uint32_t i = 0; i < status_pdu.nacks.size(); i++) {
      *ptr = (status_pdu.nacks[i].nack_sn >> 10) & 0xFF; // upper 8 bits of SN
      ptr++;
      *ptr = (status_pdu.nacks[i].nack_sn >> 2) & 0xFF; // center 8 bits of SN
      ptr++;
      *ptr = (status_pdu.nacks[i].nack_sn << 6) & 0xC0; // lower 2 bits of SN

      if (i < (uint32_t)(status_pdu.nacks.size() - 1)) {
        *ptr |= 0x20; // Set E1
      }
      if (status_pdu.nacks[i].has_so) {
        *ptr |= 0x10; // Set E2
      }
      if (status_pdu.nacks[i].has_nack_range) {
        *ptr |= 0x08; // Set E3
      }

      ptr++;
      if (status_pdu.nacks[i].has_so) {
        (*ptr) = status_pdu.nacks[i].so_start >> 8;
        ptr++;
        (*ptr) = status_pdu.nacks[i].so_start;
        ptr++;
        (*ptr) = status_pdu.nacks[i].so_end >> 8;
        ptr++;
        (*ptr) = status_pdu.nacks[i].so_end;
        ptr++;
      }
      if (status_pdu.nacks[i].has_nack_range) {
        (*ptr) = status_pdu.nacks[i].nack_range;
        ptr++;
      }
    }
  }

  pdu->N_bytes = ptr - pdu->msg;

  return SRSRAN_SUCCESS;
}

} // namespace srsran
