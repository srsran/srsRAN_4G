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

#include "srsran/rlc/rlc_am_lte_packing.h"
#include <sstream>

namespace srsran {

/****************************************************************************
 * Header pack/unpack helper functions
 * Ref: 3GPP TS 36.322 v10.0.0 Section 6.2.1
 ***************************************************************************/

// Read header from pdu struct, don't strip header
void rlc_am_read_data_pdu_header(byte_buffer_t* pdu, rlc_amd_pdu_header_t* header)
{
  uint8_t* ptr = pdu->msg;
  uint32_t n   = 0;
  rlc_am_read_data_pdu_header(&ptr, &n, header);
}

// Read header from raw pointer, strip header
void rlc_am_read_data_pdu_header(uint8_t** payload, uint32_t* nof_bytes, rlc_amd_pdu_header_t* header)
{
  uint8_t  ext;
  uint8_t* ptr = *payload;

  header->dc = static_cast<rlc_dc_field_t>((*ptr >> 7) & 0x01);

  if (RLC_DC_FIELD_DATA_PDU == header->dc) {
    // Fixed part
    header->rf = ((*ptr >> 6) & 0x01);
    header->p  = ((*ptr >> 5) & 0x01);
    header->fi = static_cast<rlc_fi_field_t>((*ptr >> 3) & 0x03);
    ext        = ((*ptr >> 2) & 0x01);
    header->sn = (*ptr & 0x03) << 8; // 2 bits SN
    ptr++;
    header->sn |= (*ptr & 0xFF); // 8 bits SN
    ptr++;

    if (header->rf) {
      header->lsf = ((*ptr >> 7) & 0x01);
      header->so  = (*ptr & 0x7F) << 8; // 7 bits of SO
      ptr++;
      header->so |= (*ptr & 0xFF); // 8 bits of SO
      ptr++;
    }

    // Extension part
    header->N_li = 0;
    while (ext) {
      if (header->N_li % 2 == 0) {
        ext                      = ((*ptr >> 7) & 0x01);
        header->li[header->N_li] = (*ptr & 0x7F) << 4; // 7 bits of LI
        ptr++;
        header->li[header->N_li] |= (*ptr & 0xF0) >> 4; // 4 bits of LI
        header->N_li++;
      } else {
        ext                      = (*ptr >> 3) & 0x01;
        header->li[header->N_li] = (*ptr & 0x07) << 8; // 3 bits of LI
        ptr++;
        header->li[header->N_li] |= (*ptr & 0xFF); // 8 bits of LI
        header->N_li++;
        ptr++;
      }
    }

    // Account for padding if N_li is odd
    if (header->N_li % 2 == 1) {
      ptr++;
    }

    *nof_bytes -= ptr - *payload;
    *payload = ptr;
  }
}

// Write header to pdu struct
void rlc_am_write_data_pdu_header(rlc_amd_pdu_header_t* header, byte_buffer_t* pdu)
{
  uint8_t* ptr = pdu->msg;
  rlc_am_write_data_pdu_header(header, &ptr);
  pdu->N_bytes += ptr - pdu->msg;
}

// Write header to pointer & move pointer
void rlc_am_write_data_pdu_header(rlc_amd_pdu_header_t* header, uint8_t** payload)
{
  uint32_t i;
  uint8_t  ext = (header->N_li > 0) ? 1 : 0;

  uint8_t* ptr = *payload;

  // Fixed part
  *ptr = (header->dc & 0x01) << 7;
  *ptr |= (header->rf & 0x01) << 6;
  *ptr |= (header->p & 0x01) << 5;
  *ptr |= (header->fi & 0x03) << 3;
  *ptr |= (ext & 0x01) << 2;

  *ptr |= (header->sn & 0x300) >> 8; // 2 bits SN
  ptr++;
  *ptr = (header->sn & 0xFF); // 8 bits SN
  ptr++;

  // Segment part
  if (header->rf) {
    *ptr = (header->lsf & 0x01) << 7;
    *ptr |= (header->so & 0x7F00) >> 8; // 7 bits of SO
    ptr++;
    *ptr = (header->so & 0x00FF); // 8 bits of SO
    ptr++;
  }

  // Extension part
  i = 0;
  while (i < header->N_li) {
    ext  = ((i + 1) == header->N_li) ? 0 : 1;
    *ptr = (ext & 0x01) << 7;             // 1 bit header
    *ptr |= (header->li[i] & 0x7F0) >> 4; // 7 bits of LI
    ptr++;
    *ptr = (header->li[i] & 0x00F) << 4; // 4 bits of LI
    i++;
    if (i < header->N_li) {
      ext = ((i + 1) == header->N_li) ? 0 : 1;
      *ptr |= (ext & 0x01) << 3;            // 1 bit header
      *ptr |= (header->li[i] & 0x700) >> 8; // 3 bits of LI
      ptr++;
      *ptr = (header->li[i] & 0x0FF); // 8 bits of LI
      ptr++;
      i++;
    }
  }
  // Pad if N_li is odd
  if (header->N_li % 2 == 1) {
    ptr++;
  }

  *payload = ptr;
}

void rlc_am_read_status_pdu(byte_buffer_t* pdu, rlc_status_pdu_t* status)
{
  rlc_am_read_status_pdu(pdu->msg, pdu->N_bytes, status);
}

void rlc_am_read_status_pdu(uint8_t* payload, uint32_t nof_bytes, rlc_status_pdu_t* status)
{
  uint32_t     i;
  uint8_t      ext1, ext2;
  bit_buffer_t tmp;
  uint8_t*     ptr = tmp.msg;

  srsran_bit_unpack_vector(payload, tmp.msg, nof_bytes * 8);
  tmp.N_bits = nof_bytes * 8;

  rlc_dc_field_t dc = static_cast<rlc_dc_field_t>(srsran_bit_pack(&ptr, 1));

  if (RLC_DC_FIELD_CONTROL_PDU == dc) {
    uint8_t cpt = srsran_bit_pack(&ptr, 3); // 3-bit Control PDU Type (0 == status)
    if (0 == cpt) {
      status->ack_sn = srsran_bit_pack(&ptr, 10); // 10 bits ACK_SN
      ext1           = srsran_bit_pack(&ptr, 1);  // 1 bits E1
      status->N_nack = 0;
      while (ext1) {
        status->nacks[status->N_nack].nack_sn = srsran_bit_pack(&ptr, 10);
        ext1                                  = srsran_bit_pack(&ptr, 1); // 1 bits E1
        ext2                                  = srsran_bit_pack(&ptr, 1); // 1 bits E2
        if (ext2) {
          status->nacks[status->N_nack].has_so   = true;
          status->nacks[status->N_nack].so_start = srsran_bit_pack(&ptr, 15);
          status->nacks[status->N_nack].so_end   = srsran_bit_pack(&ptr, 15);
        }
        status->N_nack++;
      }
    }
  }
}

void rlc_am_write_status_pdu(rlc_status_pdu_t* status, byte_buffer_t* pdu)
{
  pdu->N_bytes = rlc_am_write_status_pdu(status, pdu->msg);
}

int rlc_am_write_status_pdu(rlc_status_pdu_t* status, uint8_t* payload)
{
  uint32_t     i;
  uint8_t      ext1;
  bit_buffer_t tmp;
  uint8_t*     ptr = tmp.msg;

  srsran_bit_unpack(RLC_DC_FIELD_CONTROL_PDU, &ptr, 1); // D/C
  srsran_bit_unpack(0, &ptr, 3);                        // CPT (0 == STATUS)
  srsran_bit_unpack(status->ack_sn, &ptr, 10);          // 10 bit ACK_SN
  ext1 = (status->N_nack == 0) ? 0 : 1;
  srsran_bit_unpack(ext1, &ptr, 1); // E1
  for (i = 0; i < status->N_nack; i++) {
    srsran_bit_unpack(status->nacks[i].nack_sn, &ptr, 10); // 10 bit NACK_SN
    ext1 = ((status->N_nack - 1) == i) ? 0 : 1;
    srsran_bit_unpack(ext1, &ptr, 1); // E1
    if (status->nacks[i].has_so) {
      srsran_bit_unpack(1, &ptr, 1); // E2
      srsran_bit_unpack(status->nacks[i].so_start, &ptr, 15);
      srsran_bit_unpack(status->nacks[i].so_end, &ptr, 15);
    } else {
      srsran_bit_unpack(0, &ptr, 1); // E2
    }
  }

  // Pad
  tmp.N_bits    = ptr - tmp.msg;
  uint8_t n_pad = 8 - (tmp.N_bits % 8);
  srsran_bit_unpack(0, &ptr, n_pad);
  tmp.N_bits = ptr - tmp.msg;

  // Pack bits
  srsran_bit_pack_vector(tmp.msg, payload, tmp.N_bits);
  return tmp.N_bits / 8;
}

uint32_t rlc_am_packed_length(rlc_amd_pdu_header_t* header)
{
  uint32_t len = 2; // Fixed part is 2 bytes
  if (header->rf) {
    len += 2; // Segment header is 2 bytes
  }
  len += header->N_li * 1.5 + 0.5; // Extension part - integer rounding up
  return len;
}

uint32_t rlc_am_packed_length(rlc_status_pdu_t* status)
{
  uint32_t len_bits = 15; // Fixed part is 15 bits
  for (uint32_t i = 0; i < status->N_nack; i++) {
    if (status->nacks[i].has_so) {
      len_bits += 42; // 10 bits SN, 2 bits ext, 15 bits so_start, 15 bits so_end
    } else {
      len_bits += 12; // 10 bits SN, 2 bits ext
    }
  }

  return (len_bits + 7) / 8; // Convert to bytes - integer rounding up
}

bool rlc_am_is_pdu_segment(uint8_t* payload)
{
  return ((*(payload) >> 6) & 0x01) == 1;
}

bool rlc_am_is_valid_status_pdu(const rlc_status_pdu_t& status, uint32_t rx_win_min)
{
  // check if ACK_SN is inside Rx window
  if ((MOD + status.ack_sn - rx_win_min) % MOD > RLC_AM_WINDOW_SIZE) {
    return false;
  }

  for (uint32_t i = 0; i < status.N_nack; ++i) {
    // NACK can't be larger than ACK
    if ((MOD + status.ack_sn - status.nacks[i].nack_sn) % MOD > RLC_AM_WINDOW_SIZE) {
      return false;
    }
    // Don't NACK the ACK SN
    if (status.nacks[i].nack_sn == status.ack_sn) {
      return false;
    }
  }
  return true;
}

void rlc_am_undelivered_sdu_info_to_string(fmt::memory_buffer&                                      buffer,
                                           const std::vector<pdcp_pdu_info<rlc_amd_pdu_header_t> >& info_queue)
{
  fmt::format_to(buffer, "\n");
  for (const auto& pdcp_pdu : info_queue) {
    fmt::format_to(buffer, "\tPDCP_SN = {}, undelivered RLC SNs = [", pdcp_pdu.sn);
    for (const auto& nacked_segment : pdcp_pdu) {
      fmt::format_to(buffer, "{} ", nacked_segment.rlc_sn());
    }
    fmt::format_to(buffer, "]\n");
  }
}

bool rlc_am_start_aligned(const uint8_t fi)
{
  return (fi == RLC_FI_FIELD_START_AND_END_ALIGNED || fi == RLC_FI_FIELD_NOT_END_ALIGNED);
}

bool rlc_am_end_aligned(const uint8_t fi)
{
  return (fi == RLC_FI_FIELD_START_AND_END_ALIGNED || fi == RLC_FI_FIELD_NOT_START_ALIGNED);
}

bool rlc_am_is_unaligned(const uint8_t fi)
{
  return (fi == RLC_FI_FIELD_NOT_START_OR_END_ALIGNED);
}

bool rlc_am_not_start_aligned(const uint8_t fi)
{
  return (fi == RLC_FI_FIELD_NOT_START_ALIGNED || fi == RLC_FI_FIELD_NOT_START_OR_END_ALIGNED);
}

} // namespace srsran
