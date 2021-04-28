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

#ifndef SRSASN1_RRC_PAGING_H
#define SRSASN1_RRC_PAGING_H

#include "common.h"

namespace asn1 {
namespace rrc {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// Paging-v1530-IEs ::= SEQUENCE
struct paging_v1530_ies_s {
  bool access_type_present  = false;
  bool non_crit_ext_present = false;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// IMSI ::= SEQUENCE (SIZE (6..21)) OF INTEGER (0..9)
using imsi_l = bounded_array<uint8_t, 21>;

// Paging-v1310-IEs ::= SEQUENCE
struct paging_v1310_ies_s {
  bool               redist_ind_r13_present         = false;
  bool               sys_info_mod_e_drx_r13_present = false;
  bool               non_crit_ext_present           = false;
  paging_v1530_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// S-TMSI ::= SEQUENCE
struct s_tmsi_s {
  fixed_bitstring<8>  mmec;
  fixed_bitstring<32> m_tmsi;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Paging-v1130-IEs ::= SEQUENCE
struct paging_v1130_ies_s {
  bool               eab_param_mod_r11_present = false;
  bool               non_crit_ext_present      = false;
  paging_v1310_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingUE-Identity ::= CHOICE
struct paging_ue_id_c {
  struct types_opts {
    enum options { s_tmsi, imsi, /*...*/ ng_minus5_g_s_tmsi_r15, full_i_rnti_r15, nulltype } value;
    typedef int8_t number_type;

    const char* to_string() const;
    int8_t      to_number() const;
  };
  typedef enumerated<types_opts, true, 2> types;

  // choice methods
  paging_ue_id_c() = default;
  paging_ue_id_c(const paging_ue_id_c& other);
  paging_ue_id_c& operator=(const paging_ue_id_c& other);
  ~paging_ue_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  s_tmsi_s& s_tmsi()
  {
    assert_choice_type(types::s_tmsi, type_, "PagingUE-Identity");
    return c.get<s_tmsi_s>();
  }
  imsi_l& imsi()
  {
    assert_choice_type(types::imsi, type_, "PagingUE-Identity");
    return c.get<imsi_l>();
  }
  fixed_bitstring<48>& ng_minus5_g_s_tmsi_r15()
  {
    assert_choice_type(types::ng_minus5_g_s_tmsi_r15, type_, "PagingUE-Identity");
    return c.get<fixed_bitstring<48> >();
  }
  fixed_bitstring<40>& full_i_rnti_r15()
  {
    assert_choice_type(types::full_i_rnti_r15, type_, "PagingUE-Identity");
    return c.get<fixed_bitstring<40> >();
  }
  const s_tmsi_s& s_tmsi() const
  {
    assert_choice_type(types::s_tmsi, type_, "PagingUE-Identity");
    return c.get<s_tmsi_s>();
  }
  const imsi_l& imsi() const
  {
    assert_choice_type(types::imsi, type_, "PagingUE-Identity");
    return c.get<imsi_l>();
  }
  const fixed_bitstring<48>& ng_minus5_g_s_tmsi_r15() const
  {
    assert_choice_type(types::ng_minus5_g_s_tmsi_r15, type_, "PagingUE-Identity");
    return c.get<fixed_bitstring<48> >();
  }
  const fixed_bitstring<40>& full_i_rnti_r15() const
  {
    assert_choice_type(types::full_i_rnti_r15, type_, "PagingUE-Identity");
    return c.get<fixed_bitstring<40> >();
  }
  s_tmsi_s&            set_s_tmsi();
  imsi_l&              set_imsi();
  fixed_bitstring<48>& set_ng_minus5_g_s_tmsi_r15();
  fixed_bitstring<40>& set_full_i_rnti_r15();

private:
  types                                                  type_;
  choice_buffer_t<fixed_bitstring<48>, imsi_l, s_tmsi_s> c;

  void destroy_();
};

// Paging-v920-IEs ::= SEQUENCE
struct paging_v920_ies_s {
  bool               cmas_ind_r9_present  = false;
  bool               non_crit_ext_present = false;
  paging_v1130_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingRecord ::= SEQUENCE
struct paging_record_s {
  struct cn_domain_opts {
    enum options { ps, cs, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<cn_domain_opts> cn_domain_e_;

  // member variables
  bool           ext = false;
  paging_ue_id_c ue_id;
  cn_domain_e_   cn_domain;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// Paging-v890-IEs ::= SEQUENCE
struct paging_v890_ies_s {
  bool              late_non_crit_ext_present = false;
  bool              non_crit_ext_present      = false;
  dyn_octstring     late_non_crit_ext;
  paging_v920_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PagingRecordList ::= SEQUENCE (SIZE (1..16)) OF PagingRecord
using paging_record_list_l = dyn_array<paging_record_s>;

// Paging ::= SEQUENCE
struct paging_s {
  bool                 paging_record_list_present = false;
  bool                 sys_info_mod_present       = false;
  bool                 etws_ind_present           = false;
  bool                 non_crit_ext_present       = false;
  paging_record_list_l paging_record_list;
  paging_v890_ies_s    non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PCCH-MessageType ::= CHOICE
struct pcch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { paging, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::paging; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    paging_s&       paging() { return c; }
    const paging_s& paging() const { return c; }

  private:
    paging_s c;
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  pcch_msg_type_c() = default;
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "PCCH-MessageType");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "PCCH-MessageType");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// PCCH-Message ::= SEQUENCE
struct pcch_msg_s {
  pcch_msg_type_c msg;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEPagingCoverageInformation-r13-IEs ::= SEQUENCE
struct ue_paging_coverage_info_r13_ies_s {
  bool     mpdcch_num_repeat_r13_present = false;
  bool     non_crit_ext_present          = false;
  uint16_t mpdcch_num_repeat_r13         = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEPagingCoverageInformation ::= SEQUENCE
struct ue_paging_coverage_info_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options {
          ue_paging_coverage_info_r13,
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
      ue_paging_coverage_info_r13_ies_s& ue_paging_coverage_info_r13()
      {
        assert_choice_type(types::ue_paging_coverage_info_r13, type_, "c1");
        return c;
      }
      const ue_paging_coverage_info_r13_ies_s& ue_paging_coverage_info_r13() const
      {
        assert_choice_type(types::ue_paging_coverage_info_r13, type_, "c1");
        return c;
      }
      ue_paging_coverage_info_r13_ies_s& set_ue_paging_coverage_info_r13();
      void                               set_spare7();
      void                               set_spare6();
      void                               set_spare5();
      void                               set_spare4();
      void                               set_spare3();
      void                               set_spare2();
      void                               set_spare1();

    private:
      types                             type_;
      ue_paging_coverage_info_r13_ies_s c;
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

// UERadioPagingInformation-v1310-IEs ::= SEQUENCE
struct ue_radio_paging_info_v1310_ies_s {
  using supported_band_list_eutra_for_paging_r13_l_ = dyn_array<uint16_t>;

  // member variables
  bool                                        supported_band_list_eutra_for_paging_r13_present = false;
  bool                                        non_crit_ext_present                             = false;
  supported_band_list_eutra_for_paging_r13_l_ supported_band_list_eutra_for_paging_r13;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioPagingInformation-r12-IEs ::= SEQUENCE
struct ue_radio_paging_info_r12_ies_s {
  bool                             non_crit_ext_present = false;
  dyn_octstring                    ue_radio_paging_info_r12;
  ue_radio_paging_info_v1310_ies_s non_crit_ext;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UERadioPagingInformation ::= SEQUENCE
struct ue_radio_paging_info_s {
  struct crit_exts_c_ {
    struct c1_c_ {
      struct types_opts {
        enum options {
          ue_radio_paging_info_r12,
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
      ue_radio_paging_info_r12_ies_s& ue_radio_paging_info_r12()
      {
        assert_choice_type(types::ue_radio_paging_info_r12, type_, "c1");
        return c;
      }
      const ue_radio_paging_info_r12_ies_s& ue_radio_paging_info_r12() const
      {
        assert_choice_type(types::ue_radio_paging_info_r12, type_, "c1");
        return c;
      }
      ue_radio_paging_info_r12_ies_s& set_ue_radio_paging_info_r12();
      void                            set_spare7();
      void                            set_spare6();
      void                            set_spare5();
      void                            set_spare4();
      void                            set_spare3();
      void                            set_spare2();
      void                            set_spare1();

    private:
      types                          type_;
      ue_radio_paging_info_r12_ies_s c;
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

#endif // SRSASN1_RRC_PAGING_H
