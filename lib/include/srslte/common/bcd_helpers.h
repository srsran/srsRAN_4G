/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSLTE_BCD_HELPERS_H
#define SRSLTE_BCD_HELPERS_H

#include <ctype.h>
#include <stdint.h>
#include <string>
#include <srslte/asn1/liblte_rrc.h>

namespace srslte {

/******************************************************************************
 * Convert between string and BCD-coded MCC.
 * Digits are represented by 4-bit nibbles. Unused nibbles are filled with 0xF.
 * MCC 001 results in 0xF001
 *****************************************************************************/
inline bool string_to_mcc(std::string str, uint16_t *mcc)
{
  uint32_t len = str.size();
  if(len != 3) {
    return false;
  }
  if(!isdigit(str[0]) || !isdigit(str[1]) || !isdigit(str[2])) {
    return false;
  }
  *mcc = 0xF000;
  *mcc |= ((uint8_t)(str[0]-'0') << 8);
  *mcc |= ((uint8_t)(str[1]-'0') << 4);
  *mcc |= ((uint8_t)(str[2]-'0'));
  return true;
}

inline bool mcc_to_string(uint16_t mcc, std::string *str)
{
  if((mcc & 0xF000) != 0xF000) {
    return false;
  }
  *str = "";
  *str += ((mcc & 0x0F00) >> 8) + '0';
  *str += ((mcc & 0x00F0) >> 4) + '0';
  *str += (mcc & 0x000F) + '0';
  return true;
}

/******************************************************************************
 * Convert between string and BCD-coded MNC.
 * Digits are represented by 4-bit nibbles. Unused nibbles are filled with 0xF.
 * MNC 001 results in 0xF001
 * MNC 01 results in 0xFF01
 *****************************************************************************/
inline bool string_to_mnc(std::string str, uint16_t *mnc)
{
  uint32_t len = str.size();
  if(len != 3 && len != 2) {
    return false;
  }
  if(len == 3) {
    if(!isdigit(str[0]) || !isdigit(str[1]) || !isdigit(str[2])) {
      return false;
    }
    *mnc = 0xF000;
    *mnc |= ((uint8_t)(str[0]-'0') << 8);
    *mnc |= ((uint8_t)(str[1]-'0') << 4);
    *mnc |= ((uint8_t)(str[2]-'0'));
  }
  if(len == 2) {
    if(!isdigit(str[0]) || !isdigit(str[1])) {
      return false;
    }
    *mnc = 0xFF00;
    *mnc |= ((uint8_t)(str[0]-'0') << 4);
    *mnc |= ((uint8_t)(str[1]-'0'));
  }

  return true;
}

inline bool mnc_to_string(uint16_t mnc, std::string *str)
{
  if((mnc & 0xF000) != 0xF000) {
    return false;
  }
  *str = "";
  if((mnc & 0xFF00) != 0xFF00) {
    *str += ((mnc & 0x0F00) >> 8) + '0';
  }
  *str += ((mnc & 0x00F0) >> 4) + '0';
  *str += (mnc & 0x000F) + '0';
  return true;
}
inline std::string plmn_id_to_string(LIBLTE_RRC_PLMN_IDENTITY_STRUCT plmn_id) {
  std::string mcc_str, mnc_str;
  mnc_to_string(plmn_id.mnc, &mnc_str);
  mcc_to_string(plmn_id.mcc, &mcc_str);
  return mcc_str + mnc_str;
}

/******************************************************************************
 * Convert PLMN to BCD-coded MCC and MNC.
 * Digits are represented by 4-bit nibbles. Unused nibbles are filled with 0xF.
 * MNC 001 represented as 0xF001
 * MNC 01 represented as 0xFF01
 * PLMN encoded as per TS 36.413 sec 9.2.3.8
 *****************************************************************************/
inline void s1ap_plmn_to_mccmnc(uint32_t plmn, uint16_t *mcc, uint16_t *mnc)
{
  uint8_t nibbles[6];
  nibbles[0] = (plmn & 0xF00000) >> 20;
  nibbles[1] = (plmn & 0x0F0000) >> 16;
  nibbles[2] = (plmn & 0x00F000) >> 12;
  nibbles[3] = (plmn & 0x000F00) >> 8;
  nibbles[4] = (plmn & 0x0000F0) >> 4;
  nibbles[5] = (plmn & 0x00000F);

  *mcc = 0xF000;
  *mnc = 0xF000;
  *mcc |= nibbles[1] << 8;    // MCC digit 1
  *mcc |= nibbles[0] << 4;    // MCC digit 2
  *mcc |= nibbles[3];         // MCC digit 3

  if(nibbles[2] == 0xF) {
    // 2-digit MNC
    *mnc |= 0x0F00;           // MNC digit 1
    *mnc |= nibbles[5] << 4;  // MNC digit 2
    *mnc |= nibbles[4];       // MNC digit 3
  } else {
    // 3-digit MNC
    *mnc |= nibbles[5] << 8;  // MNC digit 1
    *mnc |= nibbles[4] << 4;  // MNC digit 2
    *mnc |= nibbles[2] ;      // MNC digit 3
  }
}

/******************************************************************************
 * Convert BCD-coded MCC and MNC to PLMN.
 * Digits are represented by 4-bit nibbles. Unused nibbles are filled with 0xF.
 * MNC 001 represented as 0xF001
 * MNC 01 represented as 0xFF01
 * PLMN encoded as per TS 36.413 sec 9.2.3.8
 *****************************************************************************/
inline void s1ap_mccmnc_to_plmn(uint16_t mcc, uint16_t mnc, uint32_t *plmn)
{
  uint8_t nibbles[6];
  nibbles[1] = (mcc & 0x0F00) >> 8; // MCC digit 1
  nibbles[0] = (mcc & 0x00F0) >> 4; // MCC digit 2
  nibbles[3] = (mcc & 0x000F);      // MCC digit 3

  if((mnc & 0xFF00) == 0xFF00) {
    // 2-digit MNC
    nibbles[2] = 0x0F;                // MNC digit 1
    nibbles[5] = (mnc & 0x00F0) >> 4; // MNC digit 2
    nibbles[4] = (mnc & 0x000F);      // MNC digit 3
  } else {
    // 3-digit MNC
    nibbles[5] = (mnc & 0x0F00) >> 8; // MNC digit 1
    nibbles[4] = (mnc & 0x00F0) >> 4; // MNC digit 2
    nibbles[2] = (mnc & 0x000F);      // MNC digit 3
  }

  *plmn = 0x000000;
  *plmn |= nibbles[0] << 20;
  *plmn |= nibbles[1] << 16;
  *plmn |= nibbles[2] << 12;
  *plmn |= nibbles[3] << 8;
  *plmn |= nibbles[4] << 4;
  *plmn |= nibbles[5];
}


} // namespace srslte

#endif // SRSLTE_BCD_HELPERS_H
