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

#ifndef SRSLTE_RLC_INTERFACE_TYPES_H
#define SRSLTE_RLC_INTERFACE_TYPES_H

#include "srslte/interfaces/rrc_interface_types.h"

/***************************
 *      RLC Config
 **************************/

namespace srslte {

enum class rlc_mode_t { tm, um, am, nulltype };
inline std::string to_string(const rlc_mode_t& mode, bool long_txt = true)
{
  constexpr static const char* long_options[]  = {"Transparent Mode", "Unacknowledged Mode", "Acknowledged Mode"};
  constexpr static const char* short_options[] = {"TM", "UM", "AM"};
  if (long_txt) {
    return enum_to_text(long_options, (uint32_t)rlc_mode_t::nulltype, (uint32_t)mode);
  }
  return enum_to_text(short_options, (uint32_t)rlc_mode_t::nulltype, (uint32_t)mode);
}

enum class rlc_umd_sn_size_t { size5bits, size10bits, nulltype };
inline std::string to_string(const rlc_umd_sn_size_t& sn_size)
{
  constexpr static const char* options[] = {"5 bits", "10 bits"};
  return enum_to_text(options, (uint32_t)rlc_mode_t::nulltype, (uint32_t)sn_size);
}
inline uint16_t to_number(const rlc_umd_sn_size_t& sn_size)
{
  constexpr static uint16_t options[] = {5, 10};
  return enum_to_number(options, (uint32_t)rlc_mode_t::nulltype, (uint32_t)sn_size);
}

///< RLC UM NR sequence number field
enum class rlc_um_nr_sn_size_t { size6bits, size12bits, nulltype };
inline std::string to_string(const rlc_um_nr_sn_size_t& sn_size)
{
  constexpr static const char* options[] = {"6 bits", "12 bits"};
  return enum_to_text(options, (uint32_t)rlc_mode_t::nulltype, (uint32_t)sn_size);
}
inline uint16_t to_number(const rlc_um_nr_sn_size_t& sn_size)
{
  constexpr static uint16_t options[] = {6, 12};
  return enum_to_number(options, (uint32_t)rlc_mode_t::nulltype, (uint32_t)sn_size);
}

///< RLC AM NR sequence number field
enum class rlc_am_nr_sn_size_t { size12bits, size18bits, nulltype };
inline std::string to_string(const rlc_am_nr_sn_size_t& sn_size)
{
  constexpr static const char* options[] = {"12 bits", "18 bits"};
  return enum_to_text(options, (uint32_t)rlc_mode_t::nulltype, (uint32_t)sn_size);
}
inline uint16_t to_number(const rlc_am_nr_sn_size_t& sn_size)
{
  constexpr static uint16_t options[] = {12, 18};
  return enum_to_number(options, (uint32_t)rlc_mode_t::nulltype, (uint32_t)sn_size);
}

struct rlc_am_config_t {
  /****************************************************************************
   * Configurable parameters
   * Ref: 3GPP TS 36.322 v10.0.0 Section 7
   ***************************************************************************/

  // TX configs
  int32_t  t_poll_retx;     // Poll retx timeout (ms)
  int32_t  poll_pdu;        // Insert poll bit after this many PDUs
  int32_t  poll_byte;       // Insert poll bit after this much data (KB)
  uint32_t max_retx_thresh; // Max number of retx

  // RX configs
  int32_t t_reordering;      // Timer used by rx to detect PDU loss  (ms)
  int32_t t_status_prohibit; // Timer used by rx to prohibit tx of status PDU (ms)
};

struct rlc_um_config_t {
  /****************************************************************************
   * Configurable parameters
   * Ref: 3GPP TS 36.322 v10.0.0 Section 7
   ***************************************************************************/

  int32_t           t_reordering;       // Timer used by rx to detect PDU loss  (ms)
  rlc_umd_sn_size_t tx_sn_field_length; // Number of bits used for tx (UL) sequence number
  rlc_umd_sn_size_t rx_sn_field_length; // Number of bits used for rx (DL) sequence number

  uint32_t rx_window_size;
  uint32_t rx_mod; // Rx counter modulus
  uint32_t tx_mod; // Tx counter modulus
  bool     is_mrb; // Whether this is a multicast bearer
};

struct rlc_um_nr_config_t {
  /****************************************************************************
   * Configurable parameters
   * Ref: 3GPP TS 38.322 v15.3.0 Section 7
   ***************************************************************************/

  rlc_um_nr_sn_size_t sn_field_length; // Number of bits used for sequence number
  uint32_t            UM_Window_Size;
  uint32_t            mod;             // Rx/Tx counter modulus
  int32_t             t_reassembly_ms; // Timer used by rx to detect PDU loss (ms)
};

#define RLC_TX_QUEUE_LEN (256)

enum class srslte_rat_t { lte, nr, nulltype };
inline std::string to_string(const srslte_rat_t& type)
{
  constexpr static const char* options[] = {"LTE", "NR"};
  return enum_to_text(options, (uint32_t)srslte_rat_t::nulltype, (uint32_t)type);
}

class rlc_config_t
{
public:
  srslte_rat_t       rat;
  rlc_mode_t         rlc_mode;
  rlc_am_config_t    am;
  rlc_um_config_t    um;
  rlc_um_nr_config_t um_nr;
  uint32_t           tx_queue_length;

  rlc_config_t() :
    rat(srslte_rat_t::lte),
    rlc_mode(rlc_mode_t::tm),
    am(),
    um(),
    um_nr(),
    tx_queue_length(RLC_TX_QUEUE_LEN){};

  // Factory for MCH
  static rlc_config_t mch_config()
  {
    rlc_config_t cfg          = {};
    cfg.rat                   = srslte_rat_t::lte;
    cfg.rlc_mode              = rlc_mode_t::um;
    cfg.um.t_reordering       = 45;
    cfg.um.rx_sn_field_length = rlc_umd_sn_size_t::size5bits;
    cfg.um.rx_window_size     = 16;
    cfg.um.rx_mod             = 32;
    cfg.um.tx_sn_field_length = rlc_umd_sn_size_t::size5bits;
    cfg.um.tx_mod             = 32;
    cfg.um.is_mrb             = true;
    cfg.tx_queue_length       = 1024;
    return cfg;
  }
  static rlc_config_t srb_config(uint32_t idx)
  {
    if (idx == 0 or idx > 2) {
      return {};
    }
    // SRB1 and SRB2 are AM
    rlc_config_t rlc_cfg         = {};
    rlc_cfg.rat                  = srslte_rat_t::lte;
    rlc_cfg.rlc_mode             = rlc_mode_t::am;
    rlc_cfg.am.t_poll_retx       = 45;
    rlc_cfg.am.poll_pdu          = -1;
    rlc_cfg.am.poll_byte         = -1;
    rlc_cfg.am.max_retx_thresh   = 4;
    rlc_cfg.am.t_reordering      = 35;
    rlc_cfg.am.t_status_prohibit = 0;
    return rlc_cfg;
  }
  static rlc_config_t default_rlc_um_config(uint32_t sn_size = 10)
  {
    rlc_config_t cnfg    = {};
    cnfg.rat             = srslte_rat_t::lte;
    cnfg.rlc_mode        = rlc_mode_t::um;
    cnfg.um.t_reordering = 5;
    if (sn_size == 10) {
      cnfg.um.rx_sn_field_length = rlc_umd_sn_size_t::size10bits;
      cnfg.um.rx_window_size     = 512;
      cnfg.um.rx_mod             = 1024;
      cnfg.um.tx_sn_field_length = rlc_umd_sn_size_t::size10bits;
      cnfg.um.tx_mod             = 1024;
    } else if (sn_size == 5) {
      cnfg.um.rx_sn_field_length = rlc_umd_sn_size_t::size5bits;
      cnfg.um.rx_window_size     = 16;
      cnfg.um.rx_mod             = 32;
      cnfg.um.tx_sn_field_length = rlc_umd_sn_size_t::size5bits;
      cnfg.um.tx_mod             = 32;
    } else {
      return {};
    }
    return cnfg;
  }
  static rlc_config_t default_rlc_am_config()
  {
    rlc_config_t rlc_cnfg         = {};
    rlc_cnfg.rat                  = srslte_rat_t::lte;
    rlc_cnfg.rlc_mode             = rlc_mode_t::am;
    rlc_cnfg.am.t_reordering      = 5;
    rlc_cnfg.am.t_status_prohibit = 5;
    rlc_cnfg.am.max_retx_thresh   = 4;
    rlc_cnfg.am.poll_byte         = 25;
    rlc_cnfg.am.poll_pdu          = 4;
    rlc_cnfg.am.t_poll_retx       = 5;
    return rlc_cnfg;
  }
  static rlc_config_t default_rlc_um_nr_config(uint32_t sn_size = 6)
  {
    rlc_config_t cnfg = {};
    cnfg.rat          = srslte_rat_t::nr;
    cnfg.rlc_mode     = rlc_mode_t::um;
    if (sn_size == 6) {
      cnfg.um_nr.sn_field_length = rlc_um_nr_sn_size_t::size6bits;
      cnfg.um_nr.UM_Window_Size  = 32;
      cnfg.um_nr.mod             = 64;
    } else if (sn_size == 12) {
      cnfg.um_nr.sn_field_length = rlc_um_nr_sn_size_t::size12bits;
      cnfg.um_nr.UM_Window_Size  = 2048;
      cnfg.um_nr.mod             = 64;
    } else {
      return {};
    }
    cnfg.um_nr.t_reassembly_ms = 5; // lowest non-zero value
    return cnfg;
  }
};

} // namespace srslte

#endif // SRSLTE_RLC_INTERFACE_TYPES_H
