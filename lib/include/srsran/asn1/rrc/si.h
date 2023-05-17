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

#ifndef SRSASN1_RRC_SI_H
#define SRSASN1_RRC_SI_H

#include "rr_common.h"

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// MasterInformationBlock ::= SEQUENCE
struct mib_s {
  struct dl_bw_opts {
    enum options { n6, n15, n25, n50, n75, n100, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<dl_bw_opts> dl_bw_e_;
  struct part_earfcn_minus17_c_ {
    struct types_opts {
      enum options { spare, earfcn_lsb, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    part_earfcn_minus17_c_() = default;
    part_earfcn_minus17_c_(const part_earfcn_minus17_c_& other);
    part_earfcn_minus17_c_& operator=(const part_earfcn_minus17_c_& other);
    ~part_earfcn_minus17_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<2>& spare()
    {
      assert_choice_type(types::spare, type_, "partEARFCN-17");
      return c.get<fixed_bitstring<2> >();
    }
    fixed_bitstring<2>& earfcn_lsb()
    {
      assert_choice_type(types::earfcn_lsb, type_, "partEARFCN-17");
      return c.get<fixed_bitstring<2> >();
    }
    const fixed_bitstring<2>& spare() const
    {
      assert_choice_type(types::spare, type_, "partEARFCN-17");
      return c.get<fixed_bitstring<2> >();
    }
    const fixed_bitstring<2>& earfcn_lsb() const
    {
      assert_choice_type(types::earfcn_lsb, type_, "partEARFCN-17");
      return c.get<fixed_bitstring<2> >();
    }
    fixed_bitstring<2>& set_spare();
    fixed_bitstring<2>& set_earfcn_lsb();

  private:
    types                                type_;
    choice_buffer_t<fixed_bitstring<2> > c;

    void destroy_();
  };

  // member variables
  dl_bw_e_               dl_bw;
  phich_cfg_s            phich_cfg;
  fixed_bitstring<8>     sys_frame_num;
  uint8_t                sched_info_sib1_br_r13    = 0;
  bool                   sys_info_unchanged_br_r15 = false;
  part_earfcn_minus17_c_ part_earfcn_minus17;
  fixed_bitstring<1>     spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TrackingAreaList-r17 ::= SEQUENCE (SIZE (1..12)) OF BIT STRING (SIZE (16))
using tracking_area_list_r17_l = bounded_array<fixed_bitstring<16>, 12>;

// GNSS-ID-r15 ::= SEQUENCE
struct gnss_id_r15_s {
  struct gnss_id_r15_opts {
    enum options { gps, sbas, qzss, galileo, glonass, bds, /*...*/ navic_v1610, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<gnss_id_r15_opts, true, 1> gnss_id_r15_e_;

  // member variables
  bool           ext = false;
  gnss_id_r15_e_ gnss_id_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityInfo-v1700 ::= SEQUENCE
struct plmn_id_info_v1700_s {
  bool                     tracking_area_list_r17_present = false;
  tracking_area_list_r17_l tracking_area_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SBAS-ID-r15 ::= SEQUENCE
struct sbas_id_r15_s {
  struct sbas_id_r15_opts {
    enum options { waas, egnos, msas, gagan, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<sbas_id_r15_opts, true> sbas_id_r15_e_;

  // member variables
  bool           ext = false;
  sbas_id_r15_e_ sbas_id_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityInfo-v1610 ::= SEQUENCE
struct plmn_id_info_v1610_s {
  bool cp_cio_t_minus5_gs_optim_r16_present = false;
  bool up_cio_t_minus5_gs_optim_r16_present = false;
  bool iab_support_r16_present              = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityList-v1700 ::= SEQUENCE (SIZE (1..6)) OF PLMN-IdentityInfo-v1700
using plmn_id_list_v1700_l = dyn_array<plmn_id_info_v1700_s>;

// PosSIB-Type-r15 ::= SEQUENCE
struct pos_sib_type_r15_s {
  struct pos_sib_type_r15_opts {
    enum options {
      pos_sib_type1_minus1,
      pos_sib_type1_minus2,
      pos_sib_type1_minus3,
      pos_sib_type1_minus4,
      pos_sib_type1_minus5,
      pos_sib_type1_minus6,
      pos_sib_type1_minus7,
      pos_sib_type2_minus1,
      pos_sib_type2_minus2,
      pos_sib_type2_minus3,
      pos_sib_type2_minus4,
      pos_sib_type2_minus5,
      pos_sib_type2_minus6,
      pos_sib_type2_minus7,
      pos_sib_type2_minus8,
      pos_sib_type2_minus9,
      pos_sib_type2_minus10,
      pos_sib_type2_minus11,
      pos_sib_type2_minus12,
      pos_sib_type2_minus13,
      pos_sib_type2_minus14,
      pos_sib_type2_minus15,
      pos_sib_type2_minus16,
      pos_sib_type2_minus17,
      pos_sib_type2_minus18,
      pos_sib_type2_minus19,
      pos_sib_type3_minus1,
      // ...
      pos_sib_type1_minus8_v1610,
      pos_sib_type2_minus20_v1610,
      pos_sib_type2_minus21_v1610,
      pos_sib_type2_minus22_v1610,
      pos_sib_type2_minus23_v1610,
      pos_sib_type2_minus24_v1610,
      pos_sib_type2_minus25_v1610,
      pos_sib_type4_minus1_v1610,
      pos_sib_type5_minus1_v1610,
      pos_sib_type1_minus9_v1700,
      pos_sib_type1_minus10_v1700,
      nulltype
    } value;

    const char* to_string() const;
  };
  typedef enumerated<pos_sib_type_r15_opts, true, 11> pos_sib_type_r15_e_;

  // member variables
  bool                ext                   = false;
  bool                encrypted_r15_present = false;
  bool                gnss_id_r15_present   = false;
  bool                sbas_id_r15_present   = false;
  gnss_id_r15_s       gnss_id_r15;
  sbas_id_r15_s       sbas_id_r15;
  pos_sib_type_r15_e_ pos_sib_type_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityList-v1610 ::= SEQUENCE (SIZE (1..6)) OF PLMN-IdentityInfo-v1610
using plmn_id_list_v1610_l = dyn_array<plmn_id_info_v1610_s>;

// PosSIB-MappingInfo-r15 ::= SEQUENCE (SIZE (1..32)) OF PosSIB-Type-r15
using pos_sib_map_info_r15_l = dyn_array<pos_sib_type_r15_s>;

// SystemInformationBlockType1-v1700-IEs ::= SEQUENCE
struct sib_type1_v1700_ies_s {
  struct cell_access_related_info_ntn_r17_s_ {
    struct cell_barred_ntn_r17_opts {
      enum options { barred, not_barred, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<cell_barred_ntn_r17_opts> cell_barred_ntn_r17_e_;

    // member variables
    bool                   plmn_id_list_v1700_present = false;
    cell_barred_ntn_r17_e_ cell_barred_ntn_r17;
    plmn_id_list_v1700_l   plmn_id_list_v1700;
  };

  // member variables
  bool                                cell_access_related_info_ntn_r17_present = false;
  bool                                non_crit_ext_present                     = false;
  cell_access_related_info_ntn_r17_s_ cell_access_related_info_ntn_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityInfo-v1530 ::= SEQUENCE
struct plmn_id_info_v1530_s {
  struct cell_reserved_for_oper_crs_r15_opts {
    enum options { reserved, not_reserved, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<cell_reserved_for_oper_crs_r15_opts> cell_reserved_for_oper_crs_r15_e_;

  // member variables
  cell_reserved_for_oper_crs_r15_e_ cell_reserved_for_oper_crs_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PosSchedulingInfo-r15 ::= SEQUENCE
struct pos_sched_info_r15_s {
  struct pos_si_periodicity_r15_opts {
    enum options { rf8, rf16, rf32, rf64, rf128, rf256, rf512, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<pos_si_periodicity_r15_opts> pos_si_periodicity_r15_e_;

  // member variables
  pos_si_periodicity_r15_e_ pos_si_periodicity_r15;
  pos_sib_map_info_r15_l    pos_sib_map_info_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1-v1610-IEs ::= SEQUENCE
struct sib_type1_v1610_ies_s {
  bool                  edrx_allowed_minus5_gc_r16_present = false;
  bool                  tx_in_ctrl_ch_region_r16_present   = false;
  bool                  camping_allowed_in_ce_r16_present  = false;
  bool                  plmn_id_list_v1610_present         = false;
  bool                  non_crit_ext_present               = false;
  plmn_id_list_v1610_l  plmn_id_list_v1610;
  sib_type1_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellSelectionInfoCE-v1530 ::= SEQUENCE
struct cell_sel_info_ce_v1530_s {
  struct pwr_class14dbm_offset_r15_opts {
    enum options { db_minus6, db_minus3, db3, db6, db9, db12, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<pwr_class14dbm_offset_r15_opts> pwr_class14dbm_offset_r15_e_;

  // member variables
  pwr_class14dbm_offset_r15_e_ pwr_class14dbm_offset_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityList-v1530 ::= SEQUENCE (SIZE (1..6)) OF PLMN-IdentityInfo-v1530
using plmn_id_list_v1530_l = dyn_array<plmn_id_info_v1530_s>;

// PosSchedulingInfoList-r15 ::= SEQUENCE (SIZE (1..32)) OF PosSchedulingInfo-r15
using pos_sched_info_list_r15_l = dyn_array<pos_sched_info_r15_s>;

// SystemInformationBlockType1-v1540-IEs ::= SEQUENCE
struct sib_type1_v1540_ies_s {
  bool                  si_pos_offset_r15_present = false;
  bool                  non_crit_ext_present      = false;
  sib_type1_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1-v1530-IEs ::= SEQUENCE
struct sib_type1_v1530_ies_s {
  struct crs_intf_mitig_cfg_r15_c_ {
    struct crs_intf_mitig_num_prbs_opts {
      enum options { n6, n24, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<crs_intf_mitig_num_prbs_opts> crs_intf_mitig_num_prbs_e_;
    struct types_opts {
      enum options { crs_intf_mitig_enabled, crs_intf_mitig_num_prbs, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crs_intf_mitig_cfg_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    crs_intf_mitig_num_prbs_e_& crs_intf_mitig_num_prbs()
    {
      assert_choice_type(types::crs_intf_mitig_num_prbs, type_, "crs-IntfMitigConfig-r15");
      return c;
    }
    const crs_intf_mitig_num_prbs_e_& crs_intf_mitig_num_prbs() const
    {
      assert_choice_type(types::crs_intf_mitig_num_prbs, type_, "crs-IntfMitigConfig-r15");
      return c;
    }
    void                        set_crs_intf_mitig_enabled();
    crs_intf_mitig_num_prbs_e_& set_crs_intf_mitig_num_prbs();

  private:
    types                      type_;
    crs_intf_mitig_num_prbs_e_ c;
  };
  struct cell_barred_crs_r15_opts {
    enum options { barred, not_barred, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<cell_barred_crs_r15_opts> cell_barred_crs_r15_e_;
  struct cell_access_related_info_minus5_gc_r15_s_ {
    struct cell_barred_minus5_gc_r15_opts {
      enum options { barred, not_barred, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<cell_barred_minus5_gc_r15_opts> cell_barred_minus5_gc_r15_e_;
    struct cell_barred_minus5_gc_crs_r15_opts {
      enum options { barred, not_barred, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<cell_barred_minus5_gc_crs_r15_opts> cell_barred_minus5_gc_crs_r15_e_;
    using cell_access_related_info_list_minus5_gc_r15_l_ = dyn_array<cell_access_related_info_minus5_gc_r15_s>;

    // member variables
    cell_barred_minus5_gc_r15_e_                   cell_barred_minus5_gc_r15;
    cell_barred_minus5_gc_crs_r15_e_               cell_barred_minus5_gc_crs_r15;
    cell_access_related_info_list_minus5_gc_r15_l_ cell_access_related_info_list_minus5_gc_r15;
  };

  // member variables
  bool                                      hsdn_cell_r15_present                          = false;
  bool                                      cell_sel_info_ce_v1530_present                 = false;
  bool                                      crs_intf_mitig_cfg_r15_present                 = false;
  bool                                      plmn_id_list_v1530_present                     = false;
  bool                                      pos_sched_info_list_r15_present                = false;
  bool                                      cell_access_related_info_minus5_gc_r15_present = false;
  bool                                      ims_emergency_support5_gc_r15_present          = false;
  bool                                      ecall_over_ims_support5_gc_r15_present         = false;
  bool                                      non_crit_ext_present                           = false;
  cell_sel_info_ce_v1530_s                  cell_sel_info_ce_v1530;
  crs_intf_mitig_cfg_r15_c_                 crs_intf_mitig_cfg_r15;
  cell_barred_crs_r15_e_                    cell_barred_crs_r15;
  plmn_id_list_v1530_l                      plmn_id_list_v1530;
  pos_sched_info_list_r15_l                 pos_sched_info_list_r15;
  cell_access_related_info_minus5_gc_r15_s_ cell_access_related_info_minus5_gc_r15;
  sib_type1_v1540_ies_s                     non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellAccessRelatedInfo-r14 ::= SEQUENCE
struct cell_access_related_info_r14_s {
  plmn_id_list_l      plmn_id_list_r14;
  fixed_bitstring<16> tac_r14;
  fixed_bitstring<28> cell_id_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1-v1450-IEs ::= SEQUENCE
struct sib_type1_v1450_ies_s {
  bool                  tdd_cfg_v1450_present = false;
  bool                  non_crit_ext_present  = false;
  tdd_cfg_v1450_s       tdd_cfg_v1450;
  sib_type1_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellSelectionInfoCE1-v1360 ::= SEQUENCE
struct cell_sel_info_ce1_v1360_s {
  int8_t delta_rx_lev_min_ce1_v1360 = -8;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1-v1430-IEs ::= SEQUENCE
struct sib_type1_v1430_ies_s {
  using cell_access_related_info_list_r14_l_ = dyn_array<cell_access_related_info_r14_s>;

  // member variables
  bool                                 ecall_over_ims_support_r14_present        = false;
  bool                                 tdd_cfg_v1430_present                     = false;
  bool                                 cell_access_related_info_list_r14_present = false;
  bool                                 non_crit_ext_present                      = false;
  tdd_cfg_v1430_s                      tdd_cfg_v1430;
  cell_access_related_info_list_r14_l_ cell_access_related_info_list_r14;
  sib_type1_v1450_ies_s                non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellSelectionInfoCE1-r13 ::= SEQUENCE
struct cell_sel_info_ce1_r13_s {
  bool   q_qual_min_rsrq_ce1_r13_present = false;
  int8_t q_rx_lev_min_ce1_r13            = -70;
  int8_t q_qual_min_rsrq_ce1_r13         = -34;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1-v1360-IEs ::= SEQUENCE
struct sib_type1_v1360_ies_s {
  bool                      cell_sel_info_ce1_v1360_present = false;
  bool                      non_crit_ext_present            = false;
  cell_sel_info_ce1_v1360_s cell_sel_info_ce1_v1360;
  sib_type1_v1430_ies_s     non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingInfo-BR-r13 ::= SEQUENCE
struct sched_info_br_r13_s {
  struct si_tbs_r13_opts {
    enum options { b152, b208, b256, b328, b408, b504, b600, b712, b808, b936, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<si_tbs_r13_opts> si_tbs_r13_e_;

  // member variables
  uint8_t       si_nb_r13 = 1;
  si_tbs_r13_e_ si_tbs_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1-v1350-IEs ::= SEQUENCE
struct sib_type1_v1350_ies_s {
  bool                    cell_sel_info_ce1_r13_present = false;
  bool                    non_crit_ext_present          = false;
  cell_sel_info_ce1_r13_s cell_sel_info_ce1_r13;
  sib_type1_v1360_ies_s   non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BarringPerACDC-Category-r13 ::= SEQUENCE
struct barr_per_acdc_category_r13_s {
  struct acdc_barr_cfg_r13_s_ {
    struct ac_barr_factor_r13_opts {
      enum options { p00, p05, p10, p15, p20, p25, p30, p40, p50, p60, p70, p75, p80, p85, p90, p95, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<ac_barr_factor_r13_opts> ac_barr_factor_r13_e_;
    struct ac_barr_time_r13_opts {
      enum options { s4, s8, s16, s32, s64, s128, s256, s512, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<ac_barr_time_r13_opts> ac_barr_time_r13_e_;

    // member variables
    ac_barr_factor_r13_e_ ac_barr_factor_r13;
    ac_barr_time_r13_e_   ac_barr_time_r13;
  };

  // member variables
  bool                 acdc_barr_cfg_r13_present = false;
  uint8_t              acdc_category_r13         = 1;
  acdc_barr_cfg_r13_s_ acdc_barr_cfg_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellSelectionInfoCE-r13 ::= SEQUENCE
struct cell_sel_info_ce_r13_s {
  bool   q_qual_min_rsrq_ce_r13_present = false;
  int8_t q_rx_lev_min_ce_r13            = -70;
  int8_t q_qual_min_rsrq_ce_r13         = -34;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingInfoList-BR-r13 ::= SEQUENCE (SIZE (1..32)) OF SchedulingInfo-BR-r13
using sched_info_list_br_r13_l = dyn_array<sched_info_br_r13_s>;

// SystemInfoValueTagList-r13 ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..3)
using sys_info_value_tag_list_r13_l = bounded_array<uint8_t, 32>;

// SystemInformationBlockType1-v1320-IEs ::= SEQUENCE
struct sib_type1_v1320_ies_s {
  struct freq_hop_params_dl_r13_s_ {
    struct mpdcch_pdsch_hop_nb_r13_opts {
      enum options { nb2, nb4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<mpdcch_pdsch_hop_nb_r13_opts> mpdcch_pdsch_hop_nb_r13_e_;
    struct interv_dl_hop_cfg_common_mode_a_r13_c_ {
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
      interv_dl_hop_cfg_common_mode_a_r13_c_() = default;
      interv_dl_hop_cfg_common_mode_a_r13_c_(const interv_dl_hop_cfg_common_mode_a_r13_c_& other);
      interv_dl_hop_cfg_common_mode_a_r13_c_& operator=(const interv_dl_hop_cfg_common_mode_a_r13_c_& other);
      ~interv_dl_hop_cfg_common_mode_a_r13_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      interv_fdd_r13_e_& interv_fdd_r13()
      {
        assert_choice_type(types::interv_fdd_r13, type_, "interval-DLHoppingConfigCommonModeA-r13");
        return c.get<interv_fdd_r13_e_>();
      }
      interv_tdd_r13_e_& interv_tdd_r13()
      {
        assert_choice_type(types::interv_tdd_r13, type_, "interval-DLHoppingConfigCommonModeA-r13");
        return c.get<interv_tdd_r13_e_>();
      }
      const interv_fdd_r13_e_& interv_fdd_r13() const
      {
        assert_choice_type(types::interv_fdd_r13, type_, "interval-DLHoppingConfigCommonModeA-r13");
        return c.get<interv_fdd_r13_e_>();
      }
      const interv_tdd_r13_e_& interv_tdd_r13() const
      {
        assert_choice_type(types::interv_tdd_r13, type_, "interval-DLHoppingConfigCommonModeA-r13");
        return c.get<interv_tdd_r13_e_>();
      }
      interv_fdd_r13_e_& set_interv_fdd_r13();
      interv_tdd_r13_e_& set_interv_tdd_r13();

    private:
      types               type_;
      pod_choice_buffer_t c;

      void destroy_();
    };
    struct interv_dl_hop_cfg_common_mode_b_r13_c_ {
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
      interv_dl_hop_cfg_common_mode_b_r13_c_() = default;
      interv_dl_hop_cfg_common_mode_b_r13_c_(const interv_dl_hop_cfg_common_mode_b_r13_c_& other);
      interv_dl_hop_cfg_common_mode_b_r13_c_& operator=(const interv_dl_hop_cfg_common_mode_b_r13_c_& other);
      ~interv_dl_hop_cfg_common_mode_b_r13_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      interv_fdd_r13_e_& interv_fdd_r13()
      {
        assert_choice_type(types::interv_fdd_r13, type_, "interval-DLHoppingConfigCommonModeB-r13");
        return c.get<interv_fdd_r13_e_>();
      }
      interv_tdd_r13_e_& interv_tdd_r13()
      {
        assert_choice_type(types::interv_tdd_r13, type_, "interval-DLHoppingConfigCommonModeB-r13");
        return c.get<interv_tdd_r13_e_>();
      }
      const interv_fdd_r13_e_& interv_fdd_r13() const
      {
        assert_choice_type(types::interv_fdd_r13, type_, "interval-DLHoppingConfigCommonModeB-r13");
        return c.get<interv_fdd_r13_e_>();
      }
      const interv_tdd_r13_e_& interv_tdd_r13() const
      {
        assert_choice_type(types::interv_tdd_r13, type_, "interval-DLHoppingConfigCommonModeB-r13");
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
    bool                                   mpdcch_pdsch_hop_nb_r13_present             = false;
    bool                                   interv_dl_hop_cfg_common_mode_a_r13_present = false;
    bool                                   interv_dl_hop_cfg_common_mode_b_r13_present = false;
    bool                                   mpdcch_pdsch_hop_offset_r13_present         = false;
    mpdcch_pdsch_hop_nb_r13_e_             mpdcch_pdsch_hop_nb_r13;
    interv_dl_hop_cfg_common_mode_a_r13_c_ interv_dl_hop_cfg_common_mode_a_r13;
    interv_dl_hop_cfg_common_mode_b_r13_c_ interv_dl_hop_cfg_common_mode_b_r13;
    uint8_t                                mpdcch_pdsch_hop_offset_r13 = 1;
  };

  // member variables
  bool                      freq_hop_params_dl_r13_present = false;
  bool                      non_crit_ext_present           = false;
  freq_hop_params_dl_r13_s_ freq_hop_params_dl_r13;
  sib_type1_v1350_ies_s     non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AC-BarringConfig ::= SEQUENCE
struct ac_barr_cfg_s {
  struct ac_barr_factor_opts {
    enum options { p00, p05, p10, p15, p20, p25, p30, p40, p50, p60, p70, p75, p80, p85, p90, p95, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<ac_barr_factor_opts> ac_barr_factor_e_;
  struct ac_barr_time_opts {
    enum options { s4, s8, s16, s32, s64, s128, s256, s512, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<ac_barr_time_opts> ac_barr_time_e_;

  // member variables
  ac_barr_factor_e_  ac_barr_factor;
  ac_barr_time_e_    ac_barr_time;
  fixed_bitstring<5> ac_barr_for_special_ac;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BarringPerACDC-CategoryList-r13 ::= SEQUENCE (SIZE (1..16)) OF BarringPerACDC-Category-r13
using barr_per_acdc_category_list_r13_l = dyn_array<barr_per_acdc_category_r13_s>;

// CellSelectionInfo-v1250 ::= SEQUENCE
struct cell_sel_info_v1250_s {
  int8_t q_qual_min_rsrq_on_all_symbols_r12 = -34;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1-v1310-IEs ::= SEQUENCE
struct sib_type1_v1310_ies_s {
  struct bw_reduced_access_related_info_r13_s_ {
    struct si_win_len_br_r13_opts {
      enum options { ms20, ms40, ms60, ms80, ms120, ms160, ms200, spare, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<si_win_len_br_r13_opts> si_win_len_br_r13_e_;
    struct si_repeat_pattern_r13_opts {
      enum options { every_rf, every2nd_rf, every4th_rf, every8th_rf, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<si_repeat_pattern_r13_opts> si_repeat_pattern_r13_e_;
    struct fdd_dl_or_tdd_sf_bitmap_br_r13_c_ {
      struct types_opts {
        enum options { sf_pattern10_r13, sf_pattern40_r13, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      fdd_dl_or_tdd_sf_bitmap_br_r13_c_() = default;
      fdd_dl_or_tdd_sf_bitmap_br_r13_c_(const fdd_dl_or_tdd_sf_bitmap_br_r13_c_& other);
      fdd_dl_or_tdd_sf_bitmap_br_r13_c_& operator=(const fdd_dl_or_tdd_sf_bitmap_br_r13_c_& other);
      ~fdd_dl_or_tdd_sf_bitmap_br_r13_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      fixed_bitstring<10>& sf_pattern10_r13()
      {
        assert_choice_type(types::sf_pattern10_r13, type_, "fdd-DownlinkOrTddSubframeBitmapBR-r13");
        return c.get<fixed_bitstring<10> >();
      }
      fixed_bitstring<40>& sf_pattern40_r13()
      {
        assert_choice_type(types::sf_pattern40_r13, type_, "fdd-DownlinkOrTddSubframeBitmapBR-r13");
        return c.get<fixed_bitstring<40> >();
      }
      const fixed_bitstring<10>& sf_pattern10_r13() const
      {
        assert_choice_type(types::sf_pattern10_r13, type_, "fdd-DownlinkOrTddSubframeBitmapBR-r13");
        return c.get<fixed_bitstring<10> >();
      }
      const fixed_bitstring<40>& sf_pattern40_r13() const
      {
        assert_choice_type(types::sf_pattern40_r13, type_, "fdd-DownlinkOrTddSubframeBitmapBR-r13");
        return c.get<fixed_bitstring<40> >();
      }
      fixed_bitstring<10>& set_sf_pattern10_r13();
      fixed_bitstring<40>& set_sf_pattern40_r13();

    private:
      types                                 type_;
      choice_buffer_t<fixed_bitstring<40> > c;

      void destroy_();
    };
    struct si_hop_cfg_common_r13_opts {
      enum options { on, off, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<si_hop_cfg_common_r13_opts> si_hop_cfg_common_r13_e_;

    // member variables
    bool                              sched_info_list_br_r13_present         = false;
    bool                              fdd_dl_or_tdd_sf_bitmap_br_r13_present = false;
    bool                              fdd_ul_sf_bitmap_br_r13_present        = false;
    bool                              si_validity_time_r13_present           = false;
    bool                              sys_info_value_tag_list_r13_present    = false;
    si_win_len_br_r13_e_              si_win_len_br_r13;
    si_repeat_pattern_r13_e_          si_repeat_pattern_r13;
    sched_info_list_br_r13_l          sched_info_list_br_r13;
    fdd_dl_or_tdd_sf_bitmap_br_r13_c_ fdd_dl_or_tdd_sf_bitmap_br_r13;
    fixed_bitstring<10>               fdd_ul_sf_bitmap_br_r13;
    uint8_t                           start_symbol_br_r13 = 1;
    si_hop_cfg_common_r13_e_          si_hop_cfg_common_r13;
    sys_info_value_tag_list_r13_l     sys_info_value_tag_list_r13;
  };

  // member variables
  bool                                  hyper_sfn_r13_present                      = false;
  bool                                  edrx_allowed_r13_present                   = false;
  bool                                  cell_sel_info_ce_r13_present               = false;
  bool                                  bw_reduced_access_related_info_r13_present = false;
  bool                                  non_crit_ext_present                       = false;
  fixed_bitstring<10>                   hyper_sfn_r13;
  cell_sel_info_ce_r13_s                cell_sel_info_ce_r13;
  bw_reduced_access_related_info_r13_s_ bw_reduced_access_related_info_r13;
  sib_type1_v1320_ies_s                 non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UDT-Restricting-r13 ::= SEQUENCE
struct udt_restricting_r13_s {
  struct udt_restricting_time_r13_opts {
    enum options { s4, s8, s16, s32, s64, s128, s256, s512, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<udt_restricting_time_r13_opts> udt_restricting_time_r13_e_;

  // member variables
  bool                        udt_restricting_r13_present      = false;
  bool                        udt_restricting_time_r13_present = false;
  udt_restricting_time_r13_e_ udt_restricting_time_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AC-BarringPerPLMN-r12 ::= SEQUENCE
struct ac_barr_per_plmn_r12_s {
  struct ac_barr_info_r12_s_ {
    bool          ac_barr_for_mo_sig_r12_present  = false;
    bool          ac_barr_for_mo_data_r12_present = false;
    bool          ac_barr_for_emergency_r12       = false;
    ac_barr_cfg_s ac_barr_for_mo_sig_r12;
    ac_barr_cfg_s ac_barr_for_mo_data_r12;
  };

  // member variables
  bool                ac_barr_info_r12_present                 = false;
  bool                ac_barr_skip_for_mmtel_voice_r12_present = false;
  bool                ac_barr_skip_for_mmtel_video_r12_present = false;
  bool                ac_barr_skip_for_sms_r12_present         = false;
  bool                ac_barr_for_csfb_r12_present             = false;
  bool                ssac_barr_for_mmtel_voice_r12_present    = false;
  bool                ssac_barr_for_mmtel_video_r12_present    = false;
  uint8_t             plmn_id_idx_r12                          = 1;
  ac_barr_info_r12_s_ ac_barr_info_r12;
  ac_barr_cfg_s       ac_barr_for_csfb_r12;
  ac_barr_cfg_s       ssac_barr_for_mmtel_voice_r12;
  ac_barr_cfg_s       ssac_barr_for_mmtel_video_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ACDC-BarringPerPLMN-r13 ::= SEQUENCE
struct acdc_barr_per_plmn_r13_s {
  uint8_t                           plmn_id_idx_r13         = 1;
  bool                              acdc_only_for_hplmn_r13 = false;
  barr_per_acdc_category_list_r13_l barr_per_acdc_category_list_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CIOT-OptimisationPLMN-r13 ::= SEQUENCE
struct ciot_optim_plmn_r13_s {
  bool up_cio_t_eps_optim_r13_present         = false;
  bool cp_cio_t_eps_optim_r13_present         = false;
  bool attach_without_pdn_connect_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellSelectionInfo-v1130 ::= SEQUENCE
struct cell_sel_info_v1130_s {
  int8_t q_qual_min_wb_r11 = -34;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-Info-r15 ::= SEQUENCE
struct plmn_info_r15_s {
  bool upper_layer_ind_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB-Type ::= ENUMERATED
struct sib_type_opts {
  enum options {
    sib_type3,
    sib_type4,
    sib_type5,
    sib_type6,
    sib_type7,
    sib_type8,
    sib_type9,
    sib_type10,
    sib_type11,
    sib_type12_v920,
    sib_type13_v920,
    sib_type14_v1130,
    sib_type15_v1130,
    sib_type16_v1130,
    sib_type17_v1250,
    sib_type18_v1250,
    // ...
    sib_type19_v1250,
    sib_type20_v1310,
    sib_type21_v1430,
    sib_type24_v1530,
    sib_type25_v1530,
    sib_type26_v1530,
    sib_type26a_v1610,
    sib_type27_v1610,
    sib_type28_v1610,
    sib_type29_v1610,
    nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<sib_type_opts, true, 10> sib_type_e;

// SystemInformationBlockType1-v1250-IEs ::= SEQUENCE
struct sib_type1_v1250_ies_s {
  struct cell_access_related_info_v1250_s_ {
    bool category0_allowed_r12_present = false;
  };

  // member variables
  bool                              cell_sel_info_v1250_present    = false;
  bool                              freq_band_ind_prio_r12_present = false;
  bool                              non_crit_ext_present           = false;
  cell_access_related_info_v1250_s_ cell_access_related_info_v1250;
  cell_sel_info_v1250_s             cell_sel_info_v1250;
  sib_type1_v1310_ies_s             non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UDT-RestrictingPerPLMN-r13 ::= SEQUENCE
struct udt_restricting_per_plmn_r13_s {
  bool                  udt_restricting_r13_present = false;
  uint8_t               plmn_id_idx_r13             = 1;
  udt_restricting_r13_s udt_restricting_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AC-BarringPerPLMN-List-r12 ::= SEQUENCE (SIZE (1..6)) OF AC-BarringPerPLMN-r12
using ac_barr_per_plmn_list_r12_l = dyn_array<ac_barr_per_plmn_r12_s>;

// ACDC-BarringForCommon-r13 ::= SEQUENCE
struct acdc_barr_for_common_r13_s {
  bool                              acdc_hplm_nonly_r13 = false;
  barr_per_acdc_category_list_r13_l barr_per_acdc_category_list_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ACDC-BarringPerPLMN-List-r13 ::= SEQUENCE (SIZE (1..6)) OF ACDC-BarringPerPLMN-r13
using acdc_barr_per_plmn_list_r13_l = dyn_array<acdc_barr_per_plmn_r13_s>;

// CIOT-EPS-OptimisationInfo-r13 ::= SEQUENCE (SIZE (1..6)) OF CIOT-OptimisationPLMN-r13
using ciot_eps_optim_info_r13_l = dyn_array<ciot_optim_plmn_r13_s>;

// CellSelectionInfo-v920 ::= SEQUENCE
struct cell_sel_info_v920_s {
  bool    q_qual_min_offset_r9_present = false;
  int8_t  q_qual_min_r9                = -34;
  uint8_t q_qual_min_offset_r9         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-InfoList-r15 ::= SEQUENCE (SIZE (1..6)) OF PLMN-Info-r15
using plmn_info_list_r15_l = dyn_array<plmn_info_r15_s>;

// SI-Periodicity-r12 ::= ENUMERATED
struct si_periodicity_r12_opts {
  enum options { rf8, rf16, rf32, rf64, rf128, rf256, rf512, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<si_periodicity_r12_opts> si_periodicity_r12_e;

// SIB-MappingInfo ::= SEQUENCE (SIZE (0..31)) OF SIB-Type
using sib_map_info_l = bounded_array<sib_type_e, 31>;

// SystemInformationBlockType1-v1130-IEs ::= SEQUENCE
struct sib_type1_v1130_ies_s {
  bool                  tdd_cfg_v1130_present       = false;
  bool                  cell_sel_info_v1130_present = false;
  bool                  non_crit_ext_present        = false;
  tdd_cfg_v1130_s       tdd_cfg_v1130;
  cell_sel_info_v1130_s cell_sel_info_v1130;
  sib_type1_v1250_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UDT-RestrictingPerPLMN-List-r13 ::= SEQUENCE (SIZE (1..6)) OF UDT-RestrictingPerPLMN-r13
using udt_restricting_per_plmn_list_r13_l = dyn_array<udt_restricting_per_plmn_r13_s>;

// UE-TimersAndConstants ::= SEQUENCE
struct ue_timers_and_consts_s {
  struct t300_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t300_opts> t300_e_;
  struct t301_opts {
    enum options { ms100, ms200, ms300, ms400, ms600, ms1000, ms1500, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t301_opts> t301_e_;
  struct t310_opts {
    enum options { ms0, ms50, ms100, ms200, ms500, ms1000, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t310_opts> t310_e_;
  struct n310_opts {
    enum options { n1, n2, n3, n4, n6, n8, n10, n20, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<n310_opts> n310_e_;
  struct t311_opts {
    enum options { ms1000, ms3000, ms5000, ms10000, ms15000, ms20000, ms30000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t311_opts> t311_e_;
  struct n311_opts {
    enum options { n1, n2, n3, n4, n5, n6, n8, n10, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<n311_opts> n311_e_;
  struct t300_v1310_opts {
    enum options { ms2500, ms3000, ms3500, ms4000, ms5000, ms6000, ms8000, ms10000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t300_v1310_opts> t300_v1310_e_;
  struct t301_v1310_opts {
    enum options { ms2500, ms3000, ms3500, ms4000, ms5000, ms6000, ms8000, ms10000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t301_v1310_opts> t301_v1310_e_;
  struct t310_v1330_opts {
    enum options { ms4000, ms6000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t310_v1330_opts> t310_v1330_e_;
  struct t300_r15_opts {
    enum options { ms4000, ms6000, ms8000, ms10000, ms15000, ms25000, ms40000, ms60000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t300_r15_opts> t300_r15_e_;

  // member variables
  bool    ext = false;
  t300_e_ t300;
  t301_e_ t301;
  t310_e_ t310;
  n310_e_ n310;
  t311_e_ t311;
  n311_e_ n311;
  // ...
  // group 0
  bool          t300_v1310_present = false;
  bool          t301_v1310_present = false;
  t300_v1310_e_ t300_v1310;
  t301_v1310_e_ t301_v1310;
  // group 1
  bool          t310_v1330_present = false;
  t310_v1330_e_ t310_v1330;
  // group 2
  bool        t300_r15_present = false;
  t300_r15_e_ t300_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingInfo ::= SEQUENCE
struct sched_info_s {
  si_periodicity_r12_e si_periodicity;
  sib_map_info_l       sib_map_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1-v920-IEs ::= SEQUENCE
struct sib_type1_v920_ies_s {
  bool                  ims_emergency_support_r9_present = false;
  bool                  cell_sel_info_v920_present       = false;
  bool                  non_crit_ext_present             = false;
  cell_sel_info_v920_s  cell_sel_info_v920;
  sib_type1_v1130_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType2 ::= SEQUENCE
struct sib_type2_s {
  struct ac_barr_info_s_ {
    bool          ac_barr_for_mo_sig_present  = false;
    bool          ac_barr_for_mo_data_present = false;
    bool          ac_barr_for_emergency       = false;
    ac_barr_cfg_s ac_barr_for_mo_sig;
    ac_barr_cfg_s ac_barr_for_mo_data;
  };
  struct freq_info_s_ {
    struct ul_bw_opts {
      enum options { n6, n15, n25, n50, n75, n100, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<ul_bw_opts> ul_bw_e_;

    // member variables
    bool     ul_carrier_freq_present = false;
    bool     ul_bw_present           = false;
    uint32_t ul_carrier_freq         = 0;
    ul_bw_e_ ul_bw;
    uint8_t  add_spec_emission = 1;
  };
  struct mpdcch_cqi_report_r16_opts {
    enum options { four_bits, both, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<mpdcch_cqi_report_r16_opts> mpdcch_cqi_report_r16_e_;

  // member variables
  bool                   ext                       = false;
  bool                   ac_barr_info_present      = false;
  bool                   mbsfn_sf_cfg_list_present = false;
  ac_barr_info_s_        ac_barr_info;
  rr_cfg_common_sib_s    rr_cfg_common;
  ue_timers_and_consts_s ue_timers_and_consts;
  freq_info_s_           freq_info;
  mbsfn_sf_cfg_list_l    mbsfn_sf_cfg_list;
  time_align_timer_e     time_align_timer_common;
  // ...
  bool          late_non_crit_ext_present = false;
  dyn_octstring late_non_crit_ext;
  // group 0
  copy_ptr<ac_barr_cfg_s> ssac_barr_for_mmtel_voice_r9;
  copy_ptr<ac_barr_cfg_s> ssac_barr_for_mmtel_video_r9;
  // group 1
  copy_ptr<ac_barr_cfg_s> ac_barr_for_csfb_r10;
  // group 2
  bool                                  ac_barr_skip_for_mmtel_voice_r12_present = false;
  bool                                  ac_barr_skip_for_mmtel_video_r12_present = false;
  bool                                  ac_barr_skip_for_sms_r12_present         = false;
  copy_ptr<ac_barr_per_plmn_list_r12_l> ac_barr_per_plmn_list_r12;
  // group 3
  bool voice_service_cause_ind_r12_present = false;
  // group 4
  copy_ptr<acdc_barr_for_common_r13_s>    acdc_barr_for_common_r13;
  copy_ptr<acdc_barr_per_plmn_list_r13_l> acdc_barr_per_plmn_list_r13;
  // group 5
  bool                                          use_full_resume_id_r13_present = false;
  copy_ptr<udt_restricting_r13_s>               udt_restricting_for_common_r13;
  copy_ptr<udt_restricting_per_plmn_list_r13_l> udt_restricting_per_plmn_list_r13;
  copy_ptr<ciot_eps_optim_info_r13_l>           cio_t_eps_optim_info_r13;
  // group 6
  bool unicast_freq_hop_ind_r13_present = false;
  // group 7
  bool                                video_service_cause_ind_r14_present = false;
  copy_ptr<mbsfn_sf_cfg_list_v1430_l> mbsfn_sf_cfg_list_v1430;
  // group 8
  copy_ptr<plmn_info_list_r15_l> plmn_info_list_r15;
  // group 9
  bool cp_edt_r15_present                     = false;
  bool up_edt_r15_present                     = false;
  bool idle_mode_meass_r15_present            = false;
  bool reduced_cp_latency_enabled_r15_present = false;
  // group 10
  bool mbms_rom_service_ind_r15_present = false;
  // group 11
  bool                     rlos_enabled_r16_present                = false;
  bool                     early_security_reactivation_r16_present = false;
  bool                     cp_edt_minus5_gc_r16_present            = false;
  bool                     up_edt_minus5_gc_r16_present            = false;
  bool                     cp_pur_epc_r16_present                  = false;
  bool                     up_pur_epc_r16_present                  = false;
  bool                     cp_pur_minus5_gc_r16_present            = false;
  bool                     up_pur_minus5_gc_r16_present            = false;
  bool                     mpdcch_cqi_report_r16_present           = false;
  bool                     rai_activation_enh_r16_present          = false;
  bool                     idle_mode_meass_nr_r16_present          = false;
  mpdcch_cqi_report_r16_e_ mpdcch_cqi_report_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingInfoList ::= SEQUENCE (SIZE (1..32)) OF SchedulingInfo
using sched_info_list_l = dyn_array<sched_info_s>;

// SystemInformationBlockType1-v890-IEs ::= SEQUENCE
struct sib_type1_v890_ies_s {
  bool                 late_non_crit_ext_present = false;
  bool                 non_crit_ext_present      = false;
  dyn_octstring        late_non_crit_ext;
  sib_type1_v920_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1 ::= SEQUENCE
struct sib_type1_s {
  struct cell_access_related_info_s_ {
    struct cell_barred_opts {
      enum options { barred, not_barred, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<cell_barred_opts> cell_barred_e_;
    struct intra_freq_resel_opts {
      enum options { allowed, not_allowed, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<intra_freq_resel_opts> intra_freq_resel_e_;

    // member variables
    bool                csg_id_present = false;
    plmn_id_list_l      plmn_id_list;
    fixed_bitstring<16> tac;
    fixed_bitstring<28> cell_id;
    cell_barred_e_      cell_barred;
    intra_freq_resel_e_ intra_freq_resel;
    bool                csg_ind = false;
    fixed_bitstring<27> csg_id;
  };
  struct cell_sel_info_s_ {
    bool    q_rx_lev_min_offset_present = false;
    int8_t  q_rx_lev_min                = -70;
    uint8_t q_rx_lev_min_offset         = 1;
  };
  struct si_win_len_opts {
    enum options { ms1, ms2, ms5, ms10, ms15, ms20, ms40, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<si_win_len_opts> si_win_len_e_;

  // member variables
  bool                        p_max_present        = false;
  bool                        tdd_cfg_present      = false;
  bool                        non_crit_ext_present = false;
  cell_access_related_info_s_ cell_access_related_info;
  cell_sel_info_s_            cell_sel_info;
  int8_t                      p_max         = -30;
  uint8_t                     freq_band_ind = 1;
  sched_info_list_l           sched_info_list;
  tdd_cfg_s                   tdd_cfg;
  si_win_len_e_               si_win_len;
  uint8_t                     sys_info_value_tag = 0;
  sib_type1_v890_ies_s        non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc
} // namespace asn1

#endif // SRSASN1_RRC_SI_H
