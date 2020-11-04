/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srslte/asn1/rrc/ul_ccch_msg.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// EstablishmentCause ::= ENUMERATED
std::string establishment_cause_opts::to_string() const
{
  static const char* options[] = {"emergency",
                                  "highPriorityAccess",
                                  "mt-Access",
                                  "mo-Signalling",
                                  "mo-Data",
                                  "delayTolerantAccess-v1020",
                                  "mo-VoiceCall-v1280",
                                  "spare1"};
  return convert_enum_idx(options, 8, value, "establishment_cause_e");
}

// EstablishmentCause-5GC ::= ENUMERATED
std::string establishment_cause_minus5_gc_opts::to_string() const
{
  static const char* options[] = {
      "emergency", "highPriorityAccess", "mt-Access", "mo-Signalling", "mo-Data", "mo-VoiceCall", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "establishment_cause_minus5_gc_e");
}

// InitialUE-Identity ::= CHOICE
void init_ue_id_c::destroy_()
{
  switch (type_) {
    case types::s_tmsi:
      c.destroy<s_tmsi_s>();
      break;
    case types::random_value:
      c.destroy<fixed_bitstring<40> >();
      break;
    default:
      break;
  }
}
void init_ue_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::s_tmsi:
      c.init<s_tmsi_s>();
      break;
    case types::random_value:
      c.init<fixed_bitstring<40> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
  }
}
init_ue_id_c::init_ue_id_c(const init_ue_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::s_tmsi:
      c.init(other.c.get<s_tmsi_s>());
      break;
    case types::random_value:
      c.init(other.c.get<fixed_bitstring<40> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
  }
}
init_ue_id_c& init_ue_id_c::operator=(const init_ue_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::s_tmsi:
      c.set(other.c.get<s_tmsi_s>());
      break;
    case types::random_value:
      c.set(other.c.get<fixed_bitstring<40> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
  }

  return *this;
}
void init_ue_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::s_tmsi:
      j.write_fieldname("s-TMSI");
      c.get<s_tmsi_s>().to_json(j);
      break;
    case types::random_value:
      j.write_str("randomValue", c.get<fixed_bitstring<40> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
  }
  j.end_obj();
}
SRSASN_CODE init_ue_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::s_tmsi:
      HANDLE_CODE(c.get<s_tmsi_s>().pack(bref));
      break;
    case types::random_value:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE init_ue_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::s_tmsi:
      HANDLE_CODE(c.get<s_tmsi_s>().unpack(bref));
      break;
    case types::random_value:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// InitialUE-Identity-5GC ::= CHOICE
void init_ue_id_minus5_gc_c::destroy_()
{
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_part1:
      c.destroy<fixed_bitstring<40> >();
      break;
    case types::random_value:
      c.destroy<fixed_bitstring<40> >();
      break;
    default:
      break;
  }
}
void init_ue_id_minus5_gc_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_part1:
      c.init<fixed_bitstring<40> >();
      break;
    case types::random_value:
      c.init<fixed_bitstring<40> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_minus5_gc_c");
  }
}
init_ue_id_minus5_gc_c::init_ue_id_minus5_gc_c(const init_ue_id_minus5_gc_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_part1:
      c.init(other.c.get<fixed_bitstring<40> >());
      break;
    case types::random_value:
      c.init(other.c.get<fixed_bitstring<40> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_minus5_gc_c");
  }
}
init_ue_id_minus5_gc_c& init_ue_id_minus5_gc_c::operator=(const init_ue_id_minus5_gc_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_part1:
      c.set(other.c.get<fixed_bitstring<40> >());
      break;
    case types::random_value:
      c.set(other.c.get<fixed_bitstring<40> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_minus5_gc_c");
  }

  return *this;
}
void init_ue_id_minus5_gc_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_part1:
      j.write_str("ng-5G-S-TMSI-Part1", c.get<fixed_bitstring<40> >().to_string());
      break;
    case types::random_value:
      j.write_str("randomValue", c.get<fixed_bitstring<40> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_minus5_gc_c");
  }
  j.end_obj();
}
SRSASN_CODE init_ue_id_minus5_gc_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_part1:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().pack(bref));
      break;
    case types::random_value:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_minus5_gc_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE init_ue_id_minus5_gc_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ng_minus5_g_s_tmsi_part1:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().unpack(bref));
      break;
    case types::random_value:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "init_ue_id_minus5_gc_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// ReestabUE-Identity ::= SEQUENCE
SRSASN_CODE reestab_ue_id_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(c_rnti.pack(bref));
  HANDLE_CODE(pack_integer(bref, pci, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(short_mac_i.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE reestab_ue_id_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(c_rnti.unpack(bref));
  HANDLE_CODE(unpack_integer(pci, bref, (uint16_t)0u, (uint16_t)503u));
  HANDLE_CODE(short_mac_i.unpack(bref));

  return SRSASN_SUCCESS;
}
void reestab_ue_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("c-RNTI", c_rnti.to_string());
  j.write_int("physCellId", pci);
  j.write_str("shortMAC-I", short_mac_i.to_string());
  j.end_obj();
}

// ReestablishmentCause ::= ENUMERATED
std::string reest_cause_opts::to_string() const
{
  static const char* options[] = {"reconfigurationFailure", "handoverFailure", "otherFailure", "spare1"};
  return convert_enum_idx(options, 4, value, "reest_cause_e");
}

// ResumeCause ::= ENUMERATED
std::string resume_cause_opts::to_string() const
{
  static const char* options[] = {"emergency",
                                  "highPriorityAccess",
                                  "mt-Access",
                                  "mo-Signalling",
                                  "mo-Data",
                                  "delayTolerantAccess-v1020",
                                  "mo-VoiceCall-v1280",
                                  "spare1"};
  return convert_enum_idx(options, 8, value, "resume_cause_e");
}

// ResumeCause-r15 ::= ENUMERATED
std::string resume_cause_r15_opts::to_string() const
{
  static const char* options[] = {"emergency",
                                  "highPriorityAccess",
                                  "mt-Access",
                                  "mo-Signalling",
                                  "mo-Data",
                                  "rna-Update",
                                  "mo-VoiceCall",
                                  "spare1"};
  return convert_enum_idx(options, 8, value, "resume_cause_r15_e");
}

// RRCConnectionReestablishmentRequest-r8-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_request_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ue_id.pack(bref));
  HANDLE_CODE(reest_cause.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_request_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ue_id.unpack(bref));
  HANDLE_CODE(reest_cause.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_request_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-Identity");
  ue_id.to_json(j);
  j.write_str("reestablishmentCause", reest_cause.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCConnectionRequest-5GC-r15-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_request_minus5_gc_r15_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ue_id.pack(bref));
  HANDLE_CODE(establishment_cause.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_request_minus5_gc_r15_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ue_id.unpack(bref));
  HANDLE_CODE(establishment_cause.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_request_minus5_gc_r15_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-Identity");
  ue_id.to_json(j);
  j.write_str("establishmentCause", establishment_cause.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCConnectionRequest-r8-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_request_r8_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ue_id.pack(bref));
  HANDLE_CODE(establishment_cause.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_request_r8_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ue_id.unpack(bref));
  HANDLE_CODE(establishment_cause.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_request_r8_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ue-Identity");
  ue_id.to_json(j);
  j.write_str("establishmentCause", establishment_cause.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

// RRCConnectionResumeRequest-5GC-r15-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_request_minus5_gc_r15_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(resume_id_r15.pack(bref));
  HANDLE_CODE(short_resume_mac_i_r15.pack(bref));
  HANDLE_CODE(resume_cause_r15.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_request_minus5_gc_r15_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(resume_id_r15.unpack(bref));
  HANDLE_CODE(short_resume_mac_i_r15.unpack(bref));
  HANDLE_CODE(resume_cause_r15.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_request_minus5_gc_r15_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("resumeIdentity-r15");
  resume_id_r15.to_json(j);
  j.write_str("shortResumeMAC-I-r15", short_resume_mac_i_r15.to_string());
  j.write_str("resumeCause-r15", resume_cause_r15.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

void rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_::destroy_()
{
  switch (type_) {
    case types::full_i_rnti_r15:
      c.destroy<fixed_bitstring<40> >();
      break;
    case types::short_i_rnti_r15:
      c.destroy<fixed_bitstring<24> >();
      break;
    default:
      break;
  }
}
void rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::full_i_rnti_r15:
      c.init<fixed_bitstring<40> >();
      break;
    case types::short_i_rnti_r15:
      c.init<fixed_bitstring<24> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_");
  }
}
rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_::resume_id_r15_c_(
    const rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::full_i_rnti_r15:
      c.init(other.c.get<fixed_bitstring<40> >());
      break;
    case types::short_i_rnti_r15:
      c.init(other.c.get<fixed_bitstring<24> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_");
  }
}
rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_&
rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_::operator=(
    const rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::full_i_rnti_r15:
      c.set(other.c.get<fixed_bitstring<40> >());
      break;
    case types::short_i_rnti_r15:
      c.set(other.c.get<fixed_bitstring<24> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_");
  }

  return *this;
}
void rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::full_i_rnti_r15:
      j.write_str("fullI-RNTI-r15", c.get<fixed_bitstring<40> >().to_string());
      break;
    case types::short_i_rnti_r15:
      j.write_str("shortI-RNTI-r15", c.get<fixed_bitstring<24> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::full_i_rnti_r15:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().pack(bref));
      break;
    case types::short_i_rnti_r15:
      HANDLE_CODE(c.get<fixed_bitstring<24> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::full_i_rnti_r15:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().unpack(bref));
      break;
    case types::short_i_rnti_r15:
      HANDLE_CODE(c.get<fixed_bitstring<24> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_minus5_gc_r15_ies_s::resume_id_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionResumeRequest-r13-IEs ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_request_r13_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(resume_id_r13.pack(bref));
  HANDLE_CODE(short_resume_mac_i_r13.pack(bref));
  HANDLE_CODE(resume_cause_r13.pack(bref));
  HANDLE_CODE(spare.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_request_r13_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(resume_id_r13.unpack(bref));
  HANDLE_CODE(short_resume_mac_i_r13.unpack(bref));
  HANDLE_CODE(resume_cause_r13.unpack(bref));
  HANDLE_CODE(spare.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_request_r13_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("resumeIdentity-r13");
  resume_id_r13.to_json(j);
  j.write_str("shortResumeMAC-I-r13", short_resume_mac_i_r13.to_string());
  j.write_str("resumeCause-r13", resume_cause_r13.to_string());
  j.write_str("spare", spare.to_string());
  j.end_obj();
}

void rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_::destroy_()
{
  switch (type_) {
    case types::resume_id_r13:
      c.destroy<fixed_bitstring<40> >();
      break;
    case types::truncated_resume_id_r13:
      c.destroy<fixed_bitstring<24> >();
      break;
    default:
      break;
  }
}
void rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::resume_id_r13:
      c.init<fixed_bitstring<40> >();
      break;
    case types::truncated_resume_id_r13:
      c.init<fixed_bitstring<24> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_");
  }
}
rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_::resume_id_r13_c_(
    const rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::resume_id_r13:
      c.init(other.c.get<fixed_bitstring<40> >());
      break;
    case types::truncated_resume_id_r13:
      c.init(other.c.get<fixed_bitstring<24> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_");
  }
}
rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_& rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_::operator=(
    const rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::resume_id_r13:
      c.set(other.c.get<fixed_bitstring<40> >());
      break;
    case types::truncated_resume_id_r13:
      c.set(other.c.get<fixed_bitstring<24> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_");
  }

  return *this;
}
void rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::resume_id_r13:
      j.write_str("resumeID-r13", c.get<fixed_bitstring<40> >().to_string());
      break;
    case types::truncated_resume_id_r13:
      j.write_str("truncatedResumeID-r13", c.get<fixed_bitstring<24> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::resume_id_r13:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().pack(bref));
      break;
    case types::truncated_resume_id_r13:
      HANDLE_CODE(c.get<fixed_bitstring<24> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::resume_id_r13:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().unpack(bref));
      break;
    case types::truncated_resume_id_r13:
      HANDLE_CODE(c.get<fixed_bitstring<24> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_r13_ies_s::resume_id_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCEarlyDataRequest-r15-IEs ::= SEQUENCE
SRSASN_CODE rrc_early_data_request_r15_ies_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(non_crit_ext_present, 1));

  HANDLE_CODE(s_tmsi_r15.pack(bref));
  HANDLE_CODE(establishment_cause_r15.pack(bref));
  HANDLE_CODE(ded_info_nas_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_early_data_request_r15_ies_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(non_crit_ext_present, 1));

  HANDLE_CODE(s_tmsi_r15.unpack(bref));
  HANDLE_CODE(establishment_cause_r15.unpack(bref));
  HANDLE_CODE(ded_info_nas_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_early_data_request_r15_ies_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("s-TMSI-r15");
  s_tmsi_r15.to_json(j);
  j.write_str("establishmentCause-r15", establishment_cause_r15.to_string());
  j.write_str("dedicatedInfoNAS-r15", ded_info_nas_r15.to_string());
  if (non_crit_ext_present) {
    j.write_fieldname("nonCriticalExtension");
    j.start_obj();
    j.end_obj();
  }
  j.end_obj();
}

std::string rrc_early_data_request_r15_ies_s::establishment_cause_r15_opts::to_string() const
{
  static const char* options[] = {"mo-Data-r15", "delayTolerantAccess-r15"};
  return convert_enum_idx(options, 2, value, "rrc_early_data_request_r15_ies_s::establishment_cause_r15_e_");
}

// RRCConnectionReestablishmentRequest ::= SEQUENCE
SRSASN_CODE rrc_conn_reest_request_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_request_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_reest_request_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_reest_request_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::rrc_conn_reest_request_r8:
      c.destroy<rrc_conn_reest_request_r8_ies_s>();
      break;
    default:
      break;
  }
}
void rrc_conn_reest_request_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_conn_reest_request_r8:
      c.init<rrc_conn_reest_request_r8_ies_s>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_s::crit_exts_c_");
  }
}
rrc_conn_reest_request_s::crit_exts_c_::crit_exts_c_(const rrc_conn_reest_request_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_conn_reest_request_r8:
      c.init(other.c.get<rrc_conn_reest_request_r8_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_s::crit_exts_c_");
  }
}
rrc_conn_reest_request_s::crit_exts_c_&
rrc_conn_reest_request_s::crit_exts_c_::operator=(const rrc_conn_reest_request_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_conn_reest_request_r8:
      c.set(other.c.get<rrc_conn_reest_request_r8_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_s::crit_exts_c_");
  }

  return *this;
}
void rrc_conn_reest_request_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_reest_request_r8:
      j.write_fieldname("rrcConnectionReestablishmentRequest-r8");
      c.get<rrc_conn_reest_request_r8_ies_s>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_reest_request_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_reest_request_r8:
      HANDLE_CODE(c.get<rrc_conn_reest_request_r8_ies_s>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_reest_request_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_reest_request_r8:
      HANDLE_CODE(c.get<rrc_conn_reest_request_r8_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_reest_request_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionRequest ::= SEQUENCE
SRSASN_CODE rrc_conn_request_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_request_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_request_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_request_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::rrc_conn_request_r8:
      c.destroy<rrc_conn_request_r8_ies_s>();
      break;
    case types::rrc_conn_request_r15:
      c.destroy<rrc_conn_request_minus5_gc_r15_ies_s>();
      break;
    default:
      break;
  }
}
void rrc_conn_request_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_conn_request_r8:
      c.init<rrc_conn_request_r8_ies_s>();
      break;
    case types::rrc_conn_request_r15:
      c.init<rrc_conn_request_minus5_gc_r15_ies_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_request_s::crit_exts_c_");
  }
}
rrc_conn_request_s::crit_exts_c_::crit_exts_c_(const rrc_conn_request_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_conn_request_r8:
      c.init(other.c.get<rrc_conn_request_r8_ies_s>());
      break;
    case types::rrc_conn_request_r15:
      c.init(other.c.get<rrc_conn_request_minus5_gc_r15_ies_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_request_s::crit_exts_c_");
  }
}
rrc_conn_request_s::crit_exts_c_&
rrc_conn_request_s::crit_exts_c_::operator=(const rrc_conn_request_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_conn_request_r8:
      c.set(other.c.get<rrc_conn_request_r8_ies_s>());
      break;
    case types::rrc_conn_request_r15:
      c.set(other.c.get<rrc_conn_request_minus5_gc_r15_ies_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_request_s::crit_exts_c_");
  }

  return *this;
}
void rrc_conn_request_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_request_r8:
      j.write_fieldname("rrcConnectionRequest-r8");
      c.get<rrc_conn_request_r8_ies_s>().to_json(j);
      break;
    case types::rrc_conn_request_r15:
      j.write_fieldname("rrcConnectionRequest-r15");
      c.get<rrc_conn_request_minus5_gc_r15_ies_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_request_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_request_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_request_r8:
      HANDLE_CODE(c.get<rrc_conn_request_r8_ies_s>().pack(bref));
      break;
    case types::rrc_conn_request_r15:
      HANDLE_CODE(c.get<rrc_conn_request_minus5_gc_r15_ies_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_request_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_request_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_request_r8:
      HANDLE_CODE(c.get<rrc_conn_request_r8_ies_s>().unpack(bref));
      break;
    case types::rrc_conn_request_r15:
      HANDLE_CODE(c.get<rrc_conn_request_minus5_gc_r15_ies_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_request_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCConnectionResumeRequest-r13 ::= SEQUENCE
SRSASN_CODE rrc_conn_resume_request_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_request_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_conn_resume_request_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_conn_resume_request_r13_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::rrc_conn_resume_request_r13:
      c.destroy<rrc_conn_resume_request_r13_ies_s>();
      break;
    case types::rrc_conn_resume_request_r15:
      c.destroy<rrc_conn_resume_request_minus5_gc_r15_ies_s>();
      break;
    default:
      break;
  }
}
void rrc_conn_resume_request_r13_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_conn_resume_request_r13:
      c.init<rrc_conn_resume_request_r13_ies_s>();
      break;
    case types::rrc_conn_resume_request_r15:
      c.init<rrc_conn_resume_request_minus5_gc_r15_ies_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_r13_s::crit_exts_c_");
  }
}
rrc_conn_resume_request_r13_s::crit_exts_c_::crit_exts_c_(const rrc_conn_resume_request_r13_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_conn_resume_request_r13:
      c.init(other.c.get<rrc_conn_resume_request_r13_ies_s>());
      break;
    case types::rrc_conn_resume_request_r15:
      c.init(other.c.get<rrc_conn_resume_request_minus5_gc_r15_ies_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_r13_s::crit_exts_c_");
  }
}
rrc_conn_resume_request_r13_s::crit_exts_c_&
rrc_conn_resume_request_r13_s::crit_exts_c_::operator=(const rrc_conn_resume_request_r13_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_conn_resume_request_r13:
      c.set(other.c.get<rrc_conn_resume_request_r13_ies_s>());
      break;
    case types::rrc_conn_resume_request_r15:
      c.set(other.c.get<rrc_conn_resume_request_minus5_gc_r15_ies_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_r13_s::crit_exts_c_");
  }

  return *this;
}
void rrc_conn_resume_request_r13_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_resume_request_r13:
      j.write_fieldname("rrcConnectionResumeRequest-r13");
      c.get<rrc_conn_resume_request_r13_ies_s>().to_json(j);
      break;
    case types::rrc_conn_resume_request_r15:
      j.write_fieldname("rrcConnectionResumeRequest-r15");
      c.get<rrc_conn_resume_request_minus5_gc_r15_ies_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_r13_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_conn_resume_request_r13_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_resume_request_r13:
      HANDLE_CODE(c.get<rrc_conn_resume_request_r13_ies_s>().pack(bref));
      break;
    case types::rrc_conn_resume_request_r15:
      HANDLE_CODE(c.get<rrc_conn_resume_request_minus5_gc_r15_ies_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_r13_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_conn_resume_request_r13_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_resume_request_r13:
      HANDLE_CODE(c.get<rrc_conn_resume_request_r13_ies_s>().unpack(bref));
      break;
    case types::rrc_conn_resume_request_r15:
      HANDLE_CODE(c.get<rrc_conn_resume_request_minus5_gc_r15_ies_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_conn_resume_request_r13_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// RRCEarlyDataRequest-r15 ::= SEQUENCE
SRSASN_CODE rrc_early_data_request_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(crit_exts.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_early_data_request_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(crit_exts.unpack(bref));

  return SRSASN_SUCCESS;
}
void rrc_early_data_request_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("criticalExtensions");
  crit_exts.to_json(j);
  j.end_obj();
}

void rrc_early_data_request_r15_s::crit_exts_c_::destroy_()
{
  switch (type_) {
    case types::rrc_early_data_request_r15:
      c.destroy<rrc_early_data_request_r15_ies_s>();
      break;
    default:
      break;
  }
}
void rrc_early_data_request_r15_s::crit_exts_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_early_data_request_r15:
      c.init<rrc_early_data_request_r15_ies_s>();
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_request_r15_s::crit_exts_c_");
  }
}
rrc_early_data_request_r15_s::crit_exts_c_::crit_exts_c_(const rrc_early_data_request_r15_s::crit_exts_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_early_data_request_r15:
      c.init(other.c.get<rrc_early_data_request_r15_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_request_r15_s::crit_exts_c_");
  }
}
rrc_early_data_request_r15_s::crit_exts_c_&
rrc_early_data_request_r15_s::crit_exts_c_::operator=(const rrc_early_data_request_r15_s::crit_exts_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_early_data_request_r15:
      c.set(other.c.get<rrc_early_data_request_r15_ies_s>());
      break;
    case types::crit_exts_future:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_request_r15_s::crit_exts_c_");
  }

  return *this;
}
void rrc_early_data_request_r15_s::crit_exts_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_early_data_request_r15:
      j.write_fieldname("rrcEarlyDataRequest-r15");
      c.get<rrc_early_data_request_r15_ies_s>().to_json(j);
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_request_r15_s::crit_exts_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_early_data_request_r15_s::crit_exts_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_early_data_request_r15:
      HANDLE_CODE(c.get<rrc_early_data_request_r15_ies_s>().pack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_request_r15_s::crit_exts_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_early_data_request_r15_s::crit_exts_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_early_data_request_r15:
      HANDLE_CODE(c.get<rrc_early_data_request_r15_ies_s>().unpack(bref));
      break;
    case types::crit_exts_future:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_early_data_request_r15_s::crit_exts_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UL-CCCH-MessageType ::= CHOICE
void ul_ccch_msg_type_c::destroy_()
{
  switch (type_) {
    case types::c1:
      c.destroy<c1_c_>();
      break;
    case types::msg_class_ext:
      c.destroy<msg_class_ext_c_>();
      break;
    default:
      break;
  }
}
void ul_ccch_msg_type_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c1:
      c.init<c1_c_>();
      break;
    case types::msg_class_ext:
      c.init<msg_class_ext_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c");
  }
}
ul_ccch_msg_type_c::ul_ccch_msg_type_c(const ul_ccch_msg_type_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c1:
      c.init(other.c.get<c1_c_>());
      break;
    case types::msg_class_ext:
      c.init(other.c.get<msg_class_ext_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c");
  }
}
ul_ccch_msg_type_c& ul_ccch_msg_type_c::operator=(const ul_ccch_msg_type_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c1:
      c.set(other.c.get<c1_c_>());
      break;
    case types::msg_class_ext:
      c.set(other.c.get<msg_class_ext_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c");
  }

  return *this;
}
void ul_ccch_msg_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c1:
      j.write_fieldname("c1");
      c.get<c1_c_>().to_json(j);
      break;
    case types::msg_class_ext:
      j.write_fieldname("messageClassExtension");
      c.get<msg_class_ext_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c");
  }
  j.end_obj();
}
SRSASN_CODE ul_ccch_msg_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().pack(bref));
      break;
    case types::msg_class_ext:
      HANDLE_CODE(c.get<msg_class_ext_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch_msg_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c1:
      HANDLE_CODE(c.get<c1_c_>().unpack(bref));
      break;
    case types::msg_class_ext:
      HANDLE_CODE(c.get<msg_class_ext_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_ccch_msg_type_c::c1_c_::destroy_()
{
  switch (type_) {
    case types::rrc_conn_reest_request:
      c.destroy<rrc_conn_reest_request_s>();
      break;
    case types::rrc_conn_request:
      c.destroy<rrc_conn_request_s>();
      break;
    default:
      break;
  }
}
void ul_ccch_msg_type_c::c1_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::rrc_conn_reest_request:
      c.init<rrc_conn_reest_request_s>();
      break;
    case types::rrc_conn_request:
      c.init<rrc_conn_request_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
  }
}
ul_ccch_msg_type_c::c1_c_::c1_c_(const ul_ccch_msg_type_c::c1_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::rrc_conn_reest_request:
      c.init(other.c.get<rrc_conn_reest_request_s>());
      break;
    case types::rrc_conn_request:
      c.init(other.c.get<rrc_conn_request_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
  }
}
ul_ccch_msg_type_c::c1_c_& ul_ccch_msg_type_c::c1_c_::operator=(const ul_ccch_msg_type_c::c1_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::rrc_conn_reest_request:
      c.set(other.c.get<rrc_conn_reest_request_s>());
      break;
    case types::rrc_conn_request:
      c.set(other.c.get<rrc_conn_request_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
  }

  return *this;
}
void ul_ccch_msg_type_c::c1_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_conn_reest_request:
      j.write_fieldname("rrcConnectionReestablishmentRequest");
      c.get<rrc_conn_reest_request_s>().to_json(j);
      break;
    case types::rrc_conn_request:
      j.write_fieldname("rrcConnectionRequest");
      c.get<rrc_conn_request_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_ccch_msg_type_c::c1_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_conn_reest_request:
      HANDLE_CODE(c.get<rrc_conn_reest_request_s>().pack(bref));
      break;
    case types::rrc_conn_request:
      HANDLE_CODE(c.get<rrc_conn_request_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch_msg_type_c::c1_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_conn_reest_request:
      HANDLE_CODE(c.get<rrc_conn_reest_request_s>().unpack(bref));
      break;
    case types::rrc_conn_request:
      HANDLE_CODE(c.get<rrc_conn_request_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::c1_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_ccch_msg_type_c::msg_class_ext_c_::destroy_()
{
  switch (type_) {
    case types::c2:
      c.destroy<c2_c_>();
      break;
    case types::msg_class_ext_future_r13:
      c.destroy<msg_class_ext_future_r13_c_>();
      break;
    default:
      break;
  }
}
void ul_ccch_msg_type_c::msg_class_ext_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c2:
      c.init<c2_c_>();
      break;
    case types::msg_class_ext_future_r13:
      c.init<msg_class_ext_future_r13_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::msg_class_ext_c_");
  }
}
ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_c_(const ul_ccch_msg_type_c::msg_class_ext_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c2:
      c.init(other.c.get<c2_c_>());
      break;
    case types::msg_class_ext_future_r13:
      c.init(other.c.get<msg_class_ext_future_r13_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::msg_class_ext_c_");
  }
}
ul_ccch_msg_type_c::msg_class_ext_c_&
ul_ccch_msg_type_c::msg_class_ext_c_::operator=(const ul_ccch_msg_type_c::msg_class_ext_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c2:
      c.set(other.c.get<c2_c_>());
      break;
    case types::msg_class_ext_future_r13:
      c.set(other.c.get<msg_class_ext_future_r13_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::msg_class_ext_c_");
  }

  return *this;
}
void ul_ccch_msg_type_c::msg_class_ext_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c2:
      j.write_fieldname("c2");
      c.get<c2_c_>().to_json(j);
      break;
    case types::msg_class_ext_future_r13:
      j.write_fieldname("messageClassExtensionFuture-r13");
      c.get<msg_class_ext_future_r13_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::msg_class_ext_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_ccch_msg_type_c::msg_class_ext_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c2:
      HANDLE_CODE(c.get<c2_c_>().pack(bref));
      break;
    case types::msg_class_ext_future_r13:
      HANDLE_CODE(c.get<msg_class_ext_future_r13_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::msg_class_ext_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch_msg_type_c::msg_class_ext_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c2:
      HANDLE_CODE(c.get<c2_c_>().unpack(bref));
      break;
    case types::msg_class_ext_future_r13:
      HANDLE_CODE(c.get<msg_class_ext_future_r13_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::msg_class_ext_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_ccch_msg_type_c::msg_class_ext_c_::c2_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rrcConnectionResumeRequest-r13");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE ul_ccch_msg_type_c::msg_class_ext_c_::c2_c_::pack(bit_ref& bref) const
{
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch_msg_type_c::msg_class_ext_c_::c2_c_::unpack(cbit_ref& bref)
{
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

void ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::destroy_()
{
  switch (type_) {
    case types::c3:
      c.destroy<c3_c_>();
      break;
    default:
      break;
  }
}
void ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::c3:
      c.init<c3_c_>();
      break;
    case types::msg_class_ext_future_r15:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_");
  }
}
ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::msg_class_ext_future_r13_c_(
    const ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::c3:
      c.init(other.c.get<c3_c_>());
      break;
    case types::msg_class_ext_future_r15:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_");
  }
}
ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_&
ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::operator=(
    const ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::c3:
      c.set(other.c.get<c3_c_>());
      break;
    case types::msg_class_ext_future_r15:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_");
  }

  return *this;
}
void ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::c3:
      j.write_fieldname("c3");
      c.get<c3_c_>().to_json(j);
      break;
    case types::msg_class_ext_future_r15:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::c3:
      HANDLE_CODE(c.get<c3_c_>().pack(bref));
      break;
    case types::msg_class_ext_future_r15:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::c3:
      HANDLE_CODE(c.get<c3_c_>().unpack(bref));
      break;
    case types::msg_class_ext_future_r15:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

void ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::c3_c_::set(types::options e)
{
  type_ = e;
}
void ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::c3_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::rrc_early_data_request_r15:
      j.write_fieldname("rrcEarlyDataRequest-r15");
      c.to_json(j);
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::c3_c_");
  }
  j.end_obj();
}
SRSASN_CODE ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::c3_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::rrc_early_data_request_r15:
      HANDLE_CODE(c.pack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::c3_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::c3_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::rrc_early_data_request_r15:
      HANDLE_CODE(c.unpack(bref));
      break;
    case types::spare3:
      break;
    case types::spare2:
      break;
    case types::spare1:
      break;
    default:
      log_invalid_choice_id(type_, "ul_ccch_msg_type_c::msg_class_ext_c_::msg_class_ext_future_r13_c_::c3_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// UL-CCCH-Message ::= SEQUENCE
SRSASN_CODE ul_ccch_msg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(msg.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ccch_msg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(msg.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void ul_ccch_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("UL-CCCH-Message");
  j.write_fieldname("message");
  msg.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}
