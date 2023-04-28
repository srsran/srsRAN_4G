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

#ifndef SRSASN1_RRC_ULDCCH_MSG_H
#define SRSASN1_RRC_ULDCCH_MSG_H

#include "meascfg.h"
#include "paging.h"
#include "security.h"
#include "uecap.h"

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// TMGI-r9 ::= SEQUENCE
struct tmgi_r9_s {
  struct plmn_id_r9_c_ {
    struct types_opts {
      enum options { plmn_idx_r9, explicit_value_r9, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    plmn_id_r9_c_() = default;
    plmn_id_r9_c_(const plmn_id_r9_c_& other);
    plmn_id_r9_c_& operator=(const plmn_id_r9_c_& other);
    ~plmn_id_r9_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& plmn_idx_r9()
    {
      assert_choice_type(types::plmn_idx_r9, type_, "plmn-Id-r9");
      return c.get<uint8_t>();
    }
    plmn_id_s& explicit_value_r9()
    {
      assert_choice_type(types::explicit_value_r9, type_, "plmn-Id-r9");
      return c.get<plmn_id_s>();
    }
    const uint8_t& plmn_idx_r9() const
    {
      assert_choice_type(types::plmn_idx_r9, type_, "plmn-Id-r9");
      return c.get<uint8_t>();
    }
    const plmn_id_s& explicit_value_r9() const
    {
      assert_choice_type(types::explicit_value_r9, type_, "plmn-Id-r9");
      return c.get<plmn_id_s>();
    }
    uint8_t&   set_plmn_idx_r9();
    plmn_id_s& set_explicit_value_r9();

  private:
    types                      type_;
    choice_buffer_t<plmn_id_s> c;

    void destroy_();
  };

  // member variables
  plmn_id_r9_c_      plmn_id_r9;
  fixed_octstring<3> service_id_r9;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GNSS-ValidityDuration-r17 ::= ENUMERATED
struct gnss_validity_dur_r17_opts {
  enum options {
    s10,
    s20,
    s30,
    s40,
    s50,
    s60,
    min5,
    min10,
    min15,
    min20,
    min25,
    min30,
    min50,
    min90,
    min120,
    infinity,
    nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<gnss_validity_dur_r17_opts> gnss_validity_dur_r17_e;

// RRCConnectionSetupComplete-v1710-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_v1710_ies_s {
  bool                    gnss_validity_dur_r17_present = false;
  bool                    non_crit_ext_present          = false;
  gnss_validity_dur_r17_e gnss_validity_dur_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-v1690-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_v1690_ies_s {
  bool                                ul_rrc_segmentation_r16_present = false;
  bool                                non_crit_ext_present            = false;
  rrc_conn_setup_complete_v1710_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResultsPerSSB-IndexIdle-r16 ::= SEQUENCE
struct results_per_ssb_idx_idle_r16_s {
  struct ssb_results_r16_s_ {
    bool    ssb_rsrp_result_r16_present = false;
    bool    ssb_rsrq_result_r16_present = false;
    uint8_t ssb_rsrp_result_r16         = 0;
    uint8_t ssb_rsrq_result_r16         = 0;
  };

  // member variables
  bool               ssb_results_r16_present = false;
  uint8_t            ssb_idx_r16             = 0;
  ssb_results_r16_s_ ssb_results_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-v1610-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_v1610_ies_s {
  bool                                rlos_request_r16_present             = false;
  bool                                cp_cio_t_minus5_gs_optim_r16_present = false;
  bool                                up_cio_t_minus5_gs_optim_r16_present = false;
  bool                                pur_cfg_id_r16_present               = false;
  bool                                lte_m_r16_present                    = false;
  bool                                iab_node_ind_r16_present             = false;
  bool                                non_crit_ext_present                 = false;
  fixed_bitstring<20>                 pur_cfg_id_r16;
  rrc_conn_setup_complete_v1690_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResultsPerSSB-IndexList-r16 ::= SEQUENCE (SIZE (1..32)) OF ResultsPerSSB-IndexIdle-r16
using results_per_ssb_idx_list_r16_l = dyn_array<results_per_ssb_idx_idle_r16_s>;

// MeasResultIdleEUTRA-r15 ::= SEQUENCE
struct meas_result_idle_eutra_r15_s {
  struct meas_result_r15_s_ {
    uint8_t rsrp_result_r15 = 0;
    int8_t  rsrq_result_r15 = -30;
  };

  // member variables
  bool               ext              = false;
  uint32_t           carrier_freq_r15 = 0;
  uint16_t           pci_r15          = 0;
  meas_result_r15_s_ meas_result_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultsPerCellIdleNR-r16 ::= SEQUENCE
struct meas_results_per_cell_idle_nr_r16_s {
  struct meas_idle_result_nr_r16_s_ {
    bool                           rsrp_result_nr_r16_present     = false;
    bool                           rsrq_result_nr_r16_present     = false;
    bool                           result_rs_idx_list_r16_present = false;
    uint8_t                        rsrp_result_nr_r16             = 0;
    uint8_t                        rsrq_result_nr_r16             = 0;
    results_per_ssb_idx_list_r16_l result_rs_idx_list_r16;
  };

  // member variables
  bool                       ext        = false;
  uint16_t                   pci_nr_r16 = 0;
  meas_idle_result_nr_r16_s_ meas_idle_result_nr_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfigurationComplete-v1710-IEs ::= SEQUENCE
struct rrc_conn_recfg_complete_v1710_ies_s {
  bool                    gnss_validity_dur_r17_present = false;
  bool                    non_crit_ext_present          = false;
  gnss_validity_dur_r17_e gnss_validity_dur_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-v1540-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_v1540_ies_s {
  struct guami_type_r15_opts {
    enum options { native, mapped, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<guami_type_r15_opts> guami_type_r15_e_;

  // member variables
  bool                                gummei_type_v1540_present = false;
  bool                                guami_type_r15_present    = false;
  bool                                non_crit_ext_present      = false;
  guami_type_r15_e_                   guami_type_r15;
  rrc_conn_setup_complete_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RegisteredAMF-r15 ::= SEQUENCE
struct registered_amf_r15_s {
  bool                plmn_id_r15_present = false;
  plmn_id_s           plmn_id_r15;
  fixed_bitstring<24> amf_id_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// S-NSSAI-r15 ::= CHOICE
struct s_nssai_r15_c {
  struct types_opts {
    enum options { sst, sst_sd, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  s_nssai_r15_c() = default;
  s_nssai_r15_c(const s_nssai_r15_c& other);
  s_nssai_r15_c& operator=(const s_nssai_r15_c& other);
  ~s_nssai_r15_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<8>& sst()
  {
    assert_choice_type(types::sst, type_, "S-NSSAI-r15");
    return c.get<fixed_bitstring<8> >();
  }
  fixed_bitstring<32>& sst_sd()
  {
    assert_choice_type(types::sst_sd, type_, "S-NSSAI-r15");
    return c.get<fixed_bitstring<32> >();
  }
  const fixed_bitstring<8>& sst() const
  {
    assert_choice_type(types::sst, type_, "S-NSSAI-r15");
    return c.get<fixed_bitstring<8> >();
  }
  const fixed_bitstring<32>& sst_sd() const
  {
    assert_choice_type(types::sst_sd, type_, "S-NSSAI-r15");
    return c.get<fixed_bitstring<32> >();
  }
  fixed_bitstring<8>&  set_sst();
  fixed_bitstring<32>& set_sst_sd();

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<32> > c;

  void destroy_();
};

// MeasResultIdleListEUTRA-r15 ::= SEQUENCE (SIZE (1..8)) OF MeasResultIdleEUTRA-r15
using meas_result_idle_list_eutra_r15_l = dyn_array<meas_result_idle_eutra_r15_s>;

// MeasResultIdleNR-r16 ::= SEQUENCE
struct meas_result_idle_nr_r16_s {
  using meas_results_per_cell_list_idle_nr_r16_l_ = dyn_array<meas_results_per_cell_idle_nr_r16_s>;

  // member variables
  bool                                      ext                 = false;
  uint32_t                                  carrier_freq_nr_r16 = 0;
  meas_results_per_cell_list_idle_nr_r16_l_ meas_results_per_cell_list_idle_nr_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfigurationComplete-v1700-IEs ::= SEQUENCE
struct rrc_conn_recfg_complete_v1700_ies_s {
  bool                                sel_cond_recfg_to_apply_r17_present = false;
  bool                                non_crit_ext_present                = false;
  uint8_t                             sel_cond_recfg_to_apply_r17         = 1;
  rrc_conn_recfg_complete_v1710_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-v1530-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_v1530_ies_s {
  using s_nssai_list_r15_l_ = dyn_array<s_nssai_r15_c>;
  struct ng_minus5_g_s_tmsi_bits_r15_c_ {
    struct types_opts {
      enum options { ng_minus5_g_s_tmsi_r15, ng_minus5_g_s_tmsi_part2_r15, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ng_minus5_g_s_tmsi_bits_r15_c_() = default;
    ng_minus5_g_s_tmsi_bits_r15_c_(const ng_minus5_g_s_tmsi_bits_r15_c_& other);
    ng_minus5_g_s_tmsi_bits_r15_c_& operator=(const ng_minus5_g_s_tmsi_bits_r15_c_& other);
    ~ng_minus5_g_s_tmsi_bits_r15_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<48>& ng_minus5_g_s_tmsi_r15()
    {
      assert_choice_type(types::ng_minus5_g_s_tmsi_r15, type_, "ng-5G-S-TMSI-Bits-r15");
      return c.get<fixed_bitstring<48> >();
    }
    fixed_bitstring<8>& ng_minus5_g_s_tmsi_part2_r15()
    {
      assert_choice_type(types::ng_minus5_g_s_tmsi_part2_r15, type_, "ng-5G-S-TMSI-Bits-r15");
      return c.get<fixed_bitstring<8> >();
    }
    const fixed_bitstring<48>& ng_minus5_g_s_tmsi_r15() const
    {
      assert_choice_type(types::ng_minus5_g_s_tmsi_r15, type_, "ng-5G-S-TMSI-Bits-r15");
      return c.get<fixed_bitstring<48> >();
    }
    const fixed_bitstring<8>& ng_minus5_g_s_tmsi_part2_r15() const
    {
      assert_choice_type(types::ng_minus5_g_s_tmsi_part2_r15, type_, "ng-5G-S-TMSI-Bits-r15");
      return c.get<fixed_bitstring<8> >();
    }
    fixed_bitstring<48>& set_ng_minus5_g_s_tmsi_r15();
    fixed_bitstring<8>&  set_ng_minus5_g_s_tmsi_part2_r15();

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<48> > c;

    void destroy_();
  };

  // member variables
  bool                                log_meas_available_bt_r15_present      = false;
  bool                                log_meas_available_wlan_r15_present    = false;
  bool                                idle_meas_available_r15_present        = false;
  bool                                flight_path_info_available_r15_present = false;
  bool                                connect_to5_gc_r15_present             = false;
  bool                                registered_amf_r15_present             = false;
  bool                                s_nssai_list_r15_present               = false;
  bool                                ng_minus5_g_s_tmsi_bits_r15_present    = false;
  bool                                non_crit_ext_present                   = false;
  registered_amf_r15_s                registered_amf_r15;
  s_nssai_list_r15_l_                 s_nssai_list_r15;
  ng_minus5_g_s_tmsi_bits_r15_c_      ng_minus5_g_s_tmsi_bits_r15;
  rrc_conn_setup_complete_v1540_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AffectedCarrierFreqComb-r15 ::= SEQUENCE (SIZE (1..32)) OF INTEGER (1..64)
using affected_carrier_freq_comb_r15_l = bounded_array<uint8_t, 32>;

// AffectedCarrierFreqCombNR-r15 ::= SEQUENCE (SIZE (1..16)) OF INTEGER (0..3279165)
using affected_carrier_freq_comb_nr_r15_l = bounded_array<uint32_t, 16>;

// MeasResultIdle-r15 ::= SEQUENCE
struct meas_result_idle_r15_s {
  struct meas_result_serving_cell_r15_s_ {
    uint8_t rsrp_result_r15 = 0;
    int8_t  rsrq_result_r15 = -30;
  };
  struct meas_result_neigh_cells_r15_c_ {
    struct types_opts {
      enum options { meas_result_idle_list_eutra_r15, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts, true> types;

    // choice methods
    types       type() const { return types::meas_result_idle_list_eutra_r15; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    meas_result_idle_list_eutra_r15_l&       meas_result_idle_list_eutra_r15() { return c; }
    const meas_result_idle_list_eutra_r15_l& meas_result_idle_list_eutra_r15() const { return c; }

  private:
    meas_result_idle_list_eutra_r15_l c;
  };

  // member variables
  bool                            ext                                 = false;
  bool                            meas_result_neigh_cells_r15_present = false;
  meas_result_serving_cell_r15_s_ meas_result_serving_cell_r15;
  meas_result_neigh_cells_r15_c_  meas_result_neigh_cells_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultListExtIdle-r16 ::= SEQUENCE (SIZE (1..5)) OF MeasResultIdleListEUTRA-r15
using meas_result_list_ext_idle_r16_l = dyn_array<meas_result_idle_list_eutra_r15_l>;

// MeasResultListIdleNR-r16 ::= SEQUENCE (SIZE (1..8)) OF MeasResultIdleNR-r16
using meas_result_list_idle_nr_r16_l = dyn_array<meas_result_idle_nr_r16_s>;

// OverheatingAssistance-v1710 ::= SEQUENCE
struct overheat_assist_v1710_s {
  dyn_octstring overheat_assist_for_scg_fr2_minus2_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PerCC-GapIndication-r14 ::= SEQUENCE
struct per_cc_gap_ind_r14_s {
  struct gap_ind_r14_opts {
    enum options { gap, ncsg, nogap_no_ncsg, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<gap_ind_r14_opts> gap_ind_r14_e_;

  // member variables
  uint8_t        serv_cell_id_r14 = 0;
  gap_ind_r14_e_ gap_ind_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RACH-Report-v1610 ::= SEQUENCE
struct rach_report_v1610_s {
  uint8_t init_cel_r16     = 0;
  bool    edt_fallback_r16 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfigurationComplete-v1530-IEs ::= SEQUENCE
struct rrc_conn_recfg_complete_v1530_ies_s {
  bool                                log_meas_available_bt_r15_present      = false;
  bool                                log_meas_available_wlan_r15_present    = false;
  bool                                flight_path_info_available_r15_present = false;
  bool                                non_crit_ext_present                   = false;
  rrc_conn_recfg_complete_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentComplete-v1710-IEs ::= SEQUENCE
struct rrc_conn_reest_complete_v1710_ies_s {
  bool                    gnss_validity_dur_r17_present = false;
  bool                    non_crit_ext_present          = false;
  gnss_validity_dur_r17_e gnss_validity_dur_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-v1430-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_v1430_ies_s {
  bool                                dcn_id_r14_present   = false;
  bool                                non_crit_ext_present = false;
  uint32_t                            dcn_id_r14           = 0;
  rrc_conn_setup_complete_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationResponse-v1710-IEs ::= SEQUENCE
struct ue_info_resp_v1710_ies_s {
  bool          coarse_location_info_r17_present = false;
  bool          non_crit_ext_present             = false;
  dyn_octstring coarse_location_info_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VictimSystemType-r11 ::= SEQUENCE
struct victim_sys_type_r11_s {
  bool gps_r11_present       = false;
  bool glonass_r11_present   = false;
  bool bds_r11_present       = false;
  bool galileo_r11_present   = false;
  bool wlan_r11_present      = false;
  bool bluetooth_r11_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VisitedCellInfo-r12 ::= SEQUENCE
struct visited_cell_info_r12_s {
  struct visited_cell_id_r12_c_ {
    struct pci_arfcn_r12_s_ {
      uint16_t pci_r12          = 0;
      uint32_t carrier_freq_r12 = 0;
    };
    struct types_opts {
      enum options { cell_global_id_r12, pci_arfcn_r12, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    visited_cell_id_r12_c_() = default;
    visited_cell_id_r12_c_(const visited_cell_id_r12_c_& other);
    visited_cell_id_r12_c_& operator=(const visited_cell_id_r12_c_& other);
    ~visited_cell_id_r12_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cell_global_id_eutra_s& cell_global_id_r12()
    {
      assert_choice_type(types::cell_global_id_r12, type_, "visitedCellId-r12");
      return c.get<cell_global_id_eutra_s>();
    }
    pci_arfcn_r12_s_& pci_arfcn_r12()
    {
      assert_choice_type(types::pci_arfcn_r12, type_, "visitedCellId-r12");
      return c.get<pci_arfcn_r12_s_>();
    }
    const cell_global_id_eutra_s& cell_global_id_r12() const
    {
      assert_choice_type(types::cell_global_id_r12, type_, "visitedCellId-r12");
      return c.get<cell_global_id_eutra_s>();
    }
    const pci_arfcn_r12_s_& pci_arfcn_r12() const
    {
      assert_choice_type(types::pci_arfcn_r12, type_, "visitedCellId-r12");
      return c.get<pci_arfcn_r12_s_>();
    }
    cell_global_id_eutra_s& set_cell_global_id_r12();
    pci_arfcn_r12_s_&       set_pci_arfcn_r12();

  private:
    types                                                     type_;
    choice_buffer_t<cell_global_id_eutra_s, pci_arfcn_r12_s_> c;

    void destroy_();
  };

  // member variables
  bool                   ext                         = false;
  bool                   visited_cell_id_r12_present = false;
  visited_cell_id_r12_c_ visited_cell_id_r12;
  uint16_t               time_spent_r12 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WayPointLocation-r15 ::= SEQUENCE
struct way_point_location_r15_s {
  bool                time_stamp_r15_present = false;
  location_info_r10_s way_point_location_r15;
  fixed_bitstring<48> time_stamp_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AffectedCarrierFreqCombInfoMRDC-r15 ::= SEQUENCE
struct affected_carrier_freq_comb_info_mrdc_r15_s {
  struct interference_direction_mrdc_r15_opts {
    enum options { eutra_nr, nr, other, eutra_nr_other, nr_other, spare3, spare2, spare1, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<interference_direction_mrdc_r15_opts> interference_direction_mrdc_r15_e_;
  struct affected_carrier_freq_comb_mrdc_r15_s_ {
    bool                                affected_carrier_freq_comb_eutra_r15_present = false;
    affected_carrier_freq_comb_r15_l    affected_carrier_freq_comb_eutra_r15;
    affected_carrier_freq_comb_nr_r15_l affected_carrier_freq_comb_nr_r15;
  };

  // member variables
  bool                                   affected_carrier_freq_comb_mrdc_r15_present = false;
  victim_sys_type_r11_s                  victim_sys_type_r15;
  interference_direction_mrdc_r15_e_     interference_direction_mrdc_r15;
  affected_carrier_freq_comb_mrdc_r15_s_ affected_carrier_freq_comb_mrdc_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FlightPathInfoReport-r15 ::= SEQUENCE
struct flight_path_info_report_r15_s {
  using flight_path_r15_l_ = dyn_array<way_point_location_r15_s>;

  // member variables
  bool               flight_path_r15_present = false;
  bool               dummy_present           = false;
  flight_path_r15_l_ flight_path_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultListIdle-r15 ::= SEQUENCE (SIZE (1..3)) OF MeasResultIdle-r15
using meas_result_list_idle_r15_l = dyn_array<meas_result_idle_r15_s>;

// PerCC-GapIndicationList-r14 ::= SEQUENCE (SIZE (1..32)) OF PerCC-GapIndication-r14
using per_cc_gap_ind_list_r14_l = dyn_array<per_cc_gap_ind_r14_s>;

// RRCConnectionReconfigurationComplete-v1510-IEs ::= SEQUENCE
struct rrc_conn_recfg_complete_v1510_ies_s {
  bool                                scg_cfg_resp_nr_r15_present = false;
  bool                                non_crit_ext_present        = false;
  dyn_octstring                       scg_cfg_resp_nr_r15;
  rrc_conn_recfg_complete_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentComplete-v1530-IEs ::= SEQUENCE
struct rrc_conn_reest_complete_v1530_ies_s {
  bool                                log_meas_available_bt_r15_present      = false;
  bool                                log_meas_available_wlan_r15_present    = false;
  bool                                flight_path_info_available_r15_present = false;
  bool                                non_crit_ext_present                   = false;
  rrc_conn_reest_complete_v1710_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-v1330-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_v1330_ies_s {
  bool                                ue_ce_need_ul_gaps_r13_present = false;
  bool                                non_crit_ext_present           = false;
  rrc_conn_setup_complete_v1430_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEAssistanceInformation-v1710-IEs ::= SEQUENCE
struct ueassist_info_v1710_ies_s {
  bool                    overheat_assist_v1710_present = false;
  bool                    non_crit_ext_present          = false;
  overheat_assist_v1710_s overheat_assist_v1710;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationResponse-v1610-IEs ::= SEQUENCE
struct ue_info_resp_v1610_ies_s {
  bool                            rach_report_v1610_present             = false;
  bool                            meas_result_list_ext_idle_r16_present = false;
  bool                            meas_result_list_idle_nr_r16_present  = false;
  bool                            non_crit_ext_present                  = false;
  rach_report_v1610_s             rach_report_v1610;
  meas_result_list_ext_idle_r16_l meas_result_list_ext_idle_r16;
  meas_result_list_idle_nr_r16_l  meas_result_list_idle_nr_r16;
  ue_info_resp_v1710_ies_s        non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VictimSystemType-v1610 ::= SEQUENCE
struct victim_sys_type_v1610_s {
  bool navic_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VisitedCellInfoList-r12 ::= SEQUENCE (SIZE (1..16)) OF VisitedCellInfo-r12
using visited_cell_info_list_r12_l = dyn_array<visited_cell_info_r12_s>;

// InDeviceCoexIndication-v1610-IEs ::= SEQUENCE
struct in_dev_coex_ind_v1610_ies_s {
  bool                    victim_sys_type_v1610_present = false;
  bool                    non_crit_ext_present          = false;
  victim_sys_type_v1610_s victim_sys_type_v1610;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MRDC-AssistanceInfo-r15 ::= SEQUENCE
struct mrdc_assist_info_r15_s {
  using affected_carrier_freq_comb_info_list_mrdc_r15_l_   = dyn_array<affected_carrier_freq_comb_info_mrdc_r15_s>;
  using affected_carrier_freq_comb_info_list_mrdc_v1610_l_ = dyn_array<victim_sys_type_v1610_s>;

  // member variables
  bool                                             ext = false;
  affected_carrier_freq_comb_info_list_mrdc_r15_l_ affected_carrier_freq_comb_info_list_mrdc_r15;
  // ...
  // group 0
  copy_ptr<affected_carrier_freq_comb_info_list_mrdc_v1610_l_> affected_carrier_freq_comb_info_list_mrdc_v1610;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityHistoryReport-r12 ::= VisitedCellInfoList-r12
using mob_history_report_r12_l = visited_cell_info_list_r12_l;

// OverheatingAssistance-v1610 ::= SEQUENCE
struct overheat_assist_v1610_s {
  dyn_octstring overheat_assist_for_scg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfigurationComplete-v1430-IEs ::= SEQUENCE
struct rrc_conn_recfg_complete_v1430_ies_s {
  bool                                per_cc_gap_ind_list_r14_present        = false;
  bool                                num_freq_effective_r14_present         = false;
  bool                                num_freq_effective_reduced_r14_present = false;
  bool                                non_crit_ext_present                   = false;
  per_cc_gap_ind_list_r14_l           per_cc_gap_ind_list_r14;
  uint8_t                             num_freq_effective_r14         = 1;
  uint8_t                             num_freq_effective_reduced_r14 = 1;
  rrc_conn_recfg_complete_v1510_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentComplete-v1250-IEs ::= SEQUENCE
struct rrc_conn_reest_complete_v1250_ies_s {
  bool                                log_meas_available_mbsfn_r12_present = false;
  bool                                non_crit_ext_present                 = false;
  rrc_conn_reest_complete_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-v1320-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_v1320_ies_s {
  bool                                ce_mode_b_r13_present                  = false;
  bool                                s_tmsi_r13_present                     = false;
  bool                                attach_without_pdn_connect_r13_present = false;
  bool                                up_cio_t_eps_optim_r13_present         = false;
  bool                                cp_cio_t_eps_optim_r13_present         = false;
  bool                                non_crit_ext_present                   = false;
  s_tmsi_s                            s_tmsi_r13;
  rrc_conn_setup_complete_v1330_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TrafficPatternInfo-v1530 ::= SEQUENCE
struct traffic_pattern_info_v1530_s {
  bool                traffic_dest_r15_present        = false;
  bool                reliability_info_sl_r15_present = false;
  fixed_bitstring<24> traffic_dest_r15;
  uint8_t             reliability_info_sl_r15 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEAssistanceInformation-v1700-IEs ::= SEQUENCE
struct ueassist_info_v1700_ies_s {
  struct scg_deactivation_pref_r17_opts {
    enum options { scg_deactivation_preferred, no_pref, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<scg_deactivation_pref_r17_opts> scg_deactivation_pref_r17_e_;

  // member variables
  bool                         ul_data_r17_present               = false;
  bool                         scg_deactivation_pref_r17_present = false;
  bool                         non_crit_ext_present              = false;
  scg_deactivation_pref_r17_e_ scg_deactivation_pref_r17;
  ueassist_info_v1710_ies_s    non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationResponse-v1530-IEs ::= SEQUENCE
struct ue_info_resp_v1530_ies_s {
  bool                          meas_result_list_idle_r15_present   = false;
  bool                          flight_path_info_report_r15_present = false;
  bool                          non_crit_ext_present                = false;
  meas_result_list_idle_r15_l   meas_result_list_idle_r15;
  flight_path_info_report_r15_s flight_path_info_report_r15;
  ue_info_resp_v1610_ies_s      non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AffectedCarrierFreq-v1310 ::= SEQUENCE
struct affected_carrier_freq_v1310_s {
  bool    carrier_freq_v1310_present = false;
  uint8_t carrier_freq_v1310         = 33;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AffectedCarrierFreqComb-r13 ::= SEQUENCE (SIZE (2..32)) OF INTEGER (1..64)
using affected_carrier_freq_comb_r13_l = bounded_array<uint8_t, 32>;

// ConnEstFailReport-r11 ::= SEQUENCE
struct conn_est_fail_report_r11_s {
  struct meas_result_failed_cell_r11_s_ {
    bool    rsrq_result_r11_present = false;
    uint8_t rsrp_result_r11         = 0;
    uint8_t rsrq_result_r11         = 0;
  };
  struct meas_result_neigh_cells_r11_s_ {
    bool                            meas_result_list_eutra_r11_present = false;
    bool                            meas_result_list_utra_r11_present  = false;
    bool                            meas_result_list_geran_r11_present = false;
    bool                            meas_results_cdma2000_r11_present  = false;
    meas_result_list2_eutra_r9_l    meas_result_list_eutra_r11;
    meas_result_list2_utra_r9_l     meas_result_list_utra_r11;
    meas_result_list_geran_l        meas_result_list_geran_r11;
    meas_result_list2_cdma2000_r9_l meas_results_cdma2000_r11;
  };
  struct meas_result_list_nr_v1640_s_ {
    uint32_t carrier_freq_nr_r16 = 0;
  };

  // member variables
  bool                           ext                                  = false;
  bool                           location_info_r11_present            = false;
  bool                           meas_result_neigh_cells_r11_present  = false;
  bool                           meas_result_list_eutra_v1130_present = false;
  cell_global_id_eutra_s         failed_cell_id_r11;
  location_info_r10_s            location_info_r11;
  meas_result_failed_cell_r11_s_ meas_result_failed_cell_r11;
  meas_result_neigh_cells_r11_s_ meas_result_neigh_cells_r11;
  uint8_t                        nof_preambs_sent_r11    = 1;
  bool                           contention_detected_r11 = false;
  bool                           max_tx_pwr_reached_r11  = false;
  uint32_t                       time_since_fail_r11     = 0;
  meas_result_list2_eutra_v9e0_l meas_result_list_eutra_v1130;
  // ...
  // group 0
  bool                                      meas_result_failed_cell_v1250_present = false;
  int8_t                                    meas_result_failed_cell_v1250         = -30;
  copy_ptr<rsrq_type_r12_s>                 failed_cell_rsrq_type_r12;
  copy_ptr<meas_result_list2_eutra_v1250_l> meas_result_list_eutra_v1250;
  // group 1
  bool   meas_result_failed_cell_v1360_present = false;
  int8_t meas_result_failed_cell_v1360         = -17;
  // group 2
  copy_ptr<log_meas_result_list_bt_r15_l>   log_meas_result_list_bt_r15;
  copy_ptr<log_meas_result_list_wlan_r15_l> log_meas_result_list_wlan_r15;
  // group 3
  copy_ptr<meas_result_cell_list_nr_r15_l> meas_result_list_nr_r16;
  // group 4
  copy_ptr<meas_result_list_nr_v1640_s_>   meas_result_list_nr_v1640;
  copy_ptr<meas_result_freq_list_nr_r16_l> meas_result_list_ext_nr_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-CountInfo ::= SEQUENCE
struct drb_count_info_s {
  uint8_t  drb_id   = 1;
  uint64_t count_ul = 0;
  uint64_t count_dl = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InDeviceCoexIndication-v1530-IEs ::= SEQUENCE
struct in_dev_coex_ind_v1530_ies_s {
  bool                        mrdc_assist_info_r15_present = false;
  bool                        non_crit_ext_present         = false;
  mrdc_assist_info_r15_s      mrdc_assist_info_r15;
  in_dev_coex_ind_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMS-ROM-Info-r16 ::= SEQUENCE
struct mbms_rom_info_r16_s {
  struct mbms_rom_subcarrier_spacing_r16_opts {
    enum options { khz2dot5, khz0dot37, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<mbms_rom_subcarrier_spacing_r16_opts> mbms_rom_subcarrier_spacing_r16_e_;
  struct mbms_bw_r16_opts {
    enum options { n6, n15, n25, n50, n75, n100, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<mbms_bw_r16_opts> mbms_bw_r16_e_;

  // member variables
  uint32_t                           mbms_rom_freq_r16 = 0;
  mbms_rom_subcarrier_spacing_r16_e_ mbms_rom_subcarrier_spacing_r16;
  mbms_bw_r16_e_                     mbms_bw_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfigurationComplete-v1250-IEs ::= SEQUENCE
struct rrc_conn_recfg_complete_v1250_ies_s {
  bool                                log_meas_available_mbsfn_r12_present = false;
  bool                                non_crit_ext_present                 = false;
  rrc_conn_recfg_complete_v1430_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentComplete-v1130-IEs ::= SEQUENCE
struct rrc_conn_reest_complete_v1130_ies_s {
  bool                                conn_est_fail_info_available_r11_present = false;
  bool                                non_crit_ext_present                     = false;
  rrc_conn_reest_complete_v1250_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-v1250-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_v1250_ies_s {
  struct mob_state_r12_opts {
    enum options { normal, medium, high, spare, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<mob_state_r12_opts> mob_state_r12_e_;

  // member variables
  bool                                mob_state_r12_present                = false;
  bool                                mob_history_avail_r12_present        = false;
  bool                                log_meas_available_mbsfn_r12_present = false;
  bool                                non_crit_ext_present                 = false;
  mob_state_r12_e_                    mob_state_r12;
  rrc_conn_setup_complete_v1320_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-V2X-CommTxResourceReq-r14 ::= SEQUENCE
struct sl_v2x_comm_tx_res_req_r14_s {
  bool                    carrier_freq_comm_tx_r14_present = false;
  bool                    v2x_type_tx_sync_r14_present     = false;
  bool                    v2x_dest_info_list_r14_present   = false;
  uint8_t                 carrier_freq_comm_tx_r14         = 0;
  sl_type_tx_sync_r14_e   v2x_type_tx_sync_r14;
  sl_dest_info_list_r12_l v2x_dest_info_list_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TrafficPatternInfoList-v1530 ::= SEQUENCE (SIZE (1..8)) OF TrafficPatternInfo-v1530
using traffic_pattern_info_list_v1530_l = dyn_array<traffic_pattern_info_v1530_s>;

// UEAssistanceInformation-v1610-IEs ::= SEQUENCE
struct ueassist_info_v1610_ies_s {
  bool                      overheat_assist_v1610_present = false;
  bool                      non_crit_ext_present          = false;
  overheat_assist_v1610_s   overheat_assist_v1610;
  ueassist_info_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationResponse-v1250-IEs ::= SEQUENCE
struct ue_info_resp_v1250_ies_s {
  bool                     mob_history_report_r12_present = false;
  bool                     non_crit_ext_present           = false;
  mob_history_report_r12_l mob_history_report_r12;
  ue_info_resp_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AffectedCarrierFreqComb-r11 ::= SEQUENCE (SIZE (2..5)) OF INTEGER (1..32)
using affected_carrier_freq_comb_r11_l = bounded_array<uint8_t, 5>;

// AffectedCarrierFreqCombList-r13 ::= SEQUENCE (SIZE (1..128)) OF AffectedCarrierFreqComb-r13
using affected_carrier_freq_comb_list_r13_l = dyn_array<affected_carrier_freq_comb_r13_l>;

// AffectedCarrierFreqList-v1310 ::= SEQUENCE (SIZE (1..32)) OF AffectedCarrierFreq-v1310
using affected_carrier_freq_list_v1310_l = dyn_array<affected_carrier_freq_v1310_s>;

// DRB-CountInfoListExt-r15 ::= SEQUENCE (SIZE (1..4)) OF DRB-CountInfo
using drb_count_info_list_ext_r15_l = dyn_array<drb_count_info_s>;

// IDC-SubframePattern-r11 ::= CHOICE
struct idc_sf_pattern_r11_c {
  struct sf_pattern_tdd_r11_c_ {
    struct types_opts {
      enum options { sf_cfg0_r11, sf_cfg1_minus5_r11, sf_cfg6_r11, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    sf_pattern_tdd_r11_c_() = default;
    sf_pattern_tdd_r11_c_(const sf_pattern_tdd_r11_c_& other);
    sf_pattern_tdd_r11_c_& operator=(const sf_pattern_tdd_r11_c_& other);
    ~sf_pattern_tdd_r11_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fixed_bitstring<70>& sf_cfg0_r11()
    {
      assert_choice_type(types::sf_cfg0_r11, type_, "subframePatternTDD-r11");
      return c.get<fixed_bitstring<70> >();
    }
    fixed_bitstring<10>& sf_cfg1_minus5_r11()
    {
      assert_choice_type(types::sf_cfg1_minus5_r11, type_, "subframePatternTDD-r11");
      return c.get<fixed_bitstring<10> >();
    }
    fixed_bitstring<60>& sf_cfg6_r11()
    {
      assert_choice_type(types::sf_cfg6_r11, type_, "subframePatternTDD-r11");
      return c.get<fixed_bitstring<60> >();
    }
    const fixed_bitstring<70>& sf_cfg0_r11() const
    {
      assert_choice_type(types::sf_cfg0_r11, type_, "subframePatternTDD-r11");
      return c.get<fixed_bitstring<70> >();
    }
    const fixed_bitstring<10>& sf_cfg1_minus5_r11() const
    {
      assert_choice_type(types::sf_cfg1_minus5_r11, type_, "subframePatternTDD-r11");
      return c.get<fixed_bitstring<10> >();
    }
    const fixed_bitstring<60>& sf_cfg6_r11() const
    {
      assert_choice_type(types::sf_cfg6_r11, type_, "subframePatternTDD-r11");
      return c.get<fixed_bitstring<60> >();
    }
    fixed_bitstring<70>& set_sf_cfg0_r11();
    fixed_bitstring<10>& set_sf_cfg1_minus5_r11();
    fixed_bitstring<60>& set_sf_cfg6_r11();

  private:
    types                                 type_;
    choice_buffer_t<fixed_bitstring<70> > c;

    void destroy_();
  };
  struct types_opts {
    enum options { sf_pattern_fdd_r11, sf_pattern_tdd_r11, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  idc_sf_pattern_r11_c() = default;
  idc_sf_pattern_r11_c(const idc_sf_pattern_r11_c& other);
  idc_sf_pattern_r11_c& operator=(const idc_sf_pattern_r11_c& other);
  ~idc_sf_pattern_r11_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<4>& sf_pattern_fdd_r11()
  {
    assert_choice_type(types::sf_pattern_fdd_r11, type_, "IDC-SubframePattern-r11");
    return c.get<fixed_bitstring<4> >();
  }
  sf_pattern_tdd_r11_c_& sf_pattern_tdd_r11()
  {
    assert_choice_type(types::sf_pattern_tdd_r11, type_, "IDC-SubframePattern-r11");
    return c.get<sf_pattern_tdd_r11_c_>();
  }
  const fixed_bitstring<4>& sf_pattern_fdd_r11() const
  {
    assert_choice_type(types::sf_pattern_fdd_r11, type_, "IDC-SubframePattern-r11");
    return c.get<fixed_bitstring<4> >();
  }
  const sf_pattern_tdd_r11_c_& sf_pattern_tdd_r11() const
  {
    assert_choice_type(types::sf_pattern_tdd_r11, type_, "IDC-SubframePattern-r11");
    return c.get<sf_pattern_tdd_r11_c_>();
  }
  fixed_bitstring<4>&    set_sf_pattern_fdd_r11();
  sf_pattern_tdd_r11_c_& set_sf_pattern_tdd_r11();

private:
  types                                                      type_;
  choice_buffer_t<fixed_bitstring<4>, sf_pattern_tdd_r11_c_> c;

  void destroy_();
};

// InDeviceCoexIndication-v1360-IEs ::= SEQUENCE
struct in_dev_coex_ind_v1360_ies_s {
  bool                        hardware_sharing_problem_r13_present = false;
  bool                        non_crit_ext_present                 = false;
  in_dev_coex_ind_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMS-ROM-Info-r15 ::= SEQUENCE
struct mbms_rom_info_r15_s {
  struct mbms_rom_subcarrier_spacing_r15_opts {
    enum options { khz15, khz7dot5, khz1dot25, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<mbms_rom_subcarrier_spacing_r15_opts> mbms_rom_subcarrier_spacing_r15_e_;
  struct mbms_bw_r15_opts {
    enum options { n6, n15, n25, n50, n75, n100, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<mbms_bw_r15_opts> mbms_bw_r15_e_;

  // member variables
  uint32_t                           mbms_rom_freq_r15 = 0;
  mbms_rom_subcarrier_spacing_r15_e_ mbms_rom_subcarrier_spacing_r15;
  mbms_bw_r15_e_                     mbms_bw_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMS-ServiceInfo-r13 ::= SEQUENCE
struct mbms_service_info_r13_s {
  tmgi_r9_s tmgi_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMSInterestIndication-v1610-IEs ::= SEQUENCE
struct mbms_interest_ind_v1610_ies_s {
  using mbms_rom_info_list_r16_l_ = dyn_array<mbms_rom_info_r16_s>;

  // member variables
  bool                      mbms_rom_info_list_r16_present = false;
  bool                      non_crit_ext_present           = false;
  mbms_rom_info_list_r16_l_ mbms_rom_info_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResult3EUTRA-r15 ::= SEQUENCE
struct meas_result3_eutra_r15_s {
  bool                     ext                                     = false;
  bool                     meas_result_serving_cell_r15_present    = false;
  bool                     meas_result_neigh_cell_list_r15_present = false;
  uint32_t                 carrier_freq_r15                        = 0;
  meas_result_eutra_s      meas_result_serving_cell_r15;
  meas_result_list_eutra_l meas_result_neigh_cell_list_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OverheatingAssistance-r14 ::= SEQUENCE
struct overheat_assist_r14_s {
  struct reduced_ue_category_s_ {
    uint8_t reduced_ue_category_dl = 0;
    uint8_t reduced_ue_category_ul = 0;
  };
  struct reduced_max_ccs_s_ {
    uint8_t reduced_ccs_dl = 0;
    uint8_t reduced_ccs_ul = 0;
  };

  // member variables
  bool                   reduced_ue_category_present = false;
  bool                   reduced_max_ccs_present     = false;
  reduced_ue_category_s_ reduced_ue_category;
  reduced_max_ccs_s_     reduced_max_ccs;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfigurationComplete-v1130-IEs ::= SEQUENCE
struct rrc_conn_recfg_complete_v1130_ies_s {
  bool                                conn_est_fail_info_available_r11_present = false;
  bool                                non_crit_ext_present                     = false;
  rrc_conn_recfg_complete_v1250_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentComplete-v1020-IEs ::= SEQUENCE
struct rrc_conn_reest_complete_v1020_ies_s {
  bool                                log_meas_available_r10_present = false;
  bool                                non_crit_ext_present           = false;
  rrc_conn_reest_complete_v1130_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResumeComplete-v1710-IEs ::= SEQUENCE
struct rrc_conn_resume_complete_v1710_ies_s {
  bool                    gnss_validity_dur_r17_present = false;
  bool                    non_crit_ext_present          = false;
  gnss_validity_dur_r17_e gnss_validity_dur_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-v1130-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_v1130_ies_s {
  bool                                conn_est_fail_info_available_r11_present = false;
  bool                                non_crit_ext_present                     = false;
  rrc_conn_setup_complete_v1250_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-DiscSysInfoReport-r13 ::= SEQUENCE
struct sl_disc_sys_info_report_r13_s {
  struct disc_cell_sel_info_r13_s_ {
    bool    q_rx_lev_min_offset_r13_present = false;
    int8_t  q_rx_lev_min_r13                = -70;
    uint8_t q_rx_lev_min_offset_r13         = 1;
  };
  struct cell_resel_info_r13_s_ {
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
    q_hyst_r13_e_ q_hyst_r13;
    int8_t        q_rx_lev_min_r13  = -70;
    uint8_t       t_resel_eutra_r13 = 0;
  };
  struct freq_info_r13_s_ {
    struct ul_bw_r13_opts {
      enum options { n6, n15, n25, n50, n75, n100, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<ul_bw_r13_opts> ul_bw_r13_e_;

    // member variables
    bool         ul_carrier_freq_r13_present   = false;
    bool         ul_bw_r13_present             = false;
    bool         add_spec_emission_r13_present = false;
    uint32_t     ul_carrier_freq_r13           = 0;
    ul_bw_r13_e_ ul_bw_r13;
    uint8_t      add_spec_emission_r13 = 1;
  };
  struct freq_info_v1370_s_ {
    uint16_t add_spec_emission_v1370 = 33;
  };

  // member variables
  bool                           ext                               = false;
  bool                           plmn_id_list_r13_present          = false;
  bool                           cell_id_minus13_present           = false;
  bool                           carrier_freq_info_minus13_present = false;
  bool                           disc_rx_res_r13_present           = false;
  bool                           disc_tx_pool_common_r13_present   = false;
  bool                           disc_tx_pwr_info_r13_present      = false;
  bool                           disc_sync_cfg_r13_present         = false;
  bool                           disc_cell_sel_info_r13_present    = false;
  bool                           cell_resel_info_r13_present       = false;
  bool                           tdd_cfg_r13_present               = false;
  bool                           freq_info_r13_present             = false;
  bool                           p_max_r13_present                 = false;
  bool                           ref_sig_pwr_r13_present           = false;
  plmn_id_list_l                 plmn_id_list_r13;
  fixed_bitstring<28>            cell_id_minus13;
  uint32_t                       carrier_freq_info_minus13 = 0;
  sl_disc_rx_pool_list_r12_l     disc_rx_res_r13;
  sl_disc_tx_pool_list_r12_l     disc_tx_pool_common_r13;
  sl_disc_tx_pwr_info_list_r12_l disc_tx_pwr_info_r13;
  sl_sync_cfg_nfreq_r13_s        disc_sync_cfg_r13;
  disc_cell_sel_info_r13_s_      disc_cell_sel_info_r13;
  cell_resel_info_r13_s_         cell_resel_info_r13;
  tdd_cfg_s                      tdd_cfg_r13;
  freq_info_r13_s_               freq_info_r13;
  int8_t                         p_max_r13       = -30;
  int8_t                         ref_sig_pwr_r13 = -60;
  // ...
  // group 0
  copy_ptr<freq_info_v1370_s_> freq_info_v1370;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-DiscTxResourceReq-r13 ::= SEQUENCE
struct sl_disc_tx_res_req_r13_s {
  bool    carrier_freq_disc_tx_r13_present = false;
  uint8_t carrier_freq_disc_tx_r13         = 1;
  uint8_t disc_tx_res_req_r13              = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-GapFreqInfo-r13 ::= SEQUENCE
struct sl_gap_freq_info_r13_s {
  bool                      carrier_freq_r13_present = false;
  uint32_t                  carrier_freq_r13         = 0;
  sl_gap_pattern_list_r13_l gap_pattern_list_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-V2X-CommFreqList-r14 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (0..7)
using sl_v2x_comm_freq_list_r14_l = bounded_array<uint8_t, 8>;

// SL-V2X-CommTxFreqList-r14 ::= SEQUENCE (SIZE (1..8)) OF SL-V2X-CommTxResourceReq-r14
using sl_v2x_comm_tx_freq_list_r14_l = dyn_array<sl_v2x_comm_tx_res_req_r14_s>;

// SidelinkUEInformation-v1530-IEs ::= SEQUENCE
struct sidelink_ue_info_v1530_ies_s {
  bool                      reliability_info_list_sl_r15_present = false;
  bool                      non_crit_ext_present                 = false;
  sl_reliability_list_r15_l reliability_info_list_sl_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TrafficPatternInfo-r14 ::= SEQUENCE
struct traffic_pattern_info_r14_s {
  struct traffic_periodicity_r14_opts {
    enum options { sf20, sf50, sf100, sf200, sf300, sf400, sf500, sf600, sf700, sf800, sf900, sf1000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<traffic_periodicity_r14_opts> traffic_periodicity_r14_e_;

  // member variables
  bool                       prio_info_sl_r14_present = false;
  bool                       lc_ch_id_ul_r14_present  = false;
  traffic_periodicity_r14_e_ traffic_periodicity_r14;
  uint16_t                   timing_offset_r14 = 0;
  uint8_t                    prio_info_sl_r14  = 1;
  uint8_t                    lc_ch_id_ul_r14   = 3;
  fixed_bitstring<6>         msg_size_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEAssistanceInformation-v1530-IEs ::= SEQUENCE
struct ueassist_info_v1530_ies_s {
  struct sps_assist_info_v1530_s_ {
    traffic_pattern_info_list_v1530_l traffic_pattern_info_list_sl_v1530;
  };

  // member variables
  bool                      sps_assist_info_v1530_present = false;
  bool                      non_crit_ext_present          = false;
  sps_assist_info_v1530_s_  sps_assist_info_v1530;
  ueassist_info_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationResponse-v1130-IEs ::= SEQUENCE
struct ue_info_resp_v1130_ies_s {
  bool                       conn_est_fail_report_r11_present = false;
  bool                       non_crit_ext_present             = false;
  conn_est_fail_report_r11_s conn_est_fail_report_r11;
  ue_info_resp_v1250_ies_s   non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AffectedCarrierFreq-r11 ::= SEQUENCE
struct affected_carrier_freq_r11_s {
  struct interference_direction_r11_opts {
    enum options { eutra, other, both, spare, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<interference_direction_r11_opts> interference_direction_r11_e_;

  // member variables
  uint8_t                       carrier_freq_r11 = 1;
  interference_direction_r11_e_ interference_direction_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AffectedCarrierFreqCombList-r11 ::= SEQUENCE (SIZE (1..128)) OF AffectedCarrierFreqComb-r11
using affected_carrier_freq_comb_list_r11_l = dyn_array<affected_carrier_freq_comb_r11_l>;

// BW-Preference-r14 ::= SEQUENCE
struct bw_pref_r14_s {
  struct dl_pref_r14_opts {
    enum options { mhz1dot4, mhz5, mhz20, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<dl_pref_r14_opts> dl_pref_r14_e_;
  struct ul_pref_r14_opts {
    enum options { mhz1dot4, mhz5, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<ul_pref_r14_opts> ul_pref_r14_e_;

  // member variables
  bool           dl_pref_r14_present = false;
  bool           ul_pref_r14_present = false;
  dl_pref_r14_e_ dl_pref_r14;
  ul_pref_r14_e_ ul_pref_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellGlobalIdNR-r16 ::= SEQUENCE
struct cell_global_id_nr_r16_s {
  bool                tac_r16_present = false;
  plmn_id_s           plmn_id_r16;
  fixed_bitstring<36> cell_id_r16;
  fixed_bitstring<24> tac_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CounterCheckResponse-v1530-IEs ::= SEQUENCE
struct counter_check_resp_v1530_ies_s {
  bool                          drb_count_info_list_ext_r15_present = false;
  bool                          non_crit_ext_present                = false;
  drb_count_info_list_ext_r15_l drb_count_info_list_ext_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CountingResponseInfo-r10 ::= SEQUENCE
struct count_resp_info_r10_s {
  bool    ext                    = false;
  uint8_t count_resp_service_r10 = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DelayBudgetReport-r14 ::= CHOICE
struct delay_budget_report_r14_c {
  struct type1_opts {
    enum options {
      ms_minus1280,
      ms_minus640,
      ms_minus320,
      ms_minus160,
      ms_minus80,
      ms_minus60,
      ms_minus40,
      ms_minus20,
      ms0,
      ms20,
      ms40,
      ms60,
      ms80,
      ms160,
      ms320,
      ms640,
      ms1280,
      nulltype
    } value;
    typedef int16_t number_type;

    const char* to_string() const;
    int16_t     to_number() const;
  };
  typedef enumerated<type1_opts> type1_e_;
  struct type2_opts {
    enum options {
      ms_minus192,
      ms_minus168,
      ms_minus144,
      ms_minus120,
      ms_minus96,
      ms_minus72,
      ms_minus48,
      ms_minus24,
      ms0,
      ms24,
      ms48,
      ms72,
      ms96,
      ms120,
      ms144,
      ms168,
      ms192,
      nulltype
    } value;
    typedef int16_t number_type;

    const char* to_string() const;
    int16_t     to_number() const;
  };
  typedef enumerated<type2_opts> type2_e_;
  struct types_opts {
    enum options { type1, type2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  delay_budget_report_r14_c() = default;
  delay_budget_report_r14_c(const delay_budget_report_r14_c& other);
  delay_budget_report_r14_c& operator=(const delay_budget_report_r14_c& other);
  ~delay_budget_report_r14_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  type1_e_& type1()
  {
    assert_choice_type(types::type1, type_, "DelayBudgetReport-r14");
    return c.get<type1_e_>();
  }
  type2_e_& type2()
  {
    assert_choice_type(types::type2, type_, "DelayBudgetReport-r14");
    return c.get<type2_e_>();
  }
  const type1_e_& type1() const
  {
    assert_choice_type(types::type1, type_, "DelayBudgetReport-r14");
    return c.get<type1_e_>();
  }
  const type2_e_& type2() const
  {
    assert_choice_type(types::type2, type_, "DelayBudgetReport-r14");
    return c.get<type2_e_>();
  }
  type1_e_& set_type1();
  type2_e_& set_type2();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// IDC-SubframePatternList-r11 ::= SEQUENCE (SIZE (1..8)) OF IDC-SubframePattern-r11
using idc_sf_pattern_list_r11_l = dyn_array<idc_sf_pattern_r11_c>;

// InDeviceCoexIndication-v1310-IEs ::= SEQUENCE
struct in_dev_coex_ind_v1310_ies_s {
  bool                                  affected_carrier_freq_list_v1310_present    = false;
  bool                                  affected_carrier_freq_comb_list_r13_present = false;
  bool                                  non_crit_ext_present                        = false;
  affected_carrier_freq_list_v1310_l    affected_carrier_freq_list_v1310;
  affected_carrier_freq_comb_list_r13_l affected_carrier_freq_comb_list_r13;
  in_dev_coex_ind_v1360_ies_s           non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMS-ServiceList-r13 ::= SEQUENCE (SIZE (0..15)) OF MBMS-ServiceInfo-r13
using mbms_service_list_r13_l = dyn_array<mbms_service_info_r13_s>;

// MBMSInterestIndication-v1540-IEs ::= SEQUENCE
struct mbms_interest_ind_v1540_ies_s {
  using mbms_rom_info_list_r15_l_ = dyn_array<mbms_rom_info_r15_s>;

  // member variables
  bool                          mbms_rom_info_list_r15_present = false;
  bool                          non_crit_ext_present           = false;
  mbms_rom_info_list_r15_l_     mbms_rom_info_list_r15;
  mbms_interest_ind_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultFreqListFailNR-r15 ::= SEQUENCE (SIZE (1..5)) OF MeasResultFreqFailNR-r15
using meas_result_freq_list_fail_nr_r15_l = dyn_array<meas_result_freq_fail_nr_r15_s>;

// MeasResultList3EUTRA-r15 ::= SEQUENCE (SIZE (1..8)) OF MeasResult3EUTRA-r15
using meas_result_list3_eutra_r15_l = dyn_array<meas_result3_eutra_r15_s>;

// RRCConnectionReconfigurationComplete-v1020-IEs ::= SEQUENCE
struct rrc_conn_recfg_complete_v1020_ies_s {
  bool                                rlf_info_available_r10_present = false;
  bool                                log_meas_available_r10_present = false;
  bool                                non_crit_ext_present           = false;
  rrc_conn_recfg_complete_v1130_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentComplete-v8a0-IEs ::= SEQUENCE
struct rrc_conn_reest_complete_v8a0_ies_s {
  bool                                late_non_crit_ext_present = false;
  bool                                non_crit_ext_present      = false;
  dyn_octstring                       late_non_crit_ext;
  rrc_conn_reest_complete_v1020_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResumeComplete-v1610-IEs ::= SEQUENCE
struct rrc_conn_resume_complete_v1610_ies_s {
  bool                                 meas_result_list_idle_r16_present     = false;
  bool                                 meas_result_list_ext_idle_r16_present = false;
  bool                                 meas_result_list_idle_nr_r16_present  = false;
  bool                                 scg_cfg_resp_nr_r16_present           = false;
  bool                                 non_crit_ext_present                  = false;
  meas_result_list_idle_r15_l          meas_result_list_idle_r16;
  meas_result_list_ext_idle_r16_l      meas_result_list_ext_idle_r16;
  meas_result_list_idle_nr_r16_l       meas_result_list_idle_nr_r16;
  dyn_octstring                        scg_cfg_resp_nr_r16;
  rrc_conn_resume_complete_v1710_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-v1020-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_v1020_ies_s {
  struct gummei_type_r10_opts {
    enum options { native, mapped, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<gummei_type_r10_opts> gummei_type_r10_e_;
  struct rn_sf_cfg_req_r10_opts {
    enum options { required, not_required, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<rn_sf_cfg_req_r10_opts> rn_sf_cfg_req_r10_e_;

  // member variables
  bool                                gummei_type_r10_present        = false;
  bool                                rlf_info_available_r10_present = false;
  bool                                log_meas_available_r10_present = false;
  bool                                rn_sf_cfg_req_r10_present      = false;
  bool                                non_crit_ext_present           = false;
  gummei_type_r10_e_                  gummei_type_r10;
  rn_sf_cfg_req_r10_e_                rn_sf_cfg_req_r10;
  rrc_conn_setup_complete_v1130_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RSTD-InterFreqInfo-r10 ::= SEQUENCE
struct rstd_inter_freq_info_r10_s {
  struct meas_prs_offset_r15_c_ {
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
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    meas_prs_offset_r15_c_() = default;
    meas_prs_offset_r15_c_(const meas_prs_offset_r15_c_& other);
    meas_prs_offset_r15_c_& operator=(const meas_prs_offset_r15_c_& other);
    ~meas_prs_offset_r15_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& rstd0_r15()
    {
      assert_choice_type(types::rstd0_r15, type_, "measPRS-Offset-r15");
      return c.get<uint8_t>();
    }
    uint8_t& rstd1_r15()
    {
      assert_choice_type(types::rstd1_r15, type_, "measPRS-Offset-r15");
      return c.get<uint8_t>();
    }
    uint16_t& rstd2_r15()
    {
      assert_choice_type(types::rstd2_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    uint16_t& rstd3_r15()
    {
      assert_choice_type(types::rstd3_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    uint16_t& rstd4_r15()
    {
      assert_choice_type(types::rstd4_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    uint8_t& rstd5_r15()
    {
      assert_choice_type(types::rstd5_r15, type_, "measPRS-Offset-r15");
      return c.get<uint8_t>();
    }
    uint16_t& rstd6_r15()
    {
      assert_choice_type(types::rstd6_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    uint16_t& rstd7_r15()
    {
      assert_choice_type(types::rstd7_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    uint16_t& rstd8_r15()
    {
      assert_choice_type(types::rstd8_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    uint16_t& rstd9_r15()
    {
      assert_choice_type(types::rstd9_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    uint16_t& rstd10_r15()
    {
      assert_choice_type(types::rstd10_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    uint16_t& rstd11_r15()
    {
      assert_choice_type(types::rstd11_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    uint16_t& rstd12_r15()
    {
      assert_choice_type(types::rstd12_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    uint16_t& rstd13_r15()
    {
      assert_choice_type(types::rstd13_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    uint16_t& rstd14_r15()
    {
      assert_choice_type(types::rstd14_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    uint16_t& rstd15_r15()
    {
      assert_choice_type(types::rstd15_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    uint16_t& rstd16_r15()
    {
      assert_choice_type(types::rstd16_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    uint16_t& rstd17_r15()
    {
      assert_choice_type(types::rstd17_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    uint16_t& rstd18_r15()
    {
      assert_choice_type(types::rstd18_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    uint16_t& rstd19_r15()
    {
      assert_choice_type(types::rstd19_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    uint16_t& rstd20_r15()
    {
      assert_choice_type(types::rstd20_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint8_t& rstd0_r15() const
    {
      assert_choice_type(types::rstd0_r15, type_, "measPRS-Offset-r15");
      return c.get<uint8_t>();
    }
    const uint8_t& rstd1_r15() const
    {
      assert_choice_type(types::rstd1_r15, type_, "measPRS-Offset-r15");
      return c.get<uint8_t>();
    }
    const uint16_t& rstd2_r15() const
    {
      assert_choice_type(types::rstd2_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint16_t& rstd3_r15() const
    {
      assert_choice_type(types::rstd3_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint16_t& rstd4_r15() const
    {
      assert_choice_type(types::rstd4_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint8_t& rstd5_r15() const
    {
      assert_choice_type(types::rstd5_r15, type_, "measPRS-Offset-r15");
      return c.get<uint8_t>();
    }
    const uint16_t& rstd6_r15() const
    {
      assert_choice_type(types::rstd6_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint16_t& rstd7_r15() const
    {
      assert_choice_type(types::rstd7_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint16_t& rstd8_r15() const
    {
      assert_choice_type(types::rstd8_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint16_t& rstd9_r15() const
    {
      assert_choice_type(types::rstd9_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint16_t& rstd10_r15() const
    {
      assert_choice_type(types::rstd10_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint16_t& rstd11_r15() const
    {
      assert_choice_type(types::rstd11_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint16_t& rstd12_r15() const
    {
      assert_choice_type(types::rstd12_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint16_t& rstd13_r15() const
    {
      assert_choice_type(types::rstd13_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint16_t& rstd14_r15() const
    {
      assert_choice_type(types::rstd14_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint16_t& rstd15_r15() const
    {
      assert_choice_type(types::rstd15_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint16_t& rstd16_r15() const
    {
      assert_choice_type(types::rstd16_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint16_t& rstd17_r15() const
    {
      assert_choice_type(types::rstd17_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint16_t& rstd18_r15() const
    {
      assert_choice_type(types::rstd18_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint16_t& rstd19_r15() const
    {
      assert_choice_type(types::rstd19_r15, type_, "measPRS-Offset-r15");
      return c.get<uint16_t>();
    }
    const uint16_t& rstd20_r15() const
    {
      assert_choice_type(types::rstd20_r15, type_, "measPRS-Offset-r15");
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
  bool     ext                 = false;
  uint32_t carrier_freq_r10    = 0;
  uint8_t  meas_prs_offset_r10 = 0;
  // ...
  // group 0
  bool     carrier_freq_v1090_present = false;
  uint32_t carrier_freq_v1090         = 65536;
  // group 1
  copy_ptr<meas_prs_offset_r15_c_> meas_prs_offset_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-CommTxResourceReq-r12 ::= SEQUENCE
struct sl_comm_tx_res_req_r12_s {
  bool                    carrier_freq_r12_present = false;
  uint32_t                carrier_freq_r12         = 0;
  sl_dest_info_list_r12_l dest_info_list_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-DiscSysInfoReportFreqList-r13 ::= SEQUENCE (SIZE (1..8)) OF SL-DiscSysInfoReport-r13
using sl_disc_sys_info_report_freq_list_r13_l = dyn_array<sl_disc_sys_info_report_r13_s>;

// SL-DiscTxResourceReqPerFreqList-r13 ::= SEQUENCE (SIZE (1..8)) OF SL-DiscTxResourceReq-r13
using sl_disc_tx_res_req_per_freq_list_r13_l = dyn_array<sl_disc_tx_res_req_r13_s>;

// SL-GapRequest-r13 ::= SEQUENCE (SIZE (1..8)) OF SL-GapFreqInfo-r13
using sl_gap_request_r13_l = dyn_array<sl_gap_freq_info_r13_s>;

// SidelinkUEInformation-v1430-IEs ::= SEQUENCE
struct sidelink_ue_info_v1430_ies_s {
  bool                           v2x_comm_rx_interested_freq_list_r14_present = false;
  bool                           p2x_comm_tx_type_r14_present                 = false;
  bool                           v2x_comm_tx_res_req_r14_present              = false;
  bool                           non_crit_ext_present                         = false;
  sl_v2x_comm_freq_list_r14_l    v2x_comm_rx_interested_freq_list_r14;
  sl_v2x_comm_tx_freq_list_r14_l v2x_comm_tx_res_req_r14;
  sidelink_ue_info_v1530_ies_s   non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TrafficPatternInfoList-r14 ::= SEQUENCE (SIZE (1..8)) OF TrafficPatternInfo-r14
using traffic_pattern_info_list_r14_l = dyn_array<traffic_pattern_info_r14_s>;

// UEAssistanceInformation-v1450-IEs ::= SEQUENCE
struct ueassist_info_v1450_ies_s {
  bool                      overheat_assist_r14_present = false;
  bool                      non_crit_ext_present        = false;
  overheat_assist_r14_s     overheat_assist_r14;
  ueassist_info_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationResponse-v1020-IEs ::= SEQUENCE
struct ue_info_resp_v1020_ies_s {
  bool                     log_meas_report_r10_present = false;
  bool                     non_crit_ext_present        = false;
  log_meas_report_r10_s    log_meas_report_r10;
  ue_info_resp_v1130_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WLAN-Status-v1430 ::= ENUMERATED
struct wlan_status_v1430_opts {
  enum options { suspended, resumed, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<wlan_status_v1430_opts> wlan_status_v1430_e;

// AffectedCarrierFreqList-r11 ::= SEQUENCE (SIZE (1..32)) OF AffectedCarrierFreq-r11
using affected_carrier_freq_list_r11_l = dyn_array<affected_carrier_freq_r11_s>;

// CSFBParametersRequestCDMA2000-v8a0-IEs ::= SEQUENCE
struct csfb_params_request_cdma2000_v8a0_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqListMBMS-r11 ::= SEQUENCE (SIZE (1..5)) OF INTEGER (0..262143)
using carrier_freq_list_mbms_r11_l = bounded_array<uint32_t, 5>;

// CounterCheckResponse-v8a0-IEs ::= SEQUENCE
struct counter_check_resp_v8a0_ies_s {
  bool                           late_non_crit_ext_present = false;
  bool                           non_crit_ext_present      = false;
  dyn_octstring                  late_non_crit_ext;
  counter_check_resp_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CountingResponseList-r10 ::= SEQUENCE (SIZE (1..16)) OF CountingResponseInfo-r10
using count_resp_list_r10_l = dyn_array<count_resp_info_r10_s>;

// DRB-CountInfoList ::= SEQUENCE (SIZE (0..11)) OF DRB-CountInfo
using drb_count_info_list_l = dyn_array<drb_count_info_s>;

// FailedLogicalChannelIdentity-r16 ::= SEQUENCE
struct failed_lc_ch_id_r16_s {
  struct cell_group_ind_r16_opts {
    enum options { mn, sn, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<cell_group_ind_r16_opts> cell_group_ind_r16_e_;

  // member variables
  bool                  lc_ch_id_r16_present     = false;
  bool                  lc_ch_id_ext_r16_present = false;
  cell_group_ind_r16_e_ cell_group_ind_r16;
  uint8_t               lc_ch_id_r16     = 1;
  uint8_t               lc_ch_id_ext_r16 = 32;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FailureReportMCG-r16 ::= SEQUENCE
struct fail_report_mcg_r16_s {
  struct fail_type_r16_opts {
    enum options {
      t310_expiry,
      random_access_problem,
      rlc_max_num_retx,
      t312_expiry,
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
  typedef enumerated<fail_type_r16_opts> fail_type_r16_e_;

  // member variables
  bool                                ext                                     = false;
  bool                                fail_type_r16_present                   = false;
  bool                                meas_result_freq_list_eutra_r16_present = false;
  bool                                meas_result_freq_list_nr_r16_present    = false;
  bool                                meas_result_freq_list_geran_r16_present = false;
  bool                                meas_result_freq_list_utra_r16_present  = false;
  bool                                meas_result_scg_r16_present             = false;
  fail_type_r16_e_                    fail_type_r16;
  meas_result_list3_eutra_r15_l       meas_result_freq_list_eutra_r16;
  meas_result_freq_list_fail_nr_r15_l meas_result_freq_list_nr_r16;
  meas_result_list2_geran_r10_l       meas_result_freq_list_geran_r16;
  meas_result_list2_utra_r9_l         meas_result_freq_list_utra_r16;
  dyn_octstring                       meas_result_scg_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FailureReportSCG-NR-r15 ::= SEQUENCE
struct fail_report_scg_nr_r15_s {
  struct fail_type_r15_opts {
    enum options {
      t310_expiry,
      random_access_problem,
      rlc_max_num_retx,
      synch_recfg_fail_scg,
      scg_recfg_fail,
      srb3_integrity_fail,
      dummy,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<fail_type_r15_opts> fail_type_r15_e_;
  struct fail_type_v1610_opts {
    enum options {
      t312_expiry,
      scg_lbt_fail,
      beam_fail_recovery_fail,
      bh_rlf_r16,
      beam_fail_r17,
      spare3,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<fail_type_v1610_opts> fail_type_v1610_e_;

  // member variables
  bool                                ext                                  = false;
  bool                                meas_result_freq_list_nr_r15_present = false;
  bool                                meas_result_scg_r15_present          = false;
  fail_type_r15_e_                    fail_type_r15;
  meas_result_freq_list_fail_nr_r15_l meas_result_freq_list_nr_r15;
  dyn_octstring                       meas_result_scg_r15;
  // ...
  // group 0
  bool                                      fail_type_v1610_present = false;
  copy_ptr<location_info_r10_s>             location_info_r16;
  copy_ptr<log_meas_result_list_bt_r15_l>   log_meas_result_list_bt_r16;
  copy_ptr<log_meas_result_list_wlan_r15_l> log_meas_result_list_wlan_r16;
  fail_type_v1610_e_                        fail_type_v1610;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FailureReportSCG-r12 ::= SEQUENCE
struct fail_report_scg_r12_s {
  struct fail_type_r12_opts {
    enum options { t313_expiry, random_access_problem, rlc_max_num_retx, scg_change_fail, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<fail_type_r12_opts> fail_type_r12_e_;

  // member variables
  bool                             ext                                    = false;
  bool                             meas_result_serv_freq_list_r12_present = false;
  bool                             meas_result_neigh_cells_r12_present    = false;
  fail_type_r12_e_                 fail_type_r12;
  meas_result_serv_freq_list_r10_l meas_result_serv_freq_list_r12;
  meas_result_list2_eutra_r9_l     meas_result_neigh_cells_r12;
  // ...
  // group 0
  bool fail_type_v1290_present = false;
  // group 1
  copy_ptr<meas_result_serv_freq_list_ext_r13_l> meas_result_serv_freq_list_ext_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InDeviceCoexIndication-v11d0-IEs ::= SEQUENCE
struct in_dev_coex_ind_v11d0_ies_s {
  struct ul_ca_assist_info_r11_s_ {
    bool                                  affected_carrier_freq_comb_list_r11_present = false;
    affected_carrier_freq_comb_list_r11_l affected_carrier_freq_comb_list_r11;
    victim_sys_type_r11_s                 victim_sys_type_r11;
  };

  // member variables
  bool                        ul_ca_assist_info_r11_present = false;
  bool                        non_crit_ext_present          = false;
  ul_ca_assist_info_r11_s_    ul_ca_assist_info_r11;
  in_dev_coex_ind_v1310_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMSInterestIndication-v1310-IEs ::= SEQUENCE
struct mbms_interest_ind_v1310_ies_s {
  bool                          mbms_services_r13_present = false;
  bool                          non_crit_ext_present      = false;
  mbms_service_list_r13_l       mbms_services_r13;
  mbms_interest_ind_v1540_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasReportAppLayer-v1590-IEs ::= SEQUENCE
struct meas_report_app_layer_v1590_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ProximityIndication-v930-IEs ::= SEQUENCE
struct proximity_ind_v930_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RACH-Report-r16 ::= SEQUENCE
struct rach_report_r16_s {
  uint8_t nof_preambs_sent_r16    = 1;
  bool    contention_detected_r16 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLF-Report-r9 ::= SEQUENCE
struct rlf_report_r9_s {
  struct meas_result_last_serv_cell_r9_s_ {
    bool    rsrq_result_r9_present = false;
    uint8_t rsrp_result_r9         = 0;
    uint8_t rsrq_result_r9         = 0;
  };
  struct meas_result_neigh_cells_r9_s_ {
    bool                            meas_result_list_eutra_r9_present = false;
    bool                            meas_result_list_utra_r9_present  = false;
    bool                            meas_result_list_geran_r9_present = false;
    bool                            meas_results_cdma2000_r9_present  = false;
    meas_result_list2_eutra_r9_l    meas_result_list_eutra_r9;
    meas_result_list2_utra_r9_l     meas_result_list_utra_r9;
    meas_result_list_geran_l        meas_result_list_geran_r9;
    meas_result_list2_cdma2000_r9_l meas_results_cdma2000_r9;
  };
  struct failed_pcell_id_r10_c_ {
    struct pci_arfcn_r10_s_ {
      uint16_t pci_r10          = 0;
      uint32_t carrier_freq_r10 = 0;
    };
    struct types_opts {
      enum options { cell_global_id_r10, pci_arfcn_r10, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    failed_pcell_id_r10_c_() = default;
    failed_pcell_id_r10_c_(const failed_pcell_id_r10_c_& other);
    failed_pcell_id_r10_c_& operator=(const failed_pcell_id_r10_c_& other);
    ~failed_pcell_id_r10_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cell_global_id_eutra_s& cell_global_id_r10()
    {
      assert_choice_type(types::cell_global_id_r10, type_, "failedPCellId-r10");
      return c.get<cell_global_id_eutra_s>();
    }
    pci_arfcn_r10_s_& pci_arfcn_r10()
    {
      assert_choice_type(types::pci_arfcn_r10, type_, "failedPCellId-r10");
      return c.get<pci_arfcn_r10_s_>();
    }
    const cell_global_id_eutra_s& cell_global_id_r10() const
    {
      assert_choice_type(types::cell_global_id_r10, type_, "failedPCellId-r10");
      return c.get<cell_global_id_eutra_s>();
    }
    const pci_arfcn_r10_s_& pci_arfcn_r10() const
    {
      assert_choice_type(types::pci_arfcn_r10, type_, "failedPCellId-r10");
      return c.get<pci_arfcn_r10_s_>();
    }
    cell_global_id_eutra_s& set_cell_global_id_r10();
    pci_arfcn_r10_s_&       set_pci_arfcn_r10();

  private:
    types                                                     type_;
    choice_buffer_t<cell_global_id_eutra_s, pci_arfcn_r10_s_> c;

    void destroy_();
  };
  struct conn_fail_type_r10_opts {
    enum options { rlf, hof, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<conn_fail_type_r10_opts> conn_fail_type_r10_e_;
  struct failed_pcell_id_v1090_s_ {
    uint32_t carrier_freq_v1090 = 65536;
  };
  struct basic_fields_r11_s_ {
    struct rlf_cause_r11_opts {
      enum options { t310_expiry, random_access_problem, rlc_max_num_retx, t312_expiry_r12, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<rlf_cause_r11_opts> rlf_cause_r11_e_;

    // member variables
    fixed_bitstring<16> c_rnti_r11;
    rlf_cause_r11_e_    rlf_cause_r11;
    uint32_t            time_since_fail_r11 = 0;
  };
  struct prev_utra_cell_id_r11_s_ {
    struct pci_r11_c_ {
      struct types_opts {
        enum options { fdd_r11, tdd_r11, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      pci_r11_c_() = default;
      pci_r11_c_(const pci_r11_c_& other);
      pci_r11_c_& operator=(const pci_r11_c_& other);
      ~pci_r11_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      uint16_t& fdd_r11()
      {
        assert_choice_type(types::fdd_r11, type_, "physCellId-r11");
        return c.get<uint16_t>();
      }
      uint8_t& tdd_r11()
      {
        assert_choice_type(types::tdd_r11, type_, "physCellId-r11");
        return c.get<uint8_t>();
      }
      const uint16_t& fdd_r11() const
      {
        assert_choice_type(types::fdd_r11, type_, "physCellId-r11");
        return c.get<uint16_t>();
      }
      const uint8_t& tdd_r11() const
      {
        assert_choice_type(types::tdd_r11, type_, "physCellId-r11");
        return c.get<uint8_t>();
      }
      uint16_t& set_fdd_r11();
      uint8_t&  set_tdd_r11();

    private:
      types               type_;
      pod_choice_buffer_t c;

      void destroy_();
    };

    // member variables
    bool                  cell_global_id_r11_present = false;
    uint16_t              carrier_freq_r11           = 0;
    pci_r11_c_            pci_r11;
    cell_global_id_utra_s cell_global_id_r11;
  };
  struct sel_utra_cell_id_r11_s_ {
    struct pci_r11_c_ {
      struct types_opts {
        enum options { fdd_r11, tdd_r11, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      pci_r11_c_() = default;
      pci_r11_c_(const pci_r11_c_& other);
      pci_r11_c_& operator=(const pci_r11_c_& other);
      ~pci_r11_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      uint16_t& fdd_r11()
      {
        assert_choice_type(types::fdd_r11, type_, "physCellId-r11");
        return c.get<uint16_t>();
      }
      uint8_t& tdd_r11()
      {
        assert_choice_type(types::tdd_r11, type_, "physCellId-r11");
        return c.get<uint8_t>();
      }
      const uint16_t& fdd_r11() const
      {
        assert_choice_type(types::fdd_r11, type_, "physCellId-r11");
        return c.get<uint16_t>();
      }
      const uint8_t& tdd_r11() const
      {
        assert_choice_type(types::tdd_r11, type_, "physCellId-r11");
        return c.get<uint8_t>();
      }
      uint16_t& set_fdd_r11();
      uint8_t&  set_tdd_r11();

    private:
      types               type_;
      pod_choice_buffer_t c;

      void destroy_();
    };

    // member variables
    uint16_t   carrier_freq_r11 = 0;
    pci_r11_c_ pci_r11;
  };
  struct failed_pcell_id_v1250_s_ {
    fixed_bitstring<16> tac_failed_pcell_r12;
  };
  struct failed_nr_pcell_id_r16_c_ {
    struct pci_arfcn_s_ {
      uint16_t pci_r16          = 0;
      uint32_t carrier_freq_r16 = 0;
    };
    struct types_opts {
      enum options { cell_global_id, pci_arfcn, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    failed_nr_pcell_id_r16_c_() = default;
    failed_nr_pcell_id_r16_c_(const failed_nr_pcell_id_r16_c_& other);
    failed_nr_pcell_id_r16_c_& operator=(const failed_nr_pcell_id_r16_c_& other);
    ~failed_nr_pcell_id_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cell_global_id_nr_r16_s& cell_global_id()
    {
      assert_choice_type(types::cell_global_id, type_, "failedNR-PCellId-r16");
      return c.get<cell_global_id_nr_r16_s>();
    }
    pci_arfcn_s_& pci_arfcn()
    {
      assert_choice_type(types::pci_arfcn, type_, "failedNR-PCellId-r16");
      return c.get<pci_arfcn_s_>();
    }
    const cell_global_id_nr_r16_s& cell_global_id() const
    {
      assert_choice_type(types::cell_global_id, type_, "failedNR-PCellId-r16");
      return c.get<cell_global_id_nr_r16_s>();
    }
    const pci_arfcn_s_& pci_arfcn() const
    {
      assert_choice_type(types::pci_arfcn, type_, "failedNR-PCellId-r16");
      return c.get<pci_arfcn_s_>();
    }
    cell_global_id_nr_r16_s& set_cell_global_id();
    pci_arfcn_s_&            set_pci_arfcn();

  private:
    types                                                  type_;
    choice_buffer_t<cell_global_id_nr_r16_s, pci_arfcn_s_> c;

    void destroy_();
  };
  struct reconnect_cell_id_r16_c_ {
    struct eutra_reconnect_cell_id_s_ {
      bool                   tac_epc_r16_present       = false;
      bool                   tac_minus5_gc_r16_present = false;
      cell_global_id_eutra_s cell_global_id_r16;
      fixed_bitstring<16>    tac_epc_r16;
      fixed_bitstring<24>    tac_minus5_gc_r16;
    };
    struct types_opts {
      enum options { nr_reconnect_cell_id, eutra_reconnect_cell_id, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    reconnect_cell_id_r16_c_() = default;
    reconnect_cell_id_r16_c_(const reconnect_cell_id_r16_c_& other);
    reconnect_cell_id_r16_c_& operator=(const reconnect_cell_id_r16_c_& other);
    ~reconnect_cell_id_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    cell_global_id_nr_r16_s& nr_reconnect_cell_id()
    {
      assert_choice_type(types::nr_reconnect_cell_id, type_, "reconnectCellId-r16");
      return c.get<cell_global_id_nr_r16_s>();
    }
    eutra_reconnect_cell_id_s_& eutra_reconnect_cell_id()
    {
      assert_choice_type(types::eutra_reconnect_cell_id, type_, "reconnectCellId-r16");
      return c.get<eutra_reconnect_cell_id_s_>();
    }
    const cell_global_id_nr_r16_s& nr_reconnect_cell_id() const
    {
      assert_choice_type(types::nr_reconnect_cell_id, type_, "reconnectCellId-r16");
      return c.get<cell_global_id_nr_r16_s>();
    }
    const eutra_reconnect_cell_id_s_& eutra_reconnect_cell_id() const
    {
      assert_choice_type(types::eutra_reconnect_cell_id, type_, "reconnectCellId-r16");
      return c.get<eutra_reconnect_cell_id_s_>();
    }
    cell_global_id_nr_r16_s&    set_nr_reconnect_cell_id();
    eutra_reconnect_cell_id_s_& set_eutra_reconnect_cell_id();

  private:
    types                                                                type_;
    choice_buffer_t<cell_global_id_nr_r16_s, eutra_reconnect_cell_id_s_> c;

    void destroy_();
  };
  struct meas_result_list_nr_v1640_s_ {
    uint32_t carrier_freq_nr_r16 = 0;
  };

  // member variables
  bool                             ext                                = false;
  bool                             meas_result_neigh_cells_r9_present = false;
  meas_result_last_serv_cell_r9_s_ meas_result_last_serv_cell_r9;
  meas_result_neigh_cells_r9_s_    meas_result_neigh_cells_r9;
  // ...
  // group 0
  bool                             time_conn_fail_r10_present = false;
  bool                             conn_fail_type_r10_present = false;
  copy_ptr<location_info_r10_s>    location_info_r10;
  copy_ptr<failed_pcell_id_r10_c_> failed_pcell_id_r10;
  copy_ptr<cell_global_id_eutra_s> reest_cell_id_r10;
  uint16_t                         time_conn_fail_r10 = 0;
  conn_fail_type_r10_e_            conn_fail_type_r10;
  copy_ptr<cell_global_id_eutra_s> prev_pcell_id_r10;
  // group 1
  copy_ptr<failed_pcell_id_v1090_s_> failed_pcell_id_v1090;
  // group 2
  copy_ptr<basic_fields_r11_s_>      basic_fields_r11;
  copy_ptr<prev_utra_cell_id_r11_s_> prev_utra_cell_id_r11;
  copy_ptr<sel_utra_cell_id_r11_s_>  sel_utra_cell_id_r11;
  // group 3
  bool                                      meas_result_last_serv_cell_v1250_present = false;
  copy_ptr<failed_pcell_id_v1250_s_>        failed_pcell_id_v1250;
  int8_t                                    meas_result_last_serv_cell_v1250 = -30;
  copy_ptr<rsrq_type_r12_s>                 last_serv_cell_rsrq_type_r12;
  copy_ptr<meas_result_list2_eutra_v1250_l> meas_result_list_eutra_v1250;
  // group 4
  bool drb_established_with_qci_minus1_r13_present = false;
  // group 5
  bool   meas_result_last_serv_cell_v1360_present = false;
  int8_t meas_result_last_serv_cell_v1360         = -17;
  // group 6
  copy_ptr<log_meas_result_list_bt_r15_l>   log_meas_result_list_bt_r15;
  copy_ptr<log_meas_result_list_wlan_r15_l> log_meas_result_list_wlan_r15;
  // group 7
  bool                                     time_until_reconn_r16_present = false;
  copy_ptr<meas_result_cell_list_nr_r15_l> meas_result_list_nr_r16;
  copy_ptr<cell_global_id_nr_r16_s>        prev_nr_pcell_id_r16;
  copy_ptr<failed_nr_pcell_id_r16_c_>      failed_nr_pcell_id_r16;
  copy_ptr<reconnect_cell_id_r16_c_>       reconnect_cell_id_r16;
  uint32_t                                 time_until_reconn_r16 = 0;
  // group 8
  copy_ptr<meas_result_list_nr_v1640_s_>   meas_result_list_nr_v1640;
  copy_ptr<meas_result_freq_list_nr_r16_l> meas_result_list_ext_nr_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfigurationComplete-v8a0-IEs ::= SEQUENCE
struct rrc_conn_recfg_complete_v8a0_ies_s {
  bool                                late_non_crit_ext_present = false;
  bool                                non_crit_ext_present      = false;
  dyn_octstring                       late_non_crit_ext;
  rrc_conn_recfg_complete_v1020_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentComplete-v920-IEs ::= SEQUENCE
struct rrc_conn_reest_complete_v920_ies_s {
  bool                               rlf_info_available_r9_present = false;
  bool                               non_crit_ext_present          = false;
  rrc_conn_reest_complete_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResumeComplete-v1530-IEs ::= SEQUENCE
struct rrc_conn_resume_complete_v1530_ies_s {
  bool                                 log_meas_available_bt_r15_present      = false;
  bool                                 log_meas_available_wlan_r15_present    = false;
  bool                                 idle_meas_available_r15_present        = false;
  bool                                 flight_path_info_available_r15_present = false;
  bool                                 non_crit_ext_present                   = false;
  rrc_conn_resume_complete_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-v8a0-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_v8a0_ies_s {
  bool                                late_non_crit_ext_present = false;
  bool                                non_crit_ext_present      = false;
  dyn_octstring                       late_non_crit_ext;
  rrc_conn_setup_complete_v1020_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RSTD-InterFreqInfoList-r10 ::= SEQUENCE (SIZE (1..3)) OF RSTD-InterFreqInfo-r10
using rstd_inter_freq_info_list_r10_l = dyn_array<rstd_inter_freq_info_r10_s>;

// RegisteredMME ::= SEQUENCE
struct registered_mme_s {
  bool                plmn_id_present = false;
  plmn_id_s           plmn_id;
  fixed_bitstring<16> mmegi;
  fixed_bitstring<8>  mmec;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCGFailureInformation-v12d0a-IEs ::= SEQUENCE
struct scg_fail_info_v12d0a_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCGFailureInformationNR-v1590-IEs ::= SEQUENCE
struct scg_fail_info_nr_v1590_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SidelinkUEInformation-v1310-IEs ::= SEQUENCE
struct sidelink_ue_info_v1310_ies_s {
  struct comm_tx_res_info_req_relay_r13_s_ {
    struct ue_type_r13_opts {
      enum options { relay_ue, remote_ue, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<ue_type_r13_opts> ue_type_r13_e_;

    // member variables
    bool                     comm_tx_res_req_relay_r13_present    = false;
    bool                     comm_tx_res_req_relay_uc_r13_present = false;
    sl_comm_tx_res_req_r12_s comm_tx_res_req_relay_r13;
    sl_comm_tx_res_req_r12_s comm_tx_res_req_relay_uc_r13;
    ue_type_r13_e_           ue_type_r13;
  };
  struct disc_tx_res_req_v1310_s_ {
    bool                                   carrier_freq_disc_tx_r13_present     = false;
    bool                                   disc_tx_res_req_add_freq_r13_present = false;
    uint8_t                                carrier_freq_disc_tx_r13             = 1;
    sl_disc_tx_res_req_per_freq_list_r13_l disc_tx_res_req_add_freq_r13;
  };

  // member variables
  bool                                    comm_tx_res_req_uc_r13_present             = false;
  bool                                    comm_tx_res_info_req_relay_r13_present     = false;
  bool                                    disc_tx_res_req_v1310_present              = false;
  bool                                    disc_tx_res_req_ps_r13_present             = false;
  bool                                    disc_rx_gap_req_r13_present                = false;
  bool                                    disc_tx_gap_req_r13_present                = false;
  bool                                    disc_sys_info_report_freq_list_r13_present = false;
  bool                                    non_crit_ext_present                       = false;
  sl_comm_tx_res_req_r12_s                comm_tx_res_req_uc_r13;
  comm_tx_res_info_req_relay_r13_s_       comm_tx_res_info_req_relay_r13;
  disc_tx_res_req_v1310_s_                disc_tx_res_req_v1310;
  sl_disc_tx_res_req_r13_s                disc_tx_res_req_ps_r13;
  sl_gap_request_r13_l                    disc_rx_gap_req_r13;
  sl_gap_request_r13_l                    disc_tx_gap_req_r13;
  sl_disc_sys_info_report_freq_list_r13_l disc_sys_info_report_freq_list_r13;
  sidelink_ue_info_v1430_ies_s            non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDM-AssistanceInfo-r11 ::= CHOICE
struct tdm_assist_info_r11_c {
  struct drx_assist_info_r11_s_ {
    struct drx_cycle_len_r11_opts {
      enum options { sf40, sf64, sf80, sf128, sf160, sf256, spare2, spare1, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<drx_cycle_len_r11_opts> drx_cycle_len_r11_e_;
    struct drx_active_time_r11_opts {
      enum options { sf20, sf30, sf40, sf60, sf80, sf100, spare2, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<drx_active_time_r11_opts> drx_active_time_r11_e_;

    // member variables
    bool                   drx_offset_r11_present = false;
    drx_cycle_len_r11_e_   drx_cycle_len_r11;
    uint16_t               drx_offset_r11 = 0;
    drx_active_time_r11_e_ drx_active_time_r11;
  };
  struct types_opts {
    enum options { drx_assist_info_r11, idc_sf_pattern_list_r11, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  tdm_assist_info_r11_c() = default;
  tdm_assist_info_r11_c(const tdm_assist_info_r11_c& other);
  tdm_assist_info_r11_c& operator=(const tdm_assist_info_r11_c& other);
  ~tdm_assist_info_r11_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  drx_assist_info_r11_s_& drx_assist_info_r11()
  {
    assert_choice_type(types::drx_assist_info_r11, type_, "TDM-AssistanceInfo-r11");
    return c.get<drx_assist_info_r11_s_>();
  }
  idc_sf_pattern_list_r11_l& idc_sf_pattern_list_r11()
  {
    assert_choice_type(types::idc_sf_pattern_list_r11, type_, "TDM-AssistanceInfo-r11");
    return c.get<idc_sf_pattern_list_r11_l>();
  }
  const drx_assist_info_r11_s_& drx_assist_info_r11() const
  {
    assert_choice_type(types::drx_assist_info_r11, type_, "TDM-AssistanceInfo-r11");
    return c.get<drx_assist_info_r11_s_>();
  }
  const idc_sf_pattern_list_r11_l& idc_sf_pattern_list_r11() const
  {
    assert_choice_type(types::idc_sf_pattern_list_r11, type_, "TDM-AssistanceInfo-r11");
    return c.get<idc_sf_pattern_list_r11_l>();
  }
  drx_assist_info_r11_s_&    set_drx_assist_info_r11();
  idc_sf_pattern_list_r11_l& set_idc_sf_pattern_list_r11();

private:
  types                                                              type_;
  choice_buffer_t<drx_assist_info_r11_s_, idc_sf_pattern_list_r11_l> c;

  void destroy_();
};

// UEAssistanceInformation-v1430-IEs ::= SEQUENCE
struct ueassist_info_v1430_ies_s {
  struct sps_assist_info_r14_s_ {
    bool                            traffic_pattern_info_list_sl_r14_present = false;
    bool                            traffic_pattern_info_list_ul_r14_present = false;
    traffic_pattern_info_list_r14_l traffic_pattern_info_list_sl_r14;
    traffic_pattern_info_list_r14_l traffic_pattern_info_list_ul_r14;
  };
  struct rlm_report_r14_s_ {
    struct rlm_event_r14_opts {
      enum options { early_out_of_sync, early_in_sync, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<rlm_event_r14_opts> rlm_event_r14_e_;
    struct excess_rep_mpdcch_r14_opts {
      enum options { excess_rep1, excess_rep2, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<excess_rep_mpdcch_r14_opts> excess_rep_mpdcch_r14_e_;

    // member variables
    bool                     excess_rep_mpdcch_r14_present = false;
    rlm_event_r14_e_         rlm_event_r14;
    excess_rep_mpdcch_r14_e_ excess_rep_mpdcch_r14;
  };

  // member variables
  bool                      bw_pref_r14_present             = false;
  bool                      sps_assist_info_r14_present     = false;
  bool                      rlm_report_r14_present          = false;
  bool                      delay_budget_report_r14_present = false;
  bool                      non_crit_ext_present            = false;
  bw_pref_r14_s             bw_pref_r14;
  sps_assist_info_r14_s_    sps_assist_info_r14;
  rlm_report_r14_s_         rlm_report_r14;
  delay_budget_report_r14_c delay_budget_report_r14;
  ueassist_info_v1450_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationResponse-v930-IEs ::= SEQUENCE
struct ue_info_resp_v930_ies_s {
  bool                     late_non_crit_ext_present = false;
  bool                     non_crit_ext_present      = false;
  dyn_octstring            late_non_crit_ext;
  ue_info_resp_v1020_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULHandoverPreparationTransfer-v8a0-IEs ::= SEQUENCE
struct ul_ho_prep_transfer_v8a0_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULInformationTransfer-v8a0-IEs ::= SEQUENCE
struct ul_info_transfer_v8a0_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WLAN-Status-r13 ::= ENUMERATED
struct wlan_status_r13_opts {
  enum options { successful_assoc, fail_wlan_radio_link, fail_wlan_unavailable, fail_timeout, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<wlan_status_r13_opts> wlan_status_r13_e;

// WLANConnectionStatusReport-v1430-IEs ::= SEQUENCE
struct wlan_conn_status_report_v1430_ies_s {
  bool                non_crit_ext_present = false;
  wlan_status_v1430_e wlan_status_v1430;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSFBParametersRequestCDMA2000-r8-IEs ::= SEQUENCE
struct csfb_params_request_cdma2000_r8_ies_s {
  bool                                    non_crit_ext_present = false;
  csfb_params_request_cdma2000_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CounterCheckResponse-r8-IEs ::= SEQUENCE
struct counter_check_resp_r8_ies_s {
  bool                          non_crit_ext_present = false;
  drb_count_info_list_l         drb_count_info_list;
  counter_check_resp_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FailedLogicalChannelInfo-r15 ::= SEQUENCE
struct failed_lc_ch_info_r15_s {
  struct failed_lc_ch_id_r15_s_ {
    struct cell_group_ind_r15_opts {
      enum options { mn, sn, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<cell_group_ind_r15_opts> cell_group_ind_r15_e_;

    // member variables
    bool                  lc_ch_id_r15_present     = false;
    bool                  lc_ch_id_ext_r15_present = false;
    cell_group_ind_r15_e_ cell_group_ind_r15;
    uint8_t               lc_ch_id_r15     = 1;
    uint8_t               lc_ch_id_ext_r15 = 32;
  };
  struct fail_type_opts {
    enum options { dupl, spare3, spare2, spare1, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<fail_type_opts> fail_type_e_;

  // member variables
  failed_lc_ch_id_r15_s_ failed_lc_ch_id_r15;
  fail_type_e_           fail_type;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FailureInformation-r16-IEs ::= SEQUENCE
struct fail_info_r16_ies_s {
  struct fail_type_r16_opts {
    enum options { dupl, daps_ho_fail, spare2, spare1, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<fail_type_r16_opts> fail_type_r16_e_;

  // member variables
  bool                  failed_lc_ch_id_r16_present = false;
  bool                  fail_type_r16_present       = false;
  bool                  non_crit_ext_present        = false;
  failed_lc_ch_id_r16_s failed_lc_ch_id_r16;
  fail_type_r16_e_      fail_type_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InDeviceCoexIndication-r11-IEs ::= SEQUENCE
struct in_dev_coex_ind_r11_ies_s {
  bool                             affected_carrier_freq_list_r11_present = false;
  bool                             tdm_assist_info_r11_present            = false;
  bool                             late_non_crit_ext_present              = false;
  bool                             non_crit_ext_present                   = false;
  affected_carrier_freq_list_r11_l affected_carrier_freq_list_r11;
  tdm_assist_info_r11_c            tdm_assist_info_r11;
  dyn_octstring                    late_non_crit_ext;
  in_dev_coex_ind_v11d0_ies_s      non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqRSTDMeasurementIndication-r10-IEs ::= SEQUENCE
struct inter_freq_rstd_meas_ind_r10_ies_s {
  struct rstd_inter_freq_ind_r10_c_ {
    struct start_s_ {
      rstd_inter_freq_info_list_r10_l rstd_inter_freq_info_list_r10;
    };
    struct types_opts {
      enum options { start, stop, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    rstd_inter_freq_ind_r10_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    start_s_& start()
    {
      assert_choice_type(types::start, type_, "rstd-InterFreqIndication-r10");
      return c;
    }
    const start_s_& start() const
    {
      assert_choice_type(types::start, type_, "rstd-InterFreqIndication-r10");
      return c;
    }
    start_s_& set_start();
    void      set_stop();

  private:
    types    type_;
    start_s_ c;
  };

  // member variables
  bool                       late_non_crit_ext_present = false;
  bool                       non_crit_ext_present      = false;
  rstd_inter_freq_ind_r10_c_ rstd_inter_freq_ind_r10;
  dyn_octstring              late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMSCountingResponse-r10-IEs ::= SEQUENCE
struct mbms_count_resp_r10_ies_s {
  bool                  mbsfn_area_idx_r10_present  = false;
  bool                  count_resp_list_r10_present = false;
  bool                  late_non_crit_ext_present   = false;
  bool                  non_crit_ext_present        = false;
  uint8_t               mbsfn_area_idx_r10          = 0;
  count_resp_list_r10_l count_resp_list_r10;
  dyn_octstring         late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMSInterestIndication-r11-IEs ::= SEQUENCE
struct mbms_interest_ind_r11_ies_s {
  bool                          mbms_freq_list_r11_present = false;
  bool                          mbms_prio_r11_present      = false;
  bool                          late_non_crit_ext_present  = false;
  bool                          non_crit_ext_present       = false;
  carrier_freq_list_mbms_r11_l  mbms_freq_list_r11;
  dyn_octstring                 late_non_crit_ext;
  mbms_interest_ind_v1310_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MCGFailureInformation-r16-IEs ::= SEQUENCE
struct mcg_fail_info_r16_ies_s {
  bool                  fail_report_mcg_r16_present = false;
  bool                  late_non_crit_ext_present   = false;
  bool                  non_crit_ext_present        = false;
  fail_report_mcg_r16_s fail_report_mcg_r16;
  dyn_octstring         late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasReportAppLayer-r15-IEs ::= SEQUENCE
struct meas_report_app_layer_r15_ies_s {
  struct service_type_r15_opts {
    enum options { qoe, qoemtsi, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<service_type_r15_opts> service_type_r15_e_;

  // member variables
  bool                              meas_report_app_layer_container_r15_present = false;
  bool                              service_type_r15_present                    = false;
  bool                              non_crit_ext_present                        = false;
  bounded_octstring<1, 8000>        meas_report_app_layer_container_r15;
  service_type_r15_e_               service_type_r15;
  meas_report_app_layer_v1590_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PURConfigurationRequest-r16-IEs ::= SEQUENCE
struct pur_cfg_request_r16_ies_s {
  struct pur_cfg_request_r16_c_ {
    struct pur_setup_request_s_ {
      struct requested_num_occasions_r16_opts {
        enum options { one, infinite, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<requested_num_occasions_r16_opts> requested_num_occasions_r16_e_;
      struct requested_tbs_r16_opts {
        enum options {
          b328,
          b344,
          b376,
          b392,
          b408,
          b424,
          b440,
          b456,
          b472,
          b488,
          b504,
          b536,
          b568,
          b584,
          b616,
          b648,
          b680,
          b712,
          b744,
          b776,
          b808,
          b840,
          b872,
          b904,
          b936,
          b968,
          b1000,
          b1032,
          b1064,
          b1096,
          b1128,
          b1160,
          b1192,
          b1224,
          b1256,
          b1288,
          b1320,
          b1352,
          b1384,
          b1416,
          b1480,
          b1544,
          b1608,
          b1672,
          b1736,
          b1800,
          b1864,
          b1928,
          b1992,
          b2024,
          b2088,
          b2152,
          b2216,
          b2280,
          b2344,
          b2408,
          b2472,
          b2536,
          b2600,
          b2664,
          b2728,
          b2792,
          b2856,
          b2984,
          nulltype
        } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
      };
      typedef enumerated<requested_tbs_r16_opts> requested_tbs_r16_e_;

      // member variables
      bool                             requested_periodicity_and_offset_r16_present = false;
      bool                             rrc_ack_r16_present                          = false;
      requested_num_occasions_r16_e_   requested_num_occasions_r16;
      pur_periodicity_and_offset_r16_c requested_periodicity_and_offset_r16;
      requested_tbs_r16_e_             requested_tbs_r16;
    };
    struct types_opts {
      enum options { pur_release_request, pur_setup_request, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    pur_cfg_request_r16_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pur_setup_request_s_& pur_setup_request()
    {
      assert_choice_type(types::pur_setup_request, type_, "pur-ConfigRequest-r16");
      return c;
    }
    const pur_setup_request_s_& pur_setup_request() const
    {
      assert_choice_type(types::pur_setup_request, type_, "pur-ConfigRequest-r16");
      return c;
    }
    void                  set_pur_release_request();
    pur_setup_request_s_& set_pur_setup_request();

  private:
    types                type_;
    pur_setup_request_s_ c;
  };

  // member variables
  bool                   pur_cfg_request_r16_present = false;
  bool                   late_non_crit_ext_present   = false;
  bool                   non_crit_ext_present        = false;
  pur_cfg_request_r16_c_ pur_cfg_request_r16;
  dyn_octstring          late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ProximityIndication-r9-IEs ::= SEQUENCE
struct proximity_ind_r9_ies_s {
  struct type_r9_opts {
    enum options { entering, leaving, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<type_r9_opts> type_r9_e_;
  struct carrier_freq_r9_c_ {
    struct types_opts {
      enum options { eutra_r9, utra_r9, /*...*/ eutra2_v9e0, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts, true, 1> types;

    // choice methods
    carrier_freq_r9_c_() = default;
    carrier_freq_r9_c_(const carrier_freq_r9_c_& other);
    carrier_freq_r9_c_& operator=(const carrier_freq_r9_c_& other);
    ~carrier_freq_r9_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint32_t& eutra_r9()
    {
      assert_choice_type(types::eutra_r9, type_, "carrierFreq-r9");
      return c.get<uint32_t>();
    }
    uint16_t& utra_r9()
    {
      assert_choice_type(types::utra_r9, type_, "carrierFreq-r9");
      return c.get<uint16_t>();
    }
    uint32_t& eutra2_v9e0()
    {
      assert_choice_type(types::eutra2_v9e0, type_, "carrierFreq-r9");
      return c.get<uint32_t>();
    }
    const uint32_t& eutra_r9() const
    {
      assert_choice_type(types::eutra_r9, type_, "carrierFreq-r9");
      return c.get<uint32_t>();
    }
    const uint16_t& utra_r9() const
    {
      assert_choice_type(types::utra_r9, type_, "carrierFreq-r9");
      return c.get<uint16_t>();
    }
    const uint32_t& eutra2_v9e0() const
    {
      assert_choice_type(types::eutra2_v9e0, type_, "carrierFreq-r9");
      return c.get<uint32_t>();
    }
    uint32_t& set_eutra_r9();
    uint16_t& set_utra_r9();
    uint32_t& set_eutra2_v9e0();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool                     non_crit_ext_present = false;
  type_r9_e_               type_r9;
  carrier_freq_r9_c_       carrier_freq_r9;
  proximity_ind_v930_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RNReconfigurationComplete-r10-IEs ::= SEQUENCE
struct rn_recfg_complete_r10_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfigurationComplete-r8-IEs ::= SEQUENCE
struct rrc_conn_recfg_complete_r8_ies_s {
  bool                               non_crit_ext_present = false;
  rrc_conn_recfg_complete_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentComplete-r8-IEs ::= SEQUENCE
struct rrc_conn_reest_complete_r8_ies_s {
  bool                               non_crit_ext_present = false;
  rrc_conn_reest_complete_v920_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResumeComplete-r13-IEs ::= SEQUENCE
struct rrc_conn_resume_complete_r13_ies_s {
  struct mob_state_r13_opts {
    enum options { normal, medium, high, spare, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<mob_state_r13_opts> mob_state_r13_e_;

  // member variables
  bool                                 sel_plmn_id_r13_present                  = false;
  bool                                 ded_info_nas_r13_present                 = false;
  bool                                 rlf_info_available_r13_present           = false;
  bool                                 log_meas_available_r13_present           = false;
  bool                                 conn_est_fail_info_available_r13_present = false;
  bool                                 mob_state_r13_present                    = false;
  bool                                 mob_history_avail_r13_present            = false;
  bool                                 log_meas_available_mbsfn_r13_present     = false;
  bool                                 late_non_crit_ext_present                = false;
  bool                                 non_crit_ext_present                     = false;
  uint8_t                              sel_plmn_id_r13                          = 1;
  dyn_octstring                        ded_info_nas_r13;
  mob_state_r13_e_                     mob_state_r13;
  dyn_octstring                        late_non_crit_ext;
  rrc_conn_resume_complete_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-r8-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_r8_ies_s {
  bool                               registered_mme_present = false;
  bool                               non_crit_ext_present   = false;
  uint8_t                            sel_plmn_id            = 1;
  registered_mme_s                   registered_mme;
  dyn_octstring                      ded_info_nas;
  rrc_conn_setup_complete_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCGFailureInformation-r12-IEs ::= SEQUENCE
struct scg_fail_info_r12_ies_s {
  bool                       fail_report_scg_r12_present = false;
  bool                       non_crit_ext_present        = false;
  fail_report_scg_r12_s      fail_report_scg_r12;
  scg_fail_info_v12d0a_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCGFailureInformationNR-r15-IEs ::= SEQUENCE
struct scg_fail_info_nr_r15_ies_s {
  bool                         fail_report_scg_nr_r15_present = false;
  bool                         non_crit_ext_present           = false;
  fail_report_scg_nr_r15_s     fail_report_scg_nr_r15;
  scg_fail_info_nr_v1590_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SidelinkUEInformation-r12-IEs ::= SEQUENCE
struct sidelink_ue_info_r12_ies_s {
  bool                         comm_rx_interested_freq_r12_present = false;
  bool                         comm_tx_res_req_r12_present         = false;
  bool                         disc_rx_interest_r12_present        = false;
  bool                         disc_tx_res_req_r12_present         = false;
  bool                         late_non_crit_ext_present           = false;
  bool                         non_crit_ext_present                = false;
  uint32_t                     comm_rx_interested_freq_r12         = 0;
  sl_comm_tx_res_req_r12_s     comm_tx_res_req_r12;
  uint8_t                      disc_tx_res_req_r12 = 1;
  dyn_octstring                late_non_crit_ext;
  sidelink_ue_info_v1310_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEAssistanceInformation-r11-IEs ::= SEQUENCE
struct ueassist_info_r11_ies_s {
  struct pwr_pref_ind_r11_opts {
    enum options { normal, low_pwr_consumption, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<pwr_pref_ind_r11_opts> pwr_pref_ind_r11_e_;

  // member variables
  bool                      pwr_pref_ind_r11_present  = false;
  bool                      late_non_crit_ext_present = false;
  bool                      non_crit_ext_present      = false;
  pwr_pref_ind_r11_e_       pwr_pref_ind_r11;
  dyn_octstring             late_non_crit_ext;
  ueassist_info_v1430_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationResponse-r9-IEs ::= SEQUENCE
struct ue_info_resp_r9_ies_s {
  bool                    rach_report_r9_present = false;
  bool                    rlf_report_r9_present  = false;
  bool                    non_crit_ext_present   = false;
  rach_report_r16_s       rach_report_r9;
  rlf_report_r9_s         rlf_report_r9;
  ue_info_resp_v930_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULDedicatedMessageSegment-r16-IEs ::= SEQUENCE
struct ul_ded_msg_segment_r16_ies_s {
  struct rrc_msg_segment_type_r16_opts {
    enum options { not_last_segment, last_segment, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<rrc_msg_segment_type_r16_opts> rrc_msg_segment_type_r16_e_;

  // member variables
  bool                        late_non_crit_ext_present = false;
  bool                        non_crit_ext_present      = false;
  uint8_t                     segment_num_r16           = 0;
  dyn_octstring               rrc_msg_segment_container_r16;
  rrc_msg_segment_type_r16_e_ rrc_msg_segment_type_r16;
  dyn_octstring               late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULHandoverPreparationTransfer-r8-IEs ::= SEQUENCE
struct ul_ho_prep_transfer_r8_ies_s {
  bool                           meid_present         = false;
  bool                           non_crit_ext_present = false;
  cdma2000_type_e                cdma2000_type;
  fixed_bitstring<56>            meid;
  dyn_octstring                  ded_info;
  ul_ho_prep_transfer_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULInformationTransfer-r16-IEs ::= SEQUENCE
struct ul_info_transfer_r16_ies_s {
  struct ded_info_type_r16_c_ {
    struct types_opts {
      enum options { ded_info_nas_r16, ded_info_cdma2000_minus1_xrtt_r16, ded_info_cdma2000_hrpd_r16, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ded_info_type_r16_c_() = default;
    ded_info_type_r16_c_(const ded_info_type_r16_c_& other);
    ded_info_type_r16_c_& operator=(const ded_info_type_r16_c_& other);
    ~ded_info_type_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dyn_octstring& ded_info_nas_r16()
    {
      assert_choice_type(types::ded_info_nas_r16, type_, "dedicatedInfoType-r16");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& ded_info_cdma2000_minus1_xrtt_r16()
    {
      assert_choice_type(types::ded_info_cdma2000_minus1_xrtt_r16, type_, "dedicatedInfoType-r16");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& ded_info_cdma2000_hrpd_r16()
    {
      assert_choice_type(types::ded_info_cdma2000_hrpd_r16, type_, "dedicatedInfoType-r16");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ded_info_nas_r16() const
    {
      assert_choice_type(types::ded_info_nas_r16, type_, "dedicatedInfoType-r16");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ded_info_cdma2000_minus1_xrtt_r16() const
    {
      assert_choice_type(types::ded_info_cdma2000_minus1_xrtt_r16, type_, "dedicatedInfoType-r16");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ded_info_cdma2000_hrpd_r16() const
    {
      assert_choice_type(types::ded_info_cdma2000_hrpd_r16, type_, "dedicatedInfoType-r16");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& set_ded_info_nas_r16();
    dyn_octstring& set_ded_info_cdma2000_minus1_xrtt_r16();
    dyn_octstring& set_ded_info_cdma2000_hrpd_r16();

  private:
    types                          type_;
    choice_buffer_t<dyn_octstring> c;

    void destroy_();
  };

  // member variables
  bool                        ded_info_type_r16_present = false;
  bool                        ded_info_f1c_r16_present  = false;
  bool                        non_crit_ext_present      = false;
  ded_info_type_r16_c_        ded_info_type_r16;
  dyn_octstring               ded_info_f1c_r16;
  ul_info_transfer_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULInformationTransfer-r8-IEs ::= SEQUENCE
struct ul_info_transfer_r8_ies_s {
  struct ded_info_type_c_ {
    struct types_opts {
      enum options { ded_info_nas, ded_info_cdma2000_minus1_xrtt, ded_info_cdma2000_hrpd, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ded_info_type_c_() = default;
    ded_info_type_c_(const ded_info_type_c_& other);
    ded_info_type_c_& operator=(const ded_info_type_c_& other);
    ~ded_info_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dyn_octstring& ded_info_nas()
    {
      assert_choice_type(types::ded_info_nas, type_, "dedicatedInfoType");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& ded_info_cdma2000_minus1_xrtt()
    {
      assert_choice_type(types::ded_info_cdma2000_minus1_xrtt, type_, "dedicatedInfoType");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& ded_info_cdma2000_hrpd()
    {
      assert_choice_type(types::ded_info_cdma2000_hrpd, type_, "dedicatedInfoType");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ded_info_nas() const
    {
      assert_choice_type(types::ded_info_nas, type_, "dedicatedInfoType");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ded_info_cdma2000_minus1_xrtt() const
    {
      assert_choice_type(types::ded_info_cdma2000_minus1_xrtt, type_, "dedicatedInfoType");
      return c.get<dyn_octstring>();
    }
    const dyn_octstring& ded_info_cdma2000_hrpd() const
    {
      assert_choice_type(types::ded_info_cdma2000_hrpd, type_, "dedicatedInfoType");
      return c.get<dyn_octstring>();
    }
    dyn_octstring& set_ded_info_nas();
    dyn_octstring& set_ded_info_cdma2000_minus1_xrtt();
    dyn_octstring& set_ded_info_cdma2000_hrpd();

  private:
    types                          type_;
    choice_buffer_t<dyn_octstring> c;

    void destroy_();
  };

  // member variables
  bool                        non_crit_ext_present = false;
  ded_info_type_c_            ded_info_type;
  ul_info_transfer_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULInformationTransferIRAT-r16-IEs ::= SEQUENCE
struct ul_info_transfer_irat_r16_ies_s {
  bool          ul_dcch_msg_nr_r16_present = false;
  bool          late_non_crit_ext_present  = false;
  bool          non_crit_ext_present       = false;
  dyn_octstring ul_dcch_msg_nr_r16;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULInformationTransferMRDC-r15-IEs ::= SEQUENCE
struct ul_info_transfer_mrdc_r15_ies_s {
  bool          ul_dcch_msg_nr_r15_present = false;
  bool          late_non_crit_ext_present  = false;
  bool          non_crit_ext_present       = false;
  dyn_octstring ul_dcch_msg_nr_r15;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WLANConnectionStatusReport-r13-IEs ::= SEQUENCE
struct wlan_conn_status_report_r13_ies_s {
  bool                                late_non_crit_ext_present = false;
  bool                                non_crit_ext_present      = false;
  wlan_status_r13_e                   wlan_status_r13;
  dyn_octstring                       late_non_crit_ext;
  wlan_conn_status_report_v1430_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSFBParametersRequestCDMA2000 ::= SEQUENCE
struct csfb_params_request_cdma2000_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { csfb_params_request_cdma2000_r8, crit_exts_future, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    csfb_params_request_cdma2000_r8_ies_s& csfb_params_request_cdma2000_r8()
    {
      assert_choice_type(types::csfb_params_request_cdma2000_r8, type_, "criticalExtensions");
      return c;
    }
    const csfb_params_request_cdma2000_r8_ies_s& csfb_params_request_cdma2000_r8() const
    {
      assert_choice_type(types::csfb_params_request_cdma2000_r8, type_, "criticalExtensions");
      return c;
    }
    csfb_params_request_cdma2000_r8_ies_s& set_csfb_params_request_cdma2000_r8();
    void                                   set_crit_exts_future();

  private:
    types                                 type_;
    csfb_params_request_cdma2000_r8_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CounterCheckResponse ::= SEQUENCE
struct counter_check_resp_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { counter_check_resp_r8, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    counter_check_resp_r8_ies_s& counter_check_resp_r8()
    {
      assert_choice_type(types::counter_check_resp_r8, type_, "criticalExtensions");
      return c;
    }
    const counter_check_resp_r8_ies_s& counter_check_resp_r8() const
    {
      assert_choice_type(types::counter_check_resp_r8, type_, "criticalExtensions");
      return c;
    }
    counter_check_resp_r8_ies_s& set_counter_check_resp_r8();
    void                         set_crit_exts_future();

  private:
    types                       type_;
    counter_check_resp_r8_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FailureInformation-r15 ::= SEQUENCE
struct fail_info_r15_s {
  bool                    failed_lc_ch_info_r15_present = false;
  failed_lc_ch_info_r15_s failed_lc_ch_info_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FailureInformation-r16 ::= SEQUENCE
struct fail_info_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { fail_info_r16, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    fail_info_r16_ies_s& fail_info_r16()
    {
      assert_choice_type(types::fail_info_r16, type_, "criticalExtensions");
      return c;
    }
    const fail_info_r16_ies_s& fail_info_r16() const
    {
      assert_choice_type(types::fail_info_r16, type_, "criticalExtensions");
      return c;
    }
    fail_info_r16_ies_s& set_fail_info_r16();
    void                 set_crit_exts_future();

  private:
    types               type_;
    fail_info_r16_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InDeviceCoexIndication-r11 ::= SEQUENCE
struct in_dev_coex_ind_r11_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { in_dev_coex_ind_r11, spare3, spare2, spare1, nulltype } value;

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
      in_dev_coex_ind_r11_ies_s& in_dev_coex_ind_r11()
      {
        assert_choice_type(types::in_dev_coex_ind_r11, type_, "c1");
        return c;
      }
      const in_dev_coex_ind_r11_ies_s& in_dev_coex_ind_r11() const
      {
        assert_choice_type(types::in_dev_coex_ind_r11, type_, "c1");
        return c;
      }
      in_dev_coex_ind_r11_ies_s& set_in_dev_coex_ind_r11();
      void                       set_spare3();
      void                       set_spare2();
      void                       set_spare1();

    private:
      types                     type_;
      in_dev_coex_ind_r11_ies_s c;
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

// InterFreqRSTDMeasurementIndication-r10 ::= SEQUENCE
struct inter_freq_rstd_meas_ind_r10_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { inter_freq_rstd_meas_ind_r10, spare3, spare2, spare1, nulltype } value;

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
      inter_freq_rstd_meas_ind_r10_ies_s& inter_freq_rstd_meas_ind_r10()
      {
        assert_choice_type(types::inter_freq_rstd_meas_ind_r10, type_, "c1");
        return c;
      }
      const inter_freq_rstd_meas_ind_r10_ies_s& inter_freq_rstd_meas_ind_r10() const
      {
        assert_choice_type(types::inter_freq_rstd_meas_ind_r10, type_, "c1");
        return c;
      }
      inter_freq_rstd_meas_ind_r10_ies_s& set_inter_freq_rstd_meas_ind_r10();
      void                                set_spare3();
      void                                set_spare2();
      void                                set_spare1();

    private:
      types                              type_;
      inter_freq_rstd_meas_ind_r10_ies_s c;
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

// MBMSCountingResponse-r10 ::= SEQUENCE
struct mbms_count_resp_r10_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { count_resp_r10, spare3, spare2, spare1, nulltype } value;

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
      mbms_count_resp_r10_ies_s& count_resp_r10()
      {
        assert_choice_type(types::count_resp_r10, type_, "c1");
        return c;
      }
      const mbms_count_resp_r10_ies_s& count_resp_r10() const
      {
        assert_choice_type(types::count_resp_r10, type_, "c1");
        return c;
      }
      mbms_count_resp_r10_ies_s& set_count_resp_r10();
      void                       set_spare3();
      void                       set_spare2();
      void                       set_spare1();

    private:
      types                     type_;
      mbms_count_resp_r10_ies_s c;
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

// MBMSInterestIndication-r11 ::= SEQUENCE
struct mbms_interest_ind_r11_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { interest_ind_r11, spare3, spare2, spare1, nulltype } value;

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
      mbms_interest_ind_r11_ies_s& interest_ind_r11()
      {
        assert_choice_type(types::interest_ind_r11, type_, "c1");
        return c;
      }
      const mbms_interest_ind_r11_ies_s& interest_ind_r11() const
      {
        assert_choice_type(types::interest_ind_r11, type_, "c1");
        return c;
      }
      mbms_interest_ind_r11_ies_s& set_interest_ind_r11();
      void                         set_spare3();
      void                         set_spare2();
      void                         set_spare1();

    private:
      types                       type_;
      mbms_interest_ind_r11_ies_s c;
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

// MCGFailureInformation-r16 ::= SEQUENCE
struct mcg_fail_info_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { mcg_fail_info, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mcg_fail_info_r16_ies_s& mcg_fail_info()
    {
      assert_choice_type(types::mcg_fail_info, type_, "criticalExtensions");
      return c;
    }
    const mcg_fail_info_r16_ies_s& mcg_fail_info() const
    {
      assert_choice_type(types::mcg_fail_info, type_, "criticalExtensions");
      return c;
    }
    mcg_fail_info_r16_ies_s& set_mcg_fail_info();
    void                     set_crit_exts_future();

  private:
    types                   type_;
    mcg_fail_info_r16_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasReportAppLayer-r15 ::= SEQUENCE
struct meas_report_app_layer_r15_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { meas_report_app_layer_r15, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    meas_report_app_layer_r15_ies_s& meas_report_app_layer_r15()
    {
      assert_choice_type(types::meas_report_app_layer_r15, type_, "criticalExtensions");
      return c;
    }
    const meas_report_app_layer_r15_ies_s& meas_report_app_layer_r15() const
    {
      assert_choice_type(types::meas_report_app_layer_r15, type_, "criticalExtensions");
      return c;
    }
    meas_report_app_layer_r15_ies_s& set_meas_report_app_layer_r15();
    void                             set_crit_exts_future();

  private:
    types                           type_;
    meas_report_app_layer_r15_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PURConfigurationRequest-r16 ::= SEQUENCE
struct pur_cfg_request_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { pur_cfg_request, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    pur_cfg_request_r16_ies_s& pur_cfg_request()
    {
      assert_choice_type(types::pur_cfg_request, type_, "criticalExtensions");
      return c;
    }
    const pur_cfg_request_r16_ies_s& pur_cfg_request() const
    {
      assert_choice_type(types::pur_cfg_request, type_, "criticalExtensions");
      return c;
    }
    pur_cfg_request_r16_ies_s& set_pur_cfg_request();
    void                       set_crit_exts_future();

  private:
    types                     type_;
    pur_cfg_request_r16_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ProximityIndication-r9 ::= SEQUENCE
struct proximity_ind_r9_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { proximity_ind_r9, spare3, spare2, spare1, nulltype } value;

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
      proximity_ind_r9_ies_s& proximity_ind_r9()
      {
        assert_choice_type(types::proximity_ind_r9, type_, "c1");
        return c;
      }
      const proximity_ind_r9_ies_s& proximity_ind_r9() const
      {
        assert_choice_type(types::proximity_ind_r9, type_, "c1");
        return c;
      }
      proximity_ind_r9_ies_s& set_proximity_ind_r9();
      void                    set_spare3();
      void                    set_spare2();
      void                    set_spare1();

    private:
      types                  type_;
      proximity_ind_r9_ies_s c;
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

// RNReconfigurationComplete-r10 ::= SEQUENCE
struct rn_recfg_complete_r10_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { rn_recfg_complete_r10, spare3, spare2, spare1, nulltype } value;

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
      rn_recfg_complete_r10_ies_s& rn_recfg_complete_r10()
      {
        assert_choice_type(types::rn_recfg_complete_r10, type_, "c1");
        return c;
      }
      const rn_recfg_complete_r10_ies_s& rn_recfg_complete_r10() const
      {
        assert_choice_type(types::rn_recfg_complete_r10, type_, "c1");
        return c;
      }
      rn_recfg_complete_r10_ies_s& set_rn_recfg_complete_r10();
      void                         set_spare3();
      void                         set_spare2();
      void                         set_spare1();

    private:
      types                       type_;
      rn_recfg_complete_r10_ies_s c;
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
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfigurationComplete ::= SEQUENCE
struct rrc_conn_recfg_complete_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_conn_recfg_complete_r8, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_conn_recfg_complete_r8_ies_s& rrc_conn_recfg_complete_r8()
    {
      assert_choice_type(types::rrc_conn_recfg_complete_r8, type_, "criticalExtensions");
      return c;
    }
    const rrc_conn_recfg_complete_r8_ies_s& rrc_conn_recfg_complete_r8() const
    {
      assert_choice_type(types::rrc_conn_recfg_complete_r8, type_, "criticalExtensions");
      return c;
    }
    rrc_conn_recfg_complete_r8_ies_s& set_rrc_conn_recfg_complete_r8();
    void                              set_crit_exts_future();

  private:
    types                            type_;
    rrc_conn_recfg_complete_r8_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentComplete ::= SEQUENCE
struct rrc_conn_reest_complete_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_conn_reest_complete_r8, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_conn_reest_complete_r8_ies_s& rrc_conn_reest_complete_r8()
    {
      assert_choice_type(types::rrc_conn_reest_complete_r8, type_, "criticalExtensions");
      return c;
    }
    const rrc_conn_reest_complete_r8_ies_s& rrc_conn_reest_complete_r8() const
    {
      assert_choice_type(types::rrc_conn_reest_complete_r8, type_, "criticalExtensions");
      return c;
    }
    rrc_conn_reest_complete_r8_ies_s& set_rrc_conn_reest_complete_r8();
    void                              set_crit_exts_future();

  private:
    types                            type_;
    rrc_conn_reest_complete_r8_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResumeComplete-r13 ::= SEQUENCE
struct rrc_conn_resume_complete_r13_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_conn_resume_complete_r13, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_conn_resume_complete_r13_ies_s& rrc_conn_resume_complete_r13()
    {
      assert_choice_type(types::rrc_conn_resume_complete_r13, type_, "criticalExtensions");
      return c;
    }
    const rrc_conn_resume_complete_r13_ies_s& rrc_conn_resume_complete_r13() const
    {
      assert_choice_type(types::rrc_conn_resume_complete_r13, type_, "criticalExtensions");
      return c;
    }
    rrc_conn_resume_complete_r13_ies_s& set_rrc_conn_resume_complete_r13();
    void                                set_crit_exts_future();

  private:
    types                              type_;
    rrc_conn_resume_complete_r13_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete ::= SEQUENCE
struct rrc_conn_setup_complete_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { rrc_conn_setup_complete_r8, spare3, spare2, spare1, nulltype } value;

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
      rrc_conn_setup_complete_r8_ies_s& rrc_conn_setup_complete_r8()
      {
        assert_choice_type(types::rrc_conn_setup_complete_r8, type_, "c1");
        return c;
      }
      const rrc_conn_setup_complete_r8_ies_s& rrc_conn_setup_complete_r8() const
      {
        assert_choice_type(types::rrc_conn_setup_complete_r8, type_, "c1");
        return c;
      }
      rrc_conn_setup_complete_r8_ies_s& set_rrc_conn_setup_complete_r8();
      void                              set_spare3();
      void                              set_spare2();
      void                              set_spare1();

    private:
      types                            type_;
      rrc_conn_setup_complete_r8_ies_s c;
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
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCGFailureInformation-r12 ::= SEQUENCE
struct scg_fail_info_r12_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { scg_fail_info_r12, spare3, spare2, spare1, nulltype } value;

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
      scg_fail_info_r12_ies_s& scg_fail_info_r12()
      {
        assert_choice_type(types::scg_fail_info_r12, type_, "c1");
        return c;
      }
      const scg_fail_info_r12_ies_s& scg_fail_info_r12() const
      {
        assert_choice_type(types::scg_fail_info_r12, type_, "c1");
        return c;
      }
      scg_fail_info_r12_ies_s& set_scg_fail_info_r12();
      void                     set_spare3();
      void                     set_spare2();
      void                     set_spare1();

    private:
      types                   type_;
      scg_fail_info_r12_ies_s c;
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

// SCGFailureInformationNR-r15 ::= SEQUENCE
struct scg_fail_info_nr_r15_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { scg_fail_info_nr_r15, spare3, spare2, spare1, nulltype } value;

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
      scg_fail_info_nr_r15_ies_s& scg_fail_info_nr_r15()
      {
        assert_choice_type(types::scg_fail_info_nr_r15, type_, "c1");
        return c;
      }
      const scg_fail_info_nr_r15_ies_s& scg_fail_info_nr_r15() const
      {
        assert_choice_type(types::scg_fail_info_nr_r15, type_, "c1");
        return c;
      }
      scg_fail_info_nr_r15_ies_s& set_scg_fail_info_nr_r15();
      void                        set_spare3();
      void                        set_spare2();
      void                        set_spare1();

    private:
      types                      type_;
      scg_fail_info_nr_r15_ies_s c;
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

// SidelinkUEInformation-r12 ::= SEQUENCE
struct sidelink_ue_info_r12_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { sidelink_ue_info_r12, spare3, spare2, spare1, nulltype } value;

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
      sidelink_ue_info_r12_ies_s& sidelink_ue_info_r12()
      {
        assert_choice_type(types::sidelink_ue_info_r12, type_, "c1");
        return c;
      }
      const sidelink_ue_info_r12_ies_s& sidelink_ue_info_r12() const
      {
        assert_choice_type(types::sidelink_ue_info_r12, type_, "c1");
        return c;
      }
      sidelink_ue_info_r12_ies_s& set_sidelink_ue_info_r12();
      void                        set_spare3();
      void                        set_spare2();
      void                        set_spare1();

    private:
      types                      type_;
      sidelink_ue_info_r12_ies_s c;
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

// UEAssistanceInformation-r11 ::= SEQUENCE
struct ueassist_info_r11_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ue_assist_info_r11, spare3, spare2, spare1, nulltype } value;

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
      ueassist_info_r11_ies_s& ue_assist_info_r11()
      {
        assert_choice_type(types::ue_assist_info_r11, type_, "c1");
        return c;
      }
      const ueassist_info_r11_ies_s& ue_assist_info_r11() const
      {
        assert_choice_type(types::ue_assist_info_r11, type_, "c1");
        return c;
      }
      ueassist_info_r11_ies_s& set_ue_assist_info_r11();
      void                     set_spare3();
      void                     set_spare2();
      void                     set_spare1();

    private:
      types                   type_;
      ueassist_info_r11_ies_s c;
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

// UEInformationResponse-r9 ::= SEQUENCE
struct ue_info_resp_r9_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ue_info_resp_r9, spare3, spare2, spare1, nulltype } value;

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
      ue_info_resp_r9_ies_s& ue_info_resp_r9()
      {
        assert_choice_type(types::ue_info_resp_r9, type_, "c1");
        return c;
      }
      const ue_info_resp_r9_ies_s& ue_info_resp_r9() const
      {
        assert_choice_type(types::ue_info_resp_r9, type_, "c1");
        return c;
      }
      ue_info_resp_r9_ies_s& set_ue_info_resp_r9();
      void                   set_spare3();
      void                   set_spare2();
      void                   set_spare1();

    private:
      types                 type_;
      ue_info_resp_r9_ies_s c;
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
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULDedicatedMessageSegment-r16 ::= SEQUENCE
struct ul_ded_msg_segment_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ul_ded_msg_segment_r16, crit_exts_future, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    crit_exts_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ul_ded_msg_segment_r16_ies_s& ul_ded_msg_segment_r16()
    {
      assert_choice_type(types::ul_ded_msg_segment_r16, type_, "criticalExtensions");
      return c;
    }
    const ul_ded_msg_segment_r16_ies_s& ul_ded_msg_segment_r16() const
    {
      assert_choice_type(types::ul_ded_msg_segment_r16, type_, "criticalExtensions");
      return c;
    }
    ul_ded_msg_segment_r16_ies_s& set_ul_ded_msg_segment_r16();
    void                          set_crit_exts_future();

  private:
    types                        type_;
    ul_ded_msg_segment_r16_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULHandoverPreparationTransfer ::= SEQUENCE
struct ul_ho_prep_transfer_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ul_ho_prep_transfer_r8, spare3, spare2, spare1, nulltype } value;

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
      ul_ho_prep_transfer_r8_ies_s& ul_ho_prep_transfer_r8()
      {
        assert_choice_type(types::ul_ho_prep_transfer_r8, type_, "c1");
        return c;
      }
      const ul_ho_prep_transfer_r8_ies_s& ul_ho_prep_transfer_r8() const
      {
        assert_choice_type(types::ul_ho_prep_transfer_r8, type_, "c1");
        return c;
      }
      ul_ho_prep_transfer_r8_ies_s& set_ul_ho_prep_transfer_r8();
      void                          set_spare3();
      void                          set_spare2();
      void                          set_spare1();

    private:
      types                        type_;
      ul_ho_prep_transfer_r8_ies_s c;
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

// ULInformationTransfer ::= SEQUENCE
struct ul_info_transfer_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ul_info_transfer_r8, ul_info_transfer_r16, spare2, spare1, nulltype } value;

        const char* to_string() const;
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
      ul_info_transfer_r8_ies_s& ul_info_transfer_r8()
      {
        assert_choice_type(types::ul_info_transfer_r8, type_, "c1");
        return c.get<ul_info_transfer_r8_ies_s>();
      }
      ul_info_transfer_r16_ies_s& ul_info_transfer_r16()
      {
        assert_choice_type(types::ul_info_transfer_r16, type_, "c1");
        return c.get<ul_info_transfer_r16_ies_s>();
      }
      const ul_info_transfer_r8_ies_s& ul_info_transfer_r8() const
      {
        assert_choice_type(types::ul_info_transfer_r8, type_, "c1");
        return c.get<ul_info_transfer_r8_ies_s>();
      }
      const ul_info_transfer_r16_ies_s& ul_info_transfer_r16() const
      {
        assert_choice_type(types::ul_info_transfer_r16, type_, "c1");
        return c.get<ul_info_transfer_r16_ies_s>();
      }
      ul_info_transfer_r8_ies_s&  set_ul_info_transfer_r8();
      ul_info_transfer_r16_ies_s& set_ul_info_transfer_r16();
      void                        set_spare2();
      void                        set_spare1();

    private:
      types                                                                  type_;
      choice_buffer_t<ul_info_transfer_r16_ies_s, ul_info_transfer_r8_ies_s> c;

      void destroy_();
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

// ULInformationTransferIRAT-r16 ::= SEQUENCE
struct ul_info_transfer_irat_r16_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ul_info_transfer_irat_r16, spare3, spare2, spare1, nulltype } value;

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
      ul_info_transfer_irat_r16_ies_s& ul_info_transfer_irat_r16()
      {
        assert_choice_type(types::ul_info_transfer_irat_r16, type_, "c1");
        return c;
      }
      const ul_info_transfer_irat_r16_ies_s& ul_info_transfer_irat_r16() const
      {
        assert_choice_type(types::ul_info_transfer_irat_r16, type_, "c1");
        return c;
      }
      ul_info_transfer_irat_r16_ies_s& set_ul_info_transfer_irat_r16();
      void                             set_spare3();
      void                             set_spare2();
      void                             set_spare1();

    private:
      types                           type_;
      ul_info_transfer_irat_r16_ies_s c;
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

// ULInformationTransferMRDC-r15 ::= SEQUENCE
struct ul_info_transfer_mrdc_r15_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ul_info_transfer_mrdc_r15, spare3, spare2, spare1, nulltype } value;

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
      ul_info_transfer_mrdc_r15_ies_s& ul_info_transfer_mrdc_r15()
      {
        assert_choice_type(types::ul_info_transfer_mrdc_r15, type_, "c1");
        return c;
      }
      const ul_info_transfer_mrdc_r15_ies_s& ul_info_transfer_mrdc_r15() const
      {
        assert_choice_type(types::ul_info_transfer_mrdc_r15, type_, "c1");
        return c;
      }
      ul_info_transfer_mrdc_r15_ies_s& set_ul_info_transfer_mrdc_r15();
      void                             set_spare3();
      void                             set_spare2();
      void                             set_spare1();

    private:
      types                           type_;
      ul_info_transfer_mrdc_r15_ies_s c;
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

// WLANConnectionStatusReport-r13 ::= SEQUENCE
struct wlan_conn_status_report_r13_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { wlan_conn_status_report_r13, spare3, spare2, spare1, nulltype } value;

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
      wlan_conn_status_report_r13_ies_s& wlan_conn_status_report_r13()
      {
        assert_choice_type(types::wlan_conn_status_report_r13, type_, "c1");
        return c;
      }
      const wlan_conn_status_report_r13_ies_s& wlan_conn_status_report_r13() const
      {
        assert_choice_type(types::wlan_conn_status_report_r13, type_, "c1");
        return c;
      }
      wlan_conn_status_report_r13_ies_s& set_wlan_conn_status_report_r13();
      void                               set_spare3();
      void                               set_spare2();
      void                               set_spare1();

    private:
      types                             type_;
      wlan_conn_status_report_r13_ies_s c;
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

// UL-DCCH-MessageType ::= CHOICE
struct ul_dcch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options {
        csfb_params_request_cdma2000,
        meas_report,
        rrc_conn_recfg_complete,
        rrc_conn_reest_complete,
        rrc_conn_setup_complete,
        security_mode_complete,
        security_mode_fail,
        ue_cap_info,
        ul_ho_prep_transfer,
        ul_info_transfer,
        counter_check_resp,
        ue_info_resp_r9,
        proximity_ind_r9,
        rn_recfg_complete_r10,
        mbms_count_resp_r10,
        inter_freq_rstd_meas_ind_r10,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
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
    csfb_params_request_cdma2000_s& csfb_params_request_cdma2000()
    {
      assert_choice_type(types::csfb_params_request_cdma2000, type_, "c1");
      return c.get<csfb_params_request_cdma2000_s>();
    }
    meas_report_s& meas_report()
    {
      assert_choice_type(types::meas_report, type_, "c1");
      return c.get<meas_report_s>();
    }
    rrc_conn_recfg_complete_s& rrc_conn_recfg_complete()
    {
      assert_choice_type(types::rrc_conn_recfg_complete, type_, "c1");
      return c.get<rrc_conn_recfg_complete_s>();
    }
    rrc_conn_reest_complete_s& rrc_conn_reest_complete()
    {
      assert_choice_type(types::rrc_conn_reest_complete, type_, "c1");
      return c.get<rrc_conn_reest_complete_s>();
    }
    rrc_conn_setup_complete_s& rrc_conn_setup_complete()
    {
      assert_choice_type(types::rrc_conn_setup_complete, type_, "c1");
      return c.get<rrc_conn_setup_complete_s>();
    }
    security_mode_complete_s& security_mode_complete()
    {
      assert_choice_type(types::security_mode_complete, type_, "c1");
      return c.get<security_mode_complete_s>();
    }
    security_mode_fail_s& security_mode_fail()
    {
      assert_choice_type(types::security_mode_fail, type_, "c1");
      return c.get<security_mode_fail_s>();
    }
    ue_cap_info_s& ue_cap_info()
    {
      assert_choice_type(types::ue_cap_info, type_, "c1");
      return c.get<ue_cap_info_s>();
    }
    ul_ho_prep_transfer_s& ul_ho_prep_transfer()
    {
      assert_choice_type(types::ul_ho_prep_transfer, type_, "c1");
      return c.get<ul_ho_prep_transfer_s>();
    }
    ul_info_transfer_s& ul_info_transfer()
    {
      assert_choice_type(types::ul_info_transfer, type_, "c1");
      return c.get<ul_info_transfer_s>();
    }
    counter_check_resp_s& counter_check_resp()
    {
      assert_choice_type(types::counter_check_resp, type_, "c1");
      return c.get<counter_check_resp_s>();
    }
    ue_info_resp_r9_s& ue_info_resp_r9()
    {
      assert_choice_type(types::ue_info_resp_r9, type_, "c1");
      return c.get<ue_info_resp_r9_s>();
    }
    proximity_ind_r9_s& proximity_ind_r9()
    {
      assert_choice_type(types::proximity_ind_r9, type_, "c1");
      return c.get<proximity_ind_r9_s>();
    }
    rn_recfg_complete_r10_s& rn_recfg_complete_r10()
    {
      assert_choice_type(types::rn_recfg_complete_r10, type_, "c1");
      return c.get<rn_recfg_complete_r10_s>();
    }
    mbms_count_resp_r10_s& mbms_count_resp_r10()
    {
      assert_choice_type(types::mbms_count_resp_r10, type_, "c1");
      return c.get<mbms_count_resp_r10_s>();
    }
    inter_freq_rstd_meas_ind_r10_s& inter_freq_rstd_meas_ind_r10()
    {
      assert_choice_type(types::inter_freq_rstd_meas_ind_r10, type_, "c1");
      return c.get<inter_freq_rstd_meas_ind_r10_s>();
    }
    const csfb_params_request_cdma2000_s& csfb_params_request_cdma2000() const
    {
      assert_choice_type(types::csfb_params_request_cdma2000, type_, "c1");
      return c.get<csfb_params_request_cdma2000_s>();
    }
    const meas_report_s& meas_report() const
    {
      assert_choice_type(types::meas_report, type_, "c1");
      return c.get<meas_report_s>();
    }
    const rrc_conn_recfg_complete_s& rrc_conn_recfg_complete() const
    {
      assert_choice_type(types::rrc_conn_recfg_complete, type_, "c1");
      return c.get<rrc_conn_recfg_complete_s>();
    }
    const rrc_conn_reest_complete_s& rrc_conn_reest_complete() const
    {
      assert_choice_type(types::rrc_conn_reest_complete, type_, "c1");
      return c.get<rrc_conn_reest_complete_s>();
    }
    const rrc_conn_setup_complete_s& rrc_conn_setup_complete() const
    {
      assert_choice_type(types::rrc_conn_setup_complete, type_, "c1");
      return c.get<rrc_conn_setup_complete_s>();
    }
    const security_mode_complete_s& security_mode_complete() const
    {
      assert_choice_type(types::security_mode_complete, type_, "c1");
      return c.get<security_mode_complete_s>();
    }
    const security_mode_fail_s& security_mode_fail() const
    {
      assert_choice_type(types::security_mode_fail, type_, "c1");
      return c.get<security_mode_fail_s>();
    }
    const ue_cap_info_s& ue_cap_info() const
    {
      assert_choice_type(types::ue_cap_info, type_, "c1");
      return c.get<ue_cap_info_s>();
    }
    const ul_ho_prep_transfer_s& ul_ho_prep_transfer() const
    {
      assert_choice_type(types::ul_ho_prep_transfer, type_, "c1");
      return c.get<ul_ho_prep_transfer_s>();
    }
    const ul_info_transfer_s& ul_info_transfer() const
    {
      assert_choice_type(types::ul_info_transfer, type_, "c1");
      return c.get<ul_info_transfer_s>();
    }
    const counter_check_resp_s& counter_check_resp() const
    {
      assert_choice_type(types::counter_check_resp, type_, "c1");
      return c.get<counter_check_resp_s>();
    }
    const ue_info_resp_r9_s& ue_info_resp_r9() const
    {
      assert_choice_type(types::ue_info_resp_r9, type_, "c1");
      return c.get<ue_info_resp_r9_s>();
    }
    const proximity_ind_r9_s& proximity_ind_r9() const
    {
      assert_choice_type(types::proximity_ind_r9, type_, "c1");
      return c.get<proximity_ind_r9_s>();
    }
    const rn_recfg_complete_r10_s& rn_recfg_complete_r10() const
    {
      assert_choice_type(types::rn_recfg_complete_r10, type_, "c1");
      return c.get<rn_recfg_complete_r10_s>();
    }
    const mbms_count_resp_r10_s& mbms_count_resp_r10() const
    {
      assert_choice_type(types::mbms_count_resp_r10, type_, "c1");
      return c.get<mbms_count_resp_r10_s>();
    }
    const inter_freq_rstd_meas_ind_r10_s& inter_freq_rstd_meas_ind_r10() const
    {
      assert_choice_type(types::inter_freq_rstd_meas_ind_r10, type_, "c1");
      return c.get<inter_freq_rstd_meas_ind_r10_s>();
    }
    csfb_params_request_cdma2000_s& set_csfb_params_request_cdma2000();
    meas_report_s&                  set_meas_report();
    rrc_conn_recfg_complete_s&      set_rrc_conn_recfg_complete();
    rrc_conn_reest_complete_s&      set_rrc_conn_reest_complete();
    rrc_conn_setup_complete_s&      set_rrc_conn_setup_complete();
    security_mode_complete_s&       set_security_mode_complete();
    security_mode_fail_s&           set_security_mode_fail();
    ue_cap_info_s&                  set_ue_cap_info();
    ul_ho_prep_transfer_s&          set_ul_ho_prep_transfer();
    ul_info_transfer_s&             set_ul_info_transfer();
    counter_check_resp_s&           set_counter_check_resp();
    ue_info_resp_r9_s&              set_ue_info_resp_r9();
    proximity_ind_r9_s&             set_proximity_ind_r9();
    rn_recfg_complete_r10_s&        set_rn_recfg_complete_r10();
    mbms_count_resp_r10_s&          set_mbms_count_resp_r10();
    inter_freq_rstd_meas_ind_r10_s& set_inter_freq_rstd_meas_ind_r10();

  private:
    types type_;
    choice_buffer_t<counter_check_resp_s,
                    csfb_params_request_cdma2000_s,
                    inter_freq_rstd_meas_ind_r10_s,
                    mbms_count_resp_r10_s,
                    meas_report_s,
                    proximity_ind_r9_s,
                    rn_recfg_complete_r10_s,
                    rrc_conn_recfg_complete_s,
                    rrc_conn_reest_complete_s,
                    rrc_conn_setup_complete_s,
                    security_mode_complete_s,
                    security_mode_fail_s,
                    ue_cap_info_s,
                    ue_info_resp_r9_s,
                    ul_ho_prep_transfer_s,
                    ul_info_transfer_s>
        c;

    void destroy_();
  };
  struct msg_class_ext_c_ {
    struct c2_c_ {
      struct types_opts {
        enum options {
          ue_assist_info_r11,
          in_dev_coex_ind_r11,
          mbms_interest_ind_r11,
          scg_fail_info_r12,
          sidelink_ue_info_r12,
          wlan_conn_status_report_r13,
          rrc_conn_resume_complete_r13,
          ul_info_transfer_mrdc_r15,
          scg_fail_info_nr_r15,
          meas_report_app_layer_r15,
          fail_info_r15,
          ul_ded_msg_segment_r16,
          pur_cfg_request_r16,
          fail_info_r16,
          mcg_fail_info_r16,
          ul_info_transfer_irat_r16,
          nulltype
        } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      c2_c_() = default;
      c2_c_(const c2_c_& other);
      c2_c_& operator=(const c2_c_& other);
      ~c2_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      ueassist_info_r11_s& ue_assist_info_r11()
      {
        assert_choice_type(types::ue_assist_info_r11, type_, "c2");
        return c.get<ueassist_info_r11_s>();
      }
      in_dev_coex_ind_r11_s& in_dev_coex_ind_r11()
      {
        assert_choice_type(types::in_dev_coex_ind_r11, type_, "c2");
        return c.get<in_dev_coex_ind_r11_s>();
      }
      mbms_interest_ind_r11_s& mbms_interest_ind_r11()
      {
        assert_choice_type(types::mbms_interest_ind_r11, type_, "c2");
        return c.get<mbms_interest_ind_r11_s>();
      }
      scg_fail_info_r12_s& scg_fail_info_r12()
      {
        assert_choice_type(types::scg_fail_info_r12, type_, "c2");
        return c.get<scg_fail_info_r12_s>();
      }
      sidelink_ue_info_r12_s& sidelink_ue_info_r12()
      {
        assert_choice_type(types::sidelink_ue_info_r12, type_, "c2");
        return c.get<sidelink_ue_info_r12_s>();
      }
      wlan_conn_status_report_r13_s& wlan_conn_status_report_r13()
      {
        assert_choice_type(types::wlan_conn_status_report_r13, type_, "c2");
        return c.get<wlan_conn_status_report_r13_s>();
      }
      rrc_conn_resume_complete_r13_s& rrc_conn_resume_complete_r13()
      {
        assert_choice_type(types::rrc_conn_resume_complete_r13, type_, "c2");
        return c.get<rrc_conn_resume_complete_r13_s>();
      }
      ul_info_transfer_mrdc_r15_s& ul_info_transfer_mrdc_r15()
      {
        assert_choice_type(types::ul_info_transfer_mrdc_r15, type_, "c2");
        return c.get<ul_info_transfer_mrdc_r15_s>();
      }
      scg_fail_info_nr_r15_s& scg_fail_info_nr_r15()
      {
        assert_choice_type(types::scg_fail_info_nr_r15, type_, "c2");
        return c.get<scg_fail_info_nr_r15_s>();
      }
      meas_report_app_layer_r15_s& meas_report_app_layer_r15()
      {
        assert_choice_type(types::meas_report_app_layer_r15, type_, "c2");
        return c.get<meas_report_app_layer_r15_s>();
      }
      fail_info_r15_s& fail_info_r15()
      {
        assert_choice_type(types::fail_info_r15, type_, "c2");
        return c.get<fail_info_r15_s>();
      }
      ul_ded_msg_segment_r16_s& ul_ded_msg_segment_r16()
      {
        assert_choice_type(types::ul_ded_msg_segment_r16, type_, "c2");
        return c.get<ul_ded_msg_segment_r16_s>();
      }
      pur_cfg_request_r16_s& pur_cfg_request_r16()
      {
        assert_choice_type(types::pur_cfg_request_r16, type_, "c2");
        return c.get<pur_cfg_request_r16_s>();
      }
      fail_info_r16_s& fail_info_r16()
      {
        assert_choice_type(types::fail_info_r16, type_, "c2");
        return c.get<fail_info_r16_s>();
      }
      mcg_fail_info_r16_s& mcg_fail_info_r16()
      {
        assert_choice_type(types::mcg_fail_info_r16, type_, "c2");
        return c.get<mcg_fail_info_r16_s>();
      }
      ul_info_transfer_irat_r16_s& ul_info_transfer_irat_r16()
      {
        assert_choice_type(types::ul_info_transfer_irat_r16, type_, "c2");
        return c.get<ul_info_transfer_irat_r16_s>();
      }
      const ueassist_info_r11_s& ue_assist_info_r11() const
      {
        assert_choice_type(types::ue_assist_info_r11, type_, "c2");
        return c.get<ueassist_info_r11_s>();
      }
      const in_dev_coex_ind_r11_s& in_dev_coex_ind_r11() const
      {
        assert_choice_type(types::in_dev_coex_ind_r11, type_, "c2");
        return c.get<in_dev_coex_ind_r11_s>();
      }
      const mbms_interest_ind_r11_s& mbms_interest_ind_r11() const
      {
        assert_choice_type(types::mbms_interest_ind_r11, type_, "c2");
        return c.get<mbms_interest_ind_r11_s>();
      }
      const scg_fail_info_r12_s& scg_fail_info_r12() const
      {
        assert_choice_type(types::scg_fail_info_r12, type_, "c2");
        return c.get<scg_fail_info_r12_s>();
      }
      const sidelink_ue_info_r12_s& sidelink_ue_info_r12() const
      {
        assert_choice_type(types::sidelink_ue_info_r12, type_, "c2");
        return c.get<sidelink_ue_info_r12_s>();
      }
      const wlan_conn_status_report_r13_s& wlan_conn_status_report_r13() const
      {
        assert_choice_type(types::wlan_conn_status_report_r13, type_, "c2");
        return c.get<wlan_conn_status_report_r13_s>();
      }
      const rrc_conn_resume_complete_r13_s& rrc_conn_resume_complete_r13() const
      {
        assert_choice_type(types::rrc_conn_resume_complete_r13, type_, "c2");
        return c.get<rrc_conn_resume_complete_r13_s>();
      }
      const ul_info_transfer_mrdc_r15_s& ul_info_transfer_mrdc_r15() const
      {
        assert_choice_type(types::ul_info_transfer_mrdc_r15, type_, "c2");
        return c.get<ul_info_transfer_mrdc_r15_s>();
      }
      const scg_fail_info_nr_r15_s& scg_fail_info_nr_r15() const
      {
        assert_choice_type(types::scg_fail_info_nr_r15, type_, "c2");
        return c.get<scg_fail_info_nr_r15_s>();
      }
      const meas_report_app_layer_r15_s& meas_report_app_layer_r15() const
      {
        assert_choice_type(types::meas_report_app_layer_r15, type_, "c2");
        return c.get<meas_report_app_layer_r15_s>();
      }
      const fail_info_r15_s& fail_info_r15() const
      {
        assert_choice_type(types::fail_info_r15, type_, "c2");
        return c.get<fail_info_r15_s>();
      }
      const ul_ded_msg_segment_r16_s& ul_ded_msg_segment_r16() const
      {
        assert_choice_type(types::ul_ded_msg_segment_r16, type_, "c2");
        return c.get<ul_ded_msg_segment_r16_s>();
      }
      const pur_cfg_request_r16_s& pur_cfg_request_r16() const
      {
        assert_choice_type(types::pur_cfg_request_r16, type_, "c2");
        return c.get<pur_cfg_request_r16_s>();
      }
      const fail_info_r16_s& fail_info_r16() const
      {
        assert_choice_type(types::fail_info_r16, type_, "c2");
        return c.get<fail_info_r16_s>();
      }
      const mcg_fail_info_r16_s& mcg_fail_info_r16() const
      {
        assert_choice_type(types::mcg_fail_info_r16, type_, "c2");
        return c.get<mcg_fail_info_r16_s>();
      }
      const ul_info_transfer_irat_r16_s& ul_info_transfer_irat_r16() const
      {
        assert_choice_type(types::ul_info_transfer_irat_r16, type_, "c2");
        return c.get<ul_info_transfer_irat_r16_s>();
      }
      ueassist_info_r11_s&            set_ue_assist_info_r11();
      in_dev_coex_ind_r11_s&          set_in_dev_coex_ind_r11();
      mbms_interest_ind_r11_s&        set_mbms_interest_ind_r11();
      scg_fail_info_r12_s&            set_scg_fail_info_r12();
      sidelink_ue_info_r12_s&         set_sidelink_ue_info_r12();
      wlan_conn_status_report_r13_s&  set_wlan_conn_status_report_r13();
      rrc_conn_resume_complete_r13_s& set_rrc_conn_resume_complete_r13();
      ul_info_transfer_mrdc_r15_s&    set_ul_info_transfer_mrdc_r15();
      scg_fail_info_nr_r15_s&         set_scg_fail_info_nr_r15();
      meas_report_app_layer_r15_s&    set_meas_report_app_layer_r15();
      fail_info_r15_s&                set_fail_info_r15();
      ul_ded_msg_segment_r16_s&       set_ul_ded_msg_segment_r16();
      pur_cfg_request_r16_s&          set_pur_cfg_request_r16();
      fail_info_r16_s&                set_fail_info_r16();
      mcg_fail_info_r16_s&            set_mcg_fail_info_r16();
      ul_info_transfer_irat_r16_s&    set_ul_info_transfer_irat_r16();

    private:
      types type_;
      choice_buffer_t<fail_info_r15_s,
                      fail_info_r16_s,
                      in_dev_coex_ind_r11_s,
                      mbms_interest_ind_r11_s,
                      mcg_fail_info_r16_s,
                      meas_report_app_layer_r15_s,
                      pur_cfg_request_r16_s,
                      rrc_conn_resume_complete_r13_s,
                      scg_fail_info_nr_r15_s,
                      scg_fail_info_r12_s,
                      sidelink_ue_info_r12_s,
                      ueassist_info_r11_s,
                      ul_ded_msg_segment_r16_s,
                      ul_info_transfer_irat_r16_s,
                      ul_info_transfer_mrdc_r15_s,
                      wlan_conn_status_report_r13_s>
          c;

      void destroy_();
    };
    struct types_opts {
      enum options { c2, msg_class_ext_future_r11, nulltype } value;
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
    void   set_msg_class_ext_future_r11();

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
  ul_dcch_msg_type_c() = default;
  ul_dcch_msg_type_c(const ul_dcch_msg_type_c& other);
  ul_dcch_msg_type_c& operator=(const ul_dcch_msg_type_c& other);
  ~ul_dcch_msg_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "UL-DCCH-MessageType");
    return c.get<c1_c_>();
  }
  msg_class_ext_c_& msg_class_ext()
  {
    assert_choice_type(types::msg_class_ext, type_, "UL-DCCH-MessageType");
    return c.get<msg_class_ext_c_>();
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "UL-DCCH-MessageType");
    return c.get<c1_c_>();
  }
  const msg_class_ext_c_& msg_class_ext() const
  {
    assert_choice_type(types::msg_class_ext, type_, "UL-DCCH-MessageType");
    return c.get<msg_class_ext_c_>();
  }
  c1_c_&            set_c1();
  msg_class_ext_c_& set_msg_class_ext();

private:
  types                                    type_;
  choice_buffer_t<c1_c_, msg_class_ext_c_> c;

  void destroy_();
};

// UL-DCCH-Message ::= SEQUENCE
struct ul_dcch_msg_s {
  ul_dcch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLF-Report-v9e0 ::= SEQUENCE
struct rlf_report_v9e0_s {
  meas_result_list2_eutra_v9e0_l meas_result_list_eutra_v9e0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationResponse-v9e0-IEs ::= SEQUENCE
struct ue_info_resp_v9e0_ies_s {
  bool              rlf_report_v9e0_present = false;
  bool              non_crit_ext_present    = false;
  rlf_report_v9e0_s rlf_report_v9e0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc
} // namespace asn1

#endif // SRSASN1_RRC_ULDCCH_MSG_H
