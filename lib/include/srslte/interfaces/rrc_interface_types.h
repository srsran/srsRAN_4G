/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

#ifndef SRSLTE_RRC_INTERFACE_TYPES_H
#define SRSLTE_RRC_INTERFACE_TYPES_H

#include "srslte/common/bcd_helpers.h"
#include "srslte/config.h"
#include <string>

/************************
 * Forward declarations
 ***********************/
namespace asn1 {
namespace rrc {

struct plmn_id_s;
struct s_tmsi_s;
struct rlc_cfg_c;
struct srb_to_add_mod_s;

} // namespace rrc
} // namespace asn1

/************************
 *   Interface types
 ***********************/
namespace srslte {

// helper functions
inline const char* enum_to_text(const char* const array[], uint32_t nof_types, uint32_t enum_val)
{
  return enum_val >= nof_types ? "" : array[enum_val];
}

template <class ItemType>
ItemType enum_to_number(ItemType* array, uint32_t nof_types, uint32_t enum_val)
{
  return enum_val >= nof_types ? -1 : array[enum_val];
}

/***************************
 *        PLMN ID
 **************************/

struct plmn_id_t {
  uint8_t mcc[3];
  uint8_t mnc[3];
  uint8_t nof_mnc_digits;

  plmn_id_t() : mcc(), mnc(), nof_mnc_digits(0) {}
  void reset()
  {
    nof_mnc_digits = 0;
    std::fill(&mnc[0], &mnc[3], 0);
    std::fill(&mcc[0], &mcc[3], 0);
  }
  int from_number(uint16_t mcc_num, uint16_t mnc_num)
  {
    srslte::mcc_to_bytes(mcc_num, &mcc[0]);
    if (not srslte::mnc_to_bytes(mnc_num, &mnc[0], &nof_mnc_digits)) {
      reset();
      return SRSLTE_ERROR;
    }
    return SRSLTE_SUCCESS;
  }
  int from_string(const std::string& plmn_str)
  {
    if (plmn_str.size() < 5 or plmn_str.size() > 6) {
      reset();
      return SRSLTE_ERROR;
    }
    uint16_t mnc_num, mcc_num;
    if (not string_to_mcc(std::string(plmn_str.begin(), plmn_str.begin() + 3), &mcc_num)) {
      reset();
      return SRSLTE_ERROR;
    }
    if (not string_to_mnc(std::string(plmn_str.begin() + 3, plmn_str.end()), &mnc_num)) {
      reset();
      return SRSLTE_ERROR;
    }
    return from_number(mcc_num, mnc_num);
  }
  std::string to_string() const
  {
    std::string mcc_str, mnc_str;
    uint16_t    mnc_num, mcc_num;
    bytes_to_mnc(&mnc[0], &mnc_num, nof_mnc_digits);
    bytes_to_mcc(&mcc[0], &mcc_num);
    mnc_to_string(mnc_num, &mnc_str);
    mcc_to_string(mcc_num, &mcc_str);
    return mcc_str + mnc_str;
  }
  bool operator==(const plmn_id_t& other) const
  {
    return std::equal(&mcc[0], &mcc[3], &other.mcc[0]) and nof_mnc_digits == other.nof_mnc_digits and
           std::equal(&mnc[0], &mnc[nof_mnc_digits], &other.mnc[0]);
  }
  bool is_valid() const { return nof_mnc_digits == 2 or nof_mnc_digits == 3; }
};

/***************************
 *        s-TMSI
 **************************/

struct s_tmsi_t {
  uint8_t  mmec   = 0;
  uint32_t m_tmsi = 0;
};

/***************************
 *   Establishment Cause
 **************************/

enum class establishment_cause_t {
  emergency,
  high_prio_access,
  mt_access,
  mo_sig,
  mo_data,
  delay_tolerant_access_v1020,
  mo_voice_call_v1280,
  spare1,
  nulltype
};
inline std::string to_string(const establishment_cause_t& cause)
{
  constexpr static const char* options[] = {"emergency",
                                            "highPriorityAccess",
                                            "mt-Access",
                                            "mo-Signalling",
                                            "mo-Data",
                                            "delayTolerantAccess-v1020",
                                            "mo-VoiceCall-v1280",
                                            "spare1"};
  return enum_to_text(options, (uint32_t)establishment_cause_t::nulltype, (uint32_t)cause);
}

/***************************
 *      RLC Config
 **************************/
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

#define RLC_TX_QUEUE_LEN (128)

class rlc_config_t
{
public:
  rlc_mode_t      rlc_mode;
  rlc_am_config_t am;
  rlc_um_config_t um;
  uint32_t        tx_queue_length;

  rlc_config_t() : rlc_mode(rlc_mode_t::tm), am(), um(), tx_queue_length(RLC_TX_QUEUE_LEN){};

  // Factory for MCH
  static rlc_config_t mch_config()
  {
    rlc_config_t cfg;
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
    rlc_config_t rlc_cfg;
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
    rlc_config_t cnfg;
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
    rlc_config_t rlc_cnfg;
    rlc_cnfg.rlc_mode             = rlc_mode_t::am;
    rlc_cnfg.am.t_reordering      = 5;
    rlc_cnfg.am.t_status_prohibit = 5;
    rlc_cnfg.am.max_retx_thresh   = 4;
    rlc_cnfg.am.poll_byte         = 25;
    rlc_cnfg.am.poll_pdu          = 4;
    rlc_cnfg.am.t_poll_retx       = 5;
    return rlc_cnfg;
  }
};
} // namespace srslte

#endif // SRSLTE_RRC_INTERFACE_TYPES_H
