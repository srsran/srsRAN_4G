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

#ifndef SRSLTE_RRC_INTERFACE_TYPES_H
#define SRSLTE_RRC_INTERFACE_TYPES_H

#include "srslte/common/bcd_helpers.h"
#include "srslte/config.h"
#include "srslte/srslte.h"
#include <string>

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
  std::pair<uint16_t, uint16_t> to_number()
  {
    uint16_t mcc_num, mnc_num;
    srslte::bytes_to_mcc(&mcc[0], &mcc_num);
    srslte::bytes_to_mnc(&mnc[0], &mnc_num, nof_mnc_digits);
    return std::make_pair(mcc_num, mnc_num);
  }
  uint32_t to_s1ap_plmn()
  {
    auto     mcc_mnc_pair = to_number();
    uint32_t s1ap_plmn;
    srslte::s1ap_mccmnc_to_plmn(mcc_mnc_pair.first, mcc_mnc_pair.second, &s1ap_plmn);
    return s1ap_plmn;
  }
  void to_s1ap_plmn_bytes(uint8_t* plmn_bytes)
  {
    uint32_t s1ap_plmn = to_s1ap_plmn();
    s1ap_plmn          = htonl(s1ap_plmn);
    uint8_t* plmn_ptr  = (uint8_t*)&s1ap_plmn;
    memcpy(&plmn_bytes[0], plmn_ptr + 1, 3);
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
  int to_number(uint16_t* mcc_num, uint16_t* mnc_num) const
  {
    srslte::bytes_to_mcc(&mcc[0], mcc_num);
    srslte::bytes_to_mnc(&mnc[0], mnc_num, nof_mnc_digits);
    return SRSLTE_SUCCESS;
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
 *      PHY Config
 **************************/

struct phy_cfg_t {
  phy_cfg_t() { set_defaults(); }

  void set_defaults()
  {
    ul_cfg = {};
    dl_cfg = {};

    prach_cfg_present = false;
    prach_cfg         = {};

    // CommonConfig defaults for non-zero values
    ul_cfg.pucch.delta_pucch_shift     = 1;
    ul_cfg.power_ctrl.delta_f_pucch[0] = 0;
    ul_cfg.power_ctrl.delta_f_pucch[1] = 1;
    ul_cfg.power_ctrl.delta_f_pucch[2] = 0;
    ul_cfg.power_ctrl.delta_f_pucch[3] = 0;
    ul_cfg.power_ctrl.delta_f_pucch[4] = 0;

    set_defaults_dedicated();
  }

  /**
   *  @brief Sets default PUCCH and SRS configuration (release)
   *
   *  @remark Implemented as specified by TS 36.331 V8.21.0 5.3.13 UE actions upon PUCCH/ SRS release request
   *  @remark Values are according to TS 36.331 V8.21.0 9.2.4 for CQI-ReportConfig, soundingRS-UL-ConfigDedicated
   * and schedulingRequestConfig
   *
   * @see set_defaults_dedicated
   * @see phy_controller::set_phy_to_default_pucch_srs
   */
  void set_defaults_pucch_sr()
  {
    dl_cfg.cqi_report.periodic_configured  = false;
    dl_cfg.cqi_report.aperiodic_configured = false;

    ul_cfg.srs.dedicated_enabled = false;
    ul_cfg.srs.configured        = false;

    ul_cfg.pucch.sr_configured = false;
  }

  /**
   *  @brief Implements physical layer dedicated configuration default values setting
   *
   *  @remark Implemented as specified by TS 36.331 V8.21.0 9.2.4
   *
   *  @see set_defaults
   */
  void set_defaults_dedicated()
  {
    dl_cfg.tm = SRSLTE_TM1;

    dl_cfg.pdsch.use_tbs_index_alt = false;
    dl_cfg.pdsch.p_a               = 0;

    ul_cfg.pucch.tdd_ack_multiplex = false;

    ul_cfg.pusch.uci_offset.I_offset_ack = 10;
    ul_cfg.pusch.uci_offset.I_offset_ri  = 12;
    ul_cfg.pusch.uci_offset.I_offset_cqi = 15;

    ul_cfg.power_ctrl.p0_nominal_pusch = 0;
    ul_cfg.power_ctrl.delta_mcs_based  = false;
    ul_cfg.power_ctrl.acc_enabled      = true;
    ul_cfg.power_ctrl.p0_nominal_pucch = 0;
    ul_cfg.power_ctrl.p_srs_offset     = 7;

    set_defaults_pucch_sr();
  }

  srslte_dl_cfg_t dl_cfg = {};
  srslte_ul_cfg_t ul_cfg = {};

  bool               prach_cfg_present = false;
  srslte_prach_cfg_t prach_cfg         = {};
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

enum class barring_t { none = 0, mo_data, mo_signalling, mt, all };
inline std::string to_string(const barring_t& b)
{
  constexpr static const char* options[] = {"none", "mo-data", "mo-signalling", "mt", "all"};
  return enum_to_text(options, 5u, (uint32_t)b);
}

/**
 * Flat UE capabilities
 */
struct rrc_ue_capabilities_t {
  uint8_t release           = 8;
  uint8_t category          = 4;
  uint8_t category_dl       = 0;
  uint8_t category_ul       = 0;
  bool    support_dl_256qam = false;
  bool    support_ul_64qam  = false;
};

} // namespace srslte

#endif // SRSLTE_RRC_INTERFACE_TYPES_H
