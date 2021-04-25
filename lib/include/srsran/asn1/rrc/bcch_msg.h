/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#ifndef SRSASN1_RRC_BCCH_MSG_H
#define SRSASN1_RRC_BCCH_MSG_H

#include "meascfg.h"
#include "si.h"

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// BCCH-BCH-MessageType ::= MasterInformationBlock
using bcch_bch_msg_type_s = mib_s;

// BCCH-BCH-Message ::= SEQUENCE
struct bcch_bch_msg_s {
  bcch_bch_msg_type_s msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandClassInfoCDMA2000 ::= SEQUENCE
struct band_class_info_cdma2000_s {
  bool                 ext                     = false;
  bool                 cell_resel_prio_present = false;
  bandclass_cdma2000_e band_class;
  uint8_t              cell_resel_prio = 0;
  uint8_t              thresh_x_high   = 0;
  uint8_t              thresh_x_low    = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeighCellsPerBandclassCDMA2000-r11 ::= SEQUENCE
struct neigh_cells_per_bandclass_cdma2000_r11_s {
  using pci_list_r11_l_ = dyn_array<uint16_t>;

  // member variables
  uint16_t        arfcn = 0;
  pci_list_r11_l_ pci_list_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhysCellIdListCDMA2000 ::= SEQUENCE (SIZE (1..16)) OF INTEGER (0..511)
using pci_list_cdma2000_l = bounded_array<uint16_t, 16>;

// PhysCellIdListCDMA2000-v920 ::= SEQUENCE (SIZE (0..24)) OF INTEGER (0..511)
using pci_list_cdma2000_v920_l = bounded_array<uint16_t, 24>;

// BandClassListCDMA2000 ::= SEQUENCE (SIZE (1..32)) OF BandClassInfoCDMA2000
using band_class_list_cdma2000_l = dyn_array<band_class_info_cdma2000_s>;

// NS-PmaxValue-r10 ::= SEQUENCE
struct ns_pmax_value_r10_s {
  bool    add_pmax_r10_present = false;
  int8_t  add_pmax_r10         = -30;
  uint8_t add_spec_emission    = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NS-PmaxValueNR-r15 ::= SEQUENCE
struct ns_pmax_value_nr_r15_s {
  bool    add_pmax_nr_r15_present  = false;
  int8_t  add_pmax_nr_r15          = -30;
  uint8_t add_spec_emission_nr_r15 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeighCellCDMA2000-r11 ::= SEQUENCE
struct neigh_cell_cdma2000_r11_s {
  using neigh_freq_info_list_r11_l_ = dyn_array<neigh_cells_per_bandclass_cdma2000_r11_s>;

  // member variables
  bandclass_cdma2000_e        band_class;
  neigh_freq_info_list_r11_l_ neigh_freq_info_list_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeighCellsPerBandclassCDMA2000 ::= SEQUENCE
struct neigh_cells_per_bandclass_cdma2000_s {
  uint16_t            arfcn = 0;
  pci_list_cdma2000_l pci_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeighCellsPerBandclassCDMA2000-v920 ::= SEQUENCE
struct neigh_cells_per_bandclass_cdma2000_v920_s {
  pci_list_cdma2000_v920_l pci_list_v920;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RedistributionNeighCell-r13 ::= SEQUENCE
struct redist_neigh_cell_r13_s {
  uint16_t pci_r13                = 0;
  uint8_t  redist_factor_cell_r13 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AC-BarringConfig1XRTT-r9 ::= SEQUENCE
struct ac_barr_cfg1_xrtt_r9_s {
  uint8_t ac_barr0to9_r9 = 0;
  uint8_t ac_barr10_r9   = 0;
  uint8_t ac_barr11_r9   = 0;
  uint8_t ac_barr12_r9   = 0;
  uint8_t ac_barr13_r9   = 0;
  uint8_t ac_barr14_r9   = 0;
  uint8_t ac_barr15_r9   = 0;
  uint8_t ac_barr_msg_r9 = 0;
  uint8_t ac_barr_reg_r9 = 0;
  uint8_t ac_barr_emg_r9 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSFB-RegistrationParam1XRTT ::= SEQUENCE
struct csfb_regist_param1_xrtt_s {
  fixed_bitstring<15> sid;
  fixed_bitstring<16> nid;
  bool                multiple_sid    = false;
  bool                multiple_nid    = false;
  bool                home_reg        = false;
  bool                foreign_sid_reg = false;
  bool                foreign_nid_reg = false;
  bool                param_reg       = false;
  bool                pwr_up_reg      = false;
  fixed_bitstring<7>  regist_period;
  fixed_bitstring<12> regist_zone;
  fixed_bitstring<3>  total_zone;
  fixed_bitstring<3>  zone_timer;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSFB-RegistrationParam1XRTT-v920 ::= SEQUENCE
struct csfb_regist_param1_xrtt_v920_s {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellReselectionParametersCDMA2000-r11 ::= SEQUENCE
struct cell_resel_params_cdma2000_r11_s {
  using neigh_cell_list_r11_l_ = dyn_array<neigh_cell_cdma2000_r11_s>;

  // member variables
  bool                        t_resel_cdma2000_sf_present = false;
  band_class_list_cdma2000_l  band_class_list;
  neigh_cell_list_r11_l_      neigh_cell_list_r11;
  uint8_t                     t_resel_cdma2000 = 0;
  speed_state_scale_factors_s t_resel_cdma2000_sf;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqNeighCellInfo ::= SEQUENCE
struct inter_freq_neigh_cell_info_s {
  uint16_t         pci = 0;
  q_offset_range_e q_offset_cell;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NS-PmaxList-r10 ::= SEQUENCE (SIZE (1..8)) OF NS-PmaxValue-r10
using ns_pmax_list_r10_l = dyn_array<ns_pmax_value_r10_s>;

// NS-PmaxListNR-r15 ::= SEQUENCE (SIZE (1..8)) OF NS-PmaxValueNR-r15
using ns_pmax_list_nr_r15_l = dyn_array<ns_pmax_value_nr_r15_s>;

// NeighCellsPerBandclassListCDMA2000 ::= SEQUENCE (SIZE (1..16)) OF NeighCellsPerBandclassCDMA2000
using neigh_cells_per_bandclass_list_cdma2000_l = dyn_array<neigh_cells_per_bandclass_cdma2000_s>;

// NeighCellsPerBandclassListCDMA2000-v920 ::= SEQUENCE (SIZE (1..16)) OF NeighCellsPerBandclassCDMA2000-v920
using neigh_cells_per_bandclass_list_cdma2000_v920_l = dyn_array<neigh_cells_per_bandclass_cdma2000_v920_s>;

// PLMN-IdentityInfo2-r12 ::= CHOICE
struct plmn_id_info2_r12_c {
  struct types_opts {
    enum options { plmn_idx_r12, plmn_id_r12, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  plmn_id_info2_r12_c() = default;
  plmn_id_info2_r12_c(const plmn_id_info2_r12_c& other);
  plmn_id_info2_r12_c& operator=(const plmn_id_info2_r12_c& other);
  ~plmn_id_info2_r12_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& plmn_idx_r12()
  {
    assert_choice_type(types::plmn_idx_r12, type_, "PLMN-IdentityInfo2-r12");
    return c.get<uint8_t>();
  }
  plmn_id_s& plmn_id_r12()
  {
    assert_choice_type(types::plmn_id_r12, type_, "PLMN-IdentityInfo2-r12");
    return c.get<plmn_id_s>();
  }
  const uint8_t& plmn_idx_r12() const
  {
    assert_choice_type(types::plmn_idx_r12, type_, "PLMN-IdentityInfo2-r12");
    return c.get<uint8_t>();
  }
  const plmn_id_s& plmn_id_r12() const
  {
    assert_choice_type(types::plmn_id_r12, type_, "PLMN-IdentityInfo2-r12");
    return c.get<plmn_id_s>();
  }
  uint8_t&   set_plmn_idx_r12();
  plmn_id_s& set_plmn_id_r12();

private:
  types                      type_;
  choice_buffer_t<plmn_id_s> c;

  void destroy_();
};

// RedistributionNeighCellList-r13 ::= SEQUENCE (SIZE (1..16)) OF RedistributionNeighCell-r13
using redist_neigh_cell_list_r13_l = dyn_array<redist_neigh_cell_r13_s>;

// SL-SyncConfigListNFreq-r13 ::= SEQUENCE (SIZE (1..16)) OF SL-SyncConfigNFreq-r13
using sl_sync_cfg_list_nfreq_r13_l = dyn_array<sl_sync_cfg_nfreq_r13_s>;

// SystemTimeInfoCDMA2000 ::= SEQUENCE
struct sys_time_info_cdma2000_s {
  struct cdma_sys_time_c_ {
    struct types_opts {
      enum options { sync_sys_time, async_sys_time, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    cdma_sys_time_c_() = default;
    cdma_sys_time_c_(const cdma_sys_time_c_& other);
    cdma_sys_time_c_& operator=(const cdma_sys_time_c_& other);
    ~cdma_sys_time_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<39>& sync_sys_time()
    {
      assert_choice_type(types::sync_sys_time, type_, "cdma-SystemTime");
      return c.get<fixed_bitstring<39> >();
    }
    fixed_bitstring<49>& async_sys_time()
    {
      assert_choice_type(types::async_sys_time, type_, "cdma-SystemTime");
      return c.get<fixed_bitstring<49> >();
    }
    const fixed_bitstring<39>& sync_sys_time() const
    {
      assert_choice_type(types::sync_sys_time, type_, "cdma-SystemTime");
      return c.get<fixed_bitstring<39> >();
    }
    const fixed_bitstring<49>& async_sys_time() const
    {
      assert_choice_type(types::async_sys_time, type_, "cdma-SystemTime");
      return c.get<fixed_bitstring<49> >();
    }
    fixed_bitstring<39>& set_sync_sys_time();
    fixed_bitstring<49>& set_async_sys_time();

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<49> > c;

    void destroy_();
  };

  // member variables
  bool             cdma_eutra_synchronisation = false;
  cdma_sys_time_c_ cdma_sys_time;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UAC-BarringPerCat-r15 ::= SEQUENCE
struct uac_barr_per_cat_r15_s {
  uint8_t access_category_r15       = 1;
  uint8_t uac_barr_info_set_idx_r15 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqBlackCellList ::= SEQUENCE (SIZE (1..16)) OF PhysCellIdRange
using inter_freq_black_cell_list_l = dyn_array<pci_range_s>;

// InterFreqNeighCellList ::= SEQUENCE (SIZE (1..16)) OF InterFreqNeighCellInfo
using inter_freq_neigh_cell_list_l = dyn_array<inter_freq_neigh_cell_info_s>;

// InterFreqNeighHSDN-CellList-r15 ::= SEQUENCE (SIZE (1..16)) OF PhysCellIdRange
using inter_freq_neigh_hsdn_cell_list_r15_l = dyn_array<pci_range_s>;

// MBMS-SAI-List-r11 ::= SEQUENCE (SIZE (1..64)) OF INTEGER (0..65535)
using mbms_sai_list_r11_l = dyn_array<uint32_t>;

// MultiBandInfoList-v10j0 ::= SEQUENCE (SIZE (1..8)) OF NS-PmaxList-r10
using multi_band_info_list_v10j0_l = dyn_array<ns_pmax_list_r10_l>;

// MultiBandNsPmaxListNR-1-v1550 ::= SEQUENCE (SIZE (1..31)) OF NS-PmaxListNR-r15
using multi_band_ns_pmax_list_nr_minus1_v1550_l = dyn_array<ns_pmax_list_nr_r15_l>;

// MultiBandNsPmaxListNR-v1550 ::= SEQUENCE (SIZE (1..32)) OF NS-PmaxListNR-r15
using multi_band_ns_pmax_list_nr_v1550_l = dyn_array<ns_pmax_list_nr_r15_l>;

// NeighCellCDMA2000 ::= SEQUENCE
struct neigh_cell_cdma2000_s {
  bandclass_cdma2000_e                      band_class;
  neigh_cells_per_bandclass_list_cdma2000_l neigh_cells_per_freq_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeighCellCDMA2000-v920 ::= SEQUENCE
struct neigh_cell_cdma2000_v920_s {
  neigh_cells_per_bandclass_list_cdma2000_v920_l neigh_cells_per_freq_list_v920;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityList4-r12 ::= SEQUENCE (SIZE (1..6)) OF PLMN-IdentityInfo2-r12
using plmn_id_list4_r12_l = dyn_array<plmn_id_info2_r12_c>;

// ParametersCDMA2000-r11 ::= SEQUENCE
struct params_cdma2000_r11_s {
  struct sys_time_info_r11_c_ {
    struct types_opts {
      enum options { explicit_value, default_value, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    sys_time_info_r11_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sys_time_info_cdma2000_s& explicit_value()
    {
      assert_choice_type(types::explicit_value, type_, "systemTimeInfo-r11");
      return c;
    }
    const sys_time_info_cdma2000_s& explicit_value() const
    {
      assert_choice_type(types::explicit_value, type_, "systemTimeInfo-r11");
      return c;
    }
    sys_time_info_cdma2000_s& set_explicit_value();
    void                      set_default_value();

  private:
    types                    type_;
    sys_time_info_cdma2000_s c;
  };
  struct params_hrpd_r11_s_ {
    bool                             cell_resel_params_hrpd_r11_present = false;
    pre_regist_info_hrpd_s           pre_regist_info_hrpd_r11;
    cell_resel_params_cdma2000_r11_s cell_resel_params_hrpd_r11;
  };
  struct params1_xrtt_r11_s_ {
    bool                             csfb_regist_param1_xrtt_r11_present      = false;
    bool                             csfb_regist_param1_xrtt_ext_r11_present  = false;
    bool                             long_code_state1_xrtt_r11_present        = false;
    bool                             cell_resel_params1_xrtt_r11_present      = false;
    bool                             ac_barr_cfg1_xrtt_r11_present            = false;
    bool                             csfb_support_for_dual_rx_ues_r11_present = false;
    bool                             csfb_dual_rx_tx_support_r11_present      = false;
    csfb_regist_param1_xrtt_s        csfb_regist_param1_xrtt_r11;
    csfb_regist_param1_xrtt_v920_s   csfb_regist_param1_xrtt_ext_r11;
    fixed_bitstring<42>              long_code_state1_xrtt_r11;
    cell_resel_params_cdma2000_r11_s cell_resel_params1_xrtt_r11;
    ac_barr_cfg1_xrtt_r9_s           ac_barr_cfg1_xrtt_r11;
    bool                             csfb_support_for_dual_rx_ues_r11 = false;
  };

  // member variables
  bool                 ext                       = false;
  bool                 sys_time_info_r11_present = false;
  bool                 params_hrpd_r11_present   = false;
  bool                 params1_xrtt_r11_present  = false;
  sys_time_info_r11_c_ sys_time_info_r11;
  uint8_t              search_win_size_r11 = 0;
  params_hrpd_r11_s_   params_hrpd_r11;
  params1_xrtt_r11_s_  params1_xrtt_r11;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RedistributionInterFreqInfo-r13 ::= SEQUENCE
struct redist_inter_freq_info_r13_s {
  bool                         redist_factor_freq_r13_present     = false;
  bool                         redist_neigh_cell_list_r13_present = false;
  uint8_t                      redist_factor_freq_r13             = 1;
  redist_neigh_cell_list_r13_l redist_neigh_cell_list_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-DiscConfigOtherInterFreq-r13 ::= SEQUENCE
struct sl_disc_cfg_other_inter_freq_r13_s {
  bool                           tx_pwr_info_r13_present        = false;
  bool                           ref_carrier_common_r13_present = false;
  bool                           disc_sync_cfg_r13_present      = false;
  bool                           disc_cell_sel_info_r13_present = false;
  sl_disc_tx_pwr_info_list_r12_l tx_pwr_info_r13;
  sl_sync_cfg_list_nfreq_r13_l   disc_sync_cfg_r13;
  cell_sel_info_nfreq_r13_s      disc_cell_sel_info_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UAC-BarringPerCatList-r15 ::= SEQUENCE (SIZE (1..63)) OF UAC-BarringPerCat-r15
using uac_barr_per_cat_list_r15_l = dyn_array<uac_barr_per_cat_r15_s>;

// CarrierFreqNR-r15 ::= SEQUENCE
struct carrier_freq_nr_r15_s {
  struct subcarrier_spacing_ssb_r15_opts {
    enum options { khz15, khz30, khz120, khz240, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<subcarrier_spacing_ssb_r15_opts> subcarrier_spacing_ssb_r15_e_;
  struct thresh_x_q_r15_s_ {
    uint8_t thresh_x_high_q_r15 = 0;
    uint8_t thresh_x_low_q_r15  = 0;
  };

  // member variables
  bool                          ext                                  = false;
  bool                          multi_band_info_list_r15_present     = false;
  bool                          multi_band_info_list_sul_r15_present = false;
  bool                          meas_timing_cfg_r15_present          = false;
  bool                          ss_rssi_meas_r15_present             = false;
  bool                          cell_resel_prio_r15_present          = false;
  bool                          cell_resel_sub_prio_r15_present      = false;
  bool                          thresh_x_q_r15_present               = false;
  bool                          q_rx_lev_min_sul_r15_present         = false;
  bool                          ns_pmax_list_nr_r15_present          = false;
  bool                          q_qual_min_r15_present               = false;
  bool                          max_rs_idx_cell_qual_r15_present     = false;
  bool                          thresh_rs_idx_r15_present            = false;
  uint32_t                      carrier_freq_r15                     = 0;
  multi_freq_band_list_nr_r15_l multi_band_info_list_r15;
  multi_freq_band_list_nr_r15_l multi_band_info_list_sul_r15;
  mtc_ssb_nr_r15_s              meas_timing_cfg_r15;
  subcarrier_spacing_ssb_r15_e_ subcarrier_spacing_ssb_r15;
  ss_rssi_meas_r15_s            ss_rssi_meas_r15;
  uint8_t                       cell_resel_prio_r15 = 0;
  cell_resel_sub_prio_r13_e     cell_resel_sub_prio_r15;
  uint8_t                       thresh_x_high_r15 = 0;
  uint8_t                       thresh_x_low_r15  = 0;
  thresh_x_q_r15_s_             thresh_x_q_r15;
  int8_t                        q_rx_lev_min_r15     = -70;
  int8_t                        q_rx_lev_min_sul_r15 = -70;
  int8_t                        p_max_nr_r15         = -30;
  ns_pmax_list_nr_r15_l         ns_pmax_list_nr_r15;
  int8_t                        q_qual_min_r15               = -43;
  bool                          derive_ssb_idx_from_cell_r15 = false;
  uint8_t                       max_rs_idx_cell_qual_r15     = 1;
  thres_list_nr_r15_s           thresh_rs_idx_r15;
  // ...
  // group 0
  copy_ptr<multi_band_ns_pmax_list_nr_minus1_v1550_l> multi_band_ns_pmax_list_nr_v1550;
  copy_ptr<multi_band_ns_pmax_list_nr_v1550_l>        multi_band_ns_pmax_list_nr_sul_v1550;
  copy_ptr<ssb_to_measure_r15_c>                      ssb_to_measure_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqUTRA-FDD ::= SEQUENCE
struct carrier_freq_utra_fdd_s {
  struct thresh_x_q_r9_s_ {
    uint8_t thresh_x_high_q_r9 = 0;
    uint8_t thresh_x_low_q_r9  = 0;
  };

  // member variables
  bool     ext                     = false;
  bool     cell_resel_prio_present = false;
  uint16_t carrier_freq            = 0;
  uint8_t  cell_resel_prio         = 0;
  uint8_t  thresh_x_high           = 0;
  uint8_t  thresh_x_low            = 0;
  int8_t   q_rx_lev_min            = -60;
  int8_t   p_max_utra              = -50;
  int8_t   q_qual_min              = -24;
  // ...
  // group 0
  copy_ptr<thresh_x_q_r9_s_> thresh_x_q_r9;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqUTRA-FDD-Ext-r12 ::= SEQUENCE
struct carrier_freq_utra_fdd_ext_r12_s {
  struct thresh_x_q_r12_s_ {
    uint8_t thresh_x_high_q_r12 = 0;
    uint8_t thresh_x_low_q_r12  = 0;
  };
  using multi_band_info_list_r12_l_ = bounded_array<uint8_t, 8>;

  // member variables
  bool                        ext                                  = false;
  bool                        cell_resel_prio_r12_present          = false;
  bool                        thresh_x_q_r12_present               = false;
  bool                        multi_band_info_list_r12_present     = false;
  bool                        reduced_meas_performance_r12_present = false;
  uint16_t                    carrier_freq_r12                     = 0;
  uint8_t                     cell_resel_prio_r12                  = 0;
  uint8_t                     thresh_x_high_r12                    = 0;
  uint8_t                     thresh_x_low_r12                     = 0;
  int8_t                      q_rx_lev_min_r12                     = -60;
  int8_t                      p_max_utra_r12                       = -50;
  int8_t                      q_qual_min_r12                       = -24;
  thresh_x_q_r12_s_           thresh_x_q_r12;
  multi_band_info_list_r12_l_ multi_band_info_list_r12;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqUTRA-TDD ::= SEQUENCE
struct carrier_freq_utra_tdd_s {
  bool     ext                     = false;
  bool     cell_resel_prio_present = false;
  uint16_t carrier_freq            = 0;
  uint8_t  cell_resel_prio         = 0;
  uint8_t  thresh_x_high           = 0;
  uint8_t  thresh_x_low            = 0;
  int8_t   q_rx_lev_min            = -60;
  int8_t   p_max_utra              = -50;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqUTRA-TDD-r12 ::= SEQUENCE
struct carrier_freq_utra_tdd_r12_s {
  bool     ext                                  = false;
  bool     cell_resel_prio_r12_present          = false;
  bool     reduced_meas_performance_r12_present = false;
  uint16_t carrier_freq_r12                     = 0;
  uint8_t  cell_resel_prio_r12                  = 0;
  uint8_t  thresh_x_high_r12                    = 0;
  uint8_t  thresh_x_low_r12                     = 0;
  int8_t   q_rx_lev_min_r12                     = -60;
  int8_t   p_max_utra_r12                       = -50;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqsInfoGERAN ::= SEQUENCE
struct carrier_freqs_info_geran_s {
  struct common_info_s_ {
    bool               cell_resel_prio_present = false;
    bool               p_max_geran_present     = false;
    uint8_t            cell_resel_prio         = 0;
    fixed_bitstring<8> ncc_permitted;
    uint8_t            q_rx_lev_min  = 0;
    uint8_t            p_max_geran   = 0;
    uint8_t            thresh_x_high = 0;
    uint8_t            thresh_x_low  = 0;
  };

  // member variables
  bool                  ext = false;
  carrier_freqs_geran_s carrier_freqs;
  common_info_s_        common_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EAB-Config-r11 ::= SEQUENCE
struct eab_cfg_r11_s {
  struct eab_category_r11_opts {
    enum options { a, b, c, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<eab_category_r11_opts> eab_category_r11_e_;

  // member variables
  eab_category_r11_e_ eab_category_r11;
  fixed_bitstring<10> eab_barr_bitmap_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqInfo ::= SEQUENCE
struct inter_freq_carrier_freq_info_s {
  struct thresh_x_q_r9_s_ {
    uint8_t thresh_x_high_q_r9 = 0;
    uint8_t thresh_x_low_q_r9  = 0;
  };

  // member variables
  bool                         ext                                = false;
  bool                         p_max_present                      = false;
  bool                         t_resel_eutra_sf_present           = false;
  bool                         cell_resel_prio_present            = false;
  bool                         q_offset_freq_present              = false;
  bool                         inter_freq_neigh_cell_list_present = false;
  bool                         inter_freq_black_cell_list_present = false;
  uint32_t                     dl_carrier_freq                    = 0;
  int8_t                       q_rx_lev_min                       = -70;
  int8_t                       p_max                              = -30;
  uint8_t                      t_resel_eutra                      = 0;
  speed_state_scale_factors_s  t_resel_eutra_sf;
  uint8_t                      thresh_x_high = 0;
  uint8_t                      thresh_x_low  = 0;
  allowed_meas_bw_e            allowed_meas_bw;
  bool                         presence_ant_port1 = false;
  uint8_t                      cell_resel_prio    = 0;
  fixed_bitstring<2>           neigh_cell_cfg;
  q_offset_range_e             q_offset_freq;
  inter_freq_neigh_cell_list_l inter_freq_neigh_cell_list;
  inter_freq_black_cell_list_l inter_freq_black_cell_list;
  // ...
  // group 0
  bool                       q_qual_min_r9_present = false;
  int8_t                     q_qual_min_r9         = -34;
  copy_ptr<thresh_x_q_r9_s_> thresh_x_q_r9;
  // group 1
  bool   q_qual_min_wb_r11_present = false;
  int8_t q_qual_min_wb_r11         = -34;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqInfo-r12 ::= SEQUENCE
struct inter_freq_carrier_freq_info_r12_s {
  struct thresh_x_q_r12_s_ {
    uint8_t thresh_x_high_q_r12 = 0;
    uint8_t thresh_x_low_q_r12  = 0;
  };

  // member variables
  bool                         ext                                        = false;
  bool                         p_max_r12_present                          = false;
  bool                         t_resel_eutra_sf_r12_present               = false;
  bool                         cell_resel_prio_r12_present                = false;
  bool                         q_offset_freq_r12_present                  = false;
  bool                         inter_freq_neigh_cell_list_r12_present     = false;
  bool                         inter_freq_black_cell_list_r12_present     = false;
  bool                         q_qual_min_r12_present                     = false;
  bool                         thresh_x_q_r12_present                     = false;
  bool                         q_qual_min_wb_r12_present                  = false;
  bool                         multi_band_info_list_r12_present           = false;
  bool                         reduced_meas_performance_r12_present       = false;
  bool                         q_qual_min_rsrq_on_all_symbols_r12_present = false;
  uint32_t                     dl_carrier_freq_r12                        = 0;
  int8_t                       q_rx_lev_min_r12                           = -70;
  int8_t                       p_max_r12                                  = -30;
  uint8_t                      t_resel_eutra_r12                          = 0;
  speed_state_scale_factors_s  t_resel_eutra_sf_r12;
  uint8_t                      thresh_x_high_r12 = 0;
  uint8_t                      thresh_x_low_r12  = 0;
  allowed_meas_bw_e            allowed_meas_bw_r12;
  bool                         presence_ant_port1_r12 = false;
  uint8_t                      cell_resel_prio_r12    = 0;
  fixed_bitstring<2>           neigh_cell_cfg_r12;
  q_offset_range_e             q_offset_freq_r12;
  inter_freq_neigh_cell_list_l inter_freq_neigh_cell_list_r12;
  inter_freq_black_cell_list_l inter_freq_black_cell_list_r12;
  int8_t                       q_qual_min_r12 = -34;
  thresh_x_q_r12_s_            thresh_x_q_r12;
  int8_t                       q_qual_min_wb_r12 = -34;
  multi_band_info_list_r11_l   multi_band_info_list_r12;
  int8_t                       q_qual_min_rsrq_on_all_symbols_r12 = -34;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqInfo-v10j0 ::= SEQUENCE
struct inter_freq_carrier_freq_info_v10j0_s {
  bool                         freq_band_info_r10_present         = false;
  bool                         multi_band_info_list_v10j0_present = false;
  ns_pmax_list_r10_l           freq_band_info_r10;
  multi_band_info_list_v10j0_l multi_band_info_list_v10j0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqInfo-v1250 ::= SEQUENCE
struct inter_freq_carrier_freq_info_v1250_s {
  bool   reduced_meas_performance_r12_present       = false;
  bool   q_qual_min_rsrq_on_all_symbols_r12_present = false;
  int8_t q_qual_min_rsrq_on_all_symbols_r12         = -34;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqInfo-v1310 ::= SEQUENCE
struct inter_freq_carrier_freq_info_v1310_s {
  bool                         cell_resel_sub_prio_r13_present    = false;
  bool                         redist_inter_freq_info_r13_present = false;
  bool                         cell_sel_info_ce_r13_present       = false;
  bool                         t_resel_eutra_ce_r13_present       = false;
  cell_resel_sub_prio_r13_e    cell_resel_sub_prio_r13;
  redist_inter_freq_info_r13_s redist_inter_freq_info_r13;
  cell_sel_info_ce_r13_s       cell_sel_info_ce_r13;
  uint8_t                      t_resel_eutra_ce_r13 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqInfo-v1350 ::= SEQUENCE
struct inter_freq_carrier_freq_info_v1350_s {
  bool                    cell_sel_info_ce1_r13_present = false;
  cell_sel_info_ce1_r13_s cell_sel_info_ce1_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqInfo-v1360 ::= SEQUENCE
struct inter_freq_carrier_freq_info_v1360_s {
  bool                      cell_sel_info_ce1_v1360_present = false;
  cell_sel_info_ce1_v1360_s cell_sel_info_ce1_v1360;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqInfo-v1530 ::= SEQUENCE
struct inter_freq_carrier_freq_info_v1530_s {
  bool                                  inter_freq_neigh_hsdn_cell_list_r15_present = false;
  bool                                  cell_sel_info_ce_v1530_present              = false;
  bool                                  hsdn_ind_r15                                = false;
  inter_freq_neigh_hsdn_cell_list_r15_l inter_freq_neigh_hsdn_cell_list_r15;
  cell_sel_info_ce_v1530_s              cell_sel_info_ce_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraFreqNeighCellInfo ::= SEQUENCE
struct intra_freq_neigh_cell_info_s {
  bool             ext = false;
  uint16_t         pci = 0;
  q_offset_range_e q_offset_cell;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMS-CarrierType-r14 ::= SEQUENCE
struct mbms_carrier_type_r14_s {
  struct carrier_type_r14_opts {
    enum options { mbms, fembms_mixed, fembms_ded, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<carrier_type_r14_opts> carrier_type_r14_e_;

  // member variables
  bool                frame_offset_r14_present = false;
  carrier_type_r14_e_ carrier_type_r14;
  uint8_t             frame_offset_r14 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMS-SAI-InterFreq-r11 ::= SEQUENCE
struct mbms_sai_inter_freq_r11_s {
  uint32_t            dl_carrier_freq_r11 = 0;
  mbms_sai_list_r11_l mbms_sai_list_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMS-SAI-InterFreq-v1140 ::= SEQUENCE
struct mbms_sai_inter_freq_v1140_s {
  bool                       multi_band_info_list_r11_present = false;
  multi_band_info_list_r11_l multi_band_info_list_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBSFN-AreaInfo-r9 ::= SEQUENCE
struct mbsfn_area_info_r9_s {
  struct non_mbsfn_region_len_opts {
    enum options { s1, s2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<non_mbsfn_region_len_opts> non_mbsfn_region_len_e_;
  struct mcch_cfg_r9_s_ {
    struct mcch_repeat_period_r9_opts {
      enum options { rf32, rf64, rf128, rf256, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<mcch_repeat_period_r9_opts> mcch_repeat_period_r9_e_;
    struct mcch_mod_period_r9_opts {
      enum options { rf512, rf1024, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<mcch_mod_period_r9_opts> mcch_mod_period_r9_e_;
    struct sig_mcs_r9_opts {
      enum options { n2, n7, n13, n19, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<sig_mcs_r9_opts> sig_mcs_r9_e_;

    // member variables
    mcch_repeat_period_r9_e_ mcch_repeat_period_r9;
    uint8_t                  mcch_offset_r9 = 0;
    mcch_mod_period_r9_e_    mcch_mod_period_r9;
    fixed_bitstring<6>       sf_alloc_info_r9;
    sig_mcs_r9_e_            sig_mcs_r9;
  };
  struct mcch_cfg_r14_s_ {
    struct mcch_repeat_period_v1430_opts {
      enum options { rf1, rf2, rf4, rf8, rf16, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<mcch_repeat_period_v1430_opts> mcch_repeat_period_v1430_e_;
    struct mcch_mod_period_v1430_opts {
      enum options { rf1, rf2, rf4, rf8, rf16, rf32, rf64, rf128, rf256, spare7, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<mcch_mod_period_v1430_opts> mcch_mod_period_v1430_e_;

    // member variables
    bool                        mcch_repeat_period_v1430_present = false;
    bool                        mcch_mod_period_v1430_present    = false;
    mcch_repeat_period_v1430_e_ mcch_repeat_period_v1430;
    mcch_mod_period_v1430_e_    mcch_mod_period_v1430;
  };
  struct subcarrier_spacing_mbms_r14_opts {
    enum options { khz7dot5, khz1dot25, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<subcarrier_spacing_mbms_r14_opts> subcarrier_spacing_mbms_r14_e_;

  // member variables
  bool                    ext              = false;
  uint16_t                mbsfn_area_id_r9 = 0;
  non_mbsfn_region_len_e_ non_mbsfn_region_len;
  uint8_t                 notif_ind_r9 = 0;
  mcch_cfg_r9_s_          mcch_cfg_r9;
  // ...
  // group 0
  bool                           subcarrier_spacing_mbms_r14_present = false;
  copy_ptr<mcch_cfg_r14_s_>      mcch_cfg_r14;
  subcarrier_spacing_mbms_r14_e_ subcarrier_spacing_mbms_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeighCellListCDMA2000 ::= SEQUENCE (SIZE (1..16)) OF NeighCellCDMA2000
using neigh_cell_list_cdma2000_l = dyn_array<neigh_cell_cdma2000_s>;

// NeighCellListCDMA2000-v920 ::= SEQUENCE (SIZE (1..16)) OF NeighCellCDMA2000-v920
using neigh_cell_list_cdma2000_v920_l = dyn_array<neigh_cell_cdma2000_v920_s>;

// ReselectionInfoRelay-r13 ::= SEQUENCE
struct resel_info_relay_r13_s {
  struct min_hyst_r13_opts {
    enum options { db0, db3, db6, db9, db12, dbinf, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<min_hyst_r13_opts> min_hyst_r13_e_;

  // member variables
  bool            min_hyst_r13_present = false;
  int8_t          q_rx_lev_min_r13     = -70;
  filt_coef_e     filt_coef_r13;
  min_hyst_r13_e_ min_hyst_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB8-PerPLMN-r11 ::= SEQUENCE
struct sib8_per_plmn_r11_s {
  struct params_cdma2000_r11_c_ {
    struct types_opts {
      enum options { explicit_value, default_value, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    params_cdma2000_r11_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    params_cdma2000_r11_s& explicit_value()
    {
      assert_choice_type(types::explicit_value, type_, "parametersCDMA2000-r11");
      return c;
    }
    const params_cdma2000_r11_s& explicit_value() const
    {
      assert_choice_type(types::explicit_value, type_, "parametersCDMA2000-r11");
      return c;
    }
    params_cdma2000_r11_s& set_explicit_value();
    void                   set_default_value();

  private:
    types                 type_;
    params_cdma2000_r11_s c;
  };

  // member variables
  uint8_t                plmn_id_r11 = 1;
  params_cdma2000_r11_c_ params_cdma2000_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-CarrierFreqInfo-r12 ::= SEQUENCE
struct sl_carrier_freq_info_r12_s {
  bool                plmn_id_list_r12_present = false;
  uint32_t            carrier_freq_r12         = 0;
  plmn_id_list4_r12_l plmn_id_list_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-CarrierFreqInfo-v1310 ::= SEQUENCE
struct sl_carrier_freq_info_v1310_s {
  bool                               ext                         = false;
  bool                               disc_res_non_ps_r13_present = false;
  bool                               disc_res_ps_r13_present     = false;
  bool                               disc_cfg_other_r13_present  = false;
  sl_res_inter_freq_r13_s            disc_res_non_ps_r13;
  sl_res_inter_freq_r13_s            disc_res_ps_r13;
  sl_disc_cfg_other_inter_freq_r13_s disc_cfg_other_r13;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-PPPP-TxConfigIndex-r15 ::= SEQUENCE
struct sl_pppp_tx_cfg_idx_r15_s {
  using tx_cfg_idx_list_r15_l_      = bounded_array<uint8_t, 16>;
  using mcs_pssch_range_list_r15_l_ = dyn_array<mcs_pssch_range_r15_s>;

  // member variables
  uint8_t                     prio_thres_r15         = 1;
  uint8_t                     default_tx_cfg_idx_r15 = 0;
  uint8_t                     cbr_cfg_idx_r15        = 0;
  tx_cfg_idx_list_r15_l_      tx_cfg_idx_list_r15;
  mcs_pssch_range_list_r15_l_ mcs_pssch_range_list_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UAC-BarringInfoSet-r15 ::= SEQUENCE
struct uac_barr_info_set_r15_s {
  struct uac_barr_factor_r15_opts {
    enum options { p00, p05, p10, p15, p20, p25, p30, p40, p50, p60, p70, p75, p80, p85, p90, p95, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<uac_barr_factor_r15_opts> uac_barr_factor_r15_e_;
  struct uac_barr_time_r15_opts {
    enum options { s4, s8, s16, s32, s64, s128, s256, s512, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<uac_barr_time_r15_opts> uac_barr_time_r15_e_;

  // member variables
  uac_barr_factor_r15_e_ uac_barr_factor_r15;
  uac_barr_time_r15_e_   uac_barr_time_r15;
  fixed_bitstring<7>     uac_barr_for_access_id_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UAC-BarringPerPLMN-r15 ::= SEQUENCE
struct uac_barr_per_plmn_r15_s {
  struct uac_ac_barr_list_type_r15_c_ {
    using uac_implicit_ac_barr_list_r15_l_ = std::array<uint8_t, 63>;
    struct types_opts {
      enum options { uac_implicit_ac_barr_list_r15, uac_explicit_ac_barr_list_r15, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    uac_ac_barr_list_type_r15_c_() = default;
    uac_ac_barr_list_type_r15_c_(const uac_ac_barr_list_type_r15_c_& other);
    uac_ac_barr_list_type_r15_c_& operator=(const uac_ac_barr_list_type_r15_c_& other);
    ~uac_ac_barr_list_type_r15_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uac_implicit_ac_barr_list_r15_l_& uac_implicit_ac_barr_list_r15()
    {
      assert_choice_type(types::uac_implicit_ac_barr_list_r15, type_, "uac-AC-BarringListType-r15");
      return c.get<uac_implicit_ac_barr_list_r15_l_>();
    }
    uac_barr_per_cat_list_r15_l& uac_explicit_ac_barr_list_r15()
    {
      assert_choice_type(types::uac_explicit_ac_barr_list_r15, type_, "uac-AC-BarringListType-r15");
      return c.get<uac_barr_per_cat_list_r15_l>();
    }
    const uac_implicit_ac_barr_list_r15_l_& uac_implicit_ac_barr_list_r15() const
    {
      assert_choice_type(types::uac_implicit_ac_barr_list_r15, type_, "uac-AC-BarringListType-r15");
      return c.get<uac_implicit_ac_barr_list_r15_l_>();
    }
    const uac_barr_per_cat_list_r15_l& uac_explicit_ac_barr_list_r15() const
    {
      assert_choice_type(types::uac_explicit_ac_barr_list_r15, type_, "uac-AC-BarringListType-r15");
      return c.get<uac_barr_per_cat_list_r15_l>();
    }
    uac_implicit_ac_barr_list_r15_l_& set_uac_implicit_ac_barr_list_r15();
    uac_barr_per_cat_list_r15_l&      set_uac_explicit_ac_barr_list_r15();

  private:
    types                                                                          type_;
    choice_buffer_t<uac_barr_per_cat_list_r15_l, uac_implicit_ac_barr_list_r15_l_> c;

    void destroy_();
  };

  // member variables
  bool                         uac_ac_barr_list_type_r15_present = false;
  uint8_t                      plmn_id_idx_r15                   = 1;
  uac_ac_barr_list_type_r15_c_ uac_ac_barr_list_type_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqInfoUTRA-v1250 ::= SEQUENCE
struct carrier_freq_info_utra_v1250_s {
  bool reduced_meas_performance_r12_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqListNR-r15 ::= SEQUENCE (SIZE (1..8)) OF CarrierFreqNR-r15
using carrier_freq_list_nr_r15_l = dyn_array<carrier_freq_nr_r15_s>;

// CarrierFreqListUTRA-FDD ::= SEQUENCE (SIZE (1..16)) OF CarrierFreqUTRA-FDD
using carrier_freq_list_utra_fdd_l = dyn_array<carrier_freq_utra_fdd_s>;

// CarrierFreqListUTRA-FDD-Ext-r12 ::= SEQUENCE (SIZE (1..16)) OF CarrierFreqUTRA-FDD-Ext-r12
using carrier_freq_list_utra_fdd_ext_r12_l = dyn_array<carrier_freq_utra_fdd_ext_r12_s>;

// CarrierFreqListUTRA-TDD ::= SEQUENCE (SIZE (1..16)) OF CarrierFreqUTRA-TDD
using carrier_freq_list_utra_tdd_l = dyn_array<carrier_freq_utra_tdd_s>;

// CarrierFreqListUTRA-TDD-Ext-r12 ::= SEQUENCE (SIZE (1..16)) OF CarrierFreqUTRA-TDD-r12
using carrier_freq_list_utra_tdd_ext_r12_l = dyn_array<carrier_freq_utra_tdd_r12_s>;

// CarrierFreqsInfoListGERAN ::= SEQUENCE (SIZE (1..16)) OF CarrierFreqsInfoGERAN
using carrier_freqs_info_list_geran_l = dyn_array<carrier_freqs_info_geran_s>;

// CellReselectionInfoCommon-v1460 ::= SEQUENCE
struct cell_resel_info_common_v1460_s {
  struct s_search_delta_p_r14_opts {
    enum options { db6, db9, db12, db15, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<s_search_delta_p_r14_opts> s_search_delta_p_r14_e_;

  // member variables
  s_search_delta_p_r14_e_ s_search_delta_p_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellReselectionInfoHSDN-r15 ::= SEQUENCE
struct cell_resel_info_hsdn_r15_s {
  uint8_t cell_equivalent_size_r15 = 2;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellReselectionParametersCDMA2000 ::= SEQUENCE
struct cell_resel_params_cdma2000_s {
  bool                        t_resel_cdma2000_sf_present = false;
  band_class_list_cdma2000_l  band_class_list;
  neigh_cell_list_cdma2000_l  neigh_cell_list;
  uint8_t                     t_resel_cdma2000 = 0;
  speed_state_scale_factors_s t_resel_cdma2000_sf;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellReselectionParametersCDMA2000-v920 ::= SEQUENCE
struct cell_resel_params_cdma2000_v920_s {
  neigh_cell_list_cdma2000_v920_l neigh_cell_list_v920;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellReselectionServingFreqInfo-v1310 ::= SEQUENCE
struct cell_resel_serving_freq_info_v1310_s {
  cell_resel_sub_prio_r13_e cell_resel_sub_prio_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EAB-ConfigPLMN-r11 ::= SEQUENCE
struct eab_cfg_plmn_r11_s {
  bool          eab_cfg_r11_present = false;
  eab_cfg_r11_s eab_cfg_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqList ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo
using inter_freq_carrier_freq_list_l = dyn_array<inter_freq_carrier_freq_info_s>;

// InterFreqCarrierFreqList-v1250 ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo-v1250
using inter_freq_carrier_freq_list_v1250_l = dyn_array<inter_freq_carrier_freq_info_v1250_s>;

// InterFreqCarrierFreqList-v1310 ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo-v1310
using inter_freq_carrier_freq_list_v1310_l = dyn_array<inter_freq_carrier_freq_info_v1310_s>;

// InterFreqCarrierFreqList-v1350 ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo-v1350
using inter_freq_carrier_freq_list_v1350_l = dyn_array<inter_freq_carrier_freq_info_v1350_s>;

// InterFreqCarrierFreqList-v1530 ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo-v1530
using inter_freq_carrier_freq_list_v1530_l = dyn_array<inter_freq_carrier_freq_info_v1530_s>;

// InterFreqCarrierFreqListExt-r12 ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo-r12
using inter_freq_carrier_freq_list_ext_r12_l = dyn_array<inter_freq_carrier_freq_info_r12_s>;

// InterFreqCarrierFreqListExt-v1280 ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo-v10j0
using inter_freq_carrier_freq_list_ext_v1280_l = dyn_array<inter_freq_carrier_freq_info_v10j0_s>;

// InterFreqCarrierFreqListExt-v1310 ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo-v1310
using inter_freq_carrier_freq_list_ext_v1310_l = dyn_array<inter_freq_carrier_freq_info_v1310_s>;

// InterFreqCarrierFreqListExt-v1350 ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo-v1350
using inter_freq_carrier_freq_list_ext_v1350_l = dyn_array<inter_freq_carrier_freq_info_v1350_s>;

// InterFreqCarrierFreqListExt-v1360 ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo-v1360
using inter_freq_carrier_freq_list_ext_v1360_l = dyn_array<inter_freq_carrier_freq_info_v1360_s>;

// InterFreqCarrierFreqListExt-v1530 ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo-v1530
using inter_freq_carrier_freq_list_ext_v1530_l = dyn_array<inter_freq_carrier_freq_info_v1530_s>;

// IntraFreqBlackCellList ::= SEQUENCE (SIZE (1..16)) OF PhysCellIdRange
using intra_freq_black_cell_list_l = dyn_array<pci_range_s>;

// IntraFreqNeighCellList ::= SEQUENCE (SIZE (1..16)) OF IntraFreqNeighCellInfo
using intra_freq_neigh_cell_list_l = dyn_array<intra_freq_neigh_cell_info_s>;

// IntraFreqNeighHSDN-CellList-r15 ::= SEQUENCE (SIZE (1..16)) OF PhysCellIdRange
using intra_freq_neigh_hsdn_cell_list_r15_l = dyn_array<pci_range_s>;

// MBMS-InterFreqCarrierTypeList-r14 ::= SEQUENCE (SIZE (1..8)) OF MBMS-CarrierType-r14
using mbms_inter_freq_carrier_type_list_r14_l = dyn_array<mbms_carrier_type_r14_s>;

// MBMS-NotificationConfig-r9 ::= SEQUENCE
struct mbms_notif_cfg_r9_s {
  struct notif_repeat_coeff_r9_opts {
    enum options { n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<notif_repeat_coeff_r9_opts> notif_repeat_coeff_r9_e_;

  // member variables
  notif_repeat_coeff_r9_e_ notif_repeat_coeff_r9;
  uint8_t                  notif_offset_r9 = 0;
  uint8_t                  notif_sf_idx_r9 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMS-NotificationConfig-v1430 ::= SEQUENCE
struct mbms_notif_cfg_v1430_s {
  uint8_t notif_sf_idx_v1430 = 7;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMS-SAI-InterFreqList-r11 ::= SEQUENCE (SIZE (1..8)) OF MBMS-SAI-InterFreq-r11
using mbms_sai_inter_freq_list_r11_l = dyn_array<mbms_sai_inter_freq_r11_s>;

// MBMS-SAI-InterFreqList-v1140 ::= SEQUENCE (SIZE (1..8)) OF MBMS-SAI-InterFreq-v1140
using mbms_sai_inter_freq_list_v1140_l = dyn_array<mbms_sai_inter_freq_v1140_s>;

// MBSFN-AreaInfoList-r9 ::= SEQUENCE (SIZE (1..8)) OF MBSFN-AreaInfo-r9
using mbsfn_area_info_list_r9_l = dyn_array<mbsfn_area_info_r9_s>;

// MeasIdleConfigSIB-r15 ::= SEQUENCE
struct meas_idle_cfg_sib_r15_s {
  bool                     ext = false;
  eutra_carrier_list_r15_l meas_idle_carrier_list_eutra_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RedistributionServingInfo-r13 ::= SEQUENCE
struct redist_serving_info_r13_s {
  struct t360_r13_opts {
    enum options { min4, min8, min16, min32, infinity, spare3, spare2, spare1, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<t360_r13_opts> t360_r13_e_;

  // member variables
  bool        redist_factor_cell_r13_present     = false;
  bool        redistr_on_paging_only_r13_present = false;
  uint8_t     redist_factor_serving_r13          = 0;
  t360_r13_e_ t360_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SC-MCCH-SchedulingInfo-r14 ::= SEQUENCE
struct sc_mcch_sched_info_r14_s {
  struct on_dur_timer_scptm_r14_opts {
    enum options { psf10, psf20, psf100, psf300, psf500, psf1000, psf1200, psf1600, nulltype } value;
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

// SIB8-PerPLMN-List-r11 ::= SEQUENCE (SIZE (1..6)) OF SIB8-PerPLMN-r11
using sib8_per_plmn_list_r11_l = dyn_array<sib8_per_plmn_r11_s>;

// SL-CBR-PPPP-TxConfigList-r15 ::= SEQUENCE (SIZE (1..8)) OF SL-PPPP-TxConfigIndex-r15
using sl_cbr_pppp_tx_cfg_list_r15_l = dyn_array<sl_pppp_tx_cfg_idx_r15_s>;

// SL-CarrierFreqInfoList-r12 ::= SEQUENCE (SIZE (1..8)) OF SL-CarrierFreqInfo-r12
using sl_carrier_freq_info_list_r12_l = dyn_array<sl_carrier_freq_info_r12_s>;

// SL-CarrierFreqInfoList-v1310 ::= SEQUENCE (SIZE (1..8)) OF SL-CarrierFreqInfo-v1310
using sl_carrier_freq_info_list_v1310_l = dyn_array<sl_carrier_freq_info_v1310_s>;

// SL-CommRxPoolList-r12 ::= SEQUENCE (SIZE (1..16)) OF SL-CommResourcePool-r12
using sl_comm_rx_pool_list_r12_l = dyn_array<sl_comm_res_pool_r12_s>;

// SL-CommTxPoolList-r12 ::= SEQUENCE (SIZE (1..4)) OF SL-CommResourcePool-r12
using sl_comm_tx_pool_list_r12_l = dyn_array<sl_comm_res_pool_r12_s>;

// SL-CommTxPoolListExt-r13 ::= SEQUENCE (SIZE (1..4)) OF SL-CommResourcePool-r12
using sl_comm_tx_pool_list_ext_r13_l = dyn_array<sl_comm_res_pool_r12_s>;

// SL-DiscConfigRelayUE-r13 ::= SEQUENCE
struct sl_disc_cfg_relay_ue_r13_s {
  struct hyst_max_r13_opts {
    enum options { db0, db3, db6, db9, db12, dbinf, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<hyst_max_r13_opts> hyst_max_r13_e_;
  struct hyst_min_r13_opts {
    enum options { db0, db3, db6, db9, db12, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<hyst_min_r13_opts> hyst_min_r13_e_;

  // member variables
  bool            thresh_high_r13_present = false;
  bool            thresh_low_r13_present  = false;
  bool            hyst_max_r13_present    = false;
  bool            hyst_min_r13_present    = false;
  uint8_t         thresh_high_r13         = 0;
  uint8_t         thresh_low_r13          = 0;
  hyst_max_r13_e_ hyst_max_r13;
  hyst_min_r13_e_ hyst_min_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-DiscConfigRemoteUE-r13 ::= SEQUENCE
struct sl_disc_cfg_remote_ue_r13_s {
  struct hyst_max_r13_opts {
    enum options { db0, db3, db6, db9, db12, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<hyst_max_r13_opts> hyst_max_r13_e_;

  // member variables
  bool                   thresh_high_r13_present = false;
  bool                   hyst_max_r13_present    = false;
  uint8_t                thresh_high_r13         = 0;
  hyst_max_r13_e_        hyst_max_r13;
  resel_info_relay_r13_s resel_info_ic_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-SyncConfigList-r12 ::= SEQUENCE (SIZE (1..16)) OF SL-SyncConfig-r12
using sl_sync_cfg_list_r12_l = dyn_array<sl_sync_cfg_r12_s>;

// UAC-AC1-SelectAssistInfo-r15 ::= ENUMERATED
struct uac_ac1_select_assist_info_r15_opts {
  enum options { a, b, c, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<uac_ac1_select_assist_info_r15_opts> uac_ac1_select_assist_info_r15_e;

// UAC-BarringInfoSetList-r15 ::= SEQUENCE (SIZE (1..8)) OF UAC-BarringInfoSet-r15
using uac_barr_info_set_list_r15_l = dyn_array<uac_barr_info_set_r15_s>;

// UAC-BarringPerPLMN-List-r15 ::= SEQUENCE (SIZE (1..6)) OF UAC-BarringPerPLMN-r15
using uac_barr_per_plmn_list_r15_l = dyn_array<uac_barr_per_plmn_r15_s>;

// SystemInformation-v8a0-IEs ::= SEQUENCE
struct sys_info_v8a0_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockPos-r15 ::= SEQUENCE
struct sib_pos_r15_s {
  bool          ext                       = false;
  bool          late_non_crit_ext_present = false;
  dyn_octstring assist_data_sib_elem_r15;
  dyn_octstring late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType10 ::= SEQUENCE
struct sib_type10_s {
  bool                ext           = false;
  bool                dummy_present = false;
  fixed_bitstring<16> msg_id;
  fixed_bitstring<16> serial_num;
  fixed_octstring<2>  warning_type;
  fixed_octstring<50> dummy;
  // ...
  bool          late_non_crit_ext_present = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType11 ::= SEQUENCE
struct sib_type11_s {
  struct warning_msg_segment_type_opts {
    enum options { not_last_segment, last_segment, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<warning_msg_segment_type_opts> warning_msg_segment_type_e_;

  // member variables
  bool                        ext                        = false;
  bool                        data_coding_scheme_present = false;
  fixed_bitstring<16>         msg_id;
  fixed_bitstring<16>         serial_num;
  warning_msg_segment_type_e_ warning_msg_segment_type;
  uint8_t                     warning_msg_segment_num = 0;
  dyn_octstring               warning_msg_segment;
  fixed_octstring<1>          data_coding_scheme;
  // ...
  bool          late_non_crit_ext_present = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType12-r9 ::= SEQUENCE
struct sib_type12_r9_s {
  struct warning_msg_segment_type_r9_opts {
    enum options { not_last_segment, last_segment, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<warning_msg_segment_type_r9_opts> warning_msg_segment_type_r9_e_;

  // member variables
  bool                           ext                           = false;
  bool                           data_coding_scheme_r9_present = false;
  bool                           late_non_crit_ext_present     = false;
  fixed_bitstring<16>            msg_id_r9;
  fixed_bitstring<16>            serial_num_r9;
  warning_msg_segment_type_r9_e_ warning_msg_segment_type_r9;
  uint8_t                        warning_msg_segment_num_r9 = 0;
  dyn_octstring                  warning_msg_segment_r9;
  fixed_octstring<1>             data_coding_scheme_r9;
  dyn_octstring                  late_non_crit_ext;
  // ...
  // group 0
  bool          warning_area_coordinates_segment_r15_present = false;
  dyn_octstring warning_area_coordinates_segment_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType13-r9 ::= SEQUENCE
struct sib_type13_r9_s {
  bool                      ext                       = false;
  bool                      late_non_crit_ext_present = false;
  mbsfn_area_info_list_r9_l mbsfn_area_info_list_r9;
  mbms_notif_cfg_r9_s       notif_cfg_r9;
  dyn_octstring             late_non_crit_ext;
  // ...
  // group 0
  copy_ptr<mbms_notif_cfg_v1430_s> notif_cfg_v1430;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType14-r11 ::= SEQUENCE
struct sib_type14_r11_s {
  struct eab_param_r11_c_ {
    using eab_per_plmn_list_r11_l_ = dyn_array<eab_cfg_plmn_r11_s>;
    struct types_opts {
      enum options { eab_common_r11, eab_per_plmn_list_r11, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    eab_param_r11_c_() = default;
    eab_param_r11_c_(const eab_param_r11_c_& other);
    eab_param_r11_c_& operator=(const eab_param_r11_c_& other);
    ~eab_param_r11_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    eab_cfg_r11_s& eab_common_r11()
    {
      assert_choice_type(types::eab_common_r11, type_, "eab-Param-r11");
      return c.get<eab_cfg_r11_s>();
    }
    eab_per_plmn_list_r11_l_& eab_per_plmn_list_r11()
    {
      assert_choice_type(types::eab_per_plmn_list_r11, type_, "eab-Param-r11");
      return c.get<eab_per_plmn_list_r11_l_>();
    }
    const eab_cfg_r11_s& eab_common_r11() const
    {
      assert_choice_type(types::eab_common_r11, type_, "eab-Param-r11");
      return c.get<eab_cfg_r11_s>();
    }
    const eab_per_plmn_list_r11_l_& eab_per_plmn_list_r11() const
    {
      assert_choice_type(types::eab_per_plmn_list_r11, type_, "eab-Param-r11");
      return c.get<eab_per_plmn_list_r11_l_>();
    }
    eab_cfg_r11_s&            set_eab_common_r11();
    eab_per_plmn_list_r11_l_& set_eab_per_plmn_list_r11();

  private:
    types                                                    type_;
    choice_buffer_t<eab_cfg_r11_s, eab_per_plmn_list_r11_l_> c;

    void destroy_();
  };
  struct eab_per_rsrp_r15_opts {
    enum options { thresh0, thresh1, thresh2, thresh3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<eab_per_rsrp_r15_opts> eab_per_rsrp_r15_e_;

  // member variables
  bool             ext                       = false;
  bool             eab_param_r11_present     = false;
  bool             late_non_crit_ext_present = false;
  eab_param_r11_c_ eab_param_r11;
  dyn_octstring    late_non_crit_ext;
  // ...
  // group 0
  bool                eab_per_rsrp_r15_present = false;
  eab_per_rsrp_r15_e_ eab_per_rsrp_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType15-r11 ::= SEQUENCE
struct sib_type15_r11_s {
  bool                           ext                                  = false;
  bool                           mbms_sai_intra_freq_r11_present      = false;
  bool                           mbms_sai_inter_freq_list_r11_present = false;
  bool                           late_non_crit_ext_present            = false;
  mbms_sai_list_r11_l            mbms_sai_intra_freq_r11;
  mbms_sai_inter_freq_list_r11_l mbms_sai_inter_freq_list_r11;
  dyn_octstring                  late_non_crit_ext;
  // ...
  // group 0
  copy_ptr<mbms_sai_inter_freq_list_v1140_l> mbms_sai_inter_freq_list_v1140;
  // group 1
  copy_ptr<mbms_carrier_type_r14_s>                 mbms_intra_freq_carrier_type_r14;
  copy_ptr<mbms_inter_freq_carrier_type_list_r14_l> mbms_inter_freq_carrier_type_list_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType16-r11 ::= SEQUENCE
struct sib_type16_r11_s {
  struct time_info_r11_s_ {
    bool               day_light_saving_time_r11_present = false;
    bool               leap_seconds_r11_present          = false;
    bool               local_time_offset_r11_present     = false;
    uint64_t           time_info_utc_r11                 = 0;
    fixed_bitstring<2> day_light_saving_time_r11;
    int16_t            leap_seconds_r11      = -127;
    int8_t             local_time_offset_r11 = -63;
  };

  // member variables
  bool             ext                       = false;
  bool             time_info_r11_present     = false;
  bool             late_non_crit_ext_present = false;
  time_info_r11_s_ time_info_r11;
  dyn_octstring    late_non_crit_ext;
  // ...
  // group 0
  copy_ptr<time_ref_info_r15_s> time_ref_info_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType17-r12 ::= SEQUENCE
struct sib_type17_r12_s {
  using wlan_offload_info_per_plmn_list_r12_l_ = dyn_array<wlan_offload_info_per_plmn_r12_s>;

  // member variables
  bool                                   ext                                         = false;
  bool                                   wlan_offload_info_per_plmn_list_r12_present = false;
  bool                                   late_non_crit_ext_present                   = false;
  wlan_offload_info_per_plmn_list_r12_l_ wlan_offload_info_per_plmn_list_r12;
  dyn_octstring                          late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType18-r12 ::= SEQUENCE
struct sib_type18_r12_s {
  struct comm_cfg_r12_s_ {
    bool                       comm_tx_pool_normal_common_r12_present = false;
    bool                       comm_tx_pool_exceptional_r12_present   = false;
    bool                       comm_sync_cfg_r12_present              = false;
    sl_comm_rx_pool_list_r12_l comm_rx_pool_r12;
    sl_comm_tx_pool_list_r12_l comm_tx_pool_normal_common_r12;
    sl_comm_tx_pool_list_r12_l comm_tx_pool_exceptional_r12;
    sl_sync_cfg_list_r12_l     comm_sync_cfg_r12;
  };

  // member variables
  bool            ext                       = false;
  bool            comm_cfg_r12_present      = false;
  bool            late_non_crit_ext_present = false;
  comm_cfg_r12_s_ comm_cfg_r12;
  dyn_octstring   late_non_crit_ext;
  // ...
  // group 0
  bool                                     comm_tx_res_uc_req_allowed_r13_present = false;
  bool                                     comm_tx_allow_relay_common_r13_present = false;
  copy_ptr<sl_comm_tx_pool_list_ext_r13_l> comm_tx_pool_normal_common_ext_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType19-r12 ::= SEQUENCE
struct sib_type19_r12_s {
  struct disc_cfg_r12_s_ {
    bool                           disc_tx_pool_common_r12_present = false;
    bool                           disc_tx_pwr_info_r12_present    = false;
    bool                           disc_sync_cfg_r12_present       = false;
    sl_disc_rx_pool_list_r12_l     disc_rx_pool_r12;
    sl_disc_tx_pool_list_r12_l     disc_tx_pool_common_r12;
    sl_disc_tx_pwr_info_list_r12_l disc_tx_pwr_info_r12;
    sl_sync_cfg_list_r12_l         disc_sync_cfg_r12;
  };
  struct disc_cfg_v1310_s_ {
    bool                              disc_inter_freq_list_v1310_present  = false;
    bool                              gap_requests_allowed_common_present = false;
    sl_carrier_freq_info_list_v1310_l disc_inter_freq_list_v1310;
  };
  struct disc_cfg_relay_r13_s_ {
    sl_disc_cfg_relay_ue_r13_s  relay_ue_cfg_r13;
    sl_disc_cfg_remote_ue_r13_s remote_ue_cfg_r13;
  };
  struct disc_cfg_ps_minus13_s_ {
    bool                       disc_tx_pool_ps_common_r13_present = false;
    sl_disc_rx_pool_list_r12_l disc_rx_pool_ps_r13;
    sl_disc_tx_pool_list_r12_l disc_tx_pool_ps_common_r13;
  };

  // member variables
  bool                            ext                              = false;
  bool                            disc_cfg_r12_present             = false;
  bool                            disc_inter_freq_list_r12_present = false;
  bool                            late_non_crit_ext_present        = false;
  disc_cfg_r12_s_                 disc_cfg_r12;
  sl_carrier_freq_info_list_r12_l disc_inter_freq_list_r12;
  dyn_octstring                   late_non_crit_ext;
  // ...
  // group 0
  copy_ptr<disc_cfg_v1310_s_>      disc_cfg_v1310;
  copy_ptr<disc_cfg_relay_r13_s_>  disc_cfg_relay_r13;
  copy_ptr<disc_cfg_ps_minus13_s_> disc_cfg_ps_minus13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType20-r13 ::= SEQUENCE
struct sib_type20_r13_s {
  struct sc_mcch_repeat_period_r13_opts {
    enum options { rf2, rf4, rf8, rf16, rf32, rf64, rf128, rf256, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<sc_mcch_repeat_period_r13_opts> sc_mcch_repeat_period_r13_e_;
  struct sc_mcch_mod_period_r13_opts {
    enum options {
      rf2,
      rf4,
      rf8,
      rf16,
      rf32,
      rf64,
      rf128,
      rf256,
      rf512,
      rf1024,
      r2048,
      rf4096,
      rf8192,
      rf16384,
      rf32768,
      rf65536,
      nulltype
    } value;
    typedef uint32_t number_type;

    const char* to_string() const;
    uint32_t    to_number() const;
  };
  typedef enumerated<sc_mcch_mod_period_r13_opts> sc_mcch_mod_period_r13_e_;
  struct br_bcch_cfg_r14_s_ {
    struct mpdcch_num_repeat_sc_mcch_r14_opts {
      enum options { r1, r2, r4, r8, r16, r32, r64, r128, r256, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<mpdcch_num_repeat_sc_mcch_r14_opts> mpdcch_num_repeat_sc_mcch_r14_e_;
    struct mpdcch_start_sf_sc_mcch_r14_c_ {
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
      mpdcch_start_sf_sc_mcch_r14_c_() = default;
      mpdcch_start_sf_sc_mcch_r14_c_(const mpdcch_start_sf_sc_mcch_r14_c_& other);
      mpdcch_start_sf_sc_mcch_r14_c_& operator=(const mpdcch_start_sf_sc_mcch_r14_c_& other);
      ~mpdcch_start_sf_sc_mcch_r14_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      fdd_r14_e_& fdd_r14()
      {
        assert_choice_type(types::fdd_r14, type_, "mpdcch-StartSF-SC-MCCH-r14");
        return c.get<fdd_r14_e_>();
      }
      tdd_r14_e_& tdd_r14()
      {
        assert_choice_type(types::tdd_r14, type_, "mpdcch-StartSF-SC-MCCH-r14");
        return c.get<tdd_r14_e_>();
      }
      const fdd_r14_e_& fdd_r14() const
      {
        assert_choice_type(types::fdd_r14, type_, "mpdcch-StartSF-SC-MCCH-r14");
        return c.get<fdd_r14_e_>();
      }
      const tdd_r14_e_& tdd_r14() const
      {
        assert_choice_type(types::tdd_r14, type_, "mpdcch-StartSF-SC-MCCH-r14");
        return c.get<tdd_r14_e_>();
      }
      fdd_r14_e_& set_fdd_r14();
      tdd_r14_e_& set_tdd_r14();

    private:
      types               type_;
      pod_choice_buffer_t c;

      void destroy_();
    };
    struct mpdcch_pdsch_hop_cfg_sc_mcch_r14_opts {
      enum options { off, ce_mode_a, ce_mode_b, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<mpdcch_pdsch_hop_cfg_sc_mcch_r14_opts> mpdcch_pdsch_hop_cfg_sc_mcch_r14_e_;
    struct sc_mcch_repeat_period_br_r14_opts {
      enum options { rf32, rf128, rf512, rf1024, rf2048, rf4096, rf8192, rf16384, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<sc_mcch_repeat_period_br_r14_opts> sc_mcch_repeat_period_br_r14_e_;
    struct sc_mcch_mod_period_br_r14_opts {
      enum options {
        rf32,
        rf128,
        rf256,
        rf512,
        rf1024,
        rf2048,
        rf4096,
        rf8192,
        rf16384,
        rf32768,
        rf65536,
        rf131072,
        rf262144,
        rf524288,
        rf1048576,
        nulltype
      } value;
      typedef uint32_t number_type;

      const char* to_string() const;
      uint32_t    to_number() const;
    };
    typedef enumerated<sc_mcch_mod_period_br_r14_opts> sc_mcch_mod_period_br_r14_e_;

    // member variables
    uint8_t                             mpdcch_nb_sc_mcch_r14 = 1;
    mpdcch_num_repeat_sc_mcch_r14_e_    mpdcch_num_repeat_sc_mcch_r14;
    mpdcch_start_sf_sc_mcch_r14_c_      mpdcch_start_sf_sc_mcch_r14;
    mpdcch_pdsch_hop_cfg_sc_mcch_r14_e_ mpdcch_pdsch_hop_cfg_sc_mcch_r14;
    uint32_t                            sc_mcch_carrier_freq_r14 = 0;
    uint8_t                             sc_mcch_offset_br_r14    = 0;
    sc_mcch_repeat_period_br_r14_e_     sc_mcch_repeat_period_br_r14;
    sc_mcch_mod_period_br_r14_e_        sc_mcch_mod_period_br_r14;
  };
  struct pdsch_max_num_repeat_cemode_a_sc_mtch_r14_opts {
    enum options { r16, r32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pdsch_max_num_repeat_cemode_a_sc_mtch_r14_opts> pdsch_max_num_repeat_cemode_a_sc_mtch_r14_e_;
  struct pdsch_max_num_repeat_cemode_b_sc_mtch_r14_opts {
    enum options { r192, r256, r384, r512, r768, r1024, r1536, r2048, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<pdsch_max_num_repeat_cemode_b_sc_mtch_r14_opts> pdsch_max_num_repeat_cemode_b_sc_mtch_r14_e_;

  // member variables
  bool                         ext                       = false;
  bool                         sc_mcch_dur_r13_present   = false;
  bool                         late_non_crit_ext_present = false;
  sc_mcch_repeat_period_r13_e_ sc_mcch_repeat_period_r13;
  uint8_t                      sc_mcch_offset_r13   = 0;
  uint8_t                      sc_mcch_first_sf_r13 = 0;
  uint8_t                      sc_mcch_dur_r13      = 2;
  sc_mcch_mod_period_r13_e_    sc_mcch_mod_period_r13;
  dyn_octstring                late_non_crit_ext;
  // ...
  // group 0
  bool                                         pdsch_max_num_repeat_cemode_a_sc_mtch_r14_present = false;
  bool                                         pdsch_max_num_repeat_cemode_b_sc_mtch_r14_present = false;
  copy_ptr<br_bcch_cfg_r14_s_>                 br_bcch_cfg_r14;
  copy_ptr<sc_mcch_sched_info_r14_s>           sc_mcch_sched_info_r14;
  pdsch_max_num_repeat_cemode_a_sc_mtch_r14_e_ pdsch_max_num_repeat_cemode_a_sc_mtch_r14;
  pdsch_max_num_repeat_cemode_b_sc_mtch_r14_e_ pdsch_max_num_repeat_cemode_b_sc_mtch_r14;
  // group 1
  bool sc_mcch_repeat_period_v1470_present = false;
  bool sc_mcch_mod_period_v1470_present    = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType21-r14 ::= SEQUENCE
struct sib_type21_r14_s {
  bool                    ext                           = false;
  bool                    sl_v2x_cfg_common_r14_present = false;
  bool                    late_non_crit_ext_present     = false;
  sl_v2x_cfg_common_r14_s sl_v2x_cfg_common_r14;
  dyn_octstring           late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType24-r15 ::= SEQUENCE
struct sib_type24_r15_s {
  bool                        ext                              = false;
  bool                        carrier_freq_list_nr_r15_present = false;
  bool                        t_resel_nr_sf_r15_present        = false;
  bool                        late_non_crit_ext_present        = false;
  carrier_freq_list_nr_r15_l  carrier_freq_list_nr_r15;
  uint8_t                     t_resel_nr_r15 = 0;
  speed_state_scale_factors_s t_resel_nr_sf_r15;
  dyn_octstring               late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType25-r15 ::= SEQUENCE
struct sib_type25_r15_s {
  struct uac_ac1_select_assist_info_r15_c_ {
    using individual_plmn_list_r15_l_ = bounded_array<uac_ac1_select_assist_info_r15_e, 6>;
    struct types_opts {
      enum options { plmn_common_r15, individual_plmn_list_r15, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    uac_ac1_select_assist_info_r15_c_() = default;
    uac_ac1_select_assist_info_r15_c_(const uac_ac1_select_assist_info_r15_c_& other);
    uac_ac1_select_assist_info_r15_c_& operator=(const uac_ac1_select_assist_info_r15_c_& other);
    ~uac_ac1_select_assist_info_r15_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uac_ac1_select_assist_info_r15_e& plmn_common_r15()
    {
      assert_choice_type(types::plmn_common_r15, type_, "uac-AC1-SelectAssistInfo-r15");
      return c.get<uac_ac1_select_assist_info_r15_e>();
    }
    individual_plmn_list_r15_l_& individual_plmn_list_r15()
    {
      assert_choice_type(types::individual_plmn_list_r15, type_, "uac-AC1-SelectAssistInfo-r15");
      return c.get<individual_plmn_list_r15_l_>();
    }
    const uac_ac1_select_assist_info_r15_e& plmn_common_r15() const
    {
      assert_choice_type(types::plmn_common_r15, type_, "uac-AC1-SelectAssistInfo-r15");
      return c.get<uac_ac1_select_assist_info_r15_e>();
    }
    const individual_plmn_list_r15_l_& individual_plmn_list_r15() const
    {
      assert_choice_type(types::individual_plmn_list_r15, type_, "uac-AC1-SelectAssistInfo-r15");
      return c.get<individual_plmn_list_r15_l_>();
    }
    uac_ac1_select_assist_info_r15_e& set_plmn_common_r15();
    individual_plmn_list_r15_l_&      set_individual_plmn_list_r15();

  private:
    types                                        type_;
    choice_buffer_t<individual_plmn_list_r15_l_> c;

    void destroy_();
  };

  // member variables
  bool                              ext                                    = false;
  bool                              uac_barr_for_common_r15_present        = false;
  bool                              uac_barr_per_plmn_list_r15_present     = false;
  bool                              uac_ac1_select_assist_info_r15_present = false;
  bool                              late_non_crit_ext_present              = false;
  uac_barr_per_cat_list_r15_l       uac_barr_for_common_r15;
  uac_barr_per_plmn_list_r15_l      uac_barr_per_plmn_list_r15;
  uac_barr_info_set_list_r15_l      uac_barr_info_set_list_r15;
  uac_ac1_select_assist_info_r15_c_ uac_ac1_select_assist_info_r15;
  dyn_octstring                     late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType26-r15 ::= SEQUENCE
struct sib_type26_r15_s {
  bool                              ext                                  = false;
  bool                              v2x_inter_freq_info_list_r15_present = false;
  bool                              cbr_pssch_tx_cfg_list_r15_present    = false;
  bool                              v2x_packet_dupl_cfg_r15_present      = false;
  bool                              sync_freq_list_r15_present           = false;
  bool                              slss_tx_multi_freq_r15_present       = false;
  bool                              v2x_freq_sel_cfg_list_r15_present    = false;
  bool                              thresh_s_rssi_cbr_r15_present        = false;
  sl_inter_freq_info_list_v2x_r14_l v2x_inter_freq_info_list_r15;
  sl_cbr_pppp_tx_cfg_list_r15_l     cbr_pssch_tx_cfg_list_r15;
  sl_v2x_packet_dupl_cfg_r15_s      v2x_packet_dupl_cfg_r15;
  sl_v2x_sync_freq_list_r15_l       sync_freq_list_r15;
  sl_v2x_freq_sel_cfg_list_r15_l    v2x_freq_sel_cfg_list_r15;
  uint8_t                           thresh_s_rssi_cbr_r15 = 0;
  // ...
  bool          late_non_crit_ext_present = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType3 ::= SEQUENCE
struct sib_type3_s {
  struct cell_resel_info_common_s_ {
    struct q_hyst_opts {
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
    typedef enumerated<q_hyst_opts> q_hyst_e_;
    struct speed_state_resel_pars_s_ {
      struct q_hyst_sf_s_ {
        struct sf_medium_opts {
          enum options { db_minus6, db_minus4, db_minus2, db0, nulltype } value;
          typedef int8_t number_type;

          const char* to_string() const;
          int8_t      to_number() const;
        };
        typedef enumerated<sf_medium_opts> sf_medium_e_;
        struct sf_high_opts {
          enum options { db_minus6, db_minus4, db_minus2, db0, nulltype } value;
          typedef int8_t number_type;

          const char* to_string() const;
          int8_t      to_number() const;
        };
        typedef enumerated<sf_high_opts> sf_high_e_;

        // member variables
        sf_medium_e_ sf_medium;
        sf_high_e_   sf_high;
      };

      // member variables
      mob_state_params_s mob_state_params;
      q_hyst_sf_s_       q_hyst_sf;
    };

    // member variables
    bool                      speed_state_resel_pars_present = false;
    q_hyst_e_                 q_hyst;
    speed_state_resel_pars_s_ speed_state_resel_pars;
  };
  struct cell_resel_serving_freq_info_s_ {
    bool    s_non_intra_search_present = false;
    uint8_t s_non_intra_search         = 0;
    uint8_t thresh_serving_low         = 0;
    uint8_t cell_resel_prio            = 0;
  };
  struct intra_freq_cell_resel_info_s_ {
    bool                        p_max_present            = false;
    bool                        s_intra_search_present   = false;
    bool                        allowed_meas_bw_present  = false;
    bool                        t_resel_eutra_sf_present = false;
    int8_t                      q_rx_lev_min             = -70;
    int8_t                      p_max                    = -30;
    uint8_t                     s_intra_search           = 0;
    allowed_meas_bw_e           allowed_meas_bw;
    bool                        presence_ant_port1 = false;
    fixed_bitstring<2>          neigh_cell_cfg;
    uint8_t                     t_resel_eutra = 0;
    speed_state_scale_factors_s t_resel_eutra_sf;
  };
  struct s_intra_search_v920_s_ {
    uint8_t s_intra_search_p_r9 = 0;
    uint8_t s_intra_search_q_r9 = 0;
  };
  struct s_non_intra_search_v920_s_ {
    uint8_t s_non_intra_search_p_r9 = 0;
    uint8_t s_non_intra_search_q_r9 = 0;
  };

  // member variables
  bool                            ext = false;
  cell_resel_info_common_s_       cell_resel_info_common;
  cell_resel_serving_freq_info_s_ cell_resel_serving_freq_info;
  intra_freq_cell_resel_info_s_   intra_freq_cell_resel_info;
  // ...
  bool          late_non_crit_ext_present = false;
  dyn_octstring late_non_crit_ext;
  // group 0
  bool                                 q_qual_min_r9_present           = false;
  bool                                 thresh_serving_low_q_r9_present = false;
  copy_ptr<s_intra_search_v920_s_>     s_intra_search_v920;
  copy_ptr<s_non_intra_search_v920_s_> s_non_intra_search_v920;
  int8_t                               q_qual_min_r9           = -34;
  uint8_t                              thresh_serving_low_q_r9 = 0;
  // group 1
  bool   q_qual_min_wb_r11_present = false;
  int8_t q_qual_min_wb_r11         = -34;
  // group 2
  bool   q_qual_min_rsrq_on_all_symbols_r12_present = false;
  int8_t q_qual_min_rsrq_on_all_symbols_r12         = -34;
  // group 3
  bool                                           t_resel_eutra_ce_r13_present = false;
  copy_ptr<cell_resel_serving_freq_info_v1310_s> cell_resel_serving_freq_info_v1310;
  copy_ptr<redist_serving_info_r13_s>            redist_serving_info_r13;
  copy_ptr<cell_sel_info_ce_r13_s>               cell_sel_info_ce_r13;
  uint8_t                                        t_resel_eutra_ce_r13 = 0;
  // group 4
  copy_ptr<cell_sel_info_ce1_r13_s> cell_sel_info_ce1_r13;
  // group 5
  copy_ptr<cell_sel_info_ce1_v1360_s> cell_sel_info_ce1_v1360;
  // group 6
  copy_ptr<cell_resel_info_common_v1460_s> cell_resel_info_common_v1460;
  // group 7
  bool                                 crs_intf_mitig_neigh_cells_ce_r15_present = false;
  copy_ptr<cell_resel_info_hsdn_r15_s> cell_resel_info_hsdn_r15;
  copy_ptr<cell_sel_info_ce_v1530_s>   cell_sel_info_ce_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType4 ::= SEQUENCE
struct sib_type4_s {
  bool                         ext                                = false;
  bool                         intra_freq_neigh_cell_list_present = false;
  bool                         intra_freq_black_cell_list_present = false;
  bool                         csg_pci_range_present              = false;
  intra_freq_neigh_cell_list_l intra_freq_neigh_cell_list;
  intra_freq_black_cell_list_l intra_freq_black_cell_list;
  pci_range_s                  csg_pci_range;
  // ...
  bool          late_non_crit_ext_present = false;
  dyn_octstring late_non_crit_ext;
  // group 0
  copy_ptr<intra_freq_neigh_hsdn_cell_list_r15_l> intra_freq_neigh_hsdn_cell_list_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType5 ::= SEQUENCE
struct sib_type5_s {
  bool                           ext = false;
  inter_freq_carrier_freq_list_l inter_freq_carrier_freq_list;
  // ...
  bool          late_non_crit_ext_present = false;
  dyn_octstring late_non_crit_ext;
  // group 0
  copy_ptr<inter_freq_carrier_freq_list_v1250_l>   inter_freq_carrier_freq_list_v1250;
  copy_ptr<inter_freq_carrier_freq_list_ext_r12_l> inter_freq_carrier_freq_list_ext_r12;
  // group 1
  copy_ptr<inter_freq_carrier_freq_list_ext_v1280_l> inter_freq_carrier_freq_list_ext_v1280;
  // group 2
  copy_ptr<inter_freq_carrier_freq_list_v1310_l>     inter_freq_carrier_freq_list_v1310;
  copy_ptr<inter_freq_carrier_freq_list_ext_v1310_l> inter_freq_carrier_freq_list_ext_v1310;
  // group 3
  copy_ptr<inter_freq_carrier_freq_list_v1350_l>     inter_freq_carrier_freq_list_v1350;
  copy_ptr<inter_freq_carrier_freq_list_ext_v1350_l> inter_freq_carrier_freq_list_ext_v1350;
  // group 4
  copy_ptr<inter_freq_carrier_freq_list_ext_v1360_l> inter_freq_carrier_freq_list_ext_v1360;
  // group 5
  bool    scptm_freq_offset_r14_present = false;
  uint8_t scptm_freq_offset_r14         = 1;
  // group 6
  copy_ptr<inter_freq_carrier_freq_list_v1530_l>     inter_freq_carrier_freq_list_v1530;
  copy_ptr<inter_freq_carrier_freq_list_ext_v1530_l> inter_freq_carrier_freq_list_ext_v1530;
  copy_ptr<meas_idle_cfg_sib_r15_s>                  meas_idle_cfg_sib_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType6 ::= SEQUENCE
struct sib_type6_s {
  using carrier_freq_list_utra_fdd_v1250_l_ = dyn_array<carrier_freq_info_utra_v1250_s>;
  using carrier_freq_list_utra_tdd_v1250_l_ = dyn_array<carrier_freq_info_utra_v1250_s>;

  // member variables
  bool                         ext                                = false;
  bool                         carrier_freq_list_utra_fdd_present = false;
  bool                         carrier_freq_list_utra_tdd_present = false;
  bool                         t_resel_utra_sf_present            = false;
  carrier_freq_list_utra_fdd_l carrier_freq_list_utra_fdd;
  carrier_freq_list_utra_tdd_l carrier_freq_list_utra_tdd;
  uint8_t                      t_resel_utra = 0;
  speed_state_scale_factors_s  t_resel_utra_sf;
  // ...
  bool          late_non_crit_ext_present = false;
  dyn_octstring late_non_crit_ext;
  // group 0
  copy_ptr<carrier_freq_list_utra_fdd_v1250_l_>  carrier_freq_list_utra_fdd_v1250;
  copy_ptr<carrier_freq_list_utra_tdd_v1250_l_>  carrier_freq_list_utra_tdd_v1250;
  copy_ptr<carrier_freq_list_utra_fdd_ext_r12_l> carrier_freq_list_utra_fdd_ext_r12;
  copy_ptr<carrier_freq_list_utra_tdd_ext_r12_l> carrier_freq_list_utra_tdd_ext_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType7 ::= SEQUENCE
struct sib_type7_s {
  bool                            ext                             = false;
  bool                            t_resel_geran_sf_present        = false;
  bool                            carrier_freqs_info_list_present = false;
  uint8_t                         t_resel_geran                   = 0;
  speed_state_scale_factors_s     t_resel_geran_sf;
  carrier_freqs_info_list_geran_l carrier_freqs_info_list;
  // ...
  bool          late_non_crit_ext_present = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType8 ::= SEQUENCE
struct sib_type8_s {
  struct params_hrpd_s_ {
    bool                         cell_resel_params_hrpd_present = false;
    pre_regist_info_hrpd_s       pre_regist_info_hrpd;
    cell_resel_params_cdma2000_s cell_resel_params_hrpd;
  };
  struct params1_xrtt_s_ {
    bool                         csfb_regist_param1_xrtt_present = false;
    bool                         long_code_state1_xrtt_present   = false;
    bool                         cell_resel_params1_xrtt_present = false;
    csfb_regist_param1_xrtt_s    csfb_regist_param1_xrtt;
    fixed_bitstring<42>          long_code_state1_xrtt;
    cell_resel_params_cdma2000_s cell_resel_params1_xrtt;
  };

  // member variables
  bool                     ext                     = false;
  bool                     sys_time_info_present   = false;
  bool                     search_win_size_present = false;
  bool                     params_hrpd_present     = false;
  bool                     params1_xrtt_present    = false;
  sys_time_info_cdma2000_s sys_time_info;
  uint8_t                  search_win_size = 0;
  params_hrpd_s_           params_hrpd;
  params1_xrtt_s_          params1_xrtt;
  // ...
  bool          late_non_crit_ext_present = false;
  dyn_octstring late_non_crit_ext;
  // group 0
  bool                                        csfb_support_for_dual_rx_ues_r9_present = false;
  bool                                        csfb_support_for_dual_rx_ues_r9         = false;
  copy_ptr<cell_resel_params_cdma2000_v920_s> cell_resel_params_hrpd_v920;
  copy_ptr<cell_resel_params_cdma2000_v920_s> cell_resel_params1_xrtt_v920;
  copy_ptr<csfb_regist_param1_xrtt_v920_s>    csfb_regist_param1_xrtt_v920;
  copy_ptr<ac_barr_cfg1_xrtt_r9_s>            ac_barr_cfg1_xrtt_r9;
  // group 1
  bool csfb_dual_rx_tx_support_r10_present = false;
  // group 2
  copy_ptr<sib8_per_plmn_list_r11_l> sib8_per_plmn_list_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType9 ::= SEQUENCE
struct sib_type9_s {
  bool                     ext              = false;
  bool                     hnb_name_present = false;
  bounded_octstring<1, 48> hnb_name;
  // ...
  bool          late_non_crit_ext_present = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PosSystemInformation-r15-IEs ::= SEQUENCE
struct pos_sys_info_r15_ies_s {
  struct pos_sib_type_and_info_r15_item_c_ {
    struct types_opts {
      enum options {
        pos_sib1_minus1_r15,
        pos_sib1_minus2_r15,
        pos_sib1_minus3_r15,
        pos_sib1_minus4_r15,
        pos_sib1_minus5_r15,
        pos_sib1_minus6_r15,
        pos_sib1_minus7_r15,
        pos_sib2_minus1_r15,
        pos_sib2_minus2_r15,
        pos_sib2_minus3_r15,
        pos_sib2_minus4_r15,
        pos_sib2_minus5_r15,
        pos_sib2_minus6_r15,
        pos_sib2_minus7_r15,
        pos_sib2_minus8_r15,
        pos_sib2_minus9_r15,
        pos_sib2_minus10_r15,
        pos_sib2_minus11_r15,
        pos_sib2_minus12_r15,
        pos_sib2_minus13_r15,
        pos_sib2_minus14_r15,
        pos_sib2_minus15_r15,
        pos_sib2_minus16_r15,
        pos_sib2_minus17_r15,
        pos_sib2_minus18_r15,
        pos_sib2_minus19_r15,
        pos_sib3_minus1_r15,
        // ...
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts, true> types;

    // choice methods
    pos_sib_type_and_info_r15_item_c_() = default;
    pos_sib_type_and_info_r15_item_c_(const pos_sib_type_and_info_r15_item_c_& other);
    pos_sib_type_and_info_r15_item_c_& operator=(const pos_sib_type_and_info_r15_item_c_& other);
    ~pos_sib_type_and_info_r15_item_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sib_pos_r15_s& pos_sib1_minus1_r15()
    {
      assert_choice_type(types::pos_sib1_minus1_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib1_minus2_r15()
    {
      assert_choice_type(types::pos_sib1_minus2_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib1_minus3_r15()
    {
      assert_choice_type(types::pos_sib1_minus3_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib1_minus4_r15()
    {
      assert_choice_type(types::pos_sib1_minus4_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib1_minus5_r15()
    {
      assert_choice_type(types::pos_sib1_minus5_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib1_minus6_r15()
    {
      assert_choice_type(types::pos_sib1_minus6_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib1_minus7_r15()
    {
      assert_choice_type(types::pos_sib1_minus7_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus1_r15()
    {
      assert_choice_type(types::pos_sib2_minus1_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus2_r15()
    {
      assert_choice_type(types::pos_sib2_minus2_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus3_r15()
    {
      assert_choice_type(types::pos_sib2_minus3_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus4_r15()
    {
      assert_choice_type(types::pos_sib2_minus4_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus5_r15()
    {
      assert_choice_type(types::pos_sib2_minus5_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus6_r15()
    {
      assert_choice_type(types::pos_sib2_minus6_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus7_r15()
    {
      assert_choice_type(types::pos_sib2_minus7_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus8_r15()
    {
      assert_choice_type(types::pos_sib2_minus8_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus9_r15()
    {
      assert_choice_type(types::pos_sib2_minus9_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus10_r15()
    {
      assert_choice_type(types::pos_sib2_minus10_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus11_r15()
    {
      assert_choice_type(types::pos_sib2_minus11_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus12_r15()
    {
      assert_choice_type(types::pos_sib2_minus12_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus13_r15()
    {
      assert_choice_type(types::pos_sib2_minus13_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus14_r15()
    {
      assert_choice_type(types::pos_sib2_minus14_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus15_r15()
    {
      assert_choice_type(types::pos_sib2_minus15_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus16_r15()
    {
      assert_choice_type(types::pos_sib2_minus16_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus17_r15()
    {
      assert_choice_type(types::pos_sib2_minus17_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus18_r15()
    {
      assert_choice_type(types::pos_sib2_minus18_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib2_minus19_r15()
    {
      assert_choice_type(types::pos_sib2_minus19_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& pos_sib3_minus1_r15()
    {
      assert_choice_type(types::pos_sib3_minus1_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib1_minus1_r15() const
    {
      assert_choice_type(types::pos_sib1_minus1_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib1_minus2_r15() const
    {
      assert_choice_type(types::pos_sib1_minus2_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib1_minus3_r15() const
    {
      assert_choice_type(types::pos_sib1_minus3_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib1_minus4_r15() const
    {
      assert_choice_type(types::pos_sib1_minus4_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib1_minus5_r15() const
    {
      assert_choice_type(types::pos_sib1_minus5_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib1_minus6_r15() const
    {
      assert_choice_type(types::pos_sib1_minus6_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib1_minus7_r15() const
    {
      assert_choice_type(types::pos_sib1_minus7_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus1_r15() const
    {
      assert_choice_type(types::pos_sib2_minus1_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus2_r15() const
    {
      assert_choice_type(types::pos_sib2_minus2_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus3_r15() const
    {
      assert_choice_type(types::pos_sib2_minus3_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus4_r15() const
    {
      assert_choice_type(types::pos_sib2_minus4_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus5_r15() const
    {
      assert_choice_type(types::pos_sib2_minus5_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus6_r15() const
    {
      assert_choice_type(types::pos_sib2_minus6_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus7_r15() const
    {
      assert_choice_type(types::pos_sib2_minus7_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus8_r15() const
    {
      assert_choice_type(types::pos_sib2_minus8_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus9_r15() const
    {
      assert_choice_type(types::pos_sib2_minus9_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus10_r15() const
    {
      assert_choice_type(types::pos_sib2_minus10_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus11_r15() const
    {
      assert_choice_type(types::pos_sib2_minus11_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus12_r15() const
    {
      assert_choice_type(types::pos_sib2_minus12_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus13_r15() const
    {
      assert_choice_type(types::pos_sib2_minus13_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus14_r15() const
    {
      assert_choice_type(types::pos_sib2_minus14_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus15_r15() const
    {
      assert_choice_type(types::pos_sib2_minus15_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus16_r15() const
    {
      assert_choice_type(types::pos_sib2_minus16_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus17_r15() const
    {
      assert_choice_type(types::pos_sib2_minus17_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus18_r15() const
    {
      assert_choice_type(types::pos_sib2_minus18_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib2_minus19_r15() const
    {
      assert_choice_type(types::pos_sib2_minus19_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    const sib_pos_r15_s& pos_sib3_minus1_r15() const
    {
      assert_choice_type(types::pos_sib3_minus1_r15, type_, "posSIB-TypeAndInfo-r15-item");
      return c.get<sib_pos_r15_s>();
    }
    sib_pos_r15_s& set_pos_sib1_minus1_r15();
    sib_pos_r15_s& set_pos_sib1_minus2_r15();
    sib_pos_r15_s& set_pos_sib1_minus3_r15();
    sib_pos_r15_s& set_pos_sib1_minus4_r15();
    sib_pos_r15_s& set_pos_sib1_minus5_r15();
    sib_pos_r15_s& set_pos_sib1_minus6_r15();
    sib_pos_r15_s& set_pos_sib1_minus7_r15();
    sib_pos_r15_s& set_pos_sib2_minus1_r15();
    sib_pos_r15_s& set_pos_sib2_minus2_r15();
    sib_pos_r15_s& set_pos_sib2_minus3_r15();
    sib_pos_r15_s& set_pos_sib2_minus4_r15();
    sib_pos_r15_s& set_pos_sib2_minus5_r15();
    sib_pos_r15_s& set_pos_sib2_minus6_r15();
    sib_pos_r15_s& set_pos_sib2_minus7_r15();
    sib_pos_r15_s& set_pos_sib2_minus8_r15();
    sib_pos_r15_s& set_pos_sib2_minus9_r15();
    sib_pos_r15_s& set_pos_sib2_minus10_r15();
    sib_pos_r15_s& set_pos_sib2_minus11_r15();
    sib_pos_r15_s& set_pos_sib2_minus12_r15();
    sib_pos_r15_s& set_pos_sib2_minus13_r15();
    sib_pos_r15_s& set_pos_sib2_minus14_r15();
    sib_pos_r15_s& set_pos_sib2_minus15_r15();
    sib_pos_r15_s& set_pos_sib2_minus16_r15();
    sib_pos_r15_s& set_pos_sib2_minus17_r15();
    sib_pos_r15_s& set_pos_sib2_minus18_r15();
    sib_pos_r15_s& set_pos_sib2_minus19_r15();
    sib_pos_r15_s& set_pos_sib3_minus1_r15();

  private:
    types                          type_;
    choice_buffer_t<sib_pos_r15_s> c;

    void destroy_();
  };
  using pos_sib_type_and_info_r15_l_ = dyn_array<pos_sib_type_and_info_r15_item_c_>;

  // member variables
  bool                         late_non_crit_ext_present = false;
  bool                         non_crit_ext_present      = false;
  pos_sib_type_and_info_r15_l_ pos_sib_type_and_info_r15;
  dyn_octstring                late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

struct sib_info_item_c {
  struct types_opts {
    enum options {
      sib2,
      sib3,
      sib4,
      sib5,
      sib6,
      sib7,
      sib8,
      sib9,
      sib10,
      sib11,
      // ...
      sib12_v920,
      sib13_v920,
      sib14_v1130,
      sib15_v1130,
      sib16_v1130,
      sib17_v1250,
      sib18_v1250,
      sib19_v1250,
      sib20_v1310,
      sib21_v1430,
      sib24_v1530,
      sib25_v1530,
      sib26_v1530,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts, true, 13> types;

  // choice methods
  sib_info_item_c() = default;
  sib_info_item_c(const sib_info_item_c& other);
  sib_info_item_c& operator=(const sib_info_item_c& other);
  ~sib_info_item_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  sib_type2_s& sib2()
  {
    assert_choice_type(types::sib2, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type2_s>();
  }
  sib_type3_s& sib3()
  {
    assert_choice_type(types::sib3, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type3_s>();
  }
  sib_type4_s& sib4()
  {
    assert_choice_type(types::sib4, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type4_s>();
  }
  sib_type5_s& sib5()
  {
    assert_choice_type(types::sib5, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type5_s>();
  }
  sib_type6_s& sib6()
  {
    assert_choice_type(types::sib6, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type6_s>();
  }
  sib_type7_s& sib7()
  {
    assert_choice_type(types::sib7, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type7_s>();
  }
  sib_type8_s& sib8()
  {
    assert_choice_type(types::sib8, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type8_s>();
  }
  sib_type9_s& sib9()
  {
    assert_choice_type(types::sib9, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type9_s>();
  }
  sib_type10_s& sib10()
  {
    assert_choice_type(types::sib10, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type10_s>();
  }
  sib_type11_s& sib11()
  {
    assert_choice_type(types::sib11, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type11_s>();
  }
  sib_type12_r9_s& sib12_v920()
  {
    assert_choice_type(types::sib12_v920, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type12_r9_s>();
  }
  sib_type13_r9_s& sib13_v920()
  {
    assert_choice_type(types::sib13_v920, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type13_r9_s>();
  }
  sib_type14_r11_s& sib14_v1130()
  {
    assert_choice_type(types::sib14_v1130, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type14_r11_s>();
  }
  sib_type15_r11_s& sib15_v1130()
  {
    assert_choice_type(types::sib15_v1130, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type15_r11_s>();
  }
  sib_type16_r11_s& sib16_v1130()
  {
    assert_choice_type(types::sib16_v1130, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type16_r11_s>();
  }
  sib_type17_r12_s& sib17_v1250()
  {
    assert_choice_type(types::sib17_v1250, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type17_r12_s>();
  }
  sib_type18_r12_s& sib18_v1250()
  {
    assert_choice_type(types::sib18_v1250, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type18_r12_s>();
  }
  sib_type19_r12_s& sib19_v1250()
  {
    assert_choice_type(types::sib19_v1250, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type19_r12_s>();
  }
  sib_type20_r13_s& sib20_v1310()
  {
    assert_choice_type(types::sib20_v1310, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type20_r13_s>();
  }
  sib_type21_r14_s& sib21_v1430()
  {
    assert_choice_type(types::sib21_v1430, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type21_r14_s>();
  }
  sib_type24_r15_s& sib24_v1530()
  {
    assert_choice_type(types::sib24_v1530, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type24_r15_s>();
  }
  sib_type25_r15_s& sib25_v1530()
  {
    assert_choice_type(types::sib25_v1530, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type25_r15_s>();
  }
  sib_type26_r15_s& sib26_v1530()
  {
    assert_choice_type(types::sib26_v1530, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type26_r15_s>();
  }
  const sib_type2_s& sib2() const
  {
    assert_choice_type(types::sib2, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type2_s>();
  }
  const sib_type3_s& sib3() const
  {
    assert_choice_type(types::sib3, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type3_s>();
  }
  const sib_type4_s& sib4() const
  {
    assert_choice_type(types::sib4, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type4_s>();
  }
  const sib_type5_s& sib5() const
  {
    assert_choice_type(types::sib5, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type5_s>();
  }
  const sib_type6_s& sib6() const
  {
    assert_choice_type(types::sib6, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type6_s>();
  }
  const sib_type7_s& sib7() const
  {
    assert_choice_type(types::sib7, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type7_s>();
  }
  const sib_type8_s& sib8() const
  {
    assert_choice_type(types::sib8, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type8_s>();
  }
  const sib_type9_s& sib9() const
  {
    assert_choice_type(types::sib9, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type9_s>();
  }
  const sib_type10_s& sib10() const
  {
    assert_choice_type(types::sib10, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type10_s>();
  }
  const sib_type11_s& sib11() const
  {
    assert_choice_type(types::sib11, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type11_s>();
  }
  const sib_type12_r9_s& sib12_v920() const
  {
    assert_choice_type(types::sib12_v920, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type12_r9_s>();
  }
  const sib_type13_r9_s& sib13_v920() const
  {
    assert_choice_type(types::sib13_v920, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type13_r9_s>();
  }
  const sib_type14_r11_s& sib14_v1130() const
  {
    assert_choice_type(types::sib14_v1130, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type14_r11_s>();
  }
  const sib_type15_r11_s& sib15_v1130() const
  {
    assert_choice_type(types::sib15_v1130, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type15_r11_s>();
  }
  const sib_type16_r11_s& sib16_v1130() const
  {
    assert_choice_type(types::sib16_v1130, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type16_r11_s>();
  }
  const sib_type17_r12_s& sib17_v1250() const
  {
    assert_choice_type(types::sib17_v1250, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type17_r12_s>();
  }
  const sib_type18_r12_s& sib18_v1250() const
  {
    assert_choice_type(types::sib18_v1250, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type18_r12_s>();
  }
  const sib_type19_r12_s& sib19_v1250() const
  {
    assert_choice_type(types::sib19_v1250, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type19_r12_s>();
  }
  const sib_type20_r13_s& sib20_v1310() const
  {
    assert_choice_type(types::sib20_v1310, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type20_r13_s>();
  }
  const sib_type21_r14_s& sib21_v1430() const
  {
    assert_choice_type(types::sib21_v1430, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type21_r14_s>();
  }
  const sib_type24_r15_s& sib24_v1530() const
  {
    assert_choice_type(types::sib24_v1530, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type24_r15_s>();
  }
  const sib_type25_r15_s& sib25_v1530() const
  {
    assert_choice_type(types::sib25_v1530, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type25_r15_s>();
  }
  const sib_type26_r15_s& sib26_v1530() const
  {
    assert_choice_type(types::sib26_v1530, type_, "sib-TypeAndInfo-item");
    return c.get<sib_type26_r15_s>();
  }
  sib_type2_s&      set_sib2();
  sib_type3_s&      set_sib3();
  sib_type4_s&      set_sib4();
  sib_type5_s&      set_sib5();
  sib_type6_s&      set_sib6();
  sib_type7_s&      set_sib7();
  sib_type8_s&      set_sib8();
  sib_type9_s&      set_sib9();
  sib_type10_s&     set_sib10();
  sib_type11_s&     set_sib11();
  sib_type12_r9_s&  set_sib12_v920();
  sib_type13_r9_s&  set_sib13_v920();
  sib_type14_r11_s& set_sib14_v1130();
  sib_type15_r11_s& set_sib15_v1130();
  sib_type16_r11_s& set_sib16_v1130();
  sib_type17_r12_s& set_sib17_v1250();
  sib_type18_r12_s& set_sib18_v1250();
  sib_type19_r12_s& set_sib19_v1250();
  sib_type20_r13_s& set_sib20_v1310();
  sib_type21_r14_s& set_sib21_v1430();
  sib_type24_r15_s& set_sib24_v1530();
  sib_type25_r15_s& set_sib25_v1530();
  sib_type26_r15_s& set_sib26_v1530();

private:
  types type_;
  choice_buffer_t<sib_type10_s,
                  sib_type11_s,
                  sib_type12_r9_s,
                  sib_type13_r9_s,
                  sib_type14_r11_s,
                  sib_type15_r11_s,
                  sib_type16_r11_s,
                  sib_type17_r12_s,
                  sib_type18_r12_s,
                  sib_type19_r12_s,
                  sib_type20_r13_s,
                  sib_type21_r14_s,
                  sib_type24_r15_s,
                  sib_type25_r15_s,
                  sib_type26_r15_s,
                  sib_type2_s,
                  sib_type3_s,
                  sib_type4_s,
                  sib_type5_s,
                  sib_type6_s,
                  sib_type7_s,
                  sib_type8_s,
                  sib_type9_s>
      c;

  void destroy_();
};

// SystemInformation-r8-IEs ::= SEQUENCE
struct sys_info_r8_ies_s {
  using sib_type_and_info_item_c_ = sib_info_item_c;
  using sib_type_and_info_l_      = dyn_array<sib_type_and_info_item_c_>;

  // member variables
  bool                 non_crit_ext_present = false;
  sib_type_and_info_l_ sib_type_and_info;
  sys_info_v8a0_ies_s  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformation ::= SEQUENCE
struct sys_info_s {
  struct crit_exts_c_ {
    struct crit_exts_future_r15_c_ {
      struct types_opts {
        enum options { pos_sys_info_r15, crit_exts_future, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      crit_exts_future_r15_c_() = default;
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      pos_sys_info_r15_ies_s& pos_sys_info_r15()
      {
        assert_choice_type(types::pos_sys_info_r15, type_, "criticalExtensionsFuture-r15");
        return c;
      }
      const pos_sys_info_r15_ies_s& pos_sys_info_r15() const
      {
        assert_choice_type(types::pos_sys_info_r15, type_, "criticalExtensionsFuture-r15");
        return c;
      }
      pos_sys_info_r15_ies_s& set_pos_sys_info_r15();
      void                    set_crit_exts_future();

    private:
      types                  type_;
      pos_sys_info_r15_ies_s c;
    };
    struct types_opts {
      enum options { sys_info_r8, crit_exts_future_r15, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    crit_exts_c_(const crit_exts_c_& other);
    crit_exts_c_& operator=(const crit_exts_c_& other);
    ~crit_exts_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sys_info_r8_ies_s& sys_info_r8()
    {
      assert_choice_type(types::sys_info_r8, type_, "criticalExtensions");
      return c.get<sys_info_r8_ies_s>();
    }
    crit_exts_future_r15_c_& crit_exts_future_r15()
    {
      assert_choice_type(types::crit_exts_future_r15, type_, "criticalExtensions");
      return c.get<crit_exts_future_r15_c_>();
    }
    const sys_info_r8_ies_s& sys_info_r8() const
    {
      assert_choice_type(types::sys_info_r8, type_, "criticalExtensions");
      return c.get<sys_info_r8_ies_s>();
    }
    const crit_exts_future_r15_c_& crit_exts_future_r15() const
    {
      assert_choice_type(types::crit_exts_future_r15, type_, "criticalExtensions");
      return c.get<crit_exts_future_r15_c_>();
    }
    sys_info_r8_ies_s&       set_sys_info_r8();
    crit_exts_future_r15_c_& set_crit_exts_future_r15();

  private:
    types                                                       type_;
    choice_buffer_t<crit_exts_future_r15_c_, sys_info_r8_ies_s> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCCH-DL-SCH-MessageType ::= CHOICE
struct bcch_dl_sch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { sys_info, sib_type1, nulltype } value;
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
    sys_info_s& sys_info()
    {
      assert_choice_type(types::sys_info, type_, "c1");
      return c.get<sys_info_s>();
    }
    sib_type1_s& sib_type1()
    {
      assert_choice_type(types::sib_type1, type_, "c1");
      return c.get<sib_type1_s>();
    }
    const sys_info_s& sys_info() const
    {
      assert_choice_type(types::sys_info, type_, "c1");
      return c.get<sys_info_s>();
    }
    const sib_type1_s& sib_type1() const
    {
      assert_choice_type(types::sib_type1, type_, "c1");
      return c.get<sib_type1_s>();
    }
    sys_info_s&  set_sys_info();
    sib_type1_s& set_sib_type1();

  private:
    types                                    type_;
    choice_buffer_t<sib_type1_s, sys_info_s> c;

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
  bcch_dl_sch_msg_type_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "BCCH-DL-SCH-MessageType");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "BCCH-DL-SCH-MessageType");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// BCCH-DL-SCH-Message ::= SEQUENCE
struct bcch_dl_sch_msg_s {
  bcch_dl_sch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformation-BR-r13 ::= SystemInformation
using sys_info_br_r13_s = sys_info_s;

// SystemInformationBlockType1-BR-r13 ::= SystemInformationBlockType1
using sib_type1_br_r13_s = sib_type1_s;

// BCCH-DL-SCH-MessageType-BR-r13 ::= CHOICE
struct bcch_dl_sch_msg_type_br_r13_c {
  struct c1_c_ {
    struct types_opts {
      enum options { sys_info_br_r13, sib_type1_br_r13, nulltype } value;
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
    sys_info_br_r13_s& sys_info_br_r13()
    {
      assert_choice_type(types::sys_info_br_r13, type_, "c1");
      return c.get<sys_info_br_r13_s>();
    }
    sib_type1_br_r13_s& sib_type1_br_r13()
    {
      assert_choice_type(types::sib_type1_br_r13, type_, "c1");
      return c.get<sib_type1_br_r13_s>();
    }
    const sys_info_br_r13_s& sys_info_br_r13() const
    {
      assert_choice_type(types::sys_info_br_r13, type_, "c1");
      return c.get<sys_info_br_r13_s>();
    }
    const sib_type1_br_r13_s& sib_type1_br_r13() const
    {
      assert_choice_type(types::sib_type1_br_r13, type_, "c1");
      return c.get<sib_type1_br_r13_s>();
    }
    sys_info_br_r13_s&  set_sys_info_br_r13();
    sib_type1_br_r13_s& set_sib_type1_br_r13();

  private:
    types                                                  type_;
    choice_buffer_t<sib_type1_br_r13_s, sys_info_br_r13_s> c;

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
  bcch_dl_sch_msg_type_br_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "BCCH-DL-SCH-MessageType-BR-r13");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "BCCH-DL-SCH-MessageType-BR-r13");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// BCCH-DL-SCH-Message-BR ::= SEQUENCE
struct bcch_dl_sch_msg_br_s {
  bcch_dl_sch_msg_type_br_r13_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB-Type-MBMS-r14 ::= ENUMERATED
struct sib_type_mbms_r14_opts {
  enum options {
    sib_type10,
    sib_type11,
    sib_type12_v920,
    sib_type13_v920,
    sib_type15_v1130,
    sib_type16_v1130,
    // ...
    nulltype
  } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<sib_type_mbms_r14_opts, true> sib_type_mbms_r14_e;

// SIB-MappingInfo-MBMS-r14 ::= SEQUENCE (SIZE (0..31)) OF SIB-Type-MBMS-r14
using sib_map_info_mbms_r14_l = bounded_array<sib_type_mbms_r14_e, 31>;

// SchedulingInfo-MBMS-r14 ::= SEQUENCE
struct sched_info_mbms_r14_s {
  struct si_periodicity_r14_opts {
    enum options { rf16, rf32, rf64, rf128, rf256, rf512, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<si_periodicity_r14_opts> si_periodicity_r14_e_;

  // member variables
  si_periodicity_r14_e_   si_periodicity_r14;
  sib_map_info_mbms_r14_l sib_map_info_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NonMBSFN-SubframeConfig-r14 ::= SEQUENCE
struct non_mbsfn_sf_cfg_r14_s {
  struct radio_frame_alloc_period_r14_opts {
    enum options { rf4, rf8, rf16, rf32, rf64, rf128, rf512, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<radio_frame_alloc_period_r14_opts> radio_frame_alloc_period_r14_e_;

  // member variables
  radio_frame_alloc_period_r14_e_ radio_frame_alloc_period_r14;
  uint8_t                         radio_frame_alloc_offset_r14 = 0;
  fixed_bitstring<9>              sf_alloc_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityList-MBMS-r14 ::= SEQUENCE (SIZE (1..6)) OF PLMN-Identity
using plmn_id_list_mbms_r14_l = dyn_array<plmn_id_s>;

// SchedulingInfoList-MBMS-r14 ::= SEQUENCE (SIZE (1..32)) OF SchedulingInfo-MBMS-r14
using sched_info_list_mbms_r14_l = dyn_array<sched_info_mbms_r14_s>;

// SystemInformationBlockType1-MBMS-r14 ::= SEQUENCE
struct sib_type1_mbms_r14_s {
  struct cell_access_related_info_r14_s_ {
    plmn_id_list_mbms_r14_l plmn_id_list_r14;
    fixed_bitstring<16>     tac_r14;
    fixed_bitstring<28>     cell_id_r14;
  };
  struct si_win_len_r14_opts {
    enum options { ms1, ms2, ms5, ms10, ms15, ms20, ms40, ms80, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<si_win_len_r14_opts> si_win_len_r14_e_;
  using cell_access_related_info_list_r14_l_ = dyn_array<cell_access_related_info_r14_s>;

  // member variables
  bool                                 multi_band_info_list_r14_present          = false;
  bool                                 non_mbsfn_sf_cfg_r14_present              = false;
  bool                                 sib_type13_r14_present                    = false;
  bool                                 cell_access_related_info_list_r14_present = false;
  bool                                 non_crit_ext_present                      = false;
  cell_access_related_info_r14_s_      cell_access_related_info_r14;
  uint16_t                             freq_band_ind_r14 = 1;
  multi_band_info_list_r11_l           multi_band_info_list_r14;
  sched_info_list_mbms_r14_l           sched_info_list_mbms_r14;
  si_win_len_r14_e_                    si_win_len_r14;
  uint8_t                              sys_info_value_tag_r14 = 0;
  non_mbsfn_sf_cfg_r14_s               non_mbsfn_sf_cfg_r14;
  pdsch_cfg_common_s                   pdsch_cfg_common_r14;
  sib_type13_r9_s                      sib_type13_r14;
  cell_access_related_info_list_r14_l_ cell_access_related_info_list_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqInfoUTRA-FDD-v8h0 ::= SEQUENCE
struct carrier_freq_info_utra_fdd_v8h0_s {
  using multi_band_info_list_l_ = bounded_array<uint8_t, 8>;

  // member variables
  bool                    multi_band_info_list_present = false;
  multi_band_info_list_l_ multi_band_info_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NS-PmaxValue-v10l0 ::= SEQUENCE
struct ns_pmax_value_v10l0_s {
  bool     add_spec_emission_v10l0_present = false;
  uint16_t add_spec_emission_v10l0         = 33;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NS-PmaxList-v10l0 ::= SEQUENCE (SIZE (1..8)) OF NS-PmaxValue-v10l0
using ns_pmax_list_v10l0_l = dyn_array<ns_pmax_value_v10l0_s>;

// MultiBandInfoList-v10l0 ::= SEQUENCE (SIZE (1..8)) OF NS-PmaxList-v10l0
using multi_band_info_list_v10l0_l = dyn_array<ns_pmax_list_v10l0_l>;

// InterFreqCarrierFreqInfo-v10l0 ::= SEQUENCE
struct inter_freq_carrier_freq_info_v10l0_s {
  bool                         freq_band_info_v10l0_present       = false;
  bool                         multi_band_info_list_v10l0_present = false;
  ns_pmax_list_v10l0_l         freq_band_info_v10l0;
  multi_band_info_list_v10l0_l multi_band_info_list_v10l0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MultiBandInfoList ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..64)
using multi_band_info_list_l = bounded_array<uint8_t, 8>;

// InterFreqCarrierFreqInfo-v8h0 ::= SEQUENCE
struct inter_freq_carrier_freq_info_v8h0_s {
  bool                   multi_band_info_list_present = false;
  multi_band_info_list_l multi_band_info_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MultiBandInfo-v9e0 ::= SEQUENCE
struct multi_band_info_v9e0_s {
  bool     freq_band_ind_v9e0_present = false;
  uint16_t freq_band_ind_v9e0         = 65;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MultiBandInfoList-v9e0 ::= SEQUENCE (SIZE (1..8)) OF MultiBandInfo-v9e0
using multi_band_info_list_v9e0_l = dyn_array<multi_band_info_v9e0_s>;

// InterFreqCarrierFreqInfo-v9e0 ::= SEQUENCE
struct inter_freq_carrier_freq_info_v9e0_s {
  bool                        dl_carrier_freq_v9e0_present      = false;
  bool                        multi_band_info_list_v9e0_present = false;
  uint32_t                    dl_carrier_freq_v9e0              = 65536;
  multi_band_info_list_v9e0_l multi_band_info_list_v9e0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqList-v13a0 ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo-v1360
using inter_freq_carrier_freq_list_v13a0_l = dyn_array<inter_freq_carrier_freq_info_v1360_s>;

// SIB-Type-v12j0 ::= ENUMERATED
struct sib_type_v12j0_opts {
  enum options {
    sib_type19_v1250,
    sib_type20_v1310,
    sib_type21_v1430,
    sib_type24_v1530,
    sib_type25_v1530,
    sib_type26_v1530,
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
typedef enumerated<sib_type_v12j0_opts, true> sib_type_v12j0_e;

// SIB-MappingInfo-v12j0 ::= SEQUENCE (SIZE (1..31)) OF SIB-Type-v12j0
using sib_map_info_v12j0_l = bounded_array<sib_type_v12j0_e, 31>;

// SchedulingInfo-v12j0 ::= SEQUENCE
struct sched_info_v12j0_s {
  bool                 sib_map_info_v12j0_present = false;
  sib_map_info_v12j0_l sib_map_info_v12j0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingInfoExt-r12 ::= SEQUENCE
struct sched_info_ext_r12_s {
  si_periodicity_r12_e si_periodicity_r12;
  sib_map_info_v12j0_l sib_map_info_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingInfoList-v12j0 ::= SEQUENCE (SIZE (1..32)) OF SchedulingInfo-v12j0
using sched_info_list_v12j0_l = dyn_array<sched_info_v12j0_s>;

// SchedulingInfoListExt-r12 ::= SEQUENCE (SIZE (1..32)) OF SchedulingInfoExt-r12
using sched_info_list_ext_r12_l = dyn_array<sched_info_ext_r12_s>;

// SystemInformationBlockType1-v12j0-IEs ::= SEQUENCE
struct sib_type1_v12j0_ies_s {
  bool                      sched_info_list_v12j0_present   = false;
  bool                      sched_info_list_ext_r12_present = false;
  bool                      non_crit_ext_present            = false;
  sched_info_list_v12j0_l   sched_info_list_v12j0;
  sched_info_list_ext_r12_l sched_info_list_ext_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1-v10x0-IEs ::= SEQUENCE
struct sib_type1_v10x0_ies_s {
  bool                  late_non_crit_ext_present = false;
  bool                  non_crit_ext_present      = false;
  dyn_octstring         late_non_crit_ext;
  sib_type1_v12j0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1-v10l0-IEs ::= SEQUENCE
struct sib_type1_v10l0_ies_s {
  bool                         freq_band_info_v10l0_present       = false;
  bool                         multi_band_info_list_v10l0_present = false;
  bool                         non_crit_ext_present               = false;
  ns_pmax_list_v10l0_l         freq_band_info_v10l0;
  multi_band_info_list_v10l0_l multi_band_info_list_v10l0;
  sib_type1_v10x0_ies_s        non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1-v10j0-IEs ::= SEQUENCE
struct sib_type1_v10j0_ies_s {
  bool                         freq_band_info_r10_present         = false;
  bool                         multi_band_info_list_v10j0_present = false;
  bool                         non_crit_ext_present               = false;
  ns_pmax_list_r10_l           freq_band_info_r10;
  multi_band_info_list_v10j0_l multi_band_info_list_v10j0;
  sib_type1_v10l0_ies_s        non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1-v9e0-IEs ::= SEQUENCE
struct sib_type1_v9e0_ies_s {
  bool                        freq_band_ind_v9e0_present        = false;
  bool                        multi_band_info_list_v9e0_present = false;
  bool                        non_crit_ext_present              = false;
  uint16_t                    freq_band_ind_v9e0                = 65;
  multi_band_info_list_v9e0_l multi_band_info_list_v9e0;
  sib_type1_v10j0_ies_s       non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1-v8h0-IEs ::= SEQUENCE
struct sib_type1_v8h0_ies_s {
  bool                   multi_band_info_list_present = false;
  bool                   non_crit_ext_present         = false;
  multi_band_info_list_l multi_band_info_list;
  sib_type1_v9e0_ies_s   non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType2-v13c0-IEs ::= SEQUENCE
struct sib_type2_v13c0_ies_s {
  bool                       ul_pwr_ctrl_common_v13c0_present = false;
  bool                       non_crit_ext_present             = false;
  ul_pwr_ctrl_common_v1310_s ul_pwr_ctrl_common_v13c0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType2-v10n0-IEs ::= SEQUENCE
struct sib_type2_v10n0_ies_s {
  bool                  late_non_crit_ext_present = false;
  bool                  non_crit_ext_present      = false;
  dyn_octstring         late_non_crit_ext;
  sib_type2_v13c0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType2-v10m0-IEs ::= SEQUENCE
struct sib_type2_v10m0_ies_s {
  struct freq_info_v10l0_s_ {
    uint16_t add_spec_emission_v10l0 = 33;
  };
  using multi_band_info_list_v10l0_l_ = bounded_array<uint16_t, 8>;

  // member variables
  bool                          freq_info_v10l0_present            = false;
  bool                          multi_band_info_list_v10l0_present = false;
  bool                          non_crit_ext_present               = false;
  freq_info_v10l0_s_            freq_info_v10l0;
  multi_band_info_list_v10l0_l_ multi_band_info_list_v10l0;
  sib_type2_v10n0_ies_s         non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType2-v9i0-IEs ::= SEQUENCE
struct sib_type2_v9i0_ies_s {
  bool          non_crit_ext_present = false;
  bool          dummy_present        = false;
  dyn_octstring non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType2-v9e0-IEs ::= SEQUENCE
struct sib_type2_v9e0_ies_s {
  bool                 ul_carrier_freq_v9e0_present = false;
  bool                 non_crit_ext_present         = false;
  uint32_t             ul_carrier_freq_v9e0         = 65536;
  sib_type2_v9i0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType2-v8h0-IEs ::= SEQUENCE
struct sib_type2_v8h0_ies_s {
  using multi_band_info_list_l_ = bounded_array<uint8_t, 8>;

  // member variables
  bool                    multi_band_info_list_present = false;
  bool                    non_crit_ext_present         = false;
  multi_band_info_list_l_ multi_band_info_list;
  sib_type2_v9e0_ies_s    non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType3-v10l0-IEs ::= SEQUENCE
struct sib_type3_v10l0_ies_s {
  bool                         freq_band_info_v10l0_present       = false;
  bool                         multi_band_info_list_v10l0_present = false;
  bool                         non_crit_ext_present               = false;
  ns_pmax_list_v10l0_l         freq_band_info_v10l0;
  multi_band_info_list_v10l0_l multi_band_info_list_v10l0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType3-v10j0-IEs ::= SEQUENCE
struct sib_type3_v10j0_ies_s {
  bool                         freq_band_info_r10_present         = false;
  bool                         multi_band_info_list_v10j0_present = false;
  bool                         non_crit_ext_present               = false;
  ns_pmax_list_r10_l           freq_band_info_r10;
  multi_band_info_list_v10j0_l multi_band_info_list_v10j0;
  sib_type3_v10l0_ies_s        non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType5-v13a0-IEs ::= SEQUENCE
struct sib_type5_v13a0_ies_s {
  bool                                 late_non_crit_ext_present                  = false;
  bool                                 inter_freq_carrier_freq_list_v13a0_present = false;
  bool                                 non_crit_ext_present                       = false;
  dyn_octstring                        late_non_crit_ext;
  inter_freq_carrier_freq_list_v13a0_l inter_freq_carrier_freq_list_v13a0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType5-v10l0-IEs ::= SEQUENCE
struct sib_type5_v10l0_ies_s {
  using inter_freq_carrier_freq_list_v10l0_l_ = dyn_array<inter_freq_carrier_freq_info_v10l0_s>;

  // member variables
  bool                                  inter_freq_carrier_freq_list_v10l0_present = false;
  bool                                  non_crit_ext_present                       = false;
  inter_freq_carrier_freq_list_v10l0_l_ inter_freq_carrier_freq_list_v10l0;
  sib_type5_v13a0_ies_s                 non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType5-v10j0-IEs ::= SEQUENCE
struct sib_type5_v10j0_ies_s {
  using inter_freq_carrier_freq_list_v10j0_l_ = dyn_array<inter_freq_carrier_freq_info_v10j0_s>;

  // member variables
  bool                                  inter_freq_carrier_freq_list_v10j0_present = false;
  bool                                  non_crit_ext_present                       = false;
  inter_freq_carrier_freq_list_v10j0_l_ inter_freq_carrier_freq_list_v10j0;
  sib_type5_v10l0_ies_s                 non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType5-v9e0-IEs ::= SEQUENCE
struct sib_type5_v9e0_ies_s {
  using inter_freq_carrier_freq_list_v9e0_l_ = dyn_array<inter_freq_carrier_freq_info_v9e0_s>;

  // member variables
  bool                                 inter_freq_carrier_freq_list_v9e0_present = false;
  bool                                 non_crit_ext_present                      = false;
  inter_freq_carrier_freq_list_v9e0_l_ inter_freq_carrier_freq_list_v9e0;
  sib_type5_v10j0_ies_s                non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType5-v8h0-IEs ::= SEQUENCE
struct sib_type5_v8h0_ies_s {
  using inter_freq_carrier_freq_list_v8h0_l_ = dyn_array<inter_freq_carrier_freq_info_v8h0_s>;

  // member variables
  bool                                 inter_freq_carrier_freq_list_v8h0_present = false;
  bool                                 non_crit_ext_present                      = false;
  inter_freq_carrier_freq_list_v8h0_l_ inter_freq_carrier_freq_list_v8h0;
  sib_type5_v9e0_ies_s                 non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType6-v8h0-IEs ::= SEQUENCE
struct sib_type6_v8h0_ies_s {
  using carrier_freq_list_utra_fdd_v8h0_l_ = dyn_array<carrier_freq_info_utra_fdd_v8h0_s>;

  // member variables
  bool                               carrier_freq_list_utra_fdd_v8h0_present = false;
  bool                               non_crit_ext_present                    = false;
  carrier_freq_list_utra_fdd_v8h0_l_ carrier_freq_list_utra_fdd_v8h0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType16-NB-r13 ::= SystemInformationBlockType16-r11
using sib_type16_nb_r13_s = sib_type16_r11_s;

} // namespace rrc
} // namespace asn1

#endif // SRSASN1_RRC_BCCH_MSG_H
