/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of srsLTE.
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

#ifndef SRSENB_COMMON_ENB_H
#define SRSENB_COMMON_ENB_H

/*******************************************************************************
                              INCLUDES
*******************************************************************************/

#include <stdint.h>

namespace srsenb {

#define ENB_METRICS_MAX_USERS  64
  
#define SRSENB_RRC_MAX_N_PLMN_IDENTITIES 6

#define SRSENB_N_SRB           3
#define SRSENB_N_DRB           8
#define SRSENB_N_RADIO_BEARERS 11

typedef enum{
  RB_ID_SRB0 = 0,
  RB_ID_SRB1,
  RB_ID_SRB2,
  RB_ID_DRB1,
  RB_ID_DRB2,
  RB_ID_DRB3,
  RB_ID_DRB4,
  RB_ID_DRB5,
  RB_ID_DRB6,
  RB_ID_DRB7,
  RB_ID_DRB8,
  RB_ID_N_ITEMS,
}rb_id_t;
static const char rb_id_text[RB_ID_N_ITEMS][20] = { "SRB0",
                                                    "SRB1",
                                                    "SRB2",
                                                    "DRB1",
                                                    "DRB2",
                                                    "DRB3",
                                                    "DRB4",
                                                    "DRB5",
                                                    "DRB6",
                                                    "DRB7",
                                                    "DRB8"};

// Cat 3 UE - Max number of DL-SCH transport block bits received within a TTI
// 3GPP 36.306 Table 4.1.1
#define SRSENB_MAX_BUFFER_SIZE_BITS  102048
#define SRSENB_MAX_BUFFER_SIZE_BYTES 12756
#define SRSENB_BUFFER_HEADER_OFFSET  1024

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

/******************************************************************************
 * Safe conversions between byte buffers and integer types.
 * Note: these don't perform endian conversion - use e.g. htonl/ntohl if required
 *****************************************************************************/

inline void uint8_to_uint32(uint8_t *buf, uint32_t *i)
{
  *i =  (uint32_t)buf[0] << 24 |
        (uint32_t)buf[1] << 16 |
        (uint32_t)buf[2] << 8  |
        (uint32_t)buf[3];
}

inline void uint32_to_uint8(uint32_t i, uint8_t *buf)
{
  buf[0] = (i >> 24) & 0xFF;
  buf[1] = (i >> 16) & 0xFF;
  buf[2] = (i >> 8) & 0xFF;
  buf[3] = i & 0xFF;
}

inline void uint8_to_uint16(uint8_t *buf, uint16_t *i)
{
  *i =  (uint32_t)buf[0] << 8  |
        (uint32_t)buf[1];
}

inline void uint16_to_uint8(uint16_t i, uint8_t *buf)
{
  buf[0] = (i >> 8) & 0xFF;
  buf[1] = i & 0xFF;
}

} // namespace srsenb

#endif // SRSENB_COMMON_ENB_H
