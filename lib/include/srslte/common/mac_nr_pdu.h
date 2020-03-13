/*
 * Copyright 2013-2020 Software Radio Systems Limited
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

#ifndef SRSLTE_MAC_NR_PDU_H
#define SRSLTE_MAC_NR_PDU_H

#include "srslte/common/common.h"
#include <memory>
#include <stdint.h>
#include <vector>

namespace srslte {

class mac_nr_sch_pdu;

class mac_nr_sch_subpdu
{
public:
  // 3GPP 38.321 v15.3.0 Combined Tables 6.2.1-1, 6.2.1-2
  typedef enum {
    // Values for DL-SCH
    CCCH       = 0b000000,
    DRX_CMD    = 0b111100,
    TA_CMD     = 0b111101,
    CON_RES_ID = 0b111110,

    // Values for UL-SCH
    CRNTI           = 0b111010,
    SHORT_TRUNC_BSR = 0b111011,
    LONG_TRUNC_BSR  = 0b111100,
    CCCH_SIZE_48    = 0b110100,
    CCCH_SIZE_64    = 0b000000,

    SHORT_BSR = 0b111101,
    LONG_BSR  = 0b111110,

    // Common
    PADDING = 0b111111,
  } nr_lcid_sch_t;

  mac_nr_sch_subpdu(mac_nr_sch_pdu* parent_);

  nr_lcid_sch_t get_type();
  bool          is_sdu();
  bool          is_valid_lcid();
  bool          is_var_len_ce();
  bool          is_ul_ccch();

  int32_t  read_subheader(const uint8_t* ptr);
  uint32_t get_total_length();
  uint32_t get_sdu_length();
  uint32_t get_lcid();
  uint8_t* get_sdu();

  void set_sdu(const uint32_t lcid_, const uint8_t* payload_, const uint32_t len_);

  void set_padding(const uint32_t len_);

  uint32_t write_subpdu(const uint8_t* start_);

private:
  uint32_t sizeof_ce(uint32_t lcid, bool is_ul);

  // protected:
  uint32_t lcid          = 0;
  int      header_length = 0;
  int      sdu_length    = 0;
  bool     F_bit         = false;
  uint8_t* sdu           = nullptr;

  mac_nr_sch_pdu* parent = nullptr;
};

class mac_nr_sch_pdu
{
public:
  mac_nr_sch_pdu(bool ulsch_ = false) : ulsch(ulsch_) {}

  void                     pack();
  void                     unpack(const uint8_t* payload, const uint32_t& len);
  uint32_t                 get_num_subpdus();
  const mac_nr_sch_subpdu& get_subpdu(const uint32_t& index);
  bool                     is_ulsch();

  void init_tx(byte_buffer_t* buffer_, uint32_t pdu_len_, bool is_ulsch_ = false);

  uint32_t add_sdu(const uint32_t lcid_, const uint8_t* payload_, const uint32_t len_);

  uint32_t get_remaing_len();

private:
  uint32_t size_header_sdu(const uint32_t lcid_, const uint32_t nbytes);

  bool                           ulsch = false;
  std::vector<mac_nr_sch_subpdu> subpdus;

  byte_buffer_t* buffer        = nullptr;
  uint32_t       pdu_len       = 0;
  uint32_t       remaining_len = 0;
};

} // namespace srslte

#endif // SRSLTE_MAC_NR_PDU_H
