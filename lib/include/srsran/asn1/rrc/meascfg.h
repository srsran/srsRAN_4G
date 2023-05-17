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

#ifndef SRSASN1_RRC_MEASCFG_H
#define SRSASN1_RRC_MEASCFG_H

#include "common_ext.h"
#include "rr_common.h"

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// Q-OffsetRange ::= ENUMERATED
struct q_offset_range_opts {
  enum options {
    db_minus24,
    db_minus22,
    db_minus20,
    db_minus18,
    db_minus16,
    db_minus14,
    db_minus12,
    db_minus10,
    db_minus8,
    db_minus6,
    db_minus5,
    db_minus4,
    db_minus3,
    db_minus2,
    db_minus1,
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
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
typedef enumerated<q_offset_range_opts> q_offset_range_e;

// RSS-MeasPowerBias-r16 ::= ENUMERATED
struct rss_meas_pwr_bias_r16_opts {
  enum options { db_minus6, db_minus3, db0, db3, db6, db9, db12, rss_not_used, nulltype } value;
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
typedef enumerated<rss_meas_pwr_bias_r16_opts> rss_meas_pwr_bias_r16_e;

// SecondaryPreRegistrationZoneIdListHRPD ::= SEQUENCE (SIZE (1..2)) OF INTEGER (0..255)
using secondary_pre_regist_zone_id_list_hrpd_l = bounded_array<uint16_t, 2>;

// SpeedStateScaleFactors ::= SEQUENCE
struct speed_state_scale_factors_s {
  struct sf_medium_opts {
    enum options { odot25, odot5, odot75, ldot0, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<sf_medium_opts> sf_medium_e_;
  struct sf_high_opts {
    enum options { odot25, odot5, odot75, ldot0, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<sf_high_opts> sf_high_e_;

  // member variables
  sf_medium_e_ sf_medium;
  sf_high_e_   sf_high;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandIndicatorGERAN ::= ENUMERATED
struct band_ind_geran_opts {
  enum options { dcs1800, pcs1900, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<band_ind_geran_opts> band_ind_geran_e;

// ExplicitListOfARFCNs ::= SEQUENCE (SIZE (0..31)) OF INTEGER (0..1023)
using explicit_list_of_arfcns_l = bounded_array<uint16_t, 31>;

// MTC-SSB-NR-r15 ::= SEQUENCE
struct mtc_ssb_nr_r15_s {
  struct periodicity_and_offset_r15_c_ {
    struct types_opts {
      enum options { sf5_r15, sf10_r15, sf20_r15, sf40_r15, sf80_r15, sf160_r15, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    periodicity_and_offset_r15_c_() = default;
    periodicity_and_offset_r15_c_(const periodicity_and_offset_r15_c_& other);
    periodicity_and_offset_r15_c_& operator=(const periodicity_and_offset_r15_c_& other);
    ~periodicity_and_offset_r15_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const periodicity_and_offset_r15_c_& other) const;
    bool        operator!=(const periodicity_and_offset_r15_c_& other) const { return not(*this == other); }
    // getters
    uint8_t& sf5_r15()
    {
      assert_choice_type(types::sf5_r15, type_, "periodicityAndOffset-r15");
      return c.get<uint8_t>();
    }
    uint8_t& sf10_r15()
    {
      assert_choice_type(types::sf10_r15, type_, "periodicityAndOffset-r15");
      return c.get<uint8_t>();
    }
    uint8_t& sf20_r15()
    {
      assert_choice_type(types::sf20_r15, type_, "periodicityAndOffset-r15");
      return c.get<uint8_t>();
    }
    uint8_t& sf40_r15()
    {
      assert_choice_type(types::sf40_r15, type_, "periodicityAndOffset-r15");
      return c.get<uint8_t>();
    }
    uint8_t& sf80_r15()
    {
      assert_choice_type(types::sf80_r15, type_, "periodicityAndOffset-r15");
      return c.get<uint8_t>();
    }
    uint8_t& sf160_r15()
    {
      assert_choice_type(types::sf160_r15, type_, "periodicityAndOffset-r15");
      return c.get<uint8_t>();
    }
    const uint8_t& sf5_r15() const
    {
      assert_choice_type(types::sf5_r15, type_, "periodicityAndOffset-r15");
      return c.get<uint8_t>();
    }
    const uint8_t& sf10_r15() const
    {
      assert_choice_type(types::sf10_r15, type_, "periodicityAndOffset-r15");
      return c.get<uint8_t>();
    }
    const uint8_t& sf20_r15() const
    {
      assert_choice_type(types::sf20_r15, type_, "periodicityAndOffset-r15");
      return c.get<uint8_t>();
    }
    const uint8_t& sf40_r15() const
    {
      assert_choice_type(types::sf40_r15, type_, "periodicityAndOffset-r15");
      return c.get<uint8_t>();
    }
    const uint8_t& sf80_r15() const
    {
      assert_choice_type(types::sf80_r15, type_, "periodicityAndOffset-r15");
      return c.get<uint8_t>();
    }
    const uint8_t& sf160_r15() const
    {
      assert_choice_type(types::sf160_r15, type_, "periodicityAndOffset-r15");
      return c.get<uint8_t>();
    }
    uint8_t& set_sf5_r15();
    uint8_t& set_sf10_r15();
    uint8_t& set_sf20_r15();
    uint8_t& set_sf40_r15();
    uint8_t& set_sf80_r15();
    uint8_t& set_sf160_r15();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct ssb_dur_r15_opts {
    enum options { sf1, sf2, sf3, sf4, sf5, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ssb_dur_r15_opts> ssb_dur_r15_e_;

  // member variables
  periodicity_and_offset_r15_c_ periodicity_and_offset_r15;
  ssb_dur_r15_e_                ssb_dur_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const mtc_ssb_nr_r15_s& other) const;
  bool        operator!=(const mtc_ssb_nr_r15_s& other) const { return not(*this == other); }
};

// MultiFrequencyBandListNR-r15 ::= SEQUENCE (SIZE (1..32)) OF INTEGER (1..1024)
using multi_freq_band_list_nr_r15_l = bounded_array<uint16_t, 32>;

// PreRegistrationInfoHRPD ::= SEQUENCE
struct pre_regist_info_hrpd_s {
  bool                                     pre_regist_zone_id_present                = false;
  bool                                     secondary_pre_regist_zone_id_list_present = false;
  bool                                     pre_regist_allowed                        = false;
  uint16_t                                 pre_regist_zone_id                        = 0;
  secondary_pre_regist_zone_id_list_hrpd_l secondary_pre_regist_zone_id_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SS-RSSI-Measurement-r15 ::= SEQUENCE
struct ss_rssi_meas_r15_s {
  bounded_bitstring<1, 80> meas_slots_r15;
  uint8_t                  end_symbol_r15 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ss_rssi_meas_r15_s& other) const;
  bool        operator!=(const ss_rssi_meas_r15_s& other) const { return not(*this == other); }
};

// SSB-ToMeasure-r15 ::= CHOICE
struct ssb_to_measure_r15_c {
  struct types_opts {
    enum options { short_bitmap_r15, medium_bitmap_r15, long_bitmap_r15, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  ssb_to_measure_r15_c() = default;
  ssb_to_measure_r15_c(const ssb_to_measure_r15_c& other);
  ssb_to_measure_r15_c& operator=(const ssb_to_measure_r15_c& other);
  ~ssb_to_measure_r15_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ssb_to_measure_r15_c& other) const;
  bool        operator!=(const ssb_to_measure_r15_c& other) const { return not(*this == other); }
  // getters
  fixed_bitstring<4>& short_bitmap_r15()
  {
    assert_choice_type(types::short_bitmap_r15, type_, "SSB-ToMeasure-r15");
    return c.get<fixed_bitstring<4> >();
  }
  fixed_bitstring<8>& medium_bitmap_r15()
  {
    assert_choice_type(types::medium_bitmap_r15, type_, "SSB-ToMeasure-r15");
    return c.get<fixed_bitstring<8> >();
  }
  fixed_bitstring<64>& long_bitmap_r15()
  {
    assert_choice_type(types::long_bitmap_r15, type_, "SSB-ToMeasure-r15");
    return c.get<fixed_bitstring<64> >();
  }
  const fixed_bitstring<4>& short_bitmap_r15() const
  {
    assert_choice_type(types::short_bitmap_r15, type_, "SSB-ToMeasure-r15");
    return c.get<fixed_bitstring<4> >();
  }
  const fixed_bitstring<8>& medium_bitmap_r15() const
  {
    assert_choice_type(types::medium_bitmap_r15, type_, "SSB-ToMeasure-r15");
    return c.get<fixed_bitstring<8> >();
  }
  const fixed_bitstring<64>& long_bitmap_r15() const
  {
    assert_choice_type(types::long_bitmap_r15, type_, "SSB-ToMeasure-r15");
    return c.get<fixed_bitstring<64> >();
  }
  fixed_bitstring<4>&  set_short_bitmap_r15();
  fixed_bitstring<8>&  set_medium_bitmap_r15();
  fixed_bitstring<64>& set_long_bitmap_r15();

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<64> > c;

  void destroy_();
};

// ThresholdListNR-r15 ::= SEQUENCE
struct thres_list_nr_r15_s {
  bool    nr_rsrp_r15_present = false;
  bool    nr_rsrq_r15_present = false;
  bool    nr_sinr_r15_present = false;
  uint8_t nr_rsrp_r15         = 0;
  uint8_t nr_rsrq_r15         = 0;
  uint8_t nr_sinr_r15         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const thres_list_nr_r15_s& other) const;
  bool        operator!=(const thres_list_nr_r15_s& other) const { return not(*this == other); }
};

// CarrierFreqsGERAN ::= SEQUENCE
struct carrier_freqs_geran_s {
  struct following_arfcns_c_ {
    struct equally_spaced_arfcns_s_ {
      uint8_t arfcn_spacing        = 1;
      uint8_t nof_following_arfcns = 0;
    };
    struct types_opts {
      enum options { explicit_list_of_arfcns, equally_spaced_arfcns, variable_bit_map_of_arfcns, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    following_arfcns_c_() = default;
    following_arfcns_c_(const following_arfcns_c_& other);
    following_arfcns_c_& operator=(const following_arfcns_c_& other);
    ~following_arfcns_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const following_arfcns_c_& other) const;
    bool        operator!=(const following_arfcns_c_& other) const { return not(*this == other); }
    // getters
    explicit_list_of_arfcns_l& explicit_list_of_arfcns()
    {
      assert_choice_type(types::explicit_list_of_arfcns, type_, "followingARFCNs");
      return c.get<explicit_list_of_arfcns_l>();
    }
    equally_spaced_arfcns_s_& equally_spaced_arfcns()
    {
      assert_choice_type(types::equally_spaced_arfcns, type_, "followingARFCNs");
      return c.get<equally_spaced_arfcns_s_>();
    }
    bounded_octstring<1, 16>& variable_bit_map_of_arfcns()
    {
      assert_choice_type(types::variable_bit_map_of_arfcns, type_, "followingARFCNs");
      return c.get<bounded_octstring<1, 16> >();
    }
    const explicit_list_of_arfcns_l& explicit_list_of_arfcns() const
    {
      assert_choice_type(types::explicit_list_of_arfcns, type_, "followingARFCNs");
      return c.get<explicit_list_of_arfcns_l>();
    }
    const equally_spaced_arfcns_s_& equally_spaced_arfcns() const
    {
      assert_choice_type(types::equally_spaced_arfcns, type_, "followingARFCNs");
      return c.get<equally_spaced_arfcns_s_>();
    }
    const bounded_octstring<1, 16>& variable_bit_map_of_arfcns() const
    {
      assert_choice_type(types::variable_bit_map_of_arfcns, type_, "followingARFCNs");
      return c.get<bounded_octstring<1, 16> >();
    }
    explicit_list_of_arfcns_l& set_explicit_list_of_arfcns();
    equally_spaced_arfcns_s_&  set_equally_spaced_arfcns();
    bounded_octstring<1, 16>&  set_variable_bit_map_of_arfcns();

  private:
    types                                                                                          type_;
    choice_buffer_t<bounded_octstring<1, 16>, equally_spaced_arfcns_s_, explicit_list_of_arfcns_l> c;

    void destroy_();
  };

  // member variables
  uint16_t            start_arfcn = 0;
  band_ind_geran_e    band_ind;
  following_arfcns_c_ following_arfcns;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const carrier_freqs_geran_s& other) const;
  bool        operator!=(const carrier_freqs_geran_s& other) const { return not(*this == other); }
};

// CellReselectionSubPriority-r13 ::= ENUMERATED
struct cell_resel_sub_prio_r13_opts {
  enum options { odot2, odot4, odot6, odot8, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
typedef enumerated<cell_resel_sub_prio_r13_opts> cell_resel_sub_prio_r13_e;

// MultiBandInfoList-r11 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..256)
using multi_band_info_list_r11_l = bounded_array<uint16_t, 8>;

// RSS-ConfigCarrierInfo-r16 ::= SEQUENCE
struct rss_cfg_carrier_info_r16_s {
  struct time_offset_granularity_r16_opts {
    enum options { g1, g2, g4, g8, g16, g32, g64, g128, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<time_offset_granularity_r16_opts> time_offset_granularity_r16_e_;

  // member variables
  bounded_bitstring<1, 15>       nb_idx_r16;
  time_offset_granularity_r16_e_ time_offset_granularity_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const rss_cfg_carrier_info_r16_s& other) const;
  bool        operator!=(const rss_cfg_carrier_info_r16_s& other) const { return not(*this == other); }
};

// SSB-PositionQCL-RelationNR-r16 ::= ENUMERATED
struct ssb_position_qcl_relation_nr_r16_opts {
  enum options { n1, n2, n4, n8, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<ssb_position_qcl_relation_nr_r16_opts> ssb_position_qcl_relation_nr_r16_e;

// SSB-PositionQCL-RelationNR-r17 ::= ENUMERATED
struct ssb_position_qcl_relation_nr_r17_opts {
  enum options { n32, n64, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<ssb_position_qcl_relation_nr_r17_opts> ssb_position_qcl_relation_nr_r17_e;

// MobilityStateParameters ::= SEQUENCE
struct mob_state_params_s {
  struct t_eval_opts {
    enum options { s30, s60, s120, s180, s240, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<t_eval_opts> t_eval_e_;
  struct t_hyst_normal_opts {
    enum options { s30, s60, s120, s180, s240, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<t_hyst_normal_opts> t_hyst_normal_e_;

  // member variables
  t_eval_e_        t_eval;
  t_hyst_normal_e_ t_hyst_normal;
  uint8_t          n_cell_change_medium = 1;
  uint8_t          n_cell_change_high   = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqCDMA2000 ::= SEQUENCE
struct carrier_freq_cdma2000_s {
  bandclass_cdma2000_e band_class;
  uint16_t             arfcn = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const carrier_freq_cdma2000_s& other) const;
  bool        operator!=(const carrier_freq_cdma2000_s& other) const { return not(*this == other); }
};

// CellsToAddMod-v1610 ::= SEQUENCE
struct cells_to_add_mod_v1610_s {
  rss_meas_pwr_bias_r16_e rss_meas_pwr_bias_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cells_to_add_mod_v1610_s& other) const;
  bool        operator!=(const cells_to_add_mod_v1610_s& other) const { return not(*this == other); }
};

// MeasCSI-RS-Config-r12 ::= SEQUENCE
struct meas_csi_rs_cfg_r12_s {
  bool             ext                = false;
  uint8_t          meas_csi_rs_id_r12 = 1;
  uint16_t         pci_r12            = 0;
  uint16_t         scrambling_id_r12  = 0;
  uint8_t          res_cfg_r12        = 0;
  uint8_t          sf_offset_r12      = 0;
  q_offset_range_e csi_rs_individual_offset_r12;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const meas_csi_rs_cfg_r12_s& other) const;
  bool        operator!=(const meas_csi_rs_cfg_r12_s& other) const { return not(*this == other); }
};

// PhysCellIdRangeUTRA-FDD-r9 ::= SEQUENCE
struct pci_range_utra_fdd_r9_s {
  bool     range_r9_present = false;
  uint16_t start_r9         = 0;
  uint16_t range_r9         = 2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pci_range_utra_fdd_r9_s& other) const;
  bool        operator!=(const pci_range_utra_fdd_r9_s& other) const { return not(*this == other); }
};

// SSB-PositionQCL-CellsToAddNR-r16 ::= SEQUENCE
struct ssb_position_qcl_cells_to_add_nr_r16_s {
  uint16_t                           pci_r16 = 0;
  ssb_position_qcl_relation_nr_r16_e ssb_position_qcl_nr_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ssb_position_qcl_cells_to_add_nr_r16_s& other) const;
  bool        operator!=(const ssb_position_qcl_cells_to_add_nr_r16_s& other) const { return not(*this == other); }
};

// SSB-PositionQCL-CellsToAddNR-r17 ::= SEQUENCE
struct ssb_position_qcl_cells_to_add_nr_r17_s {
  uint16_t                           pci_nr_r17 = 0;
  ssb_position_qcl_relation_nr_r17_e ssb_position_qcl_nr_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ssb_position_qcl_cells_to_add_nr_r17_s& other) const;
  bool        operator!=(const ssb_position_qcl_cells_to_add_nr_r17_s& other) const { return not(*this == other); }
};

// AllowedCellsToAddMod-r13 ::= SEQUENCE
struct allowed_cells_to_add_mod_r13_s {
  uint8_t     cell_idx_r13 = 1;
  pci_range_s pci_range_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const allowed_cells_to_add_mod_r13_s& other) const;
  bool        operator!=(const allowed_cells_to_add_mod_r13_s& other) const { return not(*this == other); }
};

// AltTTT-CellsToAddMod-r12 ::= SEQUENCE
struct alt_ttt_cells_to_add_mod_r12_s {
  uint8_t     cell_idx_r12 = 1;
  pci_range_s pci_range_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const alt_ttt_cells_to_add_mod_r12_s& other) const;
  bool        operator!=(const alt_ttt_cells_to_add_mod_r12_s& other) const { return not(*this == other); }
};

// BT-NameList-r15 ::= SEQUENCE (SIZE (1..4)) OF OCTET STRING (SIZE (1..248))
using bt_name_list_r15_l = bounded_array<bounded_octstring<1, 248>, 4>;

// CellsToAddMod ::= SEQUENCE
struct cells_to_add_mod_s {
  uint8_t          cell_idx = 1;
  uint16_t         pci      = 0;
  q_offset_range_e cell_individual_offset;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cells_to_add_mod_s& other) const;
  bool        operator!=(const cells_to_add_mod_s& other) const { return not(*this == other); }
};

// CellsToAddModCDMA2000 ::= SEQUENCE
struct cells_to_add_mod_cdma2000_s {
  uint8_t  cell_idx = 1;
  uint16_t pci      = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cells_to_add_mod_cdma2000_s& other) const;
  bool        operator!=(const cells_to_add_mod_cdma2000_s& other) const { return not(*this == other); }
};

// CellsToAddModList-v1610 ::= SEQUENCE (SIZE (1..32)) OF CellsToAddMod-v1610
using cells_to_add_mod_list_v1610_l = dyn_array<cells_to_add_mod_v1610_s>;

// CellsToAddModNR-r15 ::= SEQUENCE
struct cells_to_add_mod_nr_r15_s {
  uint8_t  cell_idx_r15 = 1;
  uint16_t pci_r15      = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cells_to_add_mod_nr_r15_s& other) const;
  bool        operator!=(const cells_to_add_mod_nr_r15_s& other) const { return not(*this == other); }
};

// CellsToAddModNR-r16 ::= SEQUENCE
struct cells_to_add_mod_nr_r16_s {
  uint8_t          cell_idx_r16 = 1;
  uint16_t         pci_r16      = 0;
  q_offset_range_e cell_individual_offset_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cells_to_add_mod_nr_r16_s& other) const;
  bool        operator!=(const cells_to_add_mod_nr_r16_s& other) const { return not(*this == other); }
};

// CellsToAddModUTRA-FDD ::= SEQUENCE
struct cells_to_add_mod_utra_fdd_s {
  uint8_t  cell_idx = 1;
  uint16_t pci      = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cells_to_add_mod_utra_fdd_s& other) const;
  bool        operator!=(const cells_to_add_mod_utra_fdd_s& other) const { return not(*this == other); }
};

// CellsToAddModUTRA-TDD ::= SEQUENCE
struct cells_to_add_mod_utra_tdd_s {
  uint8_t cell_idx = 1;
  uint8_t pci      = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cells_to_add_mod_utra_tdd_s& other) const;
  bool        operator!=(const cells_to_add_mod_utra_tdd_s& other) const { return not(*this == other); }
};

// ExcludedCellsToAddMod ::= SEQUENCE
struct excluded_cells_to_add_mod_s {
  uint8_t     cell_idx = 1;
  pci_range_s pci_range;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const excluded_cells_to_add_mod_s& other) const;
  bool        operator!=(const excluded_cells_to_add_mod_s& other) const { return not(*this == other); }
};

// MeasCSI-RS-ToAddModList-r12 ::= SEQUENCE (SIZE (1..96)) OF MeasCSI-RS-Config-r12
using meas_csi_rs_to_add_mod_list_r12_l = dyn_array<meas_csi_rs_cfg_r12_s>;

// MeasCSI-RS-ToRemoveList-r12 ::= SEQUENCE (SIZE (1..96)) OF INTEGER (1..96)
using meas_csi_rs_to_rem_list_r12_l = dyn_array<uint8_t>;

// MeasSubframeCellList-r10 ::= SEQUENCE (SIZE (1..32)) OF PhysCellIdRange
using meas_sf_cell_list_r10_l = dyn_array<pci_range_s>;

// PhysCellIdRangeUTRA-FDDList-r9 ::= SEQUENCE (SIZE (1..4)) OF PhysCellIdRangeUTRA-FDD-r9
using pci_range_utra_fdd_list_r9_l = dyn_array<pci_range_utra_fdd_r9_s>;

// SSB-PositionQCL-CellsToAddModListNR-r16 ::= SEQUENCE (SIZE (1..32)) OF SSB-PositionQCL-CellsToAddNR-r16
using ssb_position_qcl_cells_to_add_mod_list_nr_r16_l = dyn_array<ssb_position_qcl_cells_to_add_nr_r16_s>;

// SSB-PositionQCL-CellsToAddModListNR-r17 ::= SEQUENCE (SIZE (1..32)) OF SSB-PositionQCL-CellsToAddNR-r17
using ssb_position_qcl_cells_to_add_mod_list_nr_r17_l = dyn_array<ssb_position_qcl_cells_to_add_nr_r17_s>;

// ThresholdEUTRA ::= CHOICE
struct thres_eutra_c {
  struct types_opts {
    enum options { thres_rsrp, thres_rsrq, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  thres_eutra_c() = default;
  thres_eutra_c(const thres_eutra_c& other);
  thres_eutra_c& operator=(const thres_eutra_c& other);
  ~thres_eutra_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const thres_eutra_c& other) const;
  bool        operator!=(const thres_eutra_c& other) const { return not(*this == other); }
  // getters
  uint8_t& thres_rsrp()
  {
    assert_choice_type(types::thres_rsrp, type_, "ThresholdEUTRA");
    return c.get<uint8_t>();
  }
  uint8_t& thres_rsrq()
  {
    assert_choice_type(types::thres_rsrq, type_, "ThresholdEUTRA");
    return c.get<uint8_t>();
  }
  const uint8_t& thres_rsrp() const
  {
    assert_choice_type(types::thres_rsrp, type_, "ThresholdEUTRA");
    return c.get<uint8_t>();
  }
  const uint8_t& thres_rsrq() const
  {
    assert_choice_type(types::thres_rsrq, type_, "ThresholdEUTRA");
    return c.get<uint8_t>();
  }
  uint8_t& set_thres_rsrp();
  uint8_t& set_thres_rsrq();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// ThresholdNR-r15 ::= CHOICE
struct thres_nr_r15_c {
  struct types_opts {
    enum options { nr_rsrp_r15, nr_rsrq_r15, nr_sinr_r15, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  thres_nr_r15_c() = default;
  thres_nr_r15_c(const thres_nr_r15_c& other);
  thres_nr_r15_c& operator=(const thres_nr_r15_c& other);
  ~thres_nr_r15_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const thres_nr_r15_c& other) const;
  bool        operator!=(const thres_nr_r15_c& other) const { return not(*this == other); }
  // getters
  uint8_t& nr_rsrp_r15()
  {
    assert_choice_type(types::nr_rsrp_r15, type_, "ThresholdNR-r15");
    return c.get<uint8_t>();
  }
  uint8_t& nr_rsrq_r15()
  {
    assert_choice_type(types::nr_rsrq_r15, type_, "ThresholdNR-r15");
    return c.get<uint8_t>();
  }
  uint8_t& nr_sinr_r15()
  {
    assert_choice_type(types::nr_sinr_r15, type_, "ThresholdNR-r15");
    return c.get<uint8_t>();
  }
  const uint8_t& nr_rsrp_r15() const
  {
    assert_choice_type(types::nr_rsrp_r15, type_, "ThresholdNR-r15");
    return c.get<uint8_t>();
  }
  const uint8_t& nr_rsrq_r15() const
  {
    assert_choice_type(types::nr_rsrq_r15, type_, "ThresholdNR-r15");
    return c.get<uint8_t>();
  }
  const uint8_t& nr_sinr_r15() const
  {
    assert_choice_type(types::nr_sinr_r15, type_, "ThresholdNR-r15");
    return c.get<uint8_t>();
  }
  uint8_t& set_nr_rsrp_r15();
  uint8_t& set_nr_rsrq_r15();
  uint8_t& set_nr_sinr_r15();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// TimeToTrigger ::= ENUMERATED
struct time_to_trigger_opts {
  enum options {
    ms0,
    ms40,
    ms64,
    ms80,
    ms100,
    ms128,
    ms160,
    ms256,
    ms320,
    ms480,
    ms512,
    ms640,
    ms1024,
    ms1280,
    ms2560,
    ms5120,
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<time_to_trigger_opts> time_to_trigger_e;

// WLAN-ChannelList-r13 ::= SEQUENCE (SIZE (1..16)) OF INTEGER (0..255)
using wlan_ch_list_r13_l = bounded_array<uint16_t, 16>;

// WLAN-NameList-r15 ::= SEQUENCE (SIZE (1..4)) OF OCTET STRING (SIZE (1..32))
using wlan_name_list_r15_l = bounded_array<bounded_octstring<1, 32>, 4>;

// AllowedCellsToAddModList-r13 ::= SEQUENCE (SIZE (1..32)) OF AllowedCellsToAddMod-r13
using allowed_cells_to_add_mod_list_r13_l = dyn_array<allowed_cells_to_add_mod_r13_s>;

// AltTTT-CellsToAddModList-r12 ::= SEQUENCE (SIZE (1..32)) OF AltTTT-CellsToAddMod-r12
using alt_ttt_cells_to_add_mod_list_r12_l = dyn_array<alt_ttt_cells_to_add_mod_r12_s>;

// BT-NameListConfig-r15 ::= CHOICE
struct bt_name_list_cfg_r15_c {
  using types = setup_e;

  // choice methods
  bt_name_list_cfg_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const bt_name_list_cfg_r15_c& other) const;
  bool        operator!=(const bt_name_list_cfg_r15_c& other) const { return not(*this == other); }
  // getters
  bt_name_list_r15_l& setup()
  {
    assert_choice_type(types::setup, type_, "BT-NameListConfig-r15");
    return c;
  }
  const bt_name_list_r15_l& setup() const
  {
    assert_choice_type(types::setup, type_, "BT-NameListConfig-r15");
    return c;
  }
  void                set_release();
  bt_name_list_r15_l& set_setup();

private:
  types              type_;
  bt_name_list_r15_l c;
};

// CDMA2000-Type ::= ENUMERATED
struct cdma2000_type_opts {
  enum options { type1_xrtt, type_hrpd, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<cdma2000_type_opts> cdma2000_type_e;

// CSG-AllowedReportingCells-r9 ::= SEQUENCE
struct csg_allowed_report_cells_r9_s {
  bool                         pci_range_utra_fdd_list_r9_present = false;
  pci_range_utra_fdd_list_r9_l pci_range_utra_fdd_list_r9;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const csg_allowed_report_cells_r9_s& other) const;
  bool        operator!=(const csg_allowed_report_cells_r9_s& other) const { return not(*this == other); }
};

// CellIndexList ::= SEQUENCE (SIZE (1..32)) OF INTEGER (1..32)
using cell_idx_list_l = bounded_array<uint8_t, 32>;

// CellsToAddModList ::= SEQUENCE (SIZE (1..32)) OF CellsToAddMod
using cells_to_add_mod_list_l = dyn_array<cells_to_add_mod_s>;

// CellsToAddModListCDMA2000 ::= SEQUENCE (SIZE (1..32)) OF CellsToAddModCDMA2000
using cells_to_add_mod_list_cdma2000_l = dyn_array<cells_to_add_mod_cdma2000_s>;

// CellsToAddModListNR-r15 ::= SEQUENCE (SIZE (1..32)) OF CellsToAddModNR-r15
using cells_to_add_mod_list_nr_r15_l = dyn_array<cells_to_add_mod_nr_r15_s>;

// CellsToAddModListNR-r16 ::= SEQUENCE (SIZE (1..32)) OF CellsToAddModNR-r16
using cells_to_add_mod_list_nr_r16_l = dyn_array<cells_to_add_mod_nr_r16_s>;

// CellsToAddModListUTRA-FDD ::= SEQUENCE (SIZE (1..32)) OF CellsToAddModUTRA-FDD
using cells_to_add_mod_list_utra_fdd_l = dyn_array<cells_to_add_mod_utra_fdd_s>;

// CellsToAddModListUTRA-TDD ::= SEQUENCE (SIZE (1..32)) OF CellsToAddModUTRA-TDD
using cells_to_add_mod_list_utra_tdd_l = dyn_array<cells_to_add_mod_utra_tdd_s>;

// CondReconfigurationTriggerEUTRA-r16 ::= SEQUENCE
struct cond_recfg_trigger_eutra_r16_s {
  struct cond_event_id_r16_c_ {
    struct cond_event_a3_r16_s_ {
      int8_t            a3_offset_r16  = -30;
      uint8_t           hysteresis_r16 = 0;
      time_to_trigger_e time_to_trigger_r16;
    };
    struct cond_event_a5_r16_s_ {
      thres_eutra_c     a5_thres1_r16;
      thres_eutra_c     a5_thres2_r16;
      uint8_t           hysteresis_r16 = 0;
      time_to_trigger_e time_to_trigger_r16;
    };
    struct types_opts {
      enum options { cond_event_a3_r16, cond_event_a5_r16, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts, true> types;

    // choice methods
    cond_event_id_r16_c_() = default;
    cond_event_id_r16_c_(const cond_event_id_r16_c_& other);
    cond_event_id_r16_c_& operator=(const cond_event_id_r16_c_& other);
    ~cond_event_id_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const cond_event_id_r16_c_& other) const;
    bool        operator!=(const cond_event_id_r16_c_& other) const { return not(*this == other); }
    // getters
    cond_event_a3_r16_s_& cond_event_a3_r16()
    {
      assert_choice_type(types::cond_event_a3_r16, type_, "condEventId-r16");
      return c.get<cond_event_a3_r16_s_>();
    }
    cond_event_a5_r16_s_& cond_event_a5_r16()
    {
      assert_choice_type(types::cond_event_a5_r16, type_, "condEventId-r16");
      return c.get<cond_event_a5_r16_s_>();
    }
    const cond_event_a3_r16_s_& cond_event_a3_r16() const
    {
      assert_choice_type(types::cond_event_a3_r16, type_, "condEventId-r16");
      return c.get<cond_event_a3_r16_s_>();
    }
    const cond_event_a5_r16_s_& cond_event_a5_r16() const
    {
      assert_choice_type(types::cond_event_a5_r16, type_, "condEventId-r16");
      return c.get<cond_event_a5_r16_s_>();
    }
    cond_event_a3_r16_s_& set_cond_event_a3_r16();
    cond_event_a5_r16_s_& set_cond_event_a5_r16();

  private:
    types                                                       type_;
    choice_buffer_t<cond_event_a3_r16_s_, cond_event_a5_r16_s_> c;

    void destroy_();
  };

  // member variables
  cond_event_id_r16_c_ cond_event_id_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cond_recfg_trigger_eutra_r16_s& other) const;
  bool        operator!=(const cond_recfg_trigger_eutra_r16_s& other) const { return not(*this == other); }
};

// CondReconfigurationTriggerNR-r17 ::= SEQUENCE
struct cond_recfg_trigger_nr_r17_s {
  struct cond_event_id_r17_c_ {
    struct cond_event_b1_nr_r17_s_ {
      thres_nr_r15_c    b1_thres_nr_r17;
      uint8_t           hysteresis_r17 = 0;
      time_to_trigger_e time_to_trigger_r17;
    };
    struct types_opts {
      enum options { cond_event_b1_nr_r17, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts, true> types;

    // choice methods
    types       type() const { return types::cond_event_b1_nr_r17; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const cond_event_id_r17_c_& other) const;
    bool        operator!=(const cond_event_id_r17_c_& other) const { return not(*this == other); }
    // getters
    cond_event_b1_nr_r17_s_&       cond_event_b1_nr_r17() { return c; }
    const cond_event_b1_nr_r17_s_& cond_event_b1_nr_r17() const { return c; }

  private:
    cond_event_b1_nr_r17_s_ c;
  };

  // member variables
  cond_event_id_r17_c_ cond_event_id_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const cond_recfg_trigger_nr_r17_s& other) const;
  bool        operator!=(const cond_recfg_trigger_nr_r17_s& other) const { return not(*this == other); }
};

// ExcludedCellsToAddModList ::= SEQUENCE (SIZE (1..32)) OF ExcludedCellsToAddMod
using excluded_cells_to_add_mod_list_l = dyn_array<excluded_cells_to_add_mod_s>;

// MeasCycleSCell-r10 ::= ENUMERATED
struct meas_cycle_scell_r10_opts {
  enum options { sf160, sf256, sf320, sf512, sf640, sf1024, sf1280, spare1, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<meas_cycle_scell_r10_opts> meas_cycle_scell_r10_e;

// MeasDS-Config-r12 ::= CHOICE
struct meas_ds_cfg_r12_c {
  struct setup_s_ {
    struct dmtc_period_offset_r12_c_ {
      struct types_opts {
        enum options { ms40_r12, ms80_r12, ms160_r12, /*...*/ nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<types_opts, true> types;

      // choice methods
      dmtc_period_offset_r12_c_() = default;
      dmtc_period_offset_r12_c_(const dmtc_period_offset_r12_c_& other);
      dmtc_period_offset_r12_c_& operator=(const dmtc_period_offset_r12_c_& other);
      ~dmtc_period_offset_r12_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const dmtc_period_offset_r12_c_& other) const;
      bool        operator!=(const dmtc_period_offset_r12_c_& other) const { return not(*this == other); }
      // getters
      uint8_t& ms40_r12()
      {
        assert_choice_type(types::ms40_r12, type_, "dmtc-PeriodOffset-r12");
        return c.get<uint8_t>();
      }
      uint8_t& ms80_r12()
      {
        assert_choice_type(types::ms80_r12, type_, "dmtc-PeriodOffset-r12");
        return c.get<uint8_t>();
      }
      uint8_t& ms160_r12()
      {
        assert_choice_type(types::ms160_r12, type_, "dmtc-PeriodOffset-r12");
        return c.get<uint8_t>();
      }
      const uint8_t& ms40_r12() const
      {
        assert_choice_type(types::ms40_r12, type_, "dmtc-PeriodOffset-r12");
        return c.get<uint8_t>();
      }
      const uint8_t& ms80_r12() const
      {
        assert_choice_type(types::ms80_r12, type_, "dmtc-PeriodOffset-r12");
        return c.get<uint8_t>();
      }
      const uint8_t& ms160_r12() const
      {
        assert_choice_type(types::ms160_r12, type_, "dmtc-PeriodOffset-r12");
        return c.get<uint8_t>();
      }
      uint8_t& set_ms40_r12();
      uint8_t& set_ms80_r12();
      uint8_t& set_ms160_r12();

    private:
      types               type_;
      pod_choice_buffer_t c;

      void destroy_();
    };
    struct ds_occasion_dur_r12_c_ {
      struct types_opts {
        enum options { dur_fdd_r12, dur_tdd_r12, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      ds_occasion_dur_r12_c_() = default;
      ds_occasion_dur_r12_c_(const ds_occasion_dur_r12_c_& other);
      ds_occasion_dur_r12_c_& operator=(const ds_occasion_dur_r12_c_& other);
      ~ds_occasion_dur_r12_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      bool        operator==(const ds_occasion_dur_r12_c_& other) const;
      bool        operator!=(const ds_occasion_dur_r12_c_& other) const { return not(*this == other); }
      // getters
      uint8_t& dur_fdd_r12()
      {
        assert_choice_type(types::dur_fdd_r12, type_, "ds-OccasionDuration-r12");
        return c.get<uint8_t>();
      }
      uint8_t& dur_tdd_r12()
      {
        assert_choice_type(types::dur_tdd_r12, type_, "ds-OccasionDuration-r12");
        return c.get<uint8_t>();
      }
      const uint8_t& dur_fdd_r12() const
      {
        assert_choice_type(types::dur_fdd_r12, type_, "ds-OccasionDuration-r12");
        return c.get<uint8_t>();
      }
      const uint8_t& dur_tdd_r12() const
      {
        assert_choice_type(types::dur_tdd_r12, type_, "ds-OccasionDuration-r12");
        return c.get<uint8_t>();
      }
      uint8_t& set_dur_fdd_r12();
      uint8_t& set_dur_tdd_r12();

    private:
      types               type_;
      pod_choice_buffer_t c;

      void destroy_();
    };

    // member variables
    bool                              ext                                     = false;
    bool                              meas_csi_rs_to_rem_list_r12_present     = false;
    bool                              meas_csi_rs_to_add_mod_list_r12_present = false;
    dmtc_period_offset_r12_c_         dmtc_period_offset_r12;
    ds_occasion_dur_r12_c_            ds_occasion_dur_r12;
    meas_csi_rs_to_rem_list_r12_l     meas_csi_rs_to_rem_list_r12;
    meas_csi_rs_to_add_mod_list_r12_l meas_csi_rs_to_add_mod_list_r12;
    // ...
  };
  using types = setup_e;

  // choice methods
  meas_ds_cfg_r12_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const meas_ds_cfg_r12_c& other) const;
  bool        operator!=(const meas_ds_cfg_r12_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "MeasDS-Config-r12");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "MeasDS-Config-r12");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// MeasGapConfig ::= CHOICE
struct meas_gap_cfg_c {
  struct setup_s_ {
    struct gap_offset_c_ {
      struct types_opts {
        enum options {
          gp0,
          gp1,
          // ...
          gp2_r14,
          gp3_r14,
          gp_ncsg0_r14,
          gp_ncsg1_r14,
          gp_ncsg2_r14,
          gp_ncsg3_r14,
          gp_non_uniform1_r14,
          gp_non_uniform2_r14,
          gp_non_uniform3_r14,
          gp_non_uniform4_r14,
          gp4_r15,
          gp5_r15,
          gp6_r15,
          gp7_r15,
          gp8_r15,
          gp9_r15,
          gp10_r15,
          gp11_r15,
          nulltype
        } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts, true, 18> types;

      // choice methods
      gap_offset_c_() = default;
      gap_offset_c_(const gap_offset_c_& other);
      gap_offset_c_& operator=(const gap_offset_c_& other);
      ~gap_offset_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      uint8_t& gp0()
      {
        assert_choice_type(types::gp0, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      uint8_t& gp1()
      {
        assert_choice_type(types::gp1, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      uint8_t& gp2_r14()
      {
        assert_choice_type(types::gp2_r14, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      uint8_t& gp3_r14()
      {
        assert_choice_type(types::gp3_r14, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      uint8_t& gp_ncsg0_r14()
      {
        assert_choice_type(types::gp_ncsg0_r14, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      uint8_t& gp_ncsg1_r14()
      {
        assert_choice_type(types::gp_ncsg1_r14, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      uint8_t& gp_ncsg2_r14()
      {
        assert_choice_type(types::gp_ncsg2_r14, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      uint8_t& gp_ncsg3_r14()
      {
        assert_choice_type(types::gp_ncsg3_r14, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      uint16_t& gp_non_uniform1_r14()
      {
        assert_choice_type(types::gp_non_uniform1_r14, type_, "gapOffset");
        return c.get<uint16_t>();
      }
      uint16_t& gp_non_uniform2_r14()
      {
        assert_choice_type(types::gp_non_uniform2_r14, type_, "gapOffset");
        return c.get<uint16_t>();
      }
      uint16_t& gp_non_uniform3_r14()
      {
        assert_choice_type(types::gp_non_uniform3_r14, type_, "gapOffset");
        return c.get<uint16_t>();
      }
      uint16_t& gp_non_uniform4_r14()
      {
        assert_choice_type(types::gp_non_uniform4_r14, type_, "gapOffset");
        return c.get<uint16_t>();
      }
      uint8_t& gp4_r15()
      {
        assert_choice_type(types::gp4_r15, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      uint8_t& gp5_r15()
      {
        assert_choice_type(types::gp5_r15, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      uint8_t& gp6_r15()
      {
        assert_choice_type(types::gp6_r15, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      uint8_t& gp7_r15()
      {
        assert_choice_type(types::gp7_r15, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      uint8_t& gp8_r15()
      {
        assert_choice_type(types::gp8_r15, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      uint8_t& gp9_r15()
      {
        assert_choice_type(types::gp9_r15, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      uint8_t& gp10_r15()
      {
        assert_choice_type(types::gp10_r15, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      uint8_t& gp11_r15()
      {
        assert_choice_type(types::gp11_r15, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& gp0() const
      {
        assert_choice_type(types::gp0, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& gp1() const
      {
        assert_choice_type(types::gp1, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& gp2_r14() const
      {
        assert_choice_type(types::gp2_r14, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& gp3_r14() const
      {
        assert_choice_type(types::gp3_r14, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& gp_ncsg0_r14() const
      {
        assert_choice_type(types::gp_ncsg0_r14, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& gp_ncsg1_r14() const
      {
        assert_choice_type(types::gp_ncsg1_r14, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& gp_ncsg2_r14() const
      {
        assert_choice_type(types::gp_ncsg2_r14, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& gp_ncsg3_r14() const
      {
        assert_choice_type(types::gp_ncsg3_r14, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      const uint16_t& gp_non_uniform1_r14() const
      {
        assert_choice_type(types::gp_non_uniform1_r14, type_, "gapOffset");
        return c.get<uint16_t>();
      }
      const uint16_t& gp_non_uniform2_r14() const
      {
        assert_choice_type(types::gp_non_uniform2_r14, type_, "gapOffset");
        return c.get<uint16_t>();
      }
      const uint16_t& gp_non_uniform3_r14() const
      {
        assert_choice_type(types::gp_non_uniform3_r14, type_, "gapOffset");
        return c.get<uint16_t>();
      }
      const uint16_t& gp_non_uniform4_r14() const
      {
        assert_choice_type(types::gp_non_uniform4_r14, type_, "gapOffset");
        return c.get<uint16_t>();
      }
      const uint8_t& gp4_r15() const
      {
        assert_choice_type(types::gp4_r15, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& gp5_r15() const
      {
        assert_choice_type(types::gp5_r15, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& gp6_r15() const
      {
        assert_choice_type(types::gp6_r15, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& gp7_r15() const
      {
        assert_choice_type(types::gp7_r15, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& gp8_r15() const
      {
        assert_choice_type(types::gp8_r15, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& gp9_r15() const
      {
        assert_choice_type(types::gp9_r15, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& gp10_r15() const
      {
        assert_choice_type(types::gp10_r15, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      const uint8_t& gp11_r15() const
      {
        assert_choice_type(types::gp11_r15, type_, "gapOffset");
        return c.get<uint8_t>();
      }
      uint8_t&  set_gp0();
      uint8_t&  set_gp1();
      uint8_t&  set_gp2_r14();
      uint8_t&  set_gp3_r14();
      uint8_t&  set_gp_ncsg0_r14();
      uint8_t&  set_gp_ncsg1_r14();
      uint8_t&  set_gp_ncsg2_r14();
      uint8_t&  set_gp_ncsg3_r14();
      uint16_t& set_gp_non_uniform1_r14();
      uint16_t& set_gp_non_uniform2_r14();
      uint16_t& set_gp_non_uniform3_r14();
      uint16_t& set_gp_non_uniform4_r14();
      uint8_t&  set_gp4_r15();
      uint8_t&  set_gp5_r15();
      uint8_t&  set_gp6_r15();
      uint8_t&  set_gp7_r15();
      uint8_t&  set_gp8_r15();
      uint8_t&  set_gp9_r15();
      uint8_t&  set_gp10_r15();
      uint8_t&  set_gp11_r15();

    private:
      types               type_;
      pod_choice_buffer_t c;

      void destroy_();
    };

    // member variables
    gap_offset_c_ gap_offset;
  };
  using types = setup_e;

  // choice methods
  meas_gap_cfg_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "MeasGapConfig");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "MeasGapConfig");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// MeasRSS-DedicatedConfig-r16 ::= SEQUENCE
struct meas_rss_ded_cfg_r16_s {
  bool                          rss_cfg_carrier_info_r16_present    = false;
  bool                          cells_to_add_mod_list_v1610_present = false;
  rss_cfg_carrier_info_r16_s    rss_cfg_carrier_info_r16;
  cells_to_add_mod_list_v1610_l cells_to_add_mod_list_v1610;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const meas_rss_ded_cfg_r16_s& other) const;
  bool        operator!=(const meas_rss_ded_cfg_r16_s& other) const { return not(*this == other); }
};

// MeasRSSI-ReportConfig-r13 ::= SEQUENCE
struct meas_rssi_report_cfg_r13_s {
  bool    ch_occupancy_thres_r13_present = false;
  uint8_t ch_occupancy_thres_r13         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const meas_rssi_report_cfg_r13_s& other) const;
  bool        operator!=(const meas_rssi_report_cfg_r13_s& other) const { return not(*this == other); }
};

// MeasSensing-Config-r15 ::= SEQUENCE
struct meas_sensing_cfg_r15_s {
  struct sensing_periodicity_r15_opts {
    enum options { ms20, ms50, ms100, ms200, ms300, ms400, ms500, ms600, ms700, ms800, ms900, ms1000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<sensing_periodicity_r15_opts> sensing_periodicity_r15_e_;

  // member variables
  uint8_t                    sensing_subch_num_r15 = 1;
  sensing_periodicity_r15_e_ sensing_periodicity_r15;
  uint8_t                    sensing_resel_counter_r15 = 5;
  uint8_t                    sensing_prio_r15          = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const meas_sensing_cfg_r15_s& other) const;
  bool        operator!=(const meas_sensing_cfg_r15_s& other) const { return not(*this == other); }
};

// MeasSubframePatternConfigNeigh-r10 ::= CHOICE
struct meas_sf_pattern_cfg_neigh_r10_c {
  struct setup_s_ {
    bool                    meas_sf_cell_list_r10_present = false;
    meas_sf_pattern_r10_c   meas_sf_pattern_neigh_r10;
    meas_sf_cell_list_r10_l meas_sf_cell_list_r10;
  };
  using types = setup_e;

  // choice methods
  meas_sf_pattern_cfg_neigh_r10_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const meas_sf_pattern_cfg_neigh_r10_c& other) const;
  bool        operator!=(const meas_sf_pattern_cfg_neigh_r10_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "MeasSubframePatternConfigNeigh-r10");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "MeasSubframePatternConfigNeigh-r10");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// PhysCellIdGERAN ::= SEQUENCE
struct pci_geran_s {
  fixed_bitstring<3> network_colour_code;
  fixed_bitstring<3> base_station_colour_code;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const pci_geran_s& other) const;
  bool        operator!=(const pci_geran_s& other) const { return not(*this == other); }
};

// QuantityConfigRS-NR-r15 ::= SEQUENCE
struct quant_cfg_rs_nr_r15_s {
  bool        filt_coeff_rsrp_r15_present = false;
  bool        filt_coeff_rsrq_r15_present = false;
  bool        filt_coef_sinr_r13_present  = false;
  filt_coef_e filt_coeff_rsrp_r15;
  filt_coef_e filt_coeff_rsrq_r15;
  filt_coef_e filt_coef_sinr_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const quant_cfg_rs_nr_r15_s& other) const;
  bool        operator!=(const quant_cfg_rs_nr_r15_s& other) const { return not(*this == other); }
};

// RMTC-Config-r13 ::= CHOICE
struct rmtc_cfg_r13_c {
  struct setup_s_ {
    struct rmtc_period_r13_opts {
      enum options { ms40, ms80, ms160, ms320, ms640, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<rmtc_period_r13_opts> rmtc_period_r13_e_;
    struct meas_dur_r13_opts {
      enum options { sym1, sym14, sym28, sym42, sym70, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<meas_dur_r13_opts> meas_dur_r13_e_;

    // member variables
    bool               ext                        = false;
    bool               rmtc_sf_offset_r13_present = false;
    rmtc_period_r13_e_ rmtc_period_r13;
    uint16_t           rmtc_sf_offset_r13 = 0;
    meas_dur_r13_e_    meas_dur_r13;
    // ...
  };
  using types = setup_e;

  // choice methods
  rmtc_cfg_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const rmtc_cfg_r13_c& other) const;
  bool        operator!=(const rmtc_cfg_r13_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "RMTC-Config-r13");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "RMTC-Config-r13");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// RMTC-ConfigNR-r16 ::= SEQUENCE
struct rmtc_cfg_nr_r16_s {
  struct rmtc_periodicity_nr_r16_opts {
    enum options { ms40, ms80, ms160, ms320, ms640, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<rmtc_periodicity_nr_r16_opts> rmtc_periodicity_nr_r16_e_;
  struct meas_dur_nr_r16_opts {
    enum options { sym1, sym14or12, sym28or24, sym42or36, sym70or60, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<meas_dur_nr_r16_opts> meas_dur_nr_r16_e_;
  struct ref_scs_cp_nr_r16_opts {
    enum options { khz15, khz30, khz60_ncp, khz60_ecp, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<ref_scs_cp_nr_r16_opts> ref_scs_cp_nr_r16_e_;
  struct rmtc_bw_nr_r17_opts {
    enum options { mhz100, mhz400, mhz800, mhz1600, mhz2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<rmtc_bw_nr_r17_opts> rmtc_bw_nr_r17_e_;
  struct meas_dur_nr_r17_opts {
    enum options { sym140, sym560, sym1120, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<meas_dur_nr_r17_opts> meas_dur_nr_r17_e_;
  struct ref_scs_cp_nr_r17_opts {
    enum options { khz120, khz480, khz960, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<ref_scs_cp_nr_r17_opts> ref_scs_cp_nr_r17_e_;

  // member variables
  bool                       ext                           = false;
  bool                       rmtc_sf_offset_nr_r16_present = false;
  rmtc_periodicity_nr_r16_e_ rmtc_periodicity_nr_r16;
  uint16_t                   rmtc_sf_offset_nr_r16 = 0;
  meas_dur_nr_r16_e_         meas_dur_nr_r16;
  uint32_t                   rmtc_freq_nr_r16 = 0;
  ref_scs_cp_nr_r16_e_       ref_scs_cp_nr_r16;
  // ...
  // group 0
  bool                 rmtc_bw_nr_r17_present    = false;
  bool                 meas_dur_nr_r17_present   = false;
  bool                 ref_scs_cp_nr_r17_present = false;
  rmtc_bw_nr_r17_e_    rmtc_bw_nr_r17;
  meas_dur_nr_r17_e_   meas_dur_nr_r17;
  ref_scs_cp_nr_r17_e_ ref_scs_cp_nr_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const rmtc_cfg_nr_r16_s& other) const;
  bool        operator!=(const rmtc_cfg_nr_r16_s& other) const { return not(*this == other); }
};

// RS-ConfigSSB-NR-r15 ::= SEQUENCE
struct rs_cfg_ssb_nr_r15_s {
  struct subcarrier_spacing_ssb_r15_opts {
    enum options { khz15, khz30, khz120, khz240, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<subcarrier_spacing_ssb_r15_opts> subcarrier_spacing_ssb_r15_e_;
  struct ssb_to_measure_r15_c_ {
    using types = setup_e;

    // choice methods
    ssb_to_measure_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const ssb_to_measure_r15_c_& other) const;
    bool        operator!=(const ssb_to_measure_r15_c_& other) const { return not(*this == other); }
    // getters
    ssb_to_measure_r15_c& setup()
    {
      assert_choice_type(types::setup, type_, "ssb-ToMeasure-r15");
      return c;
    }
    const ssb_to_measure_r15_c& setup() const
    {
      assert_choice_type(types::setup, type_, "ssb-ToMeasure-r15");
      return c;
    }
    void                  set_release();
    ssb_to_measure_r15_c& set_setup();

  private:
    types                type_;
    ssb_to_measure_r15_c c;
  };
  using ssb_position_qcl_cells_to_rem_list_nr_r16_l_ = bounded_array<uint16_t, 32>;
  struct subcarrier_spacing_ssb_r17_opts {
    enum options { khz480, khz960, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<subcarrier_spacing_ssb_r17_opts> subcarrier_spacing_ssb_r17_e_;
  using ssb_position_qcl_cells_to_rem_list_nr_r17_l_ = bounded_array<uint16_t, 32>;

  // member variables
  bool                          ext = false;
  mtc_ssb_nr_r15_s              meas_timing_cfg_r15;
  subcarrier_spacing_ssb_r15_e_ subcarrier_spacing_ssb_r15;
  // ...
  // group 0
  copy_ptr<ssb_to_measure_r15_c_> ssb_to_measure_r15;
  // group 1
  bool                                                      ssb_position_qcl_common_nr_r16_present = false;
  ssb_position_qcl_relation_nr_r16_e                        ssb_position_qcl_common_nr_r16;
  copy_ptr<ssb_position_qcl_cells_to_add_mod_list_nr_r16_l> ssb_position_qcl_cells_to_add_mod_list_nr_r16;
  copy_ptr<ssb_position_qcl_cells_to_rem_list_nr_r16_l_>    ssb_position_qcl_cells_to_rem_list_nr_r16;
  // group 2
  bool                                                      subcarrier_spacing_ssb_r17_present     = false;
  bool                                                      ssb_position_qcl_common_nr_r17_present = false;
  subcarrier_spacing_ssb_r17_e_                             subcarrier_spacing_ssb_r17;
  ssb_position_qcl_relation_nr_r17_e                        ssb_position_qcl_common_nr_r17;
  copy_ptr<ssb_position_qcl_cells_to_add_mod_list_nr_r17_l> ssb_position_qcl_cells_to_add_mod_list_nr_r17;
  copy_ptr<ssb_position_qcl_cells_to_rem_list_nr_r17_l_>    ssb_position_qcl_cells_to_rem_list_nr_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const rs_cfg_ssb_nr_r15_s& other) const;
  bool        operator!=(const rs_cfg_ssb_nr_r15_s& other) const { return not(*this == other); }
};

// RSRQ-RangeConfig-r12 ::= CHOICE
struct rsrq_range_cfg_r12_c {
  using types = setup_e;

  // choice methods
  rsrq_range_cfg_r12_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const rsrq_range_cfg_r12_c& other) const;
  bool        operator!=(const rsrq_range_cfg_r12_c& other) const { return not(*this == other); }
  // getters
  int8_t& setup()
  {
    assert_choice_type(types::setup, type_, "RSRQ-RangeConfig-r12");
    return c;
  }
  const int8_t& setup() const
  {
    assert_choice_type(types::setup, type_, "RSRQ-RangeConfig-r12");
    return c;
  }
  void    set_release();
  int8_t& set_setup();

private:
  types  type_;
  int8_t c;
};

// ReportInterval ::= ENUMERATED
struct report_interv_opts {
  enum options {
    ms120,
    ms240,
    ms480,
    ms640,
    ms1024,
    ms2048,
    ms5120,
    ms10240,
    min1,
    min6,
    min12,
    min30,
    min60,
    spare3,
    spare2,
    spare1,
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<report_interv_opts> report_interv_e;

// ReportQuantityNR-r15 ::= SEQUENCE
struct report_quant_nr_r15_s {
  bool ss_rsrp = false;
  bool ss_rsrq = false;
  bool ss_sinr = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const report_quant_nr_r15_s& other) const;
  bool        operator!=(const report_quant_nr_r15_s& other) const { return not(*this == other); }
};

// ReportQuantityWLAN-r13 ::= SEQUENCE
struct report_quant_wlan_r13_s {
  bool ext                                                   = false;
  bool band_request_wlan_r13_present                         = false;
  bool carrier_info_request_wlan_r13_present                 = false;
  bool available_admission_capacity_request_wlan_r13_present = false;
  bool backhaul_dl_bw_request_wlan_r13_present               = false;
  bool backhaul_ul_bw_request_wlan_r13_present               = false;
  bool ch_utilization_request_wlan_r13_present               = false;
  bool station_count_request_wlan_r13_present                = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const report_quant_wlan_r13_s& other) const;
  bool        operator!=(const report_quant_wlan_r13_s& other) const { return not(*this == other); }
};

// ThresholdUTRA ::= CHOICE
struct thres_utra_c {
  struct types_opts {
    enum options { utra_rscp, utra_ec_n0, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  thres_utra_c() = default;
  thres_utra_c(const thres_utra_c& other);
  thres_utra_c& operator=(const thres_utra_c& other);
  ~thres_utra_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const thres_utra_c& other) const;
  bool        operator!=(const thres_utra_c& other) const { return not(*this == other); }
  // getters
  int8_t& utra_rscp()
  {
    assert_choice_type(types::utra_rscp, type_, "ThresholdUTRA");
    return c.get<int8_t>();
  }
  uint8_t& utra_ec_n0()
  {
    assert_choice_type(types::utra_ec_n0, type_, "ThresholdUTRA");
    return c.get<uint8_t>();
  }
  const int8_t& utra_rscp() const
  {
    assert_choice_type(types::utra_rscp, type_, "ThresholdUTRA");
    return c.get<int8_t>();
  }
  const uint8_t& utra_ec_n0() const
  {
    assert_choice_type(types::utra_ec_n0, type_, "ThresholdUTRA");
    return c.get<uint8_t>();
  }
  int8_t&  set_utra_rscp();
  uint8_t& set_utra_ec_n0();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// Tx-ResourcePoolMeasList-r14 ::= SEQUENCE (SIZE (1..72)) OF INTEGER (1..72)
using tx_res_pool_meas_list_r14_l = dyn_array<uint8_t>;

// UL-DelayConfig-r13 ::= CHOICE
struct ul_delay_cfg_r13_c {
  struct setup_s_ {
    struct delay_thres_r13_opts {
      enum options {
        ms30,
        ms40,
        ms50,
        ms60,
        ms70,
        ms80,
        ms90,
        ms100,
        ms150,
        ms300,
        ms500,
        ms750,
        spare4,
        spare3,
        spare2,
        spare1,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<delay_thres_r13_opts> delay_thres_r13_e_;

    // member variables
    delay_thres_r13_e_ delay_thres_r13;
  };
  using types = setup_e;

  // choice methods
  ul_delay_cfg_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_delay_cfg_r13_c& other) const;
  bool        operator!=(const ul_delay_cfg_r13_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "UL-DelayConfig-r13");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "UL-DelayConfig-r13");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// UL-DelayValueConfig-r16 ::= CHOICE
struct ul_delay_value_cfg_r16_c {
  struct setup_s_ {
    using delay_dr_blist_r16_l_ = bounded_array<uint8_t, 11>;

    // member variables
    delay_dr_blist_r16_l_ delay_dr_blist_r16;
  };
  using types = setup_e;

  // choice methods
  ul_delay_value_cfg_r16_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const ul_delay_value_cfg_r16_c& other) const;
  bool        operator!=(const ul_delay_value_cfg_r16_c& other) const { return not(*this == other); }
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "UL-DelayValueConfig-r16");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "UL-DelayValueConfig-r16");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// WLAN-CarrierInfo-r13 ::= SEQUENCE
struct wlan_carrier_info_r13_s {
  struct country_code_r13_opts {
    enum options { united_states, europe, japan, global, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<country_code_r13_opts, true> country_code_r13_e_;

  // member variables
  bool                ext                         = false;
  bool                operating_class_r13_present = false;
  bool                country_code_r13_present    = false;
  bool                ch_nums_r13_present         = false;
  uint16_t            operating_class_r13         = 0;
  country_code_r13_e_ country_code_r13;
  wlan_ch_list_r13_l  ch_nums_r13;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const wlan_carrier_info_r13_s& other) const;
  bool        operator!=(const wlan_carrier_info_r13_s& other) const { return not(*this == other); }
};

// WLAN-Id-List-r13 ::= SEQUENCE (SIZE (1..32)) OF WLAN-Identifiers-r12
using wlan_id_list_r13_l = dyn_array<wlan_ids_r12_s>;

// WLAN-NameListConfig-r15 ::= CHOICE
struct wlan_name_list_cfg_r15_c {
  using types = setup_e;

  // choice methods
  wlan_name_list_cfg_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const wlan_name_list_cfg_r15_c& other) const;
  bool        operator!=(const wlan_name_list_cfg_r15_c& other) const { return not(*this == other); }
  // getters
  wlan_name_list_r15_l& setup()
  {
    assert_choice_type(types::setup, type_, "WLAN-NameListConfig-r15");
    return c;
  }
  const wlan_name_list_r15_l& setup() const
  {
    assert_choice_type(types::setup, type_, "WLAN-NameListConfig-r15");
    return c;
  }
  void                  set_release();
  wlan_name_list_r15_l& set_setup();

private:
  types                type_;
  wlan_name_list_r15_l c;
};

// MeasGapConfigPerCC-r14 ::= SEQUENCE
struct meas_gap_cfg_per_cc_r14_s {
  uint8_t        serv_cell_id_r14 = 0;
  meas_gap_cfg_c meas_gap_cfg_cc_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectCDMA2000 ::= SEQUENCE
struct meas_obj_cdma2000_s {
  bool                             ext                                  = false;
  bool                             search_win_size_present              = false;
  bool                             offset_freq_present                  = false;
  bool                             cells_to_rem_list_present            = false;
  bool                             cells_to_add_mod_list_present        = false;
  bool                             cell_for_which_to_report_cgi_present = false;
  cdma2000_type_e                  cdma2000_type;
  carrier_freq_cdma2000_s          carrier_freq;
  uint8_t                          search_win_size = 0;
  int8_t                           offset_freq     = -15;
  cell_idx_list_l                  cells_to_rem_list;
  cells_to_add_mod_list_cdma2000_l cells_to_add_mod_list;
  uint16_t                         cell_for_which_to_report_cgi = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const meas_obj_cdma2000_s& other) const;
  bool        operator!=(const meas_obj_cdma2000_s& other) const { return not(*this == other); }
};

// MeasObjectEUTRA ::= SEQUENCE
struct meas_obj_eutra_s {
  struct t312_r12_c_ {
    struct setup_opts {
      enum options { ms0, ms50, ms100, ms200, ms300, ms400, ms500, ms1000, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<setup_opts> setup_e_;
    using types = setup_e;

    // choice methods
    t312_r12_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const t312_r12_c_& other) const;
    bool        operator!=(const t312_r12_c_& other) const { return not(*this == other); }
    // getters
    setup_e_& setup()
    {
      assert_choice_type(types::setup, type_, "t312-r12");
      return c;
    }
    const setup_e_& setup() const
    {
      assert_choice_type(types::setup, type_, "t312-r12");
      return c;
    }
    void      set_release();
    setup_e_& set_setup();

  private:
    types    type_;
    setup_e_ c;
  };

  // member variables
  bool                             ext                                    = false;
  bool                             offset_freq_present                    = false;
  bool                             cells_to_rem_list_present              = false;
  bool                             cells_to_add_mod_list_present          = false;
  bool                             excluded_cells_to_rem_list_present     = false;
  bool                             excluded_cells_to_add_mod_list_present = false;
  bool                             cell_for_which_to_report_cgi_present   = false;
  uint32_t                         carrier_freq                           = 0;
  allowed_meas_bw_e                allowed_meas_bw;
  bool                             presence_ant_port1 = false;
  fixed_bitstring<2>               neigh_cell_cfg;
  q_offset_range_e                 offset_freq;
  cell_idx_list_l                  cells_to_rem_list;
  cells_to_add_mod_list_l          cells_to_add_mod_list;
  cell_idx_list_l                  excluded_cells_to_rem_list;
  excluded_cells_to_add_mod_list_l excluded_cells_to_add_mod_list;
  uint16_t                         cell_for_which_to_report_cgi = 0;
  // ...
  // group 0
  bool                                      meas_cycle_scell_r10_present = false;
  meas_cycle_scell_r10_e                    meas_cycle_scell_r10;
  copy_ptr<meas_sf_pattern_cfg_neigh_r10_c> meas_sf_pattern_cfg_neigh_r10;
  // group 1
  bool wideband_rsrq_meas_r11_present = false;
  bool wideband_rsrq_meas_r11         = false;
  // group 2
  bool                                          reduced_meas_performance_r12_present = false;
  copy_ptr<cell_idx_list_l>                     alt_ttt_cells_to_rem_list_r12;
  copy_ptr<alt_ttt_cells_to_add_mod_list_r12_l> alt_ttt_cells_to_add_mod_list_r12;
  copy_ptr<t312_r12_c_>                         t312_r12;
  bool                                          reduced_meas_performance_r12 = false;
  copy_ptr<meas_ds_cfg_r12_c>                   meas_ds_cfg_r12;
  // group 3
  bool                                          carrier_freq_r13_present = false;
  copy_ptr<cell_idx_list_l>                     allowed_cells_to_rem_list_r13;
  copy_ptr<allowed_cells_to_add_mod_list_r13_l> allowed_cells_to_add_mod_list_r13;
  copy_ptr<rmtc_cfg_r13_c>                      rmtc_cfg_r13;
  uint32_t                                      carrier_freq_r13 = 65536;
  // group 4
  bool                                  fembms_mixed_carrier_r14_present = false;
  copy_ptr<tx_res_pool_meas_list_r14_l> tx_res_pool_to_rem_list_r14;
  copy_ptr<tx_res_pool_meas_list_r14_l> tx_res_pool_to_add_list_r14;
  bool                                  fembms_mixed_carrier_r14 = false;
  // group 5
  copy_ptr<meas_sensing_cfg_r15_s> meas_sensing_cfg_r15;
  // group 6
  copy_ptr<setup_release_c<meas_rss_ded_cfg_r16_s> > meas_rss_ded_cfg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const meas_obj_eutra_s& other) const;
  bool        operator!=(const meas_obj_eutra_s& other) const { return not(*this == other); }
};

// MeasObjectEUTRA-v9e0 ::= SEQUENCE
struct meas_obj_eutra_v9e0_s {
  uint32_t carrier_freq_v9e0 = 65536;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectGERAN ::= SEQUENCE
struct meas_obj_geran_s {
  bool                  ext                                  = false;
  bool                  offset_freq_present                  = false;
  bool                  ncc_permitted_present                = false;
  bool                  cell_for_which_to_report_cgi_present = false;
  carrier_freqs_geran_s carrier_freqs;
  int8_t                offset_freq = -15;
  fixed_bitstring<8>    ncc_permitted;
  pci_geran_s           cell_for_which_to_report_cgi;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const meas_obj_geran_s& other) const;
  bool        operator!=(const meas_obj_geran_s& other) const { return not(*this == other); }
};

// MeasObjectNR-r15 ::= SEQUENCE
struct meas_obj_nr_r15_s {
  using cells_for_which_to_report_sftd_r15_l_ = bounded_array<uint16_t, 3>;
  struct band_nr_r15_c_ {
    using types = setup_e;

    // choice methods
    band_nr_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const band_nr_r15_c_& other) const;
    bool        operator!=(const band_nr_r15_c_& other) const { return not(*this == other); }
    // getters
    uint16_t& setup()
    {
      assert_choice_type(types::setup, type_, "bandNR-r15");
      return c;
    }
    const uint16_t& setup() const
    {
      assert_choice_type(types::setup, type_, "bandNR-r15");
      return c;
    }
    void      set_release();
    uint16_t& set_setup();

  private:
    types    type_;
    uint16_t c;
  };

  // member variables
  bool                                  ext                                        = false;
  bool                                  thresh_rs_idx_r15_present                  = false;
  bool                                  max_rs_idx_cell_qual_r15_present           = false;
  bool                                  offset_freq_r15_present                    = false;
  bool                                  excluded_cells_to_rem_list_r15_present     = false;
  bool                                  excluded_cells_to_add_mod_list_r15_present = false;
  bool                                  cells_for_which_to_report_sftd_r15_present = false;
  uint32_t                              carrier_freq_r15                           = 0;
  rs_cfg_ssb_nr_r15_s                   rs_cfg_ssb_r15;
  thres_list_nr_r15_s                   thresh_rs_idx_r15;
  uint8_t                               max_rs_idx_cell_qual_r15 = 1;
  int8_t                                offset_freq_r15          = -15;
  cell_idx_list_l                       excluded_cells_to_rem_list_r15;
  cells_to_add_mod_list_nr_r15_l        excluded_cells_to_add_mod_list_r15;
  uint8_t                               quant_cfg_set_r15 = 1;
  cells_for_which_to_report_sftd_r15_l_ cells_for_which_to_report_sftd_r15;
  // ...
  // group 0
  bool                         cell_for_which_to_report_cgi_r15_present = false;
  bool                         derive_ssb_idx_from_cell_r15_present     = false;
  uint16_t                     cell_for_which_to_report_cgi_r15         = 0;
  bool                         derive_ssb_idx_from_cell_r15             = false;
  copy_ptr<ss_rssi_meas_r15_s> ss_rssi_meas_r15;
  copy_ptr<band_nr_r15_c_>     band_nr_r15;
  // group 1
  copy_ptr<setup_release_c<rmtc_cfg_nr_r16_s> > rmtc_cfg_nr_r16;
  // group 2
  copy_ptr<cell_idx_list_l>                cells_to_rem_list_r16;
  copy_ptr<cells_to_add_mod_list_nr_r16_l> cells_to_add_mod_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const meas_obj_nr_r15_s& other) const;
  bool        operator!=(const meas_obj_nr_r15_s& other) const { return not(*this == other); }
};

// MeasObjectUTRA ::= SEQUENCE
struct meas_obj_utra_s {
  struct cells_to_add_mod_list_c_ {
    struct types_opts {
      enum options { cells_to_add_mod_list_utra_fdd, cells_to_add_mod_list_utra_tdd, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    cells_to_add_mod_list_c_() = default;
    cells_to_add_mod_list_c_(const cells_to_add_mod_list_c_& other);
    cells_to_add_mod_list_c_& operator=(const cells_to_add_mod_list_c_& other);
    ~cells_to_add_mod_list_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const cells_to_add_mod_list_c_& other) const;
    bool        operator!=(const cells_to_add_mod_list_c_& other) const { return not(*this == other); }
    // getters
    cells_to_add_mod_list_utra_fdd_l& cells_to_add_mod_list_utra_fdd()
    {
      assert_choice_type(types::cells_to_add_mod_list_utra_fdd, type_, "cellsToAddModList");
      return c.get<cells_to_add_mod_list_utra_fdd_l>();
    }
    cells_to_add_mod_list_utra_tdd_l& cells_to_add_mod_list_utra_tdd()
    {
      assert_choice_type(types::cells_to_add_mod_list_utra_tdd, type_, "cellsToAddModList");
      return c.get<cells_to_add_mod_list_utra_tdd_l>();
    }
    const cells_to_add_mod_list_utra_fdd_l& cells_to_add_mod_list_utra_fdd() const
    {
      assert_choice_type(types::cells_to_add_mod_list_utra_fdd, type_, "cellsToAddModList");
      return c.get<cells_to_add_mod_list_utra_fdd_l>();
    }
    const cells_to_add_mod_list_utra_tdd_l& cells_to_add_mod_list_utra_tdd() const
    {
      assert_choice_type(types::cells_to_add_mod_list_utra_tdd, type_, "cellsToAddModList");
      return c.get<cells_to_add_mod_list_utra_tdd_l>();
    }
    cells_to_add_mod_list_utra_fdd_l& set_cells_to_add_mod_list_utra_fdd();
    cells_to_add_mod_list_utra_tdd_l& set_cells_to_add_mod_list_utra_tdd();

  private:
    types                                                                               type_;
    choice_buffer_t<cells_to_add_mod_list_utra_fdd_l, cells_to_add_mod_list_utra_tdd_l> c;

    void destroy_();
  };
  struct cell_for_which_to_report_cgi_c_ {
    struct types_opts {
      enum options { utra_fdd, utra_tdd, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    cell_for_which_to_report_cgi_c_() = default;
    cell_for_which_to_report_cgi_c_(const cell_for_which_to_report_cgi_c_& other);
    cell_for_which_to_report_cgi_c_& operator=(const cell_for_which_to_report_cgi_c_& other);
    ~cell_for_which_to_report_cgi_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const cell_for_which_to_report_cgi_c_& other) const;
    bool        operator!=(const cell_for_which_to_report_cgi_c_& other) const { return not(*this == other); }
    // getters
    uint16_t& utra_fdd()
    {
      assert_choice_type(types::utra_fdd, type_, "cellForWhichToReportCGI");
      return c.get<uint16_t>();
    }
    uint8_t& utra_tdd()
    {
      assert_choice_type(types::utra_tdd, type_, "cellForWhichToReportCGI");
      return c.get<uint8_t>();
    }
    const uint16_t& utra_fdd() const
    {
      assert_choice_type(types::utra_fdd, type_, "cellForWhichToReportCGI");
      return c.get<uint16_t>();
    }
    const uint8_t& utra_tdd() const
    {
      assert_choice_type(types::utra_tdd, type_, "cellForWhichToReportCGI");
      return c.get<uint8_t>();
    }
    uint16_t& set_utra_fdd();
    uint8_t&  set_utra_tdd();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                            ext                                  = false;
  bool                            offset_freq_present                  = false;
  bool                            cells_to_rem_list_present            = false;
  bool                            cells_to_add_mod_list_present        = false;
  bool                            cell_for_which_to_report_cgi_present = false;
  uint16_t                        carrier_freq                         = 0;
  int8_t                          offset_freq                          = -15;
  cell_idx_list_l                 cells_to_rem_list;
  cells_to_add_mod_list_c_        cells_to_add_mod_list;
  cell_for_which_to_report_cgi_c_ cell_for_which_to_report_cgi;
  // ...
  // group 0
  copy_ptr<csg_allowed_report_cells_r9_s> csg_allowed_report_cells_v930;
  // group 1
  bool reduced_meas_performance_r12_present = false;
  bool reduced_meas_performance_r12         = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const meas_obj_utra_s& other) const;
  bool        operator!=(const meas_obj_utra_s& other) const { return not(*this == other); }
};

// MeasObjectWLAN-r13 ::= SEQUENCE
struct meas_obj_wlan_r13_s {
  struct carrier_freq_r13_c_ {
    using band_ind_list_wlan_r13_l_     = bounded_array<wlan_band_ind_r13_e, 8>;
    using carrier_info_list_wlan_r13_l_ = dyn_array<wlan_carrier_info_r13_s>;
    struct types_opts {
      enum options { band_ind_list_wlan_r13, carrier_info_list_wlan_r13, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    carrier_freq_r13_c_() = default;
    carrier_freq_r13_c_(const carrier_freq_r13_c_& other);
    carrier_freq_r13_c_& operator=(const carrier_freq_r13_c_& other);
    ~carrier_freq_r13_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const carrier_freq_r13_c_& other) const;
    bool        operator!=(const carrier_freq_r13_c_& other) const { return not(*this == other); }
    // getters
    band_ind_list_wlan_r13_l_& band_ind_list_wlan_r13()
    {
      assert_choice_type(types::band_ind_list_wlan_r13, type_, "carrierFreq-r13");
      return c.get<band_ind_list_wlan_r13_l_>();
    }
    carrier_info_list_wlan_r13_l_& carrier_info_list_wlan_r13()
    {
      assert_choice_type(types::carrier_info_list_wlan_r13, type_, "carrierFreq-r13");
      return c.get<carrier_info_list_wlan_r13_l_>();
    }
    const band_ind_list_wlan_r13_l_& band_ind_list_wlan_r13() const
    {
      assert_choice_type(types::band_ind_list_wlan_r13, type_, "carrierFreq-r13");
      return c.get<band_ind_list_wlan_r13_l_>();
    }
    const carrier_info_list_wlan_r13_l_& carrier_info_list_wlan_r13() const
    {
      assert_choice_type(types::carrier_info_list_wlan_r13, type_, "carrierFreq-r13");
      return c.get<carrier_info_list_wlan_r13_l_>();
    }
    band_ind_list_wlan_r13_l_&     set_band_ind_list_wlan_r13();
    carrier_info_list_wlan_r13_l_& set_carrier_info_list_wlan_r13();

  private:
    types                                                                     type_;
    choice_buffer_t<band_ind_list_wlan_r13_l_, carrier_info_list_wlan_r13_l_> c;

    void destroy_();
  };

  // member variables
  bool                ext                              = false;
  bool                carrier_freq_r13_present         = false;
  bool                wlan_to_add_mod_list_r13_present = false;
  bool                wlan_to_rem_list_r13_present     = false;
  carrier_freq_r13_c_ carrier_freq_r13;
  wlan_id_list_r13_l  wlan_to_add_mod_list_r13;
  wlan_id_list_r13_l  wlan_to_rem_list_r13;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const meas_obj_wlan_r13_s& other) const;
  bool        operator!=(const meas_obj_wlan_r13_s& other) const { return not(*this == other); }
};

// QuantityConfigNR-r15 ::= SEQUENCE
struct quant_cfg_nr_r15_s {
  bool                  meas_quant_rs_idx_nr_r15_present = false;
  quant_cfg_rs_nr_r15_s meas_quant_cell_nr_r15;
  quant_cfg_rs_nr_r15_s meas_quant_rs_idx_nr_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const quant_cfg_nr_r15_s& other) const;
  bool        operator!=(const quant_cfg_nr_r15_s& other) const { return not(*this == other); }
};

struct eutra_event_s {
  struct event_id_c_ {
    struct event_a1_s_ {
      thres_eutra_c a1_thres;
    };
    struct event_a2_s_ {
      thres_eutra_c a2_thres;
    };
    struct event_a3_s_ {
      int8_t a3_offset       = -30;
      bool   report_on_leave = false;
    };
    struct event_a4_s_ {
      thres_eutra_c a4_thres;
    };
    struct event_a5_s_ {
      thres_eutra_c a5_thres1;
      thres_eutra_c a5_thres2;
    };
    struct event_a6_r10_s_ {
      int8_t a6_offset_r10          = -30;
      bool   a6_report_on_leave_r10 = false;
    };
    struct event_c1_r12_s_ {
      uint8_t c1_thres_r12           = 0;
      bool    c1_report_on_leave_r12 = false;
    };
    struct event_c2_r12_s_ {
      uint8_t c2_ref_csi_rs_r12      = 1;
      int8_t  c2_offset_r12          = -30;
      bool    c2_report_on_leave_r12 = false;
    };
    struct event_v1_r14_s_ {
      uint8_t v1_thres_r14 = 0;
    };
    struct event_v2_r14_s_ {
      uint8_t v2_thres_r14 = 0;
    };
    struct event_h1_r15_s_ {
      uint16_t h1_thres_offset_r15 = 0;
      uint8_t  h1_hysteresis_r15   = 1;
    };
    struct event_h2_r15_s_ {
      uint16_t h2_thres_offset_r15 = 0;
      uint8_t  h2_hysteresis_r15   = 1;
    };
    struct types_opts {
      enum options {
        event_a1,
        event_a2,
        event_a3,
        event_a4,
        event_a5,
        // ...
        event_a6_r10,
        event_c1_r12,
        event_c2_r12,
        event_v1_r14,
        event_v2_r14,
        event_h1_r15,
        event_h2_r15,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts, true, 7> types;

    // choice methods
    event_id_c_() = default;
    event_id_c_(const event_id_c_& other);
    event_id_c_& operator=(const event_id_c_& other);
    ~event_id_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const event_id_c_& other) const;
    bool        operator!=(const event_id_c_& other) const { return not(*this == other); }
    // getters
    event_a1_s_& event_a1()
    {
      assert_choice_type(types::event_a1, type_, "eventId");
      return c.get<event_a1_s_>();
    }
    event_a2_s_& event_a2()
    {
      assert_choice_type(types::event_a2, type_, "eventId");
      return c.get<event_a2_s_>();
    }
    event_a3_s_& event_a3()
    {
      assert_choice_type(types::event_a3, type_, "eventId");
      return c.get<event_a3_s_>();
    }
    event_a4_s_& event_a4()
    {
      assert_choice_type(types::event_a4, type_, "eventId");
      return c.get<event_a4_s_>();
    }
    event_a5_s_& event_a5()
    {
      assert_choice_type(types::event_a5, type_, "eventId");
      return c.get<event_a5_s_>();
    }
    event_a6_r10_s_& event_a6_r10()
    {
      assert_choice_type(types::event_a6_r10, type_, "eventId");
      return c.get<event_a6_r10_s_>();
    }
    event_c1_r12_s_& event_c1_r12()
    {
      assert_choice_type(types::event_c1_r12, type_, "eventId");
      return c.get<event_c1_r12_s_>();
    }
    event_c2_r12_s_& event_c2_r12()
    {
      assert_choice_type(types::event_c2_r12, type_, "eventId");
      return c.get<event_c2_r12_s_>();
    }
    event_v1_r14_s_& event_v1_r14()
    {
      assert_choice_type(types::event_v1_r14, type_, "eventId");
      return c.get<event_v1_r14_s_>();
    }
    event_v2_r14_s_& event_v2_r14()
    {
      assert_choice_type(types::event_v2_r14, type_, "eventId");
      return c.get<event_v2_r14_s_>();
    }
    event_h1_r15_s_& event_h1_r15()
    {
      assert_choice_type(types::event_h1_r15, type_, "eventId");
      return c.get<event_h1_r15_s_>();
    }
    event_h2_r15_s_& event_h2_r15()
    {
      assert_choice_type(types::event_h2_r15, type_, "eventId");
      return c.get<event_h2_r15_s_>();
    }
    const event_a1_s_& event_a1() const
    {
      assert_choice_type(types::event_a1, type_, "eventId");
      return c.get<event_a1_s_>();
    }
    const event_a2_s_& event_a2() const
    {
      assert_choice_type(types::event_a2, type_, "eventId");
      return c.get<event_a2_s_>();
    }
    const event_a3_s_& event_a3() const
    {
      assert_choice_type(types::event_a3, type_, "eventId");
      return c.get<event_a3_s_>();
    }
    const event_a4_s_& event_a4() const
    {
      assert_choice_type(types::event_a4, type_, "eventId");
      return c.get<event_a4_s_>();
    }
    const event_a5_s_& event_a5() const
    {
      assert_choice_type(types::event_a5, type_, "eventId");
      return c.get<event_a5_s_>();
    }
    const event_a6_r10_s_& event_a6_r10() const
    {
      assert_choice_type(types::event_a6_r10, type_, "eventId");
      return c.get<event_a6_r10_s_>();
    }
    const event_c1_r12_s_& event_c1_r12() const
    {
      assert_choice_type(types::event_c1_r12, type_, "eventId");
      return c.get<event_c1_r12_s_>();
    }
    const event_c2_r12_s_& event_c2_r12() const
    {
      assert_choice_type(types::event_c2_r12, type_, "eventId");
      return c.get<event_c2_r12_s_>();
    }
    const event_v1_r14_s_& event_v1_r14() const
    {
      assert_choice_type(types::event_v1_r14, type_, "eventId");
      return c.get<event_v1_r14_s_>();
    }
    const event_v2_r14_s_& event_v2_r14() const
    {
      assert_choice_type(types::event_v2_r14, type_, "eventId");
      return c.get<event_v2_r14_s_>();
    }
    const event_h1_r15_s_& event_h1_r15() const
    {
      assert_choice_type(types::event_h1_r15, type_, "eventId");
      return c.get<event_h1_r15_s_>();
    }
    const event_h2_r15_s_& event_h2_r15() const
    {
      assert_choice_type(types::event_h2_r15, type_, "eventId");
      return c.get<event_h2_r15_s_>();
    }
    event_a1_s_&     set_event_a1();
    event_a2_s_&     set_event_a2();
    event_a3_s_&     set_event_a3();
    event_a4_s_&     set_event_a4();
    event_a5_s_&     set_event_a5();
    event_a6_r10_s_& set_event_a6_r10();
    event_c1_r12_s_& set_event_c1_r12();
    event_c2_r12_s_& set_event_c2_r12();
    event_v1_r14_s_& set_event_v1_r14();
    event_v2_r14_s_& set_event_v2_r14();
    event_h1_r15_s_& set_event_h1_r15();
    event_h2_r15_s_& set_event_h2_r15();

  private:
    types type_;
    choice_buffer_t<event_a1_s_,
                    event_a2_s_,
                    event_a3_s_,
                    event_a4_s_,
                    event_a5_s_,
                    event_a6_r10_s_,
                    event_c1_r12_s_,
                    event_c2_r12_s_,
                    event_h1_r15_s_,
                    event_h2_r15_s_,
                    event_v1_r14_s_,
                    event_v2_r14_s_>
        c;

    void destroy_();
  };

  // member variables
  event_id_c_       event_id;
  uint8_t           hysteresis = 0;
  time_to_trigger_e time_to_trigger;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const eutra_event_s& other) const;
  bool        operator!=(const eutra_event_s& other) const { return not(*this == other); }
};

// ReportConfigEUTRA ::= SEQUENCE
struct report_cfg_eutra_s {
  struct trigger_type_c_ {
    using event_s_ = eutra_event_s;
    struct periodical_s_ {
      struct purpose_opts {
        enum options { report_strongest_cells, report_cgi, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<purpose_opts> purpose_e_;

      // member variables
      purpose_e_ purpose;
    };
    struct types_opts {
      enum options { event, periodical, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    trigger_type_c_() = default;
    trigger_type_c_(const trigger_type_c_& other);
    trigger_type_c_& operator=(const trigger_type_c_& other);
    ~trigger_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const trigger_type_c_& other) const;
    bool        operator!=(const trigger_type_c_& other) const { return not(*this == other); }
    // getters
    event_s_& event()
    {
      assert_choice_type(types::event, type_, "triggerType");
      return c.get<event_s_>();
    }
    periodical_s_& periodical()
    {
      assert_choice_type(types::periodical, type_, "triggerType");
      return c.get<periodical_s_>();
    }
    const event_s_& event() const
    {
      assert_choice_type(types::event, type_, "triggerType");
      return c.get<event_s_>();
    }
    const periodical_s_& periodical() const
    {
      assert_choice_type(types::periodical, type_, "triggerType");
      return c.get<periodical_s_>();
    }
    event_s_&      set_event();
    periodical_s_& set_periodical();

  private:
    types                                    type_;
    choice_buffer_t<event_s_, periodical_s_> c;

    void destroy_();
  };
  struct trigger_quant_opts {
    enum options { rsrp, rsrq, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<trigger_quant_opts> trigger_quant_e_;
  struct report_quant_opts {
    enum options { same_as_trigger_quant, both, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<report_quant_opts> report_quant_e_;
  struct report_amount_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<report_amount_opts> report_amount_e_;
  struct alt_time_to_trigger_r12_c_ {
    using types = setup_e;

    // choice methods
    alt_time_to_trigger_r12_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const alt_time_to_trigger_r12_c_& other) const;
    bool        operator!=(const alt_time_to_trigger_r12_c_& other) const { return not(*this == other); }
    // getters
    time_to_trigger_e& setup()
    {
      assert_choice_type(types::setup, type_, "alternativeTimeToTrigger-r12");
      return c;
    }
    const time_to_trigger_e& setup() const
    {
      assert_choice_type(types::setup, type_, "alternativeTimeToTrigger-r12");
      return c;
    }
    void               set_release();
    time_to_trigger_e& set_setup();

  private:
    types             type_;
    time_to_trigger_e c;
  };
  struct rs_sinr_cfg_r13_c_ {
    struct setup_s_ {
      struct report_quant_v1310_opts {
        enum options { rsrp_andsinr, rsrq_andsinr, all, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<report_quant_v1310_opts> report_quant_v1310_e_;

      // member variables
      bool                  trigger_quant_v1310_present = false;
      bool                  an_thres1_r13_present       = false;
      bool                  a5_thres2_r13_present       = false;
      uint8_t               an_thres1_r13               = 0;
      uint8_t               a5_thres2_r13               = 0;
      report_quant_v1310_e_ report_quant_v1310;
    };
    using types = setup_e;

    // choice methods
    rs_sinr_cfg_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const rs_sinr_cfg_r13_c_& other) const;
    bool        operator!=(const rs_sinr_cfg_r13_c_& other) const { return not(*this == other); }
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "rs-sinr-Config-r13");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "rs-sinr-Config-r13");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct purpose_v1430_opts {
    enum options { report_location, sidelink, spare2, spare1, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<purpose_v1430_opts> purpose_v1430_e_;

  // member variables
  bool             ext = false;
  trigger_type_c_  trigger_type;
  trigger_quant_e_ trigger_quant;
  report_quant_e_  report_quant;
  uint8_t          max_report_cells = 1;
  report_interv_e  report_interv;
  report_amount_e_ report_amount;
  // ...
  // group 0
  bool si_request_for_ho_r9_present             = false;
  bool ue_rx_tx_time_diff_periodical_r9_present = false;
  // group 1
  bool include_location_info_r10_present = false;
  bool report_add_neigh_meas_r10_present = false;
  // group 2
  bool                                 use_t312_r12_present                 = false;
  bool                                 use_ps_cell_r12_present              = false;
  bool                                 report_strongest_csi_rss_r12_present = false;
  bool                                 report_crs_meas_r12_present          = false;
  bool                                 trigger_quant_csi_rs_r12_present     = false;
  copy_ptr<alt_time_to_trigger_r12_c_> alt_time_to_trigger_r12;
  bool                                 use_t312_r12    = false;
  bool                                 use_ps_cell_r12 = false;
  copy_ptr<rsrq_range_cfg_r12_c>       an_thres1_v1250;
  copy_ptr<rsrq_range_cfg_r12_c>       a5_thres2_v1250;
  bool                                 report_strongest_csi_rss_r12 = false;
  bool                                 report_crs_meas_r12          = false;
  bool                                 trigger_quant_csi_rs_r12     = false;
  // group 3
  bool                                 report_sstd_meas_r13_present        = false;
  bool                                 use_allowed_cell_list_r13_present   = false;
  bool                                 include_multi_band_info_r13_present = false;
  bool                                 report_sstd_meas_r13                = false;
  copy_ptr<rs_sinr_cfg_r13_c_>         rs_sinr_cfg_r13;
  bool                                 use_allowed_cell_list_r13 = false;
  copy_ptr<meas_rssi_report_cfg_r13_s> meas_rssi_report_cfg_r13;
  copy_ptr<ul_delay_cfg_r13_c>         ul_delay_cfg_r13;
  // group 4
  bool ue_rx_tx_time_diff_periodical_tdd_r13_present = false;
  bool ue_rx_tx_time_diff_periodical_tdd_r13         = false;
  // group 5
  bool             purpose_v1430_present = false;
  purpose_v1430_e_ purpose_v1430;
  // group 6
  bool    max_report_rs_idx_r15_present = false;
  uint8_t max_report_rs_idx_r15         = 0;
  // group 7
  bool                               purpose_r15_present               = false;
  bool                               nof_trigger_cells_r15_present     = false;
  bool                               a4_a5_report_on_leave_r15_present = false;
  copy_ptr<bt_name_list_cfg_r15_c>   include_bt_meas_r15;
  copy_ptr<wlan_name_list_cfg_r15_c> include_wlan_meas_r15;
  uint8_t                            nof_trigger_cells_r15     = 2;
  bool                               a4_a5_report_on_leave_r15 = false;
  // group 8
  copy_ptr<cond_recfg_trigger_eutra_r16_s> cond_recfg_trigger_eutra_r16;
  copy_ptr<ul_delay_value_cfg_r16_c>       ul_delay_value_cfg_r16;
  // group 9
  bool include_uncom_bar_pre_meas_r17_present = false;
  bool coarse_location_req_r17_present        = false;
  bool include_uncom_bar_pre_meas_r17         = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const report_cfg_eutra_s& other) const;
  bool        operator!=(const report_cfg_eutra_s& other) const { return not(*this == other); }
};

// ReportConfigInterRAT ::= SEQUENCE
struct report_cfg_inter_rat_s {
  struct trigger_type_c_ {
    struct event_s_ {
      struct event_id_c_ {
        struct event_b1_s_ {
          struct b1_thres_c_ {
            struct types_opts {
              enum options { b1_thres_utra, b1_thres_geran, b1_thres_cdma2000, nulltype } value;

              const char* to_string() const;
            };
            typedef enumerated<types_opts> types;

            // choice methods
            b1_thres_c_() = default;
            b1_thres_c_(const b1_thres_c_& other);
            b1_thres_c_& operator=(const b1_thres_c_& other);
            ~b1_thres_c_() { destroy_(); }
            void        set(types::options e = types::nulltype);
            types       type() const { return type_; }
            SRSASN_CODE pack(bit_ref& bref) const;
            SRSASN_CODE unpack(cbit_ref& bref);
            void        to_json(json_writer& j) const;
            bool        operator==(const b1_thres_c_& other) const;
            bool        operator!=(const b1_thres_c_& other) const { return not(*this == other); }
            // getters
            thres_utra_c& b1_thres_utra()
            {
              assert_choice_type(types::b1_thres_utra, type_, "b1-Threshold");
              return c.get<thres_utra_c>();
            }
            uint8_t& b1_thres_geran()
            {
              assert_choice_type(types::b1_thres_geran, type_, "b1-Threshold");
              return c.get<uint8_t>();
            }
            uint8_t& b1_thres_cdma2000()
            {
              assert_choice_type(types::b1_thres_cdma2000, type_, "b1-Threshold");
              return c.get<uint8_t>();
            }
            const thres_utra_c& b1_thres_utra() const
            {
              assert_choice_type(types::b1_thres_utra, type_, "b1-Threshold");
              return c.get<thres_utra_c>();
            }
            const uint8_t& b1_thres_geran() const
            {
              assert_choice_type(types::b1_thres_geran, type_, "b1-Threshold");
              return c.get<uint8_t>();
            }
            const uint8_t& b1_thres_cdma2000() const
            {
              assert_choice_type(types::b1_thres_cdma2000, type_, "b1-Threshold");
              return c.get<uint8_t>();
            }
            thres_utra_c& set_b1_thres_utra();
            uint8_t&      set_b1_thres_geran();
            uint8_t&      set_b1_thres_cdma2000();

          private:
            types                         type_;
            choice_buffer_t<thres_utra_c> c;

            void destroy_();
          };

          // member variables
          b1_thres_c_ b1_thres;
        };
        struct event_b2_s_ {
          struct b2_thres2_c_ {
            struct types_opts {
              enum options { b2_thres2_utra, b2_thres2_geran, b2_thres2_cdma2000, nulltype } value;

              const char* to_string() const;
            };
            typedef enumerated<types_opts> types;

            // choice methods
            b2_thres2_c_() = default;
            b2_thres2_c_(const b2_thres2_c_& other);
            b2_thres2_c_& operator=(const b2_thres2_c_& other);
            ~b2_thres2_c_() { destroy_(); }
            void        set(types::options e = types::nulltype);
            types       type() const { return type_; }
            SRSASN_CODE pack(bit_ref& bref) const;
            SRSASN_CODE unpack(cbit_ref& bref);
            void        to_json(json_writer& j) const;
            bool        operator==(const b2_thres2_c_& other) const;
            bool        operator!=(const b2_thres2_c_& other) const { return not(*this == other); }
            // getters
            thres_utra_c& b2_thres2_utra()
            {
              assert_choice_type(types::b2_thres2_utra, type_, "b2-Threshold2");
              return c.get<thres_utra_c>();
            }
            uint8_t& b2_thres2_geran()
            {
              assert_choice_type(types::b2_thres2_geran, type_, "b2-Threshold2");
              return c.get<uint8_t>();
            }
            uint8_t& b2_thres2_cdma2000()
            {
              assert_choice_type(types::b2_thres2_cdma2000, type_, "b2-Threshold2");
              return c.get<uint8_t>();
            }
            const thres_utra_c& b2_thres2_utra() const
            {
              assert_choice_type(types::b2_thres2_utra, type_, "b2-Threshold2");
              return c.get<thres_utra_c>();
            }
            const uint8_t& b2_thres2_geran() const
            {
              assert_choice_type(types::b2_thres2_geran, type_, "b2-Threshold2");
              return c.get<uint8_t>();
            }
            const uint8_t& b2_thres2_cdma2000() const
            {
              assert_choice_type(types::b2_thres2_cdma2000, type_, "b2-Threshold2");
              return c.get<uint8_t>();
            }
            thres_utra_c& set_b2_thres2_utra();
            uint8_t&      set_b2_thres2_geran();
            uint8_t&      set_b2_thres2_cdma2000();

          private:
            types                         type_;
            choice_buffer_t<thres_utra_c> c;

            void destroy_();
          };

          // member variables
          thres_eutra_c b2_thres1;
          b2_thres2_c_  b2_thres2;
        };
        struct event_w1_r13_s_ {
          uint8_t w1_thres_r13 = 0;
        };
        struct event_w2_r13_s_ {
          uint8_t w2_thres1_r13 = 0;
          uint8_t w2_thres2_r13 = 0;
        };
        struct event_w3_r13_s_ {
          uint8_t w3_thres_r13 = 0;
        };
        struct event_b1_nr_r15_s_ {
          thres_nr_r15_c b1_thres_nr_r15;
          bool           report_on_leave_r15 = false;
        };
        struct event_b2_nr_r15_s_ {
          thres_eutra_c  b2_thres1_r15;
          thres_nr_r15_c b2_thres2_nr_r15;
          bool           report_on_leave_r15 = false;
        };
        struct types_opts {
          enum options {
            event_b1,
            event_b2,
            // ...
            event_w1_r13,
            event_w2_r13,
            event_w3_r13,
            event_b1_nr_r15,
            event_b2_nr_r15,
            nulltype
          } value;

          const char* to_string() const;
        };
        typedef enumerated<types_opts, true, 5> types;

        // choice methods
        event_id_c_() = default;
        event_id_c_(const event_id_c_& other);
        event_id_c_& operator=(const event_id_c_& other);
        ~event_id_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        bool        operator==(const event_id_c_& other) const;
        bool        operator!=(const event_id_c_& other) const { return not(*this == other); }
        // getters
        event_b1_s_& event_b1()
        {
          assert_choice_type(types::event_b1, type_, "eventId");
          return c.get<event_b1_s_>();
        }
        event_b2_s_& event_b2()
        {
          assert_choice_type(types::event_b2, type_, "eventId");
          return c.get<event_b2_s_>();
        }
        event_w1_r13_s_& event_w1_r13()
        {
          assert_choice_type(types::event_w1_r13, type_, "eventId");
          return c.get<event_w1_r13_s_>();
        }
        event_w2_r13_s_& event_w2_r13()
        {
          assert_choice_type(types::event_w2_r13, type_, "eventId");
          return c.get<event_w2_r13_s_>();
        }
        event_w3_r13_s_& event_w3_r13()
        {
          assert_choice_type(types::event_w3_r13, type_, "eventId");
          return c.get<event_w3_r13_s_>();
        }
        event_b1_nr_r15_s_& event_b1_nr_r15()
        {
          assert_choice_type(types::event_b1_nr_r15, type_, "eventId");
          return c.get<event_b1_nr_r15_s_>();
        }
        event_b2_nr_r15_s_& event_b2_nr_r15()
        {
          assert_choice_type(types::event_b2_nr_r15, type_, "eventId");
          return c.get<event_b2_nr_r15_s_>();
        }
        const event_b1_s_& event_b1() const
        {
          assert_choice_type(types::event_b1, type_, "eventId");
          return c.get<event_b1_s_>();
        }
        const event_b2_s_& event_b2() const
        {
          assert_choice_type(types::event_b2, type_, "eventId");
          return c.get<event_b2_s_>();
        }
        const event_w1_r13_s_& event_w1_r13() const
        {
          assert_choice_type(types::event_w1_r13, type_, "eventId");
          return c.get<event_w1_r13_s_>();
        }
        const event_w2_r13_s_& event_w2_r13() const
        {
          assert_choice_type(types::event_w2_r13, type_, "eventId");
          return c.get<event_w2_r13_s_>();
        }
        const event_w3_r13_s_& event_w3_r13() const
        {
          assert_choice_type(types::event_w3_r13, type_, "eventId");
          return c.get<event_w3_r13_s_>();
        }
        const event_b1_nr_r15_s_& event_b1_nr_r15() const
        {
          assert_choice_type(types::event_b1_nr_r15, type_, "eventId");
          return c.get<event_b1_nr_r15_s_>();
        }
        const event_b2_nr_r15_s_& event_b2_nr_r15() const
        {
          assert_choice_type(types::event_b2_nr_r15, type_, "eventId");
          return c.get<event_b2_nr_r15_s_>();
        }
        event_b1_s_&        set_event_b1();
        event_b2_s_&        set_event_b2();
        event_w1_r13_s_&    set_event_w1_r13();
        event_w2_r13_s_&    set_event_w2_r13();
        event_w3_r13_s_&    set_event_w3_r13();
        event_b1_nr_r15_s_& set_event_b1_nr_r15();
        event_b2_nr_r15_s_& set_event_b2_nr_r15();

      private:
        types type_;
        choice_buffer_t<event_b1_nr_r15_s_,
                        event_b1_s_,
                        event_b2_nr_r15_s_,
                        event_b2_s_,
                        event_w1_r13_s_,
                        event_w2_r13_s_,
                        event_w3_r13_s_>
            c;

        void destroy_();
      };

      // member variables
      event_id_c_       event_id;
      uint8_t           hysteresis = 0;
      time_to_trigger_e time_to_trigger;
    };
    struct periodical_s_ {
      struct purpose_opts {
        enum options { report_strongest_cells, report_strongest_cells_for_son, report_cgi, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<purpose_opts> purpose_e_;

      // member variables
      purpose_e_ purpose;
    };
    struct types_opts {
      enum options { event, periodical, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    trigger_type_c_() = default;
    trigger_type_c_(const trigger_type_c_& other);
    trigger_type_c_& operator=(const trigger_type_c_& other);
    ~trigger_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const trigger_type_c_& other) const;
    bool        operator!=(const trigger_type_c_& other) const { return not(*this == other); }
    // getters
    event_s_& event()
    {
      assert_choice_type(types::event, type_, "triggerType");
      return c.get<event_s_>();
    }
    periodical_s_& periodical()
    {
      assert_choice_type(types::periodical, type_, "triggerType");
      return c.get<periodical_s_>();
    }
    const event_s_& event() const
    {
      assert_choice_type(types::event, type_, "triggerType");
      return c.get<event_s_>();
    }
    const periodical_s_& periodical() const
    {
      assert_choice_type(types::periodical, type_, "triggerType");
      return c.get<periodical_s_>();
    }
    event_s_&      set_event();
    periodical_s_& set_periodical();

  private:
    types                                    type_;
    choice_buffer_t<event_s_, periodical_s_> c;

    void destroy_();
  };
  struct report_amount_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, infinity, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<report_amount_opts> report_amount_e_;
  struct b2_thres1_v1250_c_ {
    using types = setup_e;

    // choice methods
    b2_thres1_v1250_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const b2_thres1_v1250_c_& other) const;
    bool        operator!=(const b2_thres1_v1250_c_& other) const { return not(*this == other); }
    // getters
    int8_t& setup()
    {
      assert_choice_type(types::setup, type_, "b2-Threshold1-v1250");
      return c;
    }
    const int8_t& setup() const
    {
      assert_choice_type(types::setup, type_, "b2-Threshold1-v1250");
      return c;
    }
    void    set_release();
    int8_t& set_setup();

  private:
    types  type_;
    int8_t c;
  };
  struct report_sftd_meas_r15_opts {
    enum options { pscell, neighbor_cells, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<report_sftd_meas_r15_opts> report_sftd_meas_r15_e_;

  // member variables
  bool             ext = false;
  trigger_type_c_  trigger_type;
  uint8_t          max_report_cells = 1;
  report_interv_e  report_interv;
  report_amount_e_ report_amount;
  // ...
  // group 0
  bool si_request_for_ho_r9_present = false;
  // group 1
  bool report_quant_utra_fdd_r10_present = false;
  // group 2
  bool include_location_info_r11_present = false;
  bool include_location_info_r11         = false;
  // group 3
  copy_ptr<b2_thres1_v1250_c_> b2_thres1_v1250;
  // group 4
  copy_ptr<report_quant_wlan_r13_s> report_quant_wlan_r13;
  // group 5
  bool report_any_wlan_r14_present = false;
  bool report_any_wlan_r14         = false;
  // group 6
  bool                            max_report_rs_idx_r15_present    = false;
  bool                            report_rs_idx_results_nr_present = false;
  bool                            report_sftd_meas_r15_present     = false;
  copy_ptr<report_quant_nr_r15_s> report_quant_cell_nr_r15;
  uint8_t                         max_report_rs_idx_r15 = 0;
  copy_ptr<report_quant_nr_r15_s> report_quant_rs_idx_nr_r15;
  bool                            report_rs_idx_results_nr = false;
  report_sftd_meas_r15_e_         report_sftd_meas_r15;
  // group 7
  bool                                 use_autonomous_gaps_nr_r16_present = false;
  copy_ptr<meas_rssi_report_cfg_r13_s> meas_rssi_report_cfg_nr_r16;
  // group 8
  copy_ptr<cond_recfg_trigger_nr_r17_s> cond_recfg_trigger_nr_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const report_cfg_inter_rat_s& other) const;
  bool        operator!=(const report_cfg_inter_rat_s& other) const { return not(*this == other); }
};

// MeasGapConfigToAddModList-r14 ::= SEQUENCE (SIZE (1..32)) OF MeasGapConfigPerCC-r14
using meas_gap_cfg_to_add_mod_list_r14_l = dyn_array<meas_gap_cfg_per_cc_r14_s>;

// MeasGapConfigToRemoveList-r14 ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..31)
using meas_gap_cfg_to_rem_list_r14_l = bounded_array<uint8_t, 32>;

// MeasIdToAddMod ::= SEQUENCE
struct meas_id_to_add_mod_s {
  uint8_t meas_id       = 1;
  uint8_t meas_obj_id   = 1;
  uint8_t report_cfg_id = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const meas_id_to_add_mod_s& other) const;
  bool        operator!=(const meas_id_to_add_mod_s& other) const { return not(*this == other); }
};

// MeasIdToAddMod-v1310 ::= SEQUENCE
struct meas_id_to_add_mod_v1310_s {
  bool    meas_obj_id_v1310_present = false;
  uint8_t meas_obj_id_v1310         = 33;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasIdToAddModExt-r12 ::= SEQUENCE
struct meas_id_to_add_mod_ext_r12_s {
  uint8_t meas_id_v1250     = 33;
  uint8_t meas_obj_id_r12   = 1;
  uint8_t report_cfg_id_r12 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectToAddMod ::= SEQUENCE
struct meas_obj_to_add_mod_s {
  struct meas_obj_c_ {
    struct types_opts {
      enum options {
        meas_obj_eutra,
        meas_obj_utra,
        meas_obj_geran,
        meas_obj_cdma2000,
        // ...
        meas_obj_wlan_r13,
        meas_obj_nr_r15,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<types_opts, true, 2> types;

    // choice methods
    meas_obj_c_() = default;
    meas_obj_c_(const meas_obj_c_& other);
    meas_obj_c_& operator=(const meas_obj_c_& other);
    ~meas_obj_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const meas_obj_c_& other) const;
    bool        operator!=(const meas_obj_c_& other) const { return not(*this == other); }
    // getters
    meas_obj_eutra_s& meas_obj_eutra()
    {
      assert_choice_type(types::meas_obj_eutra, type_, "measObject");
      return c.get<meas_obj_eutra_s>();
    }
    meas_obj_utra_s& meas_obj_utra()
    {
      assert_choice_type(types::meas_obj_utra, type_, "measObject");
      return c.get<meas_obj_utra_s>();
    }
    meas_obj_geran_s& meas_obj_geran()
    {
      assert_choice_type(types::meas_obj_geran, type_, "measObject");
      return c.get<meas_obj_geran_s>();
    }
    meas_obj_cdma2000_s& meas_obj_cdma2000()
    {
      assert_choice_type(types::meas_obj_cdma2000, type_, "measObject");
      return c.get<meas_obj_cdma2000_s>();
    }
    meas_obj_wlan_r13_s& meas_obj_wlan_r13()
    {
      assert_choice_type(types::meas_obj_wlan_r13, type_, "measObject");
      return c.get<meas_obj_wlan_r13_s>();
    }
    meas_obj_nr_r15_s& meas_obj_nr_r15()
    {
      assert_choice_type(types::meas_obj_nr_r15, type_, "measObject");
      return c.get<meas_obj_nr_r15_s>();
    }
    const meas_obj_eutra_s& meas_obj_eutra() const
    {
      assert_choice_type(types::meas_obj_eutra, type_, "measObject");
      return c.get<meas_obj_eutra_s>();
    }
    const meas_obj_utra_s& meas_obj_utra() const
    {
      assert_choice_type(types::meas_obj_utra, type_, "measObject");
      return c.get<meas_obj_utra_s>();
    }
    const meas_obj_geran_s& meas_obj_geran() const
    {
      assert_choice_type(types::meas_obj_geran, type_, "measObject");
      return c.get<meas_obj_geran_s>();
    }
    const meas_obj_cdma2000_s& meas_obj_cdma2000() const
    {
      assert_choice_type(types::meas_obj_cdma2000, type_, "measObject");
      return c.get<meas_obj_cdma2000_s>();
    }
    const meas_obj_wlan_r13_s& meas_obj_wlan_r13() const
    {
      assert_choice_type(types::meas_obj_wlan_r13, type_, "measObject");
      return c.get<meas_obj_wlan_r13_s>();
    }
    const meas_obj_nr_r15_s& meas_obj_nr_r15() const
    {
      assert_choice_type(types::meas_obj_nr_r15, type_, "measObject");
      return c.get<meas_obj_nr_r15_s>();
    }
    meas_obj_eutra_s&    set_meas_obj_eutra();
    meas_obj_utra_s&     set_meas_obj_utra();
    meas_obj_geran_s&    set_meas_obj_geran();
    meas_obj_cdma2000_s& set_meas_obj_cdma2000();
    meas_obj_wlan_r13_s& set_meas_obj_wlan_r13();
    meas_obj_nr_r15_s&   set_meas_obj_nr_r15();

  private:
    types type_;
    choice_buffer_t<meas_obj_cdma2000_s,
                    meas_obj_eutra_s,
                    meas_obj_geran_s,
                    meas_obj_nr_r15_s,
                    meas_obj_utra_s,
                    meas_obj_wlan_r13_s>
        c;

    void destroy_();
  };

  // member variables
  uint8_t     meas_obj_id = 1;
  meas_obj_c_ meas_obj;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const meas_obj_to_add_mod_s& other) const;
  bool        operator!=(const meas_obj_to_add_mod_s& other) const { return not(*this == other); }
};

// MeasObjectToAddMod-v9e0 ::= SEQUENCE
struct meas_obj_to_add_mod_v9e0_s {
  bool                  meas_obj_eutra_v9e0_present = false;
  meas_obj_eutra_v9e0_s meas_obj_eutra_v9e0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasObjectToAddModExt-r13 ::= SEQUENCE
struct meas_obj_to_add_mod_ext_r13_s {
  struct meas_obj_r13_c_ {
    struct types_opts {
      enum options {
        meas_obj_eutra_r13,
        meas_obj_utra_r13,
        meas_obj_geran_r13,
        meas_obj_cdma2000_r13,
        // ...
        meas_obj_wlan_v1320,
        meas_obj_nr_r15,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<types_opts, true, 2> types;

    // choice methods
    meas_obj_r13_c_() = default;
    meas_obj_r13_c_(const meas_obj_r13_c_& other);
    meas_obj_r13_c_& operator=(const meas_obj_r13_c_& other);
    ~meas_obj_r13_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    meas_obj_eutra_s& meas_obj_eutra_r13()
    {
      assert_choice_type(types::meas_obj_eutra_r13, type_, "measObject-r13");
      return c.get<meas_obj_eutra_s>();
    }
    meas_obj_utra_s& meas_obj_utra_r13()
    {
      assert_choice_type(types::meas_obj_utra_r13, type_, "measObject-r13");
      return c.get<meas_obj_utra_s>();
    }
    meas_obj_geran_s& meas_obj_geran_r13()
    {
      assert_choice_type(types::meas_obj_geran_r13, type_, "measObject-r13");
      return c.get<meas_obj_geran_s>();
    }
    meas_obj_cdma2000_s& meas_obj_cdma2000_r13()
    {
      assert_choice_type(types::meas_obj_cdma2000_r13, type_, "measObject-r13");
      return c.get<meas_obj_cdma2000_s>();
    }
    meas_obj_wlan_r13_s& meas_obj_wlan_v1320()
    {
      assert_choice_type(types::meas_obj_wlan_v1320, type_, "measObject-r13");
      return c.get<meas_obj_wlan_r13_s>();
    }
    meas_obj_nr_r15_s& meas_obj_nr_r15()
    {
      assert_choice_type(types::meas_obj_nr_r15, type_, "measObject-r13");
      return c.get<meas_obj_nr_r15_s>();
    }
    const meas_obj_eutra_s& meas_obj_eutra_r13() const
    {
      assert_choice_type(types::meas_obj_eutra_r13, type_, "measObject-r13");
      return c.get<meas_obj_eutra_s>();
    }
    const meas_obj_utra_s& meas_obj_utra_r13() const
    {
      assert_choice_type(types::meas_obj_utra_r13, type_, "measObject-r13");
      return c.get<meas_obj_utra_s>();
    }
    const meas_obj_geran_s& meas_obj_geran_r13() const
    {
      assert_choice_type(types::meas_obj_geran_r13, type_, "measObject-r13");
      return c.get<meas_obj_geran_s>();
    }
    const meas_obj_cdma2000_s& meas_obj_cdma2000_r13() const
    {
      assert_choice_type(types::meas_obj_cdma2000_r13, type_, "measObject-r13");
      return c.get<meas_obj_cdma2000_s>();
    }
    const meas_obj_wlan_r13_s& meas_obj_wlan_v1320() const
    {
      assert_choice_type(types::meas_obj_wlan_v1320, type_, "measObject-r13");
      return c.get<meas_obj_wlan_r13_s>();
    }
    const meas_obj_nr_r15_s& meas_obj_nr_r15() const
    {
      assert_choice_type(types::meas_obj_nr_r15, type_, "measObject-r13");
      return c.get<meas_obj_nr_r15_s>();
    }
    meas_obj_eutra_s&    set_meas_obj_eutra_r13();
    meas_obj_utra_s&     set_meas_obj_utra_r13();
    meas_obj_geran_s&    set_meas_obj_geran_r13();
    meas_obj_cdma2000_s& set_meas_obj_cdma2000_r13();
    meas_obj_wlan_r13_s& set_meas_obj_wlan_v1320();
    meas_obj_nr_r15_s&   set_meas_obj_nr_r15();

  private:
    types type_;
    choice_buffer_t<meas_obj_cdma2000_s,
                    meas_obj_eutra_s,
                    meas_obj_geran_s,
                    meas_obj_nr_r15_s,
                    meas_obj_utra_s,
                    meas_obj_wlan_r13_s>
        c;

    void destroy_();
  };

  // member variables
  uint8_t         meas_obj_id_r13 = 33;
  meas_obj_r13_c_ meas_obj_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUR-PeriodicityAndOffset-r16 ::= CHOICE
struct pur_periodicity_and_offset_r16_c {
  struct types_opts {
    enum options {
      periodicity8,
      periodicity16,
      periodicity32,
      periodicity64,
      periodicity128,
      periodicity256,
      periodicity512,
      periodicity1024,
      periodicity2048,
      periodicity4096,
      periodicity8192,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  pur_periodicity_and_offset_r16_c() = default;
  pur_periodicity_and_offset_r16_c(const pur_periodicity_and_offset_r16_c& other);
  pur_periodicity_and_offset_r16_c& operator=(const pur_periodicity_and_offset_r16_c& other);
  ~pur_periodicity_and_offset_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& periodicity8()
  {
    assert_choice_type(types::periodicity8, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& periodicity16()
  {
    assert_choice_type(types::periodicity16, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& periodicity32()
  {
    assert_choice_type(types::periodicity32, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& periodicity64()
  {
    assert_choice_type(types::periodicity64, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint8_t& periodicity128()
  {
    assert_choice_type(types::periodicity128, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  uint16_t& periodicity256()
  {
    assert_choice_type(types::periodicity256, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  uint16_t& periodicity512()
  {
    assert_choice_type(types::periodicity512, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  uint16_t& periodicity1024()
  {
    assert_choice_type(types::periodicity1024, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  uint16_t& periodicity2048()
  {
    assert_choice_type(types::periodicity2048, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  uint16_t& periodicity4096()
  {
    assert_choice_type(types::periodicity4096, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  uint16_t& periodicity8192()
  {
    assert_choice_type(types::periodicity8192, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  const uint8_t& periodicity8() const
  {
    assert_choice_type(types::periodicity8, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& periodicity16() const
  {
    assert_choice_type(types::periodicity16, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& periodicity32() const
  {
    assert_choice_type(types::periodicity32, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& periodicity64() const
  {
    assert_choice_type(types::periodicity64, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& periodicity128() const
  {
    assert_choice_type(types::periodicity128, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint8_t>();
  }
  const uint16_t& periodicity256() const
  {
    assert_choice_type(types::periodicity256, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& periodicity512() const
  {
    assert_choice_type(types::periodicity512, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& periodicity1024() const
  {
    assert_choice_type(types::periodicity1024, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& periodicity2048() const
  {
    assert_choice_type(types::periodicity2048, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& periodicity4096() const
  {
    assert_choice_type(types::periodicity4096, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& periodicity8192() const
  {
    assert_choice_type(types::periodicity8192, type_, "PUR-PeriodicityAndOffset-r16");
    return c.get<uint16_t>();
  }
  uint8_t&  set_periodicity8();
  uint8_t&  set_periodicity16();
  uint8_t&  set_periodicity32();
  uint8_t&  set_periodicity64();
  uint8_t&  set_periodicity128();
  uint16_t& set_periodicity256();
  uint16_t& set_periodicity512();
  uint16_t& set_periodicity1024();
  uint16_t& set_periodicity2048();
  uint16_t& set_periodicity4096();
  uint16_t& set_periodicity8192();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// QuantityConfigCDMA2000 ::= SEQUENCE
struct quant_cfg_cdma2000_s {
  struct meas_quant_cdma2000_opts {
    enum options { pilot_strength, pilot_pn_phase_and_pilot_strength, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<meas_quant_cdma2000_opts> meas_quant_cdma2000_e_;

  // member variables
  meas_quant_cdma2000_e_ meas_quant_cdma2000;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const quant_cfg_cdma2000_s& other) const;
  bool        operator!=(const quant_cfg_cdma2000_s& other) const { return not(*this == other); }
};

// QuantityConfigEUTRA ::= SEQUENCE
struct quant_cfg_eutra_s {
  bool        filt_coef_rsrp_present = false;
  bool        filt_coef_rsrq_present = false;
  filt_coef_e filt_coef_rsrp;
  filt_coef_e filt_coef_rsrq;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const quant_cfg_eutra_s& other) const;
  bool        operator!=(const quant_cfg_eutra_s& other) const { return not(*this == other); }
};

// QuantityConfigEUTRA-v1250 ::= SEQUENCE
struct quant_cfg_eutra_v1250_s {
  bool        filt_coef_csi_rsrp_r12_present = false;
  filt_coef_e filt_coef_csi_rsrp_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const quant_cfg_eutra_v1250_s& other) const;
  bool        operator!=(const quant_cfg_eutra_v1250_s& other) const { return not(*this == other); }
};

// QuantityConfigEUTRA-v1310 ::= SEQUENCE
struct quant_cfg_eutra_v1310_s {
  bool        filt_coef_rs_sinr_r13_present = false;
  filt_coef_e filt_coef_rs_sinr_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const quant_cfg_eutra_v1310_s& other) const;
  bool        operator!=(const quant_cfg_eutra_v1310_s& other) const { return not(*this == other); }
};

// QuantityConfigGERAN ::= SEQUENCE
struct quant_cfg_geran_s {
  bool        filt_coef_present = false;
  filt_coef_e filt_coef;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const quant_cfg_geran_s& other) const;
  bool        operator!=(const quant_cfg_geran_s& other) const { return not(*this == other); }
};

// QuantityConfigNRList-r15 ::= SEQUENCE (SIZE (1..2)) OF QuantityConfigNR-r15
using quant_cfg_nr_list_r15_l = dyn_array<quant_cfg_nr_r15_s>;

// QuantityConfigUTRA ::= SEQUENCE
struct quant_cfg_utra_s {
  struct meas_quant_utra_fdd_opts {
    enum options { cpich_rscp, cpich_ec_n0, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<meas_quant_utra_fdd_opts> meas_quant_utra_fdd_e_;

  // member variables
  bool                   filt_coef_present = false;
  meas_quant_utra_fdd_e_ meas_quant_utra_fdd;
  filt_coef_e            filt_coef;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const quant_cfg_utra_s& other) const;
  bool        operator!=(const quant_cfg_utra_s& other) const { return not(*this == other); }
};

// QuantityConfigUTRA-v1020 ::= SEQUENCE
struct quant_cfg_utra_v1020_s {
  bool        filt_coef2_fdd_r10_present = false;
  filt_coef_e filt_coef2_fdd_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const quant_cfg_utra_v1020_s& other) const;
  bool        operator!=(const quant_cfg_utra_v1020_s& other) const { return not(*this == other); }
};

// QuantityConfigWLAN-r13 ::= SEQUENCE
struct quant_cfg_wlan_r13_s {
  bool        filt_coef_r13_present = false;
  filt_coef_e filt_coef_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const quant_cfg_wlan_r13_s& other) const;
  bool        operator!=(const quant_cfg_wlan_r13_s& other) const { return not(*this == other); }
};

// ReportConfigToAddMod ::= SEQUENCE
struct report_cfg_to_add_mod_s {
  struct report_cfg_c_ {
    struct types_opts {
      enum options { report_cfg_eutra, report_cfg_inter_rat, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    report_cfg_c_() = default;
    report_cfg_c_(const report_cfg_c_& other);
    report_cfg_c_& operator=(const report_cfg_c_& other);
    ~report_cfg_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    bool        operator==(const report_cfg_c_& other) const;
    bool        operator!=(const report_cfg_c_& other) const { return not(*this == other); }
    // getters
    report_cfg_eutra_s& report_cfg_eutra()
    {
      assert_choice_type(types::report_cfg_eutra, type_, "reportConfig");
      return c.get<report_cfg_eutra_s>();
    }
    report_cfg_inter_rat_s& report_cfg_inter_rat()
    {
      assert_choice_type(types::report_cfg_inter_rat, type_, "reportConfig");
      return c.get<report_cfg_inter_rat_s>();
    }
    const report_cfg_eutra_s& report_cfg_eutra() const
    {
      assert_choice_type(types::report_cfg_eutra, type_, "reportConfig");
      return c.get<report_cfg_eutra_s>();
    }
    const report_cfg_inter_rat_s& report_cfg_inter_rat() const
    {
      assert_choice_type(types::report_cfg_inter_rat, type_, "reportConfig");
      return c.get<report_cfg_inter_rat_s>();
    }
    report_cfg_eutra_s&     set_report_cfg_eutra();
    report_cfg_inter_rat_s& set_report_cfg_inter_rat();

  private:
    types                                                       type_;
    choice_buffer_t<report_cfg_eutra_s, report_cfg_inter_rat_s> c;

    void destroy_();
  };

  // member variables
  uint8_t       report_cfg_id = 1;
  report_cfg_c_ report_cfg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const report_cfg_to_add_mod_s& other) const;
  bool        operator!=(const report_cfg_to_add_mod_s& other) const { return not(*this == other); }
};

// MeasGapConfigDensePRS-r15 ::= CHOICE
struct meas_gap_cfg_dense_prs_r15_c {
  struct setup_s_ {
    struct gap_offset_dense_prs_r15_c_ {
      struct types_opts {
        enum options {
          rstd0_r15,
          rstd1_r15,
          rstd2_r15,
          rstd3_r15,
          rstd4_r15,
          rstd5_r15,
          rstd6_r15,
          rstd7_r15,
          rstd8_r15,
          rstd9_r15,
          rstd10_r15,
          rstd11_r15,
          rstd12_r15,
          rstd13_r15,
          rstd14_r15,
          rstd15_r15,
          rstd16_r15,
          rstd17_r15,
          rstd18_r15,
          rstd19_r15,
          rstd20_r15,
          // ...
          nulltype
        } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<types_opts, true> types;

      // choice methods
      gap_offset_dense_prs_r15_c_() = default;
      gap_offset_dense_prs_r15_c_(const gap_offset_dense_prs_r15_c_& other);
      gap_offset_dense_prs_r15_c_& operator=(const gap_offset_dense_prs_r15_c_& other);
      ~gap_offset_dense_prs_r15_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      uint8_t& rstd0_r15()
      {
        assert_choice_type(types::rstd0_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint8_t>();
      }
      uint8_t& rstd1_r15()
      {
        assert_choice_type(types::rstd1_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint8_t>();
      }
      uint16_t& rstd2_r15()
      {
        assert_choice_type(types::rstd2_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint16_t& rstd3_r15()
      {
        assert_choice_type(types::rstd3_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint16_t& rstd4_r15()
      {
        assert_choice_type(types::rstd4_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint8_t& rstd5_r15()
      {
        assert_choice_type(types::rstd5_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint8_t>();
      }
      uint16_t& rstd6_r15()
      {
        assert_choice_type(types::rstd6_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint16_t& rstd7_r15()
      {
        assert_choice_type(types::rstd7_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint16_t& rstd8_r15()
      {
        assert_choice_type(types::rstd8_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint16_t& rstd9_r15()
      {
        assert_choice_type(types::rstd9_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint16_t& rstd10_r15()
      {
        assert_choice_type(types::rstd10_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint16_t& rstd11_r15()
      {
        assert_choice_type(types::rstd11_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint16_t& rstd12_r15()
      {
        assert_choice_type(types::rstd12_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint16_t& rstd13_r15()
      {
        assert_choice_type(types::rstd13_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint16_t& rstd14_r15()
      {
        assert_choice_type(types::rstd14_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint16_t& rstd15_r15()
      {
        assert_choice_type(types::rstd15_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint16_t& rstd16_r15()
      {
        assert_choice_type(types::rstd16_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint16_t& rstd17_r15()
      {
        assert_choice_type(types::rstd17_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint16_t& rstd18_r15()
      {
        assert_choice_type(types::rstd18_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint16_t& rstd19_r15()
      {
        assert_choice_type(types::rstd19_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint16_t& rstd20_r15()
      {
        assert_choice_type(types::rstd20_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint8_t& rstd0_r15() const
      {
        assert_choice_type(types::rstd0_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint8_t>();
      }
      const uint8_t& rstd1_r15() const
      {
        assert_choice_type(types::rstd1_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint8_t>();
      }
      const uint16_t& rstd2_r15() const
      {
        assert_choice_type(types::rstd2_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint16_t& rstd3_r15() const
      {
        assert_choice_type(types::rstd3_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint16_t& rstd4_r15() const
      {
        assert_choice_type(types::rstd4_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint8_t& rstd5_r15() const
      {
        assert_choice_type(types::rstd5_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint8_t>();
      }
      const uint16_t& rstd6_r15() const
      {
        assert_choice_type(types::rstd6_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint16_t& rstd7_r15() const
      {
        assert_choice_type(types::rstd7_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint16_t& rstd8_r15() const
      {
        assert_choice_type(types::rstd8_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint16_t& rstd9_r15() const
      {
        assert_choice_type(types::rstd9_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint16_t& rstd10_r15() const
      {
        assert_choice_type(types::rstd10_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint16_t& rstd11_r15() const
      {
        assert_choice_type(types::rstd11_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint16_t& rstd12_r15() const
      {
        assert_choice_type(types::rstd12_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint16_t& rstd13_r15() const
      {
        assert_choice_type(types::rstd13_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint16_t& rstd14_r15() const
      {
        assert_choice_type(types::rstd14_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint16_t& rstd15_r15() const
      {
        assert_choice_type(types::rstd15_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint16_t& rstd16_r15() const
      {
        assert_choice_type(types::rstd16_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint16_t& rstd17_r15() const
      {
        assert_choice_type(types::rstd17_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint16_t& rstd18_r15() const
      {
        assert_choice_type(types::rstd18_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint16_t& rstd19_r15() const
      {
        assert_choice_type(types::rstd19_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      const uint16_t& rstd20_r15() const
      {
        assert_choice_type(types::rstd20_r15, type_, "gapOffsetDensePRS-r15");
        return c.get<uint16_t>();
      }
      uint8_t&  set_rstd0_r15();
      uint8_t&  set_rstd1_r15();
      uint16_t& set_rstd2_r15();
      uint16_t& set_rstd3_r15();
      uint16_t& set_rstd4_r15();
      uint8_t&  set_rstd5_r15();
      uint16_t& set_rstd6_r15();
      uint16_t& set_rstd7_r15();
      uint16_t& set_rstd8_r15();
      uint16_t& set_rstd9_r15();
      uint16_t& set_rstd10_r15();
      uint16_t& set_rstd11_r15();
      uint16_t& set_rstd12_r15();
      uint16_t& set_rstd13_r15();
      uint16_t& set_rstd14_r15();
      uint16_t& set_rstd15_r15();
      uint16_t& set_rstd16_r15();
      uint16_t& set_rstd17_r15();
      uint16_t& set_rstd18_r15();
      uint16_t& set_rstd19_r15();
      uint16_t& set_rstd20_r15();

    private:
      types               type_;
      pod_choice_buffer_t c;

      void destroy_();
    };

    // member variables
    gap_offset_dense_prs_r15_c_ gap_offset_dense_prs_r15;
  };
  using types = setup_e;

  // choice methods
  meas_gap_cfg_dense_prs_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "MeasGapConfigDensePRS-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "MeasGapConfigDensePRS-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// MeasGapConfigPerCC-List-r14 ::= CHOICE
struct meas_gap_cfg_per_cc_list_r14_c {
  struct setup_s_ {
    bool                               meas_gap_cfg_to_rem_list_r14_present     = false;
    bool                               meas_gap_cfg_to_add_mod_list_r14_present = false;
    meas_gap_cfg_to_rem_list_r14_l     meas_gap_cfg_to_rem_list_r14;
    meas_gap_cfg_to_add_mod_list_r14_l meas_gap_cfg_to_add_mod_list_r14;
  };
  using types = setup_e;

  // choice methods
  meas_gap_cfg_per_cc_list_r14_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "MeasGapConfigPerCC-List-r14");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "MeasGapConfigPerCC-List-r14");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// MeasGapSharingConfig-r14 ::= CHOICE
struct meas_gap_sharing_cfg_r14_c {
  struct setup_s_ {
    struct meas_gap_sharing_scheme_r14_opts {
      enum options { scheme00, scheme01, scheme10, scheme11, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<meas_gap_sharing_scheme_r14_opts> meas_gap_sharing_scheme_r14_e_;

    // member variables
    meas_gap_sharing_scheme_r14_e_ meas_gap_sharing_scheme_r14;
  };
  using types = setup_e;

  // choice methods
  meas_gap_sharing_cfg_r14_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "MeasGapSharingConfig-r14");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "MeasGapSharingConfig-r14");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// MeasIdToAddModList ::= SEQUENCE (SIZE (1..32)) OF MeasIdToAddMod
using meas_id_to_add_mod_list_l = dyn_array<meas_id_to_add_mod_s>;

// MeasIdToAddModList-v1310 ::= SEQUENCE (SIZE (1..32)) OF MeasIdToAddMod-v1310
using meas_id_to_add_mod_list_v1310_l = dyn_array<meas_id_to_add_mod_v1310_s>;

// MeasIdToAddModListExt-r12 ::= SEQUENCE (SIZE (1..32)) OF MeasIdToAddModExt-r12
using meas_id_to_add_mod_list_ext_r12_l = dyn_array<meas_id_to_add_mod_ext_r12_s>;

// MeasIdToAddModListExt-v1310 ::= SEQUENCE (SIZE (1..32)) OF MeasIdToAddMod-v1310
using meas_id_to_add_mod_list_ext_v1310_l = dyn_array<meas_id_to_add_mod_v1310_s>;

// MeasIdToRemoveList ::= SEQUENCE (SIZE (1..32)) OF INTEGER (1..32)
using meas_id_to_rem_list_l = bounded_array<uint8_t, 32>;

// MeasIdToRemoveListExt-r12 ::= SEQUENCE (SIZE (1..32)) OF INTEGER (33..64)
using meas_id_to_rem_list_ext_r12_l = bounded_array<uint8_t, 32>;

// MeasObjectToAddModList ::= SEQUENCE (SIZE (1..32)) OF MeasObjectToAddMod
using meas_obj_to_add_mod_list_l = dyn_array<meas_obj_to_add_mod_s>;

// MeasObjectToAddModList-v9e0 ::= SEQUENCE (SIZE (1..32)) OF MeasObjectToAddMod-v9e0
using meas_obj_to_add_mod_list_v9e0_l = dyn_array<meas_obj_to_add_mod_v9e0_s>;

// MeasObjectToAddModListExt-r13 ::= SEQUENCE (SIZE (1..32)) OF MeasObjectToAddModExt-r13
using meas_obj_to_add_mod_list_ext_r13_l = dyn_array<meas_obj_to_add_mod_ext_r13_s>;

// MeasObjectToRemoveList ::= SEQUENCE (SIZE (1..32)) OF INTEGER (1..32)
using meas_obj_to_rem_list_l = bounded_array<uint8_t, 32>;

// MeasObjectToRemoveListExt-r13 ::= SEQUENCE (SIZE (1..32)) OF INTEGER (33..64)
using meas_obj_to_rem_list_ext_r13_l = bounded_array<uint8_t, 32>;

// MeasScaleFactor-r12 ::= ENUMERATED
struct meas_scale_factor_r12_opts {
  enum options { sf_eutra_cf1, sf_eutra_cf2, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<meas_scale_factor_r12_opts> meas_scale_factor_r12_e;

// QuantityConfig ::= SEQUENCE
struct quant_cfg_s {
  bool                 ext                        = false;
  bool                 quant_cfg_eutra_present    = false;
  bool                 quant_cfg_utra_present     = false;
  bool                 quant_cfg_geran_present    = false;
  bool                 quant_cfg_cdma2000_present = false;
  quant_cfg_eutra_s    quant_cfg_eutra;
  quant_cfg_utra_s     quant_cfg_utra;
  quant_cfg_geran_s    quant_cfg_geran;
  quant_cfg_cdma2000_s quant_cfg_cdma2000;
  // ...
  // group 0
  copy_ptr<quant_cfg_utra_v1020_s> quant_cfg_utra_v1020;
  // group 1
  copy_ptr<quant_cfg_eutra_v1250_s> quant_cfg_eutra_v1250;
  // group 2
  copy_ptr<quant_cfg_eutra_v1310_s> quant_cfg_eutra_v1310;
  copy_ptr<quant_cfg_wlan_r13_s>    quant_cfg_wlan_r13;
  // group 3
  copy_ptr<quant_cfg_nr_list_r15_l> quant_cfg_nr_list_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        operator==(const quant_cfg_s& other) const;
  bool        operator!=(const quant_cfg_s& other) const { return not(*this == other); }
};

// RACH-ConfigDedicated ::= SEQUENCE
struct rach_cfg_ded_s {
  uint8_t ra_preamb_idx     = 0;
  uint8_t ra_prach_mask_idx = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RACH-Skip-r14 ::= SEQUENCE
struct rach_skip_r14_s {
  struct target_ta_r14_c_ {
    struct types_opts {
      enum options { ta0_r14, mcg_ptag_r14, scg_ptag_r14, mcg_stag_r14, scg_stag_r14, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    target_ta_r14_c_() = default;
    target_ta_r14_c_(const target_ta_r14_c_& other);
    target_ta_r14_c_& operator=(const target_ta_r14_c_& other);
    ~target_ta_r14_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& mcg_stag_r14()
    {
      assert_choice_type(types::mcg_stag_r14, type_, "targetTA-r14");
      return c.get<uint8_t>();
    }
    uint8_t& scg_stag_r14()
    {
      assert_choice_type(types::scg_stag_r14, type_, "targetTA-r14");
      return c.get<uint8_t>();
    }
    const uint8_t& mcg_stag_r14() const
    {
      assert_choice_type(types::mcg_stag_r14, type_, "targetTA-r14");
      return c.get<uint8_t>();
    }
    const uint8_t& scg_stag_r14() const
    {
      assert_choice_type(types::scg_stag_r14, type_, "targetTA-r14");
      return c.get<uint8_t>();
    }
    void     set_ta0_r14();
    void     set_mcg_ptag_r14();
    void     set_scg_ptag_r14();
    uint8_t& set_mcg_stag_r14();
    uint8_t& set_scg_stag_r14();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct ul_cfg_info_r14_s_ {
    struct ul_sched_interv_r14_opts {
      enum options { sf2, sf5, sf10, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<ul_sched_interv_r14_opts> ul_sched_interv_r14_e_;

    // member variables
    uint8_t                nof_conf_ul_processes_r14 = 1;
    ul_sched_interv_r14_e_ ul_sched_interv_r14;
    uint8_t                ul_start_sf_r14 = 0;
    fixed_bitstring<16>    ul_grant_r14;
  };

  // member variables
  bool               ul_cfg_info_r14_present = false;
  target_ta_r14_c_   target_ta_r14;
  ul_cfg_info_r14_s_ ul_cfg_info_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReportConfigToAddModList ::= SEQUENCE (SIZE (1..32)) OF ReportConfigToAddMod
using report_cfg_to_add_mod_list_l = dyn_array<report_cfg_to_add_mod_s>;

// ReportConfigToRemoveList ::= SEQUENCE (SIZE (1..32)) OF INTEGER (1..32)
using report_cfg_to_rem_list_l = bounded_array<uint8_t, 32>;

// MeasConfig ::= SEQUENCE
struct meas_cfg_s {
  struct speed_state_pars_c_ {
    struct setup_s_ {
      mob_state_params_s          mob_state_params;
      speed_state_scale_factors_s time_to_trigger_sf;
    };
    using types = setup_e;

    // choice methods
    speed_state_pars_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "speedStatePars");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "speedStatePars");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct meas_scale_factor_r12_c_ {
    using types = setup_e;

    // choice methods
    meas_scale_factor_r12_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    meas_scale_factor_r12_e& setup()
    {
      assert_choice_type(types::setup, type_, "measScaleFactor-r12");
      return c;
    }
    const meas_scale_factor_r12_e& setup() const
    {
      assert_choice_type(types::setup, type_, "measScaleFactor-r12");
      return c;
    }
    void                     set_release();
    meas_scale_factor_r12_e& set_setup();

  private:
    types                   type_;
    meas_scale_factor_r12_e c;
  };
  struct height_thresh_ref_r15_c_ {
    using types = setup_e;

    // choice methods
    height_thresh_ref_r15_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& setup()
    {
      assert_choice_type(types::setup, type_, "heightThreshRef-r15");
      return c;
    }
    const uint8_t& setup() const
    {
      assert_choice_type(types::setup, type_, "heightThreshRef-r15");
      return c;
    }
    void     set_release();
    uint8_t& set_setup();

  private:
    types   type_;
    uint8_t c;
  };

  // member variables
  bool                         ext                                = false;
  bool                         meas_obj_to_rem_list_present       = false;
  bool                         meas_obj_to_add_mod_list_present   = false;
  bool                         report_cfg_to_rem_list_present     = false;
  bool                         report_cfg_to_add_mod_list_present = false;
  bool                         meas_id_to_rem_list_present        = false;
  bool                         meas_id_to_add_mod_list_present    = false;
  bool                         quant_cfg_present                  = false;
  bool                         meas_gap_cfg_present               = false;
  bool                         s_measure_present                  = false;
  bool                         pre_regist_info_hrpd_present       = false;
  bool                         speed_state_pars_present           = false;
  meas_obj_to_rem_list_l       meas_obj_to_rem_list;
  meas_obj_to_add_mod_list_l   meas_obj_to_add_mod_list;
  report_cfg_to_rem_list_l     report_cfg_to_rem_list;
  report_cfg_to_add_mod_list_l report_cfg_to_add_mod_list;
  meas_id_to_rem_list_l        meas_id_to_rem_list;
  meas_id_to_add_mod_list_l    meas_id_to_add_mod_list;
  quant_cfg_s                  quant_cfg;
  meas_gap_cfg_c               meas_gap_cfg;
  uint8_t                      s_measure = 0;
  pre_regist_info_hrpd_s       pre_regist_info_hrpd;
  speed_state_pars_c_          speed_state_pars;
  // ...
  // group 0
  copy_ptr<meas_obj_to_add_mod_list_v9e0_l> meas_obj_to_add_mod_list_v9e0;
  // group 1
  bool allow_interruptions_r11_present = false;
  bool allow_interruptions_r11         = false;
  // group 2
  bool                                        meas_rsrq_on_all_symbols_r12_present = false;
  copy_ptr<meas_scale_factor_r12_c_>          meas_scale_factor_r12;
  copy_ptr<meas_id_to_rem_list_ext_r12_l>     meas_id_to_rem_list_ext_r12;
  copy_ptr<meas_id_to_add_mod_list_ext_r12_l> meas_id_to_add_mod_list_ext_r12;
  bool                                        meas_rsrq_on_all_symbols_r12 = false;
  // group 3
  copy_ptr<meas_obj_to_rem_list_ext_r13_l>      meas_obj_to_rem_list_ext_r13;
  copy_ptr<meas_obj_to_add_mod_list_ext_r13_l>  meas_obj_to_add_mod_list_ext_r13;
  copy_ptr<meas_id_to_add_mod_list_v1310_l>     meas_id_to_add_mod_list_v1310;
  copy_ptr<meas_id_to_add_mod_list_ext_v1310_l> meas_id_to_add_mod_list_ext_v1310;
  // group 4
  copy_ptr<meas_gap_cfg_per_cc_list_r14_c> meas_gap_cfg_per_cc_list_r14;
  copy_ptr<meas_gap_sharing_cfg_r14_c>     meas_gap_sharing_cfg_r14;
  // group 5
  bool fr1_gap_r15_present = false;
  bool mgta_r15_present    = false;
  bool fr1_gap_r15         = false;
  bool mgta_r15            = false;
  // group 6
  copy_ptr<meas_gap_cfg_dense_prs_r15_c> meas_gap_cfg_dense_prs_r15;
  copy_ptr<height_thresh_ref_r15_c_>     height_thresh_ref_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EventType-r17 ::= CHOICE
struct event_type_r17_c {
  struct event_l1_s_ {
    thres_eutra_c     l1_thres_r17;
    uint8_t           hysteresis_r17 = 0;
    time_to_trigger_e time_to_trigger_r17;
  };
  struct types_opts {
    enum options { out_of_coverage, event_l1, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  event_type_r17_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  event_l1_s_& event_l1()
  {
    assert_choice_type(types::event_l1, type_, "EventType-r17");
    return c;
  }
  const event_l1_s_& event_l1() const
  {
    assert_choice_type(types::event_l1, type_, "EventType-r17");
    return c;
  }
  void         set_out_of_coverage();
  event_l1_s_& set_event_l1();

private:
  types       type_;
  event_l1_s_ c;
};

// LoggedEventTriggerConfig-r17 ::= SEQUENCE
struct logged_event_trigger_cfg_r17_s {
  event_type_r17_c event_type_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqGERAN ::= SEQUENCE
struct carrier_freq_geran_s {
  uint16_t         arfcn = 0;
  band_ind_geran_e band_ind;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggedMeasurementConfiguration-v1700-IEs ::= SEQUENCE
struct logged_meas_cfg_v1700_ies_s {
  bool                           logged_event_trigger_cfg_r17_present = false;
  bool                           meas_uncom_bar_pre_r17_present       = false;
  bool                           non_crit_ext_present                 = false;
  logged_event_trigger_cfg_r17_s logged_event_trigger_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LoggedMeasurementConfiguration-v1530-IEs ::= SEQUENCE
struct logged_meas_cfg_v1530_ies_s {
  bool                        bt_name_list_r15_present   = false;
  bool                        wlan_name_list_r15_present = false;
  bool                        non_crit_ext_present       = false;
  bt_name_list_r15_l          bt_name_list_r15;
  wlan_name_list_r15_l        wlan_name_list_r15;
  logged_meas_cfg_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellGlobalIdEUTRA ::= SEQUENCE
struct cell_global_id_eutra_s {
  plmn_id_s           plmn_id;
  fixed_bitstring<28> cell_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DAPS-PowerCoordinationInfo-r16 ::= SEQUENCE
struct daps_pwr_coordination_info_r16_s {
  uint8_t p_daps_source_r16 = 1;
  uint8_t p_daps_target_r16 = 1;
  uint8_t pwr_ctrl_mode_r16 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierBandwidthEUTRA ::= SEQUENCE
struct carrier_bw_eutra_s {
  struct dl_bw_opts {
    enum options {
      n6,
      n15,
      n25,
      n50,
      n75,
      n100,
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
  typedef enumerated<dl_bw_opts> dl_bw_e_;
  struct ul_bw_opts {
    enum options {
      n6,
      n15,
      n25,
      n50,
      n75,
      n100,
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
  typedef enumerated<ul_bw_opts> ul_bw_e_;

  // member variables
  bool     ul_bw_present = false;
  dl_bw_e_ dl_bw;
  ul_bw_e_ ul_bw;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqEUTRA ::= SEQUENCE
struct carrier_freq_eutra_s {
  bool     ul_carrier_freq_present = false;
  uint32_t dl_carrier_freq         = 0;
  uint32_t ul_carrier_freq         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqEUTRA-v9e0 ::= SEQUENCE
struct carrier_freq_eutra_v9e0_s {
  bool     ul_carrier_freq_v9e0_present = false;
  uint32_t dl_carrier_freq_v9e0         = 0;
  uint32_t ul_carrier_freq_v9e0         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DAPS-Config-r16 ::= SEQUENCE
struct daps_cfg_r16_s {
  bool                             ext                                    = false;
  bool                             daps_pwr_coordination_info_r16_present = false;
  daps_pwr_coordination_info_r16_s daps_pwr_coordination_info_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityControlInfoV2X-r14 ::= SEQUENCE
struct mob_ctrl_info_v2x_r14_s {
  bool                            v2x_comm_tx_pool_exceptional_r14_present = false;
  bool                            v2x_comm_rx_pool_r14_present             = false;
  bool                            v2x_comm_sync_cfg_r14_present            = false;
  bool                            cbr_mob_tx_cfg_list_r14_present          = false;
  sl_comm_res_pool_v2x_r14_s      v2x_comm_tx_pool_exceptional_r14;
  sl_comm_rx_pool_list_v2x_r14_l  v2x_comm_rx_pool_r14;
  sl_sync_cfg_list_v2x_r14_l      v2x_comm_sync_cfg_r14;
  sl_cbr_common_tx_cfg_list_r14_s cbr_mob_tx_cfg_list_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityControlInfo ::= SEQUENCE
struct mob_ctrl_info_s {
  struct t304_opts {
    enum options { ms50, ms100, ms150, ms200, ms500, ms1000, ms2000, ms10000_v1310, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t304_opts> t304_e_;
  struct ho_without_wt_change_r14_opts {
    enum options { keep_lwa_cfg, send_end_marker, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<ho_without_wt_change_r14_opts> ho_without_wt_change_r14_e_;

  // member variables
  bool                 ext                       = false;
  bool                 carrier_freq_present      = false;
  bool                 carrier_bw_present        = false;
  bool                 add_spec_emission_present = false;
  bool                 rach_cfg_ded_present      = false;
  uint16_t             target_pci                = 0;
  carrier_freq_eutra_s carrier_freq;
  carrier_bw_eutra_s   carrier_bw;
  uint8_t              add_spec_emission = 1;
  t304_e_              t304;
  fixed_bitstring<16>  new_ue_id;
  rr_cfg_common_s      rr_cfg_common;
  rach_cfg_ded_s       rach_cfg_ded;
  // ...
  // group 0
  copy_ptr<carrier_freq_eutra_v9e0_s> carrier_freq_v9e0;
  // group 1
  bool drb_continue_rohc_r11_present = false;
  // group 2
  bool                              ho_without_wt_change_r14_present = false;
  bool                              make_before_break_r14_present    = false;
  bool                              same_sfn_ind_r14_present         = false;
  copy_ptr<mob_ctrl_info_v2x_r14_s> mob_ctrl_info_v2x_r14;
  ho_without_wt_change_r14_e_       ho_without_wt_change_r14;
  copy_ptr<rach_skip_r14_s>         rach_skip_r14;
  // group 3
  bool    mib_repeat_status_r14_present  = false;
  bool    sched_info_sib1_br_r14_present = false;
  bool    mib_repeat_status_r14          = false;
  uint8_t sched_info_sib1_br_r14         = 0;
  // group 4
  copy_ptr<daps_cfg_r16_s> daps_cfg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TraceReference-r10 ::= SEQUENCE
struct trace_ref_r10_s {
  plmn_id_s          plmn_id_r10;
  fixed_octstring<3> trace_id_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityListNR-r15 ::= SEQUENCE (SIZE (1..12)) OF PLMN-Identity
using plmn_id_list_nr_r15_l = dyn_array<plmn_id_s>;

// MeasResultNR-r15 ::= SEQUENCE
struct meas_result_nr_r15_s {
  bool    ext                        = false;
  bool    rsrp_result_r15_present    = false;
  bool    rsrq_result_r15_present    = false;
  bool    rs_sinr_result_r15_present = false;
  uint8_t rsrp_result_r15            = 0;
  uint8_t rsrq_result_r15            = 0;
  uint8_t rs_sinr_result_r15         = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityInfoNR-r15 ::= SEQUENCE
struct plmn_id_info_nr_r15_s {
  bool                  tac_r15_present           = false;
  bool                  ran_area_code_r15_present = false;
  plmn_id_list_nr_r15_l plmn_id_list_r15;
  fixed_bitstring<24>   tac_r15;
  uint16_t              ran_area_code_r15 = 0;
  fixed_bitstring<36>   cell_id_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityInfoNR-v1710 ::= SEQUENCE
struct plmn_id_info_nr_v1710_s {
  bool    gnb_id_len_r17_present = false;
  uint8_t gnb_id_len_r17         = 22;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellGlobalIdCDMA2000 ::= CHOICE
struct cell_global_id_cdma2000_c {
  struct types_opts {
    enum options { cell_global_id1_xrtt, cell_global_id_hrpd, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  cell_global_id_cdma2000_c() = default;
  cell_global_id_cdma2000_c(const cell_global_id_cdma2000_c& other);
  cell_global_id_cdma2000_c& operator=(const cell_global_id_cdma2000_c& other);
  ~cell_global_id_cdma2000_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<47>& cell_global_id1_xrtt()
  {
    assert_choice_type(types::cell_global_id1_xrtt, type_, "CellGlobalIdCDMA2000");
    return c.get<fixed_bitstring<47> >();
  }
  fixed_bitstring<128>& cell_global_id_hrpd()
  {
    assert_choice_type(types::cell_global_id_hrpd, type_, "CellGlobalIdCDMA2000");
    return c.get<fixed_bitstring<128> >();
  }
  const fixed_bitstring<47>& cell_global_id1_xrtt() const
  {
    assert_choice_type(types::cell_global_id1_xrtt, type_, "CellGlobalIdCDMA2000");
    return c.get<fixed_bitstring<47> >();
  }
  const fixed_bitstring<128>& cell_global_id_hrpd() const
  {
    assert_choice_type(types::cell_global_id_hrpd, type_, "CellGlobalIdCDMA2000");
    return c.get<fixed_bitstring<128> >();
  }
  fixed_bitstring<47>&  set_cell_global_id1_xrtt();
  fixed_bitstring<128>& set_cell_global_id_hrpd();

private:
  types                                  type_;
  choice_buffer_t<fixed_bitstring<128> > c;

  void destroy_();
};

// MeasResultSSB-Index-r15 ::= SEQUENCE
struct meas_result_ssb_idx_r15_s {
  bool                 ext                             = false;
  bool                 meas_result_ssb_idx_r15_present = false;
  uint8_t              ssb_idx_r15                     = 0;
  meas_result_nr_r15_s meas_result_ssb_idx_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityInfoListNR-r15 ::= SEQUENCE (SIZE (1..12)) OF PLMN-IdentityInfoNR-r15
using plmn_id_info_list_nr_r15_l = dyn_array<plmn_id_info_nr_r15_s>;

// PLMN-IdentityInfoListNR-v1710 ::= SEQUENCE (SIZE (1..12)) OF PLMN-IdentityInfoNR-v1710
using plmn_id_info_list_nr_v1710_l = dyn_array<plmn_id_info_nr_v1710_s>;

// AdditionalSI-Info-r9 ::= SEQUENCE
struct add_si_info_r9_s {
  bool                csg_member_status_r9_present = false;
  bool                csg_id_r9_present            = false;
  fixed_bitstring<27> csg_id_r9;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BLER-Result-r12 ::= SEQUENCE
struct bler_result_r12_s {
  struct blocks_rx_r12_s_ {
    fixed_bitstring<3> n_r12;
    fixed_bitstring<8> m_r12;
  };

  // member variables
  uint8_t          bler_r12 = 0;
  blocks_rx_r12_s_ blocks_rx_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CGI-InfoNR-r15 ::= SEQUENCE
struct cgi_info_nr_r15_s {
  struct no_sib1_r15_s_ {
    uint8_t  ssb_subcarrier_offset_r15 = 0;
    uint16_t pdcch_cfg_sib1_r15        = 0;
  };

  // member variables
  bool                          ext                           = false;
  bool                          plmn_id_info_list_r15_present = false;
  bool                          freq_band_list_r15_present    = false;
  bool                          no_sib1_r15_present           = false;
  plmn_id_info_list_nr_r15_l    plmn_id_info_list_r15;
  multi_freq_band_list_nr_r15_l freq_band_list_r15;
  no_sib1_r15_s_                no_sib1_r15;
  // ...
  // group 0
  copy_ptr<plmn_id_info_list_nr_v1710_l> plmn_id_info_list_v1710;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellGlobalIdUTRA ::= SEQUENCE
struct cell_global_id_utra_s {
  plmn_id_s           plmn_id;
  fixed_bitstring<28> cell_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultCDMA2000 ::= SEQUENCE
struct meas_result_cdma2000_s {
  struct meas_result_s_ {
    bool     ext                    = false;
    bool     pilot_pn_phase_present = false;
    uint16_t pilot_pn_phase         = 0;
    uint8_t  pilot_strength         = 0;
    // ...
  };

  // member variables
  bool                      cgi_info_present = false;
  uint16_t                  pci              = 0;
  cell_global_id_cdma2000_c cgi_info;
  meas_result_s_            meas_result;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultSSB-IndexList-r15 ::= SEQUENCE (SIZE (1..32)) OF MeasResultSSB-Index-r15
using meas_result_ssb_idx_list_r15_l = dyn_array<meas_result_ssb_idx_r15_s>;

// PLMN-IdentityList2 ::= SEQUENCE (SIZE (1..5)) OF PLMN-Identity
using plmn_id_list2_l = dyn_array<plmn_id_s>;

// CellGlobalIdGERAN ::= SEQUENCE
struct cell_global_id_geran_s {
  plmn_id_s           plmn_id;
  fixed_bitstring<16> location_area_code;
  fixed_bitstring<16> cell_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DataBLER-MCH-Result-r12 ::= SEQUENCE
struct data_bler_mch_result_r12_s {
  uint8_t           mch_idx_r12 = 1;
  bler_result_r12_s data_bler_result_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultCellNR-r15 ::= SEQUENCE
struct meas_result_cell_nr_r15_s {
  bool                           ext                                 = false;
  bool                           meas_result_rs_idx_list_r15_present = false;
  uint16_t                       pci_r15                             = 0;
  meas_result_nr_r15_s           meas_result_cell_r15;
  meas_result_ssb_idx_list_r15_l meas_result_rs_idx_list_r15;
  // ...
  // group 0
  copy_ptr<cgi_info_nr_r15_s> cgi_info_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultEUTRA ::= SEQUENCE
struct meas_result_eutra_s {
  struct cgi_info_s_ {
    bool                   plmn_id_list_present = false;
    cell_global_id_eutra_s cell_global_id;
    fixed_bitstring<16>    tac;
    plmn_id_list2_l        plmn_id_list;
  };
  struct meas_result_s_ {
    struct cgi_info_v1310_s_ {
      bool                       freq_band_ind_r13_present        = false;
      bool                       multi_band_info_list_r13_present = false;
      bool                       freq_band_ind_prio_r13_present   = false;
      uint16_t                   freq_band_ind_r13                = 1;
      multi_band_info_list_r11_l multi_band_info_list_r13;
    };
    using cgi_info_minus5_gc_r15_l_ = dyn_array<cell_access_related_info_minus5_gc_r15_s>;

    // member variables
    bool    ext                 = false;
    bool    rsrp_result_present = false;
    bool    rsrq_result_present = false;
    uint8_t rsrp_result         = 0;
    uint8_t rsrq_result         = 0;
    // ...
    // group 0
    copy_ptr<add_si_info_r9_s> add_si_info_r9;
    // group 1
    bool   primary_plmn_suitable_r12_present = false;
    bool   meas_result_v1250_present         = false;
    int8_t meas_result_v1250                 = -30;
    // group 2
    bool                        rs_sinr_result_r13_present = false;
    uint8_t                     rs_sinr_result_r13         = 0;
    copy_ptr<cgi_info_v1310_s_> cgi_info_v1310;
    // group 3
    bool   meas_result_v1360_present = false;
    int8_t meas_result_v1360         = -17;
    // group 4
    copy_ptr<cgi_info_minus5_gc_r15_l_> cgi_info_minus5_gc_r15;

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool           cgi_info_present = false;
  uint16_t       pci              = 0;
  cgi_info_s_    cgi_info;
  meas_result_s_ meas_result;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultListCDMA2000 ::= SEQUENCE (SIZE (1..8)) OF MeasResultCDMA2000
using meas_result_list_cdma2000_l = dyn_array<meas_result_cdma2000_s>;

// MeasResultUTRA ::= SEQUENCE
struct meas_result_utra_s {
  struct pci_c_ {
    struct types_opts {
      enum options { fdd, tdd, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    pci_c_() = default;
    pci_c_(const pci_c_& other);
    pci_c_& operator=(const pci_c_& other);
    ~pci_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t& fdd()
    {
      assert_choice_type(types::fdd, type_, "physCellId");
      return c.get<uint16_t>();
    }
    uint8_t& tdd()
    {
      assert_choice_type(types::tdd, type_, "physCellId");
      return c.get<uint8_t>();
    }
    const uint16_t& fdd() const
    {
      assert_choice_type(types::fdd, type_, "physCellId");
      return c.get<uint16_t>();
    }
    const uint8_t& tdd() const
    {
      assert_choice_type(types::tdd, type_, "physCellId");
      return c.get<uint8_t>();
    }
    uint16_t& set_fdd();
    uint8_t&  set_tdd();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct cgi_info_s_ {
    bool                  location_area_code_present = false;
    bool                  routing_area_code_present  = false;
    bool                  plmn_id_list_present       = false;
    cell_global_id_utra_s cell_global_id;
    fixed_bitstring<16>   location_area_code;
    fixed_bitstring<8>    routing_area_code;
    plmn_id_list2_l       plmn_id_list;
  };
  struct meas_result_s_ {
    bool    ext                = false;
    bool    utra_rscp_present  = false;
    bool    utra_ec_n0_present = false;
    int8_t  utra_rscp          = -5;
    uint8_t utra_ec_n0         = 0;
    // ...
    // group 0
    copy_ptr<add_si_info_r9_s> add_si_info_r9;
    // group 1
    bool primary_plmn_suitable_r12_present = false;

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };

  // member variables
  bool           cgi_info_present = false;
  pci_c_         pci;
  cgi_info_s_    cgi_info;
  meas_result_s_ meas_result;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DataBLER-MCH-ResultList-r12 ::= SEQUENCE (SIZE (1..15)) OF DataBLER-MCH-Result-r12
using data_bler_mch_result_list_r12_l = dyn_array<data_bler_mch_result_r12_s>;

// LocationInfo-r10 ::= SEQUENCE
struct location_info_r10_s {
  struct location_coordinates_r10_c_ {
    struct types_opts {
      enum options {
        ellipsoid_point_r10,
        ellipsoid_point_with_altitude_r10,
        // ...
        ellipsoid_point_with_uncertainty_circle_r11,
        ellipsoid_point_with_uncertainty_ellipse_r11,
        ellipsoid_point_with_altitude_and_uncertainty_ellipsoid_r11,
        ellipsoid_arc_r11,
        polygon_r11,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts, true, 5> types;

    // choice methods
    location_coordinates_r10_c_() = default;
    location_coordinates_r10_c_(const location_coordinates_r10_c_& other);
    location_coordinates_r10_c_& operator=(const location_coordinates_r10_c_& other);
    ~location_coordinates_r10_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dyn_octstring& ellipsoid_point_r10()
    {
      assert_choice_type(types::ellipsoid_point_r10, type_, "locationCoordinates-r10");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& ellipsoid_point_with_altitude_r10()
    {
      assert_choice_type(types::ellipsoid_point_with_altitude_r10, type_, "locationCoordinates-r10");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& ellipsoid_point_with_uncertainty_circle_r11()
    {
      assert_choice_type(types::ellipsoid_point_with_uncertainty_circle_r11, type_, "locationCoordinates-r10");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& ellipsoid_point_with_uncertainty_ellipse_r11()
    {
      assert_choice_type(types::ellipsoid_point_with_uncertainty_ellipse_r11, type_, "locationCoordinates-r10");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& ellipsoid_point_with_altitude_and_uncertainty_ellipsoid_r11()
    {
      assert_choice_type(
          types::ellipsoid_point_with_altitude_and_uncertainty_ellipsoid_r11, type_, "locationCoordinates-r10");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& ellipsoid_arc_r11()
    {
      assert_choice_type(types::ellipsoid_arc_r11, type_, "locationCoordinates-r10");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& polygon_r11()
    {
      assert_choice_type(types::polygon_r11, type_, "locationCoordinates-r10");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ellipsoid_point_r10() const
    {
      assert_choice_type(types::ellipsoid_point_r10, type_, "locationCoordinates-r10");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ellipsoid_point_with_altitude_r10() const
    {
      assert_choice_type(types::ellipsoid_point_with_altitude_r10, type_, "locationCoordinates-r10");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ellipsoid_point_with_uncertainty_circle_r11() const
    {
      assert_choice_type(types::ellipsoid_point_with_uncertainty_circle_r11, type_, "locationCoordinates-r10");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ellipsoid_point_with_uncertainty_ellipse_r11() const
    {
      assert_choice_type(types::ellipsoid_point_with_uncertainty_ellipse_r11, type_, "locationCoordinates-r10");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ellipsoid_point_with_altitude_and_uncertainty_ellipsoid_r11() const
    {
      assert_choice_type(
          types::ellipsoid_point_with_altitude_and_uncertainty_ellipsoid_r11, type_, "locationCoordinates-r10");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ellipsoid_arc_r11() const
    {
      assert_choice_type(types::ellipsoid_arc_r11, type_, "locationCoordinates-r10");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& polygon_r11() const
    {
      assert_choice_type(types::polygon_r11, type_, "locationCoordinates-r10");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& set_ellipsoid_point_r10();
    dyn_octstring& set_ellipsoid_point_with_altitude_r10();
    dyn_octstring& set_ellipsoid_point_with_uncertainty_circle_r11();
    dyn_octstring& set_ellipsoid_point_with_uncertainty_ellipse_r11();
    dyn_octstring& set_ellipsoid_point_with_altitude_and_uncertainty_ellipsoid_r11();
    dyn_octstring& set_ellipsoid_arc_r11();
    dyn_octstring& set_polygon_r11();

  private:
    types                          type_;
    choice_buffer_t<dyn_octstring> c;

    void destroy_();
  };
  struct vertical_velocity_info_r15_c_ {
    struct types_opts {
      enum options { vertical_velocity_r15, vertical_velocity_and_uncertainty_r15, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    vertical_velocity_info_r15_c_() = default;
    vertical_velocity_info_r15_c_(const vertical_velocity_info_r15_c_& other);
    vertical_velocity_info_r15_c_& operator=(const vertical_velocity_info_r15_c_& other);
    ~vertical_velocity_info_r15_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dyn_octstring& vertical_velocity_r15()
    {
      assert_choice_type(types::vertical_velocity_r15, type_, "verticalVelocityInfo-r15");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& vertical_velocity_and_uncertainty_r15()
    {
      assert_choice_type(types::vertical_velocity_and_uncertainty_r15, type_, "verticalVelocityInfo-r15");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& vertical_velocity_r15() const
    {
      assert_choice_type(types::vertical_velocity_r15, type_, "verticalVelocityInfo-r15");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& vertical_velocity_and_uncertainty_r15() const
    {
      assert_choice_type(types::vertical_velocity_and_uncertainty_r15, type_, "verticalVelocityInfo-r15");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& set_vertical_velocity_r15();
    dyn_octstring& set_vertical_velocity_and_uncertainty_r15();

  private:
    types                          type_;
    choice_buffer_t<dyn_octstring> c;

    void destroy_();
  };

  // member variables
  bool                        ext                             = false;
  bool                        horizontal_velocity_r10_present = false;
  bool                        gnss_tod_msec_r10_present       = false;
  location_coordinates_r10_c_ location_coordinates_r10;
  dyn_octstring               horizontal_velocity_r10;
  dyn_octstring               gnss_tod_msec_r10;
  // ...
  // group 0
  copy_ptr<vertical_velocity_info_r15_c_> vertical_velocity_info_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultCellListNR-r15 ::= SEQUENCE (SIZE (1..8)) OF MeasResultCellNR-r15
using meas_result_cell_list_nr_r15_l = dyn_array<meas_result_cell_nr_r15_s>;

// MeasResultGERAN ::= SEQUENCE
struct meas_result_geran_s {
  struct cgi_info_s_ {
    bool                   routing_area_code_present = false;
    cell_global_id_geran_s cell_global_id;
    fixed_bitstring<8>     routing_area_code;
  };
  struct meas_result_s_ {
    bool    ext  = false;
    uint8_t rssi = 0;
    // ...
  };

  // member variables
  bool                 cgi_info_present = false;
  carrier_freq_geran_s carrier_freq;
  pci_geran_s          pci;
  cgi_info_s_          cgi_info;
  meas_result_s_       meas_result;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultListEUTRA ::= SEQUENCE (SIZE (1..8)) OF MeasResultEUTRA
using meas_result_list_eutra_l = dyn_array<meas_result_eutra_s>;

// MeasResultListUTRA ::= SEQUENCE (SIZE (1..8)) OF MeasResultUTRA
using meas_result_list_utra_l = dyn_array<meas_result_utra_s>;

// MeasResultsCDMA2000 ::= SEQUENCE
struct meas_results_cdma2000_s {
  bool                        pre_regist_status_hrpd = false;
  meas_result_list_cdma2000_l meas_result_list_cdma2000;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RSRQ-Type-r12 ::= SEQUENCE
struct rsrq_type_r12_s {
  bool all_symbols_r12 = false;
  bool wide_band_r12   = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WLAN-RTT-r15 ::= SEQUENCE
struct wlan_rtt_r15_s {
  struct rtt_units_r15_opts {
    enum options {
      microseconds,
      hundredsofnanoseconds,
      tensofnanoseconds,
      nanoseconds,
      tenthsofnanoseconds,
      /*...*/ nulltype
    } value;

    const char* to_string() const;
  };
  typedef enumerated<rtt_units_r15_opts, true> rtt_units_r15_e_;

  // member variables
  bool             ext                      = false;
  bool             rtt_accuracy_r15_present = false;
  uint32_t         rtt_value_r15            = 0;
  rtt_units_r15_e_ rtt_units_r15;
  uint16_t         rtt_accuracy_r15 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LogMeasResultBT-r15 ::= SEQUENCE
struct log_meas_result_bt_r15_s {
  bool                ext                 = false;
  bool                rssi_bt_r15_present = false;
  fixed_bitstring<48> bt_addr_r15;
  int16_t             rssi_bt_r15 = -128;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LogMeasResultWLAN-r15 ::= SEQUENCE
struct log_meas_result_wlan_r15_s {
  bool           ext                   = false;
  bool           rssi_wlan_r15_present = false;
  bool           rtt_wlan_r15_present  = false;
  wlan_ids_r12_s wlan_ids_r15;
  uint8_t        rssi_wlan_r15 = 0;
  wlan_rtt_r15_s rtt_wlan_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResult2CDMA2000-r9 ::= SEQUENCE
struct meas_result2_cdma2000_r9_s {
  carrier_freq_cdma2000_s carrier_freq_r9;
  meas_results_cdma2000_s meas_result_list_r9;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResult2EUTRA-r9 ::= SEQUENCE
struct meas_result2_eutra_r9_s {
  uint32_t                 carrier_freq_r9 = 0;
  meas_result_list_eutra_l meas_result_list_r9;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResult2EUTRA-v1250 ::= SEQUENCE
struct meas_result2_eutra_v1250_s {
  bool            rsrq_type_r12_present = false;
  rsrq_type_r12_s rsrq_type_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResult2EUTRA-v9e0 ::= SEQUENCE
struct meas_result2_eutra_v9e0_s {
  bool     carrier_freq_v9e0_present = false;
  uint32_t carrier_freq_v9e0         = 65536;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResult2UTRA-r9 ::= SEQUENCE
struct meas_result2_utra_r9_s {
  uint16_t                carrier_freq_r9 = 0;
  meas_result_list_utra_l meas_result_list_r9;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultFreqFailNR-r15 ::= SEQUENCE
struct meas_result_freq_fail_nr_r15_s {
  bool                           ext                               = false;
  bool                           meas_result_cell_list_r15_present = false;
  uint32_t                       carrier_freq_r15                  = 0;
  meas_result_cell_list_nr_r15_l meas_result_cell_list_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultListGERAN ::= SEQUENCE (SIZE (1..8)) OF MeasResultGERAN
using meas_result_list_geran_l = dyn_array<meas_result_geran_s>;

// MeasResultMBSFN-r12 ::= SEQUENCE
struct meas_result_mbsfn_r12_s {
  struct mbsfn_area_r12_s_ {
    uint16_t mbsfn_area_id_r12 = 0;
    uint32_t carrier_freq_r12  = 0;
  };

  // member variables
  bool                            ext                                   = false;
  bool                            sig_bler_result_r12_present           = false;
  bool                            data_bler_mch_result_list_r12_present = false;
  mbsfn_area_r12_s_               mbsfn_area_r12;
  uint8_t                         rsrp_result_mbsfn_r12 = 0;
  uint8_t                         rsrq_result_mbsfn_r12 = 0;
  bler_result_r12_s               sig_bler_result_r12;
  data_bler_mch_result_list_r12_l data_bler_mch_result_list_r12;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LogMeasResultListBT-r15 ::= SEQUENCE (SIZE (1..32)) OF LogMeasResultBT-r15
using log_meas_result_list_bt_r15_l = dyn_array<log_meas_result_bt_r15_s>;

// LogMeasResultListWLAN-r15 ::= SEQUENCE (SIZE (1..32)) OF LogMeasResultWLAN-r15
using log_meas_result_list_wlan_r15_l = dyn_array<log_meas_result_wlan_r15_s>;

// MeasResultFreqListNR-r16 ::= SEQUENCE (SIZE (1..7)) OF MeasResultFreqFailNR-r15
using meas_result_freq_list_nr_r16_l = dyn_array<meas_result_freq_fail_nr_r15_s>;

// MeasResultList2CDMA2000-r9 ::= SEQUENCE (SIZE (1..8)) OF MeasResult2CDMA2000-r9
using meas_result_list2_cdma2000_r9_l = dyn_array<meas_result2_cdma2000_r9_s>;

// MeasResultList2EUTRA-r9 ::= SEQUENCE (SIZE (1..8)) OF MeasResult2EUTRA-r9
using meas_result_list2_eutra_r9_l = dyn_array<meas_result2_eutra_r9_s>;

// MeasResultList2EUTRA-v1250 ::= SEQUENCE (SIZE (1..8)) OF MeasResult2EUTRA-v1250
using meas_result_list2_eutra_v1250_l = dyn_array<meas_result2_eutra_v1250_s>;

// MeasResultList2EUTRA-v9e0 ::= SEQUENCE (SIZE (1..8)) OF MeasResult2EUTRA-v9e0
using meas_result_list2_eutra_v9e0_l = dyn_array<meas_result2_eutra_v9e0_s>;

// MeasResultList2GERAN-r10 ::= SEQUENCE (SIZE (1..3)) OF MeasResultListGERAN
using meas_result_list2_geran_r10_l = dyn_array<meas_result_list_geran_l>;

// MeasResultList2UTRA-r9 ::= SEQUENCE (SIZE (1..8)) OF MeasResult2UTRA-r9
using meas_result_list2_utra_r9_l = dyn_array<meas_result2_utra_r9_s>;

// MeasResultListMBSFN-r12 ::= SEQUENCE (SIZE (1..8)) OF MeasResultMBSFN-r12
using meas_result_list_mbsfn_r12_l = dyn_array<meas_result_mbsfn_r12_s>;

// LogMeasInfo-r10 ::= SEQUENCE
struct log_meas_info_r10_s {
  struct meas_result_serv_cell_r10_s_ {
    uint8_t rsrp_result_r10 = 0;
    uint8_t rsrq_result_r10 = 0;
  };
  struct meas_result_neigh_cells_r10_s_ {
    bool                            meas_result_list_eutra_r10_present    = false;
    bool                            meas_result_list_utra_r10_present     = false;
    bool                            meas_result_list_geran_r10_present    = false;
    bool                            meas_result_list_cdma2000_r10_present = false;
    meas_result_list2_eutra_r9_l    meas_result_list_eutra_r10;
    meas_result_list2_utra_r9_l     meas_result_list_utra_r10;
    meas_result_list2_geran_r10_l   meas_result_list_geran_r10;
    meas_result_list2_cdma2000_r9_l meas_result_list_cdma2000_r10;
  };
  struct meas_result_list_nr_v1640_s_ {
    uint32_t carrier_freq_nr_r16 = 0;
  };

  // member variables
  bool                           ext                                 = false;
  bool                           location_info_r10_present           = false;
  bool                           meas_result_neigh_cells_r10_present = false;
  location_info_r10_s            location_info_r10;
  uint16_t                       relative_time_stamp_r10 = 0;
  cell_global_id_eutra_s         serv_cell_id_r10;
  meas_result_serv_cell_r10_s_   meas_result_serv_cell_r10;
  meas_result_neigh_cells_r10_s_ meas_result_neigh_cells_r10;
  // ...
  // group 0
  copy_ptr<meas_result_list2_eutra_v9e0_l> meas_result_list_eutra_v1090;
  // group 1
  bool                                      meas_result_serv_cell_v1250_present = false;
  copy_ptr<meas_result_list_mbsfn_r12_l>    meas_result_list_mbsfn_r12;
  int8_t                                    meas_result_serv_cell_v1250 = -30;
  copy_ptr<rsrq_type_r12_s>                 serv_cell_rsrq_type_r12;
  copy_ptr<meas_result_list2_eutra_v1250_l> meas_result_list_eutra_v1250;
  // group 2
  bool in_dev_coex_detected_r13_present = false;
  // group 3
  bool   meas_result_serv_cell_v1360_present = false;
  int8_t meas_result_serv_cell_v1360         = -17;
  // group 4
  copy_ptr<log_meas_result_list_bt_r15_l>   log_meas_result_list_bt_r15;
  copy_ptr<log_meas_result_list_wlan_r15_l> log_meas_result_list_wlan_r15;
  // group 5
  bool any_cell_sel_detected_r15_present = false;
  // group 6
  copy_ptr<meas_result_cell_list_nr_r15_l> meas_result_list_nr_r16;
  // group 7
  copy_ptr<meas_result_list_nr_v1640_s_>   meas_result_list_nr_v1640;
  copy_ptr<meas_result_freq_list_nr_r16_l> meas_result_list_ext_nr_r16;
  // group 8
  bool          uncom_bar_pre_meas_result_r17_present = false;
  dyn_octstring uncom_bar_pre_meas_result_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LogMeasInfoList-r10 ::= SEQUENCE (SIZE (1..520)) OF LogMeasInfo-r10
using log_meas_info_list_r10_l = dyn_array<log_meas_info_r10_s>;

// LogMeasReport-r10 ::= SEQUENCE
struct log_meas_report_r10_s {
  bool                     ext                            = false;
  bool                     log_meas_available_r10_present = false;
  fixed_bitstring<48>      absolute_time_stamp_r10;
  trace_ref_r10_s          trace_ref_r10;
  fixed_octstring<2>       trace_recording_session_ref_r10;
  fixed_octstring<1>       tce_id_r10;
  log_meas_info_list_r10_l log_meas_info_list_r10;
  // ...
  // group 0
  bool log_meas_available_bt_r15_present   = false;
  bool log_meas_available_wlan_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultCBR-r14 ::= SEQUENCE
struct meas_result_cbr_r14_s {
  bool    cbr_pscch_r14_present = false;
  uint8_t pool_id_r14           = 1;
  uint8_t cbr_pssch_r14         = 0;
  uint8_t cbr_pscch_r14         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultCSI-RS-r12 ::= SEQUENCE
struct meas_result_csi_rs_r12_s {
  bool    ext                 = false;
  uint8_t meas_csi_rs_id_r12  = 1;
  uint8_t csi_rsrp_result_r12 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultCellSFTD-r15 ::= SEQUENCE
struct meas_result_cell_sftd_r15_s {
  bool     rsrp_result_r15_present          = false;
  uint16_t pci_r15                          = 0;
  uint16_t sfn_offset_result_r15            = 0;
  int16_t  frame_boundary_offset_result_r15 = -30720;
  uint8_t  rsrp_result_r15                  = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultServFreq-r10 ::= SEQUENCE
struct meas_result_serv_freq_r10_s {
  struct meas_result_scell_r10_s_ {
    uint8_t rsrp_result_scell_r10 = 0;
    uint8_t rsrq_result_scell_r10 = 0;
  };
  struct meas_result_best_neigh_cell_r10_s_ {
    uint16_t pci_r10               = 0;
    uint8_t  rsrp_result_ncell_r10 = 0;
    uint8_t  rsrq_result_ncell_r10 = 0;
  };
  struct meas_result_scell_v1310_s_ {
    uint8_t rs_sinr_result_r13 = 0;
  };
  struct meas_result_best_neigh_cell_v1310_s_ {
    uint8_t rs_sinr_result_r13 = 0;
  };

  // member variables
  bool                               ext                                     = false;
  bool                               meas_result_scell_r10_present           = false;
  bool                               meas_result_best_neigh_cell_r10_present = false;
  uint8_t                            serv_freq_id_r10                        = 0;
  meas_result_scell_r10_s_           meas_result_scell_r10;
  meas_result_best_neigh_cell_r10_s_ meas_result_best_neigh_cell_r10;
  // ...
  // group 0
  bool   meas_result_scell_v1250_present           = false;
  bool   meas_result_best_neigh_cell_v1250_present = false;
  int8_t meas_result_scell_v1250                   = -30;
  int8_t meas_result_best_neigh_cell_v1250         = -30;
  // group 1
  copy_ptr<meas_result_scell_v1310_s_>           meas_result_scell_v1310;
  copy_ptr<meas_result_best_neigh_cell_v1310_s_> meas_result_best_neigh_cell_v1310;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultServFreq-r13 ::= SEQUENCE
struct meas_result_serv_freq_r13_s {
  struct meas_result_scell_r13_s_ {
    bool    rs_sinr_result_r13_present = false;
    uint8_t rsrp_result_scell_r13      = 0;
    int8_t  rsrq_result_scell_r13      = -30;
    uint8_t rs_sinr_result_r13         = 0;
  };
  struct meas_result_best_neigh_cell_r13_s_ {
    bool     rs_sinr_result_r13_present = false;
    uint16_t pci_r13                    = 0;
    uint8_t  rsrp_result_ncell_r13      = 0;
    int8_t   rsrq_result_ncell_r13      = -30;
    uint8_t  rs_sinr_result_r13         = 0;
  };
  struct meas_result_best_neigh_cell_v1360_s_ {
    int8_t rsrp_result_ncell_v1360 = -17;
  };

  // member variables
  bool                               ext                                     = false;
  bool                               meas_result_scell_r13_present           = false;
  bool                               meas_result_best_neigh_cell_r13_present = false;
  uint8_t                            serv_freq_id_r13                        = 0;
  meas_result_scell_r13_s_           meas_result_scell_r13;
  meas_result_best_neigh_cell_r13_s_ meas_result_best_neigh_cell_r13;
  // ...
  // group 0
  copy_ptr<meas_result_best_neigh_cell_v1360_s_> meas_result_best_neigh_cell_v1360;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultServFreqNR-r15 ::= SEQUENCE
struct meas_result_serv_freq_nr_r15_s {
  bool                      ext                                     = false;
  bool                      meas_result_scell_r15_present           = false;
  bool                      meas_result_best_neigh_cell_r15_present = false;
  uint32_t                  carrier_freq_r15                        = 0;
  meas_result_cell_nr_r15_s meas_result_scell_r15;
  meas_result_cell_nr_r15_s meas_result_best_neigh_cell_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultWLAN-r13 ::= SEQUENCE
struct meas_result_wlan_r13_s {
  bool                     ext                                           = false;
  bool                     carrier_info_wlan_r13_present                 = false;
  bool                     band_wlan_r13_present                         = false;
  bool                     available_admission_capacity_wlan_r13_present = false;
  bool                     backhaul_dl_bw_wlan_r13_present               = false;
  bool                     backhaul_ul_bw_wlan_r13_present               = false;
  bool                     ch_utilization_wlan_r13_present               = false;
  bool                     station_count_wlan_r13_present                = false;
  bool                     connected_wlan_r13_present                    = false;
  wlan_ids_r12_s           wlan_ids_r13;
  wlan_carrier_info_r13_s  carrier_info_wlan_r13;
  wlan_band_ind_r13_e      band_wlan_r13;
  uint8_t                  rssi_wlan_r13                         = 0;
  uint16_t                 available_admission_capacity_wlan_r13 = 0;
  wlan_backhaul_rate_r12_e backhaul_dl_bw_wlan_r13;
  wlan_backhaul_rate_r12_e backhaul_ul_bw_wlan_r13;
  uint16_t                 ch_utilization_wlan_r13 = 0;
  uint32_t                 station_count_wlan_r13  = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SensingResult-r15 ::= SEQUENCE
struct sensing_result_r15_s {
  uint16_t res_idx_r15 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-PDCP-DelayResult-r13 ::= SEQUENCE
struct ul_pdcp_delay_result_r13_s {
  struct qci_id_r13_opts {
    enum options { qci1, qci2, qci3, qci4, spare4, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<qci_id_r13_opts> qci_id_r13_e_;

  // member variables
  bool          ext = false;
  qci_id_r13_e_ qci_id_r13;
  uint8_t       excess_delay_r13 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-PDCP-DelayValueResult-r16 ::= SEQUENCE
struct ul_pdcp_delay_value_result_r16_s {
  bool     ext               = false;
  uint8_t  drb_id_r16        = 1;
  uint16_t average_delay_r16 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultCSI-RS-List-r12 ::= SEQUENCE (SIZE (1..8)) OF MeasResultCSI-RS-r12
using meas_result_csi_rs_list_r12_l = dyn_array<meas_result_csi_rs_r12_s>;

// MeasResultCellListSFTD-r15 ::= SEQUENCE (SIZE (1..3)) OF MeasResultCellSFTD-r15
using meas_result_cell_list_sftd_r15_l = dyn_array<meas_result_cell_sftd_r15_s>;

// MeasResultForECID-r9 ::= SEQUENCE
struct meas_result_for_ecid_r9_s {
  uint16_t            ue_rx_tx_time_diff_result_r9 = 0;
  fixed_bitstring<10> current_sfn_r9;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultForRSSI-NR-r16 ::= SEQUENCE
struct meas_result_for_rssi_nr_r16_s {
  bool    ext                 = false;
  uint8_t rssi_result_nr_r16  = 0;
  uint8_t ch_occupancy_nr_r16 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultForRSSI-r13 ::= SEQUENCE
struct meas_result_for_rssi_r13_s {
  bool    ext              = false;
  uint8_t rssi_result_r13  = 0;
  uint8_t ch_occupancy_r13 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultListCBR-r14 ::= SEQUENCE (SIZE (1..72)) OF MeasResultCBR-r14
using meas_result_list_cbr_r14_l = dyn_array<meas_result_cbr_r14_s>;

// MeasResultListWLAN-r13 ::= SEQUENCE (SIZE (1..8)) OF MeasResultWLAN-r13
using meas_result_list_wlan_r13_l = dyn_array<meas_result_wlan_r13_s>;

// MeasResultListWLAN-r14 ::= SEQUENCE (SIZE (1..32)) OF MeasResultWLAN-r13
using meas_result_list_wlan_r14_l = dyn_array<meas_result_wlan_r13_s>;

// MeasResultSSTD-r13 ::= SEQUENCE
struct meas_result_sstd_r13_s {
  uint16_t sfn_offset_result_r13            = 0;
  int8_t   frame_boundary_offset_result_r13 = -5;
  uint8_t  sf_boundary_offset_result_r13    = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultSensing-r15 ::= SEQUENCE
struct meas_result_sensing_r15_s {
  using sensing_result_r15_l_ = dyn_array<sensing_result_r15_s>;

  // member variables
  uint16_t              sl_sf_ref_r15 = 0;
  sensing_result_r15_l_ sensing_result_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultServFreqList-r10 ::= SEQUENCE (SIZE (1..5)) OF MeasResultServFreq-r10
using meas_result_serv_freq_list_r10_l = dyn_array<meas_result_serv_freq_r10_s>;

// MeasResultServFreqListExt-r13 ::= SEQUENCE (SIZE (1..32)) OF MeasResultServFreq-r13
using meas_result_serv_freq_list_ext_r13_l = dyn_array<meas_result_serv_freq_r13_s>;

// MeasResultServFreqListNR-r15 ::= SEQUENCE (SIZE (1..32)) OF MeasResultServFreqNR-r15
using meas_result_serv_freq_list_nr_r15_l = dyn_array<meas_result_serv_freq_nr_r15_s>;

// UL-PDCP-DelayResultList-r13 ::= SEQUENCE (SIZE (1..6)) OF UL-PDCP-DelayResult-r13
using ul_pdcp_delay_result_list_r13_l = dyn_array<ul_pdcp_delay_result_r13_s>;

// UL-PDCP-DelayValueResultList-r16 ::= SEQUENCE (SIZE (1..11)) OF UL-PDCP-DelayValueResult-r16
using ul_pdcp_delay_value_result_list_r16_l = dyn_array<ul_pdcp_delay_value_result_r16_s>;

// MeasResults ::= SEQUENCE
struct meas_results_s {
  struct meas_result_pcell_s_ {
    uint8_t rsrp_result = 0;
    uint8_t rsrq_result = 0;
  };
  struct meas_result_neigh_cells_c_ {
    struct types_opts {
      enum options {
        meas_result_list_eutra,
        meas_result_list_utra,
        meas_result_list_geran,
        meas_results_cdma2000,
        // ...
        meas_result_neigh_cell_list_nr_r15,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<types_opts, true, 1> types;

    // choice methods
    meas_result_neigh_cells_c_() = default;
    meas_result_neigh_cells_c_(const meas_result_neigh_cells_c_& other);
    meas_result_neigh_cells_c_& operator=(const meas_result_neigh_cells_c_& other);
    ~meas_result_neigh_cells_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    meas_result_list_eutra_l& meas_result_list_eutra()
    {
      assert_choice_type(types::meas_result_list_eutra, type_, "measResultNeighCells");
      return c.get<meas_result_list_eutra_l>();
    }
    meas_result_list_utra_l& meas_result_list_utra()
    {
      assert_choice_type(types::meas_result_list_utra, type_, "measResultNeighCells");
      return c.get<meas_result_list_utra_l>();
    }
    meas_result_list_geran_l& meas_result_list_geran()
    {
      assert_choice_type(types::meas_result_list_geran, type_, "measResultNeighCells");
      return c.get<meas_result_list_geran_l>();
    }
    meas_results_cdma2000_s& meas_results_cdma2000()
    {
      assert_choice_type(types::meas_results_cdma2000, type_, "measResultNeighCells");
      return c.get<meas_results_cdma2000_s>();
    }
    meas_result_cell_list_nr_r15_l& meas_result_neigh_cell_list_nr_r15()
    {
      assert_choice_type(types::meas_result_neigh_cell_list_nr_r15, type_, "measResultNeighCells");
      return c.get<meas_result_cell_list_nr_r15_l>();
    }
    const meas_result_list_eutra_l& meas_result_list_eutra() const
    {
      assert_choice_type(types::meas_result_list_eutra, type_, "measResultNeighCells");
      return c.get<meas_result_list_eutra_l>();
    }
    const meas_result_list_utra_l& meas_result_list_utra() const
    {
      assert_choice_type(types::meas_result_list_utra, type_, "measResultNeighCells");
      return c.get<meas_result_list_utra_l>();
    }
    const meas_result_list_geran_l& meas_result_list_geran() const
    {
      assert_choice_type(types::meas_result_list_geran, type_, "measResultNeighCells");
      return c.get<meas_result_list_geran_l>();
    }
    const meas_results_cdma2000_s& meas_results_cdma2000() const
    {
      assert_choice_type(types::meas_results_cdma2000, type_, "measResultNeighCells");
      return c.get<meas_results_cdma2000_s>();
    }
    const meas_result_cell_list_nr_r15_l& meas_result_neigh_cell_list_nr_r15() const
    {
      assert_choice_type(types::meas_result_neigh_cell_list_nr_r15, type_, "measResultNeighCells");
      return c.get<meas_result_cell_list_nr_r15_l>();
    }
    meas_result_list_eutra_l&       set_meas_result_list_eutra();
    meas_result_list_utra_l&        set_meas_result_list_utra();
    meas_result_list_geran_l&       set_meas_result_list_geran();
    meas_results_cdma2000_s&        set_meas_results_cdma2000();
    meas_result_cell_list_nr_r15_l& set_meas_result_neigh_cell_list_nr_r15();

  private:
    types type_;
    choice_buffer_t<meas_result_cell_list_nr_r15_l,
                    meas_result_list_eutra_l,
                    meas_result_list_geran_l,
                    meas_result_list_utra_l,
                    meas_results_cdma2000_s>
        c;

    void destroy_();
  };
  struct meas_result_pcell_v1310_s_ {
    uint8_t rs_sinr_result_r13 = 0;
  };

  // member variables
  bool                       ext                             = false;
  bool                       meas_result_neigh_cells_present = false;
  uint8_t                    meas_id                         = 1;
  meas_result_pcell_s_       meas_result_pcell;
  meas_result_neigh_cells_c_ meas_result_neigh_cells;
  // ...
  // group 0
  copy_ptr<meas_result_for_ecid_r9_s> meas_result_for_ecid_r9;
  // group 1
  copy_ptr<location_info_r10_s>              location_info_r10;
  copy_ptr<meas_result_serv_freq_list_r10_l> meas_result_serv_freq_list_r10;
  // group 2
  bool                                    meas_id_v1250_present           = false;
  bool                                    meas_result_pcell_v1250_present = false;
  uint8_t                                 meas_id_v1250                   = 33;
  int8_t                                  meas_result_pcell_v1250         = -30;
  copy_ptr<meas_result_csi_rs_list_r12_l> meas_result_csi_rs_list_r12;
  // group 3
  copy_ptr<meas_result_for_rssi_r13_s>           meas_result_for_rssi_r13;
  copy_ptr<meas_result_serv_freq_list_ext_r13_l> meas_result_serv_freq_list_ext_r13;
  copy_ptr<meas_result_sstd_r13_s>               meas_result_sstd_r13;
  copy_ptr<meas_result_pcell_v1310_s_>           meas_result_pcell_v1310;
  copy_ptr<ul_pdcp_delay_result_list_r13_l>      ul_pdcp_delay_result_list_r13;
  copy_ptr<meas_result_list_wlan_r13_l>          meas_result_list_wlan_r13;
  // group 4
  bool   meas_result_pcell_v1360_present = false;
  int8_t meas_result_pcell_v1360         = -17;
  // group 5
  copy_ptr<meas_result_list_cbr_r14_l>  meas_result_list_cbr_r14;
  copy_ptr<meas_result_list_wlan_r14_l> meas_result_list_wlan_r14;
  // group 6
  copy_ptr<meas_result_serv_freq_list_nr_r15_l> meas_result_serv_freq_list_nr_r15;
  copy_ptr<meas_result_cell_list_sftd_r15_l>    meas_result_cell_list_sftd_r15;
  // group 7
  bool                                      height_ue_r15_present = false;
  copy_ptr<log_meas_result_list_bt_r15_l>   log_meas_result_list_bt_r15;
  copy_ptr<log_meas_result_list_wlan_r15_l> log_meas_result_list_wlan_r15;
  copy_ptr<meas_result_sensing_r15_s>       meas_result_sensing_r15;
  int16_t                                   height_ue_r15 = -400;
  // group 8
  copy_ptr<ul_pdcp_delay_value_result_list_r16_l> ul_pdcp_delay_value_result_list_r16;
  copy_ptr<meas_result_for_rssi_nr_r16_s>         meas_result_for_rssi_nr_r16;
  // group 9
  bool          uncom_bar_pre_meas_result_r17_present = false;
  bool          coarse_location_info_r17_present      = false;
  dyn_octstring uncom_bar_pre_meas_result_r17;
  dyn_octstring coarse_location_info_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementReport-v8a0-IEs ::= SEQUENCE
struct meas_report_v8a0_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementReport-r8-IEs ::= SEQUENCE
struct meas_report_r8_ies_s {
  bool                   non_crit_ext_present = false;
  meas_results_s         meas_results;
  meas_report_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementReport ::= SEQUENCE
struct meas_report_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { meas_report_r8, spare7, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;

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
      meas_report_r8_ies_s& meas_report_r8()
      {
        assert_choice_type(types::meas_report_r8, type_, "c1");
        return c;
      }
      const meas_report_r8_ies_s& meas_report_r8() const
      {
        assert_choice_type(types::meas_report_r8, type_, "c1");
        return c;
      }
      meas_report_r8_ies_s& set_meas_report_r8();
      void                  set_spare7();
      void                  set_spare6();
      void                  set_spare5();
      void                  set_spare4();
      void                  set_spare3();
      void                  set_spare2();
      void                  set_spare1();

    private:
      types                type_;
      meas_report_r8_ies_s c;
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

// MobilityControlInfo-v10l0 ::= SEQUENCE
struct mob_ctrl_info_v10l0_s {
  bool     add_spec_emission_v10l0_present = false;
  uint16_t add_spec_emission_v10l0         = 33;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc
} // namespace asn1

#endif // SRSASN1_RRC_MEASCFG_H
