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

#include "srsran/asn1/e2ap.h"
#include <sstream>

using namespace asn1;
using namespace asn1::e2ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// CauseE2node ::= ENUMERATED
const char* cause_e2node_opts::to_string() const
{
  static const char* options[] = {"e2node-component-unknown"};
  return convert_enum_idx(options, 1, value, "cause_e2node_e");
}
uint8_t cause_e2node_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "cause_e2node_e");
}

// CauseMisc ::= ENUMERATED
const char* cause_misc_opts::to_string() const
{
  static const char* options[] = {"control-processing-overload", "hardware-failure", "om-intervention", "unspecified"};
  return convert_enum_idx(options, 4, value, "cause_misc_e");
}

// CauseProtocol ::= ENUMERATED
const char* cause_protocol_opts::to_string() const
{
  static const char* options[] = {"transfer-syntax-error",
                                  "abstract-syntax-error-reject",
                                  "abstract-syntax-error-ignore-and-notify",
                                  "message-not-compatible-with-receiver-state",
                                  "semantic-error",
                                  "abstract-syntax-error-falsely-constructed-message",
                                  "unspecified"};
  return convert_enum_idx(options, 7, value, "cause_protocol_e");
}

// CauseRICrequest ::= ENUMERATED
const char* cause_ri_crequest_opts::to_string() const
{
  static const char* options[] = {"ran-function-id-invalid",
                                  "action-not-supported",
                                  "excessive-actions",
                                  "duplicate-action",
                                  "duplicate-event-trigger",
                                  "function-resource-limit",
                                  "request-id-unknown",
                                  "inconsistent-action-subsequent-action-sequence",
                                  "control-message-invalid",
                                  "ric-call-process-id-invalid",
                                  "control-timer-expired",
                                  "control-failed-to-execute",
                                  "system-not-ready",
                                  "unspecified"};
  return convert_enum_idx(options, 14, value, "cause_ri_crequest_e");
}

// CauseRICservice ::= ENUMERATED
const char* cause_ricservice_opts::to_string() const
{
  static const char* options[] = {"ran-function-not-supported", "excessive-functions", "ric-resource-limit"};
  return convert_enum_idx(options, 3, value, "cause_ricservice_e");
}

// CauseTransport ::= ENUMERATED
const char* cause_transport_opts::to_string() const
{
  static const char* options[] = {"unspecified", "transport-resource-unavailable"};
  return convert_enum_idx(options, 2, value, "cause_transport_e");
}

// Cause ::= CHOICE
void cause_c::destroy_() {}
void cause_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
cause_c::cause_c(const cause_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ric_request:
      c.init(other.c.get<cause_ri_crequest_e>());
      break;
    case types::ric_service:
      c.init(other.c.get<cause_ricservice_e>());
      break;
    case types::e2_node:
      c.init(other.c.get<cause_e2node_e>());
      break;
    case types::transport:
      c.init(other.c.get<cause_transport_e>());
      break;
    case types::protocol:
      c.init(other.c.get<cause_protocol_e>());
      break;
    case types::misc:
      c.init(other.c.get<cause_misc_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cause_c");
  }
}
cause_c& cause_c::operator=(const cause_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ric_request:
      c.set(other.c.get<cause_ri_crequest_e>());
      break;
    case types::ric_service:
      c.set(other.c.get<cause_ricservice_e>());
      break;
    case types::e2_node:
      c.set(other.c.get<cause_e2node_e>());
      break;
    case types::transport:
      c.set(other.c.get<cause_transport_e>());
      break;
    case types::protocol:
      c.set(other.c.get<cause_protocol_e>());
      break;
    case types::misc:
      c.set(other.c.get<cause_misc_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cause_c");
  }

  return *this;
}
cause_ri_crequest_e& cause_c::set_ric_request()
{
  set(types::ric_request);
  return c.get<cause_ri_crequest_e>();
}
cause_ricservice_e& cause_c::set_ric_service()
{
  set(types::ric_service);
  return c.get<cause_ricservice_e>();
}
cause_e2node_e& cause_c::set_e2_node()
{
  set(types::e2_node);
  return c.get<cause_e2node_e>();
}
cause_transport_e& cause_c::set_transport()
{
  set(types::transport);
  return c.get<cause_transport_e>();
}
cause_protocol_e& cause_c::set_protocol()
{
  set(types::protocol);
  return c.get<cause_protocol_e>();
}
cause_misc_e& cause_c::set_misc()
{
  set(types::misc);
  return c.get<cause_misc_e>();
}
void cause_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ric_request:
      j.write_str("ricRequest", c.get<cause_ri_crequest_e>().to_string());
      break;
    case types::ric_service:
      j.write_str("ricService", c.get<cause_ricservice_e>().to_string());
      break;
    case types::e2_node:
      j.write_str("e2Node", "e2node-component-unknown");
      break;
    case types::transport:
      j.write_str("transport", c.get<cause_transport_e>().to_string());
      break;
    case types::protocol:
      j.write_str("protocol", c.get<cause_protocol_e>().to_string());
      break;
    case types::misc:
      j.write_str("misc", c.get<cause_misc_e>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "cause_c");
  }
  j.end_obj();
}
SRSASN_CODE cause_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ric_request:
      HANDLE_CODE(c.get<cause_ri_crequest_e>().pack(bref));
      break;
    case types::ric_service:
      HANDLE_CODE(c.get<cause_ricservice_e>().pack(bref));
      break;
    case types::e2_node:
      HANDLE_CODE(c.get<cause_e2node_e>().pack(bref));
      break;
    case types::transport:
      HANDLE_CODE(c.get<cause_transport_e>().pack(bref));
      break;
    case types::protocol:
      HANDLE_CODE(c.get<cause_protocol_e>().pack(bref));
      break;
    case types::misc:
      HANDLE_CODE(c.get<cause_misc_e>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cause_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cause_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ric_request:
      HANDLE_CODE(c.get<cause_ri_crequest_e>().unpack(bref));
      break;
    case types::ric_service:
      HANDLE_CODE(c.get<cause_ricservice_e>().unpack(bref));
      break;
    case types::e2_node:
      HANDLE_CODE(c.get<cause_e2node_e>().unpack(bref));
      break;
    case types::transport:
      HANDLE_CODE(c.get<cause_transport_e>().unpack(bref));
      break;
    case types::protocol:
      HANDLE_CODE(c.get<cause_protocol_e>().unpack(bref));
      break;
    case types::misc:
      HANDLE_CODE(c.get<cause_misc_e>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cause_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cause_c::types_opts::to_string() const
{
  static const char* options[] = {"ricRequest", "ricService", "e2Node", "transport", "protocol", "misc"};
  return convert_enum_idx(options, 6, value, "cause_c::types");
}
uint8_t cause_c::types_opts::to_number() const
{
  if (value == e2_node) {
    return 2;
  }
  invalid_enum_number(value, "cause_c::types");
  return 0;
}

// TypeOfError ::= ENUMERATED
const char* type_of_error_opts::to_string() const
{
  static const char* options[] = {"not-understood", "missing"};
  return convert_enum_idx(options, 2, value, "type_of_error_e");
}

// CriticalityDiagnostics-IE-Item ::= SEQUENCE
SRSASN_CODE crit_diagnostics_ie_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(iecrit.pack(bref));
  HANDLE_CODE(pack_integer(bref, ie_id, (uint32_t)0u, (uint32_t)65535u, false, true));
  HANDLE_CODE(type_of_error.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE crit_diagnostics_ie_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(iecrit.unpack(bref));
  HANDLE_CODE(unpack_integer(ie_id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
  HANDLE_CODE(type_of_error.unpack(bref));

  return SRSASN_SUCCESS;
}
void crit_diagnostics_ie_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("iECriticality", iecrit.to_string());
  j.write_int("iE-ID", ie_id);
  j.write_str("typeOfError", type_of_error.to_string());
  j.end_obj();
}

// RICrequestID ::= SEQUENCE
SRSASN_CODE ri_crequest_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_requestor_id, (uint32_t)0u, (uint32_t)65535u, false, true));
  HANDLE_CODE(pack_integer(bref, ric_instance_id, (uint32_t)0u, (uint32_t)65535u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ri_crequest_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_requestor_id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
  HANDLE_CODE(unpack_integer(ric_instance_id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));

  return SRSASN_SUCCESS;
}
void ri_crequest_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricRequestorID", ric_requestor_id);
  j.write_int("ricInstanceID", ric_instance_id);
  j.end_obj();
}

// TriggeringMessage ::= ENUMERATED
const char* trigger_msg_opts::to_string() const
{
  static const char* options[] = {"initiating-message", "successful-outcome", "unsuccessfull-outcome"};
  return convert_enum_idx(options, 3, value, "trigger_msg_e");
}

// CriticalityDiagnostics ::= SEQUENCE
SRSASN_CODE crit_diagnostics_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(proc_code_present, 1));
  HANDLE_CODE(bref.pack(trigger_msg_present, 1));
  HANDLE_CODE(bref.pack(proc_crit_present, 1));
  HANDLE_CODE(bref.pack(ric_requestor_id_present, 1));
  HANDLE_CODE(bref.pack(ies_crit_diagnostics.size() > 0, 1));

  if (proc_code_present) {
    HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (trigger_msg_present) {
    HANDLE_CODE(trigger_msg.pack(bref));
  }
  if (proc_crit_present) {
    HANDLE_CODE(proc_crit.pack(bref));
  }
  if (ric_requestor_id_present) {
    HANDLE_CODE(ric_requestor_id.pack(bref));
  }
  if (ies_crit_diagnostics.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ies_crit_diagnostics, 1, 256, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE crit_diagnostics_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(proc_code_present, 1));
  HANDLE_CODE(bref.unpack(trigger_msg_present, 1));
  HANDLE_CODE(bref.unpack(proc_crit_present, 1));
  HANDLE_CODE(bref.unpack(ric_requestor_id_present, 1));
  bool ies_crit_diagnostics_present;
  HANDLE_CODE(bref.unpack(ies_crit_diagnostics_present, 1));

  if (proc_code_present) {
    HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (trigger_msg_present) {
    HANDLE_CODE(trigger_msg.unpack(bref));
  }
  if (proc_crit_present) {
    HANDLE_CODE(proc_crit.unpack(bref));
  }
  if (ric_requestor_id_present) {
    HANDLE_CODE(ric_requestor_id.unpack(bref));
  }
  if (ies_crit_diagnostics_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ies_crit_diagnostics, bref, 1, 256, true));
  }

  return SRSASN_SUCCESS;
}
void crit_diagnostics_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (proc_code_present) {
    j.write_int("procedureCode", proc_code);
  }
  if (trigger_msg_present) {
    j.write_str("triggeringMessage", trigger_msg.to_string());
  }
  if (proc_crit_present) {
    j.write_str("procedureCriticality", proc_crit.to_string());
  }
  if (ric_requestor_id_present) {
    j.write_fieldname("ricRequestorID");
    ric_requestor_id.to_json(j);
  }
  if (ies_crit_diagnostics.size() > 0) {
    j.start_array("iEsCriticalityDiagnostics");
    for (const auto& e1 : ies_crit_diagnostics) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

// ENB-ID-Choice ::= CHOICE
void enb_id_choice_c::destroy_()
{
  switch (type_) {
    case types::enb_id_macro:
      c.destroy<fixed_bitstring<20, false, true> >();
      break;
    case types::enb_id_shortmacro:
      c.destroy<fixed_bitstring<18, false, true> >();
      break;
    case types::enb_id_longmacro:
      c.destroy<fixed_bitstring<21, false, true> >();
      break;
    default:
      break;
  }
}
void enb_id_choice_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::enb_id_macro:
      c.init<fixed_bitstring<20, false, true> >();
      break;
    case types::enb_id_shortmacro:
      c.init<fixed_bitstring<18, false, true> >();
      break;
    case types::enb_id_longmacro:
      c.init<fixed_bitstring<21, false, true> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_choice_c");
  }
}
enb_id_choice_c::enb_id_choice_c(const enb_id_choice_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::enb_id_macro:
      c.init(other.c.get<fixed_bitstring<20, false, true> >());
      break;
    case types::enb_id_shortmacro:
      c.init(other.c.get<fixed_bitstring<18, false, true> >());
      break;
    case types::enb_id_longmacro:
      c.init(other.c.get<fixed_bitstring<21, false, true> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_choice_c");
  }
}
enb_id_choice_c& enb_id_choice_c::operator=(const enb_id_choice_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::enb_id_macro:
      c.set(other.c.get<fixed_bitstring<20, false, true> >());
      break;
    case types::enb_id_shortmacro:
      c.set(other.c.get<fixed_bitstring<18, false, true> >());
      break;
    case types::enb_id_longmacro:
      c.set(other.c.get<fixed_bitstring<21, false, true> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_choice_c");
  }

  return *this;
}
fixed_bitstring<20, false, true>& enb_id_choice_c::set_enb_id_macro()
{
  set(types::enb_id_macro);
  return c.get<fixed_bitstring<20, false, true> >();
}
fixed_bitstring<18, false, true>& enb_id_choice_c::set_enb_id_shortmacro()
{
  set(types::enb_id_shortmacro);
  return c.get<fixed_bitstring<18, false, true> >();
}
fixed_bitstring<21, false, true>& enb_id_choice_c::set_enb_id_longmacro()
{
  set(types::enb_id_longmacro);
  return c.get<fixed_bitstring<21, false, true> >();
}
void enb_id_choice_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::enb_id_macro:
      j.write_str("enb-ID-macro", c.get<fixed_bitstring<20, false, true> >().to_string());
      break;
    case types::enb_id_shortmacro:
      j.write_str("enb-ID-shortmacro", c.get<fixed_bitstring<18, false, true> >().to_string());
      break;
    case types::enb_id_longmacro:
      j.write_str("enb-ID-longmacro", c.get<fixed_bitstring<21, false, true> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_choice_c");
  }
  j.end_obj();
}
SRSASN_CODE enb_id_choice_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::enb_id_macro:
      HANDLE_CODE((c.get<fixed_bitstring<20, false, true> >().pack(bref)));
      break;
    case types::enb_id_shortmacro:
      HANDLE_CODE((c.get<fixed_bitstring<18, false, true> >().pack(bref)));
      break;
    case types::enb_id_longmacro:
      HANDLE_CODE((c.get<fixed_bitstring<21, false, true> >().pack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_choice_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE enb_id_choice_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::enb_id_macro:
      HANDLE_CODE((c.get<fixed_bitstring<20, false, true> >().unpack(bref)));
      break;
    case types::enb_id_shortmacro:
      HANDLE_CODE((c.get<fixed_bitstring<18, false, true> >().unpack(bref)));
      break;
    case types::enb_id_longmacro:
      HANDLE_CODE((c.get<fixed_bitstring<21, false, true> >().unpack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_choice_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* enb_id_choice_c::types_opts::to_string() const
{
  static const char* options[] = {"enb-ID-macro", "enb-ID-shortmacro", "enb-ID-longmacro"};
  return convert_enum_idx(options, 3, value, "enb_id_choice_c::types");
}

// GNB-ID-Choice ::= CHOICE
void gnb_id_choice_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("gnb-ID", c.to_string());
  j.end_obj();
}
SRSASN_CODE gnb_id_choice_c::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_id_choice_c::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "gnb_id_choice_c");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* gnb_id_choice_c::types_opts::to_string() const
{
  static const char* options[] = {"gnb-ID"};
  return convert_enum_idx(options, 1, value, "gnb_id_choice_c::types");
}

// ENB-ID ::= CHOICE
void enb_id_c::destroy_()
{
  switch (type_) {
    case types::macro_enb_id:
      c.destroy<fixed_bitstring<20, false, true> >();
      break;
    case types::home_enb_id:
      c.destroy<fixed_bitstring<28, false, true> >();
      break;
    case types::short_macro_enb_id:
      c.destroy<fixed_bitstring<18, false, true> >();
      break;
    case types::long_macro_enb_id:
      c.destroy<fixed_bitstring<21, false, true> >();
      break;
    default:
      break;
  }
}
void enb_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::macro_enb_id:
      c.init<fixed_bitstring<20, false, true> >();
      break;
    case types::home_enb_id:
      c.init<fixed_bitstring<28, false, true> >();
      break;
    case types::short_macro_enb_id:
      c.init<fixed_bitstring<18, false, true> >();
      break;
    case types::long_macro_enb_id:
      c.init<fixed_bitstring<21, false, true> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_c");
  }
}
enb_id_c::enb_id_c(const enb_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::macro_enb_id:
      c.init(other.c.get<fixed_bitstring<20, false, true> >());
      break;
    case types::home_enb_id:
      c.init(other.c.get<fixed_bitstring<28, false, true> >());
      break;
    case types::short_macro_enb_id:
      c.init(other.c.get<fixed_bitstring<18, false, true> >());
      break;
    case types::long_macro_enb_id:
      c.init(other.c.get<fixed_bitstring<21, false, true> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_c");
  }
}
enb_id_c& enb_id_c::operator=(const enb_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::macro_enb_id:
      c.set(other.c.get<fixed_bitstring<20, false, true> >());
      break;
    case types::home_enb_id:
      c.set(other.c.get<fixed_bitstring<28, false, true> >());
      break;
    case types::short_macro_enb_id:
      c.set(other.c.get<fixed_bitstring<18, false, true> >());
      break;
    case types::long_macro_enb_id:
      c.set(other.c.get<fixed_bitstring<21, false, true> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_c");
  }

  return *this;
}
fixed_bitstring<20, false, true>& enb_id_c::set_macro_enb_id()
{
  set(types::macro_enb_id);
  return c.get<fixed_bitstring<20, false, true> >();
}
fixed_bitstring<28, false, true>& enb_id_c::set_home_enb_id()
{
  set(types::home_enb_id);
  return c.get<fixed_bitstring<28, false, true> >();
}
fixed_bitstring<18, false, true>& enb_id_c::set_short_macro_enb_id()
{
  set(types::short_macro_enb_id);
  return c.get<fixed_bitstring<18, false, true> >();
}
fixed_bitstring<21, false, true>& enb_id_c::set_long_macro_enb_id()
{
  set(types::long_macro_enb_id);
  return c.get<fixed_bitstring<21, false, true> >();
}
void enb_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::macro_enb_id:
      j.write_str("macro-eNB-ID", c.get<fixed_bitstring<20, false, true> >().to_string());
      break;
    case types::home_enb_id:
      j.write_str("home-eNB-ID", c.get<fixed_bitstring<28, false, true> >().to_string());
      break;
    case types::short_macro_enb_id:
      j.write_str("short-Macro-eNB-ID", c.get<fixed_bitstring<18, false, true> >().to_string());
      break;
    case types::long_macro_enb_id:
      j.write_str("long-Macro-eNB-ID", c.get<fixed_bitstring<21, false, true> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "enb_id_c");
  }
  j.end_obj();
}
SRSASN_CODE enb_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::macro_enb_id:
      HANDLE_CODE((c.get<fixed_bitstring<20, false, true> >().pack(bref)));
      break;
    case types::home_enb_id:
      HANDLE_CODE((c.get<fixed_bitstring<28, false, true> >().pack(bref)));
      break;
    case types::short_macro_enb_id: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE((c.get<fixed_bitstring<18, false, true> >().pack(bref)));
    } break;
    case types::long_macro_enb_id: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE((c.get<fixed_bitstring<21, false, true> >().pack(bref)));
    } break;
    default:
      log_invalid_choice_id(type_, "enb_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE enb_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::macro_enb_id:
      HANDLE_CODE((c.get<fixed_bitstring<20, false, true> >().unpack(bref)));
      break;
    case types::home_enb_id:
      HANDLE_CODE((c.get<fixed_bitstring<28, false, true> >().unpack(bref)));
      break;
    case types::short_macro_enb_id: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE((c.get<fixed_bitstring<18, false, true> >().unpack(bref)));
    } break;
    case types::long_macro_enb_id: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE((c.get<fixed_bitstring<21, false, true> >().unpack(bref)));
    } break;
    default:
      log_invalid_choice_id(type_, "enb_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* enb_id_c::types_opts::to_string() const
{
  static const char* options[] = {"macro-eNB-ID", "home-eNB-ID", "short-Macro-eNB-ID", "long-Macro-eNB-ID"};
  return convert_enum_idx(options, 4, value, "enb_id_c::types");
}

// ENGNB-ID ::= CHOICE
void engnb_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("gNB-ID", c.to_string());
  j.end_obj();
}
SRSASN_CODE engnb_id_c::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE engnb_id_c::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "engnb_id_c");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* engnb_id_c::types_opts::to_string() const
{
  static const char* options[] = {"gNB-ID"};
  return convert_enum_idx(options, 1, value, "engnb_id_c::types");
}

// GlobalgNB-ID ::= SEQUENCE
SRSASN_CODE globalg_nb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(gnb_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE globalg_nb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(gnb_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void globalg_nb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("plmn-id", plmn_id.to_string());
  j.write_fieldname("gnb-id");
  gnb_id.to_json(j);
  j.end_obj();
}

// GlobalngeNB-ID ::= SEQUENCE
SRSASN_CODE globalngenb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(enb_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE globalngenb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(enb_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void globalngenb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("plmn-id", plmn_id.to_string());
  j.write_fieldname("enb-id");
  enb_id.to_json(j);
  j.end_obj();
}

// GlobalENB-ID ::= SEQUENCE
SRSASN_CODE global_enb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(enb_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE global_enb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(enb_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void global_enb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("pLMN-Identity", plmn_id.to_string());
  j.write_fieldname("eNB-ID");
  enb_id.to_json(j);
  j.end_obj();
}

// GlobalNG-RANNode-ID ::= CHOICE
void global_ng_ran_node_id_c::destroy_()
{
  switch (type_) {
    case types::gnb:
      c.destroy<globalg_nb_id_s>();
      break;
    case types::ng_enb:
      c.destroy<globalngenb_id_s>();
      break;
    default:
      break;
  }
}
void global_ng_ran_node_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb:
      c.init<globalg_nb_id_s>();
      break;
    case types::ng_enb:
      c.init<globalngenb_id_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "global_ng_ran_node_id_c");
  }
}
global_ng_ran_node_id_c::global_ng_ran_node_id_c(const global_ng_ran_node_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb:
      c.init(other.c.get<globalg_nb_id_s>());
      break;
    case types::ng_enb:
      c.init(other.c.get<globalngenb_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "global_ng_ran_node_id_c");
  }
}
global_ng_ran_node_id_c& global_ng_ran_node_id_c::operator=(const global_ng_ran_node_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb:
      c.set(other.c.get<globalg_nb_id_s>());
      break;
    case types::ng_enb:
      c.set(other.c.get<globalngenb_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "global_ng_ran_node_id_c");
  }

  return *this;
}
globalg_nb_id_s& global_ng_ran_node_id_c::set_gnb()
{
  set(types::gnb);
  return c.get<globalg_nb_id_s>();
}
globalngenb_id_s& global_ng_ran_node_id_c::set_ng_enb()
{
  set(types::ng_enb);
  return c.get<globalngenb_id_s>();
}
void global_ng_ran_node_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb:
      j.write_fieldname("gNB");
      c.get<globalg_nb_id_s>().to_json(j);
      break;
    case types::ng_enb:
      j.write_fieldname("ng-eNB");
      c.get<globalngenb_id_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "global_ng_ran_node_id_c");
  }
  j.end_obj();
}
SRSASN_CODE global_ng_ran_node_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::gnb:
      HANDLE_CODE(c.get<globalg_nb_id_s>().pack(bref));
      break;
    case types::ng_enb:
      HANDLE_CODE(c.get<globalngenb_id_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "global_ng_ran_node_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE global_ng_ran_node_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::gnb:
      HANDLE_CODE(c.get<globalg_nb_id_s>().unpack(bref));
      break;
    case types::ng_enb:
      HANDLE_CODE(c.get<globalngenb_id_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "global_ng_ran_node_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* global_ng_ran_node_id_c::types_opts::to_string() const
{
  static const char* options[] = {"gNB", "ng-eNB"};
  return convert_enum_idx(options, 2, value, "global_ng_ran_node_id_c::types");
}

// GlobalenGNB-ID ::= SEQUENCE
SRSASN_CODE globalen_gnb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(gnb_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE globalen_gnb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(gnb_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void globalen_gnb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("pLMN-Identity", plmn_id.to_string());
  j.write_fieldname("gNB-ID");
  gnb_id.to_json(j);
  j.end_obj();
}

// RICsubsequentActionType ::= ENUMERATED
const char* ricsubsequent_action_type_opts::to_string() const
{
  static const char* options[] = {"continue", "wait"};
  return convert_enum_idx(options, 2, value, "ricsubsequent_action_type_e");
}

// RICtimeToWait ::= ENUMERATED
const char* ri_ctime_to_wait_opts::to_string() const
{
  static const char* options[] = {"w1ms",
                                  "w2ms",
                                  "w5ms",
                                  "w10ms",
                                  "w20ms",
                                  "w30ms",
                                  "w40ms",
                                  "w50ms",
                                  "w100ms",
                                  "w200ms",
                                  "w500ms",
                                  "w1s",
                                  "w2s",
                                  "w5s",
                                  "w10s",
                                  "w20s",
                                  "w60s"};
  return convert_enum_idx(options, 17, value, "ri_ctime_to_wait_e");
}

// E2nodeComponentInterfaceE1 ::= SEQUENCE
SRSASN_CODE e2node_component_interface_e1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, gnb_cu_cp_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_interface_e1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(gnb_cu_cp_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
void e2node_component_interface_e1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("gNB-CU-CP-ID", gnb_cu_cp_id);
  j.end_obj();
}

// E2nodeComponentInterfaceF1 ::= SEQUENCE
SRSASN_CODE e2node_component_interface_f1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, gnb_du_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_interface_f1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(gnb_du_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
void e2node_component_interface_f1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("gNB-DU-ID", gnb_du_id);
  j.end_obj();
}

// E2nodeComponentInterfaceNG ::= SEQUENCE
SRSASN_CODE e2node_component_interface_ng_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(amf_name.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_interface_ng_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(amf_name.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_interface_ng_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("amf-name", amf_name.to_string());
  j.end_obj();
}

// E2nodeComponentInterfaceS1 ::= SEQUENCE
SRSASN_CODE e2node_component_interface_s1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(mme_name.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_interface_s1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(mme_name.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_interface_s1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("mme-name", mme_name.to_string());
  j.end_obj();
}

// E2nodeComponentInterfaceW1 ::= SEQUENCE
SRSASN_CODE e2node_component_interface_w1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ng_enb_du_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_interface_w1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ng_enb_du_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
void e2node_component_interface_w1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ng-eNB-DU-ID", ng_enb_du_id);
  j.end_obj();
}

// E2nodeComponentInterfaceX2 ::= SEQUENCE
SRSASN_CODE e2node_component_interface_x2_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(global_enb_id_present, 1));
  HANDLE_CODE(bref.pack(global_en_g_nb_id_present, 1));

  if (global_enb_id_present) {
    HANDLE_CODE(global_enb_id.pack(bref));
  }
  if (global_en_g_nb_id_present) {
    HANDLE_CODE(global_en_g_nb_id.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_interface_x2_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(global_enb_id_present, 1));
  HANDLE_CODE(bref.unpack(global_en_g_nb_id_present, 1));

  if (global_enb_id_present) {
    HANDLE_CODE(global_enb_id.unpack(bref));
  }
  if (global_en_g_nb_id_present) {
    HANDLE_CODE(global_en_g_nb_id.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void e2node_component_interface_x2_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (global_enb_id_present) {
    j.write_fieldname("global-eNB-ID");
    global_enb_id.to_json(j);
  }
  if (global_en_g_nb_id_present) {
    j.write_fieldname("global-en-gNB-ID");
    global_en_g_nb_id.to_json(j);
  }
  j.end_obj();
}

// E2nodeComponentInterfaceXn ::= SEQUENCE
SRSASN_CODE e2node_component_interface_xn_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(global_ng_ran_node_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_interface_xn_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(global_ng_ran_node_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_interface_xn_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("global-NG-RAN-Node-ID");
  global_ng_ran_node_id.to_json(j);
  j.end_obj();
}

// RICactionType ::= ENUMERATED
const char* ri_caction_type_opts::to_string() const
{
  static const char* options[] = {"report", "insert", "policy"};
  return convert_enum_idx(options, 3, value, "ri_caction_type_e");
}

// RICsubsequentAction ::= SEQUENCE
SRSASN_CODE ricsubsequent_action_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ric_subsequent_action_type.pack(bref));
  HANDLE_CODE(ric_time_to_wait.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ricsubsequent_action_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ric_subsequent_action_type.unpack(bref));
  HANDLE_CODE(ric_time_to_wait.unpack(bref));

  return SRSASN_SUCCESS;
}
void ricsubsequent_action_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ricSubsequentActionType", ric_subsequent_action_type.to_string());
  j.write_str("ricTimeToWait", ric_time_to_wait.to_string());
  j.end_obj();
}

// E2nodeComponentConfiguration ::= SEQUENCE
SRSASN_CODE e2node_component_cfg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(e2node_component_request_part.pack(bref));
  HANDLE_CODE(e2node_component_resp_part.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(e2node_component_request_part.unpack(bref));
  HANDLE_CODE(e2node_component_resp_part.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e2nodeComponentRequestPart", e2node_component_request_part.to_string());
  j.write_str("e2nodeComponentResponsePart", e2node_component_resp_part.to_string());
  j.end_obj();
}

// E2nodeComponentConfigurationAck ::= SEQUENCE
SRSASN_CODE e2node_component_cfg_ack_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(fail_cause_present, 1));

  HANDLE_CODE(upd_outcome.pack(bref));
  if (fail_cause_present) {
    HANDLE_CODE(fail_cause.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_ack_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(fail_cause_present, 1));

  HANDLE_CODE(upd_outcome.unpack(bref));
  if (fail_cause_present) {
    HANDLE_CODE(fail_cause.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void e2node_component_cfg_ack_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("updateOutcome", upd_outcome.to_string());
  if (fail_cause_present) {
    j.write_fieldname("failureCause");
    fail_cause.to_json(j);
  }
  j.end_obj();
}

const char* e2node_component_cfg_ack_s::upd_outcome_opts::to_string() const
{
  static const char* options[] = {"success", "failure"};
  return convert_enum_idx(options, 2, value, "e2node_component_cfg_ack_s::upd_outcome_e_");
}

// E2nodeComponentID ::= CHOICE
void e2node_component_id_c::destroy_()
{
  switch (type_) {
    case types::e2node_component_interface_type_ng:
      c.destroy<e2node_component_interface_ng_s>();
      break;
    case types::e2node_component_interface_type_xn:
      c.destroy<e2node_component_interface_xn_s>();
      break;
    case types::e2node_component_interface_type_e1:
      c.destroy<e2node_component_interface_e1_s>();
      break;
    case types::e2node_component_interface_type_f1:
      c.destroy<e2node_component_interface_f1_s>();
      break;
    case types::e2node_component_interface_type_w1:
      c.destroy<e2node_component_interface_w1_s>();
      break;
    case types::e2node_component_interface_type_s1:
      c.destroy<e2node_component_interface_s1_s>();
      break;
    case types::e2node_component_interface_type_x2:
      c.destroy<e2node_component_interface_x2_s>();
      break;
    default:
      break;
  }
}
void e2node_component_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::e2node_component_interface_type_ng:
      c.init<e2node_component_interface_ng_s>();
      break;
    case types::e2node_component_interface_type_xn:
      c.init<e2node_component_interface_xn_s>();
      break;
    case types::e2node_component_interface_type_e1:
      c.init<e2node_component_interface_e1_s>();
      break;
    case types::e2node_component_interface_type_f1:
      c.init<e2node_component_interface_f1_s>();
      break;
    case types::e2node_component_interface_type_w1:
      c.init<e2node_component_interface_w1_s>();
      break;
    case types::e2node_component_interface_type_s1:
      c.init<e2node_component_interface_s1_s>();
      break;
    case types::e2node_component_interface_type_x2:
      c.init<e2node_component_interface_x2_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_component_id_c");
  }
}
e2node_component_id_c::e2node_component_id_c(const e2node_component_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::e2node_component_interface_type_ng:
      c.init(other.c.get<e2node_component_interface_ng_s>());
      break;
    case types::e2node_component_interface_type_xn:
      c.init(other.c.get<e2node_component_interface_xn_s>());
      break;
    case types::e2node_component_interface_type_e1:
      c.init(other.c.get<e2node_component_interface_e1_s>());
      break;
    case types::e2node_component_interface_type_f1:
      c.init(other.c.get<e2node_component_interface_f1_s>());
      break;
    case types::e2node_component_interface_type_w1:
      c.init(other.c.get<e2node_component_interface_w1_s>());
      break;
    case types::e2node_component_interface_type_s1:
      c.init(other.c.get<e2node_component_interface_s1_s>());
      break;
    case types::e2node_component_interface_type_x2:
      c.init(other.c.get<e2node_component_interface_x2_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_component_id_c");
  }
}
e2node_component_id_c& e2node_component_id_c::operator=(const e2node_component_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::e2node_component_interface_type_ng:
      c.set(other.c.get<e2node_component_interface_ng_s>());
      break;
    case types::e2node_component_interface_type_xn:
      c.set(other.c.get<e2node_component_interface_xn_s>());
      break;
    case types::e2node_component_interface_type_e1:
      c.set(other.c.get<e2node_component_interface_e1_s>());
      break;
    case types::e2node_component_interface_type_f1:
      c.set(other.c.get<e2node_component_interface_f1_s>());
      break;
    case types::e2node_component_interface_type_w1:
      c.set(other.c.get<e2node_component_interface_w1_s>());
      break;
    case types::e2node_component_interface_type_s1:
      c.set(other.c.get<e2node_component_interface_s1_s>());
      break;
    case types::e2node_component_interface_type_x2:
      c.set(other.c.get<e2node_component_interface_x2_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_component_id_c");
  }

  return *this;
}
e2node_component_interface_ng_s& e2node_component_id_c::set_e2node_component_interface_type_ng()
{
  set(types::e2node_component_interface_type_ng);
  return c.get<e2node_component_interface_ng_s>();
}
e2node_component_interface_xn_s& e2node_component_id_c::set_e2node_component_interface_type_xn()
{
  set(types::e2node_component_interface_type_xn);
  return c.get<e2node_component_interface_xn_s>();
}
e2node_component_interface_e1_s& e2node_component_id_c::set_e2node_component_interface_type_e1()
{
  set(types::e2node_component_interface_type_e1);
  return c.get<e2node_component_interface_e1_s>();
}
e2node_component_interface_f1_s& e2node_component_id_c::set_e2node_component_interface_type_f1()
{
  set(types::e2node_component_interface_type_f1);
  return c.get<e2node_component_interface_f1_s>();
}
e2node_component_interface_w1_s& e2node_component_id_c::set_e2node_component_interface_type_w1()
{
  set(types::e2node_component_interface_type_w1);
  return c.get<e2node_component_interface_w1_s>();
}
e2node_component_interface_s1_s& e2node_component_id_c::set_e2node_component_interface_type_s1()
{
  set(types::e2node_component_interface_type_s1);
  return c.get<e2node_component_interface_s1_s>();
}
e2node_component_interface_x2_s& e2node_component_id_c::set_e2node_component_interface_type_x2()
{
  set(types::e2node_component_interface_type_x2);
  return c.get<e2node_component_interface_x2_s>();
}
void e2node_component_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::e2node_component_interface_type_ng:
      j.write_fieldname("e2nodeComponentInterfaceTypeNG");
      c.get<e2node_component_interface_ng_s>().to_json(j);
      break;
    case types::e2node_component_interface_type_xn:
      j.write_fieldname("e2nodeComponentInterfaceTypeXn");
      c.get<e2node_component_interface_xn_s>().to_json(j);
      break;
    case types::e2node_component_interface_type_e1:
      j.write_fieldname("e2nodeComponentInterfaceTypeE1");
      c.get<e2node_component_interface_e1_s>().to_json(j);
      break;
    case types::e2node_component_interface_type_f1:
      j.write_fieldname("e2nodeComponentInterfaceTypeF1");
      c.get<e2node_component_interface_f1_s>().to_json(j);
      break;
    case types::e2node_component_interface_type_w1:
      j.write_fieldname("e2nodeComponentInterfaceTypeW1");
      c.get<e2node_component_interface_w1_s>().to_json(j);
      break;
    case types::e2node_component_interface_type_s1:
      j.write_fieldname("e2nodeComponentInterfaceTypeS1");
      c.get<e2node_component_interface_s1_s>().to_json(j);
      break;
    case types::e2node_component_interface_type_x2:
      j.write_fieldname("e2nodeComponentInterfaceTypeX2");
      c.get<e2node_component_interface_x2_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2node_component_id_c");
  }
  j.end_obj();
}
SRSASN_CODE e2node_component_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::e2node_component_interface_type_ng:
      HANDLE_CODE(c.get<e2node_component_interface_ng_s>().pack(bref));
      break;
    case types::e2node_component_interface_type_xn:
      HANDLE_CODE(c.get<e2node_component_interface_xn_s>().pack(bref));
      break;
    case types::e2node_component_interface_type_e1:
      HANDLE_CODE(c.get<e2node_component_interface_e1_s>().pack(bref));
      break;
    case types::e2node_component_interface_type_f1:
      HANDLE_CODE(c.get<e2node_component_interface_f1_s>().pack(bref));
      break;
    case types::e2node_component_interface_type_w1:
      HANDLE_CODE(c.get<e2node_component_interface_w1_s>().pack(bref));
      break;
    case types::e2node_component_interface_type_s1:
      HANDLE_CODE(c.get<e2node_component_interface_s1_s>().pack(bref));
      break;
    case types::e2node_component_interface_type_x2:
      HANDLE_CODE(c.get<e2node_component_interface_x2_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2node_component_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::e2node_component_interface_type_ng:
      HANDLE_CODE(c.get<e2node_component_interface_ng_s>().unpack(bref));
      break;
    case types::e2node_component_interface_type_xn:
      HANDLE_CODE(c.get<e2node_component_interface_xn_s>().unpack(bref));
      break;
    case types::e2node_component_interface_type_e1:
      HANDLE_CODE(c.get<e2node_component_interface_e1_s>().unpack(bref));
      break;
    case types::e2node_component_interface_type_f1:
      HANDLE_CODE(c.get<e2node_component_interface_f1_s>().unpack(bref));
      break;
    case types::e2node_component_interface_type_w1:
      HANDLE_CODE(c.get<e2node_component_interface_w1_s>().unpack(bref));
      break;
    case types::e2node_component_interface_type_s1:
      HANDLE_CODE(c.get<e2node_component_interface_s1_s>().unpack(bref));
      break;
    case types::e2node_component_interface_type_x2:
      HANDLE_CODE(c.get<e2node_component_interface_x2_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2node_component_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2node_component_id_c::types_opts::to_string() const
{
  static const char* options[] = {"e2nodeComponentInterfaceTypeNG",
                                  "e2nodeComponentInterfaceTypeXn",
                                  "e2nodeComponentInterfaceTypeE1",
                                  "e2nodeComponentInterfaceTypeF1",
                                  "e2nodeComponentInterfaceTypeW1",
                                  "e2nodeComponentInterfaceTypeS1",
                                  "e2nodeComponentInterfaceTypeX2"};
  return convert_enum_idx(options, 7, value, "e2node_component_id_c::types");
}

// E2nodeComponentInterfaceType ::= ENUMERATED
const char* e2node_component_interface_type_opts::to_string() const
{
  static const char* options[] = {"ng", "xn", "e1", "f1", "w1", "s1", "x2"};
  return convert_enum_idx(options, 7, value, "e2node_component_interface_type_e");
}

// RICaction-ToBeSetup-Item ::= SEQUENCE
SRSASN_CODE ri_caction_to_be_setup_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ric_action_definition.size() > 0, 1));
  HANDLE_CODE(bref.pack(ric_subsequent_action_present, 1));

  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(ric_action_type.pack(bref));
  if (ric_action_definition.size() > 0) {
    HANDLE_CODE(ric_action_definition.pack(bref));
  }
  if (ric_subsequent_action_present) {
    HANDLE_CODE(ric_subsequent_action.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ri_caction_to_be_setup_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool ric_action_definition_present;
  HANDLE_CODE(bref.unpack(ric_action_definition_present, 1));
  HANDLE_CODE(bref.unpack(ric_subsequent_action_present, 1));

  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(ric_action_type.unpack(bref));
  if (ric_action_definition_present) {
    HANDLE_CODE(ric_action_definition.unpack(bref));
  }
  if (ric_subsequent_action_present) {
    HANDLE_CODE(ric_subsequent_action.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ri_caction_to_be_setup_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.write_str("ricActionType", ric_action_type.to_string());
  if (ric_action_definition.size() > 0) {
    j.write_str("ricActionDefinition", ric_action_definition.to_string());
  }
  if (ric_subsequent_action_present) {
    j.write_fieldname("ricSubsequentAction");
    ric_subsequent_action.to_json(j);
  }
  j.end_obj();
}

// TNLinformation ::= SEQUENCE
SRSASN_CODE tn_linfo_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(tnl_port_present, 1));

  HANDLE_CODE(tnl_address.pack(bref));
  if (tnl_port_present) {
    HANDLE_CODE(tnl_port.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE tn_linfo_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(tnl_port_present, 1));

  HANDLE_CODE(tnl_address.unpack(bref));
  if (tnl_port_present) {
    HANDLE_CODE(tnl_port.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void tn_linfo_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("tnlAddress", tnl_address.to_string());
  if (tnl_port_present) {
    j.write_str("tnlPort", tnl_port.to_string());
  }
  j.end_obj();
}

// TNLusage ::= ENUMERATED
const char* tn_lusage_opts::to_string() const
{
  static const char* options[] = {"ric-service", "support-function", "both"};
  return convert_enum_idx(options, 3, value, "tn_lusage_e");
}

// E2connectionSetupFailed-Item ::= SEQUENCE
SRSASN_CODE e2conn_setup_failed_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(tnl_info.pack(bref));
  HANDLE_CODE(cause.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_setup_failed_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(tnl_info.unpack(bref));
  HANDLE_CODE(cause.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2conn_setup_failed_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tnlInformation");
  tnl_info.to_json(j);
  j.write_fieldname("cause");
  cause.to_json(j);
  j.end_obj();
}

// E2connectionUpdate-Item ::= SEQUENCE
SRSASN_CODE e2conn_upd_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(tnl_info.pack(bref));
  HANDLE_CODE(tnl_usage.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(tnl_info.unpack(bref));
  HANDLE_CODE(tnl_usage.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2conn_upd_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tnlInformation");
  tnl_info.to_json(j);
  j.write_str("tnlUsage", tnl_usage.to_string());
  j.end_obj();
}

// E2connectionUpdateRemove-Item ::= SEQUENCE
SRSASN_CODE e2conn_upd_rem_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(tnl_info.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_rem_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(tnl_info.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2conn_upd_rem_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tnlInformation");
  tnl_info.to_json(j);
  j.end_obj();
}

// E2nodeComponentConfigAddition-Item ::= SEQUENCE
SRSASN_CODE e2node_component_cfg_addition_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.pack(bref));
  HANDLE_CODE(e2node_component_id.pack(bref));
  HANDLE_CODE(e2node_component_cfg.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_addition_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.unpack(bref));
  HANDLE_CODE(e2node_component_id.unpack(bref));
  HANDLE_CODE(e2node_component_cfg.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_cfg_addition_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e2nodeComponentInterfaceType", e2node_component_interface_type.to_string());
  j.write_fieldname("e2nodeComponentID");
  e2node_component_id.to_json(j);
  j.write_fieldname("e2nodeComponentConfiguration");
  e2node_component_cfg.to_json(j);
  j.end_obj();
}

// E2nodeComponentConfigAdditionAck-Item ::= SEQUENCE
SRSASN_CODE e2node_component_cfg_addition_ack_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.pack(bref));
  HANDLE_CODE(e2node_component_id.pack(bref));
  HANDLE_CODE(e2node_component_cfg_ack.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_addition_ack_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.unpack(bref));
  HANDLE_CODE(e2node_component_id.unpack(bref));
  HANDLE_CODE(e2node_component_cfg_ack.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_cfg_addition_ack_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e2nodeComponentInterfaceType", e2node_component_interface_type.to_string());
  j.write_fieldname("e2nodeComponentID");
  e2node_component_id.to_json(j);
  j.write_fieldname("e2nodeComponentConfigurationAck");
  e2node_component_cfg_ack.to_json(j);
  j.end_obj();
}

// E2nodeComponentConfigRemoval-Item ::= SEQUENCE
SRSASN_CODE e2node_component_cfg_removal_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.pack(bref));
  HANDLE_CODE(e2node_component_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_removal_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.unpack(bref));
  HANDLE_CODE(e2node_component_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_cfg_removal_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e2nodeComponentInterfaceType", e2node_component_interface_type.to_string());
  j.write_fieldname("e2nodeComponentID");
  e2node_component_id.to_json(j);
  j.end_obj();
}

// E2nodeComponentConfigRemovalAck-Item ::= SEQUENCE
SRSASN_CODE e2node_component_cfg_removal_ack_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.pack(bref));
  HANDLE_CODE(e2node_component_id.pack(bref));
  HANDLE_CODE(e2node_component_cfg_ack.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_removal_ack_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.unpack(bref));
  HANDLE_CODE(e2node_component_id.unpack(bref));
  HANDLE_CODE(e2node_component_cfg_ack.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_cfg_removal_ack_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e2nodeComponentInterfaceType", e2node_component_interface_type.to_string());
  j.write_fieldname("e2nodeComponentID");
  e2node_component_id.to_json(j);
  j.write_fieldname("e2nodeComponentConfigurationAck");
  e2node_component_cfg_ack.to_json(j);
  j.end_obj();
}

// E2nodeComponentConfigUpdate-Item ::= SEQUENCE
SRSASN_CODE e2node_component_cfg_upd_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.pack(bref));
  HANDLE_CODE(e2node_component_id.pack(bref));
  HANDLE_CODE(e2node_component_cfg.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_upd_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.unpack(bref));
  HANDLE_CODE(e2node_component_id.unpack(bref));
  HANDLE_CODE(e2node_component_cfg.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_cfg_upd_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e2nodeComponentInterfaceType", e2node_component_interface_type.to_string());
  j.write_fieldname("e2nodeComponentID");
  e2node_component_id.to_json(j);
  j.write_fieldname("e2nodeComponentConfiguration");
  e2node_component_cfg.to_json(j);
  j.end_obj();
}

// E2nodeComponentConfigUpdateAck-Item ::= SEQUENCE
SRSASN_CODE e2node_component_cfg_upd_ack_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.pack(bref));
  HANDLE_CODE(e2node_component_id.pack(bref));
  HANDLE_CODE(e2node_component_cfg_ack.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_upd_ack_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(e2node_component_interface_type.unpack(bref));
  HANDLE_CODE(e2node_component_id.unpack(bref));
  HANDLE_CODE(e2node_component_cfg_ack.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_component_cfg_upd_ack_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("e2nodeComponentInterfaceType", e2node_component_interface_type.to_string());
  j.write_fieldname("e2nodeComponentID");
  e2node_component_id.to_json(j);
  j.write_fieldname("e2nodeComponentConfigurationAck");
  e2node_component_cfg_ack.to_json(j);
  j.end_obj();
}

// E2nodeTNLassociationRemoval-Item ::= SEQUENCE
SRSASN_CODE e2node_tn_lassoc_removal_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(tnl_info.pack(bref));
  HANDLE_CODE(tnl_info_ric.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_tn_lassoc_removal_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(tnl_info.unpack(bref));
  HANDLE_CODE(tnl_info_ric.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2node_tn_lassoc_removal_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("tnlInformation");
  tnl_info.to_json(j);
  j.write_fieldname("tnlInformationRIC");
  tnl_info_ric.to_json(j);
  j.end_obj();
}

// RANfunction-Item ::= SEQUENCE
SRSASN_CODE ra_nfunction_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(ran_function_definition.pack(bref));
  HANDLE_CODE(pack_integer(bref, ran_function_revision, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(ran_function_oid.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ra_nfunction_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(ran_function_definition.unpack(bref));
  HANDLE_CODE(unpack_integer(ran_function_revision, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(ran_function_oid.unpack(bref));

  return SRSASN_SUCCESS;
}
void ra_nfunction_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ranFunctionID", ran_function_id);
  j.write_str("ranFunctionDefinition", ran_function_definition.to_string());
  j.write_int("ranFunctionRevision", ran_function_revision);
  j.write_str("ranFunctionOID", ran_function_oid.to_string());
  j.end_obj();
}

// RANfunctionID-Item ::= SEQUENCE
SRSASN_CODE ra_nfunction_id_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(pack_integer(bref, ran_function_revision, (uint16_t)0u, (uint16_t)4095u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ra_nfunction_id_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(unpack_integer(ran_function_revision, bref, (uint16_t)0u, (uint16_t)4095u, false, true));

  return SRSASN_SUCCESS;
}
void ra_nfunction_id_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ranFunctionID", ran_function_id);
  j.write_int("ranFunctionRevision", ran_function_revision);
  j.end_obj();
}

// RANfunctionIDcause-Item ::= SEQUENCE
SRSASN_CODE ra_nfunction_idcause_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(cause.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ra_nfunction_idcause_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(cause.unpack(bref));

  return SRSASN_SUCCESS;
}
void ra_nfunction_idcause_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ranFunctionID", ran_function_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  j.end_obj();
}

// RICaction-Admitted-Item ::= SEQUENCE
SRSASN_CODE ri_caction_admitted_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ri_caction_admitted_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));

  return SRSASN_SUCCESS;
}
void ri_caction_admitted_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.end_obj();
}

// RICaction-NotAdmitted-Item ::= SEQUENCE
SRSASN_CODE ri_caction_not_admitted_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ric_action_id, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ri_caction_not_admitted_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ric_action_id, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(cause.unpack(bref));

  return SRSASN_SUCCESS;
}
void ri_caction_not_admitted_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ricActionID", ric_action_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  j.end_obj();
}

// RICaction-ToBeSetup-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ri_caction_to_be_setup_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {19};
  return map_enum_number(options, 1, idx, "id");
}
bool ri_caction_to_be_setup_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 19 == id;
}
crit_e ri_caction_to_be_setup_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 19) {
    return crit_e::ignore;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
ri_caction_to_be_setup_item_ies_o::value_c ri_caction_to_be_setup_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 19) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ri_caction_to_be_setup_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 19) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ri_caction_to_be_setup_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-ToBeSetup-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ri_caction_to_be_setup_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ri_caction_to_be_setup_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ri_caction_to_be_setup_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"RICaction-ToBeSetup-Item"};
  return convert_enum_idx(options, 1, value, "ri_caction_to_be_setup_item_ies_o::value_c::types");
}

// RICsubscription-withCause-Item ::= SEQUENCE
SRSASN_CODE ricsubscription_with_cause_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ric_request_id.pack(bref));
  HANDLE_CODE(pack_integer(bref, ran_function_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(cause.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ricsubscription_with_cause_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ric_request_id.unpack(bref));
  HANDLE_CODE(unpack_integer(ran_function_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
  HANDLE_CODE(cause.unpack(bref));

  return SRSASN_SUCCESS;
}
void ricsubscription_with_cause_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ricRequestID");
  ric_request_id.to_json(j);
  j.write_int("ranFunctionID", ran_function_id);
  j.write_fieldname("cause");
  cause.to_json(j);
  j.end_obj();
}

// E2connectionSetupFailed-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2conn_setup_failed_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {41};
  return map_enum_number(options, 1, idx, "id");
}
bool e2conn_setup_failed_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 41 == id;
}
crit_e e2conn_setup_failed_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 41) {
    return crit_e::ignore;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
e2conn_setup_failed_item_ies_o::value_c e2conn_setup_failed_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 41) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2conn_setup_failed_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 41) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2conn_setup_failed_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2connectionSetupFailed-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2conn_setup_failed_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_setup_failed_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2conn_setup_failed_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"E2connectionSetupFailed-Item"};
  return convert_enum_idx(options, 1, value, "e2conn_setup_failed_item_ies_o::value_c::types");
}
uint8_t e2conn_setup_failed_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "e2conn_setup_failed_item_ies_o::value_c::types");
}

// E2connectionUpdate-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2conn_upd_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {43};
  return map_enum_number(options, 1, idx, "id");
}
bool e2conn_upd_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 43 == id;
}
crit_e e2conn_upd_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 43) {
    return crit_e::ignore;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
e2conn_upd_item_ies_o::value_c e2conn_upd_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 43) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2conn_upd_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 43) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2conn_upd_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2connectionUpdate-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2conn_upd_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2conn_upd_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"E2connectionUpdate-Item"};
  return convert_enum_idx(options, 1, value, "e2conn_upd_item_ies_o::value_c::types");
}
uint8_t e2conn_upd_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "e2conn_upd_item_ies_o::value_c::types");
}

// E2connectionUpdateRemove-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2conn_upd_rem_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {47};
  return map_enum_number(options, 1, idx, "id");
}
bool e2conn_upd_rem_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 47 == id;
}
crit_e e2conn_upd_rem_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 47) {
    return crit_e::ignore;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
e2conn_upd_rem_item_ies_o::value_c e2conn_upd_rem_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 47) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2conn_upd_rem_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 47) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2conn_upd_rem_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2connectionUpdateRemove-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2conn_upd_rem_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_rem_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2conn_upd_rem_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"E2connectionUpdateRemove-Item"};
  return convert_enum_idx(options, 1, value, "e2conn_upd_rem_item_ies_o::value_c::types");
}
uint8_t e2conn_upd_rem_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "e2conn_upd_rem_item_ies_o::value_c::types");
}

// E2nodeComponentConfigAddition-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_component_cfg_addition_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {51};
  return map_enum_number(options, 1, idx, "id");
}
bool e2node_component_cfg_addition_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 51 == id;
}
crit_e e2node_component_cfg_addition_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 51) {
    return crit_e::reject;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
e2node_component_cfg_addition_item_ies_o::value_c
e2node_component_cfg_addition_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 51) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2node_component_cfg_addition_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 51) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2node_component_cfg_addition_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2nodeComponentConfigAddition-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2node_component_cfg_addition_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_addition_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2node_component_cfg_addition_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"E2nodeComponentConfigAddition-Item"};
  return convert_enum_idx(options, 1, value, "e2node_component_cfg_addition_item_ies_o::value_c::types");
}
uint8_t e2node_component_cfg_addition_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "e2node_component_cfg_addition_item_ies_o::value_c::types");
}

// E2nodeComponentConfigAdditionAck-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_component_cfg_addition_ack_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {53};
  return map_enum_number(options, 1, idx, "id");
}
bool e2node_component_cfg_addition_ack_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 53 == id;
}
crit_e e2node_component_cfg_addition_ack_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 53) {
    return crit_e::reject;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
e2node_component_cfg_addition_ack_item_ies_o::value_c
e2node_component_cfg_addition_ack_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 53) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2node_component_cfg_addition_ack_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 53) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2node_component_cfg_addition_ack_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2nodeComponentConfigAdditionAck-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2node_component_cfg_addition_ack_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_addition_ack_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2node_component_cfg_addition_ack_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"E2nodeComponentConfigAdditionAck-Item"};
  return convert_enum_idx(options, 1, value, "e2node_component_cfg_addition_ack_item_ies_o::value_c::types");
}
uint8_t e2node_component_cfg_addition_ack_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "e2node_component_cfg_addition_ack_item_ies_o::value_c::types");
}

// E2nodeComponentConfigRemoval-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_component_cfg_removal_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {55};
  return map_enum_number(options, 1, idx, "id");
}
bool e2node_component_cfg_removal_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 55 == id;
}
crit_e e2node_component_cfg_removal_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 55) {
    return crit_e::reject;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
e2node_component_cfg_removal_item_ies_o::value_c e2node_component_cfg_removal_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 55) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2node_component_cfg_removal_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 55) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2node_component_cfg_removal_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2nodeComponentConfigRemoval-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2node_component_cfg_removal_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_removal_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2node_component_cfg_removal_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"E2nodeComponentConfigRemoval-Item"};
  return convert_enum_idx(options, 1, value, "e2node_component_cfg_removal_item_ies_o::value_c::types");
}
uint8_t e2node_component_cfg_removal_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "e2node_component_cfg_removal_item_ies_o::value_c::types");
}

// E2nodeComponentConfigRemovalAck-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_component_cfg_removal_ack_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {57};
  return map_enum_number(options, 1, idx, "id");
}
bool e2node_component_cfg_removal_ack_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 57 == id;
}
crit_e e2node_component_cfg_removal_ack_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 57) {
    return crit_e::reject;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
e2node_component_cfg_removal_ack_item_ies_o::value_c
e2node_component_cfg_removal_ack_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 57) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2node_component_cfg_removal_ack_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 57) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2node_component_cfg_removal_ack_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2nodeComponentConfigRemovalAck-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2node_component_cfg_removal_ack_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_removal_ack_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2node_component_cfg_removal_ack_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"E2nodeComponentConfigRemovalAck-Item"};
  return convert_enum_idx(options, 1, value, "e2node_component_cfg_removal_ack_item_ies_o::value_c::types");
}
uint8_t e2node_component_cfg_removal_ack_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "e2node_component_cfg_removal_ack_item_ies_o::value_c::types");
}

// E2nodeComponentConfigUpdate-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_component_cfg_upd_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {34};
  return map_enum_number(options, 1, idx, "id");
}
bool e2node_component_cfg_upd_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 34 == id;
}
crit_e e2node_component_cfg_upd_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 34) {
    return crit_e::reject;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
e2node_component_cfg_upd_item_ies_o::value_c e2node_component_cfg_upd_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 34) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2node_component_cfg_upd_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 34) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2node_component_cfg_upd_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2nodeComponentConfigUpdate-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2node_component_cfg_upd_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_upd_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2node_component_cfg_upd_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"E2nodeComponentConfigUpdate-Item"};
  return convert_enum_idx(options, 1, value, "e2node_component_cfg_upd_item_ies_o::value_c::types");
}
uint8_t e2node_component_cfg_upd_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "e2node_component_cfg_upd_item_ies_o::value_c::types");
}

// E2nodeComponentConfigUpdateAck-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_component_cfg_upd_ack_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {36};
  return map_enum_number(options, 1, idx, "id");
}
bool e2node_component_cfg_upd_ack_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 36 == id;
}
crit_e e2node_component_cfg_upd_ack_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 36) {
    return crit_e::reject;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
e2node_component_cfg_upd_ack_item_ies_o::value_c e2node_component_cfg_upd_ack_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 36) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2node_component_cfg_upd_ack_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 36) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2node_component_cfg_upd_ack_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2nodeComponentConfigUpdateAck-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2node_component_cfg_upd_ack_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_component_cfg_upd_ack_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2node_component_cfg_upd_ack_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"E2nodeComponentConfigUpdateAck-Item"};
  return convert_enum_idx(options, 1, value, "e2node_component_cfg_upd_ack_item_ies_o::value_c::types");
}
uint8_t e2node_component_cfg_upd_ack_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "e2node_component_cfg_upd_ack_item_ies_o::value_c::types");
}

// E2nodeTNLassociationRemoval-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_tn_lassoc_removal_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {59};
  return map_enum_number(options, 1, idx, "id");
}
bool e2node_tn_lassoc_removal_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 59 == id;
}
crit_e e2node_tn_lassoc_removal_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 59) {
    return crit_e::reject;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
e2node_tn_lassoc_removal_item_ies_o::value_c e2node_tn_lassoc_removal_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 59) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2node_tn_lassoc_removal_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 59) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2node_tn_lassoc_removal_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("E2nodeTNLassociationRemoval-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2node_tn_lassoc_removal_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_tn_lassoc_removal_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2node_tn_lassoc_removal_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"E2nodeTNLassociationRemoval-Item"};
  return convert_enum_idx(options, 1, value, "e2node_tn_lassoc_removal_item_ies_o::value_c::types");
}
uint8_t e2node_tn_lassoc_removal_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {2};
  return map_enum_number(options, 1, value, "e2node_tn_lassoc_removal_item_ies_o::value_c::types");
}

// GlobalE2node-eNB-ID ::= SEQUENCE
SRSASN_CODE global_e2node_enb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(global_enb_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE global_e2node_enb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(global_enb_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void global_e2node_enb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("global-eNB-ID");
  global_enb_id.to_json(j);
  j.end_obj();
}

// GlobalE2node-en-gNB-ID ::= SEQUENCE
SRSASN_CODE global_e2node_en_g_nb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(en_g_nb_cu_up_id_present, 1));
  HANDLE_CODE(bref.pack(en_g_nb_du_id_present, 1));

  HANDLE_CODE(global_en_g_nb_id.pack(bref));
  if (en_g_nb_cu_up_id_present) {
    HANDLE_CODE(pack_integer(bref, en_g_nb_cu_up_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }
  if (en_g_nb_du_id_present) {
    HANDLE_CODE(pack_integer(bref, en_g_nb_du_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE global_e2node_en_g_nb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(en_g_nb_cu_up_id_present, 1));
  HANDLE_CODE(bref.unpack(en_g_nb_du_id_present, 1));

  HANDLE_CODE(global_en_g_nb_id.unpack(bref));
  if (en_g_nb_cu_up_id_present) {
    HANDLE_CODE(unpack_integer(en_g_nb_cu_up_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }
  if (en_g_nb_du_id_present) {
    HANDLE_CODE(unpack_integer(en_g_nb_du_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }

  return SRSASN_SUCCESS;
}
void global_e2node_en_g_nb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("global-en-gNB-ID");
  global_en_g_nb_id.to_json(j);
  if (en_g_nb_cu_up_id_present) {
    j.write_int("en-gNB-CU-UP-ID", en_g_nb_cu_up_id);
  }
  if (en_g_nb_du_id_present) {
    j.write_int("en-gNB-DU-ID", en_g_nb_du_id);
  }
  j.end_obj();
}

// GlobalE2node-gNB-ID ::= SEQUENCE
SRSASN_CODE global_e2node_g_nb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(global_en_g_nb_id_present, 1));
  HANDLE_CODE(bref.pack(gnb_cu_up_id_present, 1));
  HANDLE_CODE(bref.pack(gnb_du_id_present, 1));

  HANDLE_CODE(global_g_nb_id.pack(bref));
  if (global_en_g_nb_id_present) {
    HANDLE_CODE(global_en_g_nb_id.pack(bref));
  }
  if (gnb_cu_up_id_present) {
    HANDLE_CODE(pack_integer(bref, gnb_cu_up_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }
  if (gnb_du_id_present) {
    HANDLE_CODE(pack_integer(bref, gnb_du_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE global_e2node_g_nb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(global_en_g_nb_id_present, 1));
  HANDLE_CODE(bref.unpack(gnb_cu_up_id_present, 1));
  HANDLE_CODE(bref.unpack(gnb_du_id_present, 1));

  HANDLE_CODE(global_g_nb_id.unpack(bref));
  if (global_en_g_nb_id_present) {
    HANDLE_CODE(global_en_g_nb_id.unpack(bref));
  }
  if (gnb_cu_up_id_present) {
    HANDLE_CODE(unpack_integer(gnb_cu_up_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }
  if (gnb_du_id_present) {
    HANDLE_CODE(unpack_integer(gnb_du_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }

  return SRSASN_SUCCESS;
}
void global_e2node_g_nb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("global-gNB-ID");
  global_g_nb_id.to_json(j);
  if (global_en_g_nb_id_present) {
    j.write_fieldname("global-en-gNB-ID");
    global_en_g_nb_id.to_json(j);
  }
  if (gnb_cu_up_id_present) {
    j.write_int("gNB-CU-UP-ID", gnb_cu_up_id);
  }
  if (gnb_du_id_present) {
    j.write_int("gNB-DU-ID", gnb_du_id);
  }
  j.end_obj();
}

// GlobalE2node-ng-eNB-ID ::= SEQUENCE
SRSASN_CODE global_e2node_ng_enb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(global_enb_id_present, 1));
  HANDLE_CODE(bref.pack(ng_enb_du_id_present, 1));

  HANDLE_CODE(global_ng_enb_id.pack(bref));
  if (global_enb_id_present) {
    HANDLE_CODE(global_enb_id.pack(bref));
  }
  if (ng_enb_du_id_present) {
    HANDLE_CODE(pack_integer(bref, ng_enb_du_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE global_e2node_ng_enb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(global_enb_id_present, 1));
  HANDLE_CODE(bref.unpack(ng_enb_du_id_present, 1));

  HANDLE_CODE(global_ng_enb_id.unpack(bref));
  if (global_enb_id_present) {
    HANDLE_CODE(global_enb_id.unpack(bref));
  }
  if (ng_enb_du_id_present) {
    HANDLE_CODE(unpack_integer(ng_enb_du_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));
  }

  return SRSASN_SUCCESS;
}
void global_e2node_ng_enb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("global-ng-eNB-ID");
  global_ng_enb_id.to_json(j);
  if (global_enb_id_present) {
    j.write_fieldname("global-eNB-ID");
    global_enb_id.to_json(j);
  }
  if (ng_enb_du_id_present) {
    j.write_int("ngENB-DU-ID", ng_enb_du_id);
  }
  j.end_obj();
}

// RANfunction-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ra_nfunction_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {8};
  return map_enum_number(options, 1, idx, "id");
}
bool ra_nfunction_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 8 == id;
}
crit_e ra_nfunction_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 8) {
    return crit_e::ignore;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
ra_nfunction_item_ies_o::value_c ra_nfunction_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 8) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ra_nfunction_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 8) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ra_nfunction_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RANfunction-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ra_nfunction_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ra_nfunction_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ra_nfunction_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"RANfunction-Item"};
  return convert_enum_idx(options, 1, value, "ra_nfunction_item_ies_o::value_c::types");
}

// RANfunctionID-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ra_nfunction_id_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {6};
  return map_enum_number(options, 1, idx, "id");
}
bool ra_nfunction_id_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 6 == id;
}
crit_e ra_nfunction_id_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 6) {
    return crit_e::ignore;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
ra_nfunction_id_item_ies_o::value_c ra_nfunction_id_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 6) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ra_nfunction_id_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 6) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ra_nfunction_id_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RANfunctionID-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ra_nfunction_id_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ra_nfunction_id_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ra_nfunction_id_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"RANfunctionID-Item"};
  return convert_enum_idx(options, 1, value, "ra_nfunction_id_item_ies_o::value_c::types");
}

// RANfunctionIDcause-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ra_nfunction_idcause_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {7};
  return map_enum_number(options, 1, idx, "id");
}
bool ra_nfunction_idcause_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 7 == id;
}
crit_e ra_nfunction_idcause_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 7) {
    return crit_e::ignore;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
ra_nfunction_idcause_item_ies_o::value_c ra_nfunction_idcause_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 7) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ra_nfunction_idcause_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 7) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ra_nfunction_idcause_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RANfunctionIDcause-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ra_nfunction_idcause_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ra_nfunction_idcause_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ra_nfunction_idcause_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"RANfunctionIDcause-Item"};
  return convert_enum_idx(options, 1, value, "ra_nfunction_idcause_item_ies_o::value_c::types");
}

// RICaction-Admitted-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ri_caction_admitted_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {14};
  return map_enum_number(options, 1, idx, "id");
}
bool ri_caction_admitted_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 14 == id;
}
crit_e ri_caction_admitted_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 14) {
    return crit_e::ignore;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
ri_caction_admitted_item_ies_o::value_c ri_caction_admitted_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 14) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ri_caction_admitted_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 14) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ri_caction_admitted_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-Admitted-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ri_caction_admitted_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ri_caction_admitted_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ri_caction_admitted_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"RICaction-Admitted-Item"};
  return convert_enum_idx(options, 1, value, "ri_caction_admitted_item_ies_o::value_c::types");
}

// RICaction-NotAdmitted-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ri_caction_not_admitted_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {16};
  return map_enum_number(options, 1, idx, "id");
}
bool ri_caction_not_admitted_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 16 == id;
}
crit_e ri_caction_not_admitted_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 16) {
    return crit_e::ignore;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
ri_caction_not_admitted_item_ies_o::value_c ri_caction_not_admitted_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 16) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ri_caction_not_admitted_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 16) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ri_caction_not_admitted_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICaction-NotAdmitted-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ri_caction_not_admitted_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ri_caction_not_admitted_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ri_caction_not_admitted_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"RICaction-NotAdmitted-Item"};
  return convert_enum_idx(options, 1, value, "ri_caction_not_admitted_item_ies_o::value_c::types");
}

template struct asn1::protocol_ie_single_container_s<ri_caction_to_be_setup_item_ies_o>;

// RICsubscription-withCause-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ricsubscription_with_cause_item_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {61};
  return map_enum_number(options, 1, idx, "id");
}
bool ricsubscription_with_cause_item_ies_o::is_id_valid(const uint32_t& id)
{
  return 61 == id;
}
crit_e ricsubscription_with_cause_item_ies_o::get_crit(const uint32_t& id)
{
  if (id == 61) {
    return crit_e::ignore;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
ricsubscription_with_cause_item_ies_o::value_c ricsubscription_with_cause_item_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 61) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ricsubscription_with_cause_item_ies_o::get_presence(const uint32_t& id)
{
  if (id == 61) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ricsubscription_with_cause_item_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("RICsubscription-withCause-Item");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ricsubscription_with_cause_item_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ricsubscription_with_cause_item_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* ricsubscription_with_cause_item_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"RICsubscription-withCause-Item"};
  return convert_enum_idx(options, 1, value, "ricsubscription_with_cause_item_ies_o::value_c::types");
}

template struct asn1::protocol_ie_single_container_s<e2conn_setup_failed_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2conn_upd_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2conn_upd_rem_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2node_component_cfg_addition_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2node_component_cfg_addition_ack_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2node_component_cfg_removal_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2node_component_cfg_removal_ack_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2node_component_cfg_upd_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2node_component_cfg_upd_ack_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<e2node_tn_lassoc_removal_item_ies_o>;

// GlobalE2node-ID ::= CHOICE
void global_e2node_id_c::destroy_()
{
  switch (type_) {
    case types::gnb:
      c.destroy<global_e2node_g_nb_id_s>();
      break;
    case types::en_g_nb:
      c.destroy<global_e2node_en_g_nb_id_s>();
      break;
    case types::ng_enb:
      c.destroy<global_e2node_ng_enb_id_s>();
      break;
    case types::enb:
      c.destroy<global_e2node_enb_id_s>();
      break;
    default:
      break;
  }
}
void global_e2node_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb:
      c.init<global_e2node_g_nb_id_s>();
      break;
    case types::en_g_nb:
      c.init<global_e2node_en_g_nb_id_s>();
      break;
    case types::ng_enb:
      c.init<global_e2node_ng_enb_id_s>();
      break;
    case types::enb:
      c.init<global_e2node_enb_id_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "global_e2node_id_c");
  }
}
global_e2node_id_c::global_e2node_id_c(const global_e2node_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb:
      c.init(other.c.get<global_e2node_g_nb_id_s>());
      break;
    case types::en_g_nb:
      c.init(other.c.get<global_e2node_en_g_nb_id_s>());
      break;
    case types::ng_enb:
      c.init(other.c.get<global_e2node_ng_enb_id_s>());
      break;
    case types::enb:
      c.init(other.c.get<global_e2node_enb_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "global_e2node_id_c");
  }
}
global_e2node_id_c& global_e2node_id_c::operator=(const global_e2node_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb:
      c.set(other.c.get<global_e2node_g_nb_id_s>());
      break;
    case types::en_g_nb:
      c.set(other.c.get<global_e2node_en_g_nb_id_s>());
      break;
    case types::ng_enb:
      c.set(other.c.get<global_e2node_ng_enb_id_s>());
      break;
    case types::enb:
      c.set(other.c.get<global_e2node_enb_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "global_e2node_id_c");
  }

  return *this;
}
global_e2node_g_nb_id_s& global_e2node_id_c::set_gnb()
{
  set(types::gnb);
  return c.get<global_e2node_g_nb_id_s>();
}
global_e2node_en_g_nb_id_s& global_e2node_id_c::set_en_g_nb()
{
  set(types::en_g_nb);
  return c.get<global_e2node_en_g_nb_id_s>();
}
global_e2node_ng_enb_id_s& global_e2node_id_c::set_ng_enb()
{
  set(types::ng_enb);
  return c.get<global_e2node_ng_enb_id_s>();
}
global_e2node_enb_id_s& global_e2node_id_c::set_enb()
{
  set(types::enb);
  return c.get<global_e2node_enb_id_s>();
}
void global_e2node_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb:
      j.write_fieldname("gNB");
      c.get<global_e2node_g_nb_id_s>().to_json(j);
      break;
    case types::en_g_nb:
      j.write_fieldname("en-gNB");
      c.get<global_e2node_en_g_nb_id_s>().to_json(j);
      break;
    case types::ng_enb:
      j.write_fieldname("ng-eNB");
      c.get<global_e2node_ng_enb_id_s>().to_json(j);
      break;
    case types::enb:
      j.write_fieldname("eNB");
      c.get<global_e2node_enb_id_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "global_e2node_id_c");
  }
  j.end_obj();
}
SRSASN_CODE global_e2node_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::gnb:
      HANDLE_CODE(c.get<global_e2node_g_nb_id_s>().pack(bref));
      break;
    case types::en_g_nb:
      HANDLE_CODE(c.get<global_e2node_en_g_nb_id_s>().pack(bref));
      break;
    case types::ng_enb:
      HANDLE_CODE(c.get<global_e2node_ng_enb_id_s>().pack(bref));
      break;
    case types::enb:
      HANDLE_CODE(c.get<global_e2node_enb_id_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "global_e2node_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE global_e2node_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::gnb:
      HANDLE_CODE(c.get<global_e2node_g_nb_id_s>().unpack(bref));
      break;
    case types::en_g_nb:
      HANDLE_CODE(c.get<global_e2node_en_g_nb_id_s>().unpack(bref));
      break;
    case types::ng_enb:
      HANDLE_CODE(c.get<global_e2node_ng_enb_id_s>().unpack(bref));
      break;
    case types::enb:
      HANDLE_CODE(c.get<global_e2node_enb_id_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "global_e2node_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* global_e2node_id_c::types_opts::to_string() const
{
  static const char* options[] = {"gNB", "en-gNB", "ng-eNB", "eNB"};
  return convert_enum_idx(options, 4, value, "global_e2node_id_c::types");
}

// GlobalRIC-ID ::= SEQUENCE
SRSASN_CODE global_ric_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(ric_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE global_ric_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(ric_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void global_ric_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("pLMN-Identity", plmn_id.to_string());
  j.write_str("ric-ID", ric_id.to_string());
  j.end_obj();
}

template struct asn1::protocol_ie_single_container_s<ra_nfunction_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ra_nfunction_id_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ra_nfunction_idcause_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ri_caction_admitted_item_ies_o>;

template struct asn1::protocol_ie_single_container_s<ri_caction_not_admitted_item_ies_o>;

// RICcontrolAckRequest ::= ENUMERATED
const char* ri_cctrl_ack_request_opts::to_string() const
{
  static const char* options[] = {"noAck", "ack"};
  return convert_enum_idx(options, 2, value, "ri_cctrl_ack_request_e");
}

// RICindicationType ::= ENUMERATED
const char* ri_cind_type_opts::to_string() const
{
  static const char* options[] = {"report", "insert"};
  return convert_enum_idx(options, 2, value, "ri_cind_type_e");
}

template struct asn1::protocol_ie_single_container_s<ricsubscription_with_cause_item_ies_o>;

// RICsubscriptionDetails ::= SEQUENCE
SRSASN_CODE ricsubscription_details_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ric_event_trigger_definition.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, ric_action_to_be_setup_list, 1, 16, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ricsubscription_details_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ric_event_trigger_definition.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(ric_action_to_be_setup_list, bref, 1, 16, true));

  return SRSASN_SUCCESS;
}
void ricsubscription_details_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ricEventTriggerDefinition", ric_event_trigger_definition.to_string());
  j.start_array("ricAction-ToBeSetup-List");
  for (const auto& e1 : ric_action_to_be_setup_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// TimeToWait ::= ENUMERATED
const char* time_to_wait_opts::to_string() const
{
  static const char* options[] = {"v1s", "v2s", "v5s", "v10s", "v20s", "v60s"};
  return convert_enum_idx(options, 6, value, "time_to_wait_e");
}
uint8_t time_to_wait_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 5, 10, 20, 60};
  return map_enum_number(options, 6, value, "time_to_wait_e");
}

// E2RemovalFailureIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2_removal_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 1, 2};
  return map_enum_number(options, 3, idx, "id");
}
bool e2_removal_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 1, 2};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2_removal_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
e2_removal_fail_ies_o::value_c e2_removal_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2_removal_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 1:
      return presence_e::mandatory;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2_removal_fail_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::cause:
      c.destroy<cause_c>();
      break;
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    default:
      break;
  }
}
void e2_removal_fail_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_fail_ies_o::value_c");
  }
}
e2_removal_fail_ies_o::value_c::value_c(const e2_removal_fail_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_fail_ies_o::value_c");
  }
}
e2_removal_fail_ies_o::value_c& e2_removal_fail_ies_o::value_c::operator=(const e2_removal_fail_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_fail_ies_o::value_c");
  }

  return *this;
}
uint16_t& e2_removal_fail_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& e2_removal_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
crit_diagnostics_s& e2_removal_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& e2_removal_fail_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& e2_removal_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const crit_diagnostics_s& e2_removal_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void e2_removal_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2_removal_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_removal_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2_removal_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 3, value, "e2_removal_fail_ies_o::value_c::types");
}
uint8_t e2_removal_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "e2_removal_fail_ies_o::value_c::types");
}

// E2RemovalRequestIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2_removal_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49};
  return map_enum_number(options, 1, idx, "id");
}
bool e2_removal_request_ies_o::is_id_valid(const uint32_t& id)
{
  return 49 == id;
}
crit_e e2_removal_request_ies_o::get_crit(const uint32_t& id)
{
  if (id == 49) {
    return crit_e::reject;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
e2_removal_request_ies_o::value_c e2_removal_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 49) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2_removal_request_ies_o::get_presence(const uint32_t& id)
{
  if (id == 49) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void e2_removal_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("INTEGER (0..255,...)", c);
  j.end_obj();
}
SRSASN_CODE e2_removal_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(pack_integer(bref, c, (uint16_t)0u, (uint16_t)255u, true, true));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_removal_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(unpack_integer(c, bref, (uint16_t)0u, (uint16_t)255u, true, true));
  return SRSASN_SUCCESS;
}

const char* e2_removal_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)"};
  return convert_enum_idx(options, 1, value, "e2_removal_request_ies_o::value_c::types");
}
uint8_t e2_removal_request_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "e2_removal_request_ies_o::value_c::types");
}

// E2RemovalResponseIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2_removal_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 2};
  return map_enum_number(options, 2, idx, "id");
}
bool e2_removal_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 2};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2_removal_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
e2_removal_resp_ies_o::value_c e2_removal_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2_removal_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2_removal_resp_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    default:
      break;
  }
}
void e2_removal_resp_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_resp_ies_o::value_c");
  }
}
e2_removal_resp_ies_o::value_c::value_c(const e2_removal_resp_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_resp_ies_o::value_c");
  }
}
e2_removal_resp_ies_o::value_c& e2_removal_resp_ies_o::value_c::operator=(const e2_removal_resp_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_resp_ies_o::value_c");
  }

  return *this;
}
uint16_t& e2_removal_resp_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
crit_diagnostics_s& e2_removal_resp_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& e2_removal_resp_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const crit_diagnostics_s& e2_removal_resp_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void e2_removal_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2_removal_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_removal_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_removal_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2_removal_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 2, value, "e2_removal_resp_ies_o::value_c::types");
}
uint8_t e2_removal_resp_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "e2_removal_resp_ies_o::value_c::types");
}

// E2connectionUpdate-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2conn_upd_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 44, 46, 45};
  return map_enum_number(options, 4, idx, "id");
}
bool e2conn_upd_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 44, 46, 45};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2conn_upd_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 44:
      return crit_e::reject;
    case 46:
      return crit_e::reject;
    case 45:
      return crit_e::reject;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
e2conn_upd_ies_o::value_c e2conn_upd_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 44:
      ret.set(value_c::types::e2conn_upd_add);
      break;
    case 46:
      ret.set(value_c::types::e2conn_upd_rem);
      break;
    case 45:
      ret.set(value_c::types::e2conn_upd_modify);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2conn_upd_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 44:
      return presence_e::optional;
    case 46:
      return presence_e::optional;
    case 45:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2conn_upd_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::e2conn_upd_add:
      c.destroy<e2conn_upd_list_l>();
      break;
    case types::e2conn_upd_rem:
      c.destroy<e2conn_upd_rem_list_l>();
      break;
    case types::e2conn_upd_modify:
      c.destroy<e2conn_upd_list_l>();
      break;
    default:
      break;
  }
}
void e2conn_upd_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::e2conn_upd_add:
      c.init<e2conn_upd_list_l>();
      break;
    case types::e2conn_upd_rem:
      c.init<e2conn_upd_rem_list_l>();
      break;
    case types::e2conn_upd_modify:
      c.init<e2conn_upd_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ies_o::value_c");
  }
}
e2conn_upd_ies_o::value_c::value_c(const e2conn_upd_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::e2conn_upd_add:
      c.init(other.c.get<e2conn_upd_list_l>());
      break;
    case types::e2conn_upd_rem:
      c.init(other.c.get<e2conn_upd_rem_list_l>());
      break;
    case types::e2conn_upd_modify:
      c.init(other.c.get<e2conn_upd_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ies_o::value_c");
  }
}
e2conn_upd_ies_o::value_c& e2conn_upd_ies_o::value_c::operator=(const e2conn_upd_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::e2conn_upd_add:
      c.set(other.c.get<e2conn_upd_list_l>());
      break;
    case types::e2conn_upd_rem:
      c.set(other.c.get<e2conn_upd_rem_list_l>());
      break;
    case types::e2conn_upd_modify:
      c.set(other.c.get<e2conn_upd_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ies_o::value_c");
  }

  return *this;
}
uint16_t& e2conn_upd_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
e2conn_upd_list_l& e2conn_upd_ies_o::value_c::e2conn_upd_add()
{
  assert_choice_type(types::e2conn_upd_add, type_, "Value");
  return c.get<e2conn_upd_list_l>();
}
e2conn_upd_rem_list_l& e2conn_upd_ies_o::value_c::e2conn_upd_rem()
{
  assert_choice_type(types::e2conn_upd_rem, type_, "Value");
  return c.get<e2conn_upd_rem_list_l>();
}
e2conn_upd_list_l& e2conn_upd_ies_o::value_c::e2conn_upd_modify()
{
  assert_choice_type(types::e2conn_upd_modify, type_, "Value");
  return c.get<e2conn_upd_list_l>();
}
const uint16_t& e2conn_upd_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const e2conn_upd_list_l& e2conn_upd_ies_o::value_c::e2conn_upd_add() const
{
  assert_choice_type(types::e2conn_upd_add, type_, "Value");
  return c.get<e2conn_upd_list_l>();
}
const e2conn_upd_rem_list_l& e2conn_upd_ies_o::value_c::e2conn_upd_rem() const
{
  assert_choice_type(types::e2conn_upd_rem, type_, "Value");
  return c.get<e2conn_upd_rem_list_l>();
}
const e2conn_upd_list_l& e2conn_upd_ies_o::value_c::e2conn_upd_modify() const
{
  assert_choice_type(types::e2conn_upd_modify, type_, "Value");
  return c.get<e2conn_upd_list_l>();
}
void e2conn_upd_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::e2conn_upd_add:
      j.start_array("E2connectionUpdate-List");
      for (const auto& e1 : c.get<e2conn_upd_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2conn_upd_rem:
      j.start_array("E2connectionUpdateRemove-List");
      for (const auto& e1 : c.get<e2conn_upd_rem_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2conn_upd_modify:
      j.start_array("E2connectionUpdate-List");
      for (const auto& e1 : c.get<e2conn_upd_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2conn_upd_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::e2conn_upd_add:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2conn_upd_list_l>(), 1, 32, true));
      break;
    case types::e2conn_upd_rem:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2conn_upd_rem_list_l>(), 1, 32, true));
      break;
    case types::e2conn_upd_modify:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2conn_upd_list_l>(), 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::e2conn_upd_add:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2conn_upd_list_l>(), bref, 1, 32, true));
      break;
    case types::e2conn_upd_rem:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2conn_upd_rem_list_l>(), bref, 1, 32, true));
      break;
    case types::e2conn_upd_modify:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2conn_upd_list_l>(), bref, 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2conn_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {
      "INTEGER (0..255,...)", "E2connectionUpdate-List", "E2connectionUpdateRemove-List", "E2connectionUpdate-List"};
  return convert_enum_idx(options, 4, value, "e2conn_upd_ies_o::value_c::types");
}

// E2connectionUpdateAck-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2conn_upd_ack_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 39, 40};
  return map_enum_number(options, 3, idx, "id");
}
bool e2conn_upd_ack_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 39, 40};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2conn_upd_ack_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 39:
      return crit_e::reject;
    case 40:
      return crit_e::reject;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
e2conn_upd_ack_ies_o::value_c e2conn_upd_ack_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 39:
      ret.set(value_c::types::e2conn_setup);
      break;
    case 40:
      ret.set(value_c::types::e2conn_setup_failed);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2conn_upd_ack_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 39:
      return presence_e::optional;
    case 40:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2conn_upd_ack_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::e2conn_setup:
      c.destroy<e2conn_upd_list_l>();
      break;
    case types::e2conn_setup_failed:
      c.destroy<e2conn_setup_failed_list_l>();
      break;
    default:
      break;
  }
}
void e2conn_upd_ack_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::e2conn_setup:
      c.init<e2conn_upd_list_l>();
      break;
    case types::e2conn_setup_failed:
      c.init<e2conn_setup_failed_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ack_ies_o::value_c");
  }
}
e2conn_upd_ack_ies_o::value_c::value_c(const e2conn_upd_ack_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::e2conn_setup:
      c.init(other.c.get<e2conn_upd_list_l>());
      break;
    case types::e2conn_setup_failed:
      c.init(other.c.get<e2conn_setup_failed_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ack_ies_o::value_c");
  }
}
e2conn_upd_ack_ies_o::value_c& e2conn_upd_ack_ies_o::value_c::operator=(const e2conn_upd_ack_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::e2conn_setup:
      c.set(other.c.get<e2conn_upd_list_l>());
      break;
    case types::e2conn_setup_failed:
      c.set(other.c.get<e2conn_setup_failed_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ack_ies_o::value_c");
  }

  return *this;
}
uint16_t& e2conn_upd_ack_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
e2conn_upd_list_l& e2conn_upd_ack_ies_o::value_c::e2conn_setup()
{
  assert_choice_type(types::e2conn_setup, type_, "Value");
  return c.get<e2conn_upd_list_l>();
}
e2conn_setup_failed_list_l& e2conn_upd_ack_ies_o::value_c::e2conn_setup_failed()
{
  assert_choice_type(types::e2conn_setup_failed, type_, "Value");
  return c.get<e2conn_setup_failed_list_l>();
}
const uint16_t& e2conn_upd_ack_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const e2conn_upd_list_l& e2conn_upd_ack_ies_o::value_c::e2conn_setup() const
{
  assert_choice_type(types::e2conn_setup, type_, "Value");
  return c.get<e2conn_upd_list_l>();
}
const e2conn_setup_failed_list_l& e2conn_upd_ack_ies_o::value_c::e2conn_setup_failed() const
{
  assert_choice_type(types::e2conn_setup_failed, type_, "Value");
  return c.get<e2conn_setup_failed_list_l>();
}
void e2conn_upd_ack_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::e2conn_setup:
      j.start_array("E2connectionUpdate-List");
      for (const auto& e1 : c.get<e2conn_upd_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2conn_setup_failed:
      j.start_array("E2connectionSetupFailed-List");
      for (const auto& e1 : c.get<e2conn_setup_failed_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ack_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2conn_upd_ack_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::e2conn_setup:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2conn_upd_list_l>(), 1, 32, true));
      break;
    case types::e2conn_setup_failed:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2conn_setup_failed_list_l>(), 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_ack_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::e2conn_setup:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2conn_upd_list_l>(), bref, 1, 32, true));
      break;
    case types::e2conn_setup_failed:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2conn_setup_failed_list_l>(), bref, 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2conn_upd_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)", "E2connectionUpdate-List", "E2connectionSetupFailed-List"};
  return convert_enum_idx(options, 3, value, "e2conn_upd_ack_ies_o::value_c::types");
}

// E2connectionUpdateFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2conn_upd_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 1, 31, 2};
  return map_enum_number(options, 4, idx, "id");
}
bool e2conn_upd_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 1, 31, 2};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2conn_upd_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 1:
      return crit_e::reject;
    case 31:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
e2conn_upd_fail_ies_o::value_c e2conn_upd_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 31:
      ret.set(value_c::types::time_to_wait);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2conn_upd_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 1:
      return presence_e::optional;
    case 31:
      return presence_e::optional;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2conn_upd_fail_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::cause:
      c.destroy<cause_c>();
      break;
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    default:
      break;
  }
}
void e2conn_upd_fail_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::time_to_wait:
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_fail_ies_o::value_c");
  }
}
e2conn_upd_fail_ies_o::value_c::value_c(const e2conn_upd_fail_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::time_to_wait:
      c.init(other.c.get<time_to_wait_e>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_fail_ies_o::value_c");
  }
}
e2conn_upd_fail_ies_o::value_c& e2conn_upd_fail_ies_o::value_c::operator=(const e2conn_upd_fail_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::time_to_wait:
      c.set(other.c.get<time_to_wait_e>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_fail_ies_o::value_c");
  }

  return *this;
}
uint16_t& e2conn_upd_fail_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& e2conn_upd_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
time_to_wait_e& e2conn_upd_fail_ies_o::value_c::time_to_wait()
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
crit_diagnostics_s& e2conn_upd_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& e2conn_upd_fail_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& e2conn_upd_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const time_to_wait_e& e2conn_upd_fail_ies_o::value_c::time_to_wait() const
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
const crit_diagnostics_s& e2conn_upd_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void e2conn_upd_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::time_to_wait:
      j.write_str("TimeToWait", c.get<time_to_wait_e>().to_string());
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2conn_upd_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::time_to_wait:
      HANDLE_CODE(c.get<time_to_wait_e>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::time_to_wait:
      HANDLE_CODE(c.get<time_to_wait_e>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2conn_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2conn_upd_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 4, value, "e2conn_upd_fail_ies_o::value_c::types");
}
uint8_t e2conn_upd_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "e2conn_upd_fail_ies_o::value_c::types");
}

// E2nodeConfigurationUpdate-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_cfg_upd_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 3, 50, 33, 54, 58};
  return map_enum_number(options, 6, idx, "id");
}
bool e2node_cfg_upd_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 3, 50, 33, 54, 58};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2node_cfg_upd_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 50:
      return crit_e::reject;
    case 33:
      return crit_e::reject;
    case 54:
      return crit_e::reject;
    case 58:
      return crit_e::reject;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
e2node_cfg_upd_ies_o::value_c e2node_cfg_upd_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 3:
      ret.set(value_c::types::global_e2node_id);
      break;
    case 50:
      ret.set(value_c::types::e2node_component_cfg_addition);
      break;
    case 33:
      ret.set(value_c::types::e2node_component_cfg_upd);
      break;
    case 54:
      ret.set(value_c::types::e2node_component_cfg_removal);
      break;
    case 58:
      ret.set(value_c::types::e2node_tn_lassoc_removal);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2node_cfg_upd_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 3:
      return presence_e::optional;
    case 50:
      return presence_e::optional;
    case 33:
      return presence_e::optional;
    case 54:
      return presence_e::optional;
    case 58:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2node_cfg_upd_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::global_e2node_id:
      c.destroy<global_e2node_id_c>();
      break;
    case types::e2node_component_cfg_addition:
      c.destroy<e2node_component_cfg_addition_list_l>();
      break;
    case types::e2node_component_cfg_upd:
      c.destroy<e2node_component_cfg_upd_list_l>();
      break;
    case types::e2node_component_cfg_removal:
      c.destroy<e2node_component_cfg_removal_list_l>();
      break;
    case types::e2node_tn_lassoc_removal:
      c.destroy<e2node_tn_lassoc_removal_list_l>();
      break;
    default:
      break;
  }
}
void e2node_cfg_upd_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::global_e2node_id:
      c.init<global_e2node_id_c>();
      break;
    case types::e2node_component_cfg_addition:
      c.init<e2node_component_cfg_addition_list_l>();
      break;
    case types::e2node_component_cfg_upd:
      c.init<e2node_component_cfg_upd_list_l>();
      break;
    case types::e2node_component_cfg_removal:
      c.init<e2node_component_cfg_removal_list_l>();
      break;
    case types::e2node_tn_lassoc_removal:
      c.init<e2node_tn_lassoc_removal_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ies_o::value_c");
  }
}
e2node_cfg_upd_ies_o::value_c::value_c(const e2node_cfg_upd_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::global_e2node_id:
      c.init(other.c.get<global_e2node_id_c>());
      break;
    case types::e2node_component_cfg_addition:
      c.init(other.c.get<e2node_component_cfg_addition_list_l>());
      break;
    case types::e2node_component_cfg_upd:
      c.init(other.c.get<e2node_component_cfg_upd_list_l>());
      break;
    case types::e2node_component_cfg_removal:
      c.init(other.c.get<e2node_component_cfg_removal_list_l>());
      break;
    case types::e2node_tn_lassoc_removal:
      c.init(other.c.get<e2node_tn_lassoc_removal_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ies_o::value_c");
  }
}
e2node_cfg_upd_ies_o::value_c& e2node_cfg_upd_ies_o::value_c::operator=(const e2node_cfg_upd_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::global_e2node_id:
      c.set(other.c.get<global_e2node_id_c>());
      break;
    case types::e2node_component_cfg_addition:
      c.set(other.c.get<e2node_component_cfg_addition_list_l>());
      break;
    case types::e2node_component_cfg_upd:
      c.set(other.c.get<e2node_component_cfg_upd_list_l>());
      break;
    case types::e2node_component_cfg_removal:
      c.set(other.c.get<e2node_component_cfg_removal_list_l>());
      break;
    case types::e2node_tn_lassoc_removal:
      c.set(other.c.get<e2node_tn_lassoc_removal_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ies_o::value_c");
  }

  return *this;
}
uint16_t& e2node_cfg_upd_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
global_e2node_id_c& e2node_cfg_upd_ies_o::value_c::global_e2node_id()
{
  assert_choice_type(types::global_e2node_id, type_, "Value");
  return c.get<global_e2node_id_c>();
}
e2node_component_cfg_addition_list_l& e2node_cfg_upd_ies_o::value_c::e2node_component_cfg_addition()
{
  assert_choice_type(types::e2node_component_cfg_addition, type_, "Value");
  return c.get<e2node_component_cfg_addition_list_l>();
}
e2node_component_cfg_upd_list_l& e2node_cfg_upd_ies_o::value_c::e2node_component_cfg_upd()
{
  assert_choice_type(types::e2node_component_cfg_upd, type_, "Value");
  return c.get<e2node_component_cfg_upd_list_l>();
}
e2node_component_cfg_removal_list_l& e2node_cfg_upd_ies_o::value_c::e2node_component_cfg_removal()
{
  assert_choice_type(types::e2node_component_cfg_removal, type_, "Value");
  return c.get<e2node_component_cfg_removal_list_l>();
}
e2node_tn_lassoc_removal_list_l& e2node_cfg_upd_ies_o::value_c::e2node_tn_lassoc_removal()
{
  assert_choice_type(types::e2node_tn_lassoc_removal, type_, "Value");
  return c.get<e2node_tn_lassoc_removal_list_l>();
}
const uint16_t& e2node_cfg_upd_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const global_e2node_id_c& e2node_cfg_upd_ies_o::value_c::global_e2node_id() const
{
  assert_choice_type(types::global_e2node_id, type_, "Value");
  return c.get<global_e2node_id_c>();
}
const e2node_component_cfg_addition_list_l& e2node_cfg_upd_ies_o::value_c::e2node_component_cfg_addition() const
{
  assert_choice_type(types::e2node_component_cfg_addition, type_, "Value");
  return c.get<e2node_component_cfg_addition_list_l>();
}
const e2node_component_cfg_upd_list_l& e2node_cfg_upd_ies_o::value_c::e2node_component_cfg_upd() const
{
  assert_choice_type(types::e2node_component_cfg_upd, type_, "Value");
  return c.get<e2node_component_cfg_upd_list_l>();
}
const e2node_component_cfg_removal_list_l& e2node_cfg_upd_ies_o::value_c::e2node_component_cfg_removal() const
{
  assert_choice_type(types::e2node_component_cfg_removal, type_, "Value");
  return c.get<e2node_component_cfg_removal_list_l>();
}
const e2node_tn_lassoc_removal_list_l& e2node_cfg_upd_ies_o::value_c::e2node_tn_lassoc_removal() const
{
  assert_choice_type(types::e2node_tn_lassoc_removal, type_, "Value");
  return c.get<e2node_tn_lassoc_removal_list_l>();
}
void e2node_cfg_upd_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::global_e2node_id:
      j.write_fieldname("GlobalE2node-ID");
      c.get<global_e2node_id_c>().to_json(j);
      break;
    case types::e2node_component_cfg_addition:
      j.start_array("E2nodeComponentConfigAddition-List");
      for (const auto& e1 : c.get<e2node_component_cfg_addition_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2node_component_cfg_upd:
      j.start_array("E2nodeComponentConfigUpdate-List");
      for (const auto& e1 : c.get<e2node_component_cfg_upd_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2node_component_cfg_removal:
      j.start_array("E2nodeComponentConfigRemoval-List");
      for (const auto& e1 : c.get<e2node_component_cfg_removal_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2node_tn_lassoc_removal:
      j.start_array("E2nodeTNLassociationRemoval-List");
      for (const auto& e1 : c.get<e2node_tn_lassoc_removal_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2node_cfg_upd_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::global_e2node_id:
      HANDLE_CODE(c.get<global_e2node_id_c>().pack(bref));
      break;
    case types::e2node_component_cfg_addition:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_component_cfg_addition_list_l>(), 1, 1024, true));
      break;
    case types::e2node_component_cfg_upd:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_component_cfg_upd_list_l>(), 1, 1024, true));
      break;
    case types::e2node_component_cfg_removal:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_component_cfg_removal_list_l>(), 1, 1024, true));
      break;
    case types::e2node_tn_lassoc_removal:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_tn_lassoc_removal_list_l>(), 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_cfg_upd_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::global_e2node_id:
      HANDLE_CODE(c.get<global_e2node_id_c>().unpack(bref));
      break;
    case types::e2node_component_cfg_addition:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_component_cfg_addition_list_l>(), bref, 1, 1024, true));
      break;
    case types::e2node_component_cfg_upd:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_component_cfg_upd_list_l>(), bref, 1, 1024, true));
      break;
    case types::e2node_component_cfg_removal:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_component_cfg_removal_list_l>(), bref, 1, 1024, true));
      break;
    case types::e2node_tn_lassoc_removal:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_tn_lassoc_removal_list_l>(), bref, 1, 32, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2node_cfg_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)",
                                  "GlobalE2node-ID",
                                  "E2nodeComponentConfigAddition-List",
                                  "E2nodeComponentConfigUpdate-List",
                                  "E2nodeComponentConfigRemoval-List",
                                  "E2nodeTNLassociationRemoval-List"};
  return convert_enum_idx(options, 6, value, "e2node_cfg_upd_ies_o::value_c::types");
}

// E2nodeConfigurationUpdateAcknowledge-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_cfg_upd_ack_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 52, 35, 56};
  return map_enum_number(options, 4, idx, "id");
}
bool e2node_cfg_upd_ack_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 52, 35, 56};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2node_cfg_upd_ack_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 52:
      return crit_e::reject;
    case 35:
      return crit_e::reject;
    case 56:
      return crit_e::reject;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
e2node_cfg_upd_ack_ies_o::value_c e2node_cfg_upd_ack_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 52:
      ret.set(value_c::types::e2node_component_cfg_addition_ack);
      break;
    case 35:
      ret.set(value_c::types::e2node_component_cfg_upd_ack);
      break;
    case 56:
      ret.set(value_c::types::e2node_component_cfg_removal_ack);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2node_cfg_upd_ack_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 52:
      return presence_e::optional;
    case 35:
      return presence_e::optional;
    case 56:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2node_cfg_upd_ack_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::e2node_component_cfg_addition_ack:
      c.destroy<e2node_component_cfg_addition_ack_list_l>();
      break;
    case types::e2node_component_cfg_upd_ack:
      c.destroy<e2node_component_cfg_upd_ack_list_l>();
      break;
    case types::e2node_component_cfg_removal_ack:
      c.destroy<e2node_component_cfg_removal_ack_list_l>();
      break;
    default:
      break;
  }
}
void e2node_cfg_upd_ack_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::e2node_component_cfg_addition_ack:
      c.init<e2node_component_cfg_addition_ack_list_l>();
      break;
    case types::e2node_component_cfg_upd_ack:
      c.init<e2node_component_cfg_upd_ack_list_l>();
      break;
    case types::e2node_component_cfg_removal_ack:
      c.init<e2node_component_cfg_removal_ack_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ack_ies_o::value_c");
  }
}
e2node_cfg_upd_ack_ies_o::value_c::value_c(const e2node_cfg_upd_ack_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::e2node_component_cfg_addition_ack:
      c.init(other.c.get<e2node_component_cfg_addition_ack_list_l>());
      break;
    case types::e2node_component_cfg_upd_ack:
      c.init(other.c.get<e2node_component_cfg_upd_ack_list_l>());
      break;
    case types::e2node_component_cfg_removal_ack:
      c.init(other.c.get<e2node_component_cfg_removal_ack_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ack_ies_o::value_c");
  }
}
e2node_cfg_upd_ack_ies_o::value_c&
e2node_cfg_upd_ack_ies_o::value_c::operator=(const e2node_cfg_upd_ack_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::e2node_component_cfg_addition_ack:
      c.set(other.c.get<e2node_component_cfg_addition_ack_list_l>());
      break;
    case types::e2node_component_cfg_upd_ack:
      c.set(other.c.get<e2node_component_cfg_upd_ack_list_l>());
      break;
    case types::e2node_component_cfg_removal_ack:
      c.set(other.c.get<e2node_component_cfg_removal_ack_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ack_ies_o::value_c");
  }

  return *this;
}
uint16_t& e2node_cfg_upd_ack_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
e2node_component_cfg_addition_ack_list_l& e2node_cfg_upd_ack_ies_o::value_c::e2node_component_cfg_addition_ack()
{
  assert_choice_type(types::e2node_component_cfg_addition_ack, type_, "Value");
  return c.get<e2node_component_cfg_addition_ack_list_l>();
}
e2node_component_cfg_upd_ack_list_l& e2node_cfg_upd_ack_ies_o::value_c::e2node_component_cfg_upd_ack()
{
  assert_choice_type(types::e2node_component_cfg_upd_ack, type_, "Value");
  return c.get<e2node_component_cfg_upd_ack_list_l>();
}
e2node_component_cfg_removal_ack_list_l& e2node_cfg_upd_ack_ies_o::value_c::e2node_component_cfg_removal_ack()
{
  assert_choice_type(types::e2node_component_cfg_removal_ack, type_, "Value");
  return c.get<e2node_component_cfg_removal_ack_list_l>();
}
const uint16_t& e2node_cfg_upd_ack_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const e2node_component_cfg_addition_ack_list_l&
e2node_cfg_upd_ack_ies_o::value_c::e2node_component_cfg_addition_ack() const
{
  assert_choice_type(types::e2node_component_cfg_addition_ack, type_, "Value");
  return c.get<e2node_component_cfg_addition_ack_list_l>();
}
const e2node_component_cfg_upd_ack_list_l& e2node_cfg_upd_ack_ies_o::value_c::e2node_component_cfg_upd_ack() const
{
  assert_choice_type(types::e2node_component_cfg_upd_ack, type_, "Value");
  return c.get<e2node_component_cfg_upd_ack_list_l>();
}
const e2node_component_cfg_removal_ack_list_l&
e2node_cfg_upd_ack_ies_o::value_c::e2node_component_cfg_removal_ack() const
{
  assert_choice_type(types::e2node_component_cfg_removal_ack, type_, "Value");
  return c.get<e2node_component_cfg_removal_ack_list_l>();
}
void e2node_cfg_upd_ack_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::e2node_component_cfg_addition_ack:
      j.start_array("E2nodeComponentConfigAdditionAck-List");
      for (const auto& e1 : c.get<e2node_component_cfg_addition_ack_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2node_component_cfg_upd_ack:
      j.start_array("E2nodeComponentConfigUpdateAck-List");
      for (const auto& e1 : c.get<e2node_component_cfg_upd_ack_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2node_component_cfg_removal_ack:
      j.start_array("E2nodeComponentConfigRemovalAck-List");
      for (const auto& e1 : c.get<e2node_component_cfg_removal_ack_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ack_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2node_cfg_upd_ack_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::e2node_component_cfg_addition_ack:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_component_cfg_addition_ack_list_l>(), 1, 1024, true));
      break;
    case types::e2node_component_cfg_upd_ack:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_component_cfg_upd_ack_list_l>(), 1, 1024, true));
      break;
    case types::e2node_component_cfg_removal_ack:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_component_cfg_removal_ack_list_l>(), 1, 1024, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_cfg_upd_ack_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::e2node_component_cfg_addition_ack:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_component_cfg_addition_ack_list_l>(), bref, 1, 1024, true));
      break;
    case types::e2node_component_cfg_upd_ack:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_component_cfg_upd_ack_list_l>(), bref, 1, 1024, true));
      break;
    case types::e2node_component_cfg_removal_ack:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_component_cfg_removal_ack_list_l>(), bref, 1, 1024, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2node_cfg_upd_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)",
                                  "E2nodeComponentConfigAdditionAck-List",
                                  "E2nodeComponentConfigUpdateAck-List",
                                  "E2nodeComponentConfigRemovalAck-List"};
  return convert_enum_idx(options, 4, value, "e2node_cfg_upd_ack_ies_o::value_c::types");
}

// E2nodeConfigurationUpdateFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2node_cfg_upd_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 1, 31, 2};
  return map_enum_number(options, 4, idx, "id");
}
bool e2node_cfg_upd_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 1, 31, 2};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2node_cfg_upd_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 31:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
e2node_cfg_upd_fail_ies_o::value_c e2node_cfg_upd_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 31:
      ret.set(value_c::types::time_to_wait);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2node_cfg_upd_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 1:
      return presence_e::mandatory;
    case 31:
      return presence_e::optional;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2node_cfg_upd_fail_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::cause:
      c.destroy<cause_c>();
      break;
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    default:
      break;
  }
}
void e2node_cfg_upd_fail_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::time_to_wait:
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_fail_ies_o::value_c");
  }
}
e2node_cfg_upd_fail_ies_o::value_c::value_c(const e2node_cfg_upd_fail_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::time_to_wait:
      c.init(other.c.get<time_to_wait_e>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_fail_ies_o::value_c");
  }
}
e2node_cfg_upd_fail_ies_o::value_c&
e2node_cfg_upd_fail_ies_o::value_c::operator=(const e2node_cfg_upd_fail_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::time_to_wait:
      c.set(other.c.get<time_to_wait_e>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_fail_ies_o::value_c");
  }

  return *this;
}
uint16_t& e2node_cfg_upd_fail_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& e2node_cfg_upd_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
time_to_wait_e& e2node_cfg_upd_fail_ies_o::value_c::time_to_wait()
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
crit_diagnostics_s& e2node_cfg_upd_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& e2node_cfg_upd_fail_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& e2node_cfg_upd_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const time_to_wait_e& e2node_cfg_upd_fail_ies_o::value_c::time_to_wait() const
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
const crit_diagnostics_s& e2node_cfg_upd_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void e2node_cfg_upd_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::time_to_wait:
      j.write_str("TimeToWait", c.get<time_to_wait_e>().to_string());
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2node_cfg_upd_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::time_to_wait:
      HANDLE_CODE(c.get<time_to_wait_e>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_cfg_upd_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::time_to_wait:
      HANDLE_CODE(c.get<time_to_wait_e>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2node_cfg_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2node_cfg_upd_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 4, value, "e2node_cfg_upd_fail_ies_o::value_c::types");
}
uint8_t e2node_cfg_upd_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "e2node_cfg_upd_fail_ies_o::value_c::types");
}

// E2setupFailureIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2setup_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 1, 31, 2, 48};
  return map_enum_number(options, 5, idx, "id");
}
bool e2setup_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 1, 31, 2, 48};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2setup_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 31:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    case 48:
      return crit_e::ignore;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
e2setup_fail_ies_o::value_c e2setup_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 31:
      ret.set(value_c::types::time_to_wait);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    case 48:
      ret.set(value_c::types::tn_linfo);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2setup_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 1:
      return presence_e::mandatory;
    case 31:
      return presence_e::optional;
    case 2:
      return presence_e::optional;
    case 48:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2setup_fail_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::cause:
      c.destroy<cause_c>();
      break;
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    case types::tn_linfo:
      c.destroy<tn_linfo_s>();
      break;
    default:
      break;
  }
}
void e2setup_fail_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::time_to_wait:
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::tn_linfo:
      c.init<tn_linfo_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_fail_ies_o::value_c");
  }
}
e2setup_fail_ies_o::value_c::value_c(const e2setup_fail_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::time_to_wait:
      c.init(other.c.get<time_to_wait_e>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::tn_linfo:
      c.init(other.c.get<tn_linfo_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_fail_ies_o::value_c");
  }
}
e2setup_fail_ies_o::value_c& e2setup_fail_ies_o::value_c::operator=(const e2setup_fail_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::time_to_wait:
      c.set(other.c.get<time_to_wait_e>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::tn_linfo:
      c.set(other.c.get<tn_linfo_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_fail_ies_o::value_c");
  }

  return *this;
}
uint16_t& e2setup_fail_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& e2setup_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
time_to_wait_e& e2setup_fail_ies_o::value_c::time_to_wait()
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
crit_diagnostics_s& e2setup_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
tn_linfo_s& e2setup_fail_ies_o::value_c::tn_linfo()
{
  assert_choice_type(types::tn_linfo, type_, "Value");
  return c.get<tn_linfo_s>();
}
const uint16_t& e2setup_fail_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& e2setup_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const time_to_wait_e& e2setup_fail_ies_o::value_c::time_to_wait() const
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
const crit_diagnostics_s& e2setup_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const tn_linfo_s& e2setup_fail_ies_o::value_c::tn_linfo() const
{
  assert_choice_type(types::tn_linfo, type_, "Value");
  return c.get<tn_linfo_s>();
}
void e2setup_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::time_to_wait:
      j.write_str("TimeToWait", c.get<time_to_wait_e>().to_string());
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    case types::tn_linfo:
      j.write_fieldname("TNLinformation");
      c.get<tn_linfo_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2setup_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::time_to_wait:
      HANDLE_CODE(c.get<time_to_wait_e>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    case types::tn_linfo:
      HANDLE_CODE(c.get<tn_linfo_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2setup_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::time_to_wait:
      HANDLE_CODE(c.get<time_to_wait_e>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    case types::tn_linfo:
      HANDLE_CODE(c.get<tn_linfo_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2setup_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {
      "INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics", "TNLinformation"};
  return convert_enum_idx(options, 5, value, "e2setup_fail_ies_o::value_c::types");
}
uint8_t e2setup_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "e2setup_fail_ies_o::value_c::types");
}

// E2setupRequestIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2setup_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 3, 10, 50};
  return map_enum_number(options, 4, idx, "id");
}
bool e2setup_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 3, 10, 50};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2setup_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 10:
      return crit_e::reject;
    case 50:
      return crit_e::reject;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
e2setup_request_ies_o::value_c e2setup_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 3:
      ret.set(value_c::types::global_e2node_id);
      break;
    case 10:
      ret.set(value_c::types::ra_nfunctions_added);
      break;
    case 50:
      ret.set(value_c::types::e2node_component_cfg_addition);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2setup_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 3:
      return presence_e::mandatory;
    case 10:
      return presence_e::mandatory;
    case 50:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2setup_request_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::global_e2node_id:
      c.destroy<global_e2node_id_c>();
      break;
    case types::ra_nfunctions_added:
      c.destroy<ra_nfunctions_list_l>();
      break;
    case types::e2node_component_cfg_addition:
      c.destroy<e2node_component_cfg_addition_list_l>();
      break;
    default:
      break;
  }
}
void e2setup_request_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::global_e2node_id:
      c.init<global_e2node_id_c>();
      break;
    case types::ra_nfunctions_added:
      c.init<ra_nfunctions_list_l>();
      break;
    case types::e2node_component_cfg_addition:
      c.init<e2node_component_cfg_addition_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_request_ies_o::value_c");
  }
}
e2setup_request_ies_o::value_c::value_c(const e2setup_request_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::global_e2node_id:
      c.init(other.c.get<global_e2node_id_c>());
      break;
    case types::ra_nfunctions_added:
      c.init(other.c.get<ra_nfunctions_list_l>());
      break;
    case types::e2node_component_cfg_addition:
      c.init(other.c.get<e2node_component_cfg_addition_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_request_ies_o::value_c");
  }
}
e2setup_request_ies_o::value_c& e2setup_request_ies_o::value_c::operator=(const e2setup_request_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::global_e2node_id:
      c.set(other.c.get<global_e2node_id_c>());
      break;
    case types::ra_nfunctions_added:
      c.set(other.c.get<ra_nfunctions_list_l>());
      break;
    case types::e2node_component_cfg_addition:
      c.set(other.c.get<e2node_component_cfg_addition_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_request_ies_o::value_c");
  }

  return *this;
}
uint16_t& e2setup_request_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
global_e2node_id_c& e2setup_request_ies_o::value_c::global_e2node_id()
{
  assert_choice_type(types::global_e2node_id, type_, "Value");
  return c.get<global_e2node_id_c>();
}
ra_nfunctions_list_l& e2setup_request_ies_o::value_c::ra_nfunctions_added()
{
  assert_choice_type(types::ra_nfunctions_added, type_, "Value");
  return c.get<ra_nfunctions_list_l>();
}
e2node_component_cfg_addition_list_l& e2setup_request_ies_o::value_c::e2node_component_cfg_addition()
{
  assert_choice_type(types::e2node_component_cfg_addition, type_, "Value");
  return c.get<e2node_component_cfg_addition_list_l>();
}
const uint16_t& e2setup_request_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const global_e2node_id_c& e2setup_request_ies_o::value_c::global_e2node_id() const
{
  assert_choice_type(types::global_e2node_id, type_, "Value");
  return c.get<global_e2node_id_c>();
}
const ra_nfunctions_list_l& e2setup_request_ies_o::value_c::ra_nfunctions_added() const
{
  assert_choice_type(types::ra_nfunctions_added, type_, "Value");
  return c.get<ra_nfunctions_list_l>();
}
const e2node_component_cfg_addition_list_l& e2setup_request_ies_o::value_c::e2node_component_cfg_addition() const
{
  assert_choice_type(types::e2node_component_cfg_addition, type_, "Value");
  return c.get<e2node_component_cfg_addition_list_l>();
}
void e2setup_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::global_e2node_id:
      j.write_fieldname("GlobalE2node-ID");
      c.get<global_e2node_id_c>().to_json(j);
      break;
    case types::ra_nfunctions_added:
      j.start_array("RANfunctions-List");
      for (const auto& e1 : c.get<ra_nfunctions_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2node_component_cfg_addition:
      j.start_array("E2nodeComponentConfigAddition-List");
      for (const auto& e1 : c.get<e2node_component_cfg_addition_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2setup_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::global_e2node_id:
      HANDLE_CODE(c.get<global_e2node_id_c>().pack(bref));
      break;
    case types::ra_nfunctions_added:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ra_nfunctions_list_l>(), 1, 256, true));
      break;
    case types::e2node_component_cfg_addition:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_component_cfg_addition_list_l>(), 1, 1024, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2setup_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::global_e2node_id:
      HANDLE_CODE(c.get<global_e2node_id_c>().unpack(bref));
      break;
    case types::ra_nfunctions_added:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ra_nfunctions_list_l>(), bref, 1, 256, true));
      break;
    case types::e2node_component_cfg_addition:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_component_cfg_addition_list_l>(), bref, 1, 1024, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2setup_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {
      "INTEGER (0..255,...)", "GlobalE2node-ID", "RANfunctions-List", "E2nodeComponentConfigAddition-List"};
  return convert_enum_idx(options, 4, value, "e2setup_request_ies_o::value_c::types");
}

// E2setupResponseIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t e2setup_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 4, 9, 13, 52};
  return map_enum_number(options, 5, idx, "id");
}
bool e2setup_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 4, 9, 13, 52};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e e2setup_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 4:
      return crit_e::reject;
    case 9:
      return crit_e::reject;
    case 13:
      return crit_e::reject;
    case 52:
      return crit_e::reject;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
e2setup_resp_ies_o::value_c e2setup_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 4:
      ret.set(value_c::types::global_ric_id);
      break;
    case 9:
      ret.set(value_c::types::ra_nfunctions_accepted);
      break;
    case 13:
      ret.set(value_c::types::ra_nfunctions_rejected);
      break;
    case 52:
      ret.set(value_c::types::e2node_component_cfg_addition_ack);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e e2setup_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 4:
      return presence_e::mandatory;
    case 9:
      return presence_e::optional;
    case 13:
      return presence_e::optional;
    case 52:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void e2setup_resp_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::global_ric_id:
      c.destroy<global_ric_id_s>();
      break;
    case types::ra_nfunctions_accepted:
      c.destroy<ra_nfunctions_id_list_l>();
      break;
    case types::ra_nfunctions_rejected:
      c.destroy<ra_nfunctions_idcause_list_l>();
      break;
    case types::e2node_component_cfg_addition_ack:
      c.destroy<e2node_component_cfg_addition_ack_list_l>();
      break;
    default:
      break;
  }
}
void e2setup_resp_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::global_ric_id:
      c.init<global_ric_id_s>();
      break;
    case types::ra_nfunctions_accepted:
      c.init<ra_nfunctions_id_list_l>();
      break;
    case types::ra_nfunctions_rejected:
      c.init<ra_nfunctions_idcause_list_l>();
      break;
    case types::e2node_component_cfg_addition_ack:
      c.init<e2node_component_cfg_addition_ack_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_resp_ies_o::value_c");
  }
}
e2setup_resp_ies_o::value_c::value_c(const e2setup_resp_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::global_ric_id:
      c.init(other.c.get<global_ric_id_s>());
      break;
    case types::ra_nfunctions_accepted:
      c.init(other.c.get<ra_nfunctions_id_list_l>());
      break;
    case types::ra_nfunctions_rejected:
      c.init(other.c.get<ra_nfunctions_idcause_list_l>());
      break;
    case types::e2node_component_cfg_addition_ack:
      c.init(other.c.get<e2node_component_cfg_addition_ack_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_resp_ies_o::value_c");
  }
}
e2setup_resp_ies_o::value_c& e2setup_resp_ies_o::value_c::operator=(const e2setup_resp_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::global_ric_id:
      c.set(other.c.get<global_ric_id_s>());
      break;
    case types::ra_nfunctions_accepted:
      c.set(other.c.get<ra_nfunctions_id_list_l>());
      break;
    case types::ra_nfunctions_rejected:
      c.set(other.c.get<ra_nfunctions_idcause_list_l>());
      break;
    case types::e2node_component_cfg_addition_ack:
      c.set(other.c.get<e2node_component_cfg_addition_ack_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_resp_ies_o::value_c");
  }

  return *this;
}
uint16_t& e2setup_resp_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
global_ric_id_s& e2setup_resp_ies_o::value_c::global_ric_id()
{
  assert_choice_type(types::global_ric_id, type_, "Value");
  return c.get<global_ric_id_s>();
}
ra_nfunctions_id_list_l& e2setup_resp_ies_o::value_c::ra_nfunctions_accepted()
{
  assert_choice_type(types::ra_nfunctions_accepted, type_, "Value");
  return c.get<ra_nfunctions_id_list_l>();
}
ra_nfunctions_idcause_list_l& e2setup_resp_ies_o::value_c::ra_nfunctions_rejected()
{
  assert_choice_type(types::ra_nfunctions_rejected, type_, "Value");
  return c.get<ra_nfunctions_idcause_list_l>();
}
e2node_component_cfg_addition_ack_list_l& e2setup_resp_ies_o::value_c::e2node_component_cfg_addition_ack()
{
  assert_choice_type(types::e2node_component_cfg_addition_ack, type_, "Value");
  return c.get<e2node_component_cfg_addition_ack_list_l>();
}
const uint16_t& e2setup_resp_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const global_ric_id_s& e2setup_resp_ies_o::value_c::global_ric_id() const
{
  assert_choice_type(types::global_ric_id, type_, "Value");
  return c.get<global_ric_id_s>();
}
const ra_nfunctions_id_list_l& e2setup_resp_ies_o::value_c::ra_nfunctions_accepted() const
{
  assert_choice_type(types::ra_nfunctions_accepted, type_, "Value");
  return c.get<ra_nfunctions_id_list_l>();
}
const ra_nfunctions_idcause_list_l& e2setup_resp_ies_o::value_c::ra_nfunctions_rejected() const
{
  assert_choice_type(types::ra_nfunctions_rejected, type_, "Value");
  return c.get<ra_nfunctions_idcause_list_l>();
}
const e2node_component_cfg_addition_ack_list_l& e2setup_resp_ies_o::value_c::e2node_component_cfg_addition_ack() const
{
  assert_choice_type(types::e2node_component_cfg_addition_ack, type_, "Value");
  return c.get<e2node_component_cfg_addition_ack_list_l>();
}
void e2setup_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::global_ric_id:
      j.write_fieldname("GlobalRIC-ID");
      c.get<global_ric_id_s>().to_json(j);
      break;
    case types::ra_nfunctions_accepted:
      j.start_array("RANfunctionsID-List");
      for (const auto& e1 : c.get<ra_nfunctions_id_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ra_nfunctions_rejected:
      j.start_array("RANfunctionsIDcause-List");
      for (const auto& e1 : c.get<ra_nfunctions_idcause_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::e2node_component_cfg_addition_ack:
      j.start_array("E2nodeComponentConfigAdditionAck-List");
      for (const auto& e1 : c.get<e2node_component_cfg_addition_ack_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE e2setup_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::global_ric_id:
      HANDLE_CODE(c.get<global_ric_id_s>().pack(bref));
      break;
    case types::ra_nfunctions_accepted:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ra_nfunctions_id_list_l>(), 1, 256, true));
      break;
    case types::ra_nfunctions_rejected:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ra_nfunctions_idcause_list_l>(), 1, 256, true));
      break;
    case types::e2node_component_cfg_addition_ack:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<e2node_component_cfg_addition_ack_list_l>(), 1, 1024, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2setup_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::global_ric_id:
      HANDLE_CODE(c.get<global_ric_id_s>().unpack(bref));
      break;
    case types::ra_nfunctions_accepted:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ra_nfunctions_id_list_l>(), bref, 1, 256, true));
      break;
    case types::ra_nfunctions_rejected:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ra_nfunctions_idcause_list_l>(), bref, 1, 256, true));
      break;
    case types::e2node_component_cfg_addition_ack:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<e2node_component_cfg_addition_ack_list_l>(), bref, 1, 1024, true));
      break;
    default:
      log_invalid_choice_id(type_, "e2setup_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2setup_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)",
                                  "GlobalRIC-ID",
                                  "RANfunctionsID-List",
                                  "RANfunctionsIDcause-List",
                                  "E2nodeComponentConfigAdditionAck-List"};
  return convert_enum_idx(options, 5, value, "e2setup_resp_ies_o::value_c::types");
}
uint8_t e2setup_resp_ies_o::value_c::types_opts::to_number() const
{
  switch (value) {
    case transaction_id:
      return 0;
    case e2node_component_cfg_addition_ack:
      return 2;
    default:
      invalid_enum_number(value, "e2setup_resp_ies_o::value_c::types");
  }
  return 0;
}

// ErrorIndication-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t error_ind_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 29, 5, 1, 2};
  return map_enum_number(options, 5, idx, "id");
}
bool error_ind_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 29, 5, 1, 2};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e error_ind_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
error_ind_ies_o::value_c error_ind_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 29:
      ret.set(value_c::types::ri_crequest_id);
      break;
    case 5:
      ret.set(value_c::types::ra_nfunction_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e error_ind_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::optional;
    case 29:
      return presence_e::optional;
    case 5:
      return presence_e::optional;
    case 1:
      return presence_e::optional;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void error_ind_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::ri_crequest_id:
      c.destroy<ri_crequest_id_s>();
      break;
    case types::cause:
      c.destroy<cause_c>();
      break;
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    default:
      break;
  }
}
void error_ind_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::ri_crequest_id:
      c.init<ri_crequest_id_s>();
      break;
    case types::ra_nfunction_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "error_ind_ies_o::value_c");
  }
}
error_ind_ies_o::value_c::value_c(const error_ind_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ri_crequest_id:
      c.init(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "error_ind_ies_o::value_c");
  }
}
error_ind_ies_o::value_c& error_ind_ies_o::value_c::operator=(const error_ind_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ri_crequest_id:
      c.set(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "error_ind_ies_o::value_c");
  }

  return *this;
}
uint16_t& error_ind_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
ri_crequest_id_s& error_ind_ies_o::value_c::ri_crequest_id()
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
uint16_t& error_ind_ies_o::value_c::ra_nfunction_id()
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& error_ind_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
crit_diagnostics_s& error_ind_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& error_ind_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const ri_crequest_id_s& error_ind_ies_o::value_c::ri_crequest_id() const
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
const uint16_t& error_ind_ies_o::value_c::ra_nfunction_id() const
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& error_ind_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const crit_diagnostics_s& error_ind_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void error_ind_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::ri_crequest_id:
      j.write_fieldname("RICrequestID");
      c.get<ri_crequest_id_s>().to_json(j);
      break;
    case types::ra_nfunction_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "error_ind_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE error_ind_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().pack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "error_ind_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE error_ind_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().unpack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "error_ind_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* error_ind_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {
      "INTEGER (0..255,...)", "RICrequestID", "INTEGER (0..4095)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 5, value, "error_ind_ies_o::value_c::types");
}

// RICcontrolAcknowledge-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ri_cctrl_ack_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {29, 5, 20, 32};
  return map_enum_number(options, 4, idx, "id");
}
bool ri_cctrl_ack_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {29, 5, 20, 32};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ri_cctrl_ack_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 20:
      return crit_e::reject;
    case 32:
      return crit_e::reject;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
ri_cctrl_ack_ies_o::value_c ri_cctrl_ack_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ri_crequest_id);
      break;
    case 5:
      ret.set(value_c::types::ra_nfunction_id);
      break;
    case 20:
      ret.set(value_c::types::ri_ccall_process_id);
      break;
    case 32:
      ret.set(value_c::types::ri_cctrl_outcome);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ri_cctrl_ack_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 20:
      return presence_e::optional;
    case 32:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ri_cctrl_ack_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::ri_crequest_id:
      c.destroy<ri_crequest_id_s>();
      break;
    case types::ri_ccall_process_id:
      c.destroy<unbounded_octstring<true> >();
      break;
    case types::ri_cctrl_outcome:
      c.destroy<unbounded_octstring<true> >();
      break;
    default:
      break;
  }
}
void ri_cctrl_ack_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ri_crequest_id:
      c.init<ri_crequest_id_s>();
      break;
    case types::ra_nfunction_id:
      break;
    case types::ri_ccall_process_id:
      c.init<unbounded_octstring<true> >();
      break;
    case types::ri_cctrl_outcome:
      c.init<unbounded_octstring<true> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_ack_ies_o::value_c");
  }
}
ri_cctrl_ack_ies_o::value_c::value_c(const ri_cctrl_ack_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ri_crequest_id:
      c.init(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ri_ccall_process_id:
      c.init(other.c.get<unbounded_octstring<true> >());
      break;
    case types::ri_cctrl_outcome:
      c.init(other.c.get<unbounded_octstring<true> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_ack_ies_o::value_c");
  }
}
ri_cctrl_ack_ies_o::value_c& ri_cctrl_ack_ies_o::value_c::operator=(const ri_cctrl_ack_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ri_crequest_id:
      c.set(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ri_ccall_process_id:
      c.set(other.c.get<unbounded_octstring<true> >());
      break;
    case types::ri_cctrl_outcome:
      c.set(other.c.get<unbounded_octstring<true> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_ack_ies_o::value_c");
  }

  return *this;
}
ri_crequest_id_s& ri_cctrl_ack_ies_o::value_c::ri_crequest_id()
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
uint16_t& ri_cctrl_ack_ies_o::value_c::ra_nfunction_id()
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
unbounded_octstring<true>& ri_cctrl_ack_ies_o::value_c::ri_ccall_process_id()
{
  assert_choice_type(types::ri_ccall_process_id, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
unbounded_octstring<true>& ri_cctrl_ack_ies_o::value_c::ri_cctrl_outcome()
{
  assert_choice_type(types::ri_cctrl_outcome, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
const ri_crequest_id_s& ri_cctrl_ack_ies_o::value_c::ri_crequest_id() const
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
const uint16_t& ri_cctrl_ack_ies_o::value_c::ra_nfunction_id() const
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
const unbounded_octstring<true>& ri_cctrl_ack_ies_o::value_c::ri_ccall_process_id() const
{
  assert_choice_type(types::ri_ccall_process_id, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
const unbounded_octstring<true>& ri_cctrl_ack_ies_o::value_c::ri_cctrl_outcome() const
{
  assert_choice_type(types::ri_cctrl_outcome, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
void ri_cctrl_ack_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ri_crequest_id:
      j.write_fieldname("RICrequestID");
      c.get<ri_crequest_id_s>().to_json(j);
      break;
    case types::ra_nfunction_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ri_ccall_process_id:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true> >().to_string());
      break;
    case types::ri_cctrl_outcome:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_ack_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ri_cctrl_ack_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().pack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ri_ccall_process_id:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().pack(bref));
      break;
    case types::ri_cctrl_outcome:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_ack_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ri_cctrl_ack_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().unpack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ri_ccall_process_id:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().unpack(bref));
      break;
    case types::ri_cctrl_outcome:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_ack_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ri_cctrl_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"RICrequestID", "INTEGER (0..4095)", "OCTET STRING", "OCTET STRING"};
  return convert_enum_idx(options, 4, value, "ri_cctrl_ack_ies_o::value_c::types");
}
uint8_t ri_cctrl_ack_ies_o::value_c::types_opts::to_number() const
{
  if (value == ra_nfunction_id) {
    return 0;
  }
  invalid_enum_number(value, "ri_cctrl_ack_ies_o::value_c::types");
  return 0;
}

// RICcontrolFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ri_cctrl_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {29, 5, 20, 1, 32};
  return map_enum_number(options, 5, idx, "id");
}
bool ri_cctrl_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {29, 5, 20, 1, 32};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ri_cctrl_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 20:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 32:
      return crit_e::reject;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
ri_cctrl_fail_ies_o::value_c ri_cctrl_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ri_crequest_id);
      break;
    case 5:
      ret.set(value_c::types::ra_nfunction_id);
      break;
    case 20:
      ret.set(value_c::types::ri_ccall_process_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 32:
      ret.set(value_c::types::ri_cctrl_outcome);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ri_cctrl_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 20:
      return presence_e::optional;
    case 1:
      return presence_e::mandatory;
    case 32:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ri_cctrl_fail_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::ri_crequest_id:
      c.destroy<ri_crequest_id_s>();
      break;
    case types::ri_ccall_process_id:
      c.destroy<unbounded_octstring<true> >();
      break;
    case types::cause:
      c.destroy<cause_c>();
      break;
    case types::ri_cctrl_outcome:
      c.destroy<unbounded_octstring<true> >();
      break;
    default:
      break;
  }
}
void ri_cctrl_fail_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ri_crequest_id:
      c.init<ri_crequest_id_s>();
      break;
    case types::ra_nfunction_id:
      break;
    case types::ri_ccall_process_id:
      c.init<unbounded_octstring<true> >();
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::ri_cctrl_outcome:
      c.init<unbounded_octstring<true> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_fail_ies_o::value_c");
  }
}
ri_cctrl_fail_ies_o::value_c::value_c(const ri_cctrl_fail_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ri_crequest_id:
      c.init(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ri_ccall_process_id:
      c.init(other.c.get<unbounded_octstring<true> >());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::ri_cctrl_outcome:
      c.init(other.c.get<unbounded_octstring<true> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_fail_ies_o::value_c");
  }
}
ri_cctrl_fail_ies_o::value_c& ri_cctrl_fail_ies_o::value_c::operator=(const ri_cctrl_fail_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ri_crequest_id:
      c.set(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ri_ccall_process_id:
      c.set(other.c.get<unbounded_octstring<true> >());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::ri_cctrl_outcome:
      c.set(other.c.get<unbounded_octstring<true> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_fail_ies_o::value_c");
  }

  return *this;
}
ri_crequest_id_s& ri_cctrl_fail_ies_o::value_c::ri_crequest_id()
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
uint16_t& ri_cctrl_fail_ies_o::value_c::ra_nfunction_id()
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
unbounded_octstring<true>& ri_cctrl_fail_ies_o::value_c::ri_ccall_process_id()
{
  assert_choice_type(types::ri_ccall_process_id, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
cause_c& ri_cctrl_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
unbounded_octstring<true>& ri_cctrl_fail_ies_o::value_c::ri_cctrl_outcome()
{
  assert_choice_type(types::ri_cctrl_outcome, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
const ri_crequest_id_s& ri_cctrl_fail_ies_o::value_c::ri_crequest_id() const
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
const uint16_t& ri_cctrl_fail_ies_o::value_c::ra_nfunction_id() const
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
const unbounded_octstring<true>& ri_cctrl_fail_ies_o::value_c::ri_ccall_process_id() const
{
  assert_choice_type(types::ri_ccall_process_id, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
const cause_c& ri_cctrl_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const unbounded_octstring<true>& ri_cctrl_fail_ies_o::value_c::ri_cctrl_outcome() const
{
  assert_choice_type(types::ri_cctrl_outcome, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
void ri_cctrl_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ri_crequest_id:
      j.write_fieldname("RICrequestID");
      c.get<ri_crequest_id_s>().to_json(j);
      break;
    case types::ra_nfunction_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ri_ccall_process_id:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true> >().to_string());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::ri_cctrl_outcome:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ri_cctrl_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().pack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ri_ccall_process_id:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().pack(bref));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::ri_cctrl_outcome:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ri_cctrl_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().unpack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ri_ccall_process_id:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().unpack(bref));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::ri_cctrl_outcome:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ri_cctrl_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"RICrequestID", "INTEGER (0..4095)", "OCTET STRING", "Cause", "OCTET STRING"};
  return convert_enum_idx(options, 5, value, "ri_cctrl_fail_ies_o::value_c::types");
}
uint8_t ri_cctrl_fail_ies_o::value_c::types_opts::to_number() const
{
  if (value == ra_nfunction_id) {
    return 0;
  }
  invalid_enum_number(value, "ri_cctrl_fail_ies_o::value_c::types");
  return 0;
}

// RICcontrolRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ri_cctrl_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {29, 5, 20, 22, 23, 21};
  return map_enum_number(options, 6, idx, "id");
}
bool ri_cctrl_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {29, 5, 20, 22, 23, 21};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ri_cctrl_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 20:
      return crit_e::reject;
    case 22:
      return crit_e::reject;
    case 23:
      return crit_e::reject;
    case 21:
      return crit_e::reject;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
ri_cctrl_request_ies_o::value_c ri_cctrl_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ri_crequest_id);
      break;
    case 5:
      ret.set(value_c::types::ra_nfunction_id);
      break;
    case 20:
      ret.set(value_c::types::ri_ccall_process_id);
      break;
    case 22:
      ret.set(value_c::types::ri_cctrl_hdr);
      break;
    case 23:
      ret.set(value_c::types::ri_cctrl_msg);
      break;
    case 21:
      ret.set(value_c::types::ri_cctrl_ack_request);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ri_cctrl_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 20:
      return presence_e::optional;
    case 22:
      return presence_e::mandatory;
    case 23:
      return presence_e::mandatory;
    case 21:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ri_cctrl_request_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::ri_crequest_id:
      c.destroy<ri_crequest_id_s>();
      break;
    case types::ri_ccall_process_id:
      c.destroy<unbounded_octstring<true> >();
      break;
    case types::ri_cctrl_hdr:
      c.destroy<unbounded_octstring<true> >();
      break;
    case types::ri_cctrl_msg:
      c.destroy<unbounded_octstring<true> >();
      break;
    default:
      break;
  }
}
void ri_cctrl_request_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ri_crequest_id:
      c.init<ri_crequest_id_s>();
      break;
    case types::ra_nfunction_id:
      break;
    case types::ri_ccall_process_id:
      c.init<unbounded_octstring<true> >();
      break;
    case types::ri_cctrl_hdr:
      c.init<unbounded_octstring<true> >();
      break;
    case types::ri_cctrl_msg:
      c.init<unbounded_octstring<true> >();
      break;
    case types::ri_cctrl_ack_request:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_request_ies_o::value_c");
  }
}
ri_cctrl_request_ies_o::value_c::value_c(const ri_cctrl_request_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ri_crequest_id:
      c.init(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ri_ccall_process_id:
      c.init(other.c.get<unbounded_octstring<true> >());
      break;
    case types::ri_cctrl_hdr:
      c.init(other.c.get<unbounded_octstring<true> >());
      break;
    case types::ri_cctrl_msg:
      c.init(other.c.get<unbounded_octstring<true> >());
      break;
    case types::ri_cctrl_ack_request:
      c.init(other.c.get<ri_cctrl_ack_request_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_request_ies_o::value_c");
  }
}
ri_cctrl_request_ies_o::value_c&
ri_cctrl_request_ies_o::value_c::operator=(const ri_cctrl_request_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ri_crequest_id:
      c.set(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ri_ccall_process_id:
      c.set(other.c.get<unbounded_octstring<true> >());
      break;
    case types::ri_cctrl_hdr:
      c.set(other.c.get<unbounded_octstring<true> >());
      break;
    case types::ri_cctrl_msg:
      c.set(other.c.get<unbounded_octstring<true> >());
      break;
    case types::ri_cctrl_ack_request:
      c.set(other.c.get<ri_cctrl_ack_request_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_request_ies_o::value_c");
  }

  return *this;
}
ri_crequest_id_s& ri_cctrl_request_ies_o::value_c::ri_crequest_id()
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
uint16_t& ri_cctrl_request_ies_o::value_c::ra_nfunction_id()
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
unbounded_octstring<true>& ri_cctrl_request_ies_o::value_c::ri_ccall_process_id()
{
  assert_choice_type(types::ri_ccall_process_id, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
unbounded_octstring<true>& ri_cctrl_request_ies_o::value_c::ri_cctrl_hdr()
{
  assert_choice_type(types::ri_cctrl_hdr, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
unbounded_octstring<true>& ri_cctrl_request_ies_o::value_c::ri_cctrl_msg()
{
  assert_choice_type(types::ri_cctrl_msg, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
ri_cctrl_ack_request_e& ri_cctrl_request_ies_o::value_c::ri_cctrl_ack_request()
{
  assert_choice_type(types::ri_cctrl_ack_request, type_, "Value");
  return c.get<ri_cctrl_ack_request_e>();
}
const ri_crequest_id_s& ri_cctrl_request_ies_o::value_c::ri_crequest_id() const
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
const uint16_t& ri_cctrl_request_ies_o::value_c::ra_nfunction_id() const
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
const unbounded_octstring<true>& ri_cctrl_request_ies_o::value_c::ri_ccall_process_id() const
{
  assert_choice_type(types::ri_ccall_process_id, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
const unbounded_octstring<true>& ri_cctrl_request_ies_o::value_c::ri_cctrl_hdr() const
{
  assert_choice_type(types::ri_cctrl_hdr, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
const unbounded_octstring<true>& ri_cctrl_request_ies_o::value_c::ri_cctrl_msg() const
{
  assert_choice_type(types::ri_cctrl_msg, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
const ri_cctrl_ack_request_e& ri_cctrl_request_ies_o::value_c::ri_cctrl_ack_request() const
{
  assert_choice_type(types::ri_cctrl_ack_request, type_, "Value");
  return c.get<ri_cctrl_ack_request_e>();
}
void ri_cctrl_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ri_crequest_id:
      j.write_fieldname("RICrequestID");
      c.get<ri_crequest_id_s>().to_json(j);
      break;
    case types::ra_nfunction_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ri_ccall_process_id:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true> >().to_string());
      break;
    case types::ri_cctrl_hdr:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true> >().to_string());
      break;
    case types::ri_cctrl_msg:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true> >().to_string());
      break;
    case types::ri_cctrl_ack_request:
      j.write_str("RICcontrolAckRequest", c.get<ri_cctrl_ack_request_e>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ri_cctrl_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().pack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ri_ccall_process_id:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().pack(bref));
      break;
    case types::ri_cctrl_hdr:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().pack(bref));
      break;
    case types::ri_cctrl_msg:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().pack(bref));
      break;
    case types::ri_cctrl_ack_request:
      HANDLE_CODE(c.get<ri_cctrl_ack_request_e>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ri_cctrl_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().unpack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ri_ccall_process_id:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().unpack(bref));
      break;
    case types::ri_cctrl_hdr:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().unpack(bref));
      break;
    case types::ri_cctrl_msg:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().unpack(bref));
      break;
    case types::ri_cctrl_ack_request:
      HANDLE_CODE(c.get<ri_cctrl_ack_request_e>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ri_cctrl_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ri_cctrl_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {
      "RICrequestID", "INTEGER (0..4095)", "OCTET STRING", "OCTET STRING", "OCTET STRING", "RICcontrolAckRequest"};
  return convert_enum_idx(options, 6, value, "ri_cctrl_request_ies_o::value_c::types");
}
uint8_t ri_cctrl_request_ies_o::value_c::types_opts::to_number() const
{
  if (value == ra_nfunction_id) {
    return 0;
  }
  invalid_enum_number(value, "ri_cctrl_request_ies_o::value_c::types");
  return 0;
}

// RICindication-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ri_cind_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {29, 5, 15, 27, 28, 25, 26, 20};
  return map_enum_number(options, 8, idx, "id");
}
bool ri_cind_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {29, 5, 15, 27, 28, 25, 26, 20};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ri_cind_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 15:
      return crit_e::reject;
    case 27:
      return crit_e::reject;
    case 28:
      return crit_e::reject;
    case 25:
      return crit_e::reject;
    case 26:
      return crit_e::reject;
    case 20:
      return crit_e::reject;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
ri_cind_ies_o::value_c ri_cind_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ri_crequest_id);
      break;
    case 5:
      ret.set(value_c::types::ra_nfunction_id);
      break;
    case 15:
      ret.set(value_c::types::ri_caction_id);
      break;
    case 27:
      ret.set(value_c::types::ri_cind_sn);
      break;
    case 28:
      ret.set(value_c::types::ri_cind_type);
      break;
    case 25:
      ret.set(value_c::types::ri_cind_hdr);
      break;
    case 26:
      ret.set(value_c::types::ri_cind_msg);
      break;
    case 20:
      ret.set(value_c::types::ri_ccall_process_id);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ri_cind_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 15:
      return presence_e::mandatory;
    case 27:
      return presence_e::optional;
    case 28:
      return presence_e::mandatory;
    case 25:
      return presence_e::mandatory;
    case 26:
      return presence_e::mandatory;
    case 20:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ri_cind_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::ri_crequest_id:
      c.destroy<ri_crequest_id_s>();
      break;
    case types::ri_cind_hdr:
      c.destroy<unbounded_octstring<true> >();
      break;
    case types::ri_cind_msg:
      c.destroy<unbounded_octstring<true> >();
      break;
    case types::ri_ccall_process_id:
      c.destroy<unbounded_octstring<true> >();
      break;
    default:
      break;
  }
}
void ri_cind_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ri_crequest_id:
      c.init<ri_crequest_id_s>();
      break;
    case types::ra_nfunction_id:
      break;
    case types::ri_caction_id:
      break;
    case types::ri_cind_sn:
      break;
    case types::ri_cind_type:
      break;
    case types::ri_cind_hdr:
      c.init<unbounded_octstring<true> >();
      break;
    case types::ri_cind_msg:
      c.init<unbounded_octstring<true> >();
      break;
    case types::ri_ccall_process_id:
      c.init<unbounded_octstring<true> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ri_cind_ies_o::value_c");
  }
}
ri_cind_ies_o::value_c::value_c(const ri_cind_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ri_crequest_id:
      c.init(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ri_caction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ri_cind_sn:
      c.init(other.c.get<uint32_t>());
      break;
    case types::ri_cind_type:
      c.init(other.c.get<ri_cind_type_e>());
      break;
    case types::ri_cind_hdr:
      c.init(other.c.get<unbounded_octstring<true> >());
      break;
    case types::ri_cind_msg:
      c.init(other.c.get<unbounded_octstring<true> >());
      break;
    case types::ri_ccall_process_id:
      c.init(other.c.get<unbounded_octstring<true> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ri_cind_ies_o::value_c");
  }
}
ri_cind_ies_o::value_c& ri_cind_ies_o::value_c::operator=(const ri_cind_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ri_crequest_id:
      c.set(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ri_caction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ri_cind_sn:
      c.set(other.c.get<uint32_t>());
      break;
    case types::ri_cind_type:
      c.set(other.c.get<ri_cind_type_e>());
      break;
    case types::ri_cind_hdr:
      c.set(other.c.get<unbounded_octstring<true> >());
      break;
    case types::ri_cind_msg:
      c.set(other.c.get<unbounded_octstring<true> >());
      break;
    case types::ri_ccall_process_id:
      c.set(other.c.get<unbounded_octstring<true> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ri_cind_ies_o::value_c");
  }

  return *this;
}
ri_crequest_id_s& ri_cind_ies_o::value_c::ri_crequest_id()
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
uint16_t& ri_cind_ies_o::value_c::ra_nfunction_id()
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
uint16_t& ri_cind_ies_o::value_c::ri_caction_id()
{
  assert_choice_type(types::ri_caction_id, type_, "Value");
  return c.get<uint16_t>();
}
uint32_t& ri_cind_ies_o::value_c::ri_cind_sn()
{
  assert_choice_type(types::ri_cind_sn, type_, "Value");
  return c.get<uint32_t>();
}
ri_cind_type_e& ri_cind_ies_o::value_c::ri_cind_type()
{
  assert_choice_type(types::ri_cind_type, type_, "Value");
  return c.get<ri_cind_type_e>();
}
unbounded_octstring<true>& ri_cind_ies_o::value_c::ri_cind_hdr()
{
  assert_choice_type(types::ri_cind_hdr, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
unbounded_octstring<true>& ri_cind_ies_o::value_c::ri_cind_msg()
{
  assert_choice_type(types::ri_cind_msg, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
unbounded_octstring<true>& ri_cind_ies_o::value_c::ri_ccall_process_id()
{
  assert_choice_type(types::ri_ccall_process_id, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
const ri_crequest_id_s& ri_cind_ies_o::value_c::ri_crequest_id() const
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
const uint16_t& ri_cind_ies_o::value_c::ra_nfunction_id() const
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
const uint16_t& ri_cind_ies_o::value_c::ri_caction_id() const
{
  assert_choice_type(types::ri_caction_id, type_, "Value");
  return c.get<uint16_t>();
}
const uint32_t& ri_cind_ies_o::value_c::ri_cind_sn() const
{
  assert_choice_type(types::ri_cind_sn, type_, "Value");
  return c.get<uint32_t>();
}
const ri_cind_type_e& ri_cind_ies_o::value_c::ri_cind_type() const
{
  assert_choice_type(types::ri_cind_type, type_, "Value");
  return c.get<ri_cind_type_e>();
}
const unbounded_octstring<true>& ri_cind_ies_o::value_c::ri_cind_hdr() const
{
  assert_choice_type(types::ri_cind_hdr, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
const unbounded_octstring<true>& ri_cind_ies_o::value_c::ri_cind_msg() const
{
  assert_choice_type(types::ri_cind_msg, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
const unbounded_octstring<true>& ri_cind_ies_o::value_c::ri_ccall_process_id() const
{
  assert_choice_type(types::ri_ccall_process_id, type_, "Value");
  return c.get<unbounded_octstring<true> >();
}
void ri_cind_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ri_crequest_id:
      j.write_fieldname("RICrequestID");
      c.get<ri_crequest_id_s>().to_json(j);
      break;
    case types::ra_nfunction_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ri_caction_id:
      j.write_int("INTEGER (0..255)", c.get<uint16_t>());
      break;
    case types::ri_cind_sn:
      j.write_int("INTEGER (0..65535)", c.get<uint32_t>());
      break;
    case types::ri_cind_type:
      j.write_str("RICindicationType", c.get<ri_cind_type_e>().to_string());
      break;
    case types::ri_cind_hdr:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true> >().to_string());
      break;
    case types::ri_cind_msg:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true> >().to_string());
      break;
    case types::ri_ccall_process_id:
      j.write_str("OCTET STRING", c.get<unbounded_octstring<true> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ri_cind_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ri_cind_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().pack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ri_caction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, false, true));
      break;
    case types::ri_cind_sn:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)0u, (uint32_t)65535u, false, true));
      break;
    case types::ri_cind_type:
      HANDLE_CODE(c.get<ri_cind_type_e>().pack(bref));
      break;
    case types::ri_cind_hdr:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().pack(bref));
      break;
    case types::ri_cind_msg:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().pack(bref));
      break;
    case types::ri_ccall_process_id:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ri_cind_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ri_cind_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().unpack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ri_caction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, false, true));
      break;
    case types::ri_cind_sn:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)0u, (uint32_t)65535u, false, true));
      break;
    case types::ri_cind_type:
      HANDLE_CODE(c.get<ri_cind_type_e>().unpack(bref));
      break;
    case types::ri_cind_hdr:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().unpack(bref));
      break;
    case types::ri_cind_msg:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().unpack(bref));
      break;
    case types::ri_ccall_process_id:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ri_cind_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ri_cind_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"RICrequestID",
                                  "INTEGER (0..4095)",
                                  "INTEGER (0..255)",
                                  "INTEGER (0..65535)",
                                  "RICindicationType",
                                  "OCTET STRING",
                                  "OCTET STRING",
                                  "OCTET STRING"};
  return convert_enum_idx(options, 8, value, "ri_cind_ies_o::value_c::types");
}

// RICserviceQuery-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ricservice_query_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 9};
  return map_enum_number(options, 2, idx, "id");
}
bool ricservice_query_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 9};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ricservice_query_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 9:
      return crit_e::reject;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
ricservice_query_ies_o::value_c ricservice_query_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 9:
      ret.set(value_c::types::ra_nfunctions_accepted);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ricservice_query_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 9:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ricservice_query_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::ra_nfunctions_accepted:
      c.destroy<ra_nfunctions_id_list_l>();
      break;
    default:
      break;
  }
}
void ricservice_query_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::ra_nfunctions_accepted:
      c.init<ra_nfunctions_id_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_query_ies_o::value_c");
  }
}
ricservice_query_ies_o::value_c::value_c(const ricservice_query_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ra_nfunctions_accepted:
      c.init(other.c.get<ra_nfunctions_id_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_query_ies_o::value_c");
  }
}
ricservice_query_ies_o::value_c&
ricservice_query_ies_o::value_c::operator=(const ricservice_query_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ra_nfunctions_accepted:
      c.set(other.c.get<ra_nfunctions_id_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_query_ies_o::value_c");
  }

  return *this;
}
uint16_t& ricservice_query_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
ra_nfunctions_id_list_l& ricservice_query_ies_o::value_c::ra_nfunctions_accepted()
{
  assert_choice_type(types::ra_nfunctions_accepted, type_, "Value");
  return c.get<ra_nfunctions_id_list_l>();
}
const uint16_t& ricservice_query_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const ra_nfunctions_id_list_l& ricservice_query_ies_o::value_c::ra_nfunctions_accepted() const
{
  assert_choice_type(types::ra_nfunctions_accepted, type_, "Value");
  return c.get<ra_nfunctions_id_list_l>();
}
void ricservice_query_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::ra_nfunctions_accepted:
      j.start_array("RANfunctionsID-List");
      for (const auto& e1 : c.get<ra_nfunctions_id_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_query_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ricservice_query_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ra_nfunctions_accepted:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ra_nfunctions_id_list_l>(), 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_query_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ricservice_query_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ra_nfunctions_accepted:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ra_nfunctions_id_list_l>(), bref, 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_query_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ricservice_query_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)", "RANfunctionsID-List"};
  return convert_enum_idx(options, 2, value, "ricservice_query_ies_o::value_c::types");
}
uint8_t ricservice_query_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "ricservice_query_ies_o::value_c::types");
}

// RICserviceUpdate-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ricservice_upd_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 10, 12, 11};
  return map_enum_number(options, 4, idx, "id");
}
bool ricservice_upd_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 10, 12, 11};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ricservice_upd_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 10:
      return crit_e::reject;
    case 12:
      return crit_e::reject;
    case 11:
      return crit_e::reject;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
ricservice_upd_ies_o::value_c ricservice_upd_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 10:
      ret.set(value_c::types::ra_nfunctions_added);
      break;
    case 12:
      ret.set(value_c::types::ra_nfunctions_modified);
      break;
    case 11:
      ret.set(value_c::types::ra_nfunctions_deleted);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ricservice_upd_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 10:
      return presence_e::optional;
    case 12:
      return presence_e::optional;
    case 11:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ricservice_upd_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::ra_nfunctions_added:
      c.destroy<ra_nfunctions_list_l>();
      break;
    case types::ra_nfunctions_modified:
      c.destroy<ra_nfunctions_list_l>();
      break;
    case types::ra_nfunctions_deleted:
      c.destroy<ra_nfunctions_id_list_l>();
      break;
    default:
      break;
  }
}
void ricservice_upd_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::ra_nfunctions_added:
      c.init<ra_nfunctions_list_l>();
      break;
    case types::ra_nfunctions_modified:
      c.init<ra_nfunctions_list_l>();
      break;
    case types::ra_nfunctions_deleted:
      c.init<ra_nfunctions_id_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_ies_o::value_c");
  }
}
ricservice_upd_ies_o::value_c::value_c(const ricservice_upd_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ra_nfunctions_added:
      c.init(other.c.get<ra_nfunctions_list_l>());
      break;
    case types::ra_nfunctions_modified:
      c.init(other.c.get<ra_nfunctions_list_l>());
      break;
    case types::ra_nfunctions_deleted:
      c.init(other.c.get<ra_nfunctions_id_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_ies_o::value_c");
  }
}
ricservice_upd_ies_o::value_c& ricservice_upd_ies_o::value_c::operator=(const ricservice_upd_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ra_nfunctions_added:
      c.set(other.c.get<ra_nfunctions_list_l>());
      break;
    case types::ra_nfunctions_modified:
      c.set(other.c.get<ra_nfunctions_list_l>());
      break;
    case types::ra_nfunctions_deleted:
      c.set(other.c.get<ra_nfunctions_id_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_ies_o::value_c");
  }

  return *this;
}
uint16_t& ricservice_upd_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
ra_nfunctions_list_l& ricservice_upd_ies_o::value_c::ra_nfunctions_added()
{
  assert_choice_type(types::ra_nfunctions_added, type_, "Value");
  return c.get<ra_nfunctions_list_l>();
}
ra_nfunctions_list_l& ricservice_upd_ies_o::value_c::ra_nfunctions_modified()
{
  assert_choice_type(types::ra_nfunctions_modified, type_, "Value");
  return c.get<ra_nfunctions_list_l>();
}
ra_nfunctions_id_list_l& ricservice_upd_ies_o::value_c::ra_nfunctions_deleted()
{
  assert_choice_type(types::ra_nfunctions_deleted, type_, "Value");
  return c.get<ra_nfunctions_id_list_l>();
}
const uint16_t& ricservice_upd_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const ra_nfunctions_list_l& ricservice_upd_ies_o::value_c::ra_nfunctions_added() const
{
  assert_choice_type(types::ra_nfunctions_added, type_, "Value");
  return c.get<ra_nfunctions_list_l>();
}
const ra_nfunctions_list_l& ricservice_upd_ies_o::value_c::ra_nfunctions_modified() const
{
  assert_choice_type(types::ra_nfunctions_modified, type_, "Value");
  return c.get<ra_nfunctions_list_l>();
}
const ra_nfunctions_id_list_l& ricservice_upd_ies_o::value_c::ra_nfunctions_deleted() const
{
  assert_choice_type(types::ra_nfunctions_deleted, type_, "Value");
  return c.get<ra_nfunctions_id_list_l>();
}
void ricservice_upd_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::ra_nfunctions_added:
      j.start_array("RANfunctions-List");
      for (const auto& e1 : c.get<ra_nfunctions_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ra_nfunctions_modified:
      j.start_array("RANfunctions-List");
      for (const auto& e1 : c.get<ra_nfunctions_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ra_nfunctions_deleted:
      j.start_array("RANfunctionsID-List");
      for (const auto& e1 : c.get<ra_nfunctions_id_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ricservice_upd_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ra_nfunctions_added:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ra_nfunctions_list_l>(), 1, 256, true));
      break;
    case types::ra_nfunctions_modified:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ra_nfunctions_list_l>(), 1, 256, true));
      break;
    case types::ra_nfunctions_deleted:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ra_nfunctions_id_list_l>(), 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ricservice_upd_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ra_nfunctions_added:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ra_nfunctions_list_l>(), bref, 1, 256, true));
      break;
    case types::ra_nfunctions_modified:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ra_nfunctions_list_l>(), bref, 1, 256, true));
      break;
    case types::ra_nfunctions_deleted:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ra_nfunctions_id_list_l>(), bref, 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ricservice_upd_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {
      "INTEGER (0..255,...)", "RANfunctions-List", "RANfunctions-List", "RANfunctionsID-List"};
  return convert_enum_idx(options, 4, value, "ricservice_upd_ies_o::value_c::types");
}
uint8_t ricservice_upd_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "ricservice_upd_ies_o::value_c::types");
}

// RICserviceUpdateAcknowledge-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ricservice_upd_ack_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 9, 13};
  return map_enum_number(options, 3, idx, "id");
}
bool ricservice_upd_ack_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 9, 13};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ricservice_upd_ack_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 9:
      return crit_e::reject;
    case 13:
      return crit_e::reject;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
ricservice_upd_ack_ies_o::value_c ricservice_upd_ack_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 9:
      ret.set(value_c::types::ra_nfunctions_accepted);
      break;
    case 13:
      ret.set(value_c::types::ra_nfunctions_rejected);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ricservice_upd_ack_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 9:
      return presence_e::optional;
    case 13:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ricservice_upd_ack_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::ra_nfunctions_accepted:
      c.destroy<ra_nfunctions_id_list_l>();
      break;
    case types::ra_nfunctions_rejected:
      c.destroy<ra_nfunctions_idcause_list_l>();
      break;
    default:
      break;
  }
}
void ricservice_upd_ack_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::ra_nfunctions_accepted:
      c.init<ra_nfunctions_id_list_l>();
      break;
    case types::ra_nfunctions_rejected:
      c.init<ra_nfunctions_idcause_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_ack_ies_o::value_c");
  }
}
ricservice_upd_ack_ies_o::value_c::value_c(const ricservice_upd_ack_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ra_nfunctions_accepted:
      c.init(other.c.get<ra_nfunctions_id_list_l>());
      break;
    case types::ra_nfunctions_rejected:
      c.init(other.c.get<ra_nfunctions_idcause_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_ack_ies_o::value_c");
  }
}
ricservice_upd_ack_ies_o::value_c&
ricservice_upd_ack_ies_o::value_c::operator=(const ricservice_upd_ack_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ra_nfunctions_accepted:
      c.set(other.c.get<ra_nfunctions_id_list_l>());
      break;
    case types::ra_nfunctions_rejected:
      c.set(other.c.get<ra_nfunctions_idcause_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_ack_ies_o::value_c");
  }

  return *this;
}
uint16_t& ricservice_upd_ack_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
ra_nfunctions_id_list_l& ricservice_upd_ack_ies_o::value_c::ra_nfunctions_accepted()
{
  assert_choice_type(types::ra_nfunctions_accepted, type_, "Value");
  return c.get<ra_nfunctions_id_list_l>();
}
ra_nfunctions_idcause_list_l& ricservice_upd_ack_ies_o::value_c::ra_nfunctions_rejected()
{
  assert_choice_type(types::ra_nfunctions_rejected, type_, "Value");
  return c.get<ra_nfunctions_idcause_list_l>();
}
const uint16_t& ricservice_upd_ack_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const ra_nfunctions_id_list_l& ricservice_upd_ack_ies_o::value_c::ra_nfunctions_accepted() const
{
  assert_choice_type(types::ra_nfunctions_accepted, type_, "Value");
  return c.get<ra_nfunctions_id_list_l>();
}
const ra_nfunctions_idcause_list_l& ricservice_upd_ack_ies_o::value_c::ra_nfunctions_rejected() const
{
  assert_choice_type(types::ra_nfunctions_rejected, type_, "Value");
  return c.get<ra_nfunctions_idcause_list_l>();
}
void ricservice_upd_ack_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::ra_nfunctions_accepted:
      j.start_array("RANfunctionsID-List");
      for (const auto& e1 : c.get<ra_nfunctions_id_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ra_nfunctions_rejected:
      j.start_array("RANfunctionsIDcause-List");
      for (const auto& e1 : c.get<ra_nfunctions_idcause_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_ack_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ricservice_upd_ack_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ra_nfunctions_accepted:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ra_nfunctions_id_list_l>(), 1, 256, true));
      break;
    case types::ra_nfunctions_rejected:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ra_nfunctions_idcause_list_l>(), 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ricservice_upd_ack_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::ra_nfunctions_accepted:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ra_nfunctions_id_list_l>(), bref, 1, 256, true));
      break;
    case types::ra_nfunctions_rejected:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ra_nfunctions_idcause_list_l>(), bref, 1, 256, true));
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_ack_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ricservice_upd_ack_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)", "RANfunctionsID-List", "RANfunctionsIDcause-List"};
  return convert_enum_idx(options, 3, value, "ricservice_upd_ack_ies_o::value_c::types");
}
uint8_t ricservice_upd_ack_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "ricservice_upd_ack_ies_o::value_c::types");
}

// RICserviceUpdateFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ricservice_upd_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 1, 31, 2};
  return map_enum_number(options, 4, idx, "id");
}
bool ricservice_upd_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 1, 31, 2};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ricservice_upd_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 1:
      return crit_e::reject;
    case 31:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
ricservice_upd_fail_ies_o::value_c ricservice_upd_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 31:
      ret.set(value_c::types::time_to_wait);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ricservice_upd_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 1:
      return presence_e::mandatory;
    case 31:
      return presence_e::optional;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ricservice_upd_fail_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::cause:
      c.destroy<cause_c>();
      break;
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    default:
      break;
  }
}
void ricservice_upd_fail_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::time_to_wait:
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_fail_ies_o::value_c");
  }
}
ricservice_upd_fail_ies_o::value_c::value_c(const ricservice_upd_fail_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::time_to_wait:
      c.init(other.c.get<time_to_wait_e>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_fail_ies_o::value_c");
  }
}
ricservice_upd_fail_ies_o::value_c&
ricservice_upd_fail_ies_o::value_c::operator=(const ricservice_upd_fail_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::time_to_wait:
      c.set(other.c.get<time_to_wait_e>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_fail_ies_o::value_c");
  }

  return *this;
}
uint16_t& ricservice_upd_fail_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& ricservice_upd_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
time_to_wait_e& ricservice_upd_fail_ies_o::value_c::time_to_wait()
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
crit_diagnostics_s& ricservice_upd_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& ricservice_upd_fail_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& ricservice_upd_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const time_to_wait_e& ricservice_upd_fail_ies_o::value_c::time_to_wait() const
{
  assert_choice_type(types::time_to_wait, type_, "Value");
  return c.get<time_to_wait_e>();
}
const crit_diagnostics_s& ricservice_upd_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void ricservice_upd_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::time_to_wait:
      j.write_str("TimeToWait", c.get<time_to_wait_e>().to_string());
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ricservice_upd_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::time_to_wait:
      HANDLE_CODE(c.get<time_to_wait_e>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ricservice_upd_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::time_to_wait:
      HANDLE_CODE(c.get<time_to_wait_e>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ricservice_upd_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ricservice_upd_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)", "Cause", "TimeToWait", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 4, value, "ricservice_upd_fail_ies_o::value_c::types");
}
uint8_t ricservice_upd_fail_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "ricservice_upd_fail_ies_o::value_c::types");
}

// RICsubscriptionDeleteFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ricsubscription_delete_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {29, 5, 1, 2};
  return map_enum_number(options, 4, idx, "id");
}
bool ricsubscription_delete_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {29, 5, 1, 2};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ricsubscription_delete_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
ricsubscription_delete_fail_ies_o::value_c ricsubscription_delete_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ri_crequest_id);
      break;
    case 5:
      ret.set(value_c::types::ra_nfunction_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ricsubscription_delete_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 1:
      return presence_e::mandatory;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ricsubscription_delete_fail_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::ri_crequest_id:
      c.destroy<ri_crequest_id_s>();
      break;
    case types::cause:
      c.destroy<cause_c>();
      break;
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    default:
      break;
  }
}
void ricsubscription_delete_fail_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ri_crequest_id:
      c.init<ri_crequest_id_s>();
      break;
    case types::ra_nfunction_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_fail_ies_o::value_c");
  }
}
ricsubscription_delete_fail_ies_o::value_c::value_c(const ricsubscription_delete_fail_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ri_crequest_id:
      c.init(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_fail_ies_o::value_c");
  }
}
ricsubscription_delete_fail_ies_o::value_c&
ricsubscription_delete_fail_ies_o::value_c::operator=(const ricsubscription_delete_fail_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ri_crequest_id:
      c.set(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_fail_ies_o::value_c");
  }

  return *this;
}
ri_crequest_id_s& ricsubscription_delete_fail_ies_o::value_c::ri_crequest_id()
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
uint16_t& ricsubscription_delete_fail_ies_o::value_c::ra_nfunction_id()
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& ricsubscription_delete_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
crit_diagnostics_s& ricsubscription_delete_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const ri_crequest_id_s& ricsubscription_delete_fail_ies_o::value_c::ri_crequest_id() const
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
const uint16_t& ricsubscription_delete_fail_ies_o::value_c::ra_nfunction_id() const
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& ricsubscription_delete_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const crit_diagnostics_s& ricsubscription_delete_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void ricsubscription_delete_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ri_crequest_id:
      j.write_fieldname("RICrequestID");
      c.get<ri_crequest_id_s>().to_json(j);
      break;
    case types::ra_nfunction_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ricsubscription_delete_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().pack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ricsubscription_delete_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().unpack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ricsubscription_delete_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"RICrequestID", "INTEGER (0..4095)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 4, value, "ricsubscription_delete_fail_ies_o::value_c::types");
}
uint8_t ricsubscription_delete_fail_ies_o::value_c::types_opts::to_number() const
{
  if (value == ra_nfunction_id) {
    return 0;
  }
  invalid_enum_number(value, "ricsubscription_delete_fail_ies_o::value_c::types");
  return 0;
}

// RICsubscriptionDeleteRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ricsubscription_delete_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {29, 5};
  return map_enum_number(options, 2, idx, "id");
}
bool ricsubscription_delete_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {29, 5};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ricsubscription_delete_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
ricsubscription_delete_request_ies_o::value_c ricsubscription_delete_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ri_crequest_id);
      break;
    case 5:
      ret.set(value_c::types::ra_nfunction_id);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ricsubscription_delete_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ricsubscription_delete_request_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::ri_crequest_id:
      c.destroy<ri_crequest_id_s>();
      break;
    default:
      break;
  }
}
void ricsubscription_delete_request_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ri_crequest_id:
      c.init<ri_crequest_id_s>();
      break;
    case types::ra_nfunction_id:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_request_ies_o::value_c");
  }
}
ricsubscription_delete_request_ies_o::value_c::value_c(const ricsubscription_delete_request_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ri_crequest_id:
      c.init(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_request_ies_o::value_c");
  }
}
ricsubscription_delete_request_ies_o::value_c&
ricsubscription_delete_request_ies_o::value_c::operator=(const ricsubscription_delete_request_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ri_crequest_id:
      c.set(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_request_ies_o::value_c");
  }

  return *this;
}
ri_crequest_id_s& ricsubscription_delete_request_ies_o::value_c::ri_crequest_id()
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
uint16_t& ricsubscription_delete_request_ies_o::value_c::ra_nfunction_id()
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
const ri_crequest_id_s& ricsubscription_delete_request_ies_o::value_c::ri_crequest_id() const
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
const uint16_t& ricsubscription_delete_request_ies_o::value_c::ra_nfunction_id() const
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
void ricsubscription_delete_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ri_crequest_id:
      j.write_fieldname("RICrequestID");
      c.get<ri_crequest_id_s>().to_json(j);
      break;
    case types::ra_nfunction_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ricsubscription_delete_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().pack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ricsubscription_delete_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().unpack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ricsubscription_delete_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"RICrequestID", "INTEGER (0..4095)"};
  return convert_enum_idx(options, 2, value, "ricsubscription_delete_request_ies_o::value_c::types");
}
uint8_t ricsubscription_delete_request_ies_o::value_c::types_opts::to_number() const
{
  if (value == ra_nfunction_id) {
    return 0;
  }
  invalid_enum_number(value, "ricsubscription_delete_request_ies_o::value_c::types");
  return 0;
}

// RICsubscriptionDeleteRequired-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ricsubscription_delete_required_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {60};
  return map_enum_number(options, 1, idx, "id");
}
bool ricsubscription_delete_required_ies_o::is_id_valid(const uint32_t& id)
{
  return 60 == id;
}
crit_e ricsubscription_delete_required_ies_o::get_crit(const uint32_t& id)
{
  if (id == 60) {
    return crit_e::ignore;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}
ricsubscription_delete_required_ies_o::value_c ricsubscription_delete_required_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  if (id != 60) {
    asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ricsubscription_delete_required_ies_o::get_presence(const uint32_t& id)
{
  if (id == 60) {
    return presence_e::mandatory;
  }
  asn1::log_error("The id=%d is not recognized", id);
  return {};
}

// Value ::= OPEN TYPE
void ricsubscription_delete_required_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("RICsubscription-List-withCause");
  for (const auto& e1 : c) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}
SRSASN_CODE ricsubscription_delete_required_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  HANDLE_CODE(pack_dyn_seq_of(bref, c, 1, 1024, true));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ricsubscription_delete_required_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  HANDLE_CODE(unpack_dyn_seq_of(c, bref, 1, 1024, true));
  return SRSASN_SUCCESS;
}

const char* ricsubscription_delete_required_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"RICsubscription-List-withCause"};
  return convert_enum_idx(options, 1, value, "ricsubscription_delete_required_ies_o::value_c::types");
}

// RICsubscriptionDeleteResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ricsubscription_delete_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {29, 5};
  return map_enum_number(options, 2, idx, "id");
}
bool ricsubscription_delete_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {29, 5};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ricsubscription_delete_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
ricsubscription_delete_resp_ies_o::value_c ricsubscription_delete_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ri_crequest_id);
      break;
    case 5:
      ret.set(value_c::types::ra_nfunction_id);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ricsubscription_delete_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ricsubscription_delete_resp_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::ri_crequest_id:
      c.destroy<ri_crequest_id_s>();
      break;
    default:
      break;
  }
}
void ricsubscription_delete_resp_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ri_crequest_id:
      c.init<ri_crequest_id_s>();
      break;
    case types::ra_nfunction_id:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_resp_ies_o::value_c");
  }
}
ricsubscription_delete_resp_ies_o::value_c::value_c(const ricsubscription_delete_resp_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ri_crequest_id:
      c.init(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_resp_ies_o::value_c");
  }
}
ricsubscription_delete_resp_ies_o::value_c&
ricsubscription_delete_resp_ies_o::value_c::operator=(const ricsubscription_delete_resp_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ri_crequest_id:
      c.set(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_resp_ies_o::value_c");
  }

  return *this;
}
ri_crequest_id_s& ricsubscription_delete_resp_ies_o::value_c::ri_crequest_id()
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
uint16_t& ricsubscription_delete_resp_ies_o::value_c::ra_nfunction_id()
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
const ri_crequest_id_s& ricsubscription_delete_resp_ies_o::value_c::ri_crequest_id() const
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
const uint16_t& ricsubscription_delete_resp_ies_o::value_c::ra_nfunction_id() const
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
void ricsubscription_delete_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ri_crequest_id:
      j.write_fieldname("RICrequestID");
      c.get<ri_crequest_id_s>().to_json(j);
      break;
    case types::ra_nfunction_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ricsubscription_delete_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().pack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ricsubscription_delete_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().unpack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_delete_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ricsubscription_delete_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"RICrequestID", "INTEGER (0..4095)"};
  return convert_enum_idx(options, 2, value, "ricsubscription_delete_resp_ies_o::value_c::types");
}
uint8_t ricsubscription_delete_resp_ies_o::value_c::types_opts::to_number() const
{
  if (value == ra_nfunction_id) {
    return 0;
  }
  invalid_enum_number(value, "ricsubscription_delete_resp_ies_o::value_c::types");
  return 0;
}

// RICsubscriptionFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ricsubscription_fail_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {29, 5, 1, 2};
  return map_enum_number(options, 4, idx, "id");
}
bool ricsubscription_fail_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {29, 5, 1, 2};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ricsubscription_fail_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 1:
      return crit_e::reject;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
ricsubscription_fail_ies_o::value_c ricsubscription_fail_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ri_crequest_id);
      break;
    case 5:
      ret.set(value_c::types::ra_nfunction_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ricsubscription_fail_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 1:
      return presence_e::mandatory;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ricsubscription_fail_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::ri_crequest_id:
      c.destroy<ri_crequest_id_s>();
      break;
    case types::cause:
      c.destroy<cause_c>();
      break;
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    default:
      break;
  }
}
void ricsubscription_fail_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ri_crequest_id:
      c.init<ri_crequest_id_s>();
      break;
    case types::ra_nfunction_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_fail_ies_o::value_c");
  }
}
ricsubscription_fail_ies_o::value_c::value_c(const ricsubscription_fail_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ri_crequest_id:
      c.init(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_fail_ies_o::value_c");
  }
}
ricsubscription_fail_ies_o::value_c&
ricsubscription_fail_ies_o::value_c::operator=(const ricsubscription_fail_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ri_crequest_id:
      c.set(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_fail_ies_o::value_c");
  }

  return *this;
}
ri_crequest_id_s& ricsubscription_fail_ies_o::value_c::ri_crequest_id()
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
uint16_t& ricsubscription_fail_ies_o::value_c::ra_nfunction_id()
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& ricsubscription_fail_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
crit_diagnostics_s& ricsubscription_fail_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const ri_crequest_id_s& ricsubscription_fail_ies_o::value_c::ri_crequest_id() const
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
const uint16_t& ricsubscription_fail_ies_o::value_c::ra_nfunction_id() const
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& ricsubscription_fail_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const crit_diagnostics_s& ricsubscription_fail_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void ricsubscription_fail_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ri_crequest_id:
      j.write_fieldname("RICrequestID");
      c.get<ri_crequest_id_s>().to_json(j);
      break;
    case types::ra_nfunction_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_fail_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ricsubscription_fail_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().pack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_fail_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ricsubscription_fail_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().unpack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_fail_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ricsubscription_fail_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"RICrequestID", "INTEGER (0..4095)", "Cause", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 4, value, "ricsubscription_fail_ies_o::value_c::types");
}
uint8_t ricsubscription_fail_ies_o::value_c::types_opts::to_number() const
{
  if (value == ra_nfunction_id) {
    return 0;
  }
  invalid_enum_number(value, "ricsubscription_fail_ies_o::value_c::types");
  return 0;
}

// RICsubscriptionRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ricsubscription_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {29, 5, 30};
  return map_enum_number(options, 3, idx, "id");
}
bool ricsubscription_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {29, 5, 30};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ricsubscription_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 30:
      return crit_e::reject;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
ricsubscription_request_ies_o::value_c ricsubscription_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ri_crequest_id);
      break;
    case 5:
      ret.set(value_c::types::ra_nfunction_id);
      break;
    case 30:
      ret.set(value_c::types::ricsubscription_details);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ricsubscription_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 30:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ricsubscription_request_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::ri_crequest_id:
      c.destroy<ri_crequest_id_s>();
      break;
    case types::ricsubscription_details:
      c.destroy<ricsubscription_details_s>();
      break;
    default:
      break;
  }
}
void ricsubscription_request_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ri_crequest_id:
      c.init<ri_crequest_id_s>();
      break;
    case types::ra_nfunction_id:
      break;
    case types::ricsubscription_details:
      c.init<ricsubscription_details_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_request_ies_o::value_c");
  }
}
ricsubscription_request_ies_o::value_c::value_c(const ricsubscription_request_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ri_crequest_id:
      c.init(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ricsubscription_details:
      c.init(other.c.get<ricsubscription_details_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_request_ies_o::value_c");
  }
}
ricsubscription_request_ies_o::value_c&
ricsubscription_request_ies_o::value_c::operator=(const ricsubscription_request_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ri_crequest_id:
      c.set(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ricsubscription_details:
      c.set(other.c.get<ricsubscription_details_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_request_ies_o::value_c");
  }

  return *this;
}
ri_crequest_id_s& ricsubscription_request_ies_o::value_c::ri_crequest_id()
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
uint16_t& ricsubscription_request_ies_o::value_c::ra_nfunction_id()
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
ricsubscription_details_s& ricsubscription_request_ies_o::value_c::ricsubscription_details()
{
  assert_choice_type(types::ricsubscription_details, type_, "Value");
  return c.get<ricsubscription_details_s>();
}
const ri_crequest_id_s& ricsubscription_request_ies_o::value_c::ri_crequest_id() const
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
const uint16_t& ricsubscription_request_ies_o::value_c::ra_nfunction_id() const
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
const ricsubscription_details_s& ricsubscription_request_ies_o::value_c::ricsubscription_details() const
{
  assert_choice_type(types::ricsubscription_details, type_, "Value");
  return c.get<ricsubscription_details_s>();
}
void ricsubscription_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ri_crequest_id:
      j.write_fieldname("RICrequestID");
      c.get<ri_crequest_id_s>().to_json(j);
      break;
    case types::ra_nfunction_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ricsubscription_details:
      j.write_fieldname("RICsubscriptionDetails");
      c.get<ricsubscription_details_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ricsubscription_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().pack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ricsubscription_details:
      HANDLE_CODE(c.get<ricsubscription_details_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ricsubscription_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().unpack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ricsubscription_details:
      HANDLE_CODE(c.get<ricsubscription_details_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ricsubscription_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"RICrequestID", "INTEGER (0..4095)", "RICsubscriptionDetails"};
  return convert_enum_idx(options, 3, value, "ricsubscription_request_ies_o::value_c::types");
}
uint8_t ricsubscription_request_ies_o::value_c::types_opts::to_number() const
{
  if (value == ra_nfunction_id) {
    return 0;
  }
  invalid_enum_number(value, "ricsubscription_request_ies_o::value_c::types");
  return 0;
}

// RICsubscriptionResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t ricsubscription_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {29, 5, 17, 18};
  return map_enum_number(options, 4, idx, "id");
}
bool ricsubscription_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {29, 5, 17, 18};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e ricsubscription_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 29:
      return crit_e::reject;
    case 5:
      return crit_e::reject;
    case 17:
      return crit_e::reject;
    case 18:
      return crit_e::reject;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
ricsubscription_resp_ies_o::value_c ricsubscription_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 29:
      ret.set(value_c::types::ri_crequest_id);
      break;
    case 5:
      ret.set(value_c::types::ra_nfunction_id);
      break;
    case 17:
      ret.set(value_c::types::ri_cactions_admitted);
      break;
    case 18:
      ret.set(value_c::types::ri_cactions_not_admitted);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e ricsubscription_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 29:
      return presence_e::mandatory;
    case 5:
      return presence_e::mandatory;
    case 17:
      return presence_e::mandatory;
    case 18:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void ricsubscription_resp_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::ri_crequest_id:
      c.destroy<ri_crequest_id_s>();
      break;
    case types::ri_cactions_admitted:
      c.destroy<ri_caction_admitted_list_l>();
      break;
    case types::ri_cactions_not_admitted:
      c.destroy<ri_caction_not_admitted_list_l>();
      break;
    default:
      break;
  }
}
void ricsubscription_resp_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ri_crequest_id:
      c.init<ri_crequest_id_s>();
      break;
    case types::ra_nfunction_id:
      break;
    case types::ri_cactions_admitted:
      c.init<ri_caction_admitted_list_l>();
      break;
    case types::ri_cactions_not_admitted:
      c.init<ri_caction_not_admitted_list_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_resp_ies_o::value_c");
  }
}
ricsubscription_resp_ies_o::value_c::value_c(const ricsubscription_resp_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ri_crequest_id:
      c.init(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::ri_cactions_admitted:
      c.init(other.c.get<ri_caction_admitted_list_l>());
      break;
    case types::ri_cactions_not_admitted:
      c.init(other.c.get<ri_caction_not_admitted_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_resp_ies_o::value_c");
  }
}
ricsubscription_resp_ies_o::value_c&
ricsubscription_resp_ies_o::value_c::operator=(const ricsubscription_resp_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ri_crequest_id:
      c.set(other.c.get<ri_crequest_id_s>());
      break;
    case types::ra_nfunction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::ri_cactions_admitted:
      c.set(other.c.get<ri_caction_admitted_list_l>());
      break;
    case types::ri_cactions_not_admitted:
      c.set(other.c.get<ri_caction_not_admitted_list_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_resp_ies_o::value_c");
  }

  return *this;
}
ri_crequest_id_s& ricsubscription_resp_ies_o::value_c::ri_crequest_id()
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
uint16_t& ricsubscription_resp_ies_o::value_c::ra_nfunction_id()
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
ri_caction_admitted_list_l& ricsubscription_resp_ies_o::value_c::ri_cactions_admitted()
{
  assert_choice_type(types::ri_cactions_admitted, type_, "Value");
  return c.get<ri_caction_admitted_list_l>();
}
ri_caction_not_admitted_list_l& ricsubscription_resp_ies_o::value_c::ri_cactions_not_admitted()
{
  assert_choice_type(types::ri_cactions_not_admitted, type_, "Value");
  return c.get<ri_caction_not_admitted_list_l>();
}
const ri_crequest_id_s& ricsubscription_resp_ies_o::value_c::ri_crequest_id() const
{
  assert_choice_type(types::ri_crequest_id, type_, "Value");
  return c.get<ri_crequest_id_s>();
}
const uint16_t& ricsubscription_resp_ies_o::value_c::ra_nfunction_id() const
{
  assert_choice_type(types::ra_nfunction_id, type_, "Value");
  return c.get<uint16_t>();
}
const ri_caction_admitted_list_l& ricsubscription_resp_ies_o::value_c::ri_cactions_admitted() const
{
  assert_choice_type(types::ri_cactions_admitted, type_, "Value");
  return c.get<ri_caction_admitted_list_l>();
}
const ri_caction_not_admitted_list_l& ricsubscription_resp_ies_o::value_c::ri_cactions_not_admitted() const
{
  assert_choice_type(types::ri_cactions_not_admitted, type_, "Value");
  return c.get<ri_caction_not_admitted_list_l>();
}
void ricsubscription_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ri_crequest_id:
      j.write_fieldname("RICrequestID");
      c.get<ri_crequest_id_s>().to_json(j);
      break;
    case types::ra_nfunction_id:
      j.write_int("INTEGER (0..4095)", c.get<uint16_t>());
      break;
    case types::ri_cactions_admitted:
      j.start_array("RICaction-Admitted-List");
      for (const auto& e1 : c.get<ri_caction_admitted_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    case types::ri_cactions_not_admitted:
      j.start_array("RICaction-NotAdmitted-List");
      for (const auto& e1 : c.get<ri_caction_not_admitted_list_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE ricsubscription_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().pack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ri_cactions_admitted:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ri_caction_admitted_list_l>(), 1, 16, true));
      break;
    case types::ri_cactions_not_admitted:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<ri_caction_not_admitted_list_l>(), 0, 16, true));
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ricsubscription_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ri_crequest_id:
      HANDLE_CODE(c.get<ri_crequest_id_s>().unpack(bref));
      break;
    case types::ra_nfunction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)4095u, false, true));
      break;
    case types::ri_cactions_admitted:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ri_caction_admitted_list_l>(), bref, 1, 16, true));
      break;
    case types::ri_cactions_not_admitted:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<ri_caction_not_admitted_list_l>(), bref, 0, 16, true));
      break;
    default:
      log_invalid_choice_id(type_, "ricsubscription_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ricsubscription_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {
      "RICrequestID", "INTEGER (0..4095)", "RICaction-Admitted-List", "RICaction-NotAdmitted-List"};
  return convert_enum_idx(options, 4, value, "ricsubscription_resp_ies_o::value_c::types");
}
uint8_t ricsubscription_resp_ies_o::value_c::types_opts::to_number() const
{
  if (value == ra_nfunction_id) {
    return 0;
  }
  invalid_enum_number(value, "ricsubscription_resp_ies_o::value_c::types");
  return 0;
}

// ResetRequestIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t reset_request_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 1};
  return map_enum_number(options, 2, idx, "id");
}
bool reset_request_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 1};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e reset_request_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 1:
      return crit_e::ignore;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
reset_request_ies_o::value_c reset_request_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 1:
      ret.set(value_c::types::cause);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e reset_request_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 1:
      return presence_e::mandatory;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void reset_request_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::cause:
      c.destroy<cause_c>();
      break;
    default:
      break;
  }
}
void reset_request_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::cause:
      c.init<cause_c>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_request_ies_o::value_c");
  }
}
reset_request_ies_o::value_c::value_c(const reset_request_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.init(other.c.get<cause_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_request_ies_o::value_c");
  }
}
reset_request_ies_o::value_c& reset_request_ies_o::value_c::operator=(const reset_request_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::cause:
      c.set(other.c.get<cause_c>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_request_ies_o::value_c");
  }

  return *this;
}
uint16_t& reset_request_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
cause_c& reset_request_ies_o::value_c::cause()
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
const uint16_t& reset_request_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const cause_c& reset_request_ies_o::value_c::cause() const
{
  assert_choice_type(types::cause, type_, "Value");
  return c.get<cause_c>();
}
void reset_request_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::cause:
      j.write_fieldname("Cause");
      c.get<cause_c>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "reset_request_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE reset_request_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "reset_request_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE reset_request_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::cause:
      HANDLE_CODE(c.get<cause_c>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "reset_request_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* reset_request_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)", "Cause"};
  return convert_enum_idx(options, 2, value, "reset_request_ies_o::value_c::types");
}
uint8_t reset_request_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "reset_request_ies_o::value_c::types");
}

// ResetResponseIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
uint32_t reset_resp_ies_o::idx_to_id(uint32_t idx)
{
  static const uint32_t options[] = {49, 2};
  return map_enum_number(options, 2, idx, "id");
}
bool reset_resp_ies_o::is_id_valid(const uint32_t& id)
{
  static const uint32_t options[] = {49, 2};
  for (const auto& o : options) {
    if (o == id) {
      return true;
    }
  }
  return false;
}
crit_e reset_resp_ies_o::get_crit(const uint32_t& id)
{
  switch (id) {
    case 49:
      return crit_e::reject;
    case 2:
      return crit_e::ignore;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}
reset_resp_ies_o::value_c reset_resp_ies_o::get_value(const uint32_t& id)
{
  value_c ret{};
  switch (id) {
    case 49:
      ret.set(value_c::types::transaction_id);
      break;
    case 2:
      ret.set(value_c::types::crit_diagnostics);
      break;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return ret;
}
presence_e reset_resp_ies_o::get_presence(const uint32_t& id)
{
  switch (id) {
    case 49:
      return presence_e::mandatory;
    case 2:
      return presence_e::optional;
    default:
      asn1::log_error("The id=%d is not recognized", id);
  }
  return {};
}

// Value ::= OPEN TYPE
void reset_resp_ies_o::value_c::destroy_()
{
  switch (type_) {
    case types::crit_diagnostics:
      c.destroy<crit_diagnostics_s>();
      break;
    default:
      break;
  }
}
void reset_resp_ies_o::value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::transaction_id:
      break;
    case types::crit_diagnostics:
      c.init<crit_diagnostics_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_resp_ies_o::value_c");
  }
}
reset_resp_ies_o::value_c::value_c(const reset_resp_ies_o::value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::transaction_id:
      c.init(other.c.get<uint16_t>());
      break;
    case types::crit_diagnostics:
      c.init(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_resp_ies_o::value_c");
  }
}
reset_resp_ies_o::value_c& reset_resp_ies_o::value_c::operator=(const reset_resp_ies_o::value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::transaction_id:
      c.set(other.c.get<uint16_t>());
      break;
    case types::crit_diagnostics:
      c.set(other.c.get<crit_diagnostics_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "reset_resp_ies_o::value_c");
  }

  return *this;
}
uint16_t& reset_resp_ies_o::value_c::transaction_id()
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
crit_diagnostics_s& reset_resp_ies_o::value_c::crit_diagnostics()
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
const uint16_t& reset_resp_ies_o::value_c::transaction_id() const
{
  assert_choice_type(types::transaction_id, type_, "Value");
  return c.get<uint16_t>();
}
const crit_diagnostics_s& reset_resp_ies_o::value_c::crit_diagnostics() const
{
  assert_choice_type(types::crit_diagnostics, type_, "Value");
  return c.get<crit_diagnostics_s>();
}
void reset_resp_ies_o::value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::transaction_id:
      j.write_int("INTEGER (0..255,...)", c.get<uint16_t>());
      break;
    case types::crit_diagnostics:
      j.write_fieldname("CriticalityDiagnostics");
      c.get<crit_diagnostics_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "reset_resp_ies_o::value_c");
  }
  j.end_obj();
}
SRSASN_CODE reset_resp_ies_o::value_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "reset_resp_ies_o::value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE reset_resp_ies_o::value_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::transaction_id:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::crit_diagnostics:
      HANDLE_CODE(c.get<crit_diagnostics_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "reset_resp_ies_o::value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* reset_resp_ies_o::value_c::types_opts::to_string() const
{
  static const char* options[] = {"INTEGER (0..255,...)", "CriticalityDiagnostics"};
  return convert_enum_idx(options, 2, value, "reset_resp_ies_o::value_c::types");
}
uint8_t reset_resp_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t options[] = {0};
  return map_enum_number(options, 1, value, "reset_resp_ies_o::value_c::types");
}

template struct asn1::protocol_ie_field_s<e2_removal_fail_ies_o>;

e2_removal_fail_ies_container::e2_removal_fail_ies_container() :
  transaction_id(49, crit_e::reject), cause(1, crit_e::ignore), crit_diagnostics(2, crit_e::ignore)
{
}
SRSASN_CODE e2_removal_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_removal_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        cause.id = id;
        HANDLE_CODE(cause.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.value.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2_removal_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<e2_removal_resp_ies_o>;

e2_removal_resp_ies_container::e2_removal_resp_ies_container() :
  transaction_id(49, crit_e::reject), crit_diagnostics(2, crit_e::ignore)
{
}
SRSASN_CODE e2_removal_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_removal_resp_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2_removal_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<e2conn_upd_ies_o>;

e2conn_upd_ies_container::e2conn_upd_ies_container() :
  transaction_id(49, crit_e::reject),
  e2conn_upd_add(44, crit_e::reject),
  e2conn_upd_rem(46, crit_e::reject),
  e2conn_upd_modify(45, crit_e::reject)
{
}
SRSASN_CODE e2conn_upd_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += e2conn_upd_add_present ? 1 : 0;
  nof_ies += e2conn_upd_rem_present ? 1 : 0;
  nof_ies += e2conn_upd_modify_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  if (e2conn_upd_add_present) {
    HANDLE_CODE(e2conn_upd_add.pack(bref));
  }
  if (e2conn_upd_rem_present) {
    HANDLE_CODE(e2conn_upd_rem.pack(bref));
  }
  if (e2conn_upd_modify_present) {
    HANDLE_CODE(e2conn_upd_modify.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 44: {
        e2conn_upd_add_present = true;
        e2conn_upd_add.id      = id;
        HANDLE_CODE(e2conn_upd_add.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(e2conn_upd_add.value.unpack(bref));
        break;
      }
      case 46: {
        e2conn_upd_rem_present = true;
        e2conn_upd_rem.id      = id;
        HANDLE_CODE(e2conn_upd_rem.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(e2conn_upd_rem.value.unpack(bref));
        break;
      }
      case 45: {
        e2conn_upd_modify_present = true;
        e2conn_upd_modify.id      = id;
        HANDLE_CODE(e2conn_upd_modify.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(e2conn_upd_modify.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2conn_upd_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (e2conn_upd_add_present) {
    j.write_fieldname("");
    e2conn_upd_add.to_json(j);
  }
  if (e2conn_upd_rem_present) {
    j.write_fieldname("");
    e2conn_upd_rem.to_json(j);
  }
  if (e2conn_upd_modify_present) {
    j.write_fieldname("");
    e2conn_upd_modify.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<e2conn_upd_ack_ies_o>;

e2conn_upd_ack_ies_container::e2conn_upd_ack_ies_container() :
  transaction_id(49, crit_e::reject), e2conn_setup(39, crit_e::reject), e2conn_setup_failed(40, crit_e::reject)
{
}
SRSASN_CODE e2conn_upd_ack_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += e2conn_setup_present ? 1 : 0;
  nof_ies += e2conn_setup_failed_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  if (e2conn_setup_present) {
    HANDLE_CODE(e2conn_setup.pack(bref));
  }
  if (e2conn_setup_failed_present) {
    HANDLE_CODE(e2conn_setup_failed.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_ack_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 39: {
        e2conn_setup_present = true;
        e2conn_setup.id      = id;
        HANDLE_CODE(e2conn_setup.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(e2conn_setup.value.unpack(bref));
        break;
      }
      case 40: {
        e2conn_setup_failed_present = true;
        e2conn_setup_failed.id      = id;
        HANDLE_CODE(e2conn_setup_failed.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(e2conn_setup_failed.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2conn_upd_ack_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (e2conn_setup_present) {
    j.write_fieldname("");
    e2conn_setup.to_json(j);
  }
  if (e2conn_setup_failed_present) {
    j.write_fieldname("");
    e2conn_setup_failed.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<e2conn_upd_fail_ies_o>;

e2conn_upd_fail_ies_container::e2conn_upd_fail_ies_container() :
  transaction_id(49, crit_e::reject),
  cause(1, crit_e::reject),
  time_to_wait(31, crit_e::ignore),
  crit_diagnostics(2, crit_e::ignore)
{
}
SRSASN_CODE e2conn_upd_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += cause_present ? 1 : 0;
  nof_ies += time_to_wait_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  if (cause_present) {
    HANDLE_CODE(cause.pack(bref));
  }
  if (time_to_wait_present) {
    HANDLE_CODE(time_to_wait.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2conn_upd_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 1: {
        cause_present = true;
        cause.id      = id;
        HANDLE_CODE(cause.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.value.unpack(bref));
        break;
      }
      case 31: {
        time_to_wait_present = true;
        time_to_wait.id      = id;
        HANDLE_CODE(time_to_wait.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(time_to_wait.value.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2conn_upd_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (cause_present) {
    j.write_fieldname("");
    cause.to_json(j);
  }
  if (time_to_wait_present) {
    j.write_fieldname("");
    time_to_wait.to_json(j);
  }
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<e2node_cfg_upd_ies_o>;

e2node_cfg_upd_ies_container::e2node_cfg_upd_ies_container() :
  transaction_id(49, crit_e::reject),
  global_e2node_id(3, crit_e::reject),
  e2node_component_cfg_addition(50, crit_e::reject),
  e2node_component_cfg_upd(33, crit_e::reject),
  e2node_component_cfg_removal(54, crit_e::reject),
  e2node_tn_lassoc_removal(58, crit_e::reject)
{
}
SRSASN_CODE e2node_cfg_upd_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += global_e2node_id_present ? 1 : 0;
  nof_ies += e2node_component_cfg_addition_present ? 1 : 0;
  nof_ies += e2node_component_cfg_upd_present ? 1 : 0;
  nof_ies += e2node_component_cfg_removal_present ? 1 : 0;
  nof_ies += e2node_tn_lassoc_removal_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  if (global_e2node_id_present) {
    HANDLE_CODE(global_e2node_id.pack(bref));
  }
  if (e2node_component_cfg_addition_present) {
    HANDLE_CODE(e2node_component_cfg_addition.pack(bref));
  }
  if (e2node_component_cfg_upd_present) {
    HANDLE_CODE(e2node_component_cfg_upd.pack(bref));
  }
  if (e2node_component_cfg_removal_present) {
    HANDLE_CODE(e2node_component_cfg_removal.pack(bref));
  }
  if (e2node_tn_lassoc_removal_present) {
    HANDLE_CODE(e2node_tn_lassoc_removal.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_cfg_upd_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 3: {
        global_e2node_id_present = true;
        global_e2node_id.id      = id;
        HANDLE_CODE(global_e2node_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(global_e2node_id.value.unpack(bref));
        break;
      }
      case 50: {
        e2node_component_cfg_addition_present = true;
        e2node_component_cfg_addition.id      = id;
        HANDLE_CODE(e2node_component_cfg_addition.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(e2node_component_cfg_addition.value.unpack(bref));
        break;
      }
      case 33: {
        e2node_component_cfg_upd_present = true;
        e2node_component_cfg_upd.id      = id;
        HANDLE_CODE(e2node_component_cfg_upd.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(e2node_component_cfg_upd.value.unpack(bref));
        break;
      }
      case 54: {
        e2node_component_cfg_removal_present = true;
        e2node_component_cfg_removal.id      = id;
        HANDLE_CODE(e2node_component_cfg_removal.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(e2node_component_cfg_removal.value.unpack(bref));
        break;
      }
      case 58: {
        e2node_tn_lassoc_removal_present = true;
        e2node_tn_lassoc_removal.id      = id;
        HANDLE_CODE(e2node_tn_lassoc_removal.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(e2node_tn_lassoc_removal.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2node_cfg_upd_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (global_e2node_id_present) {
    j.write_fieldname("");
    global_e2node_id.to_json(j);
  }
  if (e2node_component_cfg_addition_present) {
    j.write_fieldname("");
    e2node_component_cfg_addition.to_json(j);
  }
  if (e2node_component_cfg_upd_present) {
    j.write_fieldname("");
    e2node_component_cfg_upd.to_json(j);
  }
  if (e2node_component_cfg_removal_present) {
    j.write_fieldname("");
    e2node_component_cfg_removal.to_json(j);
  }
  if (e2node_tn_lassoc_removal_present) {
    j.write_fieldname("");
    e2node_tn_lassoc_removal.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<e2node_cfg_upd_ack_ies_o>;

e2node_cfg_upd_ack_ies_container::e2node_cfg_upd_ack_ies_container() :
  transaction_id(49, crit_e::reject),
  e2node_component_cfg_addition_ack(52, crit_e::reject),
  e2node_component_cfg_upd_ack(35, crit_e::reject),
  e2node_component_cfg_removal_ack(56, crit_e::reject)
{
}
SRSASN_CODE e2node_cfg_upd_ack_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += e2node_component_cfg_addition_ack_present ? 1 : 0;
  nof_ies += e2node_component_cfg_upd_ack_present ? 1 : 0;
  nof_ies += e2node_component_cfg_removal_ack_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  if (e2node_component_cfg_addition_ack_present) {
    HANDLE_CODE(e2node_component_cfg_addition_ack.pack(bref));
  }
  if (e2node_component_cfg_upd_ack_present) {
    HANDLE_CODE(e2node_component_cfg_upd_ack.pack(bref));
  }
  if (e2node_component_cfg_removal_ack_present) {
    HANDLE_CODE(e2node_component_cfg_removal_ack.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_cfg_upd_ack_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 52: {
        e2node_component_cfg_addition_ack_present = true;
        e2node_component_cfg_addition_ack.id      = id;
        HANDLE_CODE(e2node_component_cfg_addition_ack.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(e2node_component_cfg_addition_ack.value.unpack(bref));
        break;
      }
      case 35: {
        e2node_component_cfg_upd_ack_present = true;
        e2node_component_cfg_upd_ack.id      = id;
        HANDLE_CODE(e2node_component_cfg_upd_ack.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(e2node_component_cfg_upd_ack.value.unpack(bref));
        break;
      }
      case 56: {
        e2node_component_cfg_removal_ack_present = true;
        e2node_component_cfg_removal_ack.id      = id;
        HANDLE_CODE(e2node_component_cfg_removal_ack.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(e2node_component_cfg_removal_ack.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2node_cfg_upd_ack_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (e2node_component_cfg_addition_ack_present) {
    j.write_fieldname("");
    e2node_component_cfg_addition_ack.to_json(j);
  }
  if (e2node_component_cfg_upd_ack_present) {
    j.write_fieldname("");
    e2node_component_cfg_upd_ack.to_json(j);
  }
  if (e2node_component_cfg_removal_ack_present) {
    j.write_fieldname("");
    e2node_component_cfg_removal_ack.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<e2node_cfg_upd_fail_ies_o>;

e2node_cfg_upd_fail_ies_container::e2node_cfg_upd_fail_ies_container() :
  transaction_id(49, crit_e::reject),
  cause(1, crit_e::ignore),
  time_to_wait(31, crit_e::ignore),
  crit_diagnostics(2, crit_e::ignore)
{
}
SRSASN_CODE e2node_cfg_upd_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += time_to_wait_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  if (time_to_wait_present) {
    HANDLE_CODE(time_to_wait.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2node_cfg_upd_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        cause.id = id;
        HANDLE_CODE(cause.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.value.unpack(bref));
        break;
      }
      case 31: {
        time_to_wait_present = true;
        time_to_wait.id      = id;
        HANDLE_CODE(time_to_wait.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(time_to_wait.value.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2node_cfg_upd_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  if (time_to_wait_present) {
    j.write_fieldname("");
    time_to_wait.to_json(j);
  }
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<e2setup_fail_ies_o>;

e2setup_fail_ies_container::e2setup_fail_ies_container() :
  transaction_id(49, crit_e::reject),
  cause(1, crit_e::ignore),
  time_to_wait(31, crit_e::ignore),
  crit_diagnostics(2, crit_e::ignore),
  tn_linfo(48, crit_e::ignore)
{
}
SRSASN_CODE e2setup_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += time_to_wait_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  nof_ies += tn_linfo_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  if (time_to_wait_present) {
    HANDLE_CODE(time_to_wait.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }
  if (tn_linfo_present) {
    HANDLE_CODE(tn_linfo.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2setup_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        cause.id = id;
        HANDLE_CODE(cause.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.value.unpack(bref));
        break;
      }
      case 31: {
        time_to_wait_present = true;
        time_to_wait.id      = id;
        HANDLE_CODE(time_to_wait.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(time_to_wait.value.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
        break;
      }
      case 48: {
        tn_linfo_present = true;
        tn_linfo.id      = id;
        HANDLE_CODE(tn_linfo.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(tn_linfo.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2setup_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  if (time_to_wait_present) {
    j.write_fieldname("");
    time_to_wait.to_json(j);
  }
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  if (tn_linfo_present) {
    j.write_fieldname("");
    tn_linfo.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<e2setup_request_ies_o>;

e2setup_request_ies_container::e2setup_request_ies_container() :
  transaction_id(49, crit_e::reject),
  global_e2node_id(3, crit_e::reject),
  ra_nfunctions_added(10, crit_e::reject),
  e2node_component_cfg_addition(50, crit_e::reject)
{
}
SRSASN_CODE e2setup_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 4;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  HANDLE_CODE(global_e2node_id.pack(bref));
  HANDLE_CODE(ra_nfunctions_added.pack(bref));
  HANDLE_CODE(e2node_component_cfg_addition.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2setup_request_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 4;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 3: {
        nof_mandatory_ies--;
        global_e2node_id.id = id;
        HANDLE_CODE(global_e2node_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(global_e2node_id.value.unpack(bref));
        break;
      }
      case 10: {
        nof_mandatory_ies--;
        ra_nfunctions_added.id = id;
        HANDLE_CODE(ra_nfunctions_added.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunctions_added.value.unpack(bref));
        break;
      }
      case 50: {
        nof_mandatory_ies--;
        e2node_component_cfg_addition.id = id;
        HANDLE_CODE(e2node_component_cfg_addition.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(e2node_component_cfg_addition.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2setup_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  j.write_fieldname("");
  global_e2node_id.to_json(j);
  j.write_fieldname("");
  ra_nfunctions_added.to_json(j);
  j.write_fieldname("");
  e2node_component_cfg_addition.to_json(j);
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<e2setup_resp_ies_o>;

e2setup_resp_ies_container::e2setup_resp_ies_container() :
  transaction_id(49, crit_e::reject),
  global_ric_id(4, crit_e::reject),
  ra_nfunctions_accepted(9, crit_e::reject),
  ra_nfunctions_rejected(13, crit_e::reject),
  e2node_component_cfg_addition_ack(52, crit_e::reject)
{
}
SRSASN_CODE e2setup_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += ra_nfunctions_accepted_present ? 1 : 0;
  nof_ies += ra_nfunctions_rejected_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  HANDLE_CODE(global_ric_id.pack(bref));
  if (ra_nfunctions_accepted_present) {
    HANDLE_CODE(ra_nfunctions_accepted.pack(bref));
  }
  if (ra_nfunctions_rejected_present) {
    HANDLE_CODE(ra_nfunctions_rejected.pack(bref));
  }
  HANDLE_CODE(e2node_component_cfg_addition_ack.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2setup_resp_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 4: {
        nof_mandatory_ies--;
        global_ric_id.id = id;
        HANDLE_CODE(global_ric_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(global_ric_id.value.unpack(bref));
        break;
      }
      case 9: {
        ra_nfunctions_accepted_present = true;
        ra_nfunctions_accepted.id      = id;
        HANDLE_CODE(ra_nfunctions_accepted.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunctions_accepted.value.unpack(bref));
        break;
      }
      case 13: {
        ra_nfunctions_rejected_present = true;
        ra_nfunctions_rejected.id      = id;
        HANDLE_CODE(ra_nfunctions_rejected.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunctions_rejected.value.unpack(bref));
        break;
      }
      case 52: {
        nof_mandatory_ies--;
        e2node_component_cfg_addition_ack.id = id;
        HANDLE_CODE(e2node_component_cfg_addition_ack.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(e2node_component_cfg_addition_ack.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void e2setup_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  j.write_fieldname("");
  global_ric_id.to_json(j);
  if (ra_nfunctions_accepted_present) {
    j.write_fieldname("");
    ra_nfunctions_accepted.to_json(j);
  }
  if (ra_nfunctions_rejected_present) {
    j.write_fieldname("");
    ra_nfunctions_rejected.to_json(j);
  }
  j.write_fieldname("");
  e2node_component_cfg_addition_ack.to_json(j);
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<error_ind_ies_o>;

error_ind_ies_container::error_ind_ies_container() :
  transaction_id(49, crit_e::reject),
  ri_crequest_id(29, crit_e::reject),
  ra_nfunction_id(5, crit_e::reject),
  cause(1, crit_e::ignore),
  crit_diagnostics(2, crit_e::ignore)
{
}
SRSASN_CODE error_ind_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 0;
  nof_ies += transaction_id_present ? 1 : 0;
  nof_ies += ri_crequest_id_present ? 1 : 0;
  nof_ies += ra_nfunction_id_present ? 1 : 0;
  nof_ies += cause_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  if (transaction_id_present) {
    HANDLE_CODE(transaction_id.pack(bref));
  }
  if (ri_crequest_id_present) {
    HANDLE_CODE(ri_crequest_id.pack(bref));
  }
  if (ra_nfunction_id_present) {
    HANDLE_CODE(ra_nfunction_id.pack(bref));
  }
  if (cause_present) {
    HANDLE_CODE(cause.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE error_ind_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        transaction_id_present = true;
        transaction_id.id      = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 29: {
        ri_crequest_id_present = true;
        ri_crequest_id.id      = id;
        HANDLE_CODE(ri_crequest_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_crequest_id.value.unpack(bref));
        break;
      }
      case 5: {
        ra_nfunction_id_present = true;
        ra_nfunction_id.id      = id;
        HANDLE_CODE(ra_nfunction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunction_id.value.unpack(bref));
        break;
      }
      case 1: {
        cause_present = true;
        cause.id      = id;
        HANDLE_CODE(cause.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.value.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }

  return SRSASN_SUCCESS;
}
void error_ind_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  if (transaction_id_present) {
    j.write_fieldname("");
    transaction_id.to_json(j);
  }
  if (ri_crequest_id_present) {
    j.write_fieldname("");
    ri_crequest_id.to_json(j);
  }
  if (ra_nfunction_id_present) {
    j.write_fieldname("");
    ra_nfunction_id.to_json(j);
  }
  if (cause_present) {
    j.write_fieldname("");
    cause.to_json(j);
  }
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ri_cctrl_ack_ies_o>;

ri_cctrl_ack_ies_container::ri_cctrl_ack_ies_container() :
  ri_crequest_id(29, crit_e::reject),
  ra_nfunction_id(5, crit_e::reject),
  ri_ccall_process_id(20, crit_e::reject),
  ri_cctrl_outcome(32, crit_e::reject)
{
}
SRSASN_CODE ri_cctrl_ack_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += ri_ccall_process_id_present ? 1 : 0;
  nof_ies += ri_cctrl_outcome_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(ri_crequest_id.pack(bref));
  HANDLE_CODE(ra_nfunction_id.pack(bref));
  if (ri_ccall_process_id_present) {
    HANDLE_CODE(ri_ccall_process_id.pack(bref));
  }
  if (ri_cctrl_outcome_present) {
    HANDLE_CODE(ri_cctrl_outcome.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ri_cctrl_ack_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        ri_crequest_id.id = id;
        HANDLE_CODE(ri_crequest_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_crequest_id.value.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        ra_nfunction_id.id = id;
        HANDLE_CODE(ra_nfunction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunction_id.value.unpack(bref));
        break;
      }
      case 20: {
        ri_ccall_process_id_present = true;
        ri_ccall_process_id.id      = id;
        HANDLE_CODE(ri_ccall_process_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_ccall_process_id.value.unpack(bref));
        break;
      }
      case 32: {
        ri_cctrl_outcome_present = true;
        ri_cctrl_outcome.id      = id;
        HANDLE_CODE(ri_cctrl_outcome.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_cctrl_outcome.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ri_cctrl_ack_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  ri_crequest_id.to_json(j);
  j.write_fieldname("");
  ra_nfunction_id.to_json(j);
  if (ri_ccall_process_id_present) {
    j.write_fieldname("");
    ri_ccall_process_id.to_json(j);
  }
  if (ri_cctrl_outcome_present) {
    j.write_fieldname("");
    ri_cctrl_outcome.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ri_cctrl_fail_ies_o>;

ri_cctrl_fail_ies_container::ri_cctrl_fail_ies_container() :
  ri_crequest_id(29, crit_e::reject),
  ra_nfunction_id(5, crit_e::reject),
  ri_ccall_process_id(20, crit_e::reject),
  cause(1, crit_e::ignore),
  ri_cctrl_outcome(32, crit_e::reject)
{
}
SRSASN_CODE ri_cctrl_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += ri_ccall_process_id_present ? 1 : 0;
  nof_ies += ri_cctrl_outcome_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(ri_crequest_id.pack(bref));
  HANDLE_CODE(ra_nfunction_id.pack(bref));
  if (ri_ccall_process_id_present) {
    HANDLE_CODE(ri_ccall_process_id.pack(bref));
  }
  HANDLE_CODE(cause.pack(bref));
  if (ri_cctrl_outcome_present) {
    HANDLE_CODE(ri_cctrl_outcome.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ri_cctrl_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        ri_crequest_id.id = id;
        HANDLE_CODE(ri_crequest_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_crequest_id.value.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        ra_nfunction_id.id = id;
        HANDLE_CODE(ra_nfunction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunction_id.value.unpack(bref));
        break;
      }
      case 20: {
        ri_ccall_process_id_present = true;
        ri_ccall_process_id.id      = id;
        HANDLE_CODE(ri_ccall_process_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_ccall_process_id.value.unpack(bref));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        cause.id = id;
        HANDLE_CODE(cause.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.value.unpack(bref));
        break;
      }
      case 32: {
        ri_cctrl_outcome_present = true;
        ri_cctrl_outcome.id      = id;
        HANDLE_CODE(ri_cctrl_outcome.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_cctrl_outcome.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ri_cctrl_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  ri_crequest_id.to_json(j);
  j.write_fieldname("");
  ra_nfunction_id.to_json(j);
  if (ri_ccall_process_id_present) {
    j.write_fieldname("");
    ri_ccall_process_id.to_json(j);
  }
  j.write_fieldname("");
  cause.to_json(j);
  if (ri_cctrl_outcome_present) {
    j.write_fieldname("");
    ri_cctrl_outcome.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ri_cctrl_request_ies_o>;

ri_cctrl_request_ies_container::ri_cctrl_request_ies_container() :
  ri_crequest_id(29, crit_e::reject),
  ra_nfunction_id(5, crit_e::reject),
  ri_ccall_process_id(20, crit_e::reject),
  ri_cctrl_hdr(22, crit_e::reject),
  ri_cctrl_msg(23, crit_e::reject),
  ri_cctrl_ack_request(21, crit_e::reject)
{
}
SRSASN_CODE ri_cctrl_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 4;
  nof_ies += ri_ccall_process_id_present ? 1 : 0;
  nof_ies += ri_cctrl_ack_request_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(ri_crequest_id.pack(bref));
  HANDLE_CODE(ra_nfunction_id.pack(bref));
  if (ri_ccall_process_id_present) {
    HANDLE_CODE(ri_ccall_process_id.pack(bref));
  }
  HANDLE_CODE(ri_cctrl_hdr.pack(bref));
  HANDLE_CODE(ri_cctrl_msg.pack(bref));
  if (ri_cctrl_ack_request_present) {
    HANDLE_CODE(ri_cctrl_ack_request.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ri_cctrl_request_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 4;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        ri_crequest_id.id = id;
        HANDLE_CODE(ri_crequest_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_crequest_id.value.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        ra_nfunction_id.id = id;
        HANDLE_CODE(ra_nfunction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunction_id.value.unpack(bref));
        break;
      }
      case 20: {
        ri_ccall_process_id_present = true;
        ri_ccall_process_id.id      = id;
        HANDLE_CODE(ri_ccall_process_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_ccall_process_id.value.unpack(bref));
        break;
      }
      case 22: {
        nof_mandatory_ies--;
        ri_cctrl_hdr.id = id;
        HANDLE_CODE(ri_cctrl_hdr.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_cctrl_hdr.value.unpack(bref));
        break;
      }
      case 23: {
        nof_mandatory_ies--;
        ri_cctrl_msg.id = id;
        HANDLE_CODE(ri_cctrl_msg.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_cctrl_msg.value.unpack(bref));
        break;
      }
      case 21: {
        ri_cctrl_ack_request_present = true;
        ri_cctrl_ack_request.id      = id;
        HANDLE_CODE(ri_cctrl_ack_request.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_cctrl_ack_request.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ri_cctrl_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  ri_crequest_id.to_json(j);
  j.write_fieldname("");
  ra_nfunction_id.to_json(j);
  if (ri_ccall_process_id_present) {
    j.write_fieldname("");
    ri_ccall_process_id.to_json(j);
  }
  j.write_fieldname("");
  ri_cctrl_hdr.to_json(j);
  j.write_fieldname("");
  ri_cctrl_msg.to_json(j);
  if (ri_cctrl_ack_request_present) {
    j.write_fieldname("");
    ri_cctrl_ack_request.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ri_cind_ies_o>;

ri_cind_ies_container::ri_cind_ies_container() :
  ri_crequest_id(29, crit_e::reject),
  ra_nfunction_id(5, crit_e::reject),
  ri_caction_id(15, crit_e::reject),
  ri_cind_sn(27, crit_e::reject),
  ri_cind_type(28, crit_e::reject),
  ri_cind_hdr(25, crit_e::reject),
  ri_cind_msg(26, crit_e::reject),
  ri_ccall_process_id(20, crit_e::reject)
{
}
SRSASN_CODE ri_cind_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 6;
  nof_ies += ri_cind_sn_present ? 1 : 0;
  nof_ies += ri_ccall_process_id_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(ri_crequest_id.pack(bref));
  HANDLE_CODE(ra_nfunction_id.pack(bref));
  HANDLE_CODE(ri_caction_id.pack(bref));
  if (ri_cind_sn_present) {
    HANDLE_CODE(ri_cind_sn.pack(bref));
  }
  HANDLE_CODE(ri_cind_type.pack(bref));
  HANDLE_CODE(ri_cind_hdr.pack(bref));
  HANDLE_CODE(ri_cind_msg.pack(bref));
  if (ri_ccall_process_id_present) {
    HANDLE_CODE(ri_ccall_process_id.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ri_cind_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 6;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        ri_crequest_id.id = id;
        HANDLE_CODE(ri_crequest_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_crequest_id.value.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        ra_nfunction_id.id = id;
        HANDLE_CODE(ra_nfunction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunction_id.value.unpack(bref));
        break;
      }
      case 15: {
        nof_mandatory_ies--;
        ri_caction_id.id = id;
        HANDLE_CODE(ri_caction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_caction_id.value.unpack(bref));
        break;
      }
      case 27: {
        ri_cind_sn_present = true;
        ri_cind_sn.id      = id;
        HANDLE_CODE(ri_cind_sn.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_cind_sn.value.unpack(bref));
        break;
      }
      case 28: {
        nof_mandatory_ies--;
        ri_cind_type.id = id;
        HANDLE_CODE(ri_cind_type.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_cind_type.value.unpack(bref));
        break;
      }
      case 25: {
        nof_mandatory_ies--;
        ri_cind_hdr.id = id;
        HANDLE_CODE(ri_cind_hdr.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_cind_hdr.value.unpack(bref));
        break;
      }
      case 26: {
        nof_mandatory_ies--;
        ri_cind_msg.id = id;
        HANDLE_CODE(ri_cind_msg.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_cind_msg.value.unpack(bref));
        break;
      }
      case 20: {
        ri_ccall_process_id_present = true;
        ri_ccall_process_id.id      = id;
        HANDLE_CODE(ri_ccall_process_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_ccall_process_id.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ri_cind_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  ri_crequest_id.to_json(j);
  j.write_fieldname("");
  ra_nfunction_id.to_json(j);
  j.write_fieldname("");
  ri_caction_id.to_json(j);
  if (ri_cind_sn_present) {
    j.write_fieldname("");
    ri_cind_sn.to_json(j);
  }
  j.write_fieldname("");
  ri_cind_type.to_json(j);
  j.write_fieldname("");
  ri_cind_hdr.to_json(j);
  j.write_fieldname("");
  ri_cind_msg.to_json(j);
  if (ri_ccall_process_id_present) {
    j.write_fieldname("");
    ri_ccall_process_id.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ricservice_query_ies_o>;

ricservice_query_ies_container::ricservice_query_ies_container() :
  transaction_id(49, crit_e::reject), ra_nfunctions_accepted(9, crit_e::reject)
{
}
SRSASN_CODE ricservice_query_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += ra_nfunctions_accepted_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  if (ra_nfunctions_accepted_present) {
    HANDLE_CODE(ra_nfunctions_accepted.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ricservice_query_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 9: {
        ra_nfunctions_accepted_present = true;
        ra_nfunctions_accepted.id      = id;
        HANDLE_CODE(ra_nfunctions_accepted.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunctions_accepted.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ricservice_query_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (ra_nfunctions_accepted_present) {
    j.write_fieldname("");
    ra_nfunctions_accepted.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ricservice_upd_ies_o>;

ricservice_upd_ies_container::ricservice_upd_ies_container() :
  transaction_id(49, crit_e::reject),
  ra_nfunctions_added(10, crit_e::reject),
  ra_nfunctions_modified(12, crit_e::reject),
  ra_nfunctions_deleted(11, crit_e::reject)
{
}
SRSASN_CODE ricservice_upd_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += ra_nfunctions_added_present ? 1 : 0;
  nof_ies += ra_nfunctions_modified_present ? 1 : 0;
  nof_ies += ra_nfunctions_deleted_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  if (ra_nfunctions_added_present) {
    HANDLE_CODE(ra_nfunctions_added.pack(bref));
  }
  if (ra_nfunctions_modified_present) {
    HANDLE_CODE(ra_nfunctions_modified.pack(bref));
  }
  if (ra_nfunctions_deleted_present) {
    HANDLE_CODE(ra_nfunctions_deleted.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ricservice_upd_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 10: {
        ra_nfunctions_added_present = true;
        ra_nfunctions_added.id      = id;
        HANDLE_CODE(ra_nfunctions_added.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunctions_added.value.unpack(bref));
        break;
      }
      case 12: {
        ra_nfunctions_modified_present = true;
        ra_nfunctions_modified.id      = id;
        HANDLE_CODE(ra_nfunctions_modified.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunctions_modified.value.unpack(bref));
        break;
      }
      case 11: {
        ra_nfunctions_deleted_present = true;
        ra_nfunctions_deleted.id      = id;
        HANDLE_CODE(ra_nfunctions_deleted.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunctions_deleted.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ricservice_upd_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (ra_nfunctions_added_present) {
    j.write_fieldname("");
    ra_nfunctions_added.to_json(j);
  }
  if (ra_nfunctions_modified_present) {
    j.write_fieldname("");
    ra_nfunctions_modified.to_json(j);
  }
  if (ra_nfunctions_deleted_present) {
    j.write_fieldname("");
    ra_nfunctions_deleted.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ricservice_upd_ack_ies_o>;

ricservice_upd_ack_ies_container::ricservice_upd_ack_ies_container() :
  transaction_id(49, crit_e::reject),
  ra_nfunctions_accepted(9, crit_e::reject),
  ra_nfunctions_rejected(13, crit_e::reject)
{
}
SRSASN_CODE ricservice_upd_ack_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += ra_nfunctions_accepted_present ? 1 : 0;
  nof_ies += ra_nfunctions_rejected_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  if (ra_nfunctions_accepted_present) {
    HANDLE_CODE(ra_nfunctions_accepted.pack(bref));
  }
  if (ra_nfunctions_rejected_present) {
    HANDLE_CODE(ra_nfunctions_rejected.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ricservice_upd_ack_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 9: {
        ra_nfunctions_accepted_present = true;
        ra_nfunctions_accepted.id      = id;
        HANDLE_CODE(ra_nfunctions_accepted.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunctions_accepted.value.unpack(bref));
        break;
      }
      case 13: {
        ra_nfunctions_rejected_present = true;
        ra_nfunctions_rejected.id      = id;
        HANDLE_CODE(ra_nfunctions_rejected.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunctions_rejected.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ricservice_upd_ack_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (ra_nfunctions_accepted_present) {
    j.write_fieldname("");
    ra_nfunctions_accepted.to_json(j);
  }
  if (ra_nfunctions_rejected_present) {
    j.write_fieldname("");
    ra_nfunctions_rejected.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ricservice_upd_fail_ies_o>;

ricservice_upd_fail_ies_container::ricservice_upd_fail_ies_container() :
  transaction_id(49, crit_e::reject),
  cause(1, crit_e::reject),
  time_to_wait(31, crit_e::ignore),
  crit_diagnostics(2, crit_e::ignore)
{
}
SRSASN_CODE ricservice_upd_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  nof_ies += time_to_wait_present ? 1 : 0;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  if (time_to_wait_present) {
    HANDLE_CODE(time_to_wait.pack(bref));
  }
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ricservice_upd_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        cause.id = id;
        HANDLE_CODE(cause.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.value.unpack(bref));
        break;
      }
      case 31: {
        time_to_wait_present = true;
        time_to_wait.id      = id;
        HANDLE_CODE(time_to_wait.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(time_to_wait.value.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ricservice_upd_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  if (time_to_wait_present) {
    j.write_fieldname("");
    time_to_wait.to_json(j);
  }
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ricsubscription_delete_fail_ies_o>;

ricsubscription_delete_fail_ies_container::ricsubscription_delete_fail_ies_container() :
  ri_crequest_id(29, crit_e::reject),
  ra_nfunction_id(5, crit_e::reject),
  cause(1, crit_e::ignore),
  crit_diagnostics(2, crit_e::ignore)
{
}
SRSASN_CODE ricsubscription_delete_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(ri_crequest_id.pack(bref));
  HANDLE_CODE(ra_nfunction_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ricsubscription_delete_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        ri_crequest_id.id = id;
        HANDLE_CODE(ri_crequest_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_crequest_id.value.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        ra_nfunction_id.id = id;
        HANDLE_CODE(ra_nfunction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunction_id.value.unpack(bref));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        cause.id = id;
        HANDLE_CODE(cause.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.value.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ricsubscription_delete_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  ri_crequest_id.to_json(j);
  j.write_fieldname("");
  ra_nfunction_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ricsubscription_delete_request_ies_o>;

ricsubscription_delete_request_ies_container::ricsubscription_delete_request_ies_container() :
  ri_crequest_id(29, crit_e::reject), ra_nfunction_id(5, crit_e::reject)
{
}
SRSASN_CODE ricsubscription_delete_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(ri_crequest_id.pack(bref));
  HANDLE_CODE(ra_nfunction_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ricsubscription_delete_request_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        ri_crequest_id.id = id;
        HANDLE_CODE(ri_crequest_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_crequest_id.value.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        ra_nfunction_id.id = id;
        HANDLE_CODE(ra_nfunction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunction_id.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ricsubscription_delete_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  ri_crequest_id.to_json(j);
  j.write_fieldname("");
  ra_nfunction_id.to_json(j);
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ricsubscription_delete_resp_ies_o>;

ricsubscription_delete_resp_ies_container::ricsubscription_delete_resp_ies_container() :
  ri_crequest_id(29, crit_e::reject), ra_nfunction_id(5, crit_e::reject)
{
}
SRSASN_CODE ricsubscription_delete_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(ri_crequest_id.pack(bref));
  HANDLE_CODE(ra_nfunction_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ricsubscription_delete_resp_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        ri_crequest_id.id = id;
        HANDLE_CODE(ri_crequest_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_crequest_id.value.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        ra_nfunction_id.id = id;
        HANDLE_CODE(ra_nfunction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunction_id.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ricsubscription_delete_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  ri_crequest_id.to_json(j);
  j.write_fieldname("");
  ra_nfunction_id.to_json(j);
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ricsubscription_fail_ies_o>;

ricsubscription_fail_ies_container::ricsubscription_fail_ies_container() :
  ri_crequest_id(29, crit_e::reject),
  ra_nfunction_id(5, crit_e::reject),
  cause(1, crit_e::reject),
  crit_diagnostics(2, crit_e::ignore)
{
}
SRSASN_CODE ricsubscription_fail_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(ri_crequest_id.pack(bref));
  HANDLE_CODE(ra_nfunction_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ricsubscription_fail_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        ri_crequest_id.id = id;
        HANDLE_CODE(ri_crequest_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_crequest_id.value.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        ra_nfunction_id.id = id;
        HANDLE_CODE(ra_nfunction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunction_id.value.unpack(bref));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        cause.id = id;
        HANDLE_CODE(cause.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.value.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ricsubscription_fail_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  ri_crequest_id.to_json(j);
  j.write_fieldname("");
  ra_nfunction_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ricsubscription_request_ies_o>;

ricsubscription_request_ies_container::ricsubscription_request_ies_container() :
  ri_crequest_id(29, crit_e::reject), ra_nfunction_id(5, crit_e::reject), ricsubscription_details(30, crit_e::reject)
{
}
SRSASN_CODE ricsubscription_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(ri_crequest_id.pack(bref));
  HANDLE_CODE(ra_nfunction_id.pack(bref));
  HANDLE_CODE(ricsubscription_details.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ricsubscription_request_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        ri_crequest_id.id = id;
        HANDLE_CODE(ri_crequest_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_crequest_id.value.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        ra_nfunction_id.id = id;
        HANDLE_CODE(ra_nfunction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunction_id.value.unpack(bref));
        break;
      }
      case 30: {
        nof_mandatory_ies--;
        ricsubscription_details.id = id;
        HANDLE_CODE(ricsubscription_details.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ricsubscription_details.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ricsubscription_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  ri_crequest_id.to_json(j);
  j.write_fieldname("");
  ra_nfunction_id.to_json(j);
  j.write_fieldname("");
  ricsubscription_details.to_json(j);
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<ricsubscription_resp_ies_o>;

ricsubscription_resp_ies_container::ricsubscription_resp_ies_container() :
  ri_crequest_id(29, crit_e::reject),
  ra_nfunction_id(5, crit_e::reject),
  ri_cactions_admitted(17, crit_e::reject),
  ri_cactions_not_admitted(18, crit_e::reject)
{
}
SRSASN_CODE ricsubscription_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 3;
  nof_ies += ri_cactions_not_admitted_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(ri_crequest_id.pack(bref));
  HANDLE_CODE(ra_nfunction_id.pack(bref));
  HANDLE_CODE(ri_cactions_admitted.pack(bref));
  if (ri_cactions_not_admitted_present) {
    HANDLE_CODE(ri_cactions_not_admitted.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ricsubscription_resp_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 3;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 29: {
        nof_mandatory_ies--;
        ri_crequest_id.id = id;
        HANDLE_CODE(ri_crequest_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_crequest_id.value.unpack(bref));
        break;
      }
      case 5: {
        nof_mandatory_ies--;
        ra_nfunction_id.id = id;
        HANDLE_CODE(ra_nfunction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ra_nfunction_id.value.unpack(bref));
        break;
      }
      case 17: {
        nof_mandatory_ies--;
        ri_cactions_admitted.id = id;
        HANDLE_CODE(ri_cactions_admitted.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_cactions_admitted.value.unpack(bref));
        break;
      }
      case 18: {
        ri_cactions_not_admitted_present = true;
        ri_cactions_not_admitted.id      = id;
        HANDLE_CODE(ri_cactions_not_admitted.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(ri_cactions_not_admitted.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void ricsubscription_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  ri_crequest_id.to_json(j);
  j.write_fieldname("");
  ra_nfunction_id.to_json(j);
  j.write_fieldname("");
  ri_cactions_admitted.to_json(j);
  if (ri_cactions_not_admitted_present) {
    j.write_fieldname("");
    ri_cactions_not_admitted.to_json(j);
  }
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<reset_request_ies_o>;

reset_request_ies_container::reset_request_ies_container() :
  transaction_id(49, crit_e::reject), cause(1, crit_e::ignore)
{
}
SRSASN_CODE reset_request_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 2;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  HANDLE_CODE(cause.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE reset_request_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 2;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 1: {
        nof_mandatory_ies--;
        cause.id = id;
        HANDLE_CODE(cause.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(cause.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void reset_request_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  j.write_fieldname("");
  cause.to_json(j);
  j.end_obj();
}

template struct asn1::protocol_ie_field_s<reset_resp_ies_o>;

reset_resp_ies_container::reset_resp_ies_container() :
  transaction_id(49, crit_e::reject), crit_diagnostics(2, crit_e::ignore)
{
}
SRSASN_CODE reset_resp_ies_container::pack(bit_ref& bref) const
{
  uint32_t nof_ies = 1;
  nof_ies += crit_diagnostics_present ? 1 : 0;
  pack_length(bref, nof_ies, 0u, 65535u, true);

  HANDLE_CODE(transaction_id.pack(bref));
  if (crit_diagnostics_present) {
    HANDLE_CODE(crit_diagnostics.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE reset_resp_ies_container::unpack(cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  unpack_length(nof_ies, bref, 0u, 65535u, true);

  uint32_t nof_mandatory_ies = 1;

  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    switch (id) {
      case 49: {
        nof_mandatory_ies--;
        transaction_id.id = id;
        HANDLE_CODE(transaction_id.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(transaction_id.value.unpack(bref));
        break;
      }
      case 2: {
        crit_diagnostics_present = true;
        crit_diagnostics.id      = id;
        HANDLE_CODE(crit_diagnostics.crit.unpack(bref));
        varlength_field_unpack_guard varlen_scope(bref, true);
        HANDLE_CODE(crit_diagnostics.value.unpack(bref));
        break;
      }
      default:
        asn1::log_error("Unpacked object ID=%d is not recognized\n", id);
        return SRSASN_ERROR_DECODE_FAIL;
    }
  }
  if (nof_mandatory_ies > 0) {
    asn1::log_error("Mandatory fields are missing\n");

    return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
void reset_resp_ies_container::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("");
  transaction_id.to_json(j);
  if (crit_diagnostics_present) {
    j.write_fieldname("");
    crit_diagnostics.to_json(j);
  }
  j.end_obj();
}

// E2AP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF E2AP-ELEMENTARY-PROCEDURE
uint16_t e2_ap_elem_procs_o::idx_to_proc_code(uint32_t idx)
{
  static const uint16_t options[] = {8, 9, 7, 4, 1, 10, 11, 3, 13, 5, 6, 2, 12};
  return map_enum_number(options, 13, idx, "proc_code");
}
bool e2_ap_elem_procs_o::is_proc_code_valid(const uint16_t& proc_code)
{
  static const uint16_t options[] = {8, 9, 7, 4, 1, 10, 11, 3, 13, 5, 6, 2, 12};
  for (const auto& o : options) {
    if (o == proc_code) {
      return true;
    }
  }
  return false;
}
e2_ap_elem_procs_o::init_msg_c e2_ap_elem_procs_o::get_init_msg(const uint16_t& proc_code)
{
  init_msg_c ret{};
  switch (proc_code) {
    case 8:
      ret.set(init_msg_c::types::ricsubscription_request);
      break;
    case 9:
      ret.set(init_msg_c::types::ricsubscription_delete_request);
      break;
    case 7:
      ret.set(init_msg_c::types::ricservice_upd);
      break;
    case 4:
      ret.set(init_msg_c::types::ri_cctrl_request);
      break;
    case 1:
      ret.set(init_msg_c::types::e2setup_request);
      break;
    case 10:
      ret.set(init_msg_c::types::e2node_cfg_upd);
      break;
    case 11:
      ret.set(init_msg_c::types::e2conn_upd);
      break;
    case 3:
      ret.set(init_msg_c::types::reset_request);
      break;
    case 13:
      ret.set(init_msg_c::types::e2_removal_request);
      break;
    case 5:
      ret.set(init_msg_c::types::ri_cind);
      break;
    case 6:
      ret.set(init_msg_c::types::ricservice_query);
      break;
    case 2:
      ret.set(init_msg_c::types::error_ind);
      break;
    case 12:
      ret.set(init_msg_c::types::ricsubscription_delete_required);
      break;
    default:
      asn1::log_error("The proc_code=%d is not recognized", proc_code);
  }
  return ret;
}
e2_ap_elem_procs_o::successful_outcome_c e2_ap_elem_procs_o::get_successful_outcome(const uint16_t& proc_code)
{
  successful_outcome_c ret{};
  switch (proc_code) {
    case 8:
      ret.set(successful_outcome_c::types::ricsubscription_resp);
      break;
    case 9:
      ret.set(successful_outcome_c::types::ricsubscription_delete_resp);
      break;
    case 7:
      ret.set(successful_outcome_c::types::ricservice_upd_ack);
      break;
    case 4:
      ret.set(successful_outcome_c::types::ri_cctrl_ack);
      break;
    case 1:
      ret.set(successful_outcome_c::types::e2setup_resp);
      break;
    case 10:
      ret.set(successful_outcome_c::types::e2node_cfg_upd_ack);
      break;
    case 11:
      ret.set(successful_outcome_c::types::e2conn_upd_ack);
      break;
    case 3:
      ret.set(successful_outcome_c::types::reset_resp);
      break;
    case 13:
      ret.set(successful_outcome_c::types::e2_removal_resp);
      break;
    default:
      asn1::log_error("The proc_code=%d is not recognized", proc_code);
  }
  return ret;
}
e2_ap_elem_procs_o::unsuccessful_outcome_c e2_ap_elem_procs_o::get_unsuccessful_outcome(const uint16_t& proc_code)
{
  unsuccessful_outcome_c ret{};
  switch (proc_code) {
    case 8:
      ret.set(unsuccessful_outcome_c::types::ricsubscription_fail);
      break;
    case 9:
      ret.set(unsuccessful_outcome_c::types::ricsubscription_delete_fail);
      break;
    case 7:
      ret.set(unsuccessful_outcome_c::types::ricservice_upd_fail);
      break;
    case 4:
      ret.set(unsuccessful_outcome_c::types::ri_cctrl_fail);
      break;
    case 1:
      ret.set(unsuccessful_outcome_c::types::e2setup_fail);
      break;
    case 10:
      ret.set(unsuccessful_outcome_c::types::e2node_cfg_upd_fail);
      break;
    case 11:
      ret.set(unsuccessful_outcome_c::types::e2conn_upd_fail);
      break;
    case 13:
      ret.set(unsuccessful_outcome_c::types::e2_removal_fail);
      break;
    default:
      asn1::log_error("The proc_code=%d is not recognized", proc_code);
  }
  return ret;
}
crit_e e2_ap_elem_procs_o::get_crit(const uint16_t& proc_code)
{
  switch (proc_code) {
    case 8:
      return crit_e::reject;
    case 9:
      return crit_e::reject;
    case 7:
      return crit_e::reject;
    case 4:
      return crit_e::reject;
    case 1:
      return crit_e::reject;
    case 10:
      return crit_e::reject;
    case 11:
      return crit_e::reject;
    case 3:
      return crit_e::reject;
    case 13:
      return crit_e::reject;
    case 5:
      return crit_e::ignore;
    case 6:
      return crit_e::ignore;
    case 2:
      return crit_e::ignore;
    case 12:
      return crit_e::ignore;
    default:
      asn1::log_error("The proc_code=%d is not recognized", proc_code);
  }
  return {};
}

// InitiatingMessage ::= OPEN TYPE
void e2_ap_elem_procs_o::init_msg_c::destroy_()
{
  switch (type_) {
    case types::ricsubscription_request:
      c.destroy<ricsubscription_request_s>();
      break;
    case types::ricsubscription_delete_request:
      c.destroy<ricsubscription_delete_request_s>();
      break;
    case types::ricservice_upd:
      c.destroy<ricservice_upd_s>();
      break;
    case types::ri_cctrl_request:
      c.destroy<ri_cctrl_request_s>();
      break;
    case types::e2setup_request:
      c.destroy<e2setup_request_s>();
      break;
    case types::e2node_cfg_upd:
      c.destroy<e2node_cfg_upd_s>();
      break;
    case types::e2conn_upd:
      c.destroy<e2conn_upd_s>();
      break;
    case types::reset_request:
      c.destroy<reset_request_s>();
      break;
    case types::e2_removal_request:
      c.destroy<e2_removal_request_s>();
      break;
    case types::ri_cind:
      c.destroy<ri_cind_s>();
      break;
    case types::ricservice_query:
      c.destroy<ricservice_query_s>();
      break;
    case types::error_ind:
      c.destroy<error_ind_s>();
      break;
    case types::ricsubscription_delete_required:
      c.destroy<ricsubscription_delete_required_s>();
      break;
    default:
      break;
  }
}
void e2_ap_elem_procs_o::init_msg_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ricsubscription_request:
      c.init<ricsubscription_request_s>();
      break;
    case types::ricsubscription_delete_request:
      c.init<ricsubscription_delete_request_s>();
      break;
    case types::ricservice_upd:
      c.init<ricservice_upd_s>();
      break;
    case types::ri_cctrl_request:
      c.init<ri_cctrl_request_s>();
      break;
    case types::e2setup_request:
      c.init<e2setup_request_s>();
      break;
    case types::e2node_cfg_upd:
      c.init<e2node_cfg_upd_s>();
      break;
    case types::e2conn_upd:
      c.init<e2conn_upd_s>();
      break;
    case types::reset_request:
      c.init<reset_request_s>();
      break;
    case types::e2_removal_request:
      c.init<e2_removal_request_s>();
      break;
    case types::ri_cind:
      c.init<ri_cind_s>();
      break;
    case types::ricservice_query:
      c.init<ricservice_query_s>();
      break;
    case types::error_ind:
      c.init<error_ind_s>();
      break;
    case types::ricsubscription_delete_required:
      c.init<ricsubscription_delete_required_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::init_msg_c");
  }
}
e2_ap_elem_procs_o::init_msg_c::init_msg_c(const e2_ap_elem_procs_o::init_msg_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ricsubscription_request:
      c.init(other.c.get<ricsubscription_request_s>());
      break;
    case types::ricsubscription_delete_request:
      c.init(other.c.get<ricsubscription_delete_request_s>());
      break;
    case types::ricservice_upd:
      c.init(other.c.get<ricservice_upd_s>());
      break;
    case types::ri_cctrl_request:
      c.init(other.c.get<ri_cctrl_request_s>());
      break;
    case types::e2setup_request:
      c.init(other.c.get<e2setup_request_s>());
      break;
    case types::e2node_cfg_upd:
      c.init(other.c.get<e2node_cfg_upd_s>());
      break;
    case types::e2conn_upd:
      c.init(other.c.get<e2conn_upd_s>());
      break;
    case types::reset_request:
      c.init(other.c.get<reset_request_s>());
      break;
    case types::e2_removal_request:
      c.init(other.c.get<e2_removal_request_s>());
      break;
    case types::ri_cind:
      c.init(other.c.get<ri_cind_s>());
      break;
    case types::ricservice_query:
      c.init(other.c.get<ricservice_query_s>());
      break;
    case types::error_ind:
      c.init(other.c.get<error_ind_s>());
      break;
    case types::ricsubscription_delete_required:
      c.init(other.c.get<ricsubscription_delete_required_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::init_msg_c");
  }
}
e2_ap_elem_procs_o::init_msg_c& e2_ap_elem_procs_o::init_msg_c::operator=(const e2_ap_elem_procs_o::init_msg_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ricsubscription_request:
      c.set(other.c.get<ricsubscription_request_s>());
      break;
    case types::ricsubscription_delete_request:
      c.set(other.c.get<ricsubscription_delete_request_s>());
      break;
    case types::ricservice_upd:
      c.set(other.c.get<ricservice_upd_s>());
      break;
    case types::ri_cctrl_request:
      c.set(other.c.get<ri_cctrl_request_s>());
      break;
    case types::e2setup_request:
      c.set(other.c.get<e2setup_request_s>());
      break;
    case types::e2node_cfg_upd:
      c.set(other.c.get<e2node_cfg_upd_s>());
      break;
    case types::e2conn_upd:
      c.set(other.c.get<e2conn_upd_s>());
      break;
    case types::reset_request:
      c.set(other.c.get<reset_request_s>());
      break;
    case types::e2_removal_request:
      c.set(other.c.get<e2_removal_request_s>());
      break;
    case types::ri_cind:
      c.set(other.c.get<ri_cind_s>());
      break;
    case types::ricservice_query:
      c.set(other.c.get<ricservice_query_s>());
      break;
    case types::error_ind:
      c.set(other.c.get<error_ind_s>());
      break;
    case types::ricsubscription_delete_required:
      c.set(other.c.get<ricsubscription_delete_required_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::init_msg_c");
  }

  return *this;
}
ricsubscription_request_s& e2_ap_elem_procs_o::init_msg_c::ricsubscription_request()
{
  assert_choice_type(types::ricsubscription_request, type_, "InitiatingMessage");
  return c.get<ricsubscription_request_s>();
}
ricsubscription_delete_request_s& e2_ap_elem_procs_o::init_msg_c::ricsubscription_delete_request()
{
  assert_choice_type(types::ricsubscription_delete_request, type_, "InitiatingMessage");
  return c.get<ricsubscription_delete_request_s>();
}
ricservice_upd_s& e2_ap_elem_procs_o::init_msg_c::ricservice_upd()
{
  assert_choice_type(types::ricservice_upd, type_, "InitiatingMessage");
  return c.get<ricservice_upd_s>();
}
ri_cctrl_request_s& e2_ap_elem_procs_o::init_msg_c::ri_cctrl_request()
{
  assert_choice_type(types::ri_cctrl_request, type_, "InitiatingMessage");
  return c.get<ri_cctrl_request_s>();
}
e2setup_request_s& e2_ap_elem_procs_o::init_msg_c::e2setup_request()
{
  assert_choice_type(types::e2setup_request, type_, "InitiatingMessage");
  return c.get<e2setup_request_s>();
}
e2node_cfg_upd_s& e2_ap_elem_procs_o::init_msg_c::e2node_cfg_upd()
{
  assert_choice_type(types::e2node_cfg_upd, type_, "InitiatingMessage");
  return c.get<e2node_cfg_upd_s>();
}
e2conn_upd_s& e2_ap_elem_procs_o::init_msg_c::e2conn_upd()
{
  assert_choice_type(types::e2conn_upd, type_, "InitiatingMessage");
  return c.get<e2conn_upd_s>();
}
reset_request_s& e2_ap_elem_procs_o::init_msg_c::reset_request()
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
e2_removal_request_s& e2_ap_elem_procs_o::init_msg_c::e2_removal_request()
{
  assert_choice_type(types::e2_removal_request, type_, "InitiatingMessage");
  return c.get<e2_removal_request_s>();
}
ri_cind_s& e2_ap_elem_procs_o::init_msg_c::ri_cind()
{
  assert_choice_type(types::ri_cind, type_, "InitiatingMessage");
  return c.get<ri_cind_s>();
}
ricservice_query_s& e2_ap_elem_procs_o::init_msg_c::ricservice_query()
{
  assert_choice_type(types::ricservice_query, type_, "InitiatingMessage");
  return c.get<ricservice_query_s>();
}
error_ind_s& e2_ap_elem_procs_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
ricsubscription_delete_required_s& e2_ap_elem_procs_o::init_msg_c::ricsubscription_delete_required()
{
  assert_choice_type(types::ricsubscription_delete_required, type_, "InitiatingMessage");
  return c.get<ricsubscription_delete_required_s>();
}
const ricsubscription_request_s& e2_ap_elem_procs_o::init_msg_c::ricsubscription_request() const
{
  assert_choice_type(types::ricsubscription_request, type_, "InitiatingMessage");
  return c.get<ricsubscription_request_s>();
}
const ricsubscription_delete_request_s& e2_ap_elem_procs_o::init_msg_c::ricsubscription_delete_request() const
{
  assert_choice_type(types::ricsubscription_delete_request, type_, "InitiatingMessage");
  return c.get<ricsubscription_delete_request_s>();
}
const ricservice_upd_s& e2_ap_elem_procs_o::init_msg_c::ricservice_upd() const
{
  assert_choice_type(types::ricservice_upd, type_, "InitiatingMessage");
  return c.get<ricservice_upd_s>();
}
const ri_cctrl_request_s& e2_ap_elem_procs_o::init_msg_c::ri_cctrl_request() const
{
  assert_choice_type(types::ri_cctrl_request, type_, "InitiatingMessage");
  return c.get<ri_cctrl_request_s>();
}
const e2setup_request_s& e2_ap_elem_procs_o::init_msg_c::e2setup_request() const
{
  assert_choice_type(types::e2setup_request, type_, "InitiatingMessage");
  return c.get<e2setup_request_s>();
}
const e2node_cfg_upd_s& e2_ap_elem_procs_o::init_msg_c::e2node_cfg_upd() const
{
  assert_choice_type(types::e2node_cfg_upd, type_, "InitiatingMessage");
  return c.get<e2node_cfg_upd_s>();
}
const e2conn_upd_s& e2_ap_elem_procs_o::init_msg_c::e2conn_upd() const
{
  assert_choice_type(types::e2conn_upd, type_, "InitiatingMessage");
  return c.get<e2conn_upd_s>();
}
const reset_request_s& e2_ap_elem_procs_o::init_msg_c::reset_request() const
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
const e2_removal_request_s& e2_ap_elem_procs_o::init_msg_c::e2_removal_request() const
{
  assert_choice_type(types::e2_removal_request, type_, "InitiatingMessage");
  return c.get<e2_removal_request_s>();
}
const ri_cind_s& e2_ap_elem_procs_o::init_msg_c::ri_cind() const
{
  assert_choice_type(types::ri_cind, type_, "InitiatingMessage");
  return c.get<ri_cind_s>();
}
const ricservice_query_s& e2_ap_elem_procs_o::init_msg_c::ricservice_query() const
{
  assert_choice_type(types::ricservice_query, type_, "InitiatingMessage");
  return c.get<ricservice_query_s>();
}
const error_ind_s& e2_ap_elem_procs_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const ricsubscription_delete_required_s& e2_ap_elem_procs_o::init_msg_c::ricsubscription_delete_required() const
{
  assert_choice_type(types::ricsubscription_delete_required, type_, "InitiatingMessage");
  return c.get<ricsubscription_delete_required_s>();
}
void e2_ap_elem_procs_o::init_msg_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ricsubscription_request:
      j.write_fieldname("RICsubscriptionRequest");
      c.get<ricsubscription_request_s>().to_json(j);
      break;
    case types::ricsubscription_delete_request:
      j.write_fieldname("RICsubscriptionDeleteRequest");
      c.get<ricsubscription_delete_request_s>().to_json(j);
      break;
    case types::ricservice_upd:
      j.write_fieldname("RICserviceUpdate");
      c.get<ricservice_upd_s>().to_json(j);
      break;
    case types::ri_cctrl_request:
      j.write_fieldname("RICcontrolRequest");
      c.get<ri_cctrl_request_s>().to_json(j);
      break;
    case types::e2setup_request:
      j.write_fieldname("E2setupRequest");
      c.get<e2setup_request_s>().to_json(j);
      break;
    case types::e2node_cfg_upd:
      j.write_fieldname("E2nodeConfigurationUpdate");
      c.get<e2node_cfg_upd_s>().to_json(j);
      break;
    case types::e2conn_upd:
      j.write_fieldname("E2connectionUpdate");
      c.get<e2conn_upd_s>().to_json(j);
      break;
    case types::reset_request:
      j.write_fieldname("ResetRequest");
      c.get<reset_request_s>().to_json(j);
      break;
    case types::e2_removal_request:
      j.write_fieldname("E2RemovalRequest");
      c.get<e2_removal_request_s>().to_json(j);
      break;
    case types::ri_cind:
      j.write_fieldname("RICindication");
      c.get<ri_cind_s>().to_json(j);
      break;
    case types::ricservice_query:
      j.write_fieldname("RICserviceQuery");
      c.get<ricservice_query_s>().to_json(j);
      break;
    case types::error_ind:
      j.write_fieldname("ErrorIndication");
      c.get<error_ind_s>().to_json(j);
      break;
    case types::ricsubscription_delete_required:
      j.write_fieldname("RICsubscriptionDeleteRequired");
      c.get<ricsubscription_delete_required_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::init_msg_c");
  }
  j.end_obj();
}
SRSASN_CODE e2_ap_elem_procs_o::init_msg_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ricsubscription_request:
      HANDLE_CODE(c.get<ricsubscription_request_s>().pack(bref));
      break;
    case types::ricsubscription_delete_request:
      HANDLE_CODE(c.get<ricsubscription_delete_request_s>().pack(bref));
      break;
    case types::ricservice_upd:
      HANDLE_CODE(c.get<ricservice_upd_s>().pack(bref));
      break;
    case types::ri_cctrl_request:
      HANDLE_CODE(c.get<ri_cctrl_request_s>().pack(bref));
      break;
    case types::e2setup_request:
      HANDLE_CODE(c.get<e2setup_request_s>().pack(bref));
      break;
    case types::e2node_cfg_upd:
      HANDLE_CODE(c.get<e2node_cfg_upd_s>().pack(bref));
      break;
    case types::e2conn_upd:
      HANDLE_CODE(c.get<e2conn_upd_s>().pack(bref));
      break;
    case types::reset_request:
      HANDLE_CODE(c.get<reset_request_s>().pack(bref));
      break;
    case types::e2_removal_request:
      HANDLE_CODE(c.get<e2_removal_request_s>().pack(bref));
      break;
    case types::ri_cind:
      HANDLE_CODE(c.get<ri_cind_s>().pack(bref));
      break;
    case types::ricservice_query:
      HANDLE_CODE(c.get<ricservice_query_s>().pack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().pack(bref));
      break;
    case types::ricsubscription_delete_required:
      HANDLE_CODE(c.get<ricsubscription_delete_required_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::init_msg_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_ap_elem_procs_o::init_msg_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ricsubscription_request:
      HANDLE_CODE(c.get<ricsubscription_request_s>().unpack(bref));
      break;
    case types::ricsubscription_delete_request:
      HANDLE_CODE(c.get<ricsubscription_delete_request_s>().unpack(bref));
      break;
    case types::ricservice_upd:
      HANDLE_CODE(c.get<ricservice_upd_s>().unpack(bref));
      break;
    case types::ri_cctrl_request:
      HANDLE_CODE(c.get<ri_cctrl_request_s>().unpack(bref));
      break;
    case types::e2setup_request:
      HANDLE_CODE(c.get<e2setup_request_s>().unpack(bref));
      break;
    case types::e2node_cfg_upd:
      HANDLE_CODE(c.get<e2node_cfg_upd_s>().unpack(bref));
      break;
    case types::e2conn_upd:
      HANDLE_CODE(c.get<e2conn_upd_s>().unpack(bref));
      break;
    case types::reset_request:
      HANDLE_CODE(c.get<reset_request_s>().unpack(bref));
      break;
    case types::e2_removal_request:
      HANDLE_CODE(c.get<e2_removal_request_s>().unpack(bref));
      break;
    case types::ri_cind:
      HANDLE_CODE(c.get<ri_cind_s>().unpack(bref));
      break;
    case types::ricservice_query:
      HANDLE_CODE(c.get<ricservice_query_s>().unpack(bref));
      break;
    case types::error_ind:
      HANDLE_CODE(c.get<error_ind_s>().unpack(bref));
      break;
    case types::ricsubscription_delete_required:
      HANDLE_CODE(c.get<ricsubscription_delete_required_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::init_msg_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2_ap_elem_procs_o::init_msg_c::types_opts::to_string() const
{
  static const char* options[] = {"RICsubscriptionRequest",
                                  "RICsubscriptionDeleteRequest",
                                  "RICserviceUpdate",
                                  "RICcontrolRequest",
                                  "E2setupRequest",
                                  "E2nodeConfigurationUpdate",
                                  "E2connectionUpdate",
                                  "ResetRequest",
                                  "E2RemovalRequest",
                                  "RICindication",
                                  "RICserviceQuery",
                                  "ErrorIndication",
                                  "RICsubscriptionDeleteRequired"};
  return convert_enum_idx(options, 13, value, "e2_ap_elem_procs_o::init_msg_c::types");
}

// SuccessfulOutcome ::= OPEN TYPE
void e2_ap_elem_procs_o::successful_outcome_c::destroy_()
{
  switch (type_) {
    case types::ricsubscription_resp:
      c.destroy<ricsubscription_resp_s>();
      break;
    case types::ricsubscription_delete_resp:
      c.destroy<ricsubscription_delete_resp_s>();
      break;
    case types::ricservice_upd_ack:
      c.destroy<ricservice_upd_ack_s>();
      break;
    case types::ri_cctrl_ack:
      c.destroy<ri_cctrl_ack_s>();
      break;
    case types::e2setup_resp:
      c.destroy<e2setup_resp_s>();
      break;
    case types::e2node_cfg_upd_ack:
      c.destroy<e2node_cfg_upd_ack_s>();
      break;
    case types::e2conn_upd_ack:
      c.destroy<e2conn_upd_ack_s>();
      break;
    case types::reset_resp:
      c.destroy<reset_resp_s>();
      break;
    case types::e2_removal_resp:
      c.destroy<e2_removal_resp_s>();
      break;
    default:
      break;
  }
}
void e2_ap_elem_procs_o::successful_outcome_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ricsubscription_resp:
      c.init<ricsubscription_resp_s>();
      break;
    case types::ricsubscription_delete_resp:
      c.init<ricsubscription_delete_resp_s>();
      break;
    case types::ricservice_upd_ack:
      c.init<ricservice_upd_ack_s>();
      break;
    case types::ri_cctrl_ack:
      c.init<ri_cctrl_ack_s>();
      break;
    case types::e2setup_resp:
      c.init<e2setup_resp_s>();
      break;
    case types::e2node_cfg_upd_ack:
      c.init<e2node_cfg_upd_ack_s>();
      break;
    case types::e2conn_upd_ack:
      c.init<e2conn_upd_ack_s>();
      break;
    case types::reset_resp:
      c.init<reset_resp_s>();
      break;
    case types::e2_removal_resp:
      c.init<e2_removal_resp_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::successful_outcome_c");
  }
}
e2_ap_elem_procs_o::successful_outcome_c::successful_outcome_c(const e2_ap_elem_procs_o::successful_outcome_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ricsubscription_resp:
      c.init(other.c.get<ricsubscription_resp_s>());
      break;
    case types::ricsubscription_delete_resp:
      c.init(other.c.get<ricsubscription_delete_resp_s>());
      break;
    case types::ricservice_upd_ack:
      c.init(other.c.get<ricservice_upd_ack_s>());
      break;
    case types::ri_cctrl_ack:
      c.init(other.c.get<ri_cctrl_ack_s>());
      break;
    case types::e2setup_resp:
      c.init(other.c.get<e2setup_resp_s>());
      break;
    case types::e2node_cfg_upd_ack:
      c.init(other.c.get<e2node_cfg_upd_ack_s>());
      break;
    case types::e2conn_upd_ack:
      c.init(other.c.get<e2conn_upd_ack_s>());
      break;
    case types::reset_resp:
      c.init(other.c.get<reset_resp_s>());
      break;
    case types::e2_removal_resp:
      c.init(other.c.get<e2_removal_resp_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::successful_outcome_c");
  }
}
e2_ap_elem_procs_o::successful_outcome_c&
e2_ap_elem_procs_o::successful_outcome_c::operator=(const e2_ap_elem_procs_o::successful_outcome_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ricsubscription_resp:
      c.set(other.c.get<ricsubscription_resp_s>());
      break;
    case types::ricsubscription_delete_resp:
      c.set(other.c.get<ricsubscription_delete_resp_s>());
      break;
    case types::ricservice_upd_ack:
      c.set(other.c.get<ricservice_upd_ack_s>());
      break;
    case types::ri_cctrl_ack:
      c.set(other.c.get<ri_cctrl_ack_s>());
      break;
    case types::e2setup_resp:
      c.set(other.c.get<e2setup_resp_s>());
      break;
    case types::e2node_cfg_upd_ack:
      c.set(other.c.get<e2node_cfg_upd_ack_s>());
      break;
    case types::e2conn_upd_ack:
      c.set(other.c.get<e2conn_upd_ack_s>());
      break;
    case types::reset_resp:
      c.set(other.c.get<reset_resp_s>());
      break;
    case types::e2_removal_resp:
      c.set(other.c.get<e2_removal_resp_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::successful_outcome_c");
  }

  return *this;
}
ricsubscription_resp_s& e2_ap_elem_procs_o::successful_outcome_c::ricsubscription_resp()
{
  assert_choice_type(types::ricsubscription_resp, type_, "SuccessfulOutcome");
  return c.get<ricsubscription_resp_s>();
}
ricsubscription_delete_resp_s& e2_ap_elem_procs_o::successful_outcome_c::ricsubscription_delete_resp()
{
  assert_choice_type(types::ricsubscription_delete_resp, type_, "SuccessfulOutcome");
  return c.get<ricsubscription_delete_resp_s>();
}
ricservice_upd_ack_s& e2_ap_elem_procs_o::successful_outcome_c::ricservice_upd_ack()
{
  assert_choice_type(types::ricservice_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ricservice_upd_ack_s>();
}
ri_cctrl_ack_s& e2_ap_elem_procs_o::successful_outcome_c::ri_cctrl_ack()
{
  assert_choice_type(types::ri_cctrl_ack, type_, "SuccessfulOutcome");
  return c.get<ri_cctrl_ack_s>();
}
e2setup_resp_s& e2_ap_elem_procs_o::successful_outcome_c::e2setup_resp()
{
  assert_choice_type(types::e2setup_resp, type_, "SuccessfulOutcome");
  return c.get<e2setup_resp_s>();
}
e2node_cfg_upd_ack_s& e2_ap_elem_procs_o::successful_outcome_c::e2node_cfg_upd_ack()
{
  assert_choice_type(types::e2node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2node_cfg_upd_ack_s>();
}
e2conn_upd_ack_s& e2_ap_elem_procs_o::successful_outcome_c::e2conn_upd_ack()
{
  assert_choice_type(types::e2conn_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2conn_upd_ack_s>();
}
reset_resp_s& e2_ap_elem_procs_o::successful_outcome_c::reset_resp()
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
e2_removal_resp_s& e2_ap_elem_procs_o::successful_outcome_c::e2_removal_resp()
{
  assert_choice_type(types::e2_removal_resp, type_, "SuccessfulOutcome");
  return c.get<e2_removal_resp_s>();
}
const ricsubscription_resp_s& e2_ap_elem_procs_o::successful_outcome_c::ricsubscription_resp() const
{
  assert_choice_type(types::ricsubscription_resp, type_, "SuccessfulOutcome");
  return c.get<ricsubscription_resp_s>();
}
const ricsubscription_delete_resp_s& e2_ap_elem_procs_o::successful_outcome_c::ricsubscription_delete_resp() const
{
  assert_choice_type(types::ricsubscription_delete_resp, type_, "SuccessfulOutcome");
  return c.get<ricsubscription_delete_resp_s>();
}
const ricservice_upd_ack_s& e2_ap_elem_procs_o::successful_outcome_c::ricservice_upd_ack() const
{
  assert_choice_type(types::ricservice_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ricservice_upd_ack_s>();
}
const ri_cctrl_ack_s& e2_ap_elem_procs_o::successful_outcome_c::ri_cctrl_ack() const
{
  assert_choice_type(types::ri_cctrl_ack, type_, "SuccessfulOutcome");
  return c.get<ri_cctrl_ack_s>();
}
const e2setup_resp_s& e2_ap_elem_procs_o::successful_outcome_c::e2setup_resp() const
{
  assert_choice_type(types::e2setup_resp, type_, "SuccessfulOutcome");
  return c.get<e2setup_resp_s>();
}
const e2node_cfg_upd_ack_s& e2_ap_elem_procs_o::successful_outcome_c::e2node_cfg_upd_ack() const
{
  assert_choice_type(types::e2node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2node_cfg_upd_ack_s>();
}
const e2conn_upd_ack_s& e2_ap_elem_procs_o::successful_outcome_c::e2conn_upd_ack() const
{
  assert_choice_type(types::e2conn_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2conn_upd_ack_s>();
}
const reset_resp_s& e2_ap_elem_procs_o::successful_outcome_c::reset_resp() const
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
const e2_removal_resp_s& e2_ap_elem_procs_o::successful_outcome_c::e2_removal_resp() const
{
  assert_choice_type(types::e2_removal_resp, type_, "SuccessfulOutcome");
  return c.get<e2_removal_resp_s>();
}
void e2_ap_elem_procs_o::successful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ricsubscription_resp:
      j.write_fieldname("RICsubscriptionResponse");
      c.get<ricsubscription_resp_s>().to_json(j);
      break;
    case types::ricsubscription_delete_resp:
      j.write_fieldname("RICsubscriptionDeleteResponse");
      c.get<ricsubscription_delete_resp_s>().to_json(j);
      break;
    case types::ricservice_upd_ack:
      j.write_fieldname("RICserviceUpdateAcknowledge");
      c.get<ricservice_upd_ack_s>().to_json(j);
      break;
    case types::ri_cctrl_ack:
      j.write_fieldname("RICcontrolAcknowledge");
      c.get<ri_cctrl_ack_s>().to_json(j);
      break;
    case types::e2setup_resp:
      j.write_fieldname("E2setupResponse");
      c.get<e2setup_resp_s>().to_json(j);
      break;
    case types::e2node_cfg_upd_ack:
      j.write_fieldname("E2nodeConfigurationUpdateAcknowledge");
      c.get<e2node_cfg_upd_ack_s>().to_json(j);
      break;
    case types::e2conn_upd_ack:
      j.write_fieldname("E2connectionUpdateAcknowledge");
      c.get<e2conn_upd_ack_s>().to_json(j);
      break;
    case types::reset_resp:
      j.write_fieldname("ResetResponse");
      c.get<reset_resp_s>().to_json(j);
      break;
    case types::e2_removal_resp:
      j.write_fieldname("E2RemovalResponse");
      c.get<e2_removal_resp_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::successful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE e2_ap_elem_procs_o::successful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ricsubscription_resp:
      HANDLE_CODE(c.get<ricsubscription_resp_s>().pack(bref));
      break;
    case types::ricsubscription_delete_resp:
      HANDLE_CODE(c.get<ricsubscription_delete_resp_s>().pack(bref));
      break;
    case types::ricservice_upd_ack:
      HANDLE_CODE(c.get<ricservice_upd_ack_s>().pack(bref));
      break;
    case types::ri_cctrl_ack:
      HANDLE_CODE(c.get<ri_cctrl_ack_s>().pack(bref));
      break;
    case types::e2setup_resp:
      HANDLE_CODE(c.get<e2setup_resp_s>().pack(bref));
      break;
    case types::e2node_cfg_upd_ack:
      HANDLE_CODE(c.get<e2node_cfg_upd_ack_s>().pack(bref));
      break;
    case types::e2conn_upd_ack:
      HANDLE_CODE(c.get<e2conn_upd_ack_s>().pack(bref));
      break;
    case types::reset_resp:
      HANDLE_CODE(c.get<reset_resp_s>().pack(bref));
      break;
    case types::e2_removal_resp:
      HANDLE_CODE(c.get<e2_removal_resp_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::successful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_ap_elem_procs_o::successful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ricsubscription_resp:
      HANDLE_CODE(c.get<ricsubscription_resp_s>().unpack(bref));
      break;
    case types::ricsubscription_delete_resp:
      HANDLE_CODE(c.get<ricsubscription_delete_resp_s>().unpack(bref));
      break;
    case types::ricservice_upd_ack:
      HANDLE_CODE(c.get<ricservice_upd_ack_s>().unpack(bref));
      break;
    case types::ri_cctrl_ack:
      HANDLE_CODE(c.get<ri_cctrl_ack_s>().unpack(bref));
      break;
    case types::e2setup_resp:
      HANDLE_CODE(c.get<e2setup_resp_s>().unpack(bref));
      break;
    case types::e2node_cfg_upd_ack:
      HANDLE_CODE(c.get<e2node_cfg_upd_ack_s>().unpack(bref));
      break;
    case types::e2conn_upd_ack:
      HANDLE_CODE(c.get<e2conn_upd_ack_s>().unpack(bref));
      break;
    case types::reset_resp:
      HANDLE_CODE(c.get<reset_resp_s>().unpack(bref));
      break;
    case types::e2_removal_resp:
      HANDLE_CODE(c.get<e2_removal_resp_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::successful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2_ap_elem_procs_o::successful_outcome_c::types_opts::to_string() const
{
  static const char* options[] = {"RICsubscriptionResponse",
                                  "RICsubscriptionDeleteResponse",
                                  "RICserviceUpdateAcknowledge",
                                  "RICcontrolAcknowledge",
                                  "E2setupResponse",
                                  "E2nodeConfigurationUpdateAcknowledge",
                                  "E2connectionUpdateAcknowledge",
                                  "ResetResponse",
                                  "E2RemovalResponse"};
  return convert_enum_idx(options, 9, value, "e2_ap_elem_procs_o::successful_outcome_c::types");
}

// UnsuccessfulOutcome ::= OPEN TYPE
void e2_ap_elem_procs_o::unsuccessful_outcome_c::destroy_()
{
  switch (type_) {
    case types::ricsubscription_fail:
      c.destroy<ricsubscription_fail_s>();
      break;
    case types::ricsubscription_delete_fail:
      c.destroy<ricsubscription_delete_fail_s>();
      break;
    case types::ricservice_upd_fail:
      c.destroy<ricservice_upd_fail_s>();
      break;
    case types::ri_cctrl_fail:
      c.destroy<ri_cctrl_fail_s>();
      break;
    case types::e2setup_fail:
      c.destroy<e2setup_fail_s>();
      break;
    case types::e2node_cfg_upd_fail:
      c.destroy<e2node_cfg_upd_fail_s>();
      break;
    case types::e2conn_upd_fail:
      c.destroy<e2conn_upd_fail_s>();
      break;
    case types::e2_removal_fail:
      c.destroy<e2_removal_fail_s>();
      break;
    default:
      break;
  }
}
void e2_ap_elem_procs_o::unsuccessful_outcome_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ricsubscription_fail:
      c.init<ricsubscription_fail_s>();
      break;
    case types::ricsubscription_delete_fail:
      c.init<ricsubscription_delete_fail_s>();
      break;
    case types::ricservice_upd_fail:
      c.init<ricservice_upd_fail_s>();
      break;
    case types::ri_cctrl_fail:
      c.init<ri_cctrl_fail_s>();
      break;
    case types::e2setup_fail:
      c.init<e2setup_fail_s>();
      break;
    case types::e2node_cfg_upd_fail:
      c.init<e2node_cfg_upd_fail_s>();
      break;
    case types::e2conn_upd_fail:
      c.init<e2conn_upd_fail_s>();
      break;
    case types::e2_removal_fail:
      c.init<e2_removal_fail_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::unsuccessful_outcome_c");
  }
}
e2_ap_elem_procs_o::unsuccessful_outcome_c::unsuccessful_outcome_c(
    const e2_ap_elem_procs_o::unsuccessful_outcome_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ricsubscription_fail:
      c.init(other.c.get<ricsubscription_fail_s>());
      break;
    case types::ricsubscription_delete_fail:
      c.init(other.c.get<ricsubscription_delete_fail_s>());
      break;
    case types::ricservice_upd_fail:
      c.init(other.c.get<ricservice_upd_fail_s>());
      break;
    case types::ri_cctrl_fail:
      c.init(other.c.get<ri_cctrl_fail_s>());
      break;
    case types::e2setup_fail:
      c.init(other.c.get<e2setup_fail_s>());
      break;
    case types::e2node_cfg_upd_fail:
      c.init(other.c.get<e2node_cfg_upd_fail_s>());
      break;
    case types::e2conn_upd_fail:
      c.init(other.c.get<e2conn_upd_fail_s>());
      break;
    case types::e2_removal_fail:
      c.init(other.c.get<e2_removal_fail_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::unsuccessful_outcome_c");
  }
}
e2_ap_elem_procs_o::unsuccessful_outcome_c&
e2_ap_elem_procs_o::unsuccessful_outcome_c::operator=(const e2_ap_elem_procs_o::unsuccessful_outcome_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ricsubscription_fail:
      c.set(other.c.get<ricsubscription_fail_s>());
      break;
    case types::ricsubscription_delete_fail:
      c.set(other.c.get<ricsubscription_delete_fail_s>());
      break;
    case types::ricservice_upd_fail:
      c.set(other.c.get<ricservice_upd_fail_s>());
      break;
    case types::ri_cctrl_fail:
      c.set(other.c.get<ri_cctrl_fail_s>());
      break;
    case types::e2setup_fail:
      c.set(other.c.get<e2setup_fail_s>());
      break;
    case types::e2node_cfg_upd_fail:
      c.set(other.c.get<e2node_cfg_upd_fail_s>());
      break;
    case types::e2conn_upd_fail:
      c.set(other.c.get<e2conn_upd_fail_s>());
      break;
    case types::e2_removal_fail:
      c.set(other.c.get<e2_removal_fail_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::unsuccessful_outcome_c");
  }

  return *this;
}
ricsubscription_fail_s& e2_ap_elem_procs_o::unsuccessful_outcome_c::ricsubscription_fail()
{
  assert_choice_type(types::ricsubscription_fail, type_, "UnsuccessfulOutcome");
  return c.get<ricsubscription_fail_s>();
}
ricsubscription_delete_fail_s& e2_ap_elem_procs_o::unsuccessful_outcome_c::ricsubscription_delete_fail()
{
  assert_choice_type(types::ricsubscription_delete_fail, type_, "UnsuccessfulOutcome");
  return c.get<ricsubscription_delete_fail_s>();
}
ricservice_upd_fail_s& e2_ap_elem_procs_o::unsuccessful_outcome_c::ricservice_upd_fail()
{
  assert_choice_type(types::ricservice_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ricservice_upd_fail_s>();
}
ri_cctrl_fail_s& e2_ap_elem_procs_o::unsuccessful_outcome_c::ri_cctrl_fail()
{
  assert_choice_type(types::ri_cctrl_fail, type_, "UnsuccessfulOutcome");
  return c.get<ri_cctrl_fail_s>();
}
e2setup_fail_s& e2_ap_elem_procs_o::unsuccessful_outcome_c::e2setup_fail()
{
  assert_choice_type(types::e2setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2setup_fail_s>();
}
e2node_cfg_upd_fail_s& e2_ap_elem_procs_o::unsuccessful_outcome_c::e2node_cfg_upd_fail()
{
  assert_choice_type(types::e2node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2node_cfg_upd_fail_s>();
}
e2conn_upd_fail_s& e2_ap_elem_procs_o::unsuccessful_outcome_c::e2conn_upd_fail()
{
  assert_choice_type(types::e2conn_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2conn_upd_fail_s>();
}
e2_removal_fail_s& e2_ap_elem_procs_o::unsuccessful_outcome_c::e2_removal_fail()
{
  assert_choice_type(types::e2_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2_removal_fail_s>();
}
const ricsubscription_fail_s& e2_ap_elem_procs_o::unsuccessful_outcome_c::ricsubscription_fail() const
{
  assert_choice_type(types::ricsubscription_fail, type_, "UnsuccessfulOutcome");
  return c.get<ricsubscription_fail_s>();
}
const ricsubscription_delete_fail_s& e2_ap_elem_procs_o::unsuccessful_outcome_c::ricsubscription_delete_fail() const
{
  assert_choice_type(types::ricsubscription_delete_fail, type_, "UnsuccessfulOutcome");
  return c.get<ricsubscription_delete_fail_s>();
}
const ricservice_upd_fail_s& e2_ap_elem_procs_o::unsuccessful_outcome_c::ricservice_upd_fail() const
{
  assert_choice_type(types::ricservice_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ricservice_upd_fail_s>();
}
const ri_cctrl_fail_s& e2_ap_elem_procs_o::unsuccessful_outcome_c::ri_cctrl_fail() const
{
  assert_choice_type(types::ri_cctrl_fail, type_, "UnsuccessfulOutcome");
  return c.get<ri_cctrl_fail_s>();
}
const e2setup_fail_s& e2_ap_elem_procs_o::unsuccessful_outcome_c::e2setup_fail() const
{
  assert_choice_type(types::e2setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2setup_fail_s>();
}
const e2node_cfg_upd_fail_s& e2_ap_elem_procs_o::unsuccessful_outcome_c::e2node_cfg_upd_fail() const
{
  assert_choice_type(types::e2node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2node_cfg_upd_fail_s>();
}
const e2conn_upd_fail_s& e2_ap_elem_procs_o::unsuccessful_outcome_c::e2conn_upd_fail() const
{
  assert_choice_type(types::e2conn_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2conn_upd_fail_s>();
}
const e2_removal_fail_s& e2_ap_elem_procs_o::unsuccessful_outcome_c::e2_removal_fail() const
{
  assert_choice_type(types::e2_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2_removal_fail_s>();
}
void e2_ap_elem_procs_o::unsuccessful_outcome_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ricsubscription_fail:
      j.write_fieldname("RICsubscriptionFailure");
      c.get<ricsubscription_fail_s>().to_json(j);
      break;
    case types::ricsubscription_delete_fail:
      j.write_fieldname("RICsubscriptionDeleteFailure");
      c.get<ricsubscription_delete_fail_s>().to_json(j);
      break;
    case types::ricservice_upd_fail:
      j.write_fieldname("RICserviceUpdateFailure");
      c.get<ricservice_upd_fail_s>().to_json(j);
      break;
    case types::ri_cctrl_fail:
      j.write_fieldname("RICcontrolFailure");
      c.get<ri_cctrl_fail_s>().to_json(j);
      break;
    case types::e2setup_fail:
      j.write_fieldname("E2setupFailure");
      c.get<e2setup_fail_s>().to_json(j);
      break;
    case types::e2node_cfg_upd_fail:
      j.write_fieldname("E2nodeConfigurationUpdateFailure");
      c.get<e2node_cfg_upd_fail_s>().to_json(j);
      break;
    case types::e2conn_upd_fail:
      j.write_fieldname("E2connectionUpdateFailure");
      c.get<e2conn_upd_fail_s>().to_json(j);
      break;
    case types::e2_removal_fail:
      j.write_fieldname("E2RemovalFailure");
      c.get<e2_removal_fail_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::unsuccessful_outcome_c");
  }
  j.end_obj();
}
SRSASN_CODE e2_ap_elem_procs_o::unsuccessful_outcome_c::pack(bit_ref& bref) const
{
  varlength_field_pack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ricsubscription_fail:
      HANDLE_CODE(c.get<ricsubscription_fail_s>().pack(bref));
      break;
    case types::ricsubscription_delete_fail:
      HANDLE_CODE(c.get<ricsubscription_delete_fail_s>().pack(bref));
      break;
    case types::ricservice_upd_fail:
      HANDLE_CODE(c.get<ricservice_upd_fail_s>().pack(bref));
      break;
    case types::ri_cctrl_fail:
      HANDLE_CODE(c.get<ri_cctrl_fail_s>().pack(bref));
      break;
    case types::e2setup_fail:
      HANDLE_CODE(c.get<e2setup_fail_s>().pack(bref));
      break;
    case types::e2node_cfg_upd_fail:
      HANDLE_CODE(c.get<e2node_cfg_upd_fail_s>().pack(bref));
      break;
    case types::e2conn_upd_fail:
      HANDLE_CODE(c.get<e2conn_upd_fail_s>().pack(bref));
      break;
    case types::e2_removal_fail:
      HANDLE_CODE(c.get<e2_removal_fail_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_ap_elem_procs_o::unsuccessful_outcome_c::unpack(cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  switch (type_) {
    case types::ricsubscription_fail:
      HANDLE_CODE(c.get<ricsubscription_fail_s>().unpack(bref));
      break;
    case types::ricsubscription_delete_fail:
      HANDLE_CODE(c.get<ricsubscription_delete_fail_s>().unpack(bref));
      break;
    case types::ricservice_upd_fail:
      HANDLE_CODE(c.get<ricservice_upd_fail_s>().unpack(bref));
      break;
    case types::ri_cctrl_fail:
      HANDLE_CODE(c.get<ri_cctrl_fail_s>().unpack(bref));
      break;
    case types::e2setup_fail:
      HANDLE_CODE(c.get<e2setup_fail_s>().unpack(bref));
      break;
    case types::e2node_cfg_upd_fail:
      HANDLE_CODE(c.get<e2node_cfg_upd_fail_s>().unpack(bref));
      break;
    case types::e2conn_upd_fail:
      HANDLE_CODE(c.get<e2conn_upd_fail_s>().unpack(bref));
      break;
    case types::e2_removal_fail:
      HANDLE_CODE(c.get<e2_removal_fail_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_elem_procs_o::unsuccessful_outcome_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::to_string() const
{
  static const char* options[] = {"RICsubscriptionFailure",
                                  "RICsubscriptionDeleteFailure",
                                  "RICserviceUpdateFailure",
                                  "RICcontrolFailure",
                                  "E2setupFailure",
                                  "E2nodeConfigurationUpdateFailure",
                                  "E2connectionUpdateFailure",
                                  "E2RemovalFailure"};
  return convert_enum_idx(options, 8, value, "e2_ap_elem_procs_o::unsuccessful_outcome_c::types");
}

// InitiatingMessage ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE init_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != e2_ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE init_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = e2_ap_elem_procs_o::get_init_msg(proc_code);
  HANDLE_CODE(value.unpack(bref));

  return SRSASN_SUCCESS;
}
void init_msg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("procedureCode", proc_code);
  j.write_str("criticality", crit.to_string());
  j.write_fieldname("value");
  value.to_json(j);
  j.end_obj();
}
bool init_msg_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not e2_ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = e2_ap_elem_procs_o::get_crit(proc_code);
  value     = e2_ap_elem_procs_o::get_init_msg(proc_code);
  return value.type().value != e2_ap_elem_procs_o::init_msg_c::types_opts::nulltype;
}

// SuccessfulOutcome ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE successful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != e2_ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE successful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = e2_ap_elem_procs_o::get_successful_outcome(proc_code);
  HANDLE_CODE(value.unpack(bref));

  return SRSASN_SUCCESS;
}
void successful_outcome_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("procedureCode", proc_code);
  j.write_str("criticality", crit.to_string());
  j.write_fieldname("value");
  value.to_json(j);
  j.end_obj();
}
bool successful_outcome_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not e2_ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = e2_ap_elem_procs_o::get_crit(proc_code);
  value     = e2_ap_elem_procs_o::get_successful_outcome(proc_code);
  return value.type().value != e2_ap_elem_procs_o::successful_outcome_c::types_opts::nulltype;
}

// UnsuccessfulOutcome ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
SRSASN_CODE unsuccessful_outcome_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, proc_code, (uint16_t)0u, (uint16_t)255u, false, true));
  warn_assert(crit != e2_ap_elem_procs_o::get_crit(proc_code), __func__, __LINE__);
  HANDLE_CODE(crit.pack(bref));
  HANDLE_CODE(value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE unsuccessful_outcome_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(proc_code, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  HANDLE_CODE(crit.unpack(bref));
  value = e2_ap_elem_procs_o::get_unsuccessful_outcome(proc_code);
  HANDLE_CODE(value.unpack(bref));

  return SRSASN_SUCCESS;
}
void unsuccessful_outcome_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("procedureCode", proc_code);
  j.write_str("criticality", crit.to_string());
  j.write_fieldname("value");
  value.to_json(j);
  j.end_obj();
}
bool unsuccessful_outcome_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not e2_ap_elem_procs_o::is_proc_code_valid(proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = e2_ap_elem_procs_o::get_crit(proc_code);
  value     = e2_ap_elem_procs_o::get_unsuccessful_outcome(proc_code);
  return value.type().value != e2_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::nulltype;
}

// E2AP-PDU ::= CHOICE
void e2_ap_pdu_c::destroy_()
{
  switch (type_) {
    case types::init_msg:
      c.destroy<init_msg_s>();
      break;
    case types::successful_outcome:
      c.destroy<successful_outcome_s>();
      break;
    case types::unsuccessful_outcome:
      c.destroy<unsuccessful_outcome_s>();
      break;
    default:
      break;
  }
}
void e2_ap_pdu_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::init_msg:
      c.init<init_msg_s>();
      break;
    case types::successful_outcome:
      c.init<successful_outcome_s>();
      break;
    case types::unsuccessful_outcome:
      c.init<unsuccessful_outcome_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_pdu_c");
  }
}
e2_ap_pdu_c::e2_ap_pdu_c(const e2_ap_pdu_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::init_msg:
      c.init(other.c.get<init_msg_s>());
      break;
    case types::successful_outcome:
      c.init(other.c.get<successful_outcome_s>());
      break;
    case types::unsuccessful_outcome:
      c.init(other.c.get<unsuccessful_outcome_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_pdu_c");
  }
}
e2_ap_pdu_c& e2_ap_pdu_c::operator=(const e2_ap_pdu_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::init_msg:
      c.set(other.c.get<init_msg_s>());
      break;
    case types::successful_outcome:
      c.set(other.c.get<successful_outcome_s>());
      break;
    case types::unsuccessful_outcome:
      c.set(other.c.get<unsuccessful_outcome_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_pdu_c");
  }

  return *this;
}
init_msg_s& e2_ap_pdu_c::set_init_msg()
{
  set(types::init_msg);
  return c.get<init_msg_s>();
}
successful_outcome_s& e2_ap_pdu_c::set_successful_outcome()
{
  set(types::successful_outcome);
  return c.get<successful_outcome_s>();
}
unsuccessful_outcome_s& e2_ap_pdu_c::set_unsuccessful_outcome()
{
  set(types::unsuccessful_outcome);
  return c.get<unsuccessful_outcome_s>();
}
void e2_ap_pdu_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::init_msg:
      j.write_fieldname("initiatingMessage");
      c.get<init_msg_s>().to_json(j);
      break;
    case types::successful_outcome:
      j.write_fieldname("successfulOutcome");
      c.get<successful_outcome_s>().to_json(j);
      break;
    case types::unsuccessful_outcome:
      j.write_fieldname("unsuccessfulOutcome");
      c.get<unsuccessful_outcome_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_pdu_c");
  }
  j.end_obj();
}
SRSASN_CODE e2_ap_pdu_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::init_msg:
      HANDLE_CODE(c.get<init_msg_s>().pack(bref));
      break;
    case types::successful_outcome:
      HANDLE_CODE(c.get<successful_outcome_s>().pack(bref));
      break;
    case types::unsuccessful_outcome:
      HANDLE_CODE(c.get<unsuccessful_outcome_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_pdu_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_ap_pdu_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::init_msg:
      HANDLE_CODE(c.get<init_msg_s>().unpack(bref));
      break;
    case types::successful_outcome:
      HANDLE_CODE(c.get<successful_outcome_s>().unpack(bref));
      break;
    case types::unsuccessful_outcome:
      HANDLE_CODE(c.get<unsuccessful_outcome_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "e2_ap_pdu_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2_ap_pdu_c::types_opts::to_string() const
{
  static const char* options[] = {"initiatingMessage", "successfulOutcome", "unsuccessfulOutcome"};
  return convert_enum_idx(options, 3, value, "e2_ap_pdu_c::types");
}

// ProtocolIE-FieldPair{E2AP-PROTOCOL-IES-PAIR : IEsSetParam} ::= SEQUENCE{{E2AP-PROTOCOL-IES-PAIR}}
template <class ies_set_paramT_>
SRSASN_CODE protocol_ie_field_pair_s<ies_set_paramT_>::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, id, (uint32_t)0u, (uint32_t)65535u, false, true));
  warn_assert(first_crit != ies_set_paramT_::get_first_crit(id), __func__, __LINE__);
  HANDLE_CODE(first_crit.pack(bref));
  HANDLE_CODE(first_value.pack(bref));
  warn_assert(second_crit != ies_set_paramT_::get_second_crit(id), __func__, __LINE__);
  HANDLE_CODE(second_crit.pack(bref));
  HANDLE_CODE(second_value.pack(bref));

  return SRSASN_SUCCESS;
}
template <class ies_set_paramT_>
SRSASN_CODE protocol_ie_field_pair_s<ies_set_paramT_>::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
  HANDLE_CODE(first_crit.unpack(bref));
  first_value = ies_set_paramT_::get_first_value(id);
  HANDLE_CODE(first_value.unpack(bref));
  HANDLE_CODE(second_crit.unpack(bref));
  second_value = ies_set_paramT_::get_second_value(id);
  HANDLE_CODE(second_value.unpack(bref));

  return SRSASN_SUCCESS;
}
template <class ies_set_paramT_>
void protocol_ie_field_pair_s<ies_set_paramT_>::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("id", id);
  j.write_str("firstCriticality", first_crit.to_string());
  j.write_str("secondCriticality", second_crit.to_string());
  j.end_obj();
}
template <class ies_set_paramT_>
bool protocol_ie_field_pair_s<ies_set_paramT_>::load_info_obj(const uint32_t& id_)
{
  if (not ies_set_paramT_::is_id_valid(id_)) {
    return false;
  }
  id           = id_;
  first_crit   = ies_set_paramT_::get_first_crit(id);
  first_value  = ies_set_paramT_::get_first_value(id);
  second_crit  = ies_set_paramT_::get_second_crit(id);
  second_value = ies_set_paramT_::get_second_value(id);
  return first_value.type().value != ies_set_paramT_::first_value_c::types_opts::nulltype and
         second_value.type().value != ies_set_paramT_::second_value_c::types_opts::nulltype;
}
