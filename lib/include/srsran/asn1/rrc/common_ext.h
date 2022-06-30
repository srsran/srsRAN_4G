/**
 * Copyright 2013-2022 Software Radio Systems Limited
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
 *                     3GPP TS ASN1 RRC v15.11.0 (2020-09)
 *
 ******************************************************************************/

#ifndef SRSASN1_RRC_SL_H
#define SRSASN1_RRC_SL_H

#include "rr_common.h"

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// MCS-PSSCH-Range-r15 ::= SEQUENCE
struct mcs_pssch_range_r15_s {
  uint8_t min_mcs_pssch_r15 = 0;
  uint8_t max_mcs_pssch_r15 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PriorityList-r13 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..8)
using sl_prio_list_r13_l = bounded_array<uint8_t, 8>;

// SL-TxPower-r14 ::= CHOICE
struct sl_tx_pwr_r14_c {
  struct types_opts {
    enum options { minusinfinity_r14, tx_pwr_r14, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  sl_tx_pwr_r14_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  int8_t& tx_pwr_r14()
  {
    assert_choice_type(types::tx_pwr_r14, type_, "SL-TxPower-r14");
    return c;
  }
  const int8_t& tx_pwr_r14() const
  {
    assert_choice_type(types::tx_pwr_r14, type_, "SL-TxPower-r14");
    return c;
  }
  void    set_minusinfinity_r14();
  int8_t& set_tx_pwr_r14();

private:
  types  type_;
  int8_t c;
};

// SL-MinT2Value-r15 ::= SEQUENCE
struct sl_min_t2_value_r15_s {
  sl_prio_list_r13_l prio_list_r15;
  uint8_t            min_t2_value_r15 = 10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PPPP-TxConfigIndex-r14 ::= SEQUENCE
struct sl_pppp_tx_cfg_idx_r14_s {
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

// SL-PPPP-TxConfigIndex-v1530 ::= SEQUENCE
struct sl_pppp_tx_cfg_idx_v1530_s {
  using mcs_pssch_range_list_r15_l_ = dyn_array<mcs_pssch_range_r15_s>;

  // member variables
  bool                        mcs_pssch_range_list_r15_present = false;
  mcs_pssch_range_list_r15_l_ mcs_pssch_range_list_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PSSCH-TxParameters-r14 ::= SEQUENCE
struct sl_pssch_tx_params_r14_s {
  struct allowed_retx_num_pssch_r14_opts {
    enum options { n0, n1, both, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<allowed_retx_num_pssch_r14_opts> allowed_retx_num_pssch_r14_e_;

  // member variables
  bool                          max_tx_pwr_r14_present   = false;
  uint8_t                       min_mcs_pssch_r14        = 0;
  uint8_t                       max_mcs_pssch_r14        = 0;
  uint8_t                       min_sub_ch_num_pssch_r14 = 1;
  uint8_t                       max_subch_num_pssch_r14  = 1;
  allowed_retx_num_pssch_r14_e_ allowed_retx_num_pssch_r14;
  sl_tx_pwr_r14_c               max_tx_pwr_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PSSCH-TxParameters-v1530 ::= SEQUENCE
struct sl_pssch_tx_params_v1530_s {
  uint8_t min_mcs_pssch_r15 = 0;
  uint8_t max_mcs_pssch_r15 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-RestrictResourceReservationPeriod-r14 ::= ENUMERATED
struct sl_restrict_res_reserv_period_r14_opts {
  enum options {
    v0dot2,
    v0dot5,
    v1,
    v2,
    v3,
    v4,
    v5,
    v6,
    v7,
    v8,
    v9,
    v10,
    spare4,
    spare3,
    spare2,
    spare1,
    nulltype
  } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
typedef enumerated<sl_restrict_res_reserv_period_r14_opts> sl_restrict_res_reserv_period_r14_e;

// SL-TypeTxSync-r14 ::= ENUMERATED
struct sl_type_tx_sync_r14_opts {
  enum options { gnss, enb, ue, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<sl_type_tx_sync_r14_opts> sl_type_tx_sync_r14_e;

// SL-CBR-PPPP-TxConfigList-r14 ::= SEQUENCE (SIZE (1..8)) OF SL-PPPP-TxConfigIndex-r14
using sl_cbr_pppp_tx_cfg_list_r14_l = dyn_array<sl_pppp_tx_cfg_idx_r14_s>;

// SL-CBR-PPPP-TxConfigList-v1530 ::= SEQUENCE (SIZE (1..8)) OF SL-PPPP-TxConfigIndex-v1530
using sl_cbr_pppp_tx_cfg_list_v1530_l = dyn_array<sl_pppp_tx_cfg_idx_v1530_s>;

// SL-CP-Len-r12 ::= ENUMERATED
struct sl_cp_len_r12_opts {
  enum options { normal, extended, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<sl_cp_len_r12_opts> sl_cp_len_r12_e;

// SL-MinT2ValueList-r15 ::= SEQUENCE (SIZE (1..8)) OF SL-MinT2Value-r15
using sl_min_t2_value_list_r15_l = dyn_array<sl_min_t2_value_r15_s>;

// SL-OffsetIndicator-r12 ::= CHOICE
struct sl_offset_ind_r12_c {
  struct types_opts {
    enum options { small_r12, large_r12, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  sl_offset_ind_r12_c() = default;
  sl_offset_ind_r12_c(const sl_offset_ind_r12_c& other);
  sl_offset_ind_r12_c& operator=(const sl_offset_ind_r12_c& other);
  ~sl_offset_ind_r12_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& small_r12()
  {
    assert_choice_type(types::small_r12, type_, "SL-OffsetIndicator-r12");
    return c.get<uint16_t>();
  }
  uint16_t& large_r12()
  {
    assert_choice_type(types::large_r12, type_, "SL-OffsetIndicator-r12");
    return c.get<uint16_t>();
  }
  const uint16_t& small_r12() const
  {
    assert_choice_type(types::small_r12, type_, "SL-OffsetIndicator-r12");
    return c.get<uint16_t>();
  }
  const uint16_t& large_r12() const
  {
    assert_choice_type(types::large_r12, type_, "SL-OffsetIndicator-r12");
    return c.get<uint16_t>();
  }
  uint16_t& set_small_r12();
  uint16_t& set_large_r12();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// SL-P2X-ResourceSelectionConfig-r14 ::= SEQUENCE
struct sl_p2_x_res_sel_cfg_r14_s {
  bool partial_sensing_r14_present = false;
  bool random_sel_r14_present      = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PSSCH-TxConfig-r14 ::= SEQUENCE
struct sl_pssch_tx_cfg_r14_s {
  struct thres_ue_speed_r14_opts {
    enum options { kmph60, kmph80, kmph100, kmph120, kmph140, kmph160, kmph180, kmph200, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<thres_ue_speed_r14_opts> thres_ue_speed_r14_e_;

  // member variables
  bool                     ext                      = false;
  bool                     type_tx_sync_r14_present = false;
  sl_type_tx_sync_r14_e    type_tx_sync_r14;
  thres_ue_speed_r14_e_    thres_ue_speed_r14;
  sl_pssch_tx_params_r14_s params_above_thres_r14;
  sl_pssch_tx_params_r14_s params_below_thres_r14;
  // ...
  // group 0
  copy_ptr<sl_pssch_tx_params_v1530_s> params_above_thres_v1530;
  copy_ptr<sl_pssch_tx_params_v1530_s> params_below_thres_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-RestrictResourceReservationPeriodList-r14 ::= SEQUENCE (SIZE (1..16)) OF SL-RestrictResourceReservationPeriod-r14
using sl_restrict_res_reserv_period_list_r14_l = bounded_array<sl_restrict_res_reserv_period_r14_e, 16>;

// SL-SyncAllowed-r14 ::= SEQUENCE
struct sl_sync_allowed_r14_s {
  bool gnss_sync_r14_present = false;
  bool enb_sync_r14_present  = false;
  bool ue_sync_r14_present   = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TxParameters-r12 ::= SEQUENCE
struct sl_tx_params_r12_s {
  alpha_r12_e alpha_r12;
  int8_t      p0_r12 = -126;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SubframeBitmapSL-r12 ::= CHOICE
struct sf_bitmap_sl_r12_c {
  struct types_opts {
    enum options { bs4_r12, bs8_r12, bs12_r12, bs16_r12, bs30_r12, bs40_r12, bs42_r12, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  sf_bitmap_sl_r12_c() = default;
  sf_bitmap_sl_r12_c(const sf_bitmap_sl_r12_c& other);
  sf_bitmap_sl_r12_c& operator=(const sf_bitmap_sl_r12_c& other);
  ~sf_bitmap_sl_r12_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<4>& bs4_r12()
  {
    assert_choice_type(types::bs4_r12, type_, "SubframeBitmapSL-r12");
    return c.get<fixed_bitstring<4> >();
  }
  fixed_bitstring<8>& bs8_r12()
  {
    assert_choice_type(types::bs8_r12, type_, "SubframeBitmapSL-r12");
    return c.get<fixed_bitstring<8> >();
  }
  fixed_bitstring<12>& bs12_r12()
  {
    assert_choice_type(types::bs12_r12, type_, "SubframeBitmapSL-r12");
    return c.get<fixed_bitstring<12> >();
  }
  fixed_bitstring<16>& bs16_r12()
  {
    assert_choice_type(types::bs16_r12, type_, "SubframeBitmapSL-r12");
    return c.get<fixed_bitstring<16> >();
  }
  fixed_bitstring<30>& bs30_r12()
  {
    assert_choice_type(types::bs30_r12, type_, "SubframeBitmapSL-r12");
    return c.get<fixed_bitstring<30> >();
  }
  fixed_bitstring<40>& bs40_r12()
  {
    assert_choice_type(types::bs40_r12, type_, "SubframeBitmapSL-r12");
    return c.get<fixed_bitstring<40> >();
  }
  fixed_bitstring<42>& bs42_r12()
  {
    assert_choice_type(types::bs42_r12, type_, "SubframeBitmapSL-r12");
    return c.get<fixed_bitstring<42> >();
  }
  const fixed_bitstring<4>& bs4_r12() const
  {
    assert_choice_type(types::bs4_r12, type_, "SubframeBitmapSL-r12");
    return c.get<fixed_bitstring<4> >();
  }
  const fixed_bitstring<8>& bs8_r12() const
  {
    assert_choice_type(types::bs8_r12, type_, "SubframeBitmapSL-r12");
    return c.get<fixed_bitstring<8> >();
  }
  const fixed_bitstring<12>& bs12_r12() const
  {
    assert_choice_type(types::bs12_r12, type_, "SubframeBitmapSL-r12");
    return c.get<fixed_bitstring<12> >();
  }
  const fixed_bitstring<16>& bs16_r12() const
  {
    assert_choice_type(types::bs16_r12, type_, "SubframeBitmapSL-r12");
    return c.get<fixed_bitstring<16> >();
  }
  const fixed_bitstring<30>& bs30_r12() const
  {
    assert_choice_type(types::bs30_r12, type_, "SubframeBitmapSL-r12");
    return c.get<fixed_bitstring<30> >();
  }
  const fixed_bitstring<40>& bs40_r12() const
  {
    assert_choice_type(types::bs40_r12, type_, "SubframeBitmapSL-r12");
    return c.get<fixed_bitstring<40> >();
  }
  const fixed_bitstring<42>& bs42_r12() const
  {
    assert_choice_type(types::bs42_r12, type_, "SubframeBitmapSL-r12");
    return c.get<fixed_bitstring<42> >();
  }
  fixed_bitstring<4>&  set_bs4_r12();
  fixed_bitstring<8>&  set_bs8_r12();
  fixed_bitstring<12>& set_bs12_r12();
  fixed_bitstring<16>& set_bs16_r12();
  fixed_bitstring<30>& set_bs30_r12();
  fixed_bitstring<40>& set_bs40_r12();
  fixed_bitstring<42>& set_bs42_r12();

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<42> > c;

  void destroy_();
};

// SubframeBitmapSL-r14 ::= CHOICE
struct sf_bitmap_sl_r14_c {
  struct types_opts {
    enum options { bs10_r14, bs16_r14, bs20_r14, bs30_r14, bs40_r14, bs50_r14, bs60_r14, bs100_r14, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  sf_bitmap_sl_r14_c() = default;
  sf_bitmap_sl_r14_c(const sf_bitmap_sl_r14_c& other);
  sf_bitmap_sl_r14_c& operator=(const sf_bitmap_sl_r14_c& other);
  ~sf_bitmap_sl_r14_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<10>& bs10_r14()
  {
    assert_choice_type(types::bs10_r14, type_, "SubframeBitmapSL-r14");
    return c.get<fixed_bitstring<10> >();
  }
  fixed_bitstring<16>& bs16_r14()
  {
    assert_choice_type(types::bs16_r14, type_, "SubframeBitmapSL-r14");
    return c.get<fixed_bitstring<16> >();
  }
  fixed_bitstring<20>& bs20_r14()
  {
    assert_choice_type(types::bs20_r14, type_, "SubframeBitmapSL-r14");
    return c.get<fixed_bitstring<20> >();
  }
  fixed_bitstring<30>& bs30_r14()
  {
    assert_choice_type(types::bs30_r14, type_, "SubframeBitmapSL-r14");
    return c.get<fixed_bitstring<30> >();
  }
  fixed_bitstring<40>& bs40_r14()
  {
    assert_choice_type(types::bs40_r14, type_, "SubframeBitmapSL-r14");
    return c.get<fixed_bitstring<40> >();
  }
  fixed_bitstring<50>& bs50_r14()
  {
    assert_choice_type(types::bs50_r14, type_, "SubframeBitmapSL-r14");
    return c.get<fixed_bitstring<50> >();
  }
  fixed_bitstring<60>& bs60_r14()
  {
    assert_choice_type(types::bs60_r14, type_, "SubframeBitmapSL-r14");
    return c.get<fixed_bitstring<60> >();
  }
  fixed_bitstring<100>& bs100_r14()
  {
    assert_choice_type(types::bs100_r14, type_, "SubframeBitmapSL-r14");
    return c.get<fixed_bitstring<100> >();
  }
  const fixed_bitstring<10>& bs10_r14() const
  {
    assert_choice_type(types::bs10_r14, type_, "SubframeBitmapSL-r14");
    return c.get<fixed_bitstring<10> >();
  }
  const fixed_bitstring<16>& bs16_r14() const
  {
    assert_choice_type(types::bs16_r14, type_, "SubframeBitmapSL-r14");
    return c.get<fixed_bitstring<16> >();
  }
  const fixed_bitstring<20>& bs20_r14() const
  {
    assert_choice_type(types::bs20_r14, type_, "SubframeBitmapSL-r14");
    return c.get<fixed_bitstring<20> >();
  }
  const fixed_bitstring<30>& bs30_r14() const
  {
    assert_choice_type(types::bs30_r14, type_, "SubframeBitmapSL-r14");
    return c.get<fixed_bitstring<30> >();
  }
  const fixed_bitstring<40>& bs40_r14() const
  {
    assert_choice_type(types::bs40_r14, type_, "SubframeBitmapSL-r14");
    return c.get<fixed_bitstring<40> >();
  }
  const fixed_bitstring<50>& bs50_r14() const
  {
    assert_choice_type(types::bs50_r14, type_, "SubframeBitmapSL-r14");
    return c.get<fixed_bitstring<50> >();
  }
  const fixed_bitstring<60>& bs60_r14() const
  {
    assert_choice_type(types::bs60_r14, type_, "SubframeBitmapSL-r14");
    return c.get<fixed_bitstring<60> >();
  }
  const fixed_bitstring<100>& bs100_r14() const
  {
    assert_choice_type(types::bs100_r14, type_, "SubframeBitmapSL-r14");
    return c.get<fixed_bitstring<100> >();
  }
  fixed_bitstring<10>&  set_bs10_r14();
  fixed_bitstring<16>&  set_bs16_r14();
  fixed_bitstring<20>&  set_bs20_r14();
  fixed_bitstring<30>&  set_bs30_r14();
  fixed_bitstring<40>&  set_bs40_r14();
  fixed_bitstring<50>&  set_bs50_r14();
  fixed_bitstring<60>&  set_bs60_r14();
  fixed_bitstring<100>& set_bs100_r14();

private:
  types                                  type_;
  choice_buffer_t<fixed_bitstring<100> > c;

  void destroy_();
};

// PhysCellIdList-r13 ::= SEQUENCE (SIZE (1..16)) OF INTEGER (0..503)
using pci_list_r13_l = bounded_array<uint16_t, 16>;

// SL-CommResourcePoolV2X-r14 ::= SEQUENCE
struct sl_comm_res_pool_v2x_r14_s {
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
  struct rx_params_ncell_r14_s_ {
    bool      tdd_cfg_r14_present = false;
    tdd_cfg_s tdd_cfg_r14;
    uint8_t   sync_cfg_idx_r14 = 0;
  };

  // member variables
  bool                                     ext                                    = false;
  bool                                     sl_offset_ind_r14_present              = false;
  bool                                     start_rb_pscch_pool_r14_present        = false;
  bool                                     rx_params_ncell_r14_present            = false;
  bool                                     data_tx_params_r14_present             = false;
  bool                                     zone_id_r14_present                    = false;
  bool                                     thresh_s_rssi_cbr_r14_present          = false;
  bool                                     pool_report_id_r14_present             = false;
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
  rx_params_ncell_r14_s_                   rx_params_ncell_r14;
  sl_tx_params_r12_s                       data_tx_params_r14;
  uint8_t                                  zone_id_r14           = 0;
  uint8_t                                  thresh_s_rssi_cbr_r14 = 0;
  uint8_t                                  pool_report_id_r14    = 1;
  sl_cbr_pppp_tx_cfg_list_r14_l            cbr_pssch_tx_cfg_list_r14;
  sl_p2_x_res_sel_cfg_r14_s                res_sel_cfg_p2_x_r14;
  sl_sync_allowed_r14_s                    sync_allowed_r14;
  sl_restrict_res_reserv_period_list_r14_l restrict_res_reserv_period_r14;
  // ...
  // group 0
  copy_ptr<sl_min_t2_value_list_r15_l>      sl_min_t2_value_list_r15;
  copy_ptr<sl_cbr_pppp_tx_cfg_list_v1530_l> cbr_pssch_tx_cfg_list_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PSSCH-TxConfigList-r14 ::= SEQUENCE (SIZE (1..16)) OF SL-PSSCH-TxConfig-r14
using sl_pssch_tx_cfg_list_r14_l = dyn_array<sl_pssch_tx_cfg_r14_s>;

// SL-PoolSelectionConfig-r12 ::= SEQUENCE
struct sl_pool_sel_cfg_r12_s {
  uint8_t thresh_low_r12  = 0;
  uint8_t thresh_high_r12 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-SyncConfigNFreq-r13 ::= SEQUENCE
struct sl_sync_cfg_nfreq_r13_s {
  struct async_params_r13_s_ {
    sl_cp_len_r12_e sync_cp_len_r13;
    uint8_t         sync_offset_ind_r13 = 0;
    uint8_t         slssid_r13          = 0;
  };
  struct tx_params_r13_s_ {
    bool                sync_info_reserved_r13_present = false;
    bool                sync_tx_periodic_r13_present   = false;
    sl_tx_params_r12_s  sync_tx_params_r13;
    uint8_t             sync_tx_thresh_ic_r13 = 0;
    fixed_bitstring<19> sync_info_reserved_r13;
  };
  struct rx_params_r13_s_ {
    struct disc_sync_win_r13_opts {
      enum options { w1, w2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<disc_sync_win_r13_opts> disc_sync_win_r13_e_;

    // member variables
    disc_sync_win_r13_e_ disc_sync_win_r13;
  };

  // member variables
  bool                ext                      = false;
  bool                async_params_r13_present = false;
  bool                tx_params_r13_present    = false;
  bool                rx_params_r13_present    = false;
  async_params_r13_s_ async_params_r13;
  tx_params_r13_s_    tx_params_r13;
  rx_params_r13_s_    rx_params_r13;
  // ...
  // group 0
  bool    sync_offset_ind_v1430_present = false;
  bool    gnss_sync_r14_present         = false;
  uint8_t sync_offset_ind_v1430         = 40;
  // group 1
  bool    sync_offset_ind2_r14_present = false;
  bool    sync_offset_ind3_r14_present = false;
  uint8_t sync_offset_ind2_r14         = 0;
  uint8_t sync_offset_ind3_r14         = 0;
  // group 2
  bool slss_tx_disabled_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-TF-ResourceConfig-r12 ::= SEQUENCE
struct sl_tf_res_cfg_r12_s {
  uint8_t             prb_num_r12   = 1;
  uint8_t             prb_start_r12 = 0;
  uint8_t             prb_end_r12   = 0;
  sl_offset_ind_r12_c offset_ind_r12;
  sf_bitmap_sl_r12_c  sf_bitmap_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-ThresPSSCH-RSRP-List-r14 ::= SEQUENCE (SIZE (64)) OF INTEGER (0..66)
using sl_thres_pssch_rsrp_list_r14_l = std::array<uint8_t, 64>;

// SL-CommRxPoolListV2X-r14 ::= SEQUENCE (SIZE (1..16)) OF SL-CommResourcePoolV2X-r14
using sl_comm_rx_pool_list_v2x_r14_l = dyn_array<sl_comm_res_pool_v2x_r14_s>;

// SL-CommTxPoolListV2X-r14 ::= SEQUENCE (SIZE (1..8)) OF SL-CommResourcePoolV2X-r14
using sl_comm_tx_pool_list_v2x_r14_l = dyn_array<sl_comm_res_pool_v2x_r14_s>;

// SL-CommTxPoolSensingConfig-r14 ::= SEQUENCE
struct sl_comm_tx_pool_sensing_cfg_r14_s {
  struct prob_res_keep_r14_opts {
    enum options { v0, v0dot2, v0dot4, v0dot6, v0dot8, spare3, spare2, spare1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<prob_res_keep_r14_opts> prob_res_keep_r14_e_;
  struct p2x_sensing_cfg_r14_s_ {
    uint8_t             min_num_candidate_sf_r14 = 1;
    fixed_bitstring<10> gap_candidate_sensing_r14;
  };
  struct sl_reselect_after_r14_opts {
    enum options {
      n1,
      n2,
      n3,
      n4,
      n5,
      n6,
      n7,
      n8,
      n9,
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
  typedef enumerated<sl_reselect_after_r14_opts> sl_reselect_after_r14_e_;

  // member variables
  bool                                     restrict_res_reserv_period_r14_present = false;
  bool                                     p2x_sensing_cfg_r14_present            = false;
  bool                                     sl_reselect_after_r14_present          = false;
  sl_pssch_tx_cfg_list_r14_l               pssch_tx_cfg_list_r14;
  sl_thres_pssch_rsrp_list_r14_l           thres_pssch_rsrp_list_r14;
  sl_restrict_res_reserv_period_list_r14_l restrict_res_reserv_period_r14;
  prob_res_keep_r14_e_                     prob_res_keep_r14;
  p2x_sensing_cfg_r14_s_                   p2x_sensing_cfg_r14;
  sl_reselect_after_r14_e_                 sl_reselect_after_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-DiscResourcePool-r12 ::= SEQUENCE
struct sl_disc_res_pool_r12_s {
  struct disc_period_r12_opts {
    enum options { rf32, rf64, rf128, rf256, rf512, rf1024, rf16_v1310, spare, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<disc_period_r12_opts> disc_period_r12_e_;
  struct tx_params_r12_s_ {
    struct ue_sel_res_cfg_r12_s_ {
      struct pool_sel_r12_c_ {
        struct types_opts {
          enum options { rsrp_based_r12, random_r12, nulltype } value;

          const char* to_string() const;
        };
        typedef enumerated<types_opts> types;

        // choice methods
        pool_sel_r12_c_() = default;
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        sl_pool_sel_cfg_r12_s& rsrp_based_r12()
        {
          assert_choice_type(types::rsrp_based_r12, type_, "poolSelection-r12");
          return c;
        }
        const sl_pool_sel_cfg_r12_s& rsrp_based_r12() const
        {
          assert_choice_type(types::rsrp_based_r12, type_, "poolSelection-r12");
          return c;
        }
        sl_pool_sel_cfg_r12_s& set_rsrp_based_r12();
        void                   set_random_r12();

      private:
        types                 type_;
        sl_pool_sel_cfg_r12_s c;
      };
      struct tx_probability_r12_opts {
        enum options { p25, p50, p75, p100, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<tx_probability_r12_opts> tx_probability_r12_e_;

      // member variables
      pool_sel_r12_c_       pool_sel_r12;
      tx_probability_r12_e_ tx_probability_r12;
    };

    // member variables
    bool                  ue_sel_res_cfg_r12_present = false;
    sl_tx_params_r12_s    tx_params_general_r12;
    ue_sel_res_cfg_r12_s_ ue_sel_res_cfg_r12;
  };
  struct rx_params_r12_s_ {
    bool      tdd_cfg_r12_present = false;
    tdd_cfg_s tdd_cfg_r12;
    uint8_t   sync_cfg_idx_r12 = 0;
  };
  struct disc_period_v1310_c_ {
    struct setup_opts {
      enum options { rf4, rf6, rf7, rf8, rf12, rf14, rf24, rf28, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<setup_opts> setup_e_;
    using types = setup_e;

    // choice methods
    disc_period_v1310_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_e_& setup()
    {
      assert_choice_type(types::setup, type_, "discPeriod-v1310");
      return c;
    }
    const setup_e_& setup() const
    {
      assert_choice_type(types::setup, type_, "discPeriod-v1310");
      return c;
    }
    void      set_release();
    setup_e_& set_setup();

  private:
    types    type_;
    setup_e_ c;
  };
  struct rx_params_add_neigh_freq_r13_c_ {
    struct setup_s_ {
      pci_list_r13_l pci_r13;
    };
    using types = setup_e;

    // choice methods
    rx_params_add_neigh_freq_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "rxParamsAddNeighFreq-r13");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "rxParamsAddNeighFreq-r13");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct tx_params_add_neigh_freq_r13_c_ {
    struct setup_s_ {
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

      // member variables
      bool            p_max_present            = false;
      bool            tdd_cfg_r13_present      = false;
      bool            tdd_cfg_v1130_present    = false;
      bool            sync_cfg_idx_r13_present = false;
      pci_list_r13_l  pci_r13;
      int8_t          p_max = -30;
      tdd_cfg_s       tdd_cfg_r13;
      tdd_cfg_v1130_s tdd_cfg_v1130;
      freq_info_s_    freq_info;
      int8_t          ref_sig_pwr      = -60;
      uint8_t         sync_cfg_idx_r13 = 0;
    };
    using types = setup_e;

    // choice methods
    tx_params_add_neigh_freq_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "txParamsAddNeighFreq-r13");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "txParamsAddNeighFreq-r13");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct tx_params_add_neigh_freq_v1370_c_ {
    struct setup_s_ {
      struct freq_info_v1370_s_ {
        uint16_t add_spec_emission_v1370 = 33;
      };

      // member variables
      freq_info_v1370_s_ freq_info_v1370;
    };
    using types = setup_e;

    // choice methods
    tx_params_add_neigh_freq_v1370_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "txParamsAddNeighFreq-v1370");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "txParamsAddNeighFreq-v1370");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };

  // member variables
  bool                ext                   = false;
  bool                tx_params_r12_present = false;
  bool                rx_params_r12_present = false;
  sl_cp_len_r12_e     cp_len_r12;
  disc_period_r12_e_  disc_period_r12;
  uint8_t             num_retx_r12   = 0;
  uint8_t             num_repeat_r12 = 1;
  sl_tf_res_cfg_r12_s tf_res_cfg_r12;
  tx_params_r12_s_    tx_params_r12;
  rx_params_r12_s_    rx_params_r12;
  // ...
  // group 0
  copy_ptr<disc_period_v1310_c_>            disc_period_v1310;
  copy_ptr<rx_params_add_neigh_freq_r13_c_> rx_params_add_neigh_freq_r13;
  copy_ptr<tx_params_add_neigh_freq_r13_c_> tx_params_add_neigh_freq_r13;
  // group 1
  copy_ptr<tx_params_add_neigh_freq_v1370_c_> tx_params_add_neigh_freq_v1370;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-SyncConfigListNFreqV2X-r14 ::= SEQUENCE (SIZE (1..16)) OF SL-SyncConfigNFreq-r13
using sl_sync_cfg_list_nfreq_v2x_r14_l = dyn_array<sl_sync_cfg_nfreq_r13_s>;

// SL-ZoneConfig-r14 ::= SEQUENCE
struct sl_zone_cfg_r14_s {
  struct zone_len_r14_opts {
    enum options { m5, m10, m20, m50, m100, m200, m500, spare1, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<zone_len_r14_opts> zone_len_r14_e_;
  struct zone_width_r14_opts {
    enum options { m5, m10, m20, m50, m100, m200, m500, spare1, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<zone_width_r14_opts> zone_width_r14_e_;

  // member variables
  zone_len_r14_e_   zone_len_r14;
  zone_width_r14_e_ zone_width_r14;
  uint8_t           zone_id_longi_mod_r14 = 1;
  uint8_t           zone_id_lati_mod_r14  = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhysCellIdRange ::= SEQUENCE
struct pci_range_s {
  struct range_opts {
    enum options {
      n4,
      n8,
      n12,
      n16,
      n24,
      n32,
      n48,
      n64,
      n84,
      n96,
      n128,
      n168,
      n252,
      n504,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<range_opts> range_e_;

  // member variables
  bool     range_present = false;
  uint16_t start         = 0;
  range_e_ range;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pci_range_s& other) const;
  bool        operator!=(const pci_range_s& other) const { return not(*this == other); }
};

// SL-DiscTxPoolList-r12 ::= SEQUENCE (SIZE (1..4)) OF SL-DiscResourcePool-r12
using sl_disc_tx_pool_list_r12_l = dyn_array<sl_disc_res_pool_r12_s>;

// SL-DiscTxPowerInfo-r12 ::= SEQUENCE
struct sl_disc_tx_pwr_info_r12_s {
  bool   ext                 = false;
  int8_t disc_max_tx_pwr_r12 = -30;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-V2X-FreqSelectionConfig-r15 ::= SEQUENCE
struct sl_v2x_freq_sel_cfg_r15_s {
  bool               thresh_cbr_freq_resel_r15_present   = false;
  bool               thresh_cbr_freq_keeping_r15_present = false;
  sl_prio_list_r13_l prio_list_r15;
  uint8_t            thresh_cbr_freq_resel_r15   = 0;
  uint8_t            thresh_cbr_freq_keeping_r15 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-V2X-InterFreqUE-Config-r14 ::= SEQUENCE
struct sl_v2x_inter_freq_ue_cfg_r14_s {
  bool                              ext                                      = false;
  bool                              pci_list_r14_present                     = false;
  bool                              type_tx_sync_r14_present                 = false;
  bool                              v2x_sync_cfg_r14_present                 = false;
  bool                              v2x_comm_rx_pool_r14_present             = false;
  bool                              v2x_comm_tx_pool_normal_r14_present      = false;
  bool                              p2x_comm_tx_pool_normal_r14_present      = false;
  bool                              v2x_comm_tx_pool_exceptional_r14_present = false;
  bool                              v2x_res_sel_cfg_r14_present              = false;
  bool                              zone_cfg_r14_present                     = false;
  bool                              offset_dfn_r14_present                   = false;
  pci_list_r13_l                    pci_list_r14;
  sl_type_tx_sync_r14_e             type_tx_sync_r14;
  sl_sync_cfg_list_nfreq_v2x_r14_l  v2x_sync_cfg_r14;
  sl_comm_rx_pool_list_v2x_r14_l    v2x_comm_rx_pool_r14;
  sl_comm_tx_pool_list_v2x_r14_l    v2x_comm_tx_pool_normal_r14;
  sl_comm_tx_pool_list_v2x_r14_l    p2x_comm_tx_pool_normal_r14;
  sl_comm_res_pool_v2x_r14_s        v2x_comm_tx_pool_exceptional_r14;
  sl_comm_tx_pool_sensing_cfg_r14_s v2x_res_sel_cfg_r14;
  sl_zone_cfg_r14_s                 zone_cfg_r14;
  uint16_t                          offset_dfn_r14 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AllowedMeasBandwidth ::= ENUMERATED
struct allowed_meas_bw_opts {
  enum options { mbw6, mbw15, mbw25, mbw50, mbw75, mbw100, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<allowed_meas_bw_opts> allowed_meas_bw_e;

// CellList-r15 ::= SEQUENCE (SIZE (1..8)) OF PhysCellIdRange
using cell_list_r15_l = dyn_array<pci_range_s>;

// CellSelectionInfoNFreq-r13 ::= SEQUENCE
struct cell_sel_info_nfreq_r13_s {
  struct q_hyst_r13_opts {
    enum options {
      db0,
      db1,
      db2,
      db3,
      db4,
      db5,
      db6,
      db8,
      db10,
      db12,
      db14,
      db16,
      db18,
      db20,
      db22,
      db24,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<q_hyst_r13_opts> q_hyst_r13_e_;

  // member variables
  bool          q_rx_lev_min_offset_present = false;
  int8_t        q_rx_lev_min_r13            = -70;
  uint8_t       q_rx_lev_min_offset         = 1;
  q_hyst_r13_e_ q_hyst_r13;
  int8_t        q_rx_lev_min_resel_r13 = -70;
  uint8_t       t_resel_eutra_r13      = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-AllowedCarrierFreqList-r15 ::= SEQUENCE
struct sl_allowed_carrier_freq_list_r15_s {
  using allowed_carrier_freq_set1_l_ = bounded_array<uint32_t, 8>;
  using allowed_carrier_freq_set2_l_ = bounded_array<uint32_t, 8>;

  // member variables
  allowed_carrier_freq_set1_l_ allowed_carrier_freq_set1;
  allowed_carrier_freq_set2_l_ allowed_carrier_freq_set2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-DestinationInfoList-r12 ::= SEQUENCE (SIZE (1..16)) OF BIT STRING (SIZE (24))
using sl_dest_info_list_r12_l = bounded_array<fixed_bitstring<24>, 16>;

// SL-DiscRxPoolList-r12 ::= SEQUENCE (SIZE (1..16)) OF SL-DiscResourcePool-r12
using sl_disc_rx_pool_list_r12_l = dyn_array<sl_disc_res_pool_r12_s>;

// SL-DiscTxPowerInfoList-r12 ::= SEQUENCE (SIZE (3)) OF SL-DiscTxPowerInfo-r12
using sl_disc_tx_pwr_info_list_r12_l = std::array<sl_disc_tx_pwr_info_r12_s, 3>;

// SL-DiscTxResourcesInterFreq-r13 ::= CHOICE
struct sl_disc_tx_res_inter_freq_r13_c {
  struct types_opts {
    enum options {
      acquire_si_from_carrier_r13,
      disc_tx_pool_common_r13,
      request_ded_r13,
      no_tx_on_carrier_r13,
      nulltype
    } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  sl_disc_tx_res_inter_freq_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  sl_disc_tx_pool_list_r12_l& disc_tx_pool_common_r13()
  {
    assert_choice_type(types::disc_tx_pool_common_r13, type_, "SL-DiscTxResourcesInterFreq-r13");
    return c;
  }
  const sl_disc_tx_pool_list_r12_l& disc_tx_pool_common_r13() const
  {
    assert_choice_type(types::disc_tx_pool_common_r13, type_, "SL-DiscTxResourcesInterFreq-r13");
    return c;
  }
  void                        set_acquire_si_from_carrier_r13();
  sl_disc_tx_pool_list_r12_l& set_disc_tx_pool_common_r13();
  void                        set_request_ded_r13();
  void                        set_no_tx_on_carrier_r13();

private:
  types                      type_;
  sl_disc_tx_pool_list_r12_l c;
};

// SL-V2X-FreqSelectionConfigList-r15 ::= SEQUENCE (SIZE (1..8)) OF SL-V2X-FreqSelectionConfig-r15
using sl_v2x_freq_sel_cfg_list_r15_l = dyn_array<sl_v2x_freq_sel_cfg_r15_s>;

// SL-V2X-UE-ConfigList-r14 ::= SEQUENCE (SIZE (1..16)) OF SL-V2X-InterFreqUE-Config-r14
using sl_v2x_ue_cfg_list_r14_l = dyn_array<sl_v2x_inter_freq_ue_cfg_r14_s>;

// MeasIdleCarrierEUTRA-r15 ::= SEQUENCE
struct meas_idle_carrier_eutra_r15_s {
  struct report_quantities_opts {
    enum options { rsrp, rsrq, both, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<report_quantities_opts> report_quantities_e_;
  struct quality_thres_r15_s_ {
    bool    idle_rsrp_thres_r15_present = false;
    bool    idle_rsrq_thres_r15_present = false;
    uint8_t idle_rsrp_thres_r15         = 0;
    int8_t  idle_rsrq_thres_r15         = -30;
  };

  // member variables
  bool                 ext                        = false;
  bool                 validity_area_r15_present  = false;
  bool                 meas_cell_list_r15_present = false;
  bool                 quality_thres_r15_present  = false;
  uint32_t             carrier_freq_r15           = 0;
  allowed_meas_bw_e    allowed_meas_bw_r15;
  cell_list_r15_l      validity_area_r15;
  cell_list_r15_l      meas_cell_list_r15;
  report_quantities_e_ report_quantities;
  quality_thres_r15_s_ quality_thres_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-CBR-Levels-Config-r14 ::= SEQUENCE (SIZE (1..16)) OF INTEGER (0..100)
using sl_cbr_levels_cfg_r14_l = bounded_array<uint8_t, 16>;

// SL-CBR-PSSCH-TxConfig-r14 ::= SEQUENCE
struct sl_cbr_pssch_tx_cfg_r14_s {
  uint16_t                 cr_limit_r14 = 0;
  sl_pssch_tx_params_r14_s tx_params_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-HoppingConfigComm-r12 ::= SEQUENCE
struct sl_hop_cfg_comm_r12_s {
  struct num_subbands_r12_opts {
    enum options { ns1, ns2, ns4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<num_subbands_r12_opts> num_subbands_r12_e_;

  // member variables
  uint16_t            hop_param_r12 = 0;
  num_subbands_r12_e_ num_subbands_r12;
  uint8_t             rb_offset_r12 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-InterFreqInfoV2X-r14 ::= SEQUENCE
struct sl_inter_freq_info_v2x_r14_s {
  struct sl_bw_r14_opts {
    enum options { n6, n15, n25, n50, n75, n100, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sl_bw_r14_opts> sl_bw_r14_e_;
  struct add_spec_emission_v2x_r14_c_ {
    struct types_opts {
      enum options { add_spec_emission_r14, add_spec_emission_v1440, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    add_spec_emission_v2x_r14_c_() = default;
    add_spec_emission_v2x_r14_c_(const add_spec_emission_v2x_r14_c_& other);
    add_spec_emission_v2x_r14_c_& operator=(const add_spec_emission_v2x_r14_c_& other);
    ~add_spec_emission_v2x_r14_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& add_spec_emission_r14()
    {
      assert_choice_type(types::add_spec_emission_r14, type_, "additionalSpectrumEmissionV2X-r14");
      return c.get<uint8_t>();
    }
    uint16_t& add_spec_emission_v1440()
    {
      assert_choice_type(types::add_spec_emission_v1440, type_, "additionalSpectrumEmissionV2X-r14");
      return c.get<uint16_t>();
    }
    const uint8_t& add_spec_emission_r14() const
    {
      assert_choice_type(types::add_spec_emission_r14, type_, "additionalSpectrumEmissionV2X-r14");
      return c.get<uint8_t>();
    }
    const uint16_t& add_spec_emission_v1440() const
    {
      assert_choice_type(types::add_spec_emission_v1440, type_, "additionalSpectrumEmissionV2X-r14");
      return c.get<uint16_t>();
    }
    uint8_t&  set_add_spec_emission_r14();
    uint16_t& set_add_spec_emission_v1440();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                       ext                         = false;
  bool                       plmn_id_list_r14_present    = false;
  bool                       sl_max_tx_pwr_r14_present   = false;
  bool                       sl_bw_r14_present           = false;
  bool                       v2x_sched_pool_r14_present  = false;
  bool                       v2x_ue_cfg_list_r14_present = false;
  plmn_id_list_l             plmn_id_list_r14;
  uint32_t                   v2x_comm_carrier_freq_r14 = 0;
  int8_t                     sl_max_tx_pwr_r14         = -30;
  sl_bw_r14_e_               sl_bw_r14;
  sl_comm_res_pool_v2x_r14_s v2x_sched_pool_r14;
  sl_v2x_ue_cfg_list_r14_l   v2x_ue_cfg_list_r14;
  // ...
  // group 0
  copy_ptr<add_spec_emission_v2x_r14_c_> add_spec_emission_v2x_r14;
  // group 1
  copy_ptr<sl_v2x_freq_sel_cfg_list_r15_l> v2x_freq_sel_cfg_list_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PPPR-Dest-CarrierFreq ::= SEQUENCE
struct sl_pppr_dest_carrier_freq_s {
  bool                               dest_info_list_r15_present            = false;
  bool                               allowed_carrier_freq_list_r15_present = false;
  sl_dest_info_list_r12_l            dest_info_list_r15;
  sl_allowed_carrier_freq_list_r15_s allowed_carrier_freq_list_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PeriodComm-r12 ::= ENUMERATED
struct sl_period_comm_r12_opts {
  enum options {
    sf40,
    sf60,
    sf70,
    sf80,
    sf120,
    sf140,
    sf160,
    sf240,
    sf280,
    sf320,
    spare6,
    spare5,
    spare4,
    spare3,
    spare2,
    spare,
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<sl_period_comm_r12_opts> sl_period_comm_r12_e;

// SL-ResourcesInterFreq-r13 ::= SEQUENCE
struct sl_res_inter_freq_r13_s {
  bool                            disc_rx_res_inter_freq_r13_present = false;
  bool                            disc_tx_res_inter_freq_r13_present = false;
  sl_disc_rx_pool_list_r12_l      disc_rx_res_inter_freq_r13;
  sl_disc_tx_res_inter_freq_r13_c disc_tx_res_inter_freq_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-SyncConfig-r12 ::= SEQUENCE
struct sl_sync_cfg_r12_s {
  struct tx_params_r12_s_ {
    bool                sync_info_reserved_r12_present = false;
    sl_tx_params_r12_s  sync_tx_params_r12;
    uint8_t             sync_tx_thresh_ic_r12 = 0;
    fixed_bitstring<19> sync_info_reserved_r12;
  };
  struct rx_params_ncell_r12_s_ {
    struct disc_sync_win_r12_opts {
      enum options { w1, w2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<disc_sync_win_r12_opts> disc_sync_win_r12_e_;

    // member variables
    uint16_t             pci_r12 = 0;
    disc_sync_win_r12_e_ disc_sync_win_r12;
  };

  // member variables
  bool                   ext                         = false;
  bool                   tx_params_r12_present       = false;
  bool                   rx_params_ncell_r12_present = false;
  sl_cp_len_r12_e        sync_cp_len_r12;
  uint8_t                sync_offset_ind_r12 = 0;
  uint8_t                slssid_r12          = 0;
  tx_params_r12_s_       tx_params_r12;
  rx_params_ncell_r12_s_ rx_params_ncell_r12;
  // ...
  // group 0
  bool sync_tx_periodic_r13_present = false;
  // group 1
  bool    sync_offset_ind_v1430_present = false;
  bool    gnss_sync_r14_present         = false;
  uint8_t sync_offset_ind_v1430         = 40;
  // group 2
  bool    sync_offset_ind2_r14_present = false;
  bool    sync_offset_ind3_r14_present = false;
  uint8_t sync_offset_ind2_r14         = 0;
  uint8_t sync_offset_ind3_r14         = 0;
  // group 3
  bool slss_tx_disabled_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WLAN-Identifiers-r12 ::= SEQUENCE
struct wlan_ids_r12_s {
  bool                     ext                = false;
  bool                     ssid_r12_present   = false;
  bool                     bssid_r12_present  = false;
  bool                     hessid_r12_present = false;
  bounded_octstring<1, 32> ssid_r12;
  fixed_octstring<6>       bssid_r12;
  fixed_octstring<6>       hessid_r12;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const wlan_ids_r12_s& other) const;
  bool        operator!=(const wlan_ids_r12_s& other) const { return not(*this == other); }
};

// WLAN-backhaulRate-r12 ::= ENUMERATED
struct wlan_backhaul_rate_r12_opts {
  enum options {
    r0,
    r4,
    r8,
    r16,
    r32,
    r64,
    r128,
    r256,
    r512,
    r1024,
    r2048,
    r4096,
    r8192,
    r16384,
    r32768,
    r65536,
    r131072,
    r262144,
    r524288,
    r1048576,
    r2097152,
    r4194304,
    r8388608,
    r16777216,
    r33554432,
    r67108864,
    r134217728,
    r268435456,
    r536870912,
    r1073741824,
    r2147483648,
    r4294967296,
    nulltype
  } value;
  typedef uint64_t number_type;

  const char* to_string() const;
  uint64_t    to_number() const;
};
typedef enumerated<wlan_backhaul_rate_r12_opts> wlan_backhaul_rate_r12_e;

// EUTRA-CarrierList-r15 ::= SEQUENCE (SIZE (1..8)) OF MeasIdleCarrierEUTRA-r15
using eutra_carrier_list_r15_l = dyn_array<meas_idle_carrier_eutra_r15_s>;

// ReferenceTime-r15 ::= SEQUENCE
struct ref_time_r15_s {
  uint32_t ref_days_r15                  = 0;
  uint32_t ref_seconds_r15               = 0;
  uint16_t ref_milli_seconds_r15         = 0;
  uint16_t ref_quarter_micro_seconds_r15 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-AnchorCarrierFreqList-V2X-r14 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (0..262143)
using sl_anchor_carrier_freq_list_v2x_r14_l = bounded_array<uint32_t, 8>;

// SL-CBR-CommonTxConfigList-r14 ::= SEQUENCE
struct sl_cbr_common_tx_cfg_list_r14_s {
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

// SL-CommResourcePool-r12 ::= SEQUENCE
struct sl_comm_res_pool_r12_s {
  struct ue_sel_res_cfg_r12_s_ {
    bool                    trpt_subset_r12_present = false;
    sl_tf_res_cfg_r12_s     data_tf_res_cfg_r12;
    bounded_bitstring<3, 5> trpt_subset_r12;
  };
  struct rx_params_ncell_r12_s_ {
    bool      tdd_cfg_r12_present = false;
    tdd_cfg_s tdd_cfg_r12;
    uint8_t   sync_cfg_idx_r12 = 0;
  };
  struct tx_params_r12_s_ {
    sl_tx_params_r12_s sc_tx_params_r12;
    sl_tx_params_r12_s data_tx_params_r12;
  };

  // member variables
  bool                   ext                         = false;
  bool                   ue_sel_res_cfg_r12_present  = false;
  bool                   rx_params_ncell_r12_present = false;
  bool                   tx_params_r12_present       = false;
  sl_cp_len_r12_e        sc_cp_len_r12;
  sl_period_comm_r12_e   sc_period_r12;
  sl_tf_res_cfg_r12_s    sc_tf_res_cfg_r12;
  sl_cp_len_r12_e        data_cp_len_r12;
  sl_hop_cfg_comm_r12_s  data_hop_cfg_r12;
  ue_sel_res_cfg_r12_s_  ue_sel_res_cfg_r12;
  rx_params_ncell_r12_s_ rx_params_ncell_r12;
  tx_params_r12_s_       tx_params_r12;
  // ...
  // group 0
  copy_ptr<sl_prio_list_r13_l> prio_list_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-InterFreqInfoListV2X-r14 ::= SEQUENCE (SIZE (0..7)) OF SL-InterFreqInfoV2X-r14
using sl_inter_freq_info_list_v2x_r14_l = dyn_array<sl_inter_freq_info_v2x_r14_s>;

// SL-PPPR-Dest-CarrierFreqList-r15 ::= SEQUENCE (SIZE (1..16)) OF SL-PPPR-Dest-CarrierFreq
using sl_pppr_dest_carrier_freq_list_r15_l = dyn_array<sl_pppr_dest_carrier_freq_s>;

// SL-SyncConfigListV2X-r14 ::= SEQUENCE (SIZE (1..16)) OF SL-SyncConfig-r12
using sl_sync_cfg_list_v2x_r14_l = dyn_array<sl_sync_cfg_r12_s>;

// WLAN-Id-List-r12 ::= SEQUENCE (SIZE (1..16)) OF WLAN-Identifiers-r12
using wlan_id_list_r12_l = dyn_array<wlan_ids_r12_s>;

// WLAN-OffloadConfig-r12 ::= SEQUENCE
struct wlan_offload_cfg_r12_s {
  struct thres_rsrp_r12_s_ {
    uint8_t thres_rsrp_low_r12  = 0;
    uint8_t thres_rsrp_high_r12 = 0;
  };
  struct thres_rsrq_r12_s_ {
    uint8_t thres_rsrq_low_r12  = 0;
    uint8_t thres_rsrq_high_r12 = 0;
  };
  struct thres_rsrq_on_all_symbols_with_wb_r12_s_ {
    uint8_t thres_rsrq_on_all_symbols_with_wb_low_r12  = 0;
    uint8_t thres_rsrq_on_all_symbols_with_wb_high_r12 = 0;
  };
  struct thres_rsrq_on_all_symbols_r12_s_ {
    uint8_t thres_rsrq_on_all_symbols_low_r12  = 0;
    uint8_t thres_rsrq_on_all_symbols_high_r12 = 0;
  };
  struct thres_rsrq_wb_r12_s_ {
    uint8_t thres_rsrq_wb_low_r12  = 0;
    uint8_t thres_rsrq_wb_high_r12 = 0;
  };
  struct thres_ch_utilization_r12_s_ {
    uint16_t thres_ch_utilization_low_r12  = 0;
    uint16_t thres_ch_utilization_high_r12 = 0;
  };
  struct thres_backhaul_bw_r12_s_ {
    wlan_backhaul_rate_r12_e thres_backhaul_dl_bw_low_r12;
    wlan_backhaul_rate_r12_e thres_backhaul_dl_bw_high_r12;
    wlan_backhaul_rate_r12_e thres_backhaul_ul_bw_low_r12;
    wlan_backhaul_rate_r12_e thres_backhaul_ul_bw_high_r12;
  };
  struct thres_wlan_rssi_r12_s_ {
    uint16_t thres_wlan_rssi_low_r12  = 0;
    uint16_t thres_wlan_rssi_high_r12 = 0;
  };

  // member variables
  bool                                     ext                                           = false;
  bool                                     thres_rsrp_r12_present                        = false;
  bool                                     thres_rsrq_r12_present                        = false;
  bool                                     thres_rsrq_on_all_symbols_with_wb_r12_present = false;
  bool                                     thres_rsrq_on_all_symbols_r12_present         = false;
  bool                                     thres_rsrq_wb_r12_present                     = false;
  bool                                     thres_ch_utilization_r12_present              = false;
  bool                                     thres_backhaul_bw_r12_present                 = false;
  bool                                     thres_wlan_rssi_r12_present                   = false;
  bool                                     offload_pref_ind_r12_present                  = false;
  bool                                     t_steering_wlan_r12_present                   = false;
  thres_rsrp_r12_s_                        thres_rsrp_r12;
  thres_rsrq_r12_s_                        thres_rsrq_r12;
  thres_rsrq_on_all_symbols_with_wb_r12_s_ thres_rsrq_on_all_symbols_with_wb_r12;
  thres_rsrq_on_all_symbols_r12_s_         thres_rsrq_on_all_symbols_r12;
  thres_rsrq_wb_r12_s_                     thres_rsrq_wb_r12;
  thres_ch_utilization_r12_s_              thres_ch_utilization_r12;
  thres_backhaul_bw_r12_s_                 thres_backhaul_bw_r12;
  thres_wlan_rssi_r12_s_                   thres_wlan_rssi_r12;
  fixed_bitstring<16>                      offload_pref_ind_r12;
  uint8_t                                  t_steering_wlan_r12 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-V2X-ConfigCommon-r14 ::= SEQUENCE
struct sl_v2x_cfg_common_r14_s {
  bool                                  v2x_comm_rx_pool_r14_present               = false;
  bool                                  v2x_comm_tx_pool_normal_common_r14_present = false;
  bool                                  p2x_comm_tx_pool_normal_common_r14_present = false;
  bool                                  v2x_comm_tx_pool_exceptional_r14_present   = false;
  bool                                  v2x_sync_cfg_r14_present                   = false;
  bool                                  v2x_inter_freq_info_list_r14_present       = false;
  bool                                  v2x_res_sel_cfg_r14_present                = false;
  bool                                  zone_cfg_r14_present                       = false;
  bool                                  type_tx_sync_r14_present                   = false;
  bool                                  thres_sl_tx_prioritization_r14_present     = false;
  bool                                  anchor_carrier_freq_list_r14_present       = false;
  bool                                  offset_dfn_r14_present                     = false;
  bool                                  cbr_common_tx_cfg_list_r14_present         = false;
  sl_comm_rx_pool_list_v2x_r14_l        v2x_comm_rx_pool_r14;
  sl_comm_tx_pool_list_v2x_r14_l        v2x_comm_tx_pool_normal_common_r14;
  sl_comm_tx_pool_list_v2x_r14_l        p2x_comm_tx_pool_normal_common_r14;
  sl_comm_res_pool_v2x_r14_s            v2x_comm_tx_pool_exceptional_r14;
  sl_sync_cfg_list_v2x_r14_l            v2x_sync_cfg_r14;
  sl_inter_freq_info_list_v2x_r14_l     v2x_inter_freq_info_list_r14;
  sl_comm_tx_pool_sensing_cfg_r14_s     v2x_res_sel_cfg_r14;
  sl_zone_cfg_r14_s                     zone_cfg_r14;
  sl_type_tx_sync_r14_e                 type_tx_sync_r14;
  uint8_t                               thres_sl_tx_prioritization_r14 = 1;
  sl_anchor_carrier_freq_list_v2x_r14_l anchor_carrier_freq_list_r14;
  uint16_t                              offset_dfn_r14 = 0;
  sl_cbr_common_tx_cfg_list_r14_s       cbr_common_tx_cfg_list_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-V2X-PacketDuplicationConfig-r15 ::= SEQUENCE
struct sl_v2x_packet_dupl_cfg_r15_s {
  bool                                 ext                                  = false;
  bool                                 allowed_carrier_freq_cfg_r15_present = false;
  uint8_t                              thresh_sl_reliability_r15            = 1;
  sl_pppr_dest_carrier_freq_list_r15_l allowed_carrier_freq_cfg_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-V2X-SyncFreqList-r15 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (0..262143)
using sl_v2x_sync_freq_list_r15_l = bounded_array<uint32_t, 8>;

// TimeReferenceInfo-r15 ::= SEQUENCE
struct time_ref_info_r15_s {
  bool           uncertainty_r15_present    = false;
  bool           time_info_type_r15_present = false;
  bool           ref_sfn_r15_present        = false;
  ref_time_r15_s time_r15;
  uint8_t        uncertainty_r15 = 0;
  uint16_t       ref_sfn_r15     = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WLAN-OffloadInfoPerPLMN-r12 ::= SEQUENCE
struct wlan_offload_info_per_plmn_r12_s {
  bool                   ext                                 = false;
  bool                   wlan_offload_cfg_common_r12_present = false;
  bool                   wlan_id_list_r12_present            = false;
  wlan_offload_cfg_r12_s wlan_offload_cfg_common_r12;
  wlan_id_list_r12_l     wlan_id_list_r12;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-ReliabilityList-r15 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..8)
using sl_reliability_list_r15_l = bounded_array<uint8_t, 8>;

// SL-GapPattern-r13 ::= SEQUENCE
struct sl_gap_pattern_r13_s {
  struct gap_period_r13_opts {
    enum options {
      sf40,
      sf60,
      sf70,
      sf80,
      sf120,
      sf140,
      sf160,
      sf240,
      sf280,
      sf320,
      sf640,
      sf1280,
      sf2560,
      sf5120,
      sf10240,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<gap_period_r13_opts> gap_period_r13_e_;

  // member variables
  bool                        ext = false;
  gap_period_r13_e_           gap_period_r13;
  sl_offset_ind_r12_c         gap_offset_r12;
  bounded_bitstring<1, 10240> gap_sf_bitmap_r13;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-GapPatternList-r13 ::= SEQUENCE (SIZE (1..8)) OF SL-GapPattern-r13
using sl_gap_pattern_list_r13_l = dyn_array<sl_gap_pattern_r13_s>;

} // namespace rrc
} // namespace asn1

#endif // SRSASN1_RRC_SL_H
