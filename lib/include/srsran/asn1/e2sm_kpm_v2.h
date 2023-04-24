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
 *                   3GPP TS ASN1 E2SM KPM v15.3.0 (2019-03)
 *
 ******************************************************************************/

#pragma once

#include "asn1_utils.h"
#include <cstdio>
#include <stdarg.h>

namespace asn1 {
namespace e2sm_kpm {
/*******************************************************************************
 *                             Constant Definitions
 ******************************************************************************/

#define ASN1_E2SM_KPM_MAXNOOF_CELLS 16384
#define ASN1_E2SM_KPM_MAXNOOF_RIC_STYLES 63
#define ASN1_E2SM_KPM_MAXNOOF_MEAS_INFO 65535
#define ASN1_E2SM_KPM_MAXNOOF_LABEL_INFO 2147483647
#define ASN1_E2SM_KPM_MAXNOOF_MEAS_RECORD 65535
#define ASN1_E2SM_KPM_MAXNOOF_MEAS_VALUE 2147483647
#define ASN1_E2SM_KPM_MAXNOOF_CONDITION_INFO 32768
#define ASN1_E2SM_KPM_MAXNOOF_UEID 65535
#define ASN1_E2SM_KPM_MAXNOOF_CONDITION_INFO_PER_SUB 32768
#define ASN1_E2SM_KPM_MAXNOOF_UEID_PER_SUB 65535
#define ASN1_E2SM_KPM_MAXNOOF_UE_MEAS_REPORT 65535
#define ASN1_E2SM_KPM_MAXNOOF_BIN 65535

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/
#define None std::numeric_limits<uint32_t>::max()

struct real_s {
  SRSASN_CODE pack(bit_ref& bref) const
  {
    printf(" WARNING using unimplemented REAL packing function\n");
    return SRSASN_SUCCESS;
  };
  SRSASN_CODE unpack(cbit_ref& bref) const
  {
    printf(" WARNING using unimplemented REAL unpacking function\n");
    return SRSASN_SUCCESS;
  };
  void to_json(json_writer& j) const { printf(" WARNING using unimplemented REAL json function\n"); };
};

// BinRangeValue ::= CHOICE
struct bin_range_value_c {
  struct types_opts {
    enum options { value_int, value_real, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  bin_range_value_c() = default;
  bin_range_value_c(const bin_range_value_c& other);
  bin_range_value_c& operator=(const bin_range_value_c& other);
  ~bin_range_value_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  int64_t& value_int()
  {
    assert_choice_type(types::value_int, type_, "BinRangeValue");
    return c.get<int64_t>();
  }
  real_s& value_real()
  {
    assert_choice_type(types::value_real, type_, "BinRangeValue");
    return c.get<real_s>();
  }
  const int64_t& value_int() const
  {
    assert_choice_type(types::value_int, type_, "BinRangeValue");
    return c.get<int64_t>();
  }
  const real_s& value_real() const
  {
    assert_choice_type(types::value_real, type_, "BinRangeValue");
    return c.get<real_s>();
  }
  int64_t& set_value_int();
  real_s&  set_value_real();

private:
  types                   type_;
  choice_buffer_t<real_s> c;

  void destroy_();
};

// BinRangeItem ::= SEQUENCE
struct bin_range_item_s {
  bool              ext     = false;
  uint32_t          bin_idx = 1;
  bin_range_value_c start_value;
  bin_range_value_c end_value;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// BinRangeList ::= SEQUENCE (SIZE (1..65535)) OF BinRangeItem
using bin_range_list_l = dyn_array<bin_range_item_s>;

// BinRangeDefinition ::= SEQUENCE
struct bin_range_definition_s {
  bool             ext = false;
  bin_range_list_l bin_range_list_x;
  bin_range_list_l bin_range_list_y;
  bin_range_list_l bin_range_list_z;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EUTRA-CGI ::= SEQUENCE
struct eutra_cgi_s {
  bool                             ext = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<28, false, true> eutra_cell_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-CGI ::= SEQUENCE
struct nr_cgi_s {
  bool                             ext = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<36, false, true> nrcell_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CGI ::= CHOICE
struct cgi_c {
  struct types_opts {
    enum options { nr_cgi, eutra_cgi, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  cgi_c() = default;
  cgi_c(const cgi_c& other);
  cgi_c& operator=(const cgi_c& other);
  ~cgi_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  nr_cgi_s& nr_cgi()
  {
    assert_choice_type(types::nr_cgi, type_, "CGI");
    return c.get<nr_cgi_s>();
  }
  eutra_cgi_s& eutra_cgi()
  {
    assert_choice_type(types::eutra_cgi, type_, "CGI");
    return c.get<eutra_cgi_s>();
  }
  const nr_cgi_s& nr_cgi() const
  {
    assert_choice_type(types::nr_cgi, type_, "CGI");
    return c.get<nr_cgi_s>();
  }
  const eutra_cgi_s& eutra_cgi() const
  {
    assert_choice_type(types::eutra_cgi, type_, "CGI");
    return c.get<eutra_cgi_s>();
  }
  nr_cgi_s&    set_nr_cgi();
  eutra_cgi_s& set_eutra_cgi();

private:
  types                                  type_;
  choice_buffer_t<eutra_cgi_s, nr_cgi_s> c;

  void destroy_();
};

// GUAMI ::= SEQUENCE
struct guami_s {
  bool                             ext = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<8, false, true>  amf_region_id;
  fixed_bitstring<10, false, true> amf_set_id;
  fixed_bitstring<6, false, true>  amf_pointer;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GUMMEI ::= SEQUENCE
struct gummei_s {
  bool                     ext = false;
  fixed_octstring<3, true> plmn_id;
  fixed_octstring<2, true> mme_group_id;
  fixed_octstring<1, true> mme_code;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CoreCPID ::= CHOICE
struct core_cpid_c {
  struct types_opts {
    enum options { five_gc, epc, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  core_cpid_c() = default;
  core_cpid_c(const core_cpid_c& other);
  core_cpid_c& operator=(const core_cpid_c& other);
  ~core_cpid_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  guami_s& five_gc()
  {
    assert_choice_type(types::five_gc, type_, "CoreCPID");
    return c.get<guami_s>();
  }
  gummei_s& epc()
  {
    assert_choice_type(types::epc, type_, "CoreCPID");
    return c.get<gummei_s>();
  }
  const guami_s& five_gc() const
  {
    assert_choice_type(types::five_gc, type_, "CoreCPID");
    return c.get<guami_s>();
  }
  const gummei_s& epc() const
  {
    assert_choice_type(types::epc, type_, "CoreCPID");
    return c.get<gummei_s>();
  }
  guami_s&  set_five_gc();
  gummei_s& set_epc();

private:
  types                              type_;
  choice_buffer_t<guami_s, gummei_s> c;

  void destroy_();
};

// MeasurementType ::= CHOICE
struct meas_type_c {
  struct types_opts {
    enum options { meas_name, meas_id, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  meas_type_c() = default;
  meas_type_c(const meas_type_c& other);
  meas_type_c& operator=(const meas_type_c& other);
  ~meas_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  printable_string<1, 150, true, true>& meas_name()
  {
    assert_choice_type(types::meas_name, type_, "MeasurementType");
    return c.get<printable_string<1, 150, true, true> >();
  }
  uint32_t& meas_id()
  {
    assert_choice_type(types::meas_id, type_, "MeasurementType");
    return c.get<uint32_t>();
  }
  const printable_string<1, 150, true, true>& meas_name() const
  {
    assert_choice_type(types::meas_name, type_, "MeasurementType");
    return c.get<printable_string<1, 150, true, true> >();
  }
  const uint32_t& meas_id() const
  {
    assert_choice_type(types::meas_id, type_, "MeasurementType");
    return c.get<uint32_t>();
  }
  printable_string<1, 150, true, true>& set_meas_name();
  uint32_t&                             set_meas_id();

private:
  types                                                  type_;
  choice_buffer_t<printable_string<1, 150, true, true> > c;

  void destroy_();
};

// DistMeasurementBinRangeItem ::= SEQUENCE
struct dist_meas_bin_range_item_s {
  bool                   ext = false;
  meas_type_c            meas_type;
  bin_range_definition_s bin_range_def;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// DistMeasurementBinRangeList ::= SEQUENCE (SIZE (1..65535)) OF DistMeasurementBinRangeItem
using dist_meas_bin_range_list_l = dyn_array<dist_meas_bin_range_item_s>;

// S-NSSAI ::= SEQUENCE
struct s_nssai_s {
  bool                     ext        = false;
  bool                     sd_present = false;
  fixed_octstring<1, true> sst;
  fixed_octstring<3, true> sd;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TestCond-Expression ::= ENUMERATED
struct test_cond_expression_opts {
  enum options { equal, greaterthan, lessthan, contains, present, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<test_cond_expression_opts, true> test_cond_expression_e;

// TestCond-Type ::= CHOICE
struct test_cond_type_c {
  struct gbr_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<gbr_opts, true> gbr_e_;
  struct ambr_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<ambr_opts, true> ambr_e_;
  struct is_stat_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<is_stat_opts, true> is_stat_e_;
  struct is_cat_m_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<is_cat_m_opts, true> is_cat_m_e_;
  struct rsrp_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<rsrp_opts, true> rsrp_e_;
  struct rsrq_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<rsrq_opts, true> rsrq_e_;
  struct ul_r_srp_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<ul_r_srp_opts, true> ul_r_srp_e_;
  struct cqi_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<cqi_opts, true> cqi_e_;
  struct five_qi_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<five_qi_opts, true> five_qi_e_;
  struct qci_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<qci_opts, true> qci_e_;
  struct snssai_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<snssai_opts, true> snssai_e_;
  struct types_opts {
    enum options {
      gbr,
      ambr,
      is_stat,
      is_cat_m,
      rsrp,
      rsrq,
      /*...*/ ul_r_srp,
      cqi,
      five_qi,
      qci,
      snssai,
      nulltype
    } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts, true, 5> types;

  // choice methods
  test_cond_type_c() = default;
  test_cond_type_c(const test_cond_type_c& other);
  test_cond_type_c& operator=(const test_cond_type_c& other);
  ~test_cond_type_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  gbr_e_& gbr()
  {
    assert_choice_type(types::gbr, type_, "TestCond-Type");
    return c.get<gbr_e_>();
  }
  ambr_e_& ambr()
  {
    assert_choice_type(types::ambr, type_, "TestCond-Type");
    return c.get<ambr_e_>();
  }
  is_stat_e_& is_stat()
  {
    assert_choice_type(types::is_stat, type_, "TestCond-Type");
    return c.get<is_stat_e_>();
  }
  is_cat_m_e_& is_cat_m()
  {
    assert_choice_type(types::is_cat_m, type_, "TestCond-Type");
    return c.get<is_cat_m_e_>();
  }
  rsrp_e_& rsrp()
  {
    assert_choice_type(types::rsrp, type_, "TestCond-Type");
    return c.get<rsrp_e_>();
  }
  rsrq_e_& rsrq()
  {
    assert_choice_type(types::rsrq, type_, "TestCond-Type");
    return c.get<rsrq_e_>();
  }
  ul_r_srp_e_& ul_r_srp()
  {
    assert_choice_type(types::ul_r_srp, type_, "TestCond-Type");
    return c.get<ul_r_srp_e_>();
  }
  cqi_e_& cqi()
  {
    assert_choice_type(types::cqi, type_, "TestCond-Type");
    return c.get<cqi_e_>();
  }
  five_qi_e_& five_qi()
  {
    assert_choice_type(types::five_qi, type_, "TestCond-Type");
    return c.get<five_qi_e_>();
  }
  qci_e_& qci()
  {
    assert_choice_type(types::qci, type_, "TestCond-Type");
    return c.get<qci_e_>();
  }
  snssai_e_& snssai()
  {
    assert_choice_type(types::snssai, type_, "TestCond-Type");
    return c.get<snssai_e_>();
  }
  const gbr_e_& gbr() const
  {
    assert_choice_type(types::gbr, type_, "TestCond-Type");
    return c.get<gbr_e_>();
  }
  const ambr_e_& ambr() const
  {
    assert_choice_type(types::ambr, type_, "TestCond-Type");
    return c.get<ambr_e_>();
  }
  const is_stat_e_& is_stat() const
  {
    assert_choice_type(types::is_stat, type_, "TestCond-Type");
    return c.get<is_stat_e_>();
  }
  const is_cat_m_e_& is_cat_m() const
  {
    assert_choice_type(types::is_cat_m, type_, "TestCond-Type");
    return c.get<is_cat_m_e_>();
  }
  const rsrp_e_& rsrp() const
  {
    assert_choice_type(types::rsrp, type_, "TestCond-Type");
    return c.get<rsrp_e_>();
  }
  const rsrq_e_& rsrq() const
  {
    assert_choice_type(types::rsrq, type_, "TestCond-Type");
    return c.get<rsrq_e_>();
  }
  const ul_r_srp_e_& ul_r_srp() const
  {
    assert_choice_type(types::ul_r_srp, type_, "TestCond-Type");
    return c.get<ul_r_srp_e_>();
  }
  const cqi_e_& cqi() const
  {
    assert_choice_type(types::cqi, type_, "TestCond-Type");
    return c.get<cqi_e_>();
  }
  const five_qi_e_& five_qi() const
  {
    assert_choice_type(types::five_qi, type_, "TestCond-Type");
    return c.get<five_qi_e_>();
  }
  const qci_e_& qci() const
  {
    assert_choice_type(types::qci, type_, "TestCond-Type");
    return c.get<qci_e_>();
  }
  const snssai_e_& snssai() const
  {
    assert_choice_type(types::snssai, type_, "TestCond-Type");
    return c.get<snssai_e_>();
  }
  gbr_e_&      set_gbr();
  ambr_e_&     set_ambr();
  is_stat_e_&  set_is_stat();
  is_cat_m_e_& set_is_cat_m();
  rsrp_e_&     set_rsrp();
  rsrq_e_&     set_rsrq();
  ul_r_srp_e_& set_ul_r_srp();
  cqi_e_&      set_cqi();
  five_qi_e_&  set_five_qi();
  qci_e_&      set_qci();
  snssai_e_&   set_snssai();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// TestCond-Value ::= CHOICE
struct test_cond_value_c {
  struct types_opts {
    enum options {
      value_int,
      value_enum,
      value_bool,
      value_bit_s,
      value_oct_s,
      value_prt_s,
      /*...*/ value_real,
      nulltype
    } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true, 1> types;

  // choice methods
  test_cond_value_c() = default;
  test_cond_value_c(const test_cond_value_c& other);
  test_cond_value_c& operator=(const test_cond_value_c& other);
  ~test_cond_value_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  int64_t& value_int()
  {
    assert_choice_type(types::value_int, type_, "TestCond-Value");
    return c.get<int64_t>();
  }
  int64_t& value_enum()
  {
    assert_choice_type(types::value_enum, type_, "TestCond-Value");
    return c.get<int64_t>();
  }
  bool& value_bool()
  {
    assert_choice_type(types::value_bool, type_, "TestCond-Value");
    return c.get<bool>();
  }
  dyn_bitstring& value_bit_s()
  {
    assert_choice_type(types::value_bit_s, type_, "TestCond-Value");
    return c.get<dyn_bitstring>();
  }
  unbounded_octstring<true>& value_oct_s()
  {
    assert_choice_type(types::value_oct_s, type_, "TestCond-Value");
    return c.get<unbounded_octstring<true> >();
  }
  printable_string<0, None, false, true>& value_prt_s()
  {
    assert_choice_type(types::value_prt_s, type_, "TestCond-Value");
    return c.get<printable_string<0, None, false, true> >();
  }
  real_s& value_real()
  {
    assert_choice_type(types::value_real, type_, "TestCond-Value");
    return c.get<real_s>();
  }
  const int64_t& value_int() const
  {
    assert_choice_type(types::value_int, type_, "TestCond-Value");
    return c.get<int64_t>();
  }
  const int64_t& value_enum() const
  {
    assert_choice_type(types::value_enum, type_, "TestCond-Value");
    return c.get<int64_t>();
  }
  const bool& value_bool() const
  {
    assert_choice_type(types::value_bool, type_, "TestCond-Value");
    return c.get<bool>();
  }
  const dyn_bitstring& value_bit_s() const
  {
    assert_choice_type(types::value_bit_s, type_, "TestCond-Value");
    return c.get<dyn_bitstring>();
  }
  const unbounded_octstring<true>& value_oct_s() const
  {
    assert_choice_type(types::value_oct_s, type_, "TestCond-Value");
    return c.get<unbounded_octstring<true> >();
  }
  const printable_string<0, None, false, true>& value_prt_s() const
  {
    assert_choice_type(types::value_prt_s, type_, "TestCond-Value");
    return c.get<printable_string<0, None, false, true> >();
  }
  const real_s& value_real() const
  {
    assert_choice_type(types::value_real, type_, "TestCond-Value");
    return c.get<real_s>();
  }
  int64_t&                                set_value_int();
  int64_t&                                set_value_enum();
  bool&                                   set_value_bool();
  dyn_bitstring&                          set_value_bit_s();
  unbounded_octstring<true>&              set_value_oct_s();
  printable_string<0, None, false, true>& set_value_prt_s();
  real_s&                                 set_value_real();

private:
  types type_;
  choice_buffer_t<bool, dyn_bitstring, printable_string<0, None, false, true>, real_s, unbounded_octstring<true> > c;

  void destroy_();
};

// ENB-ID ::= CHOICE
struct enb_id_c {
  struct types_opts {
    enum options { macro_enb_id, home_enb_id, /*...*/ short_macro_enb_id, long_macro_enb_id, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true, 2> types;

  // choice methods
  enb_id_c() = default;
  enb_id_c(const enb_id_c& other);
  enb_id_c& operator=(const enb_id_c& other);
  ~enb_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<20, false, true>& macro_enb_id()
  {
    assert_choice_type(types::macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<20, false, true> >();
  }
  fixed_bitstring<28, false, true>& home_enb_id()
  {
    assert_choice_type(types::home_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<28, false, true> >();
  }
  fixed_bitstring<18, false, true>& short_macro_enb_id()
  {
    assert_choice_type(types::short_macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<18, false, true> >();
  }
  fixed_bitstring<21, false, true>& long_macro_enb_id()
  {
    assert_choice_type(types::long_macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<21, false, true> >();
  }
  const fixed_bitstring<20, false, true>& macro_enb_id() const
  {
    assert_choice_type(types::macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<20, false, true> >();
  }
  const fixed_bitstring<28, false, true>& home_enb_id() const
  {
    assert_choice_type(types::home_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<28, false, true> >();
  }
  const fixed_bitstring<18, false, true>& short_macro_enb_id() const
  {
    assert_choice_type(types::short_macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<18, false, true> >();
  }
  const fixed_bitstring<21, false, true>& long_macro_enb_id() const
  {
    assert_choice_type(types::long_macro_enb_id, type_, "ENB-ID");
    return c.get<fixed_bitstring<21, false, true> >();
  }
  fixed_bitstring<20, false, true>& set_macro_enb_id();
  fixed_bitstring<28, false, true>& set_home_enb_id();
  fixed_bitstring<18, false, true>& set_short_macro_enb_id();
  fixed_bitstring<21, false, true>& set_long_macro_enb_id();

private:
  types                                              type_;
  choice_buffer_t<fixed_bitstring<28, false, true> > c;

  void destroy_();
};

// GNB-ID ::= CHOICE
struct gnb_id_c {
  struct types_opts {
    enum options { gnb_id, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  types       type() const { return types::gnb_id; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  bounded_bitstring<22, 32, false, true>&       gnb_id() { return c; }
  const bounded_bitstring<22, 32, false, true>& gnb_id() const { return c; }

private:
  bounded_bitstring<22, 32, false, true> c;
};

// MeasurementLabel ::= SEQUENCE
struct meas_label_s {
  struct no_label_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<no_label_opts, true> no_label_e_;
  struct sum_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<sum_opts, true> sum_e_;
  struct pre_label_override_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<pre_label_override_opts, true> pre_label_override_e_;
  struct start_end_ind_opts {
    enum options { start, end, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<start_end_ind_opts, true> start_end_ind_e_;
  struct min_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<min_opts, true> min_e_;
  struct max_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<max_opts, true> max_e_;
  struct avg_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<avg_opts, true> avg_e_;

  // member variables
  bool                     ext                        = false;
  bool                     no_label_present           = false;
  bool                     plmn_id_present            = false;
  bool                     slice_id_present           = false;
  bool                     five_qi_present            = false;
  bool                     qfi_present                = false;
  bool                     qci_present                = false;
  bool                     qcimax_present             = false;
  bool                     qcimin_present             = false;
  bool                     arpmax_present             = false;
  bool                     arpmin_present             = false;
  bool                     bitrate_range_present      = false;
  bool                     layer_mu_mimo_present      = false;
  bool                     sum_present                = false;
  bool                     dist_bin_x_present         = false;
  bool                     dist_bin_y_present         = false;
  bool                     dist_bin_z_present         = false;
  bool                     pre_label_override_present = false;
  bool                     start_end_ind_present      = false;
  bool                     min_present                = false;
  bool                     max_present                = false;
  bool                     avg_present                = false;
  no_label_e_              no_label;
  fixed_octstring<3, true> plmn_id;
  s_nssai_s                slice_id;
  uint16_t                 five_qi       = 0;
  uint8_t                  qfi           = 0;
  uint16_t                 qci           = 0;
  uint16_t                 qcimax        = 0;
  uint16_t                 qcimin        = 0;
  uint8_t                  arpmax        = 1;
  uint8_t                  arpmin        = 1;
  uint32_t                 bitrate_range = 1;
  uint32_t                 layer_mu_mimo = 1;
  sum_e_                   sum;
  uint32_t                 dist_bin_x = 1;
  uint32_t                 dist_bin_y = 1;
  uint32_t                 dist_bin_z = 1;
  pre_label_override_e_    pre_label_override;
  start_end_ind_e_         start_end_ind;
  min_e_                   min;
  max_e_                   max;
  avg_e_                   avg;
  // ...
  bool     ssb_idx_present             = false;
  bool     non_go_b_bfmode_idx_present = false;
  bool     mimo_mode_idx_present       = false;
  uint32_t ssb_idx                     = 1;
  uint32_t non_go_b_bfmode_idx         = 1;
  uint8_t  mimo_mode_idx               = 1;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NgENB-ID ::= CHOICE
struct ng_enb_id_c {
  struct types_opts {
    enum options { macro_ng_enb_id, short_macro_ng_enb_id, long_macro_ng_enb_id, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  ng_enb_id_c() = default;
  ng_enb_id_c(const ng_enb_id_c& other);
  ng_enb_id_c& operator=(const ng_enb_id_c& other);
  ~ng_enb_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<20, false, true>& macro_ng_enb_id()
  {
    assert_choice_type(types::macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<20, false, true> >();
  }
  fixed_bitstring<18, false, true>& short_macro_ng_enb_id()
  {
    assert_choice_type(types::short_macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<18, false, true> >();
  }
  fixed_bitstring<21, false, true>& long_macro_ng_enb_id()
  {
    assert_choice_type(types::long_macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<21, false, true> >();
  }
  const fixed_bitstring<20, false, true>& macro_ng_enb_id() const
  {
    assert_choice_type(types::macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<20, false, true> >();
  }
  const fixed_bitstring<18, false, true>& short_macro_ng_enb_id() const
  {
    assert_choice_type(types::short_macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<18, false, true> >();
  }
  const fixed_bitstring<21, false, true>& long_macro_ng_enb_id() const
  {
    assert_choice_type(types::long_macro_ng_enb_id, type_, "NgENB-ID");
    return c.get<fixed_bitstring<21, false, true> >();
  }
  fixed_bitstring<20, false, true>& set_macro_ng_enb_id();
  fixed_bitstring<18, false, true>& set_short_macro_ng_enb_id();
  fixed_bitstring<21, false, true>& set_long_macro_ng_enb_id();

private:
  types                                              type_;
  choice_buffer_t<fixed_bitstring<21, false, true> > c;

  void destroy_();
};

// TestCondInfo ::= SEQUENCE
struct test_cond_info_s {
  bool                   ext                = false;
  bool                   test_expr_present  = false;
  bool                   test_value_present = false;
  test_cond_type_c       test_type;
  test_cond_expression_e test_expr;
  test_cond_value_c      test_value;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-GNB-CU-CP-E1AP-ID-Item ::= SEQUENCE
struct ueid_gnb_cu_cp_e1_ap_id_item_s {
  bool     ext                   = false;
  uint64_t gnb_cu_cp_ue_e1_ap_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-GNB-CU-CP-F1AP-ID-Item ::= SEQUENCE
struct ueid_gnb_cu_cp_f1_ap_id_item_s {
  bool     ext                = false;
  uint64_t gnb_cu_ue_f1_ap_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalENB-ID ::= SEQUENCE
struct global_enb_id_s {
  bool                     ext = false;
  fixed_octstring<3, true> plmn_id;
  enb_id_c                 enb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalGNB-ID ::= SEQUENCE
struct global_gnb_id_s {
  bool                     ext = false;
  fixed_octstring<3, true> plmn_id;
  gnb_id_c                 gnb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalNgENB-ID ::= SEQUENCE
struct global_ng_enb_id_s {
  bool                     ext = false;
  fixed_octstring<3, true> plmn_id;
  ng_enb_id_c              ng_enb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LabelInfoItem ::= SEQUENCE
struct label_info_item_s {
  bool         ext = false;
  meas_label_s meas_label;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// LogicalOR ::= ENUMERATED
struct lc_or_opts {
  enum options { true_value, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<lc_or_opts, true> lc_or_e;

// MatchingCondItem-Choice ::= CHOICE
struct matching_cond_item_choice_c {
  struct types_opts {
    enum options { meas_label, test_cond_info, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  matching_cond_item_choice_c() = default;
  matching_cond_item_choice_c(const matching_cond_item_choice_c& other);
  matching_cond_item_choice_c& operator=(const matching_cond_item_choice_c& other);
  ~matching_cond_item_choice_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  meas_label_s& meas_label()
  {
    assert_choice_type(types::meas_label, type_, "MatchingCondItem-Choice");
    return c.get<meas_label_s>();
  }
  test_cond_info_s& test_cond_info()
  {
    assert_choice_type(types::test_cond_info, type_, "MatchingCondItem-Choice");
    return c.get<test_cond_info_s>();
  }
  const meas_label_s& meas_label() const
  {
    assert_choice_type(types::meas_label, type_, "MatchingCondItem-Choice");
    return c.get<meas_label_s>();
  }
  const test_cond_info_s& test_cond_info() const
  {
    assert_choice_type(types::test_cond_info, type_, "MatchingCondItem-Choice");
    return c.get<test_cond_info_s>();
  }
  meas_label_s&     set_meas_label();
  test_cond_info_s& set_test_cond_info();

private:
  types                                           type_;
  choice_buffer_t<meas_label_s, test_cond_info_s> c;

  void destroy_();
};

// UEID-GNB-CU-CP-E1AP-ID-List ::= SEQUENCE (SIZE (1..65535)) OF UEID-GNB-CU-CP-E1AP-ID-Item
using ueid_gnb_cu_cp_e1_ap_id_list_l = dyn_array<ueid_gnb_cu_cp_e1_ap_id_item_s>;

// UEID-GNB-CU-F1AP-ID-List ::= SEQUENCE (SIZE (1..4)) OF UEID-GNB-CU-CP-F1AP-ID-Item
using ueid_gnb_cu_f1_ap_id_list_l = dyn_array<ueid_gnb_cu_cp_f1_ap_id_item_s>;

// LabelInfoList ::= SEQUENCE (SIZE (1..2147483647)) OF LabelInfoItem
using label_info_list_l = dyn_array<label_info_item_s>;

// MatchingCondItem ::= SEQUENCE
struct matching_cond_item_s {
  bool                        ext           = false;
  bool                        lc_or_present = false;
  matching_cond_item_choice_c matching_cond_choice;
  lc_or_e                     lc_or;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-EN-GNB ::= SEQUENCE
struct ueid_en_gnb_s {
  bool                           ext                          = false;
  bool                           m_enb_ue_x2ap_id_ext_present = false;
  bool                           gnb_cu_ue_f1_ap_id_present   = false;
  bool                           ran_ueid_present             = false;
  uint16_t                       m_enb_ue_x2ap_id             = 0;
  uint16_t                       m_enb_ue_x2ap_id_ext         = 0;
  global_enb_id_s                global_enb_id;
  uint64_t                       gnb_cu_ue_f1_ap_id = 0;
  ueid_gnb_cu_cp_e1_ap_id_list_l gnb_cu_cp_ue_e1_ap_id_list;
  fixed_octstring<8, true>       ran_ueid;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-ENB ::= SEQUENCE
struct ueid_enb_s {
  bool            ext                          = false;
  bool            m_enb_ue_x2ap_id_present     = false;
  bool            m_enb_ue_x2ap_id_ext_present = false;
  bool            global_enb_id_present        = false;
  uint64_t        mme_ue_s1ap_id               = 0;
  gummei_s        gummei;
  uint16_t        m_enb_ue_x2ap_id     = 0;
  uint16_t        m_enb_ue_x2ap_id_ext = 0;
  global_enb_id_s global_enb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-GNB ::= SEQUENCE
struct ueid_gnb_s {
  bool                           ext                          = false;
  bool                           ran_ueid_present             = false;
  bool                           m_ng_ran_ue_xn_ap_id_present = false;
  bool                           global_gnb_id_present        = false;
  uint64_t                       amf_ue_ngap_id               = 0;
  guami_s                        guami;
  ueid_gnb_cu_f1_ap_id_list_l    gnb_cu_ue_f1_ap_id_list;
  ueid_gnb_cu_cp_e1_ap_id_list_l gnb_cu_cp_ue_e1_ap_id_list;
  fixed_octstring<8, true>       ran_ueid;
  uint64_t                       m_ng_ran_ue_xn_ap_id = 0;
  global_gnb_id_s                global_gnb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-GNB-CU-UP ::= SEQUENCE
struct ueid_gnb_cu_up_s {
  bool                     ext                   = false;
  bool                     ran_ueid_present      = false;
  uint64_t                 gnb_cu_cp_ue_e1_ap_id = 0;
  fixed_octstring<8, true> ran_ueid;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-GNB-DU ::= SEQUENCE
struct ueid_gnb_du_s {
  bool                     ext                = false;
  bool                     ran_ueid_present   = false;
  uint64_t                 gnb_cu_ue_f1_ap_id = 0;
  fixed_octstring<8, true> ran_ueid;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-NG-ENB ::= SEQUENCE
struct ueid_ng_enb_s {
  bool               ext                           = false;
  bool               ng_enb_cu_ue_w1_ap_id_present = false;
  bool               m_ng_ran_ue_xn_ap_id_present  = false;
  bool               global_ng_enb_id_present      = false;
  uint64_t           amf_ue_ngap_id                = 0;
  guami_s            guami;
  uint64_t           ng_enb_cu_ue_w1_ap_id = 0;
  uint64_t           m_ng_ran_ue_xn_ap_id  = 0;
  global_ng_enb_id_s global_ng_enb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID-NG-ENB-DU ::= SEQUENCE
struct ueid_ng_enb_du_s {
  bool     ext                   = false;
  uint64_t ng_enb_cu_ue_w1_ap_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MatchingCondList ::= SEQUENCE (SIZE (1..32768)) OF MatchingCondItem
using matching_cond_list_l = dyn_array<matching_cond_item_s>;

// MeasurementInfoItem ::= SEQUENCE
struct meas_info_item_s {
  bool              ext = false;
  meas_type_c       meas_type;
  label_info_list_l label_info_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEID ::= CHOICE
struct ueid_c {
  struct types_opts {
    enum options {
      gnb_ueid,
      gnb_du_ueid,
      gnb_cu_up_ueid,
      ng_enb_ueid,
      ng_enb_du_ueid,
      en_g_nb_ueid,
      enb_ueid,
      // ...
      nulltype
    } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  ueid_c() = default;
  ueid_c(const ueid_c& other);
  ueid_c& operator=(const ueid_c& other);
  ~ueid_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  ueid_gnb_s& gnb_ueid()
  {
    assert_choice_type(types::gnb_ueid, type_, "UEID");
    return c.get<ueid_gnb_s>();
  }
  ueid_gnb_du_s& gnb_du_ueid()
  {
    assert_choice_type(types::gnb_du_ueid, type_, "UEID");
    return c.get<ueid_gnb_du_s>();
  }
  ueid_gnb_cu_up_s& gnb_cu_up_ueid()
  {
    assert_choice_type(types::gnb_cu_up_ueid, type_, "UEID");
    return c.get<ueid_gnb_cu_up_s>();
  }
  ueid_ng_enb_s& ng_enb_ueid()
  {
    assert_choice_type(types::ng_enb_ueid, type_, "UEID");
    return c.get<ueid_ng_enb_s>();
  }
  ueid_ng_enb_du_s& ng_enb_du_ueid()
  {
    assert_choice_type(types::ng_enb_du_ueid, type_, "UEID");
    return c.get<ueid_ng_enb_du_s>();
  }
  ueid_en_gnb_s& en_g_nb_ueid()
  {
    assert_choice_type(types::en_g_nb_ueid, type_, "UEID");
    return c.get<ueid_en_gnb_s>();
  }
  ueid_enb_s& enb_ueid()
  {
    assert_choice_type(types::enb_ueid, type_, "UEID");
    return c.get<ueid_enb_s>();
  }
  const ueid_gnb_s& gnb_ueid() const
  {
    assert_choice_type(types::gnb_ueid, type_, "UEID");
    return c.get<ueid_gnb_s>();
  }
  const ueid_gnb_du_s& gnb_du_ueid() const
  {
    assert_choice_type(types::gnb_du_ueid, type_, "UEID");
    return c.get<ueid_gnb_du_s>();
  }
  const ueid_gnb_cu_up_s& gnb_cu_up_ueid() const
  {
    assert_choice_type(types::gnb_cu_up_ueid, type_, "UEID");
    return c.get<ueid_gnb_cu_up_s>();
  }
  const ueid_ng_enb_s& ng_enb_ueid() const
  {
    assert_choice_type(types::ng_enb_ueid, type_, "UEID");
    return c.get<ueid_ng_enb_s>();
  }
  const ueid_ng_enb_du_s& ng_enb_du_ueid() const
  {
    assert_choice_type(types::ng_enb_du_ueid, type_, "UEID");
    return c.get<ueid_ng_enb_du_s>();
  }
  const ueid_en_gnb_s& en_g_nb_ueid() const
  {
    assert_choice_type(types::en_g_nb_ueid, type_, "UEID");
    return c.get<ueid_en_gnb_s>();
  }
  const ueid_enb_s& enb_ueid() const
  {
    assert_choice_type(types::enb_ueid, type_, "UEID");
    return c.get<ueid_enb_s>();
  }
  ueid_gnb_s&       set_gnb_ueid();
  ueid_gnb_du_s&    set_gnb_du_ueid();
  ueid_gnb_cu_up_s& set_gnb_cu_up_ueid();
  ueid_ng_enb_s&    set_ng_enb_ueid();
  ueid_ng_enb_du_s& set_ng_enb_du_ueid();
  ueid_en_gnb_s&    set_en_g_nb_ueid();
  ueid_enb_s&       set_enb_ueid();

private:
  types type_;
  choice_buffer_t<ueid_en_gnb_s,
                  ueid_enb_s,
                  ueid_gnb_cu_up_s,
                  ueid_gnb_du_s,
                  ueid_gnb_s,
                  ueid_ng_enb_du_s,
                  ueid_ng_enb_s>
      c;

  void destroy_();
};

// MatchingUEidPerSubItem ::= SEQUENCE
struct matching_ueid_per_sub_item_s {
  bool   ext = false;
  ueid_c ue_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MatchingUeCondPerSubItem ::= SEQUENCE
struct matching_ue_cond_per_sub_item_s {
  bool             ext = false;
  test_cond_info_s test_cond_info;
  // ...
  bool    lc_or_present = false;
  lc_or_e lc_or;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementCondItem ::= SEQUENCE
struct meas_cond_item_s {
  bool                 ext = false;
  meas_type_c          meas_type;
  matching_cond_list_l matching_cond;
  // ...
  copy_ptr<bin_range_definition_s> bin_range_def;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementInfoList ::= SEQUENCE (SIZE (1..65535)) OF MeasurementInfoItem
using meas_info_list_l = dyn_array<meas_info_item_s>;

// E2SM-KPM-ActionDefinition-Format1 ::= SEQUENCE
struct e2_sm_kpm_action_definition_format1_s {
  bool             ext                    = false;
  bool             cell_global_id_present = false;
  meas_info_list_l meas_info_list;
  uint64_t         granul_period = 1;
  cgi_c            cell_global_id;
  // ...
  copy_ptr<dist_meas_bin_range_list_l> dist_meas_bin_range_info;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MatchingUEidPerSubList ::= SEQUENCE (SIZE (2..65535)) OF MatchingUEidPerSubItem
using matching_ueid_per_sub_list_l = dyn_array<matching_ueid_per_sub_item_s>;

// MatchingUeCondPerSubList ::= SEQUENCE (SIZE (1..32768)) OF MatchingUeCondPerSubItem
using matching_ue_cond_per_sub_list_l = dyn_array<matching_ue_cond_per_sub_item_s>;

// MeasurementCondList ::= SEQUENCE (SIZE (1..65535)) OF MeasurementCondItem
using meas_cond_list_l = dyn_array<meas_cond_item_s>;

// E2SM-KPM-ActionDefinition-Format2 ::= SEQUENCE
struct e2_sm_kpm_action_definition_format2_s {
  bool                                  ext = false;
  ueid_c                                ue_id;
  e2_sm_kpm_action_definition_format1_s subscript_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-ActionDefinition-Format3 ::= SEQUENCE
struct e2_sm_kpm_action_definition_format3_s {
  bool             ext                    = false;
  bool             cell_global_id_present = false;
  meas_cond_list_l meas_cond_list;
  uint64_t         granul_period = 1;
  cgi_c            cell_global_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-ActionDefinition-Format4 ::= SEQUENCE
struct e2_sm_kpm_action_definition_format4_s {
  bool                                  ext = false;
  matching_ue_cond_per_sub_list_l       matching_ue_cond_list;
  e2_sm_kpm_action_definition_format1_s subscription_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-ActionDefinition-Format5 ::= SEQUENCE
struct e2_sm_kpm_action_definition_format5_s {
  bool                                  ext = false;
  matching_ueid_per_sub_list_l          matching_ueid_list;
  e2_sm_kpm_action_definition_format1_s subscription_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-ActionDefinition ::= SEQUENCE
struct e2_sm_kpm_action_definition_s {
  struct action_definition_formats_c_ {
    struct types_opts {
      enum options {
        action_definition_format1,
        action_definition_format2,
        action_definition_format3,
        // ...
        action_definition_format4,
        action_definition_format5,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts, true, 2> types;

    // choice methods
    action_definition_formats_c_() = default;
    action_definition_formats_c_(const action_definition_formats_c_& other);
    action_definition_formats_c_& operator=(const action_definition_formats_c_& other);
    ~action_definition_formats_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2_sm_kpm_action_definition_format1_s& action_definition_format1()
    {
      assert_choice_type(types::action_definition_format1, type_, "actionDefinition-formats");
      return c.get<e2_sm_kpm_action_definition_format1_s>();
    }
    e2_sm_kpm_action_definition_format2_s& action_definition_format2()
    {
      assert_choice_type(types::action_definition_format2, type_, "actionDefinition-formats");
      return c.get<e2_sm_kpm_action_definition_format2_s>();
    }
    e2_sm_kpm_action_definition_format3_s& action_definition_format3()
    {
      assert_choice_type(types::action_definition_format3, type_, "actionDefinition-formats");
      return c.get<e2_sm_kpm_action_definition_format3_s>();
    }
    e2_sm_kpm_action_definition_format4_s& action_definition_format4()
    {
      assert_choice_type(types::action_definition_format4, type_, "actionDefinition-formats");
      return c.get<e2_sm_kpm_action_definition_format4_s>();
    }
    e2_sm_kpm_action_definition_format5_s& action_definition_format5()
    {
      assert_choice_type(types::action_definition_format5, type_, "actionDefinition-formats");
      return c.get<e2_sm_kpm_action_definition_format5_s>();
    }
    const e2_sm_kpm_action_definition_format1_s& action_definition_format1() const
    {
      assert_choice_type(types::action_definition_format1, type_, "actionDefinition-formats");
      return c.get<e2_sm_kpm_action_definition_format1_s>();
    }
    const e2_sm_kpm_action_definition_format2_s& action_definition_format2() const
    {
      assert_choice_type(types::action_definition_format2, type_, "actionDefinition-formats");
      return c.get<e2_sm_kpm_action_definition_format2_s>();
    }
    const e2_sm_kpm_action_definition_format3_s& action_definition_format3() const
    {
      assert_choice_type(types::action_definition_format3, type_, "actionDefinition-formats");
      return c.get<e2_sm_kpm_action_definition_format3_s>();
    }
    const e2_sm_kpm_action_definition_format4_s& action_definition_format4() const
    {
      assert_choice_type(types::action_definition_format4, type_, "actionDefinition-formats");
      return c.get<e2_sm_kpm_action_definition_format4_s>();
    }
    const e2_sm_kpm_action_definition_format5_s& action_definition_format5() const
    {
      assert_choice_type(types::action_definition_format5, type_, "actionDefinition-formats");
      return c.get<e2_sm_kpm_action_definition_format5_s>();
    }
    e2_sm_kpm_action_definition_format1_s& set_action_definition_format1();
    e2_sm_kpm_action_definition_format2_s& set_action_definition_format2();
    e2_sm_kpm_action_definition_format3_s& set_action_definition_format3();
    e2_sm_kpm_action_definition_format4_s& set_action_definition_format4();
    e2_sm_kpm_action_definition_format5_s& set_action_definition_format5();

  private:
    types type_;
    choice_buffer_t<e2_sm_kpm_action_definition_format1_s,
                    e2_sm_kpm_action_definition_format2_s,
                    e2_sm_kpm_action_definition_format3_s,
                    e2_sm_kpm_action_definition_format4_s,
                    e2_sm_kpm_action_definition_format5_s>
        c;

    void destroy_();
  };

  // member variables
  bool                         ext            = false;
  int64_t                      ric_style_type = 0;
  action_definition_formats_c_ action_definition_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-EventTriggerDefinition-Format1 ::= SEQUENCE
struct e2_sm_kpm_event_trigger_definition_format1_s {
  bool     ext           = false;
  uint64_t report_period = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-EventTriggerDefinition ::= SEQUENCE
struct e2_sm_kpm_event_trigger_definition_s {
  struct event_definition_formats_c_ {
    struct types_opts {
      enum options { event_definition_format1, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts, true> types;

    // choice methods
    types       type() const { return types::event_definition_format1; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2_sm_kpm_event_trigger_definition_format1_s&       event_definition_format1() { return c; }
    const e2_sm_kpm_event_trigger_definition_format1_s& event_definition_format1() const { return c; }

  private:
    e2_sm_kpm_event_trigger_definition_format1_s c;
  };

  // member variables
  bool                        ext = false;
  event_definition_formats_c_ event_definition_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-IndicationHeader-Format1 ::= SEQUENCE
struct e2_sm_kpm_ind_hdr_format1_s {
  bool                                  ext                        = false;
  bool                                  file_formatversion_present = false;
  bool                                  sender_name_present        = false;
  bool                                  sender_type_present        = false;
  bool                                  vendor_name_present        = false;
  fixed_octstring<4, true>              collet_start_time;
  printable_string<0, 15, false, true>  file_formatversion;
  printable_string<0, 400, false, true> sender_name;
  printable_string<0, 8, false, true>   sender_type;
  printable_string<0, 32, false, true>  vendor_name;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-IndicationHeader ::= SEQUENCE
struct e2_sm_kpm_ind_hdr_s {
  struct ind_hdr_formats_c_ {
    struct types_opts {
      enum options { ind_hdr_format1, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts, true> types;

    // choice methods
    types       type() const { return types::ind_hdr_format1; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2_sm_kpm_ind_hdr_format1_s&       ind_hdr_format1() { return c; }
    const e2_sm_kpm_ind_hdr_format1_s& ind_hdr_format1() const { return c; }

  private:
    e2_sm_kpm_ind_hdr_format1_s c;
  };

  // member variables
  bool               ext = false;
  ind_hdr_formats_c_ ind_hdr_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementRecordItem ::= CHOICE
struct meas_record_item_c {
  struct types_opts {
    enum options { integer, real, no_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  meas_record_item_c() = default;
  meas_record_item_c(const meas_record_item_c& other);
  meas_record_item_c& operator=(const meas_record_item_c& other);
  ~meas_record_item_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint64_t& integer()
  {
    assert_choice_type(types::integer, type_, "MeasurementRecordItem");
    return c.get<uint64_t>();
  }
  real_s& real()
  {
    assert_choice_type(types::real, type_, "MeasurementRecordItem");
    return c.get<real_s>();
  }
  const uint64_t& integer() const
  {
    assert_choice_type(types::integer, type_, "MeasurementRecordItem");
    return c.get<uint64_t>();
  }
  const real_s& real() const
  {
    assert_choice_type(types::real, type_, "MeasurementRecordItem");
    return c.get<real_s>();
  }
  uint64_t& set_integer();
  real_s&   set_real();
  void      set_no_value();

private:
  types                   type_;
  choice_buffer_t<real_s> c;

  void destroy_();
};

// MatchingUEidItem-PerGP ::= SEQUENCE
struct matching_ueid_item_per_gp_s {
  bool   ext = false;
  ueid_c ue_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementRecord ::= SEQUENCE (SIZE (1..2147483647)) OF MeasurementRecordItem
using meas_record_l = dyn_array<meas_record_item_c>;

// MatchingUEidList-PerGP ::= SEQUENCE (SIZE (1..65535)) OF MatchingUEidItem-PerGP
using matching_ueid_list_per_gp_l = dyn_array<matching_ueid_item_per_gp_s>;

// MeasurementDataItem ::= SEQUENCE
struct meas_data_item_s {
  struct incomplete_flag_opts {
    enum options { true_value, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<incomplete_flag_opts, true> incomplete_flag_e_;

  // member variables
  bool               ext                     = false;
  bool               incomplete_flag_present = false;
  meas_record_l      meas_record;
  incomplete_flag_e_ incomplete_flag;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MatchingUEidItem ::= SEQUENCE
struct matching_ueid_item_s {
  bool   ext = false;
  ueid_c ue_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MatchingUEidPerGP-Item ::= SEQUENCE
struct matching_ueid_per_gp_item_s {
  struct matched_per_gp_c_ {
    struct no_uematched_opts {
      enum options { true_value, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<no_uematched_opts, true> no_uematched_e_;
    struct types_opts {
      enum options { no_uematched, one_or_more_uematched, /*...*/ nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts, true> types;

    // choice methods
    matched_per_gp_c_() = default;
    matched_per_gp_c_(const matched_per_gp_c_& other);
    matched_per_gp_c_& operator=(const matched_per_gp_c_& other);
    ~matched_per_gp_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    no_uematched_e_& no_uematched()
    {
      assert_choice_type(types::no_uematched, type_, "matchedPerGP");
      return c.get<no_uematched_e_>();
    }
    matching_ueid_list_per_gp_l& one_or_more_uematched()
    {
      assert_choice_type(types::one_or_more_uematched, type_, "matchedPerGP");
      return c.get<matching_ueid_list_per_gp_l>();
    }
    const no_uematched_e_& no_uematched() const
    {
      assert_choice_type(types::no_uematched, type_, "matchedPerGP");
      return c.get<no_uematched_e_>();
    }
    const matching_ueid_list_per_gp_l& one_or_more_uematched() const
    {
      assert_choice_type(types::one_or_more_uematched, type_, "matchedPerGP");
      return c.get<matching_ueid_list_per_gp_l>();
    }
    no_uematched_e_&             set_no_uematched();
    matching_ueid_list_per_gp_l& set_one_or_more_uematched();

  private:
    types                                        type_;
    choice_buffer_t<matching_ueid_list_per_gp_l> c;

    void destroy_();
  };

  // member variables
  bool              ext = false;
  matched_per_gp_c_ matched_per_gp;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementData ::= SEQUENCE (SIZE (1..65535)) OF MeasurementDataItem
using meas_data_l = dyn_array<meas_data_item_s>;

// E2SM-KPM-IndicationMessage-Format1 ::= SEQUENCE
struct e2_sm_kpm_ind_msg_format1_s {
  bool             ext                   = false;
  bool             granul_period_present = false;
  meas_data_l      meas_data;
  meas_info_list_l meas_info_list;
  uint64_t         granul_period = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MatchingUEidList ::= SEQUENCE (SIZE (1..65535)) OF MatchingUEidItem
using matching_ueid_list_l = dyn_array<matching_ueid_item_s>;

// MatchingUEidPerGP ::= SEQUENCE (SIZE (1..65535)) OF MatchingUEidPerGP-Item
using matching_ueid_per_gp_l = dyn_array<matching_ueid_per_gp_item_s>;

// MeasurementCondUEidItem ::= SEQUENCE
struct meas_cond_ueid_item_s {
  bool                 ext = false;
  meas_type_c          meas_type;
  matching_cond_list_l matching_cond;
  matching_ueid_list_l matching_ueid_list;
  // ...
  copy_ptr<matching_ueid_per_gp_l> matching_ueid_per_gp;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// UEMeasurementReportItem ::= SEQUENCE
struct ue_meas_report_item_s {
  bool                        ext = false;
  ueid_c                      ue_id;
  e2_sm_kpm_ind_msg_format1_s meas_report;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementCondUEidList ::= SEQUENCE (SIZE (1..65535)) OF MeasurementCondUEidItem
using meas_cond_ueid_list_l = dyn_array<meas_cond_ueid_item_s>;

// UEMeasurementReportList ::= SEQUENCE (SIZE (1..65535)) OF UEMeasurementReportItem
using ue_meas_report_list_l = dyn_array<ue_meas_report_item_s>;

// E2SM-KPM-IndicationMessage-Format2 ::= SEQUENCE
struct e2_sm_kpm_ind_msg_format2_s {
  bool                  ext                   = false;
  bool                  granul_period_present = false;
  meas_data_l           meas_data;
  meas_cond_ueid_list_l meas_cond_ueid_list;
  uint64_t              granul_period = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-IndicationMessage-Format3 ::= SEQUENCE
struct e2_sm_kpm_ind_msg_format3_s {
  bool                  ext = false;
  ue_meas_report_list_l ue_meas_report_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-IndicationMessage ::= SEQUENCE
struct e2_sm_kpm_ind_msg_s {
  struct ind_msg_formats_c_ {
    struct types_opts {
      enum options { ind_msg_format1, ind_msg_format2, /*...*/ ind_msg_format3, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts, true, 1> types;

    // choice methods
    ind_msg_formats_c_() = default;
    ind_msg_formats_c_(const ind_msg_formats_c_& other);
    ind_msg_formats_c_& operator=(const ind_msg_formats_c_& other);
    ~ind_msg_formats_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2_sm_kpm_ind_msg_format1_s& ind_msg_format1()
    {
      assert_choice_type(types::ind_msg_format1, type_, "indicationMessage-formats");
      return c.get<e2_sm_kpm_ind_msg_format1_s>();
    }
    e2_sm_kpm_ind_msg_format2_s& ind_msg_format2()
    {
      assert_choice_type(types::ind_msg_format2, type_, "indicationMessage-formats");
      return c.get<e2_sm_kpm_ind_msg_format2_s>();
    }
    e2_sm_kpm_ind_msg_format3_s& ind_msg_format3()
    {
      assert_choice_type(types::ind_msg_format3, type_, "indicationMessage-formats");
      return c.get<e2_sm_kpm_ind_msg_format3_s>();
    }
    const e2_sm_kpm_ind_msg_format1_s& ind_msg_format1() const
    {
      assert_choice_type(types::ind_msg_format1, type_, "indicationMessage-formats");
      return c.get<e2_sm_kpm_ind_msg_format1_s>();
    }
    const e2_sm_kpm_ind_msg_format2_s& ind_msg_format2() const
    {
      assert_choice_type(types::ind_msg_format2, type_, "indicationMessage-formats");
      return c.get<e2_sm_kpm_ind_msg_format2_s>();
    }
    const e2_sm_kpm_ind_msg_format3_s& ind_msg_format3() const
    {
      assert_choice_type(types::ind_msg_format3, type_, "indicationMessage-formats");
      return c.get<e2_sm_kpm_ind_msg_format3_s>();
    }
    e2_sm_kpm_ind_msg_format1_s& set_ind_msg_format1();
    e2_sm_kpm_ind_msg_format2_s& set_ind_msg_format2();
    e2_sm_kpm_ind_msg_format3_s& set_ind_msg_format3();

  private:
    types                                                                                                  type_;
    choice_buffer_t<e2_sm_kpm_ind_msg_format1_s, e2_sm_kpm_ind_msg_format2_s, e2_sm_kpm_ind_msg_format3_s> c;

    void destroy_();
  };

  // member variables
  bool               ext = false;
  ind_msg_formats_c_ ind_msg_formats;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementInfo-Action-Item ::= SEQUENCE
struct meas_info_action_item_s {
  bool                                 ext             = false;
  bool                                 meas_id_present = false;
  printable_string<1, 150, true, true> meas_name;
  uint32_t                             meas_id = 1;
  // ...
  copy_ptr<bin_range_definition_s> bin_range_def;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// MeasurementInfo-Action-List ::= SEQUENCE (SIZE (1..65535)) OF MeasurementInfo-Action-Item
using meas_info_action_list_l = dyn_array<meas_info_action_item_s>;

// RANfunction-Name ::= SEQUENCE
struct ra_nfunction_name_s {
  bool                                  ext                           = false;
  bool                                  ran_function_instance_present = false;
  printable_string<1, 150, true, true>  ran_function_short_name;
  printable_string<1, 1000, true, true> ran_function_e2_sm_oid;
  printable_string<1, 150, true, true>  ran_function_description;
  int64_t                               ran_function_instance = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RIC-EventTriggerStyle-Item ::= SEQUENCE
struct ric_event_trigger_style_item_s {
  bool                                 ext                          = false;
  int64_t                              ric_event_trigger_style_type = 0;
  printable_string<1, 150, true, true> ric_event_trigger_style_name;
  int64_t                              ric_event_trigger_format_type = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RIC-ReportStyle-Item ::= SEQUENCE
struct ric_report_style_item_s {
  bool                                 ext                   = false;
  int64_t                              ric_report_style_type = 0;
  printable_string<1, 150, true, true> ric_report_style_name;
  int64_t                              ric_action_format_type = 0;
  meas_info_action_list_l              meas_info_action_list;
  int64_t                              ric_ind_hdr_format_type = 0;
  int64_t                              ric_ind_msg_format_type = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-RANfunction-Description ::= SEQUENCE
struct e2_sm_kpm_ra_nfunction_description_s {
  using ric_event_trigger_style_list_l_ = dyn_array<ric_event_trigger_style_item_s>;
  using ric_report_style_list_l_        = dyn_array<ric_report_style_item_s>;

  // member variables
  bool                            ext = false;
  ra_nfunction_name_s             ran_function_name;
  ric_event_trigger_style_list_l_ ric_event_trigger_style_list;
  ric_report_style_list_l_        ric_report_style_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalRANNodeID ::= CHOICE
struct global_ran_node_id_c {
  struct types_opts {
    enum options { global_gnb_id, global_ng_enb_id, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  global_ran_node_id_c() = default;
  global_ran_node_id_c(const global_ran_node_id_c& other);
  global_ran_node_id_c& operator=(const global_ran_node_id_c& other);
  ~global_ran_node_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  global_gnb_id_s& global_gnb_id()
  {
    assert_choice_type(types::global_gnb_id, type_, "GlobalRANNodeID");
    return c.get<global_gnb_id_s>();
  }
  global_ng_enb_id_s& global_ng_enb_id()
  {
    assert_choice_type(types::global_ng_enb_id, type_, "GlobalRANNodeID");
    return c.get<global_ng_enb_id_s>();
  }
  const global_gnb_id_s& global_gnb_id() const
  {
    assert_choice_type(types::global_gnb_id, type_, "GlobalRANNodeID");
    return c.get<global_gnb_id_s>();
  }
  const global_ng_enb_id_s& global_ng_enb_id() const
  {
    assert_choice_type(types::global_ng_enb_id, type_, "GlobalRANNodeID");
    return c.get<global_ng_enb_id_s>();
  }
  global_gnb_id_s&    set_global_gnb_id();
  global_ng_enb_id_s& set_global_ng_enb_id();

private:
  types                                                type_;
  choice_buffer_t<global_gnb_id_s, global_ng_enb_id_s> c;

  void destroy_();
};

// EN-GNB-ID ::= CHOICE
struct en_gnb_id_c {
  struct types_opts {
    enum options { en_g_nb_id, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  types       type() const { return types::en_g_nb_id; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  bounded_bitstring<22, 32, false, true>&       en_g_nb_id() { return c; }
  const bounded_bitstring<22, 32, false, true>& en_g_nb_id() const { return c; }

private:
  bounded_bitstring<22, 32, false, true> c;
};

// GlobalenGNB-ID ::= SEQUENCE
struct globalen_gnb_id_s {
  bool                     ext = false;
  fixed_octstring<3, true> plmn_id;
  en_gnb_id_c              en_g_nb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GroupID ::= CHOICE
struct group_id_c {
  struct types_opts {
    enum options { five_gc, epc, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  group_id_c() = default;
  group_id_c(const group_id_c& other);
  group_id_c& operator=(const group_id_c& other);
  ~group_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& five_gc()
  {
    assert_choice_type(types::five_gc, type_, "GroupID");
    return c.get<uint16_t>();
  }
  uint16_t& epc()
  {
    assert_choice_type(types::epc, type_, "GroupID");
    return c.get<uint16_t>();
  }
  const uint16_t& five_gc() const
  {
    assert_choice_type(types::five_gc, type_, "GroupID");
    return c.get<uint16_t>();
  }
  const uint16_t& epc() const
  {
    assert_choice_type(types::epc, type_, "GroupID");
    return c.get<uint16_t>();
  }
  uint16_t& set_five_gc();
  uint16_t& set_epc();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// InterfaceID-E1 ::= SEQUENCE
struct interface_id_e1_s {
  bool                 ext = false;
  global_ran_node_id_c global_ng_ran_id;
  uint64_t             gnb_cu_up_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterfaceID-F1 ::= SEQUENCE
struct interface_id_f1_s {
  bool                 ext = false;
  global_ran_node_id_c global_ng_ran_id;
  uint64_t             gnb_du_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterfaceID-NG ::= SEQUENCE
struct interface_id_ng_s {
  bool    ext = false;
  guami_s guami;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterfaceID-S1 ::= SEQUENCE
struct interface_id_s1_s {
  bool     ext = false;
  gummei_s gummei;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterfaceID-W1 ::= SEQUENCE
struct interface_id_w1_s {
  bool               ext = false;
  global_ng_enb_id_s global_ng_enb_id;
  uint64_t           ng_enb_du_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterfaceID-X2 ::= SEQUENCE
struct interface_id_x2_s {
  struct node_type_c_ {
    struct types_opts {
      enum options { global_enb_id, global_en_g_nb_id, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts, true> types;

    // choice methods
    node_type_c_() = default;
    node_type_c_(const node_type_c_& other);
    node_type_c_& operator=(const node_type_c_& other);
    ~node_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    global_enb_id_s& global_enb_id()
    {
      assert_choice_type(types::global_enb_id, type_, "nodeType");
      return c.get<global_enb_id_s>();
    }
    globalen_gnb_id_s& global_en_g_nb_id()
    {
      assert_choice_type(types::global_en_g_nb_id, type_, "nodeType");
      return c.get<globalen_gnb_id_s>();
    }
    const global_enb_id_s& global_enb_id() const
    {
      assert_choice_type(types::global_enb_id, type_, "nodeType");
      return c.get<global_enb_id_s>();
    }
    const globalen_gnb_id_s& global_en_g_nb_id() const
    {
      assert_choice_type(types::global_en_g_nb_id, type_, "nodeType");
      return c.get<globalen_gnb_id_s>();
    }
    global_enb_id_s&   set_global_enb_id();
    globalen_gnb_id_s& set_global_en_g_nb_id();

  private:
    types                                               type_;
    choice_buffer_t<global_enb_id_s, globalen_gnb_id_s> c;

    void destroy_();
  };

  // member variables
  bool         ext = false;
  node_type_c_ node_type;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterfaceID-Xn ::= SEQUENCE
struct interface_id_xn_s {
  bool                 ext = false;
  global_ran_node_id_c global_ng_ran_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// InterfaceIdentifier ::= CHOICE
struct interface_id_c {
  struct types_opts {
    enum options { ng, xn, f1, e1, s1, x2, w1, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  interface_id_c() = default;
  interface_id_c(const interface_id_c& other);
  interface_id_c& operator=(const interface_id_c& other);
  ~interface_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  interface_id_ng_s& ng()
  {
    assert_choice_type(types::ng, type_, "InterfaceIdentifier");
    return c.get<interface_id_ng_s>();
  }
  interface_id_xn_s& xn()
  {
    assert_choice_type(types::xn, type_, "InterfaceIdentifier");
    return c.get<interface_id_xn_s>();
  }
  interface_id_f1_s& f1()
  {
    assert_choice_type(types::f1, type_, "InterfaceIdentifier");
    return c.get<interface_id_f1_s>();
  }
  interface_id_e1_s& e1()
  {
    assert_choice_type(types::e1, type_, "InterfaceIdentifier");
    return c.get<interface_id_e1_s>();
  }
  interface_id_s1_s& s1()
  {
    assert_choice_type(types::s1, type_, "InterfaceIdentifier");
    return c.get<interface_id_s1_s>();
  }
  interface_id_x2_s& x2()
  {
    assert_choice_type(types::x2, type_, "InterfaceIdentifier");
    return c.get<interface_id_x2_s>();
  }
  interface_id_w1_s& w1()
  {
    assert_choice_type(types::w1, type_, "InterfaceIdentifier");
    return c.get<interface_id_w1_s>();
  }
  const interface_id_ng_s& ng() const
  {
    assert_choice_type(types::ng, type_, "InterfaceIdentifier");
    return c.get<interface_id_ng_s>();
  }
  const interface_id_xn_s& xn() const
  {
    assert_choice_type(types::xn, type_, "InterfaceIdentifier");
    return c.get<interface_id_xn_s>();
  }
  const interface_id_f1_s& f1() const
  {
    assert_choice_type(types::f1, type_, "InterfaceIdentifier");
    return c.get<interface_id_f1_s>();
  }
  const interface_id_e1_s& e1() const
  {
    assert_choice_type(types::e1, type_, "InterfaceIdentifier");
    return c.get<interface_id_e1_s>();
  }
  const interface_id_s1_s& s1() const
  {
    assert_choice_type(types::s1, type_, "InterfaceIdentifier");
    return c.get<interface_id_s1_s>();
  }
  const interface_id_x2_s& x2() const
  {
    assert_choice_type(types::x2, type_, "InterfaceIdentifier");
    return c.get<interface_id_x2_s>();
  }
  const interface_id_w1_s& w1() const
  {
    assert_choice_type(types::w1, type_, "InterfaceIdentifier");
    return c.get<interface_id_w1_s>();
  }
  interface_id_ng_s& set_ng();
  interface_id_xn_s& set_xn();
  interface_id_f1_s& set_f1();
  interface_id_e1_s& set_e1();
  interface_id_s1_s& set_s1();
  interface_id_x2_s& set_x2();
  interface_id_w1_s& set_w1();

private:
  types type_;
  choice_buffer_t<interface_id_e1_s,
                  interface_id_f1_s,
                  interface_id_ng_s,
                  interface_id_s1_s,
                  interface_id_w1_s,
                  interface_id_x2_s,
                  interface_id_xn_s>
      c;

  void destroy_();
};

// FreqBandNrItem ::= SEQUENCE
struct freq_band_nr_item_s {
  bool     ext              = false;
  uint16_t freq_band_ind_nr = 1;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NR-ARFCN ::= SEQUENCE
struct nr_arfcn_s {
  using freq_band_list_nr_l_ = dyn_array<freq_band_nr_item_s>;

  // member variables
  bool                 ext     = false;
  uint32_t             nrarfcn = 0;
  freq_band_list_nr_l_ freq_band_list_nr;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// QoSID ::= CHOICE
struct qo_sid_c {
  struct types_opts {
    enum options { five_gc, epc, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  qo_sid_c() = default;
  qo_sid_c(const qo_sid_c& other);
  qo_sid_c& operator=(const qo_sid_c& other);
  ~qo_sid_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& five_gc()
  {
    assert_choice_type(types::five_gc, type_, "QoSID");
    return c.get<uint16_t>();
  }
  uint16_t& epc()
  {
    assert_choice_type(types::epc, type_, "QoSID");
    return c.get<uint16_t>();
  }
  const uint16_t& five_gc() const
  {
    assert_choice_type(types::five_gc, type_, "QoSID");
    return c.get<uint16_t>();
  }
  const uint16_t& epc() const
  {
    assert_choice_type(types::epc, type_, "QoSID");
    return c.get<uint16_t>();
  }
  uint16_t& set_five_gc();
  uint16_t& set_epc();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// RRCclass-LTE ::= ENUMERATED
struct rr_cclass_lte_opts {
  enum options {
    bcch_bch,
    bcch_bch_mbms,
    bcch_dl_sch,
    bcch_dl_sch_br,
    bcch_dl_sch_mbms,
    mcch,
    pcch,
    dl_ccch,
    dl_dcch,
    ul_ccch,
    ul_dcch,
    sc_mcch,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<rr_cclass_lte_opts, true> rr_cclass_lte_e;

// RRCclass-NR ::= ENUMERATED
struct rr_cclass_nr_opts {
  enum options { bcch_bch, bcch_dl_sch, dl_ccch, dl_dcch, pcch, ul_ccch, ul_ccch1, ul_dcch, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<rr_cclass_nr_opts, true> rr_cclass_nr_e;

// RRC-MessageID ::= SEQUENCE
struct rrc_msg_id_s {
  struct rrc_type_c_ {
    struct types_opts {
      enum options { lte, nr, /*...*/ nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts, true> types;

    // choice methods
    rrc_type_c_() = default;
    rrc_type_c_(const rrc_type_c_& other);
    rrc_type_c_& operator=(const rrc_type_c_& other);
    ~rrc_type_c_() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    rr_cclass_lte_e& lte()
    {
      assert_choice_type(types::lte, type_, "rrcType");
      return c.get<rr_cclass_lte_e>();
    }
    rr_cclass_nr_e& nr()
    {
      assert_choice_type(types::nr, type_, "rrcType");
      return c.get<rr_cclass_nr_e>();
    }
    const rr_cclass_lte_e& lte() const
    {
      assert_choice_type(types::lte, type_, "rrcType");
      return c.get<rr_cclass_lte_e>();
    }
    const rr_cclass_nr_e& nr() const
    {
      assert_choice_type(types::nr, type_, "rrcType");
      return c.get<rr_cclass_nr_e>();
    }
    rr_cclass_lte_e& set_lte();
    rr_cclass_nr_e&  set_nr();

  private:
    types               type_;
    pod_choice_buffer_t c;

    void destroy_();
  };

  // member variables
  bool        ext = false;
  rrc_type_c_ rrc_type;
  int64_t     msg_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServingCell-ARFCN ::= CHOICE
struct serving_cell_arfcn_c {
  struct types_opts {
    enum options { nr, eutra, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  serving_cell_arfcn_c() = default;
  serving_cell_arfcn_c(const serving_cell_arfcn_c& other);
  serving_cell_arfcn_c& operator=(const serving_cell_arfcn_c& other);
  ~serving_cell_arfcn_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  nr_arfcn_s& nr()
  {
    assert_choice_type(types::nr, type_, "ServingCell-ARFCN");
    return c.get<nr_arfcn_s>();
  }
  uint32_t& eutra()
  {
    assert_choice_type(types::eutra, type_, "ServingCell-ARFCN");
    return c.get<uint32_t>();
  }
  const nr_arfcn_s& nr() const
  {
    assert_choice_type(types::nr, type_, "ServingCell-ARFCN");
    return c.get<nr_arfcn_s>();
  }
  const uint32_t& eutra() const
  {
    assert_choice_type(types::eutra, type_, "ServingCell-ARFCN");
    return c.get<uint32_t>();
  }
  nr_arfcn_s& set_nr();
  uint32_t&   set_eutra();

private:
  types                       type_;
  choice_buffer_t<nr_arfcn_s> c;

  void destroy_();
};

// ServingCell-PCI ::= CHOICE
struct serving_cell_pci_c {
  struct types_opts {
    enum options { nr, eutra, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  serving_cell_pci_c() = default;
  serving_cell_pci_c(const serving_cell_pci_c& other);
  serving_cell_pci_c& operator=(const serving_cell_pci_c& other);
  ~serving_cell_pci_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  uint16_t& nr()
  {
    assert_choice_type(types::nr, type_, "ServingCell-PCI");
    return c.get<uint16_t>();
  }
  uint16_t& eutra()
  {
    assert_choice_type(types::eutra, type_, "ServingCell-PCI");
    return c.get<uint16_t>();
  }
  const uint16_t& nr() const
  {
    assert_choice_type(types::nr, type_, "ServingCell-PCI");
    return c.get<uint16_t>();
  }
  const uint16_t& eutra() const
  {
    assert_choice_type(types::eutra, type_, "ServingCell-PCI");
    return c.get<uint16_t>();
  }
  uint16_t& set_nr();
  uint16_t& set_eutra();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

} // namespace e2sm_kpm
} // namespace asn1
