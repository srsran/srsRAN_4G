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

#include "srsran/asn1/e2sm.h"
#include <sstream>

using namespace asn1;
using namespace asn1::e2sm;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// EUTRA-CGI ::= SEQUENCE
SRSASN_CODE eutra_cgi_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(eutra_cell_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE eutra_cgi_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(eutra_cell_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void eutra_cgi_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("pLMNIdentity", plmn_id.to_string());
  j.write_str("eUTRACellIdentity", eutra_cell_id.to_string());
  j.end_obj();
}

// NR-CGI ::= SEQUENCE
SRSASN_CODE nr_cgi_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(nrcell_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_cgi_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(nrcell_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void nr_cgi_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("pLMNIdentity", plmn_id.to_string());
  j.write_str("nRCellIdentity", nrcell_id.to_string());
  j.end_obj();
}

// CGI ::= CHOICE
void cgi_c::destroy_()
{
  switch (type_) {
    case types::nr_cgi:
      c.destroy<nr_cgi_s>();
      break;
    case types::eutra_cgi:
      c.destroy<eutra_cgi_s>();
      break;
    default:
      break;
  }
}
void cgi_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nr_cgi:
      c.init<nr_cgi_s>();
      break;
    case types::eutra_cgi:
      c.init<eutra_cgi_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cgi_c");
  }
}
cgi_c::cgi_c(const cgi_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr_cgi:
      c.init(other.c.get<nr_cgi_s>());
      break;
    case types::eutra_cgi:
      c.init(other.c.get<eutra_cgi_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cgi_c");
  }
}
cgi_c& cgi_c::operator=(const cgi_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr_cgi:
      c.set(other.c.get<nr_cgi_s>());
      break;
    case types::eutra_cgi:
      c.set(other.c.get<eutra_cgi_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cgi_c");
  }

  return *this;
}
nr_cgi_s& cgi_c::set_nr_cgi()
{
  set(types::nr_cgi);
  return c.get<nr_cgi_s>();
}
eutra_cgi_s& cgi_c::set_eutra_cgi()
{
  set(types::eutra_cgi);
  return c.get<eutra_cgi_s>();
}
void cgi_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr_cgi:
      j.write_fieldname("nR-CGI");
      c.get<nr_cgi_s>().to_json(j);
      break;
    case types::eutra_cgi:
      j.write_fieldname("eUTRA-CGI");
      c.get<eutra_cgi_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "cgi_c");
  }
  j.end_obj();
}
SRSASN_CODE cgi_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr_cgi:
      HANDLE_CODE(c.get<nr_cgi_s>().pack(bref));
      break;
    case types::eutra_cgi:
      HANDLE_CODE(c.get<eutra_cgi_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cgi_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cgi_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr_cgi:
      HANDLE_CODE(c.get<nr_cgi_s>().unpack(bref));
      break;
    case types::eutra_cgi:
      HANDLE_CODE(c.get<eutra_cgi_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "cgi_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* cgi_c::types_opts::to_string() const
{
  static const char* options[] = {"nR-CGI", "eUTRA-CGI"};
  return convert_enum_idx(options, 2, value, "cgi_c::types");
}

// GUAMI ::= SEQUENCE
SRSASN_CODE guami_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(amf_region_id.pack(bref));
  HANDLE_CODE(amf_set_id.pack(bref));
  HANDLE_CODE(amf_pointer.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE guami_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(amf_region_id.unpack(bref));
  HANDLE_CODE(amf_set_id.unpack(bref));
  HANDLE_CODE(amf_pointer.unpack(bref));

  return SRSASN_SUCCESS;
}
void guami_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("pLMNIdentity", plmn_id.to_string());
  j.write_str("aMFRegionID", amf_region_id.to_string());
  j.write_str("aMFSetID", amf_set_id.to_string());
  j.write_str("aMFPointer", amf_pointer.to_string());
  j.end_obj();
}

// GUMMEI ::= SEQUENCE
SRSASN_CODE gummei_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(mme_group_id.pack(bref));
  HANDLE_CODE(mme_code.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE gummei_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(mme_group_id.unpack(bref));
  HANDLE_CODE(mme_code.unpack(bref));

  return SRSASN_SUCCESS;
}
void gummei_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("pLMN-Identity", plmn_id.to_string());
  j.write_str("mME-Group-ID", mme_group_id.to_string());
  j.write_str("mME-Code", mme_code.to_string());
  j.end_obj();
}

// CoreCPID ::= CHOICE
void core_cpid_c::destroy_()
{
  switch (type_) {
    case types::five_gc:
      c.destroy<guami_s>();
      break;
    case types::epc:
      c.destroy<gummei_s>();
      break;
    default:
      break;
  }
}
void core_cpid_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::five_gc:
      c.init<guami_s>();
      break;
    case types::epc:
      c.init<gummei_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "core_cpid_c");
  }
}
core_cpid_c::core_cpid_c(const core_cpid_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::five_gc:
      c.init(other.c.get<guami_s>());
      break;
    case types::epc:
      c.init(other.c.get<gummei_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "core_cpid_c");
  }
}
core_cpid_c& core_cpid_c::operator=(const core_cpid_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::five_gc:
      c.set(other.c.get<guami_s>());
      break;
    case types::epc:
      c.set(other.c.get<gummei_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "core_cpid_c");
  }

  return *this;
}
guami_s& core_cpid_c::set_five_gc()
{
  set(types::five_gc);
  return c.get<guami_s>();
}
gummei_s& core_cpid_c::set_epc()
{
  set(types::epc);
  return c.get<gummei_s>();
}
void core_cpid_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::five_gc:
      j.write_fieldname("fiveGC");
      c.get<guami_s>().to_json(j);
      break;
    case types::epc:
      j.write_fieldname("ePC");
      c.get<gummei_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "core_cpid_c");
  }
  j.end_obj();
}
SRSASN_CODE core_cpid_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::five_gc:
      HANDLE_CODE(c.get<guami_s>().pack(bref));
      break;
    case types::epc:
      HANDLE_CODE(c.get<gummei_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "core_cpid_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE core_cpid_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::five_gc:
      HANDLE_CODE(c.get<guami_s>().unpack(bref));
      break;
    case types::epc:
      HANDLE_CODE(c.get<gummei_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "core_cpid_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* core_cpid_c::types_opts::to_string() const
{
  static const char* options[] = {"fiveGC", "ePC"};
  return convert_enum_idx(options, 2, value, "core_cpid_c::types");
}
uint8_t core_cpid_c::types_opts::to_number() const
{
  static const uint8_t options[] = {5};
  return map_enum_number(options, 1, value, "core_cpid_c::types");
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
  j.write_str("pLMNIdentity", plmn_id.to_string());
  j.write_fieldname("eNB-ID");
  enb_id.to_json(j);
  j.end_obj();
}

// GNB-ID ::= CHOICE
void gnb_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("gNB-ID", c.to_string());
  j.end_obj();
}
SRSASN_CODE gnb_id_c::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE gnb_id_c::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "gnb_id_c");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* gnb_id_c::types_opts::to_string() const
{
  static const char* options[] = {"gNB-ID"};
  return convert_enum_idx(options, 1, value, "gnb_id_c::types");
}

// GlobalGNB-ID ::= SEQUENCE
SRSASN_CODE global_gnb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(gnb_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE global_gnb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(gnb_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void global_gnb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("pLMNIdentity", plmn_id.to_string());
  j.write_fieldname("gNB-ID");
  gnb_id.to_json(j);
  j.end_obj();
}

// NgENB-ID ::= CHOICE
void ng_enb_id_c::destroy_()
{
  switch (type_) {
    case types::macro_ng_enb_id:
      c.destroy<fixed_bitstring<20, false, true> >();
      break;
    case types::short_macro_ng_enb_id:
      c.destroy<fixed_bitstring<18, false, true> >();
      break;
    case types::long_macro_ng_enb_id:
      c.destroy<fixed_bitstring<21, false, true> >();
      break;
    default:
      break;
  }
}
void ng_enb_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::macro_ng_enb_id:
      c.init<fixed_bitstring<20, false, true> >();
      break;
    case types::short_macro_ng_enb_id:
      c.init<fixed_bitstring<18, false, true> >();
      break;
    case types::long_macro_ng_enb_id:
      c.init<fixed_bitstring<21, false, true> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_enb_id_c");
  }
}
ng_enb_id_c::ng_enb_id_c(const ng_enb_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::macro_ng_enb_id:
      c.init(other.c.get<fixed_bitstring<20, false, true> >());
      break;
    case types::short_macro_ng_enb_id:
      c.init(other.c.get<fixed_bitstring<18, false, true> >());
      break;
    case types::long_macro_ng_enb_id:
      c.init(other.c.get<fixed_bitstring<21, false, true> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_enb_id_c");
  }
}
ng_enb_id_c& ng_enb_id_c::operator=(const ng_enb_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::macro_ng_enb_id:
      c.set(other.c.get<fixed_bitstring<20, false, true> >());
      break;
    case types::short_macro_ng_enb_id:
      c.set(other.c.get<fixed_bitstring<18, false, true> >());
      break;
    case types::long_macro_ng_enb_id:
      c.set(other.c.get<fixed_bitstring<21, false, true> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ng_enb_id_c");
  }

  return *this;
}
fixed_bitstring<20, false, true>& ng_enb_id_c::set_macro_ng_enb_id()
{
  set(types::macro_ng_enb_id);
  return c.get<fixed_bitstring<20, false, true> >();
}
fixed_bitstring<18, false, true>& ng_enb_id_c::set_short_macro_ng_enb_id()
{
  set(types::short_macro_ng_enb_id);
  return c.get<fixed_bitstring<18, false, true> >();
}
fixed_bitstring<21, false, true>& ng_enb_id_c::set_long_macro_ng_enb_id()
{
  set(types::long_macro_ng_enb_id);
  return c.get<fixed_bitstring<21, false, true> >();
}
void ng_enb_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::macro_ng_enb_id:
      j.write_str("macroNgENB-ID", c.get<fixed_bitstring<20, false, true> >().to_string());
      break;
    case types::short_macro_ng_enb_id:
      j.write_str("shortMacroNgENB-ID", c.get<fixed_bitstring<18, false, true> >().to_string());
      break;
    case types::long_macro_ng_enb_id:
      j.write_str("longMacroNgENB-ID", c.get<fixed_bitstring<21, false, true> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "ng_enb_id_c");
  }
  j.end_obj();
}
SRSASN_CODE ng_enb_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::macro_ng_enb_id:
      HANDLE_CODE((c.get<fixed_bitstring<20, false, true> >().pack(bref)));
      break;
    case types::short_macro_ng_enb_id:
      HANDLE_CODE((c.get<fixed_bitstring<18, false, true> >().pack(bref)));
      break;
    case types::long_macro_ng_enb_id:
      HANDLE_CODE((c.get<fixed_bitstring<21, false, true> >().pack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "ng_enb_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ng_enb_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::macro_ng_enb_id:
      HANDLE_CODE((c.get<fixed_bitstring<20, false, true> >().unpack(bref)));
      break;
    case types::short_macro_ng_enb_id:
      HANDLE_CODE((c.get<fixed_bitstring<18, false, true> >().unpack(bref)));
      break;
    case types::long_macro_ng_enb_id:
      HANDLE_CODE((c.get<fixed_bitstring<21, false, true> >().unpack(bref)));
      break;
    default:
      log_invalid_choice_id(type_, "ng_enb_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ng_enb_id_c::types_opts::to_string() const
{
  static const char* options[] = {"macroNgENB-ID", "shortMacroNgENB-ID", "longMacroNgENB-ID"};
  return convert_enum_idx(options, 3, value, "ng_enb_id_c::types");
}

// GlobalNgENB-ID ::= SEQUENCE
SRSASN_CODE global_ng_enb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(ng_enb_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE global_ng_enb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(ng_enb_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void global_ng_enb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("pLMNIdentity", plmn_id.to_string());
  j.write_fieldname("ngENB-ID");
  ng_enb_id.to_json(j);
  j.end_obj();
}

// GlobalRANNodeID ::= CHOICE
void global_ran_node_id_c::destroy_()
{
  switch (type_) {
    case types::global_gnb_id:
      c.destroy<global_gnb_id_s>();
      break;
    case types::global_ng_enb_id:
      c.destroy<global_ng_enb_id_s>();
      break;
    default:
      break;
  }
}
void global_ran_node_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::global_gnb_id:
      c.init<global_gnb_id_s>();
      break;
    case types::global_ng_enb_id:
      c.init<global_ng_enb_id_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "global_ran_node_id_c");
  }
}
global_ran_node_id_c::global_ran_node_id_c(const global_ran_node_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::global_gnb_id:
      c.init(other.c.get<global_gnb_id_s>());
      break;
    case types::global_ng_enb_id:
      c.init(other.c.get<global_ng_enb_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "global_ran_node_id_c");
  }
}
global_ran_node_id_c& global_ran_node_id_c::operator=(const global_ran_node_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::global_gnb_id:
      c.set(other.c.get<global_gnb_id_s>());
      break;
    case types::global_ng_enb_id:
      c.set(other.c.get<global_ng_enb_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "global_ran_node_id_c");
  }

  return *this;
}
global_gnb_id_s& global_ran_node_id_c::set_global_gnb_id()
{
  set(types::global_gnb_id);
  return c.get<global_gnb_id_s>();
}
global_ng_enb_id_s& global_ran_node_id_c::set_global_ng_enb_id()
{
  set(types::global_ng_enb_id);
  return c.get<global_ng_enb_id_s>();
}
void global_ran_node_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::global_gnb_id:
      j.write_fieldname("globalGNB-ID");
      c.get<global_gnb_id_s>().to_json(j);
      break;
    case types::global_ng_enb_id:
      j.write_fieldname("globalNgENB-ID");
      c.get<global_ng_enb_id_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "global_ran_node_id_c");
  }
  j.end_obj();
}
SRSASN_CODE global_ran_node_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::global_gnb_id:
      HANDLE_CODE(c.get<global_gnb_id_s>().pack(bref));
      break;
    case types::global_ng_enb_id:
      HANDLE_CODE(c.get<global_ng_enb_id_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "global_ran_node_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE global_ran_node_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::global_gnb_id:
      HANDLE_CODE(c.get<global_gnb_id_s>().unpack(bref));
      break;
    case types::global_ng_enb_id:
      HANDLE_CODE(c.get<global_ng_enb_id_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "global_ran_node_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* global_ran_node_id_c::types_opts::to_string() const
{
  static const char* options[] = {"globalGNB-ID", "globalNgENB-ID"};
  return convert_enum_idx(options, 2, value, "global_ran_node_id_c::types");
}

// EN-GNB-ID ::= CHOICE
void en_gnb_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("en-gNB-ID", c.to_string());
  j.end_obj();
}
SRSASN_CODE en_gnb_id_c::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE en_gnb_id_c::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "en_gnb_id_c");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* en_gnb_id_c::types_opts::to_string() const
{
  static const char* options[] = {"en-gNB-ID"};
  return convert_enum_idx(options, 1, value, "en_gnb_id_c::types");
}

// GlobalenGNB-ID ::= SEQUENCE
SRSASN_CODE globalen_gnb_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(en_g_nb_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE globalen_gnb_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(en_g_nb_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void globalen_gnb_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("pLMN-Identity", plmn_id.to_string());
  j.write_fieldname("en-gNB-ID");
  en_g_nb_id.to_json(j);
  j.end_obj();
}

// GroupID ::= CHOICE
void group_id_c::destroy_() {}
void group_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
group_id_c::group_id_c(const group_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::five_gc:
      c.init(other.c.get<uint16_t>());
      break;
    case types::epc:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "group_id_c");
  }
}
group_id_c& group_id_c::operator=(const group_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::five_gc:
      c.set(other.c.get<uint16_t>());
      break;
    case types::epc:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "group_id_c");
  }

  return *this;
}
uint16_t& group_id_c::set_five_gc()
{
  set(types::five_gc);
  return c.get<uint16_t>();
}
uint16_t& group_id_c::set_epc()
{
  set(types::epc);
  return c.get<uint16_t>();
}
void group_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::five_gc:
      j.write_int("fiveGC", c.get<uint16_t>());
      break;
    case types::epc:
      j.write_int("ePC", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "group_id_c");
  }
  j.end_obj();
}
SRSASN_CODE group_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::five_gc:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::epc:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "group_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE group_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::five_gc:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::epc:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "group_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* group_id_c::types_opts::to_string() const
{
  static const char* options[] = {"fiveGC", "ePC"};
  return convert_enum_idx(options, 2, value, "group_id_c::types");
}
uint8_t group_id_c::types_opts::to_number() const
{
  static const uint8_t options[] = {5};
  return map_enum_number(options, 1, value, "group_id_c::types");
}

// InterfaceID-E1 ::= SEQUENCE
SRSASN_CODE interface_id_e1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(global_ng_ran_id.pack(bref));
  HANDLE_CODE(pack_integer(bref, gnb_cu_up_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE interface_id_e1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(global_ng_ran_id.unpack(bref));
  HANDLE_CODE(unpack_integer(gnb_cu_up_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
void interface_id_e1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("global-NG-RAN-ID");
  global_ng_ran_id.to_json(j);
  j.write_int("gNB-CU-UP-ID", gnb_cu_up_id);
  j.end_obj();
}

// InterfaceID-F1 ::= SEQUENCE
SRSASN_CODE interface_id_f1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(global_ng_ran_id.pack(bref));
  HANDLE_CODE(pack_integer(bref, gnb_du_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE interface_id_f1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(global_ng_ran_id.unpack(bref));
  HANDLE_CODE(unpack_integer(gnb_du_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
void interface_id_f1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("global-NG-RAN-ID");
  global_ng_ran_id.to_json(j);
  j.write_int("gNB-DU-ID", gnb_du_id);
  j.end_obj();
}

// InterfaceID-NG ::= SEQUENCE
SRSASN_CODE interface_id_ng_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(guami.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE interface_id_ng_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(guami.unpack(bref));

  return SRSASN_SUCCESS;
}
void interface_id_ng_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("guami");
  guami.to_json(j);
  j.end_obj();
}

// InterfaceID-S1 ::= SEQUENCE
SRSASN_CODE interface_id_s1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(gummei.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE interface_id_s1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(gummei.unpack(bref));

  return SRSASN_SUCCESS;
}
void interface_id_s1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("gUMMEI");
  gummei.to_json(j);
  j.end_obj();
}

// InterfaceID-W1 ::= SEQUENCE
SRSASN_CODE interface_id_w1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(global_ng_enb_id.pack(bref));
  HANDLE_CODE(pack_integer(bref, ng_enb_du_id, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE interface_id_w1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(global_ng_enb_id.unpack(bref));
  HANDLE_CODE(unpack_integer(ng_enb_du_id, bref, (uint64_t)0u, (uint64_t)68719476735u, false, true));

  return SRSASN_SUCCESS;
}
void interface_id_w1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("global-ng-eNB-ID");
  global_ng_enb_id.to_json(j);
  j.write_int("ng-eNB-DU-ID", ng_enb_du_id);
  j.end_obj();
}

// InterfaceID-X2 ::= SEQUENCE
SRSASN_CODE interface_id_x2_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(node_type.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE interface_id_x2_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(node_type.unpack(bref));

  return SRSASN_SUCCESS;
}
void interface_id_x2_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("nodeType");
  node_type.to_json(j);
  j.end_obj();
}

void interface_id_x2_s::node_type_c_::destroy_()
{
  switch (type_) {
    case types::global_enb_id:
      c.destroy<global_enb_id_s>();
      break;
    case types::global_en_g_nb_id:
      c.destroy<globalen_gnb_id_s>();
      break;
    default:
      break;
  }
}
void interface_id_x2_s::node_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::global_enb_id:
      c.init<global_enb_id_s>();
      break;
    case types::global_en_g_nb_id:
      c.init<globalen_gnb_id_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "interface_id_x2_s::node_type_c_");
  }
}
interface_id_x2_s::node_type_c_::node_type_c_(const interface_id_x2_s::node_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::global_enb_id:
      c.init(other.c.get<global_enb_id_s>());
      break;
    case types::global_en_g_nb_id:
      c.init(other.c.get<globalen_gnb_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "interface_id_x2_s::node_type_c_");
  }
}
interface_id_x2_s::node_type_c_&
interface_id_x2_s::node_type_c_::operator=(const interface_id_x2_s::node_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::global_enb_id:
      c.set(other.c.get<global_enb_id_s>());
      break;
    case types::global_en_g_nb_id:
      c.set(other.c.get<globalen_gnb_id_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "interface_id_x2_s::node_type_c_");
  }

  return *this;
}
global_enb_id_s& interface_id_x2_s::node_type_c_::set_global_enb_id()
{
  set(types::global_enb_id);
  return c.get<global_enb_id_s>();
}
globalen_gnb_id_s& interface_id_x2_s::node_type_c_::set_global_en_g_nb_id()
{
  set(types::global_en_g_nb_id);
  return c.get<globalen_gnb_id_s>();
}
void interface_id_x2_s::node_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::global_enb_id:
      j.write_fieldname("global-eNB-ID");
      c.get<global_enb_id_s>().to_json(j);
      break;
    case types::global_en_g_nb_id:
      j.write_fieldname("global-en-gNB-ID");
      c.get<globalen_gnb_id_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "interface_id_x2_s::node_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE interface_id_x2_s::node_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::global_enb_id:
      HANDLE_CODE(c.get<global_enb_id_s>().pack(bref));
      break;
    case types::global_en_g_nb_id:
      HANDLE_CODE(c.get<globalen_gnb_id_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "interface_id_x2_s::node_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE interface_id_x2_s::node_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::global_enb_id:
      HANDLE_CODE(c.get<global_enb_id_s>().unpack(bref));
      break;
    case types::global_en_g_nb_id:
      HANDLE_CODE(c.get<globalen_gnb_id_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "interface_id_x2_s::node_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* interface_id_x2_s::node_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"global-eNB-ID", "global-en-gNB-ID"};
  return convert_enum_idx(options, 2, value, "interface_id_x2_s::node_type_c_::types");
}

// InterfaceID-Xn ::= SEQUENCE
SRSASN_CODE interface_id_xn_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(global_ng_ran_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE interface_id_xn_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(global_ng_ran_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void interface_id_xn_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("global-NG-RAN-ID");
  global_ng_ran_id.to_json(j);
  j.end_obj();
}

// InterfaceIdentifier ::= CHOICE
void interface_id_c::destroy_()
{
  switch (type_) {
    case types::ng:
      c.destroy<interface_id_ng_s>();
      break;
    case types::xn:
      c.destroy<interface_id_xn_s>();
      break;
    case types::f1:
      c.destroy<interface_id_f1_s>();
      break;
    case types::e1:
      c.destroy<interface_id_e1_s>();
      break;
    case types::s1:
      c.destroy<interface_id_s1_s>();
      break;
    case types::x2:
      c.destroy<interface_id_x2_s>();
      break;
    case types::w1:
      c.destroy<interface_id_w1_s>();
      break;
    default:
      break;
  }
}
void interface_id_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ng:
      c.init<interface_id_ng_s>();
      break;
    case types::xn:
      c.init<interface_id_xn_s>();
      break;
    case types::f1:
      c.init<interface_id_f1_s>();
      break;
    case types::e1:
      c.init<interface_id_e1_s>();
      break;
    case types::s1:
      c.init<interface_id_s1_s>();
      break;
    case types::x2:
      c.init<interface_id_x2_s>();
      break;
    case types::w1:
      c.init<interface_id_w1_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "interface_id_c");
  }
}
interface_id_c::interface_id_c(const interface_id_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ng:
      c.init(other.c.get<interface_id_ng_s>());
      break;
    case types::xn:
      c.init(other.c.get<interface_id_xn_s>());
      break;
    case types::f1:
      c.init(other.c.get<interface_id_f1_s>());
      break;
    case types::e1:
      c.init(other.c.get<interface_id_e1_s>());
      break;
    case types::s1:
      c.init(other.c.get<interface_id_s1_s>());
      break;
    case types::x2:
      c.init(other.c.get<interface_id_x2_s>());
      break;
    case types::w1:
      c.init(other.c.get<interface_id_w1_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "interface_id_c");
  }
}
interface_id_c& interface_id_c::operator=(const interface_id_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ng:
      c.set(other.c.get<interface_id_ng_s>());
      break;
    case types::xn:
      c.set(other.c.get<interface_id_xn_s>());
      break;
    case types::f1:
      c.set(other.c.get<interface_id_f1_s>());
      break;
    case types::e1:
      c.set(other.c.get<interface_id_e1_s>());
      break;
    case types::s1:
      c.set(other.c.get<interface_id_s1_s>());
      break;
    case types::x2:
      c.set(other.c.get<interface_id_x2_s>());
      break;
    case types::w1:
      c.set(other.c.get<interface_id_w1_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "interface_id_c");
  }

  return *this;
}
interface_id_ng_s& interface_id_c::set_ng()
{
  set(types::ng);
  return c.get<interface_id_ng_s>();
}
interface_id_xn_s& interface_id_c::set_xn()
{
  set(types::xn);
  return c.get<interface_id_xn_s>();
}
interface_id_f1_s& interface_id_c::set_f1()
{
  set(types::f1);
  return c.get<interface_id_f1_s>();
}
interface_id_e1_s& interface_id_c::set_e1()
{
  set(types::e1);
  return c.get<interface_id_e1_s>();
}
interface_id_s1_s& interface_id_c::set_s1()
{
  set(types::s1);
  return c.get<interface_id_s1_s>();
}
interface_id_x2_s& interface_id_c::set_x2()
{
  set(types::x2);
  return c.get<interface_id_x2_s>();
}
interface_id_w1_s& interface_id_c::set_w1()
{
  set(types::w1);
  return c.get<interface_id_w1_s>();
}
void interface_id_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ng:
      j.write_fieldname("nG");
      c.get<interface_id_ng_s>().to_json(j);
      break;
    case types::xn:
      j.write_fieldname("xN");
      c.get<interface_id_xn_s>().to_json(j);
      break;
    case types::f1:
      j.write_fieldname("f1");
      c.get<interface_id_f1_s>().to_json(j);
      break;
    case types::e1:
      j.write_fieldname("e1");
      c.get<interface_id_e1_s>().to_json(j);
      break;
    case types::s1:
      j.write_fieldname("s1");
      c.get<interface_id_s1_s>().to_json(j);
      break;
    case types::x2:
      j.write_fieldname("x2");
      c.get<interface_id_x2_s>().to_json(j);
      break;
    case types::w1:
      j.write_fieldname("w1");
      c.get<interface_id_w1_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "interface_id_c");
  }
  j.end_obj();
}
SRSASN_CODE interface_id_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ng:
      HANDLE_CODE(c.get<interface_id_ng_s>().pack(bref));
      break;
    case types::xn:
      HANDLE_CODE(c.get<interface_id_xn_s>().pack(bref));
      break;
    case types::f1:
      HANDLE_CODE(c.get<interface_id_f1_s>().pack(bref));
      break;
    case types::e1:
      HANDLE_CODE(c.get<interface_id_e1_s>().pack(bref));
      break;
    case types::s1:
      HANDLE_CODE(c.get<interface_id_s1_s>().pack(bref));
      break;
    case types::x2:
      HANDLE_CODE(c.get<interface_id_x2_s>().pack(bref));
      break;
    case types::w1:
      HANDLE_CODE(c.get<interface_id_w1_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "interface_id_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE interface_id_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ng:
      HANDLE_CODE(c.get<interface_id_ng_s>().unpack(bref));
      break;
    case types::xn:
      HANDLE_CODE(c.get<interface_id_xn_s>().unpack(bref));
      break;
    case types::f1:
      HANDLE_CODE(c.get<interface_id_f1_s>().unpack(bref));
      break;
    case types::e1:
      HANDLE_CODE(c.get<interface_id_e1_s>().unpack(bref));
      break;
    case types::s1:
      HANDLE_CODE(c.get<interface_id_s1_s>().unpack(bref));
      break;
    case types::x2:
      HANDLE_CODE(c.get<interface_id_x2_s>().unpack(bref));
      break;
    case types::w1:
      HANDLE_CODE(c.get<interface_id_w1_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "interface_id_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* interface_id_c::types_opts::to_string() const
{
  static const char* options[] = {"nG", "xN", "f1", "e1", "s1", "x2", "w1"};
  return convert_enum_idx(options, 7, value, "interface_id_c::types");
}

// FreqBandNrItem ::= SEQUENCE
SRSASN_CODE freq_band_nr_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, freq_band_ind_nr, (uint16_t)1u, (uint16_t)1024u, true, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_band_nr_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(freq_band_ind_nr, bref, (uint16_t)1u, (uint16_t)1024u, true, true));

  return SRSASN_SUCCESS;
}
void freq_band_nr_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("freqBandIndicatorNr", freq_band_ind_nr);
  j.end_obj();
}

// NR-ARFCN ::= SEQUENCE
SRSASN_CODE nr_arfcn_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, nrarfcn, (uint32_t)0u, (uint32_t)3279165u, false, true));
  HANDLE_CODE(pack_dyn_seq_of(bref, freq_band_list_nr, 1, 32, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE nr_arfcn_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(nrarfcn, bref, (uint32_t)0u, (uint32_t)3279165u, false, true));
  HANDLE_CODE(unpack_dyn_seq_of(freq_band_list_nr, bref, 1, 32, true));

  return SRSASN_SUCCESS;
}
void nr_arfcn_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("nRARFCN", nrarfcn);
  j.start_array("freqBandListNr");
  for (const auto& e1 : freq_band_list_nr) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// QoSID ::= CHOICE
void qo_sid_c::destroy_() {}
void qo_sid_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
qo_sid_c::qo_sid_c(const qo_sid_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::five_gc:
      c.init(other.c.get<uint16_t>());
      break;
    case types::epc:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "qo_sid_c");
  }
}
qo_sid_c& qo_sid_c::operator=(const qo_sid_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::five_gc:
      c.set(other.c.get<uint16_t>());
      break;
    case types::epc:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "qo_sid_c");
  }

  return *this;
}
uint16_t& qo_sid_c::set_five_gc()
{
  set(types::five_gc);
  return c.get<uint16_t>();
}
uint16_t& qo_sid_c::set_epc()
{
  set(types::epc);
  return c.get<uint16_t>();
}
void qo_sid_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::five_gc:
      j.write_int("fiveGC", c.get<uint16_t>());
      break;
    case types::epc:
      j.write_int("ePC", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "qo_sid_c");
  }
  j.end_obj();
}
SRSASN_CODE qo_sid_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::five_gc:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::epc:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)255u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "qo_sid_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE qo_sid_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::five_gc:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, true, true));
      break;
    case types::epc:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)255u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "qo_sid_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* qo_sid_c::types_opts::to_string() const
{
  static const char* options[] = {"fiveGC", "ePC"};
  return convert_enum_idx(options, 2, value, "qo_sid_c::types");
}
uint8_t qo_sid_c::types_opts::to_number() const
{
  static const uint8_t options[] = {5};
  return map_enum_number(options, 1, value, "qo_sid_c::types");
}

// RRCclass-LTE ::= ENUMERATED
const char* rr_cclass_lte_opts::to_string() const
{
  static const char* options[] = {"bCCH-BCH",
                                  "bCCH-BCH-MBMS",
                                  "bCCH-DL-SCH",
                                  "bCCH-DL-SCH-BR",
                                  "bCCH-DL-SCH-MBMS",
                                  "mCCH",
                                  "pCCH",
                                  "dL-CCCH",
                                  "dL-DCCH",
                                  "uL-CCCH",
                                  "uL-DCCH",
                                  "sC-MCCH"};
  return convert_enum_idx(options, 12, value, "rr_cclass_lte_e");
}

// RRCclass-NR ::= ENUMERATED
const char* rr_cclass_nr_opts::to_string() const
{
  static const char* options[] = {
      "bCCH-BCH", "bCCH-DL-SCH", "dL-CCCH", "dL-DCCH", "pCCH", "uL-CCCH", "uL-CCCH1", "uL-DCCH"};
  return convert_enum_idx(options, 8, value, "rr_cclass_nr_e");
}
uint8_t rr_cclass_nr_opts::to_number() const
{
  if (value == ul_ccch1) {
    return 1;
  }
  invalid_enum_number(value, "rr_cclass_nr_e");
  return 0;
}

// RRC-MessageID ::= SEQUENCE
SRSASN_CODE rrc_msg_id_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(rrc_type.pack(bref));
  HANDLE_CODE(pack_unconstrained_integer(bref, msg_id, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_msg_id_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(rrc_type.unpack(bref));
  HANDLE_CODE(unpack_unconstrained_integer(msg_id, bref, false, true));

  return SRSASN_SUCCESS;
}
void rrc_msg_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rrcType");
  rrc_type.to_json(j);
  j.write_int("messageID", msg_id);
  j.end_obj();
}

void rrc_msg_id_s::rrc_type_c_::destroy_() {}
void rrc_msg_id_s::rrc_type_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
rrc_msg_id_s::rrc_type_c_::rrc_type_c_(const rrc_msg_id_s::rrc_type_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::lte:
      c.init(other.c.get<rr_cclass_lte_e>());
      break;
    case types::nr:
      c.init(other.c.get<rr_cclass_nr_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_msg_id_s::rrc_type_c_");
  }
}
rrc_msg_id_s::rrc_type_c_& rrc_msg_id_s::rrc_type_c_::operator=(const rrc_msg_id_s::rrc_type_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::lte:
      c.set(other.c.get<rr_cclass_lte_e>());
      break;
    case types::nr:
      c.set(other.c.get<rr_cclass_nr_e>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "rrc_msg_id_s::rrc_type_c_");
  }

  return *this;
}
rr_cclass_lte_e& rrc_msg_id_s::rrc_type_c_::set_lte()
{
  set(types::lte);
  return c.get<rr_cclass_lte_e>();
}
rr_cclass_nr_e& rrc_msg_id_s::rrc_type_c_::set_nr()
{
  set(types::nr);
  return c.get<rr_cclass_nr_e>();
}
void rrc_msg_id_s::rrc_type_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::lte:
      j.write_str("lTE", c.get<rr_cclass_lte_e>().to_string());
      break;
    case types::nr:
      j.write_str("nR", c.get<rr_cclass_nr_e>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "rrc_msg_id_s::rrc_type_c_");
  }
  j.end_obj();
}
SRSASN_CODE rrc_msg_id_s::rrc_type_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::lte:
      HANDLE_CODE(c.get<rr_cclass_lte_e>().pack(bref));
      break;
    case types::nr:
      HANDLE_CODE(c.get<rr_cclass_nr_e>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_msg_id_s::rrc_type_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rrc_msg_id_s::rrc_type_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::lte:
      HANDLE_CODE(c.get<rr_cclass_lte_e>().unpack(bref));
      break;
    case types::nr:
      HANDLE_CODE(c.get<rr_cclass_nr_e>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "rrc_msg_id_s::rrc_type_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* rrc_msg_id_s::rrc_type_c_::types_opts::to_string() const
{
  static const char* options[] = {"lTE", "nR"};
  return convert_enum_idx(options, 2, value, "rrc_msg_id_s::rrc_type_c_::types");
}

// S-NSSAI ::= SEQUENCE
SRSASN_CODE s_nssai_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(sd_present, 1));

  HANDLE_CODE(sst.pack(bref));
  if (sd_present) {
    HANDLE_CODE(sd.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE s_nssai_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(sd_present, 1));

  HANDLE_CODE(sst.unpack(bref));
  if (sd_present) {
    HANDLE_CODE(sd.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void s_nssai_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("sST", sst.to_string());
  if (sd_present) {
    j.write_str("sD", sd.to_string());
  }
  j.end_obj();
}

// ServingCell-ARFCN ::= CHOICE
void serving_cell_arfcn_c::destroy_()
{
  switch (type_) {
    case types::nr:
      c.destroy<nr_arfcn_s>();
      break;
    default:
      break;
  }
}
void serving_cell_arfcn_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::nr:
      c.init<nr_arfcn_s>();
      break;
    case types::eutra:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_arfcn_c");
  }
}
serving_cell_arfcn_c::serving_cell_arfcn_c(const serving_cell_arfcn_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr:
      c.init(other.c.get<nr_arfcn_s>());
      break;
    case types::eutra:
      c.init(other.c.get<uint32_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_arfcn_c");
  }
}
serving_cell_arfcn_c& serving_cell_arfcn_c::operator=(const serving_cell_arfcn_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr:
      c.set(other.c.get<nr_arfcn_s>());
      break;
    case types::eutra:
      c.set(other.c.get<uint32_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_arfcn_c");
  }

  return *this;
}
nr_arfcn_s& serving_cell_arfcn_c::set_nr()
{
  set(types::nr);
  return c.get<nr_arfcn_s>();
}
uint32_t& serving_cell_arfcn_c::set_eutra()
{
  set(types::eutra);
  return c.get<uint32_t>();
}
void serving_cell_arfcn_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr:
      j.write_fieldname("nR");
      c.get<nr_arfcn_s>().to_json(j);
      break;
    case types::eutra:
      j.write_int("eUTRA", c.get<uint32_t>());
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_arfcn_c");
  }
  j.end_obj();
}
SRSASN_CODE serving_cell_arfcn_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr:
      HANDLE_CODE(c.get<nr_arfcn_s>().pack(bref));
      break;
    case types::eutra:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)0u, (uint32_t)65535u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_arfcn_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE serving_cell_arfcn_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr:
      HANDLE_CODE(c.get<nr_arfcn_s>().unpack(bref));
      break;
    case types::eutra:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)0u, (uint32_t)65535u, false, true));
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_arfcn_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* serving_cell_arfcn_c::types_opts::to_string() const
{
  static const char* options[] = {"nR", "eUTRA"};
  return convert_enum_idx(options, 2, value, "serving_cell_arfcn_c::types");
}

// ServingCell-PCI ::= CHOICE
void serving_cell_pci_c::destroy_() {}
void serving_cell_pci_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
serving_cell_pci_c::serving_cell_pci_c(const serving_cell_pci_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::nr:
      c.init(other.c.get<uint16_t>());
      break;
    case types::eutra:
      c.init(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_pci_c");
  }
}
serving_cell_pci_c& serving_cell_pci_c::operator=(const serving_cell_pci_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::nr:
      c.set(other.c.get<uint16_t>());
      break;
    case types::eutra:
      c.set(other.c.get<uint16_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_pci_c");
  }

  return *this;
}
uint16_t& serving_cell_pci_c::set_nr()
{
  set(types::nr);
  return c.get<uint16_t>();
}
uint16_t& serving_cell_pci_c::set_eutra()
{
  set(types::eutra);
  return c.get<uint16_t>();
}
void serving_cell_pci_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::nr:
      j.write_int("nR", c.get<uint16_t>());
      break;
    case types::eutra:
      j.write_int("eUTRA", c.get<uint16_t>());
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_pci_c");
  }
  j.end_obj();
}
SRSASN_CODE serving_cell_pci_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::nr:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)1007u, false, true));
      break;
    case types::eutra:
      HANDLE_CODE(pack_integer(bref, c.get<uint16_t>(), (uint16_t)0u, (uint16_t)503u, true, true));
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_pci_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE serving_cell_pci_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::nr:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)1007u, false, true));
      break;
    case types::eutra:
      HANDLE_CODE(unpack_integer(c.get<uint16_t>(), bref, (uint16_t)0u, (uint16_t)503u, true, true));
      break;
    default:
      log_invalid_choice_id(type_, "serving_cell_pci_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* serving_cell_pci_c::types_opts::to_string() const
{
  static const char* options[] = {"nR", "eUTRA"};
  return convert_enum_idx(options, 2, value, "serving_cell_pci_c::types");
}

// UEID-GNB-CU-CP-E1AP-ID-Item ::= SEQUENCE
SRSASN_CODE ueid_gnb_cu_cp_e1_ap_id_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1_ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ueid_gnb_cu_cp_e1_ap_id_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1_ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));

  return SRSASN_SUCCESS;
}
void ueid_gnb_cu_cp_e1_ap_id_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("gNB-CU-CP-UE-E1AP-ID", gnb_cu_cp_ue_e1_ap_id);
  j.end_obj();
}

// UEID-GNB-CU-CP-F1AP-ID-Item ::= SEQUENCE
SRSASN_CODE ueid_gnb_cu_cp_f1_ap_id_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1_ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ueid_gnb_cu_cp_f1_ap_id_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(gnb_cu_ue_f1_ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));

  return SRSASN_SUCCESS;
}
void ueid_gnb_cu_cp_f1_ap_id_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("gNB-CU-UE-F1AP-ID", gnb_cu_ue_f1_ap_id);
  j.end_obj();
}

// UEID-EN-GNB ::= SEQUENCE
SRSASN_CODE ueid_en_gnb_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(m_enb_ue_x2ap_id_ext_present, 1));
  HANDLE_CODE(bref.pack(gnb_cu_ue_f1_ap_id_present, 1));
  HANDLE_CODE(bref.pack(gnb_cu_cp_ue_e1_ap_id_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ran_ueid_present, 1));

  HANDLE_CODE(pack_integer(bref, m_enb_ue_x2ap_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  if (m_enb_ue_x2ap_id_ext_present) {
    HANDLE_CODE(pack_integer(bref, m_enb_ue_x2ap_id_ext, (uint16_t)0u, (uint16_t)4095u, true, true));
  }
  HANDLE_CODE(global_enb_id.pack(bref));
  if (gnb_cu_ue_f1_ap_id_present) {
    HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1_ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (gnb_cu_cp_ue_e1_ap_id_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, gnb_cu_cp_ue_e1_ap_id_list, 1, 65535, true));
  }
  if (ran_ueid_present) {
    HANDLE_CODE(ran_ueid.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ueid_en_gnb_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(m_enb_ue_x2ap_id_ext_present, 1));
  HANDLE_CODE(bref.unpack(gnb_cu_ue_f1_ap_id_present, 1));
  bool gnb_cu_cp_ue_e1_ap_id_list_present;
  HANDLE_CODE(bref.unpack(gnb_cu_cp_ue_e1_ap_id_list_present, 1));
  HANDLE_CODE(bref.unpack(ran_ueid_present, 1));

  HANDLE_CODE(unpack_integer(m_enb_ue_x2ap_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
  if (m_enb_ue_x2ap_id_ext_present) {
    HANDLE_CODE(unpack_integer(m_enb_ue_x2ap_id_ext, bref, (uint16_t)0u, (uint16_t)4095u, true, true));
  }
  HANDLE_CODE(global_enb_id.unpack(bref));
  if (gnb_cu_ue_f1_ap_id_present) {
    HANDLE_CODE(unpack_integer(gnb_cu_ue_f1_ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (gnb_cu_cp_ue_e1_ap_id_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(gnb_cu_cp_ue_e1_ap_id_list, bref, 1, 65535, true));
  }
  if (ran_ueid_present) {
    HANDLE_CODE(ran_ueid.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ueid_en_gnb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("m-eNB-UE-X2AP-ID", m_enb_ue_x2ap_id);
  if (m_enb_ue_x2ap_id_ext_present) {
    j.write_int("m-eNB-UE-X2AP-ID-Extension", m_enb_ue_x2ap_id_ext);
  }
  j.write_fieldname("globalENB-ID");
  global_enb_id.to_json(j);
  if (gnb_cu_ue_f1_ap_id_present) {
    j.write_int("gNB-CU-UE-F1AP-ID", gnb_cu_ue_f1_ap_id);
  }
  if (gnb_cu_cp_ue_e1_ap_id_list.size() > 0) {
    j.start_array("gNB-CU-CP-UE-E1AP-ID-List");
    for (const auto& e1 : gnb_cu_cp_ue_e1_ap_id_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ran_ueid_present) {
    j.write_str("ran-UEID", ran_ueid.to_string());
  }
  j.end_obj();
}

// UEID-ENB ::= SEQUENCE
SRSASN_CODE ueid_enb_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(m_enb_ue_x2ap_id_present, 1));
  HANDLE_CODE(bref.pack(m_enb_ue_x2ap_id_ext_present, 1));
  HANDLE_CODE(bref.pack(global_enb_id_present, 1));

  HANDLE_CODE(pack_integer(bref, mme_ue_s1ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  HANDLE_CODE(gummei.pack(bref));
  if (m_enb_ue_x2ap_id_present) {
    HANDLE_CODE(pack_integer(bref, m_enb_ue_x2ap_id, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  if (m_enb_ue_x2ap_id_ext_present) {
    HANDLE_CODE(pack_integer(bref, m_enb_ue_x2ap_id_ext, (uint16_t)0u, (uint16_t)4095u, true, true));
  }
  if (global_enb_id_present) {
    HANDLE_CODE(global_enb_id.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ueid_enb_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(m_enb_ue_x2ap_id_present, 1));
  HANDLE_CODE(bref.unpack(m_enb_ue_x2ap_id_ext_present, 1));
  HANDLE_CODE(bref.unpack(global_enb_id_present, 1));

  HANDLE_CODE(unpack_integer(mme_ue_s1ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  HANDLE_CODE(gummei.unpack(bref));
  if (m_enb_ue_x2ap_id_present) {
    HANDLE_CODE(unpack_integer(m_enb_ue_x2ap_id, bref, (uint16_t)0u, (uint16_t)4095u, false, true));
  }
  if (m_enb_ue_x2ap_id_ext_present) {
    HANDLE_CODE(unpack_integer(m_enb_ue_x2ap_id_ext, bref, (uint16_t)0u, (uint16_t)4095u, true, true));
  }
  if (global_enb_id_present) {
    HANDLE_CODE(global_enb_id.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ueid_enb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("mME-UE-S1AP-ID", mme_ue_s1ap_id);
  j.write_fieldname("gUMMEI");
  gummei.to_json(j);
  if (m_enb_ue_x2ap_id_present) {
    j.write_int("m-eNB-UE-X2AP-ID", m_enb_ue_x2ap_id);
  }
  if (m_enb_ue_x2ap_id_ext_present) {
    j.write_int("m-eNB-UE-X2AP-ID-Extension", m_enb_ue_x2ap_id_ext);
  }
  if (global_enb_id_present) {
    j.write_fieldname("globalENB-ID");
    global_enb_id.to_json(j);
  }
  j.end_obj();
}

// UEID-GNB ::= SEQUENCE
SRSASN_CODE ueid_gnb_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(gnb_cu_ue_f1_ap_id_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(gnb_cu_cp_ue_e1_ap_id_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ran_ueid_present, 1));
  HANDLE_CODE(bref.pack(m_ng_ran_ue_xn_ap_id_present, 1));
  HANDLE_CODE(bref.pack(global_gnb_id_present, 1));

  HANDLE_CODE(pack_integer(bref, amf_ue_ngap_id, (uint64_t)0u, (uint64_t)1099511627775u, false, true));
  HANDLE_CODE(guami.pack(bref));
  if (gnb_cu_ue_f1_ap_id_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, gnb_cu_ue_f1_ap_id_list, 1, 4, true));
  }
  if (gnb_cu_cp_ue_e1_ap_id_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, gnb_cu_cp_ue_e1_ap_id_list, 1, 65535, true));
  }
  if (ran_ueid_present) {
    HANDLE_CODE(ran_ueid.pack(bref));
  }
  if (m_ng_ran_ue_xn_ap_id_present) {
    HANDLE_CODE(pack_integer(bref, m_ng_ran_ue_xn_ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (global_gnb_id_present) {
    HANDLE_CODE(global_gnb_id.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ueid_gnb_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool gnb_cu_ue_f1_ap_id_list_present;
  HANDLE_CODE(bref.unpack(gnb_cu_ue_f1_ap_id_list_present, 1));
  bool gnb_cu_cp_ue_e1_ap_id_list_present;
  HANDLE_CODE(bref.unpack(gnb_cu_cp_ue_e1_ap_id_list_present, 1));
  HANDLE_CODE(bref.unpack(ran_ueid_present, 1));
  HANDLE_CODE(bref.unpack(m_ng_ran_ue_xn_ap_id_present, 1));
  HANDLE_CODE(bref.unpack(global_gnb_id_present, 1));

  HANDLE_CODE(unpack_integer(amf_ue_ngap_id, bref, (uint64_t)0u, (uint64_t)1099511627775u, false, true));
  HANDLE_CODE(guami.unpack(bref));
  if (gnb_cu_ue_f1_ap_id_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(gnb_cu_ue_f1_ap_id_list, bref, 1, 4, true));
  }
  if (gnb_cu_cp_ue_e1_ap_id_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(gnb_cu_cp_ue_e1_ap_id_list, bref, 1, 65535, true));
  }
  if (ran_ueid_present) {
    HANDLE_CODE(ran_ueid.unpack(bref));
  }
  if (m_ng_ran_ue_xn_ap_id_present) {
    HANDLE_CODE(unpack_integer(m_ng_ran_ue_xn_ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (global_gnb_id_present) {
    HANDLE_CODE(global_gnb_id.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ueid_gnb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("amf-UE-NGAP-ID", amf_ue_ngap_id);
  j.write_fieldname("guami");
  guami.to_json(j);
  if (gnb_cu_ue_f1_ap_id_list.size() > 0) {
    j.start_array("gNB-CU-UE-F1AP-ID-List");
    for (const auto& e1 : gnb_cu_ue_f1_ap_id_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (gnb_cu_cp_ue_e1_ap_id_list.size() > 0) {
    j.start_array("gNB-CU-CP-UE-E1AP-ID-List");
    for (const auto& e1 : gnb_cu_cp_ue_e1_ap_id_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ran_ueid_present) {
    j.write_str("ran-UEID", ran_ueid.to_string());
  }
  if (m_ng_ran_ue_xn_ap_id_present) {
    j.write_int("m-NG-RAN-UE-XnAP-ID", m_ng_ran_ue_xn_ap_id);
  }
  if (global_gnb_id_present) {
    j.write_fieldname("globalGNB-ID");
    global_gnb_id.to_json(j);
  }
  j.end_obj();
}

// UEID-GNB-CU-UP ::= SEQUENCE
SRSASN_CODE ueid_gnb_cu_up_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ran_ueid_present, 1));

  HANDLE_CODE(pack_integer(bref, gnb_cu_cp_ue_e1_ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  if (ran_ueid_present) {
    HANDLE_CODE(ran_ueid.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ueid_gnb_cu_up_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ran_ueid_present, 1));

  HANDLE_CODE(unpack_integer(gnb_cu_cp_ue_e1_ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  if (ran_ueid_present) {
    HANDLE_CODE(ran_ueid.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ueid_gnb_cu_up_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("gNB-CU-CP-UE-E1AP-ID", gnb_cu_cp_ue_e1_ap_id);
  if (ran_ueid_present) {
    j.write_str("ran-UEID", ran_ueid.to_string());
  }
  j.end_obj();
}

// UEID-GNB-DU ::= SEQUENCE
SRSASN_CODE ueid_gnb_du_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ran_ueid_present, 1));

  HANDLE_CODE(pack_integer(bref, gnb_cu_ue_f1_ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  if (ran_ueid_present) {
    HANDLE_CODE(ran_ueid.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ueid_gnb_du_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ran_ueid_present, 1));

  HANDLE_CODE(unpack_integer(gnb_cu_ue_f1_ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  if (ran_ueid_present) {
    HANDLE_CODE(ran_ueid.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ueid_gnb_du_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("gNB-CU-UE-F1AP-ID", gnb_cu_ue_f1_ap_id);
  if (ran_ueid_present) {
    j.write_str("ran-UEID", ran_ueid.to_string());
  }
  j.end_obj();
}

// UEID-NG-ENB ::= SEQUENCE
SRSASN_CODE ueid_ng_enb_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ng_enb_cu_ue_w1_ap_id_present, 1));
  HANDLE_CODE(bref.pack(m_ng_ran_ue_xn_ap_id_present, 1));
  HANDLE_CODE(bref.pack(global_ng_enb_id_present, 1));

  HANDLE_CODE(pack_integer(bref, amf_ue_ngap_id, (uint64_t)0u, (uint64_t)1099511627775u, false, true));
  HANDLE_CODE(guami.pack(bref));
  if (ng_enb_cu_ue_w1_ap_id_present) {
    HANDLE_CODE(pack_integer(bref, ng_enb_cu_ue_w1_ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (m_ng_ran_ue_xn_ap_id_present) {
    HANDLE_CODE(pack_integer(bref, m_ng_ran_ue_xn_ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (global_ng_enb_id_present) {
    HANDLE_CODE(global_ng_enb_id.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ueid_ng_enb_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ng_enb_cu_ue_w1_ap_id_present, 1));
  HANDLE_CODE(bref.unpack(m_ng_ran_ue_xn_ap_id_present, 1));
  HANDLE_CODE(bref.unpack(global_ng_enb_id_present, 1));

  HANDLE_CODE(unpack_integer(amf_ue_ngap_id, bref, (uint64_t)0u, (uint64_t)1099511627775u, false, true));
  HANDLE_CODE(guami.unpack(bref));
  if (ng_enb_cu_ue_w1_ap_id_present) {
    HANDLE_CODE(unpack_integer(ng_enb_cu_ue_w1_ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (m_ng_ran_ue_xn_ap_id_present) {
    HANDLE_CODE(unpack_integer(m_ng_ran_ue_xn_ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
  }
  if (global_ng_enb_id_present) {
    HANDLE_CODE(global_ng_enb_id.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ueid_ng_enb_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("amf-UE-NGAP-ID", amf_ue_ngap_id);
  j.write_fieldname("guami");
  guami.to_json(j);
  if (ng_enb_cu_ue_w1_ap_id_present) {
    j.write_int("ng-eNB-CU-UE-W1AP-ID", ng_enb_cu_ue_w1_ap_id);
  }
  if (m_ng_ran_ue_xn_ap_id_present) {
    j.write_int("m-NG-RAN-UE-XnAP-ID", m_ng_ran_ue_xn_ap_id);
  }
  if (global_ng_enb_id_present) {
    j.write_fieldname("globalNgENB-ID");
    global_ng_enb_id.to_json(j);
  }
  j.end_obj();
}

// UEID-NG-ENB-DU ::= SEQUENCE
SRSASN_CODE ueid_ng_enb_du_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, ng_enb_cu_ue_w1_ap_id, (uint64_t)0u, (uint64_t)4294967295u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ueid_ng_enb_du_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(ng_enb_cu_ue_w1_ap_id, bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));

  return SRSASN_SUCCESS;
}
void ueid_ng_enb_du_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ng-eNB-CU-UE-W1AP-ID", ng_enb_cu_ue_w1_ap_id);
  j.end_obj();
}

// UEID ::= CHOICE
void ueid_c::destroy_()
{
  switch (type_) {
    case types::gnb_ueid:
      c.destroy<ueid_gnb_s>();
      break;
    case types::gnb_du_ueid:
      c.destroy<ueid_gnb_du_s>();
      break;
    case types::gnb_cu_up_ueid:
      c.destroy<ueid_gnb_cu_up_s>();
      break;
    case types::ng_enb_ueid:
      c.destroy<ueid_ng_enb_s>();
      break;
    case types::ng_enb_du_ueid:
      c.destroy<ueid_ng_enb_du_s>();
      break;
    case types::en_g_nb_ueid:
      c.destroy<ueid_en_gnb_s>();
      break;
    case types::enb_ueid:
      c.destroy<ueid_enb_s>();
      break;
    default:
      break;
  }
}
void ueid_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::gnb_ueid:
      c.init<ueid_gnb_s>();
      break;
    case types::gnb_du_ueid:
      c.init<ueid_gnb_du_s>();
      break;
    case types::gnb_cu_up_ueid:
      c.init<ueid_gnb_cu_up_s>();
      break;
    case types::ng_enb_ueid:
      c.init<ueid_ng_enb_s>();
      break;
    case types::ng_enb_du_ueid:
      c.init<ueid_ng_enb_du_s>();
      break;
    case types::en_g_nb_ueid:
      c.init<ueid_en_gnb_s>();
      break;
    case types::enb_ueid:
      c.init<ueid_enb_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ueid_c");
  }
}
ueid_c::ueid_c(const ueid_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gnb_ueid:
      c.init(other.c.get<ueid_gnb_s>());
      break;
    case types::gnb_du_ueid:
      c.init(other.c.get<ueid_gnb_du_s>());
      break;
    case types::gnb_cu_up_ueid:
      c.init(other.c.get<ueid_gnb_cu_up_s>());
      break;
    case types::ng_enb_ueid:
      c.init(other.c.get<ueid_ng_enb_s>());
      break;
    case types::ng_enb_du_ueid:
      c.init(other.c.get<ueid_ng_enb_du_s>());
      break;
    case types::en_g_nb_ueid:
      c.init(other.c.get<ueid_en_gnb_s>());
      break;
    case types::enb_ueid:
      c.init(other.c.get<ueid_enb_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ueid_c");
  }
}
ueid_c& ueid_c::operator=(const ueid_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gnb_ueid:
      c.set(other.c.get<ueid_gnb_s>());
      break;
    case types::gnb_du_ueid:
      c.set(other.c.get<ueid_gnb_du_s>());
      break;
    case types::gnb_cu_up_ueid:
      c.set(other.c.get<ueid_gnb_cu_up_s>());
      break;
    case types::ng_enb_ueid:
      c.set(other.c.get<ueid_ng_enb_s>());
      break;
    case types::ng_enb_du_ueid:
      c.set(other.c.get<ueid_ng_enb_du_s>());
      break;
    case types::en_g_nb_ueid:
      c.set(other.c.get<ueid_en_gnb_s>());
      break;
    case types::enb_ueid:
      c.set(other.c.get<ueid_enb_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "ueid_c");
  }

  return *this;
}
ueid_gnb_s& ueid_c::set_gnb_ueid()
{
  set(types::gnb_ueid);
  return c.get<ueid_gnb_s>();
}
ueid_gnb_du_s& ueid_c::set_gnb_du_ueid()
{
  set(types::gnb_du_ueid);
  return c.get<ueid_gnb_du_s>();
}
ueid_gnb_cu_up_s& ueid_c::set_gnb_cu_up_ueid()
{
  set(types::gnb_cu_up_ueid);
  return c.get<ueid_gnb_cu_up_s>();
}
ueid_ng_enb_s& ueid_c::set_ng_enb_ueid()
{
  set(types::ng_enb_ueid);
  return c.get<ueid_ng_enb_s>();
}
ueid_ng_enb_du_s& ueid_c::set_ng_enb_du_ueid()
{
  set(types::ng_enb_du_ueid);
  return c.get<ueid_ng_enb_du_s>();
}
ueid_en_gnb_s& ueid_c::set_en_g_nb_ueid()
{
  set(types::en_g_nb_ueid);
  return c.get<ueid_en_gnb_s>();
}
ueid_enb_s& ueid_c::set_enb_ueid()
{
  set(types::enb_ueid);
  return c.get<ueid_enb_s>();
}
void ueid_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gnb_ueid:
      j.write_fieldname("gNB-UEID");
      c.get<ueid_gnb_s>().to_json(j);
      break;
    case types::gnb_du_ueid:
      j.write_fieldname("gNB-DU-UEID");
      c.get<ueid_gnb_du_s>().to_json(j);
      break;
    case types::gnb_cu_up_ueid:
      j.write_fieldname("gNB-CU-UP-UEID");
      c.get<ueid_gnb_cu_up_s>().to_json(j);
      break;
    case types::ng_enb_ueid:
      j.write_fieldname("ng-eNB-UEID");
      c.get<ueid_ng_enb_s>().to_json(j);
      break;
    case types::ng_enb_du_ueid:
      j.write_fieldname("ng-eNB-DU-UEID");
      c.get<ueid_ng_enb_du_s>().to_json(j);
      break;
    case types::en_g_nb_ueid:
      j.write_fieldname("en-gNB-UEID");
      c.get<ueid_en_gnb_s>().to_json(j);
      break;
    case types::enb_ueid:
      j.write_fieldname("eNB-UEID");
      c.get<ueid_enb_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "ueid_c");
  }
  j.end_obj();
}
SRSASN_CODE ueid_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::gnb_ueid:
      HANDLE_CODE(c.get<ueid_gnb_s>().pack(bref));
      break;
    case types::gnb_du_ueid:
      HANDLE_CODE(c.get<ueid_gnb_du_s>().pack(bref));
      break;
    case types::gnb_cu_up_ueid:
      HANDLE_CODE(c.get<ueid_gnb_cu_up_s>().pack(bref));
      break;
    case types::ng_enb_ueid:
      HANDLE_CODE(c.get<ueid_ng_enb_s>().pack(bref));
      break;
    case types::ng_enb_du_ueid:
      HANDLE_CODE(c.get<ueid_ng_enb_du_s>().pack(bref));
      break;
    case types::en_g_nb_ueid:
      HANDLE_CODE(c.get<ueid_en_gnb_s>().pack(bref));
      break;
    case types::enb_ueid:
      HANDLE_CODE(c.get<ueid_enb_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ueid_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE ueid_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::gnb_ueid:
      HANDLE_CODE(c.get<ueid_gnb_s>().unpack(bref));
      break;
    case types::gnb_du_ueid:
      HANDLE_CODE(c.get<ueid_gnb_du_s>().unpack(bref));
      break;
    case types::gnb_cu_up_ueid:
      HANDLE_CODE(c.get<ueid_gnb_cu_up_s>().unpack(bref));
      break;
    case types::ng_enb_ueid:
      HANDLE_CODE(c.get<ueid_ng_enb_s>().unpack(bref));
      break;
    case types::ng_enb_du_ueid:
      HANDLE_CODE(c.get<ueid_ng_enb_du_s>().unpack(bref));
      break;
    case types::en_g_nb_ueid:
      HANDLE_CODE(c.get<ueid_en_gnb_s>().unpack(bref));
      break;
    case types::enb_ueid:
      HANDLE_CODE(c.get<ueid_enb_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "ueid_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* ueid_c::types_opts::to_string() const
{
  static const char* options[] = {
      "gNB-UEID", "gNB-DU-UEID", "gNB-CU-UP-UEID", "ng-eNB-UEID", "ng-eNB-DU-UEID", "en-gNB-UEID", "eNB-UEID"};
  return convert_enum_idx(options, 7, value, "ueid_c::types");
}
