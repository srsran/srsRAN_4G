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

#include "srsran/mac/mac_rar_pdu_nr.h"
#include <sstream>
#ifdef __cplusplus
extern "C" {
#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/vector.h"
}
#endif

namespace srsran {

mac_rar_subpdu_nr::mac_rar_subpdu_nr(mac_rar_pdu_nr* parent_) :
  parent(parent_), logger(srslog::fetch_basic_logger("MAC-NR"))
{}

// Return true if subPDU could be parsed correctly, false otherwise
bool mac_rar_subpdu_nr::read_subpdu(const uint8_t* ptr)
{
  E_bit = (bool)(*ptr & 0x80) ? true : false;
  type  = (*ptr & 0x40) ? RAPID : BACKOFF;
  if (type == RAPID) {
    rapid = *ptr & 0x3f;
    // if PDU is not configured with SI request, extract MAC RAR
    if (parent->has_si_rapid() == false) {
      if (parent->get_remaining_len() >= MAC_RAR_NBYTES) {
        uint8_t* rar = const_cast<uint8_t*>(ptr + 1);
        // check reserved bits
        if (*rar & 0x80) {
          logger.error("Error parsing RAR PDU, reserved bit is set.");
          return false;
        }
        // TA is first 7 bits of 1st + 5 bits of 2nd octet
        ta = (uint16_t)(((*(rar + 0) & 0x7f) << 5 | ((*(rar + 1) & 0xf8) >> 3)));
        // Extract the first 3 bits of the UL grant from the 2nd octet
        ul_grant.at(0) = *(rar + 1) & 0x4 ? 1 : 0;
        ul_grant.at(1) = *(rar + 1) & 0x2 ? 1 : 0;
        ul_grant.at(2) = *(rar + 1) & 0x1 ? 1 : 0;
        // And now the remaining 3 full octets
        uint8_t* x = &ul_grant.at(3);
        srsran_bit_unpack(*(rar + 2), &x, 8);
        srsran_bit_unpack(*(rar + 3), &x, 8);
        srsran_bit_unpack(*(rar + 4), &x, 8);
        // Temp CRNTI is octet 6 + 7
        temp_crnti     = ((uint16_t) * (rar + 5)) << 8 | *(rar + 6);
        payload_length = MAC_RAR_NBYTES;
      } else {
        logger.error("Error parsing RAR PDU, remaining bytes not sufficant (%d < %d)",
                     parent->get_remaining_len(),
                     MAC_RAR_NBYTES);
        return false;
      }
    }
  } else {
    // check reserved bits
    if (*ptr & 0x10 || *ptr & 0x20) {
      logger.error("Error parsing RAR PDU, reserved bit is set.");
      return false;
    }
    backoff_indicator = *ptr & 0xf;
  }
  return true;
}

// Return true if another subPDU follows after that
bool mac_rar_subpdu_nr::has_more_subpdus()
{
  return E_bit;
}

// Section 6.1.2
void mac_rar_subpdu_nr::write_subpdu(const uint8_t* start_)
{
  uint8_t* ptr = const_cast<uint8_t*>(start_);

  if (type == RAPID) {
    // write E/T/RAPID MAC subheader
    *ptr = (uint8_t)((E_bit ? 1 : 0) << 7) | ((int)rar_subh_type_t::RAPID << 6) | ((uint8_t)rapid & 0x3f);
    ptr += 1;

    // if PDU is not configured with SI request, insert MAC RAR
    if (parent->has_si_rapid() == false) {
      // high 7 bits of TA go into first octet
      *ptr = (uint8_t)((ta >> 5) & 0x7f);
      ptr += 1;

      // low 5 bit of TA and first 3 bit of UL grant
      *ptr = (uint8_t)((ta & 0x1f) << 3) | (ul_grant.at(0) << 2) | (ul_grant.at(1) << 1) | (ul_grant.at(2));
      ptr += 1;

      // add remaining 3 full octets of UL grant
      uint8_t* x = &ul_grant.at(3);
      *(ptr + 0) = (uint8_t)srsran_bit_pack(&x, 8);
      *(ptr + 1) = (uint8_t)srsran_bit_pack(&x, 8);
      *(ptr + 2) = (uint8_t)srsran_bit_pack(&x, 8);
      ptr += 3;

      // 2 byte C-RNTI
      *(ptr + 0) = (uint8_t)((temp_crnti & 0xff00) >> 8);
      *(ptr + 1) = (uint8_t)(temp_crnti & 0x00ff);
      ptr += 2;
    }
  } else {
    // write E/T/R/R/BI MAC subheader
    *ptr = (uint8_t)((E_bit ? 1 : 0) << 7) | ((int)rar_subh_type_t::BACKOFF << 6) | ((uint8_t)backoff_indicator & 0xf);
    ptr += 1;
  }
}

uint32_t mac_rar_subpdu_nr::get_total_length()
{
  return (header_length + payload_length);
}

bool mac_rar_subpdu_nr::has_rapid() const
{
  return (type == rar_subh_type_t::RAPID);
}

uint8_t mac_rar_subpdu_nr::get_rapid() const
{
  return rapid;
}

uint32_t mac_rar_subpdu_nr::get_ta() const
{
  return ta;
}

uint16_t mac_rar_subpdu_nr::get_temp_crnti() const
{
  return temp_crnti;
}

bool mac_rar_subpdu_nr::has_backoff() const
{
  return (type == rar_subh_type_t::BACKOFF);
}

void mac_rar_subpdu_nr::set_backoff(const uint8_t backoff_indicator_)
{
  type              = rar_subh_type_t::BACKOFF;
  payload_length    = 0;
  backoff_indicator = backoff_indicator_;
}

uint8_t mac_rar_subpdu_nr::get_backoff() const
{
  return backoff_indicator;
}

std::array<uint8_t, mac_rar_subpdu_nr::UL_GRANT_NBITS> mac_rar_subpdu_nr::get_ul_grant() const
{
  return ul_grant;
}

void mac_rar_subpdu_nr::set_ta(const uint32_t ta_)
{
  ta = ta_;
}

void mac_rar_subpdu_nr::set_temp_crnti(const uint16_t temp_crnti_)
{
  temp_crnti = temp_crnti_;
}

void mac_rar_subpdu_nr::set_rapid(const uint8_t rapid_)
{
  type           = rar_subh_type_t::RAPID;
  payload_length = MAC_RAR_NBYTES;
  rapid          = rapid_;
}

void mac_rar_subpdu_nr::set_ul_grant(std::array<uint8_t, mac_rar_subpdu_nr::UL_GRANT_NBITS> ul_grant_)
{
  ul_grant = ul_grant_;
}

void mac_rar_subpdu_nr::set_is_last_subpdu()
{
  E_bit = false;
}

void mac_rar_subpdu_nr::to_string(fmt::memory_buffer& buffer)
{
  // Add space for new subPDU
  fmt::format_to(buffer, " ");

  if (has_rapid()) {
    char tmp[16] = {};
    srsran_vec_sprint_hex(tmp, sizeof(tmp), ul_grant.data(), UL_GRANT_NBITS);
    fmt::format_to(buffer, "RAPID: {}, Temp C-RNTI: {:#04x}, TA: {}, UL Grant: {}", rapid, temp_crnti, ta, tmp);
  } else {
    fmt::format_to(buffer, "Backoff Indicator: {}", backoff_indicator);
  }
}

mac_rar_pdu_nr::mac_rar_pdu_nr() : logger(srslog::fetch_basic_logger("MAC-NR")) {}

int mac_rar_pdu_nr::init_tx(byte_buffer_t* buffer_, uint32_t pdu_len_)
{
  if (buffer_ == nullptr || buffer_->msg == nullptr) {
    logger.error("Invalid buffer");
    return SRSRAN_ERROR;
  }
  buffer          = buffer_;
  buffer->N_bytes = 0;
  subpdus.clear();
  pdu_len       = pdu_len_;
  remaining_len = pdu_len_;
  return SRSRAN_SUCCESS;
}

mac_rar_subpdu_nr& mac_rar_pdu_nr::add_subpdu()
{
  mac_rar_subpdu_nr rar_subpdu(this);
  subpdus.push_back(rar_subpdu);
  return subpdus.back();
}

int mac_rar_pdu_nr::pack()
{
  int ret = SRSRAN_ERROR;
  if (buffer == nullptr) {
    logger.error("Invalid buffer");
    return ret;
  }

  // set E_bit for last subPDU
  subpdus.back().set_is_last_subpdu();

  // write subPDUs one by one
  for (uint32_t i = 0; i < subpdus.size(); ++i) {
    mac_rar_subpdu_nr& subpdu = subpdus.at(i);
    if (remaining_len >= subpdu.get_total_length()) {
      subpdu.write_subpdu(buffer->msg + buffer->N_bytes);
      buffer->N_bytes += subpdu.get_total_length();
      remaining_len -= subpdu.get_total_length();
    } else {
      logger.error("Not enough space in PDU to write subPDU");
      return ret;
    }
  }

  // fill up with padding, if any
  if (remaining_len > 0) {
    memset(buffer->msg + buffer->N_bytes, 0, remaining_len);
    buffer->N_bytes += remaining_len;
  }

  ret = SRSRAN_SUCCESS;

  return ret;
}

bool mac_rar_pdu_nr::has_si_rapid()
{
  return si_rapid_set;
}

void mac_rar_pdu_nr::set_si_rapid(uint16_t si_rapid_)
{
  si_rapid     = si_rapid_;
  si_rapid_set = true;
}

// Return true if PDU could be parsed successfully
bool mac_rar_pdu_nr::unpack(const uint8_t* payload, const uint32_t& len)
{
  bool     ret               = false;
  bool     have_more_subpdus = false;
  uint32_t offset            = 0;
  bool     success           = false;

  remaining_len = len;

  do {
    mac_rar_subpdu_nr rar_subpdu(this);
    success           = rar_subpdu.read_subpdu(payload + offset);
    have_more_subpdus = rar_subpdu.has_more_subpdus();
    offset += rar_subpdu.get_total_length();
    remaining_len -= rar_subpdu.get_total_length();

    // only append if subPDU could be read successfully
    if (success == true) {
      subpdus.push_back(rar_subpdu);
    }
    ret |= success;
    // continue reading as long as subPDUs can be extracted ok and we are not overrunning the PDU length
  } while (success && have_more_subpdus && offset <= len);

  return ret;
}

uint32_t mac_rar_pdu_nr::get_num_subpdus()
{
  return subpdus.size();
}

const mac_rar_subpdu_nr& mac_rar_pdu_nr::get_subpdu(const uint32_t& index)
{
  return subpdus.at(index);
}

const std::vector<mac_rar_subpdu_nr>& mac_rar_pdu_nr::get_subpdus()
{
  return subpdus;
}

uint32_t mac_rar_pdu_nr::get_remaining_len()
{
  return remaining_len;
}

void mac_rar_pdu_nr::to_string(fmt::memory_buffer& buffer)
{
  fmt::format_to(buffer, "DL");
  for (auto& subpdu : subpdus) {
    subpdu.to_string(buffer);
  }
}

} // namespace srsran
