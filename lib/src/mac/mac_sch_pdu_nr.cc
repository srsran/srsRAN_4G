/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsran/mac/mac_sch_pdu_nr.h"

namespace srsran {

mac_sch_subpdu_nr::nr_lcid_sch_t mac_sch_subpdu_nr::get_type()
{
  if (lcid >= 32) {
    return (nr_lcid_sch_t)lcid;
  }

  return CCCH;
}

bool mac_sch_subpdu_nr::is_sdu()
{
  // for UL-SCH LCID 52 is also valid for carrying SDUs
  return (lcid <= 32 || (parent->is_ulsch() && lcid == 52));
}

// returns false for all reserved values in Table 6.2.1-1 and 6.2.1-2
bool mac_sch_subpdu_nr::is_valid_lcid()
{
  return (lcid <= 63 && ((parent->is_ulsch() && (lcid <= 32 || lcid >= 52)) || (lcid <= 32 || lcid >= 47)));
}

bool mac_sch_subpdu_nr::is_var_len_ce()
{
  return false;
}

// return length of PDU (or SRSRAN_ERROR otherwise)
int32_t mac_sch_subpdu_nr::read_subheader(const uint8_t* ptr)
{
  // Skip R, read F bit and LCID
  F_bit = (bool)(*ptr & 0x40) ? true : false;
  lcid  = (uint8_t)*ptr & 0x3f;
  ptr++;
  header_length = 1;

  if (is_valid_lcid()) {
    if ((is_sdu() || is_var_len_ce()) && not is_ul_ccch()) {
      // Read first length byte
      sdu_length = (uint32_t)*ptr;
      ptr++;
      header_length++;

      if (F_bit) {
        // add second length byte
        sdu_length = sdu_length << 8 | ((uint32_t)*ptr & 0xff);
        ptr++;
        header_length++;
      }
    } else {
      sdu_length = sizeof_ce(lcid, parent->is_ulsch());
    }
    sdu = (uint8_t*)ptr;
  } else {
    srslog::fetch_basic_logger("MAC-NR").warning("Invalid LCID (%d) in MAC PDU", lcid);
    return SRSRAN_ERROR;
  }
  return header_length;
}

void mac_sch_subpdu_nr::set_sdu(const uint32_t lcid_, const uint8_t* payload_, const uint32_t len_)
{
  lcid          = lcid_;
  sdu           = const_cast<uint8_t*>(payload_);
  header_length = is_ul_ccch() ? 1 : 2;
  sdu_length    = len_;
  if (is_ul_ccch()) {
    F_bit      = false;
    sdu_length = sizeof_ce(lcid, parent->is_ulsch());
    if (len_ != static_cast<uint32_t>(sdu_length)) {
      logger->warning("Invalid SDU length of UL-SCH SDU (%d != %d)", len_, sdu_length);
    }
  }

  if (sdu_length >= MAC_SUBHEADER_LEN_THRESHOLD) {
    F_bit = true;
    header_length += 1;
  }
}

void mac_sch_subpdu_nr::set_padding(const uint32_t len_)
{
  lcid = PADDING;
  // 1 Byte R/LCID MAC subheader
  sdu_length    = len_ - 1;
  header_length = 1;
}

// Turn a subPDU into a C-RNTI CE, error checking takes place in the caller
void mac_sch_subpdu_nr::set_c_rnti(const uint16_t crnti_)
{
  lcid                  = CRNTI;
  header_length         = 1;
  sdu_length            = sizeof_ce(lcid, parent->is_ulsch());
  sdu                   = ce_write_buffer.data();
  uint16_t crnti        = htole32(crnti_);
  ce_write_buffer.at(0) = (uint8_t)((crnti & 0xff00) >> 8);
  ce_write_buffer.at(1) = (uint8_t)((crnti & 0x00ff));
}

// Turn a subPDU into a single entry PHR CE, error checking takes place in the caller
void mac_sch_subpdu_nr::set_se_phr(const uint8_t phr_, const uint8_t pcmax_)
{
  lcid                  = SE_PHR;
  header_length         = 1;
  sdu_length            = sizeof_ce(lcid, parent->is_ulsch());
  sdu                   = ce_write_buffer.data();
  ce_write_buffer.at(0) = (uint8_t)(phr_ & 0x3f);
  ce_write_buffer.at(1) = (uint8_t)(pcmax_ & 0x3f);
}

// Turn a subPDU into a single short BSR
void mac_sch_subpdu_nr::set_sbsr(const lcg_bsr_t bsr_)
{
  lcid                  = SHORT_BSR;
  header_length         = 1;
  sdu_length            = sizeof_ce(lcid, parent->is_ulsch());
  sdu                   = ce_write_buffer.data();
  ce_write_buffer.at(0) = ((bsr_.lcg_id & 0x07) << 5) | (bsr_.buffer_size & 0x1f);
}

// Turn a subPDU into a long BSR with variable size
void mac_sch_subpdu_nr::set_lbsr(const std::array<mac_sch_subpdu_nr::lcg_bsr_t, max_num_lcg_lbsr> bsr_) {}

// Section 6.1.2
uint32_t mac_sch_subpdu_nr::write_subpdu(const uint8_t* start_)
{
  uint8_t* ptr = const_cast<uint8_t*>(start_);
  *ptr         = (uint8_t)((F_bit ? 1 : 0) << 6) | ((uint8_t)lcid & 0x3f);
  ptr += 1;

  if (header_length == 3) {
    // 3 Byte R/F/LCID/L MAC subheader with 16-bit L field
    *ptr = ((sdu_length & 0xff00) >> 8);
    ptr += 1;
    *ptr = static_cast<uint8_t>(sdu_length);
    ptr += 1;

  } else if (header_length == 2) {
    // 2 Byte R/F/LCID/L MAC subheader with 8-bit L field
    *ptr = static_cast<uint8_t>(sdu_length);
    ptr += 1;
  } else if (header_length == 1) {
    // do nothing
  } else {
    logger->error("Error while packing PDU. Unsupported header length (%d)", header_length);
  }

  // copy SDU payload
  if (sdu) {
    memcpy(ptr, sdu, sdu_length);
  } else {
    // clear memory
    memset(ptr, 0, sdu_length);
  }

  ptr += sdu_length;

  // return total length of subpdu
  return ptr - start_;
}

uint32_t mac_sch_subpdu_nr::get_total_length()
{
  return (header_length + sdu_length);
}

uint32_t mac_sch_subpdu_nr::get_sdu_length()
{
  return sdu_length;
}

uint32_t mac_sch_subpdu_nr::get_lcid()
{
  return lcid;
}

uint8_t* mac_sch_subpdu_nr::get_sdu()
{
  return sdu;
}

uint16_t mac_sch_subpdu_nr::get_c_rnti()
{
  if (parent->is_ulsch() && lcid == CRNTI) {
    return le16toh((uint16_t)sdu[0] << 8 | sdu[1]);
  }
  return 0;
}

uint8_t mac_sch_subpdu_nr::get_phr()
{
  if (parent->is_ulsch() && lcid == SE_PHR) {
    return sdu[0] & 0x3f;
  }
  return 0;
}

uint8_t mac_sch_subpdu_nr::get_pcmax()
{
  if (parent->is_ulsch() && lcid == SE_PHR) {
    return sdu[1] & 0x3f;
  }
  return 0;
}

mac_sch_subpdu_nr::ta_t mac_sch_subpdu_nr::get_ta()
{
  ta_t ta = {};
  if (lcid == TA_CMD) {
    ta.tag_id     = (sdu[0] & 0xc0) >> 6;
    ta.ta_command = sdu[0] & 0x3f;
  }
  return ta;
}

mac_sch_subpdu_nr::lcg_bsr_t mac_sch_subpdu_nr::get_sbsr()
{
  lcg_bsr_t sbsr = {};
  if (parent->is_ulsch() && lcid == SHORT_BSR) {
    sbsr.lcg_id      = (sdu[0] & 0xe0) >> 5;
    sbsr.buffer_size = sdu[0] & 0x1f;
  }
  return sbsr;
}

uint32_t mac_sch_subpdu_nr::sizeof_ce(uint32_t lcid, bool is_ul)
{
  if (is_ul) {
    switch (lcid) {
      case CCCH_SIZE_48:
        return 6;
      case CCCH_SIZE_64:
        return 8;
      case CRNTI:
        return 2;
      case SHORT_TRUNC_BSR:
        return 1;
      case SHORT_BSR:
        return 1;
      case SE_PHR:
        return 2;
      case PADDING:
        return 0;
    }
  } else {
    switch (lcid) {
      case CON_RES_ID:
        return 6;
      case TA_CMD:
        return 1;
      case DRX_CMD:
        return 0;
      case PADDING:
        return 0;
    }
  }
  return 0;
}

inline bool mac_sch_subpdu_nr::is_ul_ccch()
{
  return (parent->is_ulsch() && (lcid == CCCH_SIZE_48 || lcid == CCCH_SIZE_64));
}

void mac_sch_pdu_nr::pack()
{
  // SDU and CEs are written in-place, only add padding if needed
  if (remaining_len) {
    mac_sch_subpdu_nr padding_subpdu(this);
    padding_subpdu.set_padding(remaining_len);
    padding_subpdu.write_subpdu(buffer->msg + buffer->N_bytes);

    // update length
    buffer->N_bytes += padding_subpdu.get_total_length();
    remaining_len -= padding_subpdu.get_total_length();
    subpdus.push_back(padding_subpdu);
  }
}

int mac_sch_pdu_nr::unpack(const uint8_t* payload, const uint32_t& len)
{
  uint32_t offset = 0;
  while (offset < len) {
    mac_sch_subpdu_nr sch_pdu(this);
    if (sch_pdu.read_subheader(payload + offset) == SRSRAN_ERROR) {
      logger.error("Malformed MAC PDU (len=%d, offset=%d)\n", len, offset);
      return SRSRAN_ERROR;
    }
    offset += sch_pdu.get_total_length();
    if (sch_pdu.get_lcid() == mac_sch_subpdu_nr::PADDING) {
      // set SDU length to rest of PDU
      sch_pdu.set_padding(len - offset + 1); // One byte for Padding header will be substracted again
      // skip remaining bytes
      offset = len;
    }
    subpdus.push_back(sch_pdu);
  }
  if (offset != len) {
    logger.error("Malformed MAC PDU (len=%d, offset=%d)\n", len, offset);
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

uint32_t mac_sch_pdu_nr::get_num_subpdus()
{
  return subpdus.size();
}

const mac_sch_subpdu_nr& mac_sch_pdu_nr::get_subpdu(const uint32_t& index)
{
  return subpdus.at(index);
}

bool mac_sch_pdu_nr::is_ulsch()
{
  return ulsch;
}

void mac_sch_pdu_nr::init_tx(byte_buffer_t* buffer_, uint32_t pdu_len_, bool ulsch_)
{
  buffer = buffer_;
  subpdus.clear();
  pdu_len       = pdu_len_;
  remaining_len = pdu_len_;
  ulsch         = ulsch_;
}

void mac_sch_pdu_nr::init_rx(bool ulsch_)
{
  buffer = nullptr;
  subpdus.clear();
  pdu_len       = 0;
  remaining_len = 0;
  ulsch         = ulsch_;
}

uint32_t mac_sch_pdu_nr::size_header_sdu(const uint32_t lcid, const uint32_t nbytes)
{
  if (ulsch && (lcid == mac_sch_subpdu_nr::CCCH_SIZE_48 || lcid == mac_sch_subpdu_nr::CCCH_SIZE_64)) {
    return 1;
  } else {
    if (nbytes < 256) {
      return 2;
    } else {
      return 3;
    }
  }
}

uint32_t mac_sch_pdu_nr::get_remaing_len()
{
  return remaining_len;
}

uint32_t mac_sch_pdu_nr::add_sdu(const uint32_t lcid_, const uint8_t* payload_, const uint32_t len_)
{
  int header_size = size_header_sdu(lcid_, len_);
  if (header_size + len_ > remaining_len) {
    logger.error("Header and SDU exceed space in PDU (%d + %d > %d)", header_size, len_, remaining_len);
    return SRSRAN_ERROR;
  }

  mac_sch_subpdu_nr sch_pdu(this);
  sch_pdu.set_sdu(lcid_, payload_, len_);
  return add_sudpdu(sch_pdu);
}

uint32_t mac_sch_pdu_nr::add_crnti_ce(const uint16_t crnti)
{
  mac_sch_subpdu_nr ce(this);
  ce.set_c_rnti(crnti);
  return add_sudpdu(ce);
}

uint32_t mac_sch_pdu_nr::add_se_phr_ce(const uint8_t phr, const uint8_t pcmax)
{
  mac_sch_subpdu_nr ce(this);
  ce.set_se_phr(phr, pcmax);
  return add_sudpdu(ce);
}

uint32_t mac_sch_pdu_nr::add_sbsr_ce(const mac_sch_subpdu_nr::lcg_bsr_t bsr_)
{
  mac_sch_subpdu_nr ce(this);
  ce.set_sbsr(bsr_);
  return add_sudpdu(ce);
}

uint32_t
mac_sch_pdu_nr::add_lbsr_ce(const std::array<mac_sch_subpdu_nr::lcg_bsr_t, mac_sch_subpdu_nr::max_num_lcg_lbsr> bsr_)
{
  mac_sch_subpdu_nr ce(this);
  ce.set_lbsr(bsr_);
  return add_sudpdu(ce);
}

uint32_t mac_sch_pdu_nr::add_sudpdu(mac_sch_subpdu_nr& subpdu)
{
  uint32_t subpdu_len = subpdu.get_total_length();
  if (subpdu_len > remaining_len) {
    logger.warning("Not enough space to add subPDU to PDU (%d > %d)", subpdu_len, remaining_len);
    return SRSRAN_ERROR;
  }

  // Write subPDU straigt into provided buffer
  subpdu.write_subpdu(buffer->msg + buffer->N_bytes);

  // adopt buffer variables
  buffer->N_bytes += subpdu_len;
  remaining_len -= subpdu_len;
  subpdus.push_back(subpdu);

  return SRSRAN_SUCCESS;
}

} // namespace srsran
