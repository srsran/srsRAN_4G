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

#ifndef SRSASN1_RRC_DLCCCH_MSG_H
#define SRSASN1_RRC_DLCCCH_MSG_H

#include "meascfg.h"
#include "rr_ded.h"

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// RRCConnectionReject-v1320-IEs ::= SEQUENCE
struct rrc_conn_reject_v1320_ies_s {
  bool rrc_suspend_ind_r13_present = false;
  bool non_crit_ext_present        = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandClassPriority1XRTT ::= SEQUENCE
struct band_class_prio1_xrtt_s {
  bandclass_cdma2000_e band_class;
  uint8_t              cell_resel_prio = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandClassPriorityHRPD ::= SEQUENCE
struct band_class_prio_hrpd_s {
  bandclass_cdma2000_e band_class;
  uint8_t              cell_resel_prio = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqPriorityEUTRA ::= SEQUENCE
struct freq_prio_eutra_s {
  uint32_t carrier_freq    = 0;
  uint8_t  cell_resel_prio = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqPriorityEUTRA-r12 ::= SEQUENCE
struct freq_prio_eutra_r12_s {
  uint32_t carrier_freq_r12    = 0;
  uint8_t  cell_resel_prio_r12 = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqPriorityEUTRA-v1310 ::= SEQUENCE
struct freq_prio_eutra_v1310_s {
  bool                      cell_resel_sub_prio_r13_present = false;
  cell_resel_sub_prio_r13_e cell_resel_sub_prio_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqPriorityNR-r15 ::= SEQUENCE
struct freq_prio_nr_r15_s {
  bool                      cell_resel_sub_prio_r15_present = false;
  uint32_t                  carrier_freq_r15                = 0;
  uint8_t                   cell_resel_prio_r15             = 0;
  cell_resel_sub_prio_r13_e cell_resel_sub_prio_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqPriorityUTRA-FDD ::= SEQUENCE
struct freq_prio_utra_fdd_s {
  uint16_t carrier_freq    = 0;
  uint8_t  cell_resel_prio = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqPriorityUTRA-TDD ::= SEQUENCE
struct freq_prio_utra_tdd_s {
  uint16_t carrier_freq    = 0;
  uint8_t  cell_resel_prio = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqsPriorityGERAN ::= SEQUENCE
struct freqs_prio_geran_s {
  carrier_freqs_geran_s carrier_freqs;
  uint8_t               cell_resel_prio = 0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReject-v1130-IEs ::= SEQUENCE
struct rrc_conn_reject_v1130_ies_s {
  struct depriorit_req_r11_s_ {
    struct depriorit_type_r11_opts {
      enum options { freq, e_utra, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<depriorit_type_r11_opts> depriorit_type_r11_e_;
    struct depriorit_timer_r11_opts {
      enum options { min5, min10, min15, min30, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<depriorit_timer_r11_opts> depriorit_timer_r11_e_;

    // member variables
    depriorit_type_r11_e_  depriorit_type_r11;
    depriorit_timer_r11_e_ depriorit_timer_r11;
  };

  // member variables
  bool                        depriorit_req_r11_present = false;
  bool                        non_crit_ext_present      = false;
  depriorit_req_r11_s_        depriorit_req_r11;
  rrc_conn_reject_v1320_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BandClassPriorityList1XRTT ::= SEQUENCE (SIZE (1..32)) OF BandClassPriority1XRTT
using band_class_prio_list1_xrtt_l = dyn_array<band_class_prio1_xrtt_s>;

// BandClassPriorityListHRPD ::= SEQUENCE (SIZE (1..32)) OF BandClassPriorityHRPD
using band_class_prio_list_hrpd_l = dyn_array<band_class_prio_hrpd_s>;

// CarrierFreqListUTRA-TDD-r10 ::= SEQUENCE (SIZE (1..6)) OF INTEGER (0..16383)
using carrier_freq_list_utra_tdd_r10_l = bounded_array<uint16_t, 6>;

// FreqPriorityEUTRA-v9e0 ::= SEQUENCE
struct freq_prio_eutra_v9e0_s {
  bool     carrier_freq_v9e0_present = false;
  uint32_t carrier_freq_v9e0         = 65536;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FreqPriorityListEUTRA ::= SEQUENCE (SIZE (1..8)) OF FreqPriorityEUTRA
using freq_prio_list_eutra_l = dyn_array<freq_prio_eutra_s>;

// FreqPriorityListEUTRA-v1310 ::= SEQUENCE (SIZE (1..8)) OF FreqPriorityEUTRA-v1310
using freq_prio_list_eutra_v1310_l = dyn_array<freq_prio_eutra_v1310_s>;

// FreqPriorityListExtEUTRA-r12 ::= SEQUENCE (SIZE (1..8)) OF FreqPriorityEUTRA-r12
using freq_prio_list_ext_eutra_r12_l = dyn_array<freq_prio_eutra_r12_s>;

// FreqPriorityListExtEUTRA-v1310 ::= SEQUENCE (SIZE (1..8)) OF FreqPriorityEUTRA-v1310
using freq_prio_list_ext_eutra_v1310_l = dyn_array<freq_prio_eutra_v1310_s>;

// FreqPriorityListNR-r15 ::= SEQUENCE (SIZE (1..8)) OF FreqPriorityNR-r15
using freq_prio_list_nr_r15_l = dyn_array<freq_prio_nr_r15_s>;

// FreqPriorityListUTRA-FDD ::= SEQUENCE (SIZE (1..16)) OF FreqPriorityUTRA-FDD
using freq_prio_list_utra_fdd_l = dyn_array<freq_prio_utra_fdd_s>;

// FreqPriorityListUTRA-TDD ::= SEQUENCE (SIZE (1..16)) OF FreqPriorityUTRA-TDD
using freq_prio_list_utra_tdd_l = dyn_array<freq_prio_utra_tdd_s>;

// FreqsPriorityListGERAN ::= SEQUENCE (SIZE (1..16)) OF FreqsPriorityGERAN
using freqs_prio_list_geran_l = dyn_array<freqs_prio_geran_s>;

// RRCConnectionReject-v1020-IEs ::= SEQUENCE
struct rrc_conn_reject_v1020_ies_s {
  bool                        extended_wait_time_r10_present = false;
  bool                        non_crit_ext_present           = false;
  uint16_t                    extended_wait_time_r10         = 1;
  rrc_conn_reject_v1130_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IdleModeMobilityControlInfo ::= SEQUENCE
struct idle_mode_mob_ctrl_info_s {
  struct t320_opts {
    enum options { min5, min10, min20, min30, min60, min120, min180, spare1, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<t320_opts> t320_e_;

  // member variables
  bool                         ext                                = false;
  bool                         freq_prio_list_eutra_present       = false;
  bool                         freq_prio_list_geran_present       = false;
  bool                         freq_prio_list_utra_fdd_present    = false;
  bool                         freq_prio_list_utra_tdd_present    = false;
  bool                         band_class_prio_list_hrpd_present  = false;
  bool                         band_class_prio_list1_xrtt_present = false;
  bool                         t320_present                       = false;
  freq_prio_list_eutra_l       freq_prio_list_eutra;
  freqs_prio_list_geran_l      freq_prio_list_geran;
  freq_prio_list_utra_fdd_l    freq_prio_list_utra_fdd;
  freq_prio_list_utra_tdd_l    freq_prio_list_utra_tdd;
  band_class_prio_list_hrpd_l  band_class_prio_list_hrpd;
  band_class_prio_list1_xrtt_l band_class_prio_list1_xrtt;
  t320_e_                      t320;
  // ...
  // group 0
  copy_ptr<freq_prio_list_ext_eutra_r12_l> freq_prio_list_ext_eutra_r12;
  // group 1
  copy_ptr<freq_prio_list_eutra_v1310_l>     freq_prio_list_eutra_v1310;
  copy_ptr<freq_prio_list_ext_eutra_v1310_l> freq_prio_list_ext_eutra_v1310;
  // group 2
  copy_ptr<freq_prio_list_nr_r15_l> freq_prio_list_nr_r15;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IdleModeMobilityControlInfo-v9e0 ::= SEQUENCE
struct idle_mode_mob_ctrl_info_v9e0_s {
  using freq_prio_list_eutra_v9e0_l_ = dyn_array<freq_prio_eutra_v9e0_s>;

  // member variables
  freq_prio_list_eutra_v9e0_l_ freq_prio_list_eutra_v9e0;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishment-v8a0-IEs ::= SEQUENCE
struct rrc_conn_reest_v8a0_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentReject-v8a0-IEs ::= SEQUENCE
struct rrc_conn_reest_reject_v8a0_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReject-v8a0-IEs ::= SEQUENCE
struct rrc_conn_reject_v8a0_ies_s {
  bool                        late_non_crit_ext_present = false;
  bool                        non_crit_ext_present      = false;
  dyn_octstring               late_non_crit_ext;
  rrc_conn_reject_v1020_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetup-v8a0-IEs ::= SEQUENCE
struct rrc_conn_setup_v8a0_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCEarlyDataComplete-v1590-IEs ::= SEQUENCE
struct rrc_early_data_complete_v1590_ies_s {
  bool          late_non_crit_ext_present = false;
  bool          non_crit_ext_present      = false;
  dyn_octstring late_non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RedirectedCarrierInfo-r15-IEs ::= CHOICE
struct redirected_carrier_info_r15_ies_c {
  struct types_opts {
    enum options {
      eutra_r15,
      geran_r15,
      utra_fdd_r15,
      cdma2000_hrpd_r15,
      cdma2000_minus1x_rtt_r15,
      utra_tdd_r15,
      nulltype
    } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  redirected_carrier_info_r15_ies_c() = default;
  redirected_carrier_info_r15_ies_c(const redirected_carrier_info_r15_ies_c& other);
  redirected_carrier_info_r15_ies_c& operator=(const redirected_carrier_info_r15_ies_c& other);
  ~redirected_carrier_info_r15_ies_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint32_t& eutra_r15()
  {
    assert_choice_type(types::eutra_r15, type_, "RedirectedCarrierInfo-r15-IEs");
    return c.get<uint32_t>();
  }
  carrier_freqs_geran_s& geran_r15()
  {
    assert_choice_type(types::geran_r15, type_, "RedirectedCarrierInfo-r15-IEs");
    return c.get<carrier_freqs_geran_s>();
  }
  uint16_t& utra_fdd_r15()
  {
    assert_choice_type(types::utra_fdd_r15, type_, "RedirectedCarrierInfo-r15-IEs");
    return c.get<uint16_t>();
  }
  carrier_freq_cdma2000_s& cdma2000_hrpd_r15()
  {
    assert_choice_type(types::cdma2000_hrpd_r15, type_, "RedirectedCarrierInfo-r15-IEs");
    return c.get<carrier_freq_cdma2000_s>();
  }
  carrier_freq_cdma2000_s& cdma2000_minus1x_rtt_r15()
  {
    assert_choice_type(types::cdma2000_minus1x_rtt_r15, type_, "RedirectedCarrierInfo-r15-IEs");
    return c.get<carrier_freq_cdma2000_s>();
  }
  carrier_freq_list_utra_tdd_r10_l& utra_tdd_r15()
  {
    assert_choice_type(types::utra_tdd_r15, type_, "RedirectedCarrierInfo-r15-IEs");
    return c.get<carrier_freq_list_utra_tdd_r10_l>();
  }
  const uint32_t& eutra_r15() const
  {
    assert_choice_type(types::eutra_r15, type_, "RedirectedCarrierInfo-r15-IEs");
    return c.get<uint32_t>();
  }
  const carrier_freqs_geran_s& geran_r15() const
  {
    assert_choice_type(types::geran_r15, type_, "RedirectedCarrierInfo-r15-IEs");
    return c.get<carrier_freqs_geran_s>();
  }
  const uint16_t& utra_fdd_r15() const
  {
    assert_choice_type(types::utra_fdd_r15, type_, "RedirectedCarrierInfo-r15-IEs");
    return c.get<uint16_t>();
  }
  const carrier_freq_cdma2000_s& cdma2000_hrpd_r15() const
  {
    assert_choice_type(types::cdma2000_hrpd_r15, type_, "RedirectedCarrierInfo-r15-IEs");
    return c.get<carrier_freq_cdma2000_s>();
  }
  const carrier_freq_cdma2000_s& cdma2000_minus1x_rtt_r15() const
  {
    assert_choice_type(types::cdma2000_minus1x_rtt_r15, type_, "RedirectedCarrierInfo-r15-IEs");
    return c.get<carrier_freq_cdma2000_s>();
  }
  const carrier_freq_list_utra_tdd_r10_l& utra_tdd_r15() const
  {
    assert_choice_type(types::utra_tdd_r15, type_, "RedirectedCarrierInfo-r15-IEs");
    return c.get<carrier_freq_list_utra_tdd_r10_l>();
  }
  uint32_t&                         set_eutra_r15();
  carrier_freqs_geran_s&            set_geran_r15();
  uint16_t&                         set_utra_fdd_r15();
  carrier_freq_cdma2000_s&          set_cdma2000_hrpd_r15();
  carrier_freq_cdma2000_s&          set_cdma2000_minus1x_rtt_r15();
  carrier_freq_list_utra_tdd_r10_l& set_utra_tdd_r15();

private:
  types                                                                                             type_;
  choice_buffer_t<carrier_freq_cdma2000_s, carrier_freq_list_utra_tdd_r10_l, carrier_freqs_geran_s> c;

  void destroy_();
};

// RRCConnectionReestablishment-r8-IEs ::= SEQUENCE
struct rrc_conn_reest_r8_ies_s {
  bool                      non_crit_ext_present = false;
  rr_cfg_ded_s              rr_cfg_ded;
  uint8_t                   next_hop_chaining_count = 0;
  rrc_conn_reest_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishmentReject-r8-IEs ::= SEQUENCE
struct rrc_conn_reest_reject_r8_ies_s {
  bool                             non_crit_ext_present = false;
  rrc_conn_reest_reject_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReject-r8-IEs ::= SEQUENCE
struct rrc_conn_reject_r8_ies_s {
  bool                       non_crit_ext_present = false;
  uint8_t                    wait_time            = 1;
  rrc_conn_reject_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionSetup-r8-IEs ::= SEQUENCE
struct rrc_conn_setup_r8_ies_s {
  bool                      non_crit_ext_present = false;
  rr_cfg_ded_s              rr_cfg_ded;
  rrc_conn_setup_v8a0_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCEarlyDataComplete-r15-IEs ::= SEQUENCE
struct rrc_early_data_complete_r15_ies_s {
  bool                                ded_info_nas_r15_present                = false;
  bool                                extended_wait_time_r15_present          = false;
  bool                                idle_mode_mob_ctrl_info_r15_present     = false;
  bool                                idle_mode_mob_ctrl_info_ext_r15_present = false;
  bool                                redirected_carrier_info_r15_present     = false;
  bool                                non_crit_ext_present                    = false;
  dyn_octstring                       ded_info_nas_r15;
  uint16_t                            extended_wait_time_r15 = 1;
  idle_mode_mob_ctrl_info_s           idle_mode_mob_ctrl_info_r15;
  idle_mode_mob_ctrl_info_v9e0_s      idle_mode_mob_ctrl_info_ext_r15;
  redirected_carrier_info_r15_ies_c   redirected_carrier_info_r15;
  rrc_early_data_complete_v1590_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReestablishment ::= SEQUENCE
struct rrc_conn_reest_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { rrc_conn_reest_r8, spare7, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;

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
      rrc_conn_reest_r8_ies_s& rrc_conn_reest_r8()
      {
        assert_choice_type(types::rrc_conn_reest_r8, type_, "c1");
        return c;
      }
      const rrc_conn_reest_r8_ies_s& rrc_conn_reest_r8() const
      {
        assert_choice_type(types::rrc_conn_reest_r8, type_, "c1");
        return c;
      }
      rrc_conn_reest_r8_ies_s& set_rrc_conn_reest_r8();
      void                     set_spare7();
      void                     set_spare6();
      void                     set_spare5();
      void                     set_spare4();
      void                     set_spare3();
      void                     set_spare2();
      void                     set_spare1();

    private:
      types                   type_;
      rrc_conn_reest_r8_ies_s c;
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

// RRCConnectionReestablishmentReject ::= SEQUENCE
struct rrc_conn_reest_reject_s {
  struct crit_exts_c_ {
    struct types_opts {
      enum options { rrc_conn_reest_reject_r8, crit_exts_future, nulltype } value;

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
    rrc_conn_reest_reject_r8_ies_s& rrc_conn_reest_reject_r8()
    {
      assert_choice_type(types::rrc_conn_reest_reject_r8, type_, "criticalExtensions");
      return c;
    }
    const rrc_conn_reest_reject_r8_ies_s& rrc_conn_reest_reject_r8() const
    {
      assert_choice_type(types::rrc_conn_reest_reject_r8, type_, "criticalExtensions");
      return c;
    }
    rrc_conn_reest_reject_r8_ies_s& set_rrc_conn_reest_reject_r8();
    void                            set_crit_exts_future();

  private:
    types                          type_;
    rrc_conn_reest_reject_r8_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RRCConnectionReject ::= SEQUENCE
struct rrc_conn_reject_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { rrc_conn_reject_r8, spare3, spare2, spare1, nulltype } value;

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
      rrc_conn_reject_r8_ies_s& rrc_conn_reject_r8()
      {
        assert_choice_type(types::rrc_conn_reject_r8, type_, "c1");
        return c;
      }
      const rrc_conn_reject_r8_ies_s& rrc_conn_reject_r8() const
      {
        assert_choice_type(types::rrc_conn_reject_r8, type_, "c1");
        return c;
      }
      rrc_conn_reject_r8_ies_s& set_rrc_conn_reject_r8();
      void                      set_spare3();
      void                      set_spare2();
      void                      set_spare1();

    private:
      types                    type_;
      rrc_conn_reject_r8_ies_s c;
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

// RRCConnectionSetup ::= SEQUENCE
struct rrc_conn_setup_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options { rrc_conn_setup_r8, spare7, spare6, spare5, spare4, spare3, spare2, spare1, nulltype } value;

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
      rrc_conn_setup_r8_ies_s& rrc_conn_setup_r8()
      {
        assert_choice_type(types::rrc_conn_setup_r8, type_, "c1");
        return c;
      }
      const rrc_conn_setup_r8_ies_s& rrc_conn_setup_r8() const
      {
        assert_choice_type(types::rrc_conn_setup_r8, type_, "c1");
        return c;
      }
      rrc_conn_setup_r8_ies_s& set_rrc_conn_setup_r8();
      void                     set_spare7();
      void                     set_spare6();
      void                     set_spare5();
      void                     set_spare4();
      void                     set_spare3();
      void                     set_spare2();
      void                     set_spare1();

    private:
      types                   type_;
      rrc_conn_setup_r8_ies_s c;
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

// RRCEarlyDataComplete-r15 ::= SEQUENCE
struct rrc_early_data_complete_r15_s {
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
    rrc_early_data_complete_r15_ies_s& rrc_early_data_complete_r15()
    {
      assert_choice_type(types::rrc_early_data_complete_r15, type_, "criticalExtensions");
      return c;
    }
    const rrc_early_data_complete_r15_ies_s& rrc_early_data_complete_r15() const
    {
      assert_choice_type(types::rrc_early_data_complete_r15, type_, "criticalExtensions");
      return c;
    }
    rrc_early_data_complete_r15_ies_s& set_rrc_early_data_complete_r15();
    void                               set_crit_exts_future();

  private:
    types                             type_;
    rrc_early_data_complete_r15_ies_s c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DL-CCCH-MessageType ::= CHOICE
struct dl_ccch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { rrc_conn_reest, rrc_conn_reest_reject, rrc_conn_reject, rrc_conn_setup, nulltype } value;

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
    rrc_conn_reest_s& rrc_conn_reest()
    {
      assert_choice_type(types::rrc_conn_reest, type_, "c1");
      return c.get<rrc_conn_reest_s>();
    }
    rrc_conn_reest_reject_s& rrc_conn_reest_reject()
    {
      assert_choice_type(types::rrc_conn_reest_reject, type_, "c1");
      return c.get<rrc_conn_reest_reject_s>();
    }
    rrc_conn_reject_s& rrc_conn_reject()
    {
      assert_choice_type(types::rrc_conn_reject, type_, "c1");
      return c.get<rrc_conn_reject_s>();
    }
    rrc_conn_setup_s& rrc_conn_setup()
    {
      assert_choice_type(types::rrc_conn_setup, type_, "c1");
      return c.get<rrc_conn_setup_s>();
    }
    const rrc_conn_reest_s& rrc_conn_reest() const
    {
      assert_choice_type(types::rrc_conn_reest, type_, "c1");
      return c.get<rrc_conn_reest_s>();
    }
    const rrc_conn_reest_reject_s& rrc_conn_reest_reject() const
    {
      assert_choice_type(types::rrc_conn_reest_reject, type_, "c1");
      return c.get<rrc_conn_reest_reject_s>();
    }
    const rrc_conn_reject_s& rrc_conn_reject() const
    {
      assert_choice_type(types::rrc_conn_reject, type_, "c1");
      return c.get<rrc_conn_reject_s>();
    }
    const rrc_conn_setup_s& rrc_conn_setup() const
    {
      assert_choice_type(types::rrc_conn_setup, type_, "c1");
      return c.get<rrc_conn_setup_s>();
    }
    rrc_conn_reest_s&        set_rrc_conn_reest();
    rrc_conn_reest_reject_s& set_rrc_conn_reest_reject();
    rrc_conn_reject_s&       set_rrc_conn_reject();
    rrc_conn_setup_s&        set_rrc_conn_setup();

  private:
    types                                                                                           type_;
    choice_buffer_t<rrc_conn_reest_reject_s, rrc_conn_reest_s, rrc_conn_reject_s, rrc_conn_setup_s> c;

    void destroy_();
  };
  struct msg_class_ext_c_ {
    struct c2_c_ {
      struct types_opts {
        enum options { rrc_early_data_complete_r15, spare3, spare2, spare1, nulltype } value;

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
      rrc_early_data_complete_r15_s& rrc_early_data_complete_r15()
      {
        assert_choice_type(types::rrc_early_data_complete_r15, type_, "c2");
        return c;
      }
      const rrc_early_data_complete_r15_s& rrc_early_data_complete_r15() const
      {
        assert_choice_type(types::rrc_early_data_complete_r15, type_, "c2");
        return c;
      }
      rrc_early_data_complete_r15_s& set_rrc_early_data_complete_r15();
      void                           set_spare3();
      void                           set_spare2();
      void                           set_spare1();

    private:
      types                         type_;
      rrc_early_data_complete_r15_s c;
    };
    struct types_opts {
      enum options { c2, msg_class_ext_future_r15, nulltype } value;
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
    void   set_msg_class_ext_future_r15();

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
  dl_ccch_msg_type_c() = default;
  dl_ccch_msg_type_c(const dl_ccch_msg_type_c& other);
  dl_ccch_msg_type_c& operator=(const dl_ccch_msg_type_c& other);
  ~dl_ccch_msg_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "DL-CCCH-MessageType");
    return c.get<c1_c_>();
  }
  msg_class_ext_c_& msg_class_ext()
  {
    assert_choice_type(types::msg_class_ext, type_, "DL-CCCH-MessageType");
    return c.get<msg_class_ext_c_>();
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "DL-CCCH-MessageType");
    return c.get<c1_c_>();
  }
  const msg_class_ext_c_& msg_class_ext() const
  {
    assert_choice_type(types::msg_class_ext, type_, "DL-CCCH-MessageType");
    return c.get<msg_class_ext_c_>();
  }
  c1_c_&            set_c1();
  msg_class_ext_c_& set_msg_class_ext();

private:
  types                                    type_;
  choice_buffer_t<c1_c_, msg_class_ext_c_> c;

  void destroy_();
};

// DL-CCCH-Message ::= SEQUENCE
struct dl_ccch_msg_s {
  dl_ccch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

} // namespace rrc
} // namespace asn1

#endif // SRSASN1_RRC_DLCCCH_MSG_H
