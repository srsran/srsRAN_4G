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
 *                     3GPP TS ASN1 E2AP v15.3.0 (2019-03)
 *
 ******************************************************************************/

#ifndef SRSASN1_E2AP_H
#define SRSASN1_E2AP_H

#include "asn1_utils.h"
#include <cstdio>
#include <stdarg.h>

namespace asn1 {
namespace e2ap {

/*******************************************************************************
 *                             Constant Definitions
 ******************************************************************************/

#define ASN1_E2AP_ID_E2SETUP 1
#define ASN1_E2AP_ID_ERROR_IND 2
#define ASN1_E2AP_ID_RESET 3
#define ASN1_E2AP_ID_RI_CCTRL 4
#define ASN1_E2AP_ID_RI_CIND 5
#define ASN1_E2AP_ID_RICSERVICE_QUERY 6
#define ASN1_E2AP_ID_RICSERVICE_UPD 7
#define ASN1_E2AP_ID_RICSUBSCRIPTION 8
#define ASN1_E2AP_ID_RICSUBSCRIPTION_DELETE 9
#define ASN1_E2AP_ID_E2NODE_CFG_UPD 10
#define ASN1_E2AP_ID_E2CONN_UPD 11
#define ASN1_E2AP_ID_RICSUBSCRIPTION_DELETE_REQUIRED 12
#define ASN1_E2AP_ID_E2REMOVAL 13
#define ASN1_E2AP_MAX_PROTOCOL_IES 65535
#define ASN1_E2AP_MAXNOOF_ERRORS 256
#define ASN1_E2AP_MAXOF_E2NODE_COMPONENTS 1024
#define ASN1_E2AP_MAXOF_RA_NFUNCTION_ID 256
#define ASN1_E2AP_MAXOF_RI_CACTION_ID 16
#define ASN1_E2AP_MAXOF_TNLA 32
#define ASN1_E2AP_MAXOF_RI_CREQUEST_ID 1024
#define ASN1_E2AP_ID_CAUSE 1
#define ASN1_E2AP_ID_CRIT_DIAGNOSTICS 2
#define ASN1_E2AP_ID_GLOBAL_E2NODE_ID 3
#define ASN1_E2AP_ID_GLOBAL_RIC_ID 4
#define ASN1_E2AP_ID_RA_NFUNCTION_ID 5
#define ASN1_E2AP_ID_RA_NFUNCTION_ID_ITEM 6
#define ASN1_E2AP_ID_RA_NFUNCTION_IECAUSE_ITEM 7
#define ASN1_E2AP_ID_RA_NFUNCTION_ITEM 8
#define ASN1_E2AP_ID_RA_NFUNCTIONS_ACCEPTED 9
#define ASN1_E2AP_ID_RA_NFUNCTIONS_ADDED 10
#define ASN1_E2AP_ID_RA_NFUNCTIONS_DELETED 11
#define ASN1_E2AP_ID_RA_NFUNCTIONS_MODIFIED 12
#define ASN1_E2AP_ID_RA_NFUNCTIONS_REJECTED 13
#define ASN1_E2AP_ID_RI_CACTION_ADMITTED_ITEM 14
#define ASN1_E2AP_ID_RI_CACTION_ID 15
#define ASN1_E2AP_ID_RI_CACTION_NOT_ADMITTED_ITEM 16
#define ASN1_E2AP_ID_RI_CACTIONS_ADMITTED 17
#define ASN1_E2AP_ID_RI_CACTIONS_NOT_ADMITTED 18
#define ASN1_E2AP_ID_RI_CACTION_TO_BE_SETUP_ITEM 19
#define ASN1_E2AP_ID_RI_CCALL_PROCESS_ID 20
#define ASN1_E2AP_ID_RI_CCTRL_ACK_REQUEST 21
#define ASN1_E2AP_ID_RI_CCTRL_HDR 22
#define ASN1_E2AP_ID_RI_CCTRL_MSG 23
#define ASN1_E2AP_ID_RI_CCTRL_STATUS 24
#define ASN1_E2AP_ID_RI_CIND_HDR 25
#define ASN1_E2AP_ID_RI_CIND_MSG 26
#define ASN1_E2AP_ID_RI_CIND_SN 27
#define ASN1_E2AP_ID_RI_CIND_TYPE 28
#define ASN1_E2AP_ID_RI_CREQUEST_ID 29
#define ASN1_E2AP_ID_RICSUBSCRIPTION_DETAILS 30
#define ASN1_E2AP_ID_TIME_TO_WAIT 31
#define ASN1_E2AP_ID_RI_CCTRL_OUTCOME 32
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_UPD 33
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_UPD_ITEM 34
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_UPD_ACK 35
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_UPD_ACK_ITEM 36
#define ASN1_E2AP_ID_E2CONN_SETUP 39
#define ASN1_E2AP_ID_E2CONN_SETUP_FAILED 40
#define ASN1_E2AP_ID_E2CONN_SETUP_FAILED_ITEM 41
#define ASN1_E2AP_ID_E2CONN_FAILED_ITEM 42
#define ASN1_E2AP_ID_E2CONN_UPD_ITEM 43
#define ASN1_E2AP_ID_E2CONN_UPD_ADD 44
#define ASN1_E2AP_ID_E2CONN_UPD_MODIFY 45
#define ASN1_E2AP_ID_E2CONN_UPD_REM 46
#define ASN1_E2AP_ID_E2CONN_UPD_REM_ITEM 47
#define ASN1_E2AP_ID_TN_LINFO 48
#define ASN1_E2AP_ID_TRANSACTION_ID 49
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_ADDITION 50
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_ADDITION_ITEM 51
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_ADDITION_ACK 52
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_ADDITION_ACK_ITEM 53
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_REMOVAL 54
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_REMOVAL_ITEM 55
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_REMOVAL_ACK 56
#define ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_REMOVAL_ACK_ITEM 57
#define ASN1_E2AP_ID_E2NODE_TN_LASSOC_REMOVAL 58
#define ASN1_E2AP_ID_E2NODE_TN_LASSOC_REMOVAL_ITEM 59
#define ASN1_E2AP_ID_RICSUBSCRIPTION_TO_BE_REMD 60
#define ASN1_E2AP_ID_RICSUBSCRIPTION_WITH_CAUSE_ITEM 61

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// CauseE2node ::= ENUMERATED
struct cause_e2node_opts {
  enum options { e2node_component_unknown, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<cause_e2node_opts, true> cause_e2node_e;

// CauseMisc ::= ENUMERATED
struct cause_misc_opts {
  enum options { ctrl_processing_overload, hardware_fail, om_intervention, unspecified, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<cause_misc_opts, true> cause_misc_e;

// CauseProtocol ::= ENUMERATED
struct cause_protocol_opts {
  enum options {
    transfer_syntax_error,
    abstract_syntax_error_reject,
    abstract_syntax_error_ignore_and_notify,
    msg_not_compatible_with_receiver_state,
    semantic_error,
    abstract_syntax_error_falsely_constructed_msg,
    unspecified,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<cause_protocol_opts, true> cause_protocol_e;

// CauseRICrequest ::= ENUMERATED
struct cause_ri_crequest_opts {
  enum options {
    ran_function_id_invalid,
    action_not_supported,
    excessive_actions,
    duplicate_action,
    duplicate_event_trigger,
    function_res_limit,
    request_id_unknown,
    inconsistent_action_subsequent_action_seq,
    ctrl_msg_invalid,
    ric_call_process_id_invalid,
    ctrl_timer_expired,
    ctrl_failed_to_execute,
    sys_not_ready,
    unspecified,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<cause_ri_crequest_opts, true> cause_ri_crequest_e;

// CauseRICservice ::= ENUMERATED
struct cause_ricservice_opts {
  enum options { ran_function_not_supported, excessive_functions, ric_res_limit, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<cause_ricservice_opts, true> cause_ricservice_e;

// CauseTransport ::= ENUMERATED
struct cause_transport_opts {
  enum options { unspecified, transport_res_unavailable, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<cause_transport_opts, true> cause_transport_e;

// Cause ::= CHOICE
struct cause_c {
  struct types_opts {
    enum options { ric_request, ric_service, e2_node, transport, protocol, misc, /*...*/ nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  cause_c() = default;
  cause_c(const cause_c& other);
  cause_c& operator=(const cause_c& other);
  ~cause_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  cause_ri_crequest_e& ric_request()
  {
    assert_choice_type(types::ric_request, type_, "Cause");
    return c.get<cause_ri_crequest_e>();
  }
  cause_ricservice_e& ric_service()
  {
    assert_choice_type(types::ric_service, type_, "Cause");
    return c.get<cause_ricservice_e>();
  }
  cause_e2node_e& e2_node()
  {
    assert_choice_type(types::e2_node, type_, "Cause");
    return c.get<cause_e2node_e>();
  }
  cause_transport_e& transport()
  {
    assert_choice_type(types::transport, type_, "Cause");
    return c.get<cause_transport_e>();
  }
  cause_protocol_e& protocol()
  {
    assert_choice_type(types::protocol, type_, "Cause");
    return c.get<cause_protocol_e>();
  }
  cause_misc_e& misc()
  {
    assert_choice_type(types::misc, type_, "Cause");
    return c.get<cause_misc_e>();
  }
  const cause_ri_crequest_e& ric_request() const
  {
    assert_choice_type(types::ric_request, type_, "Cause");
    return c.get<cause_ri_crequest_e>();
  }
  const cause_ricservice_e& ric_service() const
  {
    assert_choice_type(types::ric_service, type_, "Cause");
    return c.get<cause_ricservice_e>();
  }
  const cause_e2node_e& e2_node() const
  {
    assert_choice_type(types::e2_node, type_, "Cause");
    return c.get<cause_e2node_e>();
  }
  const cause_transport_e& transport() const
  {
    assert_choice_type(types::transport, type_, "Cause");
    return c.get<cause_transport_e>();
  }
  const cause_protocol_e& protocol() const
  {
    assert_choice_type(types::protocol, type_, "Cause");
    return c.get<cause_protocol_e>();
  }
  const cause_misc_e& misc() const
  {
    assert_choice_type(types::misc, type_, "Cause");
    return c.get<cause_misc_e>();
  }
  cause_ri_crequest_e& set_ric_request();
  cause_ricservice_e&  set_ric_service();
  cause_e2node_e&      set_e2_node();
  cause_transport_e&   set_transport();
  cause_protocol_e&    set_protocol();
  cause_misc_e&        set_misc();

private:
  types               type_;
  pod_choice_buffer_t c;

  void destroy_();
};

// TypeOfError ::= ENUMERATED
struct type_of_error_opts {
  enum options { not_understood, missing, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<type_of_error_opts, true> type_of_error_e;

// CriticalityDiagnostics-IE-Item ::= SEQUENCE
struct crit_diagnostics_ie_item_s {
  bool            ext = false;
  crit_e          iecrit;
  uint32_t        ie_id = 0;
  type_of_error_e type_of_error;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// CriticalityDiagnostics-IE-List ::= SEQUENCE (SIZE (1..256)) OF CriticalityDiagnostics-IE-Item
using crit_diagnostics_ie_list_l = dyn_array<crit_diagnostics_ie_item_s>;

// RICrequestID ::= SEQUENCE
struct ri_crequest_id_s {
  bool     ext              = false;
  uint32_t ric_requestor_id = 0;
  uint32_t ric_instance_id  = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TriggeringMessage ::= ENUMERATED
struct trigger_msg_opts {
  enum options { init_msg, successful_outcome, unsuccessfull_outcome, nulltype } value;

  const char* to_string() const;
};
typedef enumerated<trigger_msg_opts> trigger_msg_e;

// CriticalityDiagnostics ::= SEQUENCE
struct crit_diagnostics_s {
  bool                       ext                      = false;
  bool                       proc_code_present        = false;
  bool                       trigger_msg_present      = false;
  bool                       proc_crit_present        = false;
  bool                       ric_requestor_id_present = false;
  uint16_t                   proc_code                = 0;
  trigger_msg_e              trigger_msg;
  crit_e                     proc_crit;
  ri_crequest_id_s           ric_requestor_id;
  crit_diagnostics_ie_list_l ies_crit_diagnostics;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
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

// GlobalNG-RANNode-ID ::= CHOICE
struct global_ng_ran_node_id_c {
  struct types_opts {
    enum options { gnb, ng_enb, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  global_ng_ran_node_id_c() = default;
  global_ng_ran_node_id_c(const global_ng_ran_node_id_c& other);
  global_ng_ran_node_id_c& operator=(const global_ng_ran_node_id_c& other);
  ~global_ng_ran_node_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  globalg_nb_id_s& gnb()
  {
    assert_choice_type(types::gnb, type_, "GlobalNG-RANNode-ID");
    return c.get<globalg_nb_id_s>();
  }
  globalngenb_id_s& ng_enb()
  {
    assert_choice_type(types::ng_enb, type_, "GlobalNG-RANNode-ID");
    return c.get<globalngenb_id_s>();
  }
  const globalg_nb_id_s& gnb() const
  {
    assert_choice_type(types::gnb, type_, "GlobalNG-RANNode-ID");
    return c.get<globalg_nb_id_s>();
  }
  const globalngenb_id_s& ng_enb() const
  {
    assert_choice_type(types::ng_enb, type_, "GlobalNG-RANNode-ID");
    return c.get<globalngenb_id_s>();
  }
  globalg_nb_id_s&  set_gnb();
  globalngenb_id_s& set_ng_enb();

private:
  types                                              type_;
  choice_buffer_t<globalg_nb_id_s, globalngenb_id_s> c;

  void destroy_();
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

// RICsubsequentActionType ::= ENUMERATED
struct ricsubsequent_action_type_opts {
  enum options { continuee, wait, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<ricsubsequent_action_type_opts, true> ricsubsequent_action_type_e;

// RICtimeToWait ::= ENUMERATED
struct ri_ctime_to_wait_opts {
  enum options {
    w1ms,
    w2ms,
    w5ms,
    w10ms,
    w20ms,
    w30ms,
    w40ms,
    w50ms,
    w100ms,
    w200ms,
    w500ms,
    w1s,
    w2s,
    w5s,
    w10s,
    w20s,
    w60s,
    // ...
    nulltype
  } value;

  const char* to_string() const;
};
typedef enumerated<ri_ctime_to_wait_opts, true> ri_ctime_to_wait_e;

// E2nodeComponentInterfaceE1 ::= SEQUENCE
struct e2node_component_interface_e1_s {
  bool     ext          = false;
  uint64_t gnb_cu_cp_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentInterfaceF1 ::= SEQUENCE
struct e2node_component_interface_f1_s {
  bool     ext       = false;
  uint64_t gnb_du_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentInterfaceNG ::= SEQUENCE
struct e2node_component_interface_ng_s {
  bool                                 ext = false;
  printable_string<1, 150, true, true> amf_name;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentInterfaceS1 ::= SEQUENCE
struct e2node_component_interface_s1_s {
  bool                                 ext = false;
  printable_string<1, 150, true, true> mme_name;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentInterfaceW1 ::= SEQUENCE
struct e2node_component_interface_w1_s {
  bool     ext          = false;
  uint64_t ng_enb_du_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentInterfaceX2 ::= SEQUENCE
struct e2node_component_interface_x2_s {
  bool              ext                       = false;
  bool              global_enb_id_present     = false;
  bool              global_en_g_nb_id_present = false;
  global_enb_id_s   global_enb_id;
  globalen_gnb_id_s global_en_g_nb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentInterfaceXn ::= SEQUENCE
struct e2node_component_interface_xn_s {
  bool                    ext = false;
  global_ng_ran_node_id_c global_ng_ran_node_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICactionType ::= ENUMERATED
struct ri_caction_type_opts {
  enum options { report, insert, policy, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<ri_caction_type_opts, true> ri_caction_type_e;

// RICsubsequentAction ::= SEQUENCE
struct ricsubsequent_action_s {
  bool                        ext = false;
  ricsubsequent_action_type_e ric_subsequent_action_type;
  ri_ctime_to_wait_e          ric_time_to_wait;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentConfiguration ::= SEQUENCE
struct e2node_component_cfg_s {
  bool                      ext = false;
  unbounded_octstring<true> e2node_component_request_part;
  unbounded_octstring<true> e2node_component_resp_part;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentConfigurationAck ::= SEQUENCE
struct e2node_component_cfg_ack_s {
  struct upd_outcome_opts {
    enum options { success, fail, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<upd_outcome_opts, true> upd_outcome_e_;

  // member variables
  bool           ext                = false;
  bool           fail_cause_present = false;
  upd_outcome_e_ upd_outcome;
  cause_c        fail_cause;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentID ::= CHOICE
struct e2node_component_id_c {
  struct types_opts {
    enum options {
      e2node_component_interface_type_ng,
      e2node_component_interface_type_xn,
      e2node_component_interface_type_e1,
      e2node_component_interface_type_f1,
      e2node_component_interface_type_w1,
      e2node_component_interface_type_s1,
      e2node_component_interface_type_x2,
      // ...
      nulltype
    } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  e2node_component_id_c() = default;
  e2node_component_id_c(const e2node_component_id_c& other);
  e2node_component_id_c& operator=(const e2node_component_id_c& other);
  ~e2node_component_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  e2node_component_interface_ng_s& e2node_component_interface_type_ng()
  {
    assert_choice_type(types::e2node_component_interface_type_ng, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_ng_s>();
  }
  e2node_component_interface_xn_s& e2node_component_interface_type_xn()
  {
    assert_choice_type(types::e2node_component_interface_type_xn, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_xn_s>();
  }
  e2node_component_interface_e1_s& e2node_component_interface_type_e1()
  {
    assert_choice_type(types::e2node_component_interface_type_e1, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_e1_s>();
  }
  e2node_component_interface_f1_s& e2node_component_interface_type_f1()
  {
    assert_choice_type(types::e2node_component_interface_type_f1, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_f1_s>();
  }
  e2node_component_interface_w1_s& e2node_component_interface_type_w1()
  {
    assert_choice_type(types::e2node_component_interface_type_w1, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_w1_s>();
  }
  e2node_component_interface_s1_s& e2node_component_interface_type_s1()
  {
    assert_choice_type(types::e2node_component_interface_type_s1, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_s1_s>();
  }
  e2node_component_interface_x2_s& e2node_component_interface_type_x2()
  {
    assert_choice_type(types::e2node_component_interface_type_x2, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_x2_s>();
  }
  const e2node_component_interface_ng_s& e2node_component_interface_type_ng() const
  {
    assert_choice_type(types::e2node_component_interface_type_ng, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_ng_s>();
  }
  const e2node_component_interface_xn_s& e2node_component_interface_type_xn() const
  {
    assert_choice_type(types::e2node_component_interface_type_xn, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_xn_s>();
  }
  const e2node_component_interface_e1_s& e2node_component_interface_type_e1() const
  {
    assert_choice_type(types::e2node_component_interface_type_e1, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_e1_s>();
  }
  const e2node_component_interface_f1_s& e2node_component_interface_type_f1() const
  {
    assert_choice_type(types::e2node_component_interface_type_f1, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_f1_s>();
  }
  const e2node_component_interface_w1_s& e2node_component_interface_type_w1() const
  {
    assert_choice_type(types::e2node_component_interface_type_w1, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_w1_s>();
  }
  const e2node_component_interface_s1_s& e2node_component_interface_type_s1() const
  {
    assert_choice_type(types::e2node_component_interface_type_s1, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_s1_s>();
  }
  const e2node_component_interface_x2_s& e2node_component_interface_type_x2() const
  {
    assert_choice_type(types::e2node_component_interface_type_x2, type_, "E2nodeComponentID");
    return c.get<e2node_component_interface_x2_s>();
  }
  e2node_component_interface_ng_s& set_e2node_component_interface_type_ng();
  e2node_component_interface_xn_s& set_e2node_component_interface_type_xn();
  e2node_component_interface_e1_s& set_e2node_component_interface_type_e1();
  e2node_component_interface_f1_s& set_e2node_component_interface_type_f1();
  e2node_component_interface_w1_s& set_e2node_component_interface_type_w1();
  e2node_component_interface_s1_s& set_e2node_component_interface_type_s1();
  e2node_component_interface_x2_s& set_e2node_component_interface_type_x2();

private:
  types type_;
  choice_buffer_t<e2node_component_interface_e1_s,
                  e2node_component_interface_f1_s,
                  e2node_component_interface_ng_s,
                  e2node_component_interface_s1_s,
                  e2node_component_interface_w1_s,
                  e2node_component_interface_x2_s,
                  e2node_component_interface_xn_s>
      c;

  void destroy_();
};

// E2nodeComponentInterfaceType ::= ENUMERATED
struct e2node_component_interface_type_opts {
  enum options { ng, xn, e1, f1, w1, s1, x2, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<e2node_component_interface_type_opts, true> e2node_component_interface_type_e;

// RICaction-ToBeSetup-Item ::= SEQUENCE
struct ri_caction_to_be_setup_item_s {
  bool                      ext                           = false;
  bool                      ric_subsequent_action_present = false;
  uint16_t                  ric_action_id                 = 0;
  ri_caction_type_e         ric_action_type;
  unbounded_octstring<true> ric_action_definition;
  ricsubsequent_action_s    ric_subsequent_action;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TNLinformation ::= SEQUENCE
struct tn_linfo_s {
  bool                                  ext              = false;
  bool                                  tnl_port_present = false;
  bounded_bitstring<1, 160, true, true> tnl_address;
  fixed_bitstring<16, false, true>      tnl_port;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TNLusage ::= ENUMERATED
struct tn_lusage_opts {
  enum options { ric_service, support_function, both, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<tn_lusage_opts, true> tn_lusage_e;

// E2connectionSetupFailed-Item ::= SEQUENCE
struct e2conn_setup_failed_item_s {
  bool       ext = false;
  tn_linfo_s tnl_info;
  cause_c    cause;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2connectionUpdate-Item ::= SEQUENCE
struct e2conn_upd_item_s {
  bool        ext = false;
  tn_linfo_s  tnl_info;
  tn_lusage_e tnl_usage;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2connectionUpdateRemove-Item ::= SEQUENCE
struct e2conn_upd_rem_item_s {
  bool       ext = false;
  tn_linfo_s tnl_info;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentConfigAddition-Item ::= SEQUENCE
struct e2node_component_cfg_addition_item_s {
  bool                              ext = false;
  e2node_component_interface_type_e e2node_component_interface_type;
  e2node_component_id_c             e2node_component_id;
  e2node_component_cfg_s            e2node_component_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentConfigAdditionAck-Item ::= SEQUENCE
struct e2node_component_cfg_addition_ack_item_s {
  bool                              ext = false;
  e2node_component_interface_type_e e2node_component_interface_type;
  e2node_component_id_c             e2node_component_id;
  e2node_component_cfg_ack_s        e2node_component_cfg_ack;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentConfigRemoval-Item ::= SEQUENCE
struct e2node_component_cfg_removal_item_s {
  bool                              ext = false;
  e2node_component_interface_type_e e2node_component_interface_type;
  e2node_component_id_c             e2node_component_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentConfigRemovalAck-Item ::= SEQUENCE
struct e2node_component_cfg_removal_ack_item_s {
  bool                              ext = false;
  e2node_component_interface_type_e e2node_component_interface_type;
  e2node_component_id_c             e2node_component_id;
  e2node_component_cfg_ack_s        e2node_component_cfg_ack;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentConfigUpdate-Item ::= SEQUENCE
struct e2node_component_cfg_upd_item_s {
  bool                              ext = false;
  e2node_component_interface_type_e e2node_component_interface_type;
  e2node_component_id_c             e2node_component_id;
  e2node_component_cfg_s            e2node_component_cfg;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeComponentConfigUpdateAck-Item ::= SEQUENCE
struct e2node_component_cfg_upd_ack_item_s {
  bool                              ext = false;
  e2node_component_interface_type_e e2node_component_interface_type;
  e2node_component_id_c             e2node_component_id;
  e2node_component_cfg_ack_s        e2node_component_cfg_ack;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeTNLassociationRemoval-Item ::= SEQUENCE
struct e2node_tn_lassoc_removal_item_s {
  bool       ext = false;
  tn_linfo_s tnl_info;
  tn_linfo_s tnl_info_ric;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANfunction-Item ::= SEQUENCE
struct ra_nfunction_item_s {
  bool                                  ext             = false;
  uint16_t                              ran_function_id = 0;
  unbounded_octstring<true>             ran_function_definition;
  uint16_t                              ran_function_revision = 0;
  printable_string<1, 1000, true, true> ran_function_oid;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANfunctionID-Item ::= SEQUENCE
struct ra_nfunction_id_item_s {
  bool     ext                   = false;
  uint16_t ran_function_id       = 0;
  uint16_t ran_function_revision = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANfunctionIDcause-Item ::= SEQUENCE
struct ra_nfunction_idcause_item_s {
  bool     ext             = false;
  uint16_t ran_function_id = 0;
  cause_c  cause;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-Admitted-Item ::= SEQUENCE
struct ri_caction_admitted_item_s {
  bool     ext           = false;
  uint16_t ric_action_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-NotAdmitted-Item ::= SEQUENCE
struct ri_caction_not_admitted_item_s {
  bool     ext           = false;
  uint16_t ric_action_id = 0;
  cause_c  cause;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICaction-ToBeSetup-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ri_caction_to_be_setup_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ri_caction_to_be_setup_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ri_caction_to_be_setup_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ri_caction_to_be_setup_item_s&       ri_caction_to_be_setup_item() { return c; }
    const ri_caction_to_be_setup_item_s& ri_caction_to_be_setup_item() const { return c; }

  private:
    ri_caction_to_be_setup_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICsubscription-withCause-Item ::= SEQUENCE
struct ricsubscription_with_cause_item_s {
  bool             ext = false;
  ri_crequest_id_s ric_request_id;
  uint16_t         ran_function_id = 0;
  cause_c          cause;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2connectionSetupFailed-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2conn_setup_failed_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2conn_setup_failed_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::e2conn_setup_failed_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2conn_setup_failed_item_s&       e2conn_setup_failed_item() { return c; }
    const e2conn_setup_failed_item_s& e2conn_setup_failed_item() const { return c; }

  private:
    e2conn_setup_failed_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2connectionUpdate-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2conn_upd_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2conn_upd_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::e2conn_upd_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2conn_upd_item_s&       e2conn_upd_item() { return c; }
    const e2conn_upd_item_s& e2conn_upd_item() const { return c; }

  private:
    e2conn_upd_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2connectionUpdateRemove-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2conn_upd_rem_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2conn_upd_rem_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::e2conn_upd_rem_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2conn_upd_rem_item_s&       e2conn_upd_rem_item() { return c; }
    const e2conn_upd_rem_item_s& e2conn_upd_rem_item() const { return c; }

  private:
    e2conn_upd_rem_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2nodeComponentConfigAddition-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_component_cfg_addition_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2node_component_cfg_addition_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::e2node_component_cfg_addition_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2node_component_cfg_addition_item_s&       e2node_component_cfg_addition_item() { return c; }
    const e2node_component_cfg_addition_item_s& e2node_component_cfg_addition_item() const { return c; }

  private:
    e2node_component_cfg_addition_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2nodeComponentConfigAdditionAck-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_component_cfg_addition_ack_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2node_component_cfg_addition_ack_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::e2node_component_cfg_addition_ack_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2node_component_cfg_addition_ack_item_s&       e2node_component_cfg_addition_ack_item() { return c; }
    const e2node_component_cfg_addition_ack_item_s& e2node_component_cfg_addition_ack_item() const { return c; }

  private:
    e2node_component_cfg_addition_ack_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2nodeComponentConfigRemoval-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_component_cfg_removal_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2node_component_cfg_removal_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::e2node_component_cfg_removal_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2node_component_cfg_removal_item_s&       e2node_component_cfg_removal_item() { return c; }
    const e2node_component_cfg_removal_item_s& e2node_component_cfg_removal_item() const { return c; }

  private:
    e2node_component_cfg_removal_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2nodeComponentConfigRemovalAck-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_component_cfg_removal_ack_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2node_component_cfg_removal_ack_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::e2node_component_cfg_removal_ack_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2node_component_cfg_removal_ack_item_s&       e2node_component_cfg_removal_ack_item() { return c; }
    const e2node_component_cfg_removal_ack_item_s& e2node_component_cfg_removal_ack_item() const { return c; }

  private:
    e2node_component_cfg_removal_ack_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2nodeComponentConfigUpdate-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_component_cfg_upd_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2node_component_cfg_upd_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::e2node_component_cfg_upd_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2node_component_cfg_upd_item_s&       e2node_component_cfg_upd_item() { return c; }
    const e2node_component_cfg_upd_item_s& e2node_component_cfg_upd_item() const { return c; }

  private:
    e2node_component_cfg_upd_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2nodeComponentConfigUpdateAck-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_component_cfg_upd_ack_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2node_component_cfg_upd_ack_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::e2node_component_cfg_upd_ack_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2node_component_cfg_upd_ack_item_s&       e2node_component_cfg_upd_ack_item() { return c; }
    const e2node_component_cfg_upd_ack_item_s& e2node_component_cfg_upd_ack_item() const { return c; }

  private:
    e2node_component_cfg_upd_ack_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2nodeTNLassociationRemoval-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_tn_lassoc_removal_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { e2node_tn_lassoc_removal_item, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::e2node_tn_lassoc_removal_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    e2node_tn_lassoc_removal_item_s&       e2node_tn_lassoc_removal_item() { return c; }
    const e2node_tn_lassoc_removal_item_s& e2node_tn_lassoc_removal_item() const { return c; }

  private:
    e2node_tn_lassoc_removal_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// GlobalE2node-eNB-ID ::= SEQUENCE
struct global_e2node_enb_id_s {
  bool            ext = false;
  global_enb_id_s global_enb_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalE2node-en-gNB-ID ::= SEQUENCE
struct global_e2node_en_g_nb_id_s {
  bool              ext                      = false;
  bool              en_g_nb_cu_up_id_present = false;
  bool              en_g_nb_du_id_present    = false;
  globalen_gnb_id_s global_en_g_nb_id;
  uint64_t          en_g_nb_cu_up_id = 0;
  uint64_t          en_g_nb_du_id    = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalE2node-gNB-ID ::= SEQUENCE
struct global_e2node_g_nb_id_s {
  bool              ext                       = false;
  bool              global_en_g_nb_id_present = false;
  bool              gnb_cu_up_id_present      = false;
  bool              gnb_du_id_present         = false;
  globalg_nb_id_s   global_g_nb_id;
  globalen_gnb_id_s global_en_g_nb_id;
  uint64_t          gnb_cu_up_id = 0;
  uint64_t          gnb_du_id    = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// GlobalE2node-ng-eNB-ID ::= SEQUENCE
struct global_e2node_ng_enb_id_s {
  bool             ext                   = false;
  bool             global_enb_id_present = false;
  bool             ng_enb_du_id_present  = false;
  globalngenb_id_s global_ng_enb_id;
  global_enb_id_s  global_enb_id;
  uint64_t         ng_enb_du_id = 0;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANfunction-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ra_nfunction_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ra_nfunction_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ra_nfunction_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ra_nfunction_item_s&       ra_nfunction_item() { return c; }
    const ra_nfunction_item_s& ra_nfunction_item() const { return c; }

  private:
    ra_nfunction_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RANfunctionID-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ra_nfunction_id_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ra_nfunction_id_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ra_nfunction_id_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ra_nfunction_id_item_s&       ra_nfunction_id_item() { return c; }
    const ra_nfunction_id_item_s& ra_nfunction_id_item() const { return c; }

  private:
    ra_nfunction_id_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RANfunctionIDcause-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ra_nfunction_idcause_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ra_nfunction_iecause_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ra_nfunction_iecause_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ra_nfunction_idcause_item_s&       ra_nfunction_iecause_item() { return c; }
    const ra_nfunction_idcause_item_s& ra_nfunction_iecause_item() const { return c; }

  private:
    ra_nfunction_idcause_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-Admitted-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ri_caction_admitted_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ri_caction_admitted_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ri_caction_admitted_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ri_caction_admitted_item_s&       ri_caction_admitted_item() { return c; }
    const ri_caction_admitted_item_s& ri_caction_admitted_item() const { return c; }

  private:
    ri_caction_admitted_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICaction-NotAdmitted-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ri_caction_not_admitted_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ri_caction_not_admitted_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ri_caction_not_admitted_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ri_caction_not_admitted_item_s&       ri_caction_not_admitted_item() { return c; }
    const ri_caction_not_admitted_item_s& ri_caction_not_admitted_item() const { return c; }

  private:
    ri_caction_not_admitted_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICactions-ToBeSetup-List ::= SEQUENCE (SIZE (1..16)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using ri_cactions_to_be_setup_list_l =
    bounded_array<protocol_ie_single_container_s<ri_caction_to_be_setup_item_ies_o>, 16>;

// RICsubscription-withCause-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ricsubscription_with_cause_item_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ricsubscription_with_cause_item, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ricsubscription_with_cause_item; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ricsubscription_with_cause_item_s&       ricsubscription_with_cause_item() { return c; }
    const ricsubscription_with_cause_item_s& ricsubscription_with_cause_item() const { return c; }

  private:
    ricsubscription_with_cause_item_s c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2connectionSetupFailed-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2conn_setup_failed_list_l = bounded_array<protocol_ie_single_container_s<e2conn_setup_failed_item_ies_o>, 32>;

// E2connectionUpdate-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using e2conn_upd_list_l = bounded_array<protocol_ie_single_container_s<e2conn_upd_item_ies_o>, 32>;

// E2connectionUpdateRemove-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2conn_upd_rem_list_l = bounded_array<protocol_ie_single_container_s<e2conn_upd_rem_item_ies_o>, 32>;

// E2nodeComponentConfigAddition-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2node_component_cfg_addition_list_l =
    dyn_array<protocol_ie_single_container_s<e2node_component_cfg_addition_item_ies_o> >;

// E2nodeComponentConfigAdditionAck-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2node_component_cfg_addition_ack_list_l =
    dyn_array<protocol_ie_single_container_s<e2node_component_cfg_addition_ack_item_ies_o> >;

// E2nodeComponentConfigRemoval-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2node_component_cfg_removal_list_l =
    dyn_array<protocol_ie_single_container_s<e2node_component_cfg_removal_item_ies_o> >;

// E2nodeComponentConfigRemovalAck-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2node_component_cfg_removal_ack_list_l =
    dyn_array<protocol_ie_single_container_s<e2node_component_cfg_removal_ack_item_ies_o> >;

// E2nodeComponentConfigUpdate-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2node_component_cfg_upd_list_l = dyn_array<protocol_ie_single_container_s<e2node_component_cfg_upd_item_ies_o> >;

// E2nodeComponentConfigUpdateAck-List ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2node_component_cfg_upd_ack_list_l =
    dyn_array<protocol_ie_single_container_s<e2node_component_cfg_upd_ack_item_ies_o> >;

// E2nodeTNLassociationRemoval-List ::= SEQUENCE (SIZE (1..32)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using e2node_tn_lassoc_removal_list_l =
    bounded_array<protocol_ie_single_container_s<e2node_tn_lassoc_removal_item_ies_o>, 32>;

// GlobalE2node-ID ::= CHOICE
struct global_e2node_id_c {
  struct types_opts {
    enum options { gnb, en_g_nb, ng_enb, enb, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  global_e2node_id_c() = default;
  global_e2node_id_c(const global_e2node_id_c& other);
  global_e2node_id_c& operator=(const global_e2node_id_c& other);
  ~global_e2node_id_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  global_e2node_g_nb_id_s& gnb()
  {
    assert_choice_type(types::gnb, type_, "GlobalE2node-ID");
    return c.get<global_e2node_g_nb_id_s>();
  }
  global_e2node_en_g_nb_id_s& en_g_nb()
  {
    assert_choice_type(types::en_g_nb, type_, "GlobalE2node-ID");
    return c.get<global_e2node_en_g_nb_id_s>();
  }
  global_e2node_ng_enb_id_s& ng_enb()
  {
    assert_choice_type(types::ng_enb, type_, "GlobalE2node-ID");
    return c.get<global_e2node_ng_enb_id_s>();
  }
  global_e2node_enb_id_s& enb()
  {
    assert_choice_type(types::enb, type_, "GlobalE2node-ID");
    return c.get<global_e2node_enb_id_s>();
  }
  const global_e2node_g_nb_id_s& gnb() const
  {
    assert_choice_type(types::gnb, type_, "GlobalE2node-ID");
    return c.get<global_e2node_g_nb_id_s>();
  }
  const global_e2node_en_g_nb_id_s& en_g_nb() const
  {
    assert_choice_type(types::en_g_nb, type_, "GlobalE2node-ID");
    return c.get<global_e2node_en_g_nb_id_s>();
  }
  const global_e2node_ng_enb_id_s& ng_enb() const
  {
    assert_choice_type(types::ng_enb, type_, "GlobalE2node-ID");
    return c.get<global_e2node_ng_enb_id_s>();
  }
  const global_e2node_enb_id_s& enb() const
  {
    assert_choice_type(types::enb, type_, "GlobalE2node-ID");
    return c.get<global_e2node_enb_id_s>();
  }
  global_e2node_g_nb_id_s&    set_gnb();
  global_e2node_en_g_nb_id_s& set_en_g_nb();
  global_e2node_ng_enb_id_s&  set_ng_enb();
  global_e2node_enb_id_s&     set_enb();

private:
  types type_;
  choice_buffer_t<global_e2node_en_g_nb_id_s,
                  global_e2node_enb_id_s,
                  global_e2node_g_nb_id_s,
                  global_e2node_ng_enb_id_s>
      c;

  void destroy_();
};

// GlobalRIC-ID ::= SEQUENCE
struct global_ric_id_s {
  bool                             ext = false;
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<20, false, true> ric_id;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RANfunctions-List ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using ra_nfunctions_list_l = dyn_array<protocol_ie_single_container_s<ra_nfunction_item_ies_o> >;

// RANfunctionsID-List ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using ra_nfunctions_id_list_l = dyn_array<protocol_ie_single_container_s<ra_nfunction_id_item_ies_o> >;

// RANfunctionsIDcause-List ::= SEQUENCE (SIZE (1..256)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using ra_nfunctions_idcause_list_l = dyn_array<protocol_ie_single_container_s<ra_nfunction_idcause_item_ies_o> >;

// RICaction-Admitted-List ::= SEQUENCE (SIZE (1..16)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using ri_caction_admitted_list_l = bounded_array<protocol_ie_single_container_s<ri_caction_admitted_item_ies_o>, 16>;

// RICaction-NotAdmitted-List ::= SEQUENCE (SIZE (0..16)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES : IEsSetParam}
using ri_caction_not_admitted_list_l =
    bounded_array<protocol_ie_single_container_s<ri_caction_not_admitted_item_ies_o>, 16>;

// RICcontrolAckRequest ::= ENUMERATED
struct ri_cctrl_ack_request_opts {
  enum options { no_ack, ack, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<ri_cctrl_ack_request_opts, true> ri_cctrl_ack_request_e;

// RICindicationType ::= ENUMERATED
struct ri_cind_type_opts {
  enum options { report, insert, /*...*/ nulltype } value;

  const char* to_string() const;
};
typedef enumerated<ri_cind_type_opts, true> ri_cind_type_e;

// RICsubscription-List-withCause ::= SEQUENCE (SIZE (1..1024)) OF ProtocolIE-SingleContainer{E2AP-PROTOCOL-IES :
// IEsSetParam}
using ricsubscription_list_with_cause_l =
    dyn_array<protocol_ie_single_container_s<ricsubscription_with_cause_item_ies_o> >;

// RICsubscriptionDetails ::= SEQUENCE
struct ricsubscription_details_s {
  bool                           ext = false;
  unbounded_octstring<true>      ric_event_trigger_definition;
  ri_cactions_to_be_setup_list_l ric_action_to_be_setup_list;
  // ...

  // sequence methods
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// TimeToWait ::= ENUMERATED
struct time_to_wait_opts {
  enum options { v1s, v2s, v5s, v10s, v20s, v60s, /*...*/ nulltype } value;
  typedef uint8_t number_type;

  const char* to_string() const;
  uint8_t     to_number() const;
};
typedef enumerated<time_to_wait_opts, true> time_to_wait_e;

// E2RemovalFailureIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2_removal_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2RemovalRequestIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2_removal_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::transaction_id; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&       transaction_id() { return c; }
    const uint16_t& transaction_id() const { return c; }

  private:
    uint16_t c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2RemovalResponseIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2_removal_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                 transaction_id();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                               type_;
    choice_buffer_t<crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2connectionUpdate-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2conn_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, e2conn_upd_add, e2conn_upd_rem, e2conn_upd_modify, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                    transaction_id();
    e2conn_upd_list_l&           e2conn_upd_add();
    e2conn_upd_rem_list_l&       e2conn_upd_rem();
    e2conn_upd_list_l&           e2conn_upd_modify();
    const uint16_t&              transaction_id() const;
    const e2conn_upd_list_l&     e2conn_upd_add() const;
    const e2conn_upd_rem_list_l& e2conn_upd_rem() const;
    const e2conn_upd_list_l&     e2conn_upd_modify() const;

  private:
    types                                                     type_;
    choice_buffer_t<e2conn_upd_list_l, e2conn_upd_rem_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2connectionUpdateAck-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2conn_upd_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, e2conn_setup, e2conn_setup_failed, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                         transaction_id();
    e2conn_upd_list_l&                e2conn_setup();
    e2conn_setup_failed_list_l&       e2conn_setup_failed();
    const uint16_t&                   transaction_id() const;
    const e2conn_upd_list_l&          e2conn_setup() const;
    const e2conn_setup_failed_list_l& e2conn_setup_failed() const;

  private:
    types                                                          type_;
    choice_buffer_t<e2conn_setup_failed_list_l, e2conn_upd_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2connectionUpdateFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2conn_upd_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2nodeConfigurationUpdate-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_cfg_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        global_e2node_id,
        e2node_component_cfg_addition,
        e2node_component_cfg_upd,
        e2node_component_cfg_removal,
        e2node_tn_lassoc_removal,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                                   transaction_id();
    global_e2node_id_c&                         global_e2node_id();
    e2node_component_cfg_addition_list_l&       e2node_component_cfg_addition();
    e2node_component_cfg_upd_list_l&            e2node_component_cfg_upd();
    e2node_component_cfg_removal_list_l&        e2node_component_cfg_removal();
    e2node_tn_lassoc_removal_list_l&            e2node_tn_lassoc_removal();
    const uint16_t&                             transaction_id() const;
    const global_e2node_id_c&                   global_e2node_id() const;
    const e2node_component_cfg_addition_list_l& e2node_component_cfg_addition() const;
    const e2node_component_cfg_upd_list_l&      e2node_component_cfg_upd() const;
    const e2node_component_cfg_removal_list_l&  e2node_component_cfg_removal() const;
    const e2node_tn_lassoc_removal_list_l&      e2node_tn_lassoc_removal() const;

  private:
    types type_;
    choice_buffer_t<e2node_component_cfg_addition_list_l,
                    e2node_component_cfg_removal_list_l,
                    e2node_component_cfg_upd_list_l,
                    e2node_tn_lassoc_removal_list_l,
                    global_e2node_id_c>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2nodeConfigurationUpdateAcknowledge-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_cfg_upd_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        e2node_component_cfg_addition_ack,
        e2node_component_cfg_upd_ack,
        e2node_component_cfg_removal_ack,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                                       transaction_id();
    e2node_component_cfg_addition_ack_list_l&       e2node_component_cfg_addition_ack();
    e2node_component_cfg_upd_ack_list_l&            e2node_component_cfg_upd_ack();
    e2node_component_cfg_removal_ack_list_l&        e2node_component_cfg_removal_ack();
    const uint16_t&                                 transaction_id() const;
    const e2node_component_cfg_addition_ack_list_l& e2node_component_cfg_addition_ack() const;
    const e2node_component_cfg_upd_ack_list_l&      e2node_component_cfg_upd_ack() const;
    const e2node_component_cfg_removal_ack_list_l&  e2node_component_cfg_removal_ack() const;

  private:
    types type_;
    choice_buffer_t<e2node_component_cfg_addition_ack_list_l,
                    e2node_component_cfg_removal_ack_list_l,
                    e2node_component_cfg_upd_ack_list_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2nodeConfigurationUpdateFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2node_cfg_upd_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2setupFailureIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2setup_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, tn_linfo, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    tn_linfo_s&               tn_linfo();
    const uint16_t&           transaction_id() const;
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;
    const tn_linfo_s&         tn_linfo() const;

  private:
    types                                                    type_;
    choice_buffer_t<cause_c, crit_diagnostics_s, tn_linfo_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2setupRequestIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2setup_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        global_e2node_id,
        ra_nfunctions_added,
        e2node_component_cfg_addition,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                                   transaction_id();
    global_e2node_id_c&                         global_e2node_id();
    ra_nfunctions_list_l&                       ra_nfunctions_added();
    e2node_component_cfg_addition_list_l&       e2node_component_cfg_addition();
    const uint16_t&                             transaction_id() const;
    const global_e2node_id_c&                   global_e2node_id() const;
    const ra_nfunctions_list_l&                 ra_nfunctions_added() const;
    const e2node_component_cfg_addition_list_l& e2node_component_cfg_addition() const;

  private:
    types                                                                                           type_;
    choice_buffer_t<e2node_component_cfg_addition_list_l, global_e2node_id_c, ra_nfunctions_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// E2setupResponseIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct e2setup_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        global_ric_id,
        ra_nfunctions_accepted,
        ra_nfunctions_rejected,
        e2node_component_cfg_addition_ack,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                                       transaction_id();
    global_ric_id_s&                                global_ric_id();
    ra_nfunctions_id_list_l&                        ra_nfunctions_accepted();
    ra_nfunctions_idcause_list_l&                   ra_nfunctions_rejected();
    e2node_component_cfg_addition_ack_list_l&       e2node_component_cfg_addition_ack();
    const uint16_t&                                 transaction_id() const;
    const global_ric_id_s&                          global_ric_id() const;
    const ra_nfunctions_id_list_l&                  ra_nfunctions_accepted() const;
    const ra_nfunctions_idcause_list_l&             ra_nfunctions_rejected() const;
    const e2node_component_cfg_addition_ack_list_l& e2node_component_cfg_addition_ack() const;

  private:
    types type_;
    choice_buffer_t<e2node_component_cfg_addition_ack_list_l,
                    global_ric_id_s,
                    ra_nfunctions_id_list_l,
                    ra_nfunctions_idcause_list_l>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ErrorIndication-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct error_ind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, ri_crequest_id, ra_nfunction_id, cause, crit_diagnostics, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                 transaction_id();
    ri_crequest_id_s&         ri_crequest_id();
    uint16_t&                 ra_nfunction_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const ri_crequest_id_s&   ri_crequest_id() const;
    const uint16_t&           ra_nfunction_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                                          type_;
    choice_buffer_t<cause_c, crit_diagnostics_s, ri_crequest_id_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICcontrolAcknowledge-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ri_cctrl_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ri_crequest_id, ra_nfunction_id, ri_ccall_process_id, ri_cctrl_outcome, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ri_crequest_id_s&                ri_crequest_id();
    uint16_t&                        ra_nfunction_id();
    unbounded_octstring<true>&       ri_ccall_process_id();
    unbounded_octstring<true>&       ri_cctrl_outcome();
    const ri_crequest_id_s&          ri_crequest_id() const;
    const uint16_t&                  ra_nfunction_id() const;
    const unbounded_octstring<true>& ri_ccall_process_id() const;
    const unbounded_octstring<true>& ri_cctrl_outcome() const;

  private:
    types                                                         type_;
    choice_buffer_t<ri_crequest_id_s, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICcontrolFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ri_cctrl_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ri_crequest_id, ra_nfunction_id, ri_ccall_process_id, cause, ri_cctrl_outcome, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ri_crequest_id_s&                ri_crequest_id();
    uint16_t&                        ra_nfunction_id();
    unbounded_octstring<true>&       ri_ccall_process_id();
    cause_c&                         cause();
    unbounded_octstring<true>&       ri_cctrl_outcome();
    const ri_crequest_id_s&          ri_crequest_id() const;
    const uint16_t&                  ra_nfunction_id() const;
    const unbounded_octstring<true>& ri_ccall_process_id() const;
    const cause_c&                   cause() const;
    const unbounded_octstring<true>& ri_cctrl_outcome() const;

  private:
    types                                                                  type_;
    choice_buffer_t<cause_c, ri_crequest_id_s, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICcontrolRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ri_cctrl_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ri_crequest_id,
        ra_nfunction_id,
        ri_ccall_process_id,
        ri_cctrl_hdr,
        ri_cctrl_msg,
        ri_cctrl_ack_request,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ri_crequest_id_s&                ri_crequest_id();
    uint16_t&                        ra_nfunction_id();
    unbounded_octstring<true>&       ri_ccall_process_id();
    unbounded_octstring<true>&       ri_cctrl_hdr();
    unbounded_octstring<true>&       ri_cctrl_msg();
    ri_cctrl_ack_request_e&          ri_cctrl_ack_request();
    const ri_crequest_id_s&          ri_crequest_id() const;
    const uint16_t&                  ra_nfunction_id() const;
    const unbounded_octstring<true>& ri_ccall_process_id() const;
    const unbounded_octstring<true>& ri_cctrl_hdr() const;
    const unbounded_octstring<true>& ri_cctrl_msg() const;
    const ri_cctrl_ack_request_e&    ri_cctrl_ack_request() const;

  private:
    types                                                         type_;
    choice_buffer_t<ri_crequest_id_s, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICindication-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ri_cind_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        ri_crequest_id,
        ra_nfunction_id,
        ri_caction_id,
        ri_cind_sn,
        ri_cind_type,
        ri_cind_hdr,
        ri_cind_msg,
        ri_ccall_process_id,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ri_crequest_id_s&                ri_crequest_id();
    uint16_t&                        ra_nfunction_id();
    uint16_t&                        ri_caction_id();
    uint32_t&                        ri_cind_sn();
    ri_cind_type_e&                  ri_cind_type();
    unbounded_octstring<true>&       ri_cind_hdr();
    unbounded_octstring<true>&       ri_cind_msg();
    unbounded_octstring<true>&       ri_ccall_process_id();
    const ri_crequest_id_s&          ri_crequest_id() const;
    const uint16_t&                  ra_nfunction_id() const;
    const uint16_t&                  ri_caction_id() const;
    const uint32_t&                  ri_cind_sn() const;
    const ri_cind_type_e&            ri_cind_type() const;
    const unbounded_octstring<true>& ri_cind_hdr() const;
    const unbounded_octstring<true>& ri_cind_msg() const;
    const unbounded_octstring<true>& ri_ccall_process_id() const;

  private:
    types                                                         type_;
    choice_buffer_t<ri_crequest_id_s, unbounded_octstring<true> > c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICserviceQuery-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ricservice_query_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, ra_nfunctions_accepted, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                      transaction_id();
    ra_nfunctions_id_list_l&       ra_nfunctions_accepted();
    const uint16_t&                transaction_id() const;
    const ra_nfunctions_id_list_l& ra_nfunctions_accepted() const;

  private:
    types                                    type_;
    choice_buffer_t<ra_nfunctions_id_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICserviceUpdate-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ricservice_upd_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options {
        transaction_id,
        ra_nfunctions_added,
        ra_nfunctions_modified,
        ra_nfunctions_deleted,
        nulltype
      } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                      transaction_id();
    ra_nfunctions_list_l&          ra_nfunctions_added();
    ra_nfunctions_list_l&          ra_nfunctions_modified();
    ra_nfunctions_id_list_l&       ra_nfunctions_deleted();
    const uint16_t&                transaction_id() const;
    const ra_nfunctions_list_l&    ra_nfunctions_added() const;
    const ra_nfunctions_list_l&    ra_nfunctions_modified() const;
    const ra_nfunctions_id_list_l& ra_nfunctions_deleted() const;

  private:
    types                                                          type_;
    choice_buffer_t<ra_nfunctions_id_list_l, ra_nfunctions_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICserviceUpdateAcknowledge-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ricservice_upd_ack_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, ra_nfunctions_accepted, ra_nfunctions_rejected, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                           transaction_id();
    ra_nfunctions_id_list_l&            ra_nfunctions_accepted();
    ra_nfunctions_idcause_list_l&       ra_nfunctions_rejected();
    const uint16_t&                     transaction_id() const;
    const ra_nfunctions_id_list_l&      ra_nfunctions_accepted() const;
    const ra_nfunctions_idcause_list_l& ra_nfunctions_rejected() const;

  private:
    types                                                                  type_;
    choice_buffer_t<ra_nfunctions_id_list_l, ra_nfunctions_idcause_list_l> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICserviceUpdateFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ricservice_upd_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, time_to_wait, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                 transaction_id();
    cause_c&                  cause();
    time_to_wait_e&           time_to_wait();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const cause_c&            cause() const;
    const time_to_wait_e&     time_to_wait() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                        type_;
    choice_buffer_t<cause_c, crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICsubscriptionDeleteFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ricsubscription_delete_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ri_crequest_id, ra_nfunction_id, cause, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ri_crequest_id_s&         ri_crequest_id();
    uint16_t&                 ra_nfunction_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const ri_crequest_id_s&   ri_crequest_id() const;
    const uint16_t&           ra_nfunction_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                                          type_;
    choice_buffer_t<cause_c, crit_diagnostics_s, ri_crequest_id_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICsubscriptionDeleteRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ricsubscription_delete_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ri_crequest_id, ra_nfunction_id, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ri_crequest_id_s&       ri_crequest_id();
    uint16_t&               ra_nfunction_id();
    const ri_crequest_id_s& ri_crequest_id() const;
    const uint16_t&         ra_nfunction_id() const;

  private:
    types                             type_;
    choice_buffer_t<ri_crequest_id_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICsubscriptionDeleteRequired-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ricsubscription_delete_required_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ricsubscription_to_be_remd, nulltype } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    types       type() const { return types::ricsubscription_to_be_remd; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ricsubscription_list_with_cause_l&       ricsubscription_to_be_remd() { return c; }
    const ricsubscription_list_with_cause_l& ricsubscription_to_be_remd() const { return c; }

  private:
    ricsubscription_list_with_cause_l c;
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICsubscriptionDeleteResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ricsubscription_delete_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ri_crequest_id, ra_nfunction_id, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ri_crequest_id_s&       ri_crequest_id();
    uint16_t&               ra_nfunction_id();
    const ri_crequest_id_s& ri_crequest_id() const;
    const uint16_t&         ra_nfunction_id() const;

  private:
    types                             type_;
    choice_buffer_t<ri_crequest_id_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICsubscriptionFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ricsubscription_fail_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ri_crequest_id, ra_nfunction_id, cause, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ri_crequest_id_s&         ri_crequest_id();
    uint16_t&                 ra_nfunction_id();
    cause_c&                  cause();
    crit_diagnostics_s&       crit_diagnostics();
    const ri_crequest_id_s&   ri_crequest_id() const;
    const uint16_t&           ra_nfunction_id() const;
    const cause_c&            cause() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                                                          type_;
    choice_buffer_t<cause_c, crit_diagnostics_s, ri_crequest_id_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICsubscriptionRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ricsubscription_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ri_crequest_id, ra_nfunction_id, ricsubscription_details, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ri_crequest_id_s&                ri_crequest_id();
    uint16_t&                        ra_nfunction_id();
    ricsubscription_details_s&       ricsubscription_details();
    const ri_crequest_id_s&          ri_crequest_id() const;
    const uint16_t&                  ra_nfunction_id() const;
    const ricsubscription_details_s& ricsubscription_details() const;

  private:
    types                                                        type_;
    choice_buffer_t<ri_crequest_id_s, ricsubscription_details_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// RICsubscriptionResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct ricsubscription_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ri_crequest_id, ra_nfunction_id, ri_cactions_admitted, ri_cactions_not_admitted, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ri_crequest_id_s&                     ri_crequest_id();
    uint16_t&                             ra_nfunction_id();
    ri_caction_admitted_list_l&           ri_cactions_admitted();
    ri_caction_not_admitted_list_l&       ri_cactions_not_admitted();
    const ri_crequest_id_s&               ri_crequest_id() const;
    const uint16_t&                       ra_nfunction_id() const;
    const ri_caction_admitted_list_l&     ri_cactions_admitted() const;
    const ri_caction_not_admitted_list_l& ri_cactions_not_admitted() const;

  private:
    types                                                                                         type_;
    choice_buffer_t<ri_caction_admitted_list_l, ri_caction_not_admitted_list_l, ri_crequest_id_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ResetRequestIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct reset_request_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, cause, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&       transaction_id();
    cause_c&        cause();
    const uint16_t& transaction_id() const;
    const cause_c&  cause() const;

  private:
    types                    type_;
    choice_buffer_t<cause_c> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

// ResetResponseIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
struct reset_resp_ies_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { transaction_id, crit_diagnostics, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    value_c() = default;
    value_c(const value_c& other);
    value_c& operator=(const value_c& other);
    ~value_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    uint16_t&                 transaction_id();
    crit_diagnostics_s&       crit_diagnostics();
    const uint16_t&           transaction_id() const;
    const crit_diagnostics_s& crit_diagnostics() const;

  private:
    types                               type_;
    choice_buffer_t<crit_diagnostics_s> c;

    void destroy_();
  };

  // members lookup methods
  static uint32_t   idx_to_id(uint32_t idx);
  static bool       is_id_valid(const uint32_t& id);
  static crit_e     get_crit(const uint32_t& id);
  static value_c    get_value(const uint32_t& id);
  static presence_e get_presence(const uint32_t& id);
};

struct e2_removal_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                               crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true> > transaction_id;
  ie_field_s<cause_c>                                cause;
  ie_field_s<crit_diagnostics_s>                     crit_diagnostics;

  // sequence methods
  e2_removal_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2RemovalFailure ::= SEQUENCE
using e2_removal_fail_s = elementary_procedure_option<e2_removal_fail_ies_container>;

// E2RemovalRequest ::= SEQUENCE
using e2_removal_request_s = elementary_procedure_option<protocol_ie_container_l<e2_removal_request_ies_o> >;

struct e2_removal_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                               crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true> > transaction_id;
  ie_field_s<crit_diagnostics_s>                     crit_diagnostics;

  // sequence methods
  e2_removal_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2RemovalResponse ::= SEQUENCE
using e2_removal_resp_s = elementary_procedure_option<e2_removal_resp_ies_container>;

struct e2conn_upd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                               e2conn_upd_add_present    = false;
  bool                                               e2conn_upd_rem_present    = false;
  bool                                               e2conn_upd_modify_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true> > transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<e2conn_upd_item_ies_o>, 1, 32, true> >     e2conn_upd_add;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<e2conn_upd_rem_item_ies_o>, 1, 32, true> > e2conn_upd_rem;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<e2conn_upd_item_ies_o>, 1, 32, true> >     e2conn_upd_modify;

  // sequence methods
  e2conn_upd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2connectionUpdate ::= SEQUENCE
using e2conn_upd_s = elementary_procedure_option<e2conn_upd_ies_container>;

struct e2conn_upd_ack_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                               e2conn_setup_present        = false;
  bool                                               e2conn_setup_failed_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true> > transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<e2conn_upd_item_ies_o>, 1, 32, true> > e2conn_setup;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<e2conn_setup_failed_item_ies_o>, 1, 32, true> >
      e2conn_setup_failed;

  // sequence methods
  e2conn_upd_ack_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2connectionUpdateAcknowledge ::= SEQUENCE
using e2conn_upd_ack_s = elementary_procedure_option<e2conn_upd_ack_ies_container>;

struct e2conn_upd_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                               cause_present            = false;
  bool                                               time_to_wait_present     = false;
  bool                                               crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true> > transaction_id;
  ie_field_s<cause_c>                                cause;
  ie_field_s<time_to_wait_e>                         time_to_wait;
  ie_field_s<crit_diagnostics_s>                     crit_diagnostics;

  // sequence methods
  e2conn_upd_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2connectionUpdateFailure ::= SEQUENCE
using e2conn_upd_fail_s = elementary_procedure_option<e2conn_upd_fail_ies_container>;

struct e2node_cfg_upd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                               global_e2node_id_present              = false;
  bool                                               e2node_component_cfg_addition_present = false;
  bool                                               e2node_component_cfg_upd_present      = false;
  bool                                               e2node_component_cfg_removal_present  = false;
  bool                                               e2node_tn_lassoc_removal_present      = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true> > transaction_id;
  ie_field_s<global_e2node_id_c>                     global_e2node_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<e2node_component_cfg_addition_item_ies_o>, 1, 1024, true> >
      e2node_component_cfg_addition;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<e2node_component_cfg_upd_item_ies_o>, 1, 1024, true> >
      e2node_component_cfg_upd;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<e2node_component_cfg_removal_item_ies_o>, 1, 1024, true> >
      e2node_component_cfg_removal;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<e2node_tn_lassoc_removal_item_ies_o>, 1, 32, true> >
      e2node_tn_lassoc_removal;

  // sequence methods
  e2node_cfg_upd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeConfigurationUpdate ::= SEQUENCE
using e2node_cfg_upd_s = elementary_procedure_option<e2node_cfg_upd_ies_container>;

struct e2node_cfg_upd_ack_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                               e2node_component_cfg_addition_ack_present = false;
  bool                                               e2node_component_cfg_upd_ack_present      = false;
  bool                                               e2node_component_cfg_removal_ack_present  = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true> > transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<e2node_component_cfg_addition_ack_item_ies_o>, 1, 1024, true> >
      e2node_component_cfg_addition_ack;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<e2node_component_cfg_upd_ack_item_ies_o>, 1, 1024, true> >
      e2node_component_cfg_upd_ack;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<e2node_component_cfg_removal_ack_item_ies_o>, 1, 1024, true> >
      e2node_component_cfg_removal_ack;

  // sequence methods
  e2node_cfg_upd_ack_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeConfigurationUpdateAcknowledge ::= SEQUENCE
using e2node_cfg_upd_ack_s = elementary_procedure_option<e2node_cfg_upd_ack_ies_container>;

struct e2node_cfg_upd_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                               time_to_wait_present     = false;
  bool                                               crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true> > transaction_id;
  ie_field_s<cause_c>                                cause;
  ie_field_s<time_to_wait_e>                         time_to_wait;
  ie_field_s<crit_diagnostics_s>                     crit_diagnostics;

  // sequence methods
  e2node_cfg_upd_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2nodeConfigurationUpdateFailure ::= SEQUENCE
using e2node_cfg_upd_fail_s = elementary_procedure_option<e2node_cfg_upd_fail_ies_container>;

struct e2setup_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                               time_to_wait_present     = false;
  bool                                               crit_diagnostics_present = false;
  bool                                               tn_linfo_present         = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true> > transaction_id;
  ie_field_s<cause_c>                                cause;
  ie_field_s<time_to_wait_e>                         time_to_wait;
  ie_field_s<crit_diagnostics_s>                     crit_diagnostics;
  ie_field_s<tn_linfo_s>                             tn_linfo;

  // sequence methods
  e2setup_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2setupFailure ::= SEQUENCE
using e2setup_fail_s = elementary_procedure_option<e2setup_fail_ies_container>;

struct e2setup_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true> >                                             transaction_id;
  ie_field_s<global_e2node_id_c>                                                                 global_e2node_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ra_nfunction_item_ies_o>, 1, 256, true> > ra_nfunctions_added;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<e2node_component_cfg_addition_item_ies_o>, 1, 1024, true> >
      e2node_component_cfg_addition;

  // sequence methods
  e2setup_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2setupRequest ::= SEQUENCE
using e2setup_request_s = elementary_procedure_option<e2setup_request_ies_container>;

struct e2setup_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                               ra_nfunctions_accepted_present = false;
  bool                                               ra_nfunctions_rejected_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true> > transaction_id;
  ie_field_s<global_ric_id_s>                        global_ric_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ra_nfunction_id_item_ies_o>, 1, 256, true> >
      ra_nfunctions_accepted;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ra_nfunction_idcause_item_ies_o>, 1, 256, true> >
      ra_nfunctions_rejected;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<e2node_component_cfg_addition_ack_item_ies_o>, 1, 1024, true> >
      e2node_component_cfg_addition_ack;

  // sequence methods
  e2setup_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// E2setupResponse ::= SEQUENCE
using e2setup_resp_s = elementary_procedure_option<e2setup_resp_ies_container>;

struct error_ind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                 transaction_id_present   = false;
  bool                                                 ri_crequest_id_present   = false;
  bool                                                 ra_nfunction_id_present  = false;
  bool                                                 cause_present            = false;
  bool                                                 crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true> >   transaction_id;
  ie_field_s<ri_crequest_id_s>                         ri_crequest_id;
  ie_field_s<integer<uint16_t, 0, 4095, false, true> > ra_nfunction_id;
  ie_field_s<cause_c>                                  cause;
  ie_field_s<crit_diagnostics_s>                       crit_diagnostics;

  // sequence methods
  error_ind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ErrorIndication ::= SEQUENCE
using error_ind_s = elementary_procedure_option<error_ind_ies_container>;

struct ri_cctrl_ack_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                 ri_ccall_process_id_present = false;
  bool                                                 ri_cctrl_outcome_present    = false;
  ie_field_s<ri_crequest_id_s>                         ri_crequest_id;
  ie_field_s<integer<uint16_t, 0, 4095, false, true> > ra_nfunction_id;
  ie_field_s<unbounded_octstring<true> >               ri_ccall_process_id;
  ie_field_s<unbounded_octstring<true> >               ri_cctrl_outcome;

  // sequence methods
  ri_cctrl_ack_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICcontrolAcknowledge ::= SEQUENCE
using ri_cctrl_ack_s = elementary_procedure_option<ri_cctrl_ack_ies_container>;

struct ri_cctrl_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                 ri_ccall_process_id_present = false;
  bool                                                 ri_cctrl_outcome_present    = false;
  ie_field_s<ri_crequest_id_s>                         ri_crequest_id;
  ie_field_s<integer<uint16_t, 0, 4095, false, true> > ra_nfunction_id;
  ie_field_s<unbounded_octstring<true> >               ri_ccall_process_id;
  ie_field_s<cause_c>                                  cause;
  ie_field_s<unbounded_octstring<true> >               ri_cctrl_outcome;

  // sequence methods
  ri_cctrl_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICcontrolFailure ::= SEQUENCE
using ri_cctrl_fail_s = elementary_procedure_option<ri_cctrl_fail_ies_container>;

struct ri_cctrl_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                 ri_ccall_process_id_present  = false;
  bool                                                 ri_cctrl_ack_request_present = false;
  ie_field_s<ri_crequest_id_s>                         ri_crequest_id;
  ie_field_s<integer<uint16_t, 0, 4095, false, true> > ra_nfunction_id;
  ie_field_s<unbounded_octstring<true> >               ri_ccall_process_id;
  ie_field_s<unbounded_octstring<true> >               ri_cctrl_hdr;
  ie_field_s<unbounded_octstring<true> >               ri_cctrl_msg;
  ie_field_s<ri_cctrl_ack_request_e>                   ri_cctrl_ack_request;

  // sequence methods
  ri_cctrl_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICcontrolRequest ::= SEQUENCE
using ri_cctrl_request_s = elementary_procedure_option<ri_cctrl_request_ies_container>;

struct ri_cind_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                  ri_cind_sn_present          = false;
  bool                                                  ri_ccall_process_id_present = false;
  ie_field_s<ri_crequest_id_s>                          ri_crequest_id;
  ie_field_s<integer<uint16_t, 0, 4095, false, true> >  ra_nfunction_id;
  ie_field_s<integer<uint16_t, 0, 255, false, true> >   ri_caction_id;
  ie_field_s<integer<uint32_t, 0, 65535, false, true> > ri_cind_sn;
  ie_field_s<ri_cind_type_e>                            ri_cind_type;
  ie_field_s<unbounded_octstring<true> >                ri_cind_hdr;
  ie_field_s<unbounded_octstring<true> >                ri_cind_msg;
  ie_field_s<unbounded_octstring<true> >                ri_ccall_process_id;

  // sequence methods
  ri_cind_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICindication ::= SEQUENCE
using ri_cind_s = elementary_procedure_option<ri_cind_ies_container>;

struct ricservice_query_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                               ra_nfunctions_accepted_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true> > transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ra_nfunction_id_item_ies_o>, 1, 256, true> >
      ra_nfunctions_accepted;

  // sequence methods
  ricservice_query_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICserviceQuery ::= SEQUENCE
using ricservice_query_s = elementary_procedure_option<ricservice_query_ies_container>;

struct ricservice_upd_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                               ra_nfunctions_added_present    = false;
  bool                                               ra_nfunctions_modified_present = false;
  bool                                               ra_nfunctions_deleted_present  = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true> > transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ra_nfunction_item_ies_o>, 1, 256, true> > ra_nfunctions_added;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ra_nfunction_item_ies_o>, 1, 256, true> > ra_nfunctions_modified;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ra_nfunction_id_item_ies_o>, 1, 256, true> >
      ra_nfunctions_deleted;

  // sequence methods
  ricservice_upd_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICserviceUpdate ::= SEQUENCE
using ricservice_upd_s = elementary_procedure_option<ricservice_upd_ies_container>;

struct ricservice_upd_ack_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                               ra_nfunctions_accepted_present = false;
  bool                                               ra_nfunctions_rejected_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true> > transaction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ra_nfunction_id_item_ies_o>, 1, 256, true> >
      ra_nfunctions_accepted;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ra_nfunction_idcause_item_ies_o>, 1, 256, true> >
      ra_nfunctions_rejected;

  // sequence methods
  ricservice_upd_ack_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICserviceUpdateAcknowledge ::= SEQUENCE
using ricservice_upd_ack_s = elementary_procedure_option<ricservice_upd_ack_ies_container>;

struct ricservice_upd_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                               time_to_wait_present     = false;
  bool                                               crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true> > transaction_id;
  ie_field_s<cause_c>                                cause;
  ie_field_s<time_to_wait_e>                         time_to_wait;
  ie_field_s<crit_diagnostics_s>                     crit_diagnostics;

  // sequence methods
  ricservice_upd_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICserviceUpdateFailure ::= SEQUENCE
using ricservice_upd_fail_s = elementary_procedure_option<ricservice_upd_fail_ies_container>;

struct ricsubscription_delete_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                 crit_diagnostics_present = false;
  ie_field_s<ri_crequest_id_s>                         ri_crequest_id;
  ie_field_s<integer<uint16_t, 0, 4095, false, true> > ra_nfunction_id;
  ie_field_s<cause_c>                                  cause;
  ie_field_s<crit_diagnostics_s>                       crit_diagnostics;

  // sequence methods
  ricsubscription_delete_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICsubscriptionDeleteFailure ::= SEQUENCE
using ricsubscription_delete_fail_s = elementary_procedure_option<ricsubscription_delete_fail_ies_container>;

struct ricsubscription_delete_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<ri_crequest_id_s>                         ri_crequest_id;
  ie_field_s<integer<uint16_t, 0, 4095, false, true> > ra_nfunction_id;

  // sequence methods
  ricsubscription_delete_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICsubscriptionDeleteRequest ::= SEQUENCE
using ricsubscription_delete_request_s = elementary_procedure_option<ricsubscription_delete_request_ies_container>;

// RICsubscriptionDeleteRequired ::= SEQUENCE
using ricsubscription_delete_required_s =
    elementary_procedure_option<protocol_ie_container_l<ricsubscription_delete_required_ies_o> >;

struct ricsubscription_delete_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<ri_crequest_id_s>                         ri_crequest_id;
  ie_field_s<integer<uint16_t, 0, 4095, false, true> > ra_nfunction_id;

  // sequence methods
  ricsubscription_delete_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICsubscriptionDeleteResponse ::= SEQUENCE
using ricsubscription_delete_resp_s = elementary_procedure_option<ricsubscription_delete_resp_ies_container>;

struct ricsubscription_fail_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                 crit_diagnostics_present = false;
  ie_field_s<ri_crequest_id_s>                         ri_crequest_id;
  ie_field_s<integer<uint16_t, 0, 4095, false, true> > ra_nfunction_id;
  ie_field_s<cause_c>                                  cause;
  ie_field_s<crit_diagnostics_s>                       crit_diagnostics;

  // sequence methods
  ricsubscription_fail_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICsubscriptionFailure ::= SEQUENCE
using ricsubscription_fail_s = elementary_procedure_option<ricsubscription_fail_ies_container>;

struct ricsubscription_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<ri_crequest_id_s>                         ri_crequest_id;
  ie_field_s<integer<uint16_t, 0, 4095, false, true> > ra_nfunction_id;
  ie_field_s<ricsubscription_details_s>                ricsubscription_details;

  // sequence methods
  ricsubscription_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICsubscriptionRequest ::= SEQUENCE
using ricsubscription_request_s = elementary_procedure_option<ricsubscription_request_ies_container>;

struct ricsubscription_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                                 ri_cactions_not_admitted_present = false;
  ie_field_s<ri_crequest_id_s>                         ri_crequest_id;
  ie_field_s<integer<uint16_t, 0, 4095, false, true> > ra_nfunction_id;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ri_caction_admitted_item_ies_o>, 1, 16, true> >
      ri_cactions_admitted;
  ie_field_s<dyn_seq_of<protocol_ie_single_container_s<ri_caction_not_admitted_item_ies_o>, 0, 16, true> >
      ri_cactions_not_admitted;

  // sequence methods
  ricsubscription_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// RICsubscriptionResponse ::= SEQUENCE
using ricsubscription_resp_s = elementary_procedure_option<ricsubscription_resp_ies_container>;

struct reset_request_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  ie_field_s<integer<uint16_t, 0, 255, true, true> > transaction_id;
  ie_field_s<cause_c>                                cause;

  // sequence methods
  reset_request_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResetRequest ::= SEQUENCE
using reset_request_s = elementary_procedure_option<reset_request_ies_container>;

struct reset_resp_ies_container {
  template <class valueT_>
  using ie_field_s = protocol_ie_container_item_s<valueT_>;

  // member variables
  bool                                               crit_diagnostics_present = false;
  ie_field_s<integer<uint16_t, 0, 255, true, true> > transaction_id;
  ie_field_s<crit_diagnostics_s>                     crit_diagnostics;

  // sequence methods
  reset_resp_ies_container();
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
};

// ResetResponse ::= SEQUENCE
using reset_resp_s = elementary_procedure_option<reset_resp_ies_container>;

// E2AP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF E2AP-ELEMENTARY-PROCEDURE
struct e2_ap_elem_procs_o {
  // InitiatingMessage ::= OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        ricsubscription_request,
        ricsubscription_delete_request,
        ricservice_upd,
        ri_cctrl_request,
        e2setup_request,
        e2node_cfg_upd,
        e2conn_upd,
        reset_request,
        e2_removal_request,
        ri_cind,
        ricservice_query,
        error_ind,
        ricsubscription_delete_required,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    init_msg_c() = default;
    init_msg_c(const init_msg_c& other);
    init_msg_c& operator=(const init_msg_c& other);
    ~init_msg_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ricsubscription_request_s&               ricsubscription_request();
    ricsubscription_delete_request_s&        ricsubscription_delete_request();
    ricservice_upd_s&                        ricservice_upd();
    ri_cctrl_request_s&                      ri_cctrl_request();
    e2setup_request_s&                       e2setup_request();
    e2node_cfg_upd_s&                        e2node_cfg_upd();
    e2conn_upd_s&                            e2conn_upd();
    reset_request_s&                         reset_request();
    e2_removal_request_s&                    e2_removal_request();
    ri_cind_s&                               ri_cind();
    ricservice_query_s&                      ricservice_query();
    error_ind_s&                             error_ind();
    ricsubscription_delete_required_s&       ricsubscription_delete_required();
    const ricsubscription_request_s&         ricsubscription_request() const;
    const ricsubscription_delete_request_s&  ricsubscription_delete_request() const;
    const ricservice_upd_s&                  ricservice_upd() const;
    const ri_cctrl_request_s&                ri_cctrl_request() const;
    const e2setup_request_s&                 e2setup_request() const;
    const e2node_cfg_upd_s&                  e2node_cfg_upd() const;
    const e2conn_upd_s&                      e2conn_upd() const;
    const reset_request_s&                   reset_request() const;
    const e2_removal_request_s&              e2_removal_request() const;
    const ri_cind_s&                         ri_cind() const;
    const ricservice_query_s&                ricservice_query() const;
    const error_ind_s&                       error_ind() const;
    const ricsubscription_delete_required_s& ricsubscription_delete_required() const;

  private:
    types type_;
    choice_buffer_t<e2_removal_request_s,
                    e2conn_upd_s,
                    e2node_cfg_upd_s,
                    e2setup_request_s,
                    error_ind_s,
                    reset_request_s,
                    ri_cctrl_request_s,
                    ri_cind_s,
                    ricservice_query_s,
                    ricservice_upd_s,
                    ricsubscription_delete_request_s,
                    ricsubscription_delete_required_s,
                    ricsubscription_request_s>
        c;

    void destroy_();
  };
  // SuccessfulOutcome ::= OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options {
        ricsubscription_resp,
        ricsubscription_delete_resp,
        ricservice_upd_ack,
        ri_cctrl_ack,
        e2setup_resp,
        e2node_cfg_upd_ack,
        e2conn_upd_ack,
        reset_resp,
        e2_removal_resp,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    successful_outcome_c() = default;
    successful_outcome_c(const successful_outcome_c& other);
    successful_outcome_c& operator=(const successful_outcome_c& other);
    ~successful_outcome_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ricsubscription_resp_s&              ricsubscription_resp();
    ricsubscription_delete_resp_s&       ricsubscription_delete_resp();
    ricservice_upd_ack_s&                ricservice_upd_ack();
    ri_cctrl_ack_s&                      ri_cctrl_ack();
    e2setup_resp_s&                      e2setup_resp();
    e2node_cfg_upd_ack_s&                e2node_cfg_upd_ack();
    e2conn_upd_ack_s&                    e2conn_upd_ack();
    reset_resp_s&                        reset_resp();
    e2_removal_resp_s&                   e2_removal_resp();
    const ricsubscription_resp_s&        ricsubscription_resp() const;
    const ricsubscription_delete_resp_s& ricsubscription_delete_resp() const;
    const ricservice_upd_ack_s&          ricservice_upd_ack() const;
    const ri_cctrl_ack_s&                ri_cctrl_ack() const;
    const e2setup_resp_s&                e2setup_resp() const;
    const e2node_cfg_upd_ack_s&          e2node_cfg_upd_ack() const;
    const e2conn_upd_ack_s&              e2conn_upd_ack() const;
    const reset_resp_s&                  reset_resp() const;
    const e2_removal_resp_s&             e2_removal_resp() const;

  private:
    types type_;
    choice_buffer_t<e2_removal_resp_s,
                    e2conn_upd_ack_s,
                    e2node_cfg_upd_ack_s,
                    e2setup_resp_s,
                    reset_resp_s,
                    ri_cctrl_ack_s,
                    ricservice_upd_ack_s,
                    ricsubscription_delete_resp_s,
                    ricsubscription_resp_s>
        c;

    void destroy_();
  };
  // UnsuccessfulOutcome ::= OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options {
        ricsubscription_fail,
        ricsubscription_delete_fail,
        ricservice_upd_fail,
        ri_cctrl_fail,
        e2setup_fail,
        e2node_cfg_upd_fail,
        e2conn_upd_fail,
        e2_removal_fail,
        nulltype
      } value;

      const char* to_string() const;
    };
    typedef enumerated<types_opts> types;

    // choice methods
    unsuccessful_outcome_c() = default;
    unsuccessful_outcome_c(const unsuccessful_outcome_c& other);
    unsuccessful_outcome_c& operator=(const unsuccessful_outcome_c& other);
    ~unsuccessful_outcome_c() { destroy_(); }
    void        set(types::options e = types::nulltype);
    types       type() const { return type_; }
    SRSASN_CODE pack(bit_ref& bref) const;
    SRSASN_CODE unpack(cbit_ref& bref);
    void        to_json(json_writer& j) const;
    // getters
    ricsubscription_fail_s&              ricsubscription_fail();
    ricsubscription_delete_fail_s&       ricsubscription_delete_fail();
    ricservice_upd_fail_s&               ricservice_upd_fail();
    ri_cctrl_fail_s&                     ri_cctrl_fail();
    e2setup_fail_s&                      e2setup_fail();
    e2node_cfg_upd_fail_s&               e2node_cfg_upd_fail();
    e2conn_upd_fail_s&                   e2conn_upd_fail();
    e2_removal_fail_s&                   e2_removal_fail();
    const ricsubscription_fail_s&        ricsubscription_fail() const;
    const ricsubscription_delete_fail_s& ricsubscription_delete_fail() const;
    const ricservice_upd_fail_s&         ricservice_upd_fail() const;
    const ri_cctrl_fail_s&               ri_cctrl_fail() const;
    const e2setup_fail_s&                e2setup_fail() const;
    const e2node_cfg_upd_fail_s&         e2node_cfg_upd_fail() const;
    const e2conn_upd_fail_s&             e2conn_upd_fail() const;
    const e2_removal_fail_s&             e2_removal_fail() const;

  private:
    types type_;
    choice_buffer_t<e2_removal_fail_s,
                    e2conn_upd_fail_s,
                    e2node_cfg_upd_fail_s,
                    e2setup_fail_s,
                    ri_cctrl_fail_s,
                    ricservice_upd_fail_s,
                    ricsubscription_delete_fail_s,
                    ricsubscription_fail_s>
        c;

    void destroy_();
  };

  // members lookup methods
  static uint16_t               idx_to_proc_code(uint32_t idx);
  static bool                   is_proc_code_valid(const uint16_t& proc_code);
  static init_msg_c             get_init_msg(const uint16_t& proc_code);
  static successful_outcome_c   get_successful_outcome(const uint16_t& proc_code);
  static unsuccessful_outcome_c get_unsuccessful_outcome(const uint16_t& proc_code);
  static crit_e                 get_crit(const uint16_t& proc_code);
};

// InitiatingMessage ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
struct init_msg_s {
  uint16_t                       proc_code = 0;
  crit_e                         crit;
  e2_ap_elem_procs_o::init_msg_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// SuccessfulOutcome ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
struct successful_outcome_s {
  uint16_t                                 proc_code = 0;
  crit_e                                   crit;
  e2_ap_elem_procs_o::successful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// UnsuccessfulOutcome ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
struct unsuccessful_outcome_s {
  uint16_t                                   proc_code = 0;
  crit_e                                     crit;
  e2_ap_elem_procs_o::unsuccessful_outcome_c value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint16_t& proc_code_);
};

// E2AP-PDU ::= CHOICE
struct e2_ap_pdu_c {
  struct types_opts {
    enum options { init_msg, successful_outcome, unsuccessful_outcome, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  typedef enumerated<types_opts, true> types;

  // choice methods
  e2_ap_pdu_c() = default;
  e2_ap_pdu_c(const e2_ap_pdu_c& other);
  e2_ap_pdu_c& operator=(const e2_ap_pdu_c& other);
  ~e2_ap_pdu_c() { destroy_(); }
  void        set(types::options e = types::nulltype);
  types       type() const { return type_; }
  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  // getters
  init_msg_s& init_msg()
  {
    assert_choice_type(types::init_msg, type_, "E2AP-PDU");
    return c.get<init_msg_s>();
  }
  successful_outcome_s& successful_outcome()
  {
    assert_choice_type(types::successful_outcome, type_, "E2AP-PDU");
    return c.get<successful_outcome_s>();
  }
  unsuccessful_outcome_s& unsuccessful_outcome()
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "E2AP-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  const init_msg_s& init_msg() const
  {
    assert_choice_type(types::init_msg, type_, "E2AP-PDU");
    return c.get<init_msg_s>();
  }
  const successful_outcome_s& successful_outcome() const
  {
    assert_choice_type(types::successful_outcome, type_, "E2AP-PDU");
    return c.get<successful_outcome_s>();
  }
  const unsuccessful_outcome_s& unsuccessful_outcome() const
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "E2AP-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  init_msg_s&             set_init_msg();
  successful_outcome_s&   set_successful_outcome();
  unsuccessful_outcome_s& set_unsuccessful_outcome();

private:
  types                                                                     type_;
  choice_buffer_t<init_msg_s, successful_outcome_s, unsuccessful_outcome_s> c;

  void destroy_();
};

// ProtocolIE-FieldPair{E2AP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE{{E2AP-PROTOCOL-IES-PAIR}}
template <class ies_set_paramT_>
struct protocol_ie_field_pair_s {
  uint32_t                                 id = 0;
  crit_e                                   first_crit;
  typename ies_set_paramT_::first_value_c  first_value;
  crit_e                                   second_crit;
  typename ies_set_paramT_::second_value_c second_value;

  SRSASN_CODE pack(bit_ref& bref) const;
  SRSASN_CODE unpack(cbit_ref& bref);
  void        to_json(json_writer& j) const;
  bool        load_info_obj(const uint32_t& id_);
};

// ProtocolIE-ContainerPair{E2AP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE (SIZE (0..65535)) OF ProtocolIE-FieldPair
template <class ies_set_paramT_>
using protocol_ie_container_pair_l = dyn_seq_of<protocol_ie_field_pair_s<ies_set_paramT_>, 0, 65535, true>;

} // namespace e2ap
} // namespace asn1

extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ri_caction_to_be_setup_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2conn_setup_failed_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2conn_upd_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2conn_upd_rem_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2node_component_cfg_addition_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2node_component_cfg_addition_ack_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2node_component_cfg_removal_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2node_component_cfg_removal_ack_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2node_component_cfg_upd_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2node_component_cfg_upd_ack_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::e2node_tn_lassoc_removal_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ra_nfunction_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ra_nfunction_id_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ra_nfunction_idcause_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ri_caction_admitted_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ri_caction_not_admitted_item_ies_o>;
extern template struct asn1::protocol_ie_single_container_s<asn1::e2ap::ricsubscription_with_cause_item_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2_removal_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2_removal_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2conn_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2conn_upd_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2conn_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2node_cfg_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2node_cfg_upd_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2node_cfg_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2setup_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2setup_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::e2setup_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::error_ind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ri_cctrl_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ri_cctrl_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ri_cctrl_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ri_cind_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ricservice_query_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ricservice_upd_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ricservice_upd_ack_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ricservice_upd_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ricsubscription_delete_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ricsubscription_delete_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ricsubscription_delete_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ricsubscription_fail_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ricsubscription_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::ricsubscription_resp_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::reset_request_ies_o>;
extern template struct asn1::protocol_ie_field_s<asn1::e2ap::reset_resp_ies_o>;

#endif // SRSASN1_E2AP_H
