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

#ifndef SRSASN1_RRC_UECAP_H
#define SRSASN1_RRC_UECAP_H

#include "common.h"

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// CA-BandwidthClass-r10 ::= ENUMERATED
struct ca_bw_class_r10_opts {
  enum options { a, b, c, d, e, f, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<ca_bw_class_r10_opts, true> ca_bw_class_r10_e;

// UECapabilityEnquiry-v1560-IEs ::= SEQUENCE
struct ue_cap_enquiry_v1560_ies_s {
  bool          requested_cap_common_r15_present = false;
  bool          non_crit_ext_present             = false;
  dyn_octstring requested_cap_common_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandIndication-r14 ::= SEQUENCE
struct band_ind_r14_s {
  bool              ca_bw_class_ul_r14_present = false;
  uint16_t          band_eutra_r14             = 1;
  ca_bw_class_r10_e ca_bw_class_dl_r14;
  ca_bw_class_r10_e ca_bw_class_ul_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityEnquiry-v1550-IEs ::= SEQUENCE
struct ue_cap_enquiry_v1550_ies_s {
  bool                       requested_cap_nr_r15_present = false;
  bool                       non_crit_ext_present         = false;
  dyn_octstring              requested_cap_nr_r15;
  ue_cap_enquiry_v1560_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombination-r14 ::= SEQUENCE (SIZE (1..64)) OF BandIndication-r14
using band_combination_r14_l = dyn_array<band_ind_r14_s>;

// UECapabilityEnquiry-v1530-IEs ::= SEQUENCE
struct ue_cap_enquiry_v1530_ies_s {
  bool                       request_stti_spt_cap_r15_present = false;
  bool                       eutra_nr_only_r15_present        = false;
  bool                       non_crit_ext_present             = false;
  ue_cap_enquiry_v1550_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationList-r14 ::= SEQUENCE (SIZE (1..384)) OF BandCombination-r14
using band_combination_list_r14_l = dyn_array<band_combination_r14_l>;

// UECapabilityEnquiry-v1510-IEs ::= SEQUENCE
struct ue_cap_enquiry_v1510_ies_s {
  bool                       requested_freq_bands_nr_mrdc_r15_present = false;
  bool                       non_crit_ext_present                     = false;
  dyn_octstring              requested_freq_bands_nr_mrdc_r15;
  ue_cap_enquiry_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityEnquiry-v1430-IEs ::= SEQUENCE
struct ue_cap_enquiry_v1430_ies_s {
  bool                        request_diff_fallback_comb_list_r14_present = false;
  bool                        non_crit_ext_present                        = false;
  band_combination_list_r14_l request_diff_fallback_comb_list_r14;
  ue_cap_enquiry_v1510_ies_s  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityEnquiry-v1310-IEs ::= SEQUENCE
struct ue_cap_enquiry_v1310_ies_s {
  bool                       request_reduced_format_r13_present            = false;
  bool                       request_skip_fallback_comb_r13_present        = false;
  bool                       requested_max_ccs_dl_r13_present              = false;
  bool                       requested_max_ccs_ul_r13_present              = false;
  bool                       request_reduced_int_non_cont_comb_r13_present = false;
  bool                       non_crit_ext_present                          = false;
  uint8_t                    requested_max_ccs_dl_r13                      = 2;
  uint8_t                    requested_max_ccs_ul_r13                      = 2;
  ue_cap_enquiry_v1430_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RAT-Type ::= ENUMERATED
struct rat_type_opts {
  enum options { eutra, utra, geran_cs, geran_ps, cdma2000_minus1_xrtt, nr, eutra_nr, spare1, /*...*/ nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<rat_type_opts, true> rat_type_e;

// UECapabilityEnquiry-v1180-IEs ::= SEQUENCE
struct ue_cap_enquiry_v1180_ies_s {
  using requested_freq_bands_r11_l_ = bounded_array<uint16_t, 16>;

  // member variables
  bool                        requested_freq_bands_r11_present = false;
  bool                        non_crit_ext_present             = false;
  requested_freq_bands_r11_l_ requested_freq_bands_r11;
  ue_cap_enquiry_v1310_ies_s  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRequest ::= SEQUENCE (SIZE (1..8)) OF RAT-Type
using ue_cap_request_l = bounded_array<rat_type_e, 8>;

// UECapabilityEnquiry-v8a0-IEs ::= SEQUENCE
struct ue_cap_enquiry_v8a0_ies_s {
  bool                       late_non_crit_ext_present = false;
  bool                       non_crit_ext_present      = false;
  dyn_octstring              late_non_crit_ext;
  ue_cap_enquiry_v1180_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityEnquiry-r8-IEs ::= SEQUENCE
struct ue_cap_enquiry_r8_ies_s {
  bool                      non_crit_ext_present = false;
  ue_cap_request_l          ue_cap_request;
  ue_cap_enquiry_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityEnquiry ::= SEQUENCE
struct ue_cap_enquiry_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ue_cap_enquiry_r8, spare3, spare2, spare1, nulltype } value;

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
      ue_cap_enquiry_r8_ies_s& ue_cap_enquiry_r8()
      {
        assert_choice_type(types::ue_cap_enquiry_r8, type_, "c1");
        return c;
      }
      const ue_cap_enquiry_r8_ies_s& ue_cap_enquiry_r8() const
      {
        assert_choice_type(types::ue_cap_enquiry_r8, type_, "c1");
        return c;
      }
      ue_cap_enquiry_r8_ies_s& set_ue_cap_enquiry_r8();
      void                     set_spare3();
      void                     set_spare2();
      void                     set_spare1();

    private:
      types                   type_;
      ue_cap_enquiry_r8_ies_s c;
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

// UE-RadioPagingInfo-r12 ::= SEQUENCE
struct ue_radio_paging_info_r12_s {
  struct wake_up_signal_min_gap_e_drx_r15_opts {
    enum options { ms40, ms240, ms1000, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<wake_up_signal_min_gap_e_drx_r15_opts> wake_up_signal_min_gap_e_drx_r15_e_;
  struct wake_up_signal_min_gap_e_drx_tdd_r15_opts {
    enum options { ms40, ms240, ms1000, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<wake_up_signal_min_gap_e_drx_tdd_r15_opts> wake_up_signal_min_gap_e_drx_tdd_r15_e_;

  // member variables
  bool    ext                       = false;
  bool    ue_category_v1250_present = false;
  uint8_t ue_category_v1250         = 0;
  // ...
  // group 0
  bool ue_category_dl_v1310_present = false;
  bool ce_mode_a_r13_present        = false;
  bool ce_mode_b_r13_present        = false;
  // group 1
  bool                                    wake_up_signal_r15_present                   = false;
  bool                                    wake_up_signal_tdd_r15_present               = false;
  bool                                    wake_up_signal_min_gap_e_drx_r15_present     = false;
  bool                                    wake_up_signal_min_gap_e_drx_tdd_r15_present = false;
  wake_up_signal_min_gap_e_drx_r15_e_     wake_up_signal_min_gap_e_drx_r15;
  wake_up_signal_min_gap_e_drx_tdd_r15_e_ wake_up_signal_min_gap_e_drx_tdd_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRAT-Container ::= SEQUENCE
struct ue_cap_rat_container_s {
  rat_type_e    rat_type;
  dyn_octstring ue_cap_rat_container;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityInformation-v1250-IEs ::= SEQUENCE
struct ue_cap_info_v1250_ies_s {
  bool                       ue_radio_paging_info_r12_present = false;
  bool                       non_crit_ext_present             = false;
  ue_radio_paging_info_r12_s ue_radio_paging_info_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-CapabilityRAT-ContainerList ::= SEQUENCE (SIZE (0..8)) OF UE-CapabilityRAT-Container
using ue_cap_rat_container_list_l = dyn_array<ue_cap_rat_container_s>;

// UECapabilityInformation-v8a0-IEs ::= SEQUENCE
struct ue_cap_info_v8a0_ies_s {
  bool                    late_non_crit_ext_present = false;
  bool                    non_crit_ext_present      = false;
  dyn_octstring           late_non_crit_ext;
  ue_cap_info_v1250_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityInformation-r8-IEs ::= SEQUENCE
struct ue_cap_info_r8_ies_s {
  bool                        non_crit_ext_present = false;
  ue_cap_rat_container_list_l ue_cap_rat_container_list;
  ue_cap_info_v8a0_ies_s      non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityInformation ::= SEQUENCE
struct ue_cap_info_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ue_cap_info_r8, spare7, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;

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
      ue_cap_info_r8_ies_s& ue_cap_info_r8()
      {
        assert_choice_type(types::ue_cap_info_r8, type_, "c1");
        return c;
      }
      const ue_cap_info_r8_ies_s& ue_cap_info_r8() const
      {
        assert_choice_type(types::ue_cap_info_r8, type_, "c1");
        return c;
      }
      ue_cap_info_r8_ies_s& set_ue_cap_info_r8();
      void                  set_spare7();
      void                  set_spare6();
      void                  set_spare5();
      void                  set_spare4();
      void                  set_spare3();
      void                  set_spare2();
      void                  set_spare1();

    private:
      types                type_;
      ue_cap_info_r8_ies_s c;
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

// InterFreqBandInfo ::= SEQUENCE
struct inter_freq_band_info_s {
  bool inter_freq_need_for_gaps = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterRAT-BandInfo ::= SEQUENCE
struct inter_rat_band_info_s {
  bool inter_rat_need_for_gaps = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqBandList ::= SEQUENCE (SIZE (1..64)) OF InterFreqBandInfo
using inter_freq_band_list_l = dyn_array<inter_freq_band_info_s>;

// InterRAT-BandList ::= SEQUENCE (SIZE (1..64)) OF InterRAT-BandInfo
using inter_rat_band_list_l = dyn_array<inter_rat_band_info_s>;

// BandInfoEUTRA ::= SEQUENCE
struct band_info_eutra_s {
  bool                   inter_rat_band_list_present = false;
  inter_freq_band_list_l inter_freq_band_list;
  inter_rat_band_list_l  inter_rat_band_list;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationListEUTRA-r10 ::= SEQUENCE (SIZE (1..128)) OF BandInfoEUTRA
using band_combination_list_eutra_r10_l = dyn_array<band_info_eutra_s>;

// MIMO-CapabilityDL-r10 ::= ENUMERATED
struct mimo_cap_dl_r10_opts {
  enum options { two_layers, four_layers, eight_layers, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<mimo_cap_dl_r10_opts> mimo_cap_dl_r10_e;

// MIMO-CapabilityUL-r10 ::= ENUMERATED
struct mimo_cap_ul_r10_opts {
  enum options { two_layers, four_layers, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<mimo_cap_ul_r10_opts> mimo_cap_ul_r10_e;

// CA-MIMO-ParametersDL-r10 ::= SEQUENCE
struct ca_mimo_params_dl_r10_s {
  bool              supported_mimo_cap_dl_r10_present = false;
  ca_bw_class_r10_e ca_bw_class_dl_r10;
  mimo_cap_dl_r10_e supported_mimo_cap_dl_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-MIMO-ParametersUL-r10 ::= SEQUENCE
struct ca_mimo_params_ul_r10_s {
  bool              supported_mimo_cap_ul_r10_present = false;
  ca_bw_class_r10_e ca_bw_class_ul_r10;
  mimo_cap_ul_r10_e supported_mimo_cap_ul_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParametersDL-r10 ::= SEQUENCE (SIZE (1..16)) OF CA-MIMO-ParametersDL-r10
using band_params_dl_r10_l = dyn_array<ca_mimo_params_dl_r10_s>;

// BandParametersUL-r10 ::= SEQUENCE (SIZE (1..16)) OF CA-MIMO-ParametersUL-r10
using band_params_ul_r10_l = dyn_array<ca_mimo_params_ul_r10_s>;

// BandParameters-r10 ::= SEQUENCE
struct band_params_r10_s {
  bool                 band_params_ul_r10_present = false;
  bool                 band_params_dl_r10_present = false;
  uint8_t              band_eutra_r10             = 1;
  band_params_ul_r10_l band_params_ul_r10;
  band_params_dl_r10_l band_params_dl_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParameters-r10 ::= SEQUENCE (SIZE (1..64)) OF BandParameters-r10
using band_combination_params_r10_l = dyn_array<band_params_r10_s>;

// BandParameters-r11 ::= SEQUENCE
struct band_params_r11_s {
  struct supported_csi_proc_r11_opts {
    enum options { n1, n3, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<supported_csi_proc_r11_opts> supported_csi_proc_r11_e_;

  // member variables
  bool                      band_params_ul_r11_present     = false;
  bool                      band_params_dl_r11_present     = false;
  bool                      supported_csi_proc_r11_present = false;
  uint16_t                  band_eutra_r11                 = 1;
  band_params_ul_r10_l      band_params_ul_r11;
  band_params_dl_r10_l      band_params_dl_r11;
  supported_csi_proc_r11_e_ supported_csi_proc_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParameters-r11 ::= SEQUENCE
struct band_combination_params_r11_s {
  using band_param_list_r11_l_ = dyn_array<band_params_r11_s>;

  // member variables
  bool                     ext                                      = false;
  bool                     supported_bw_combination_set_r11_present = false;
  bool                     multiple_timing_advance_r11_present      = false;
  bool                     simul_rx_tx_r11_present                  = false;
  band_param_list_r11_l_   band_param_list_r11;
  bounded_bitstring<1, 32> supported_bw_combination_set_r11;
  band_info_eutra_s        band_info_eutra_r11;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraBandContiguousCC-Info-r12 ::= SEQUENCE
struct intra_band_contiguous_cc_info_r12_s {
  struct supported_csi_proc_r12_opts {
    enum options { n1, n3, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<supported_csi_proc_r12_opts> supported_csi_proc_r12_e_;

  // member variables
  bool                      four_layer_tm3_tm4_per_cc_r12_present = false;
  bool                      supported_mimo_cap_dl_r12_present     = false;
  bool                      supported_csi_proc_r12_present        = false;
  mimo_cap_dl_r10_e         supported_mimo_cap_dl_r12;
  supported_csi_proc_r12_e_ supported_csi_proc_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-MIMO-ParametersDL-r13 ::= SEQUENCE
struct ca_mimo_params_dl_r13_s {
  using intra_band_contiguous_cc_info_list_r13_l_ = dyn_array<intra_band_contiguous_cc_info_r12_s>;

  // member variables
  bool                                      supported_mimo_cap_dl_r13_present = false;
  bool                                      four_layer_tm3_tm4_r13_present    = false;
  ca_bw_class_r10_e                         ca_bw_class_dl_r13;
  mimo_cap_dl_r10_e                         supported_mimo_cap_dl_r13;
  intra_band_contiguous_cc_info_list_r13_l_ intra_band_contiguous_cc_info_list_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParametersDL-r13 ::= CA-MIMO-ParametersDL-r13
using band_params_dl_r13_s = ca_mimo_params_dl_r13_s;

// BandParametersUL-r13 ::= CA-MIMO-ParametersUL-r10
using band_params_ul_r13_s = ca_mimo_params_ul_r10_s;

// BandParameters-r13 ::= SEQUENCE
struct band_params_r13_s {
  struct supported_csi_proc_r13_opts {
    enum options { n1, n3, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<supported_csi_proc_r13_opts> supported_csi_proc_r13_e_;

  // member variables
  bool                      band_params_ul_r13_present     = false;
  bool                      band_params_dl_r13_present     = false;
  bool                      supported_csi_proc_r13_present = false;
  uint16_t                  band_eutra_r13                 = 1;
  band_params_ul_r13_s      band_params_ul_r13;
  band_params_dl_r13_s      band_params_dl_r13;
  supported_csi_proc_r13_e_ supported_csi_proc_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParameters-r13 ::= SEQUENCE
struct band_combination_params_r13_s {
  using band_param_list_r13_l_ = dyn_array<band_params_r13_s>;
  struct dc_support_r13_s_ {
    struct supported_cell_grouping_r13_c_ {
      struct types_opts {
        enum options { three_entries_r13, four_entries_r13, five_entries_r13, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      supported_cell_grouping_r13_c_() = default;
      supported_cell_grouping_r13_c_(const supported_cell_grouping_r13_c_& other);
      supported_cell_grouping_r13_c_& operator=(const supported_cell_grouping_r13_c_& other);
      ~supported_cell_grouping_r13_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      fixed_bitstring<3>& three_entries_r13()
      {
        assert_choice_type(types::three_entries_r13, type_, "supportedCellGrouping-r13");
        return c.get<fixed_bitstring<3> >();
      }
      fixed_bitstring<7>& four_entries_r13()
      {
        assert_choice_type(types::four_entries_r13, type_, "supportedCellGrouping-r13");
        return c.get<fixed_bitstring<7> >();
      }
      fixed_bitstring<15>& five_entries_r13()
      {
        assert_choice_type(types::five_entries_r13, type_, "supportedCellGrouping-r13");
        return c.get<fixed_bitstring<15> >();
      }
      const fixed_bitstring<3>& three_entries_r13() const
      {
        assert_choice_type(types::three_entries_r13, type_, "supportedCellGrouping-r13");
        return c.get<fixed_bitstring<3> >();
      }
      const fixed_bitstring<7>& four_entries_r13() const
      {
        assert_choice_type(types::four_entries_r13, type_, "supportedCellGrouping-r13");
        return c.get<fixed_bitstring<7> >();
      }
      const fixed_bitstring<15>& five_entries_r13() const
      {
        assert_choice_type(types::five_entries_r13, type_, "supportedCellGrouping-r13");
        return c.get<fixed_bitstring<15> >();
      }
      fixed_bitstring<3>&  set_three_entries_r13();
      fixed_bitstring<7>&  set_four_entries_r13();
      fixed_bitstring<15>& set_five_entries_r13();

    private:
      types                                 type_;
      choice_buffer_t<fixed_bitstring<15> > c;

      void destroy_();
    };

    // member variables
    bool                           async_r13_present                   = false;
    bool                           supported_cell_grouping_r13_present = false;
    supported_cell_grouping_r13_c_ supported_cell_grouping_r13;
  };

  // member variables
  bool                     different_fallback_supported_r13_present  = false;
  bool                     supported_bw_combination_set_r13_present  = false;
  bool                     multiple_timing_advance_r13_present       = false;
  bool                     simul_rx_tx_r13_present                   = false;
  bool                     dc_support_r13_present                    = false;
  bool                     supported_naics_minus2_crs_ap_r13_present = false;
  bool                     comm_supported_bands_per_bc_r13_present   = false;
  band_param_list_r13_l_   band_param_list_r13;
  bounded_bitstring<1, 32> supported_bw_combination_set_r13;
  band_info_eutra_s        band_info_eutra_r13;
  dc_support_r13_s_        dc_support_r13;
  bounded_bitstring<1, 8>  supported_naics_minus2_crs_ap_r13;
  bounded_bitstring<1, 64> comm_supported_bands_per_bc_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParameters-v1090 ::= SEQUENCE
struct band_params_v1090_s {
  bool     ext                      = false;
  bool     band_eutra_v1090_present = false;
  uint16_t band_eutra_v1090         = 65;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParameters-v1090 ::= SEQUENCE (SIZE (1..64)) OF BandParameters-v1090
using band_combination_params_v1090_l = dyn_array<band_params_v1090_s>;

// CA-MIMO-ParametersDL-v10i0 ::= SEQUENCE
struct ca_mimo_params_dl_v10i0_s {
  bool four_layer_tm3_tm4_r10_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParameters-v10i0 ::= SEQUENCE
struct band_params_v10i0_s {
  using band_params_dl_v10i0_l_ = dyn_array<ca_mimo_params_dl_v10i0_s>;

  // member variables
  band_params_dl_v10i0_l_ band_params_dl_v10i0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParameters-v10i0 ::= SEQUENCE
struct band_combination_params_v10i0_s {
  using band_param_list_v10i0_l_ = dyn_array<band_params_v10i0_s>;

  // member variables
  bool                     band_param_list_v10i0_present = false;
  band_param_list_v10i0_l_ band_param_list_v10i0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParameters-v1130 ::= SEQUENCE
struct band_params_v1130_s {
  struct supported_csi_proc_r11_opts {
    enum options { n1, n3, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<supported_csi_proc_r11_opts> supported_csi_proc_r11_e_;

  // member variables
  supported_csi_proc_r11_e_ supported_csi_proc_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParameters-v1130 ::= SEQUENCE
struct band_combination_params_v1130_s {
  using band_param_list_r11_l_ = dyn_array<band_params_v1130_s>;

  // member variables
  bool                   ext                                 = false;
  bool                   multiple_timing_advance_r11_present = false;
  bool                   simul_rx_tx_r11_present             = false;
  bool                   band_param_list_r11_present         = false;
  band_param_list_r11_l_ band_param_list_r11;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParameters-v1250 ::= SEQUENCE
struct band_combination_params_v1250_s {
  struct dc_support_r12_s_ {
    struct supported_cell_grouping_r12_c_ {
      struct types_opts {
        enum options { three_entries_r12, four_entries_r12, five_entries_r12, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      supported_cell_grouping_r12_c_() = default;
      supported_cell_grouping_r12_c_(const supported_cell_grouping_r12_c_& other);
      supported_cell_grouping_r12_c_& operator=(const supported_cell_grouping_r12_c_& other);
      ~supported_cell_grouping_r12_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      fixed_bitstring<3>& three_entries_r12()
      {
        assert_choice_type(types::three_entries_r12, type_, "supportedCellGrouping-r12");
        return c.get<fixed_bitstring<3> >();
      }
      fixed_bitstring<7>& four_entries_r12()
      {
        assert_choice_type(types::four_entries_r12, type_, "supportedCellGrouping-r12");
        return c.get<fixed_bitstring<7> >();
      }
      fixed_bitstring<15>& five_entries_r12()
      {
        assert_choice_type(types::five_entries_r12, type_, "supportedCellGrouping-r12");
        return c.get<fixed_bitstring<15> >();
      }
      const fixed_bitstring<3>& three_entries_r12() const
      {
        assert_choice_type(types::three_entries_r12, type_, "supportedCellGrouping-r12");
        return c.get<fixed_bitstring<3> >();
      }
      const fixed_bitstring<7>& four_entries_r12() const
      {
        assert_choice_type(types::four_entries_r12, type_, "supportedCellGrouping-r12");
        return c.get<fixed_bitstring<7> >();
      }
      const fixed_bitstring<15>& five_entries_r12() const
      {
        assert_choice_type(types::five_entries_r12, type_, "supportedCellGrouping-r12");
        return c.get<fixed_bitstring<15> >();
      }
      fixed_bitstring<3>&  set_three_entries_r12();
      fixed_bitstring<7>&  set_four_entries_r12();
      fixed_bitstring<15>& set_five_entries_r12();

    private:
      types                                 type_;
      choice_buffer_t<fixed_bitstring<15> > c;

      void destroy_();
    };

    // member variables
    bool                           async_r12_present                   = false;
    bool                           supported_cell_grouping_r12_present = false;
    supported_cell_grouping_r12_c_ supported_cell_grouping_r12;
  };

  // member variables
  bool                     ext                                       = false;
  bool                     dc_support_r12_present                    = false;
  bool                     supported_naics_minus2_crs_ap_r12_present = false;
  bool                     comm_supported_bands_per_bc_r12_present   = false;
  dc_support_r12_s_        dc_support_r12;
  bounded_bitstring<1, 8>  supported_naics_minus2_crs_ap_r12;
  bounded_bitstring<1, 64> comm_supported_bands_per_bc_r12;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-MIMO-ParametersDL-v1270 ::= SEQUENCE
struct ca_mimo_params_dl_v1270_s {
  using intra_band_contiguous_cc_info_list_r12_l_ = dyn_array<intra_band_contiguous_cc_info_r12_s>;

  // member variables
  intra_band_contiguous_cc_info_list_r12_l_ intra_band_contiguous_cc_info_list_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParameters-v1270 ::= SEQUENCE
struct band_params_v1270_s {
  using band_params_dl_v1270_l_ = dyn_array<ca_mimo_params_dl_v1270_s>;

  // member variables
  band_params_dl_v1270_l_ band_params_dl_v1270;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParameters-v1270 ::= SEQUENCE
struct band_combination_params_v1270_s {
  using band_param_list_v1270_l_ = dyn_array<band_params_v1270_s>;

  // member variables
  bool                     band_param_list_v1270_present = false;
  band_param_list_v1270_l_ band_param_list_v1270;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-BeamformedCapabilities-r13 ::= SEQUENCE
struct mimo_bf_cap_r13_s {
  bool                    n_max_list_r13_present = false;
  uint8_t                 k_max_r13              = 1;
  bounded_bitstring<1, 7> n_max_list_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-BeamformedCapabilityList-r13 ::= SEQUENCE (SIZE (1..4)) OF MIMO-BeamformedCapabilities-r13
using mimo_bf_cap_list_r13_l = dyn_array<mimo_bf_cap_r13_s>;

// MIMO-NonPrecodedCapabilities-r13 ::= SEQUENCE
struct mimo_non_precoded_cap_r13_s {
  bool cfg1_r13_present = false;
  bool cfg2_r13_present = false;
  bool cfg3_r13_present = false;
  bool cfg4_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-CA-ParametersPerBoBCPerTM-r13 ::= SEQUENCE
struct mimo_ca_params_per_bo_bc_per_tm_r13_s {
  bool                        non_precoded_r13_present = false;
  bool                        bf_r13_present           = false;
  bool                        dmrs_enhance_r13_present = false;
  mimo_non_precoded_cap_r13_s non_precoded_r13;
  mimo_bf_cap_list_r13_l      bf_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-CA-ParametersPerBoBC-r13 ::= SEQUENCE
struct mimo_ca_params_per_bo_bc_r13_s {
  bool                                  params_tm9_r13_present  = false;
  bool                                  params_tm10_r13_present = false;
  mimo_ca_params_per_bo_bc_per_tm_r13_s params_tm9_r13;
  mimo_ca_params_per_bo_bc_per_tm_r13_s params_tm10_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParameters-v1320 ::= SEQUENCE
struct band_params_v1320_s {
  mimo_ca_params_per_bo_bc_r13_s band_params_dl_v1320;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParameters-v1320 ::= SEQUENCE
struct band_combination_params_v1320_s {
  using band_param_list_v1320_l_ = dyn_array<band_params_v1320_s>;

  // member variables
  bool                     band_param_list_v1320_present         = false;
  bool                     add_rx_tx_performance_req_r13_present = false;
  band_param_list_v1320_l_ band_param_list_v1320;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParameters-v1380 ::= SEQUENCE
struct band_params_v1380_s {
  bool    tx_ant_switch_dl_r13_present = false;
  bool    tx_ant_switch_ul_r13_present = false;
  uint8_t tx_ant_switch_dl_r13         = 1;
  uint8_t tx_ant_switch_ul_r13         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParameters-v1380 ::= SEQUENCE
struct band_combination_params_v1380_s {
  using band_param_list_v1380_l_ = dyn_array<band_params_v1380_s>;

  // member variables
  bool                     band_param_list_v1380_present = false;
  band_param_list_v1380_l_ band_param_list_v1380;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-CA-ParametersPerBoBCPerTM-v1430 ::= SEQUENCE
struct mimo_ca_params_per_bo_bc_per_tm_v1430_s {
  bool csi_report_np_r14_present       = false;
  bool csi_report_advanced_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-CA-ParametersPerBoBC-v1430 ::= SEQUENCE
struct mimo_ca_params_per_bo_bc_v1430_s {
  bool                                    params_tm9_v1430_present  = false;
  bool                                    params_tm10_v1430_present = false;
  mimo_ca_params_per_bo_bc_per_tm_v1430_s params_tm9_v1430;
  mimo_ca_params_per_bo_bc_per_tm_v1430_s params_tm10_v1430;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-CapabilityPerBandPair-r14 ::= SEQUENCE
struct srs_cap_per_band_pair_r14_s {
  struct retuning_info_s_ {
    struct rf_retuning_time_dl_r14_opts {
      enum options {
        n0,
        n0dot5,
        n1,
        n1dot5,
        n2,
        n2dot5,
        n3,
        n3dot5,
        n4,
        n4dot5,
        n5,
        n5dot5,
        n6,
        n6dot5,
        n7,
        spare1,
        nulltype
      } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<rf_retuning_time_dl_r14_opts> rf_retuning_time_dl_r14_e_;
    struct rf_retuning_time_ul_r14_opts {
      enum options {
        n0,
        n0dot5,
        n1,
        n1dot5,
        n2,
        n2dot5,
        n3,
        n3dot5,
        n4,
        n4dot5,
        n5,
        n5dot5,
        n6,
        n6dot5,
        n7,
        spare1,
        nulltype
      } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<rf_retuning_time_ul_r14_opts> rf_retuning_time_ul_r14_e_;

    // member variables
    bool                       rf_retuning_time_dl_r14_present = false;
    bool                       rf_retuning_time_ul_r14_present = false;
    rf_retuning_time_dl_r14_e_ rf_retuning_time_dl_r14;
    rf_retuning_time_ul_r14_e_ rf_retuning_time_ul_r14;
  };

  // member variables
  retuning_info_s_ retuning_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-256QAM-perCC-Info-r14 ::= SEQUENCE
struct ul_minus256_qam_per_cc_info_r14_s {
  bool ul_minus256_qam_per_cc_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParameters-v1430 ::= SEQUENCE
struct band_params_v1430_s {
  using ul_minus256_qam_per_cc_info_list_r14_l_ = dyn_array<ul_minus256_qam_per_cc_info_r14_s>;
  using srs_cap_per_band_pair_list_r14_l_       = dyn_array<srs_cap_per_band_pair_r14_s>;

  // member variables
  bool                                    band_params_dl_v1430_present                 = false;
  bool                                    ul_minus256_qam_r14_present                  = false;
  bool                                    ul_minus256_qam_per_cc_info_list_r14_present = false;
  bool                                    srs_cap_per_band_pair_list_r14_present       = false;
  mimo_ca_params_per_bo_bc_v1430_s        band_params_dl_v1430;
  ul_minus256_qam_per_cc_info_list_r14_l_ ul_minus256_qam_per_cc_info_list_r14;
  srs_cap_per_band_pair_list_r14_l_       srs_cap_per_band_pair_list_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParameters-v1430 ::= SEQUENCE
struct band_combination_params_v1430_s {
  using band_param_list_v1430_l_ = dyn_array<band_params_v1430_s>;

  // member variables
  bool                      band_param_list_v1430_present                      = false;
  bool                      v2x_supported_tx_band_comb_list_per_bc_r14_present = false;
  bool                      v2x_supported_rx_band_comb_list_per_bc_r14_present = false;
  band_param_list_v1430_l_  band_param_list_v1430;
  bounded_bitstring<1, 384> v2x_supported_tx_band_comb_list_per_bc_r14;
  bounded_bitstring<1, 384> v2x_supported_rx_band_comb_list_per_bc_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MUST-Parameters-r14 ::= SEQUENCE
struct must_params_r14_s {
  bool must_tm234_up_to2_tx_r14_present                     = false;
  bool must_tm89_up_to_one_interfering_layer_r14_present    = false;
  bool must_tm10_up_to_one_interfering_layer_r14_present    = false;
  bool must_tm89_up_to_three_interfering_layers_r14_present = false;
  bool must_tm10_up_to_three_interfering_layers_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParameters-v1450 ::= SEQUENCE
struct band_params_v1450_s {
  bool              must_cap_per_band_r14_present = false;
  must_params_r14_s must_cap_per_band_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParameters-v1450 ::= SEQUENCE
struct band_combination_params_v1450_s {
  using band_param_list_v1450_l_ = dyn_array<band_params_v1450_s>;

  // member variables
  bool                     band_param_list_v1450_present = false;
  band_param_list_v1450_l_ band_param_list_v1450;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-CA-ParametersPerBoBCPerTM-v1470 ::= SEQUENCE
struct mimo_ca_params_per_bo_bc_per_tm_v1470_s {
  struct csi_report_advanced_max_ports_r14_opts {
    enum options { n8, n12, n16, n20, n24, n28, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<csi_report_advanced_max_ports_r14_opts> csi_report_advanced_max_ports_r14_e_;

  // member variables
  bool                                 csi_report_advanced_max_ports_r14_present = false;
  csi_report_advanced_max_ports_r14_e_ csi_report_advanced_max_ports_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-CA-ParametersPerBoBC-v1470 ::= SEQUENCE
struct mimo_ca_params_per_bo_bc_v1470_s {
  mimo_ca_params_per_bo_bc_per_tm_v1470_s params_tm9_v1470;
  mimo_ca_params_per_bo_bc_per_tm_v1470_s params_tm10_v1470;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParameters-v1470 ::= SEQUENCE
struct band_params_v1470_s {
  bool                             band_params_dl_v1470_present = false;
  mimo_ca_params_per_bo_bc_v1470_s band_params_dl_v1470;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParameters-v1470 ::= SEQUENCE
struct band_combination_params_v1470_s {
  using band_param_list_v1470_l_ = dyn_array<band_params_v1470_s>;

  // member variables
  bool                     band_param_list_v1470_present = false;
  bool                     srs_max_simul_ccs_r14_present = false;
  band_param_list_v1470_l_ band_param_list_v1470;
  uint8_t                  srs_max_simul_ccs_r14 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRS-CapabilityPerBandPair-v14b0 ::= SEQUENCE
struct srs_cap_per_band_pair_v14b0_s {
  bool srs_flex_timing_r14_present  = false;
  bool srs_harq_ref_cfg_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParameters-v14b0 ::= SEQUENCE
struct band_params_v14b0_s {
  using srs_cap_per_band_pair_list_v14b0_l_ = dyn_array<srs_cap_per_band_pair_v14b0_s>;

  // member variables
  bool                                srs_cap_per_band_pair_list_v14b0_present = false;
  srs_cap_per_band_pair_list_v14b0_l_ srs_cap_per_band_pair_list_v14b0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParameters-v14b0 ::= SEQUENCE
struct band_combination_params_v14b0_s {
  using band_param_list_v14b0_l_ = dyn_array<band_params_v14b0_s>;

  // member variables
  bool                     band_param_list_v14b0_present = false;
  band_param_list_v14b0_l_ band_param_list_v14b0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-UL-CCs-r15 ::= SEQUENCE
struct dl_ul_ccs_r15_s {
  bool    max_num_dl_ccs_r15_present = false;
  bool    max_num_ul_ccs_r15_present = false;
  uint8_t max_num_dl_ccs_r15         = 1;
  uint8_t max_num_ul_ccs_r15         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-MIMO-ParametersDL-r15 ::= SEQUENCE
struct ca_mimo_params_dl_r15_s {
  using intra_band_contiguous_cc_info_list_r15_l_ = dyn_array<intra_band_contiguous_cc_info_r12_s>;

  // member variables
  bool                                      supported_mimo_cap_dl_r15_present              = false;
  bool                                      four_layer_tm3_tm4_r15_present                 = false;
  bool                                      intra_band_contiguous_cc_info_list_r15_present = false;
  mimo_cap_dl_r10_e                         supported_mimo_cap_dl_r15;
  intra_band_contiguous_cc_info_list_r15_l_ intra_band_contiguous_cc_info_list_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CA-MIMO-ParametersUL-r15 ::= SEQUENCE
struct ca_mimo_params_ul_r15_s {
  bool              supported_mimo_cap_ul_r15_present = false;
  mimo_cap_ul_r10_e supported_mimo_cap_ul_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// STTI-SupportedCombinations-r15 ::= SEQUENCE
struct stti_supported_combinations_r15_s {
  using combination_minus22_minus27_r15_l_ = dyn_array<dl_ul_ccs_r15_s>;
  using combination_minus77_minus22_r15_l_ = dyn_array<dl_ul_ccs_r15_s>;
  using combination_minus77_minus27_r15_l_ = dyn_array<dl_ul_ccs_r15_s>;

  // member variables
  bool                               combination_minus22_r15_present         = false;
  bool                               combination_minus77_r15_present         = false;
  bool                               combination_minus27_r15_present         = false;
  bool                               combination_minus22_minus27_r15_present = false;
  bool                               combination_minus77_minus22_r15_present = false;
  bool                               combination_minus77_minus27_r15_present = false;
  dl_ul_ccs_r15_s                    combination_minus22_r15;
  dl_ul_ccs_r15_s                    combination_minus77_r15;
  dl_ul_ccs_r15_s                    combination_minus27_r15;
  combination_minus22_minus27_r15_l_ combination_minus22_minus27_r15;
  combination_minus77_minus22_r15_l_ combination_minus77_minus22_r15;
  combination_minus77_minus27_r15_l_ combination_minus77_minus27_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// STTI-SPT-BandParameters-r15 ::= SEQUENCE
struct stti_spt_band_params_r15_s {
  struct stti_supported_csi_proc_r15_opts {
    enum options { n1, n3, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<stti_supported_csi_proc_r15_opts> stti_supported_csi_proc_r15_e_;

  // member variables
  bool                              ext                                            = false;
  bool                              dl_minus1024_qam_slot_r15_present              = false;
  bool                              dl_minus1024_qam_subslot_ta_minus1_r15_present = false;
  bool                              dl_minus1024_qam_subslot_ta_minus2_r15_present = false;
  bool                              simul_tx_different_tx_dur_r15_present          = false;
  bool                              stti_ca_mimo_params_dl_r15_present             = false;
  bool                              stti_fd_mimo_coexistence_present               = false;
  bool                              stti_mimo_ca_params_per_bo_bcs_r15_present     = false;
  bool                              stti_mimo_ca_params_per_bo_bcs_v1530_present   = false;
  bool                              stti_supported_combinations_r15_present        = false;
  bool                              stti_supported_csi_proc_r15_present            = false;
  bool                              ul_minus256_qam_slot_r15_present               = false;
  bool                              ul_minus256_qam_subslot_r15_present            = false;
  ca_mimo_params_dl_r15_s           stti_ca_mimo_params_dl_r15;
  ca_mimo_params_ul_r15_s           stti_ca_mimo_params_ul_r15;
  mimo_ca_params_per_bo_bc_r13_s    stti_mimo_ca_params_per_bo_bcs_r15;
  mimo_ca_params_per_bo_bc_v1430_s  stti_mimo_ca_params_per_bo_bcs_v1530;
  stti_supported_combinations_r15_s stti_supported_combinations_r15;
  stti_supported_csi_proc_r15_e_    stti_supported_csi_proc_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParameters-v1530 ::= SEQUENCE
struct band_params_v1530_s {
  bool                       ue_tx_ant_sel_srs_minus1_t4_r_r15_present              = false;
  bool                       ue_tx_ant_sel_srs_minus2_t4_r_minus2_pairs_r15_present = false;
  bool                       ue_tx_ant_sel_srs_minus2_t4_r_minus3_pairs_r15_present = false;
  bool                       dl_minus1024_qam_r15_present                           = false;
  bool                       qcl_type_c_operation_r15_present                       = false;
  bool                       qcl_cri_based_csi_report_r15_present                   = false;
  bool                       stti_spt_band_params_r15_present                       = false;
  stti_spt_band_params_r15_s stti_spt_band_params_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SPT-Parameters-r15 ::= SEQUENCE
struct spt_params_r15_s {
  bool               frame_structure_type_spt_r15_present = false;
  bool               max_num_ccs_spt_r15_present          = false;
  fixed_bitstring<3> frame_structure_type_spt_r15;
  uint8_t            max_num_ccs_spt_r15 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParameters-v1530 ::= SEQUENCE
struct band_combination_params_v1530_s {
  using band_param_list_v1530_l_ = dyn_array<band_params_v1530_s>;

  // member variables
  bool                     band_param_list_v1530_present = false;
  bool                     spt_params_r15_present        = false;
  band_param_list_v1530_l_ band_param_list_v1530;
  spt_params_r15_s         spt_params_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParametersExt-r10 ::= SEQUENCE
struct band_combination_params_ext_r10_s {
  bool                     supported_bw_combination_set_r10_present = false;
  bounded_bitstring<1, 32> supported_bw_combination_set_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandListEUTRA ::= SEQUENCE (SIZE (1..64)) OF BandInfoEUTRA
using band_list_eutra_l = dyn_array<band_info_eutra_s>;

// V2X-BandwidthClass-r14 ::= ENUMERATED
struct v2x_bw_class_r14_opts {
  enum options { a, b, c, d, e, f, /*...*/ c1_v1530, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<v2x_bw_class_r14_opts, true, 1> v2x_bw_class_r14_e;

// V2X-BandwidthClassSL-r14 ::= SEQUENCE (SIZE (1..16)) OF V2X-BandwidthClass-r14
using v2x_bw_class_sl_r14_l = bounded_array<v2x_bw_class_r14_e, 16>;

// BandParametersRxSL-r14 ::= SEQUENCE
struct band_params_rx_sl_r14_s {
  bool                  v2x_high_reception_r14_present = false;
  v2x_bw_class_sl_r14_l v2x_bw_class_rx_sl_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandParametersTxSL-r14 ::= SEQUENCE
struct band_params_tx_sl_r14_s {
  bool                  v2x_enb_sched_r14_present = false;
  bool                  v2x_high_pwr_r14_present  = false;
  v2x_bw_class_sl_r14_l v2x_bw_class_tx_sl_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetDL-PerCC-r15 ::= SEQUENCE
struct feature_set_dl_per_cc_r15_s {
  struct supported_csi_proc_r15_opts {
    enum options { n1, n3, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<supported_csi_proc_r15_opts> supported_csi_proc_r15_e_;

  // member variables
  bool                      four_layer_tm3_tm4_r15_present         = false;
  bool                      supported_mimo_cap_dl_mrdc_r15_present = false;
  bool                      supported_csi_proc_r15_present         = false;
  mimo_cap_dl_r10_e         supported_mimo_cap_dl_mrdc_r15;
  supported_csi_proc_r15_e_ supported_csi_proc_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-CA-ParametersPerBoBCPerTM-r15 ::= SEQUENCE
struct mimo_ca_params_per_bo_bc_per_tm_r15_s {
  bool                        non_precoded_r13_present        = false;
  bool                        bf_r13_present                  = false;
  bool                        dmrs_enhance_r13_present        = false;
  bool                        csi_report_np_r14_present       = false;
  bool                        csi_report_advanced_r14_present = false;
  mimo_non_precoded_cap_r13_s non_precoded_r13;
  mimo_bf_cap_list_r13_l      bf_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-CA-ParametersPerBoBC-r15 ::= SEQUENCE
struct mimo_ca_params_per_bo_bc_r15_s {
  bool                                  params_tm9_r15_present  = false;
  bool                                  params_tm10_r15_present = false;
  mimo_ca_params_per_bo_bc_per_tm_r15_s params_tm9_r15;
  mimo_ca_params_per_bo_bc_per_tm_r15_s params_tm10_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetDL-r15 ::= SEQUENCE
struct feature_set_dl_r15_s {
  using feature_set_per_cc_list_dl_r15_l_ = bounded_array<uint8_t, 32>;

  // member variables
  bool                              mimo_ca_params_per_bo_bc_r15_present = false;
  mimo_ca_params_per_bo_bc_r15_s    mimo_ca_params_per_bo_bc_r15;
  feature_set_per_cc_list_dl_r15_l_ feature_set_per_cc_list_dl_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetUL-PerCC-r15 ::= SEQUENCE
struct feature_set_ul_per_cc_r15_s {
  bool              supported_mimo_cap_ul_r15_present = false;
  bool              ul_minus256_qam_r15_present       = false;
  mimo_cap_ul_r10_e supported_mimo_cap_ul_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetUL-r15 ::= SEQUENCE
struct feature_set_ul_r15_s {
  using feature_set_per_cc_list_ul_r15_l_ = bounded_array<uint8_t, 32>;

  // member variables
  feature_set_per_cc_list_ul_r15_l_ feature_set_per_cc_list_ul_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetDL-v1550 ::= SEQUENCE
struct feature_set_dl_v1550_s {
  bool dl_minus1024_qam_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeatureSetsEUTRA-r15 ::= SEQUENCE
struct feature_sets_eutra_r15_s {
  using feature_sets_dl_r15_l_        = dyn_array<feature_set_dl_r15_s>;
  using feature_sets_dl_per_cc_r15_l_ = dyn_array<feature_set_dl_per_cc_r15_s>;
  using feature_sets_ul_r15_l_        = dyn_array<feature_set_ul_r15_s>;
  using feature_sets_ul_per_cc_r15_l_ = dyn_array<feature_set_ul_per_cc_r15_s>;
  using feature_sets_dl_v1550_l_      = dyn_array<feature_set_dl_v1550_s>;

  // member variables
  bool                          ext                                = false;
  bool                          feature_sets_dl_r15_present        = false;
  bool                          feature_sets_dl_per_cc_r15_present = false;
  bool                          feature_sets_ul_r15_present        = false;
  bool                          feature_sets_ul_per_cc_r15_present = false;
  feature_sets_dl_r15_l_        feature_sets_dl_r15;
  feature_sets_dl_per_cc_r15_l_ feature_sets_dl_per_cc_r15;
  feature_sets_ul_r15_l_        feature_sets_ul_r15;
  feature_sets_ul_per_cc_r15_l_ feature_sets_ul_per_cc_r15;
  // ...
  // group 0
  copy_ptr<feature_sets_dl_v1550_l_> feature_sets_dl_v1550;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqBandIndicatorListEUTRA-r12 ::= SEQUENCE (SIZE (1..64)) OF INTEGER (1..256)
using freq_band_ind_list_eutra_r12_l = dyn_array<uint16_t>;

// SupportedBandList1XRTT ::= SEQUENCE (SIZE (1..32)) OF BandclassCDMA2000
using supported_band_list1_xrtt_l = bounded_array<bandclass_cdma2000_e, 32>;

// IRAT-ParametersCDMA2000-1XRTT ::= SEQUENCE
struct irat_params_cdma2000_minus1_xrtt_s {
  struct tx_cfg1_xrtt_opts {
    enum options { single, dual, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<tx_cfg1_xrtt_opts> tx_cfg1_xrtt_e_;
  struct rx_cfg1_xrtt_opts {
    enum options { single, dual, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<rx_cfg1_xrtt_opts> rx_cfg1_xrtt_e_;

  // member variables
  supported_band_list1_xrtt_l supported_band_list1_xrtt;
  tx_cfg1_xrtt_e_             tx_cfg1_xrtt;
  rx_cfg1_xrtt_e_             rx_cfg1_xrtt;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandListHRPD ::= SEQUENCE (SIZE (1..32)) OF BandclassCDMA2000
using supported_band_list_hrpd_l = bounded_array<bandclass_cdma2000_e, 32>;

// IRAT-ParametersCDMA2000-HRPD ::= SEQUENCE
struct irat_params_cdma2000_hrpd_s {
  struct tx_cfg_hrpd_opts {
    enum options { single, dual, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<tx_cfg_hrpd_opts> tx_cfg_hrpd_e_;
  struct rx_cfg_hrpd_opts {
    enum options { single, dual, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<rx_cfg_hrpd_opts> rx_cfg_hrpd_e_;

  // member variables
  supported_band_list_hrpd_l supported_band_list_hrpd;
  tx_cfg_hrpd_e_             tx_cfg_hrpd;
  rx_cfg_hrpd_e_             rx_cfg_hrpd;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandGERAN ::= ENUMERATED
struct supported_band_geran_opts {
  enum options {
    gsm450,
    gsm480,
    gsm710,
    gsm750,
    gsm810,
    gsm850,
    gsm900_p,
    gsm900_e,
    gsm900_r,
    gsm1800,
    gsm1900,
    spare5,
    spare4,
    spare3,
    spare2,
    spare1,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<supported_band_geran_opts, true> supported_band_geran_e;

// SupportedBandListGERAN ::= SEQUENCE (SIZE (1..64)) OF SupportedBandGERAN
using supported_band_list_geran_l = dyn_array<supported_band_geran_e>;

// IRAT-ParametersGERAN ::= SEQUENCE
struct irat_params_geran_s {
  supported_band_list_geran_l supported_band_list_geran;
  bool                        inter_rat_ps_ho_to_geran = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandNR-r15 ::= SEQUENCE
struct supported_band_nr_r15_s {
  uint16_t band_nr_r15 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandListNR-r15 ::= SEQUENCE (SIZE (1..1024)) OF SupportedBandNR-r15
using supported_band_list_nr_r15_l = dyn_array<supported_band_nr_r15_s>;

// IRAT-ParametersNR-r15 ::= SEQUENCE
struct irat_params_nr_r15_s {
  bool                         en_dc_r15_present                     = false;
  bool                         event_b2_r15_present                  = false;
  bool                         supported_band_list_en_dc_r15_present = false;
  supported_band_list_nr_r15_l supported_band_list_en_dc_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IRAT-ParametersNR-v1540 ::= SEQUENCE
struct irat_params_nr_v1540_s {
  bool                         eutra_minus5_gc_ho_to_nr_fdd_fr1_r15_present = false;
  bool                         eutra_minus5_gc_ho_to_nr_tdd_fr1_r15_present = false;
  bool                         eutra_minus5_gc_ho_to_nr_fdd_fr2_r15_present = false;
  bool                         eutra_minus5_gc_ho_to_nr_tdd_fr2_r15_present = false;
  bool                         eutra_epc_ho_to_nr_fdd_fr1_r15_present       = false;
  bool                         eutra_epc_ho_to_nr_tdd_fr1_r15_present       = false;
  bool                         eutra_epc_ho_to_nr_fdd_fr2_r15_present       = false;
  bool                         eutra_epc_ho_to_nr_tdd_fr2_r15_present       = false;
  bool                         ims_voice_over_nr_fr1_r15_present            = false;
  bool                         ims_voice_over_nr_fr2_r15_present            = false;
  bool                         sa_nr_r15_present                            = false;
  bool                         supported_band_list_nr_sa_r15_present        = false;
  supported_band_list_nr_r15_l supported_band_list_nr_sa_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandUTRA-FDD ::= ENUMERATED
struct supported_band_utra_fdd_opts {
  enum options {
    band_i,
    band_ii,
    band_iii,
    band_iv,
    band_v,
    band_vi,
    band_vii,
    band_viii,
    band_ix,
    band_x,
    band_xi,
    band_xii,
    band_xiii,
    band_xiv,
    band_xv,
    band_xvi,
    // ...
    band_xvii_minus8a0,
    band_xviii_minus8a0,
    band_xix_minus8a0,
    band_xx_minus8a0,
    band_xxi_minus8a0,
    band_xxii_minus8a0,
    band_xxiii_minus8a0,
    band_xxiv_minus8a0,
    band_xxv_minus8a0,
    band_xxvi_minus8a0,
    band_xxvii_minus8a0,
    band_xxviii_minus8a0,
    band_xxix_minus8a0,
    band_xxx_minus8a0,
    band_xxxi_minus8a0,
    band_xxxii_minus8a0,
    nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<supported_band_utra_fdd_opts, true, 16> supported_band_utra_fdd_e;

// SupportedBandListUTRA-FDD ::= SEQUENCE (SIZE (1..64)) OF SupportedBandUTRA-FDD
using supported_band_list_utra_fdd_l = dyn_array<supported_band_utra_fdd_e>;

// IRAT-ParametersUTRA-FDD ::= SEQUENCE
struct irat_params_utra_fdd_s {
  supported_band_list_utra_fdd_l supported_band_list_utra_fdd;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandUTRA-TDD128 ::= ENUMERATED
struct supported_band_utra_tdd128_opts {
  enum options { a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<supported_band_utra_tdd128_opts, true> supported_band_utra_tdd128_e;

// SupportedBandListUTRA-TDD128 ::= SEQUENCE (SIZE (1..64)) OF SupportedBandUTRA-TDD128
using supported_band_list_utra_tdd128_l = dyn_array<supported_band_utra_tdd128_e>;

// IRAT-ParametersUTRA-TDD128 ::= SEQUENCE
struct irat_params_utra_tdd128_s {
  supported_band_list_utra_tdd128_l supported_band_list_utra_tdd128;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandUTRA-TDD384 ::= ENUMERATED
struct supported_band_utra_tdd384_opts {
  enum options { a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<supported_band_utra_tdd384_opts, true> supported_band_utra_tdd384_e;

// SupportedBandListUTRA-TDD384 ::= SEQUENCE (SIZE (1..64)) OF SupportedBandUTRA-TDD384
using supported_band_list_utra_tdd384_l = dyn_array<supported_band_utra_tdd384_e>;

// IRAT-ParametersUTRA-TDD384 ::= SEQUENCE
struct irat_params_utra_tdd384_s {
  supported_band_list_utra_tdd384_l supported_band_list_utra_tdd384;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandUTRA-TDD768 ::= ENUMERATED
struct supported_band_utra_tdd768_opts {
  enum options { a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<supported_band_utra_tdd768_opts, true> supported_band_utra_tdd768_e;

// SupportedBandListUTRA-TDD768 ::= SEQUENCE (SIZE (1..64)) OF SupportedBandUTRA-TDD768
using supported_band_list_utra_tdd768_l = dyn_array<supported_band_utra_tdd768_e>;

// IRAT-ParametersUTRA-TDD768 ::= SEQUENCE
struct irat_params_utra_tdd768_s {
  supported_band_list_utra_tdd768_l supported_band_list_utra_tdd768;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IRAT-ParametersWLAN-r13 ::= SEQUENCE
struct irat_params_wlan_r13_s {
  using supported_band_list_wlan_r13_l_ = bounded_array<wlan_band_ind_r13_e, 8>;

  // member variables
  bool                            supported_band_list_wlan_r13_present = false;
  supported_band_list_wlan_r13_l_ supported_band_list_wlan_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ProcessingTimelineSet-r15 ::= ENUMERATED
struct processing_timeline_set_r15_opts {
  enum options { set1, set2, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<processing_timeline_set_r15_opts> processing_timeline_set_r15_e;

// SkipSubframeProcessing-r15 ::= SEQUENCE
struct skip_sf_processing_r15_s {
  bool    skip_processing_dl_slot_r15_present     = false;
  bool    skip_processing_dl_sub_slot_r15_present = false;
  bool    skip_processing_ul_slot_r15_present     = false;
  bool    skip_processing_ul_sub_slot_r15_present = false;
  uint8_t skip_processing_dl_slot_r15             = 0;
  uint8_t skip_processing_dl_sub_slot_r15         = 0;
  uint8_t skip_processing_ul_slot_r15             = 0;
  uint8_t skip_processing_ul_sub_slot_r15         = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-Parameters-v1530 ::= SEQUENCE
struct mac_params_v1530_s {
  using min_proc_timeline_subslot_r15_l_ = bounded_array<processing_timeline_set_r15_e, 3>;

  // member variables
  bool                             min_proc_timeline_subslot_r15_present = false;
  bool                             skip_sf_processing_r15_present        = false;
  bool                             early_data_up_r15_present             = false;
  bool                             dormant_scell_state_r15_present       = false;
  bool                             direct_scell_activation_r15_present   = false;
  bool                             direct_scell_hibernation_r15_present  = false;
  bool                             extended_lcid_dupl_r15_present        = false;
  bool                             sps_serving_cell_r15_present          = false;
  min_proc_timeline_subslot_r15_l_ min_proc_timeline_subslot_r15;
  skip_sf_processing_r15_s         skip_sf_processing_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-UE-BeamformedCapabilities-r13 ::= SEQUENCE
struct mimo_ue_bf_cap_r13_s {
  bool                   alt_codebook_r13_present = false;
  mimo_bf_cap_list_r13_l mimo_bf_cap_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-UE-ParametersPerTM-r13 ::= SEQUENCE
struct mimo_ue_params_per_tm_r13_s {
  bool                        non_precoded_r13_present       = false;
  bool                        bf_r13_present                 = false;
  bool                        ch_meas_restrict_r13_present   = false;
  bool                        dmrs_enhance_r13_present       = false;
  bool                        csi_rs_enhance_tdd_r13_present = false;
  mimo_non_precoded_cap_r13_s non_precoded_r13;
  mimo_ue_bf_cap_r13_s        bf_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-UE-Parameters-r13 ::= SEQUENCE
struct mimo_ue_params_r13_s {
  bool                        params_tm9_r13_present                 = false;
  bool                        params_tm10_r13_present                = false;
  bool                        srs_enhance_tdd_r13_present            = false;
  bool                        srs_enhance_r13_present                = false;
  bool                        interference_meas_restrict_r13_present = false;
  mimo_ue_params_per_tm_r13_s params_tm9_r13;
  mimo_ue_params_per_tm_r13_s params_tm10_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-UE-ParametersPerTM-v1430 ::= SEQUENCE
struct mimo_ue_params_per_tm_v1430_s {
  struct nzp_csi_rs_aperiodic_info_r14_s_ {
    struct nmax_res_r14_opts {
      enum options { ffs1, ffs2, ffs3, ffs4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<nmax_res_r14_opts> nmax_res_r14_e_;

    // member variables
    uint8_t         nmax_proc_r14 = 5;
    nmax_res_r14_e_ nmax_res_r14;
  };
  struct nzp_csi_rs_periodic_info_r14_s_ {
    struct nmax_res_r14_opts {
      enum options { ffs1, ffs2, ffs3, ffs4, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<nmax_res_r14_opts> nmax_res_r14_e_;

    // member variables
    nmax_res_r14_e_ nmax_res_r14;
  };

  // member variables
  bool                             nzp_csi_rs_aperiodic_info_r14_present = false;
  bool                             nzp_csi_rs_periodic_info_r14_present  = false;
  bool                             zp_csi_rs_aperiodic_info_r14_present  = false;
  bool                             ul_dmrs_enhance_r14_present           = false;
  bool                             density_reduction_np_r14_present      = false;
  bool                             density_reduction_bf_r14_present      = false;
  bool                             hybrid_csi_r14_present                = false;
  bool                             semi_ol_r14_present                   = false;
  bool                             csi_report_np_r14_present             = false;
  bool                             csi_report_advanced_r14_present       = false;
  nzp_csi_rs_aperiodic_info_r14_s_ nzp_csi_rs_aperiodic_info_r14;
  nzp_csi_rs_periodic_info_r14_s_  nzp_csi_rs_periodic_info_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-UE-Parameters-v1430 ::= SEQUENCE
struct mimo_ue_params_v1430_s {
  bool                          params_tm9_v1430_present  = false;
  bool                          params_tm10_v1430_present = false;
  mimo_ue_params_per_tm_v1430_s params_tm9_v1430;
  mimo_ue_params_per_tm_v1430_s params_tm10_v1430;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-UE-ParametersPerTM-v1470 ::= SEQUENCE
struct mimo_ue_params_per_tm_v1470_s {
  struct csi_report_advanced_max_ports_r14_opts {
    enum options { n8, n12, n16, n20, n24, n28, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<csi_report_advanced_max_ports_r14_opts> csi_report_advanced_max_ports_r14_e_;

  // member variables
  bool                                 csi_report_advanced_max_ports_r14_present = false;
  csi_report_advanced_max_ports_r14_e_ csi_report_advanced_max_ports_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MIMO-UE-Parameters-v1470 ::= SEQUENCE
struct mimo_ue_params_v1470_s {
  mimo_ue_params_per_tm_v1470_s params_tm9_v1470;
  mimo_ue_params_per_tm_v1470_s params_tm10_v1470;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasParameters ::= SEQUENCE
struct meas_params_s {
  band_list_eutra_l band_list_eutra;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasParameters-v1020 ::= SEQUENCE
struct meas_params_v1020_s {
  band_combination_list_eutra_r10_l band_combination_list_eutra_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NAICS-Capability-Entry-r12 ::= SEQUENCE
struct naics_cap_entry_r12_s {
  struct nof_aggregated_prb_r12_opts {
    enum options {
      n50,
      n75,
      n100,
      n125,
      n150,
      n175,
      n200,
      n225,
      n250,
      n275,
      n300,
      n350,
      n400,
      n450,
      n500,
      spare,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<nof_aggregated_prb_r12_opts> nof_aggregated_prb_r12_e_;

  // member variables
  bool                      ext                      = false;
  uint8_t                   nof_naics_capable_cc_r12 = 1;
  nof_aggregated_prb_r12_e_ nof_aggregated_prb_r12;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NAICS-Capability-List-r12 ::= SEQUENCE (SIZE (1..8)) OF NAICS-Capability-Entry-r12
using naics_cap_list_r12_l = dyn_array<naics_cap_entry_r12_s>;

// NonContiguousUL-RA-WithinCC-r10 ::= SEQUENCE
struct non_contiguous_ul_ra_within_cc_r10_s {
  bool non_contiguous_ul_ra_within_cc_info_r10_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NonContiguousUL-RA-WithinCC-List-r10 ::= SEQUENCE (SIZE (1..64)) OF NonContiguousUL-RA-WithinCC-r10
using non_contiguous_ul_ra_within_cc_list_r10_l = dyn_array<non_contiguous_ul_ra_within_cc_r10_s>;

// ROHC-ProfileSupportList-r15 ::= SEQUENCE
struct rohc_profile_support_list_r15_s {
  bool profile0x0001_r15 = false;
  bool profile0x0002_r15 = false;
  bool profile0x0003_r15 = false;
  bool profile0x0004_r15 = false;
  bool profile0x0006_r15 = false;
  bool profile0x0101_r15 = false;
  bool profile0x0102_r15 = false;
  bool profile0x0103_r15 = false;
  bool profile0x0104_r15 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-Parameters ::= SEQUENCE
struct pdcp_params_s {
  struct max_num_rohc_context_sessions_opts {
    enum options {
      cs2,
      cs4,
      cs8,
      cs12,
      cs16,
      cs24,
      cs32,
      cs48,
      cs64,
      cs128,
      cs256,
      cs512,
      cs1024,
      cs16384,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<max_num_rohc_context_sessions_opts> max_num_rohc_context_sessions_e_;

  // member variables
  bool                             ext                                   = false;
  bool                             max_num_rohc_context_sessions_present = false;
  rohc_profile_support_list_r15_s  supported_rohc_profiles;
  max_num_rohc_context_sessions_e_ max_num_rohc_context_sessions;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedOperatorDic-r15 ::= SEQUENCE
struct supported_operator_dic_r15_s {
  uint8_t   version_of_dictionary_r15 = 0;
  plmn_id_s associated_plmn_id_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedUDC-r15 ::= SEQUENCE
struct supported_udc_r15_s {
  bool                         supported_standard_dic_r15_present = false;
  bool                         supported_operator_dic_r15_present = false;
  supported_operator_dic_r15_s supported_operator_dic_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-Parameters-v1530 ::= SEQUENCE
struct pdcp_params_v1530_s {
  bool                supported_udc_r15_present = false;
  bool                pdcp_dupl_r15_present     = false;
  supported_udc_r15_s supported_udc_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-ParametersNR-r15 ::= SEQUENCE
struct pdcp_params_nr_r15_s {
  struct rohc_context_max_sessions_r15_opts {
    enum options {
      cs2,
      cs4,
      cs8,
      cs12,
      cs16,
      cs24,
      cs32,
      cs48,
      cs64,
      cs128,
      cs256,
      cs512,
      cs1024,
      cs16384,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<rohc_context_max_sessions_r15_opts> rohc_context_max_sessions_r15_e_;
  struct rohc_profiles_ul_only_r15_s_ {
    bool profile0x0006_r15 = false;
  };

  // member variables
  bool                             rohc_context_max_sessions_r15_present         = false;
  bool                             rohc_context_continue_r15_present             = false;
  bool                             out_of_order_delivery_r15_present             = false;
  bool                             sn_size_lo_r15_present                        = false;
  bool                             ims_voice_over_nr_pdcp_mcg_bearer_r15_present = false;
  bool                             ims_voice_over_nr_pdcp_scg_bearer_r15_present = false;
  rohc_profile_support_list_r15_s  rohc_profiles_r15;
  rohc_context_max_sessions_r15_e_ rohc_context_max_sessions_r15;
  rohc_profiles_ul_only_r15_s_     rohc_profiles_ul_only_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v1020 ::= SEQUENCE
struct phy_layer_params_v1020_s {
  bool                                      two_ant_ports_for_pucch_r10_present             = false;
  bool                                      tm9_with_minus8_tx_fdd_r10_present              = false;
  bool                                      pmi_disabling_r10_present                       = false;
  bool                                      cross_carrier_sched_r10_present                 = false;
  bool                                      simul_pucch_pusch_r10_present                   = false;
  bool                                      multi_cluster_pusch_within_cc_r10_present       = false;
  bool                                      non_contiguous_ul_ra_within_cc_list_r10_present = false;
  non_contiguous_ul_ra_within_cc_list_r10_l non_contiguous_ul_ra_within_cc_list_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v1250 ::= SEQUENCE
struct phy_layer_params_v1250_s {
  bool                 e_harq_pattern_fdd_r12_present               = false;
  bool                 enhanced_minus4_tx_codebook_r12_present      = false;
  bool                 tdd_fdd_ca_pcell_duplex_r12_present          = false;
  bool                 phy_tdd_re_cfg_tdd_pcell_r12_present         = false;
  bool                 phy_tdd_re_cfg_fdd_pcell_r12_present         = false;
  bool                 pusch_feedback_mode_r12_present              = false;
  bool                 pusch_srs_pwr_ctrl_sf_set_r12_present        = false;
  bool                 csi_sf_set_r12_present                       = false;
  bool                 no_res_restrict_for_tti_bundling_r12_present = false;
  bool                 discovery_signals_in_deact_scell_r12_present = false;
  bool                 naics_cap_list_r12_present                   = false;
  fixed_bitstring<2>   tdd_fdd_ca_pcell_duplex_r12;
  naics_cap_list_r12_l naics_cap_list_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v1320 ::= SEQUENCE
struct phy_layer_params_v1320_s {
  bool                 mimo_ue_params_r13_present = false;
  mimo_ue_params_r13_s mimo_ue_params_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v1330 ::= SEQUENCE
struct phy_layer_params_v1330_s {
  bool    cch_interf_mitigation_ref_rec_type_a_r13_present = false;
  bool    cch_interf_mitigation_ref_rec_type_b_r13_present = false;
  bool    cch_interf_mitigation_max_num_ccs_r13_present    = false;
  bool    crs_interf_mitigation_tm1to_tm9_r13_present      = false;
  uint8_t cch_interf_mitigation_max_num_ccs_r13            = 1;
  uint8_t crs_interf_mitigation_tm1to_tm9_r13              = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FeMBMS-Unicast-Parameters-r14 ::= SEQUENCE
struct fe_mbms_unicast_params_r14_s {
  bool unicast_fembms_mixed_scell_r14_present = false;
  bool empty_unicast_region_r14_present       = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v1430 ::= SEQUENCE
struct phy_layer_params_v1430_s {
  struct ce_pdsch_pusch_max_bw_r14_opts {
    enum options { bw5, bw20, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ce_pdsch_pusch_max_bw_r14_opts> ce_pdsch_pusch_max_bw_r14_e_;
  struct ce_retuning_symbols_r14_opts {
    enum options { n0, n1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ce_retuning_symbols_r14_opts> ce_retuning_symbols_r14_e_;

  // member variables
  bool                         ce_pusch_nb_max_tbs_r14_present        = false;
  bool                         ce_pdsch_pusch_max_bw_r14_present      = false;
  bool                         ce_harq_ack_bundling_r14_present       = false;
  bool                         ce_pdsch_ten_processes_r14_present     = false;
  bool                         ce_retuning_symbols_r14_present        = false;
  bool                         ce_pdsch_pusch_enhancement_r14_present = false;
  bool                         ce_sched_enhancement_r14_present       = false;
  bool                         ce_srs_enhancement_r14_present         = false;
  bool                         ce_pucch_enhancement_r14_present       = false;
  bool                         ce_closed_loop_tx_ant_sel_r14_present  = false;
  bool                         tdd_special_sf_r14_present             = false;
  bool                         tdd_tti_bundling_r14_present           = false;
  bool                         dmrs_less_up_pts_r14_present           = false;
  bool                         mimo_ue_params_v1430_present           = false;
  bool                         alt_tbs_idx_r14_present                = false;
  bool                         fe_mbms_unicast_params_r14_present     = false;
  ce_pdsch_pusch_max_bw_r14_e_ ce_pdsch_pusch_max_bw_r14;
  ce_retuning_symbols_r14_e_   ce_retuning_symbols_r14;
  mimo_ue_params_v1430_s       mimo_ue_params_v1430;
  fe_mbms_unicast_params_r14_s fe_mbms_unicast_params_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v1470 ::= SEQUENCE
struct phy_layer_params_v1470_s {
  bool                   mimo_ue_params_v1470_present     = false;
  bool                   srs_up_pts_minus6sym_r14_present = false;
  mimo_ue_params_v1470_s mimo_ue_params_v1470;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v1530 ::= SEQUENCE
struct phy_layer_params_v1530_s {
  struct stti_spt_cap_r15_s_ {
    struct max_layers_slot_or_subslot_pusch_r15_opts {
      enum options { one_layer, two_layers, four_layers, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<max_layers_slot_or_subslot_pusch_r15_opts> max_layers_slot_or_subslot_pusch_r15_e_;
    struct sps_stti_r15_opts {
      enum options { slot, subslot, slot_and_subslot, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<sps_stti_r15_opts> sps_stti_r15_e_;

    // member variables
    bool                                    aperiodic_csi_report_stti_r15_present             = false;
    bool                                    dmrs_based_spdcch_mbsfn_r15_present               = false;
    bool                                    dmrs_based_spdcch_non_mbsfn_r15_present           = false;
    bool                                    dmrs_position_pattern_r15_present                 = false;
    bool                                    dmrs_sharing_subslot_pdsch_r15_present            = false;
    bool                                    dmrs_repeat_subslot_pdsch_r15_present             = false;
    bool                                    epdcch_spt_different_cells_r15_present            = false;
    bool                                    epdcch_stti_different_cells_r15_present           = false;
    bool                                    max_layers_slot_or_subslot_pusch_r15_present      = false;
    bool                                    max_num_upd_csi_proc_spt_r15_present              = false;
    bool                                    max_num_upd_csi_proc_stti_comb77_r15_present      = false;
    bool                                    max_num_upd_csi_proc_stti_comb27_r15_present      = false;
    bool                                    max_num_upd_csi_proc_stti_comb22_set1_r15_present = false;
    bool                                    max_num_upd_csi_proc_stti_comb22_set2_r15_present = false;
    bool                                    mimo_ue_params_stti_r15_present                   = false;
    bool                                    mimo_ue_params_stti_v1530_present                 = false;
    bool                                    nof_blind_decodes_uss_r15_present                 = false;
    bool                                    pdsch_slot_subslot_pdsch_decoding_r15_present     = false;
    bool                                    pwr_uci_slot_pusch_present                        = false;
    bool                                    pwr_uci_subslot_pusch_present                     = false;
    bool                                    slot_pdsch_tx_div_tm9and10_present                = false;
    bool                                    subslot_pdsch_tx_div_tm9and10_present             = false;
    bool                                    spdcch_different_rs_types_r15_present             = false;
    bool                                    srs_dci7_trigger_fs2_r15_present                  = false;
    bool                                    sps_cyclic_shift_r15_present                      = false;
    bool                                    spdcch_reuse_r15_present                          = false;
    bool                                    sps_stti_r15_present                              = false;
    bool                                    tm8_slot_pdsch_r15_present                        = false;
    bool                                    tm9_slot_subslot_r15_present                      = false;
    bool                                    tm9_slot_subslot_mbsfn_r15_present                = false;
    bool                                    tm10_slot_subslot_r15_present                     = false;
    bool                                    tm10_slot_subslot_mbsfn_r15_present               = false;
    bool                                    tx_div_spucch_r15_present                         = false;
    bool                                    ul_async_harq_sharing_diff_tti_lens_r15_present   = false;
    max_layers_slot_or_subslot_pusch_r15_e_ max_layers_slot_or_subslot_pusch_r15;
    uint8_t                                 max_num_upd_csi_proc_spt_r15              = 5;
    uint8_t                                 max_num_upd_csi_proc_stti_comb77_r15      = 1;
    uint8_t                                 max_num_upd_csi_proc_stti_comb27_r15      = 1;
    uint8_t                                 max_num_upd_csi_proc_stti_comb22_set1_r15 = 1;
    uint8_t                                 max_num_upd_csi_proc_stti_comb22_set2_r15 = 1;
    mimo_ue_params_r13_s                    mimo_ue_params_stti_r15;
    mimo_ue_params_v1430_s                  mimo_ue_params_stti_v1530;
    uint8_t                                 nof_blind_decodes_uss_r15 = 4;
    sps_stti_r15_e_                         sps_stti_r15;
  };
  struct ce_cap_r15_s_ {
    bool ce_crs_intf_mitig_r15_present                 = false;
    bool ce_cqi_alt_table_r15_present                  = false;
    bool ce_pdsch_flex_start_prb_ce_mode_a_r15_present = false;
    bool ce_pdsch_flex_start_prb_ce_mode_b_r15_present = false;
    bool ce_pdsch_minus64_qam_r15_present              = false;
    bool ce_pusch_flex_start_prb_ce_mode_a_r15_present = false;
    bool ce_pusch_flex_start_prb_ce_mode_b_r15_present = false;
    bool ce_pusch_sub_prb_alloc_r15_present            = false;
    bool ce_ul_harq_ack_feedback_r15_present           = false;
  };
  struct urllc_cap_r15_s_ {
    bool    pdsch_rep_sf_r15_present                  = false;
    bool    pdsch_rep_slot_r15_present                = false;
    bool    pdsch_rep_subslot_r15_present             = false;
    bool    pusch_sps_multi_cfg_sf_r15_present        = false;
    bool    pusch_sps_max_cfg_sf_r15_present          = false;
    bool    pusch_sps_multi_cfg_slot_r15_present      = false;
    bool    pusch_sps_max_cfg_slot_r15_present        = false;
    bool    pusch_sps_multi_cfg_subslot_r15_present   = false;
    bool    pusch_sps_max_cfg_subslot_r15_present     = false;
    bool    pusch_sps_slot_rep_pcell_r15_present      = false;
    bool    pusch_sps_slot_rep_ps_cell_r15_present    = false;
    bool    pusch_sps_slot_rep_scell_r15_present      = false;
    bool    pusch_sps_sf_rep_pcell_r15_present        = false;
    bool    pusch_sps_sf_rep_ps_cell_r15_present      = false;
    bool    pusch_sps_sf_rep_scell_r15_present        = false;
    bool    pusch_sps_subslot_rep_pcell_r15_present   = false;
    bool    pusch_sps_subslot_rep_ps_cell_r15_present = false;
    bool    pusch_sps_subslot_rep_scell_r15_present   = false;
    bool    semi_static_cfi_r15_present               = false;
    bool    semi_static_cfi_pattern_r15_present       = false;
    uint8_t pusch_sps_multi_cfg_sf_r15                = 0;
    uint8_t pusch_sps_max_cfg_sf_r15                  = 0;
    uint8_t pusch_sps_multi_cfg_slot_r15              = 0;
    uint8_t pusch_sps_max_cfg_slot_r15                = 0;
    uint8_t pusch_sps_multi_cfg_subslot_r15           = 0;
    uint8_t pusch_sps_max_cfg_subslot_r15             = 0;
  };

  // member variables
  bool                stti_spt_cap_r15_present                   = false;
  bool                ce_cap_r15_present                         = false;
  bool                short_cqi_for_scell_activation_r15_present = false;
  bool                mimo_cbsr_advanced_csi_r15_present         = false;
  bool                crs_intf_mitig_r15_present                 = false;
  bool                ul_pwr_ctrl_enhance_r15_present            = false;
  bool                urllc_cap_r15_present                      = false;
  bool                alt_mcs_table_r15_present                  = false;
  stti_spt_cap_r15_s_ stti_spt_cap_r15;
  ce_cap_r15_s_       ce_cap_r15;
  urllc_cap_r15_s_    urllc_cap_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandEUTRA ::= SEQUENCE
struct supported_band_eutra_s {
  uint8_t band_eutra  = 1;
  bool    half_duplex = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandListEUTRA ::= SEQUENCE (SIZE (1..64)) OF SupportedBandEUTRA
using supported_band_list_eutra_l = dyn_array<supported_band_eutra_s>;

// RF-Parameters ::= SEQUENCE
struct rf_params_s {
  supported_band_list_eutra_l supported_band_list_eutra;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombination-r10 ::= SEQUENCE (SIZE (1..128)) OF BandCombinationParameters-r10
using supported_band_combination_r10_l = dyn_array<band_combination_params_r10_l>;

// RF-Parameters-v1020 ::= SEQUENCE
struct rf_params_v1020_s {
  supported_band_combination_r10_l supported_band_combination_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombinationExt-r10 ::= SEQUENCE (SIZE (1..128)) OF BandCombinationParametersExt-r10
using supported_band_combination_ext_r10_l = dyn_array<band_combination_params_ext_r10_s>;

// RF-Parameters-v1060 ::= SEQUENCE
struct rf_params_v1060_s {
  supported_band_combination_ext_r10_l supported_band_combination_ext_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombination-v1090 ::= SEQUENCE (SIZE (1..128)) OF BandCombinationParameters-v1090
using supported_band_combination_v1090_l = dyn_array<band_combination_params_v1090_l>;

// RF-Parameters-v1090 ::= SEQUENCE
struct rf_params_v1090_s {
  bool                               supported_band_combination_v1090_present = false;
  supported_band_combination_v1090_l supported_band_combination_v1090;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombination-v10i0 ::= SEQUENCE (SIZE (1..128)) OF BandCombinationParameters-v10i0
using supported_band_combination_v10i0_l = dyn_array<band_combination_params_v10i0_s>;

// RF-Parameters-v10i0 ::= SEQUENCE
struct rf_params_v10i0_s {
  bool                               supported_band_combination_v10i0_present = false;
  supported_band_combination_v10i0_l supported_band_combination_v10i0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombination-v1130 ::= SEQUENCE (SIZE (1..128)) OF BandCombinationParameters-v1130
using supported_band_combination_v1130_l = dyn_array<band_combination_params_v1130_s>;

// RF-Parameters-v1130 ::= SEQUENCE
struct rf_params_v1130_s {
  bool                               supported_band_combination_v1130_present = false;
  supported_band_combination_v1130_l supported_band_combination_v1130;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombinationAdd-r11 ::= SEQUENCE (SIZE (1..256)) OF BandCombinationParameters-r11
using supported_band_combination_add_r11_l = dyn_array<band_combination_params_r11_s>;

// RF-Parameters-v1180 ::= SEQUENCE
struct rf_params_v1180_s {
  using requested_bands_r11_l_ = dyn_array<uint16_t>;

  // member variables
  bool                                 freq_band_retrieval_r11_present            = false;
  bool                                 requested_bands_r11_present                = false;
  bool                                 supported_band_combination_add_r11_present = false;
  requested_bands_r11_l_               requested_bands_r11;
  supported_band_combination_add_r11_l supported_band_combination_add_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombinationAdd-v11d0 ::= SEQUENCE (SIZE (1..256)) OF BandCombinationParameters-v10i0
using supported_band_combination_add_v11d0_l = dyn_array<band_combination_params_v10i0_s>;

// RF-Parameters-v11d0 ::= SEQUENCE
struct rf_params_v11d0_s {
  bool                                   supported_band_combination_add_v11d0_present = false;
  supported_band_combination_add_v11d0_l supported_band_combination_add_v11d0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandEUTRA-v1250 ::= SEQUENCE
struct supported_band_eutra_v1250_s {
  bool dl_minus256_qam_r12_present = false;
  bool ul_minus64_qam_r12_present  = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombination-v1250 ::= SEQUENCE (SIZE (1..128)) OF BandCombinationParameters-v1250
using supported_band_combination_v1250_l = dyn_array<band_combination_params_v1250_s>;

// SupportedBandCombinationAdd-v1250 ::= SEQUENCE (SIZE (1..256)) OF BandCombinationParameters-v1250
using supported_band_combination_add_v1250_l = dyn_array<band_combination_params_v1250_s>;

// SupportedBandListEUTRA-v1250 ::= SEQUENCE (SIZE (1..64)) OF SupportedBandEUTRA-v1250
using supported_band_list_eutra_v1250_l = dyn_array<supported_band_eutra_v1250_s>;

// RF-Parameters-v1250 ::= SEQUENCE
struct rf_params_v1250_s {
  bool                                   supported_band_list_eutra_v1250_present      = false;
  bool                                   supported_band_combination_v1250_present     = false;
  bool                                   supported_band_combination_add_v1250_present = false;
  bool                                   freq_band_prio_adjustment_r12_present        = false;
  supported_band_list_eutra_v1250_l      supported_band_list_eutra_v1250;
  supported_band_combination_v1250_l     supported_band_combination_v1250;
  supported_band_combination_add_v1250_l supported_band_combination_add_v1250;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombination-v1270 ::= SEQUENCE (SIZE (1..128)) OF BandCombinationParameters-v1270
using supported_band_combination_v1270_l = dyn_array<band_combination_params_v1270_s>;

// SupportedBandCombinationAdd-v1270 ::= SEQUENCE (SIZE (1..256)) OF BandCombinationParameters-v1270
using supported_band_combination_add_v1270_l = dyn_array<band_combination_params_v1270_s>;

// RF-Parameters-v1270 ::= SEQUENCE
struct rf_params_v1270_s {
  bool                                   supported_band_combination_v1270_present     = false;
  bool                                   supported_band_combination_add_v1270_present = false;
  supported_band_combination_v1270_l     supported_band_combination_v1270;
  supported_band_combination_add_v1270_l supported_band_combination_add_v1270;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandEUTRA-v1310 ::= SEQUENCE
struct supported_band_eutra_v1310_s {
  bool ue_pwr_class_minus5_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombinationReduced-r13 ::= SEQUENCE (SIZE (1..384)) OF BandCombinationParameters-r13
using supported_band_combination_reduced_r13_l = dyn_array<band_combination_params_r13_s>;

// SupportedBandListEUTRA-v1310 ::= SEQUENCE (SIZE (1..64)) OF SupportedBandEUTRA-v1310
using supported_band_list_eutra_v1310_l = dyn_array<supported_band_eutra_v1310_s>;

// RF-Parameters-v1310 ::= SEQUENCE
struct rf_params_v1310_s {
  struct enb_requested_params_r13_s_ {
    bool    reduced_int_non_cont_comb_requested_r13_present = false;
    bool    requested_ccs_dl_r13_present                    = false;
    bool    requested_ccs_ul_r13_present                    = false;
    bool    skip_fallback_comb_requested_r13_present        = false;
    uint8_t requested_ccs_dl_r13                            = 2;
    uint8_t requested_ccs_ul_r13                            = 2;
  };

  // member variables
  bool                                     enb_requested_params_r13_present               = false;
  bool                                     maximum_ccs_retrieval_r13_present              = false;
  bool                                     skip_fallback_combinations_r13_present         = false;
  bool                                     reduced_int_non_cont_comb_r13_present          = false;
  bool                                     supported_band_list_eutra_v1310_present        = false;
  bool                                     supported_band_combination_reduced_r13_present = false;
  enb_requested_params_r13_s_              enb_requested_params_r13;
  supported_band_list_eutra_v1310_l        supported_band_list_eutra_v1310;
  supported_band_combination_reduced_r13_l supported_band_combination_reduced_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandEUTRA-v1320 ::= SEQUENCE
struct supported_band_eutra_v1320_s {
  struct ue_pwr_class_n_r13_opts {
    enum options { class1, class2, class4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ue_pwr_class_n_r13_opts> ue_pwr_class_n_r13_e_;

  // member variables
  bool                  intra_freq_ce_need_for_gaps_r13_present = false;
  bool                  ue_pwr_class_n_r13_present              = false;
  ue_pwr_class_n_r13_e_ ue_pwr_class_n_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombination-v1320 ::= SEQUENCE (SIZE (1..128)) OF BandCombinationParameters-v1320
using supported_band_combination_v1320_l = dyn_array<band_combination_params_v1320_s>;

// SupportedBandCombinationAdd-v1320 ::= SEQUENCE (SIZE (1..256)) OF BandCombinationParameters-v1320
using supported_band_combination_add_v1320_l = dyn_array<band_combination_params_v1320_s>;

// SupportedBandCombinationReduced-v1320 ::= SEQUENCE (SIZE (1..384)) OF BandCombinationParameters-v1320
using supported_band_combination_reduced_v1320_l = dyn_array<band_combination_params_v1320_s>;

// SupportedBandListEUTRA-v1320 ::= SEQUENCE (SIZE (1..64)) OF SupportedBandEUTRA-v1320
using supported_band_list_eutra_v1320_l = dyn_array<supported_band_eutra_v1320_s>;

// RF-Parameters-v1320 ::= SEQUENCE
struct rf_params_v1320_s {
  bool                                       supported_band_list_eutra_v1320_present          = false;
  bool                                       supported_band_combination_v1320_present         = false;
  bool                                       supported_band_combination_add_v1320_present     = false;
  bool                                       supported_band_combination_reduced_v1320_present = false;
  supported_band_list_eutra_v1320_l          supported_band_list_eutra_v1320;
  supported_band_combination_v1320_l         supported_band_combination_v1320;
  supported_band_combination_add_v1320_l     supported_band_combination_add_v1320;
  supported_band_combination_reduced_v1320_l supported_band_combination_reduced_v1320;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombination-v1380 ::= SEQUENCE (SIZE (1..128)) OF BandCombinationParameters-v1380
using supported_band_combination_v1380_l = dyn_array<band_combination_params_v1380_s>;

// SupportedBandCombinationAdd-v1380 ::= SEQUENCE (SIZE (1..256)) OF BandCombinationParameters-v1380
using supported_band_combination_add_v1380_l = dyn_array<band_combination_params_v1380_s>;

// SupportedBandCombinationReduced-v1380 ::= SEQUENCE (SIZE (1..384)) OF BandCombinationParameters-v1380
using supported_band_combination_reduced_v1380_l = dyn_array<band_combination_params_v1380_s>;

// RF-Parameters-v1380 ::= SEQUENCE
struct rf_params_v1380_s {
  bool                                       supported_band_combination_v1380_present         = false;
  bool                                       supported_band_combination_add_v1380_present     = false;
  bool                                       supported_band_combination_reduced_v1380_present = false;
  supported_band_combination_v1380_l         supported_band_combination_v1380;
  supported_band_combination_add_v1380_l     supported_band_combination_add_v1380;
  supported_band_combination_reduced_v1380_l supported_band_combination_reduced_v1380;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandCombinationParameters-v1390 ::= SEQUENCE
struct band_combination_params_v1390_s {
  bool ue_ca_pwr_class_n_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombination-v1390 ::= SEQUENCE (SIZE (1..128)) OF BandCombinationParameters-v1390
using supported_band_combination_v1390_l = dyn_array<band_combination_params_v1390_s>;

// SupportedBandCombinationAdd-v1390 ::= SEQUENCE (SIZE (1..256)) OF BandCombinationParameters-v1390
using supported_band_combination_add_v1390_l = dyn_array<band_combination_params_v1390_s>;

// SupportedBandCombinationReduced-v1390 ::= SEQUENCE (SIZE (1..384)) OF BandCombinationParameters-v1390
using supported_band_combination_reduced_v1390_l = dyn_array<band_combination_params_v1390_s>;

// RF-Parameters-v1390 ::= SEQUENCE
struct rf_params_v1390_s {
  bool                                       supported_band_combination_v1390_present         = false;
  bool                                       supported_band_combination_add_v1390_present     = false;
  bool                                       supported_band_combination_reduced_v1390_present = false;
  supported_band_combination_v1390_l         supported_band_combination_v1390;
  supported_band_combination_add_v1390_l     supported_band_combination_add_v1390;
  supported_band_combination_reduced_v1390_l supported_band_combination_reduced_v1390;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombination-v1430 ::= SEQUENCE (SIZE (1..128)) OF BandCombinationParameters-v1430
using supported_band_combination_v1430_l = dyn_array<band_combination_params_v1430_s>;

// SupportedBandCombinationAdd-v1430 ::= SEQUENCE (SIZE (1..256)) OF BandCombinationParameters-v1430
using supported_band_combination_add_v1430_l = dyn_array<band_combination_params_v1430_s>;

// SupportedBandCombinationReduced-v1430 ::= SEQUENCE (SIZE (1..384)) OF BandCombinationParameters-v1430
using supported_band_combination_reduced_v1430_l = dyn_array<band_combination_params_v1430_s>;

// RF-Parameters-v1430 ::= SEQUENCE
struct rf_params_v1430_s {
  struct enb_requested_params_v1430_s_ {
    band_combination_list_r14_l requested_diff_fallback_comb_list_r14;
  };

  // member variables
  bool                                       supported_band_combination_v1430_present         = false;
  bool                                       supported_band_combination_add_v1430_present     = false;
  bool                                       supported_band_combination_reduced_v1430_present = false;
  bool                                       enb_requested_params_v1430_present               = false;
  bool                                       diff_fallback_comb_report_r14_present            = false;
  supported_band_combination_v1430_l         supported_band_combination_v1430;
  supported_band_combination_add_v1430_l     supported_band_combination_add_v1430;
  supported_band_combination_reduced_v1430_l supported_band_combination_reduced_v1430;
  enb_requested_params_v1430_s_              enb_requested_params_v1430;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombination-v1450 ::= SEQUENCE (SIZE (1..128)) OF BandCombinationParameters-v1450
using supported_band_combination_v1450_l = dyn_array<band_combination_params_v1450_s>;

// SupportedBandCombinationAdd-v1450 ::= SEQUENCE (SIZE (1..256)) OF BandCombinationParameters-v1450
using supported_band_combination_add_v1450_l = dyn_array<band_combination_params_v1450_s>;

// SupportedBandCombinationReduced-v1450 ::= SEQUENCE (SIZE (1..384)) OF BandCombinationParameters-v1450
using supported_band_combination_reduced_v1450_l = dyn_array<band_combination_params_v1450_s>;

// RF-Parameters-v1450 ::= SEQUENCE
struct rf_params_v1450_s {
  bool                                       supported_band_combination_v1450_present         = false;
  bool                                       supported_band_combination_add_v1450_present     = false;
  bool                                       supported_band_combination_reduced_v1450_present = false;
  supported_band_combination_v1450_l         supported_band_combination_v1450;
  supported_band_combination_add_v1450_l     supported_band_combination_add_v1450;
  supported_band_combination_reduced_v1450_l supported_band_combination_reduced_v1450;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombination-v1470 ::= SEQUENCE (SIZE (1..128)) OF BandCombinationParameters-v1470
using supported_band_combination_v1470_l = dyn_array<band_combination_params_v1470_s>;

// SupportedBandCombinationAdd-v1470 ::= SEQUENCE (SIZE (1..256)) OF BandCombinationParameters-v1470
using supported_band_combination_add_v1470_l = dyn_array<band_combination_params_v1470_s>;

// SupportedBandCombinationReduced-v1470 ::= SEQUENCE (SIZE (1..384)) OF BandCombinationParameters-v1470
using supported_band_combination_reduced_v1470_l = dyn_array<band_combination_params_v1470_s>;

// RF-Parameters-v1470 ::= SEQUENCE
struct rf_params_v1470_s {
  bool                                       supported_band_combination_v1470_present         = false;
  bool                                       supported_band_combination_add_v1470_present     = false;
  bool                                       supported_band_combination_reduced_v1470_present = false;
  supported_band_combination_v1470_l         supported_band_combination_v1470;
  supported_band_combination_add_v1470_l     supported_band_combination_add_v1470;
  supported_band_combination_reduced_v1470_l supported_band_combination_reduced_v1470;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombination-v14b0 ::= SEQUENCE (SIZE (1..128)) OF BandCombinationParameters-v14b0
using supported_band_combination_v14b0_l = dyn_array<band_combination_params_v14b0_s>;

// SupportedBandCombinationAdd-v14b0 ::= SEQUENCE (SIZE (1..256)) OF BandCombinationParameters-v14b0
using supported_band_combination_add_v14b0_l = dyn_array<band_combination_params_v14b0_s>;

// SupportedBandCombinationReduced-v14b0 ::= SEQUENCE (SIZE (1..384)) OF BandCombinationParameters-v14b0
using supported_band_combination_reduced_v14b0_l = dyn_array<band_combination_params_v14b0_s>;

// RF-Parameters-v14b0 ::= SEQUENCE
struct rf_params_v14b0_s {
  bool                                       supported_band_combination_v14b0_present         = false;
  bool                                       supported_band_combination_add_v14b0_present     = false;
  bool                                       supported_band_combination_reduced_v14b0_present = false;
  supported_band_combination_v14b0_l         supported_band_combination_v14b0;
  supported_band_combination_add_v14b0_l     supported_band_combination_add_v14b0;
  supported_band_combination_reduced_v14b0_l supported_band_combination_reduced_v14b0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandCombination-v1530 ::= SEQUENCE (SIZE (1..128)) OF BandCombinationParameters-v1530
using supported_band_combination_v1530_l = dyn_array<band_combination_params_v1530_s>;

// SupportedBandCombinationAdd-v1530 ::= SEQUENCE (SIZE (1..256)) OF BandCombinationParameters-v1530
using supported_band_combination_add_v1530_l = dyn_array<band_combination_params_v1530_s>;

// SupportedBandCombinationReduced-v1530 ::= SEQUENCE (SIZE (1..384)) OF BandCombinationParameters-v1530
using supported_band_combination_reduced_v1530_l = dyn_array<band_combination_params_v1530_s>;

// RF-Parameters-v1530 ::= SEQUENCE
struct rf_params_v1530_s {
  bool                                       stti_spt_supported_r15_present                   = false;
  bool                                       supported_band_combination_v1530_present         = false;
  bool                                       supported_band_combination_add_v1530_present     = false;
  bool                                       supported_band_combination_reduced_v1530_present = false;
  bool                                       pwr_class_minus14dbm_r15_present                 = false;
  supported_band_combination_v1530_l         supported_band_combination_v1530;
  supported_band_combination_add_v1530_l     supported_band_combination_add_v1530;
  supported_band_combination_reduced_v1530_l supported_band_combination_reduced_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandEUTRA-v9e0 ::= SEQUENCE
struct supported_band_eutra_v9e0_s {
  bool     band_eutra_v9e0_present = false;
  uint16_t band_eutra_v9e0         = 65;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandListEUTRA-v9e0 ::= SEQUENCE (SIZE (1..64)) OF SupportedBandEUTRA-v9e0
using supported_band_list_eutra_v9e0_l = dyn_array<supported_band_eutra_v9e0_s>;

// RF-Parameters-v9e0 ::= SEQUENCE
struct rf_params_v9e0_s {
  bool                             supported_band_list_eutra_v9e0_present = false;
  supported_band_list_eutra_v9e0_l supported_band_list_eutra_v9e0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandInfo-r12 ::= SEQUENCE
struct supported_band_info_r12_s {
  bool support_r12_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandInfoList-r12 ::= SEQUENCE (SIZE (1..64)) OF SupportedBandInfo-r12
using supported_band_info_list_r12_l = dyn_array<supported_band_info_r12_s>;

// SL-Parameters-r12 ::= SEQUENCE
struct sl_params_r12_s {
  struct disc_supported_proc_r12_opts {
    enum options { n50, n400, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<disc_supported_proc_r12_opts> disc_supported_proc_r12_e_;

  // member variables
  bool                           comm_simul_tx_r12_present         = false;
  bool                           comm_supported_bands_r12_present  = false;
  bool                           disc_supported_bands_r12_present  = false;
  bool                           disc_sched_res_alloc_r12_present  = false;
  bool                           disc_ue_sel_res_alloc_r12_present = false;
  bool                           disc_slss_r12_present             = false;
  bool                           disc_supported_proc_r12_present   = false;
  freq_band_ind_list_eutra_r12_l comm_supported_bands_r12;
  supported_band_info_list_r12_l disc_supported_bands_r12;
  disc_supported_proc_r12_e_     disc_supported_proc_r12;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// V2X-BandParameters-r14 ::= SEQUENCE
struct v2x_band_params_r14_s {
  bool                    band_params_tx_sl_r14_present = false;
  bool                    band_params_rx_sl_r14_present = false;
  uint16_t                v2x_freq_band_eutra_r14       = 1;
  band_params_tx_sl_r14_s band_params_tx_sl_r14;
  band_params_rx_sl_r14_s band_params_rx_sl_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// V2X-BandCombinationParameters-r14 ::= SEQUENCE (SIZE (1..64)) OF V2X-BandParameters-r14
using v2x_band_combination_params_r14_l = dyn_array<v2x_band_params_r14_s>;

// V2X-SupportedBandCombination-r14 ::= SEQUENCE (SIZE (1..384)) OF V2X-BandCombinationParameters-r14
using v2x_supported_band_combination_r14_l = dyn_array<v2x_band_combination_params_r14_l>;

// SL-Parameters-v1430 ::= SEQUENCE
struct sl_params_v1430_s {
  bool                                 zone_based_pool_sel_r14_present                 = false;
  bool                                 ue_autonomous_with_full_sensing_r14_present     = false;
  bool                                 ue_autonomous_with_partial_sensing_r14_present  = false;
  bool                                 sl_congestion_ctrl_r14_present                  = false;
  bool                                 v2x_tx_with_short_resv_interv_r14_present       = false;
  bool                                 v2x_num_tx_rx_timing_r14_present                = false;
  bool                                 v2x_non_adjacent_pscch_pssch_r14_present        = false;
  bool                                 slss_tx_rx_r14_present                          = false;
  bool                                 v2x_supported_band_combination_list_r14_present = false;
  uint8_t                              v2x_num_tx_rx_timing_r14                        = 1;
  v2x_supported_band_combination_r14_l v2x_supported_band_combination_list_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// V2X-BandParameters-v1530 ::= SEQUENCE
struct v2x_band_params_v1530_s {
  bool v2x_enhanced_high_reception_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// V2X-BandCombinationParameters-v1530 ::= SEQUENCE (SIZE (1..64)) OF V2X-BandParameters-v1530
using v2x_band_combination_params_v1530_l = dyn_array<v2x_band_params_v1530_s>;

// UE-CategorySL-r15 ::= SEQUENCE
struct ue_category_sl_r15_s {
  uint8_t ue_category_sl_c_tx_r15 = 1;
  uint8_t ue_category_sl_c_rx_r15 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// V2X-SupportedBandCombination-v1530 ::= SEQUENCE (SIZE (1..384)) OF V2X-BandCombinationParameters-v1530
using v2x_supported_band_combination_v1530_l = dyn_array<v2x_band_combination_params_v1530_l>;

// SL-Parameters-v1530 ::= SEQUENCE
struct sl_params_v1530_s {
  struct slss_supported_tx_freq_r15_opts {
    enum options { single, multiple, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<slss_supported_tx_freq_r15_opts> slss_supported_tx_freq_r15_e_;

  // member variables
  bool                                   slss_supported_tx_freq_r15_present                = false;
  bool                                   sl_minus64_qam_tx_r15_present                     = false;
  bool                                   sl_tx_diversity_r15_present                       = false;
  bool                                   ue_category_sl_r15_present                        = false;
  bool                                   v2x_supported_band_combination_list_v1530_present = false;
  slss_supported_tx_freq_r15_e_          slss_supported_tx_freq_r15;
  ue_category_sl_r15_s                   ue_category_sl_r15;
  v2x_supported_band_combination_v1530_l v2x_supported_band_combination_list_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeighCellSI-AcquisitionParameters-v15a0 ::= SEQUENCE
struct neigh_cell_si_acquisition_params_v15a0_s {
  bool eutra_cgi_report_nedc_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v1540 ::= SEQUENCE
struct phy_layer_params_v1540_s {
  struct stti_spt_cap_v1540_s_ {};

  // member variables
  bool stti_spt_cap_v1540_present                      = false;
  bool crs_im_tm1_to_tm9_one_rx_port_v1540_present     = false;
  bool cch_im_ref_rec_type_a_one_rx_port_v1540_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v1550 ::= SEQUENCE
struct phy_layer_params_v1550_s {
  bool dmrs_overhead_reduction_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-5GC-Parameters-r15 ::= SEQUENCE
struct eutra_minus5_gc_params_r15_s {
  bool eutra_minus5_gc_r15_present                           = false;
  bool eutra_epc_ho_eutra_minus5_gc_r15_present              = false;
  bool ho_eutra_minus5_gc_fdd_tdd_r15_present                = false;
  bool ho_interfreq_eutra_minus5_gc_r15_present              = false;
  bool ims_voice_over_mcg_bearer_eutra_minus5_gc_r15_present = false;
  bool inactive_state_r15_present                            = false;
  bool reflective_qos_r15_present                            = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-CapabilityAddXDD-Mode-v15a0 ::= SEQUENCE
struct ue_eutra_cap_add_xdd_mode_v15a0_s {
  bool                                     phy_layer_params_v1530_present = false;
  bool                                     phy_layer_params_v1540_present = false;
  bool                                     phy_layer_params_v1550_present = false;
  phy_layer_params_v1530_s                 phy_layer_params_v1530;
  phy_layer_params_v1540_s                 phy_layer_params_v1540;
  phy_layer_params_v1550_s                 phy_layer_params_v1550;
  neigh_cell_si_acquisition_params_v15a0_s neigh_cell_si_acquisition_params_v15a0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IRAT-ParametersNR-v1570 ::= SEQUENCE
struct irat_params_nr_v1570_s {
  bool ss_sinr_meas_nr_fr1_r15_present = false;
  bool ss_sinr_meas_nr_fr2_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-ParametersNR-v1560 ::= SEQUENCE
struct pdcp_params_nr_v1560_s {
  bool ims_vo_nr_pdcp_scg_ngendc_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RF-Parameters-v1570 ::= SEQUENCE
struct rf_params_v1570_s {
  struct dl_minus1024_qam_scaling_factor_r15_opts {
    enum options { v1, v1dot2, v1dot25, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<dl_minus1024_qam_scaling_factor_r15_opts> dl_minus1024_qam_scaling_factor_r15_e_;

  // member variables
  dl_minus1024_qam_scaling_factor_r15_e_ dl_minus1024_qam_scaling_factor_r15;
  uint8_t                                dl_minus1024_qam_total_weighted_layers_r15 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v15a0-IEs ::= SEQUENCE
struct ue_eutra_cap_v15a0_ies_s {
  bool                                     eutra_minus5_gc_params_r15_present = false;
  bool                                     fdd_add_ue_eutra_cap_v15a0_present = false;
  bool                                     tdd_add_ue_eutra_cap_v15a0_present = false;
  bool                                     non_crit_ext_present               = false;
  neigh_cell_si_acquisition_params_v15a0_s neigh_cell_si_acquisition_params_v15a0;
  eutra_minus5_gc_params_r15_s             eutra_minus5_gc_params_r15;
  ue_eutra_cap_add_xdd_mode_v15a0_s        fdd_add_ue_eutra_cap_v15a0;
  ue_eutra_cap_add_xdd_mode_v15a0_s        tdd_add_ue_eutra_cap_v15a0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IRAT-ParametersNR-v1560 ::= SEQUENCE
struct irat_params_nr_v1560_s {
  bool ng_en_dc_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeighCellSI-AcquisitionParameters-v1550 ::= SEQUENCE
struct neigh_cell_si_acquisition_params_v1550_s {
  bool eutra_cgi_report_endc_r15_present      = false;
  bool utra_geran_cgi_report_endc_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1570-IEs ::= SEQUENCE
struct ue_eutra_cap_v1570_ies_s {
  bool                     rf_params_v1570_present      = false;
  bool                     irat_params_nr_v1570_present = false;
  bool                     non_crit_ext_present         = false;
  rf_params_v1570_s        rf_params_v1570;
  irat_params_nr_v1570_s   irat_params_nr_v1570;
  ue_eutra_cap_v15a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-CapabilityAddXDD-Mode-v1560 ::= SEQUENCE
struct ue_eutra_cap_add_xdd_mode_v1560_s {
  pdcp_params_nr_v1560_s pdcp_params_nr_v1560;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-Parameters-v1550 ::= SEQUENCE
struct mac_params_v1550_s {
  bool elcid_support_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1560-IEs ::= SEQUENCE
struct ue_eutra_cap_v1560_ies_s {
  bool                              applied_cap_filt_common_r15_present = false;
  bool                              non_crit_ext_present                = false;
  pdcp_params_nr_v1560_s            pdcp_params_nr_v1560;
  irat_params_nr_v1560_s            irat_params_nr_v1560;
  dyn_octstring                     applied_cap_filt_common_r15;
  ue_eutra_cap_add_xdd_mode_v1560_s fdd_add_ue_eutra_cap_v1560;
  ue_eutra_cap_add_xdd_mode_v1560_s tdd_add_ue_eutra_cap_v1560;
  ue_eutra_cap_v1570_ies_s          non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-CapabilityAddXDD-Mode-v1550 ::= SEQUENCE
struct ue_eutra_cap_add_xdd_mode_v1550_s {
  bool                                     neigh_cell_si_acquisition_params_v1550_present = false;
  neigh_cell_si_acquisition_params_v1550_s neigh_cell_si_acquisition_params_v1550;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeighCellSI-AcquisitionParameters-v1530 ::= SEQUENCE
struct neigh_cell_si_acquisition_params_v1530_s {
  bool report_cgi_nr_en_dc_r15_present    = false;
  bool report_cgi_nr_no_en_dc_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Other-Parameters-v1540 ::= SEQUENCE
struct other_params_v1540_s {
  bool in_dev_coex_ind_endc_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-Parameters-v1540 ::= SEQUENCE
struct sl_params_v1540_s {
  bool sl_minus64_qam_rx_r15_present            = false;
  bool sl_rate_matching_tbs_scaling_r15_present = false;
  bool sl_low_t2min_r15_present                 = false;
  bool v2x_sensing_report_mode3_r15_present     = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1550-IEs ::= SEQUENCE
struct ue_eutra_cap_v1550_ies_s {
  bool                                     neigh_cell_si_acquisition_params_v1550_present = false;
  bool                                     non_crit_ext_present                           = false;
  neigh_cell_si_acquisition_params_v1550_s neigh_cell_si_acquisition_params_v1550;
  phy_layer_params_v1550_s                 phy_layer_params_v1550;
  mac_params_v1550_s                       mac_params_v1550;
  ue_eutra_cap_add_xdd_mode_v1550_s        fdd_add_ue_eutra_cap_v1550;
  ue_eutra_cap_add_xdd_mode_v1550_s        tdd_add_ue_eutra_cap_v1550;
  ue_eutra_cap_v1560_ies_s                 non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-CapabilityAddXDD-Mode-v1540 ::= SEQUENCE
struct ue_eutra_cap_add_xdd_mode_v1540_s {
  bool                         eutra_minus5_gc_params_r15_present = false;
  bool                         irat_params_nr_v1540_present       = false;
  eutra_minus5_gc_params_r15_s eutra_minus5_gc_params_r15;
  irat_params_nr_v1540_s       irat_params_nr_v1540;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LAA-Parameters-v1530 ::= SEQUENCE
struct laa_params_v1530_s {
  bool aul_r15_present             = false;
  bool laa_pusch_mode1_r15_present = false;
  bool laa_pusch_mode2_r15_present = false;
  bool laa_pusch_mode3_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasParameters-v1530 ::= SEQUENCE
struct meas_params_v1530_s {
  bool qoe_meas_report_r15_present            = false;
  bool qoe_mtsi_meas_report_r15_present       = false;
  bool ca_idle_mode_meass_r15_present         = false;
  bool ca_idle_mode_validity_area_r15_present = false;
  bool height_meas_r15_present                = false;
  bool multiple_cells_meas_ext_r15_present    = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Other-Parameters-v1530 ::= SEQUENCE
struct other_params_v1530_s {
  bool assist_info_bit_for_lc_r15_present = false;
  bool time_ref_provision_r15_present     = false;
  bool flight_path_plan_r15_present       = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-Parameters-v1530 ::= SEQUENCE
struct rlc_params_v1530_s {
  bool flex_um_am_combinations_r15_present = false;
  bool rlc_am_ooo_delivery_r15_present     = false;
  bool rlc_um_ooo_delivery_r15_present     = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-BasedNetwPerfMeasParameters-v1530 ::= SEQUENCE
struct ue_based_netw_perf_meas_params_v1530_s {
  bool logged_meas_bt_r15_present   = false;
  bool logged_meas_wlan_r15_present = false;
  bool imm_meas_bt_r15_present      = false;
  bool imm_meas_wlan_r15_present    = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1540-IEs ::= SEQUENCE
struct ue_eutra_cap_v1540_ies_s {
  bool                              phy_layer_params_v1540_present     = false;
  bool                              fdd_add_ue_eutra_cap_v1540_present = false;
  bool                              tdd_add_ue_eutra_cap_v1540_present = false;
  bool                              sl_params_v1540_present            = false;
  bool                              irat_params_nr_v1540_present       = false;
  bool                              non_crit_ext_present               = false;
  phy_layer_params_v1540_s          phy_layer_params_v1540;
  other_params_v1540_s              other_params_v1540;
  ue_eutra_cap_add_xdd_mode_v1540_s fdd_add_ue_eutra_cap_v1540;
  ue_eutra_cap_add_xdd_mode_v1540_s tdd_add_ue_eutra_cap_v1540;
  sl_params_v1540_s                 sl_params_v1540;
  irat_params_nr_v1540_s            irat_params_nr_v1540;
  ue_eutra_cap_v1550_ies_s          non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-CapabilityAddXDD-Mode-v1530 ::= SEQUENCE
struct ue_eutra_cap_add_xdd_mode_v1530_s {
  bool                                     neigh_cell_si_acquisition_params_v1530_present = false;
  bool                                     reduced_cp_latency_r15_present                 = false;
  neigh_cell_si_acquisition_params_v1530_s neigh_cell_si_acquisition_params_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasParameters-v1520 ::= SEQUENCE
struct meas_params_v1520_s {
  bool               meas_gap_patterns_r15_present = false;
  fixed_bitstring<8> meas_gap_patterns_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1530-IEs ::= SEQUENCE
struct ue_eutra_cap_v1530_ies_s {
  bool                                     meas_params_v1530_present                      = false;
  bool                                     other_params_v1530_present                     = false;
  bool                                     neigh_cell_si_acquisition_params_v1530_present = false;
  bool                                     mac_params_v1530_present                       = false;
  bool                                     phy_layer_params_v1530_present                 = false;
  bool                                     rf_params_v1530_present                        = false;
  bool                                     pdcp_params_v1530_present                      = false;
  bool                                     ue_category_dl_v1530_present                   = false;
  bool                                     ue_based_netw_perf_meas_params_v1530_present   = false;
  bool                                     rlc_params_v1530_present                       = false;
  bool                                     sl_params_v1530_present                        = false;
  bool                                     extended_nof_drbs_r15_present                  = false;
  bool                                     reduced_cp_latency_r15_present                 = false;
  bool                                     laa_params_v1530_present                       = false;
  bool                                     ue_category_ul_v1530_present                   = false;
  bool                                     fdd_add_ue_eutra_cap_v1530_present             = false;
  bool                                     tdd_add_ue_eutra_cap_v1530_present             = false;
  bool                                     non_crit_ext_present                           = false;
  meas_params_v1530_s                      meas_params_v1530;
  other_params_v1530_s                     other_params_v1530;
  neigh_cell_si_acquisition_params_v1530_s neigh_cell_si_acquisition_params_v1530;
  mac_params_v1530_s                       mac_params_v1530;
  phy_layer_params_v1530_s                 phy_layer_params_v1530;
  rf_params_v1530_s                        rf_params_v1530;
  pdcp_params_v1530_s                      pdcp_params_v1530;
  uint8_t                                  ue_category_dl_v1530 = 22;
  ue_based_netw_perf_meas_params_v1530_s   ue_based_netw_perf_meas_params_v1530;
  rlc_params_v1530_s                       rlc_params_v1530;
  sl_params_v1530_s                        sl_params_v1530;
  laa_params_v1530_s                       laa_params_v1530;
  uint8_t                                  ue_category_ul_v1530 = 22;
  ue_eutra_cap_add_xdd_mode_v1530_s        fdd_add_ue_eutra_cap_v1530;
  ue_eutra_cap_add_xdd_mode_v1530_s        tdd_add_ue_eutra_cap_v1530;
  ue_eutra_cap_v1540_ies_s                 non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1520-IEs ::= SEQUENCE
struct ue_eutra_cap_v1520_ies_s {
  bool                     non_crit_ext_present = false;
  meas_params_v1520_s      meas_params_v1520;
  ue_eutra_cap_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-CapabilityAddXDD-Mode-v1510 ::= SEQUENCE
struct ue_eutra_cap_add_xdd_mode_v1510_s {
  bool                 pdcp_params_nr_r15_present = false;
  pdcp_params_nr_r15_s pdcp_params_nr_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Other-Parameters-v1460 ::= SEQUENCE
struct other_params_v1460_s {
  bool non_csg_si_report_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1510-IEs ::= SEQUENCE
struct ue_eutra_cap_v1510_ies_s {
  bool                              irat_params_nr_r15_present         = false;
  bool                              feature_sets_eutra_r15_present     = false;
  bool                              pdcp_params_nr_r15_present         = false;
  bool                              fdd_add_ue_eutra_cap_v1510_present = false;
  bool                              tdd_add_ue_eutra_cap_v1510_present = false;
  bool                              non_crit_ext_present               = false;
  irat_params_nr_r15_s              irat_params_nr_r15;
  feature_sets_eutra_r15_s          feature_sets_eutra_r15;
  pdcp_params_nr_r15_s              pdcp_params_nr_r15;
  ue_eutra_cap_add_xdd_mode_v1510_s fdd_add_ue_eutra_cap_v1510;
  ue_eutra_cap_add_xdd_mode_v1510_s tdd_add_ue_eutra_cap_v1510;
  ue_eutra_cap_v1520_ies_s          non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OtherParameters-v1450 ::= SEQUENCE
struct other_params_v1450_s {
  bool overheat_ind_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v1450 ::= SEQUENCE
struct phy_layer_params_v1450_s {
  bool ce_srs_enhancement_without_comb4_r14_present = false;
  bool crs_less_dw_pts_r14_present                  = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1460-IEs ::= SEQUENCE
struct ue_eutra_cap_v1460_ies_s {
  bool                     ue_category_dl_v1460_present = false;
  bool                     non_crit_ext_present         = false;
  uint8_t                  ue_category_dl_v1460         = 21;
  other_params_v1460_s     other_params_v1460;
  ue_eutra_cap_v1510_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LWA-Parameters-v1440 ::= SEQUENCE
struct lwa_params_v1440_s {
  bool lwa_rlc_um_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-Parameters-v1440 ::= SEQUENCE
struct mac_params_v1440_s {
  bool rai_support_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MMTEL-Parameters-r14 ::= SEQUENCE
struct mmtel_params_r14_s {
  bool delay_budget_report_r14_present        = false;
  bool pusch_enhance_r14_present              = false;
  bool recommended_bit_rate_r14_present       = false;
  bool recommended_bit_rate_query_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1450-IEs ::= SEQUENCE
struct ue_eutra_cap_v1450_ies_s {
  bool                     phy_layer_params_v1450_present = false;
  bool                     rf_params_v1450_present        = false;
  bool                     ue_category_dl_v1450_present   = false;
  bool                     non_crit_ext_present           = false;
  phy_layer_params_v1450_s phy_layer_params_v1450;
  rf_params_v1450_s        rf_params_v1450;
  other_params_v1450_s     other_params_v1450;
  uint8_t                  ue_category_dl_v1450 = 20;
  ue_eutra_cap_v1460_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CE-Parameters-v1430 ::= SEQUENCE
struct ce_params_v1430_s {
  bool ce_switch_without_ho_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HighSpeedEnhParameters-r14 ::= SEQUENCE
struct high_speed_enh_params_r14_s {
  bool meas_enhance_r14_present  = false;
  bool demod_enhance_r14_present = false;
  bool prach_enhance_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LAA-Parameters-v1430 ::= SEQUENCE
struct laa_params_v1430_s {
  struct two_step_sched_timing_info_r14_opts {
    enum options { nplus1, nplus2, nplus3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<two_step_sched_timing_info_r14_opts> two_step_sched_timing_info_r14_e_;

  // member variables
  bool                              cross_carrier_sched_laa_ul_r14_present    = false;
  bool                              ul_laa_r14_present                        = false;
  bool                              two_step_sched_timing_info_r14_present    = false;
  bool                              uss_blind_decoding_adjustment_r14_present = false;
  bool                              uss_blind_decoding_reduction_r14_present  = false;
  bool                              out_of_seq_grant_handling_r14_present     = false;
  two_step_sched_timing_info_r14_e_ two_step_sched_timing_info_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LWA-Parameters-v1430 ::= SEQUENCE
struct lwa_params_v1430_s {
  bool     lwa_ho_without_wt_change_r14_present = false;
  bool     lwa_ul_r14_present                   = false;
  bool     wlan_periodic_meas_r14_present       = false;
  bool     wlan_report_any_wlan_r14_present     = false;
  bool     wlan_supported_data_rate_r14_present = false;
  uint16_t wlan_supported_data_rate_r14         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LWIP-Parameters-v1430 ::= SEQUENCE
struct lwip_params_v1430_s {
  bool lwip_aggregation_dl_r14_present = false;
  bool lwip_aggregation_ul_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-Parameters-v1430 ::= SEQUENCE
struct mac_params_v1430_s {
  bool short_sps_interv_fdd_r14_present = false;
  bool short_sps_interv_tdd_r14_present = false;
  bool skip_ul_dynamic_r14_present      = false;
  bool skip_ul_sps_r14_present          = false;
  bool multiple_ul_sps_r14_present      = false;
  bool data_inact_mon_r14_present       = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMS-Parameters-v1430 ::= SEQUENCE
struct mbms_params_v1430_s {
  bool fembms_ded_cell_r14_present                   = false;
  bool fembms_mixed_cell_r14_present                 = false;
  bool subcarrier_spacing_mbms_khz7dot5_r14_present  = false;
  bool subcarrier_spacing_mbms_khz1dot25_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasParameters-v1430 ::= SEQUENCE
struct meas_params_v1430_s {
  bool ce_meass_r14_present                  = false;
  bool ncsg_r14_present                      = false;
  bool short_meas_gap_r14_present            = false;
  bool per_serving_cell_meas_gap_r14_present = false;
  bool non_uniform_gap_r14_present           = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MobilityParameters-r14 ::= SEQUENCE
struct mob_params_r14_s {
  bool make_before_break_r14_present = false;
  bool rach_less_r14_present         = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Other-Parameters-v1430 ::= SEQUENCE
struct other_params_v1430_s {
  bool bw_pref_ind_r14_present        = false;
  bool rlm_report_support_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-Parameters-v1430 ::= SEQUENCE
struct pdcp_params_v1430_s {
  struct supported_ul_only_rohc_profiles_r14_s_ {
    bool profile0x0006_r14 = false;
  };
  struct max_num_rohc_context_sessions_r14_opts {
    enum options {
      cs2,
      cs4,
      cs8,
      cs12,
      cs16,
      cs24,
      cs32,
      cs48,
      cs64,
      cs128,
      cs256,
      cs512,
      cs1024,
      cs16384,
      spare2,
      spare1,
      nulltype
    } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<max_num_rohc_context_sessions_r14_opts> max_num_rohc_context_sessions_r14_e_;

  // member variables
  bool                                   max_num_rohc_context_sessions_r14_present = false;
  supported_ul_only_rohc_profiles_r14_s_ supported_ul_only_rohc_profiles_r14;
  max_num_rohc_context_sessions_r14_e_   max_num_rohc_context_sessions_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-Parameters-v1430 ::= SEQUENCE
struct rlc_params_v1430_s {
  bool extended_poll_byte_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-BasedNetwPerfMeasParameters-v1430 ::= SEQUENCE
struct ue_based_netw_perf_meas_params_v1430_s {
  bool location_report_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1440-IEs ::= SEQUENCE
struct ue_eutra_cap_v1440_ies_s {
  bool                     non_crit_ext_present = false;
  lwa_params_v1440_s       lwa_params_v1440;
  mac_params_v1440_s       mac_params_v1440;
  ue_eutra_cap_v1450_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-CapabilityAddXDD-Mode-v1430 ::= SEQUENCE
struct ue_eutra_cap_add_xdd_mode_v1430_s {
  bool                     phy_layer_params_v1430_present = false;
  bool                     mmtel_params_r14_present       = false;
  phy_layer_params_v1430_s phy_layer_params_v1430;
  mmtel_params_r14_s       mmtel_params_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Other-Parameters-v1360 ::= SEQUENCE
struct other_params_v1360_s {
  bool in_dev_coex_ind_hardware_sharing_ind_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1430-IEs ::= SEQUENCE
struct ue_eutra_cap_v1430_ies_s {
  struct ue_category_ul_v1430_opts {
    enum options { n16, n17, n18, n19, n20, m2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ue_category_ul_v1430_opts> ue_category_ul_v1430_e_;

  // member variables
  bool                                   ue_category_dl_v1430_present                 = false;
  bool                                   ue_category_ul_v1430_present                 = false;
  bool                                   ue_category_ul_v1430b_present                = false;
  bool                                   mac_params_v1430_present                     = false;
  bool                                   meas_params_v1430_present                    = false;
  bool                                   pdcp_params_v1430_present                    = false;
  bool                                   rf_params_v1430_present                      = false;
  bool                                   laa_params_v1430_present                     = false;
  bool                                   lwa_params_v1430_present                     = false;
  bool                                   lwip_params_v1430_present                    = false;
  bool                                   mmtel_params_r14_present                     = false;
  bool                                   mob_params_r14_present                       = false;
  bool                                   fdd_add_ue_eutra_cap_v1430_present           = false;
  bool                                   tdd_add_ue_eutra_cap_v1430_present           = false;
  bool                                   mbms_params_v1430_present                    = false;
  bool                                   sl_params_v1430_present                      = false;
  bool                                   ue_based_netw_perf_meas_params_v1430_present = false;
  bool                                   high_speed_enh_params_r14_present            = false;
  bool                                   non_crit_ext_present                         = false;
  phy_layer_params_v1430_s               phy_layer_params_v1430;
  ue_category_ul_v1430_e_                ue_category_ul_v1430;
  mac_params_v1430_s                     mac_params_v1430;
  meas_params_v1430_s                    meas_params_v1430;
  pdcp_params_v1430_s                    pdcp_params_v1430;
  rlc_params_v1430_s                     rlc_params_v1430;
  rf_params_v1430_s                      rf_params_v1430;
  laa_params_v1430_s                     laa_params_v1430;
  lwa_params_v1430_s                     lwa_params_v1430;
  lwip_params_v1430_s                    lwip_params_v1430;
  other_params_v1430_s                   other_params_v1430;
  mmtel_params_r14_s                     mmtel_params_r14;
  mob_params_r14_s                       mob_params_r14;
  ce_params_v1430_s                      ce_params_v1430;
  ue_eutra_cap_add_xdd_mode_v1430_s      fdd_add_ue_eutra_cap_v1430;
  ue_eutra_cap_add_xdd_mode_v1430_s      tdd_add_ue_eutra_cap_v1430;
  mbms_params_v1430_s                    mbms_params_v1430;
  sl_params_v1430_s                      sl_params_v1430;
  ue_based_netw_perf_meas_params_v1430_s ue_based_netw_perf_meas_params_v1430;
  high_speed_enh_params_r14_s            high_speed_enh_params_r14;
  ue_eutra_cap_v1440_ies_s               non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CE-Parameters-v1350 ::= SEQUENCE
struct ce_params_v1350_s {
  bool unicast_freq_hop_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1360-IEs ::= SEQUENCE
struct ue_eutra_cap_v1360_ies_s {
  bool                     other_params_v1360_present = false;
  bool                     non_crit_ext_present       = false;
  other_params_v1360_s     other_params_v1360;
  ue_eutra_cap_v1430_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1350-IEs ::= SEQUENCE
struct ue_eutra_cap_v1350_ies_s {
  bool                     ue_category_dl_v1350_present = false;
  bool                     ue_category_ul_v1350_present = false;
  bool                     non_crit_ext_present         = false;
  ce_params_v1350_s        ce_params_v1350;
  ue_eutra_cap_v1360_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCPTM-Parameters-r13 ::= SEQUENCE
struct scptm_params_r13_s {
  bool scptm_parallel_reception_r13_present = false;
  bool scptm_scell_r13_present              = false;
  bool scptm_non_serving_cell_r13_present   = false;
  bool scptm_async_dc_r13_present           = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1340-IEs ::= SEQUENCE
struct ue_eutra_cap_v1340_ies_s {
  bool                     ue_category_ul_v1340_present = false;
  bool                     non_crit_ext_present         = false;
  uint8_t                  ue_category_ul_v1340         = 15;
  ue_eutra_cap_v1350_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CE-Parameters-v1320 ::= SEQUENCE
struct ce_params_v1320_s {
  bool intra_freq_a3_ce_mode_a_r13_present = false;
  bool intra_freq_a3_ce_mode_b_r13_present = false;
  bool intra_freq_ho_ce_mode_a_r13_present = false;
  bool intra_freq_ho_ce_mode_b_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v1310 ::= SEQUENCE
struct phy_layer_params_v1310_s {
  struct supported_blind_decoding_r13_s_ {
    bool    max_num_decoding_r13_present                     = false;
    bool    pdcch_candidate_reductions_r13_present           = false;
    bool    skip_monitoring_dci_format0_minus1_a_r13_present = false;
    uint8_t max_num_decoding_r13                             = 1;
  };

  // member variables
  bool                            aperiodic_csi_report_r13_present       = false;
  bool                            codebook_harq_ack_r13_present          = false;
  bool                            cross_carrier_sched_b5_c_r13_present   = false;
  bool                            fdd_harq_timing_tdd_r13_present        = false;
  bool                            max_num_upd_csi_proc_r13_present       = false;
  bool                            pucch_format4_r13_present              = false;
  bool                            pucch_format5_r13_present              = false;
  bool                            pucch_scell_r13_present                = false;
  bool                            spatial_bundling_harq_ack_r13_present  = false;
  bool                            supported_blind_decoding_r13_present   = false;
  bool                            uci_pusch_ext_r13_present              = false;
  bool                            crs_interf_mitigation_tm10_r13_present = false;
  bool                            pdsch_collision_handling_r13_present   = false;
  fixed_bitstring<2>              aperiodic_csi_report_r13;
  fixed_bitstring<2>              codebook_harq_ack_r13;
  uint8_t                         max_num_upd_csi_proc_r13 = 5;
  supported_blind_decoding_r13_s_ supported_blind_decoding_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1330-IEs ::= SEQUENCE
struct ue_eutra_cap_v1330_ies_s {
  bool                     ue_category_dl_v1330_present   = false;
  bool                     phy_layer_params_v1330_present = false;
  bool                     ue_ce_need_ul_gaps_r13_present = false;
  bool                     non_crit_ext_present           = false;
  uint8_t                  ue_category_dl_v1330           = 18;
  phy_layer_params_v1330_s phy_layer_params_v1330;
  ue_eutra_cap_v1340_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-CapabilityAddXDD-Mode-v1320 ::= SEQUENCE
struct ue_eutra_cap_add_xdd_mode_v1320_s {
  bool                     phy_layer_params_v1320_present = false;
  bool                     scptm_params_r13_present       = false;
  phy_layer_params_v1320_s phy_layer_params_v1320;
  scptm_params_r13_s       scptm_params_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CE-Parameters-r13 ::= SEQUENCE
struct ce_params_r13_s {
  bool ce_mode_a_r13_present = false;
  bool ce_mode_b_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DC-Parameters-v1310 ::= SEQUENCE
struct dc_params_v1310_s {
  bool pdcp_transfer_split_ul_r13_present = false;
  bool ue_sstd_meas_r13_present           = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LAA-Parameters-r13 ::= SEQUENCE
struct laa_params_r13_s {
  bool cross_carrier_sched_laa_dl_r13_present = false;
  bool csi_rs_drs_rrm_meass_laa_r13_present   = false;
  bool dl_laa_r13_present                     = false;
  bool ending_dw_pts_r13_present              = false;
  bool second_slot_start_position_r13_present = false;
  bool tm9_laa_r13_present                    = false;
  bool tm10_laa_r13_present                   = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LWA-Parameters-r13 ::= SEQUENCE
struct lwa_params_r13_s {
  bool               lwa_r13_present              = false;
  bool               lwa_split_bearer_r13_present = false;
  bool               wlan_mac_address_r13_present = false;
  bool               lwa_buffer_size_r13_present  = false;
  fixed_octstring<6> wlan_mac_address_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LWIP-Parameters-r13 ::= SEQUENCE
struct lwip_params_r13_s {
  bool lwip_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-Parameters-v1310 ::= SEQUENCE
struct mac_params_v1310_s {
  bool extended_mac_len_field_r13_present = false;
  bool extended_long_drx_r13_present      = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasParameters-v1310 ::= SEQUENCE
struct meas_params_v1310_s {
  bool rs_sinr_meas_r13_present                 = false;
  bool white_cell_list_r13_present              = false;
  bool extended_max_obj_id_r13_present          = false;
  bool ul_pdcp_delay_r13_present                = false;
  bool extended_freq_priorities_r13_present     = false;
  bool multi_band_info_report_r13_present       = false;
  bool rssi_and_ch_occupancy_report_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-Parameters-v1310 ::= SEQUENCE
struct pdcp_params_v1310_s {
  bool pdcp_sn_ext_minus18bits_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-Parameters-v1310 ::= SEQUENCE
struct rlc_params_v1310_s {
  bool extended_rlc_sn_so_field_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SL-Parameters-v1310 ::= SEQUENCE
struct sl_params_v1310_s {
  bool disc_sys_info_report_r13_present = false;
  bool comm_multiple_tx_r13_present     = false;
  bool disc_inter_freq_tx_r13_present   = false;
  bool disc_periodic_slss_r13_present   = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1320-IEs ::= SEQUENCE
struct ue_eutra_cap_v1320_ies_s {
  bool                              ce_params_v1320_present            = false;
  bool                              phy_layer_params_v1320_present     = false;
  bool                              rf_params_v1320_present            = false;
  bool                              fdd_add_ue_eutra_cap_v1320_present = false;
  bool                              tdd_add_ue_eutra_cap_v1320_present = false;
  bool                              non_crit_ext_present               = false;
  ce_params_v1320_s                 ce_params_v1320;
  phy_layer_params_v1320_s          phy_layer_params_v1320;
  rf_params_v1320_s                 rf_params_v1320;
  ue_eutra_cap_add_xdd_mode_v1320_s fdd_add_ue_eutra_cap_v1320;
  ue_eutra_cap_add_xdd_mode_v1320_s tdd_add_ue_eutra_cap_v1320;
  ue_eutra_cap_v1330_ies_s          non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-CapabilityAddXDD-Mode-v1310 ::= SEQUENCE
struct ue_eutra_cap_add_xdd_mode_v1310_s {
  bool                     phy_layer_params_v1310_present = false;
  phy_layer_params_v1310_s phy_layer_params_v1310;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WLAN-IW-Parameters-v1310 ::= SEQUENCE
struct wlan_iw_params_v1310_s {
  bool rclwi_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v1280 ::= SEQUENCE
struct phy_layer_params_v1280_s {
  bool alt_tbs_indices_r12_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1310-IEs ::= SEQUENCE
struct ue_eutra_cap_v1310_ies_s {
  struct ue_category_dl_v1310_opts {
    enum options { n17, m1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ue_category_dl_v1310_opts> ue_category_dl_v1310_e_;
  struct ue_category_ul_v1310_opts {
    enum options { n14, m1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ue_category_ul_v1310_opts> ue_category_ul_v1310_e_;

  // member variables
  bool                              ue_category_dl_v1310_present       = false;
  bool                              ue_category_ul_v1310_present       = false;
  bool                              mac_params_v1310_present           = false;
  bool                              phy_layer_params_v1310_present     = false;
  bool                              rf_params_v1310_present            = false;
  bool                              meas_params_v1310_present          = false;
  bool                              dc_params_v1310_present            = false;
  bool                              sl_params_v1310_present            = false;
  bool                              scptm_params_r13_present           = false;
  bool                              ce_params_r13_present              = false;
  bool                              laa_params_r13_present             = false;
  bool                              lwa_params_r13_present             = false;
  bool                              fdd_add_ue_eutra_cap_v1310_present = false;
  bool                              tdd_add_ue_eutra_cap_v1310_present = false;
  bool                              non_crit_ext_present               = false;
  ue_category_dl_v1310_e_           ue_category_dl_v1310;
  ue_category_ul_v1310_e_           ue_category_ul_v1310;
  pdcp_params_v1310_s               pdcp_params_v1310;
  rlc_params_v1310_s                rlc_params_v1310;
  mac_params_v1310_s                mac_params_v1310;
  phy_layer_params_v1310_s          phy_layer_params_v1310;
  rf_params_v1310_s                 rf_params_v1310;
  meas_params_v1310_s               meas_params_v1310;
  dc_params_v1310_s                 dc_params_v1310;
  sl_params_v1310_s                 sl_params_v1310;
  scptm_params_r13_s                scptm_params_r13;
  ce_params_r13_s                   ce_params_r13;
  irat_params_wlan_r13_s            inter_rat_params_wlan_r13;
  laa_params_r13_s                  laa_params_r13;
  lwa_params_r13_s                  lwa_params_r13;
  wlan_iw_params_v1310_s            wlan_iw_params_v1310;
  lwip_params_r13_s                 lwip_params_r13;
  ue_eutra_cap_add_xdd_mode_v1310_s fdd_add_ue_eutra_cap_v1310;
  ue_eutra_cap_add_xdd_mode_v1310_s tdd_add_ue_eutra_cap_v1310;
  ue_eutra_cap_v1320_ies_s          non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1280-IEs ::= SEQUENCE
struct ue_eutra_cap_v1280_ies_s {
  bool                     phy_layer_params_v1280_present = false;
  bool                     non_crit_ext_present           = false;
  phy_layer_params_v1280_s phy_layer_params_v1280;
  ue_eutra_cap_v1310_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasParameters-v1250 ::= SEQUENCE
struct meas_params_v1250_s {
  bool timer_t312_r12_present                    = false;
  bool alt_time_to_trigger_r12_present           = false;
  bool inc_mon_eutra_r12_present                 = false;
  bool inc_mon_utra_r12_present                  = false;
  bool extended_max_meas_id_r12_present          = false;
  bool extended_rsrq_lower_range_r12_present     = false;
  bool rsrq_on_all_symbols_r12_present           = false;
  bool crs_discovery_signals_meas_r12_present    = false;
  bool csi_rs_discovery_signals_meas_r12_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1270-IEs ::= SEQUENCE
struct ue_eutra_cap_v1270_ies_s {
  bool                     rf_params_v1270_present = false;
  bool                     non_crit_ext_present    = false;
  rf_params_v1270_s        rf_params_v1270;
  ue_eutra_cap_v1280_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DC-Parameters-r12 ::= SEQUENCE
struct dc_params_r12_s {
  bool drb_type_split_r12_present = false;
  bool drb_type_scg_r12_present   = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-Parameters-r12 ::= SEQUENCE
struct mac_params_r12_s {
  bool lc_ch_sr_prohibit_timer_r12_present = false;
  bool long_drx_cmd_r12_present            = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMS-Parameters-v1250 ::= SEQUENCE
struct mbms_params_v1250_s {
  bool mbms_async_dc_r12_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-Parameters-r12 ::= SEQUENCE
struct rlc_params_r12_s {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-BasedNetwPerfMeasParameters-v1250 ::= SEQUENCE
struct ue_based_netw_perf_meas_params_v1250_s {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1260-IEs ::= SEQUENCE
struct ue_eutra_cap_v1260_ies_s {
  bool                     ue_category_dl_v1260_present = false;
  bool                     non_crit_ext_present         = false;
  uint8_t                  ue_category_dl_v1260         = 15;
  ue_eutra_cap_v1270_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-CapabilityAddXDD-Mode-v1250 ::= SEQUENCE
struct ue_eutra_cap_add_xdd_mode_v1250_s {
  bool                     phy_layer_params_v1250_present = false;
  bool                     meas_params_v1250_present      = false;
  phy_layer_params_v1250_s phy_layer_params_v1250;
  meas_params_v1250_s      meas_params_v1250;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WLAN-IW-Parameters-r12 ::= SEQUENCE
struct wlan_iw_params_r12_s {
  bool wlan_iw_ran_rules_r12_present      = false;
  bool wlan_iw_andsf_policies_r12_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMS-Parameters-r11 ::= SEQUENCE
struct mbms_params_r11_s {
  bool mbms_scell_r11_present            = false;
  bool mbms_non_serving_cell_r11_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasParameters-v11a0 ::= SEQUENCE
struct meas_params_v11a0_s {
  bool benefits_from_interruption_r11_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1250-IEs ::= SEQUENCE
struct ue_eutra_cap_v1250_ies_s {
  bool                                   phy_layer_params_v1250_present               = false;
  bool                                   rf_params_v1250_present                      = false;
  bool                                   rlc_params_r12_present                       = false;
  bool                                   ue_based_netw_perf_meas_params_v1250_present = false;
  bool                                   ue_category_dl_r12_present                   = false;
  bool                                   ue_category_ul_r12_present                   = false;
  bool                                   wlan_iw_params_r12_present                   = false;
  bool                                   meas_params_v1250_present                    = false;
  bool                                   dc_params_r12_present                        = false;
  bool                                   mbms_params_v1250_present                    = false;
  bool                                   mac_params_r12_present                       = false;
  bool                                   fdd_add_ue_eutra_cap_v1250_present           = false;
  bool                                   tdd_add_ue_eutra_cap_v1250_present           = false;
  bool                                   sl_params_r12_present                        = false;
  bool                                   non_crit_ext_present                         = false;
  phy_layer_params_v1250_s               phy_layer_params_v1250;
  rf_params_v1250_s                      rf_params_v1250;
  rlc_params_r12_s                       rlc_params_r12;
  ue_based_netw_perf_meas_params_v1250_s ue_based_netw_perf_meas_params_v1250;
  uint8_t                                ue_category_dl_r12 = 0;
  uint8_t                                ue_category_ul_r12 = 0;
  wlan_iw_params_r12_s                   wlan_iw_params_r12;
  meas_params_v1250_s                    meas_params_v1250;
  dc_params_r12_s                        dc_params_r12;
  mbms_params_v1250_s                    mbms_params_v1250;
  mac_params_r12_s                       mac_params_r12;
  ue_eutra_cap_add_xdd_mode_v1250_s      fdd_add_ue_eutra_cap_v1250;
  ue_eutra_cap_add_xdd_mode_v1250_s      tdd_add_ue_eutra_cap_v1250;
  sl_params_r12_s                        sl_params_r12;
  ue_eutra_cap_v1260_ies_s               non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v11a0-IEs ::= SEQUENCE
struct ue_eutra_cap_v11a0_ies_s {
  bool                     ue_category_v11a0_present = false;
  bool                     meas_params_v11a0_present = false;
  bool                     non_crit_ext_present      = false;
  uint8_t                  ue_category_v11a0         = 11;
  meas_params_v11a0_s      meas_params_v11a0;
  ue_eutra_cap_v1250_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-CapabilityAddXDD-Mode-v1180 ::= SEQUENCE
struct ue_eutra_cap_add_xdd_mode_v1180_s {
  mbms_params_r11_s mbms_params_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasParameters-v1130 ::= SEQUENCE
struct meas_params_v1130_s {
  bool rsrq_meas_wideband_r11_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Other-Parameters-r11 ::= SEQUENCE
struct other_params_r11_s {
  bool in_dev_coex_ind_r11_present          = false;
  bool pwr_pref_ind_r11_present             = false;
  bool ue_rx_tx_time_diff_meass_r11_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v1130 ::= SEQUENCE
struct phy_layer_params_v1130_s {
  bool crs_interf_handl_r11_present         = false;
  bool epdcch_r11_present                   = false;
  bool multi_ack_csi_report_r11_present     = false;
  bool ss_cch_interf_handl_r11_present      = false;
  bool tdd_special_sf_r11_present           = false;
  bool tx_div_pucch1b_ch_select_r11_present = false;
  bool ul_co_mp_r11_present                 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v1170 ::= SEQUENCE
struct phy_layer_params_v1170_s {
  bool               inter_band_tdd_ca_with_different_cfg_r11_present = false;
  fixed_bitstring<2> inter_band_tdd_ca_with_different_cfg_r11;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1180-IEs ::= SEQUENCE
struct ue_eutra_cap_v1180_ies_s {
  bool                              rf_params_v1180_present            = false;
  bool                              mbms_params_r11_present            = false;
  bool                              fdd_add_ue_eutra_cap_v1180_present = false;
  bool                              tdd_add_ue_eutra_cap_v1180_present = false;
  bool                              non_crit_ext_present               = false;
  rf_params_v1180_s                 rf_params_v1180;
  mbms_params_r11_s                 mbms_params_r11;
  ue_eutra_cap_add_xdd_mode_v1180_s fdd_add_ue_eutra_cap_v1180;
  ue_eutra_cap_add_xdd_mode_v1180_s tdd_add_ue_eutra_cap_v1180;
  ue_eutra_cap_v11a0_ies_s          non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IRAT-ParametersCDMA2000-v1130 ::= SEQUENCE
struct irat_params_cdma2000_v1130_s {
  bool cdma2000_nw_sharing_r11_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-Parameters-v1130 ::= SEQUENCE
struct pdcp_params_v1130_s {
  bool pdcp_sn_ext_r11_present                   = false;
  bool support_rohc_context_continue_r11_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1170-IEs ::= SEQUENCE
struct ue_eutra_cap_v1170_ies_s {
  bool                     phy_layer_params_v1170_present = false;
  bool                     ue_category_v1170_present      = false;
  bool                     non_crit_ext_present           = false;
  phy_layer_params_v1170_s phy_layer_params_v1170;
  uint8_t                  ue_category_v1170 = 9;
  ue_eutra_cap_v1180_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-CapabilityAddXDD-Mode-v1130 ::= SEQUENCE
struct ue_eutra_cap_add_xdd_mode_v1130_s {
  bool                     ext                            = false;
  bool                     phy_layer_params_v1130_present = false;
  bool                     meas_params_v1130_present      = false;
  bool                     other_params_r11_present       = false;
  phy_layer_params_v1130_s phy_layer_params_v1130;
  meas_params_v1130_s      meas_params_v1130;
  other_params_r11_s       other_params_r11;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IRAT-ParametersCDMA2000-1XRTT-v1020 ::= SEQUENCE
struct irat_params_cdma2000_minus1_xrtt_v1020_s {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IRAT-ParametersUTRA-TDD-v1020 ::= SEQUENCE
struct irat_params_utra_tdd_v1020_s {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OTDOA-PositioningCapabilities-r10 ::= SEQUENCE
struct otdoa_positioning_cap_r10_s {
  bool inter_freq_rstd_meas_r10_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1130-IEs ::= SEQUENCE
struct ue_eutra_cap_v1130_ies_s {
  bool                              phy_layer_params_v1130_present     = false;
  bool                              fdd_add_ue_eutra_cap_v1130_present = false;
  bool                              tdd_add_ue_eutra_cap_v1130_present = false;
  bool                              non_crit_ext_present               = false;
  pdcp_params_v1130_s               pdcp_params_v1130;
  phy_layer_params_v1130_s          phy_layer_params_v1130;
  rf_params_v1130_s                 rf_params_v1130;
  meas_params_v1130_s               meas_params_v1130;
  irat_params_cdma2000_v1130_s      inter_rat_params_cdma2000_v1130;
  other_params_r11_s                other_params_r11;
  ue_eutra_cap_add_xdd_mode_v1130_s fdd_add_ue_eutra_cap_v1130;
  ue_eutra_cap_add_xdd_mode_v1130_s tdd_add_ue_eutra_cap_v1130;
  ue_eutra_cap_v1170_ies_s          non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1090-IEs ::= SEQUENCE
struct ue_eutra_cap_v1090_ies_s {
  bool                     rf_params_v1090_present = false;
  bool                     non_crit_ext_present    = false;
  rf_params_v1090_s        rf_params_v1090;
  ue_eutra_cap_v1130_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-CapabilityAddXDD-Mode-v1060 ::= SEQUENCE
struct ue_eutra_cap_add_xdd_mode_v1060_s {
  bool                                     ext                                     = false;
  bool                                     phy_layer_params_v1060_present          = false;
  bool                                     feature_group_ind_rel10_v1060_present   = false;
  bool                                     inter_rat_params_cdma2000_v1060_present = false;
  bool                                     inter_rat_params_utra_tdd_v1060_present = false;
  phy_layer_params_v1020_s                 phy_layer_params_v1060;
  fixed_bitstring<32>                      feature_group_ind_rel10_v1060;
  irat_params_cdma2000_minus1_xrtt_v1020_s inter_rat_params_cdma2000_v1060;
  irat_params_utra_tdd_v1020_s             inter_rat_params_utra_tdd_v1060;
  // ...
  // group 0
  copy_ptr<otdoa_positioning_cap_r10_s> otdoa_positioning_cap_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-BasedNetwPerfMeasParameters-r10 ::= SEQUENCE
struct ue_based_netw_perf_meas_params_r10_s {
  bool logged_meass_idle_r10_present        = false;
  bool standalone_gnss_location_r10_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1060-IEs ::= SEQUENCE
struct ue_eutra_cap_v1060_ies_s {
  bool                              fdd_add_ue_eutra_cap_v1060_present = false;
  bool                              tdd_add_ue_eutra_cap_v1060_present = false;
  bool                              rf_params_v1060_present            = false;
  bool                              non_crit_ext_present               = false;
  ue_eutra_cap_add_xdd_mode_v1060_s fdd_add_ue_eutra_cap_v1060;
  ue_eutra_cap_add_xdd_mode_v1060_s tdd_add_ue_eutra_cap_v1060;
  rf_params_v1060_s                 rf_params_v1060;
  ue_eutra_cap_v1090_ies_s          non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1020-IEs ::= SEQUENCE
struct ue_eutra_cap_v1020_ies_s {
  bool                                     ue_category_v1020_present                  = false;
  bool                                     phy_layer_params_v1020_present             = false;
  bool                                     rf_params_v1020_present                    = false;
  bool                                     meas_params_v1020_present                  = false;
  bool                                     feature_group_ind_rel10_r10_present        = false;
  bool                                     inter_rat_params_cdma2000_v1020_present    = false;
  bool                                     ue_based_netw_perf_meas_params_r10_present = false;
  bool                                     inter_rat_params_utra_tdd_v1020_present    = false;
  bool                                     non_crit_ext_present                       = false;
  uint8_t                                  ue_category_v1020                          = 6;
  phy_layer_params_v1020_s                 phy_layer_params_v1020;
  rf_params_v1020_s                        rf_params_v1020;
  meas_params_v1020_s                      meas_params_v1020;
  fixed_bitstring<32>                      feature_group_ind_rel10_r10;
  irat_params_cdma2000_minus1_xrtt_v1020_s inter_rat_params_cdma2000_v1020;
  ue_based_netw_perf_meas_params_r10_s     ue_based_netw_perf_meas_params_r10;
  irat_params_utra_tdd_v1020_s             inter_rat_params_utra_tdd_v1020;
  ue_eutra_cap_v1060_ies_s                 non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CSG-ProximityIndicationParameters-r9 ::= SEQUENCE
struct csg_proximity_ind_params_r9_s {
  bool intra_freq_proximity_ind_r9_present = false;
  bool inter_freq_proximity_ind_r9_present = false;
  bool utran_proximity_ind_r9_present      = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IRAT-ParametersCDMA2000-1XRTT-v920 ::= SEQUENCE
struct irat_params_cdma2000_minus1_xrtt_v920_s {
  bool e_csfb_conc_ps_mob1_xrtt_r9_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IRAT-ParametersGERAN-v920 ::= SEQUENCE
struct irat_params_geran_v920_s {
  bool dtm_r9_present                 = false;
  bool e_redirection_geran_r9_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IRAT-ParametersUTRA-v920 ::= SEQUENCE
struct irat_params_utra_v920_s {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NeighCellSI-AcquisitionParameters-r9 ::= SEQUENCE
struct neigh_cell_si_acquisition_params_r9_s {
  bool intra_freq_si_acquisition_for_ho_r9_present = false;
  bool inter_freq_si_acquisition_for_ho_r9_present = false;
  bool utran_si_acquisition_for_ho_r9_present      = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v920 ::= SEQUENCE
struct phy_layer_params_v920_s {
  bool enhanced_dual_layer_fdd_r9_present = false;
  bool enhanced_dual_layer_tdd_r9_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SON-Parameters-r9 ::= SEQUENCE
struct son_params_r9_s {
  bool rach_report_r9_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v940-IEs ::= SEQUENCE
struct ue_eutra_cap_v940_ies_s {
  bool                     late_non_crit_ext_present = false;
  bool                     non_crit_ext_present      = false;
  dyn_octstring            late_non_crit_ext;
  ue_eutra_cap_v1020_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AccessStratumRelease ::= ENUMERATED
struct access_stratum_release_opts {
  enum options { rel8, rel9, rel10, rel11, rel12, rel13, rel14, rel15, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<access_stratum_release_opts, true> access_stratum_release_e;

// PhyLayerParameters ::= SEQUENCE
struct phy_layer_params_s {
  bool ue_tx_ant_sel_supported        = false;
  bool ue_specific_ref_sigs_supported = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v920-IEs ::= SEQUENCE
struct ue_eutra_cap_v920_ies_s {
  bool                                    inter_rat_params_utra_v920_present     = false;
  bool                                    inter_rat_params_cdma2000_v920_present = false;
  bool                                    dev_type_r9_present                    = false;
  bool                                    non_crit_ext_present                   = false;
  phy_layer_params_v920_s                 phy_layer_params_v920;
  irat_params_geran_v920_s                inter_rat_params_geran_v920;
  irat_params_utra_v920_s                 inter_rat_params_utra_v920;
  irat_params_cdma2000_minus1_xrtt_v920_s inter_rat_params_cdma2000_v920;
  csg_proximity_ind_params_r9_s           csg_proximity_ind_params_r9;
  neigh_cell_si_acquisition_params_r9_s   neigh_cell_si_acquisition_params_r9;
  son_params_r9_s                         son_params_r9;
  ue_eutra_cap_v940_ies_s                 non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability ::= SEQUENCE
struct ue_eutra_cap_s {
  struct inter_rat_params_s_ {
    bool                               utra_fdd_present             = false;
    bool                               utra_tdd128_present          = false;
    bool                               utra_tdd384_present          = false;
    bool                               utra_tdd768_present          = false;
    bool                               geran_present                = false;
    bool                               cdma2000_hrpd_present        = false;
    bool                               cdma2000_minus1x_rtt_present = false;
    irat_params_utra_fdd_s             utra_fdd;
    irat_params_utra_tdd128_s          utra_tdd128;
    irat_params_utra_tdd384_s          utra_tdd384;
    irat_params_utra_tdd768_s          utra_tdd768;
    irat_params_geran_s                geran;
    irat_params_cdma2000_hrpd_s        cdma2000_hrpd;
    irat_params_cdma2000_minus1_xrtt_s cdma2000_minus1x_rtt;
  };

  // member variables
  bool                     feature_group_inds_present = false;
  bool                     non_crit_ext_present       = false;
  access_stratum_release_e access_stratum_release;
  uint8_t                  ue_category = 1;
  pdcp_params_s            pdcp_params;
  phy_layer_params_s       phy_layer_params;
  rf_params_s              rf_params;
  meas_params_s            meas_params;
  fixed_bitstring<32>      feature_group_inds;
  inter_rat_params_s_      inter_rat_params;
  ue_eutra_cap_v920_ies_s  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v14a0 ::= SEQUENCE
struct phy_layer_params_v14a0_s {
  bool ssp10_tdd_only_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v14b0-IEs ::= SEQUENCE
struct ue_eutra_cap_v14b0_ies_s {
  bool              rf_params_v14b0_present = false;
  bool              non_crit_ext_present    = false;
  rf_params_v14b0_s rf_params_v14b0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMS-Parameters-v1470 ::= SEQUENCE
struct mbms_params_v1470_s {
  struct mbms_max_bw_r14_c_ {
    struct types_opts {
      enum options { implicit_value, explicit_value, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    mbms_max_bw_r14_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& explicit_value()
    {
      assert_choice_type(types::explicit_value, type_, "mbms-MaxBW-r14");
      return c;
    }
    const uint8_t& explicit_value() const
    {
      assert_choice_type(types::explicit_value, type_, "mbms-MaxBW-r14");
      return c;
    }
    void     set_implicit_value();
    uint8_t& set_explicit_value();

  private:
    types   type_;
    uint8_t c;
  };
  struct mbms_scaling_factor1dot25_r14_opts {
    enum options { n3, n6, n9, n12, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<mbms_scaling_factor1dot25_r14_opts> mbms_scaling_factor1dot25_r14_e_;
  struct mbms_scaling_factor7dot5_r14_opts {
    enum options { n1, n2, n3, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<mbms_scaling_factor7dot5_r14_opts> mbms_scaling_factor7dot5_r14_e_;

  // member variables
  bool                             mbms_scaling_factor1dot25_r14_present = false;
  bool                             mbms_scaling_factor7dot5_r14_present  = false;
  mbms_max_bw_r14_c_               mbms_max_bw_r14;
  mbms_scaling_factor1dot25_r14_e_ mbms_scaling_factor1dot25_r14;
  mbms_scaling_factor7dot5_r14_e_  mbms_scaling_factor7dot5_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v14a0-IEs ::= SEQUENCE
struct ue_eutra_cap_v14a0_ies_s {
  bool                     non_crit_ext_present = false;
  phy_layer_params_v14a0_s phy_layer_params_v14a0;
  ue_eutra_cap_v14b0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1470-IEs ::= SEQUENCE
struct ue_eutra_cap_v1470_ies_s {
  bool                     mbms_params_v1470_present      = false;
  bool                     phy_layer_params_v1470_present = false;
  bool                     rf_params_v1470_present        = false;
  bool                     non_crit_ext_present           = false;
  mbms_params_v1470_s      mbms_params_v1470;
  phy_layer_params_v1470_s phy_layer_params_v1470;
  rf_params_v1470_s        rf_params_v1470;
  ue_eutra_cap_v14a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CE-Parameters-v1380 ::= SEQUENCE
struct ce_params_v1380_s {
  bool tm6_ce_mode_a_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v13e0a-IEs ::= SEQUENCE
struct ue_eutra_cap_v13e0a_ies_s {
  bool                     late_non_crit_ext_present = false;
  bool                     non_crit_ext_present      = false;
  dyn_octstring            late_non_crit_ext;
  ue_eutra_cap_v1470_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CE-Parameters-v1370 ::= SEQUENCE
struct ce_params_v1370_s {
  bool tm9_ce_mode_a_r13_present = false;
  bool tm9_ce_mode_b_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1390-IEs ::= SEQUENCE
struct ue_eutra_cap_v1390_ies_s {
  bool                      rf_params_v1390_present = false;
  bool                      non_crit_ext_present    = false;
  rf_params_v1390_s         rf_params_v1390;
  ue_eutra_cap_v13e0a_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-CapabilityAddXDD-Mode-v1380 ::= SEQUENCE
struct ue_eutra_cap_add_xdd_mode_v1380_s {
  ce_params_v1380_s ce_params_v1380;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1380-IEs ::= SEQUENCE
struct ue_eutra_cap_v1380_ies_s {
  bool                              rf_params_v1380_present = false;
  bool                              non_crit_ext_present    = false;
  rf_params_v1380_s                 rf_params_v1380;
  ce_params_v1380_s                 ce_params_v1380;
  ue_eutra_cap_add_xdd_mode_v1380_s fdd_add_ue_eutra_cap_v1380;
  ue_eutra_cap_add_xdd_mode_v1380_s tdd_add_ue_eutra_cap_v1380;
  ue_eutra_cap_v1390_ies_s          non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-CapabilityAddXDD-Mode-v1370 ::= SEQUENCE
struct ue_eutra_cap_add_xdd_mode_v1370_s {
  bool              ce_params_v1370_present = false;
  ce_params_v1370_s ce_params_v1370;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v1370-IEs ::= SEQUENCE
struct ue_eutra_cap_v1370_ies_s {
  bool                              ce_params_v1370_present            = false;
  bool                              fdd_add_ue_eutra_cap_v1370_present = false;
  bool                              tdd_add_ue_eutra_cap_v1370_present = false;
  bool                              non_crit_ext_present               = false;
  ce_params_v1370_s                 ce_params_v1370;
  ue_eutra_cap_add_xdd_mode_v1370_s fdd_add_ue_eutra_cap_v1370;
  ue_eutra_cap_add_xdd_mode_v1370_s tdd_add_ue_eutra_cap_v1370;
  ue_eutra_cap_v1380_ies_s          non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RF-Parameters-v12b0 ::= SEQUENCE
struct rf_params_v12b0_s {
  bool max_layers_mimo_ind_r12_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v12x0-IEs ::= SEQUENCE
struct ue_eutra_cap_v12x0_ies_s {
  bool                     late_non_crit_ext_present = false;
  bool                     non_crit_ext_present      = false;
  dyn_octstring            late_non_crit_ext;
  ue_eutra_cap_v1370_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v12b0-IEs ::= SEQUENCE
struct ue_eutra_cap_v12b0_ies_s {
  bool                     rf_params_v12b0_present = false;
  bool                     non_crit_ext_present    = false;
  rf_params_v12b0_s        rf_params_v12b0;
  ue_eutra_cap_v12x0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Other-Parameters-v11d0 ::= SEQUENCE
struct other_params_v11d0_s {
  bool in_dev_coex_ind_ul_ca_r11_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v11x0-IEs ::= SEQUENCE
struct ue_eutra_cap_v11x0_ies_s {
  bool                     late_non_crit_ext_present = false;
  bool                     non_crit_ext_present      = false;
  dyn_octstring            late_non_crit_ext;
  ue_eutra_cap_v12b0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v11d0-IEs ::= SEQUENCE
struct ue_eutra_cap_v11d0_ies_s {
  bool                     rf_params_v11d0_present    = false;
  bool                     other_params_v11d0_present = false;
  bool                     non_crit_ext_present       = false;
  rf_params_v11d0_s        rf_params_v11d0;
  other_params_v11d0_s     other_params_v11d0;
  ue_eutra_cap_v11x0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RF-Parameters-v10f0 ::= SEQUENCE
struct rf_params_v10f0_s {
  bool                modified_mpr_behavior_r10_present = false;
  fixed_bitstring<32> modified_mpr_behavior_r10;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v10i0-IEs ::= SEQUENCE
struct ue_eutra_cap_v10i0_ies_s {
  bool                     rf_params_v10i0_present   = false;
  bool                     late_non_crit_ext_present = false;
  bool                     non_crit_ext_present      = false;
  rf_params_v10i0_s        rf_params_v10i0;
  dyn_octstring            late_non_crit_ext;
  ue_eutra_cap_v11d0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v10f0-IEs ::= SEQUENCE
struct ue_eutra_cap_v10f0_ies_s {
  bool                     rf_params_v10f0_present = false;
  bool                     non_crit_ext_present    = false;
  rf_params_v10f0_s        rf_params_v10f0;
  ue_eutra_cap_v10i0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v10c0-IEs ::= SEQUENCE
struct ue_eutra_cap_v10c0_ies_s {
  bool                        otdoa_positioning_cap_r10_present = false;
  bool                        non_crit_ext_present              = false;
  otdoa_positioning_cap_r10_s otdoa_positioning_cap_r10;
  ue_eutra_cap_v10f0_ies_s    non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RF-Parameters-v10j0 ::= SEQUENCE
struct rf_params_v10j0_s {
  bool multi_ns_pmax_r10_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v10j0-IEs ::= SEQUENCE
struct ue_eutra_cap_v10j0_ies_s {
  bool              rf_params_v10j0_present = false;
  bool              non_crit_ext_present    = false;
  rf_params_v10j0_s rf_params_v10j0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IRAT-ParametersUTRA-v9h0 ::= SEQUENCE
struct irat_params_utra_v9h0_s {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v9h0-IEs ::= SEQUENCE
struct ue_eutra_cap_v9h0_ies_s {
  bool                     inter_rat_params_utra_v9h0_present = false;
  bool                     late_non_crit_ext_present          = false;
  bool                     non_crit_ext_present               = false;
  irat_params_utra_v9h0_s  inter_rat_params_utra_v9h0;
  dyn_octstring            late_non_crit_ext;
  ue_eutra_cap_v10c0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-v9d0 ::= SEQUENCE
struct phy_layer_params_v9d0_s {
  bool tm5_fdd_r9_present = false;
  bool tm5_tdd_r9_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v9e0-IEs ::= SEQUENCE
struct ue_eutra_cap_v9e0_ies_s {
  bool                    rf_params_v9e0_present = false;
  bool                    non_crit_ext_present   = false;
  rf_params_v9e0_s        rf_params_v9e0;
  ue_eutra_cap_v9h0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IRAT-ParametersUTRA-v9c0 ::= SEQUENCE
struct irat_params_utra_v9c0_s {
  bool voice_over_ps_hs_utra_fdd_r9_present             = false;
  bool voice_over_ps_hs_utra_tdd128_r9_present          = false;
  bool srvcc_from_utra_fdd_to_utra_fdd_r9_present       = false;
  bool srvcc_from_utra_fdd_to_geran_r9_present          = false;
  bool srvcc_from_utra_tdd128_to_utra_tdd128_r9_present = false;
  bool srvcc_from_utra_tdd128_to_geran_r9_present       = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v9d0-IEs ::= SEQUENCE
struct ue_eutra_cap_v9d0_ies_s {
  bool                    phy_layer_params_v9d0_present = false;
  bool                    non_crit_ext_present          = false;
  phy_layer_params_v9d0_s phy_layer_params_v9d0;
  ue_eutra_cap_v9e0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v9c0-IEs ::= SEQUENCE
struct ue_eutra_cap_v9c0_ies_s {
  bool                    inter_rat_params_utra_v9c0_present = false;
  bool                    non_crit_ext_present               = false;
  irat_params_utra_v9c0_s inter_rat_params_utra_v9c0;
  ue_eutra_cap_v9d0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-CapabilityAddXDD-Mode-r9 ::= SEQUENCE
struct ue_eutra_cap_add_xdd_mode_r9_s {
  bool                                    ext                                         = false;
  bool                                    phy_layer_params_r9_present                 = false;
  bool                                    feature_group_inds_r9_present               = false;
  bool                                    feature_group_ind_rel9_add_r9_present       = false;
  bool                                    inter_rat_params_geran_r9_present           = false;
  bool                                    inter_rat_params_utra_r9_present            = false;
  bool                                    inter_rat_params_cdma2000_r9_present        = false;
  bool                                    neigh_cell_si_acquisition_params_r9_present = false;
  phy_layer_params_s                      phy_layer_params_r9;
  fixed_bitstring<32>                     feature_group_inds_r9;
  fixed_bitstring<32>                     feature_group_ind_rel9_add_r9;
  irat_params_geran_s                     inter_rat_params_geran_r9;
  irat_params_utra_v920_s                 inter_rat_params_utra_r9;
  irat_params_cdma2000_minus1_xrtt_v920_s inter_rat_params_cdma2000_r9;
  neigh_cell_si_acquisition_params_r9_s   neigh_cell_si_acquisition_params_r9;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-EUTRA-Capability-v9a0-IEs ::= SEQUENCE
struct ue_eutra_cap_v9a0_ies_s {
  bool                           feature_group_ind_rel9_add_r9_present = false;
  bool                           fdd_add_ue_eutra_cap_r9_present       = false;
  bool                           tdd_add_ue_eutra_cap_r9_present       = false;
  bool                           non_crit_ext_present                  = false;
  fixed_bitstring<32>            feature_group_ind_rel9_add_r9;
  ue_eutra_cap_add_xdd_mode_r9_s fdd_add_ue_eutra_cap_r9;
  ue_eutra_cap_add_xdd_mode_r9_s tdd_add_ue_eutra_cap_r9;
  ue_eutra_cap_v9c0_ies_s        non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioAccessCapabilityInformation-r8-IEs ::= SEQUENCE
struct ue_radio_access_cap_info_r8_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring ue_radio_access_cap_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioAccessCapabilityInformation ::= SEQUENCE
struct ue_radio_access_cap_info_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options {
          ue_radio_access_cap_info_r8,
          spare7,
          spare6,
          spare5,
          spare4,
          spare3,
          spare2,
          spare1,
          nulltype
        } value;

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
      ue_radio_access_cap_info_r8_ies_s& ue_radio_access_cap_info_r8()
      {
        assert_choice_type(types::ue_radio_access_cap_info_r8, type_, "c1");
        return c;
      }
      const ue_radio_access_cap_info_r8_ies_s& ue_radio_access_cap_info_r8() const
      {
        assert_choice_type(types::ue_radio_access_cap_info_r8, type_, "c1");
        return c;
      }
      ue_radio_access_cap_info_r8_ies_s& set_ue_radio_access_cap_info_r8();
      void                               set_spare7();
      void                               set_spare6();
      void                               set_spare5();
      void                               set_spare4();
      void                               set_spare3();
      void                               set_spare2();
      void                               set_spare1();

    private:
      types                             type_;
      ue_radio_access_cap_info_r8_ies_s c;
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

} // namespace rrc
} // namespace asn1

#endif // SRSASN1_RRC_UECAP_H
