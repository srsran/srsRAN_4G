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

#ifndef SRSRAN_MAC_RAR_PDU_NR_H
#define SRSRAN_MAC_RAR_PDU_NR_H

#include "srsran/common/common.h"
#include "srsran/config.h"
#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/srslog/srslog.h"
#include <memory>
#include <stdint.h>
#include <vector>

namespace srsran {

class mac_rar_pdu_nr;

// 3GPP 38.321 v15.3.0 Sec 6.1.5
class mac_rar_subpdu_nr
{
public:
  // Possible types of RAR subpdus (same like EUTRA)
  typedef enum { BACKOFF = 0, RAPID } rar_subh_type_t;

  mac_rar_subpdu_nr(mac_rar_pdu_nr* parent_);

  // RAR content length in bits (38.321 Sec 6.2.3)
  static const uint32_t UL_GRANT_NBITS   = SRSRAN_RAR_UL_GRANT_NBITS;
  static const uint32_t TA_COMMAND_NBITS = 12;

  // getter
  bool     read_subpdu(const uint8_t* ptr);
  bool     has_more_subpdus();
  uint32_t get_total_length();
  bool                                has_rapid() const;
  uint8_t                             get_rapid() const;
  uint16_t                            get_temp_crnti() const;
  uint32_t                            get_ta() const;
  std::array<uint8_t, UL_GRANT_NBITS> get_ul_grant() const;
  bool                                has_backoff() const;
  uint8_t                             get_backoff() const;

  // setter
  uint32_t write_subpdu(const uint8_t* start_);
  void     set_backoff(const uint8_t backoff_indicator_);

  std::string to_string();

private:
  int header_length  = 1; // RAR PDU subheader is always 1 B
  int payload_length = 0; // only used if MAC RAR is included

  std::array<uint8_t, UL_GRANT_NBITS> ul_grant          = {};
  uint16_t                            ta                = 0; // 12bit TA
  uint16_t                            temp_crnti        = 0;
  uint16_t                            rapid             = 0;
  uint8_t                             backoff_indicator = 0;
  rar_subh_type_t                     type              = BACKOFF;
  bool                                E_bit             = 0;

  srslog::basic_logger& logger;

  mac_rar_pdu_nr* parent = nullptr;
};

class mac_rar_pdu_nr
{
public:
  mac_rar_pdu_nr();
  ~mac_rar_pdu_nr() = default;

  bool                     pack();
  bool                     unpack(const uint8_t* payload, const uint32_t& len);
  uint32_t                 get_num_subpdus();
  // Returns reference to a single subPDU
  const mac_rar_subpdu_nr& get_subpdu(const uint32_t& index);
  // Returns reference to all subPDUs
  const std::vector<mac_rar_subpdu_nr>& get_subpdus();

  uint32_t get_remaining_len();

  void set_si_rapid(uint16_t si_rapid_); // configured through SIB1 for on-demand SI request (See 38.331 Sec 5.2.1)
  bool has_si_rapid();

  std::string to_string();

private:
  std::vector<mac_rar_subpdu_nr> subpdus;
  uint32_t                       remaining_len = 0;
  uint16_t                       si_rapid      = 0;
  bool                           si_rapid_set  = false;
  srslog::basic_logger&          logger;
};

} // namespace srsran

#endif // SRSRAN_MAC_RAR_PDU_NR_H
