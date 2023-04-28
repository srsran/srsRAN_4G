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

/*******************************************************************************
 *
 *                      3GPP TS ASN1 RRC v17.4.0 (2023-03)
 *
 ******************************************************************************/

#ifndef SRSASN1_RRC_COMMON_H
#define SRSASN1_RRC_COMMON_H

#include "srsran/asn1/asn1_utils.h"
#include <cstdio>
#include <stdarg.h>

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                             Constant Definitions
 ******************************************************************************/

#define ASN1_RRC_MAX_ACCESS_CAT_MINUS1_R15 63
#define ASN1_RRC_MAX_ACDC_CAT_R13 16
#define ASN1_RRC_MAX_AVAIL_NARROW_BANDS_R13 16
#define ASN1_RRC_MAX_AVAIL_NARROW_BANDS_MINUS1_R16 15
#define ASN1_RRC_MAX_BAND_COMB_R10 128
#define ASN1_RRC_MAX_BAND_COMB_R11 256
#define ASN1_RRC_MAX_BAND_COMB_R13 384
#define ASN1_RRC_MAX_BAND_COMB_SIDELINK_NR_R16 512
#define ASN1_RRC_MAX_BANDS 64
#define ASN1_RRC_MAX_BANDS_NR_R15 1024
#define ASN1_RRC_MAX_BANDS_ENDC_R16 10
#define ASN1_RRC_MAX_BW_CLASS_R10 16
#define ASN1_RRC_MAX_BW_COMB_SET_R10 32
#define ASN1_RRC_MAX_BARR_INFO_SET_R15 8
#define ASN1_RRC_MAX_BT_ID_REPORT_R15 32
#define ASN1_RRC_MAX_BT_NAME_R15 4
#define ASN1_RRC_MAX_CBR_LEVEL_R14 16
#define ASN1_RRC_MAX_CBR_LEVEL_MINUS1_R14 15
#define ASN1_RRC_MAX_CBR_REPORT_R14 72
#define ASN1_RRC_MAX_CDMA_BAND_CLASS 32
#define ASN1_RRC_MAX_CE_LEVEL_R13 4
#define ASN1_RRC_MAX_EXCLUDED_CELL 16
#define ASN1_RRC_MAX_CELL_HISTORY_R12 16
#define ASN1_RRC_MAX_CELL_INFO_GERAN_R9 32
#define ASN1_RRC_MAX_CELL_INFO_UTRA_R9 16
#define ASN1_RRC_MAX_CELL_MEAS_IDLE_R15 8
#define ASN1_RRC_MAX_CELL_NR_R17 8
#define ASN1_RRC_MAX_COMB_IDC_R11 128
#define ASN1_RRC_MAX_CSI_IM_R11 3
#define ASN1_RRC_MAX_CSI_IM_R12 4
#define ASN1_RRC_MIN_CSI_IM_R13 5
#define ASN1_RRC_MAX_CSI_IM_R13 24
#define ASN1_RRC_MAX_CSI_IM_V1310 20
#define ASN1_RRC_MAX_CSI_PROC_R11 4
#define ASN1_RRC_MAX_CSI_RS_NZP_R11 3
#define ASN1_RRC_MIN_CSI_RS_NZP_R13 4
#define ASN1_RRC_MAX_CSI_RS_NZP_R13 24
#define ASN1_RRC_MAX_CSI_RS_NZP_V1310 21
#define ASN1_RRC_MAX_CSI_RS_ZP_R11 4
#define ASN1_RRC_MAX_CQI_PROC_EXT_R11 3
#define ASN1_RRC_MAX_FREQ_UTRA_TDD_R10 6
#define ASN1_RRC_MAX_CELL_INTER 16
#define ASN1_RRC_MAX_CELL_INTRA 16
#define ASN1_RRC_MAX_CELL_LIST_GERAN 3
#define ASN1_RRC_MAX_CELL_MEAS 32
#define ASN1_RRC_MAX_CELL_REPORT 8
#define ASN1_RRC_MAX_CELL_SFTD 3
#define ASN1_RRC_MAX_CELL_ALLOWED_NR_R16 16
#define ASN1_RRC_MAX_COND_CFG_R16 8
#define ASN1_RRC_MAX_CFG_SPS_R14 8
#define ASN1_RRC_MAX_CFG_SPS_R15 6
#define ASN1_RRC_MAX_CSI_RS_MEAS_R12 96
#define ASN1_RRC_MAX_DRB 11
#define ASN1_RRC_MAX_DRB_EXT_R15 4
#define ASN1_RRC_MAX_DRB_R15 15
#define ASN1_RRC_MAX_DS_DUR_R12 5
#define ASN1_RRC_MAX_DS_ZTP_CSI_RS_R12 5
#define ASN1_RRC_MAX_EARFCN 65535
#define ASN1_RRC_MAX_EARFCN_PLUS1 65536
#define ASN1_RRC_MAX_EARFCN2 262143
#define ASN1_RRC_MAX_EPDCCH_SET_R11 2
#define ASN1_RRC_MAX_FBI 64
#define ASN1_RRC_MAX_FBI_NR_R15 1024
#define ASN1_RRC_MAX_FBI_PLUS1 65
#define ASN1_RRC_MAX_FBI2 256
#define ASN1_RRC_MAX_FEATURE_SETS_R15 256
#define ASN1_RRC_MAX_PER_CC_FEATURE_SETS_R15 32
#define ASN1_RRC_MAX_FREQ 8
#define ASN1_RRC_MAX_FREQ_MINUS1_R16 7
#define ASN1_RRC_MAX_FREQ_IDC_R11 32
#define ASN1_RRC_MAX_FREQ_IDLE_R15 8
#define ASN1_RRC_MAX_FREQ_MBMS_R11 5
#define ASN1_RRC_MAX_FREQ_NBIOT_R16 8
#define ASN1_RRC_MAX_FREQ_NR_R15 5
#define ASN1_RRC_MAX_FREQ_SL_NR_R16 8
#define ASN1_RRC_MAX_FREQ_V2X_R14 8
#define ASN1_RRC_MAX_FREQ_V2X_MINUS1_R14 7
#define ASN1_RRC_MAX_GERAN_SI 10
#define ASN1_RRC_MAX_GNFG 16
#define ASN1_RRC_MAX_GWUS_GROUPS_MINUS1_R16 31
#define ASN1_RRC_MAX_GWUS_RES_R16 4
#define ASN1_RRC_MAX_GWUS_PROB_THRESS_R16 3
#define ASN1_RRC_MAX_IDLE_MEAS_CARRIERS_R15 3
#define ASN1_RRC_MAX_IDLE_MEAS_CARRIERS_EXT_R16 5
#define ASN1_RRC_MAX_IDLE_MEAS_CARRIERS_R16 8
#define ASN1_RRC_MAX_LCG_R13 4
#define ASN1_RRC_MAX_LOG_MEAS_REPORT_R10 520
#define ASN1_RRC_MAX_MBSFN_ALLOCS 8
#define ASN1_RRC_MAX_MBSFN_AREA 8
#define ASN1_RRC_MAX_MBSFN_AREA_MINUS1 7
#define ASN1_RRC_MAX_MBMS_SERVICE_LIST_PER_UE_R13 15
#define ASN1_RRC_MAX_MEAS_ID 32
#define ASN1_RRC_MAX_MEAS_ID_PLUS1 33
#define ASN1_RRC_MAX_MEAS_ID_R12 64
#define ASN1_RRC_MAX_MULTI_BANDS 8
#define ASN1_RRC_MAX_MULTI_BANDS_NR_R15 32
#define ASN1_RRC_MAX_MULTI_BANDS_NR_MINUS1_R15 31
#define ASN1_RRC_MAX_NS_PMAX_R10 8
#define ASN1_RRC_MAX_NAICS_ENTRIES_R12 8
#define ASN1_RRC_MAX_NEIGH_CELL_R12 8
#define ASN1_RRC_MAX_NEIGH_CELL_SCPTM_R13 8
#define ASN1_RRC_MAX_NROF_PCI_PER_SMTC_R16 64
#define ASN1_RRC_MAX_NROF_S_NSSAI_R15 8
#define ASN1_RRC_MAX_OBJ_ID 32
#define ASN1_RRC_MAX_OBJ_ID_PLUS1_R13 33
#define ASN1_RRC_MAX_OBJ_ID_R13 64
#define ASN1_RRC_MAX_P_A_PER_NEIGH_CELL_R12 3
#define ASN1_RRC_MAX_PAGE_REC 16
#define ASN1_RRC_MAX_PCI_RANGE_R9 4
#define ASN1_RRC_MAX_PLMN_R11 6
#define ASN1_RRC_MAX_PLMN_MINUS1_R14 5
#define ASN1_RRC_MAX_PLMN_R15 8
#define ASN1_RRC_MAX_PLMN_NR_R15 12
#define ASN1_RRC_MAX_PN_OFFSET 511
#define ASN1_RRC_MAX_PMCH_PER_MBSFN 15
#define ASN1_RRC_MAX_PSSCH_TX_CFG_R14 16
#define ASN1_RRC_MAX_QUANT_SETS_NR_R15 2
#define ASN1_RRC_MAX_QCI_R13 6
#define ASN1_RRC_MAX_RAT_CAP 8
#define ASN1_RRC_MAX_RE_MAP_QCL_R11 4
#define ASN1_RRC_MAX_REPORT_CFG_ID 32
#define ASN1_RRC_MAX_RESERV_PERIOD_R14 16
#define ASN1_RRC_MAX_RS_IDX_R15 64
#define ASN1_RRC_MAX_RS_IDX_MINUS1_R15 63
#define ASN1_RRC_MAX_RS_IDX_CELL_QUAL_R15 16
#define ASN1_RRC_MAX_RS_IDX_REPORT_R15 32
#define ASN1_RRC_MAX_RSTD_FREQ_R10 3
#define ASN1_RRC_MAX_SAI_MBMS_R11 64
#define ASN1_RRC_MAX_SAT_R17 4
#define ASN1_RRC_MAX_SCELL_R10 4
#define ASN1_RRC_MAX_SCELL_R13 31
#define ASN1_RRC_MAX_SCELL_GROUPS_R15 4
#define ASN1_RRC_MAX_SC_MTCH_R13 1023
#define ASN1_RRC_MAX_SC_MTCH_BR_R14 128
#define ASN1_RRC_MAX_SL_COMM_RX_POOL_NFREQ_R13 32
#define ASN1_RRC_MAX_SL_COMM_RX_POOL_PRECONF_V1310 12
#define ASN1_RRC_MAX_SL_TX_POOL_R12_PLUS1_R13 5
#define ASN1_RRC_MAX_SL_TX_POOL_V1310 4
#define ASN1_RRC_MAX_SL_TX_POOL_R13 8
#define ASN1_RRC_MAX_SL_COMM_TX_POOL_PRECONF_V1310 7
#define ASN1_RRC_MAX_SL_DEST_R12 16
#define ASN1_RRC_MAX_SL_DISC_CELLS_R13 16
#define ASN1_RRC_MAX_SL_DISC_PWR_CLASS_R12 3
#define ASN1_RRC_MAX_SL_DISC_RX_POOL_PRECONF_R13 16
#define ASN1_RRC_MAX_SL_DISC_SYS_INFO_REPORT_FREQ_R13 8
#define ASN1_RRC_MAX_SL_DISC_TX_POOL_PRECONF_R13 4
#define ASN1_RRC_MAX_SL_GP_R13 8
#define ASN1_RRC_MAX_SL_POOL_TO_MEASURE_R14 72
#define ASN1_RRC_MAX_SL_PRIO_R13 8
#define ASN1_RRC_MAX_SL_RX_POOL_R12 16
#define ASN1_RRC_MAX_SL_RELIABILITY_R15 8
#define ASN1_RRC_MAX_SL_SYNC_CFG_R12 16
#define ASN1_RRC_MAX_SL_TF_IDX_PAIR_R12 64
#define ASN1_RRC_MAX_SL_TX_POOL_R12 4
#define ASN1_RRC_MAX_SL_V2X_RX_POOL_R14 16
#define ASN1_RRC_MAX_SL_V2X_RX_POOL_PRECONF_R14 16
#define ASN1_RRC_MAX_SL_V2X_TX_POOL_R14 8
#define ASN1_RRC_MAX_SL_V2X_TX_POOL_PRECONF_R14 8
#define ASN1_RRC_MAX_SL_V2X_SYNC_CFG_R14 16
#define ASN1_RRC_MAX_SL_V2X_CBR_CFG_R14 4
#define ASN1_RRC_MAX_SL_V2X_CBR_CFG_MINUS1_R14 3
#define ASN1_RRC_MAX_SL_V2X_TX_CFG_R14 64
#define ASN1_RRC_MAX_SL_V2X_TX_CFG_MINUS1_R14 63
#define ASN1_RRC_MAX_SL_V2X_CBR_CFG2_R14 8
#define ASN1_RRC_MAX_SL_V2X_CBR_CFG2_MINUS1_R14 7
#define ASN1_RRC_MAX_SL_V2X_TX_CFG2_R14 128
#define ASN1_RRC_MAX_SL_V2X_TX_CFG2_MINUS1_R14 127
#define ASN1_RRC_MAX_STAG_R11 3
#define ASN1_RRC_MAX_SERV_CELL_R10 5
#define ASN1_RRC_MAX_SERV_CELL_R13 32
#define ASN1_RRC_MAX_SERV_CELL_NR_R15 16
#define ASN1_RRC_MAX_SERVICE_COUNT 16
#define ASN1_RRC_MAX_SERVICE_COUNT_MINUS1 15
#define ASN1_RRC_MAX_SESSION_PER_PMCH 29
#define ASN1_RRC_MAX_SESSION_PER_PMCH_MINUS1 28
#define ASN1_RRC_MAX_SIB 32
#define ASN1_RRC_MAX_SIB_MINUS1 31
#define ASN1_RRC_MAX_SI_MSG 32
#define ASN1_RRC_MAX_SIMUL_BANDS_R10 64
#define ASN1_RRC_MAX_SF_PATTERN_IDC_R11 8
#define ASN1_RRC_MAX_TAC_R17 12
#define ASN1_RRC_MAX_TRAFFIC_PATTERN_R14 8
#define ASN1_RRC_MAX_UTRA_FDD_CARRIER 16
#define ASN1_RRC_MAX_UTRA_TDD_CARRIER 16
#define ASN1_RRC_MAX_WAY_POINT_R15 20
#define ASN1_RRC_MAX_WLAN_ID_R12 16
#define ASN1_RRC_MAX_WLAN_BANDS_R13 8
#define ASN1_RRC_MAX_WLAN_ID_R13 32
#define ASN1_RRC_MAX_WLAN_CHS_R13 16
#define ASN1_RRC_MAX_WLAN_CARRIER_INFO_R13 8
#define ASN1_RRC_MAX_WLAN_ID_REPORT_R14 32
#define ASN1_RRC_MAX_WLAN_NAME_R15 4
#define ASN1_RRC_MAX_LOG_MEAS_R10 4060
#define ASN1_RRC_MAX_REESTAB_INFO 32
#define ASN1_RRC_MAX_FREQ_ANR_NB_R16 2
#define ASN1_RRC_MAX_FREQ_EUTRA_NB_R16 8
#define ASN1_RRC_MAX_FREQS_GERAN_NB_R16 8
#define ASN1_RRC_MAX_GWUS_GROUPS_MINUS1_NB_R16 15
#define ASN1_RRC_MAX_GWUS_RES_NB_R16 2
#define ASN1_RRC_MAX_GWUS_PROB_THRESS_NB_R16 3
#define ASN1_RRC_MAX_NPRACH_RES_NB_R13 3
#define ASN1_RRC_MAX_NON_ANCHOR_CARRIERS_NB_R14 15
#define ASN1_RRC_MAX_DRB_NB_R13 2
#define ASN1_RRC_MAX_NEIGH_CELL_SCPTM_NB_R14 8
#define ASN1_RRC_MAX_NS_PMAX_NB_R13 4
#define ASN1_RRC_MAX_SC_MTCH_NB_R14 64
#define ASN1_RRC_MAX_SI_MSG_NB_R13 8
#define ASN1_RRC_MAX_TAC_NB_R17 12

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// MCC ::= SEQUENCE (SIZE (3)) OF INTEGER (0..9)
using mcc_l = std::array<uint8_t, 3>;

// MNC ::= SEQUENCE (SIZE (2..3)) OF INTEGER (0..9)
using mnc_l = bounded_array<uint8_t, 3>;

// PLMN-Identity ::= SEQUENCE
struct plmn_id_s {
  bool  mcc_present = false;
  mcc_l mcc;
  mnc_l mnc;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityInfo ::= SEQUENCE
struct plmn_id_info_s {
  struct cell_reserved_for_oper_opts {
    enum options { reserved, not_reserved, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<cell_reserved_for_oper_opts> cell_reserved_for_oper_e_;

  // member variables
  plmn_id_s                 plmn_id;
  cell_reserved_for_oper_e_ cell_reserved_for_oper;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Alpha-r12 ::= ENUMERATED
struct alpha_r12_opts {
  enum options { al0, al04, al05, al06, al07, al08, al09, al1, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
typedef enumerated<alpha_r12_opts> alpha_r12_e;

// PLMN-IdentityList ::= SEQUENCE (SIZE (1..6)) OF PLMN-IdentityInfo
using plmn_id_list_l = dyn_array<plmn_id_info_s>;

// BandclassCDMA2000 ::= ENUMERATED
struct bandclass_cdma2000_opts {
  enum options {
    bc0,
    bc1,
    bc2,
    bc3,
    bc4,
    bc5,
    bc6,
    bc7,
    bc8,
    bc9,
    bc10,
    bc11,
    bc12,
    bc13,
    bc14,
    bc15,
    bc16,
    bc17,
    bc18_v9a0,
    bc19_v9a0,
    bc20_v9a0,
    bc21_v9a0,
    spare10,
    spare9,
    spare8,
    spare7,
    spare6,
    spare5,
    spare4,
    spare3,
    spare2,
    spare1,
    // ...
    nulltype
  } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<bandclass_cdma2000_opts, true> bandclass_cdma2000_e;

struct setup_opts {
  enum options { release, setup, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<setup_opts> setup_e;

// FilterCoefficient ::= ENUMERATED
struct filt_coef_opts {
  enum options {
    fc0,
    fc1,
    fc2,
    fc3,
    fc4,
    fc5,
    fc6,
    fc7,
    fc8,
    fc9,
    fc11,
    fc13,
    fc15,
    fc17,
    fc19,
    spare1,
    /*...*/ nulltype
  } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<filt_coef_opts, true> filt_coef_e;

// MBSFN-SubframeConfig ::= SEQUENCE
struct mbsfn_sf_cfg_s {
  struct radioframe_alloc_period_opts {
    enum options { n1, n2, n4, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<radioframe_alloc_period_opts> radioframe_alloc_period_e_;
  struct sf_alloc_c_ {
    struct types_opts {
      enum options { one_frame, four_frames, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    sf_alloc_c_() = default;
    sf_alloc_c_(const sf_alloc_c_& other);
    sf_alloc_c_& operator=(const sf_alloc_c_& other);
    ~sf_alloc_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const sf_alloc_c_& other) const;
    bool        operator!=(const sf_alloc_c_& other) const { return not(*this == other); }
    // getters
    fixed_bitstring<6>& one_frame()
    {
      assert_choice_type(types::one_frame, type_, "subframeAllocation");
      return c.get<fixed_bitstring<6> >();
    }
    fixed_bitstring<24>& four_frames()
    {
      assert_choice_type(types::four_frames, type_, "subframeAllocation");
      return c.get<fixed_bitstring<24> >();
    }
    const fixed_bitstring<6>& one_frame() const
    {
      assert_choice_type(types::one_frame, type_, "subframeAllocation");
      return c.get<fixed_bitstring<6> >();
    }
    const fixed_bitstring<24>& four_frames() const
    {
      assert_choice_type(types::four_frames, type_, "subframeAllocation");
      return c.get<fixed_bitstring<24> >();
    }
    fixed_bitstring<6>&  set_one_frame();
    fixed_bitstring<24>& set_four_frames();

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<24> > c;

    void destroy_();
  };

  // member variables
  radioframe_alloc_period_e_ radioframe_alloc_period;
  uint8_t                    radioframe_alloc_offset = 0;
  sf_alloc_c_                sf_alloc;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const mbsfn_sf_cfg_s& other) const;
  bool        operator!=(const mbsfn_sf_cfg_s& other) const { return not(*this == other); }
};

// MBSFN-SubframeConfig-v1430 ::= SEQUENCE
struct mbsfn_sf_cfg_v1430_s {
  struct sf_alloc_v1430_c_ {
    struct types_opts {
      enum options { one_frame_v1430, four_frames_v1430, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    sf_alloc_v1430_c_() = default;
    sf_alloc_v1430_c_(const sf_alloc_v1430_c_& other);
    sf_alloc_v1430_c_& operator=(const sf_alloc_v1430_c_& other);
    ~sf_alloc_v1430_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const sf_alloc_v1430_c_& other) const;
    bool        operator!=(const sf_alloc_v1430_c_& other) const { return not(*this == other); }
    // getters
    fixed_bitstring<2>& one_frame_v1430()
    {
      assert_choice_type(types::one_frame_v1430, type_, "subframeAllocation-v1430");
      return c.get<fixed_bitstring<2> >();
    }
    fixed_bitstring<8>& four_frames_v1430()
    {
      assert_choice_type(types::four_frames_v1430, type_, "subframeAllocation-v1430");
      return c.get<fixed_bitstring<8> >();
    }
    const fixed_bitstring<2>& one_frame_v1430() const
    {
      assert_choice_type(types::one_frame_v1430, type_, "subframeAllocation-v1430");
      return c.get<fixed_bitstring<2> >();
    }
    const fixed_bitstring<8>& four_frames_v1430() const
    {
      assert_choice_type(types::four_frames_v1430, type_, "subframeAllocation-v1430");
      return c.get<fixed_bitstring<8> >();
    }
    fixed_bitstring<2>& set_one_frame_v1430();
    fixed_bitstring<8>& set_four_frames_v1430();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<8> > c;

    void destroy_();
  };

  // member variables
  sf_alloc_v1430_c_ sf_alloc_v1430;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const mbsfn_sf_cfg_v1430_s& other) const;
  bool        operator!=(const mbsfn_sf_cfg_v1430_s& other) const { return not(*this == other); }
};

// MBSFN-SubframeConfigList ::= SEQUENCE (SIZE (1..8)) OF MBSFN-SubframeConfig
using mbsfn_sf_cfg_list_l = dyn_array<mbsfn_sf_cfg_s>;

// MBSFN-SubframeConfigList-v1430 ::= SEQUENCE (SIZE (1..8)) OF MBSFN-SubframeConfig-v1430
using mbsfn_sf_cfg_list_v1430_l = dyn_array<mbsfn_sf_cfg_v1430_s>;

// MeasSubframePattern-r10 ::= CHOICE
struct meas_sf_pattern_r10_c {
  struct sf_pattern_tdd_r10_c_ {
    struct types_opts {
      enum options { sf_cfg1_minus5_r10, sf_cfg0_r10, sf_cfg6_r10, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts, true> types;

    // choice methods
    sf_pattern_tdd_r10_c_() = default;
    sf_pattern_tdd_r10_c_(const sf_pattern_tdd_r10_c_& other);
    sf_pattern_tdd_r10_c_& operator=(const sf_pattern_tdd_r10_c_& other);
    ~sf_pattern_tdd_r10_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const sf_pattern_tdd_r10_c_& other) const;
    bool        operator!=(const sf_pattern_tdd_r10_c_& other) const { return not(*this == other); }
    // getters
    fixed_bitstring<20>& sf_cfg1_minus5_r10()
    {
      assert_choice_type(types::sf_cfg1_minus5_r10, type_, "subframePatternTDD-r10");
      return c.get<fixed_bitstring<20> >();
    }
    fixed_bitstring<70>& sf_cfg0_r10()
    {
      assert_choice_type(types::sf_cfg0_r10, type_, "subframePatternTDD-r10");
      return c.get<fixed_bitstring<70> >();
    }
    fixed_bitstring<60>& sf_cfg6_r10()
    {
      assert_choice_type(types::sf_cfg6_r10, type_, "subframePatternTDD-r10");
      return c.get<fixed_bitstring<60> >();
    }
    const fixed_bitstring<20>& sf_cfg1_minus5_r10() const
    {
      assert_choice_type(types::sf_cfg1_minus5_r10, type_, "subframePatternTDD-r10");
      return c.get<fixed_bitstring<20> >();
    }
    const fixed_bitstring<70>& sf_cfg0_r10() const
    {
      assert_choice_type(types::sf_cfg0_r10, type_, "subframePatternTDD-r10");
      return c.get<fixed_bitstring<70> >();
    }
    const fixed_bitstring<60>& sf_cfg6_r10() const
    {
      assert_choice_type(types::sf_cfg6_r10, type_, "subframePatternTDD-r10");
      return c.get<fixed_bitstring<60> >();
    }
    fixed_bitstring<20>& set_sf_cfg1_minus5_r10();
    fixed_bitstring<70>& set_sf_cfg0_r10();
    fixed_bitstring<60>& set_sf_cfg6_r10();

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<70> > c;

    void destroy_();
  };
  struct types_opts {
    enum options { sf_pattern_fdd_r10, sf_pattern_tdd_r10, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  meas_sf_pattern_r10_c() = default;
  meas_sf_pattern_r10_c(const meas_sf_pattern_r10_c& other);
  meas_sf_pattern_r10_c& operator=(const meas_sf_pattern_r10_c& other);
  ~meas_sf_pattern_r10_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const meas_sf_pattern_r10_c& other) const;
  bool        operator!=(const meas_sf_pattern_r10_c& other) const { return not(*this == other); }
  // getters
  fixed_bitstring<40>& sf_pattern_fdd_r10()
  {
    assert_choice_type(types::sf_pattern_fdd_r10, type_, "MeasSubframePattern-r10");
    return c.get<fixed_bitstring<40> >();
  }
  sf_pattern_tdd_r10_c_& sf_pattern_tdd_r10()
  {
    assert_choice_type(types::sf_pattern_tdd_r10, type_, "MeasSubframePattern-r10");
    return c.get<sf_pattern_tdd_r10_c_>();
  }
  const fixed_bitstring<40>& sf_pattern_fdd_r10() const
  {
    assert_choice_type(types::sf_pattern_fdd_r10, type_, "MeasSubframePattern-r10");
    return c.get<fixed_bitstring<40> >();
  }
  const sf_pattern_tdd_r10_c_& sf_pattern_tdd_r10() const
  {
    assert_choice_type(types::sf_pattern_tdd_r10, type_, "MeasSubframePattern-r10");
    return c.get<sf_pattern_tdd_r10_c_>();
  }
  fixed_bitstring<40>&   set_sf_pattern_fdd_r10();
  sf_pattern_tdd_r10_c_& set_sf_pattern_tdd_r10();

private:
  types                                                       type_;
  choice_buffer_t<fixed_bitstring<40>, sf_pattern_tdd_r10_c_> c;

  void destroy_();
};

struct c1_or_crit_ext_opts {
  enum options { c1, crit_exts_future, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<c1_or_crit_ext_opts> c1_or_crit_ext_e;

// CipheringAlgorithm-r12 ::= ENUMERATED
struct ciphering_algorithm_r12_opts {
  enum options { eea0, eea1, eea2, eea3_v1130, spare4, spare3, spare2, spare1, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<ciphering_algorithm_r12_opts, true> ciphering_algorithm_r12_e;

// WLAN-BandIndicator-r13 ::= ENUMERATED
struct wlan_band_ind_r13_opts {
  enum options { band2dot4, band5, band60_v1430, spare5, spare4, spare3, spare2, spare1, /*...*/ nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
typedef enumerated<wlan_band_ind_r13_opts, true> wlan_band_ind_r13_e;

} // namespace rrc
} // namespace asn1

#endif // SRSASN1_RRC_COMMON_H
