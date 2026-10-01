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

/**
 * @file enb_rrc_interface_pws.h
 * @brief PWS (Public Warning System) interface for ETWS/CMAS support
 *
 * This interface enables the S1AP layer to communicate PWS messages
 * (Write-Replace-Warning, Kill) to the RRC layer for broadcast via
 * SIB10 (ETWS primary), SIB11 (ETWS secondary), and SIB12 (CMAS).
 *
 * References:
 * - 3GPP TS 36.413: S1AP specification (WriteReplaceWarningRequest/Response)
 * - 3GPP TS 36.331: RRC specification (SIB10, SIB11, SIB12)
 * - 3GPP TS 23.041: CBS technical realization
 */

#ifndef SRSRAN_ENB_RRC_INTERFACE_PWS_H
#define SRSRAN_ENB_RRC_INTERFACE_PWS_H

#include <cstdint>
#include <vector>
#include <string>

namespace srsenb {

/**
 * @brief PWS warning message type
 */
enum class pws_warning_type_e {
  ETWS_EARTHQUAKE        = 0,
  ETWS_TSUNAMI           = 1,
  ETWS_EARTHQUAKE_TSUNAMI = 2,
  ETWS_TEST              = 3,
  ETWS_OTHER             = 4,
  CMAS_PRESIDENTIAL      = 5,
  CMAS_EXTREME_IMMEDIATE = 6,
  CMAS_EXTREME_EXPECTED  = 7,
  CMAS_SEVERE_IMMEDIATE  = 8,
  CMAS_SEVERE_EXPECTED   = 9,
  CMAS_AMBER             = 10,
  CMAS_MONTHLY_TEST      = 11,
  CMAS_EXERCISE          = 12,
  CMAS_OPERATOR_DEFINED  = 13,
  CMAS_PUBLIC_SAFETY     = 14,
  CMAS_STATE_LOCAL_TEST  = 15
};

/**
 * @brief PWS warning message information
 *
 * This structure contains all parameters needed to broadcast
 * a PWS warning message via SIB10/11/12.
 */
struct pws_warning_info_t {
  // Message identification (16 bits)
  // For ETWS: identifies the type of emergency
  // For CMAS: identifies the alert category
  uint16_t message_id = 0;

  // Serial number (16 bits)
  // Bits 0-9: Message code
  // Bits 10-13: Geographic scope
  // Bits 14-15: Update number
  uint16_t serial_number = 0;

  // Warning type (2 octets) - ETWS only
  // Bits 0-6: Warning type value
  // Bit 7: Emergency user alert
  // Bits 8-15: Popup indicator
  bool     warning_type_present = false;
  uint16_t warning_type         = 0;

  // Data coding scheme (1 octet)
  // Specifies the encoding of the warning message
  bool    data_coding_scheme_present = false;
  uint8_t data_coding_scheme         = 0;

  // Warning message content
  // For SIB11 (ETWS): up to 9600 octets
  // For SIB12 (CMAS): up to 9600 octets
  bool                  warning_msg_contents_present = false;
  std::vector<uint8_t>  warning_msg_contents;

  // Repetition period in seconds
  // Value range: 0-4095 (basic) or 4096-131071 (extended)
  uint32_t repetition_period = 0;

  // Number of broadcast requests
  // 0 = broadcast until explicitly cancelled
  uint32_t number_of_broadcasts = 0;

  // Security information (50 octets) - ETWS only
  // Contains digital signature for message authentication
  bool                 warning_security_info_present = false;
  std::vector<uint8_t> warning_security_info;

  // Concurrent warning message indicator
  bool concurrent_warning_msg_indicator = false;

  // Helper to determine if this is ETWS or CMAS
  bool is_etws() const { return message_id >= 0x1100 && message_id <= 0x1107; }
  bool is_cmas() const { return message_id >= 0x1112 && message_id <= 0x112F; }
};

/**
 * @brief PWS kill request information
 */
struct pws_kill_info_t {
  uint16_t message_id     = 0;
  uint16_t serial_number  = 0;
  bool     kill_all       = false;  // Stop-All-Indicator
};

/**
 * @brief Result of PWS operation
 */
struct pws_result_t {
  bool success = false;
  // List of cells where broadcast failed (if any)
  std::vector<uint32_t> failed_cell_list;
  // Unknown TAI list
  std::vector<uint16_t> unknown_tai_list;
};

} // namespace srsenb

#endif // SRSRAN_ENB_RRC_INTERFACE_PWS_H
