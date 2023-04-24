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

#define ASN1_E2SM_KPM_MAXOF_MSG_PROTOCOL_TESTS 15
#define ASN1_E2SM_KPM_MAXOF_RICSTYLES 63
#define ASN1_E2SM_KPM_MAXNOOF_QCI 256
#define ASN1_E2SM_KPM_MAXNOOF_QOSFLOWS 64
#define ASN1_E2SM_KPM_MAXNOOF_SLICE_ITEMS 1024
#define ASN1_E2SM_KPM_MAXNOOF_CONTAINER_LIST_ITEMS 3
#define ASN1_E2SM_KPM_MAX_CELLING_NBDU 512
#define ASN1_E2SM_KPM_MAXOF_CONTAINERS 8
#define ASN1_E2SM_KPM_MAX_PLMN 12

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

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

// FQIPERSlicesPerPlmnListItem ::= SEQUENCE
struct fqiper_slices_per_plmn_list_item_s {
  bool     ext                   = false;
  bool     pdcp_bytes_dl_present = false;
  bool     pdcp_bytes_ul_present = false;
  uint16_t five_qi               = 0;
  uint64_t pdcp_bytes_dl         = 0;
  uint64_t pdcp_bytes_ul         = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SNSSAI ::= SEQUENCE
struct snssai_s {
  bool                     sd_present = false;
  fixed_octstring<1, true> sst;
  fixed_octstring<3, true> sd;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PerQCIReportListItemFormat ::= SEQUENCE
struct per_qci_report_list_item_format_s {
  bool     ext                   = false;
  bool     pdcp_bytes_dl_present = false;
  bool     pdcp_bytes_ul_present = false;
  uint16_t qci                   = 0;
  uint64_t pdcp_bytes_dl         = 0;
  uint64_t pdcp_bytes_ul         = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SliceToReportListItem ::= SEQUENCE
struct slice_to_report_list_item_s {
  using fqiper_slices_per_plmn_list_l_ = dyn_array<fqiper_slices_per_plmn_list_item_s>;

  // member variables
  bool                           ext = false;
  snssai_s                       slice_id;
  fqiper_slices_per_plmn_list_l_ fqiper_slices_per_plmn_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EPC-CUUP-PM-Format ::= SEQUENCE
struct epc_cuup_pm_format_s {
  using per_qci_report_list_l_ = dyn_array<per_qci_report_list_item_format_s>;

  // member variables
  bool                   ext = false;
  per_qci_report_list_l_ per_qci_report_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FGC-CUUP-PM-Format ::= SEQUENCE
struct fgc_cuup_pm_format_s {
  using slice_to_report_list_l_ = dyn_array<slice_to_report_list_item_s>;

  // member variables
  bool                    ext = false;
  slice_to_report_list_l_ slice_to_report_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PlmnID-List ::= SEQUENCE
struct plmn_id_list_s {
  bool                     ext                        = false;
  bool                     cu_up_pm_minus5_gc_present = false;
  bool                     cu_up_pm_epc_present       = false;
  fixed_octstring<3, true> plmn_id;
  fgc_cuup_pm_format_s     cu_up_pm_minus5_gc;
  epc_cuup_pm_format_s     cu_up_pm_epc;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CUUPMeasurement-Container ::= SEQUENCE
struct cuup_meas_container_s {
  using plmn_list_l_ = dyn_array<plmn_id_list_s>;

  // member variables
  bool         ext = false;
  plmn_list_l_ plmn_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FQIPERSlicesPerPlmnPerCellListItem ::= SEQUENCE
struct fqiper_slices_per_plmn_per_cell_list_item_s {
  bool     ext                 = false;
  bool     dl_prbusage_present = false;
  bool     ul_prbusage_present = false;
  uint16_t five_qi             = 0;
  uint8_t  dl_prbusage         = 0;
  uint8_t  ul_prbusage         = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PerQCIReportListItem ::= SEQUENCE
struct per_qci_report_list_item_s {
  bool     ext                 = false;
  bool     dl_prbusage_present = false;
  bool     ul_prbusage_present = false;
  uint16_t qci                 = 0;
  uint8_t  dl_prbusage         = 0;
  uint8_t  ul_prbusage         = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// SlicePerPlmnPerCellListItem ::= SEQUENCE
struct slice_per_plmn_per_cell_list_item_s {
  using fqiper_slices_per_plmn_per_cell_list_l_ = dyn_array<fqiper_slices_per_plmn_per_cell_list_item_s>;

  // member variables
  bool                                    ext = false;
  snssai_s                                slice_id;
  fqiper_slices_per_plmn_per_cell_list_l_ fqiper_slices_per_plmn_per_cell_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// EPC-DU-PM-Container ::= SEQUENCE
struct epc_du_pm_container_s {
  using per_qci_report_list_l_ = dyn_array<per_qci_report_list_item_s>;

  // member variables
  bool                   ext = false;
  per_qci_report_list_l_ per_qci_report_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// FGC-DU-PM-Container ::= SEQUENCE
struct fgc_du_pm_container_s {
  using slice_per_plmn_per_cell_list_l_ = dyn_array<slice_per_plmn_per_cell_list_item_s>;

  // member variables
  bool                            ext = false;
  slice_per_plmn_per_cell_list_l_ slice_per_plmn_per_cell_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// NRCGI ::= SEQUENCE
struct nrcgi_s {
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<36, false, true> nrcell_id;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ServedPlmnPerCellListItem ::= SEQUENCE
struct served_plmn_per_cell_list_item_s {
  bool                     ext                     = false;
  bool                     du_pm_minus5_gc_present = false;
  bool                     du_pm_epc_present       = false;
  fixed_octstring<3, true> plmn_id;
  fgc_du_pm_container_s    du_pm_minus5_gc;
  epc_du_pm_container_s    du_pm_epc;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CellResourceReportListItem ::= SEQUENCE
struct cell_res_report_list_item_s {
  using served_plmn_per_cell_list_l_ = dyn_array<served_plmn_per_cell_list_item_s>;

  // member variables
  bool                         ext                               = false;
  bool                         dl_totalof_available_prbs_present = false;
  bool                         ul_totalof_available_prbs_present = false;
  nrcgi_s                      nrcgi;
  uint8_t                      dl_totalof_available_prbs = 0;
  uint8_t                      ul_totalof_available_prbs = 0;
  served_plmn_per_cell_list_l_ served_plmn_per_cell_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
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

// E2SM-KPM-ActionDefinition ::= SEQUENCE
struct e2_sm_kpm_action_definition_s {
  bool    ext            = false;
  int64_t ric_style_type = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RT-Period-IE ::= ENUMERATED
struct rt_period_ie_opts {
  enum options {
    ms10,
    ms20,
    ms32,
    ms40,
    ms60,
    ms64,
    ms70,
    ms80,
    ms128,
    ms160,
    ms256,
    ms320,
    ms512,
    ms640,
    ms1024,
    ms1280,
    ms2048,
    ms2560,
    ms5120,
    ms10240,
    // ...
    nulltype
  } value;
  typedef uint16_t number_type;

  const char* to_string() const;
  uint16_t    to_number() const;
};
typedef enumerated<rt_period_ie_opts, true> rt_period_ie_e;

// Trigger-ConditionIE-Item ::= SEQUENCE
struct trigger_condition_ie_item_s {
  bool           ext = false;
  rt_period_ie_e report_period_ie;
  // ...

  // sequence methRaods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-EventTriggerDefinition-Format1 ::= SEQUENCE
struct e2_sm_kpm_event_trigger_definition_format1_s {
  using policy_test_list_l_ = dyn_array<trigger_condition_ie_item_s>;

  // member variables
  bool                ext = false;
  policy_test_list_l_ policy_test_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-EventTriggerDefinition ::= CHOICE
struct e2_sm_kpm_event_trigger_definition_c {
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

// ENB-ID-Choice ::= CHOICE
struct enb_id_choice_c {
  struct types_opts {
    enum options { enb_id_macro, enb_id_shortmacro, enb_id_longmacro, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  enb_id_choice_c() = default;
  enb_id_choice_c(const enb_id_choice_c& other);
  enb_id_choice_c& operator=(const enb_id_choice_c& other);
  ~enb_id_choice_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  fixed_bitstring<20, false, true>& enb_id_macro()
  {
    assert_choice_type(types::enb_id_macro, type_, "ENB-ID-Choice");
    return c.get<fixed_bitstring<20, false, true> >();
  }
  fixed_bitstring<18, false, true>& enb_id_shortmacro()
  {
    assert_choice_type(types::enb_id_shortmacro, type_, "ENB-ID-Choice");
    return c.get<fixed_bitstring<18, false, true> >();
  }
  fixed_bitstring<21, false, true>& enb_id_longmacro()
  {
    assert_choice_type(types::enb_id_longmacro, type_, "ENB-ID-Choice");
    return c.get<fixed_bitstring<21, false, true> >();
  }
  const fixed_bitstring<20, false, true>& enb_id_macro() const
  {
    assert_choice_type(types::enb_id_macro, type_, "ENB-ID-Choice");
    return c.get<fixed_bitstring<20, false, true> >();
  }
  const fixed_bitstring<18, false, true>& enb_id_shortmacro() const
  {
    assert_choice_type(types::enb_id_shortmacro, type_, "ENB-ID-Choice");
    return c.get<fixed_bitstring<18, false, true> >();
  }
  const fixed_bitstring<21, false, true>& enb_id_longmacro() const
  {
    assert_choice_type(types::enb_id_longmacro, type_, "ENB-ID-Choice");
    return c.get<fixed_bitstring<21, false, true> >();
  }
  fixed_bitstring<20, false, true>& set_enb_id_macro();
  fixed_bitstring<18, false, true>& set_enb_id_shortmacro();
  fixed_bitstring<21, false, true>& set_enb_id_longmacro();

private:
  types                                              type_;
  choice_buffer_t<fixed_bitstring<21, false, true> > c;

  void destroy_();
};

// ENGNB-ID ::= CHOICE
struct engnb_id_c {
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

// GNB-ID-Choice ::= CHOICE
struct gnb_id_choice_c {
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

// GlobalenGNB-ID ::= SEQUENCE
struct globalen_gnb_id_s {
  bool                     ext = false;
  fixed_octstring<3, true> plmn_id;
  engnb_id_c               gnb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalgNB-ID ::= SEQUENCE
struct globalg_nb_id_s {
  bool                     ext = false;
  fixed_octstring<3, true> plmn_id;
  gnb_id_choice_c          gnb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalngeNB-ID ::= SEQUENCE
struct globalngenb_id_s {
  bool                     ext = false;
  fixed_octstring<3, true> plmn_id;
  enb_id_choice_c          enb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalKPMnode-eNB-ID ::= SEQUENCE
struct global_kp_mnode_enb_id_s {
  bool            ext = false;
  global_enb_id_s global_enb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalKPMnode-en-gNB-ID ::= SEQUENCE
struct global_kp_mnode_en_g_nb_id_s {
  bool              ext = false;
  globalen_gnb_id_s global_g_nb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalKPMnode-gNB-ID ::= SEQUENCE
struct global_kp_mnode_g_nb_id_s {
  bool            ext                  = false;
  bool            gnb_cu_up_id_present = false;
  bool            gnb_du_id_present    = false;
  globalg_nb_id_s global_g_nb_id;
  uint64_t        gnb_cu_up_id = 0;
  uint64_t        gnb_du_id    = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalKPMnode-ng-eNB-ID ::= SEQUENCE
struct global_kp_mnode_ng_enb_id_s {
  bool             ext = false;
  globalngenb_id_s global_ng_enb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalKPMnode-ID ::= CHOICE
struct global_kp_mnode_id_c {
  struct types_opts {
    enum options { gnb, en_g_nb, ng_enb, enb, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  global_kp_mnode_id_c() = default;
  global_kp_mnode_id_c(const global_kp_mnode_id_c& other);
  global_kp_mnode_id_c& operator=(const global_kp_mnode_id_c& other);
  ~global_kp_mnode_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  global_kp_mnode_g_nb_id_s& gnb()
  {
    assert_choice_type(types::gnb, type_, "GlobalKPMnode-ID");
    return c.get<global_kp_mnode_g_nb_id_s>();
  }
  global_kp_mnode_en_g_nb_id_s& en_g_nb()
  {
    assert_choice_type(types::en_g_nb, type_, "GlobalKPMnode-ID");
    return c.get<global_kp_mnode_en_g_nb_id_s>();
  }
  global_kp_mnode_ng_enb_id_s& ng_enb()
  {
    assert_choice_type(types::ng_enb, type_, "GlobalKPMnode-ID");
    return c.get<global_kp_mnode_ng_enb_id_s>();
  }
  global_kp_mnode_enb_id_s& enb()
  {
    assert_choice_type(types::enb, type_, "GlobalKPMnode-ID");
    return c.get<global_kp_mnode_enb_id_s>();
  }
  const global_kp_mnode_g_nb_id_s& gnb() const
  {
    assert_choice_type(types::gnb, type_, "GlobalKPMnode-ID");
    return c.get<global_kp_mnode_g_nb_id_s>();
  }
  const global_kp_mnode_en_g_nb_id_s& en_g_nb() const
  {
    assert_choice_type(types::en_g_nb, type_, "GlobalKPMnode-ID");
    return c.get<global_kp_mnode_en_g_nb_id_s>();
  }
  const global_kp_mnode_ng_enb_id_s& ng_enb() const
  {
    assert_choice_type(types::ng_enb, type_, "GlobalKPMnode-ID");
    return c.get<global_kp_mnode_ng_enb_id_s>();
  }
  const global_kp_mnode_enb_id_s& enb() const
  {
    assert_choice_type(types::enb, type_, "GlobalKPMnode-ID");
    return c.get<global_kp_mnode_enb_id_s>();
  }
  global_kp_mnode_g_nb_id_s&    set_gnb();
  global_kp_mnode_en_g_nb_id_s& set_en_g_nb();
  global_kp_mnode_ng_enb_id_s&  set_ng_enb();
  global_kp_mnode_enb_id_s&     set_enb();

private:
  types type_;
  choice_buffer_t<global_kp_mnode_en_g_nb_id_s,
                  global_kp_mnode_enb_id_s,
                  global_kp_mnode_g_nb_id_s,
                  global_kp_mnode_ng_enb_id_s>
      c;

  void destroy_();
};

// E2SM-KPM-IndicationHeader-Format1 ::= SEQUENCE
struct e2_sm_kpm_ind_hdr_format1_s {
  bool                     ext                           = false;
  bool                     id_global_kp_mnode_id_present = false;
  bool                     nrcgi_present                 = false;
  bool                     plmn_id_present               = false;
  bool                     slice_id_present              = false;
  bool                     five_qi_present               = false;
  bool                     qci_present                   = false;
  global_kp_mnode_id_c     id_global_kp_mnode_id;
  nrcgi_s                  nrcgi;
  fixed_octstring<3, true> plmn_id;
  snssai_s                 slice_id;
  uint16_t                 five_qi = 0;
  uint16_t                 qci     = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-IndicationHeader ::= CHOICE
struct e2_sm_kpm_ind_hdr_c {
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

// NI-Type ::= ENUMERATED
struct ni_type_opts {
  enum options { x2_u, xn_u, f1_u, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<ni_type_opts, true> ni_type_e;

// PF-ContainerListItem ::= SEQUENCE
struct pf_container_list_item_s {
  bool                  ext = false;
  ni_type_e             interface_type;
  cuup_meas_container_s o_cu_up_pm_container;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OCUCP-PF-Container ::= SEQUENCE
struct ocucp_pf_container_s {
  struct cu_cp_res_status_s_ {
    bool     nof_active_ues_present = false;
    uint32_t nof_active_ues         = 1;
  };

  // member variables
  bool                                 gnb_cu_cp_name_present = false;
  printable_string<1, 150, true, true> gnb_cu_cp_name;
  cu_cp_res_status_s_                  cu_cp_res_status;

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// OCUUP-PF-Container ::= SEQUENCE
struct ocuup_pf_container_s {
  using pf_container_list_l_ = dyn_array<pf_container_list_item_s>;

  // member variables
  bool                                 ext                    = false;
  bool                                 gnb_cu_up_name_present = false;
  printable_string<1, 150, true, true> gnb_cu_up_name;
  pf_container_list_l_                 pf_container_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ODU-PF-Container ::= SEQUENCE
struct odu_pf_container_s {
  using cell_res_report_list_l_ = dyn_array<cell_res_report_list_item_s>;

  // member variables
  bool                    ext = false;
  cell_res_report_list_l_ cell_res_report_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// PF-Container ::= CHOICE
struct pf_container_c {
  struct types_opts {
    enum options { odu, ocu_cp, ocu_up, nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  pf_container_c() = default;
  pf_container_c(const pf_container_c& other);
  pf_container_c& operator=(const pf_container_c& other);
  ~pf_container_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  odu_pf_container_s& odu()
  {
    assert_choice_type(types::odu, type_, "PF-Container");
    return c.get<odu_pf_container_s>();
  }
  ocucp_pf_container_s& ocu_cp()
  {
    assert_choice_type(types::ocu_cp, type_, "PF-Container");
    return c.get<ocucp_pf_container_s>();
  }
  ocuup_pf_container_s& ocu_up()
  {
    assert_choice_type(types::ocu_up, type_, "PF-Container");
    return c.get<ocuup_pf_container_s>();
  }
  const odu_pf_container_s& odu() const
  {
    assert_choice_type(types::odu, type_, "PF-Container");
    return c.get<odu_pf_container_s>();
  }
  const ocucp_pf_container_s& ocu_cp() const
  {
    assert_choice_type(types::ocu_cp, type_, "PF-Container");
    return c.get<ocucp_pf_container_s>();
  }
  const ocuup_pf_container_s& ocu_up() const
  {
    assert_choice_type(types::ocu_up, type_, "PF-Container");
    return c.get<ocuup_pf_container_s>();
  }
  odu_pf_container_s&   set_odu();
  ocucp_pf_container_s& set_ocu_cp();
  ocuup_pf_container_s& set_ocu_up();

private:
  types                                                                           type_;
  choice_buffer_t<ocucp_pf_container_s, ocuup_pf_container_s, odu_pf_container_s> c;

  void destroy_();
};

// PM-Containers-List ::= SEQUENCE
struct pm_containers_list_s {
  bool                      ext                           = false;
  bool                      performance_container_present = false;
  pf_container_c            performance_container;
  unbounded_octstring<true> the_ran_container;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-IndicationMessage-Format1 ::= SEQUENCE
struct e2_sm_kpm_ind_msg_format1_s {
  using pm_containers_l_ = dyn_array<pm_containers_list_s>;

  // member variables
  bool             ext = false;
  pm_containers_l_ pm_containers;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2SM-KPM-IndicationMessage ::= CHOICE
struct e2_sm_kpm_ind_msg_c {
  struct types_opts {
    enum options { ric_style_type, ind_msg_format1, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  e2_sm_kpm_ind_msg_c() = default;
  e2_sm_kpm_ind_msg_c(const e2_sm_kpm_ind_msg_c& other);
  e2_sm_kpm_ind_msg_c& operator=(const e2_sm_kpm_ind_msg_c& other);
  ~e2_sm_kpm_ind_msg_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  int64_t& ric_style_type()
  {
    assert_choice_type(types::ric_style_type, type_, "E2SM-KPM-IndicationMessage");
    return c.get<int64_t>();
  }
  e2_sm_kpm_ind_msg_format1_s& ind_msg_format1()
  {
    assert_choice_type(types::ind_msg_format1, type_, "E2SM-KPM-IndicationMessage");
    return c.get<e2_sm_kpm_ind_msg_format1_s>();
  }
  const int64_t& ric_style_type() const
  {
    assert_choice_type(types::ric_style_type, type_, "E2SM-KPM-IndicationMessage");
    return c.get<int64_t>();
  }
  const e2_sm_kpm_ind_msg_format1_s& ind_msg_format1() const
  {
    assert_choice_type(types::ind_msg_format1, type_, "E2SM-KPM-IndicationMessage");
    return c.get<e2_sm_kpm_ind_msg_format1_s>();
  }
  int64_t&                     set_ric_style_type();
  e2_sm_kpm_ind_msg_format1_s& set_ind_msg_format1();

private:
  types                                        type_;
  choice_buffer_t<e2_sm_kpm_ind_msg_format1_s> c;

  void destroy_();
};

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

// RIC-EventTriggerStyle-List ::= SEQUENCE
struct ric_event_trigger_style_list_s {
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

// RIC-ReportStyle-List ::= SEQUENCE
struct ric_report_style_list_s {
  bool                                 ext                   = false;
  int64_t                              ric_report_style_type = 0;
  printable_string<1, 150, true, true> ric_report_style_name;
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
  struct e2_sm_kpm_ra_nfunction_item_s_ {
    using ric_event_trigger_style_list_l_ = dyn_array<ric_event_trigger_style_list_s>;
    using ric_report_style_list_l_        = dyn_array<ric_report_style_list_s>;

    // member variables
    bool                            ext = false;
    ric_event_trigger_style_list_l_ ric_event_trigger_style_list;
    ric_report_style_list_l_        ric_report_style_list;
    // ...
  };

  // member variables
  bool                           ext = false;
  ra_nfunction_name_s            ran_function_name;
  e2_sm_kpm_ra_nfunction_item_s_ e2_sm_kpm_ra_nfunction_item;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
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

// UEID-GNB-CU-CP-E1AP-ID-List ::= SEQUENCE (SIZE (1..65535)) OF UEID-GNB-CU-CP-E1AP-ID-Item
using ueid_gnb_cu_cp_e1_ap_id_list_l = dyn_array<ueid_gnb_cu_cp_e1_ap_id_item_s>;

// UEID-GNB-CU-F1AP-ID-List ::= SEQUENCE (SIZE (1..4)) OF UEID-GNB-CU-CP-F1AP-ID-Item
using ueid_gnb_cu_f1_ap_id_list_l = dyn_array<ueid_gnb_cu_cp_f1_ap_id_item_s>;

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

} // namespace e2sm_kpm
} // namespace asn1
