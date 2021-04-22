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
      const uint32_t MAC_RAR_NBYTES = 7;
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
uint32_t mac_rar_subpdu_nr::write_subpdu(const uint8_t* start_)
{
  return 0;
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

std::string mac_rar_subpdu_nr::to_string()
{
  std::stringstream ss;
  if (has_rapid()) {
    ss << "RAPID: " << rapid << ", Temp C-RNTI: " << std::hex << temp_crnti << ", TA: " << ta << ", UL Grant: ";
  } else {
    ss << "Backoff Indicator: " << backoff_indicator << " ";
  }

  char tmp[16] = {};
  srsran_vec_sprint_hex(tmp, sizeof(tmp), ul_grant.data(), UL_GRANT_NBITS);
  ss << tmp;

  return ss.str();
}

mac_rar_pdu_nr::mac_rar_pdu_nr() : logger(srslog::fetch_basic_logger("MAC-NR")) {}

bool mac_rar_pdu_nr::pack()
{
  // not implemented yet
  return false;
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

  remaining_len = len;

  do {
    mac_rar_subpdu_nr rar_subpdu(this);
    ret               = rar_subpdu.read_subpdu(payload + offset);
    have_more_subpdus = rar_subpdu.has_more_subpdus();
    offset += rar_subpdu.get_total_length();
    remaining_len -= rar_subpdu.get_total_length();

    // only append if subPDU could be read successfully
    if (ret == true) {
      subpdus.push_back(rar_subpdu);
    }
    // continue reading as long as subPDUs can be extracted ok and we are not overrunning the PDU length
  } while (ret && have_more_subpdus && offset <= len);

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

std::string mac_rar_pdu_nr::to_string()
{
  std::stringstream ss;
  for (auto& subpdu : subpdus) {
    ss << subpdu.to_string() << " ";
  }
  return ss.str();
}

} // namespace srsran
