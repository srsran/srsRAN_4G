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

#ifndef SRSASN1_RRC_NBIOT_H
#define SRSASN1_RRC_NBIOT_H

#include "rrc.h"

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// AB-Config-NB-r13 ::= SEQUENCE
struct ab_cfg_nb_r13_s {
  struct ab_category_r13_opts {
    enum options { a, b, c, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<ab_category_r13_opts> ab_category_r13_e_;

  // member variables
  bool                ab_barr_for_exception_data_r13_present = false;
  ab_category_r13_e_  ab_category_r13;
  fixed_bitstring<10> ab_barr_bitmap_r13;
  fixed_bitstring<5>  ab_barr_for_special_ac_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AB-ConfigPLMN-NB-r13 ::= SEQUENCE
struct ab_cfg_plmn_nb_r13_s {
  bool            ab_cfg_r13_present = false;
  ab_cfg_nb_r13_s ab_cfg_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ANR-ExcludedCellList-NB-r16 ::= SEQUENCE (SIZE (1..16)) OF INTEGER (0..503)
using anr_excluded_cell_list_nb_r16_l = bounded_array<uint16_t, 16>;

// ANR-Carrier-NB-r16 ::= SEQUENCE
struct anr_carrier_nb_r16_s {
  bool                            ext                            = false;
  bool                            excluded_cell_list_r16_present = false;
  uint8_t                         carrier_freq_idx_r16           = 1;
  anr_excluded_cell_list_nb_r16_l excluded_cell_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ANR-CarrierList-NB-r16 ::= SEQUENCE (SIZE (1..2)) OF ANR-Carrier-NB-r16
using anr_carrier_list_nb_r16_l = dyn_array<anr_carrier_nb_r16_s>;

// ANR-MeasConfig-NB-r16 ::= SEQUENCE
struct anr_meas_cfg_nb_r16_s {
  bool                      ext                   = false;
  uint8_t                   anr_quality_thres_r16 = 0;
  anr_carrier_list_nb_r16_l anr_carrier_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreq-NB-r13 ::= SEQUENCE
struct carrier_freq_nb_r13_s {
  struct carrier_freq_offset_r13_opts {
    enum options {
      v_minus10,
      v_minus9,
      v_minus8,
      v_minus7,
      v_minus6,
      v_minus5,
      v_minus4,
      v_minus3,
      v_minus2,
      v_minus1,
      v_minus0dot5,
      v0,
      v1,
      v2,
      v3,
      v4,
      v5,
      v6,
      v7,
      v8,
      v9,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<carrier_freq_offset_r13_opts> carrier_freq_offset_r13_e_;

  // member variables
  bool                       carrier_freq_offset_r13_present = false;
  uint32_t                   carrier_freq_r13                = 0;
  carrier_freq_offset_r13_e_ carrier_freq_offset_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasResultServCell-NB-r14 ::= SEQUENCE
struct meas_result_serv_cell_nb_r14_s {
  uint8_t nrsrp_result_r14 = 0;
  int8_t  nrsrq_result_r14 = -30;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ANR-MeasResult-NB-r16 ::= SEQUENCE
struct anr_meas_result_nb_r16_s {
  struct cgi_info_r16_s_ {
    bool                   plmn_id_list_r16_present = false;
    cell_global_id_eutra_s cell_global_id_r16;
    fixed_bitstring<16>    tac_r16;
    plmn_id_list2_l        plmn_id_list_r16;
  };

  // member variables
  bool                           pci_r16_present         = false;
  bool                           meas_result_r16_present = false;
  bool                           cgi_info_r16_present    = false;
  carrier_freq_nb_r13_s          carrier_freq_r16;
  uint16_t                       pci_r16 = 0;
  meas_result_serv_cell_nb_r14_s meas_result_last_serv_cell_r16;
  uint8_t                        meas_result_r16 = 0;
  cgi_info_r16_s_                cgi_info_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ANR-MeasReport-NB-r16 ::= SEQUENCE
struct anr_meas_report_nb_r16_s {
  using meas_result_list_r16_l_ = dyn_array<anr_meas_result_nb_r16_s>;

  // member variables
  bool                           ext                      = false;
  bool                           serv_cell_id_r16_present = false;
  cell_global_id_eutra_s         serv_cell_id_r16;
  meas_result_serv_cell_nb_r14_s meas_result_serv_cell_r16;
  uint8_t                        relative_time_stamp_r16 = 0;
  meas_result_list_r16_l_        meas_result_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// T-PollRetransmit-NB-r13 ::= ENUMERATED
struct t_poll_retx_nb_r13_opts {
  enum options {
    ms250,
    ms500,
    ms1000,
    ms2000,
    ms3000,
    ms4000,
    ms6000,
    ms10000,
    ms15000,
    ms25000,
    ms40000,
    ms60000,
    ms90000,
    ms120000,
    ms180000,
    ms300000_v1530,
    nulltype
  } value;
  typedef uint32_t number_type;

  const char* to_string() const;
  uint32_t    to_number() const;
};
typedef enumerated<t_poll_retx_nb_r13_opts> t_poll_retx_nb_r13_e;

// CarrierFreq-NB-v1550 ::= SEQUENCE
struct carrier_freq_nb_v1550_s {
  struct carrier_freq_offset_v1550_opts {
    enum options { v_minus8dot5, v_minus4dot5, v3dot5, v7dot5, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<carrier_freq_offset_v1550_opts> carrier_freq_offset_v1550_e_;

  // member variables
  carrier_freq_offset_v1550_e_ carrier_freq_offset_v1550;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-AM-RLC-NB-r13 ::= SEQUENCE
struct dl_am_rlc_nb_r13_s {
  bool enable_status_report_sn_gap_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-Bitmap-NB-r13 ::= CHOICE
struct dl_bitmap_nb_r13_c {
  struct types_opts {
    enum options { sf_pattern10_r13, sf_pattern40_r13, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  dl_bitmap_nb_r13_c() = default;
  dl_bitmap_nb_r13_c(const dl_bitmap_nb_r13_c& other);
  dl_bitmap_nb_r13_c& operator=(const dl_bitmap_nb_r13_c& other);
  ~dl_bitmap_nb_r13_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<10>& sf_pattern10_r13()
  {
    assert_choice_type(types::sf_pattern10_r13, type_, "DL-Bitmap-NB-r13");
    return c.get<fixed_bitstring<10> >();
  }
  fixed_bitstring<40>& sf_pattern40_r13()
  {
    assert_choice_type(types::sf_pattern40_r13, type_, "DL-Bitmap-NB-r13");
    return c.get<fixed_bitstring<40> >();
  }
  const fixed_bitstring<10>& sf_pattern10_r13() const
  {
    assert_choice_type(types::sf_pattern10_r13, type_, "DL-Bitmap-NB-r13");
    return c.get<fixed_bitstring<10> >();
  }
  const fixed_bitstring<40>& sf_pattern40_r13() const
  {
    assert_choice_type(types::sf_pattern40_r13, type_, "DL-Bitmap-NB-r13");
    return c.get<fixed_bitstring<40> >();
  }
  fixed_bitstring<10>& set_sf_pattern10_r13();
  fixed_bitstring<40>& set_sf_pattern40_r13();

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<40> > c;

  void destroy_();
};

// DL-GapConfig-NB-r13 ::= SEQUENCE
struct dl_gap_cfg_nb_r13_s {
  struct dl_gap_thres_r13_opts {
    enum options { n32, n64, n128, n256, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<dl_gap_thres_r13_opts> dl_gap_thres_r13_e_;
  struct dl_gap_periodicity_r13_opts {
    enum options { sf64, sf128, sf256, sf512, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<dl_gap_periodicity_r13_opts> dl_gap_periodicity_r13_e_;
  struct dl_gap_dur_coeff_r13_opts {
    enum options { one_eighth, one_fourth, three_eighth, one_half, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<dl_gap_dur_coeff_r13_opts> dl_gap_dur_coeff_r13_e_;

  // member variables
  dl_gap_thres_r13_e_       dl_gap_thres_r13;
  dl_gap_periodicity_r13_e_ dl_gap_periodicity_r13;
  dl_gap_dur_coeff_r13_e_   dl_gap_dur_coeff_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-GapConfig-NB-v1530 ::= SEQUENCE
struct dl_gap_cfg_nb_v1530_s {
  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UL-DL-AlignmentOffset-NB-r15 ::= ENUMERATED
struct tdd_ul_dl_align_offset_nb_r15_opts {
  enum options { khz_minus7dot5, khz0, khz7dot5, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
typedef enumerated<tdd_ul_dl_align_offset_nb_r15_opts> tdd_ul_dl_align_offset_nb_r15_e;

// UL-AM-RLC-NB-r13 ::= SEQUENCE
struct ul_am_rlc_nb_r13_s {
  struct max_retx_thres_r13_opts {
    enum options { t1, t2, t3, t4, t6, t8, t16, t32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_retx_thres_r13_opts> max_retx_thres_r13_e_;

  // member variables
  t_poll_retx_nb_r13_e  t_poll_retx_r13;
  max_retx_thres_r13_e_ max_retx_thres_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ACK-NACK-NumRepetitions-NB-r13 ::= ENUMERATED
struct ack_nack_num_repeats_nb_r13_opts {
  enum options { r1, r2, r4, r8, r16, r32, r64, r128, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<ack_nack_num_repeats_nb_r13_opts> ack_nack_num_repeats_nb_r13_e;

// DL-CarrierConfigDedicated-NB-r13 ::= SEQUENCE
struct dl_carrier_cfg_ded_nb_r13_s {
  struct dl_bitmap_non_anchor_r13_c_ {
    struct types_opts {
      enum options { use_no_bitmap_r13, use_anchor_bitmap_r13, explicit_bitmap_cfg_r13, spare, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    dl_bitmap_non_anchor_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dl_bitmap_nb_r13_c& explicit_bitmap_cfg_r13()
    {
      assert_choice_type(types::explicit_bitmap_cfg_r13, type_, "downlinkBitmapNonAnchor-r13");
      return c;
    }
    const dl_bitmap_nb_r13_c& explicit_bitmap_cfg_r13() const
    {
      assert_choice_type(types::explicit_bitmap_cfg_r13, type_, "downlinkBitmapNonAnchor-r13");
      return c;
    }
    void                set_use_no_bitmap_r13();
    void                set_use_anchor_bitmap_r13();
    dl_bitmap_nb_r13_c& set_explicit_bitmap_cfg_r13();
    void                set_spare();

  private:
    types              type_;
    dl_bitmap_nb_r13_c c;
  };
  struct dl_gap_non_anchor_r13_c_ {
    struct types_opts {
      enum options { use_no_gap_r13, use_anchor_gap_cfg_r13, explicit_gap_cfg_r13, spare, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    dl_gap_non_anchor_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dl_gap_cfg_nb_r13_s& explicit_gap_cfg_r13()
    {
      assert_choice_type(types::explicit_gap_cfg_r13, type_, "dl-GapNonAnchor-r13");
      return c;
    }
    const dl_gap_cfg_nb_r13_s& explicit_gap_cfg_r13() const
    {
      assert_choice_type(types::explicit_gap_cfg_r13, type_, "dl-GapNonAnchor-r13");
      return c;
    }
    void                 set_use_no_gap_r13();
    void                 set_use_anchor_gap_cfg_r13();
    dl_gap_cfg_nb_r13_s& set_explicit_gap_cfg_r13();
    void                 set_spare();

  private:
    types               type_;
    dl_gap_cfg_nb_r13_s c;
  };
  struct inband_carrier_info_r13_s_ {
    struct same_pci_ind_r13_c_ {
      struct same_pci_r13_s_ {
        int8_t idx_to_mid_prb_r13 = -55;
      };
      struct different_pci_r13_s_ {
        struct eutra_num_crs_ports_r13_opts {
          enum options { same, four, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<eutra_num_crs_ports_r13_opts> eutra_num_crs_ports_r13_e_;

        // member variables
        eutra_num_crs_ports_r13_e_ eutra_num_crs_ports_r13;
      };
      struct types_opts {
        enum options { same_pci_r13, different_pci_r13, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      same_pci_ind_r13_c_() = default;
      same_pci_ind_r13_c_(const same_pci_ind_r13_c_& other);
      same_pci_ind_r13_c_& operator=(const same_pci_ind_r13_c_& other);
      ~same_pci_ind_r13_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      same_pci_r13_s_& same_pci_r13()
      {
        assert_choice_type(types::same_pci_r13, type_, "samePCI-Indicator-r13");
        return c.get<same_pci_r13_s_>();
      }
      different_pci_r13_s_& different_pci_r13()
      {
        assert_choice_type(types::different_pci_r13, type_, "samePCI-Indicator-r13");
        return c.get<different_pci_r13_s_>();
      }
      const same_pci_r13_s_& same_pci_r13() const
      {
        assert_choice_type(types::same_pci_r13, type_, "samePCI-Indicator-r13");
        return c.get<same_pci_r13_s_>();
      }
      const different_pci_r13_s_& different_pci_r13() const
      {
        assert_choice_type(types::different_pci_r13, type_, "samePCI-Indicator-r13");
        return c.get<different_pci_r13_s_>();
      }
      same_pci_r13_s_&      set_same_pci_r13();
      different_pci_r13_s_& set_different_pci_r13();

    private:
      types                                                  type_;
      choice_buffer_t<different_pci_r13_s_, same_pci_r13_s_> c;

      void destroy_();
    };
    struct eutra_ctrl_region_size_r13_opts {
      enum options { n1, n2, n3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<eutra_ctrl_region_size_r13_opts> eutra_ctrl_region_size_r13_e_;

    // member variables
    bool                          same_pci_ind_r13_present = false;
    same_pci_ind_r13_c_           same_pci_ind_r13;
    eutra_ctrl_region_size_r13_e_ eutra_ctrl_region_size_r13;
  };
  struct nrs_pwr_offset_non_anchor_v1330_opts {
    enum options { db_minus12, db_minus10, db_minus8, db_minus6, db_minus4, db_minus2, db0, db3, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<nrs_pwr_offset_non_anchor_v1330_opts> nrs_pwr_offset_non_anchor_v1330_e_;

  // member variables
  bool                        ext                              = false;
  bool                        dl_bitmap_non_anchor_r13_present = false;
  bool                        dl_gap_non_anchor_r13_present    = false;
  bool                        inband_carrier_info_r13_present  = false;
  carrier_freq_nb_r13_s       dl_carrier_freq_r13;
  dl_bitmap_non_anchor_r13_c_ dl_bitmap_non_anchor_r13;
  dl_gap_non_anchor_r13_c_    dl_gap_non_anchor_r13;
  inband_carrier_info_r13_s_  inband_carrier_info_r13;
  // ...
  // group 0
  bool                               nrs_pwr_offset_non_anchor_v1330_present = false;
  nrs_pwr_offset_non_anchor_v1330_e_ nrs_pwr_offset_non_anchor_v1330;
  // group 1
  copy_ptr<dl_gap_cfg_nb_v1530_s> dl_gap_non_anchor_v1530;
  // group 2
  copy_ptr<carrier_freq_nb_v1550_s> dl_carrier_freq_v1550;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LogicalChannelConfig-NB-r13 ::= SEQUENCE
struct lc_ch_cfg_nb_r13_s {
  bool    ext                           = false;
  bool    prio_r13_present              = false;
  bool    lc_ch_sr_prohibit_r13_present = false;
  uint8_t prio_r13                      = 1;
  bool    lc_ch_sr_prohibit_r13         = false;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPDSCH-16QAM-Config-NB-r17 ::= SEQUENCE
struct npdsch_minus16_qam_cfg_nb_r17_s {
  struct nrs_pwr_ratio_r17_opts {
    enum options { db_minus6, db_minus4dot77, db_minus3, db_minus1dot77, db0, db1, db2, db3, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<nrs_pwr_ratio_r17_opts> nrs_pwr_ratio_r17_e_;
  struct nrs_pwr_ratio_with_crs_r17_opts {
    enum options { db_minus6, db_minus4dot77, db_minus3, db_minus1dot77, db0, db1, db2, db3, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<nrs_pwr_ratio_with_crs_r17_opts> nrs_pwr_ratio_with_crs_r17_e_;

  // member variables
  bool                          nrs_pwr_ratio_r17_present          = false;
  bool                          nrs_pwr_ratio_with_crs_r17_present = false;
  nrs_pwr_ratio_r17_e_          nrs_pwr_ratio_r17;
  nrs_pwr_ratio_with_crs_r17_e_ nrs_pwr_ratio_with_crs_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPDSCH-MultiTB-Config-NB-r16 ::= SEQUENCE
struct npdsch_multi_tb_cfg_nb_r16_s {
  struct multi_tb_cfg_r16_opts {
    enum options { interleaved, non_interleaved, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<multi_tb_cfg_r16_opts> multi_tb_cfg_r16_e_;

  // member variables
  bool                harq_ack_bundling_r16_present = false;
  multi_tb_cfg_r16_e_ multi_tb_cfg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-Config-NB-r13 ::= SEQUENCE
struct pdcp_cfg_nb_r13_s {
  struct discard_timer_r13_opts {
    enum options { ms5120, ms10240, ms20480, ms40960, ms81920, infinity, spare2, spare1, nulltype } value;
    typedef int32_t number_type;

    const char* to_string() const;
    int32_t     to_number() const;
  };
  typedef enumerated<discard_timer_r13_opts> discard_timer_r13_e_;
  struct hdr_compress_r13_c_ {
    struct rohc_s_ {
      struct profiles_r13_s_ {
        bool profile0x0002 = false;
        bool profile0x0003 = false;
        bool profile0x0004 = false;
        bool profile0x0006 = false;
        bool profile0x0102 = false;
        bool profile0x0103 = false;
        bool profile0x0104 = false;
      };

      // member variables
      bool            ext                 = false;
      bool            max_cid_r13_present = false;
      uint16_t        max_cid_r13         = 1;
      profiles_r13_s_ profiles_r13;
      // ...
    };
    struct types_opts {
      enum options { not_used, rohc, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    hdr_compress_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rohc_s_& rohc()
    {
      assert_choice_type(types::rohc, type_, "headerCompression-r13");
      return c;
    }
    const rohc_s_& rohc() const
    {
      assert_choice_type(types::rohc, type_, "headerCompression-r13");
      return c;
    }
    void     set_not_used();
    rohc_s_& set_rohc();

  private:
    types   type_;
    rohc_s_ c;
  };

  // member variables
  bool                 ext                       = false;
  bool                 discard_timer_r13_present = false;
  discard_timer_r13_e_ discard_timer_r13;
  hdr_compress_r13_c_  hdr_compress_r13;
  // ...
  // group 0
  bool ciphering_disabled_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-Config-NB-r13 ::= CHOICE
struct rlc_cfg_nb_r13_c {
  struct am_s_ {
    ul_am_rlc_nb_r13_s ul_am_rlc_r13;
    dl_am_rlc_nb_r13_s dl_am_rlc_r13;
  };
  struct types_opts {
    enum options { am, /*...*/ um_bi_dir_r15, um_uni_dir_ul_r15, um_uni_dir_dl_r15, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true, 3> types;

  // choice methods
  rlc_cfg_nb_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  am_s_& am()
  {
    assert_choice_type(types::am, type_, "RLC-Config-NB-r13");
    return c;
  }
  const am_s_& am() const
  {
    assert_choice_type(types::am, type_, "RLC-Config-NB-r13");
    return c;
  }
  am_s_& set_am();
  void   set_um_bi_dir_r15();
  void   set_um_uni_dir_ul_r15();
  void   set_um_uni_dir_dl_r15();

private:
  types type_;
  am_s_ c;
};

// RLC-Config-NB-v1430 ::= SEQUENCE
struct rlc_cfg_nb_v1430_s {
  bool           t_reordering_r14_present = false;
  t_reordering_e t_reordering_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-Config-NB-v1700 ::= SEQUENCE
struct rlc_cfg_nb_v1700_s {
  setup_release_c<t_reordering_ext_r17_e> t_reordering_ext_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SR-NPRACH-Resource-NB-r15 ::= SEQUENCE
struct sr_nprach_res_nb_r15_s {
  struct nprach_sub_carrier_idx_r15_c_ {
    struct types_opts {
      enum options { nprach_fmt0_fmt1_r15, nprach_fmt2_r15, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    nprach_sub_carrier_idx_r15_c_() = default;
    nprach_sub_carrier_idx_r15_c_(const nprach_sub_carrier_idx_r15_c_& other);
    nprach_sub_carrier_idx_r15_c_& operator=(const nprach_sub_carrier_idx_r15_c_& other);
    ~nprach_sub_carrier_idx_r15_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint8_t& nprach_fmt0_fmt1_r15()
    {
      assert_choice_type(types::nprach_fmt0_fmt1_r15, type_, "nprach-SubCarrierIndex-r15");
      return c.get<uint8_t>();
    }
    uint8_t& nprach_fmt2_r15()
    {
      assert_choice_type(types::nprach_fmt2_r15, type_, "nprach-SubCarrierIndex-r15");
      return c.get<uint8_t>();
    }
    const uint8_t& nprach_fmt0_fmt1_r15() const
    {
      assert_choice_type(types::nprach_fmt0_fmt1_r15, type_, "nprach-SubCarrierIndex-r15");
      return c.get<uint8_t>();
    }
    const uint8_t& nprach_fmt2_r15() const
    {
      assert_choice_type(types::nprach_fmt2_r15, type_, "nprach-SubCarrierIndex-r15");
      return c.get<uint8_t>();
    }
    uint8_t& set_nprach_fmt0_fmt1_r15();
    uint8_t& set_nprach_fmt2_r15();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };
  struct alpha_r15_opts {
    enum options { al0, al04, al05, al06, al07, al08, al09, al1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<alpha_r15_opts> alpha_r15_e_;

  // member variables
  uint8_t                       nprach_carrier_idx_r15 = 0;
  uint8_t                       nprach_res_idx_r15     = 1;
  nprach_sub_carrier_idx_r15_c_ nprach_sub_carrier_idx_r15;
  int8_t                        p0_sr_r15 = -126;
  alpha_r15_e_                  alpha_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SR-ProhibitTimerOffset-NB-r17 ::= ENUMERATED
struct sr_prohibit_timer_offset_nb_r17_opts {
  enum options { ms90, ms180, ms270, ms360, ms450, ms540, ms1080, spare, nulltype } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<sr_prohibit_timer_offset_nb_r17_opts> sr_prohibit_timer_offset_nb_r17_e;

// UL-CarrierConfigDedicated-NB-r13 ::= SEQUENCE
struct ul_carrier_cfg_ded_nb_r13_s {
  bool                  ext                         = false;
  bool                  ul_carrier_freq_r13_present = false;
  carrier_freq_nb_r13_s ul_carrier_freq_r13;
  // ...
  // group 0
  bool                            tdd_ul_dl_align_offset_r15_present = false;
  tdd_ul_dl_align_offset_nb_r15_e tdd_ul_dl_align_offset_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierConfigDedicated-NB-r13 ::= SEQUENCE
struct carrier_cfg_ded_nb_r13_s {
  dl_carrier_cfg_ded_nb_r13_s dl_carrier_cfg_r13;
  ul_carrier_cfg_ded_nb_r13_s ul_carrier_cfg_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-ToAddMod-NB-r13 ::= SEQUENCE
struct drb_to_add_mod_nb_r13_s {
  bool               ext                       = false;
  bool               eps_bearer_id_r13_present = false;
  bool               pdcp_cfg_r13_present      = false;
  bool               rlc_cfg_r13_present       = false;
  bool               lc_ch_id_r13_present      = false;
  bool               lc_ch_cfg_r13_present     = false;
  uint8_t            eps_bearer_id_r13         = 0;
  uint8_t            drb_id_r13                = 1;
  pdcp_cfg_nb_r13_s  pdcp_cfg_r13;
  rlc_cfg_nb_r13_c   rlc_cfg_r13;
  uint8_t            lc_ch_id_r13 = 3;
  lc_ch_cfg_nb_r13_s lc_ch_cfg_r13;
  // ...
  // group 0
  copy_ptr<rlc_cfg_nb_v1430_s> rlc_cfg_v1430;
  // group 1
  bool     pdu_session_r16_present = false;
  uint16_t pdu_session_r16         = 0;
  // group 2
  copy_ptr<rlc_cfg_nb_v1700_s> rlc_cfg_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRX-Config-NB-r13 ::= CHOICE
struct drx_cfg_nb_r13_c {
  struct setup_s_ {
    struct on_dur_timer_r13_opts {
      enum options { pp1, pp2, pp3, pp4, pp8, pp16, pp32, spare, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<on_dur_timer_r13_opts> on_dur_timer_r13_e_;
    struct drx_inactivity_timer_r13_opts {
      enum options { pp0, pp1, pp2, pp3, pp4, pp8, pp16, pp32, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<drx_inactivity_timer_r13_opts> drx_inactivity_timer_r13_e_;
    struct drx_retx_timer_r13_opts {
      enum options {
        pp0,
        pp1,
        pp2,
        pp4,
        pp6,
        pp8,
        pp16,
        pp24,
        pp33,
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
    typedef enumerated<drx_retx_timer_r13_opts> drx_retx_timer_r13_e_;
    struct drx_cycle_r13_opts {
      enum options {
        sf256,
        sf512,
        sf1024,
        sf1536,
        sf2048,
        sf3072,
        sf4096,
        sf4608,
        sf6144,
        sf7680,
        sf8192,
        sf9216,
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
    typedef enumerated<drx_cycle_r13_opts> drx_cycle_r13_e_;
    struct drx_ul_retx_timer_r13_opts {
      enum options {
        pp0,
        pp1,
        pp2,
        pp4,
        pp6,
        pp8,
        pp16,
        pp24,
        pp33,
        pp40,
        pp64,
        pp80,
        pp96,
        pp112,
        pp128,
        pp160,
        pp320,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<drx_ul_retx_timer_r13_opts> drx_ul_retx_timer_r13_e_;

    // member variables
    on_dur_timer_r13_e_         on_dur_timer_r13;
    drx_inactivity_timer_r13_e_ drx_inactivity_timer_r13;
    drx_retx_timer_r13_e_       drx_retx_timer_r13;
    drx_cycle_r13_e_            drx_cycle_r13;
    uint16_t                    drx_start_offset_r13 = 0;
    drx_ul_retx_timer_r13_e_    drx_ul_retx_timer_r13;
  };
  using types = setup_e;

  // choice methods
  drx_cfg_nb_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "DRX-Config-NB-r13");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "DRX-Config-NB-r13");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// NPDCCH-ConfigDedicated-NB-r13 ::= SEQUENCE
struct npdcch_cfg_ded_nb_r13_s {
  struct npdcch_num_repeats_r13_opts {
    enum options {
      r1,
      r2,
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
  typedef enumerated<npdcch_num_repeats_r13_opts> npdcch_num_repeats_r13_e_;
  struct npdcch_start_sf_uss_r13_opts {
    enum options { v1dot5, v2, v4, v8, v16, v32, v48, v64, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<npdcch_start_sf_uss_r13_opts> npdcch_start_sf_uss_r13_e_;
  struct npdcch_offset_uss_r13_opts {
    enum options { zero, one_eighth, one_fourth, three_eighth, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<npdcch_offset_uss_r13_opts> npdcch_offset_uss_r13_e_;

  // member variables
  npdcch_num_repeats_r13_e_  npdcch_num_repeats_r13;
  npdcch_start_sf_uss_r13_e_ npdcch_start_sf_uss_r13;
  npdcch_offset_uss_r13_e_   npdcch_offset_uss_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPDCCH-ConfigDedicated-NB-v1530 ::= SEQUENCE
struct npdcch_cfg_ded_nb_v1530_s {
  struct npdcch_start_sf_uss_v1530_opts {
    enum options { v96, v128, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<npdcch_start_sf_uss_v1530_opts> npdcch_start_sf_uss_v1530_e_;

  // member variables
  npdcch_start_sf_uss_v1530_e_ npdcch_start_sf_uss_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPDSCH-ConfigDedicated-NB-r16 ::= SEQUENCE
struct npdsch_cfg_ded_nb_r16_s {
  bool                         npdsch_multi_tb_cfg_r16_present = false;
  npdsch_multi_tb_cfg_nb_r16_s npdsch_multi_tb_cfg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPDSCH-ConfigDedicated-NB-v1710 ::= SEQUENCE
struct npdsch_cfg_ded_nb_v1710_s {
  setup_release_c<npdsch_minus16_qam_cfg_nb_r17_s> npdsch_minus16_qam_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPUSCH-ConfigDedicated-NB-r13 ::= SEQUENCE
struct npusch_cfg_ded_nb_r13_s {
  bool                          ack_nack_num_repeats_r13_present = false;
  bool                          npusch_all_symbols_r13_present   = false;
  bool                          group_hop_disabled_r13_present   = false;
  ack_nack_num_repeats_nb_r13_e ack_nack_num_repeats_r13;
  bool                          npusch_all_symbols_r13 = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPUSCH-ConfigDedicated-NB-v1610 ::= SEQUENCE
struct npusch_cfg_ded_nb_v1610_s {
  struct npusch_multi_tb_cfg_r16_opts {
    enum options { interleaved, non_interleaved, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<npusch_multi_tb_cfg_r16_opts> npusch_multi_tb_cfg_r16_e_;

  // member variables
  npusch_multi_tb_cfg_r16_e_ npusch_multi_tb_cfg_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPUSCH-ConfigDedicated-NB-v1700 ::= SEQUENCE
struct npusch_cfg_ded_nb_v1700_s {
  bool npusch_minus16_qam_cfg_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPUSCH-TxDuration-NB-r17 ::= SEQUENCE
struct npusch_tx_dur_nb_r17_s {
  struct npusch_tx_dur_r17_opts {
    enum options { ms2, ms4, ms8, ms16, ms32, ms64, ms128, ms256, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<npusch_tx_dur_r17_opts> npusch_tx_dur_r17_e_;

  // member variables
  npusch_tx_dur_r17_e_ npusch_tx_dur_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OffsetThresholdTA-NB-r17 ::= ENUMERATED
struct offset_thres_ta_nb_r17_opts {
  enum options {
    ms0dot5,
    ms1,
    ms2,
    ms3,
    ms4,
    ms5,
    ms6,
    ms7,
    ms8,
    ms9,
    ms10,
    ms11,
    ms12,
    ms13,
    ms14,
    ms15,
    nulltype
  } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
typedef enumerated<offset_thres_ta_nb_r17_opts> offset_thres_ta_nb_r17_e;

// PeriodicBSR-Timer-NB-r13 ::= ENUMERATED
struct periodic_bsr_timer_nb_r13_opts {
  enum options { pp2, pp4, pp8, pp16, pp64, pp128, infinity, spare, nulltype } value;
  typedef int16_t number_type;

  const char* to_string() const;
  int16_t     to_number() const;
};
typedef enumerated<periodic_bsr_timer_nb_r13_opts> periodic_bsr_timer_nb_r13_e;

// ResourceReservationConfig-NB-r16 ::= SEQUENCE
struct res_reserv_cfg_nb_r16_s {
  struct periodicity_r16_opts {
    enum options { ms10, ms20, ms40, ms80, ms160, spare3, spare2, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<periodicity_r16_opts> periodicity_r16_e_;
  struct res_reserv_r16_c_ {
    struct sf_bitmap_r16_c_ {
      struct types_opts {
        enum options { sf_pattern10ms, sf_pattern40ms, nulltype } value;
        typedef uint8_t number_type;

        const char* to_string() const;
        uint8_t     to_number() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      sf_bitmap_r16_c_() = default;
      sf_bitmap_r16_c_(const sf_bitmap_r16_c_& other);
      sf_bitmap_r16_c_& operator=(const sf_bitmap_r16_c_& other);
      ~sf_bitmap_r16_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      fixed_bitstring<10>& sf_pattern10ms()
      {
        assert_choice_type(types::sf_pattern10ms, type_, "subframeBitmap-r16");
        return c.get<fixed_bitstring<10> >();
      }
      fixed_bitstring<40>& sf_pattern40ms()
      {
        assert_choice_type(types::sf_pattern40ms, type_, "subframeBitmap-r16");
        return c.get<fixed_bitstring<40> >();
      }
      const fixed_bitstring<10>& sf_pattern10ms() const
      {
        assert_choice_type(types::sf_pattern10ms, type_, "subframeBitmap-r16");
        return c.get<fixed_bitstring<10> >();
      }
      const fixed_bitstring<40>& sf_pattern40ms() const
      {
        assert_choice_type(types::sf_pattern40ms, type_, "subframeBitmap-r16");
        return c.get<fixed_bitstring<40> >();
      }
      fixed_bitstring<10>& set_sf_pattern10ms();
      fixed_bitstring<40>& set_sf_pattern40ms();

    private:
      types                                 type_;
      choice_buffer_t<fixed_bitstring<40> > c;

      void destroy_();
    };
    struct slot_cfg_r16_s_ {
      struct slot_bitmap_r16_c_ {
        struct types_opts {
          enum options { slot_pattern10ms, slot_pattern40ms, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<types_opts> types;

        // choice methods
        slot_bitmap_r16_c_() = default;
        slot_bitmap_r16_c_(const slot_bitmap_r16_c_& other);
        slot_bitmap_r16_c_& operator=(const slot_bitmap_r16_c_& other);
        ~slot_bitmap_r16_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        fixed_bitstring<20>& slot_pattern10ms()
        {
          assert_choice_type(types::slot_pattern10ms, type_, "slotBitmap-r16");
          return c.get<fixed_bitstring<20> >();
        }
        fixed_bitstring<80>& slot_pattern40ms()
        {
          assert_choice_type(types::slot_pattern40ms, type_, "slotBitmap-r16");
          return c.get<fixed_bitstring<80> >();
        }
        const fixed_bitstring<20>& slot_pattern10ms() const
        {
          assert_choice_type(types::slot_pattern10ms, type_, "slotBitmap-r16");
          return c.get<fixed_bitstring<20> >();
        }
        const fixed_bitstring<80>& slot_pattern40ms() const
        {
          assert_choice_type(types::slot_pattern40ms, type_, "slotBitmap-r16");
          return c.get<fixed_bitstring<80> >();
        }
        fixed_bitstring<20>& set_slot_pattern10ms();
        fixed_bitstring<80>& set_slot_pattern40ms();

      private:
        types                                 type_;
        choice_buffer_t<fixed_bitstring<80> > c;

        void destroy_();
      };
      struct symbol_bitmap_r16_c_ {
        struct symbol_bitmap_fdd_dl_s_ {
          bool               symbol_bitmap1_r16_present = false;
          bool               symbol_bitmap2_r16_present = false;
          fixed_bitstring<5> symbol_bitmap1_r16;
          fixed_bitstring<5> symbol_bitmap2_r16;
        };
        struct symbol_bitmap_fdd_ul_or_tdd_s_ {
          bool               symbol_bitmap1_r16_present = false;
          bool               symbol_bitmap2_r16_present = false;
          fixed_bitstring<7> symbol_bitmap1_r16;
          fixed_bitstring<7> symbol_bitmap2_r16;
        };
        struct types_opts {
          enum options { symbol_bitmap_fdd_dl, symbol_bitmap_fdd_ul_or_tdd, nulltype } value;

          const char* to_string() const;
        };
        typedef enumerated<types_opts> types;

        // choice methods
        symbol_bitmap_r16_c_() = default;
        symbol_bitmap_r16_c_(const symbol_bitmap_r16_c_& other);
        symbol_bitmap_r16_c_& operator=(const symbol_bitmap_r16_c_& other);
        ~symbol_bitmap_r16_c_() { destroy_(); }
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        symbol_bitmap_fdd_dl_s_& symbol_bitmap_fdd_dl()
        {
          assert_choice_type(types::symbol_bitmap_fdd_dl, type_, "symbolBitmap-r16");
          return c.get<symbol_bitmap_fdd_dl_s_>();
        }
        symbol_bitmap_fdd_ul_or_tdd_s_& symbol_bitmap_fdd_ul_or_tdd()
        {
          assert_choice_type(types::symbol_bitmap_fdd_ul_or_tdd, type_, "symbolBitmap-r16");
          return c.get<symbol_bitmap_fdd_ul_or_tdd_s_>();
        }
        const symbol_bitmap_fdd_dl_s_& symbol_bitmap_fdd_dl() const
        {
          assert_choice_type(types::symbol_bitmap_fdd_dl, type_, "symbolBitmap-r16");
          return c.get<symbol_bitmap_fdd_dl_s_>();
        }
        const symbol_bitmap_fdd_ul_or_tdd_s_& symbol_bitmap_fdd_ul_or_tdd() const
        {
          assert_choice_type(types::symbol_bitmap_fdd_ul_or_tdd, type_, "symbolBitmap-r16");
          return c.get<symbol_bitmap_fdd_ul_or_tdd_s_>();
        }
        symbol_bitmap_fdd_dl_s_&        set_symbol_bitmap_fdd_dl();
        symbol_bitmap_fdd_ul_or_tdd_s_& set_symbol_bitmap_fdd_ul_or_tdd();

      private:
        types                                                                    type_;
        choice_buffer_t<symbol_bitmap_fdd_dl_s_, symbol_bitmap_fdd_ul_or_tdd_s_> c;

        void destroy_();
      };

      // member variables
      slot_bitmap_r16_c_   slot_bitmap_r16;
      symbol_bitmap_r16_c_ symbol_bitmap_r16;
    };
    struct types_opts {
      enum options { sf_bitmap_r16, slot_cfg_r16, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    res_reserv_r16_c_() = default;
    res_reserv_r16_c_(const res_reserv_r16_c_& other);
    res_reserv_r16_c_& operator=(const res_reserv_r16_c_& other);
    ~res_reserv_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sf_bitmap_r16_c_& sf_bitmap_r16()
    {
      assert_choice_type(types::sf_bitmap_r16, type_, "resourceReservation-r16");
      return c.get<sf_bitmap_r16_c_>();
    }
    slot_cfg_r16_s_& slot_cfg_r16()
    {
      assert_choice_type(types::slot_cfg_r16, type_, "resourceReservation-r16");
      return c.get<slot_cfg_r16_s_>();
    }
    const sf_bitmap_r16_c_& sf_bitmap_r16() const
    {
      assert_choice_type(types::sf_bitmap_r16, type_, "resourceReservation-r16");
      return c.get<sf_bitmap_r16_c_>();
    }
    const slot_cfg_r16_s_& slot_cfg_r16() const
    {
      assert_choice_type(types::slot_cfg_r16, type_, "resourceReservation-r16");
      return c.get<slot_cfg_r16_s_>();
    }
    sf_bitmap_r16_c_& set_sf_bitmap_r16();
    slot_cfg_r16_s_&  set_slot_cfg_r16();

  private:
    types                                              type_;
    choice_buffer_t<sf_bitmap_r16_c_, slot_cfg_r16_s_> c;

    void destroy_();
  };

  // member variables
  bool               ext = false;
  periodicity_r16_e_ periodicity_r16;
  uint8_t            start_position_r16 = 0;
  res_reserv_r16_c_  res_reserv_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RetxBSR-Timer-NB-r13 ::= ENUMERATED
struct retx_bsr_timer_nb_r13_opts {
  enum options { pp4, pp16, pp64, pp128, pp256, pp512, infinity, spare, nulltype } value;
  typedef int16_t number_type;

  const char* to_string() const;
  int16_t     to_number() const;
};
typedef enumerated<retx_bsr_timer_nb_r13_opts> retx_bsr_timer_nb_r13_e;

// SR-SPS-BSR-Config-NB-r15 ::= CHOICE
struct sr_sps_bsr_cfg_nb_r15_c {
  struct setup_s_ {
    struct semi_persist_sched_interv_ul_r15_opts {
      enum options { sf128, sf256, sf512, sf1024, sf1280, sf2048, sf2560, sf5120, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<semi_persist_sched_interv_ul_r15_opts> semi_persist_sched_interv_ul_r15_e_;

    // member variables
    fixed_bitstring<16>                 semi_persist_sched_c_rnti_r15;
    semi_persist_sched_interv_ul_r15_e_ semi_persist_sched_interv_ul_r15;
  };
  using types = setup_e;

  // choice methods
  sr_sps_bsr_cfg_nb_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SR-SPS-BSR-Config-NB-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SR-SPS-BSR-Config-NB-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SR-WithoutHARQ-ACK-Config-NB-r15 ::= CHOICE
struct sr_without_harq_ack_cfg_nb_r15_c {
  struct setup_s_ {
    bool                   sr_prohibit_timer_r15_present = false;
    bool                   sr_nprach_res_r15_present     = false;
    uint8_t                sr_prohibit_timer_r15         = 0;
    sr_nprach_res_nb_r15_s sr_nprach_res_r15;
  };
  using types = setup_e;

  // choice methods
  sr_without_harq_ack_cfg_nb_r15_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "SR-WithoutHARQ-ACK-Config-NB-r15");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "SR-WithoutHARQ-ACK-Config-NB-r15");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SR-WithoutHARQ-ACK-Config-NB-v1700 ::= SEQUENCE
struct sr_without_harq_ack_cfg_nb_v1700_s {
  bool                                               sr_prohibit_timer_offset_r17_present = false;
  setup_release_c<sr_prohibit_timer_offset_nb_r17_e> sr_prohibit_timer_offset_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SRB-ToAddMod-NB-r13 ::= SEQUENCE
struct srb_to_add_mod_nb_r13_s {
  struct rlc_cfg_r13_c_ {
    struct types_opts {
      enum options { explicit_value, default_value, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    rlc_cfg_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rlc_cfg_nb_r13_c& explicit_value()
    {
      assert_choice_type(types::explicit_value, type_, "rlc-Config-r13");
      return c;
    }
    const rlc_cfg_nb_r13_c& explicit_value() const
    {
      assert_choice_type(types::explicit_value, type_, "rlc-Config-r13");
      return c;
    }
    rlc_cfg_nb_r13_c& set_explicit_value();
    void              set_default_value();

  private:
    types            type_;
    rlc_cfg_nb_r13_c c;
  };
  struct lc_ch_cfg_r13_c_ {
    struct types_opts {
      enum options { explicit_value, default_value, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    lc_ch_cfg_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    lc_ch_cfg_nb_r13_s& explicit_value()
    {
      assert_choice_type(types::explicit_value, type_, "logicalChannelConfig-r13");
      return c;
    }
    const lc_ch_cfg_nb_r13_s& explicit_value() const
    {
      assert_choice_type(types::explicit_value, type_, "logicalChannelConfig-r13");
      return c;
    }
    lc_ch_cfg_nb_r13_s& set_explicit_value();
    void                set_default_value();

  private:
    types              type_;
    lc_ch_cfg_nb_r13_s c;
  };

  // member variables
  bool             ext                   = false;
  bool             rlc_cfg_r13_present   = false;
  bool             lc_ch_cfg_r13_present = false;
  rlc_cfg_r13_c_   rlc_cfg_r13;
  lc_ch_cfg_r13_c_ lc_ch_cfg_r13;
  // ...
  // group 0
  copy_ptr<rlc_cfg_nb_v1430_s> rlc_cfg_v1430;
  // group 1
  copy_ptr<rlc_cfg_nb_v1700_s> rlc_cfg_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkPowerControlDedicated-NB-r13 ::= SEQUENCE
struct ul_pwr_ctrl_ded_nb_r13_s {
  int8_t p0_ue_npusch_r13 = -8;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkPowerControlDedicated-NB-v1700 ::= SEQUENCE
struct ul_pwr_ctrl_ded_nb_v1700_s {
  struct delta_mcs_enabled_r17_opts {
    enum options { en0, en1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<delta_mcs_enabled_r17_opts> delta_mcs_enabled_r17_e_;

  // member variables
  delta_mcs_enabled_r17_e_ delta_mcs_enabled_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DRB-ToAddModList-NB-r13 ::= SEQUENCE (SIZE (1..2)) OF DRB-ToAddMod-NB-r13
using drb_to_add_mod_list_nb_r13_l = dyn_array<drb_to_add_mod_nb_r13_s>;

// DRB-ToReleaseList-NB-r13 ::= SEQUENCE (SIZE (1..2)) OF INTEGER (1..32)
using drb_to_release_list_nb_r13_l = bounded_array<uint8_t, 2>;

// MAC-MainConfig-NB-r13 ::= SEQUENCE
struct mac_main_cfg_nb_r13_s {
  struct ul_sch_cfg_r13_s_ {
    bool                        periodic_bsr_timer_r13_present = false;
    periodic_bsr_timer_nb_r13_e periodic_bsr_timer_r13;
    retx_bsr_timer_nb_r13_e     retx_bsr_timer_r13;
  };
  struct lc_ch_sr_cfg_r13_c_ {
    struct setup_s_ {
      struct lc_ch_sr_prohibit_timer_r13_opts {
        enum options { pp2, pp8, pp32, pp128, pp512, pp1024, pp2048, spare, nulltype } value;
        typedef uint16_t number_type;

        const char* to_string() const;
        uint16_t    to_number() const;
      };
      typedef enumerated<lc_ch_sr_prohibit_timer_r13_opts> lc_ch_sr_prohibit_timer_r13_e_;

      // member variables
      lc_ch_sr_prohibit_timer_r13_e_ lc_ch_sr_prohibit_timer_r13;
    };
    using types = setup_e;

    // choice methods
    lc_ch_sr_cfg_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "logicalChannelSR-Config-r13");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "logicalChannelSR-Config-r13");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct data_inactivity_timer_cfg_r14_c_ {
    struct setup_s_ {
      data_inactivity_timer_r14_e data_inactivity_timer_r14;
    };
    using types = setup_e;

    // choice methods
    data_inactivity_timer_cfg_r14_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    setup_s_& setup()
    {
      assert_choice_type(types::setup, type_, "dataInactivityTimerConfig-r14");
      return c;
    }
    const setup_s_& setup() const
    {
      assert_choice_type(types::setup, type_, "dataInactivityTimerConfig-r14");
      return c;
    }
    void      set_release();
    setup_s_& set_setup();

  private:
    types    type_;
    setup_s_ c;
  };
  struct drx_cycle_v1430_opts {
    enum options { sf1280, sf2560, sf5120, sf10240, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<drx_cycle_v1430_opts> drx_cycle_v1430_e_;

  // member variables
  bool                ext                      = false;
  bool                ul_sch_cfg_r13_present   = false;
  bool                drx_cfg_r13_present      = false;
  bool                lc_ch_sr_cfg_r13_present = false;
  ul_sch_cfg_r13_s_   ul_sch_cfg_r13;
  drx_cfg_nb_r13_c    drx_cfg_r13;
  time_align_timer_e  time_align_timer_ded_r13;
  lc_ch_sr_cfg_r13_c_ lc_ch_sr_cfg_r13;
  // ...
  // group 0
  bool                                       rai_activation_r14_present = false;
  copy_ptr<data_inactivity_timer_cfg_r14_c_> data_inactivity_timer_cfg_r14;
  // group 1
  bool               drx_cycle_v1430_present = false;
  drx_cycle_v1430_e_ drx_cycle_v1430;
  // group 2
  bool ra_cfra_cfg_r14_present = false;
  // group 3
  copy_ptr<setup_release_c<offset_thres_ta_nb_r17_e> > offset_thres_ta_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhysicalConfigDedicated-NB-r13 ::= SEQUENCE
struct phys_cfg_ded_nb_r13_s {
  struct ntn_cfg_ded_r17_s_ {
    setup_release_c<npusch_tx_dur_nb_r17_s> npusch_tx_dur_r17;
  };
  struct ul_segmented_precompensation_gap_r17_opts {
    enum options { sym1, sl1, sl2, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<ul_segmented_precompensation_gap_r17_opts> ul_segmented_precompensation_gap_r17_e_;

  // member variables
  bool                     ext                         = false;
  bool                     carrier_cfg_ded_r13_present = false;
  bool                     npdcch_cfg_ded_r13_present  = false;
  bool                     npusch_cfg_ded_r13_present  = false;
  bool                     ul_pwr_ctrl_ded_r13_present = false;
  carrier_cfg_ded_nb_r13_s carrier_cfg_ded_r13;
  npdcch_cfg_ded_nb_r13_s  npdcch_cfg_ded_r13;
  npusch_cfg_ded_nb_r13_s  npusch_cfg_ded_r13;
  ul_pwr_ctrl_ded_nb_r13_s ul_pwr_ctrl_ded_r13;
  // ...
  // group 0
  bool two_harq_processes_cfg_r14_present = false;
  // group 1
  bool interference_randomisation_cfg_r14_present = false;
  // group 2
  copy_ptr<npdcch_cfg_ded_nb_v1530_s> npdcch_cfg_ded_v1530;
  // group 3
  bool add_tx_sib1_cfg_v1540_present = false;
  // group 4
  copy_ptr<npusch_cfg_ded_nb_v1610_s>                 npusch_cfg_ded_v1610;
  copy_ptr<npdsch_cfg_ded_nb_r16_s>                   npdsch_cfg_ded_r16;
  copy_ptr<setup_release_c<res_reserv_cfg_nb_r16_s> > res_reserv_cfg_dl_r16;
  copy_ptr<setup_release_c<res_reserv_cfg_nb_r16_s> > res_reserv_cfg_ul_r16;
  // group 5
  copy_ptr<ntn_cfg_ded_r17_s_>         ntn_cfg_ded_r17;
  copy_ptr<npdsch_cfg_ded_nb_v1710_s>  npdsch_cfg_ded_v1700;
  copy_ptr<ul_pwr_ctrl_ded_nb_v1700_s> ul_pwr_ctrl_ded_v1700;
  // group 6
  bool                                    ul_segmented_precompensation_gap_r17_present = false;
  ul_segmented_precompensation_gap_r17_e_ ul_segmented_precompensation_gap_r17;
  // group 7
  copy_ptr<npusch_cfg_ded_nb_v1700_s> npusch_cfg_ded_v1740;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLF-TimersAndConstants-NB-r13 ::= CHOICE
struct rlf_timers_and_consts_nb_r13_c {
  struct setup_s_ {
    struct t301_r13_opts {
      enum options { ms2500, ms4000, ms6000, ms10000, ms15000, ms25000, ms40000, ms60000, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<t301_r13_opts> t301_r13_e_;
    struct t310_r13_opts {
      enum options { ms0, ms200, ms500, ms1000, ms2000, ms4000, ms8000, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<t310_r13_opts> t310_r13_e_;
    struct n310_r13_opts {
      enum options { n1, n2, n3, n4, n6, n8, n10, n20, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<n310_r13_opts> n310_r13_e_;
    struct t311_r13_opts {
      enum options { ms1000, ms3000, ms5000, ms10000, ms15000, ms20000, ms30000, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<t311_r13_opts> t311_r13_e_;
    struct n311_r13_opts {
      enum options { n1, n2, n3, n4, n5, n6, n8, n10, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<n311_r13_opts> n311_r13_e_;
    struct t311_v1350_opts {
      enum options { ms40000, ms60000, ms90000, ms120000, nulltype } value;
      typedef uint32_t number_type;

      const char* to_string() const;
      uint32_t    to_number() const;
    };
    typedef enumerated<t311_v1350_opts> t311_v1350_e_;
    struct t301_v1530_opts {
      enum options { ms80000, ms100000, ms120000, nulltype } value;
      typedef uint32_t number_type;

      const char* to_string() const;
      uint32_t    to_number() const;
    };
    typedef enumerated<t301_v1530_opts> t301_v1530_e_;
    struct t311_v1530_opts {
      enum options { ms160000, ms200000, nulltype } value;
      typedef uint32_t number_type;

      const char* to_string() const;
      uint32_t    to_number() const;
    };
    typedef enumerated<t311_v1530_opts> t311_v1530_e_;

    // member variables
    bool        ext = false;
    t301_r13_e_ t301_r13;
    t310_r13_e_ t310_r13;
    n310_r13_e_ n310_r13;
    t311_r13_e_ t311_r13;
    n311_r13_e_ n311_r13;
    // ...
    // group 0
    bool          t311_v1350_present = false;
    t311_v1350_e_ t311_v1350;
    // group 1
    bool          t301_v1530_present = false;
    bool          t311_v1530_present = false;
    t301_v1530_e_ t301_v1530;
    t311_v1530_e_ t311_v1530;

    // sequence methods
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
  };
  using types = setup_e;

  // choice methods
  rlf_timers_and_consts_nb_r13_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  setup_s_& setup()
  {
    assert_choice_type(types::setup, type_, "RLF-TimersAndConstants-NB-r13");
    return c;
  }
  const setup_s_& setup() const
  {
    assert_choice_type(types::setup, type_, "RLF-TimersAndConstants-NB-r13");
    return c;
  }
  void      set_release();
  setup_s_& set_setup();

private:
  types    type_;
  setup_s_ c;
};

// SRB-ToAddModList-NB-r13 ::= SEQUENCE (SIZE (1)) OF SRB-ToAddMod-NB-r13
using srb_to_add_mod_list_nb_r13_l = std::array<srb_to_add_mod_nb_r13_s, 1>;

// SchedulingRequestConfig-NB-r15 ::= SEQUENCE
struct sched_request_cfg_nb_r15_s {
  bool                             ext                                 = false;
  bool                             sr_with_harq_ack_cfg_r15_present    = false;
  bool                             sr_without_harq_ack_cfg_r15_present = false;
  bool                             sr_sps_bsr_cfg_r15_present          = false;
  sr_without_harq_ack_cfg_nb_r15_c sr_without_harq_ack_cfg_r15;
  sr_sps_bsr_cfg_nb_r15_c          sr_sps_bsr_cfg_r15;
  // ...
  // group 0
  copy_ptr<sr_without_harq_ack_cfg_nb_v1700_s> sr_without_harq_ack_cfg_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RadioResourceConfigDedicated-NB-r13 ::= SEQUENCE
struct rr_cfg_ded_nb_r13_s {
  struct mac_main_cfg_r13_c_ {
    struct types_opts {
      enum options { explicit_value_r13, default_value_r13, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    mac_main_cfg_r13_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    mac_main_cfg_nb_r13_s& explicit_value_r13()
    {
      assert_choice_type(types::explicit_value_r13, type_, "mac-MainConfig-r13");
      return c;
    }
    const mac_main_cfg_nb_r13_s& explicit_value_r13() const
    {
      assert_choice_type(types::explicit_value_r13, type_, "mac-MainConfig-r13");
      return c;
    }
    mac_main_cfg_nb_r13_s& set_explicit_value_r13();
    void                   set_default_value_r13();

  private:
    types                 type_;
    mac_main_cfg_nb_r13_s c;
  };

  // member variables
  bool                           ext                               = false;
  bool                           srb_to_add_mod_list_r13_present   = false;
  bool                           drb_to_add_mod_list_r13_present   = false;
  bool                           drb_to_release_list_r13_present   = false;
  bool                           mac_main_cfg_r13_present          = false;
  bool                           phys_cfg_ded_r13_present          = false;
  bool                           rlf_timers_and_consts_r13_present = false;
  srb_to_add_mod_list_nb_r13_l   srb_to_add_mod_list_r13;
  drb_to_add_mod_list_nb_r13_l   drb_to_add_mod_list_r13;
  drb_to_release_list_nb_r13_l   drb_to_release_list_r13;
  mac_main_cfg_r13_c_            mac_main_cfg_r13;
  phys_cfg_ded_nb_r13_s          phys_cfg_ded_r13;
  rlf_timers_and_consts_nb_r13_c rlf_timers_and_consts_r13;
  // ...
  // group 0
  copy_ptr<sched_request_cfg_nb_r15_s> sched_request_cfg_r15;
  // group 1
  bool                new_ue_id_r16_present = false;
  fixed_bitstring<16> new_ue_id_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Config-NB ::= SEQUENCE
struct as_cfg_nb_s {
  bool                     ext = false;
  rr_cfg_ded_nb_r13_s      source_rr_cfg_r13;
  security_algorithm_cfg_s source_security_algorithm_cfg_r13;
  fixed_bitstring<16>      source_ue_id_r13;
  carrier_freq_nb_r13_s    source_dl_carrier_freq_r13;
  // ...
  // group 0
  copy_ptr<carrier_freq_nb_v1550_s> source_dl_carrier_freq_v1550;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReestablishmentInfo-NB ::= SEQUENCE
struct reest_info_nb_s {
  bool                    ext                               = false;
  bool                    add_reestab_info_list_r13_present = false;
  uint16_t                source_pci_r13                    = 0;
  fixed_bitstring<16>     target_cell_short_mac_i_r13;
  add_reestab_info_list_l add_reestab_info_list_r13;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AS-Context-NB ::= SEQUENCE
struct as_context_nb_s {
  bool            ext                    = false;
  bool            reest_info_r13_present = false;
  reest_info_nb_s reest_info_r13;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// AdditionalBandInfoList-NB-r14 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (1..256)
using add_band_info_list_nb_r14_l = bounded_array<uint16_t, 8>;

// ChannelRasterOffset-NB-r13 ::= ENUMERATED
struct ch_raster_offset_nb_r13_opts {
  enum options { khz_minus7dot5, khz_minus2dot5, khz2dot5, khz7dot5, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
typedef enumerated<ch_raster_offset_nb_r13_opts> ch_raster_offset_nb_r13_e;

// Guardband-NB-r13 ::= SEQUENCE
struct guardband_nb_r13_s {
  ch_raster_offset_nb_r13_e raster_offset_r13;
  fixed_bitstring<3>        spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Inband-DifferentPCI-NB-r13 ::= SEQUENCE
struct inband_different_pci_nb_r13_s {
  struct eutra_num_crs_ports_r13_opts {
    enum options { same, four, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<eutra_num_crs_ports_r13_opts> eutra_num_crs_ports_r13_e_;

  // member variables
  eutra_num_crs_ports_r13_e_ eutra_num_crs_ports_r13;
  ch_raster_offset_nb_r13_e  raster_offset_r13;
  fixed_bitstring<2>         spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Inband-SamePCI-NB-r13 ::= SEQUENCE
struct inband_same_pci_nb_r13_s {
  uint8_t eutra_crs_seq_info_r13 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Standalone-NB-r13 ::= SEQUENCE
struct standalone_nb_r13_s {
  fixed_bitstring<5> spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MasterInformationBlock-NB ::= SEQUENCE
struct mib_nb_s {
  struct operation_mode_info_r13_c_ {
    struct types_opts {
      enum options { inband_same_pci_r13, inband_different_pci_r13, guardband_r13, standalone_r13, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    operation_mode_info_r13_c_() = default;
    operation_mode_info_r13_c_(const operation_mode_info_r13_c_& other);
    operation_mode_info_r13_c_& operator=(const operation_mode_info_r13_c_& other);
    ~operation_mode_info_r13_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    inband_same_pci_nb_r13_s& inband_same_pci_r13()
    {
      assert_choice_type(types::inband_same_pci_r13, type_, "operationModeInfo-r13");
      return c.get<inband_same_pci_nb_r13_s>();
    }
    inband_different_pci_nb_r13_s& inband_different_pci_r13()
    {
      assert_choice_type(types::inband_different_pci_r13, type_, "operationModeInfo-r13");
      return c.get<inband_different_pci_nb_r13_s>();
    }
    guardband_nb_r13_s& guardband_r13()
    {
      assert_choice_type(types::guardband_r13, type_, "operationModeInfo-r13");
      return c.get<guardband_nb_r13_s>();
    }
    standalone_nb_r13_s& standalone_r13()
    {
      assert_choice_type(types::standalone_r13, type_, "operationModeInfo-r13");
      return c.get<standalone_nb_r13_s>();
    }
    const inband_same_pci_nb_r13_s& inband_same_pci_r13() const
    {
      assert_choice_type(types::inband_same_pci_r13, type_, "operationModeInfo-r13");
      return c.get<inband_same_pci_nb_r13_s>();
    }
    const inband_different_pci_nb_r13_s& inband_different_pci_r13() const
    {
      assert_choice_type(types::inband_different_pci_r13, type_, "operationModeInfo-r13");
      return c.get<inband_different_pci_nb_r13_s>();
    }
    const guardband_nb_r13_s& guardband_r13() const
    {
      assert_choice_type(types::guardband_r13, type_, "operationModeInfo-r13");
      return c.get<guardband_nb_r13_s>();
    }
    const standalone_nb_r13_s& standalone_r13() const
    {
      assert_choice_type(types::standalone_r13, type_, "operationModeInfo-r13");
      return c.get<standalone_nb_r13_s>();
    }
    inband_same_pci_nb_r13_s&      set_inband_same_pci_r13();
    inband_different_pci_nb_r13_s& set_inband_different_pci_r13();
    guardband_nb_r13_s&            set_guardband_r13();
    standalone_nb_r13_s&           set_standalone_r13();

  private:
    types type_;
    choice_buffer_t<guardband_nb_r13_s, inband_different_pci_nb_r13_s, inband_same_pci_nb_r13_s, standalone_nb_r13_s> c;

    void destroy_();
  };
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
  fixed_bitstring<4>         sys_frame_num_msb_r13;
  fixed_bitstring<2>         hyper_sfn_lsb_r13;
  uint8_t                    sched_info_sib1_r13    = 0;
  uint8_t                    sys_info_value_tag_r13 = 0;
  bool                       ab_enabled_r13         = false;
  operation_mode_info_r13_c_ operation_mode_info_r13;
  bool                       add_tx_sib1_r15          = false;
  bool                       ab_enabled_minus5_gc_r16 = false;
  part_earfcn_minus17_c_     part_earfcn_minus17;
  fixed_bitstring<6>         spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCCH-BCH-MessageType-NB ::= MasterInformationBlock-NB
using bcch_bch_msg_type_nb_s = mib_nb_s;

// BCCH-BCH-Message-NB ::= SEQUENCE
struct bcch_bch_msg_nb_s {
  bcch_bch_msg_type_nb_s msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB-GuardbandAnchorTDD-NB-r15 ::= SEQUENCE
struct sib_guardband_anchor_tdd_nb_r15_s {
  fixed_bitstring<1> spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB-GuardbandGuardbandTDD-NB-r15 ::= SEQUENCE
struct sib_guardband_guardband_tdd_nb_r15_s {
  struct sib_guardband_guardband_location_r15_opts {
    enum options { same, opposite, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<sib_guardband_guardband_location_r15_opts> sib_guardband_guardband_location_r15_e_;

  // member variables
  sib_guardband_guardband_location_r15_e_ sib_guardband_guardband_location_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB-GuardbandInbandDiffPCI-TDD-NB-r15 ::= SEQUENCE
struct sib_guardband_inband_diff_pci_tdd_nb_r15_s {
  struct sib_eutra_num_crs_ports_r15_opts {
    enum options { same, four, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sib_eutra_num_crs_ports_r15_opts> sib_eutra_num_crs_ports_r15_e_;

  // member variables
  sib_eutra_num_crs_ports_r15_e_ sib_eutra_num_crs_ports_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB-GuardbandInbandSamePCI-TDD-NB-r15 ::= SEQUENCE
struct sib_guardband_inband_same_pci_tdd_nb_r15_s {
  fixed_bitstring<1> spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GuardbandTDD-NB-r15 ::= SEQUENCE
struct guardband_tdd_nb_r15_s {
  struct sib_guardband_info_r15_c_ {
    struct types_opts {
      enum options {
        sib_guardband_anchor_r15,
        sib_guardband_guardband_r15,
        sib_guardband_inband_same_pci_r15,
        sib_guardbandinband_diff_pci_r15,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    sib_guardband_info_r15_c_() = default;
    sib_guardband_info_r15_c_(const sib_guardband_info_r15_c_& other);
    sib_guardband_info_r15_c_& operator=(const sib_guardband_info_r15_c_& other);
    ~sib_guardband_info_r15_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sib_guardband_anchor_tdd_nb_r15_s& sib_guardband_anchor_r15()
    {
      assert_choice_type(types::sib_guardband_anchor_r15, type_, "sib-GuardbandInfo-r15");
      return c.get<sib_guardband_anchor_tdd_nb_r15_s>();
    }
    sib_guardband_guardband_tdd_nb_r15_s& sib_guardband_guardband_r15()
    {
      assert_choice_type(types::sib_guardband_guardband_r15, type_, "sib-GuardbandInfo-r15");
      return c.get<sib_guardband_guardband_tdd_nb_r15_s>();
    }
    sib_guardband_inband_same_pci_tdd_nb_r15_s& sib_guardband_inband_same_pci_r15()
    {
      assert_choice_type(types::sib_guardband_inband_same_pci_r15, type_, "sib-GuardbandInfo-r15");
      return c.get<sib_guardband_inband_same_pci_tdd_nb_r15_s>();
    }
    sib_guardband_inband_diff_pci_tdd_nb_r15_s& sib_guardbandinband_diff_pci_r15()
    {
      assert_choice_type(types::sib_guardbandinband_diff_pci_r15, type_, "sib-GuardbandInfo-r15");
      return c.get<sib_guardband_inband_diff_pci_tdd_nb_r15_s>();
    }
    const sib_guardband_anchor_tdd_nb_r15_s& sib_guardband_anchor_r15() const
    {
      assert_choice_type(types::sib_guardband_anchor_r15, type_, "sib-GuardbandInfo-r15");
      return c.get<sib_guardband_anchor_tdd_nb_r15_s>();
    }
    const sib_guardband_guardband_tdd_nb_r15_s& sib_guardband_guardband_r15() const
    {
      assert_choice_type(types::sib_guardband_guardband_r15, type_, "sib-GuardbandInfo-r15");
      return c.get<sib_guardband_guardband_tdd_nb_r15_s>();
    }
    const sib_guardband_inband_same_pci_tdd_nb_r15_s& sib_guardband_inband_same_pci_r15() const
    {
      assert_choice_type(types::sib_guardband_inband_same_pci_r15, type_, "sib-GuardbandInfo-r15");
      return c.get<sib_guardband_inband_same_pci_tdd_nb_r15_s>();
    }
    const sib_guardband_inband_diff_pci_tdd_nb_r15_s& sib_guardbandinband_diff_pci_r15() const
    {
      assert_choice_type(types::sib_guardbandinband_diff_pci_r15, type_, "sib-GuardbandInfo-r15");
      return c.get<sib_guardband_inband_diff_pci_tdd_nb_r15_s>();
    }
    sib_guardband_anchor_tdd_nb_r15_s&          set_sib_guardband_anchor_r15();
    sib_guardband_guardband_tdd_nb_r15_s&       set_sib_guardband_guardband_r15();
    sib_guardband_inband_same_pci_tdd_nb_r15_s& set_sib_guardband_inband_same_pci_r15();
    sib_guardband_inband_diff_pci_tdd_nb_r15_s& set_sib_guardbandinband_diff_pci_r15();

  private:
    types type_;
    choice_buffer_t<sib_guardband_anchor_tdd_nb_r15_s,
                    sib_guardband_guardband_tdd_nb_r15_s,
                    sib_guardband_inband_diff_pci_tdd_nb_r15_s,
                    sib_guardband_inband_same_pci_tdd_nb_r15_s>
        c;

    void destroy_();
  };
  struct eutra_bandwitdh_r15_opts {
    enum options { bw5or10, bw15or20, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<eutra_bandwitdh_r15_opts> eutra_bandwitdh_r15_e_;

  // member variables
  ch_raster_offset_nb_r13_e raster_offset_r15;
  sib_guardband_info_r15_c_ sib_guardband_info_r15;
  eutra_bandwitdh_r15_e_    eutra_bandwitdh_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Inband-DifferentPCI-TDD-NB-r15 ::= SEQUENCE
struct inband_different_pci_tdd_nb_r15_s {
  struct eutra_num_crs_ports_r15_opts {
    enum options { same, four, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<eutra_num_crs_ports_r15_opts> eutra_num_crs_ports_r15_e_;
  struct sib_inband_location_r15_opts {
    enum options { lower, higher, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<sib_inband_location_r15_opts> sib_inband_location_r15_e_;

  // member variables
  eutra_num_crs_ports_r15_e_ eutra_num_crs_ports_r15;
  ch_raster_offset_nb_r13_e  raster_offset_r15;
  sib_inband_location_r15_e_ sib_inband_location_r15;
  fixed_bitstring<2>         spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Inband-SamePCI-TDD-NB-r15 ::= SEQUENCE
struct inband_same_pci_tdd_nb_r15_s {
  struct sib_inband_location_r15_opts {
    enum options { lower, higher, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<sib_inband_location_r15_opts> sib_inband_location_r15_e_;

  // member variables
  uint8_t                    eutra_crs_seq_info_r15 = 0;
  sib_inband_location_r15_e_ sib_inband_location_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// StandaloneTDD-NB-r15 ::= SEQUENCE
struct standalone_tdd_nb_r15_s {
  struct sib_standalone_location_r15_opts {
    enum options { lower, higher, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<sib_standalone_location_r15_opts> sib_standalone_location_r15_e_;

  // member variables
  sib_standalone_location_r15_e_ sib_standalone_location_r15;
  fixed_bitstring<5>             spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MasterInformationBlock-TDD-NB-r15 ::= SEQUENCE
struct mib_tdd_nb_r15_s {
  struct operation_mode_info_r15_c_ {
    struct types_opts {
      enum options { inband_same_pci_r15, inband_different_pci_r15, guardband_r15, standalone_r15, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    operation_mode_info_r15_c_() = default;
    operation_mode_info_r15_c_(const operation_mode_info_r15_c_& other);
    operation_mode_info_r15_c_& operator=(const operation_mode_info_r15_c_& other);
    ~operation_mode_info_r15_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    inband_same_pci_tdd_nb_r15_s& inband_same_pci_r15()
    {
      assert_choice_type(types::inband_same_pci_r15, type_, "operationModeInfo-r15");
      return c.get<inband_same_pci_tdd_nb_r15_s>();
    }
    inband_different_pci_tdd_nb_r15_s& inband_different_pci_r15()
    {
      assert_choice_type(types::inband_different_pci_r15, type_, "operationModeInfo-r15");
      return c.get<inband_different_pci_tdd_nb_r15_s>();
    }
    guardband_tdd_nb_r15_s& guardband_r15()
    {
      assert_choice_type(types::guardband_r15, type_, "operationModeInfo-r15");
      return c.get<guardband_tdd_nb_r15_s>();
    }
    standalone_tdd_nb_r15_s& standalone_r15()
    {
      assert_choice_type(types::standalone_r15, type_, "operationModeInfo-r15");
      return c.get<standalone_tdd_nb_r15_s>();
    }
    const inband_same_pci_tdd_nb_r15_s& inband_same_pci_r15() const
    {
      assert_choice_type(types::inband_same_pci_r15, type_, "operationModeInfo-r15");
      return c.get<inband_same_pci_tdd_nb_r15_s>();
    }
    const inband_different_pci_tdd_nb_r15_s& inband_different_pci_r15() const
    {
      assert_choice_type(types::inband_different_pci_r15, type_, "operationModeInfo-r15");
      return c.get<inband_different_pci_tdd_nb_r15_s>();
    }
    const guardband_tdd_nb_r15_s& guardband_r15() const
    {
      assert_choice_type(types::guardband_r15, type_, "operationModeInfo-r15");
      return c.get<guardband_tdd_nb_r15_s>();
    }
    const standalone_tdd_nb_r15_s& standalone_r15() const
    {
      assert_choice_type(types::standalone_r15, type_, "operationModeInfo-r15");
      return c.get<standalone_tdd_nb_r15_s>();
    }
    inband_same_pci_tdd_nb_r15_s&      set_inband_same_pci_r15();
    inband_different_pci_tdd_nb_r15_s& set_inband_different_pci_r15();
    guardband_tdd_nb_r15_s&            set_guardband_r15();
    standalone_tdd_nb_r15_s&           set_standalone_r15();

  private:
    types type_;
    choice_buffer_t<guardband_tdd_nb_r15_s,
                    inband_different_pci_tdd_nb_r15_s,
                    inband_same_pci_tdd_nb_r15_s,
                    standalone_tdd_nb_r15_s>
        c;

    void destroy_();
  };
  struct sib1_carrier_info_r15_opts {
    enum options { anchor, non_anchor, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<sib1_carrier_info_r15_opts> sib1_carrier_info_r15_e_;

  // member variables
  fixed_bitstring<4>         sys_frame_num_msb_r15;
  fixed_bitstring<2>         hyper_sfn_lsb_r15;
  uint8_t                    sched_info_sib1_r15    = 0;
  uint8_t                    sys_info_value_tag_r15 = 0;
  bool                       ab_enabled_r15         = false;
  operation_mode_info_r15_c_ operation_mode_info_r15;
  sib1_carrier_info_r15_e_   sib1_carrier_info_r15;
  bool                       ab_enabled_minus5_gc_r16 = false;
  fixed_bitstring<8>         spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCCH-BCH-MessageType-TDD-NB-r15 ::= MasterInformationBlock-TDD-NB-r15
using bcch_bch_msg_type_tdd_nb_r15_s = mib_tdd_nb_r15_s;

// BCCH-BCH-Message-TDD-NB ::= SEQUENCE
struct bcch_bch_msg_tdd_nb_s {
  bcch_bch_msg_type_tdd_nb_r15_s msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TrackingAreaList-NB-r17 ::= SEQUENCE (SIZE (1..12)) OF BIT STRING (SIZE (16))
using tracking_area_list_nb_r17_l = bounded_array<fixed_bitstring<16>, 12>;

// NS-PmaxValue-NB-r13 ::= SEQUENCE
struct ns_pmax_value_nb_r13_s {
  bool    add_pmax_r13_present  = false;
  int8_t  add_pmax_r13          = -30;
  uint8_t add_spec_emission_r13 = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityInfo-NB-v1700 ::= SEQUENCE
struct plmn_id_info_nb_v1700_s {
  bool                        tracking_area_list_r17_present = false;
  tracking_area_list_nb_r17_l tracking_area_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB-Type-NB-v1530 ::= ENUMERATED
struct sib_type_nb_v1530_opts {
  enum options {
    sib_type23_nb_r15,
    sib_type27_nb_r16,
    sib_type31_nb_r17,
    sib_type32_nb_r17,
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
typedef enumerated<sib_type_nb_v1530_opts> sib_type_nb_v1530_e;

// GWUS-NumGroups-NB-r16 ::= ENUMERATED
struct gwus_num_groups_nb_r16_opts {
  enum options { n1, n2, n4, n8, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<gwus_num_groups_nb_r16_opts> gwus_num_groups_nb_r16_e;

// NS-PmaxList-NB-r13 ::= SEQUENCE (SIZE (1..4)) OF NS-PmaxValue-NB-r13
using ns_pmax_list_nb_r13_l = dyn_array<ns_pmax_value_nb_r13_s>;

// NSSS-RRM-Config-NB-r15 ::= SEQUENCE
struct nsss_rrm_cfg_nb_r15_s {
  struct nsss_rrm_pwr_offset_r15_opts {
    enum options { db_minus3, db0, db3, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<nsss_rrm_pwr_offset_r15_opts> nsss_rrm_pwr_offset_r15_e_;
  struct nsss_num_occ_diff_precoders_r15_opts {
    enum options { n1, n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nsss_num_occ_diff_precoders_r15_opts> nsss_num_occ_diff_precoders_r15_e_;

  // member variables
  bool                               nsss_num_occ_diff_precoders_r15_present = false;
  nsss_rrm_pwr_offset_r15_e_         nsss_rrm_pwr_offset_r15;
  nsss_num_occ_diff_precoders_r15_e_ nsss_num_occ_diff_precoders_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityInfo-5GC-NB-r16 ::= SEQUENCE
struct plmn_id_info_minus5_gc_nb_r16_s {
  struct plmn_id_minus5_gc_r16_c_ {
    struct types_opts {
      enum options { plmn_id_r16, plmn_idx_r16, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    plmn_id_minus5_gc_r16_c_() = default;
    plmn_id_minus5_gc_r16_c_(const plmn_id_minus5_gc_r16_c_& other);
    plmn_id_minus5_gc_r16_c_& operator=(const plmn_id_minus5_gc_r16_c_& other);
    ~plmn_id_minus5_gc_r16_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    plmn_id_s& plmn_id_r16()
    {
      assert_choice_type(types::plmn_id_r16, type_, "plmn-Identity-5GC-r16");
      return c.get<plmn_id_s>();
    }
    uint8_t& plmn_idx_r16()
    {
      assert_choice_type(types::plmn_idx_r16, type_, "plmn-Identity-5GC-r16");
      return c.get<uint8_t>();
    }
    const plmn_id_s& plmn_id_r16() const
    {
      assert_choice_type(types::plmn_id_r16, type_, "plmn-Identity-5GC-r16");
      return c.get<plmn_id_s>();
    }
    const uint8_t& plmn_idx_r16() const
    {
      assert_choice_type(types::plmn_idx_r16, type_, "plmn-Identity-5GC-r16");
      return c.get<uint8_t>();
    }
    plmn_id_s& set_plmn_id_r16();
    uint8_t&   set_plmn_idx_r16();

  private:
    types                      type_;
    choice_buffer_t<plmn_id_s> c;

    void destroy_();
  };
  struct cell_reserved_for_oper_r16_opts {
    enum options { reserved, not_reserved, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<cell_reserved_for_oper_r16_opts> cell_reserved_for_oper_r16_e_;

  // member variables
  bool                          ng_u_data_transfer_r16_present       = false;
  bool                          up_cio_t_minus5_gs_optim_r16_present = false;
  plmn_id_minus5_gc_r16_c_      plmn_id_minus5_gc_r16;
  cell_reserved_for_oper_r16_e_ cell_reserved_for_oper_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityList-NB-v1700 ::= SEQUENCE (SIZE (1..6)) OF PLMN-IdentityInfo-NB-v1700
using plmn_id_list_nb_v1700_l = dyn_array<plmn_id_info_nb_v1700_s>;

// SIB-MappingInfo-NB-v1530 ::= SEQUENCE (SIZE (1..8)) OF SIB-Type-NB-v1530
using sib_map_info_nb_v1530_l = bounded_array<sib_type_nb_v1530_e, 8>;

// EDT-TBS-NB-r15 ::= SEQUENCE
struct edt_tbs_nb_r15_s {
  struct edt_tbs_r15_opts {
    enum options { b328, b408, b504, b584, b680, b808, b936, b1000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<edt_tbs_r15_opts> edt_tbs_r15_e_;

  // member variables
  bool           edt_small_tbs_enabled_r15 = false;
  edt_tbs_r15_e_ edt_tbs_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GWUS-GroupsForServiceList-NB-r16 ::= SEQUENCE (SIZE (1..3)) OF INTEGER (1..15)
using gwus_groups_for_service_list_nb_r16_l = bounded_array<uint8_t, 3>;

// GWUS-NumGroupsList-NB-r16 ::= SEQUENCE (SIZE (1..2)) OF GWUS-NumGroups-NB-r16
using gwus_num_groups_list_nb_r16_l = bounded_array<gwus_num_groups_nb_r16_e, 2>;

// GWUS-Paging-ProbThresh-NB-r16 ::= ENUMERATED
struct gwus_paging_prob_thresh_nb_r16_opts {
  enum options { p20, p30, p40, p50, p60, p70, p80, p90, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<gwus_paging_prob_thresh_nb_r16_opts> gwus_paging_prob_thresh_nb_r16_e;

// InterFreqNeighCellInfo-NB-v1530 ::= SEQUENCE
struct inter_freq_neigh_cell_info_nb_v1530_s {
  bool                  nsss_rrm_cfg_r15_present = false;
  nsss_rrm_cfg_nb_r15_s nsss_rrm_cfg_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MultiBandInfo-NB-r13 ::= SEQUENCE
struct multi_band_info_nb_r13_s {
  bool                  freq_band_ind_r13_present  = false;
  bool                  freq_band_info_r13_present = false;
  uint16_t              freq_band_ind_r13          = 1;
  ns_pmax_list_nb_r13_l freq_band_info_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPRACH-Parameters-NB-r13 ::= SEQUENCE
struct nprach_params_nb_r13_s {
  struct nprach_periodicity_r13_opts {
    enum options { ms40, ms80, ms160, ms240, ms320, ms640, ms1280, ms2560, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<nprach_periodicity_r13_opts> nprach_periodicity_r13_e_;
  struct nprach_start_time_r13_opts {
    enum options { ms8, ms16, ms32, ms64, ms128, ms256, ms512, ms1024, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<nprach_start_time_r13_opts> nprach_start_time_r13_e_;
  struct nprach_subcarrier_offset_r13_opts {
    enum options { n0, n12, n24, n36, n2, n18, n34, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nprach_subcarrier_offset_r13_opts> nprach_subcarrier_offset_r13_e_;
  struct nprach_num_subcarriers_r13_opts {
    enum options { n12, n24, n36, n48, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nprach_num_subcarriers_r13_opts> nprach_num_subcarriers_r13_e_;
  struct nprach_subcarrier_msg3_range_start_r13_opts {
    enum options { zero, one_third, two_third, one, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<nprach_subcarrier_msg3_range_start_r13_opts> nprach_subcarrier_msg3_range_start_r13_e_;
  struct max_num_preamb_attempt_ce_r13_opts {
    enum options { n3, n4, n5, n6, n7, n8, n10, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_preamb_attempt_ce_r13_opts> max_num_preamb_attempt_ce_r13_e_;
  struct num_repeats_per_preamb_attempt_r13_opts {
    enum options { n1, n2, n4, n8, n16, n32, n64, n128, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<num_repeats_per_preamb_attempt_r13_opts> num_repeats_per_preamb_attempt_r13_e_;
  struct npdcch_num_repeats_ra_r13_opts {
    enum options {
      r1,
      r2,
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
  typedef enumerated<npdcch_num_repeats_ra_r13_opts> npdcch_num_repeats_ra_r13_e_;
  struct npdcch_start_sf_css_ra_r13_opts {
    enum options { v1dot5, v2, v4, v8, v16, v32, v48, v64, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<npdcch_start_sf_css_ra_r13_opts> npdcch_start_sf_css_ra_r13_e_;
  struct npdcch_offset_ra_r13_opts {
    enum options { zero, one_eighth, one_fourth, three_eighth, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<npdcch_offset_ra_r13_opts> npdcch_offset_ra_r13_e_;

  // member variables
  nprach_periodicity_r13_e_                 nprach_periodicity_r13;
  nprach_start_time_r13_e_                  nprach_start_time_r13;
  nprach_subcarrier_offset_r13_e_           nprach_subcarrier_offset_r13;
  nprach_num_subcarriers_r13_e_             nprach_num_subcarriers_r13;
  nprach_subcarrier_msg3_range_start_r13_e_ nprach_subcarrier_msg3_range_start_r13;
  max_num_preamb_attempt_ce_r13_e_          max_num_preamb_attempt_ce_r13;
  num_repeats_per_preamb_attempt_r13_e_     num_repeats_per_preamb_attempt_r13;
  npdcch_num_repeats_ra_r13_e_              npdcch_num_repeats_ra_r13;
  npdcch_start_sf_css_ra_r13_e_             npdcch_start_sf_css_ra_r13;
  npdcch_offset_ra_r13_e_                   npdcch_offset_ra_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPRACH-Parameters-NB-r14 ::= SEQUENCE
struct nprach_params_nb_r14_s {
  struct nprach_params_r14_s_ {
    struct nprach_periodicity_r14_opts {
      enum options { ms40, ms80, ms160, ms240, ms320, ms640, ms1280, ms2560, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<nprach_periodicity_r14_opts> nprach_periodicity_r14_e_;
    struct nprach_start_time_r14_opts {
      enum options { ms8, ms16, ms32, ms64, ms128, ms256, ms512, ms1024, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<nprach_start_time_r14_opts> nprach_start_time_r14_e_;
    struct nprach_subcarrier_offset_r14_opts {
      enum options { n0, n12, n24, n36, n2, n18, n34, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<nprach_subcarrier_offset_r14_opts> nprach_subcarrier_offset_r14_e_;
    struct nprach_num_subcarriers_r14_opts {
      enum options { n12, n24, n36, n48, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<nprach_num_subcarriers_r14_opts> nprach_num_subcarriers_r14_e_;
    struct nprach_subcarrier_msg3_range_start_r14_opts {
      enum options { zero, one_third, two_third, one, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<nprach_subcarrier_msg3_range_start_r14_opts> nprach_subcarrier_msg3_range_start_r14_e_;
    struct npdcch_num_repeats_ra_r14_opts {
      enum options {
        r1,
        r2,
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
    typedef enumerated<npdcch_num_repeats_ra_r14_opts> npdcch_num_repeats_ra_r14_e_;
    struct npdcch_start_sf_css_ra_r14_opts {
      enum options { v1dot5, v2, v4, v8, v16, v32, v48, v64, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<npdcch_start_sf_css_ra_r14_opts> npdcch_start_sf_css_ra_r14_e_;
    struct npdcch_offset_ra_r14_opts {
      enum options { zero, one_eighth, one_fourth, three_eighth, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<npdcch_offset_ra_r14_opts> npdcch_offset_ra_r14_e_;
    struct nprach_num_cbra_start_subcarriers_r14_opts {
      enum options { n8, n10, n11, n12, n20, n22, n23, n24, n32, n34, n35, n36, n40, n44, n46, n48, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<nprach_num_cbra_start_subcarriers_r14_opts> nprach_num_cbra_start_subcarriers_r14_e_;

    // member variables
    bool                                      ext                                            = false;
    bool                                      nprach_periodicity_r14_present                 = false;
    bool                                      nprach_start_time_r14_present                  = false;
    bool                                      nprach_subcarrier_offset_r14_present           = false;
    bool                                      nprach_num_subcarriers_r14_present             = false;
    bool                                      nprach_subcarrier_msg3_range_start_r14_present = false;
    bool                                      npdcch_num_repeats_ra_r14_present              = false;
    bool                                      npdcch_start_sf_css_ra_r14_present             = false;
    bool                                      npdcch_offset_ra_r14_present                   = false;
    bool                                      nprach_num_cbra_start_subcarriers_r14_present  = false;
    bool                                      npdcch_carrier_idx_r14_present                 = false;
    nprach_periodicity_r14_e_                 nprach_periodicity_r14;
    nprach_start_time_r14_e_                  nprach_start_time_r14;
    nprach_subcarrier_offset_r14_e_           nprach_subcarrier_offset_r14;
    nprach_num_subcarriers_r14_e_             nprach_num_subcarriers_r14;
    nprach_subcarrier_msg3_range_start_r14_e_ nprach_subcarrier_msg3_range_start_r14;
    npdcch_num_repeats_ra_r14_e_              npdcch_num_repeats_ra_r14;
    npdcch_start_sf_css_ra_r14_e_             npdcch_start_sf_css_ra_r14;
    npdcch_offset_ra_r14_e_                   npdcch_offset_ra_r14;
    nprach_num_cbra_start_subcarriers_r14_e_  nprach_num_cbra_start_subcarriers_r14;
    uint8_t                                   npdcch_carrier_idx_r14 = 1;
    // ...
  };

  // member variables
  bool                 nprach_params_r14_present = false;
  nprach_params_r14_s_ nprach_params_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPRACH-Parameters-NB-v1330 ::= SEQUENCE
struct nprach_params_nb_v1330_s {
  struct nprach_num_cbra_start_subcarriers_r13_opts {
    enum options { n8, n10, n11, n12, n20, n22, n23, n24, n32, n34, n35, n36, n40, n44, n46, n48, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nprach_num_cbra_start_subcarriers_r13_opts> nprach_num_cbra_start_subcarriers_r13_e_;

  // member variables
  nprach_num_cbra_start_subcarriers_r13_e_ nprach_num_cbra_start_subcarriers_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPRACH-ParametersFmt2-NB-r15 ::= SEQUENCE
struct nprach_params_fmt2_nb_r15_s {
  struct nprach_params_r15_s_ {
    struct nprach_periodicity_r15_opts {
      enum options { ms40, ms80, ms160, ms320, ms640, ms1280, ms2560, ms5120, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<nprach_periodicity_r15_opts> nprach_periodicity_r15_e_;
    struct nprach_start_time_r15_opts {
      enum options { ms8, ms16, ms32, ms64, ms128, ms256, ms512, ms1024, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<nprach_start_time_r15_opts> nprach_start_time_r15_e_;
    struct nprach_subcarrier_offset_r15_opts {
      enum options { n0, n36, n72, n108, n6, n54, n102, n42, n78, n90, n12, n24, n48, n84, n60, n18, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<nprach_subcarrier_offset_r15_opts> nprach_subcarrier_offset_r15_e_;
    struct nprach_num_subcarriers_r15_opts {
      enum options { n36, n72, n108, n144, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<nprach_num_subcarriers_r15_opts> nprach_num_subcarriers_r15_e_;
    struct nprach_subcarrier_msg3_range_start_r15_opts {
      enum options { zero, one_third, two_third, one, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<nprach_subcarrier_msg3_range_start_r15_opts> nprach_subcarrier_msg3_range_start_r15_e_;
    struct npdcch_num_repeats_ra_r15_opts {
      enum options {
        r1,
        r2,
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
    typedef enumerated<npdcch_num_repeats_ra_r15_opts> npdcch_num_repeats_ra_r15_e_;
    struct npdcch_start_sf_css_ra_r15_opts {
      enum options { v1dot5, v2, v4, v8, v16, v32, v48, v64, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<npdcch_start_sf_css_ra_r15_opts> npdcch_start_sf_css_ra_r15_e_;
    struct npdcch_offset_ra_r15_opts {
      enum options { zero, one_eighth, one_fourth, three_eighth, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<npdcch_offset_ra_r15_opts> npdcch_offset_ra_r15_e_;
    struct nprach_num_cbra_start_subcarriers_r15_opts {
      enum options {
        n24,
        n30,
        n33,
        n36,
        n60,
        n66,
        n69,
        n72,
        n96,
        n102,
        n105,
        n108,
        n120,
        n132,
        n138,
        n144,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<nprach_num_cbra_start_subcarriers_r15_opts> nprach_num_cbra_start_subcarriers_r15_e_;

    // member variables
    bool                                      ext                                            = false;
    bool                                      nprach_periodicity_r15_present                 = false;
    bool                                      nprach_start_time_r15_present                  = false;
    bool                                      nprach_subcarrier_offset_r15_present           = false;
    bool                                      nprach_num_subcarriers_r15_present             = false;
    bool                                      nprach_subcarrier_msg3_range_start_r15_present = false;
    bool                                      npdcch_num_repeats_ra_r15_present              = false;
    bool                                      npdcch_start_sf_css_ra_r15_present             = false;
    bool                                      npdcch_offset_ra_r15_present                   = false;
    bool                                      nprach_num_cbra_start_subcarriers_r15_present  = false;
    bool                                      npdcch_carrier_idx_r15_present                 = false;
    nprach_periodicity_r15_e_                 nprach_periodicity_r15;
    nprach_start_time_r15_e_                  nprach_start_time_r15;
    nprach_subcarrier_offset_r15_e_           nprach_subcarrier_offset_r15;
    nprach_num_subcarriers_r15_e_             nprach_num_subcarriers_r15;
    nprach_subcarrier_msg3_range_start_r15_e_ nprach_subcarrier_msg3_range_start_r15;
    npdcch_num_repeats_ra_r15_e_              npdcch_num_repeats_ra_r15;
    npdcch_start_sf_css_ra_r15_e_             npdcch_start_sf_css_ra_r15;
    npdcch_offset_ra_r15_e_                   npdcch_offset_ra_r15;
    nprach_num_cbra_start_subcarriers_r15_e_  nprach_num_cbra_start_subcarriers_r15;
    uint8_t                                   npdcch_carrier_idx_r15 = 1;
    // ...
  };

  // member variables
  bool                 nprach_params_r15_present = false;
  nprach_params_r15_s_ nprach_params_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPRACH-ParametersTDD-NB-r15 ::= SEQUENCE
struct nprach_params_tdd_nb_r15_s {
  struct nprach_params_r15_s_ {
    struct nprach_periodicity_r15_opts {
      enum options { ms80, ms160, ms320, ms640, ms1280, ms2560, ms5120, ms10240, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<nprach_periodicity_r15_opts> nprach_periodicity_r15_e_;
    struct nprach_start_time_r15_opts {
      enum options {
        ms10,
        ms20,
        ms40,
        ms80,
        ms160,
        ms320,
        ms640,
        ms1280,
        ms2560,
        ms5120,
        spare6,
        spare5,
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
    typedef enumerated<nprach_start_time_r15_opts> nprach_start_time_r15_e_;
    struct nprach_subcarrier_offset_r15_opts {
      enum options { n0, n12, n24, n36, n2, n18, n34, spare1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<nprach_subcarrier_offset_r15_opts> nprach_subcarrier_offset_r15_e_;
    struct nprach_num_subcarriers_r15_opts {
      enum options { n12, n24, n36, n48, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<nprach_num_subcarriers_r15_opts> nprach_num_subcarriers_r15_e_;
    struct nprach_subcarrier_msg3_range_start_r15_opts {
      enum options { zero, one_third, two_third, one, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<nprach_subcarrier_msg3_range_start_r15_opts> nprach_subcarrier_msg3_range_start_r15_e_;
    struct npdcch_num_repeats_ra_r15_opts {
      enum options {
        r1,
        r2,
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
    typedef enumerated<npdcch_num_repeats_ra_r15_opts> npdcch_num_repeats_ra_r15_e_;
    struct npdcch_start_sf_css_ra_r15_opts {
      enum options { v4, v8, v16, v32, v48, v64, v96, v128, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<npdcch_start_sf_css_ra_r15_opts> npdcch_start_sf_css_ra_r15_e_;
    struct npdcch_offset_ra_r15_opts {
      enum options { zero, one_eighth, one_fourth, three_eighth, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<npdcch_offset_ra_r15_opts> npdcch_offset_ra_r15_e_;
    struct nprach_num_cbra_start_subcarriers_r15_opts {
      enum options { n8, n10, n11, n12, n20, n22, n23, n24, n32, n34, n35, n36, n40, n44, n46, n48, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<nprach_num_cbra_start_subcarriers_r15_opts> nprach_num_cbra_start_subcarriers_r15_e_;

    // member variables
    bool                                      ext                                            = false;
    bool                                      nprach_periodicity_r15_present                 = false;
    bool                                      nprach_start_time_r15_present                  = false;
    bool                                      nprach_subcarrier_offset_r15_present           = false;
    bool                                      nprach_num_subcarriers_r15_present             = false;
    bool                                      nprach_subcarrier_msg3_range_start_r15_present = false;
    bool                                      npdcch_num_repeats_ra_r15_present              = false;
    bool                                      npdcch_start_sf_css_ra_r15_present             = false;
    bool                                      npdcch_offset_ra_r15_present                   = false;
    bool                                      nprach_num_cbra_start_subcarriers_r15_present  = false;
    nprach_periodicity_r15_e_                 nprach_periodicity_r15;
    nprach_start_time_r15_e_                  nprach_start_time_r15;
    nprach_subcarrier_offset_r15_e_           nprach_subcarrier_offset_r15;
    nprach_num_subcarriers_r15_e_             nprach_num_subcarriers_r15;
    nprach_subcarrier_msg3_range_start_r15_e_ nprach_subcarrier_msg3_range_start_r15;
    npdcch_num_repeats_ra_r15_e_              npdcch_num_repeats_ra_r15;
    npdcch_start_sf_css_ra_r15_e_             npdcch_start_sf_css_ra_r15;
    npdcch_offset_ra_r15_e_                   npdcch_offset_ra_r15;
    nprach_num_cbra_start_subcarriers_r15_e_  nprach_num_cbra_start_subcarriers_r15;
    // ...
  };

  // member variables
  bool                 nprach_params_r15_present = false;
  nprach_params_r15_s_ nprach_params_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPRACH-ParametersTDD-NB-v1550 ::= SEQUENCE
struct nprach_params_tdd_nb_v1550_s {
  struct max_num_preamb_attempt_ce_v1550_opts {
    enum options { n3, n4, n5, n6, n7, n8, n10, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_preamb_attempt_ce_v1550_opts> max_num_preamb_attempt_ce_v1550_e_;
  struct num_repeats_per_preamb_attempt_v1550_opts {
    enum options { n1, n2, n4, n8, n16, n32, n64, n128, n256, n512, n1024, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<num_repeats_per_preamb_attempt_v1550_opts> num_repeats_per_preamb_attempt_v1550_e_;

  // member variables
  max_num_preamb_attempt_ce_v1550_e_      max_num_preamb_attempt_ce_v1550;
  num_repeats_per_preamb_attempt_v1550_e_ num_repeats_per_preamb_attempt_v1550;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityList-5GC-NB-r16 ::= SEQUENCE (SIZE (1..6)) OF PLMN-IdentityInfo-5GC-NB-r16
using plmn_id_list_minus5_gc_nb_r16_l = dyn_array<plmn_id_info_minus5_gc_nb_r16_s>;

// PagingWeight-NB-r14 ::= ENUMERATED
struct paging_weight_nb_r14_opts {
  enum options { w1, w2, w3, w4, w5, w6, w7, w8, w9, w10, w11, w12, w13, w14, w15, w16, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<paging_weight_nb_r14_opts> paging_weight_nb_r14_e;

// RACH-Info-NB-r13 ::= SEQUENCE
struct rach_info_nb_r13_s {
  struct ra_resp_win_size_r13_opts {
    enum options { pp2, pp3, pp4, pp5, pp6, pp7, pp8, pp10, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ra_resp_win_size_r13_opts> ra_resp_win_size_r13_e_;
  struct mac_contention_resolution_timer_r13_opts {
    enum options { pp1, pp2, pp3, pp4, pp8, pp16, pp32, pp64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<mac_contention_resolution_timer_r13_opts> mac_contention_resolution_timer_r13_e_;

  // member variables
  ra_resp_win_size_r13_e_                ra_resp_win_size_r13;
  mac_contention_resolution_timer_r13_e_ mac_contention_resolution_timer_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RACH-Info-NB-v1530 ::= SEQUENCE
struct rach_info_nb_v1530_s {
  struct mac_contention_resolution_timer_r15_opts {
    enum options { pp1, pp2, pp3, pp4, pp8, pp16, pp32, pp64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<mac_contention_resolution_timer_r15_opts> mac_contention_resolution_timer_r15_e_;

  // member variables
  mac_contention_resolution_timer_r15_e_ mac_contention_resolution_timer_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingInfo-NB-v1530 ::= SEQUENCE
struct sched_info_nb_v1530_s {
  bool                    sib_map_info_v1530_present = false;
  sib_map_info_nb_v1530_l sib_map_info_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1-NB-v1700 ::= SEQUENCE
struct sib_type1_nb_v1700_s {
  struct cell_access_related_info_ntn_r17_s_ {
    struct cell_barred_ntn_r17_opts {
      enum options { barred, not_barred, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<cell_barred_ntn_r17_opts> cell_barred_ntn_r17_e_;

    // member variables
    bool                    plmn_id_list_v1700_present = false;
    cell_barred_ntn_r17_e_  cell_barred_ntn_r17;
    plmn_id_list_nb_v1700_l plmn_id_list_v1700;
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

// UAC-BarringPerCat-NB-r16 ::= SEQUENCE
struct uac_barr_per_cat_nb_r16_s {
  struct uac_barr_factor_r16_opts {
    enum options { p00, p05, p10, p15, p20, p25, p30, p40, p50, p60, p70, p75, p80, p85, p90, p95, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<uac_barr_factor_r16_opts> uac_barr_factor_r16_e_;
  struct uac_barr_time_r16_opts {
    enum options { s4, s8, s16, s32, s64, s128, s256, s512, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<uac_barr_time_r16_opts> uac_barr_time_r16_e_;

  // member variables
  uint8_t                uac_access_category_r16 = 1;
  uac_barr_factor_r16_e_ uac_barr_factor_r16;
  uac_barr_time_r16_e_   uac_barr_time_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WUS-MaxDurationFactor-NB-r15 ::= ENUMERATED
struct wus_max_dur_factor_nb_r15_opts {
  enum options { one128th, one64th, one32th, one16th, one_eighth, one_quarter, one_half, nulltype } value;
  typedef float number_type;

  const char* to_string() const;
  float       to_number() const;
  const char* to_number_string() const;
};
typedef enumerated<wus_max_dur_factor_nb_r15_opts> wus_max_dur_factor_nb_r15_e;

// DL-CarrierConfigCommon-NB-r14 ::= SEQUENCE
struct dl_carrier_cfg_common_nb_r14_s {
  struct dl_bitmap_non_anchor_r14_c_ {
    struct types_opts {
      enum options { use_no_bitmap_r14, use_anchor_bitmap_r14, explicit_bitmap_cfg_r14, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    dl_bitmap_non_anchor_r14_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dl_bitmap_nb_r13_c& explicit_bitmap_cfg_r14()
    {
      assert_choice_type(types::explicit_bitmap_cfg_r14, type_, "downlinkBitmapNonAnchor-r14");
      return c;
    }
    const dl_bitmap_nb_r13_c& explicit_bitmap_cfg_r14() const
    {
      assert_choice_type(types::explicit_bitmap_cfg_r14, type_, "downlinkBitmapNonAnchor-r14");
      return c;
    }
    void                set_use_no_bitmap_r14();
    void                set_use_anchor_bitmap_r14();
    dl_bitmap_nb_r13_c& set_explicit_bitmap_cfg_r14();

  private:
    types              type_;
    dl_bitmap_nb_r13_c c;
  };
  struct dl_gap_non_anchor_r14_c_ {
    struct types_opts {
      enum options { use_no_gap_r14, use_anchor_gap_cfg_r14, explicit_gap_cfg_r14, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    dl_gap_non_anchor_r14_c_() = default;
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dl_gap_cfg_nb_r13_s& explicit_gap_cfg_r14()
    {
      assert_choice_type(types::explicit_gap_cfg_r14, type_, "dl-GapNonAnchor-r14");
      return c;
    }
    const dl_gap_cfg_nb_r13_s& explicit_gap_cfg_r14() const
    {
      assert_choice_type(types::explicit_gap_cfg_r14, type_, "dl-GapNonAnchor-r14");
      return c;
    }
    void                 set_use_no_gap_r14();
    void                 set_use_anchor_gap_cfg_r14();
    dl_gap_cfg_nb_r13_s& set_explicit_gap_cfg_r14();

  private:
    types               type_;
    dl_gap_cfg_nb_r13_s c;
  };
  struct inband_carrier_info_r14_s_ {
    struct same_pci_ind_r14_c_ {
      struct same_pci_r14_s_ {
        int8_t idx_to_mid_prb_r14 = -55;
      };
      struct different_pci_r14_s_ {
        struct eutra_num_crs_ports_r14_opts {
          enum options { same, four, nulltype } value;
          typedef uint8_t number_type;

          const char* to_string() const;
          uint8_t     to_number() const;
        };
        typedef enumerated<eutra_num_crs_ports_r14_opts> eutra_num_crs_ports_r14_e_;

        // member variables
        eutra_num_crs_ports_r14_e_ eutra_num_crs_ports_r14;
      };
      struct types_opts {
        enum options { same_pci_r14, different_pci_r14, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      same_pci_ind_r14_c_() = default;
      same_pci_ind_r14_c_(const same_pci_ind_r14_c_& other);
      same_pci_ind_r14_c_& operator=(const same_pci_ind_r14_c_& other);
      ~same_pci_ind_r14_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      same_pci_r14_s_& same_pci_r14()
      {
        assert_choice_type(types::same_pci_r14, type_, "samePCI-Indicator-r14");
        return c.get<same_pci_r14_s_>();
      }
      different_pci_r14_s_& different_pci_r14()
      {
        assert_choice_type(types::different_pci_r14, type_, "samePCI-Indicator-r14");
        return c.get<different_pci_r14_s_>();
      }
      const same_pci_r14_s_& same_pci_r14() const
      {
        assert_choice_type(types::same_pci_r14, type_, "samePCI-Indicator-r14");
        return c.get<same_pci_r14_s_>();
      }
      const different_pci_r14_s_& different_pci_r14() const
      {
        assert_choice_type(types::different_pci_r14, type_, "samePCI-Indicator-r14");
        return c.get<different_pci_r14_s_>();
      }
      same_pci_r14_s_&      set_same_pci_r14();
      different_pci_r14_s_& set_different_pci_r14();

    private:
      types                                                  type_;
      choice_buffer_t<different_pci_r14_s_, same_pci_r14_s_> c;

      void destroy_();
    };
    struct eutra_ctrl_region_size_r14_opts {
      enum options { n1, n2, n3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<eutra_ctrl_region_size_r14_opts> eutra_ctrl_region_size_r14_e_;

    // member variables
    bool                          same_pci_ind_r14_present = false;
    same_pci_ind_r14_c_           same_pci_ind_r14;
    eutra_ctrl_region_size_r14_e_ eutra_ctrl_region_size_r14;
  };
  struct nrs_pwr_offset_non_anchor_r14_opts {
    enum options { db_minus12, db_minus10, db_minus8, db_minus6, db_minus4, db_minus2, db0, db3, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<nrs_pwr_offset_non_anchor_r14_opts> nrs_pwr_offset_non_anchor_r14_e_;

  // member variables
  bool                             ext                                   = false;
  bool                             inband_carrier_info_r14_present       = false;
  bool                             nrs_pwr_offset_non_anchor_r14_present = false;
  carrier_freq_nb_r13_s            dl_carrier_freq_r14;
  dl_bitmap_non_anchor_r14_c_      dl_bitmap_non_anchor_r14;
  dl_gap_non_anchor_r14_c_         dl_gap_non_anchor_r14;
  inband_carrier_info_r14_s_       inband_carrier_info_r14;
  nrs_pwr_offset_non_anchor_r14_e_ nrs_pwr_offset_non_anchor_r14;
  // ...
  // group 0
  copy_ptr<dl_gap_cfg_nb_v1530_s> dl_gap_non_anchor_v1530;
  // group 1
  copy_ptr<carrier_freq_nb_v1550_s> dl_carrier_freq_v1550;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EDT-TBS-InfoList-NB-r15 ::= SEQUENCE (SIZE (1..3)) OF EDT-TBS-NB-r15
using edt_tbs_info_list_nb_r15_l = dyn_array<edt_tbs_nb_r15_s>;

// GWUS-ProbThreshList-NB-r16 ::= SEQUENCE (SIZE (1..3)) OF GWUS-Paging-ProbThresh-NB-r16
using gwus_prob_thresh_list_nb_r16_l = bounded_array<gwus_paging_prob_thresh_nb_r16_e, 3>;

// GWUS-ResourceConfig-NB-r16 ::= SEQUENCE
struct gwus_res_cfg_nb_r16_s {
  struct res_position_r16_opts {
    enum options { primary, secondary, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<res_position_r16_opts> res_position_r16_e_;

  // member variables
  bool                                  num_groups_list_r16_present         = false;
  bool                                  groups_for_service_list_r16_present = false;
  res_position_r16_e_                   res_position_r16;
  gwus_num_groups_list_nb_r16_l         num_groups_list_r16;
  gwus_groups_for_service_list_nb_r16_l groups_for_service_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqExcludedCellList-NB-r13 ::= SEQUENCE (SIZE (1..16)) OF INTEGER (0..503)
using inter_freq_excluded_cell_list_nb_r13_l = bounded_array<uint16_t, 16>;

// InterFreqNeighCellList-NB-r13 ::= SEQUENCE (SIZE (1..16)) OF INTEGER (0..503)
using inter_freq_neigh_cell_list_nb_r13_l = bounded_array<uint16_t, 16>;

// InterFreqNeighCellList-NB-v1530 ::= SEQUENCE (SIZE (1..16)) OF InterFreqNeighCellInfo-NB-v1530
using inter_freq_neigh_cell_list_nb_v1530_l = dyn_array<inter_freq_neigh_cell_info_nb_v1530_s>;

// MultiBandInfoList-NB-r13 ::= SEQUENCE (SIZE (1..8)) OF MultiBandInfo-NB-r13
using multi_band_info_list_nb_r13_l = dyn_array<multi_band_info_nb_r13_s>;

// NPRACH-ParametersList-NB-r13 ::= SEQUENCE (SIZE (1..3)) OF NPRACH-Parameters-NB-r13
using nprach_params_list_nb_r13_l = dyn_array<nprach_params_nb_r13_s>;

// NPRACH-ParametersList-NB-r14 ::= SEQUENCE (SIZE (1..3)) OF NPRACH-Parameters-NB-r14
using nprach_params_list_nb_r14_l = dyn_array<nprach_params_nb_r14_s>;

// NPRACH-ParametersList-NB-v1330 ::= SEQUENCE (SIZE (1..3)) OF NPRACH-Parameters-NB-v1330
using nprach_params_list_nb_v1330_l = dyn_array<nprach_params_nb_v1330_s>;

// NPRACH-ParametersListFmt2-NB-r15 ::= SEQUENCE (SIZE (1..3)) OF NPRACH-ParametersFmt2-NB-r15
using nprach_params_list_fmt2_nb_r15_l = dyn_array<nprach_params_fmt2_nb_r15_s>;

// NPRACH-ParametersListTDD-NB-r15 ::= SEQUENCE (SIZE (1..3)) OF NPRACH-ParametersTDD-NB-r15
using nprach_params_list_tdd_nb_r15_l = dyn_array<nprach_params_tdd_nb_r15_s>;

// NPRACH-ParametersListTDD-NB-v1550 ::= SEQUENCE (SIZE (1..3)) OF NPRACH-ParametersTDD-NB-v1550
using nprach_params_list_tdd_nb_v1550_l = dyn_array<nprach_params_tdd_nb_v1550_s>;

// PCCH-Config-NB-r14 ::= SEQUENCE
struct pcch_cfg_nb_r14_s {
  struct npdcch_num_repeat_paging_r14_opts {
    enum options {
      r1,
      r2,
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
  typedef enumerated<npdcch_num_repeat_paging_r14_opts> npdcch_num_repeat_paging_r14_e_;

  // member variables
  bool                            ext                                  = false;
  bool                            npdcch_num_repeat_paging_r14_present = false;
  bool                            paging_weight_r14_present            = false;
  npdcch_num_repeat_paging_r14_e_ npdcch_num_repeat_paging_r14;
  paging_weight_nb_r14_e          paging_weight_r14;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCCH-Config-NB-r17 ::= SEQUENCE
struct pcch_cfg_nb_r17_s {
  struct npdcch_num_repeat_paging_r17_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, r128, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<npdcch_num_repeat_paging_r17_opts> npdcch_num_repeat_paging_r17_e_;

  // member variables
  bool                            ext                       = false;
  bool                            paging_weight_r17_present = false;
  uint8_t                         cbp_idx_r17               = 1;
  npdcch_num_repeat_paging_r17_e_ npdcch_num_repeat_paging_r17;
  paging_weight_nb_r14_e          paging_weight_r17;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PowerRampingParameters-NB-v1450 ::= SEQUENCE
struct pwr_ramp_params_nb_v1450_s {
  struct preamb_init_rx_target_pwr_v1450_opts {
    enum options {
      dbm_minus130,
      dbm_minus128,
      dbm_minus126,
      dbm_minus124,
      dbm_minus122,
      dbm_minus88,
      dbm_minus86,
      dbm_minus84,
      dbm_minus82,
      dbm_minus80,
      nulltype
    } value;
    typedef int16_t number_type;

    const char* to_string() const;
    int16_t     to_number() const;
  };
  typedef enumerated<preamb_init_rx_target_pwr_v1450_opts> preamb_init_rx_target_pwr_v1450_e_;
  struct pwr_ramp_params_ce1_r14_s_ {
    struct pwr_ramp_step_ce1_r14_opts {
      enum options { db0, db2, db4, db6, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<pwr_ramp_step_ce1_r14_opts> pwr_ramp_step_ce1_r14_e_;
    struct preamb_init_rx_target_pwr_ce1_r14_opts {
      enum options {
        dbm_minus130,
        dbm_minus128,
        dbm_minus126,
        dbm_minus124,
        dbm_minus122,
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
        dbm_minus88,
        dbm_minus86,
        dbm_minus84,
        dbm_minus82,
        dbm_minus80,
        nulltype
      } value;
      typedef int16_t number_type;

      const char* to_string() const;
      int16_t     to_number() const;
    };
    typedef enumerated<preamb_init_rx_target_pwr_ce1_r14_opts> preamb_init_rx_target_pwr_ce1_r14_e_;

    // member variables
    pwr_ramp_step_ce1_r14_e_             pwr_ramp_step_ce1_r14;
    preamb_init_rx_target_pwr_ce1_r14_e_ preamb_init_rx_target_pwr_ce1_r14;
  };

  // member variables
  bool                               preamb_init_rx_target_pwr_v1450_present = false;
  bool                               pwr_ramp_params_ce1_r14_present         = false;
  preamb_init_rx_target_pwr_v1450_e_ preamb_init_rx_target_pwr_v1450;
  pwr_ramp_params_ce1_r14_s_         pwr_ramp_params_ce1_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RACH-InfoList-NB-r13 ::= SEQUENCE (SIZE (1..3)) OF RACH-Info-NB-r13
using rach_info_list_nb_r13_l = dyn_array<rach_info_nb_r13_s>;

// RACH-InfoList-NB-v1530 ::= SEQUENCE (SIZE (1..3)) OF RACH-Info-NB-v1530
using rach_info_list_nb_v1530_l = dyn_array<rach_info_nb_v1530_s>;

// RSRP-ThresholdsNPRACH-InfoList-NB-r13 ::= SEQUENCE (SIZE (1..2)) OF INTEGER (0..97)
using rsrp_thress_nprach_info_list_nb_r13_l = bounded_array<uint8_t, 2>;

// SchedulingInfoList-NB-v1530 ::= SEQUENCE (SIZE (1..8)) OF SchedulingInfo-NB-v1530
using sched_info_list_nb_v1530_l = dyn_array<sched_info_nb_v1530_s>;

// SystemInformationBlockType1-NB-v1610 ::= SEQUENCE
struct sib_type1_nb_v1610_s {
  struct cell_access_related_info_minus5_gc_r16_s_ {
    struct cell_barred_minus5_gc_r16_opts {
      enum options { barred, not_barred, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<cell_barred_minus5_gc_r16_opts> cell_barred_minus5_gc_r16_e_;

    // member variables
    bool                            cell_id_r16_present = false;
    plmn_id_list_minus5_gc_nb_r16_l plmn_id_list_r16;
    fixed_bitstring<24>             tac_minus5_gc_r16;
    fixed_bitstring<28>             cell_id_r16;
    cell_barred_minus5_gc_r16_e_    cell_barred_minus5_gc_r16;
  };

  // member variables
  bool                                      cell_access_related_info_minus5_gc_r16_present = false;
  bool                                      non_crit_ext_present                           = false;
  cell_access_related_info_minus5_gc_r16_s_ cell_access_related_info_minus5_gc_r16;
  sib_type1_nb_v1700_s                      non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-Config-NB-r15 ::= SEQUENCE
struct tdd_cfg_nb_r15_s {
  struct sf_assign_r15_opts {
    enum options { sa1, sa2, sa3, sa4, sa5, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<sf_assign_r15_opts> sf_assign_r15_e_;
  struct special_sf_patterns_r15_opts {
    enum options {
      ssp0,
      ssp1,
      ssp2,
      ssp3,
      ssp4,
      ssp5,
      ssp6,
      ssp7,
      ssp8,
      ssp9,
      ssp10,
      ssp10_crs_less_dw_pts,
      nulltype
    } value;

    const char* to_string() const;
  };
  typedef enumerated<special_sf_patterns_r15_opts> special_sf_patterns_r15_e_;

  // member variables
  sf_assign_r15_e_           sf_assign_r15;
  special_sf_patterns_r15_e_ special_sf_patterns_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UAC-BarringPerCatList-NB-r16 ::= SEQUENCE (SIZE (1..63)) OF UAC-BarringPerCat-NB-r16
using uac_barr_per_cat_list_nb_r16_l = dyn_array<uac_barr_per_cat_nb_r16_s>;

// UL-ReferenceSignalsNPUSCH-NB-r13 ::= SEQUENCE
struct ul_ref_sigs_npusch_nb_r13_s {
  bool    group_hop_enabled_r13   = false;
  uint8_t group_assign_npusch_r13 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WUS-Config-NB-r15 ::= SEQUENCE
struct wus_cfg_nb_r15_s {
  struct num_pos_r15_opts {
    enum options { n1, n2, n4, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<num_pos_r15_opts> num_pos_r15_e_;
  struct num_drx_cycles_relaxed_r15_opts {
    enum options { n1, n2, n4, n8, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<num_drx_cycles_relaxed_r15_opts> num_drx_cycles_relaxed_r15_e_;
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
  bool                           ext                                = false;
  bool                           num_pos_r15_present                = false;
  bool                           time_offset_e_drx_long_r15_present = false;
  wus_max_dur_factor_nb_r15_e    max_dur_factor_r15;
  num_pos_r15_e_                 num_pos_r15;
  num_drx_cycles_relaxed_r15_e_  num_drx_cycles_relaxed_r15;
  time_offset_drx_r15_e_         time_offset_drx_r15;
  time_offset_e_drx_short_r15_e_ time_offset_e_drx_short_r15;
  time_offset_e_drx_long_r15_e_  time_offset_e_drx_long_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// WUS-ConfigPerCarrier-NB-r15 ::= SEQUENCE
struct wus_cfg_per_carrier_nb_r15_s {
  wus_max_dur_factor_nb_r15_e max_dur_factor_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCCH-Config-NB-r13 ::= SEQUENCE
struct bcch_cfg_nb_r13_s {
  struct mod_period_coeff_r13_opts {
    enum options { n16, n32, n64, n128, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<mod_period_coeff_r13_opts> mod_period_coeff_r13_e_;

  // member variables
  mod_period_coeff_r13_e_ mod_period_coeff_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CBP-Config-NB-r17 ::= SEQUENCE
struct cbp_cfg_nb_r17_s {
  struct nb_r17_opts {
    enum options {
      four_t,
      two_t,
      one_t,
      half_t,
      quarter_t,
      one8th_t,
      one16th_t,
      one32nd_t,
      one64th_t,
      one128th_t,
      one256th_t,
      one512th_t,
      one1024th_t,
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
  typedef enumerated<nb_r17_opts> nb_r17_e_;
  struct ue_specific_drx_cycle_min_r17_opts {
    enum options { rf32, rf64, rf128, rf256, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<ue_specific_drx_cycle_min_r17_opts> ue_specific_drx_cycle_min_r17_e_;

  // member variables
  bool                             nb_r17_present                        = false;
  bool                             ue_specific_drx_cycle_min_r17_present = false;
  uint8_t                          nrsrp_min_r17                         = 0;
  nb_r17_e_                        nb_r17;
  ue_specific_drx_cycle_min_r17_e_ ue_specific_drx_cycle_min_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqEUTRA-NB-r16 ::= SEQUENCE
struct carrier_freq_eutra_nb_r16_s {
  bool     ext                 = false;
  bool     sib1_r16_present    = false;
  bool     sib1_br_r16_present = false;
  uint32_t carrier_freq_r16    = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqsGERAN-NB-r16 ::= SEQUENCE
struct carrier_freqs_geran_nb_r16_s {
  bool                  ext                    = false;
  bool                  ec_gsm_iot_r16_present = false;
  bool                  peo_r16_present        = false;
  carrier_freqs_geran_s carrier_freqs_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-ConfigCommon-NB-r14 ::= SEQUENCE
struct dl_cfg_common_nb_r14_s {
  bool                           ext                  = false;
  bool                           pcch_cfg_r14_present = false;
  dl_carrier_cfg_common_nb_r14_s dl_carrier_cfg_r14;
  pcch_cfg_nb_r14_s              pcch_cfg_r14;
  // ...
  // group 0
  copy_ptr<wus_cfg_per_carrier_nb_r15_s> wus_cfg_r15;
  // group 1
  copy_ptr<wus_cfg_per_carrier_nb_r15_s> gwus_cfg_r16;
  // group 2
  copy_ptr<pcch_cfg_nb_r17_s> pcch_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GWUS-Config-NB-r16 ::= SEQUENCE
struct gwus_cfg_nb_r16_s {
  struct common_seq_r16_opts {
    enum options { g0, g126, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<common_seq_r16_opts> common_seq_r16_e_;

  // member variables
  bool                           ext                             = false;
  bool                           group_alternation_r16_present   = false;
  bool                           common_seq_r16_present          = false;
  bool                           time_params_r16_present         = false;
  bool                           res_cfg_e_drx_short_r16_present = false;
  bool                           res_cfg_e_drx_long_r16_present  = false;
  bool                           prob_thresh_list_r16_present    = false;
  common_seq_r16_e_              common_seq_r16;
  wus_cfg_nb_r15_s               time_params_r16;
  gwus_res_cfg_nb_r16_s          res_cfg_drx_r16;
  gwus_res_cfg_nb_r16_s          res_cfg_e_drx_short_r16;
  gwus_res_cfg_nb_r16_s          res_cfg_e_drx_long_r16;
  gwus_prob_thresh_list_nb_r16_l prob_thresh_list_r16;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterFreqCarrierFreqInfo-NB-r13 ::= SEQUENCE
struct inter_freq_carrier_freq_info_nb_r13_s {
  struct pwr_class14dbm_offset_r14_opts {
    enum options { db_minus6, db_minus3, db3, db6, db9, db12, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<pwr_class14dbm_offset_r14_opts> pwr_class14dbm_offset_r14_e_;
  struct ce_authorisation_offset_r14_opts {
    enum options { db5, db10, db15, db20, db25, db30, db35, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ce_authorisation_offset_r14_opts> ce_authorisation_offset_r14_e_;

  // member variables
  bool                                   ext                                       = false;
  bool                                   q_qual_min_r13_present                    = false;
  bool                                   p_max_r13_present                         = false;
  bool                                   q_offset_freq_r13_present                 = false;
  bool                                   inter_freq_neigh_cell_list_r13_present    = false;
  bool                                   inter_freq_excluded_cell_list_r13_present = false;
  bool                                   multi_band_info_list_r13_present          = false;
  carrier_freq_nb_r13_s                  dl_carrier_freq_r13;
  int8_t                                 q_rx_lev_min_r13 = -70;
  int8_t                                 q_qual_min_r13   = -34;
  int8_t                                 p_max_r13        = -30;
  q_offset_range_e                       q_offset_freq_r13;
  inter_freq_neigh_cell_list_nb_r13_l    inter_freq_neigh_cell_list_r13;
  inter_freq_excluded_cell_list_nb_r13_l inter_freq_excluded_cell_list_r13;
  multi_band_info_list_nb_r13_l          multi_band_info_list_r13;
  // ...
  // group 0
  bool   delta_rx_lev_min_v1350_present = false;
  int8_t delta_rx_lev_min_v1350         = -8;
  // group 1
  bool                           pwr_class14dbm_offset_r14_present   = false;
  bool                           ce_authorisation_offset_r14_present = false;
  pwr_class14dbm_offset_r14_e_   pwr_class14dbm_offset_r14;
  ce_authorisation_offset_r14_e_ ce_authorisation_offset_r14;
  // group 2
  copy_ptr<nsss_rrm_cfg_nb_r15_s>                 nsss_rrm_cfg_r15;
  copy_ptr<inter_freq_neigh_cell_list_nb_v1530_l> inter_freq_neigh_cell_list_v1530;
  // group 3
  copy_ptr<carrier_freq_nb_v1550_s> dl_carrier_freq_v1550;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraFreqNeighCellInfo-NB-v1530 ::= SEQUENCE
struct intra_freq_neigh_cell_info_nb_v1530_s {
  bool                  nsss_rrm_cfg_r15_present = false;
  nsss_rrm_cfg_nb_r15_s nsss_rrm_cfg_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MBMS-SAI-InterFreq-NB-r14 ::= SEQUENCE
struct mbms_sai_inter_freq_nb_r14_s {
  bool                        multi_band_info_list_r14_present = false;
  carrier_freq_nb_r13_s       dl_carrier_freq_r14;
  mbms_sai_list_r11_l         mbms_sai_list_r14;
  add_band_info_list_nb_r14_l multi_band_info_list_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPDSCH-ConfigCommon-NB-r13 ::= SEQUENCE
struct npdsch_cfg_common_nb_r13_s {
  int8_t nrs_pwr_r13 = -60;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPRACH-ConfigSIB-NB-r13 ::= SEQUENCE
struct nprach_cfg_sib_nb_r13_s {
  struct nprach_cp_len_r13_opts {
    enum options { us66dot7, us266dot7, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<nprach_cp_len_r13_opts> nprach_cp_len_r13_e_;

  // member variables
  bool                                  rsrp_thress_prach_info_list_r13_present = false;
  nprach_cp_len_r13_e_                  nprach_cp_len_r13;
  rsrp_thress_nprach_info_list_nb_r13_l rsrp_thress_prach_info_list_r13;
  nprach_params_list_nb_r13_l           nprach_params_list_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPRACH-ConfigSIB-NB-v1330 ::= SEQUENCE
struct nprach_cfg_sib_nb_v1330_s {
  nprach_params_list_nb_v1330_l nprach_params_list_v1330;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPRACH-ConfigSIB-NB-v1450 ::= SEQUENCE
struct nprach_cfg_sib_nb_v1450_s {
  struct max_num_preamb_attempt_ce_r14_opts {
    enum options { n3, n4, n5, n6, n7, n8, n10, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_preamb_attempt_ce_r14_opts> max_num_preamb_attempt_ce_r14_e_;

  // member variables
  max_num_preamb_attempt_ce_r14_e_ max_num_preamb_attempt_ce_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPRACH-ConfigSIB-NB-v1530 ::= SEQUENCE
struct nprach_cfg_sib_nb_v1530_s {
  struct tdd_params_r15_s_ {
    struct nprach_preamb_format_r15_opts {
      enum options { fmt0, fmt1, fmt2, fmt0_a, fmt1_a, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<nprach_preamb_format_r15_opts> nprach_preamb_format_r15_e_;
    struct dummy_opts {
      enum options { n1, n2, n4, n8, n16, n32, n64, n128, n256, n512, n1024, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<dummy_opts> dummy_e_;

    // member variables
    nprach_preamb_format_r15_e_     nprach_preamb_format_r15;
    dummy_e_                        dummy;
    nprach_params_list_tdd_nb_r15_l nprach_params_list_tdd_r15;
  };
  struct fmt2_params_r15_s_ {
    bool                             nprach_params_list_fmt2_r15_present     = false;
    bool                             nprach_params_list_fmt2_edt_r15_present = false;
    nprach_params_list_fmt2_nb_r15_l nprach_params_list_fmt2_r15;
    nprach_params_list_fmt2_nb_r15_l nprach_params_list_fmt2_edt_r15;
  };
  struct edt_params_r15_s_ {
    bool                        edt_small_tbs_subset_r15_present   = false;
    bool                        nprach_params_list_edt_r15_present = false;
    edt_tbs_info_list_nb_r15_l  edt_tbs_info_list_r15;
    nprach_params_list_nb_r14_l nprach_params_list_edt_r15;
  };

  // member variables
  bool               tdd_params_r15_present  = false;
  bool               fmt2_params_r15_present = false;
  bool               edt_params_r15_present  = false;
  tdd_params_r15_s_  tdd_params_r15;
  fmt2_params_r15_s_ fmt2_params_r15;
  edt_params_r15_s_  edt_params_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPRACH-ConfigSIB-NB-v1550 ::= SEQUENCE
struct nprach_cfg_sib_nb_v1550_s {
  struct tdd_params_v1550_s_ {
    nprach_params_list_tdd_nb_v1550_l nprach_params_list_tdd_v1550;
  };

  // member variables
  tdd_params_v1550_s_ tdd_params_v1550;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPRACH-ProbabilityAnchor-NB-r14 ::= SEQUENCE
struct nprach_probability_anchor_nb_r14_s {
  struct nprach_probability_anchor_r14_opts {
    enum options {
      zero,
      one_sixteenth,
      one_fifteenth,
      one_fourteenth,
      one_thirteenth,
      one_twelfth,
      one_eleventh,
      one_tenth,
      one_ninth,
      one_eighth,
      one_seventh,
      one_sixth,
      one_fifth,
      one_fourth,
      one_third,
      one_half,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<nprach_probability_anchor_r14_opts> nprach_probability_anchor_r14_e_;

  // member variables
  bool                             nprach_probability_anchor_r14_present = false;
  nprach_probability_anchor_r14_e_ nprach_probability_anchor_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPRACH-TxDurationFmt01-NB-r17 ::= SEQUENCE
struct nprach_tx_dur_fmt01_nb_r17_s {
  struct nprach_tx_dur_fmt01_r17_opts {
    enum options { n2, n4, n8, n16, n32, n64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nprach_tx_dur_fmt01_r17_opts> nprach_tx_dur_fmt01_r17_e_;

  // member variables
  nprach_tx_dur_fmt01_r17_e_ nprach_tx_dur_fmt01_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPRACH-TxDurationFmt2-NB-r17 ::= SEQUENCE
struct nprach_tx_dur_fmt2_nb_r17_s {
  struct nprach_tx_dur_fmt2_r17_opts {
    enum options { n1, n2, n4, n8, n16, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<nprach_tx_dur_fmt2_r17_opts> nprach_tx_dur_fmt2_r17_e_;

  // member variables
  nprach_tx_dur_fmt2_r17_e_ nprach_tx_dur_fmt2_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPUSCH-ConfigCommon-NB-r13 ::= SEQUENCE
struct npusch_cfg_common_nb_r13_s {
  using ack_nack_num_repeats_msg4_r13_l_ = bounded_array<ack_nack_num_repeats_nb_r13_e, 3>;
  struct srs_sf_cfg_r13_opts {
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
  typedef enumerated<srs_sf_cfg_r13_opts> srs_sf_cfg_r13_e_;
  struct dmrs_cfg_r13_s_ {
    bool    three_tone_base_seq_r13_present  = false;
    bool    six_tone_base_seq_r13_present    = false;
    bool    twelve_tone_base_seq_r13_present = false;
    uint8_t three_tone_base_seq_r13          = 0;
    uint8_t three_tone_cyclic_shift_r13      = 0;
    uint8_t six_tone_base_seq_r13            = 0;
    uint8_t six_tone_cyclic_shift_r13        = 0;
    uint8_t twelve_tone_base_seq_r13         = 0;
  };

  // member variables
  bool                             srs_sf_cfg_r13_present = false;
  bool                             dmrs_cfg_r13_present   = false;
  ack_nack_num_repeats_msg4_r13_l_ ack_nack_num_repeats_msg4_r13;
  srs_sf_cfg_r13_e_                srs_sf_cfg_r13;
  dmrs_cfg_r13_s_                  dmrs_cfg_r13;
  ul_ref_sigs_npusch_nb_r13_s      ul_ref_sigs_npusch_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCCH-Config-NB-r13 ::= SEQUENCE
struct pcch_cfg_nb_r13_s {
  struct default_paging_cycle_r13_opts {
    enum options { rf128, rf256, rf512, rf1024, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<default_paging_cycle_r13_opts> default_paging_cycle_r13_e_;
  struct nb_r13_opts {
    enum options {
      four_t,
      two_t,
      one_t,
      half_t,
      quarter_t,
      one8th_t,
      one16th_t,
      one32nd_t,
      one64th_t,
      one128th_t,
      one256th_t,
      one512th_t,
      one1024th_t,
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
  typedef enumerated<nb_r13_opts> nb_r13_e_;
  struct npdcch_num_repeat_paging_r13_opts {
    enum options {
      r1,
      r2,
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
  typedef enumerated<npdcch_num_repeat_paging_r13_opts> npdcch_num_repeat_paging_r13_e_;

  // member variables
  default_paging_cycle_r13_e_     default_paging_cycle_r13;
  nb_r13_e_                       nb_r13;
  npdcch_num_repeat_paging_r13_e_ npdcch_num_repeat_paging_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RACH-ConfigCommon-NB-r13 ::= SEQUENCE
struct rach_cfg_common_nb_r13_s {
  bool                    ext                              = false;
  bool                    conn_est_fail_offset_r13_present = false;
  preamb_trans_max_e      preamb_trans_max_ce_r13;
  pwr_ramp_params_s       pwr_ramp_params_r13;
  rach_info_list_nb_r13_l rach_info_list_r13;
  uint8_t                 conn_est_fail_offset_r13 = 0;
  // ...
  // group 0
  copy_ptr<pwr_ramp_params_nb_v1450_s> pwr_ramp_params_v1450;
  // group 1
  copy_ptr<rach_info_list_nb_v1530_l> rach_info_list_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SIB-Type-NB-r13 ::= ENUMERATED
struct sib_type_nb_r13_opts {
  enum options {
    sib_type3_nb_r13,
    sib_type4_nb_r13,
    sib_type5_nb_r13,
    sib_type14_nb_r13,
    sib_type16_nb_r13,
    sib_type15_nb_r14,
    sib_type20_nb_r14,
    sib_type22_nb_r14,
    nulltype
  } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<sib_type_nb_r13_opts> sib_type_nb_r13_e;

// SystemInformationBlockType1-NB-v1530 ::= SEQUENCE
struct sib_type1_nb_v1530_s {
  struct tdd_params_r15_s_ {
    struct tdd_si_carrier_info_r15_opts {
      enum options { anchor, non_anchor, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<tdd_si_carrier_info_r15_opts> tdd_si_carrier_info_r15_e_;

    // member variables
    bool                       tdd_si_sfs_bitmap_r15_present = false;
    tdd_cfg_nb_r15_s           tdd_cfg_r15;
    tdd_si_carrier_info_r15_e_ tdd_si_carrier_info_r15;
    dl_bitmap_nb_r13_c         tdd_si_sfs_bitmap_r15;
  };

  // member variables
  bool                       tdd_params_r15_present        = false;
  bool                       sched_info_list_v1530_present = false;
  bool                       non_crit_ext_present          = false;
  tdd_params_r15_s_          tdd_params_r15;
  sched_info_list_nb_v1530_l sched_info_list_v1530;
  sib_type1_nb_v1610_s       non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UAC-Barring-NB-r16 ::= SEQUENCE
struct uac_barr_nb_r16_s {
  bool                             uac_barr_per_cat_list_r16_present      = false;
  bool                             uac_ac1_select_assist_info_r16_present = false;
  uac_barr_per_cat_list_nb_r16_l   uac_barr_per_cat_list_r16;
  uac_ac1_select_assist_info_r15_e uac_ac1_select_assist_info_r16;
  fixed_bitstring<7>               uac_barr_for_access_id_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-ConfigCommon-NB-r14 ::= SEQUENCE
struct ul_cfg_common_nb_r14_s {
  bool                        ext                            = false;
  bool                        nprach_params_list_r14_present = false;
  carrier_freq_nb_r13_s       ul_carrier_freq_r14;
  nprach_params_list_nb_r14_l nprach_params_list_r14;
  // ...
  // group 0
  copy_ptr<nprach_params_list_nb_r14_l> nprach_params_list_edt_r15;
  // group 1
  copy_ptr<rsrp_thress_nprach_info_list_nb_r13_l> rsrp_thress_prach_info_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-ConfigCommon-NB-v1530 ::= SEQUENCE
struct ul_cfg_common_nb_v1530_s {
  bool                             ext                                     = false;
  bool                             nprach_params_list_fmt2_r15_present     = false;
  bool                             nprach_params_list_fmt2_edt_r15_present = false;
  nprach_params_list_fmt2_nb_r15_l nprach_params_list_fmt2_r15;
  nprach_params_list_fmt2_nb_r15_l nprach_params_list_fmt2_edt_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-ConfigCommonTDD-NB-r15 ::= SEQUENCE
struct ul_cfg_common_tdd_nb_r15_s {
  bool                            ext                                = false;
  bool                            nprach_params_list_tdd_r15_present = false;
  tdd_ul_dl_align_offset_nb_r15_e tdd_ul_dl_align_offset_r15;
  nprach_params_list_tdd_nb_r15_l nprach_params_list_tdd_r15;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UplinkPowerControlCommon-NB-r13 ::= SEQUENCE
struct ul_pwr_ctrl_common_nb_r13_s {
  struct alpha_r13_opts {
    enum options { al0, al04, al05, al06, al07, al08, al09, al1, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<alpha_r13_opts> alpha_r13_e_;

  // member variables
  int8_t       p0_nominal_npusch_r13 = -126;
  alpha_r13_e_ alpha_r13;
  int8_t       delta_preamb_msg3_r13 = -1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CarrierFreqListEUTRA-NB-r16 ::= SEQUENCE (SIZE (1..8)) OF CarrierFreqEUTRA-NB-r16
using carrier_freq_list_eutra_nb_r16_l = dyn_array<carrier_freq_eutra_nb_r16_s>;

// CarrierFreqsListGERAN-NB-r16 ::= SEQUENCE (SIZE (1..8)) OF CarrierFreqsGERAN-NB-r16
using carrier_freqs_list_geran_nb_r16_l = dyn_array<carrier_freqs_geran_nb_r16_s>;

// CellReselectionInfoCommon-NB-v1450 ::= SEQUENCE
struct cell_resel_info_common_nb_v1450_s {
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

// CellSelectionInfo-NB-v1430 ::= SEQUENCE
struct cell_sel_info_nb_v1430_s {
  struct pwr_class14dbm_offset_r14_opts {
    enum options { db_minus6, db_minus3, db3, db6, db9, db12, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<pwr_class14dbm_offset_r14_opts> pwr_class14dbm_offset_r14_e_;
  struct ce_authorisation_offset_r14_opts {
    enum options { db5, db10, db15, db20, db25, db30, db35, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ce_authorisation_offset_r14_opts> ce_authorisation_offset_r14_e_;

  // member variables
  bool                           pwr_class14dbm_offset_r14_present   = false;
  bool                           ce_authorisation_offset_r14_present = false;
  pwr_class14dbm_offset_r14_e_   pwr_class14dbm_offset_r14;
  ce_authorisation_offset_r14_e_ ce_authorisation_offset_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ConnMeasConfig-NB-r17 ::= SEQUENCE
struct conn_meas_cfg_nb_r17_s {
  struct neigh_cell_meas_criteria_r17_s_ {
    struct s_measure_delta_p_r17_opts {
      enum options { db6, db9, db12, db15, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<s_measure_delta_p_r17_opts> s_measure_delta_p_r17_e_;
    struct t_measure_delta_p_r17_opts {
      enum options { s15, s30, s45, s60, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<t_measure_delta_p_r17_opts> t_measure_delta_p_r17_e_;

    // member variables
    s_measure_delta_p_r17_e_ s_measure_delta_p_r17;
    t_measure_delta_p_r17_e_ t_measure_delta_p_r17;
  };

  // member variables
  bool                            s_measure_inter_r17_present          = false;
  bool                            neigh_cell_meas_criteria_r17_present = false;
  uint8_t                         s_measure_intra_r17                  = 0;
  uint8_t                         s_measure_inter_r17                  = 0;
  neigh_cell_meas_criteria_r17_s_ neigh_cell_meas_criteria_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CoverageBasedPagingConfig-NB-r17 ::= SEQUENCE
struct coverage_based_paging_cfg_nb_r17_s {
  struct cbp_hyst_timer_r17_opts {
    enum options { ms2560, ms7680, ms12800, ms17920, ms23040, ms28160, ms33280, ms40960, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<cbp_hyst_timer_r17_opts> cbp_hyst_timer_r17_e_;
  using cbp_cfg_list_r17_l_ = dyn_array<cbp_cfg_nb_r17_s>;

  // member variables
  cbp_hyst_timer_r17_e_ cbp_hyst_timer_r17;
  cbp_cfg_list_r17_l_   cbp_cfg_list_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-ConfigCommonList-NB-r14 ::= SEQUENCE (SIZE (1..15)) OF DL-ConfigCommon-NB-r14
using dl_cfg_common_list_nb_r14_l = dyn_array<dl_cfg_common_nb_r14_s>;

// InterFreqCarrierFreqList-NB-r13 ::= SEQUENCE (SIZE (1..8)) OF InterFreqCarrierFreqInfo-NB-r13
using inter_freq_carrier_freq_list_nb_r13_l = dyn_array<inter_freq_carrier_freq_info_nb_r13_s>;

// IntraFreqCellReselectionInfo-NB-v1350 ::= SEQUENCE
struct intra_freq_cell_resel_info_nb_v1350_s {
  int8_t delta_rx_lev_min_v1350 = -8;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraFreqCellReselectionInfo-NB-v1360 ::= SEQUENCE
struct intra_freq_cell_resel_info_nb_v1360_s {
  uint8_t s_intra_search_p_v1360 = 32;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraFreqCellReselectionInfo-NB-v1430 ::= SEQUENCE
struct intra_freq_cell_resel_info_nb_v1430_s {
  struct pwr_class14dbm_offset_r14_opts {
    enum options { db_minus6, db_minus3, db3, db6, db9, db12, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<pwr_class14dbm_offset_r14_opts> pwr_class14dbm_offset_r14_e_;
  struct ce_authorisation_offset_r14_opts {
    enum options { db5, db10, db15, db20, db25, db30, db35, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ce_authorisation_offset_r14_opts> ce_authorisation_offset_r14_e_;

  // member variables
  bool                           pwr_class14dbm_offset_r14_present   = false;
  bool                           ce_authorisation_offset_r14_present = false;
  pwr_class14dbm_offset_r14_e_   pwr_class14dbm_offset_r14;
  ce_authorisation_offset_r14_e_ ce_authorisation_offset_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IntraFreqNeighCellList-NB-v1530 ::= SEQUENCE (SIZE (1..16)) OF IntraFreqNeighCellInfo-NB-v1530
using intra_freq_neigh_cell_list_nb_v1530_l = dyn_array<intra_freq_neigh_cell_info_nb_v1530_s>;

// MBMS-SAI-InterFreqList-NB-r14 ::= SEQUENCE (SIZE (1..8)) OF MBMS-SAI-InterFreq-NB-r14
using mbms_sai_inter_freq_list_nb_r14_l = dyn_array<mbms_sai_inter_freq_nb_r14_s>;

// NPDCCH-SC-MCCH-Config-NB-r14 ::= SEQUENCE
struct npdcch_sc_mcch_cfg_nb_r14_s {
  struct npdcch_num_repeats_sc_mcch_r14_opts {
    enum options { r1, r2, r4, r8, r16, r32, r64, r128, r256, r512, r1024, r2048, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<npdcch_num_repeats_sc_mcch_r14_opts> npdcch_num_repeats_sc_mcch_r14_e_;
  struct npdcch_start_sf_sc_mcch_r14_opts {
    enum options { v1dot5, v2, v4, v8, v16, v32, v48, v64, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<npdcch_start_sf_sc_mcch_r14_opts> npdcch_start_sf_sc_mcch_r14_e_;
  struct npdcch_offset_sc_mcch_r14_opts {
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
  typedef enumerated<npdcch_offset_sc_mcch_r14_opts> npdcch_offset_sc_mcch_r14_e_;

  // member variables
  npdcch_num_repeats_sc_mcch_r14_e_ npdcch_num_repeats_sc_mcch_r14;
  npdcch_start_sf_sc_mcch_r14_e_    npdcch_start_sf_sc_mcch_r14;
  npdcch_offset_sc_mcch_r14_e_      npdcch_offset_sc_mcch_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NPRACH-ProbabilityAnchorList-NB-r14 ::= SEQUENCE (SIZE (1..3)) OF NPRACH-ProbabilityAnchor-NB-r14
using nprach_probability_anchor_list_nb_r14_l = dyn_array<nprach_probability_anchor_nb_r14_s>;

// RadioResourceConfigCommonSIB-NB-r13 ::= SEQUENCE
struct rr_cfg_common_sib_nb_r13_s {
  struct ue_specific_drx_cycle_min_r16_opts {
    enum options { rf32, rf64, rf128, rf256, rf512, rf1024, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<ue_specific_drx_cycle_min_r16_opts> ue_specific_drx_cycle_min_r16_e_;
  struct ntn_cfg_common_r17_s_ {
    struct t318_r17_opts {
      enum options { ms0, ms200, ms500, ms1000, ms2000, ms4000, ms8000, nulltype } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<t318_r17_opts> t318_r17_e_;

    // member variables
    bool                         ta_report_r17_present           = false;
    bool                         nprach_tx_dur_fmt01_r17_present = false;
    bool                         nprach_tx_dur_fmt2_r17_present  = false;
    bool                         npusch_tx_dur_r17_present       = false;
    t318_r17_e_                  t318_r17;
    nprach_tx_dur_fmt01_nb_r17_s nprach_tx_dur_fmt01_r17;
    nprach_tx_dur_fmt2_nb_r17_s  nprach_tx_dur_fmt2_r17;
    npusch_tx_dur_nb_r17_s       npusch_tx_dur_r17;
  };

  // member variables
  bool                        ext                = false;
  bool                        dl_gap_r13_present = false;
  rach_cfg_common_nb_r13_s    rach_cfg_common_r13;
  bcch_cfg_nb_r13_s           bcch_cfg_r13;
  pcch_cfg_nb_r13_s           pcch_cfg_r13;
  nprach_cfg_sib_nb_r13_s     nprach_cfg_r13;
  npdsch_cfg_common_nb_r13_s  npdsch_cfg_common_r13;
  npusch_cfg_common_nb_r13_s  npusch_cfg_common_r13;
  dl_gap_cfg_nb_r13_s         dl_gap_r13;
  ul_pwr_ctrl_common_nb_r13_s ul_pwr_ctrl_common_r13;
  // ...
  // group 0
  copy_ptr<nprach_cfg_sib_nb_v1330_s> nprach_cfg_v1330;
  // group 1
  copy_ptr<nprach_cfg_sib_nb_v1450_s> nprach_cfg_v1450;
  // group 2
  copy_ptr<nprach_cfg_sib_nb_v1530_s> nprach_cfg_v1530;
  copy_ptr<dl_gap_cfg_nb_v1530_s>     dl_gap_v1530;
  copy_ptr<wus_cfg_nb_r15_s>          wus_cfg_r15;
  // group 3
  copy_ptr<nprach_cfg_sib_nb_v1550_s> nprach_cfg_v1550;
  // group 4
  bool                             nrs_non_anchor_cfg_r16_present        = false;
  bool                             ue_specific_drx_cycle_min_r16_present = false;
  copy_ptr<gwus_cfg_nb_r16_s>      gwus_cfg_r16;
  ue_specific_drx_cycle_min_r16_e_ ue_specific_drx_cycle_min_r16;
  // group 5
  copy_ptr<ntn_cfg_common_r17_s_> ntn_cfg_common_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SC-MCCH-SchedulingInfo-NB-r14 ::= SEQUENCE
struct sc_mcch_sched_info_nb_r14_s {
  struct on_dur_timer_scptm_r14_opts {
    enum options { pp1, pp2, pp3, pp4, pp8, pp16, pp32, spare, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<on_dur_timer_scptm_r14_opts> on_dur_timer_scptm_r14_e_;
  struct drx_inactivity_timer_scptm_r14_opts {
    enum options { pp0, pp1, pp2, pp3, pp4, pp8, pp16, pp32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
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

// SIB-MappingInfo-NB-r13 ::= SEQUENCE (SIZE (0..31)) OF SIB-Type-NB-r13
using sib_map_info_nb_r13_l = bounded_array<sib_type_nb_r13_e, 31>;

// SystemInformationBlockType1-NB-v1450 ::= SEQUENCE
struct sib_type1_nb_v1450_s {
  struct nrs_crs_pwr_offset_v1450_opts {
    enum options {
      db_minus6,
      db_minus4dot77,
      db_minus3,
      db_minus1dot77,
      db0,
      db1,
      db1dot23,
      db2,
      db3,
      db4,
      db4dot23,
      db5,
      db6,
      db7,
      db8,
      db9,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<nrs_crs_pwr_offset_v1450_opts> nrs_crs_pwr_offset_v1450_e_;

  // member variables
  bool                        nrs_crs_pwr_offset_v1450_present = false;
  bool                        non_crit_ext_present             = false;
  nrs_crs_pwr_offset_v1450_e_ nrs_crs_pwr_offset_v1450;
  sib_type1_nb_v1530_s        non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// T-Reselection-NB-r13 ::= ENUMERATED
struct t_resel_nb_r13_opts {
  enum options { s0, s3, s6, s9, s12, s15, s18, s21, nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<t_resel_nb_r13_opts> t_resel_nb_r13_e;

// UAC-Param-NB-r16 ::= CHOICE
struct uac_param_nb_r16_c {
  using uac_barr_per_plmn_list_l_ = dyn_array<uac_barr_nb_r16_s>;
  struct types_opts {
    enum options { uac_barr_common, uac_barr_per_plmn_list, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  uac_param_nb_r16_c() = default;
  uac_param_nb_r16_c(const uac_param_nb_r16_c& other);
  uac_param_nb_r16_c& operator=(const uac_param_nb_r16_c& other);
  ~uac_param_nb_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uac_barr_nb_r16_s& uac_barr_common()
  {
    assert_choice_type(types::uac_barr_common, type_, "UAC-Param-NB-r16");
    return c.get<uac_barr_nb_r16_s>();
  }
  uac_barr_per_plmn_list_l_& uac_barr_per_plmn_list()
  {
    assert_choice_type(types::uac_barr_per_plmn_list, type_, "UAC-Param-NB-r16");
    return c.get<uac_barr_per_plmn_list_l_>();
  }
  const uac_barr_nb_r16_s& uac_barr_common() const
  {
    assert_choice_type(types::uac_barr_common, type_, "UAC-Param-NB-r16");
    return c.get<uac_barr_nb_r16_s>();
  }
  const uac_barr_per_plmn_list_l_& uac_barr_per_plmn_list() const
  {
    assert_choice_type(types::uac_barr_per_plmn_list, type_, "UAC-Param-NB-r16");
    return c.get<uac_barr_per_plmn_list_l_>();
  }
  uac_barr_nb_r16_s&         set_uac_barr_common();
  uac_barr_per_plmn_list_l_& set_uac_barr_per_plmn_list();

private:
  types                                                         type_;
  choice_buffer_t<uac_barr_nb_r16_s, uac_barr_per_plmn_list_l_> c;

  void destroy_();
};

// UE-TimersAndConstants-NB-r13 ::= SEQUENCE
struct ue_timers_and_consts_nb_r13_s {
  struct t300_r13_opts {
    enum options { ms2500, ms4000, ms6000, ms10000, ms15000, ms25000, ms40000, ms60000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t300_r13_opts> t300_r13_e_;
  struct t301_r13_opts {
    enum options { ms2500, ms4000, ms6000, ms10000, ms15000, ms25000, ms40000, ms60000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t301_r13_opts> t301_r13_e_;
  struct t310_r13_opts {
    enum options { ms0, ms200, ms500, ms1000, ms2000, ms4000, ms8000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t310_r13_opts> t310_r13_e_;
  struct n310_r13_opts {
    enum options { n1, n2, n3, n4, n6, n8, n10, n20, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<n310_r13_opts> n310_r13_e_;
  struct t311_r13_opts {
    enum options { ms1000, ms3000, ms5000, ms10000, ms15000, ms20000, ms30000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<t311_r13_opts> t311_r13_e_;
  struct n311_r13_opts {
    enum options { n1, n2, n3, n4, n5, n6, n8, n10, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<n311_r13_opts> n311_r13_e_;
  struct t311_v1350_opts {
    enum options { ms40000, ms60000, ms90000, ms120000, nulltype } value;
    typedef uint32_t number_type;

    const char* to_string() const;
    uint32_t    to_number() const;
  };
  typedef enumerated<t311_v1350_opts> t311_v1350_e_;
  struct t300_v1530_opts {
    enum options { ms80000, ms100000, ms120000, nulltype } value;
    typedef uint32_t number_type;

    const char* to_string() const;
    uint32_t    to_number() const;
  };
  typedef enumerated<t300_v1530_opts> t300_v1530_e_;
  struct t301_v1530_opts {
    enum options { ms80000, ms100000, ms120000, nulltype } value;
    typedef uint32_t number_type;

    const char* to_string() const;
    uint32_t    to_number() const;
  };
  typedef enumerated<t301_v1530_opts> t301_v1530_e_;
  struct t311_v1530_opts {
    enum options { ms160000, ms200000, nulltype } value;
    typedef uint32_t number_type;

    const char* to_string() const;
    uint32_t    to_number() const;
  };
  typedef enumerated<t311_v1530_opts> t311_v1530_e_;
  struct t300_r15_opts {
    enum options { ms6000, ms10000, ms15000, ms25000, ms40000, ms60000, ms80000, ms120000, nulltype } value;
    typedef uint32_t number_type;

    const char* to_string() const;
    uint32_t    to_number() const;
  };
  typedef enumerated<t300_r15_opts> t300_r15_e_;

  // member variables
  bool        ext = false;
  t300_r13_e_ t300_r13;
  t301_r13_e_ t301_r13;
  t310_r13_e_ t310_r13;
  n310_r13_e_ n310_r13;
  t311_r13_e_ t311_r13;
  n311_r13_e_ n311_r13;
  // ...
  // group 0
  bool          t311_v1350_present = false;
  t311_v1350_e_ t311_v1350;
  // group 1
  bool          t300_v1530_present = false;
  bool          t301_v1530_present = false;
  bool          t311_v1530_present = false;
  bool          t300_r15_present   = false;
  t300_v1530_e_ t300_v1530;
  t301_v1530_e_ t301_v1530;
  t311_v1530_e_ t311_v1530;
  t300_r15_e_   t300_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-ConfigCommonList-NB-r14 ::= SEQUENCE (SIZE (1..15)) OF UL-ConfigCommon-NB-r14
using ul_cfg_common_list_nb_r14_l = dyn_array<ul_cfg_common_nb_r14_s>;

// UL-ConfigCommonList-NB-v1530 ::= SEQUENCE (SIZE (1..15)) OF UL-ConfigCommon-NB-v1530
using ul_cfg_common_list_nb_v1530_l = dyn_array<ul_cfg_common_nb_v1530_s>;

// UL-ConfigCommonListTDD-NB-r15 ::= SEQUENCE (SIZE (1..15)) OF UL-ConfigCommonTDD-NB-r15
using ul_cfg_common_list_tdd_nb_r15_l = dyn_array<ul_cfg_common_tdd_nb_r15_s>;

// CellSelectionInfo-NB-v1350 ::= SEQUENCE
struct cell_sel_info_nb_v1350_s {
  int8_t delta_rx_lev_min_v1350 = -8;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityInfo-NB-r13 ::= SEQUENCE
struct plmn_id_info_nb_r13_s {
  struct cell_reserved_for_oper_r13_opts {
    enum options { reserved, not_reserved, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<cell_reserved_for_oper_r13_opts> cell_reserved_for_oper_r13_e_;

  // member variables
  bool                          attach_without_pdn_connect_r13_present = false;
  plmn_id_s                     plmn_id_r13;
  cell_reserved_for_oper_r13_e_ cell_reserved_for_oper_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SchedulingInfo-NB-r13 ::= SEQUENCE
struct sched_info_nb_r13_s {
  struct si_periodicity_r13_opts {
    enum options { rf64, rf128, rf256, rf512, rf1024, rf2048, rf4096, spare, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<si_periodicity_r13_opts> si_periodicity_r13_e_;
  struct si_repeat_pattern_r13_opts {
    enum options { every2nd_rf, every4th_rf, every8th_rf, every16th_rf, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<si_repeat_pattern_r13_opts> si_repeat_pattern_r13_e_;
  struct si_tb_r13_opts {
    enum options { b56, b120, b208, b256, b328, b440, b552, b680, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<si_tb_r13_opts> si_tb_r13_e_;

  // member variables
  si_periodicity_r13_e_    si_periodicity_r13;
  si_repeat_pattern_r13_e_ si_repeat_pattern_r13;
  sib_map_info_nb_r13_l    sib_map_info_r13;
  si_tb_r13_e_             si_tb_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1-NB-v1430 ::= SEQUENCE
struct sib_type1_nb_v1430_s {
  bool                     cell_sel_info_v1430_present = false;
  bool                     non_crit_ext_present        = false;
  cell_sel_info_nb_v1430_s cell_sel_info_v1430;
  sib_type1_nb_v1450_s     non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType14-NB-r13 ::= SEQUENCE
struct sib_type14_nb_r13_s {
  struct ab_param_r13_c_ {
    using ab_per_plmn_list_r13_l_ = dyn_array<ab_cfg_plmn_nb_r13_s>;
    struct types_opts {
      enum options { ab_common_r13, ab_per_plmn_list_r13, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    ab_param_r13_c_() = default;
    ab_param_r13_c_(const ab_param_r13_c_& other);
    ab_param_r13_c_& operator=(const ab_param_r13_c_& other);
    ~ab_param_r13_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ab_cfg_nb_r13_s& ab_common_r13()
    {
      assert_choice_type(types::ab_common_r13, type_, "ab-Param-r13");
      return c.get<ab_cfg_nb_r13_s>();
    }
    ab_per_plmn_list_r13_l_& ab_per_plmn_list_r13()
    {
      assert_choice_type(types::ab_per_plmn_list_r13, type_, "ab-Param-r13");
      return c.get<ab_per_plmn_list_r13_l_>();
    }
    const ab_cfg_nb_r13_s& ab_common_r13() const
    {
      assert_choice_type(types::ab_common_r13, type_, "ab-Param-r13");
      return c.get<ab_cfg_nb_r13_s>();
    }
    const ab_per_plmn_list_r13_l_& ab_per_plmn_list_r13() const
    {
      assert_choice_type(types::ab_per_plmn_list_r13, type_, "ab-Param-r13");
      return c.get<ab_per_plmn_list_r13_l_>();
    }
    ab_cfg_nb_r13_s&         set_ab_common_r13();
    ab_per_plmn_list_r13_l_& set_ab_per_plmn_list_r13();

  private:
    types                                                     type_;
    choice_buffer_t<ab_cfg_nb_r13_s, ab_per_plmn_list_r13_l_> c;

    void destroy_();
  };
  struct ab_per_nrsrp_r15_opts {
    enum options { thresh1, thresh2, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<ab_per_nrsrp_r15_opts> ab_per_nrsrp_r15_e_;

  // member variables
  bool            ext                       = false;
  bool            ab_param_r13_present      = false;
  bool            late_non_crit_ext_present = false;
  ab_param_r13_c_ ab_param_r13;
  dyn_octstring   late_non_crit_ext;
  // ...
  // group 0
  bool                ab_per_nrsrp_r15_present = false;
  ab_per_nrsrp_r15_e_ ab_per_nrsrp_r15;
  // group 1
  copy_ptr<uac_param_nb_r16_c> uac_param_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType15-NB-r14 ::= SEQUENCE
struct sib_type15_nb_r14_s {
  bool                              ext                                  = false;
  bool                              mbms_sai_intra_freq_r14_present      = false;
  bool                              mbms_sai_inter_freq_list_r14_present = false;
  bool                              late_non_crit_ext_present            = false;
  mbms_sai_list_r11_l               mbms_sai_intra_freq_r14;
  mbms_sai_inter_freq_list_nb_r14_l mbms_sai_inter_freq_list_r14;
  dyn_octstring                     late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType2-NB-r13 ::= SEQUENCE
struct sib_type2_nb_r13_s {
  struct freq_info_r13_s_ {
    bool                  ul_carrier_freq_r13_present = false;
    carrier_freq_nb_r13_s ul_carrier_freq_r13;
    uint8_t               add_spec_emission_r13 = 1;
  };
  using multi_band_info_list_r13_l_ = bounded_array<uint8_t, 8>;
  struct freq_info_v1530_s_ {
    tdd_ul_dl_align_offset_nb_r15_e tdd_ul_dl_align_offset_r15;
  };

  // member variables
  bool                          ext                              = false;
  bool                          multi_band_info_list_r13_present = false;
  bool                          late_non_crit_ext_present        = false;
  rr_cfg_common_sib_nb_r13_s    rr_cfg_common_r13;
  ue_timers_and_consts_nb_r13_s ue_timers_and_consts_r13;
  freq_info_r13_s_              freq_info_r13;
  time_align_timer_e            time_align_timer_common_r13;
  multi_band_info_list_r13_l_   multi_band_info_list_r13;
  dyn_octstring                 late_non_crit_ext;
  // ...
  // group 0
  bool cp_reest_r14_present = false;
  // group 1
  bool serving_cell_meas_info_r14_present = false;
  bool cqi_report_r14_present             = false;
  // group 2
  bool                         enhanced_phr_r15_present = false;
  bool                         cp_edt_r15_present       = false;
  bool                         up_edt_r15_present       = false;
  copy_ptr<freq_info_v1530_s_> freq_info_v1530;
  // group 3
  bool early_security_reactivation_r16_present = false;
  bool cp_edt_minus5_gc_r16_present            = false;
  bool up_edt_minus5_gc_r16_present            = false;
  bool cp_pur_epc_r16_present                  = false;
  bool up_pur_epc_r16_present                  = false;
  bool cp_pur_minus5_gc_r16_present            = false;
  bool up_pur_minus5_gc_r16_present            = false;
  bool rai_activation_enh_r16_present          = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType20-NB-r14 ::= SEQUENCE
struct sib_type20_nb_r14_s {
  struct sc_mcch_carrier_cfg_r14_c_ {
    struct types_opts {
      enum options { dl_carrier_cfg_r14, dl_carrier_idx_r14, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    sc_mcch_carrier_cfg_r14_c_() = default;
    sc_mcch_carrier_cfg_r14_c_(const sc_mcch_carrier_cfg_r14_c_& other);
    sc_mcch_carrier_cfg_r14_c_& operator=(const sc_mcch_carrier_cfg_r14_c_& other);
    ~sc_mcch_carrier_cfg_r14_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dl_carrier_cfg_common_nb_r14_s& dl_carrier_cfg_r14()
    {
      assert_choice_type(types::dl_carrier_cfg_r14, type_, "sc-mcch-CarrierConfig-r14");
      return c.get<dl_carrier_cfg_common_nb_r14_s>();
    }
    uint8_t& dl_carrier_idx_r14()
    {
      assert_choice_type(types::dl_carrier_idx_r14, type_, "sc-mcch-CarrierConfig-r14");
      return c.get<uint8_t>();
    }
    const dl_carrier_cfg_common_nb_r14_s& dl_carrier_cfg_r14() const
    {
      assert_choice_type(types::dl_carrier_cfg_r14, type_, "sc-mcch-CarrierConfig-r14");
      return c.get<dl_carrier_cfg_common_nb_r14_s>();
    }
    const uint8_t& dl_carrier_idx_r14() const
    {
      assert_choice_type(types::dl_carrier_idx_r14, type_, "sc-mcch-CarrierConfig-r14");
      return c.get<uint8_t>();
    }
    dl_carrier_cfg_common_nb_r14_s& set_dl_carrier_cfg_r14();
    uint8_t&                        set_dl_carrier_idx_r14();

  private:
    types                                           type_;
    choice_buffer_t<dl_carrier_cfg_common_nb_r14_s> c;

    void destroy_();
  };
  struct sc_mcch_repeat_period_r14_opts {
    enum options { rf32, rf128, rf512, rf1024, rf2048, rf4096, rf8192, rf16384, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<sc_mcch_repeat_period_r14_opts> sc_mcch_repeat_period_r14_e_;
  struct sc_mcch_mod_period_r14_opts {
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
      spare1,
      nulltype
    } value;
    typedef uint32_t number_type;

    const char* to_string() const;
    uint32_t    to_number() const;
  };
  typedef enumerated<sc_mcch_mod_period_r14_opts> sc_mcch_mod_period_r14_e_;

  // member variables
  bool                         ext                            = false;
  bool                         sc_mcch_sched_info_r14_present = false;
  bool                         late_non_crit_ext_present      = false;
  npdcch_sc_mcch_cfg_nb_r14_s  npdcch_sc_mcch_cfg_r14;
  sc_mcch_carrier_cfg_r14_c_   sc_mcch_carrier_cfg_r14;
  sc_mcch_repeat_period_r14_e_ sc_mcch_repeat_period_r14;
  uint8_t                      sc_mcch_offset_r14 = 0;
  sc_mcch_mod_period_r14_e_    sc_mcch_mod_period_r14;
  sc_mcch_sched_info_nb_r14_s  sc_mcch_sched_info_r14;
  dyn_octstring                late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType22-NB-r14 ::= SEQUENCE
struct sib_type22_nb_r14_s {
  struct mixed_operation_mode_cfg_r15_s_ {
    bool                        dl_cfg_list_mixed_r15_present   = false;
    bool                        ul_cfg_list_mixed_r15_present   = false;
    bool                        paging_distribution_r15_present = false;
    bool                        nprach_distribution_r15_present = false;
    dl_cfg_common_list_nb_r14_l dl_cfg_list_mixed_r15;
    ul_cfg_common_list_nb_r14_l ul_cfg_list_mixed_r15;
  };

  // member variables
  bool                                    ext                                        = false;
  bool                                    dl_cfg_list_r14_present                    = false;
  bool                                    ul_cfg_list_r14_present                    = false;
  bool                                    paging_weight_anchor_r14_present           = false;
  bool                                    nprach_probability_anchor_list_r14_present = false;
  bool                                    late_non_crit_ext_present                  = false;
  dl_cfg_common_list_nb_r14_l             dl_cfg_list_r14;
  ul_cfg_common_list_nb_r14_l             ul_cfg_list_r14;
  paging_weight_nb_r14_e                  paging_weight_anchor_r14;
  nprach_probability_anchor_list_nb_r14_l nprach_probability_anchor_list_r14;
  dyn_octstring                           late_non_crit_ext;
  // ...
  // group 0
  copy_ptr<mixed_operation_mode_cfg_r15_s_> mixed_operation_mode_cfg_r15;
  copy_ptr<ul_cfg_common_list_tdd_nb_r15_l> ul_cfg_list_r15;
  // group 1
  copy_ptr<coverage_based_paging_cfg_nb_r17_s> coverage_based_paging_cfg_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType23-NB-r15 ::= SEQUENCE
struct sib_type23_nb_r15_s {
  bool                          ext                             = false;
  bool                          ul_cfg_list_v1530_present       = false;
  bool                          ul_cfg_list_mixed_v1530_present = false;
  bool                          late_non_crit_ext_present       = false;
  ul_cfg_common_list_nb_v1530_l ul_cfg_list_v1530;
  ul_cfg_common_list_nb_v1530_l ul_cfg_list_mixed_v1530;
  dyn_octstring                 late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType27-NB-r16 ::= SEQUENCE
struct sib_type27_nb_r16_s {
  bool                              ext                                  = false;
  bool                              carrier_freq_list_eutra_r16_present  = false;
  bool                              carrier_freqs_list_geran_r16_present = false;
  bool                              late_non_crit_ext_present            = false;
  carrier_freq_list_eutra_nb_r16_l  carrier_freq_list_eutra_r16;
  carrier_freqs_list_geran_nb_r16_l carrier_freqs_list_geran_r16;
  dyn_octstring                     late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType3-NB-r13 ::= SEQUENCE
struct sib_type3_nb_r13_s {
  struct cell_resel_info_common_r13_s_ {
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
  };
  struct cell_resel_serving_freq_info_r13_s_ {
    uint8_t s_non_intra_search_r13 = 0;
  };
  struct intra_freq_cell_resel_info_r13_s_ {
    bool             q_qual_min_r13_present = false;
    bool             p_max_r13_present      = false;
    int8_t           q_rx_lev_min_r13       = -70;
    int8_t           q_qual_min_r13         = -34;
    int8_t           p_max_r13              = -30;
    uint8_t          s_intra_search_p_r13   = 0;
    t_resel_nb_r13_e t_resel_r13;
  };
  using multi_band_info_list_r13_l_ = dyn_array<ns_pmax_list_nb_r13_l>;

  // member variables
  bool                                ext                              = false;
  bool                                freq_band_info_r13_present       = false;
  bool                                multi_band_info_list_r13_present = false;
  bool                                late_non_crit_ext_present        = false;
  cell_resel_info_common_r13_s_       cell_resel_info_common_r13;
  cell_resel_serving_freq_info_r13_s_ cell_resel_serving_freq_info_r13;
  intra_freq_cell_resel_info_r13_s_   intra_freq_cell_resel_info_r13;
  ns_pmax_list_nb_r13_l               freq_band_info_r13;
  multi_band_info_list_r13_l_         multi_band_info_list_r13;
  dyn_octstring                       late_non_crit_ext;
  // ...
  // group 0
  copy_ptr<intra_freq_cell_resel_info_nb_v1350_s> intra_freq_cell_resel_info_v1350;
  // group 1
  copy_ptr<intra_freq_cell_resel_info_nb_v1360_s> intra_freq_cell_resel_info_v1360;
  // group 2
  copy_ptr<intra_freq_cell_resel_info_nb_v1430_s> intra_freq_cell_resel_info_v1430;
  // group 3
  copy_ptr<cell_resel_info_common_nb_v1450_s> cell_resel_info_common_v1450;
  // group 4
  bool                            npbch_rrm_cfg_r15_present = false;
  copy_ptr<nsss_rrm_cfg_nb_r15_s> nsss_rrm_cfg_r15;
  // group 5
  bool                             t_service_r17_present = false;
  copy_ptr<conn_meas_cfg_nb_r17_s> conn_meas_cfg_r17;
  uint32_t                         t_service_r17 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType31-NB-r17 ::= SEQUENCE
struct sib_type31_nb_r17_s {
  bool                         ext                       = false;
  bool                         late_non_crit_ext_present = false;
  serving_satellite_info_r17_s serving_satellite_info_r17;
  dyn_octstring                late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType32-NB-r17 ::= SEQUENCE
struct sib_type32_nb_r17_s {
  bool                      ext                             = false;
  bool                      satellite_info_list_r17_present = false;
  bool                      late_non_crit_ext_present       = false;
  satellite_info_list_r17_l satellite_info_list_r17;
  dyn_octstring             late_non_crit_ext;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType4-NB-r13 ::= SEQUENCE
struct sib_type4_nb_r13_s {
  bool                            ext                                       = false;
  bool                            intra_freq_neigh_cell_list_r13_present    = false;
  bool                            intra_freq_excluded_cell_list_r13_present = false;
  bool                            late_non_crit_ext_present                 = false;
  intra_freq_neigh_cell_list_l    intra_freq_neigh_cell_list_r13;
  intra_freq_excluded_cell_list_l intra_freq_excluded_cell_list_r13;
  dyn_octstring                   late_non_crit_ext;
  // ...
  // group 0
  copy_ptr<nsss_rrm_cfg_nb_r15_s>                 nsss_rrm_cfg_r15;
  copy_ptr<intra_freq_neigh_cell_list_nb_v1530_l> intra_freq_neigh_cell_list_v1530;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType5-NB-r13 ::= SEQUENCE
struct sib_type5_nb_r13_s {
  bool                                  ext                       = false;
  bool                                  late_non_crit_ext_present = false;
  inter_freq_carrier_freq_list_nb_r13_l inter_freq_carrier_freq_list_r13;
  t_resel_nb_r13_e                      t_resel_r13;
  dyn_octstring                         late_non_crit_ext;
  // ...
  // group 0
  bool    scptm_freq_offset_r14_present = false;
  uint8_t scptm_freq_offset_r14         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PLMN-IdentityList-NB-r13 ::= SEQUENCE (SIZE (1..6)) OF PLMN-IdentityInfo-NB-r13
using plmn_id_list_nb_r13_l = dyn_array<plmn_id_info_nb_r13_s>;

// SchedulingInfoList-NB-r13 ::= SEQUENCE (SIZE (1..8)) OF SchedulingInfo-NB-r13
using sched_info_list_nb_r13_l = dyn_array<sched_info_nb_r13_s>;

// SystemInfoValueTagList-NB-r13 ::= SEQUENCE (SIZE (1..8)) OF INTEGER (0..3)
using sys_info_value_tag_list_nb_r13_l = bounded_array<uint8_t, 8>;

// SystemInformation-NB-r13-IEs ::= SEQUENCE
struct sys_info_nb_r13_ies_s {
  struct sib_type_and_info_r13_item_c_ {
    struct types_opts {
      enum options {
        sib2_r13,
        sib3_r13,
        sib4_r13,
        sib5_r13,
        sib14_r13,
        sib16_r13,
        // ...
        sib15_v1430,
        sib20_v1430,
        sib22_v1430,
        sib23_v1530,
        sib27_v1610,
        sib31_v1700,
        sib32_v1700,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts, true, 7> types;

    // choice methods
    sib_type_and_info_r13_item_c_() = default;
    sib_type_and_info_r13_item_c_(const sib_type_and_info_r13_item_c_& other);
    sib_type_and_info_r13_item_c_& operator=(const sib_type_and_info_r13_item_c_& other);
    ~sib_type_and_info_r13_item_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    sib_type2_nb_r13_s& sib2_r13()
    {
      assert_choice_type(types::sib2_r13, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type2_nb_r13_s>();
    }
    sib_type3_nb_r13_s& sib3_r13()
    {
      assert_choice_type(types::sib3_r13, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type3_nb_r13_s>();
    }
    sib_type4_nb_r13_s& sib4_r13()
    {
      assert_choice_type(types::sib4_r13, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type4_nb_r13_s>();
    }
    sib_type5_nb_r13_s& sib5_r13()
    {
      assert_choice_type(types::sib5_r13, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type5_nb_r13_s>();
    }
    sib_type14_nb_r13_s& sib14_r13()
    {
      assert_choice_type(types::sib14_r13, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type14_nb_r13_s>();
    }
    sib_type16_nb_r13_s& sib16_r13()
    {
      assert_choice_type(types::sib16_r13, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type16_nb_r13_s>();
    }
    sib_type15_nb_r14_s& sib15_v1430()
    {
      assert_choice_type(types::sib15_v1430, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type15_nb_r14_s>();
    }
    sib_type20_nb_r14_s& sib20_v1430()
    {
      assert_choice_type(types::sib20_v1430, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type20_nb_r14_s>();
    }
    sib_type22_nb_r14_s& sib22_v1430()
    {
      assert_choice_type(types::sib22_v1430, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type22_nb_r14_s>();
    }
    sib_type23_nb_r15_s& sib23_v1530()
    {
      assert_choice_type(types::sib23_v1530, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type23_nb_r15_s>();
    }
    sib_type27_nb_r16_s& sib27_v1610()
    {
      assert_choice_type(types::sib27_v1610, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type27_nb_r16_s>();
    }
    sib_type31_nb_r17_s& sib31_v1700()
    {
      assert_choice_type(types::sib31_v1700, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type31_nb_r17_s>();
    }
    sib_type32_nb_r17_s& sib32_v1700()
    {
      assert_choice_type(types::sib32_v1700, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type32_nb_r17_s>();
    }
    const sib_type2_nb_r13_s& sib2_r13() const
    {
      assert_choice_type(types::sib2_r13, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type2_nb_r13_s>();
    }
    const sib_type3_nb_r13_s& sib3_r13() const
    {
      assert_choice_type(types::sib3_r13, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type3_nb_r13_s>();
    }
    const sib_type4_nb_r13_s& sib4_r13() const
    {
      assert_choice_type(types::sib4_r13, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type4_nb_r13_s>();
    }
    const sib_type5_nb_r13_s& sib5_r13() const
    {
      assert_choice_type(types::sib5_r13, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type5_nb_r13_s>();
    }
    const sib_type14_nb_r13_s& sib14_r13() const
    {
      assert_choice_type(types::sib14_r13, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type14_nb_r13_s>();
    }
    const sib_type16_nb_r13_s& sib16_r13() const
    {
      assert_choice_type(types::sib16_r13, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type16_nb_r13_s>();
    }
    const sib_type15_nb_r14_s& sib15_v1430() const
    {
      assert_choice_type(types::sib15_v1430, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type15_nb_r14_s>();
    }
    const sib_type20_nb_r14_s& sib20_v1430() const
    {
      assert_choice_type(types::sib20_v1430, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type20_nb_r14_s>();
    }
    const sib_type22_nb_r14_s& sib22_v1430() const
    {
      assert_choice_type(types::sib22_v1430, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type22_nb_r14_s>();
    }
    const sib_type23_nb_r15_s& sib23_v1530() const
    {
      assert_choice_type(types::sib23_v1530, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type23_nb_r15_s>();
    }
    const sib_type27_nb_r16_s& sib27_v1610() const
    {
      assert_choice_type(types::sib27_v1610, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type27_nb_r16_s>();
    }
    const sib_type31_nb_r17_s& sib31_v1700() const
    {
      assert_choice_type(types::sib31_v1700, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type31_nb_r17_s>();
    }
    const sib_type32_nb_r17_s& sib32_v1700() const
    {
      assert_choice_type(types::sib32_v1700, type_, "sib-TypeAndInfo-r13-item");
      return c.get<sib_type32_nb_r17_s>();
    }
    sib_type2_nb_r13_s&  set_sib2_r13();
    sib_type3_nb_r13_s&  set_sib3_r13();
    sib_type4_nb_r13_s&  set_sib4_r13();
    sib_type5_nb_r13_s&  set_sib5_r13();
    sib_type14_nb_r13_s& set_sib14_r13();
    sib_type16_nb_r13_s& set_sib16_r13();
    sib_type15_nb_r14_s& set_sib15_v1430();
    sib_type20_nb_r14_s& set_sib20_v1430();
    sib_type22_nb_r14_s& set_sib22_v1430();
    sib_type23_nb_r15_s& set_sib23_v1530();
    sib_type27_nb_r16_s& set_sib27_v1610();
    sib_type31_nb_r17_s& set_sib31_v1700();
    sib_type32_nb_r17_s& set_sib32_v1700();

  private:
    types type_;
    choice_buffer_t<sib_type14_nb_r13_s,
                    sib_type15_nb_r14_s,
                    sib_type16_nb_r13_s,
                    sib_type20_nb_r14_s,
                    sib_type22_nb_r14_s,
                    sib_type23_nb_r15_s,
                    sib_type27_nb_r16_s,
                    sib_type2_nb_r13_s,
                    sib_type31_nb_r17_s,
                    sib_type32_nb_r17_s,
                    sib_type3_nb_r13_s,
                    sib_type4_nb_r13_s,
                    sib_type5_nb_r13_s>
        c;

    void destroy_();
  };
  using sib_type_and_info_r13_l_ = dyn_array<sib_type_and_info_r13_item_c_>;

  // member variables
  bool                     late_non_crit_ext_present = false;
  bool                     non_crit_ext_present      = false;
  sib_type_and_info_r13_l_ sib_type_and_info_r13;
  dyn_octstring            late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1-NB-v1350 ::= SEQUENCE
struct sib_type1_nb_v1350_s {
  bool                     cell_sel_info_v1350_present = false;
  bool                     non_crit_ext_present        = false;
  cell_sel_info_nb_v1350_s cell_sel_info_v1350;
  sib_type1_nb_v1430_s     non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformation-NB ::= SEQUENCE
struct sys_info_nb_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { sys_info_r13, crit_exts_future, nulltype } value;

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
    sys_info_nb_r13_ies_s& sys_info_r13()
    {
      assert_choice_type(types::sys_info_r13, type_, "criticalExtensions");
      return c;
    }
    const sys_info_nb_r13_ies_s& sys_info_r13() const
    {
      assert_choice_type(types::sys_info_r13, type_, "criticalExtensions");
      return c;
    }
    sys_info_nb_r13_ies_s& set_sys_info_r13();
    void                   set_crit_exts_future();

  private:
    types                 type_;
    sys_info_nb_r13_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SystemInformationBlockType1-NB ::= SEQUENCE
struct sib_type1_nb_s {
  struct cell_access_related_info_r13_s_ {
    struct cell_barred_r13_opts {
      enum options { barred, not_barred, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<cell_barred_r13_opts> cell_barred_r13_e_;
    struct intra_freq_resel_r13_opts {
      enum options { allowed, not_allowed, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<intra_freq_resel_r13_opts> intra_freq_resel_r13_e_;

    // member variables
    plmn_id_list_nb_r13_l   plmn_id_list_r13;
    fixed_bitstring<16>     tac_r13;
    fixed_bitstring<28>     cell_id_r13;
    cell_barred_r13_e_      cell_barred_r13;
    intra_freq_resel_r13_e_ intra_freq_resel_r13;
  };
  struct cell_sel_info_r13_s_ {
    int8_t q_rx_lev_min_r13 = -70;
    int8_t q_qual_min_r13   = -34;
  };
  struct eutra_ctrl_region_size_r13_opts {
    enum options { n1, n2, n3, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<eutra_ctrl_region_size_r13_opts> eutra_ctrl_region_size_r13_e_;
  struct nrs_crs_pwr_offset_r13_opts {
    enum options {
      db_minus6,
      db_minus4dot77,
      db_minus3,
      db_minus1dot77,
      db0,
      db1,
      db1dot23,
      db2,
      db3,
      db4,
      db4dot23,
      db5,
      db6,
      db7,
      db8,
      db9,
      nulltype
    } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<nrs_crs_pwr_offset_r13_opts> nrs_crs_pwr_offset_r13_e_;
  struct si_win_len_r13_opts {
    enum options { ms160, ms320, ms480, ms640, ms960, ms1280, ms1600, spare1, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<si_win_len_r13_opts> si_win_len_r13_e_;

  // member variables
  bool                             p_max_r13_present                   = false;
  bool                             freq_band_info_r13_present          = false;
  bool                             multi_band_info_list_r13_present    = false;
  bool                             dl_bitmap_r13_present               = false;
  bool                             eutra_ctrl_region_size_r13_present  = false;
  bool                             nrs_crs_pwr_offset_r13_present      = false;
  bool                             si_radio_frame_offset_r13_present   = false;
  bool                             sys_info_value_tag_list_r13_present = false;
  bool                             late_non_crit_ext_present           = false;
  bool                             non_crit_ext_present                = false;
  fixed_bitstring<8>               hyper_sfn_msb_r13;
  cell_access_related_info_r13_s_  cell_access_related_info_r13;
  cell_sel_info_r13_s_             cell_sel_info_r13;
  int8_t                           p_max_r13         = -30;
  uint16_t                         freq_band_ind_r13 = 1;
  ns_pmax_list_nb_r13_l            freq_band_info_r13;
  multi_band_info_list_nb_r13_l    multi_band_info_list_r13;
  dl_bitmap_nb_r13_c               dl_bitmap_r13;
  eutra_ctrl_region_size_r13_e_    eutra_ctrl_region_size_r13;
  nrs_crs_pwr_offset_r13_e_        nrs_crs_pwr_offset_r13;
  sched_info_list_nb_r13_l         sched_info_list_r13;
  si_win_len_r13_e_                si_win_len_r13;
  uint8_t                          si_radio_frame_offset_r13 = 1;
  sys_info_value_tag_list_nb_r13_l sys_info_value_tag_list_r13;
  dyn_octstring                    late_non_crit_ext;
  sib_type1_nb_v1350_s             non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BCCH-DL-SCH-MessageType-NB ::= CHOICE
struct bcch_dl_sch_msg_type_nb_c {
  struct c1_c_ {
    struct types_opts {
      enum options { sys_info_r13, sib_type1_r13, nulltype } value;
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
    sys_info_nb_s& sys_info_r13()
    {
      assert_choice_type(types::sys_info_r13, type_, "c1");
      return c.get<sys_info_nb_s>();
    }
    sib_type1_nb_s& sib_type1_r13()
    {
      assert_choice_type(types::sib_type1_r13, type_, "c1");
      return c.get<sib_type1_nb_s>();
    }
    const sys_info_nb_s& sys_info_r13() const
    {
      assert_choice_type(types::sys_info_r13, type_, "c1");
      return c.get<sys_info_nb_s>();
    }
    const sib_type1_nb_s& sib_type1_r13() const
    {
      assert_choice_type(types::sib_type1_r13, type_, "c1");
      return c.get<sib_type1_nb_s>();
    }
    sys_info_nb_s&  set_sys_info_r13();
    sib_type1_nb_s& set_sib_type1_r13();

  private:
    types                                          type_;
    choice_buffer_t<sib_type1_nb_s, sys_info_nb_s> c;

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
  bcch_dl_sch_msg_type_nb_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "BCCH-DL-SCH-MessageType-NB");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "BCCH-DL-SCH-MessageType-NB");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// BCCH-DL-SCH-Message-NB ::= SEQUENCE
struct bcch_dl_sch_msg_nb_s {
  bcch_dl_sch_msg_type_nb_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCEarlyDataComplete-NB-v1700-IEs ::= SEQUENCE
struct rrc_early_data_complete_nb_v1700_ies_s {
  bool    cbp_idx_r17_present  = false;
  bool    non_crit_ext_present = false;
  uint8_t cbp_idx_r17          = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishment-NB-v1430-IEs ::= SEQUENCE
struct rrc_conn_reest_nb_v1430_ies_s {
  bool                dl_nas_mac_present   = false;
  bool                non_crit_ext_present = false;
  fixed_bitstring<16> dl_nas_mac;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetup-NB-v1610-IEs ::= SEQUENCE
struct rrc_conn_setup_nb_v1610_ies_s {
  bool          ded_info_nas_r16_present = false;
  bool          non_crit_ext_present     = false;
  dyn_octstring ded_info_nas_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCEarlyDataComplete-NB-v1590-IEs ::= SEQUENCE
struct rrc_early_data_complete_nb_v1590_ies_s {
  bool                                   late_non_crit_ext_present = false;
  bool                                   non_crit_ext_present      = false;
  dyn_octstring                          late_non_crit_ext;
  rrc_early_data_complete_nb_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RedirectedCarrierInfo-NB-r13 ::= CarrierFreq-NB-r13
using redirected_carrier_info_nb_r13_s = carrier_freq_nb_r13_s;

// RedirectedCarrierInfo-NB-v1430 ::= SEQUENCE
struct redirected_carrier_info_nb_v1430_s {
  struct redirected_carrier_offset_ded_r14_opts {
    enum options {
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
      db26,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<redirected_carrier_offset_ded_r14_opts> redirected_carrier_offset_ded_r14_e_;
  struct t322_r14_opts {
    enum options { min5, min10, min20, min30, min60, min120, min180, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<t322_r14_opts> t322_r14_e_;

  // member variables
  redirected_carrier_offset_ded_r14_e_ redirected_carrier_offset_ded_r14;
  t322_r14_e_                          t322_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishment-NB-r13-IEs ::= SEQUENCE
struct rrc_conn_reest_nb_r13_ies_s {
  bool                          late_non_crit_ext_present = false;
  bool                          non_crit_ext_present      = false;
  rr_cfg_ded_nb_r13_s           rr_cfg_ded_r13;
  uint8_t                       next_hop_chaining_count_r13 = 0;
  dyn_octstring                 late_non_crit_ext;
  rrc_conn_reest_nb_v1430_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReject-NB-r13-IEs ::= SEQUENCE
struct rrc_conn_reject_nb_r13_ies_s {
  bool          rrc_suspend_ind_r13_present = false;
  bool          late_non_crit_ext_present   = false;
  bool          non_crit_ext_present        = false;
  uint16_t      extended_wait_time_r13      = 1;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetup-NB-r13-IEs ::= SEQUENCE
struct rrc_conn_setup_nb_r13_ies_s {
  bool                          late_non_crit_ext_present = false;
  bool                          non_crit_ext_present      = false;
  rr_cfg_ded_nb_r13_s           rr_cfg_ded_r13;
  dyn_octstring                 late_non_crit_ext;
  rrc_conn_setup_nb_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCEarlyDataComplete-NB-r15-IEs ::= SEQUENCE
struct rrc_early_data_complete_nb_r15_ies_s {
  bool                                   ded_info_nas_r15_present                = false;
  bool                                   extended_wait_time_r15_present          = false;
  bool                                   redirected_carrier_info_r15_present     = false;
  bool                                   redirected_carrier_info_ext_r15_present = false;
  bool                                   non_crit_ext_present                    = false;
  dyn_octstring                          ded_info_nas_r15;
  uint16_t                               extended_wait_time_r15 = 1;
  redirected_carrier_info_nb_r13_s       redirected_carrier_info_r15;
  redirected_carrier_info_nb_v1430_s     redirected_carrier_info_ext_r15;
  rrc_early_data_complete_nb_v1590_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishment-NB ::= SEQUENCE
struct rrc_conn_reest_nb_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { rrc_conn_reest_r13, spare1, nulltype } value;

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
      rrc_conn_reest_nb_r13_ies_s& rrc_conn_reest_r13()
      {
        assert_choice_type(types::rrc_conn_reest_r13, type_, "c1");
        return c;
      }
      const rrc_conn_reest_nb_r13_ies_s& rrc_conn_reest_r13() const
      {
        assert_choice_type(types::rrc_conn_reest_r13, type_, "c1");
        return c;
      }
      rrc_conn_reest_nb_r13_ies_s& set_rrc_conn_reest_r13();
      void                         set_spare1();

    private:
      types                       type_;
      rrc_conn_reest_nb_r13_ies_s c;
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

// RRCConnectionReject-NB ::= SEQUENCE
struct rrc_conn_reject_nb_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { rrc_conn_reject_r13, spare1, nulltype } value;

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
      rrc_conn_reject_nb_r13_ies_s& rrc_conn_reject_r13()
      {
        assert_choice_type(types::rrc_conn_reject_r13, type_, "c1");
        return c;
      }
      const rrc_conn_reject_nb_r13_ies_s& rrc_conn_reject_r13() const
      {
        assert_choice_type(types::rrc_conn_reject_r13, type_, "c1");
        return c;
      }
      rrc_conn_reject_nb_r13_ies_s& set_rrc_conn_reject_r13();
      void                          set_spare1();

    private:
      types                        type_;
      rrc_conn_reject_nb_r13_ies_s c;
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

// RRCConnectionSetup-NB ::= SEQUENCE
struct rrc_conn_setup_nb_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { rrc_conn_setup_r13, spare1, nulltype } value;

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
      rrc_conn_setup_nb_r13_ies_s& rrc_conn_setup_r13()
      {
        assert_choice_type(types::rrc_conn_setup_r13, type_, "c1");
        return c;
      }
      const rrc_conn_setup_nb_r13_ies_s& rrc_conn_setup_r13() const
      {
        assert_choice_type(types::rrc_conn_setup_r13, type_, "c1");
        return c;
      }
      rrc_conn_setup_nb_r13_ies_s& set_rrc_conn_setup_r13();
      void                         set_spare1();

    private:
      types                       type_;
      rrc_conn_setup_nb_r13_ies_s c;
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

// RRCEarlyDataComplete-NB-r15 ::= SEQUENCE
struct rrc_early_data_complete_nb_r15_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_early_data_complete_r15, crit_exts_future, nulltype } value;

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
    rrc_early_data_complete_nb_r15_ies_s& rrc_early_data_complete_r15()
    {
      assert_choice_type(types::rrc_early_data_complete_r15, type_, "criticalExtensions");
      return c;
    }
    const rrc_early_data_complete_nb_r15_ies_s& rrc_early_data_complete_r15() const
    {
      assert_choice_type(types::rrc_early_data_complete_r15, type_, "criticalExtensions");
      return c;
    }
    rrc_early_data_complete_nb_r15_ies_s& set_rrc_early_data_complete_r15();
    void                                  set_crit_exts_future();

  private:
    types                                type_;
    rrc_early_data_complete_nb_r15_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-CCCH-MessageType-NB ::= CHOICE
struct dl_ccch_msg_type_nb_c {
  struct c1_c_ {
    struct types_opts {
      enum options {
        rrc_conn_reest_r13,
        rrc_conn_reest_reject_r13,
        rrc_conn_reject_r13,
        rrc_conn_setup_r13,
        rrc_early_data_complete_r15,
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
    c1_c_(const c1_c_& other);
    c1_c_& operator=(const c1_c_& other);
    ~c1_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_conn_reest_nb_s& rrc_conn_reest_r13()
    {
      assert_choice_type(types::rrc_conn_reest_r13, type_, "c1");
      return c.get<rrc_conn_reest_nb_s>();
    }
    rrc_conn_reest_reject_s& rrc_conn_reest_reject_r13()
    {
      assert_choice_type(types::rrc_conn_reest_reject_r13, type_, "c1");
      return c.get<rrc_conn_reest_reject_s>();
    }
    rrc_conn_reject_nb_s& rrc_conn_reject_r13()
    {
      assert_choice_type(types::rrc_conn_reject_r13, type_, "c1");
      return c.get<rrc_conn_reject_nb_s>();
    }
    rrc_conn_setup_nb_s& rrc_conn_setup_r13()
    {
      assert_choice_type(types::rrc_conn_setup_r13, type_, "c1");
      return c.get<rrc_conn_setup_nb_s>();
    }
    rrc_early_data_complete_nb_r15_s& rrc_early_data_complete_r15()
    {
      assert_choice_type(types::rrc_early_data_complete_r15, type_, "c1");
      return c.get<rrc_early_data_complete_nb_r15_s>();
    }
    const rrc_conn_reest_nb_s& rrc_conn_reest_r13() const
    {
      assert_choice_type(types::rrc_conn_reest_r13, type_, "c1");
      return c.get<rrc_conn_reest_nb_s>();
    }
    const rrc_conn_reest_reject_s& rrc_conn_reest_reject_r13() const
    {
      assert_choice_type(types::rrc_conn_reest_reject_r13, type_, "c1");
      return c.get<rrc_conn_reest_reject_s>();
    }
    const rrc_conn_reject_nb_s& rrc_conn_reject_r13() const
    {
      assert_choice_type(types::rrc_conn_reject_r13, type_, "c1");
      return c.get<rrc_conn_reject_nb_s>();
    }
    const rrc_conn_setup_nb_s& rrc_conn_setup_r13() const
    {
      assert_choice_type(types::rrc_conn_setup_r13, type_, "c1");
      return c.get<rrc_conn_setup_nb_s>();
    }
    const rrc_early_data_complete_nb_r15_s& rrc_early_data_complete_r15() const
    {
      assert_choice_type(types::rrc_early_data_complete_r15, type_, "c1");
      return c.get<rrc_early_data_complete_nb_r15_s>();
    }
    rrc_conn_reest_nb_s&              set_rrc_conn_reest_r13();
    rrc_conn_reest_reject_s&          set_rrc_conn_reest_reject_r13();
    rrc_conn_reject_nb_s&             set_rrc_conn_reject_r13();
    rrc_conn_setup_nb_s&              set_rrc_conn_setup_r13();
    rrc_early_data_complete_nb_r15_s& set_rrc_early_data_complete_r15();
    void                              set_spare3();
    void                              set_spare2();
    void                              set_spare1();

  private:
    types type_;
    choice_buffer_t<rrc_conn_reest_nb_s,
                    rrc_conn_reest_reject_s,
                    rrc_conn_reject_nb_s,
                    rrc_conn_setup_nb_s,
                    rrc_early_data_complete_nb_r15_s>
        c;

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
  dl_ccch_msg_type_nb_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "DL-CCCH-MessageType-NB");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "DL-CCCH-MessageType-NB");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// DL-CCCH-Message-NB ::= SEQUENCE
struct dl_ccch_msg_nb_s {
  dl_ccch_msg_type_nb_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NRSRP-ChangeThresh-NB-r16 ::= ENUMERATED
struct nrsrp_change_thresh_nb_r16_opts {
  enum options {
    db4,
    db6,
    db8,
    db10,
    db14,
    db18,
    db22,
    db26,
    db30,
    db34,
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
typedef enumerated<nrsrp_change_thresh_nb_r16_opts> nrsrp_change_thresh_nb_r16_e;

// PUR-NRSRP-ChangeThreshold-NB-r16 ::= SEQUENCE
struct pur_nrsrp_change_thres_nb_r16_s {
  bool                         decrease_thresh_r16_present = false;
  nrsrp_change_thresh_nb_r16_e increase_thresh_r16;
  nrsrp_change_thresh_nb_r16_e decrease_thresh_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUR-PeriodicityAndOffset-NB-r16 ::= CHOICE
struct pur_periodicity_and_offset_nb_r16_c {
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
  pur_periodicity_and_offset_nb_r16_c() = default;
  pur_periodicity_and_offset_nb_r16_c(const pur_periodicity_and_offset_nb_r16_c& other);
  pur_periodicity_and_offset_nb_r16_c& operator=(const pur_periodicity_and_offset_nb_r16_c& other);
  ~pur_periodicity_and_offset_nb_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint8_t& periodicity8()
  {
    assert_choice_type(types::periodicity8, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint8_t>();
  }
  uint8_t& periodicity16()
  {
    assert_choice_type(types::periodicity16, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint8_t>();
  }
  uint8_t& periodicity32()
  {
    assert_choice_type(types::periodicity32, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint8_t>();
  }
  uint8_t& periodicity64()
  {
    assert_choice_type(types::periodicity64, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint8_t>();
  }
  uint8_t& periodicity128()
  {
    assert_choice_type(types::periodicity128, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint8_t>();
  }
  uint16_t& periodicity256()
  {
    assert_choice_type(types::periodicity256, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint16_t>();
  }
  uint16_t& periodicity512()
  {
    assert_choice_type(types::periodicity512, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint16_t>();
  }
  uint16_t& periodicity1024()
  {
    assert_choice_type(types::periodicity1024, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint16_t>();
  }
  uint16_t& periodicity2048()
  {
    assert_choice_type(types::periodicity2048, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint16_t>();
  }
  uint16_t& periodicity4096()
  {
    assert_choice_type(types::periodicity4096, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint16_t>();
  }
  uint16_t& periodicity8192()
  {
    assert_choice_type(types::periodicity8192, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint16_t>();
  }
  const uint8_t& periodicity8() const
  {
    assert_choice_type(types::periodicity8, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& periodicity16() const
  {
    assert_choice_type(types::periodicity16, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& periodicity32() const
  {
    assert_choice_type(types::periodicity32, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& periodicity64() const
  {
    assert_choice_type(types::periodicity64, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint8_t>();
  }
  const uint8_t& periodicity128() const
  {
    assert_choice_type(types::periodicity128, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint8_t>();
  }
  const uint16_t& periodicity256() const
  {
    assert_choice_type(types::periodicity256, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& periodicity512() const
  {
    assert_choice_type(types::periodicity512, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& periodicity1024() const
  {
    assert_choice_type(types::periodicity1024, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& periodicity2048() const
  {
    assert_choice_type(types::periodicity2048, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& periodicity4096() const
  {
    assert_choice_type(types::periodicity4096, type_, "PUR-PeriodicityAndOffset-NB-r16");
    return c.get<uint16_t>();
  }
  const uint16_t& periodicity8192() const
  {
    assert_choice_type(types::periodicity8192, type_, "PUR-PeriodicityAndOffset-NB-r16");
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

// PUR-UL-16QAM-Config-NB-r17 ::= SEQUENCE
struct pur_ul_minus16_qam_cfg_nb_r17_s {
  ul_pwr_ctrl_ded_nb_v1700_s ul_pwr_ctrl_ded_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUR-Config-NB-r16 ::= SEQUENCE
struct pur_cfg_nb_r16_s {
  struct pur_implicit_release_after_r16_opts {
    enum options { n2, n4, n8, spare, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pur_implicit_release_after_r16_opts> pur_implicit_release_after_r16_e_;
  struct pur_resp_win_timer_r16_opts {
    enum options { pp1, pp2, pp3, pp4, pp8, pp16, pp32, pp64, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pur_resp_win_timer_r16_opts> pur_resp_win_timer_r16_e_;
  struct pur_start_time_params_r16_s_ {
    pur_periodicity_and_offset_nb_r16_c periodicity_and_offset_r16;
    uint16_t                            start_sfn_r16 = 0;
    uint8_t                             start_sf_r16  = 0;
    fixed_bitstring<1>                  hsfn_lsb_info_r16;
  };
  struct pur_num_occasions_r16_opts {
    enum options { one, infinite, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<pur_num_occasions_r16_opts> pur_num_occasions_r16_e_;
  struct pur_phys_cfg_r16_s_ {
    struct npusch_sub_carrier_set_idx_r16_c_ {
      struct types_opts {
        enum options { khz15, khz3dot75, nulltype } value;
        typedef float number_type;

        const char* to_string() const;
        float       to_number() const;
        const char* to_number_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      npusch_sub_carrier_set_idx_r16_c_() = default;
      npusch_sub_carrier_set_idx_r16_c_(const npusch_sub_carrier_set_idx_r16_c_& other);
      npusch_sub_carrier_set_idx_r16_c_& operator=(const npusch_sub_carrier_set_idx_r16_c_& other);
      ~npusch_sub_carrier_set_idx_r16_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      uint8_t& khz15()
      {
        assert_choice_type(types::khz15, type_, "npusch-SubCarrierSetIndex-r16");
        return c.get<uint8_t>();
      }
      uint8_t& khz3dot75()
      {
        assert_choice_type(types::khz3dot75, type_, "npusch-SubCarrierSetIndex-r16");
        return c.get<uint8_t>();
      }
      const uint8_t& khz15() const
      {
        assert_choice_type(types::khz15, type_, "npusch-SubCarrierSetIndex-r16");
        return c.get<uint8_t>();
      }
      const uint8_t& khz3dot75() const
      {
        assert_choice_type(types::khz3dot75, type_, "npusch-SubCarrierSetIndex-r16");
        return c.get<uint8_t>();
      }
      uint8_t& set_khz15();
      uint8_t& set_khz3dot75();

    private:
      types               type_;
      pod_choice_buffer_t c;

      void destroy_();
    };
    struct npusch_mcs_r16_c_ {
      struct types_opts {
        enum options { single_tone, multi_tone, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      npusch_mcs_r16_c_() = default;
      npusch_mcs_r16_c_(const npusch_mcs_r16_c_& other);
      npusch_mcs_r16_c_& operator=(const npusch_mcs_r16_c_& other);
      ~npusch_mcs_r16_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      uint8_t& single_tone()
      {
        assert_choice_type(types::single_tone, type_, "npusch-MCS-r16");
        return c.get<uint8_t>();
      }
      uint8_t& multi_tone()
      {
        assert_choice_type(types::multi_tone, type_, "npusch-MCS-r16");
        return c.get<uint8_t>();
      }
      const uint8_t& single_tone() const
      {
        assert_choice_type(types::single_tone, type_, "npusch-MCS-r16");
        return c.get<uint8_t>();
      }
      const uint8_t& multi_tone() const
      {
        assert_choice_type(types::multi_tone, type_, "npusch-MCS-r16");
        return c.get<uint8_t>();
      }
      uint8_t& set_single_tone();
      uint8_t& set_multi_tone();

    private:
      types               type_;
      pod_choice_buffer_t c;

      void destroy_();
    };
    struct alpha_r16_opts {
      enum options { al0, al04, al05, al06, al07, al08, al09, al1, nulltype } value;
      typedef float number_type;

      const char* to_string() const;
      float       to_number() const;
      const char* to_number_string() const;
    };
    typedef enumerated<alpha_r16_opts> alpha_r16_e_;
    struct npusch_cyclic_shift_r16_opts {
      enum options { n0, n6, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<npusch_cyclic_shift_r16_opts> npusch_cyclic_shift_r16_e_;

    // member variables
    carrier_cfg_ded_nb_r13_s          carrier_cfg_r16;
    uint8_t                           npusch_num_rus_idx_r16     = 0;
    uint8_t                           npusch_num_repeats_idx_r16 = 0;
    npusch_sub_carrier_set_idx_r16_c_ npusch_sub_carrier_set_idx_r16;
    npusch_mcs_r16_c_                 npusch_mcs_r16;
    int8_t                            p0_ue_npusch_r16 = -8;
    alpha_r16_e_                      alpha_r16;
    npusch_cyclic_shift_r16_e_        npusch_cyclic_shift_r16;
    npdcch_cfg_ded_nb_r13_s           npdcch_cfg_r16;
  };
  struct pur_phys_cfg_v1650_s_ {
    ack_nack_num_repeats_nb_r13_e ack_nack_num_repeats_r16;
  };
  struct pur_phys_cfg_v1700_s_ {
    bool                                             pur_ul_minus16_qam_cfg_r17_present = false;
    bool                                             pur_dl_minus16_qam_cfg_r17_present = false;
    setup_release_c<pur_ul_minus16_qam_cfg_nb_r17_s> pur_ul_minus16_qam_cfg_r17;
    setup_release_c<npdsch_minus16_qam_cfg_nb_r17_s> pur_dl_minus16_qam_cfg_r17;
  };

  // member variables
  bool                                             ext                                    = false;
  bool                                             pur_cfg_id_r16_present                 = false;
  bool                                             pur_time_align_timer_r16_present       = false;
  bool                                             pur_nrsrp_change_thres_r16_present     = false;
  bool                                             pur_implicit_release_after_r16_present = false;
  bool                                             pur_rnti_r16_present                   = false;
  bool                                             pur_resp_win_timer_r16_present         = false;
  bool                                             pur_start_time_params_r16_present      = false;
  bool                                             pur_phys_cfg_r16_present               = false;
  fixed_bitstring<20>                              pur_cfg_id_r16;
  uint8_t                                          pur_time_align_timer_r16 = 1;
  setup_release_c<pur_nrsrp_change_thres_nb_r16_s> pur_nrsrp_change_thres_r16;
  pur_implicit_release_after_r16_e_                pur_implicit_release_after_r16;
  fixed_bitstring<16>                              pur_rnti_r16;
  pur_resp_win_timer_r16_e_                        pur_resp_win_timer_r16;
  pur_start_time_params_r16_s_                     pur_start_time_params_r16;
  pur_num_occasions_r16_e_                         pur_num_occasions_r16;
  pur_phys_cfg_r16_s_                              pur_phys_cfg_r16;
  // ...
  // group 0
  copy_ptr<pur_phys_cfg_v1650_s_> pur_phys_cfg_v1650;
  // group 1
  copy_ptr<pur_phys_cfg_v1700_s_> pur_phys_cfg_v1700;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease-NB-v1700-IEs ::= SEQUENCE
struct rrc_conn_release_nb_v1700_ies_s {
  bool    cbp_idx_r17_present  = false;
  bool    non_crit_ext_present = false;
  uint8_t cbp_idx_r17          = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease-NB-v1610-IEs ::= SEQUENCE
struct rrc_conn_release_nb_v1610_ies_s {
  bool                              resume_id_r16_present    = false;
  bool                              anr_meas_cfg_r16_present = false;
  bool                              pur_cfg_r16_present      = false;
  bool                              non_crit_ext_present     = false;
  fixed_bitstring<40>               resume_id_r16;
  anr_meas_cfg_nb_r16_s             anr_meas_cfg_r16;
  setup_release_c<pur_cfg_nb_r16_s> pur_cfg_r16;
  rrc_conn_release_nb_v1700_ies_s   non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease-NB-v15b0-IEs ::= SEQUENCE
struct rrc_conn_release_nb_v15b0_ies_s {
  bool                            no_last_cell_upd_r15_present = false;
  bool                            non_crit_ext_present         = false;
  rrc_conn_release_nb_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RedirectedCarrierInfo-NB-v1550 ::= CarrierFreq-NB-v1550
using redirected_carrier_info_nb_v1550_s = carrier_freq_nb_v1550_s;

// RRCConnectionRelease-NB-v1550-IEs ::= SEQUENCE
struct rrc_conn_release_nb_v1550_ies_s {
  bool                               redirected_carrier_info_v1550_present = false;
  bool                               non_crit_ext_present                  = false;
  redirected_carrier_info_nb_v1550_s redirected_carrier_info_v1550;
  rrc_conn_release_nb_v15b0_ies_s    non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease-NB-v1530-IEs ::= SEQUENCE
struct rrc_conn_release_nb_v1530_ies_s {
  bool                            drb_continue_rohc_r15_present       = false;
  bool                            next_hop_chaining_count_r15_present = false;
  bool                            non_crit_ext_present                = false;
  uint8_t                         next_hop_chaining_count_r15         = 0;
  rrc_conn_release_nb_v1550_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease-NB-v1430-IEs ::= SEQUENCE
struct rrc_conn_release_nb_v1430_ies_s {
  bool                               redirected_carrier_info_v1430_present = false;
  bool                               extended_wait_time_cpdata_r14_present = false;
  bool                               non_crit_ext_present                  = false;
  redirected_carrier_info_nb_v1430_s redirected_carrier_info_v1430;
  uint16_t                           extended_wait_time_cpdata_r14 = 1;
  rrc_conn_release_nb_v1530_ies_s    non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResume-NB-v1610-IEs ::= SEQUENCE
struct rrc_conn_resume_nb_v1610_ies_s {
  bool full_cfg_r16_present = false;
  bool non_crit_ext_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReleaseCause-NB-r13 ::= ENUMERATED
struct release_cause_nb_r13_opts {
  enum options { load_balancing_ta_urequired, other, rrc_suspend, spare1, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<release_cause_nb_r13_opts> release_cause_nb_r13_e;

// DLInformationTransfer-NB-r13-IEs ::= SEQUENCE
struct dl_info_transfer_nb_r13_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring ded_info_nas_r13;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfiguration-NB-r13-IEs ::= SEQUENCE
struct rrc_conn_recfg_nb_r13_ies_s {
  using ded_info_nas_list_r13_l_ = bounded_array<dyn_octstring, 2>;

  // member variables
  bool                     ded_info_nas_list_r13_present = false;
  bool                     rr_cfg_ded_r13_present        = false;
  bool                     full_cfg_r13_present          = false;
  bool                     late_non_crit_ext_present     = false;
  bool                     non_crit_ext_present          = false;
  ded_info_nas_list_r13_l_ ded_info_nas_list_r13;
  rr_cfg_ded_nb_r13_s      rr_cfg_ded_r13;
  dyn_octstring            late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRelease-NB-r13-IEs ::= SEQUENCE
struct rrc_conn_release_nb_r13_ies_s {
  bool                             resume_id_r13_present               = false;
  bool                             extended_wait_time_r13_present      = false;
  bool                             redirected_carrier_info_r13_present = false;
  bool                             late_non_crit_ext_present           = false;
  bool                             non_crit_ext_present                = false;
  release_cause_nb_r13_e           release_cause_r13;
  fixed_bitstring<40>              resume_id_r13;
  uint16_t                         extended_wait_time_r13 = 1;
  redirected_carrier_info_nb_r13_s redirected_carrier_info_r13;
  dyn_octstring                    late_non_crit_ext;
  rrc_conn_release_nb_v1430_ies_s  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResume-NB-r13-IEs ::= SEQUENCE
struct rrc_conn_resume_nb_r13_ies_s {
  bool                           rr_cfg_ded_r13_present        = false;
  bool                           drb_continue_rohc_r13_present = false;
  bool                           late_non_crit_ext_present     = false;
  bool                           non_crit_ext_present          = false;
  rr_cfg_ded_nb_r13_s            rr_cfg_ded_r13;
  uint8_t                        next_hop_chaining_count_r13 = 0;
  dyn_octstring                  late_non_crit_ext;
  rrc_conn_resume_nb_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityEnquiry-NB-r13-IEs ::= SEQUENCE
struct ue_cap_enquiry_nb_r13_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationRequest-NB-r16-IEs ::= SEQUENCE
struct ue_info_request_nb_r16_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  bool          rach_report_req_r16       = false;
  bool          rlf_report_req_r16        = false;
  bool          anr_report_req_r16        = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DLInformationTransfer-NB ::= SEQUENCE
struct dl_info_transfer_nb_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { dl_info_transfer_r13, spare1, nulltype } value;

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
      dl_info_transfer_nb_r13_ies_s& dl_info_transfer_r13()
      {
        assert_choice_type(types::dl_info_transfer_r13, type_, "c1");
        return c;
      }
      const dl_info_transfer_nb_r13_ies_s& dl_info_transfer_r13() const
      {
        assert_choice_type(types::dl_info_transfer_r13, type_, "c1");
        return c;
      }
      dl_info_transfer_nb_r13_ies_s& set_dl_info_transfer_r13();
      void                           set_spare1();

    private:
      types                         type_;
      dl_info_transfer_nb_r13_ies_s c;
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

// RRCConnectionReconfiguration-NB ::= SEQUENCE
struct rrc_conn_recfg_nb_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { rrc_conn_recfg_r13, spare1, nulltype } value;

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
      rrc_conn_recfg_nb_r13_ies_s& rrc_conn_recfg_r13()
      {
        assert_choice_type(types::rrc_conn_recfg_r13, type_, "c1");
        return c;
      }
      const rrc_conn_recfg_nb_r13_ies_s& rrc_conn_recfg_r13() const
      {
        assert_choice_type(types::rrc_conn_recfg_r13, type_, "c1");
        return c;
      }
      rrc_conn_recfg_nb_r13_ies_s& set_rrc_conn_recfg_r13();
      void                         set_spare1();

    private:
      types                       type_;
      rrc_conn_recfg_nb_r13_ies_s c;
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

// RRCConnectionRelease-NB ::= SEQUENCE
struct rrc_conn_release_nb_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { rrc_conn_release_r13, spare1, nulltype } value;

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
      rrc_conn_release_nb_r13_ies_s& rrc_conn_release_r13()
      {
        assert_choice_type(types::rrc_conn_release_r13, type_, "c1");
        return c;
      }
      const rrc_conn_release_nb_r13_ies_s& rrc_conn_release_r13() const
      {
        assert_choice_type(types::rrc_conn_release_r13, type_, "c1");
        return c;
      }
      rrc_conn_release_nb_r13_ies_s& set_rrc_conn_release_r13();
      void                           set_spare1();

    private:
      types                         type_;
      rrc_conn_release_nb_r13_ies_s c;
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

// RRCConnectionResume-NB ::= SEQUENCE
struct rrc_conn_resume_nb_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { rrc_conn_resume_r13, spare1, nulltype } value;

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
      rrc_conn_resume_nb_r13_ies_s& rrc_conn_resume_r13()
      {
        assert_choice_type(types::rrc_conn_resume_r13, type_, "c1");
        return c;
      }
      const rrc_conn_resume_nb_r13_ies_s& rrc_conn_resume_r13() const
      {
        assert_choice_type(types::rrc_conn_resume_r13, type_, "c1");
        return c;
      }
      rrc_conn_resume_nb_r13_ies_s& set_rrc_conn_resume_r13();
      void                          set_spare1();

    private:
      types                        type_;
      rrc_conn_resume_nb_r13_ies_s c;
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

// UECapabilityEnquiry-NB ::= SEQUENCE
struct ue_cap_enquiry_nb_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ue_cap_enquiry_r13, spare1, nulltype } value;

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
      ue_cap_enquiry_nb_r13_ies_s& ue_cap_enquiry_r13()
      {
        assert_choice_type(types::ue_cap_enquiry_r13, type_, "c1");
        return c;
      }
      const ue_cap_enquiry_nb_r13_ies_s& ue_cap_enquiry_r13() const
      {
        assert_choice_type(types::ue_cap_enquiry_r13, type_, "c1");
        return c;
      }
      ue_cap_enquiry_nb_r13_ies_s& set_ue_cap_enquiry_r13();
      void                         set_spare1();

    private:
      types                       type_;
      ue_cap_enquiry_nb_r13_ies_s c;
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

// UEInformationRequest-NB-r16 ::= SEQUENCE
struct ue_info_request_nb_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_info_request_r16, crit_exts_future, nulltype } value;

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
    ue_info_request_nb_r16_ies_s& ue_info_request_r16()
    {
      assert_choice_type(types::ue_info_request_r16, type_, "criticalExtensions");
      return c;
    }
    const ue_info_request_nb_r16_ies_s& ue_info_request_r16() const
    {
      assert_choice_type(types::ue_info_request_r16, type_, "criticalExtensions");
      return c;
    }
    ue_info_request_nb_r16_ies_s& set_ue_info_request_r16();
    void                          set_crit_exts_future();

  private:
    types                        type_;
    ue_info_request_nb_r16_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-DCCH-MessageType-NB ::= CHOICE
struct dl_dcch_msg_type_nb_c {
  struct c1_c_ {
    struct types_opts {
      enum options {
        dl_info_transfer_r13,
        rrc_conn_recfg_r13,
        rrc_conn_release_r13,
        security_mode_cmd_r13,
        ue_cap_enquiry_r13,
        rrc_conn_resume_r13,
        ue_info_request_r16,
        spare1,
        nulltype
      } value;

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
    dl_info_transfer_nb_s& dl_info_transfer_r13()
    {
      assert_choice_type(types::dl_info_transfer_r13, type_, "c1");
      return c.get<dl_info_transfer_nb_s>();
    }
    rrc_conn_recfg_nb_s& rrc_conn_recfg_r13()
    {
      assert_choice_type(types::rrc_conn_recfg_r13, type_, "c1");
      return c.get<rrc_conn_recfg_nb_s>();
    }
    rrc_conn_release_nb_s& rrc_conn_release_r13()
    {
      assert_choice_type(types::rrc_conn_release_r13, type_, "c1");
      return c.get<rrc_conn_release_nb_s>();
    }
    security_mode_cmd_s& security_mode_cmd_r13()
    {
      assert_choice_type(types::security_mode_cmd_r13, type_, "c1");
      return c.get<security_mode_cmd_s>();
    }
    ue_cap_enquiry_nb_s& ue_cap_enquiry_r13()
    {
      assert_choice_type(types::ue_cap_enquiry_r13, type_, "c1");
      return c.get<ue_cap_enquiry_nb_s>();
    }
    rrc_conn_resume_nb_s& rrc_conn_resume_r13()
    {
      assert_choice_type(types::rrc_conn_resume_r13, type_, "c1");
      return c.get<rrc_conn_resume_nb_s>();
    }
    ue_info_request_nb_r16_s& ue_info_request_r16()
    {
      assert_choice_type(types::ue_info_request_r16, type_, "c1");
      return c.get<ue_info_request_nb_r16_s>();
    }
    const dl_info_transfer_nb_s& dl_info_transfer_r13() const
    {
      assert_choice_type(types::dl_info_transfer_r13, type_, "c1");
      return c.get<dl_info_transfer_nb_s>();
    }
    const rrc_conn_recfg_nb_s& rrc_conn_recfg_r13() const
    {
      assert_choice_type(types::rrc_conn_recfg_r13, type_, "c1");
      return c.get<rrc_conn_recfg_nb_s>();
    }
    const rrc_conn_release_nb_s& rrc_conn_release_r13() const
    {
      assert_choice_type(types::rrc_conn_release_r13, type_, "c1");
      return c.get<rrc_conn_release_nb_s>();
    }
    const security_mode_cmd_s& security_mode_cmd_r13() const
    {
      assert_choice_type(types::security_mode_cmd_r13, type_, "c1");
      return c.get<security_mode_cmd_s>();
    }
    const ue_cap_enquiry_nb_s& ue_cap_enquiry_r13() const
    {
      assert_choice_type(types::ue_cap_enquiry_r13, type_, "c1");
      return c.get<ue_cap_enquiry_nb_s>();
    }
    const rrc_conn_resume_nb_s& rrc_conn_resume_r13() const
    {
      assert_choice_type(types::rrc_conn_resume_r13, type_, "c1");
      return c.get<rrc_conn_resume_nb_s>();
    }
    const ue_info_request_nb_r16_s& ue_info_request_r16() const
    {
      assert_choice_type(types::ue_info_request_r16, type_, "c1");
      return c.get<ue_info_request_nb_r16_s>();
    }
    dl_info_transfer_nb_s&    set_dl_info_transfer_r13();
    rrc_conn_recfg_nb_s&      set_rrc_conn_recfg_r13();
    rrc_conn_release_nb_s&    set_rrc_conn_release_r13();
    security_mode_cmd_s&      set_security_mode_cmd_r13();
    ue_cap_enquiry_nb_s&      set_ue_cap_enquiry_r13();
    rrc_conn_resume_nb_s&     set_rrc_conn_resume_r13();
    ue_info_request_nb_r16_s& set_ue_info_request_r16();
    void                      set_spare1();

  private:
    types type_;
    choice_buffer_t<dl_info_transfer_nb_s,
                    rrc_conn_recfg_nb_s,
                    rrc_conn_release_nb_s,
                    rrc_conn_resume_nb_s,
                    security_mode_cmd_s,
                    ue_cap_enquiry_nb_s,
                    ue_info_request_nb_r16_s>
        c;

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
  dl_dcch_msg_type_nb_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "DL-DCCH-MessageType-NB");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "DL-DCCH-MessageType-NB");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// DL-DCCH-Message-NB ::= SEQUENCE
struct dl_dcch_msg_nb_s {
  dl_dcch_msg_type_nb_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBand-NB-r13 ::= SEQUENCE
struct supported_band_nb_r13_s {
  bool     pwr_class_nb_minus20dbm_r13_present = false;
  uint16_t band_r13                            = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandList-NB-r13 ::= SEQUENCE (SIZE (1..64)) OF SupportedBand-NB-r13
using supported_band_list_nb_r13_l = dyn_array<supported_band_nb_r13_s>;

// AccessStratumRelease-NB-r13 ::= ENUMERATED
struct access_stratum_release_nb_r13_opts {
  enum options { rel13, rel14, rel15, rel16, rel17, spare3, spare2, spare1, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<access_stratum_release_nb_r13_opts, true> access_stratum_release_nb_r13_e;

// HandoverPreparationInformation-NB-Ext-r14-IEs ::= SEQUENCE
struct ho_prep_info_nb_ext_r14_ies_s {
  bool          ue_radio_access_cap_info_ext_r14_present = false;
  bool          non_crit_ext_present                     = false;
  dyn_octstring ue_radio_access_cap_info_ext_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PDCP-Parameters-NB-r13 ::= SEQUENCE
struct pdcp_params_nb_r13_s {
  struct supported_rohc_profiles_r13_s_ {
    bool profile0x0002 = false;
    bool profile0x0003 = false;
    bool profile0x0004 = false;
    bool profile0x0006 = false;
    bool profile0x0102 = false;
    bool profile0x0103 = false;
    bool profile0x0104 = false;
  };
  struct max_num_rohc_context_sessions_r13_opts {
    enum options { cs2, cs4, cs8, cs12, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<max_num_rohc_context_sessions_r13_opts> max_num_rohc_context_sessions_r13_e_;

  // member variables
  bool                                 ext                                       = false;
  bool                                 max_num_rohc_context_sessions_r13_present = false;
  supported_rohc_profiles_r13_s_       supported_rohc_profiles_r13;
  max_num_rohc_context_sessions_r13_e_ max_num_rohc_context_sessions_r13;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-NB-r13 ::= SEQUENCE
struct phy_layer_params_nb_r13_s {
  bool multi_tone_r13_present    = false;
  bool multi_carrier_r13_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RF-Parameters-NB-r13 ::= SEQUENCE
struct rf_params_nb_r13_s {
  bool                         multi_ns_pmax_r13_present = false;
  supported_band_list_nb_r13_l supported_band_list_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-NB-v1380-IEs ::= SEQUENCE
struct ho_prep_info_nb_v1380_ies_s {
  bool                          late_non_crit_ext_present = false;
  bool                          non_crit_ext_present      = false;
  dyn_octstring                 late_non_crit_ext;
  ho_prep_info_nb_ext_r14_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRM-Config-NB ::= SEQUENCE
struct rrm_cfg_nb_s {
  struct ue_inactive_time_opts {
    enum options {
      s1,
      s2,
      s3,
      s5,
      s7,
      s10,
      s15,
      s20,
      s25,
      s30,
      s40,
      s50,
      min1,
      min1s20,
      min1s40,
      min2,
      min2s30,
      min3,
      min3s30,
      min4,
      min5,
      min6,
      min7,
      min8,
      min9,
      min10,
      min12,
      min14,
      min17,
      min20,
      min24,
      min28,
      min33,
      min38,
      min44,
      min50,
      hr1,
      hr1min30,
      hr2,
      hr2min30,
      hr3,
      hr3min30,
      hr4,
      hr5,
      hr6,
      hr8,
      hr10,
      hr13,
      hr16,
      hr20,
      day1,
      day1hr12,
      day2,
      day2hr12,
      day3,
      day4,
      day5,
      day7,
      day10,
      day14,
      day19,
      day24,
      day30,
      day_more_than30,
      nulltype
    } value;

    const char* to_string() const;
  };
  typedef enumerated<ue_inactive_time_opts> ue_inactive_time_e_;

  // member variables
  bool                ext                      = false;
  bool                ue_inactive_time_present = false;
  ue_inactive_time_e_ ue_inactive_time;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-Capability-NB-r13 ::= SEQUENCE
struct ue_cap_nb_r13_s {
  bool                            ue_category_nb_r13_present = false;
  bool                            multiple_drb_r13_present   = false;
  bool                            pdcp_params_r13_present    = false;
  bool                            dummy_present              = false;
  access_stratum_release_nb_r13_e access_stratum_release_r13;
  pdcp_params_nb_r13_s            pdcp_params_r13;
  phy_layer_params_nb_r13_s       phy_layer_params_r13;
  rf_params_nb_r13_s              rf_params_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-NB-IEs ::= SEQUENCE
struct ho_prep_info_nb_ies_s {
  bool                        rrm_cfg_r13_present    = false;
  bool                        as_context_r13_present = false;
  bool                        non_crit_ext_present   = false;
  ue_cap_nb_r13_s             ue_radio_access_cap_info_r13;
  as_cfg_nb_s                 as_cfg_r13;
  rrm_cfg_nb_s                rrm_cfg_r13;
  as_context_nb_s             as_context_r13;
  ho_prep_info_nb_v1380_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// HandoverPreparationInformation-NB ::= SEQUENCE
struct ho_prep_info_nb_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ho_prep_info_r13, spare3, spare2, spare1, nulltype } value;

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
      ho_prep_info_nb_ies_s& ho_prep_info_r13()
      {
        assert_choice_type(types::ho_prep_info_r13, type_, "c1");
        return c;
      }
      const ho_prep_info_nb_ies_s& ho_prep_info_r13() const
      {
        assert_choice_type(types::ho_prep_info_r13, type_, "c1");
        return c;
      }
      ho_prep_info_nb_ies_s& set_ho_prep_info_r13();
      void                   set_spare3();
      void                   set_spare2();
      void                   set_spare1();

    private:
      types                 type_;
      ho_prep_info_nb_ies_s c;
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

// InitialUE-Identity-5GC-NB-r16 ::= CHOICE
struct init_ue_id_minus5_gc_nb_r16_c {
  struct types_opts {
    enum options { ng_minus5_g_s_tmsi_r16, random_value, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  init_ue_id_minus5_gc_nb_r16_c() = default;
  init_ue_id_minus5_gc_nb_r16_c(const init_ue_id_minus5_gc_nb_r16_c& other);
  init_ue_id_minus5_gc_nb_r16_c& operator=(const init_ue_id_minus5_gc_nb_r16_c& other);
  ~init_ue_id_minus5_gc_nb_r16_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<48>& ng_minus5_g_s_tmsi_r16()
  {
    assert_choice_type(types::ng_minus5_g_s_tmsi_r16, type_, "InitialUE-Identity-5GC-NB-r16");
    return c.get<fixed_bitstring<48> >();
  }
  fixed_bitstring<48>& random_value()
  {
    assert_choice_type(types::random_value, type_, "InitialUE-Identity-5GC-NB-r16");
    return c.get<fixed_bitstring<48> >();
  }
  const fixed_bitstring<48>& ng_minus5_g_s_tmsi_r16() const
  {
    assert_choice_type(types::ng_minus5_g_s_tmsi_r16, type_, "InitialUE-Identity-5GC-NB-r16");
    return c.get<fixed_bitstring<48> >();
  }
  const fixed_bitstring<48>& random_value() const
  {
    assert_choice_type(types::random_value, type_, "InitialUE-Identity-5GC-NB-r16");
    return c.get<fixed_bitstring<48> >();
  }
  fixed_bitstring<48>& set_ng_minus5_g_s_tmsi_r16();
  fixed_bitstring<48>& set_random_value();

private:
  types                                 type_;
  choice_buffer_t<fixed_bitstring<48> > c;

  void destroy_();
};

// PagingRecord-NB-v1610 ::= SEQUENCE
struct paging_record_nb_v1610_s {
  bool mt_edt_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingRecord-NB-r13 ::= SEQUENCE
struct paging_record_nb_r13_s {
  bool           ext = false;
  paging_ue_id_c ue_id_r13;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingRecordList-NB-v1610 ::= SEQUENCE (SIZE (1..16)) OF PagingRecord-NB-v1610
using paging_record_list_nb_v1610_l = dyn_array<paging_record_nb_v1610_s>;

// Paging-NB-v1610-IEs ::= SEQUENCE
struct paging_nb_v1610_ies_s {
  bool                          paging_record_list_v1610_present = false;
  bool                          non_crit_ext_present             = false;
  paging_record_list_nb_v1610_l paging_record_list_v1610;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingRecordList-NB-r13 ::= SEQUENCE (SIZE (1..16)) OF PagingRecord-NB-r13
using paging_record_list_nb_r13_l = dyn_array<paging_record_nb_r13_s>;

// Paging-NB ::= SEQUENCE
struct paging_nb_s {
  bool                        paging_record_list_r13_present = false;
  bool                        sys_info_mod_r13_present       = false;
  bool                        sys_info_mod_e_drx_r13_present = false;
  bool                        non_crit_ext_present           = false;
  paging_record_list_nb_r13_l paging_record_list_r13;
  paging_nb_v1610_ies_s       non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCCH-MessageType-NB ::= CHOICE
struct pcch_msg_type_nb_c {
  struct c1_c_ {
    struct types_opts {
      enum options { paging_r13, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::paging_r13; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    paging_nb_s&       paging_r13() { return c; }
    const paging_nb_s& paging_r13() const { return c; }

  private:
    paging_nb_s c;
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  pcch_msg_type_nb_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "PCCH-MessageType-NB");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "PCCH-MessageType-NB");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// PCCH-Message-NB ::= SEQUENCE
struct pcch_msg_nb_s {
  pcch_msg_type_nb_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCI-ARFCN-NB-r14 ::= SEQUENCE
struct pci_arfcn_nb_r14_s {
  bool                  carrier_freq_r14_present = false;
  uint16_t              pci_r14                  = 0;
  carrier_freq_nb_r13_s carrier_freq_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUR-ConfigRequest-NB-r16 ::= CHOICE
struct pur_cfg_request_nb_r16_c {
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
        b376,
        b424,
        b472,
        b504,
        b552,
        b584,
        b616,
        b680,
        b744,
        b776,
        b808,
        b872,
        b904,
        b936,
        b968,
        b1000,
        b1032,
        b1096,
        b1128,
        b1192,
        b1224,
        b1256,
        b1352,
        b1384,
        b1544,
        b1608,
        b1736,
        b1800,
        b2024,
        b2280,
        b2536,
        nulltype
      } value;
      typedef uint16_t number_type;

      const char* to_string() const;
      uint16_t    to_number() const;
    };
    typedef enumerated<requested_tbs_r16_opts> requested_tbs_r16_e_;

    // member variables
    bool                                rrc_ack_r16_present = false;
    requested_num_occasions_r16_e_      requested_num_occasions_r16;
    pur_periodicity_and_offset_nb_r16_c requested_periodicity_and_offset_r16;
    requested_tbs_r16_e_                requested_tbs_r16;
  };
  struct types_opts {
    enum options { pur_release_request, pur_setup_request, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  pur_cfg_request_nb_r16_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  pur_setup_request_s_& pur_setup_request()
  {
    assert_choice_type(types::pur_setup_request, type_, "PUR-ConfigRequest-NB-r16");
    return c;
  }
  const pur_setup_request_s_& pur_setup_request() const
  {
    assert_choice_type(types::pur_setup_request, type_, "PUR-ConfigRequest-NB-r16");
    return c;
  }
  void                  set_pur_release_request();
  pur_setup_request_s_& set_pur_setup_request();

private:
  types                type_;
  pur_setup_request_s_ c;
};

// PURConfigurationRequest-NB-r16-IEs ::= SEQUENCE
struct pur_cfg_request_nb_r16_ies_s {
  bool                     pur_cfg_request_r16_present = false;
  bool                     late_non_crit_ext_present   = false;
  bool                     non_crit_ext_present        = false;
  pur_cfg_request_nb_r16_c pur_cfg_request_r16;
  dyn_octstring            late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PURConfigurationRequest-NB-r16 ::= SEQUENCE
struct pur_cfg_request_nb_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { pur_cfg_request_r16, crit_exts_future, nulltype } value;

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
    pur_cfg_request_nb_r16_ies_s& pur_cfg_request_r16()
    {
      assert_choice_type(types::pur_cfg_request_r16, type_, "criticalExtensions");
      return c;
    }
    const pur_cfg_request_nb_r16_ies_s& pur_cfg_request_r16() const
    {
      assert_choice_type(types::pur_cfg_request_r16, type_, "criticalExtensions");
      return c;
    }
    pur_cfg_request_nb_r16_ies_s& set_pur_cfg_request_r16();
    void                          set_crit_exts_future();

  private:
    types                        type_;
    pur_cfg_request_nb_r16_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBand-NB-v1710 ::= SEQUENCE
struct supported_band_nb_v1710_s {
  bool npusch_minus16_qam_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SupportedBandList-NB-v1710 ::= SEQUENCE (SIZE (1..64)) OF SupportedBand-NB-v1710
using supported_band_list_nb_v1710_l = dyn_array<supported_band_nb_v1710_s>;

// RF-Parameters-NB-v1710 ::= SEQUENCE
struct rf_params_nb_v1710_s {
  bool                           supported_band_list_v1710_present = false;
  supported_band_list_nb_v1710_l supported_band_list_v1710;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfigurationComplete-NB-r13-IEs ::= SEQUENCE
struct rrc_conn_recfg_complete_nb_r13_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReconfigurationComplete-NB ::= SEQUENCE
struct rrc_conn_recfg_complete_nb_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_conn_recfg_complete_r13, crit_exts_future, nulltype } value;

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
    rrc_conn_recfg_complete_nb_r13_ies_s& rrc_conn_recfg_complete_r13()
    {
      assert_choice_type(types::rrc_conn_recfg_complete_r13, type_, "criticalExtensions");
      return c;
    }
    const rrc_conn_recfg_complete_nb_r13_ies_s& rrc_conn_recfg_complete_r13() const
    {
      assert_choice_type(types::rrc_conn_recfg_complete_r13, type_, "criticalExtensions");
      return c;
    }
    rrc_conn_recfg_complete_nb_r13_ies_s& set_rrc_conn_recfg_complete_r13();
    void                                  set_crit_exts_future();

  private:
    types                                type_;
    rrc_conn_recfg_complete_nb_r13_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentComplete-NB-v1710-IEs ::= SEQUENCE
struct rrc_conn_reest_complete_nb_v1710_ies_s {
  bool                    gnss_validity_dur_r17_present = false;
  bool                    non_crit_ext_present          = false;
  gnss_validity_dur_r17_e gnss_validity_dur_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentComplete-NB-v1610-IEs ::= SEQUENCE
struct rrc_conn_reest_complete_nb_v1610_ies_s {
  bool                                   rlf_info_available_r16_present = false;
  bool                                   anr_info_available_r16_present = false;
  bool                                   non_crit_ext_present           = false;
  rrc_conn_reest_complete_nb_v1710_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentComplete-NB-v1470-IEs ::= SEQUENCE
struct rrc_conn_reest_complete_nb_v1470_ies_s {
  bool                                   meas_result_serv_cell_r14_present = false;
  bool                                   non_crit_ext_present              = false;
  meas_result_serv_cell_nb_r14_s         meas_result_serv_cell_r14;
  rrc_conn_reest_complete_nb_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentComplete-NB-r13-IEs ::= SEQUENCE
struct rrc_conn_reest_complete_nb_r13_ies_s {
  bool                                   late_non_crit_ext_present = false;
  bool                                   non_crit_ext_present      = false;
  dyn_octstring                          late_non_crit_ext;
  rrc_conn_reest_complete_nb_v1470_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentComplete-NB ::= SEQUENCE
struct rrc_conn_reest_complete_nb_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_conn_reest_complete_r13, crit_exts_future, nulltype } value;

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
    rrc_conn_reest_complete_nb_r13_ies_s& rrc_conn_reest_complete_r13()
    {
      assert_choice_type(types::rrc_conn_reest_complete_r13, type_, "criticalExtensions");
      return c;
    }
    const rrc_conn_reest_complete_nb_r13_ies_s& rrc_conn_reest_complete_r13() const
    {
      assert_choice_type(types::rrc_conn_reest_complete_r13, type_, "criticalExtensions");
      return c;
    }
    rrc_conn_reest_complete_nb_r13_ies_s& set_rrc_conn_reest_complete_r13();
    void                                  set_crit_exts_future();

  private:
    types                                type_;
    rrc_conn_reest_complete_nb_r13_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CQI-NPDCCH-Short-NB-r14 ::= ENUMERATED
struct cqi_npdcch_short_nb_r14_opts {
  enum options { no_meass, candidate_rep_minus1, candidate_rep_minus2, candidate_rep_minus3, nulltype } value;
  typedef int8_t number_type;

  const char* to_string() const;
  int8_t      to_number() const;
};
typedef enumerated<cqi_npdcch_short_nb_r14_opts> cqi_npdcch_short_nb_r14_e;

// ReestabUE-Identity-CP-5GC-NB-r16 ::= SEQUENCE
struct reestab_ue_id_cp_minus5_gc_nb_r16_s {
  fixed_bitstring<40> truncated5_g_s_tmsi_r16;
  fixed_bitstring<16> ul_nas_mac_r16;
  fixed_bitstring<5>  ul_nas_count_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ReestablishmentCause-NB-r13 ::= ENUMERATED
struct reest_cause_nb_r13_opts {
  enum options { recfg_fail, other_fail, spare2, spare1, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<reest_cause_nb_r13_opts> reest_cause_nb_r13_e;

// RRCConnectionReestablishmentRequest-5GC-NB-r16-IEs ::= SEQUENCE
struct rrc_conn_reest_request_minus5_gc_nb_r16_ies_s {
  reestab_ue_id_cp_minus5_gc_nb_r16_s ue_id_r16;
  reest_cause_nb_r13_e                reest_cause_r16;
  cqi_npdcch_short_nb_r14_e           cqi_npdcch_r16;
  fixed_bitstring<1>                  spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CQI-NPDCCH-NB-r14 ::= ENUMERATED
struct cqi_npdcch_nb_r14_opts {
  enum options {
    no_meass,
    candidate_rep_a,
    candidate_rep_b,
    candidate_rep_c,
    candidate_rep_d,
    candidate_rep_e,
    candidate_rep_f,
    candidate_rep_g,
    candidate_rep_h,
    candidate_rep_i,
    candidate_rep_j,
    candidate_rep_k,
    candidate_rep_l,
    nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<cqi_npdcch_nb_r14_opts> cqi_npdcch_nb_r14_e;

// ReestabUE-Identity-CP-NB-r14 ::= SEQUENCE
struct reestab_ue_id_cp_nb_r14_s {
  s_tmsi_s            s_tmsi_r14;
  fixed_bitstring<16> ul_nas_mac_r14;
  fixed_bitstring<5>  ul_nas_count_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentRequest-NB-r13-IEs ::= SEQUENCE
struct rrc_conn_reest_request_nb_r13_ies_s {
  reestab_ue_id_s      ue_id_r13;
  reest_cause_nb_r13_e reest_cause_r13;
  cqi_npdcch_nb_r14_e  cqi_npdcch_r14;
  bool                 early_contention_resolution_r14 = false;
  fixed_bitstring<20>  spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentRequest-NB-r14-IEs ::= SEQUENCE
struct rrc_conn_reest_request_nb_r14_ies_s {
  reestab_ue_id_cp_nb_r14_s ue_id_r14;
  reest_cause_nb_r13_e      reest_cause_r14;
  cqi_npdcch_short_nb_r14_e cqi_npdcch_r14;
  bool                      early_contention_resolution_r14 = false;
  fixed_bitstring<1>        spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentRequest-NB ::= SEQUENCE
struct rrc_conn_reest_request_nb_s {
  struct crit_exts_c_ {
    struct later_c_ {
      struct later_c__ {
        struct types_opts {
          enum options { rrc_conn_reest_request_r16, crit_exts_future, nulltype } value;

          const char* to_string() const;
        };
        typedef enumerated<types_opts> types;

        // choice methods
        later_c__() = default;
        void        set(types::options e = types::nulltype);
        types       type() const { return type_; }
        SRSASN_CODE pack(bit_ref& bref) const;
        SRSASN_CODE unpack(cbit_ref& bref);
        void        to_json(json_writer& j) const;
        // getters
        rrc_conn_reest_request_minus5_gc_nb_r16_ies_s& rrc_conn_reest_request_r16()
        {
          assert_choice_type(types::rrc_conn_reest_request_r16, type_, "later");
          return c;
        }
        const rrc_conn_reest_request_minus5_gc_nb_r16_ies_s& rrc_conn_reest_request_r16() const
        {
          assert_choice_type(types::rrc_conn_reest_request_r16, type_, "later");
          return c;
        }
        rrc_conn_reest_request_minus5_gc_nb_r16_ies_s& set_rrc_conn_reest_request_r16();
        void                                           set_crit_exts_future();

      private:
        types                                         type_;
        rrc_conn_reest_request_minus5_gc_nb_r16_ies_s c;
      };
      struct types_opts {
        enum options { rrc_conn_reest_request_r14, later, nulltype } value;

        const char* to_string() const;
      };
      typedef enumerated<types_opts> types;

      // choice methods
      later_c_() = default;
      later_c_(const later_c_& other);
      later_c_& operator=(const later_c_& other);
      ~later_c_() { destroy_(); }
      void        set(types::options e = types::nulltype);
      types       type() const { return type_; }
      SRSASN_CODE pack(bit_ref& bref) const;
      SRSASN_CODE unpack(cbit_ref& bref);
      void        to_json(json_writer& j) const;
      // getters
      rrc_conn_reest_request_nb_r14_ies_s& rrc_conn_reest_request_r14()
      {
        assert_choice_type(types::rrc_conn_reest_request_r14, type_, "later");
        return c.get<rrc_conn_reest_request_nb_r14_ies_s>();
      }
      later_c__& later()
      {
        assert_choice_type(types::later, type_, "later");
        return c.get<later_c__>();
      }
      const rrc_conn_reest_request_nb_r14_ies_s& rrc_conn_reest_request_r14() const
      {
        assert_choice_type(types::rrc_conn_reest_request_r14, type_, "later");
        return c.get<rrc_conn_reest_request_nb_r14_ies_s>();
      }
      const later_c__& later() const
      {
        assert_choice_type(types::later, type_, "later");
        return c.get<later_c__>();
      }
      rrc_conn_reest_request_nb_r14_ies_s& set_rrc_conn_reest_request_r14();
      later_c__&                           set_later();

    private:
      types                                                           type_;
      choice_buffer_t<later_c__, rrc_conn_reest_request_nb_r14_ies_s> c;

      void destroy_();
    };
    struct types_opts {
      enum options { rrc_conn_reest_request_r13, later, nulltype } value;

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
    rrc_conn_reest_request_nb_r13_ies_s& rrc_conn_reest_request_r13()
    {
      assert_choice_type(types::rrc_conn_reest_request_r13, type_, "criticalExtensions");
      return c.get<rrc_conn_reest_request_nb_r13_ies_s>();
    }
    later_c_& later()
    {
      assert_choice_type(types::later, type_, "criticalExtensions");
      return c.get<later_c_>();
    }
    const rrc_conn_reest_request_nb_r13_ies_s& rrc_conn_reest_request_r13() const
    {
      assert_choice_type(types::rrc_conn_reest_request_r13, type_, "criticalExtensions");
      return c.get<rrc_conn_reest_request_nb_r13_ies_s>();
    }
    const later_c_& later() const
    {
      assert_choice_type(types::later, type_, "criticalExtensions");
      return c.get<later_c_>();
    }
    rrc_conn_reest_request_nb_r13_ies_s& set_rrc_conn_reest_request_r13();
    later_c_&                            set_later();

  private:
    types                                                          type_;
    choice_buffer_t<later_c_, rrc_conn_reest_request_nb_r13_ies_s> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRequest-5GC-NB-r16-IEs ::= SEQUENCE
struct rrc_conn_request_minus5_gc_nb_r16_ies_s {
  struct establishment_cause_r16_opts {
    enum options { mt_access, mo_sig, mo_data, mo_exception_data, spare4, spare3, spare2, spare1, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<establishment_cause_r16_opts> establishment_cause_r16_e_;

  // member variables
  init_ue_id_minus5_gc_nb_r16_c ue_id_r16;
  establishment_cause_r16_e_    establishment_cause_r16;
  cqi_npdcch_nb_r14_e           cqi_npdcch_r16;
  fixed_bitstring<11>           spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EstablishmentCause-NB-r13 ::= ENUMERATED
struct establishment_cause_nb_r13_opts {
  enum options {
    mt_access,
    mo_sig,
    mo_data,
    mo_exception_data,
    delay_tolerant_access_v1330,
    mt_edt_v1610,
    spare2,
    spare1,
    nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<establishment_cause_nb_r13_opts> establishment_cause_nb_r13_e;

// RRCConnectionRequest-NB-r13-IEs ::= SEQUENCE
struct rrc_conn_request_nb_r13_ies_s {
  bool                         multi_tone_support_r13_present    = false;
  bool                         multi_carrier_support_r13_present = false;
  init_ue_id_c                 ue_id_r13;
  establishment_cause_nb_r13_e establishment_cause_r13;
  bool                         early_contention_resolution_r14 = false;
  cqi_npdcch_nb_r14_e          cqi_npdcch_r14;
  fixed_bitstring<17>          spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionRequest-NB ::= SEQUENCE
struct rrc_conn_request_nb_s {
  struct crit_exts_c_ {
    struct later_c_ {
      struct types_opts {
        enum options { rrc_conn_request_r16, crit_exts_future, nulltype } value;

        const char* to_string() const;
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
      rrc_conn_request_minus5_gc_nb_r16_ies_s& rrc_conn_request_r16()
      {
        assert_choice_type(types::rrc_conn_request_r16, type_, "later");
        return c;
      }
      const rrc_conn_request_minus5_gc_nb_r16_ies_s& rrc_conn_request_r16() const
      {
        assert_choice_type(types::rrc_conn_request_r16, type_, "later");
        return c;
      }
      rrc_conn_request_minus5_gc_nb_r16_ies_s& set_rrc_conn_request_r16();
      void                                     set_crit_exts_future();

    private:
      types                                   type_;
      rrc_conn_request_minus5_gc_nb_r16_ies_s c;
    };
    struct types_opts {
      enum options { rrc_conn_request_r13, later, nulltype } value;

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
    rrc_conn_request_nb_r13_ies_s& rrc_conn_request_r13()
    {
      assert_choice_type(types::rrc_conn_request_r13, type_, "criticalExtensions");
      return c.get<rrc_conn_request_nb_r13_ies_s>();
    }
    later_c_& later()
    {
      assert_choice_type(types::later, type_, "criticalExtensions");
      return c.get<later_c_>();
    }
    const rrc_conn_request_nb_r13_ies_s& rrc_conn_request_r13() const
    {
      assert_choice_type(types::rrc_conn_request_r13, type_, "criticalExtensions");
      return c.get<rrc_conn_request_nb_r13_ies_s>();
    }
    const later_c_& later() const
    {
      assert_choice_type(types::later, type_, "criticalExtensions");
      return c.get<later_c_>();
    }
    rrc_conn_request_nb_r13_ies_s& set_rrc_conn_request_r13();
    later_c_&                      set_later();

  private:
    types                                                    type_;
    choice_buffer_t<later_c_, rrc_conn_request_nb_r13_ies_s> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResumeComplete-NB-v1710-IEs ::= SEQUENCE
struct rrc_conn_resume_complete_nb_v1710_ies_s {
  bool                    gnss_validity_dur_r17_present = false;
  bool                    non_crit_ext_present          = false;
  gnss_validity_dur_r17_e gnss_validity_dur_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResumeComplete-NB-v1610-IEs ::= SEQUENCE
struct rrc_conn_resume_complete_nb_v1610_ies_s {
  bool                                    rlf_info_available_r16_present = false;
  bool                                    anr_info_available_r16_present = false;
  bool                                    non_crit_ext_present           = false;
  rrc_conn_resume_complete_nb_v1710_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResumeComplete-NB-v1470-IEs ::= SEQUENCE
struct rrc_conn_resume_complete_nb_v1470_ies_s {
  bool                                    meas_result_serv_cell_r14_present = false;
  bool                                    non_crit_ext_present              = false;
  meas_result_serv_cell_nb_r14_s          meas_result_serv_cell_r14;
  rrc_conn_resume_complete_nb_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResumeComplete-NB-r13-IEs ::= SEQUENCE
struct rrc_conn_resume_complete_nb_r13_ies_s {
  bool                                    sel_plmn_id_r13_present   = false;
  bool                                    ded_info_nas_r13_present  = false;
  bool                                    late_non_crit_ext_present = false;
  bool                                    non_crit_ext_present      = false;
  uint8_t                                 sel_plmn_id_r13           = 1;
  dyn_octstring                           ded_info_nas_r13;
  dyn_octstring                           late_non_crit_ext;
  rrc_conn_resume_complete_nb_v1470_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResumeComplete-NB ::= SEQUENCE
struct rrc_conn_resume_complete_nb_s {
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
    rrc_conn_resume_complete_nb_r13_ies_s& rrc_conn_resume_complete_r13()
    {
      assert_choice_type(types::rrc_conn_resume_complete_r13, type_, "criticalExtensions");
      return c;
    }
    const rrc_conn_resume_complete_nb_r13_ies_s& rrc_conn_resume_complete_r13() const
    {
      assert_choice_type(types::rrc_conn_resume_complete_r13, type_, "criticalExtensions");
      return c;
    }
    rrc_conn_resume_complete_nb_r13_ies_s& set_rrc_conn_resume_complete_r13();
    void                                   set_crit_exts_future();

  private:
    types                                 type_;
    rrc_conn_resume_complete_nb_r13_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResumeRequest-5GC-NB-r16-IEs ::= SEQUENCE
struct rrc_conn_resume_request_minus5_gc_nb_r16_ies_s {
  fixed_bitstring<40>          resume_id_r16;
  fixed_bitstring<16>          short_resume_mac_i_r16;
  establishment_cause_nb_r13_e resume_cause_r16;
  cqi_npdcch_nb_r14_e          cqi_npdcch_r16;
  fixed_bitstring<4>           spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResumeRequest-NB-r13-IEs ::= SEQUENCE
struct rrc_conn_resume_request_nb_r13_ies_s {
  fixed_bitstring<40>          resume_id_r13;
  fixed_bitstring<16>          short_resume_mac_i_r13;
  establishment_cause_nb_r13_e resume_cause_r13;
  bool                         early_contention_resolution_r14 = false;
  cqi_npdcch_nb_r14_e          cqi_npdcch_r14;
  bool                         anr_info_available_r16 = false;
  fixed_bitstring<3>           spare;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionResumeRequest-NB ::= SEQUENCE
struct rrc_conn_resume_request_nb_s {
  struct crit_exts_c_ {
    struct later_c_ {
      struct types_opts {
        enum options { rrc_conn_resume_request_r16, crit_exts_future, nulltype } value;

        const char* to_string() const;
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
      rrc_conn_resume_request_minus5_gc_nb_r16_ies_s& rrc_conn_resume_request_r16()
      {
        assert_choice_type(types::rrc_conn_resume_request_r16, type_, "later");
        return c;
      }
      const rrc_conn_resume_request_minus5_gc_nb_r16_ies_s& rrc_conn_resume_request_r16() const
      {
        assert_choice_type(types::rrc_conn_resume_request_r16, type_, "later");
        return c;
      }
      rrc_conn_resume_request_minus5_gc_nb_r16_ies_s& set_rrc_conn_resume_request_r16();
      void                                            set_crit_exts_future();

    private:
      types                                          type_;
      rrc_conn_resume_request_minus5_gc_nb_r16_ies_s c;
    };
    struct types_opts {
      enum options { rrc_conn_resume_request_r13, later, nulltype } value;

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
    rrc_conn_resume_request_nb_r13_ies_s& rrc_conn_resume_request_r13()
    {
      assert_choice_type(types::rrc_conn_resume_request_r13, type_, "criticalExtensions");
      return c.get<rrc_conn_resume_request_nb_r13_ies_s>();
    }
    later_c_& later()
    {
      assert_choice_type(types::later, type_, "criticalExtensions");
      return c.get<later_c_>();
    }
    const rrc_conn_resume_request_nb_r13_ies_s& rrc_conn_resume_request_r13() const
    {
      assert_choice_type(types::rrc_conn_resume_request_r13, type_, "criticalExtensions");
      return c.get<rrc_conn_resume_request_nb_r13_ies_s>();
    }
    const later_c_& later() const
    {
      assert_choice_type(types::later, type_, "criticalExtensions");
      return c.get<later_c_>();
    }
    rrc_conn_resume_request_nb_r13_ies_s& set_rrc_conn_resume_request_r13();
    later_c_&                             set_later();

  private:
    types                                                           type_;
    choice_buffer_t<later_c_, rrc_conn_resume_request_nb_r13_ies_s> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-NB-v1710-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_nb_v1710_ies_s {
  bool                    gnss_validity_dur_r17_present = false;
  bool                    non_crit_ext_present          = false;
  gnss_validity_dur_r17_e gnss_validity_dur_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-NB-v1610-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_nb_v1610_ies_s {
  struct guami_type_r16_opts {
    enum options { native, mapped, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<guami_type_r16_opts> guami_type_r16_e_;
  using s_nssai_list_r16_l_ = dyn_array<s_nssai_r15_c>;

  // member variables
  bool                                   ng_minus5_g_s_tmsi_r16_present       = false;
  bool                                   registered_amf_r16_present           = false;
  bool                                   gummei_type_v1610_present            = false;
  bool                                   guami_type_r16_present               = false;
  bool                                   s_nssai_list_r16_present             = false;
  bool                                   ng_u_data_transfer_r16_present       = false;
  bool                                   up_cio_t_minus5_gs_optim_r16_present = false;
  bool                                   rlf_info_available_r16_present       = false;
  bool                                   anr_info_available_r16_present       = false;
  bool                                   pur_cfg_id_r16_present               = false;
  bool                                   non_crit_ext_present                 = false;
  fixed_bitstring<48>                    ng_minus5_g_s_tmsi_r16;
  registered_amf_r15_s                   registered_amf_r16;
  guami_type_r16_e_                      guami_type_r16;
  s_nssai_list_r16_l_                    s_nssai_list_r16;
  fixed_bitstring<20>                    pur_cfg_id_r16;
  rrc_conn_setup_complete_nb_v1710_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-NB-v1470-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_nb_v1470_ies_s {
  bool                                   meas_result_serv_cell_r14_present = false;
  bool                                   non_crit_ext_present              = false;
  meas_result_serv_cell_nb_r14_s         meas_result_serv_cell_r14;
  rrc_conn_setup_complete_nb_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-NB-v1430-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_nb_v1430_ies_s {
  bool                                   gummei_type_r14_present = false;
  bool                                   dcn_id_r14_present      = false;
  bool                                   non_crit_ext_present    = false;
  uint32_t                               dcn_id_r14              = 0;
  rrc_conn_setup_complete_nb_v1470_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-NB-r13-IEs ::= SEQUENCE
struct rrc_conn_setup_complete_nb_r13_ies_s {
  bool                                   s_tmsi_r13_present                     = false;
  bool                                   registered_mme_r13_present             = false;
  bool                                   attach_without_pdn_connect_r13_present = false;
  bool                                   up_cio_t_eps_optim_r13_present         = false;
  bool                                   late_non_crit_ext_present              = false;
  bool                                   non_crit_ext_present                   = false;
  uint8_t                                sel_plmn_id_r13                        = 1;
  s_tmsi_s                               s_tmsi_r13;
  registered_mme_s                       registered_mme_r13;
  dyn_octstring                          ded_info_nas_r13;
  dyn_octstring                          late_non_crit_ext;
  rrc_conn_setup_complete_nb_v1430_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetupComplete-NB ::= SEQUENCE
struct rrc_conn_setup_complete_nb_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_conn_setup_complete_r13, crit_exts_future, nulltype } value;

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
    rrc_conn_setup_complete_nb_r13_ies_s& rrc_conn_setup_complete_r13()
    {
      assert_choice_type(types::rrc_conn_setup_complete_r13, type_, "criticalExtensions");
      return c;
    }
    const rrc_conn_setup_complete_nb_r13_ies_s& rrc_conn_setup_complete_r13() const
    {
      assert_choice_type(types::rrc_conn_setup_complete_r13, type_, "criticalExtensions");
      return c;
    }
    rrc_conn_setup_complete_nb_r13_ies_s& set_rrc_conn_setup_complete_r13();
    void                                  set_crit_exts_future();

  private:
    types                                type_;
    rrc_conn_setup_complete_nb_r13_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCEarlyDataRequest-5GC-NB-r16-IEs ::= SEQUENCE
struct rrc_early_data_request_minus5_gc_nb_r16_ies_s {
  struct establishment_cause_r16_opts {
    enum options { mo_data, mo_exception_data, mt_access, spare1, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<establishment_cause_r16_opts> establishment_cause_r16_e_;

  // member variables
  bool                       cqi_npdcch_r16_present    = false;
  bool                       late_non_crit_ext_present = false;
  bool                       non_crit_ext_present      = false;
  fixed_bitstring<48>        ng_minus5_g_s_tmsi_r16;
  establishment_cause_r16_e_ establishment_cause_r16;
  cqi_npdcch_nb_r14_e        cqi_npdcch_r16;
  dyn_octstring              ded_info_nas_r16;
  dyn_octstring              late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCEarlyDataRequest-NB-v1590-IEs ::= SEQUENCE
struct rrc_early_data_request_nb_v1590_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCEarlyDataRequest-NB-r15-IEs ::= SEQUENCE
struct rrc_early_data_request_nb_r15_ies_s {
  struct establishment_cause_r15_opts {
    enum options { mo_data, mo_exception_data, delay_tolerant_access, mt_access_v1610, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<establishment_cause_r15_opts> establishment_cause_r15_e_;

  // member variables
  bool                                  cqi_npdcch_r15_present = false;
  bool                                  non_crit_ext_present   = false;
  s_tmsi_s                              s_tmsi_r15;
  establishment_cause_r15_e_            establishment_cause_r15;
  cqi_npdcch_nb_r14_e                   cqi_npdcch_r15;
  dyn_octstring                         ded_info_nas_r15;
  rrc_early_data_request_nb_v1590_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCEarlyDataRequest-NB-r15 ::= SEQUENCE
struct rrc_early_data_request_nb_r15_s {
  struct crit_exts_c_ {
    struct later_c_ {
      struct types_opts {
        enum options { rrc_early_data_request_r16, crit_exts_future, nulltype } value;

        const char* to_string() const;
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
      rrc_early_data_request_minus5_gc_nb_r16_ies_s& rrc_early_data_request_r16()
      {
        assert_choice_type(types::rrc_early_data_request_r16, type_, "later");
        return c;
      }
      const rrc_early_data_request_minus5_gc_nb_r16_ies_s& rrc_early_data_request_r16() const
      {
        assert_choice_type(types::rrc_early_data_request_r16, type_, "later");
        return c;
      }
      rrc_early_data_request_minus5_gc_nb_r16_ies_s& set_rrc_early_data_request_r16();
      void                                           set_crit_exts_future();

    private:
      types                                         type_;
      rrc_early_data_request_minus5_gc_nb_r16_ies_s c;
    };
    struct types_opts {
      enum options { rrc_early_data_request_r15, later, nulltype } value;

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
    rrc_early_data_request_nb_r15_ies_s& rrc_early_data_request_r15()
    {
      assert_choice_type(types::rrc_early_data_request_r15, type_, "criticalExtensions");
      return c.get<rrc_early_data_request_nb_r15_ies_s>();
    }
    later_c_& later()
    {
      assert_choice_type(types::later, type_, "criticalExtensions");
      return c.get<later_c_>();
    }
    const rrc_early_data_request_nb_r15_ies_s& rrc_early_data_request_r15() const
    {
      assert_choice_type(types::rrc_early_data_request_r15, type_, "criticalExtensions");
      return c.get<rrc_early_data_request_nb_r15_ies_s>();
    }
    const later_c_& later() const
    {
      assert_choice_type(types::later, type_, "criticalExtensions");
      return c.get<later_c_>();
    }
    rrc_early_data_request_nb_r15_ies_s& set_rrc_early_data_request_r15();
    later_c_&                            set_later();

  private:
    types                                                          type_;
    choice_buffer_t<later_c_, rrc_early_data_request_nb_r15_ies_s> c;

    void destroy_();
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SC-MTCH-SchedulingInfo-NB-r14 ::= SEQUENCE
struct sc_mtch_sched_info_nb_r14_s {
  struct on_dur_timer_scptm_r14_opts {
    enum options { pp1, pp2, pp3, pp4, pp8, pp16, pp32, spare, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<on_dur_timer_scptm_r14_opts> on_dur_timer_scptm_r14_e_;
  struct drx_inactivity_timer_scptm_r14_opts {
    enum options { pp0, pp1, pp2, pp3, pp4, pp8, pp16, pp32, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
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

// SC-MTCH-Info-NB-r14 ::= SEQUENCE
struct sc_mtch_info_nb_r14_s {
  struct sc_mtch_carrier_cfg_r14_c_ {
    struct types_opts {
      enum options { dl_carrier_cfg_r14, dl_carrier_idx_r14, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    sc_mtch_carrier_cfg_r14_c_() = default;
    sc_mtch_carrier_cfg_r14_c_(const sc_mtch_carrier_cfg_r14_c_& other);
    sc_mtch_carrier_cfg_r14_c_& operator=(const sc_mtch_carrier_cfg_r14_c_& other);
    ~sc_mtch_carrier_cfg_r14_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    dl_carrier_cfg_common_nb_r14_s& dl_carrier_cfg_r14()
    {
      assert_choice_type(types::dl_carrier_cfg_r14, type_, "sc-mtch-CarrierConfig-r14");
      return c.get<dl_carrier_cfg_common_nb_r14_s>();
    }
    uint8_t& dl_carrier_idx_r14()
    {
      assert_choice_type(types::dl_carrier_idx_r14, type_, "sc-mtch-CarrierConfig-r14");
      return c.get<uint8_t>();
    }
    const dl_carrier_cfg_common_nb_r14_s& dl_carrier_cfg_r14() const
    {
      assert_choice_type(types::dl_carrier_cfg_r14, type_, "sc-mtch-CarrierConfig-r14");
      return c.get<dl_carrier_cfg_common_nb_r14_s>();
    }
    const uint8_t& dl_carrier_idx_r14() const
    {
      assert_choice_type(types::dl_carrier_idx_r14, type_, "sc-mtch-CarrierConfig-r14");
      return c.get<uint8_t>();
    }
    dl_carrier_cfg_common_nb_r14_s& set_dl_carrier_cfg_r14();
    uint8_t&                        set_dl_carrier_idx_r14();

  private:
    types                                           type_;
    choice_buffer_t<dl_carrier_cfg_common_nb_r14_s> c;

    void destroy_();
  };
  struct npdcch_npdsch_max_tbs_sc_mtch_r14_opts {
    enum options { n680, n2536, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<npdcch_npdsch_max_tbs_sc_mtch_r14_opts> npdcch_npdsch_max_tbs_sc_mtch_r14_e_;
  struct npdcch_num_repeats_sc_mtch_r14_opts {
    enum options {
      r1,
      r2,
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
  typedef enumerated<npdcch_num_repeats_sc_mtch_r14_opts> npdcch_num_repeats_sc_mtch_r14_e_;
  struct npdcch_start_sf_sc_mtch_r14_opts {
    enum options { v1dot5, v2, v4, v8, v16, v32, v48, v64, nulltype } value;
    typedef float number_type;

    const char* to_string() const;
    float       to_number() const;
    const char* to_number_string() const;
  };
  typedef enumerated<npdcch_start_sf_sc_mtch_r14_opts> npdcch_start_sf_sc_mtch_r14_e_;
  struct npdcch_offset_sc_mtch_r14_opts {
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
  typedef enumerated<npdcch_offset_sc_mtch_r14_opts> npdcch_offset_sc_mtch_r14_e_;

  // member variables
  bool                                 ext                                = false;
  bool                                 sc_mtch_sched_info_r14_present     = false;
  bool                                 sc_mtch_neighbour_cell_r14_present = false;
  sc_mtch_carrier_cfg_r14_c_           sc_mtch_carrier_cfg_r14;
  mbms_session_info_r13_s              mbms_session_info_r14;
  fixed_bitstring<16>                  g_rnti_r14;
  sc_mtch_sched_info_nb_r14_s          sc_mtch_sched_info_r14;
  fixed_bitstring<8>                   sc_mtch_neighbour_cell_r14;
  npdcch_npdsch_max_tbs_sc_mtch_r14_e_ npdcch_npdsch_max_tbs_sc_mtch_r14;
  npdcch_num_repeats_sc_mtch_r14_e_    npdcch_num_repeats_sc_mtch_r14;
  npdcch_start_sf_sc_mtch_r14_e_       npdcch_start_sf_sc_mtch_r14;
  npdcch_offset_sc_mtch_r14_e_         npdcch_offset_sc_mtch_r14;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SC-MTCH-InfoList-NB-r14 ::= SEQUENCE (SIZE (0..64)) OF SC-MTCH-Info-NB-r14
using sc_mtch_info_list_nb_r14_l = dyn_array<sc_mtch_info_nb_r14_s>;

// SCPTM-NeighbourCellList-NB-r14 ::= SEQUENCE (SIZE (1..8)) OF PCI-ARFCN-NB-r14
using scptm_neighbour_cell_list_nb_r14_l = dyn_array<pci_arfcn_nb_r14_s>;

// SCPTMConfiguration-NB-v1610 ::= SEQUENCE
struct scptm_cfg_nb_v1610_s {
  struct multi_tb_gap_r16_opts {
    enum options { sf16, sf32, sf64, sf128, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<multi_tb_gap_r16_opts> multi_tb_gap_r16_e_;

  // member variables
  bool                       multi_tb_gap_r16_present = false;
  bool                       non_crit_ext_present     = false;
  sc_mtch_info_list_nb_r14_l sc_mtch_info_list_multi_tb_r16;
  multi_tb_gap_r16_e_        multi_tb_gap_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SCPTMConfiguration-NB-r14 ::= SEQUENCE
struct scptm_cfg_nb_r14_s {
  bool                               scptm_neighbour_cell_list_r14_present = false;
  bool                               late_non_crit_ext_present             = false;
  bool                               non_crit_ext_present                  = false;
  sc_mtch_info_list_nb_r14_l         sc_mtch_info_list_r14;
  scptm_neighbour_cell_list_nb_r14_l scptm_neighbour_cell_list_r14;
  dyn_octstring                      late_non_crit_ext;
  scptm_cfg_nb_v1610_s               non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SC-MCCH-MessageType-NB ::= CHOICE
struct sc_mcch_msg_type_nb_c {
  struct c1_c_ {
    struct types_opts {
      enum options { scptm_cfg_r14, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::scptm_cfg_r14; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    scptm_cfg_nb_r14_s&       scptm_cfg_r14() { return c; }
    const scptm_cfg_nb_r14_s& scptm_cfg_r14() const { return c; }

  private:
    scptm_cfg_nb_r14_s c;
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  sc_mcch_msg_type_nb_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "SC-MCCH-MessageType-NB");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "SC-MCCH-MessageType-NB");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// SC-MCCH-Message-NB ::= SEQUENCE
struct sc_mcch_msg_nb_s {
  sc_mcch_msg_type_nb_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-NB-v1430 ::= SEQUENCE
struct phy_layer_params_nb_v1430_s {
  bool multi_carrier_nprach_r14_present = false;
  bool two_harq_processes_r14_present   = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-NB-v1530 ::= SEQUENCE
struct phy_layer_params_nb_v1530_s {
  bool mixed_operation_mode_r15_present           = false;
  bool sr_with_harq_ack_r15_present               = false;
  bool sr_without_harq_ack_r15_present            = false;
  bool nprach_format2_r15_present                 = false;
  bool add_tx_sib1_r15_present                    = false;
  bool npusch_minus3dot75k_hz_scs_tdd_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UE-Capability-NB-r15 ::= SEQUENCE
struct tdd_ue_cap_nb_r15_s {
  bool                        ext                                = false;
  bool                        ue_category_nb_r15_present         = false;
  bool                        phy_layer_params_rel13_r15_present = false;
  bool                        phy_layer_params_rel14_r15_present = false;
  bool                        phy_layer_params_v1530_present     = false;
  phy_layer_params_nb_r13_s   phy_layer_params_rel13_r15;
  phy_layer_params_nb_v1430_s phy_layer_params_rel14_r15;
  phy_layer_params_nb_v1530_s phy_layer_params_v1530;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-NB-v1700 ::= SEQUENCE
struct phy_layer_params_nb_v1700_s {
  bool npdsch_minus16_qam_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UE-Capability-NB-v1710 ::= SEQUENCE
struct tdd_ue_cap_nb_v1710_s {
  bool                        phy_layer_params_v1710_present = false;
  phy_layer_params_nb_v1700_s phy_layer_params_v1710;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NTN-Parameters-NB-v1720 ::= SEQUENCE
struct ntn_params_nb_v1720_s {
  struct ntn_segmented_precompensation_gaps_r17_opts {
    enum options { sym1, sl1, sl2, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<ntn_segmented_precompensation_gaps_r17_opts> ntn_segmented_precompensation_gaps_r17_e_;

  // member variables
  bool                                      ntn_segmented_precompensation_gaps_r17_present = false;
  ntn_segmented_precompensation_gaps_r17_e_ ntn_segmented_precompensation_gaps_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasParameters-NB-v1710 ::= SEQUENCE
struct meas_params_nb_v1710_s {
  bool conn_mode_meas_intra_freq_r17_present = false;
  bool conn_mode_meas_inter_freq_r17_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-Capability-NB-v1720-IEs ::= SEQUENCE
struct ue_cap_nb_v1720_ies_s {
  bool                  non_crit_ext_present = false;
  ntn_params_nb_v1720_s ntn_params_v1720;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NTN-Parameters-NB-r17 ::= SEQUENCE
struct ntn_params_nb_r17_s {
  struct ntn_scenario_support_r17_opts {
    enum options { ngso, gso, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<ntn_scenario_support_r17_opts> ntn_scenario_support_r17_e_;

  // member variables
  bool                        ntn_connect_epc_r17_present       = false;
  bool                        ntn_ta_report_r17_present         = false;
  bool                        ntn_pur_timer_delay_r17_present   = false;
  bool                        ntn_offset_timing_enh_r17_present = false;
  bool                        ntn_scenario_support_r17_present  = false;
  ntn_scenario_support_r17_e_ ntn_scenario_support_r17;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-Capability-NB-v1710-IEs ::= SEQUENCE
struct ue_cap_nb_v1710_ies_s {
  bool                   meas_params_v1710_present = false;
  bool                   non_crit_ext_present      = false;
  meas_params_nb_v1710_s meas_params_v1710;
  rf_params_nb_v1710_s   rf_params_v1710;
  tdd_ue_cap_nb_v1710_s  tdd_ue_cap_v1710;
  ue_cap_nb_v1720_ies_s  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-Capability-NB-v1700-IEs ::= SEQUENCE
struct ue_cap_nb_v1700_ies_s {
  bool                        coverage_based_paging_r17_present = false;
  bool                        ntn_params_r17_present            = false;
  bool                        non_crit_ext_present              = false;
  phy_layer_params_nb_v1700_s phy_layer_params_v1700;
  ntn_params_nb_r17_s         ntn_params_r17;
  ue_cap_nb_v1710_ies_s       non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-Parameters-NB-v1610 ::= SEQUENCE
struct mac_params_nb_v1610_s {
  bool rai_support_enh_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasParameters-NB-r16 ::= SEQUENCE
struct meas_params_nb_r16_s {
  bool dl_ch_quality_report_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PUR-Parameters-NB-r16 ::= SEQUENCE
struct pur_params_nb_r16_s {
  bool pur_cp_epc_r16_present           = false;
  bool pur_cp_minus5_gc_r16_present     = false;
  bool pur_up_epc_r16_present           = false;
  bool pur_up_minus5_gc_r16_present     = false;
  bool pur_nrsrp_validation_r16_present = false;
  bool pur_cp_l1_ack_r16_present        = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-NB-v1610 ::= SEQUENCE
struct phy_layer_params_nb_v1610_s {
  bool npdsch_multi_tb_r16_present              = false;
  bool npdsch_multi_tb_interleaving_r16_present = false;
  bool npusch_multi_tb_r16_present              = false;
  bool npusch_multi_tb_interleaving_r16_present = false;
  bool multi_tb_harq_ack_bundling_r16_present   = false;
  bool slot_symbol_res_resv_dl_r16_present      = false;
  bool slot_symbol_res_resv_ul_r16_present      = false;
  bool sf_res_resv_dl_r16_present               = false;
  bool sf_res_resv_ul_r16_present               = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SON-Parameters-NB-r16 ::= SEQUENCE
struct son_params_nb_r16_s {
  bool anr_report_r16_present  = false;
  bool rach_report_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TDD-UE-Capability-NB-v1610 ::= SEQUENCE
struct tdd_ue_cap_nb_v1610_s {
  bool slot_symbol_res_resv_dl_r16_present = false;
  bool slot_symbol_res_resv_ul_r16_present = false;
  bool sf_res_resv_dl_r16_present          = false;
  bool sf_res_resv_ul_r16_present          = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-Capability-NB-v16x0-IEs ::= SEQUENCE
struct ue_cap_nb_v16x0_ies_s {
  bool                  late_non_crit_ext_present = false;
  bool                  non_crit_ext_present      = false;
  dyn_octstring         late_non_crit_ext;
  ue_cap_nb_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-Capability-NB-v1610-IEs ::= SEQUENCE
struct ue_cap_nb_v1610_ies_s {
  bool                        early_security_reactivation_r16_present = false;
  bool                        early_data_up_minus5_gc_r16_present     = false;
  bool                        pur_params_r16_present                  = false;
  bool                        phy_layer_params_v1610_present          = false;
  bool                        son_params_r16_present                  = false;
  bool                        tdd_ue_cap_v1610_present                = false;
  bool                        non_crit_ext_present                    = false;
  pur_params_nb_r16_s         pur_params_r16;
  mac_params_nb_v1610_s       mac_params_v1610;
  phy_layer_params_nb_v1610_s phy_layer_params_v1610;
  son_params_nb_r16_s         son_params_r16;
  meas_params_nb_r16_s        meas_params_r16;
  tdd_ue_cap_nb_v1610_s       tdd_ue_cap_v1610;
  ue_cap_nb_v16x0_ies_s       non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-Parameters-NB-v1530 ::= SEQUENCE
struct mac_params_nb_v1530_s {
  bool sr_sps_bsr_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RLC-Parameters-NB-r15 ::= SEQUENCE
struct rlc_params_nb_r15_s {
  bool rlc_um_r15_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-Capability-NB-v15x0-IEs ::= SEQUENCE
struct ue_cap_nb_v15x0_ies_s {
  bool                  late_non_crit_ext_present = false;
  bool                  non_crit_ext_present      = false;
  dyn_octstring         late_non_crit_ext;
  ue_cap_nb_v1610_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-Capability-NB-v1530-IEs ::= SEQUENCE
struct ue_cap_nb_v1530_ies_s {
  bool                        early_data_up_r15_present      = false;
  bool                        phy_layer_params_v1530_present = false;
  bool                        tdd_ue_cap_r15_present         = false;
  bool                        non_crit_ext_present           = false;
  rlc_params_nb_r15_s         rlc_params_r15;
  mac_params_nb_v1530_s       mac_params_v1530;
  phy_layer_params_nb_v1530_s phy_layer_params_v1530;
  tdd_ue_cap_nb_r15_s         tdd_ue_cap_r15;
  ue_cap_nb_v15x0_ies_s       non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PhyLayerParameters-NB-v1440 ::= SEQUENCE
struct phy_layer_params_nb_v1440_s {
  bool interference_randomisation_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-Capability-NB-v14x0-IEs ::= SEQUENCE
struct ue_cap_nb_v14x0_ies_s {
  bool                  late_non_crit_ext_present = false;
  bool                  non_crit_ext_present      = false;
  dyn_octstring         late_non_crit_ext;
  ue_cap_nb_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MAC-Parameters-NB-r14 ::= SEQUENCE
struct mac_params_nb_r14_s {
  bool data_inact_mon_r14_present = false;
  bool rai_support_r14_present    = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RF-Parameters-NB-v1430 ::= SEQUENCE
struct rf_params_nb_v1430_s {
  bool pwr_class_nb_minus14dbm_r14_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-Capability-NB-v1440-IEs ::= SEQUENCE
struct ue_cap_nb_v1440_ies_s {
  bool                        phy_layer_params_v1440_present = false;
  bool                        non_crit_ext_present           = false;
  phy_layer_params_nb_v1440_s phy_layer_params_v1440;
  ue_cap_nb_v14x0_ies_s       non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-Capability-NB-Ext-r14-IEs ::= SEQUENCE
struct ue_cap_nb_ext_r14_ies_s {
  bool                        ue_category_nb_r14_present     = false;
  bool                        mac_params_r14_present         = false;
  bool                        phy_layer_params_v1430_present = false;
  bool                        non_crit_ext_present           = false;
  mac_params_nb_r14_s         mac_params_r14;
  phy_layer_params_nb_v1430_s phy_layer_params_v1430;
  rf_params_nb_v1430_s        rf_params_v1430;
  ue_cap_nb_v1440_ies_s       non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UE-RadioPagingInfo-NB-r13 ::= SEQUENCE
struct ue_radio_paging_info_nb_r13_s {
  struct wake_up_signal_min_gap_e_drx_r15_opts {
    enum options { ms40, ms240, ms1000, ms2000, nulltype } value;
    typedef uint16_t number_type;

    const char* to_string() const;
    uint16_t    to_number() const;
  };
  typedef enumerated<wake_up_signal_min_gap_e_drx_r15_opts> wake_up_signal_min_gap_e_drx_r15_e_;

  // member variables
  bool ext                        = false;
  bool ue_category_nb_r13_present = false;
  // ...
  // group 0
  bool multi_carrier_paging_r14_present = false;
  // group 1
  bool                                mixed_operation_mode_r15_present         = false;
  bool                                wake_up_signal_r15_present               = false;
  bool                                wake_up_signal_min_gap_e_drx_r15_present = false;
  bool                                multi_carrier_paging_tdd_r15_present     = false;
  wake_up_signal_min_gap_e_drx_r15_e_ wake_up_signal_min_gap_e_drx_r15;
  // group 2
  bool ue_category_nb_r16_present                   = false;
  bool group_wake_up_signal_r16_present             = false;
  bool group_wake_up_signal_alternation_r16_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityInformation-NB-Ext-r14-IEs ::= SEQUENCE
struct ue_cap_info_nb_ext_r14_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring ue_cap_container_ext_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityInformation-NB-r13-IEs ::= SEQUENCE
struct ue_cap_info_nb_r13_ies_s {
  bool                          late_non_crit_ext_present = false;
  bool                          non_crit_ext_present      = false;
  ue_cap_nb_r13_s               ue_cap_r13;
  ue_radio_paging_info_nb_r13_s ue_radio_paging_info_r13;
  dyn_octstring                 late_non_crit_ext;
  ue_cap_info_nb_ext_r14_ies_s  non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UECapabilityInformation-NB ::= SEQUENCE
struct ue_cap_info_nb_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_cap_info_r13, crit_exts_future, nulltype } value;

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
    ue_cap_info_nb_r13_ies_s& ue_cap_info_r13()
    {
      assert_choice_type(types::ue_cap_info_r13, type_, "criticalExtensions");
      return c;
    }
    const ue_cap_info_nb_r13_ies_s& ue_cap_info_r13() const
    {
      assert_choice_type(types::ue_cap_info_r13, type_, "criticalExtensions");
      return c;
    }
    ue_cap_info_nb_r13_ies_s& set_ue_cap_info_r13();
    void                      set_crit_exts_future();

  private:
    types                    type_;
    ue_cap_info_nb_r13_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RACH-Report-NB-r16 ::= SEQUENCE
struct rach_report_nb_r16_s {
  uint8_t nof_preambs_sent_r16    = 1;
  bool    contention_detected_r16 = false;
  uint8_t init_nrsrp_level_r16    = 0;
  bool    edt_fallback_r16        = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationResponse-NB-r16-IEs ::= SEQUENCE
struct ue_info_resp_nb_r16_ies_s {
  bool                     rach_report_r16_present     = false;
  bool                     rlf_report_r16_present      = false;
  bool                     anr_meas_report_r16_present = false;
  bool                     late_non_crit_ext_present   = false;
  bool                     non_crit_ext_present        = false;
  rach_report_nb_r16_s     rach_report_r16;
  rlf_report_nb_r16_s      rlf_report_r16;
  anr_meas_report_nb_r16_s anr_meas_report_r16;
  dyn_octstring            late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEInformationResponse-NB-r16 ::= SEQUENCE
struct ue_info_resp_nb_r16_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ue_info_resp_r16, crit_exts_future, nulltype } value;

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
    ue_info_resp_nb_r16_ies_s& ue_info_resp_r16()
    {
      assert_choice_type(types::ue_info_resp_r16, type_, "criticalExtensions");
      return c;
    }
    const ue_info_resp_nb_r16_ies_s& ue_info_resp_r16() const
    {
      assert_choice_type(types::ue_info_resp_r16, type_, "criticalExtensions");
      return c;
    }
    ue_info_resp_nb_r16_ies_s& set_ue_info_resp_r16();
    void                       set_crit_exts_future();

  private:
    types                     type_;
    ue_info_resp_nb_r16_ies_s c;
  };

  // member variables
  uint8_t      rrc_transaction_id = 0;
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEPagingCoverageInformation-NB-v1700-IEs ::= SEQUENCE
struct ue_paging_coverage_info_nb_v1700_ies_s {
  bool    cbp_idx_r17_present  = false;
  bool    non_crit_ext_present = false;
  uint8_t cbp_idx_r17          = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEPagingCoverageInformation-NB-IEs ::= SEQUENCE
struct ue_paging_coverage_info_nb_ies_s {
  bool                                   npdcch_num_repeat_paging_r13_present = false;
  bool                                   non_crit_ext_present                 = false;
  uint16_t                               npdcch_num_repeat_paging_r13         = 1;
  ue_paging_coverage_info_nb_v1700_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEPagingCoverageInformation-NB ::= SEQUENCE
struct ue_paging_coverage_info_nb_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ue_paging_coverage_info_r13, spare3, spare2, spare1, nulltype } value;

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
      ue_paging_coverage_info_nb_ies_s& ue_paging_coverage_info_r13()
      {
        assert_choice_type(types::ue_paging_coverage_info_r13, type_, "c1");
        return c;
      }
      const ue_paging_coverage_info_nb_ies_s& ue_paging_coverage_info_r13() const
      {
        assert_choice_type(types::ue_paging_coverage_info_r13, type_, "c1");
        return c;
      }
      ue_paging_coverage_info_nb_ies_s& set_ue_paging_coverage_info_r13();
      void                              set_spare3();
      void                              set_spare2();
      void                              set_spare1();

    private:
      types                            type_;
      ue_paging_coverage_info_nb_ies_s c;
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

// UERadioAccessCapabilityInformation-NB-r14-IEs ::= SEQUENCE
struct ue_radio_access_cap_info_nb_r14_ies_s {
  bool          ue_radio_access_cap_info_r14_present = false;
  bool          non_crit_ext_present                 = false;
  dyn_octstring ue_radio_access_cap_info_r14;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioAccessCapabilityInformation-NB-v1380-IEs ::= SEQUENCE
struct ue_radio_access_cap_info_nb_v1380_ies_s {
  bool                                  late_non_crit_ext_present = false;
  bool                                  non_crit_ext_present      = false;
  dyn_octstring                         late_non_crit_ext;
  ue_radio_access_cap_info_nb_r14_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioAccessCapabilityInformation-NB-IEs ::= SEQUENCE
struct ue_radio_access_cap_info_nb_ies_s {
  bool                                    non_crit_ext_present = false;
  dyn_octstring                           ue_radio_access_cap_info_r13;
  ue_radio_access_cap_info_nb_v1380_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioAccessCapabilityInformation-NB ::= SEQUENCE
struct ue_radio_access_cap_info_nb_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ue_radio_access_cap_info_r13, spare3, spare2, spare1, nulltype } value;

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
      ue_radio_access_cap_info_nb_ies_s& ue_radio_access_cap_info_r13()
      {
        assert_choice_type(types::ue_radio_access_cap_info_r13, type_, "c1");
        return c;
      }
      const ue_radio_access_cap_info_nb_ies_s& ue_radio_access_cap_info_r13() const
      {
        assert_choice_type(types::ue_radio_access_cap_info_r13, type_, "c1");
        return c;
      }
      ue_radio_access_cap_info_nb_ies_s& set_ue_radio_access_cap_info_r13();
      void                               set_spare3();
      void                               set_spare2();
      void                               set_spare1();

    private:
      types                             type_;
      ue_radio_access_cap_info_nb_ies_s c;
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

// UERadioPagingInformation-NB-IEs ::= SEQUENCE
struct ue_radio_paging_info_nb_ies_s {
  bool          non_crit_ext_present = false;
  dyn_octstring ue_radio_paging_info_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioPagingInformation-NB ::= SEQUENCE
struct ue_radio_paging_info_nb_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { ue_radio_paging_info_r13, spare3, spare2, spare1, nulltype } value;

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
      ue_radio_paging_info_nb_ies_s& ue_radio_paging_info_r13()
      {
        assert_choice_type(types::ue_radio_paging_info_r13, type_, "c1");
        return c;
      }
      const ue_radio_paging_info_nb_ies_s& ue_radio_paging_info_r13() const
      {
        assert_choice_type(types::ue_radio_paging_info_r13, type_, "c1");
        return c;
      }
      ue_radio_paging_info_nb_ies_s& set_ue_radio_paging_info_r13();
      void                           set_spare3();
      void                           set_spare2();
      void                           set_spare1();

    private:
      types                         type_;
      ue_radio_paging_info_nb_ies_s c;
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

// UL-CCCH-MessageType-NB ::= CHOICE
struct ul_ccch_msg_type_nb_c {
  struct c1_c_ {
    struct types_opts {
      enum options {
        rrc_conn_reest_request_r13,
        rrc_conn_request_r13,
        rrc_conn_resume_request_r13,
        rrc_early_data_request_r15,
        nulltype
      } value;

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
    rrc_conn_reest_request_nb_s& rrc_conn_reest_request_r13()
    {
      assert_choice_type(types::rrc_conn_reest_request_r13, type_, "c1");
      return c.get<rrc_conn_reest_request_nb_s>();
    }
    rrc_conn_request_nb_s& rrc_conn_request_r13()
    {
      assert_choice_type(types::rrc_conn_request_r13, type_, "c1");
      return c.get<rrc_conn_request_nb_s>();
    }
    rrc_conn_resume_request_nb_s& rrc_conn_resume_request_r13()
    {
      assert_choice_type(types::rrc_conn_resume_request_r13, type_, "c1");
      return c.get<rrc_conn_resume_request_nb_s>();
    }
    rrc_early_data_request_nb_r15_s& rrc_early_data_request_r15()
    {
      assert_choice_type(types::rrc_early_data_request_r15, type_, "c1");
      return c.get<rrc_early_data_request_nb_r15_s>();
    }
    const rrc_conn_reest_request_nb_s& rrc_conn_reest_request_r13() const
    {
      assert_choice_type(types::rrc_conn_reest_request_r13, type_, "c1");
      return c.get<rrc_conn_reest_request_nb_s>();
    }
    const rrc_conn_request_nb_s& rrc_conn_request_r13() const
    {
      assert_choice_type(types::rrc_conn_request_r13, type_, "c1");
      return c.get<rrc_conn_request_nb_s>();
    }
    const rrc_conn_resume_request_nb_s& rrc_conn_resume_request_r13() const
    {
      assert_choice_type(types::rrc_conn_resume_request_r13, type_, "c1");
      return c.get<rrc_conn_resume_request_nb_s>();
    }
    const rrc_early_data_request_nb_r15_s& rrc_early_data_request_r15() const
    {
      assert_choice_type(types::rrc_early_data_request_r15, type_, "c1");
      return c.get<rrc_early_data_request_nb_r15_s>();
    }
    rrc_conn_reest_request_nb_s&     set_rrc_conn_reest_request_r13();
    rrc_conn_request_nb_s&           set_rrc_conn_request_r13();
    rrc_conn_resume_request_nb_s&    set_rrc_conn_resume_request_r13();
    rrc_early_data_request_nb_r15_s& set_rrc_early_data_request_r15();

  private:
    types type_;
    choice_buffer_t<rrc_conn_reest_request_nb_s,
                    rrc_conn_request_nb_s,
                    rrc_conn_resume_request_nb_s,
                    rrc_early_data_request_nb_r15_s>
        c;

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
  ul_ccch_msg_type_nb_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "UL-CCCH-MessageType-NB");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "UL-CCCH-MessageType-NB");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// UL-CCCH-Message-NB ::= SEQUENCE
struct ul_ccch_msg_nb_s {
  ul_ccch_msg_type_nb_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULInformationTransfer-NB-r13-IEs ::= SEQUENCE
struct ul_info_transfer_nb_r13_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring ded_info_nas_r13;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ULInformationTransfer-NB ::= SEQUENCE
struct ul_info_transfer_nb_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { ul_info_transfer_r13, crit_exts_future, nulltype } value;

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
    ul_info_transfer_nb_r13_ies_s& ul_info_transfer_r13()
    {
      assert_choice_type(types::ul_info_transfer_r13, type_, "criticalExtensions");
      return c;
    }
    const ul_info_transfer_nb_r13_ies_s& ul_info_transfer_r13() const
    {
      assert_choice_type(types::ul_info_transfer_r13, type_, "criticalExtensions");
      return c;
    }
    ul_info_transfer_nb_r13_ies_s& set_ul_info_transfer_r13();
    void                           set_crit_exts_future();

  private:
    types                         type_;
    ul_info_transfer_nb_r13_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UL-DCCH-MessageType-NB ::= CHOICE
struct ul_dcch_msg_type_nb_c {
  struct c1_c_ {
    struct types_opts {
      enum options {
        rrc_conn_recfg_complete_r13,
        rrc_conn_reest_complete_r13,
        rrc_conn_setup_complete_r13,
        security_mode_complete_r13,
        security_mode_fail_r13,
        ue_cap_info_r13,
        ul_info_transfer_r13,
        rrc_conn_resume_complete_r13,
        ue_info_resp_r16,
        pur_cfg_request_r16,
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
    c1_c_(const c1_c_& other);
    c1_c_& operator=(const c1_c_& other);
    ~c1_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rrc_conn_recfg_complete_nb_s& rrc_conn_recfg_complete_r13()
    {
      assert_choice_type(types::rrc_conn_recfg_complete_r13, type_, "c1");
      return c.get<rrc_conn_recfg_complete_nb_s>();
    }
    rrc_conn_reest_complete_nb_s& rrc_conn_reest_complete_r13()
    {
      assert_choice_type(types::rrc_conn_reest_complete_r13, type_, "c1");
      return c.get<rrc_conn_reest_complete_nb_s>();
    }
    rrc_conn_setup_complete_nb_s& rrc_conn_setup_complete_r13()
    {
      assert_choice_type(types::rrc_conn_setup_complete_r13, type_, "c1");
      return c.get<rrc_conn_setup_complete_nb_s>();
    }
    security_mode_complete_s& security_mode_complete_r13()
    {
      assert_choice_type(types::security_mode_complete_r13, type_, "c1");
      return c.get<security_mode_complete_s>();
    }
    security_mode_fail_s& security_mode_fail_r13()
    {
      assert_choice_type(types::security_mode_fail_r13, type_, "c1");
      return c.get<security_mode_fail_s>();
    }
    ue_cap_info_nb_s& ue_cap_info_r13()
    {
      assert_choice_type(types::ue_cap_info_r13, type_, "c1");
      return c.get<ue_cap_info_nb_s>();
    }
    ul_info_transfer_nb_s& ul_info_transfer_r13()
    {
      assert_choice_type(types::ul_info_transfer_r13, type_, "c1");
      return c.get<ul_info_transfer_nb_s>();
    }
    rrc_conn_resume_complete_nb_s& rrc_conn_resume_complete_r13()
    {
      assert_choice_type(types::rrc_conn_resume_complete_r13, type_, "c1");
      return c.get<rrc_conn_resume_complete_nb_s>();
    }
    ue_info_resp_nb_r16_s& ue_info_resp_r16()
    {
      assert_choice_type(types::ue_info_resp_r16, type_, "c1");
      return c.get<ue_info_resp_nb_r16_s>();
    }
    pur_cfg_request_nb_r16_s& pur_cfg_request_r16()
    {
      assert_choice_type(types::pur_cfg_request_r16, type_, "c1");
      return c.get<pur_cfg_request_nb_r16_s>();
    }
    const rrc_conn_recfg_complete_nb_s& rrc_conn_recfg_complete_r13() const
    {
      assert_choice_type(types::rrc_conn_recfg_complete_r13, type_, "c1");
      return c.get<rrc_conn_recfg_complete_nb_s>();
    }
    const rrc_conn_reest_complete_nb_s& rrc_conn_reest_complete_r13() const
    {
      assert_choice_type(types::rrc_conn_reest_complete_r13, type_, "c1");
      return c.get<rrc_conn_reest_complete_nb_s>();
    }
    const rrc_conn_setup_complete_nb_s& rrc_conn_setup_complete_r13() const
    {
      assert_choice_type(types::rrc_conn_setup_complete_r13, type_, "c1");
      return c.get<rrc_conn_setup_complete_nb_s>();
    }
    const security_mode_complete_s& security_mode_complete_r13() const
    {
      assert_choice_type(types::security_mode_complete_r13, type_, "c1");
      return c.get<security_mode_complete_s>();
    }
    const security_mode_fail_s& security_mode_fail_r13() const
    {
      assert_choice_type(types::security_mode_fail_r13, type_, "c1");
      return c.get<security_mode_fail_s>();
    }
    const ue_cap_info_nb_s& ue_cap_info_r13() const
    {
      assert_choice_type(types::ue_cap_info_r13, type_, "c1");
      return c.get<ue_cap_info_nb_s>();
    }
    const ul_info_transfer_nb_s& ul_info_transfer_r13() const
    {
      assert_choice_type(types::ul_info_transfer_r13, type_, "c1");
      return c.get<ul_info_transfer_nb_s>();
    }
    const rrc_conn_resume_complete_nb_s& rrc_conn_resume_complete_r13() const
    {
      assert_choice_type(types::rrc_conn_resume_complete_r13, type_, "c1");
      return c.get<rrc_conn_resume_complete_nb_s>();
    }
    const ue_info_resp_nb_r16_s& ue_info_resp_r16() const
    {
      assert_choice_type(types::ue_info_resp_r16, type_, "c1");
      return c.get<ue_info_resp_nb_r16_s>();
    }
    const pur_cfg_request_nb_r16_s& pur_cfg_request_r16() const
    {
      assert_choice_type(types::pur_cfg_request_r16, type_, "c1");
      return c.get<pur_cfg_request_nb_r16_s>();
    }
    rrc_conn_recfg_complete_nb_s&  set_rrc_conn_recfg_complete_r13();
    rrc_conn_reest_complete_nb_s&  set_rrc_conn_reest_complete_r13();
    rrc_conn_setup_complete_nb_s&  set_rrc_conn_setup_complete_r13();
    security_mode_complete_s&      set_security_mode_complete_r13();
    security_mode_fail_s&          set_security_mode_fail_r13();
    ue_cap_info_nb_s&              set_ue_cap_info_r13();
    ul_info_transfer_nb_s&         set_ul_info_transfer_r13();
    rrc_conn_resume_complete_nb_s& set_rrc_conn_resume_complete_r13();
    ue_info_resp_nb_r16_s&         set_ue_info_resp_r16();
    pur_cfg_request_nb_r16_s&      set_pur_cfg_request_r16();
    void                           set_spare6();
    void                           set_spare5();
    void                           set_spare4();
    void                           set_spare3();
    void                           set_spare2();
    void                           set_spare1();

  private:
    types type_;
    choice_buffer_t<pur_cfg_request_nb_r16_s,
                    rrc_conn_recfg_complete_nb_s,
                    rrc_conn_reest_complete_nb_s,
                    rrc_conn_resume_complete_nb_s,
                    rrc_conn_setup_complete_nb_s,
                    security_mode_complete_s,
                    security_mode_fail_s,
                    ue_cap_info_nb_s,
                    ue_info_resp_nb_r16_s,
                    ul_info_transfer_nb_s>
        c;

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
  ul_dcch_msg_type_nb_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "UL-DCCH-MessageType-NB");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "UL-DCCH-MessageType-NB");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// UL-DCCH-Message-NB ::= SEQUENCE
struct ul_dcch_msg_nb_s {
  ul_dcch_msg_type_nb_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarANR-MeasConfig-NB-r16 ::= SEQUENCE
struct var_anr_meas_cfg_nb_r16_s {
  uint8_t                   anr_quality_thres_r16 = 0;
  anr_carrier_list_nb_r16_l anr_carrier_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// VarANR-MeasReport-NB-r16 ::= SEQUENCE
struct var_anr_meas_report_nb_r16_s {
  using meas_result_list_r16_l_ = dyn_array<anr_meas_result_nb_r16_s>;

  // member variables
  plmn_id_list3_r11_l            plmn_id_list_r16;
  cell_global_id_eutra_s         serv_cell_id_r16;
  meas_result_serv_cell_nb_r14_s meas_result_serv_cell_r16;
  uint8_t                        relative_time_stamp_r16 = 0;
  meas_result_list_r16_l_        meas_result_list_r16;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc
} // namespace asn1

#endif // SRSASN1_RRC_NBIOT_H
