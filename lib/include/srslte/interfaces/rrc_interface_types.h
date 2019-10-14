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
#include "srslte/srslte.h"
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
  bool     operator==(const s_tmsi_t& other) const { return mmec == other.mmec and m_tmsi == other.m_tmsi; }
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
  uint32_t            mod; // Rx/Tx counter modulus
};

#define RLC_TX_QUEUE_LEN (128)

enum class rlc_type_t { lte, nr, nulltype };
inline std::string to_string(const rlc_type_t& type)
{
  constexpr static const char* options[] = {"LTE", "NR"};
  return enum_to_text(options, (uint32_t)rlc_type_t::nulltype, (uint32_t)type);
}

class rlc_config_t
{
public:
  rlc_type_t         type;
  rlc_mode_t      rlc_mode;
  rlc_am_config_t am;
  rlc_um_config_t um;
  rlc_um_nr_config_t um_nr;
  uint32_t        tx_queue_length;

  rlc_config_t() :
    type(rlc_type_t::lte),
    rlc_mode(rlc_mode_t::tm),
    am(),
    um(),
    um_nr(),
    tx_queue_length(RLC_TX_QUEUE_LEN){};

  // Factory for MCH
  static rlc_config_t mch_config()
  {
    rlc_config_t cfg          = {};
    cfg.type                  = rlc_type_t::lte;
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
    rlc_cfg.type                 = rlc_type_t::lte;
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
    cnfg.type            = rlc_type_t::lte;
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
    rlc_cnfg.type                 = rlc_type_t::lte;
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
    cnfg.type         = rlc_type_t::nr;
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
    return cnfg;
  }
};

/***************************
 *      MAC Config
 **************************/
struct bsr_cfg_t {
  int periodic_timer;
  int retx_timer;
  bsr_cfg_t() { reset(); }
  void reset()
  {
    periodic_timer = -1;
    retx_timer     = 2560;
  }
};

struct phr_cfg_t {
  bool enabled;
  int  periodic_timer;
  int  prohibit_timer;
  int  db_pathloss_change;
  bool extended;
  phr_cfg_t() { reset(); }
  void reset()
  {
    enabled            = false;
    periodic_timer     = -1;
    prohibit_timer     = -1;
    db_pathloss_change = -1;
    extended           = false;
  }
};

struct sr_cfg_t {
  bool enabled;
  int  dsr_transmax;
  sr_cfg_t() { reset(); }
  void reset()
  {
    enabled      = false;
    dsr_transmax = 0;
  }
};

struct ul_harq_cfg_t {
  uint32_t max_harq_msg3_tx;
  uint32_t max_harq_tx;
  ul_harq_cfg_t() { reset(); }
  void reset()
  {
    max_harq_msg3_tx = 5;
    max_harq_tx      = 5;
  }
};

struct rach_cfg_t {
  bool     enabled;
  uint32_t nof_preambles;
  uint32_t nof_groupA_preambles;
  int32_t  messagePowerOffsetGroupB;
  uint32_t messageSizeGroupA;
  uint32_t responseWindowSize;
  uint32_t powerRampingStep;
  uint32_t preambleTransMax;
  int32_t  iniReceivedTargetPower;
  uint32_t contentionResolutionTimer;
  uint32_t new_ra_msg_len;
  rach_cfg_t() { reset(); }
  void reset()
  {
    enabled                   = false;
    nof_preambles             = 0;
    nof_groupA_preambles      = 0;
    messagePowerOffsetGroupB  = 0;
    messageSizeGroupA         = 0;
    responseWindowSize        = 0;
    powerRampingStep          = 0;
    preambleTransMax          = 0;
    iniReceivedTargetPower    = 0;
    contentionResolutionTimer = 0;
    new_ra_msg_len            = 0;
  }
};

struct mac_cfg_t {
  // Default constructor with default values as in 36.331 9.2.2
  mac_cfg_t() { set_defaults(); }

  void set_defaults()
  {
    rach_cfg.reset();
    set_mac_main_cfg_default();
  }

  void set_mac_main_cfg_default()
  {
    bsr_cfg.reset();
    phr_cfg.reset();
    sr_cfg.reset();
    harq_cfg.reset();
    time_alignment_timer = -1;
  }

  bsr_cfg_t     bsr_cfg;
  phr_cfg_t     phr_cfg;
  sr_cfg_t      sr_cfg;
  rach_cfg_t    rach_cfg;
  ul_harq_cfg_t harq_cfg;
  int           time_alignment_timer;
};

/***************************
 *      PHY Config
 **************************/

struct phy_cfg_t {
  phy_cfg_t() { set_defaults(); }

  void set_defaults()
  {
    ZERO_OBJECT(ul_cfg);
    ZERO_OBJECT(dl_cfg);
    ZERO_OBJECT(prach_cfg);

    // CommonConfig defaults for non-zero values
    ul_cfg.pucch.delta_pucch_shift     = 1;
    ul_cfg.power_ctrl.delta_f_pucch[0] = 0;
    ul_cfg.power_ctrl.delta_f_pucch[1] = 1;
    ul_cfg.power_ctrl.delta_f_pucch[2] = 0;
    ul_cfg.power_ctrl.delta_f_pucch[3] = 0;
    ul_cfg.power_ctrl.delta_f_pucch[4] = 0;

    set_defaults_dedicated();
  }

  // 36.331 9.2.4
  void set_defaults_dedicated()
  {
    dl_cfg.tm = SRSLTE_TM1;

    dl_cfg.pdsch.use_tbs_index_alt = false;
    dl_cfg.pdsch.p_a               = 0;

    dl_cfg.cqi_report.periodic_configured  = false;
    dl_cfg.cqi_report.aperiodic_configured = false;

    ul_cfg.pucch.tdd_ack_multiplex = false;

    ul_cfg.pusch.uci_offset.I_offset_ack = 10;
    ul_cfg.pusch.uci_offset.I_offset_ri  = 12;
    ul_cfg.pusch.uci_offset.I_offset_cqi = 15;

    ul_cfg.power_ctrl.p0_nominal_pusch = 0;
    ul_cfg.power_ctrl.delta_mcs_based  = false;
    ul_cfg.power_ctrl.acc_enabled      = true;
    ul_cfg.power_ctrl.p0_nominal_pucch = 0;
    ul_cfg.power_ctrl.p_srs_offset     = 7;

    ul_cfg.srs.dedicated_enabled = false;

    ul_cfg.pucch.sr_configured = false;
  }

  srslte_dl_cfg_t    dl_cfg;
  srslte_ul_cfg_t    ul_cfg;
  srslte_prach_cfg_t prach_cfg;
};

struct mbsfn_sf_cfg_t {
  enum class alloc_period_t { n1, n2, n4, n8, n16, n32, nulltype };
  alloc_period_t radioframe_alloc_period;
  uint8_t        radioframe_alloc_offset = 0;
  enum class sf_alloc_type_t { one_frame, four_frames, nulltype };
  sf_alloc_type_t nof_alloc_subfrs;
  uint32_t        sf_alloc;
};
inline uint16_t enum_to_number(const mbsfn_sf_cfg_t::alloc_period_t& radioframe_period)
{
  constexpr static uint16_t options[] = {1, 2, 4, 8, 16, 32};
  return enum_to_number(options, (uint32_t)mbsfn_sf_cfg_t::alloc_period_t::nulltype, (uint32_t)radioframe_period);
}

struct mbms_notif_cfg_t {
  enum class coeff_t { n2, n4 };
  coeff_t notif_repeat_coeff = coeff_t::n2;
  uint8_t notif_offset       = 0;
  uint8_t notif_sf_idx       = 1;
};

// MBSFN-AreaInfo-r9 ::= SEQUENCE
struct mbsfn_area_info_t {
  uint8_t mbsfn_area_id = 0;
  enum class region_len_t { s1, s2, nulltype } non_mbsfn_region_len;
  uint8_t notif_ind = 0;
  struct mcch_cfg_t {
    enum class repeat_period_t { rf32, rf64, rf128, rf256, nulltype } mcch_repeat_period;
    uint8_t mcch_offset = 0;
    enum class mod_period_t { rf512, rf1024 } mcch_mod_period;
    uint8_t sf_alloc_info = 0;
    enum class sig_mcs_t { n2, n7, n13, n19, nulltype } sig_mcs;
  } mcch_cfg;
};
inline uint16_t enum_to_number(const mbsfn_area_info_t::region_len_t& region_len)
{
  constexpr static uint16_t options[] = {1, 2};
  return enum_to_number(options, (uint32_t)mbsfn_area_info_t::region_len_t::nulltype, (uint32_t)region_len);
}
inline uint16_t enum_to_number(const mbsfn_area_info_t::mcch_cfg_t::repeat_period_t& repeat_period)
{
  constexpr static uint16_t options[] = {32, 64, 128, 256};
  return enum_to_number(
      options, (uint32_t)mbsfn_area_info_t::mcch_cfg_t::repeat_period_t::nulltype, (uint32_t)repeat_period);
}
inline uint16_t enum_to_number(const mbsfn_area_info_t::mcch_cfg_t::sig_mcs_t& sig_mcs)
{
  constexpr static uint16_t options[] = {2, 7, 13, 19};
  return enum_to_number(options, (uint32_t)mbsfn_area_info_t::mcch_cfg_t::sig_mcs_t::nulltype, (uint32_t)sig_mcs);
}

// TMGI-r9
struct tmgi_t {
  enum class plmn_id_type_t { plmn_idx, explicit_value } plmn_id_type;
  union choice {
    uint8_t   plmn_idx;
    plmn_id_t explicit_value;
    choice() : plmn_idx(0) {}
  } plmn_id;
  uint8_t serviced_id[3];
  tmgi_t() : plmn_id_type(plmn_id_type_t::plmn_idx) {}
};

struct pmch_info_t {
  // pmch_cfg_t
  uint16_t sf_alloc_end = 0;
  uint8_t  data_mcs     = 0;
  enum class mch_sched_period_t { rf8, rf16, rf32, rf64, rf128, rf256, rf512, rf1024, nulltype } mch_sched_period;
  // mbms_session_info_list
  struct mbms_session_info_t {
    bool    session_id_present = false;
    tmgi_t  tmgi;
    uint8_t session_id;
    uint8_t lc_ch_id = 0;
  };
  uint32_t              nof_mbms_session_info;
  static const uint32_t max_session_per_pmch = 29;
  mbms_session_info_t   mbms_session_info_list[max_session_per_pmch];
};
inline uint16_t enum_to_number(const pmch_info_t::mch_sched_period_t& mch_period)
{
  constexpr static uint16_t options[] = {8, 16, 32, 64, 128, 256, 512, 1024};
  return enum_to_number(options, (uint32_t)pmch_info_t::mch_sched_period_t::nulltype, (uint32_t)mch_period);
}

struct mcch_msg_t {
  uint32_t       nof_common_sf_alloc = 0;
  mbsfn_sf_cfg_t common_sf_alloc[8];
  enum class common_sf_alloc_period_t { rf4, rf8, rf16, rf32, rf64, rf128, rf256, nulltype } common_sf_alloc_period;
  uint32_t    nof_pmch_info;
  pmch_info_t pmch_info_list[15];
  // mbsfn_area_cfg_v930_ies non crit ext OPTIONAL
};
inline uint16_t enum_to_number(const mcch_msg_t::common_sf_alloc_period_t& alloc_period)
{
  constexpr static uint16_t options[] = {4, 8, 16, 32, 64, 128, 256};
  return enum_to_number(options, (uint32_t)mcch_msg_t::common_sf_alloc_period_t::nulltype, (uint32_t)alloc_period);
}

struct phy_cfg_mbsfn_t {
  mbsfn_sf_cfg_t    mbsfn_subfr_cnfg;
  mbms_notif_cfg_t  mbsfn_notification_cnfg;
  mbsfn_area_info_t mbsfn_area_info;
  mcch_msg_t        mcch;
};

// SystemInformationBlockType13-r9
struct sib13_t {
  static const uint32_t max_mbsfn_area      = 8;
  uint32_t              nof_mbsfn_area_info = 0;
  mbsfn_area_info_t     mbsfn_area_info_list[max_mbsfn_area];
  mbms_notif_cfg_t      notif_cfg;
};

} // namespace srslte

#endif // SRSLTE_RRC_INTERFACE_TYPES_H
