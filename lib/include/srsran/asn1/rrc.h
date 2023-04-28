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

#ifndef SRSASN1_RRC_H
#define SRSASN1_RRC_H

#include "rrc/bcch_msg.h"
#include "rrc/dl_ccch_msg.h"
#include "rrc/ho_cmd.h"
#include "rrc/ul_ccch_msg.h"
#include "rrc/ul_dcch_msg.h"

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// MasterInformationBlock-MBMS-r14 ::= SEQUENCE
struct mib_mbms_r14_s {
  struct dl_bw_mbms_r14_opts {
    enum options { n6, n15, n25, n50, n75, n100, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dl_bw_mbms_r14_opts> dl_bw_mbms_r14_e_;

  // member variables
  dl_bw_mbms_r14_e_   dl_bw_mbms_r14;
  fixed_bitstring<6>  sys_frame_num_r14;
  uint8_t             add_non_mbsfn_sfs_r14    = 0;
  uint8_t             semi_static_cfi_mbms_r16 = 0;
  fixed_bitstring<11> spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCCH-BCH-MessageType-MBMS-r14 ::= MasterInformationBlock-MBMS-r14
using bcch_bch_msg_type_mbms_r14_s = mib_mbms_r14_s;

// BCCH-BCH-Message-MBMS ::= SEQUENCE
struct bcch_bch_msg_mbms_s {
  bcch_bch_msg_type_mbms_r14_s msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformation-MBMS-r14 ::= SystemInformation
using sys_info_mbms_r14_s = sys_info_s;

// BCCH-DL-SCH-MessageType-MBMS-r14 ::= CHOICE
struct bcch_dl_sch_msg_type_mbms_r14_c {
  struct c1_c_ {
    struct types_opts {
      enum options { sys_info_mbms_r14, sib_type1_mbms_r14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    c1_c_() = default;
    c1_c_(const c1_c_& other);
    c1_c_& operator=(const c1_c_& other);
    ~c1_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sys_info_mbms_r14_s& sys_info_mbms_r14()
    {
      assert_choice_type(types::sys_info_mbms_r14, type_, "c1");
      return c.get<sys_info_mbms_r14_s>();
    }
    sib_type1_mbms_r14_s& sib_type1_mbms_r14()
    {
      assert_choice_type(types::sib_type1_mbms_r14, type_, "c1");
      return c.get<sib_type1_mbms_r14_s>();
    }
    const sys_info_mbms_r14_s& sys_info_mbms_r14() const
    {
      assert_choice_type(types::sys_info_mbms_r14, type_, "c1");
      return c.get<sys_info_mbms_r14_s>();
    }
    const sib_type1_mbms_r14_s& sib_type1_mbms_r14() const
    {
      assert_choice_type(types::sib_type1_mbms_r14, type_, "c1");
      return c.get<sib_type1_mbms_r14_s>();
    }
    sys_info_mbms_r14_s&  set_sys_info_mbms_r14();
    sib_type1_mbms_r14_s& set_sib_type1_mbms_r14();

  private:
    types                                                      type_;
    choice_buffer_t<sib_type1_mbms_r14_s, sys_info_mbms_r14_s> c;

    void destroy_();
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  bcch_dl_sch_msg_type_mbms_r14_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "BCCH-DL-SCH-MessageType-MBMS-r14");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "BCCH-DL-SCH-MessageType-MBMS-r14");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// BCCH-DL-SCH-Message-MBMS ::= SEQUENCE
struct bcch_dl_sch_msg_mbms_s {
  bcch_dl_sch_msg_type_mbms_r14_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ThresholdEUTRA-v1250 ::= INTEGER (0..97)
using thres_eutra_v1250 = uint8_t;

// MBMS-SessionInfo-r9 ::= SEQUENCE
struct mbms_session_info_r9_s {
  bool               ext                   = false;
  bool               session_id_r9_present = false;
  tmgi_r9_s          tmgi_r9;
  fixed_octstring<1> session_id_r9;
  uint8_t            lc_ch_id_r9 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSFN-SubframeConfig-v1610 ::= SEQUENCE
struct mbsfn_sf_cfg_v1610_s {
  struct sf_alloc_v1610_c_ {
    struct types_opts {
      enum options { one_frame_v1610, four_frames_v1610, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    sf_alloc_v1610_c_() = default;
    sf_alloc_v1610_c_(const sf_alloc_v1610_c_& other);
    sf_alloc_v1610_c_& operator=(const sf_alloc_v1610_c_& other);
    ~sf_alloc_v1610_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<2>& one_frame_v1610()
    {
      assert_choice_type(types::one_frame_v1610, type_, "subframeAllocation-v1610");
      return c.get<fixed_bitstring<2> >();
    }
    fixed_bitstring<8>& four_frames_v1610()
    {
      assert_choice_type(types::four_frames_v1610, type_, "subframeAllocation-v1610");
      return c.get<fixed_bitstring<8> >();
    }
    const fixed_bitstring<2>& one_frame_v1610() const
    {
      assert_choice_type(types::one_frame_v1610, type_, "subframeAllocation-v1610");
      return c.get<fixed_bitstring<2> >();
    }
    const fixed_bitstring<8>& four_frames_v1610() const
    {
      assert_choice_type(types::four_frames_v1610, type_, "subframeAllocation-v1610");
      return c.get<fixed_bitstring<8> >();
    }
    fixed_bitstring<2>& set_one_frame_v1610();
    fixed_bitstring<8>& set_four_frames_v1610();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<8> > c;

    void destroy_();
  };

  // member variables
  sf_alloc_v1610_c_ sf_alloc_v1610;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CommonSF-AllocPatternList-v1610 ::= SEQUENCE (SIZE (1..8)) OF MBSFN-SubframeConfig-v1610
using common_sf_alloc_pattern_list_v1610_l = dyn_array<mbsfn_sf_cfg_v1610_s>;

// MBMS-SessionInfoList-r9 ::= SEQUENCE (SIZE (0..29)) OF MBMS-SessionInfo-r9
using mbms_session_info_list_r9_l = dyn_array<mbms_session_info_r9_s>;

// PMCH-Config-r12 ::= SEQUENCE
struct pmch_cfg_r12_s {
  struct data_mcs_r12_c_ {
    struct types_opts {
      enum options { normal_r12, higer_order_r12, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    data_mcs_r12_c_() = default;
    data_mcs_r12_c_(const data_mcs_r12_c_& other);
    data_mcs_r12_c_& operator=(const data_mcs_r12_c_& other);
    ~data_mcs_r12_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& normal_r12()
    {
      assert_choice_type(types::normal_r12, type_, "dataMCS-r12");
      return c.get<uint8_t>();
    }
    uint8_t& higer_order_r12()
    {
      assert_choice_type(types::higer_order_r12, type_, "dataMCS-r12");
      return c.get<uint8_t>();
    }
    const uint8_t& normal_r12() const
    {
      assert_choice_type(types::normal_r12, type_, "dataMCS-r12");
      return c.get<uint8_t>();
    }
    const uint8_t& higer_order_r12() const
    {
      assert_choice_type(types::higer_order_r12, type_, "dataMCS-r12");
      return c.get<uint8_t>();
    }
    uint8_t& set_normal_r12();
    uint8_t& set_higer_order_r12();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct mch_sched_period_r12_opts {
    enum options { rf4, rf8, rf16, rf32, rf64, rf128, rf256, rf512, rf1024, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mch_sched_period_r12_opts> mch_sched_period_r12_e_;
  struct mch_sched_period_v1430_opts {
    enum options { rf1, rf2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<mch_sched_period_v1430_opts> mch_sched_period_v1430_e_;

  // member variables
  bool                    ext              = false;
  uint16_t                sf_alloc_end_r12 = 0;
  data_mcs_r12_c_         data_mcs_r12;
  mch_sched_period_r12_e_ mch_sched_period_r12;
  // ...
  // group 0
  bool                      mch_sched_period_v1430_present = false;
  mch_sched_period_v1430_e_ mch_sched_period_v1430;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CommonSF-AllocPatternList-v1430 ::= SEQUENCE (SIZE (1..8)) OF MBSFN-SubframeConfig-v1430
using common_sf_alloc_pattern_list_v1430_l = dyn_array<mbsfn_sf_cfg_v1430_s>;

// MBSFNAreaConfiguration-v1610-IEs ::= SEQUENCE
struct mbsfn_area_cfg_v1610_ies_s {
  bool                                 common_sf_alloc_v1610_present = false;
  bool                                 non_crit_ext_present          = false;
  common_sf_alloc_pattern_list_v1610_l common_sf_alloc_v1610;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PMCH-InfoExt-r12 ::= SEQUENCE
struct pmch_info_ext_r12_s {
  bool                        ext = false;
  pmch_cfg_r12_s              pmch_cfg_r12;
  mbms_session_info_list_r9_l mbms_session_info_list_r12;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSFNAreaConfiguration-v1430-IEs ::= SEQUENCE
struct mbsfn_area_cfg_v1430_ies_s {
  bool                                 non_crit_ext_present = false;
  common_sf_alloc_pattern_list_v1430_l common_sf_alloc_v1430;
  mbsfn_area_cfg_v1610_ies_s           non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PMCH-Config-r9 ::= SEQUENCE
struct pmch_cfg_r9_s {
  struct mch_sched_period_r9_opts {
    enum options { rf8, rf16, rf32, rf64, rf128, rf256, rf512, rf1024, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mch_sched_period_r9_opts> mch_sched_period_r9_e_;

  // member variables
  bool                   ext             = false;
  uint16_t               sf_alloc_end_r9 = 0;
  uint8_t                data_mcs_r9     = 0;
  mch_sched_period_r9_e_ mch_sched_period_r9;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PMCH-InfoListExt-r12 ::= SEQUENCE (SIZE (0..15)) OF PMCH-InfoExt-r12
using pmch_info_list_ext_r12_l = dyn_array<pmch_info_ext_r12_s>;

// CountingRequestInfo-r10 ::= SEQUENCE
struct count_request_info_r10_s {
  bool      ext = false;
  tmgi_r9_s tmgi_r10;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSFNAreaConfiguration-v1250-IEs ::= SEQUENCE
struct mbsfn_area_cfg_v1250_ies_s {
  bool                       pmch_info_list_ext_r12_present = false;
  bool                       non_crit_ext_present           = false;
  pmch_info_list_ext_r12_l   pmch_info_list_ext_r12;
  mbsfn_area_cfg_v1430_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PMCH-Info-r9 ::= SEQUENCE
struct pmch_info_r9_s {
  bool                        ext = false;
  pmch_cfg_r9_s               pmch_cfg_r9;
  mbms_session_info_list_r9_l mbms_session_info_list_r9;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CommonSF-AllocPatternList-r9 ::= SEQUENCE (SIZE (1..8)) OF MBSFN-SubframeConfig
using common_sf_alloc_pattern_list_r9_l = dyn_array<mbsfn_sf_cfg_s>;

// CountingRequestList-r10 ::= SEQUENCE (SIZE (1..16)) OF CountingRequestInfo-r10
using count_request_list_r10_l = dyn_array<count_request_info_r10_s>;

// MBSFNAreaConfiguration-v930-IEs ::= SEQUENCE
struct mbsfn_area_cfg_v930_ies_s {
  bool                       late_non_crit_ext_present = false;
  bool                       non_crit_ext_present      = false;
  dyn_octstring              late_non_crit_ext;
  mbsfn_area_cfg_v1250_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PMCH-InfoList-r9 ::= SEQUENCE (SIZE (0..15)) OF PMCH-Info-r9
using pmch_info_list_r9_l = dyn_array<pmch_info_r9_s>;

// MBMSCountingRequest-r10 ::= SEQUENCE
struct mbms_count_request_r10_s {
  bool                     late_non_crit_ext_present = false;
  bool                     non_crit_ext_present      = false;
  count_request_list_r10_l count_request_list_r10;
  dyn_octstring            late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSFNAreaConfiguration-r9 ::= SEQUENCE
struct mbsfn_area_cfg_r9_s {
  struct common_sf_alloc_period_r9_opts {
    enum options { rf4, rf8, rf16, rf32, rf64, rf128, rf256, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<common_sf_alloc_period_r9_opts> common_sf_alloc_period_r9_e_;

  // member variables
  bool                              non_crit_ext_present = false;
  common_sf_alloc_pattern_list_r9_l common_sf_alloc_r9;
  common_sf_alloc_period_r9_e_      common_sf_alloc_period_r9;
  pmch_info_list_r9_l               pmch_info_list_r9;
  mbsfn_area_cfg_v930_ies_s         non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MCCH-MessageType ::= CHOICE
struct mcch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { mbsfn_area_cfg_r9, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::mbsfn_area_cfg_r9; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mbsfn_area_cfg_r9_s&       mbsfn_area_cfg_r9() { return c; }
    const mbsfn_area_cfg_r9_s& mbsfn_area_cfg_r9() const { return c; }

  private:
    mbsfn_area_cfg_r9_s c;
  };
  struct later_c_ {
    struct c2_c_ {
      struct types_opts {
        enum options { mbms_count_request_r10, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      types       type() const { return types::mbms_count_request_r10; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      mbms_count_request_r10_s&       mbms_count_request_r10() { return c; }
      const mbms_count_request_r10_s& mbms_count_request_r10() const { return c; }

    private:
      mbms_count_request_r10_s c;
    };
    struct types_opts {
      enum options { c2, msg_class_ext, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    later_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c2_c_& c2()
    {
      assert_choice_type(types::c2, type_, "later");
      return c;
    }
    const c2_c_& c2() const
    {
      assert_choice_type(types::c2, type_, "later");
      return c;
    }
    c2_c_& set_c2();
    void   set_msg_class_ext();

  private:
    types type_;
    c2_c_ c;
  };
  struct types_opts {
    enum options { c1, later, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  mcch_msg_type_c() = default;
  mcch_msg_type_c(const mcch_msg_type_c& other);
  mcch_msg_type_c& operator=(const mcch_msg_type_c& other);
  ~mcch_msg_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "MCCH-MessageType");
    return c.get<c1_c_>();
  }
  later_c_& later()
  {
    assert_choice_type(types::later, type_, "MCCH-MessageType");
    return c.get<later_c_>();
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "MCCH-MessageType");
    return c.get<c1_c_>();
  }
  const later_c_& later() const
  {
    assert_choice_type(types::later, type_, "MCCH-MessageType");
    return c.get<later_c_>();
  }
  c1_c_&    set_c1();
  later_c_& set_later();

private:
  types                            type_;
  choice_buffer_t<c1_c_, later_c_> c;

  void destroy_();
};

// MCCH-Message ::= SEQUENCE
struct mcch_msg_s {
  mcch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMSSessionInfo-r13 ::= SEQUENCE
struct mbms_session_info_r13_s {
  bool               session_id_r13_present = false;
  tmgi_r9_s          tmgi_r13;
  fixed_octstring<1> session_id_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SC-MTCH-SchedulingInfo-BR-r14 ::= SEQUENCE
struct sc_mtch_sched_info_br_r14_s {
  struct on_dur_timer_scptm_r14_opts {
    enum options { psf300, psf400, psf500, psf600, psf800, psf1000, psf1200, psf1600, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<on_dur_timer_scptm_r14_opts> on_dur_timer_scptm_r14_e_;
  struct drx_inactivity_timer_scptm_r14_opts {
    enum options {
      psf0,
      psf1,
      psf2,
      psf4,
      psf8,
      psf16,
      psf32,
      psf64,
      psf128,
      psf256,
      ps512,
      psf1024,
      psf2048,
      psf4096,
      psf8192,
      psf16384,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<drx_inactivity_timer_scptm_r14_opts> drx_inactivity_timer_scptm_r14_e_;
  struct sched_period_start_offset_scptm_r14_c_ {
    struct types_opts {
      enum options {
        sf10,
        sf20,
        sf32,
        sf40,
        sf64,
        sf80,
        sf128,
        sf160,
        sf256,
        sf320,
        sf512,
        sf640,
        sf1024,
        sf2048,
        sf4096,
        sf8192,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    sched_period_start_offset_scptm_r14_c_() = default;
    sched_period_start_offset_scptm_r14_c_(const sched_period_start_offset_scptm_r14_c_& other);
    sched_period_start_offset_scptm_r14_c_& operator=(const sched_period_start_offset_scptm_r14_c_& other);
    ~sched_period_start_offset_scptm_r14_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sf10()
    {
      assert_choice_type(types::sf10, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint8_t>();
    }
    uint8_t& sf20()
    {
      assert_choice_type(types::sf20, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint8_t>();
    }
    uint8_t& sf32()
    {
      assert_choice_type(types::sf32, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint8_t>();
    }
    uint8_t& sf40()
    {
      assert_choice_type(types::sf40, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint8_t>();
    }
    uint8_t& sf64()
    {
      assert_choice_type(types::sf64, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint8_t>();
    }
    uint8_t& sf80()
    {
      assert_choice_type(types::sf80, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint8_t>();
    }
    uint8_t& sf128()
    {
      assert_choice_type(types::sf128, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint8_t>();
    }
    uint8_t& sf160()
    {
      assert_choice_type(types::sf160, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint8_t>();
    }
    uint16_t& sf256()
    {
      assert_choice_type(types::sf256, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint16_t>();
    }
    uint16_t& sf320()
    {
      assert_choice_type(types::sf320, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint16_t>();
    }
    uint16_t& sf512()
    {
      assert_choice_type(types::sf512, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint16_t>();
    }
    uint16_t& sf640()
    {
      assert_choice_type(types::sf640, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint16_t>();
    }
    uint16_t& sf1024()
    {
      assert_choice_type(types::sf1024, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint16_t>();
    }
    uint16_t& sf2048()
    {
      assert_choice_type(types::sf2048, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint16_t>();
    }
    uint16_t& sf4096()
    {
      assert_choice_type(types::sf4096, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint16_t>();
    }
    uint16_t& sf8192()
    {
      assert_choice_type(types::sf8192, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint16_t>();
    }
    const uint8_t& sf10() const
    {
      assert_choice_type(types::sf10, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint8_t>();
    }
    const uint8_t& sf20() const
    {
      assert_choice_type(types::sf20, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint8_t>();
    }
    const uint8_t& sf32() const
    {
      assert_choice_type(types::sf32, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint8_t>();
    }
    const uint8_t& sf40() const
    {
      assert_choice_type(types::sf40, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint8_t>();
    }
    const uint8_t& sf64() const
    {
      assert_choice_type(types::sf64, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint8_t>();
    }
    const uint8_t& sf80() const
    {
      assert_choice_type(types::sf80, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint8_t>();
    }
    const uint8_t& sf128() const
    {
      assert_choice_type(types::sf128, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint8_t>();
    }
    const uint8_t& sf160() const
    {
      assert_choice_type(types::sf160, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint8_t>();
    }
    const uint16_t& sf256() const
    {
      assert_choice_type(types::sf256, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint16_t>();
    }
    const uint16_t& sf320() const
    {
      assert_choice_type(types::sf320, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint16_t>();
    }
    const uint16_t& sf512() const
    {
      assert_choice_type(types::sf512, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint16_t>();
    }
    const uint16_t& sf640() const
    {
      assert_choice_type(types::sf640, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint16_t>();
    }
    const uint16_t& sf1024() const
    {
      assert_choice_type(types::sf1024, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint16_t>();
    }
    const uint16_t& sf2048() const
    {
      assert_choice_type(types::sf2048, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint16_t>();
    }
    const uint16_t& sf4096() const
    {
      assert_choice_type(types::sf4096, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint16_t>();
    }
    const uint16_t& sf8192() const
    {
      assert_choice_type(types::sf8192, type_, "schedulingPeriodStartOffsetSCPTM-r14");
      return c.get<uint16_t>();
    }
    uint8_t&  set_sf10();
    uint8_t&  set_sf20();
    uint8_t&  set_sf32();
    uint8_t&  set_sf40();
    uint8_t&  set_sf64();
    uint8_t&  set_sf80();
    uint8_t&  set_sf128();
    uint8_t&  set_sf160();
    uint16_t& set_sf256();
    uint16_t& set_sf320();
    uint16_t& set_sf512();
    uint16_t& set_sf640();
    uint16_t& set_sf1024();
    uint16_t& set_sf2048();
    uint16_t& set_sf4096();
    uint16_t& set_sf8192();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                                   ext = false;
  on_dur_timer_scptm_r14_e_              on_dur_timer_scptm_r14;
  drx_inactivity_timer_scptm_r14_e_      drx_inactivity_timer_scptm_r14;
  sched_period_start_offset_scptm_r14_c_ sched_period_start_offset_scptm_r14;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SC-MTCH-Info-BR-r14 ::= SEQUENCE
struct sc_mtch_info_br_r14_s {
  struct mpdcch_num_repeat_sc_mtch_r14_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, r128, r256, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mpdcch_num_repeat_sc_mtch_r14_opts> mpdcch_num_repeat_sc_mtch_r14_e_;
  struct mpdcch_start_sf_sc_mtch_r14_c_ {
    struct fdd_r14_opts {
      enum options { v1, v1dot5, v2, v2dot5, v4, v5, v8, v10, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<fdd_r14_opts> fdd_r14_e_;
    struct tdd_r14_opts {
      enum options { v1, v2, v4, v5, v8, v10, v20, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<tdd_r14_opts> tdd_r14_e_;
    struct types_opts {
      enum options { fdd_r14, tdd_r14, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    mpdcch_start_sf_sc_mtch_r14_c_() = default;
    mpdcch_start_sf_sc_mtch_r14_c_(const mpdcch_start_sf_sc_mtch_r14_c_& other);
    mpdcch_start_sf_sc_mtch_r14_c_& operator=(const mpdcch_start_sf_sc_mtch_r14_c_& other);
    ~mpdcch_start_sf_sc_mtch_r14_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fdd_r14_e_& fdd_r14()
    {
      assert_choice_type(types::fdd_r14, type_, "mpdcch-StartSF-SC-MTCH-r14");
      return c.get<fdd_r14_e_>();
    }
    tdd_r14_e_& tdd_r14()
    {
      assert_choice_type(types::tdd_r14, type_, "mpdcch-StartSF-SC-MTCH-r14");
      return c.get<tdd_r14_e_>();
    }
    const fdd_r14_e_& fdd_r14() const
    {
      assert_choice_type(types::fdd_r14, type_, "mpdcch-StartSF-SC-MTCH-r14");
      return c.get<fdd_r14_e_>();
    }
    const tdd_r14_e_& tdd_r14() const
    {
      assert_choice_type(types::tdd_r14, type_, "mpdcch-StartSF-SC-MTCH-r14");
      return c.get<tdd_r14_e_>();
    }
    fdd_r14_e_& set_fdd_r14();
    tdd_r14_e_& set_tdd_r14();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct mpdcch_pdsch_hop_cfg_sc_mtch_r14_opts {
    enum options { on, off, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<mpdcch_pdsch_hop_cfg_sc_mtch_r14_opts> mpdcch_pdsch_hop_cfg_sc_mtch_r14_e_;
  struct mpdcch_pdsch_cemode_cfg_sc_mtch_r14_opts {
    enum options { ce_mode_a, ce_mode_b, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<mpdcch_pdsch_cemode_cfg_sc_mtch_r14_opts> mpdcch_pdsch_cemode_cfg_sc_mtch_r14_e_;
  struct mpdcch_pdsch_max_bw_sc_mtch_r14_opts {
    enum options { bw1dot4, bw5, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<mpdcch_pdsch_max_bw_sc_mtch_r14_opts> mpdcch_pdsch_max_bw_sc_mtch_r14_e_;
  struct mpdcch_offset_sc_mtch_r14_opts {
    enum options {
      zero,
      one_eighth,
      one_quarter,
      three_eighth,
      one_half,
      five_eighth,
      three_quarter,
      seven_eighth,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<mpdcch_offset_sc_mtch_r14_opts> mpdcch_offset_sc_mtch_r14_e_;
  struct p_a_r14_opts {
    enum options { db_minus6, db_minus4dot77, db_minus3, db_minus1dot77, db0, db1, db2, db3, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<p_a_r14_opts> p_a_r14_e_;

  // member variables
  bool                                   ext                                = false;
  bool                                   sc_mtch_sched_info_r14_present     = false;
  bool                                   sc_mtch_neighbour_cell_r14_present = false;
  bool                                   p_a_r14_present                    = false;
  uint32_t                               sc_mtch_carrier_freq_r14           = 0;
  mbms_session_info_r13_s                mbms_session_info_r14;
  fixed_bitstring<16>                    g_rnti_r14;
  sc_mtch_sched_info_br_r14_s            sc_mtch_sched_info_r14;
  fixed_bitstring<8>                     sc_mtch_neighbour_cell_r14;
  uint8_t                                mpdcch_nb_sc_mtch_r14 = 1;
  mpdcch_num_repeat_sc_mtch_r14_e_       mpdcch_num_repeat_sc_mtch_r14;
  mpdcch_start_sf_sc_mtch_r14_c_         mpdcch_start_sf_sc_mtch_r14;
  mpdcch_pdsch_hop_cfg_sc_mtch_r14_e_    mpdcch_pdsch_hop_cfg_sc_mtch_r14;
  mpdcch_pdsch_cemode_cfg_sc_mtch_r14_e_ mpdcch_pdsch_cemode_cfg_sc_mtch_r14;
  mpdcch_pdsch_max_bw_sc_mtch_r14_e_     mpdcch_pdsch_max_bw_sc_mtch_r14;
  mpdcch_offset_sc_mtch_r14_e_           mpdcch_offset_sc_mtch_r14;
  p_a_r14_e_                             p_a_r14;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SC-MTCH-SchedulingInfo-r13 ::= SEQUENCE
struct sc_mtch_sched_info_r13_s {
  struct on_dur_timer_scptm_r13_opts {
    enum options {
      psf1,
      psf2,
      psf3,
      psf4,
      psf5,
      psf6,
      psf8,
      psf10,
      psf20,
      psf30,
      psf40,
      psf50,
      psf60,
      psf80,
      psf100,
      psf200,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<on_dur_timer_scptm_r13_opts> on_dur_timer_scptm_r13_e_;
  struct drx_inactivity_timer_scptm_r13_opts {
    enum options {
      psf0,
      psf1,
      psf2,
      psf4,
      psf8,
      psf10,
      psf20,
      psf40,
      psf80,
      psf160,
      ps320,
      psf640,
      psf960,
      psf1280,
      psf1920,
      psf2560,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<drx_inactivity_timer_scptm_r13_opts> drx_inactivity_timer_scptm_r13_e_;
  struct sched_period_start_offset_scptm_r13_c_ {
    struct types_opts {
      enum options {
        sf10,
        sf20,
        sf32,
        sf40,
        sf64,
        sf80,
        sf128,
        sf160,
        sf256,
        sf320,
        sf512,
        sf640,
        sf1024,
        sf2048,
        sf4096,
        sf8192,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    sched_period_start_offset_scptm_r13_c_() = default;
    sched_period_start_offset_scptm_r13_c_(const sched_period_start_offset_scptm_r13_c_& other);
    sched_period_start_offset_scptm_r13_c_& operator=(const sched_period_start_offset_scptm_r13_c_& other);
    ~sched_period_start_offset_scptm_r13_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& sf10()
    {
      assert_choice_type(types::sf10, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint8_t>();
    }
    uint8_t& sf20()
    {
      assert_choice_type(types::sf20, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint8_t>();
    }
    uint8_t& sf32()
    {
      assert_choice_type(types::sf32, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint8_t>();
    }
    uint8_t& sf40()
    {
      assert_choice_type(types::sf40, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint8_t>();
    }
    uint8_t& sf64()
    {
      assert_choice_type(types::sf64, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint8_t>();
    }
    uint8_t& sf80()
    {
      assert_choice_type(types::sf80, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint8_t>();
    }
    uint8_t& sf128()
    {
      assert_choice_type(types::sf128, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint8_t>();
    }
    uint8_t& sf160()
    {
      assert_choice_type(types::sf160, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint8_t>();
    }
    uint16_t& sf256()
    {
      assert_choice_type(types::sf256, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint16_t>();
    }
    uint16_t& sf320()
    {
      assert_choice_type(types::sf320, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint16_t>();
    }
    uint16_t& sf512()
    {
      assert_choice_type(types::sf512, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint16_t>();
    }
    uint16_t& sf640()
    {
      assert_choice_type(types::sf640, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint16_t>();
    }
    uint16_t& sf1024()
    {
      assert_choice_type(types::sf1024, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint16_t>();
    }
    uint16_t& sf2048()
    {
      assert_choice_type(types::sf2048, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint16_t>();
    }
    uint16_t& sf4096()
    {
      assert_choice_type(types::sf4096, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint16_t>();
    }
    uint16_t& sf8192()
    {
      assert_choice_type(types::sf8192, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint16_t>();
    }
    const uint8_t& sf10() const
    {
      assert_choice_type(types::sf10, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint8_t>();
    }
    const uint8_t& sf20() const
    {
      assert_choice_type(types::sf20, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint8_t>();
    }
    const uint8_t& sf32() const
    {
      assert_choice_type(types::sf32, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint8_t>();
    }
    const uint8_t& sf40() const
    {
      assert_choice_type(types::sf40, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint8_t>();
    }
    const uint8_t& sf64() const
    {
      assert_choice_type(types::sf64, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint8_t>();
    }
    const uint8_t& sf80() const
    {
      assert_choice_type(types::sf80, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint8_t>();
    }
    const uint8_t& sf128() const
    {
      assert_choice_type(types::sf128, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint8_t>();
    }
    const uint8_t& sf160() const
    {
      assert_choice_type(types::sf160, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint8_t>();
    }
    const uint16_t& sf256() const
    {
      assert_choice_type(types::sf256, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint16_t>();
    }
    const uint16_t& sf320() const
    {
      assert_choice_type(types::sf320, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint16_t>();
    }
    const uint16_t& sf512() const
    {
      assert_choice_type(types::sf512, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint16_t>();
    }
    const uint16_t& sf640() const
    {
      assert_choice_type(types::sf640, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint16_t>();
    }
    const uint16_t& sf1024() const
    {
      assert_choice_type(types::sf1024, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint16_t>();
    }
    const uint16_t& sf2048() const
    {
      assert_choice_type(types::sf2048, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint16_t>();
    }
    const uint16_t& sf4096() const
    {
      assert_choice_type(types::sf4096, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint16_t>();
    }
    const uint16_t& sf8192() const
    {
      assert_choice_type(types::sf8192, type_, "schedulingPeriodStartOffsetSCPTM-r13");
      return c.get<uint16_t>();
    }
    uint8_t&  set_sf10();
    uint8_t&  set_sf20();
    uint8_t&  set_sf32();
    uint8_t&  set_sf40();
    uint8_t&  set_sf64();
    uint8_t&  set_sf80();
    uint8_t&  set_sf128();
    uint8_t&  set_sf160();
    uint16_t& set_sf256();
    uint16_t& set_sf320();
    uint16_t& set_sf512();
    uint16_t& set_sf640();
    uint16_t& set_sf1024();
    uint16_t& set_sf2048();
    uint16_t& set_sf4096();
    uint16_t& set_sf8192();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                                   ext = false;
  on_dur_timer_scptm_r13_e_              on_dur_timer_scptm_r13;
  drx_inactivity_timer_scptm_r13_e_      drx_inactivity_timer_scptm_r13;
  sched_period_start_offset_scptm_r13_c_ sched_period_start_offset_scptm_r13;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCI-ARFCN-r13 ::= SEQUENCE
struct pci_arfcn_r13_s {
  bool     carrier_freq_r13_present = false;
  uint16_t pci_r13                  = 0;
  uint32_t carrier_freq_r13         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SC-MTCH-Info-r13 ::= SEQUENCE
struct sc_mtch_info_r13_s {
  struct p_a_r13_opts {
    enum options { db_minus6, db_minus4dot77, db_minus3, db_minus1dot77, db0, db1, db2, db3, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<p_a_r13_opts> p_a_r13_e_;

  // member variables
  bool                     ext                                = false;
  bool                     sc_mtch_sched_info_r13_present     = false;
  bool                     sc_mtch_neighbour_cell_r13_present = false;
  mbms_session_info_r13_s  mbms_session_info_r13;
  fixed_bitstring<16>      g_rnti_r13;
  sc_mtch_sched_info_r13_s sc_mtch_sched_info_r13;
  fixed_bitstring<8>       sc_mtch_neighbour_cell_r13;
  // ...
  // group 0
  bool       p_a_r13_present = false;
  p_a_r13_e_ p_a_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SC-MTCH-InfoList-BR-r14 ::= SEQUENCE (SIZE (0..128)) OF SC-MTCH-Info-BR-r14
using sc_mtch_info_list_br_r14_l = dyn_array<sc_mtch_info_br_r14_s>;

// SC-MTCH-InfoList-r13 ::= SEQUENCE (SIZE (0..1023)) OF SC-MTCH-Info-r13
using sc_mtch_info_list_r13_l = dyn_array<sc_mtch_info_r13_s>;

// SCPTM-NeighbourCellList-r13 ::= SEQUENCE (SIZE (1..8)) OF PCI-ARFCN-r13
using scptm_neighbour_cell_list_r13_l = dyn_array<pci_arfcn_r13_s>;

// SCPTMConfiguration-BR-v1610 ::= SEQUENCE
struct scptm_cfg_br_v1610_s {
  struct multi_tb_gap_r16_opts {
    enum options { sf2, sf4, sf8, sf16, sf32, sf64, sf128, spare, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<multi_tb_gap_r16_opts> multi_tb_gap_r16_e_;

  // member variables
  bool                       multi_tb_gap_r16_present = false;
  bool                       non_crit_ext_present     = false;
  sc_mtch_info_list_br_r14_l sc_mtch_info_list_multi_tb_r16;
  multi_tb_gap_r16_e_        multi_tb_gap_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCPTMConfiguration-v1340 ::= SEQUENCE
struct scptm_cfg_v1340_s {
  bool    p_b_r13_present      = false;
  bool    non_crit_ext_present = false;
  uint8_t p_b_r13              = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCPTMConfiguration-BR-r14 ::= SEQUENCE
struct scptm_cfg_br_r14_s {
  bool                            scptm_neighbour_cell_list_r14_present = false;
  bool                            p_b_r14_present                       = false;
  bool                            late_non_crit_ext_present             = false;
  bool                            non_crit_ext_present                  = false;
  sc_mtch_info_list_br_r14_l      sc_mtch_info_list_r14;
  scptm_neighbour_cell_list_r13_l scptm_neighbour_cell_list_r14;
  uint8_t                         p_b_r14 = 0;
  dyn_octstring                   late_non_crit_ext;
  scptm_cfg_br_v1610_s            non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCPTMConfiguration-r13 ::= SEQUENCE
struct scptm_cfg_r13_s {
  bool                            scptm_neighbour_cell_list_r13_present = false;
  bool                            late_non_crit_ext_present             = false;
  bool                            non_crit_ext_present                  = false;
  sc_mtch_info_list_r13_l         sc_mtch_info_list_r13;
  scptm_neighbour_cell_list_r13_l scptm_neighbour_cell_list_r13;
  dyn_octstring                   late_non_crit_ext;
  scptm_cfg_v1340_s               non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SC-MCCH-MessageType-r13 ::= CHOICE
struct sc_mcch_msg_type_r13_c {
  struct c1_c_ {
    struct types_opts {
      enum options { scptm_cfg_r13, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::scptm_cfg_r13; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scptm_cfg_r13_s&       scptm_cfg_r13() { return c; }
    const scptm_cfg_r13_s& scptm_cfg_r13() const { return c; }

  private:
    scptm_cfg_r13_s c;
  };
  struct msg_class_ext_c_ {
    struct c2_c_ {
      struct types_opts {
        enum options { scptm_cfg_br_r14, spare, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      c2_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      scptm_cfg_br_r14_s& scptm_cfg_br_r14()
      {
        assert_choice_type(types::scptm_cfg_br_r14, type_, "c2");
        return c;
      }
      const scptm_cfg_br_r14_s& scptm_cfg_br_r14() const
      {
        assert_choice_type(types::scptm_cfg_br_r14, type_, "c2");
        return c;
      }
      scptm_cfg_br_r14_s& set_scptm_cfg_br_r14();
      void                set_spare();

    private:
      types              type_;
      scptm_cfg_br_r14_s c;
    };
    struct types_opts {
      enum options { c2, msg_class_ext_future_r14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    msg_class_ext_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c2_c_& c2()
    {
      assert_choice_type(types::c2, type_, "messageClassExtension");
      return c;
    }
    const c2_c_& c2() const
    {
      assert_choice_type(types::c2, type_, "messageClassExtension");
      return c;
    }
    c2_c_& set_c2();
    void   set_msg_class_ext_future_r14();

  private:
    types type_;
    c2_c_ c;
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  sc_mcch_msg_type_r13_c() = default;
  sc_mcch_msg_type_r13_c(const sc_mcch_msg_type_r13_c& other);
  sc_mcch_msg_type_r13_c& operator=(const sc_mcch_msg_type_r13_c& other);
  ~sc_mcch_msg_type_r13_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "SC-MCCH-MessageType-r13");
    return c.get<c1_c_>();
  }
  msg_class_ext_c_& msg_class_ext()
  {
    assert_choice_type(types::msg_class_ext, type_, "SC-MCCH-MessageType-r13");
    return c.get<msg_class_ext_c_>();
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "SC-MCCH-MessageType-r13");
    return c.get<c1_c_>();
  }
  const msg_class_ext_c_& msg_class_ext() const
  {
    assert_choice_type(types::msg_class_ext, type_, "SC-MCCH-MessageType-r13");
    return c.get<msg_class_ext_c_>();
  }
  c1_c_&            set_c1();
  msg_class_ext_c_& set_msg_class_ext();

private:
  types                                    type_;
  choice_buffer_t<c1_c_, msg_class_ext_c_> c;

  void destroy_();
};

// SC-MCCH-Message-r13 ::= SEQUENCE
struct sc_mcch_msg_r13_s {
  sc_mcch_msg_type_r13_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FailureReportSCG-v12d0 ::= SEQUENCE
struct fail_report_scg_v12d0_s {
  bool                           meas_result_neigh_cells_v12d0_present = false;
  meas_result_list2_eutra_v9e0_l meas_result_neigh_cells_v12d0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-WeightedLayersCapabilities-r13 ::= SEQUENCE
struct mimo_weighted_layers_cap_r13_s {
  struct rel_weight_two_layers_r13_opts {
    enum options { v1, v1dot25, v1dot5, v1dot75, v2, v2dot5, v3, v4, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<rel_weight_two_layers_r13_opts> rel_weight_two_layers_r13_e_;
  struct rel_weight_four_layers_r13_opts {
    enum options { v1, v1dot25, v1dot5, v1dot75, v2, v2dot5, v3, v4, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<rel_weight_four_layers_r13_opts> rel_weight_four_layers_r13_e_;
  struct rel_weight_eight_layers_r13_opts {
    enum options { v1, v1dot25, v1dot5, v1dot75, v2, v2dot5, v3, v4, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<rel_weight_eight_layers_r13_opts> rel_weight_eight_layers_r13_e_;

  // member variables
  bool                           rel_weight_four_layers_r13_present  = false;
  bool                           rel_weight_eight_layers_r13_present = false;
  rel_weight_two_layers_r13_e_   rel_weight_two_layers_r13;
  rel_weight_four_layers_r13_e_  rel_weight_four_layers_r13;
  rel_weight_eight_layers_r13_e_ rel_weight_eight_layers_r13;
  uint8_t                        total_weighted_layers_r13 = 2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-UE-Parameters-v13e0 ::= SEQUENCE
struct mimo_ue_params_v13e0_s {
  bool                           mimo_weighted_layers_cap_r13_present = false;
  mimo_weighted_layers_cap_r13_s mimo_weighted_layers_cap_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultSCG-FailureMRDC-r15 ::= SEQUENCE
struct meas_result_scg_fail_mrdc_r15_s {
  bool                          ext = false;
  meas_result_list3_eutra_r15_l meas_result_freq_list_eutra_r15;
  // ...
  // group 0
  copy_ptr<location_info_r10_s>             location_info_r16;
  copy_ptr<log_meas_result_list_bt_r15_l>   log_meas_result_list_bt_r16;
  copy_ptr<log_meas_result_list_wlan_r15_l> log_meas_result_list_wlan_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v13e0 ::= SEQUENCE
struct phy_layer_params_v13e0_s {
  mimo_ue_params_v13e0_s mimo_ue_params_v13e0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCGFailureInformation-v12d0b-IEs ::= SEQUENCE
struct scg_fail_info_v12d0b_ies_s {
  bool                    fail_report_scg_v12d0_present = false;
  bool                    non_crit_ext_present          = false;
  fail_report_scg_v12d0_s fail_report_scg_v12d0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v13e0b-IEs ::= SEQUENCE
struct ue_eutra_cap_v13e0b_ies_s {
  bool                     non_crit_ext_present = false;
  phy_layer_params_v13e0_s phy_layer_params_v13e0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSI-RS-TriggeredList-r12 ::= SEQUENCE (SIZE (1..96)) OF INTEGER (1..96)
using csi_rs_triggered_list_r12_l = dyn_array<uint8_t>;

// SSB-IndexList-r15 ::= SEQUENCE (SIZE (1..64)) OF INTEGER (0..63)
using ssb_idx_list_r15_l = dyn_array<uint8_t>;

struct cells_triggered_list_item_c_ {
  struct pci_utra_c_ {
    struct types_opts {
      enum options { fdd, tdd, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    pci_utra_c_() = default;
    pci_utra_c_(const pci_utra_c_& other);
    pci_utra_c_& operator=(const pci_utra_c_& other);
    ~pci_utra_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t& fdd()
    {
      assert_choice_type(types::fdd, type_, "physCellIdUTRA");
      return c.get<uint16_t>();
    }
    uint8_t& tdd()
    {
      assert_choice_type(types::tdd, type_, "physCellIdUTRA");
      return c.get<uint8_t>();
    }
    const uint16_t& fdd() const
    {
      assert_choice_type(types::fdd, type_, "physCellIdUTRA");
      return c.get<uint16_t>();
    }
    const uint8_t& tdd() const
    {
      assert_choice_type(types::tdd, type_, "physCellIdUTRA");
      return c.get<uint8_t>();
    }
    uint16_t& set_fdd();
    uint8_t&  set_tdd();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct pci_geran_s_ {
    carrier_freq_geran_s carrier_freq;
    pci_geran_s          pci;
  };
  struct pci_nr_r15_s_ {
    bool               rs_idx_list_r15_present = false;
    uint32_t           carrier_freq            = 0;
    uint16_t           pci                     = 0;
    ssb_idx_list_r15_l rs_idx_list_r15;
  };
  struct types_opts {
    enum options { pci_eutra, pci_utra, pci_geran, pci_cdma2000, wlan_ids_r13, pci_nr_r15, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  cells_triggered_list_item_c_() = default;
  cells_triggered_list_item_c_(const cells_triggered_list_item_c_& other);
  cells_triggered_list_item_c_& operator=(const cells_triggered_list_item_c_& other);
  ~cells_triggered_list_item_c_() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& pci_eutra()
  {
    assert_choice_type(types::pci_eutra, type_, "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  pci_utra_c_& pci_utra()
  {
    assert_choice_type(types::pci_utra, type_, "CellsTriggeredList-item");
    return c.get<pci_utra_c_>();
  }
  pci_geran_s_& pci_geran()
  {
    assert_choice_type(types::pci_geran, type_, "CellsTriggeredList-item");
    return c.get<pci_geran_s_>();
  }
  uint16_t& pci_cdma2000()
  {
    assert_choice_type(types::pci_cdma2000, type_, "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  wlan_ids_r12_s& wlan_ids_r13()
  {
    assert_choice_type(types::wlan_ids_r13, type_, "CellsTriggeredList-item");
    return c.get<wlan_ids_r12_s>();
  }
  pci_nr_r15_s_& pci_nr_r15()
  {
    assert_choice_type(types::pci_nr_r15, type_, "CellsTriggeredList-item");
    return c.get<pci_nr_r15_s_>();
  }
  const uint16_t& pci_eutra() const
  {
    assert_choice_type(types::pci_eutra, type_, "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  const pci_utra_c_& pci_utra() const
  {
    assert_choice_type(types::pci_utra, type_, "CellsTriggeredList-item");
    return c.get<pci_utra_c_>();
  }
  const pci_geran_s_& pci_geran() const
  {
    assert_choice_type(types::pci_geran, type_, "CellsTriggeredList-item");
    return c.get<pci_geran_s_>();
  }
  const uint16_t& pci_cdma2000() const
  {
    assert_choice_type(types::pci_cdma2000, type_, "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  const wlan_ids_r12_s& wlan_ids_r13() const
  {
    assert_choice_type(types::wlan_ids_r13, type_, "CellsTriggeredList-item");
    return c.get<wlan_ids_r12_s>();
  }
  const pci_nr_r15_s_& pci_nr_r15() const
  {
    assert_choice_type(types::pci_nr_r15, type_, "CellsTriggeredList-item");
    return c.get<pci_nr_r15_s_>();
  }
  uint16_t&       set_pci_eutra();
  pci_utra_c_&    set_pci_utra();
  pci_geran_s_&   set_pci_geran();
  uint16_t&       set_pci_cdma2000();
  wlan_ids_r12_s& set_wlan_ids_r13();
  pci_nr_r15_s_&  set_pci_nr_r15();

private:
  types                                                                     type_;
  choice_buffer_t<pci_geran_s_, pci_nr_r15_s_, pci_utra_c_, wlan_ids_r12_s> c;

  void destroy_();
};

// CellsTriggeredList ::= SEQUENCE (SIZE (1..32)) OF CellsTriggeredList-item
using cells_triggered_list_l = dyn_array<cells_triggered_list_item_c_>;

// DRB-InfoSCG-r12 ::= SEQUENCE
struct drb_info_scg_r12_s {
  struct drb_type_r12_opts {
    enum options { split, scg, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<drb_type_r12_opts> drb_type_r12_e_;

  // member variables
  bool            ext                       = false;
  bool            eps_bearer_id_r12_present = false;
  bool            drb_type_r12_present      = false;
  uint8_t         eps_bearer_id_r12         = 0;
  uint8_t         drb_id_r12                = 1;
  drb_type_r12_e_ drb_type_r12;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-InfoListSCG-r12 ::= SEQUENCE (SIZE (1..11)) OF DRB-InfoSCG-r12
using drb_info_list_scg_r12_l = dyn_array<drb_info_scg_r12_s>;

// DRB-InfoListSCG-r15 ::= SEQUENCE (SIZE (1..15)) OF DRB-InfoSCG-r12
using drb_info_list_scg_r15_l = dyn_array<drb_info_scg_r12_s>;

// LogMeasInfoList2-r10 ::= SEQUENCE (SIZE (1..4060)) OF LogMeasInfo-r10
using log_meas_info_list2_r10_l = dyn_array<log_meas_info_r10_s>;

// TDD-ConfigSL-r12 ::= SEQUENCE
struct tdd_cfg_sl_r12_s {
  struct sf_assign_sl_r12_opts {
    enum options { none, sa0, sa1, sa2, sa3, sa4, sa5, sa6, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<sf_assign_sl_r12_opts> sf_assign_sl_r12_e_;

  // member variables
  sf_assign_sl_r12_e_ sf_assign_sl_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MasterInformationBlock-SL ::= SEQUENCE
struct mib_sl_s {
  struct sl_bw_r12_opts {
    enum options { n6, n15, n25, n50, n75, n100, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sl_bw_r12_opts> sl_bw_r12_e_;

  // member variables
  sl_bw_r12_e_        sl_bw_r12;
  tdd_cfg_sl_r12_s    tdd_cfg_sl_r12;
  fixed_bitstring<10> direct_frame_num_r12;
  uint8_t             direct_sf_num_r12 = 0;
  bool                in_coverage_r12   = false;
  fixed_bitstring<19> reserved_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MasterInformationBlock-SL-V2X-r14 ::= SEQUENCE
struct mib_sl_v2x_r14_s {
  struct sl_bw_r14_opts {
    enum options { n6, n15, n25, n50, n75, n100, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sl_bw_r14_opts> sl_bw_r14_e_;

  // member variables
  sl_bw_r14_e_        sl_bw_r14;
  tdd_cfg_sl_r12_s    tdd_cfg_sl_r14;
  fixed_bitstring<10> direct_frame_num_r14;
  uint8_t             direct_sf_num_r14 = 0;
  bool                in_coverage_r14   = false;
  fixed_bitstring<27> reserved_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultRSSI-SCG-r13 ::= SEQUENCE
struct meas_result_rssi_scg_r13_s {
  uint8_t                    serv_cell_id_r13 = 0;
  meas_result_for_rssi_r13_s meas_result_for_rssi_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultListRSSI-SCG-r13 ::= SEQUENCE (SIZE (1..32)) OF MeasResultRSSI-SCG-r13
using meas_result_list_rssi_scg_r13_l = dyn_array<meas_result_rssi_scg_r13_s>;

// MeasResultServCellSCG-r12 ::= SEQUENCE
struct meas_result_serv_cell_scg_r12_s {
  struct meas_result_scell_r12_s_ {
    uint8_t rsrp_result_scell_r12 = 0;
    uint8_t rsrq_result_scell_r12 = 0;
  };
  struct meas_result_scell_v1310_s_ {
    uint8_t rs_sinr_result_scell_r13 = 0;
  };

  // member variables
  bool                     ext              = false;
  uint8_t                  serv_cell_id_r12 = 0;
  meas_result_scell_r12_s_ meas_result_scell_r12;
  // ...
  // group 0
  bool                                 serv_cell_id_r13_present = false;
  uint8_t                              serv_cell_id_r13         = 0;
  copy_ptr<meas_result_scell_v1310_s_> meas_result_scell_v1310;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultServCellListSCG-Ext-r13 ::= SEQUENCE (SIZE (1..32)) OF MeasResultServCellSCG-r12
using meas_result_serv_cell_list_scg_ext_r13_l = dyn_array<meas_result_serv_cell_scg_r12_s>;

// MeasResultServCellListSCG-r12 ::= SEQUENCE (SIZE (1..5)) OF MeasResultServCellSCG-r12
using meas_result_serv_cell_list_scg_r12_l = dyn_array<meas_result_serv_cell_scg_r12_s>;

// RLF-Report-NB-r16 ::= SEQUENCE
struct rlf_report_nb_r16_s {
  struct meas_result_last_serv_cell_r16_s_ {
    bool    nrsrq_result_r16_present = false;
    uint8_t nrsrp_result_r16         = 0;
    int8_t  nrsrq_result_r16         = -30;
  };

  // member variables
  bool                              reest_cell_id_r16_present   = false;
  bool                              location_info_r16_present   = false;
  bool                              time_since_fail_r16_present = false;
  cell_global_id_eutra_s            failed_pcell_id_r16;
  cell_global_id_eutra_s            reest_cell_id_r16;
  location_info_r10_s               location_info_r16;
  meas_result_last_serv_cell_r16_s_ meas_result_last_serv_cell_r16;
  uint32_t                          time_since_fail_r16 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SBCCH-SL-BCH-MessageType ::= MasterInformationBlock-SL
using sbcch_sl_bch_msg_type_s = mib_sl_s;

// SBCCH-SL-BCH-Message ::= SEQUENCE
struct sbcch_sl_bch_msg_s {
  sbcch_sl_bch_msg_type_s msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SBCCH-SL-BCH-MessageType-V2X-r14 ::= MasterInformationBlock-SL-V2X-r14
using sbcch_sl_bch_msg_type_v2x_r14_s = mib_sl_v2x_r14_s;

// SBCCH-SL-BCH-Message-V2X-r14 ::= SEQUENCE
struct sbcch_sl_bch_msg_v2x_r14_s {
  sbcch_sl_bch_msg_type_v2x_r14_s msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-Config-v12i0b-IEs ::= SEQUENCE
struct scg_cfg_v12i0b_ies_s {
  bool                     scg_radio_cfg_v12i0_present = false;
  bool                     non_crit_ext_present        = false;
  scg_cfg_part_scg_v12f0_s scg_radio_cfg_v12i0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-ConfigInfo-v1530-IEs ::= SEQUENCE
struct scg_cfg_info_v1530_ies_s {
  bool                      drb_to_add_mod_list_scg_r15_present = false;
  bool                      drb_to_release_list_scg_r15_present = false;
  bool                      non_crit_ext_present                = false;
  drb_info_list_scg_r15_l   drb_to_add_mod_list_scg_r15;
  drb_to_release_list_r15_l drb_to_release_list_scg_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-ConfigInfo-v1430-IEs ::= SEQUENCE
struct scg_cfg_info_v1430_ies_s {
  bool                           make_before_break_scg_req_r14_present = false;
  bool                           meas_gap_cfg_per_cc_list_present      = false;
  bool                           non_crit_ext_present                  = false;
  meas_gap_cfg_per_cc_list_r14_c meas_gap_cfg_per_cc_list;
  scg_cfg_info_v1530_ies_s       non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-ConfigInfo-v1330-IEs ::= SEQUENCE
struct scg_cfg_info_v1330_ies_s {
  bool                            meas_result_list_rssi_scg_r13_present = false;
  bool                            non_crit_ext_present                  = false;
  meas_result_list_rssi_scg_r13_l meas_result_list_rssi_scg_r13;
  scg_cfg_info_v1430_ies_s        non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-ConfigInfo-v1310-IEs ::= SEQUENCE
struct scg_cfg_info_v1310_ies_s {
  bool                                     meas_result_sstd_r13_present                   = false;
  bool                                     scell_to_add_mod_list_mcg_ext_r13_present      = false;
  bool                                     meas_result_serv_cell_list_scg_ext_r13_present = false;
  bool                                     scell_to_add_mod_list_scg_ext_r13_present      = false;
  bool                                     scell_to_release_list_scg_ext_r13_present      = false;
  bool                                     non_crit_ext_present                           = false;
  meas_result_sstd_r13_s                   meas_result_sstd_r13;
  scell_to_add_mod_list_ext_r13_l          scell_to_add_mod_list_mcg_ext_r13;
  meas_result_serv_cell_list_scg_ext_r13_l meas_result_serv_cell_list_scg_ext_r13;
  scell_to_add_mod_list_scg_ext_r13_l      scell_to_add_mod_list_scg_ext_r13;
  scell_to_release_list_ext_r13_l          scell_to_release_list_scg_ext_r13;
  scg_cfg_info_v1330_ies_s                 non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-ConfigRestrictInfo-r12 ::= SEQUENCE
struct scg_cfg_restrict_info_r12_s {
  uint8_t max_sch_tb_bits_dl_r12 = 1;
  uint8_t max_sch_tb_bits_ul_r12 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-ConfigInfo-r12-IEs ::= SEQUENCE
struct scg_cfg_info_r12_ies_s {
  bool                                 rr_cfg_ded_mcg_r12_present                 = false;
  bool                                 scell_to_add_mod_list_mcg_r12_present      = false;
  bool                                 meas_gap_cfg_r12_present                   = false;
  bool                                 pwr_coordination_info_r12_present          = false;
  bool                                 scg_radio_cfg_r12_present                  = false;
  bool                                 eutra_cap_info_r12_present                 = false;
  bool                                 scg_cfg_restrict_info_r12_present          = false;
  bool                                 mbms_interest_ind_r12_present              = false;
  bool                                 meas_result_serv_cell_list_scg_r12_present = false;
  bool                                 drb_to_add_mod_list_scg_r12_present        = false;
  bool                                 drb_to_release_list_scg_r12_present        = false;
  bool                                 scell_to_add_mod_list_scg_r12_present      = false;
  bool                                 scell_to_release_list_scg_r12_present      = false;
  bool                                 p_max_r12_present                          = false;
  bool                                 non_crit_ext_present                       = false;
  rr_cfg_ded_s                         rr_cfg_ded_mcg_r12;
  scell_to_add_mod_list_r10_l          scell_to_add_mod_list_mcg_r12;
  meas_gap_cfg_c                       meas_gap_cfg_r12;
  pwr_coordination_info_r12_s          pwr_coordination_info_r12;
  scg_cfg_part_scg_r12_s               scg_radio_cfg_r12;
  dyn_octstring                        eutra_cap_info_r12;
  scg_cfg_restrict_info_r12_s          scg_cfg_restrict_info_r12;
  dyn_octstring                        mbms_interest_ind_r12;
  meas_result_serv_cell_list_scg_r12_l meas_result_serv_cell_list_scg_r12;
  drb_info_list_scg_r12_l              drb_to_add_mod_list_scg_r12;
  drb_to_release_list_l                drb_to_release_list_scg_r12;
  scell_to_add_mod_list_scg_r12_l      scell_to_add_mod_list_scg_r12;
  scell_to_release_list_r10_l          scell_to_release_list_scg_r12;
  int8_t                               p_max_r12 = -30;
  scg_cfg_info_v1310_ies_s             non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCG-ConfigInfo-r12 ::= SEQUENCE
struct scg_cfg_info_r12_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { scg_cfg_info_r12, spare7, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      c1_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      scg_cfg_info_r12_ies_s& scg_cfg_info_r12()
      {
        assert_choice_type(types::scg_cfg_info_r12, type_, "c1");
        return c;
      }
      const scg_cfg_info_r12_ies_s& scg_cfg_info_r12() const
      {
        assert_choice_type(types::scg_cfg_info_r12, type_, "c1");
        return c;
      }
      scg_cfg_info_r12_ies_s& set_scg_cfg_info_r12();
      void                    set_spare7();
      void                    set_spare6();
      void                    set_spare5();
      void                    set_spare4();
      void                    set_spare3();
      void                    set_spare2();
      void                    set_spare1();

    private:
      types                  type_;
      scg_cfg_info_r12_ies_s c;
    };
    using types = c1_or_crit_ext_e;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    c1_c_& c1()
    {
      assert_choice_type(types::c1, type_, "criticalExtensions");
      return c;
    }
    const c1_c_& c1() const
    {
      assert_choice_type(types::c1, type_, "criticalExtensions");
      return c;
    }
    c1_c_& set_c1();
    void   set_crit_exts_future();

  private:
    types type_;
    c1_c_ c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PPPP-TxPreconfigIndex-r14 ::= SEQUENCE
struct sl_pppp_tx_precfg_idx_r14_s {
  using tx_cfg_idx_list_r14_l_ = bounded_array<uint8_t, 16>;

  // member variables
  uint8_t                prio_thres_r14         = 1;
  uint8_t                default_tx_cfg_idx_r14 = 0;
  uint8_t                cbr_cfg_idx_r14        = 0;
  tx_cfg_idx_list_r14_l_ tx_cfg_idx_list_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-CBR-PPPP-TxPreconfigList-r14 ::= SEQUENCE (SIZE (1..8)) OF SL-PPPP-TxPreconfigIndex-r14
using sl_cbr_pppp_tx_precfg_list_r14_l = dyn_array<sl_pppp_tx_precfg_idx_r14_s>;

// SL-PPPP-TxPreconfigIndex-v1530 ::= SEQUENCE
struct sl_pppp_tx_precfg_idx_v1530_s {
  using mcs_pssch_range_r15_l_ = dyn_array<mcs_pssch_range_r15_s>;

  // member variables
  bool                   mcs_pssch_range_r15_present = false;
  mcs_pssch_range_r15_l_ mcs_pssch_range_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-CBR-PPPP-TxPreconfigList-v1530 ::= SEQUENCE (SIZE (1..8)) OF SL-PPPP-TxPreconfigIndex-v1530
using sl_cbr_pppp_tx_precfg_list_v1530_l = dyn_array<sl_pppp_tx_precfg_idx_v1530_s>;

// SL-CBR-PreconfigTxConfigList-r14 ::= SEQUENCE
struct sl_cbr_precfg_tx_cfg_list_r14_s {
  using cbr_range_common_cfg_list_r14_l_ = dyn_array<sl_cbr_levels_cfg_r14_l>;
  using sl_cbr_pssch_tx_cfg_list_r14_l_  = dyn_array<sl_cbr_pssch_tx_cfg_r14_s>;

  // member variables
  cbr_range_common_cfg_list_r14_l_ cbr_range_common_cfg_list_r14;
  sl_cbr_pssch_tx_cfg_list_r14_l_  sl_cbr_pssch_tx_cfg_list_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PreconfigCommPool-r12 ::= SEQUENCE
struct sl_precfg_comm_pool_r12_s {
  bool                    ext = false;
  sl_cp_len_r12_e         sc_cp_len_r12;
  sl_period_comm_r12_e    sc_period_r12;
  sl_tf_res_cfg_r12_s     sc_tf_res_cfg_r12;
  int8_t                  sc_tx_params_r12 = -126;
  sl_cp_len_r12_e         data_cp_len_r12;
  sl_tf_res_cfg_r12_s     data_tf_res_cfg_r12;
  sl_hop_cfg_comm_r12_s   data_hop_cfg_r12;
  int8_t                  data_tx_params_r12 = -126;
  bounded_bitstring<3, 5> trpt_subset_r12;
  // ...
  // group 0
  copy_ptr<sl_prio_list_r13_l> prio_list_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PreconfigCommPoolList4-r12 ::= SEQUENCE (SIZE (1..4)) OF SL-PreconfigCommPool-r12
using sl_precfg_comm_pool_list4_r12_l = dyn_array<sl_precfg_comm_pool_r12_s>;

// SL-PreconfigCommRxPoolList-r13 ::= SEQUENCE (SIZE (1..12)) OF SL-PreconfigCommPool-r12
using sl_precfg_comm_rx_pool_list_r13_l = dyn_array<sl_precfg_comm_pool_r12_s>;

// SL-PreconfigCommTxPoolList-r13 ::= SEQUENCE (SIZE (1..7)) OF SL-PreconfigCommPool-r12
using sl_precfg_comm_tx_pool_list_r13_l = dyn_array<sl_precfg_comm_pool_r12_s>;

// SL-PreconfigDiscPool-r13 ::= SEQUENCE
struct sl_precfg_disc_pool_r13_s {
  struct disc_period_r13_opts {
    enum options {
      rf4,
      rf6,
      rf7,
      rf8,
      rf12,
      rf14,
      rf16,
      rf24,
      rf28,
      rf32,
      rf64,
      rf128,
      rf256,
      rf512,
      rf1024,
      spare,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<disc_period_r13_opts> disc_period_r13_e_;
  struct tx_params_r13_s_ {
    struct tx_probability_r13_opts {
      enum options { p25, p50, p75, p100, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<tx_probability_r13_opts> tx_probability_r13_e_;

    // member variables
    int8_t                tx_params_general_r13 = -126;
    tx_probability_r13_e_ tx_probability_r13;
  };

  // member variables
  bool                ext                   = false;
  bool                tx_params_r13_present = false;
  sl_cp_len_r12_e     cp_len_r13;
  disc_period_r13_e_  disc_period_r13;
  uint8_t             num_retx_r13   = 0;
  uint8_t             num_repeat_r13 = 1;
  sl_tf_res_cfg_r12_s tf_res_cfg_r13;
  tx_params_r13_s_    tx_params_r13;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PreconfigDiscRxPoolList-r13 ::= SEQUENCE (SIZE (1..16)) OF SL-PreconfigDiscPool-r13
using sl_precfg_disc_rx_pool_list_r13_l = dyn_array<sl_precfg_disc_pool_r13_s>;

// SL-PreconfigDiscTxPoolList-r13 ::= SEQUENCE (SIZE (1..4)) OF SL-PreconfigDiscPool-r13
using sl_precfg_disc_tx_pool_list_r13_l = dyn_array<sl_precfg_disc_pool_r13_s>;

// SL-PreconfigGeneral-r12 ::= SEQUENCE
struct sl_precfg_general_r12_s {
  struct rohc_profiles_r12_s_ {
    bool profile0x0001_r12 = false;
    bool profile0x0002_r12 = false;
    bool profile0x0004_r12 = false;
    bool profile0x0006_r12 = false;
    bool profile0x0101_r12 = false;
    bool profile0x0102_r12 = false;
    bool profile0x0104_r12 = false;
  };
  struct sl_bw_r12_opts {
    enum options { n6, n15, n25, n50, n75, n100, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sl_bw_r12_opts> sl_bw_r12_e_;

  // member variables
  bool                 ext = false;
  rohc_profiles_r12_s_ rohc_profiles_r12;
  uint32_t             carrier_freq_r12      = 0;
  int8_t               max_tx_pwr_r12        = -30;
  uint8_t              add_spec_emission_r12 = 1;
  sl_bw_r12_e_         sl_bw_r12;
  tdd_cfg_sl_r12_s     tdd_cfg_sl_r12;
  fixed_bitstring<19>  reserved_r12;
  // ...
  // group 0
  bool     add_spec_emission_v1440_present = false;
  uint16_t add_spec_emission_v1440         = 33;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PreconfigRelay-r13 ::= SEQUENCE
struct sl_precfg_relay_r13_s {
  resel_info_relay_r13_s resel_info_oo_c_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PreconfigSync-r12 ::= SEQUENCE
struct sl_precfg_sync_r12_s {
  struct sync_ref_min_hyst_r12_opts {
    enum options { db0, db3, db6, db9, db12, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sync_ref_min_hyst_r12_opts> sync_ref_min_hyst_r12_e_;
  struct sync_ref_diff_hyst_r12_opts {
    enum options { db0, db3, db6, db9, db12, dbinf, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sync_ref_diff_hyst_r12_opts> sync_ref_diff_hyst_r12_e_;

  // member variables
  bool                      ext = false;
  sl_cp_len_r12_e           sync_cp_len_r12;
  uint8_t                   sync_offset_ind1_r12    = 0;
  uint8_t                   sync_offset_ind2_r12    = 0;
  int8_t                    sync_tx_params_r12      = -126;
  uint8_t                   sync_tx_thresh_oo_c_r12 = 0;
  filt_coef_e               filt_coef_r12;
  sync_ref_min_hyst_r12_e_  sync_ref_min_hyst_r12;
  sync_ref_diff_hyst_r12_e_ sync_ref_diff_hyst_r12;
  // ...
  // group 0
  bool sync_tx_periodic_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-V2X-PreconfigCommPool-r14 ::= SEQUENCE
struct sl_v2x_precfg_comm_pool_r14_s {
  struct size_subch_r14_opts {
    enum options {
      n4,
      n5,
      n6,
      n8,
      n9,
      n10,
      n12,
      n15,
      n16,
      n18,
      n20,
      n25,
      n30,
      n48,
      n50,
      n72,
      n75,
      n96,
      n100,
      spare13,
      spare12,
      spare11,
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
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<size_subch_r14_opts> size_subch_r14_e_;
  struct num_subch_r14_opts {
    enum options { n1, n3, n5, n8, n10, n15, n20, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<num_subch_r14_opts> num_subch_r14_e_;

  // member variables
  bool                                     ext                                    = false;
  bool                                     sl_offset_ind_r14_present              = false;
  bool                                     start_rb_pscch_pool_r14_present        = false;
  bool                                     zone_id_r14_present                    = false;
  bool                                     thresh_s_rssi_cbr_r14_present          = false;
  bool                                     cbr_pssch_tx_cfg_list_r14_present      = false;
  bool                                     res_sel_cfg_p2_x_r14_present           = false;
  bool                                     sync_allowed_r14_present               = false;
  bool                                     restrict_res_reserv_period_r14_present = false;
  sl_offset_ind_r12_c                      sl_offset_ind_r14;
  sf_bitmap_sl_r14_c                       sl_sf_r14;
  bool                                     adjacency_pscch_pssch_r14 = false;
  size_subch_r14_e_                        size_subch_r14;
  num_subch_r14_e_                         num_subch_r14;
  uint8_t                                  start_rb_subch_r14      = 0;
  uint8_t                                  start_rb_pscch_pool_r14 = 0;
  int8_t                                   data_tx_params_r14      = -126;
  uint8_t                                  zone_id_r14             = 0;
  uint8_t                                  thresh_s_rssi_cbr_r14   = 0;
  sl_cbr_pppp_tx_precfg_list_r14_l         cbr_pssch_tx_cfg_list_r14;
  sl_p2_x_res_sel_cfg_r14_s                res_sel_cfg_p2_x_r14;
  sl_sync_allowed_r14_s                    sync_allowed_r14;
  sl_restrict_res_reserv_period_list_r14_l restrict_res_reserv_period_r14;
  // ...
  // group 0
  copy_ptr<sl_min_t2_value_list_r15_l>         sl_min_t2_value_list_r15;
  copy_ptr<sl_cbr_pppp_tx_precfg_list_v1530_l> cbr_pssch_tx_cfg_list_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PreconfigV2X-RxPoolList-r14 ::= SEQUENCE (SIZE (1..16)) OF SL-V2X-PreconfigCommPool-r14
using sl_precfg_v2x_rx_pool_list_r14_l = dyn_array<sl_v2x_precfg_comm_pool_r14_s>;

// SL-V2X-SyncOffsetIndicators-r14 ::= SEQUENCE
struct sl_v2x_sync_offset_inds_r14_s {
  bool    sync_offset_ind3_r14_present = false;
  uint8_t sync_offset_ind1_r14         = 0;
  uint8_t sync_offset_ind2_r14         = 0;
  uint8_t sync_offset_ind3_r14         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PreconfigV2X-Sync-r14 ::= SEQUENCE
struct sl_precfg_v2x_sync_r14_s {
  struct sync_ref_min_hyst_r14_opts {
    enum options { db0, db3, db6, db9, db12, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sync_ref_min_hyst_r14_opts> sync_ref_min_hyst_r14_e_;
  struct sync_ref_diff_hyst_r14_opts {
    enum options { db0, db3, db6, db9, db12, dbinf, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sync_ref_diff_hyst_r14_opts> sync_ref_diff_hyst_r14_e_;

  // member variables
  bool                          ext = false;
  sl_v2x_sync_offset_inds_r14_s sync_offset_inds_r14;
  int8_t                        sync_tx_params_r14      = -126;
  uint8_t                       sync_tx_thresh_oo_c_r14 = 0;
  filt_coef_e                   filt_coef_r14;
  sync_ref_min_hyst_r14_e_      sync_ref_min_hyst_r14;
  sync_ref_diff_hyst_r14_e_     sync_ref_diff_hyst_r14;
  // ...
  // group 0
  bool slss_tx_disabled_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PreconfigV2X-TxPoolList-r14 ::= SEQUENCE (SIZE (1..8)) OF SL-V2X-PreconfigCommPool-r14
using sl_precfg_v2x_tx_pool_list_r14_l = dyn_array<sl_v2x_precfg_comm_pool_r14_s>;

// SL-Preconfiguration-r12 ::= SEQUENCE
struct sl_precfg_r12_s {
  struct precfg_comm_v1310_s_ {
    bool                              comm_tx_pool_list_r13_present = false;
    sl_precfg_comm_rx_pool_list_r13_l comm_rx_pool_list_r13;
    sl_precfg_comm_tx_pool_list_r13_l comm_tx_pool_list_r13;
  };
  struct precfg_disc_r13_s_ {
    bool                              disc_tx_pool_list_r13_present = false;
    sl_precfg_disc_rx_pool_list_r13_l disc_rx_pool_list_r13;
    sl_precfg_disc_tx_pool_list_r13_l disc_tx_pool_list_r13;
  };

  // member variables
  bool                            ext = false;
  sl_precfg_general_r12_s         precfg_general_r12;
  sl_precfg_sync_r12_s            precfg_sync_r12;
  sl_precfg_comm_pool_list4_r12_l precfg_comm_r12;
  // ...
  // group 0
  copy_ptr<precfg_comm_v1310_s_>  precfg_comm_v1310;
  copy_ptr<precfg_disc_r13_s_>    precfg_disc_r13;
  copy_ptr<sl_precfg_relay_r13_s> precfg_relay_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-V2X-PreconfigFreqInfo-r14 ::= SEQUENCE
struct sl_v2x_precfg_freq_info_r14_s {
  struct sync_prio_r14_opts {
    enum options { gnss, enb, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<sync_prio_r14_opts> sync_prio_r14_e_;

  // member variables
  bool                              ext                                    = false;
  bool                              v2x_comm_precfg_sync_r14_present       = false;
  bool                              v2x_res_sel_cfg_r14_present            = false;
  bool                              zone_cfg_r14_present                   = false;
  bool                              thres_sl_tx_prioritization_r14_present = false;
  bool                              offset_dfn_r14_present                 = false;
  sl_precfg_general_r12_s           v2x_comm_precfg_general_r14;
  sl_precfg_v2x_sync_r14_s          v2x_comm_precfg_sync_r14;
  sl_precfg_v2x_rx_pool_list_r14_l  v2x_comm_rx_pool_list_r14;
  sl_precfg_v2x_tx_pool_list_r14_l  v2x_comm_tx_pool_list_r14;
  sl_precfg_v2x_tx_pool_list_r14_l  p2x_comm_tx_pool_list_r14;
  sl_comm_tx_pool_sensing_cfg_r14_s v2x_res_sel_cfg_r14;
  sl_zone_cfg_r14_s                 zone_cfg_r14;
  sync_prio_r14_e_                  sync_prio_r14;
  uint8_t                           thres_sl_tx_prioritization_r14 = 1;
  uint16_t                          offset_dfn_r14                 = 0;
  // ...
  // group 0
  copy_ptr<sl_v2x_freq_sel_cfg_list_r15_l> v2x_freq_sel_cfg_list_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-V2X-PreconfigFreqList-r14 ::= SEQUENCE (SIZE (1..8)) OF SL-V2X-PreconfigFreqInfo-r14
using sl_v2x_precfg_freq_list_r14_l = dyn_array<sl_v2x_precfg_freq_info_r14_s>;

// SL-V2X-TxProfile-r15 ::= ENUMERATED
struct sl_v2x_tx_profile_r15_opts {
  enum options { rel14, rel15, spare6, spare5, spare4, spare3, spare2, spare1, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<sl_v2x_tx_profile_r15_opts, true> sl_v2x_tx_profile_r15_e;

// SL-V2X-TxProfileList-r15 ::= SEQUENCE (SIZE (1..256)) OF SL-V2X-TxProfile-r15
using sl_v2x_tx_profile_list_r15_l = dyn_array<sl_v2x_tx_profile_r15_e>;

// SL-V2X-Preconfiguration-r14 ::= SEQUENCE
struct sl_v2x_precfg_r14_s {
  bool                                  ext                                  = false;
  bool                                  anchor_carrier_freq_list_r14_present = false;
  bool                                  cbr_precfg_list_r14_present          = false;
  sl_v2x_precfg_freq_list_r14_l         v2x_precfg_freq_list_r14;
  sl_anchor_carrier_freq_list_v2x_r14_l anchor_carrier_freq_list_r14;
  sl_cbr_precfg_tx_cfg_list_r14_s       cbr_precfg_list_r14;
  // ...
  // group 0
  bool                                   slss_tx_multi_freq_r15_present = false;
  copy_ptr<sl_v2x_packet_dupl_cfg_r15_s> v2x_packet_dupl_cfg_r15;
  copy_ptr<sl_v2x_sync_freq_list_r15_l>  sync_freq_list_r15;
  copy_ptr<sl_v2x_tx_profile_list_r15_l> v2x_tx_profile_list_r15;
  // group 1
  copy_ptr<sl_nr_anchor_carrier_freq_list_r16_l> anchor_carrier_freq_list_nr_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarConditionalReconfiguration ::= SEQUENCE
struct var_conditional_recfg_s {
  bool                             cond_recfg_list_r16_present = false;
  cond_recfg_to_add_mod_list_r16_l cond_recfg_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarConnEstFailReport-r11 ::= SEQUENCE
struct var_conn_est_fail_report_r11_s {
  conn_est_fail_report_r11_s conn_est_fail_report_r11;
  plmn_id_s                  plmn_id_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarLogMeasConfig-r10 ::= SEQUENCE
struct var_log_meas_cfg_r10_s {
  bool                 area_cfg_r10_present = false;
  area_cfg_r10_c       area_cfg_r10;
  logging_dur_r10_e    logging_dur_r10;
  logging_interv_r10_e logging_interv_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarLogMeasConfig-r11 ::= SEQUENCE
struct var_log_meas_cfg_r11_s {
  bool                 area_cfg_r10_present   = false;
  bool                 area_cfg_v1130_present = false;
  area_cfg_r10_c       area_cfg_r10;
  area_cfg_v1130_s     area_cfg_v1130;
  logging_dur_r10_e    logging_dur_r10;
  logging_interv_r10_e logging_interv_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarLogMeasConfig-r12 ::= SEQUENCE
struct var_log_meas_cfg_r12_s {
  bool                         area_cfg_r10_present               = false;
  bool                         area_cfg_v1130_present             = false;
  bool                         target_mbsfn_area_list_r12_present = false;
  area_cfg_r10_c               area_cfg_r10;
  area_cfg_v1130_s             area_cfg_v1130;
  logging_dur_r10_e            logging_dur_r10;
  logging_interv_r10_e         logging_interv_r10;
  target_mbsfn_area_list_r12_l target_mbsfn_area_list_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarLogMeasConfig-r15 ::= SEQUENCE
struct var_log_meas_cfg_r15_s {
  bool                         area_cfg_r10_present               = false;
  bool                         area_cfg_v1130_present             = false;
  bool                         target_mbsfn_area_list_r12_present = false;
  bool                         bt_name_list_r15_present           = false;
  bool                         wlan_name_list_r15_present         = false;
  area_cfg_r10_c               area_cfg_r10;
  area_cfg_v1130_s             area_cfg_v1130;
  logging_dur_r10_e            logging_dur_r10;
  logging_interv_r10_e         logging_interv_r10;
  target_mbsfn_area_list_r12_l target_mbsfn_area_list_r12;
  bt_name_list_r15_l           bt_name_list_r15;
  wlan_name_list_r15_l         wlan_name_list_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarLogMeasConfig-r17 ::= SEQUENCE
struct var_log_meas_cfg_r17_s {
  bool                           area_cfg_r10_present                 = false;
  bool                           area_cfg_v1130_present               = false;
  bool                           target_mbsfn_area_list_r12_present   = false;
  bool                           bt_name_list_r15_present             = false;
  bool                           wlan_name_list_r15_present           = false;
  bool                           logged_event_trigger_cfg_r17_present = false;
  bool                           meas_uncom_bar_pre_r17_present       = false;
  area_cfg_r10_c                 area_cfg_r10;
  area_cfg_v1130_s               area_cfg_v1130;
  logging_dur_r10_e              logging_dur_r10;
  logging_interv_r10_e           logging_interv_r10;
  target_mbsfn_area_list_r12_l   target_mbsfn_area_list_r12;
  bt_name_list_r15_l             bt_name_list_r15;
  wlan_name_list_r15_l           wlan_name_list_r15;
  logged_event_trigger_cfg_r17_s logged_event_trigger_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarLogMeasReport-r10 ::= SEQUENCE
struct var_log_meas_report_r10_s {
  trace_ref_r10_s           trace_ref_r10;
  fixed_octstring<2>        trace_recording_session_ref_r10;
  fixed_octstring<1>        tce_id_r10;
  plmn_id_s                 plmn_id_r10;
  fixed_bitstring<48>       absolute_time_info_r10;
  log_meas_info_list2_r10_l log_meas_info_list_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarLogMeasReport-r11 ::= SEQUENCE
struct var_log_meas_report_r11_s {
  trace_ref_r10_s           trace_ref_r10;
  fixed_octstring<2>        trace_recording_session_ref_r10;
  fixed_octstring<1>        tce_id_r10;
  plmn_id_list3_r11_l       plmn_id_list_r11;
  fixed_bitstring<48>       absolute_time_info_r10;
  log_meas_info_list2_r10_l log_meas_info_list_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarMeasIdleConfig-r15 ::= SEQUENCE
struct var_meas_idle_cfg_r15_s {
  struct meas_idle_dur_r15_opts {
    enum options { sec10, sec30, sec60, sec120, sec180, sec240, sec300, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<meas_idle_dur_r15_opts> meas_idle_dur_r15_e_;

  // member variables
  bool                     meas_idle_carrier_list_eutra_r15_present = false;
  eutra_carrier_list_r15_l meas_idle_carrier_list_eutra_r15;
  meas_idle_dur_r15_e_     meas_idle_dur_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarMeasIdleConfig-r16 ::= SEQUENCE
struct var_meas_idle_cfg_r16_s {
  bool                     meas_idle_carrier_list_nr_r16_present = false;
  bool                     validity_area_list_r16_present        = false;
  nr_carrier_list_r16_l    meas_idle_carrier_list_nr_r16;
  validity_area_list_r16_l validity_area_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarMeasIdleReport-r15 ::= SEQUENCE
struct var_meas_idle_report_r15_s {
  meas_result_list_idle_r15_l meas_report_idle_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarMeasIdleReport-r16 ::= SEQUENCE
struct var_meas_idle_report_r16_s {
  bool                            meas_report_idle_r16_present    = false;
  bool                            meas_report_idle_nr_r16_present = false;
  meas_result_list_ext_idle_r16_l meas_report_idle_r16;
  meas_result_list_idle_nr_r16_l  meas_report_idle_nr_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarMeasReport ::= SEQUENCE
struct var_meas_report_s {
  bool                        meas_id_v1250_present             = false;
  bool                        cells_triggered_list_present      = false;
  bool                        csi_rs_triggered_list_r12_present = false;
  bool                        pools_triggered_list_r14_present  = false;
  uint8_t                     meas_id                           = 1;
  uint8_t                     meas_id_v1250                     = 33;
  cells_triggered_list_l      cells_triggered_list;
  csi_rs_triggered_list_r12_l csi_rs_triggered_list_r12;
  tx_res_pool_meas_list_r14_l pools_triggered_list_r14;
  int64_t                     nof_reports_sent = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarMeasReportList ::= SEQUENCE (SIZE (1..32)) OF VarMeasReport
using var_meas_report_list_l = dyn_array<var_meas_report_s>;

// VarMeasReportList-r12 ::= SEQUENCE (SIZE (1..64)) OF VarMeasReport
using var_meas_report_list_r12_l = dyn_array<var_meas_report_s>;

// VarMobilityHistoryReport-r12 ::= VisitedCellInfoList-r12
using var_mob_history_report_r12_l = visited_cell_info_list_r12_l;

// VarRLF-Report-NB-r16 ::= SEQUENCE
struct var_rlf_report_nb_r16_s {
  rlf_report_nb_r16_s rlf_report_r16;
  plmn_id_list3_r11_l plmn_id_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarRLF-Report-r10 ::= SEQUENCE
struct var_rlf_report_r10_s {
  rlf_report_r9_s rlf_report_r10;
  plmn_id_s       plmn_id_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarRLF-Report-r11 ::= SEQUENCE
struct var_rlf_report_r11_s {
  rlf_report_r9_s     rlf_report_r10;
  plmn_id_list3_r11_l plmn_id_list_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarShortINACTIVE-MAC-Input-r15 ::= SEQUENCE
struct var_short_inactive_mac_input_r15_s {
  fixed_bitstring<28> cell_id_r15;
  uint16_t            pci_r15 = 0;
  fixed_bitstring<16> c_rnti_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarShortMAC-Input-NB-r13 ::= VarShortMAC-Input
using var_short_mac_input_nb_r13_s = var_short_mac_input_s;

// VarShortResumeMAC-Input-r13 ::= SEQUENCE
struct var_short_resume_mac_input_r13_s {
  fixed_bitstring<28> cell_id_r13;
  uint16_t            pci_r13 = 0;
  fixed_bitstring<16> c_rnti_r13;
  fixed_bitstring<1>  resume_discriminator_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarShortResumeMAC-Input-NB-r13 ::= VarShortResumeMAC-Input-r13
using var_short_resume_mac_input_nb_r13_s = var_short_resume_mac_input_r13_s;

// VarWLAN-MobilityConfig ::= SEQUENCE
struct var_wlan_mob_cfg_s {
  bool                   wlan_mob_set_r13_present         = false;
  bool                   success_report_requested_present = false;
  bool                   wlan_suspend_cfg_r14_present     = false;
  wlan_id_list_r13_l     wlan_mob_set_r13;
  wlan_suspend_cfg_r14_s wlan_suspend_cfg_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarWLAN-Status-r13 ::= SEQUENCE
struct var_wlan_status_r13_s {
  bool                status_r14_present = false;
  wlan_status_r13_e   status_r13;
  wlan_status_v1430_e status_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc
} // namespace asn1

#endif // SRSASN1_RRC_H
