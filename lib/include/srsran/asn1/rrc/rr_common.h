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

#ifndef SRSASN1_RRC_RR_COMMON_H
#define SRSASN1_RRC_RR_COMMON_H

#include "common.h"

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// PHICH-Config ::= SEQUENCE
struct phich_cfg_s {
  struct phich_dur_opts {
    enum options { normal, extended, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<phich_dur_opts> phich_dur_e_;
  struct phich_res_opts {
    enum options { one_sixth, half, one, two, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<phich_res_opts> phich_res_e_;

  // member variables
  phich_dur_e_ phich_dur;
  phich_res_e_ phich_res;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const phich_cfg_s& other) const;
  bool        operator!=(const phich_cfg_s& other) const { return not(*this == other); }
};

// PLMN-IdentityInfo-r15 ::= SEQUENCE
struct plmn_id_info_r15_s {
  struct plmn_id_minus5_gc_r15_c_ {
    struct types_opts {
      enum options { plmn_id_r15, plmn_idx_r15, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    plmn_id_minus5_gc_r15_c_() = default;
    plmn_id_minus5_gc_r15_c_(const plmn_id_minus5_gc_r15_c_& other);
    plmn_id_minus5_gc_r15_c_& operator=(const plmn_id_minus5_gc_r15_c_& other);
    ~plmn_id_minus5_gc_r15_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    plmn_id_s& plmn_id_r15()
    {
      assert_choice_type(types::plmn_id_r15, type_, "plmn-Identity-5GC-r15");
      return c.get<plmn_id_s>();
    }
    uint8_t& plmn_idx_r15()
    {
      assert_choice_type(types::plmn_idx_r15, type_, "plmn-Identity-5GC-r15");
      return c.get<uint8_t>();
    }
    const plmn_id_s& plmn_id_r15() const
    {
      assert_choice_type(types::plmn_id_r15, type_, "plmn-Identity-5GC-r15");
      return c.get<plmn_id_s>();
    }
    const uint8_t& plmn_idx_r15() const
    {
      assert_choice_type(types::plmn_idx_r15, type_, "plmn-Identity-5GC-r15");
      return c.get<uint8_t>();
    }
    plmn_id_s& set_plmn_id_r15();
    uint8_t&   set_plmn_idx_r15();

  private:
    types                      type_;
    choice_buffer_t<plmn_id_s> c;

    void destroy_();
  };
  struct cell_reserved_for_oper_r15_opts {
    enum options { reserved, not_reserved, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<cell_reserved_for_oper_r15_opts> cell_reserved_for_oper_r15_e_;
  struct cell_reserved_for_oper_crs_r15_opts {
    enum options { reserved, not_reserved, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<cell_reserved_for_oper_crs_r15_opts> cell_reserved_for_oper_crs_r15_e_;

  // member variables
  plmn_id_minus5_gc_r15_c_          plmn_id_minus5_gc_r15;
  cell_reserved_for_oper_r15_e_     cell_reserved_for_oper_r15;
  cell_reserved_for_oper_crs_r15_e_ cell_reserved_for_oper_crs_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellIdentity-5GC-r15 ::= CHOICE
struct cell_id_minus5_gc_r15_c {
  struct types_opts {
    enum options { cell_id_r15, cell_id_idx_r15, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  cell_id_minus5_gc_r15_c() = default;
  cell_id_minus5_gc_r15_c(const cell_id_minus5_gc_r15_c& other);
  cell_id_minus5_gc_r15_c& operator=(const cell_id_minus5_gc_r15_c& other);
  ~cell_id_minus5_gc_r15_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<28>& cell_id_r15()
  {
    assert_choice_type(types::cell_id_r15, type_, "CellIdentity-5GC-r15");
    return c.get<fixed_bitstring<28> >();
  }
  uint8_t& cell_id_idx_r15()
  {
    assert_choice_type(types::cell_id_idx_r15, type_, "CellIdentity-5GC-r15");
    return c.get<uint8_t>();
  }
  const fixed_bitstring<28>& cell_id_r15() const
  {
    assert_choice_type(types::cell_id_r15, type_, "CellIdentity-5GC-r15");
    return c.get<fixed_bitstring<28> >();
  }
  const uint8_t& cell_id_idx_r15() const
  {
    assert_choice_type(types::cell_id_idx_r15, type_, "CellIdentity-5GC-r15");
    return c.get<uint8_t>();
  }
  fixed_bitstring<28>& set_cell_id_r15();
  uint8_t&             set_cell_id_idx_r15();

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<28> > c;

  void destroy_();
};

// PLMN-IdentityList-r15 ::= SEQUENCE (SIZE (1..6)) OF PLMN-IdentityInfo-r15
using plmn_id_list_r15_l = dyn_array<plmn_id_info_r15_s>;

// CellAccessRelatedInfo-5GC-r15 ::= SEQUENCE
struct cell_access_related_info_minus5_gc_r15_s {
  bool                    ran_area_code_r15_present = false;
  plmn_id_list_r15_l      plmn_id_list_r15;
  uint16_t                ran_area_code_r15 = 0;
  fixed_bitstring<24>     tac_minus5_gc_r15;
  cell_id_minus5_gc_r15_c cell_id_minus5_gc_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-Config-v1450 ::= SEQUENCE
struct tdd_cfg_v1450_s {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-Config ::= SEQUENCE
struct tdd_cfg_s {
  struct sf_assign_opts {
    enum options { sa0, sa1, sa2, sa3, sa4, sa5, sa6, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sf_assign_opts> sf_assign_e_;
  struct special_sf_patterns_opts {
    enum options { ssp0, ssp1, ssp2, ssp3, ssp4, ssp5, ssp6, ssp7, ssp8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<special_sf_patterns_opts> special_sf_patterns_e_;

  // member variables
  sf_assign_e_           sf_assign;
  special_sf_patterns_e_ special_sf_patterns;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const tdd_cfg_s& other) const;
  bool        operator!=(const tdd_cfg_s& other) const { return not(*this == other); }
};

// TDD-Config-v1430 ::= SEQUENCE
struct tdd_cfg_v1430_s {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-Config-v1130 ::= SEQUENCE
struct tdd_cfg_v1130_s {
  struct special_sf_patterns_v1130_opts {
    enum options { ssp7, ssp9, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<special_sf_patterns_v1130_opts> special_sf_patterns_v1130_e_;

  // member variables
  special_sf_patterns_v1130_e_ special_sf_patterns_v1130;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const tdd_cfg_v1130_s& other) const;
  bool        operator!=(const tdd_cfg_v1130_s& other) const { return not(*this == other); }
};

// GWUS-NumGroups-r16 ::= ENUMERATED
struct gwus_num_groups_r16_opts {
  enum options { n1, n2, n4, n8, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<gwus_num_groups_r16_opts> gwus_num_groups_r16_e;

// GWUS-GroupsForServiceList-r16 ::= SEQUENCE (SIZE (1..3)) OF INTEGER (1..31)
using gwus_groups_for_service_list_r16_l = bounded_array<uint8_t, 3>;

// GWUS-NumGroupsList-r16 ::= SEQUENCE (SIZE (1..4)) OF GWUS-NumGroups-r16
using gwus_num_groups_list_r16_l = bounded_array<gwus_num_groups_r16_e, 4>;

// GWUS-PagingProbThresh-r16 ::= ENUMERATED
struct gwus_paging_prob_thresh_r16_opts {
  enum options { p20, p30, p40, p50, p60, p70, p80, p90, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<gwus_paging_prob_thresh_r16_opts> gwus_paging_prob_thresh_r16_e;

// PRACH-ParametersCE-r13 ::= SEQUENCE
struct prach_params_ce_r13_s {
  struct prach_start_sf_r13_opts {
    enum options { sf2, sf4, sf8, sf16, sf32, sf64, sf128, sf256, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<prach_start_sf_r13_opts> prach_start_sf_r13_e_;
  struct max_num_preamb_attempt_ce_r13_opts {
    enum options { n3, n4, n5, n6, n7, n8, n10, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_preamb_attempt_ce_r13_opts> max_num_preamb_attempt_ce_r13_e_;
  struct num_repeat_per_preamb_attempt_r13_opts {
    enum options { n1, n2, n4, n8, n16, n32, n64, n128, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<num_repeat_per_preamb_attempt_r13_opts> num_repeat_per_preamb_attempt_r13_e_;
  using mpdcch_nbs_to_monitor_r13_l_ = bounded_array<uint8_t, 2>;
  struct mpdcch_num_repeat_ra_r13_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, r128, r256, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mpdcch_num_repeat_ra_r13_opts> mpdcch_num_repeat_ra_r13_e_;
  struct prach_hop_cfg_r13_opts {
    enum options { on, off, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<prach_hop_cfg_r13_opts> prach_hop_cfg_r13_e_;

  // member variables
  bool                                 prach_start_sf_r13_present            = false;
  bool                                 max_num_preamb_attempt_ce_r13_present = false;
  uint8_t                              prach_cfg_idx_r13                     = 0;
  uint8_t                              prach_freq_offset_r13                 = 0;
  prach_start_sf_r13_e_                prach_start_sf_r13;
  max_num_preamb_attempt_ce_r13_e_     max_num_preamb_attempt_ce_r13;
  num_repeat_per_preamb_attempt_r13_e_ num_repeat_per_preamb_attempt_r13;
  mpdcch_nbs_to_monitor_r13_l_         mpdcch_nbs_to_monitor_r13;
  mpdcch_num_repeat_ra_r13_e_          mpdcch_num_repeat_ra_r13;
  prach_hop_cfg_r13_e_                 prach_hop_cfg_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RACH-CE-LevelInfo-r13 ::= SEQUENCE
struct rach_ce_level_info_r13_s {
  struct preamb_map_info_r13_s_ {
    uint8_t first_preamb_r13 = 0;
    uint8_t last_preamb_r13  = 0;
  };
  struct ra_resp_win_size_r13_opts {
    enum options { sf20, sf50, sf80, sf120, sf180, sf240, sf320, sf400, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<ra_resp_win_size_r13_opts> ra_resp_win_size_r13_e_;
  struct mac_contention_resolution_timer_r13_opts {
    enum options { sf80, sf100, sf120, sf160, sf200, sf240, sf480, sf960, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mac_contention_resolution_timer_r13_opts> mac_contention_resolution_timer_r13_e_;
  struct rar_hop_cfg_r13_opts {
    enum options { on, off, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<rar_hop_cfg_r13_opts> rar_hop_cfg_r13_e_;
  struct edt_params_r15_s_ {
    struct edt_tbs_r15_opts {
      enum options { b328, b408, b504, b600, b712, b808, b936, b1000or456, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<edt_tbs_r15_opts> edt_tbs_r15_e_;
    struct mac_contention_resolution_timer_r15_opts {
      enum options { sf240, sf480, sf960, sf1920, sf3840, sf5760, sf7680, sf10240, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<mac_contention_resolution_timer_r15_opts> mac_contention_resolution_timer_r15_e_;

    // member variables
    bool                                   mac_contention_resolution_timer_r15_present = false;
    uint8_t                                edt_last_preamb_r15                         = 0;
    bool                                   edt_small_tbs_enabled_r15                   = false;
    edt_tbs_r15_e_                         edt_tbs_r15;
    mac_contention_resolution_timer_r15_e_ mac_contention_resolution_timer_r15;
  };

  // member variables
  bool                                   ext = false;
  preamb_map_info_r13_s_                 preamb_map_info_r13;
  ra_resp_win_size_r13_e_                ra_resp_win_size_r13;
  mac_contention_resolution_timer_r13_e_ mac_contention_resolution_timer_r13;
  rar_hop_cfg_r13_e_                     rar_hop_cfg_r13;
  // ...
  // group 0
  copy_ptr<edt_params_r15_s_> edt_params_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DeltaFList-PUCCH ::= SEQUENCE
struct delta_flist_pucch_s {
  struct delta_f_pucch_format1_opts {
    enum options { delta_f_minus2, delta_f0, delta_f2, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<delta_f_pucch_format1_opts> delta_f_pucch_format1_e_;
  struct delta_f_pucch_format1b_opts {
    enum options { delta_f1, delta_f3, delta_f5, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<delta_f_pucch_format1b_opts> delta_f_pucch_format1b_e_;
  struct delta_f_pucch_format2_opts {
    enum options { delta_f_minus2, delta_f0, delta_f1, delta_f2, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<delta_f_pucch_format2_opts> delta_f_pucch_format2_e_;
  struct delta_f_pucch_format2a_opts {
    enum options { delta_f_minus2, delta_f0, delta_f2, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<delta_f_pucch_format2a_opts> delta_f_pucch_format2a_e_;
  struct delta_f_pucch_format2b_opts {
    enum options { delta_f_minus2, delta_f0, delta_f2, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<delta_f_pucch_format2b_opts> delta_f_pucch_format2b_e_;

  // member variables
  delta_f_pucch_format1_e_  delta_f_pucch_format1;
  delta_f_pucch_format1b_e_ delta_f_pucch_format1b;
  delta_f_pucch_format2_e_  delta_f_pucch_format2;
  delta_f_pucch_format2a_e_ delta_f_pucch_format2a;
  delta_f_pucch_format2b_e_ delta_f_pucch_format2b;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const delta_flist_pucch_s& other) const;
  bool        operator!=(const delta_flist_pucch_s& other) const { return not(*this == other); }
};

// DeltaFList-SPUCCH-r15 ::= CHOICE
struct delta_flist_spucch_r15_c {
  struct setup_s_ {
    struct delta_f_slot_spucch_format1_r15_opts {
      enum options {
        delta_f_minus1,
        delta_f0,
        delta_f1,
        delta_f2,
        delta_f3,
        delta_f4,
        delta_f5,
        delta_f6,
        nulltype
      } value;
      typedef int8_t number_type;

      const char* to_string() const;
      int8_t      to_number() const;
    };
    typedef enumerated<delta_f_slot_spucch_format1_r15_opts> delta_f_slot_spucch_format1_r15_e_;
    struct delta_f_slot_spucch_format1a_r15_opts {
      enum options { delta_f1, delta_f2, delta_f3, delta_f4, delta_f5, delta_f6, delta_f7, delta_f8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<delta_f_slot_spucch_format1a_r15_opts> delta_f_slot_spucch_format1a_r15_e_;
    struct delta_f_slot_spucch_format1b_r15_opts {
      enum options { delta_f3, delta_f4, delta_f5, delta_f6, delta_f7, delta_f8, delta_f9, delta_f10, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<delta_f_slot_spucch_format1b_r15_opts> delta_f_slot_spucch_format1b_r15_e_;
    struct delta_f_slot_spucch_format3_r15_opts {
      enum options { delta_f4, delta_f5, delta_f6, delta_f7, delta_f8, delta_f9, delta_f10, delta_f11, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<delta_f_slot_spucch_format3_r15_opts> delta_f_slot_spucch_format3_r15_e_;
    struct delta_f_slot_spucch_rm_format4_r15_opts {
      enum options {
        delta_f13,
        delta_f14,
        delta_f15,
        delta_f16,
        delta_f17,
        delta_f18,
        delta_f19,
        delta_f20,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<delta_f_slot_spucch_rm_format4_r15_opts> delta_f_slot_spucch_rm_format4_r15_e_;
    struct delta_f_slot_spucch_tbcc_format4_r15_opts {
      enum options {
        delta_f10,
        delta_f11,
        delta_f12,
        delta_f13,
        delta_f14,
        delta_f15,
        delta_f16,
        delta_f17,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<delta_f_slot_spucch_tbcc_format4_r15_opts> delta_f_slot_spucch_tbcc_format4_r15_e_;
    struct delta_f_subslot_spucch_format1and1a_r15_opts {
      enum options {
        delta_f5,
        delta_f6,
        delta_f7,
        delta_f8,
        delta_f9,
        delta_f10,
        delta_f11,
        delta_f12,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<delta_f_subslot_spucch_format1and1a_r15_opts> delta_f_subslot_spucch_format1and1a_r15_e_;
    struct delta_f_subslot_spucch_format1b_r15_opts {
      enum options {
        delta_f6,
        delta_f7,
        delta_f8,
        delta_f9,
        delta_f10,
        delta_f11,
        delta_f12,
        delta_f13,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<delta_f_subslot_spucch_format1b_r15_opts> delta_f_subslot_spucch_format1b_r15_e_;
    struct delta_f_subslot_spucch_rm_format4_r15_opts {
      enum options {
        delta_f15,
        delta_f16,
        delta_f17,
        delta_f18,
        delta_f19,
        delta_f20,
        delta_f21,
        delta_f22,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<delta_f_subslot_spucch_rm_format4_r15_opts> delta_f_subslot_spucch_rm_format4_r15_e_;
    struct delta_f_subslot_spucch_tbcc_format4_r15_opts {
      enum options {
        delta_f10,
        delta_f11,
        delta_f12,
        delta_f13,
        delta_f14,
        delta_f15,
        delta_f16,
        delta_f17,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<delta_f_subslot_spucch_tbcc_format4_r15_opts> delta_f_subslot_spucch_tbcc_format4_r15_e_;

    // member variables
    bool                                       ext                                             = false;
    bool                                       delta_f_slot_spucch_format1_r15_present         = false;
    bool                                       delta_f_slot_spucch_format1a_r15_present        = false;
    bool                                       delta_f_slot_spucch_format1b_r15_present        = false;
    bool                                       delta_f_slot_spucch_format3_r15_present         = false;
    bool                                       delta_f_slot_spucch_rm_format4_r15_present      = false;
    bool                                       delta_f_slot_spucch_tbcc_format4_r15_present    = false;
    bool                                       delta_f_subslot_spucch_format1and1a_r15_present = false;
    bool                                       delta_f_subslot_spucch_format1b_r15_present     = false;
    bool                                       delta_f_subslot_spucch_rm_format4_r15_present   = false;
    bool                                       delta_f_subslot_spucch_tbcc_format4_r15_present = false;
    delta_f_slot_spucch_format1_r15_e_         delta_f_slot_spucch_format1_r15;
    delta_f_slot_spucch_format1a_r15_e_        delta_f_slot_spucch_format1a_r15;
    delta_f_slot_spucch_format1b_r15_e_        delta_f_slot_spucch_format1b_r15;
    delta_f_slot_spucch_format3_r15_e_         delta_f_slot_spucch_format3_r15;
    delta_f_slot_spucch_rm_format4_r15_e_      delta_f_slot_spucch_rm_format4_r15;
    delta_f_slot_spucch_tbcc_format4_r15_e_    delta_f_slot_spucch_tbcc_format4_r15;
    delta_f_subslot_spucch_format1and1a_r15_e_ delta_f_subslot_spucch_format1and1a_r15;
    delta_f_subslot_spucch_format1b_r15_e_     delta_f_subslot_spucch_format1b_r15;
    delta_f_subslot_spucch_rm_format4_r15_e_   delta_f_subslot_spucch_rm_format4_r15;
    delta_f_subslot_spucch_tbcc_format4_r15_e_ delta_f_subslot_spucch_tbcc_format4_r15;
    // ...
  };
  using types = setup_e;

  // choice methods
  delta_flist_spucch_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const delta_flist_spucch_r15_c& other) const;
  bool        operator!=(const delta_flist_spucch_r15_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "DeltaFList-SPUCCH-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "DeltaFList-SPUCCH-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// EDT-PRACH-ParametersCE-r15 ::= SEQUENCE
struct edt_prach_params_ce_r15_s {
  struct edt_prach_params_ce_r15_s_ {
    struct prach_start_sf_r15_opts {
      enum options { sf2, sf4, sf8, sf16, sf32, sf64, sf128, sf256, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<prach_start_sf_r15_opts> prach_start_sf_r15_e_;
    using mpdcch_nbs_to_monitor_r15_l_ = bounded_array<uint8_t, 2>;

    // member variables
    bool                         prach_start_sf_r15_present = false;
    uint8_t                      prach_cfg_idx_r15          = 0;
    uint8_t                      prach_freq_offset_r15      = 0;
    prach_start_sf_r15_e_        prach_start_sf_r15;
    mpdcch_nbs_to_monitor_r15_l_ mpdcch_nbs_to_monitor_r15;
  };

  // member variables
  bool                       edt_prach_params_ce_r15_present = false;
  edt_prach_params_ce_r15_s_ edt_prach_params_ce_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GWUS-GroupNarrowBandList-r16 ::= SEQUENCE (SIZE (1..16)) OF BOOLEAN
using gwus_group_narrow_band_list_r16_l = bounded_array<bool, 16>;

// GWUS-ProbThreshList-r16 ::= SEQUENCE (SIZE (1..3)) OF GWUS-PagingProbThresh-r16
using gwus_prob_thresh_list_r16_l = bounded_array<gwus_paging_prob_thresh_r16_e, 3>;

// GWUS-ResourceConfig-r16 ::= SEQUENCE
struct gwus_res_cfg_r16_s {
  struct res_map_pattern_r16_c_ {
    struct res_location_with_wus_opts {
      enum options { primary, secondary, primary3_fdm, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<res_location_with_wus_opts> res_location_with_wus_e_;
    struct res_location_without_wus_opts {
      enum options { n0, n2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<res_location_without_wus_opts> res_location_without_wus_e_;
    struct types_opts {
      enum options { res_location_with_wus, res_location_without_wus, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    res_map_pattern_r16_c_() = default;
    res_map_pattern_r16_c_(const res_map_pattern_r16_c_& other);
    res_map_pattern_r16_c_& operator=(const res_map_pattern_r16_c_& other);
    ~res_map_pattern_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    res_location_with_wus_e_& res_location_with_wus()
    {
      assert_choice_type(types::res_location_with_wus, type_, "resourceMappingPattern-r16");
      return c.get<res_location_with_wus_e_>();
    }
    res_location_without_wus_e_& res_location_without_wus()
    {
      assert_choice_type(types::res_location_without_wus, type_, "resourceMappingPattern-r16");
      return c.get<res_location_without_wus_e_>();
    }
    const res_location_with_wus_e_& res_location_with_wus() const
    {
      assert_choice_type(types::res_location_with_wus, type_, "resourceMappingPattern-r16");
      return c.get<res_location_with_wus_e_>();
    }
    const res_location_without_wus_e_& res_location_without_wus() const
    {
      assert_choice_type(types::res_location_without_wus, type_, "resourceMappingPattern-r16");
      return c.get<res_location_without_wus_e_>();
    }
    res_location_with_wus_e_&    set_res_location_with_wus();
    res_location_without_wus_e_& set_res_location_without_wus();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                               num_groups_list_r16_present         = false;
  bool                               groups_for_service_list_r16_present = false;
  res_map_pattern_r16_c_             res_map_pattern_r16;
  gwus_num_groups_list_r16_l         num_groups_list_r16;
  gwus_groups_for_service_list_r16_l groups_for_service_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GWUS-TimeParameters-r16 ::= SEQUENCE
struct gwus_time_params_r16_s {
  struct max_dur_factor_r16_opts {
    enum options { one32th, one16th, one8th, one4th, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_dur_factor_r16_opts> max_dur_factor_r16_e_;
  struct num_pos_r16_opts {
    enum options { n1, n2, n4, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<num_pos_r16_opts> num_pos_r16_e_;
  struct time_offset_drx_r16_opts {
    enum options { ms40, ms80, ms160, ms240, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<time_offset_drx_r16_opts> time_offset_drx_r16_e_;
  struct time_offset_e_drx_short_r16_opts {
    enum options { ms40, ms80, ms160, ms240, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<time_offset_e_drx_short_r16_opts> time_offset_e_drx_short_r16_e_;
  struct time_offset_e_drx_long_r16_opts {
    enum options { ms1000, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<time_offset_e_drx_long_r16_opts> time_offset_e_drx_long_r16_e_;
  struct num_drx_cycles_relaxed_r16_opts {
    enum options { n1, n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<num_drx_cycles_relaxed_r16_opts> num_drx_cycles_relaxed_r16_e_;
  struct pwr_boost_r16_opts {
    enum options { db0, db1dot8, db3, db4dot8, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<pwr_boost_r16_opts> pwr_boost_r16_e_;

  // member variables
  bool                           ext                                = false;
  bool                           num_pos_r16_present                = false;
  bool                           time_offset_e_drx_long_r16_present = false;
  bool                           num_drx_cycles_relaxed_r16_present = false;
  bool                           pwr_boost_r16_present              = false;
  max_dur_factor_r16_e_          max_dur_factor_r16;
  num_pos_r16_e_                 num_pos_r16;
  time_offset_drx_r16_e_         time_offset_drx_r16;
  time_offset_e_drx_short_r16_e_ time_offset_e_drx_short_r16;
  time_offset_e_drx_long_r16_e_  time_offset_e_drx_long_r16;
  num_drx_cycles_relaxed_r16_e_  num_drx_cycles_relaxed_r16;
  pwr_boost_r16_e_               pwr_boost_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// N1PUCCH-AN-InfoList-r13 ::= SEQUENCE (SIZE (1..4)) OF INTEGER (0..2047)
using n1_pucch_an_info_list_r13_l = bounded_array<uint16_t, 4>;

// PRACH-ConfigInfo ::= SEQUENCE
struct prach_cfg_info_s {
  uint8_t prach_cfg_idx             = 0;
  bool    high_speed_flag           = false;
  uint8_t zero_correlation_zone_cfg = 0;
  uint8_t prach_freq_offset         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const prach_cfg_info_s& other) const;
  bool        operator!=(const prach_cfg_info_s& other) const { return not(*this == other); }
};

// PRACH-ParametersListCE-r13 ::= SEQUENCE (SIZE (1..4)) OF PRACH-ParametersCE-r13
using prach_params_list_ce_r13_l = dyn_array<prach_params_ce_r13_s>;

// PowerRampingParameters ::= SEQUENCE
struct pwr_ramp_params_s {
  struct pwr_ramp_step_opts {
    enum options { db0, db2, db4, db6, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pwr_ramp_step_opts> pwr_ramp_step_e_;
  struct preamb_init_rx_target_pwr_opts {
    enum options {
      dbm_minus120,
      dbm_minus118,
      dbm_minus116,
      dbm_minus114,
      dbm_minus112,
      dbm_minus110,
      dbm_minus108,
      dbm_minus106,
      dbm_minus104,
      dbm_minus102,
      dbm_minus100,
      dbm_minus98,
      dbm_minus96,
      dbm_minus94,
      dbm_minus92,
      dbm_minus90,
      nulltype
    } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<preamb_init_rx_target_pwr_opts> preamb_init_rx_target_pwr_e_;

  // member variables
  pwr_ramp_step_e_             pwr_ramp_step;
  preamb_init_rx_target_pwr_e_ preamb_init_rx_target_pwr;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pwr_ramp_params_s& other) const;
  bool        operator!=(const pwr_ramp_params_s& other) const { return not(*this == other); }
};

// PreambleTransMax ::= ENUMERATED
struct preamb_trans_max_opts {
  enum options { n3, n4, n5, n6, n7, n8, n10, n20, n50, n100, n200, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<preamb_trans_max_opts> preamb_trans_max_e;

// RACH-CE-LevelInfoList-r13 ::= SEQUENCE (SIZE (1..4)) OF RACH-CE-LevelInfo-r13
using rach_ce_level_info_list_r13_l = dyn_array<rach_ce_level_info_r13_s>;

// RSRP-ThresholdsPrachInfoList-r13 ::= SEQUENCE (SIZE (1..3)) OF INTEGER (0..97)
using rsrp_thress_prach_info_list_r13_l = bounded_array<uint8_t, 3>;

// UL-ReferenceSignalsPUSCH ::= SEQUENCE
struct ul_ref_sigs_pusch_s {
  bool    group_hop_enabled  = false;
  uint8_t group_assign_pusch = 0;
  bool    seq_hop_enabled    = false;
  uint8_t cyclic_shift       = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_ref_sigs_pusch_s& other) const;
  bool        operator!=(const ul_ref_sigs_pusch_s& other) const { return not(*this == other); }
};

// BCCH-Config ::= SEQUENCE
struct bcch_cfg_s {
  struct mod_period_coeff_opts {
    enum options { n2, n4, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<mod_period_coeff_opts> mod_period_coeff_e_;

  // member variables
  mod_period_coeff_e_ mod_period_coeff;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCCH-Config-v1310 ::= SEQUENCE
struct bcch_cfg_v1310_s {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CRS-ChEstMPDCCH-ConfigCommon-r16 ::= SEQUENCE
struct crs_ch_est_mpdcch_cfg_common_r16_s {
  struct pwr_ratio_r16_opts {
    enum options { db_minus4dot77, db_minus3, db_minus1dot77, db0, db1, db2, db3, db4dot77, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<pwr_ratio_r16_opts> pwr_ratio_r16_e_;

  // member variables
  pwr_ratio_r16_e_ pwr_ratio_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqHoppingParameters-r13 ::= SEQUENCE
struct freq_hop_params_r13_s {
  struct dummy_opts {
    enum options { nb2, nb4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dummy_opts> dummy_e_;
  struct dummy2_c_ {
    struct interv_fdd_r13_opts {
      enum options { int1, int2, int4, int8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<interv_fdd_r13_opts> interv_fdd_r13_e_;
    struct interv_tdd_r13_opts {
      enum options { int1, int5, int10, int20, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<interv_tdd_r13_opts> interv_tdd_r13_e_;
    struct types_opts {
      enum options { interv_fdd_r13, interv_tdd_r13, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    dummy2_c_() = default;
    dummy2_c_(const dummy2_c_& other);
    dummy2_c_& operator=(const dummy2_c_& other);
    ~dummy2_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    interv_fdd_r13_e_& interv_fdd_r13()
    {
      assert_choice_type(types::interv_fdd_r13, type_, "dummy2");
      return c.get<interv_fdd_r13_e_>();
    }
    interv_tdd_r13_e_& interv_tdd_r13()
    {
      assert_choice_type(types::interv_tdd_r13, type_, "dummy2");
      return c.get<interv_tdd_r13_e_>();
    }
    const interv_fdd_r13_e_& interv_fdd_r13() const
    {
      assert_choice_type(types::interv_fdd_r13, type_, "dummy2");
      return c.get<interv_fdd_r13_e_>();
    }
    const interv_tdd_r13_e_& interv_tdd_r13() const
    {
      assert_choice_type(types::interv_tdd_r13, type_, "dummy2");
      return c.get<interv_tdd_r13_e_>();
    }
    interv_fdd_r13_e_& set_interv_fdd_r13();
    interv_tdd_r13_e_& set_interv_tdd_r13();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct dummy3_c_ {
    struct interv_fdd_r13_opts {
      enum options { int2, int4, int8, int16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<interv_fdd_r13_opts> interv_fdd_r13_e_;
    struct interv_tdd_r13_opts {
      enum options { int5, int10, int20, int40, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<interv_tdd_r13_opts> interv_tdd_r13_e_;
    struct types_opts {
      enum options { interv_fdd_r13, interv_tdd_r13, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    dummy3_c_() = default;
    dummy3_c_(const dummy3_c_& other);
    dummy3_c_& operator=(const dummy3_c_& other);
    ~dummy3_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    interv_fdd_r13_e_& interv_fdd_r13()
    {
      assert_choice_type(types::interv_fdd_r13, type_, "dummy3");
      return c.get<interv_fdd_r13_e_>();
    }
    interv_tdd_r13_e_& interv_tdd_r13()
    {
      assert_choice_type(types::interv_tdd_r13, type_, "dummy3");
      return c.get<interv_tdd_r13_e_>();
    }
    const interv_fdd_r13_e_& interv_fdd_r13() const
    {
      assert_choice_type(types::interv_fdd_r13, type_, "dummy3");
      return c.get<interv_fdd_r13_e_>();
    }
    const interv_tdd_r13_e_& interv_tdd_r13() const
    {
      assert_choice_type(types::interv_tdd_r13, type_, "dummy3");
      return c.get<interv_tdd_r13_e_>();
    }
    interv_fdd_r13_e_& set_interv_fdd_r13();
    interv_tdd_r13_e_& set_interv_tdd_r13();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct interv_ul_hop_cfg_common_mode_a_r13_c_ {
    struct interv_fdd_r13_opts {
      enum options { int1, int2, int4, int8, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<interv_fdd_r13_opts> interv_fdd_r13_e_;
    struct interv_tdd_r13_opts {
      enum options { int1, int5, int10, int20, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<interv_tdd_r13_opts> interv_tdd_r13_e_;
    struct types_opts {
      enum options { interv_fdd_r13, interv_tdd_r13, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    interv_ul_hop_cfg_common_mode_a_r13_c_() = default;
    interv_ul_hop_cfg_common_mode_a_r13_c_(const interv_ul_hop_cfg_common_mode_a_r13_c_& other);
    interv_ul_hop_cfg_common_mode_a_r13_c_& operator=(const interv_ul_hop_cfg_common_mode_a_r13_c_& other);
    ~interv_ul_hop_cfg_common_mode_a_r13_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    interv_fdd_r13_e_& interv_fdd_r13()
    {
      assert_choice_type(types::interv_fdd_r13, type_, "interval-ULHoppingConfigCommonModeA-r13");
      return c.get<interv_fdd_r13_e_>();
    }
    interv_tdd_r13_e_& interv_tdd_r13()
    {
      assert_choice_type(types::interv_tdd_r13, type_, "interval-ULHoppingConfigCommonModeA-r13");
      return c.get<interv_tdd_r13_e_>();
    }
    const interv_fdd_r13_e_& interv_fdd_r13() const
    {
      assert_choice_type(types::interv_fdd_r13, type_, "interval-ULHoppingConfigCommonModeA-r13");
      return c.get<interv_fdd_r13_e_>();
    }
    const interv_tdd_r13_e_& interv_tdd_r13() const
    {
      assert_choice_type(types::interv_tdd_r13, type_, "interval-ULHoppingConfigCommonModeA-r13");
      return c.get<interv_tdd_r13_e_>();
    }
    interv_fdd_r13_e_& set_interv_fdd_r13();
    interv_tdd_r13_e_& set_interv_tdd_r13();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct interv_ul_hop_cfg_common_mode_b_r13_c_ {
    struct interv_fdd_r13_opts {
      enum options { int2, int4, int8, int16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<interv_fdd_r13_opts> interv_fdd_r13_e_;
    struct interv_tdd_r13_opts {
      enum options { int5, int10, int20, int40, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<interv_tdd_r13_opts> interv_tdd_r13_e_;
    struct types_opts {
      enum options { interv_fdd_r13, interv_tdd_r13, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    interv_ul_hop_cfg_common_mode_b_r13_c_() = default;
    interv_ul_hop_cfg_common_mode_b_r13_c_(const interv_ul_hop_cfg_common_mode_b_r13_c_& other);
    interv_ul_hop_cfg_common_mode_b_r13_c_& operator=(const interv_ul_hop_cfg_common_mode_b_r13_c_& other);
    ~interv_ul_hop_cfg_common_mode_b_r13_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    interv_fdd_r13_e_& interv_fdd_r13()
    {
      assert_choice_type(types::interv_fdd_r13, type_, "interval-ULHoppingConfigCommonModeB-r13");
      return c.get<interv_fdd_r13_e_>();
    }
    interv_tdd_r13_e_& interv_tdd_r13()
    {
      assert_choice_type(types::interv_tdd_r13, type_, "interval-ULHoppingConfigCommonModeB-r13");
      return c.get<interv_tdd_r13_e_>();
    }
    const interv_fdd_r13_e_& interv_fdd_r13() const
    {
      assert_choice_type(types::interv_fdd_r13, type_, "interval-ULHoppingConfigCommonModeB-r13");
      return c.get<interv_fdd_r13_e_>();
    }
    const interv_tdd_r13_e_& interv_tdd_r13() const
    {
      assert_choice_type(types::interv_tdd_r13, type_, "interval-ULHoppingConfigCommonModeB-r13");
      return c.get<interv_tdd_r13_e_>();
    }
    interv_fdd_r13_e_& set_interv_fdd_r13();
    interv_tdd_r13_e_& set_interv_tdd_r13();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                                   dummy_present                               = false;
  bool                                   dummy2_present                              = false;
  bool                                   dummy3_present                              = false;
  bool                                   interv_ul_hop_cfg_common_mode_a_r13_present = false;
  bool                                   interv_ul_hop_cfg_common_mode_b_r13_present = false;
  bool                                   dummy4_present                              = false;
  dummy_e_                               dummy;
  dummy2_c_                              dummy2;
  dummy3_c_                              dummy3;
  interv_ul_hop_cfg_common_mode_a_r13_c_ interv_ul_hop_cfg_common_mode_a_r13;
  interv_ul_hop_cfg_common_mode_b_r13_c_ interv_ul_hop_cfg_common_mode_b_r13;
  uint8_t                                dummy4 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GWUS-Config-r16 ::= SEQUENCE
struct gwus_cfg_r16_s {
  struct common_seq_r16_opts {
    enum options { g0, g126, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<common_seq_r16_opts> common_seq_r16_e_;

  // member variables
  bool                              group_alternation_r16_present      = false;
  bool                              common_seq_r16_present             = false;
  bool                              time_params_r16_present            = false;
  bool                              res_cfg_e_drx_short_r16_present    = false;
  bool                              res_cfg_e_drx_long_r16_present     = false;
  bool                              prob_thresh_list_r16_present       = false;
  bool                              group_narrow_band_list_r16_present = false;
  common_seq_r16_e_                 common_seq_r16;
  gwus_time_params_r16_s            time_params_r16;
  gwus_res_cfg_r16_s                res_cfg_drx_r16;
  gwus_res_cfg_r16_s                res_cfg_e_drx_short_r16;
  gwus_res_cfg_r16_s                res_cfg_e_drx_long_r16;
  gwus_prob_thresh_list_r16_l       prob_thresh_list_r16;
  gwus_group_narrow_band_list_r16_l group_narrow_band_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HighSpeedConfig-r14 ::= SEQUENCE
struct high_speed_cfg_r14_s {
  bool high_speed_enhanced_meas_flag_r14_present  = false;
  bool high_speed_enhanced_demod_flag_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HighSpeedConfig-v1530 ::= SEQUENCE
struct high_speed_cfg_v1530_s {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HighSpeedConfig-v1610 ::= SEQUENCE
struct high_speed_cfg_v1610_s {
  bool high_speed_enh_meas_flag2_r16_present  = false;
  bool high_speed_enh_demod_flag2_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCCH-Config ::= SEQUENCE
struct pcch_cfg_s {
  struct default_paging_cycle_opts {
    enum options { rf32, rf64, rf128, rf256, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<default_paging_cycle_opts> default_paging_cycle_e_;
  struct nb_opts {
    enum options {
      four_t,
      two_t,
      one_t,
      half_t,
      quarter_t,
      one_eighth_t,
      one_sixteenth_t,
      one_thirty_second_t,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<nb_opts> nb_e_;

  // member variables
  default_paging_cycle_e_ default_paging_cycle;
  nb_e_                   nb;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCCH-Config-v1310 ::= SEQUENCE
struct pcch_cfg_v1310_s {
  struct mpdcch_num_repeat_paging_r13_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, r128, r256, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<mpdcch_num_repeat_paging_r13_opts> mpdcch_num_repeat_paging_r13_e_;
  struct nb_v1310_opts {
    enum options { one64th_t, one128th_t, one256th_t, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<nb_v1310_opts> nb_v1310_e_;

  // member variables
  bool                            nb_v1310_present        = false;
  uint8_t                         paging_narrow_bands_r13 = 1;
  mpdcch_num_repeat_paging_r13_e_ mpdcch_num_repeat_paging_r13;
  nb_v1310_e_                     nb_v1310;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCCH-Config-v1700 ::= SEQUENCE
struct pcch_cfg_v1700_s {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDSCH-ConfigCommon ::= SEQUENCE
struct pdsch_cfg_common_s {
  int8_t  ref_sig_pwr = -60;
  uint8_t p_b         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pdsch_cfg_common_s& other) const;
  bool        operator!=(const pdsch_cfg_common_s& other) const { return not(*this == other); }
};

// PDSCH-ConfigCommon-v1310 ::= SEQUENCE
struct pdsch_cfg_common_v1310_s {
  struct pdsch_max_num_repeat_cemode_a_r13_opts {
    enum options { r16, r32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pdsch_max_num_repeat_cemode_a_r13_opts> pdsch_max_num_repeat_cemode_a_r13_e_;
  struct pdsch_max_num_repeat_cemode_b_r13_opts {
    enum options { r192, r256, r384, r512, r768, r1024, r1536, r2048, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<pdsch_max_num_repeat_cemode_b_r13_opts> pdsch_max_num_repeat_cemode_b_r13_e_;

  // member variables
  bool                                 pdsch_max_num_repeat_cemode_a_r13_present = false;
  bool                                 pdsch_max_num_repeat_cemode_b_r13_present = false;
  pdsch_max_num_repeat_cemode_a_r13_e_ pdsch_max_num_repeat_cemode_a_r13;
  pdsch_max_num_repeat_cemode_b_r13_e_ pdsch_max_num_repeat_cemode_b_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRACH-Config-v1430 ::= SEQUENCE
struct prach_cfg_v1430_s {
  uint16_t root_seq_idx_high_speed_r14              = 0;
  uint8_t  zero_correlation_zone_cfg_high_speed_r14 = 0;
  uint8_t  prach_cfg_idx_high_speed_r14             = 0;
  uint8_t  prach_freq_offset_high_speed_r14         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const prach_cfg_v1430_s& other) const;
  bool        operator!=(const prach_cfg_v1430_s& other) const { return not(*this == other); }
};

// PRACH-ConfigSIB ::= SEQUENCE
struct prach_cfg_sib_s {
  uint16_t         root_seq_idx = 0;
  prach_cfg_info_s prach_cfg_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRACH-ConfigSIB-v1310 ::= SEQUENCE
struct prach_cfg_sib_v1310_s {
  struct mpdcch_start_sf_css_ra_r13_c_ {
    struct fdd_r13_opts {
      enum options { v1, v1dot5, v2, v2dot5, v4, v5, v8, v10, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<fdd_r13_opts> fdd_r13_e_;
    struct tdd_r13_opts {
      enum options { v1, v2, v4, v5, v8, v10, v20, spare, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<tdd_r13_opts> tdd_r13_e_;
    struct types_opts {
      enum options { fdd_r13, tdd_r13, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    mpdcch_start_sf_css_ra_r13_c_() = default;
    mpdcch_start_sf_css_ra_r13_c_(const mpdcch_start_sf_css_ra_r13_c_& other);
    mpdcch_start_sf_css_ra_r13_c_& operator=(const mpdcch_start_sf_css_ra_r13_c_& other);
    ~mpdcch_start_sf_css_ra_r13_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fdd_r13_e_& fdd_r13()
    {
      assert_choice_type(types::fdd_r13, type_, "mpdcch-startSF-CSS-RA-r13");
      return c.get<fdd_r13_e_>();
    }
    tdd_r13_e_& tdd_r13()
    {
      assert_choice_type(types::tdd_r13, type_, "mpdcch-startSF-CSS-RA-r13");
      return c.get<tdd_r13_e_>();
    }
    const fdd_r13_e_& fdd_r13() const
    {
      assert_choice_type(types::fdd_r13, type_, "mpdcch-startSF-CSS-RA-r13");
      return c.get<fdd_r13_e_>();
    }
    const tdd_r13_e_& tdd_r13() const
    {
      assert_choice_type(types::tdd_r13, type_, "mpdcch-startSF-CSS-RA-r13");
      return c.get<tdd_r13_e_>();
    }
    fdd_r13_e_& set_fdd_r13();
    tdd_r13_e_& set_tdd_r13();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                              mpdcch_start_sf_css_ra_r13_present = false;
  bool                              prach_hop_offset_r13_present       = false;
  rsrp_thress_prach_info_list_r13_l rsrp_thress_prach_info_list_r13;
  mpdcch_start_sf_css_ra_r13_c_     mpdcch_start_sf_css_ra_r13;
  uint8_t                           prach_hop_offset_r13 = 0;
  prach_params_list_ce_r13_l        prach_params_list_ce_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRACH-ConfigSIB-v1530 ::= SEQUENCE
struct prach_cfg_sib_v1530_s {
  using edt_prach_params_list_ce_r15_l_ = dyn_array<edt_prach_params_ce_r15_s>;

  // member variables
  edt_prach_params_list_ce_r15_l_ edt_prach_params_list_ce_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRACH-TxDuration-r17 ::= SEQUENCE
struct prach_tx_dur_r17_s {
  struct prach_tx_dur_r17_opts {
    enum options { n1, n2, n4, n8, n16, n32, n64, n128, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<prach_tx_dur_r17_opts> prach_tx_dur_r17_e_;

  // member variables
  prach_tx_dur_r17_e_ prach_tx_dur_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-ConfigCommon ::= SEQUENCE
struct pucch_cfg_common_s {
  struct delta_pucch_shift_opts {
    enum options { ds1, ds2, ds3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<delta_pucch_shift_opts> delta_pucch_shift_e_;

  // member variables
  delta_pucch_shift_e_ delta_pucch_shift;
  uint8_t              nrb_cqi     = 0;
  uint8_t              ncs_an      = 0;
  uint16_t             n1_pucch_an = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pucch_cfg_common_s& other) const;
  bool        operator!=(const pucch_cfg_common_s& other) const { return not(*this == other); }
};

// PUCCH-ConfigCommon-v1310 ::= SEQUENCE
struct pucch_cfg_common_v1310_s {
  struct pucch_num_repeat_ce_msg4_level0_r13_opts {
    enum options { n1, n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pucch_num_repeat_ce_msg4_level0_r13_opts> pucch_num_repeat_ce_msg4_level0_r13_e_;
  struct pucch_num_repeat_ce_msg4_level1_r13_opts {
    enum options { n1, n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pucch_num_repeat_ce_msg4_level1_r13_opts> pucch_num_repeat_ce_msg4_level1_r13_e_;
  struct pucch_num_repeat_ce_msg4_level2_r13_opts {
    enum options { n4, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pucch_num_repeat_ce_msg4_level2_r13_opts> pucch_num_repeat_ce_msg4_level2_r13_e_;
  struct pucch_num_repeat_ce_msg4_level3_r13_opts {
    enum options { n4, n8, n16, n32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pucch_num_repeat_ce_msg4_level3_r13_opts> pucch_num_repeat_ce_msg4_level3_r13_e_;

  // member variables
  bool                                   n1_pucch_an_info_list_r13_present           = false;
  bool                                   pucch_num_repeat_ce_msg4_level0_r13_present = false;
  bool                                   pucch_num_repeat_ce_msg4_level1_r13_present = false;
  bool                                   pucch_num_repeat_ce_msg4_level2_r13_present = false;
  bool                                   pucch_num_repeat_ce_msg4_level3_r13_present = false;
  n1_pucch_an_info_list_r13_l            n1_pucch_an_info_list_r13;
  pucch_num_repeat_ce_msg4_level0_r13_e_ pucch_num_repeat_ce_msg4_level0_r13;
  pucch_num_repeat_ce_msg4_level1_r13_e_ pucch_num_repeat_ce_msg4_level1_r13;
  pucch_num_repeat_ce_msg4_level2_r13_e_ pucch_num_repeat_ce_msg4_level2_r13;
  pucch_num_repeat_ce_msg4_level3_r13_e_ pucch_num_repeat_ce_msg4_level3_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-ConfigCommon-v1430 ::= SEQUENCE
struct pucch_cfg_common_v1430_s {
  struct pucch_num_repeat_ce_msg4_level3_r14_opts {
    enum options { n64, n128, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pucch_num_repeat_ce_msg4_level3_r14_opts> pucch_num_repeat_ce_msg4_level3_r14_e_;

  // member variables
  bool                                   pucch_num_repeat_ce_msg4_level3_r14_present = false;
  pucch_num_repeat_ce_msg4_level3_r14_e_ pucch_num_repeat_ce_msg4_level3_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUCCH-TxDuration-r17 ::= SEQUENCE
struct pucch_tx_dur_r17_s {
  struct pucch_tx_dur_r17_opts {
    enum options { sf2, sf4, sf8, sf16, sf32, sf64, sf128, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pucch_tx_dur_r17_opts> pucch_tx_dur_r17_e_;

  // member variables
  pucch_tx_dur_r17_e_ pucch_tx_dur_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-ConfigCommon ::= SEQUENCE
struct pusch_cfg_common_s {
  struct pusch_cfg_basic_s_ {
    struct hop_mode_opts {
      enum options { inter_sub_frame, intra_and_inter_sub_frame, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<hop_mode_opts> hop_mode_e_;

    // member variables
    uint8_t     n_sb = 1;
    hop_mode_e_ hop_mode;
    uint8_t     pusch_hop_offset = 0;
    bool        enable64_qam     = false;
  };

  // member variables
  pusch_cfg_basic_s_  pusch_cfg_basic;
  ul_ref_sigs_pusch_s ul_ref_sigs_pusch;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pusch_cfg_common_s& other) const;
  bool        operator!=(const pusch_cfg_common_s& other) const { return not(*this == other); }
};

// PUSCH-ConfigCommon-v1270 ::= SEQUENCE
struct pusch_cfg_common_v1270_s {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pusch_cfg_common_v1270_s& other) const;
  bool        operator!=(const pusch_cfg_common_v1270_s& other) const { return not(*this == other); }
};

// PUSCH-ConfigCommon-v1310 ::= SEQUENCE
struct pusch_cfg_common_v1310_s {
  struct pusch_max_num_repeat_cemode_a_r13_opts {
    enum options { r8, r16, r32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pusch_max_num_repeat_cemode_a_r13_opts> pusch_max_num_repeat_cemode_a_r13_e_;
  struct pusch_max_num_repeat_cemode_b_r13_opts {
    enum options { r192, r256, r384, r512, r768, r1024, r1536, r2048, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<pusch_max_num_repeat_cemode_b_r13_opts> pusch_max_num_repeat_cemode_b_r13_e_;

  // member variables
  bool                                 pusch_max_num_repeat_cemode_a_r13_present = false;
  bool                                 pusch_max_num_repeat_cemode_b_r13_present = false;
  bool                                 pusch_hop_offset_v1310_present            = false;
  pusch_max_num_repeat_cemode_a_r13_e_ pusch_max_num_repeat_cemode_a_r13;
  pusch_max_num_repeat_cemode_b_r13_e_ pusch_max_num_repeat_cemode_b_r13;
  uint8_t                              pusch_hop_offset_v1310 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUSCH-TxDuration-r17 ::= SEQUENCE
struct pusch_tx_dur_r17_s {
  struct pusch_tx_dur_r17_opts {
    enum options { n2, n4, n8, n16, n32, n64, n128, n256, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<pusch_tx_dur_r17_opts> pusch_tx_dur_r17_e_;

  // member variables
  pusch_tx_dur_r17_e_ pusch_tx_dur_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RACH-ConfigCommon ::= SEQUENCE
struct rach_cfg_common_s {
  struct preamb_info_s_ {
    struct nof_ra_preambs_opts {
      enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, n64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<nof_ra_preambs_opts> nof_ra_preambs_e_;
    struct preambs_group_a_cfg_s_ {
      struct size_of_ra_preambs_group_a_opts {
        enum options { n4, n8, n12, n16, n20, n24, n28, n32, n36, n40, n44, n48, n52, n56, n60, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<size_of_ra_preambs_group_a_opts> size_of_ra_preambs_group_a_e_;
      struct msg_size_group_a_opts {
        enum options { b56, b144, b208, b256, nulltype } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
      };
      typedef enumerated<msg_size_group_a_opts> msg_size_group_a_e_;
      struct msg_pwr_offset_group_b_opts {
        enum options { minusinfinity, db0, db5, db8, db10, db12, db15, db18, nulltype } value;
        typedef int8_t number_type;

        const char* to_string() const;
        int8_t      to_number() const;
      };
      typedef enumerated<msg_pwr_offset_group_b_opts> msg_pwr_offset_group_b_e_;

      // member variables
      bool                          ext = false;
      size_of_ra_preambs_group_a_e_ size_of_ra_preambs_group_a;
      msg_size_group_a_e_           msg_size_group_a;
      msg_pwr_offset_group_b_e_     msg_pwr_offset_group_b;
      // ...
    };

    // member variables
    bool                   preambs_group_a_cfg_present = false;
    nof_ra_preambs_e_      nof_ra_preambs;
    preambs_group_a_cfg_s_ preambs_group_a_cfg;
  };
  struct ra_supervision_info_s_ {
    struct ra_resp_win_size_opts {
      enum options { sf2, sf3, sf4, sf5, sf6, sf7, sf8, sf10, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<ra_resp_win_size_opts> ra_resp_win_size_e_;
    struct mac_contention_resolution_timer_opts {
      enum options { sf8, sf16, sf24, sf32, sf40, sf48, sf56, sf64, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<mac_contention_resolution_timer_opts> mac_contention_resolution_timer_e_;

    // member variables
    preamb_trans_max_e                 preamb_trans_max;
    ra_resp_win_size_e_                ra_resp_win_size;
    mac_contention_resolution_timer_e_ mac_contention_resolution_timer;
  };

  // member variables
  bool                   ext = false;
  preamb_info_s_         preamb_info;
  pwr_ramp_params_s      pwr_ramp_params;
  ra_supervision_info_s_ ra_supervision_info;
  uint8_t                max_harq_msg3_tx = 1;
  // ...
  // group 0
  bool                                    preamb_trans_max_ce_r13_present = false;
  preamb_trans_max_e                      preamb_trans_max_ce_r13;
  copy_ptr<rach_ce_level_info_list_r13_l> rach_ce_level_info_list_r13;
  // group 1
  bool edt_small_tbs_subset_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RACH-ConfigCommon-v1250 ::= SEQUENCE
struct rach_cfg_common_v1250_s {
  struct tx_fail_params_r12_s_ {
    struct conn_est_fail_count_r12_opts {
      enum options { n1, n2, n3, n4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<conn_est_fail_count_r12_opts> conn_est_fail_count_r12_e_;
    struct conn_est_fail_offset_validity_r12_opts {
      enum options { s30, s60, s120, s240, s300, s420, s600, s900, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<conn_est_fail_offset_validity_r12_opts> conn_est_fail_offset_validity_r12_e_;

    // member variables
    bool                                 conn_est_fail_offset_r12_present = false;
    conn_est_fail_count_r12_e_           conn_est_fail_count_r12;
    conn_est_fail_offset_validity_r12_e_ conn_est_fail_offset_validity_r12;
    uint8_t                              conn_est_fail_offset_r12 = 0;
  };

  // member variables
  tx_fail_params_r12_s_ tx_fail_params_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RSS-Config-r15 ::= SEQUENCE
struct rss_cfg_r15_s {
  struct dur_r15_opts {
    enum options { sf8, sf16, sf32, sf40, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dur_r15_opts> dur_r15_e_;
  struct periodicity_r15_opts {
    enum options { ms160, ms320, ms640, ms1280, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<periodicity_r15_opts> periodicity_r15_e_;
  struct pwr_boost_r15_opts {
    enum options { db0, db3, db4dot8, db6, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<pwr_boost_r15_opts> pwr_boost_r15_e_;

  // member variables
  dur_r15_e_         dur_r15;
  uint8_t            freq_location_r15 = 0;
  periodicity_r15_e_ periodicity_r15;
  pwr_boost_r15_e_   pwr_boost_r15;
  uint8_t            time_offset_r15 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SoundingRS-UL-ConfigCommon ::= CHOICE
struct srs_ul_cfg_common_c {
  struct setup_s_ {
    struct srs_bw_cfg_opts {
      enum options { bw0, bw1, bw2, bw3, bw4, bw5, bw6, bw7, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<srs_bw_cfg_opts> srs_bw_cfg_e_;
    struct srs_sf_cfg_opts {
      enum options {
        sc0,
        sc1,
        sc2,
        sc3,
        sc4,
        sc5,
        sc6,
        sc7,
        sc8,
        sc9,
        sc10,
        sc11,
        sc12,
        sc13,
        sc14,
        sc15,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<srs_sf_cfg_opts> srs_sf_cfg_e_;

    // member variables
    bool          srs_max_up_pts_present = false;
    srs_bw_cfg_e_ srs_bw_cfg;
    srs_sf_cfg_e_ srs_sf_cfg;
    bool          ack_nack_srs_simul_tx = false;
  };
  using types = setup_e;

  // choice methods
  srs_ul_cfg_common_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const srs_ul_cfg_common_c& other) const;
  bool        operator!=(const srs_ul_cfg_common_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SoundingRS-UL-ConfigCommon");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SoundingRS-UL-ConfigCommon");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// UL-CyclicPrefixLength ::= ENUMERATED
struct ul_cp_len_opts {
  enum options { len1, len2, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<ul_cp_len_opts> ul_cp_len_e;

// UplinkPowerControlCommon ::= SEQUENCE
struct ul_pwr_ctrl_common_s {
  int8_t              p0_nominal_pusch = -126;
  alpha_r12_e         alpha;
  int8_t              p0_nominal_pucch = -127;
  delta_flist_pucch_s delta_flist_pucch;
  int8_t              delta_preamb_msg3 = -1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkPowerControlCommon-v1020 ::= SEQUENCE
struct ul_pwr_ctrl_common_v1020_s {
  struct delta_f_pucch_format3_r10_opts {
    enum options {
      delta_f_minus1,
      delta_f0,
      delta_f1,
      delta_f2,
      delta_f3,
      delta_f4,
      delta_f5,
      delta_f6,
      nulltype
    } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<delta_f_pucch_format3_r10_opts> delta_f_pucch_format3_r10_e_;
  struct delta_f_pucch_format1b_cs_r10_opts {
    enum options { delta_f1, delta_f2, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<delta_f_pucch_format1b_cs_r10_opts> delta_f_pucch_format1b_cs_r10_e_;

  // member variables
  delta_f_pucch_format3_r10_e_     delta_f_pucch_format3_r10;
  delta_f_pucch_format1b_cs_r10_e_ delta_f_pucch_format1b_cs_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkPowerControlCommon-v1530 ::= SEQUENCE
struct ul_pwr_ctrl_common_v1530_s {
  delta_flist_spucch_r15_c delta_flist_spucch_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_pwr_ctrl_common_v1530_s& other) const;
  bool        operator!=(const ul_pwr_ctrl_common_v1530_s& other) const { return not(*this == other); }
};

// UplinkPowerControlCommon-v1610 ::= SEQUENCE
struct ul_pwr_ctrl_common_v1610_s {
  alpha_r12_e alpha_srs_add_r16;
  int8_t      p0_nominal_srs_add_r16 = -126;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WUS-Config-r15 ::= SEQUENCE
struct wus_cfg_r15_s {
  struct max_dur_factor_r15_opts {
    enum options { one32th, one16th, one8th, one4th, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_dur_factor_r15_opts> max_dur_factor_r15_e_;
  struct num_pos_r15_opts {
    enum options { n1, n2, n4, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<num_pos_r15_opts> num_pos_r15_e_;
  struct freq_location_r15_opts {
    enum options { n0, n2, n4, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<freq_location_r15_opts> freq_location_r15_e_;
  struct time_offset_drx_r15_opts {
    enum options { ms40, ms80, ms160, ms240, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<time_offset_drx_r15_opts> time_offset_drx_r15_e_;
  struct time_offset_e_drx_short_r15_opts {
    enum options { ms40, ms80, ms160, ms240, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<time_offset_e_drx_short_r15_opts> time_offset_e_drx_short_r15_e_;
  struct time_offset_e_drx_long_r15_opts {
    enum options { ms1000, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<time_offset_e_drx_long_r15_opts> time_offset_e_drx_long_r15_e_;

  // member variables
  bool                           num_pos_r15_present                = false;
  bool                           time_offset_e_drx_long_r15_present = false;
  max_dur_factor_r15_e_          max_dur_factor_r15;
  num_pos_r15_e_                 num_pos_r15;
  freq_location_r15_e_           freq_location_r15;
  time_offset_drx_r15_e_         time_offset_drx_r15;
  time_offset_e_drx_short_r15_e_ time_offset_e_drx_short_r15;
  time_offset_e_drx_long_r15_e_  time_offset_e_drx_long_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WUS-Config-v1560 ::= SEQUENCE
struct wus_cfg_v1560_s {
  struct pwr_boost_r15_opts {
    enum options { db0, db1dot8, db3, db4dot8, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<pwr_boost_r15_opts> pwr_boost_r15_e_;

  // member variables
  pwr_boost_r15_e_ pwr_boost_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WUS-Config-v1610 ::= SEQUENCE
struct wus_cfg_v1610_s {
  struct num_drx_cycles_relaxed_r16_opts {
    enum options { n1, n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<num_drx_cycles_relaxed_r16_opts> num_drx_cycles_relaxed_r16_e_;

  // member variables
  num_drx_cycles_relaxed_r16_e_ num_drx_cycles_relaxed_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioResourceConfigCommonSIB ::= SEQUENCE
struct rr_cfg_common_sib_s {
  struct ntn_cfg_common_r17_s_ {
    struct t318_r17_opts {
      enum options { ms0, ms50, ms100, ms200, ms500, ms1000, ms2000, ms4000, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<t318_r17_opts> t318_r17_e_;

    // member variables
    bool               ta_report_r17_present    = false;
    bool               prach_tx_dur_r17_present = false;
    bool               pucch_tx_dur_r17_present = false;
    bool               pusch_tx_dur_r17_present = false;
    t318_r17_e_        t318_r17;
    prach_tx_dur_r17_s prach_tx_dur_r17;
    pucch_tx_dur_r17_s pucch_tx_dur_r17;
    pusch_tx_dur_r17_s pusch_tx_dur_r17;
  };

  // member variables
  bool                 ext = false;
  rach_cfg_common_s    rach_cfg_common;
  bcch_cfg_s           bcch_cfg;
  pcch_cfg_s           pcch_cfg;
  prach_cfg_sib_s      prach_cfg;
  pdsch_cfg_common_s   pdsch_cfg_common;
  pusch_cfg_common_s   pusch_cfg_common;
  pucch_cfg_common_s   pucch_cfg_common;
  srs_ul_cfg_common_c  srs_ul_cfg_common;
  ul_pwr_ctrl_common_s ul_pwr_ctrl_common;
  ul_cp_len_e          ul_cp_len;
  // ...
  // group 0
  copy_ptr<ul_pwr_ctrl_common_v1020_s> ul_pwr_ctrl_common_v1020;
  // group 1
  copy_ptr<rach_cfg_common_v1250_s> rach_cfg_common_v1250;
  // group 2
  copy_ptr<pusch_cfg_common_v1270_s> pusch_cfg_common_v1270;
  // group 3
  copy_ptr<bcch_cfg_v1310_s>         bcch_cfg_v1310;
  copy_ptr<pcch_cfg_v1310_s>         pcch_cfg_v1310;
  copy_ptr<freq_hop_params_r13_s>    freq_hop_params_r13;
  copy_ptr<pdsch_cfg_common_v1310_s> pdsch_cfg_common_v1310;
  copy_ptr<pusch_cfg_common_v1310_s> pusch_cfg_common_v1310;
  copy_ptr<prach_cfg_sib_v1310_s>    prach_cfg_common_v1310;
  copy_ptr<pucch_cfg_common_v1310_s> pucch_cfg_common_v1310;
  // group 4
  copy_ptr<high_speed_cfg_r14_s>     high_speed_cfg_r14;
  copy_ptr<prach_cfg_v1430_s>        prach_cfg_v1430;
  copy_ptr<pucch_cfg_common_v1430_s> pucch_cfg_common_v1430;
  // group 5
  copy_ptr<prach_cfg_sib_v1530_s>  prach_cfg_v1530;
  copy_ptr<rss_cfg_r15_s>          ce_rss_cfg_r15;
  copy_ptr<wus_cfg_r15_s>          wus_cfg_r15;
  copy_ptr<high_speed_cfg_v1530_s> high_speed_cfg_v1530;
  // group 6
  copy_ptr<ul_pwr_ctrl_common_v1530_s> ul_pwr_ctrl_common_v1540;
  // group 7
  copy_ptr<wus_cfg_v1560_s> wus_cfg_v1560;
  // group 8
  bool                                         rss_meas_cfg_r16_present             = false;
  bool                                         rss_meas_non_ncl_r16_present         = false;
  bool                                         punctured_subcarriers_dl_r16_present = false;
  bool                                         high_speed_inter_rat_nr_r16_present  = false;
  copy_ptr<wus_cfg_v1610_s>                    wus_cfg_v1610;
  copy_ptr<high_speed_cfg_v1610_s>             high_speed_cfg_v1610;
  copy_ptr<crs_ch_est_mpdcch_cfg_common_r16_s> crs_ch_est_mpdcch_cfg_common_r16;
  copy_ptr<gwus_cfg_r16_s>                     gwus_cfg_r16;
  copy_ptr<ul_pwr_ctrl_common_v1610_s>         ul_pwr_ctrl_common_v1610;
  fixed_bitstring<2>                           punctured_subcarriers_dl_r16;
  bool                                         high_speed_inter_rat_nr_r16 = false;
  // group 9
  copy_ptr<pcch_cfg_v1700_s>      pcch_cfg_v1700;
  copy_ptr<ntn_cfg_common_r17_s_> ntn_cfg_common_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TimeAlignmentTimer ::= ENUMERATED
struct time_align_timer_opts {
  enum options { sf500, sf750, sf1280, sf1920, sf2560, sf5120, sf10240, infinity, nulltype } value;
  typedef int16_t number_type;

  const char* to_string() const;
  int16_t     to_number() const;
};
typedef enumerated<time_align_timer_opts> time_align_timer_e;

// AntennaInfoCommon ::= SEQUENCE
struct ant_info_common_s {
  struct ant_ports_count_opts {
    enum options { an1, an2, an4, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ant_ports_count_opts> ant_ports_count_e_;

  // member variables
  ant_ports_count_e_ ant_ports_count;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ant_info_common_s& other) const;
  bool        operator!=(const ant_info_common_s& other) const { return not(*this == other); }
};

// HighSpeedConfigSCell-r14 ::= SEQUENCE
struct high_speed_cfg_scell_r14_s {
  bool high_speed_enhanced_demod_flag_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const high_speed_cfg_scell_r14_s& other) const;
  bool        operator!=(const high_speed_cfg_scell_r14_s& other) const { return not(*this == other); }
};

// PRACH-Config ::= SEQUENCE
struct prach_cfg_s {
  bool             prach_cfg_info_present = false;
  uint16_t         root_seq_idx           = 0;
  prach_cfg_info_s prach_cfg_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const prach_cfg_s& other) const;
  bool        operator!=(const prach_cfg_s& other) const { return not(*this == other); }
};

// PRACH-ConfigSCell-r10 ::= SEQUENCE
struct prach_cfg_scell_r10_s {
  uint8_t prach_cfg_idx_r10 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const prach_cfg_scell_r10_s& other) const;
  bool        operator!=(const prach_cfg_scell_r10_s& other) const { return not(*this == other); }
};

// RACH-ConfigCommonSCell-r11 ::= SEQUENCE
struct rach_cfg_common_scell_r11_s {
  struct ra_supervision_info_r11_s_ {
    preamb_trans_max_e preamb_trans_max_r11;
  };

  // member variables
  bool                       ext = false;
  pwr_ramp_params_s          pwr_ramp_params_r11;
  ra_supervision_info_r11_s_ ra_supervision_info_r11;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const rach_cfg_common_scell_r11_s& other) const;
  bool        operator!=(const rach_cfg_common_scell_r11_s& other) const { return not(*this == other); }
};

// UplinkPowerControlCommonPUSCH-LessCell-v1430 ::= SEQUENCE
struct ul_pwr_ctrl_common_pusch_less_cell_v1430_s {
  bool        p0_nominal_periodic_srs_r14_present  = false;
  bool        p0_nominal_aperiodic_srs_r14_present = false;
  bool        alpha_srs_r14_present                = false;
  int8_t      p0_nominal_periodic_srs_r14          = -126;
  int8_t      p0_nominal_aperiodic_srs_r14         = -126;
  alpha_r12_e alpha_srs_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_pwr_ctrl_common_pusch_less_cell_v1430_s& other) const;
  bool        operator!=(const ul_pwr_ctrl_common_pusch_less_cell_v1430_s& other) const { return not(*this == other); }
};

// UplinkPowerControlCommonSCell-r10 ::= SEQUENCE
struct ul_pwr_ctrl_common_scell_r10_s {
  int8_t      p0_nominal_pusch_r10 = -126;
  alpha_r12_e alpha_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_pwr_ctrl_common_scell_r10_s& other) const;
  bool        operator!=(const ul_pwr_ctrl_common_scell_r10_s& other) const { return not(*this == other); }
};

// UplinkPowerControlCommonSCell-v1130 ::= SEQUENCE
struct ul_pwr_ctrl_common_scell_v1130_s {
  int8_t delta_preamb_msg3_r11 = -1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_pwr_ctrl_common_scell_v1130_s& other) const;
  bool        operator!=(const ul_pwr_ctrl_common_scell_v1130_s& other) const { return not(*this == other); }
};

// UplinkPowerControlCommonSCell-v1310 ::= SEQUENCE
struct ul_pwr_ctrl_common_scell_v1310_s {
  struct delta_f_pucch_format3_r12_opts {
    enum options {
      delta_f_minus1,
      delta_f0,
      delta_f1,
      delta_f2,
      delta_f3,
      delta_f4,
      delta_f5,
      delta_f6,
      nulltype
    } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<delta_f_pucch_format3_r12_opts> delta_f_pucch_format3_r12_e_;
  struct delta_f_pucch_format1b_cs_r12_opts {
    enum options { delta_f1, delta_f2, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<delta_f_pucch_format1b_cs_r12_opts> delta_f_pucch_format1b_cs_r12_e_;
  struct delta_f_pucch_format4_r13_opts {
    enum options {
      delta_f16,
      delta_f15,
      delta_f14,
      delta_f13,
      delta_f12,
      delta_f11,
      delta_f10,
      spare1,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<delta_f_pucch_format4_r13_opts> delta_f_pucch_format4_r13_e_;
  struct delta_f_pucch_format5_minus13_opts {
    enum options { delta_f13, delta_f12, delta_f11, delta_f10, delta_f9, delta_f8, delta_f7, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<delta_f_pucch_format5_minus13_opts> delta_f_pucch_format5_minus13_e_;

  // member variables
  bool                             delta_f_pucch_format3_r12_present     = false;
  bool                             delta_f_pucch_format1b_cs_r12_present = false;
  bool                             delta_f_pucch_format4_r13_present     = false;
  bool                             delta_f_pucch_format5_minus13_present = false;
  int8_t                           p0_nominal_pucch                      = -127;
  delta_flist_pucch_s              delta_flist_pucch;
  delta_f_pucch_format3_r12_e_     delta_f_pucch_format3_r12;
  delta_f_pucch_format1b_cs_r12_e_ delta_f_pucch_format1b_cs_r12;
  delta_f_pucch_format4_r13_e_     delta_f_pucch_format4_r13;
  delta_f_pucch_format5_minus13_e_ delta_f_pucch_format5_minus13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_pwr_ctrl_common_scell_v1310_s& other) const;
  bool        operator!=(const ul_pwr_ctrl_common_scell_v1310_s& other) const { return not(*this == other); }
};

// RadioResourceConfigCommonSCell-r10 ::= SEQUENCE
struct rr_cfg_common_scell_r10_s {
  struct non_ul_cfg_r10_s_ {
    struct dl_bw_r10_opts {
      enum options { n6, n15, n25, n50, n75, n100, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<dl_bw_r10_opts> dl_bw_r10_e_;

    // member variables
    bool                mbsfn_sf_cfg_list_r10_present = false;
    bool                tdd_cfg_r10_present           = false;
    dl_bw_r10_e_        dl_bw_r10;
    ant_info_common_s   ant_info_common_r10;
    mbsfn_sf_cfg_list_l mbsfn_sf_cfg_list_r10;
    phich_cfg_s         phich_cfg_r10;
    pdsch_cfg_common_s  pdsch_cfg_common_r10;
    tdd_cfg_s           tdd_cfg_r10;
  };
  struct ul_cfg_r10_s_ {
    struct ul_freq_info_r10_s_ {
      struct ul_bw_r10_opts {
        enum options { n6, n15, n25, n50, n75, n100, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<ul_bw_r10_opts> ul_bw_r10_e_;

      // member variables
      bool         ul_carrier_freq_r10_present = false;
      bool         ul_bw_r10_present           = false;
      uint32_t     ul_carrier_freq_r10         = 0;
      ul_bw_r10_e_ ul_bw_r10;
      uint8_t      add_spec_emission_scell_r10 = 1;
    };

    // member variables
    bool                           p_max_r10_present           = false;
    bool                           prach_cfg_scell_r10_present = false;
    ul_freq_info_r10_s_            ul_freq_info_r10;
    int8_t                         p_max_r10 = -30;
    ul_pwr_ctrl_common_scell_r10_s ul_pwr_ctrl_common_scell_r10;
    srs_ul_cfg_common_c            srs_ul_cfg_common_r10;
    ul_cp_len_e                    ul_cp_len_r10;
    prach_cfg_scell_r10_s          prach_cfg_scell_r10;
    pusch_cfg_common_s             pusch_cfg_common_r10;
  };
  struct ul_cfg_r14_s_ {
    struct ul_freq_info_r14_s_ {
      struct ul_bw_r14_opts {
        enum options { n6, n15, n25, n50, n75, n100, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<ul_bw_r14_opts> ul_bw_r14_e_;

      // member variables
      bool         ul_carrier_freq_r14_present = false;
      bool         ul_bw_r14_present           = false;
      uint32_t     ul_carrier_freq_r14         = 0;
      ul_bw_r14_e_ ul_bw_r14;
      uint8_t      add_spec_emission_scell_r14 = 1;
    };

    // member variables
    bool                                       p_max_r14_present                                = false;
    bool                                       prach_cfg_scell_r14_present                      = false;
    bool                                       ul_pwr_ctrl_common_pusch_less_cell_v1430_present = false;
    ul_freq_info_r14_s_                        ul_freq_info_r14;
    int8_t                                     p_max_r14 = -30;
    srs_ul_cfg_common_c                        srs_ul_cfg_common_r14;
    ul_cp_len_e                                ul_cp_len_r14;
    prach_cfg_scell_r10_s                      prach_cfg_scell_r14;
    ul_pwr_ctrl_common_pusch_less_cell_v1430_s ul_pwr_ctrl_common_pusch_less_cell_v1430;
  };
  struct harq_ref_cfg_r14_opts {
    enum options { sa2, sa4, sa5, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<harq_ref_cfg_r14_opts> harq_ref_cfg_r14_e_;

  // member variables
  bool              ext                = false;
  bool              ul_cfg_r10_present = false;
  non_ul_cfg_r10_s_ non_ul_cfg_r10;
  ul_cfg_r10_s_     ul_cfg_r10;
  // ...
  // group 0
  bool     ul_carrier_freq_v1090_present = false;
  uint32_t ul_carrier_freq_v1090         = 65536;
  // group 1
  copy_ptr<rach_cfg_common_scell_r11_s>      rach_cfg_common_scell_r11;
  copy_ptr<prach_cfg_s>                      prach_cfg_scell_r11;
  copy_ptr<tdd_cfg_v1130_s>                  tdd_cfg_v1130;
  copy_ptr<ul_pwr_ctrl_common_scell_v1130_s> ul_pwr_ctrl_common_scell_v1130;
  // group 2
  copy_ptr<pusch_cfg_common_v1270_s> pusch_cfg_common_v1270;
  // group 3
  copy_ptr<pucch_cfg_common_s>               pucch_cfg_common_r13;
  copy_ptr<ul_pwr_ctrl_common_scell_v1310_s> ul_pwr_ctrl_common_scell_v1310;
  // group 4
  bool                                 harq_ref_cfg_r14_present    = false;
  bool                                 srs_flex_timing_r14_present = false;
  copy_ptr<high_speed_cfg_scell_r14_s> high_speed_cfg_scell_r14;
  copy_ptr<prach_cfg_v1430_s>          prach_cfg_v1430;
  copy_ptr<ul_cfg_r14_s_>              ul_cfg_r14;
  harq_ref_cfg_r14_e_                  harq_ref_cfg_r14;
  // group 5
  copy_ptr<mbsfn_sf_cfg_list_v1430_l> mbsfn_sf_cfg_list_v1430;
  // group 6
  copy_ptr<ul_pwr_ctrl_common_v1530_s> ul_pwr_ctrl_common_scell_v1530;
  // group 7
  bool high_speed_enh_meas_flag_scell_r16_present = false;
  bool high_speed_enh_meas_flag_scell_r16         = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const rr_cfg_common_scell_r10_s& other) const;
  bool        operator!=(const rr_cfg_common_scell_r10_s& other) const { return not(*this == other); }
};

// UplinkPowerControlCommon-v1310 ::= SEQUENCE
struct ul_pwr_ctrl_common_v1310_s {
  struct delta_f_pucch_format4_r13_opts {
    enum options {
      delta_f16,
      delta_f15,
      delta_f14,
      delta_f13,
      delta_f12,
      delta_f11,
      delta_f10,
      spare1,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<delta_f_pucch_format4_r13_opts> delta_f_pucch_format4_r13_e_;
  struct delta_f_pucch_format5_minus13_opts {
    enum options { delta_f13, delta_f12, delta_f11, delta_f10, delta_f9, delta_f8, delta_f7, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<delta_f_pucch_format5_minus13_opts> delta_f_pucch_format5_minus13_e_;

  // member variables
  bool                             delta_f_pucch_format4_r13_present     = false;
  bool                             delta_f_pucch_format5_minus13_present = false;
  delta_f_pucch_format4_r13_e_     delta_f_pucch_format4_r13;
  delta_f_pucch_format5_minus13_e_ delta_f_pucch_format5_minus13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PRACH-Config-v1310 ::= SEQUENCE
struct prach_cfg_v1310_s {
  struct mpdcch_start_sf_css_ra_r13_c_ {
    struct fdd_r13_opts {
      enum options { v1, v1dot5, v2, v2dot5, v4, v5, v8, v10, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<fdd_r13_opts> fdd_r13_e_;
    struct tdd_r13_opts {
      enum options { v1, v2, v4, v5, v8, v10, v20, spare, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<tdd_r13_opts> tdd_r13_e_;
    struct types_opts {
      enum options { fdd_r13, tdd_r13, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    mpdcch_start_sf_css_ra_r13_c_() = default;
    mpdcch_start_sf_css_ra_r13_c_(const mpdcch_start_sf_css_ra_r13_c_& other);
    mpdcch_start_sf_css_ra_r13_c_& operator=(const mpdcch_start_sf_css_ra_r13_c_& other);
    ~mpdcch_start_sf_css_ra_r13_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fdd_r13_e_& fdd_r13()
    {
      assert_choice_type(types::fdd_r13, type_, "mpdcch-startSF-CSS-RA-r13");
      return c.get<fdd_r13_e_>();
    }
    tdd_r13_e_& tdd_r13()
    {
      assert_choice_type(types::tdd_r13, type_, "mpdcch-startSF-CSS-RA-r13");
      return c.get<tdd_r13_e_>();
    }
    const fdd_r13_e_& fdd_r13() const
    {
      assert_choice_type(types::fdd_r13, type_, "mpdcch-startSF-CSS-RA-r13");
      return c.get<fdd_r13_e_>();
    }
    const tdd_r13_e_& tdd_r13() const
    {
      assert_choice_type(types::tdd_r13, type_, "mpdcch-startSF-CSS-RA-r13");
      return c.get<tdd_r13_e_>();
    }
    fdd_r13_e_& set_fdd_r13();
    tdd_r13_e_& set_tdd_r13();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                              rsrp_thress_prach_info_list_r13_present = false;
  bool                              mpdcch_start_sf_css_ra_r13_present      = false;
  bool                              prach_hop_offset_r13_present            = false;
  bool                              prach_params_list_ce_r13_present        = false;
  bool                              init_ce_level_r13_present               = false;
  rsrp_thress_prach_info_list_r13_l rsrp_thress_prach_info_list_r13;
  mpdcch_start_sf_css_ra_r13_c_     mpdcch_start_sf_css_ra_r13;
  uint8_t                           prach_hop_offset_r13 = 0;
  prach_params_list_ce_r13_l        prach_params_list_ce_r13;
  uint8_t                           init_ce_level_r13 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioResourceConfigCommon ::= SEQUENCE
struct rr_cfg_common_s {
  struct ntn_cfg_common_r17_s_ {
    struct t318_r17_opts {
      enum options { ms0, ms50, ms100, ms200, ms500, ms1000, ms2000, ms4000, ms6000, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<t318_r17_opts> t318_r17_e_;

    // member variables
    bool               ta_report_r17_present    = false;
    bool               prach_tx_dur_r17_present = false;
    bool               pucch_tx_dur_r17_present = false;
    bool               pusch_tx_dur_r17_present = false;
    t318_r17_e_        t318_r17;
    prach_tx_dur_r17_s prach_tx_dur_r17;
    pucch_tx_dur_r17_s pucch_tx_dur_r17;
    pusch_tx_dur_r17_s pusch_tx_dur_r17;
  };

  // member variables
  bool                 ext                        = false;
  bool                 rach_cfg_common_present    = false;
  bool                 pdsch_cfg_common_present   = false;
  bool                 phich_cfg_present          = false;
  bool                 pucch_cfg_common_present   = false;
  bool                 srs_ul_cfg_common_present  = false;
  bool                 ul_pwr_ctrl_common_present = false;
  bool                 ant_info_common_present    = false;
  bool                 p_max_present              = false;
  bool                 tdd_cfg_present            = false;
  rach_cfg_common_s    rach_cfg_common;
  prach_cfg_s          prach_cfg;
  pdsch_cfg_common_s   pdsch_cfg_common;
  pusch_cfg_common_s   pusch_cfg_common;
  phich_cfg_s          phich_cfg;
  pucch_cfg_common_s   pucch_cfg_common;
  srs_ul_cfg_common_c  srs_ul_cfg_common;
  ul_pwr_ctrl_common_s ul_pwr_ctrl_common;
  ant_info_common_s    ant_info_common;
  int8_t               p_max = -30;
  tdd_cfg_s            tdd_cfg;
  ul_cp_len_e          ul_cp_len;
  // ...
  // group 0
  copy_ptr<ul_pwr_ctrl_common_v1020_s> ul_pwr_ctrl_common_v1020;
  // group 1
  copy_ptr<tdd_cfg_v1130_s> tdd_cfg_v1130;
  // group 2
  copy_ptr<pusch_cfg_common_v1270_s> pusch_cfg_common_v1270;
  // group 3
  copy_ptr<prach_cfg_v1310_s>          prach_cfg_v1310;
  copy_ptr<freq_hop_params_r13_s>      freq_hop_params_r13;
  copy_ptr<pdsch_cfg_common_v1310_s>   pdsch_cfg_common_v1310;
  copy_ptr<pucch_cfg_common_v1310_s>   pucch_cfg_common_v1310;
  copy_ptr<pusch_cfg_common_v1310_s>   pusch_cfg_common_v1310;
  copy_ptr<ul_pwr_ctrl_common_v1310_s> ul_pwr_ctrl_common_v1310;
  // group 4
  copy_ptr<high_speed_cfg_r14_s>     high_speed_cfg_r14;
  copy_ptr<prach_cfg_v1430_s>        prach_cfg_v1430;
  copy_ptr<pucch_cfg_common_v1430_s> pucch_cfg_common_v1430;
  copy_ptr<tdd_cfg_v1430_s>          tdd_cfg_v1430;
  // group 5
  copy_ptr<tdd_cfg_v1450_s> tdd_cfg_v1450;
  // group 6
  copy_ptr<ul_pwr_ctrl_common_v1530_s> ul_pwr_ctrl_common_v1530;
  copy_ptr<high_speed_cfg_v1530_s>     high_speed_cfg_v1530;
  // group 7
  bool                                 high_speed_inter_rat_nr_r16_present = false;
  copy_ptr<high_speed_cfg_v1610_s>     high_speed_cfg_v1610;
  copy_ptr<ul_pwr_ctrl_common_v1610_s> ul_pwr_ctrl_common_v1610;
  bool                                 high_speed_inter_rat_nr_r16 = false;
  // group 8
  copy_ptr<ntn_cfg_common_r17_s_> ntn_cfg_common_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc
} // namespace asn1

#endif // SRSASN1_RRC_RR_COMMON_H
