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

#include "srsran/asn1/e2sm_kpm_v2.h"
#include <sstream>

using namespace asn1;
using namespace asn1::e2sm_kpm;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// BinRangeValue ::= CHOICE
void bin_range_value_c::destroy_()
{
  switch (type_) {
    case types::value_real:
      c.destroy<real_s>();
      break;
    default:
      break;
  }
}
void bin_range_value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::value_int:
      break;
    case types::value_real:
      c.init<real_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bin_range_value_c");
  }
}
bin_range_value_c::bin_range_value_c(const bin_range_value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::value_int:
      c.init(other.c.get<int64_t>());
      break;
    case types::value_real:
      c.init(other.c.get<real_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bin_range_value_c");
  }
}
bin_range_value_c& bin_range_value_c::operator=(const bin_range_value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::value_int:
      c.set(other.c.get<int64_t>());
      break;
    case types::value_real:
      c.set(other.c.get<real_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "bin_range_value_c");
  }

  return *this;
}
int64_t& bin_range_value_c::set_value_int()
{
  set(types::value_int);
  return c.get<int64_t>();
}
real_s& bin_range_value_c::set_value_real()
{
  set(types::value_real);
  return c.get<real_s>();
}
void bin_range_value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::value_int:
      j.write_int("valueInt", c.get<int64_t>());
      break;
    case types::value_real:
      j.write_fieldname("valueReal");
      c.get<real_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "bin_range_value_c");
  }
  j.end_obj();
}
SRSASN_CODE bin_range_value_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::value_int:
      HANDLE_CODE(pack_unconstrained_integer(bref, c.get<int64_t>(), false, true));
      break;
    case types::value_real:
      HANDLE_CODE(c.get<real_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bin_range_value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE bin_range_value_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::value_int:
      HANDLE_CODE(unpack_unconstrained_integer(c.get<int64_t>(), bref, false, true));
      break;
    case types::value_real:
      HANDLE_CODE(c.get<real_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "bin_range_value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* bin_range_value_c::types_opts::to_string() const
{
  static const char* options[] = {"valueInt", "valueReal"};
  return convert_enum_idx(options, 2, value, "bin_range_value_c::types");
}

// BinRangeItem ::= SEQUENCE
SRSASN_CODE bin_range_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, bin_idx, (uint32_t)1u, (uint32_t)65535u, true, true));
  HANDLE_CODE(start_value.pack(bref));
  HANDLE_CODE(end_value.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE bin_range_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(bin_idx, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
  HANDLE_CODE(start_value.unpack(bref));
  HANDLE_CODE(end_value.unpack(bref));

  return SRSASN_SUCCESS;
}
void bin_range_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("binIndex", bin_idx);
  j.write_fieldname("startValue");
  start_value.to_json(j);
  j.write_fieldname("endValue");
  end_value.to_json(j);
  j.end_obj();
}

// BinRangeDefinition ::= SEQUENCE
SRSASN_CODE bin_range_definition_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(bin_range_list_y.size() > 0, 1));
  HANDLE_CODE(bref.pack(bin_range_list_z.size() > 0, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, bin_range_list_x, 1, 65535, true));
  if (bin_range_list_y.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, bin_range_list_y, 1, 65535, true));
  }
  if (bin_range_list_z.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, bin_range_list_z, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE bin_range_definition_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool bin_range_list_y_present;
  HANDLE_CODE(bref.unpack(bin_range_list_y_present, 1));
  bool bin_range_list_z_present;
  HANDLE_CODE(bref.unpack(bin_range_list_z_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(bin_range_list_x, bref, 1, 65535, true));
  if (bin_range_list_y_present) {
    HANDLE_CODE(unpack_dyn_seq_of(bin_range_list_y, bref, 1, 65535, true));
  }
  if (bin_range_list_z_present) {
    HANDLE_CODE(unpack_dyn_seq_of(bin_range_list_z, bref, 1, 65535, true));
  }

  return SRSASN_SUCCESS;
}
void bin_range_definition_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("binRangeListX");
  for (const auto& e1 : bin_range_list_x) {
    e1.to_json(j);
  }
  j.end_array();
  if (bin_range_list_y.size() > 0) {
    j.start_array("binRangeListY");
    for (const auto& e1 : bin_range_list_y) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (bin_range_list_z.size() > 0) {
    j.start_array("binRangeListZ");
    for (const auto& e1 : bin_range_list_z) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.end_obj();
}

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

// MeasurementType ::= CHOICE
void meas_type_c::destroy_()
{
  switch (type_) {
    case types::meas_name:
      c.destroy<printable_string<1, 150, true, true> >();
      break;
    default:
      break;
  }
}
void meas_type_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::meas_name:
      c.init<printable_string<1, 150, true, true> >();
      break;
    case types::meas_id:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_type_c");
  }
}
meas_type_c::meas_type_c(const meas_type_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::meas_name:
      c.init(other.c.get<printable_string<1, 150, true, true> >());
      break;
    case types::meas_id:
      c.init(other.c.get<uint32_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_type_c");
  }
}
meas_type_c& meas_type_c::operator=(const meas_type_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::meas_name:
      c.set(other.c.get<printable_string<1, 150, true, true> >());
      break;
    case types::meas_id:
      c.set(other.c.get<uint32_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_type_c");
  }

  return *this;
}
printable_string<1, 150, true, true>& meas_type_c::set_meas_name()
{
  set(types::meas_name);
  return c.get<printable_string<1, 150, true, true> >();
}
uint32_t& meas_type_c::set_meas_id()
{
  set(types::meas_id);
  return c.get<uint32_t>();
}
void meas_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_name:
      j.write_str("measName", c.get<printable_string<1, 150, true, true> >().to_string());
      break;
    case types::meas_id:
      j.write_int("measID", c.get<uint32_t>());
      break;
    default:
      log_invalid_choice_id(type_, "meas_type_c");
  }
  j.end_obj();
}
SRSASN_CODE meas_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_name:
      HANDLE_CODE((c.get<printable_string<1, 150, true, true> >().pack(bref)));
      break;
    case types::meas_id:
      HANDLE_CODE(pack_integer(bref, c.get<uint32_t>(), (uint32_t)1u, (uint32_t)65536u, true, true));
      break;
    default:
      log_invalid_choice_id(type_, "meas_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_name:
      HANDLE_CODE((c.get<printable_string<1, 150, true, true> >().unpack(bref)));
      break;
    case types::meas_id:
      HANDLE_CODE(unpack_integer(c.get<uint32_t>(), bref, (uint32_t)1u, (uint32_t)65536u, true, true));
      break;
    default:
      log_invalid_choice_id(type_, "meas_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* meas_type_c::types_opts::to_string() const
{
  static const char* options[] = {"measName", "measID"};
  return convert_enum_idx(options, 2, value, "meas_type_c::types");
}

// DistMeasurementBinRangeItem ::= SEQUENCE
SRSASN_CODE dist_meas_bin_range_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(meas_type.pack(bref));
  HANDLE_CODE(bin_range_def.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE dist_meas_bin_range_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(meas_type.unpack(bref));
  HANDLE_CODE(bin_range_def.unpack(bref));

  return SRSASN_SUCCESS;
}
void dist_meas_bin_range_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measType");
  meas_type.to_json(j);
  j.write_fieldname("binRangeDef");
  bin_range_def.to_json(j);
  j.end_obj();
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

// TestCond-Expression ::= ENUMERATED
const char* test_cond_expression_opts::to_string() const
{
  static const char* options[] = {"equal", "greaterthan", "lessthan", "contains", "present"};
  return convert_enum_idx(options, 5, value, "test_cond_expression_e");
}

// TestCond-Type ::= CHOICE
void test_cond_type_c::destroy_() {}
void test_cond_type_c::set(types::options e)
{
  destroy_();
  type_ = e;
}
test_cond_type_c::test_cond_type_c(const test_cond_type_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::gbr:
      c.init(other.c.get<gbr_e_>());
      break;
    case types::ambr:
      c.init(other.c.get<ambr_e_>());
      break;
    case types::is_stat:
      c.init(other.c.get<is_stat_e_>());
      break;
    case types::is_cat_m:
      c.init(other.c.get<is_cat_m_e_>());
      break;
    case types::rsrp:
      c.init(other.c.get<rsrp_e_>());
      break;
    case types::rsrq:
      c.init(other.c.get<rsrq_e_>());
      break;
    case types::ul_r_srp:
      c.init(other.c.get<ul_r_srp_e_>());
      break;
    case types::cqi:
      c.init(other.c.get<cqi_e_>());
      break;
    case types::five_qi:
      c.init(other.c.get<five_qi_e_>());
      break;
    case types::qci:
      c.init(other.c.get<qci_e_>());
      break;
    case types::snssai:
      c.init(other.c.get<snssai_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "test_cond_type_c");
  }
}
test_cond_type_c& test_cond_type_c::operator=(const test_cond_type_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::gbr:
      c.set(other.c.get<gbr_e_>());
      break;
    case types::ambr:
      c.set(other.c.get<ambr_e_>());
      break;
    case types::is_stat:
      c.set(other.c.get<is_stat_e_>());
      break;
    case types::is_cat_m:
      c.set(other.c.get<is_cat_m_e_>());
      break;
    case types::rsrp:
      c.set(other.c.get<rsrp_e_>());
      break;
    case types::rsrq:
      c.set(other.c.get<rsrq_e_>());
      break;
    case types::ul_r_srp:
      c.set(other.c.get<ul_r_srp_e_>());
      break;
    case types::cqi:
      c.set(other.c.get<cqi_e_>());
      break;
    case types::five_qi:
      c.set(other.c.get<five_qi_e_>());
      break;
    case types::qci:
      c.set(other.c.get<qci_e_>());
      break;
    case types::snssai:
      c.set(other.c.get<snssai_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "test_cond_type_c");
  }

  return *this;
}
test_cond_type_c::gbr_e_& test_cond_type_c::set_gbr()
{
  set(types::gbr);
  return c.get<gbr_e_>();
}
test_cond_type_c::ambr_e_& test_cond_type_c::set_ambr()
{
  set(types::ambr);
  return c.get<ambr_e_>();
}
test_cond_type_c::is_stat_e_& test_cond_type_c::set_is_stat()
{
  set(types::is_stat);
  return c.get<is_stat_e_>();
}
test_cond_type_c::is_cat_m_e_& test_cond_type_c::set_is_cat_m()
{
  set(types::is_cat_m);
  return c.get<is_cat_m_e_>();
}
test_cond_type_c::rsrp_e_& test_cond_type_c::set_rsrp()
{
  set(types::rsrp);
  return c.get<rsrp_e_>();
}
test_cond_type_c::rsrq_e_& test_cond_type_c::set_rsrq()
{
  set(types::rsrq);
  return c.get<rsrq_e_>();
}
test_cond_type_c::ul_r_srp_e_& test_cond_type_c::set_ul_r_srp()
{
  set(types::ul_r_srp);
  return c.get<ul_r_srp_e_>();
}
test_cond_type_c::cqi_e_& test_cond_type_c::set_cqi()
{
  set(types::cqi);
  return c.get<cqi_e_>();
}
test_cond_type_c::five_qi_e_& test_cond_type_c::set_five_qi()
{
  set(types::five_qi);
  return c.get<five_qi_e_>();
}
test_cond_type_c::qci_e_& test_cond_type_c::set_qci()
{
  set(types::qci);
  return c.get<qci_e_>();
}
test_cond_type_c::snssai_e_& test_cond_type_c::set_snssai()
{
  set(types::snssai);
  return c.get<snssai_e_>();
}
void test_cond_type_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::gbr:
      j.write_str("gBR", "true");
      break;
    case types::ambr:
      j.write_str("aMBR", "true");
      break;
    case types::is_stat:
      j.write_str("isStat", "true");
      break;
    case types::is_cat_m:
      j.write_str("isCatM", "true");
      break;
    case types::rsrp:
      j.write_str("rSRP", "true");
      break;
    case types::rsrq:
      j.write_str("rSRQ", "true");
      break;
    case types::ul_r_srp:
      j.write_str("ul-rSRP", "true");
      break;
    case types::cqi:
      j.write_str("cQI", "true");
      break;
    case types::five_qi:
      j.write_str("fiveQI", "true");
      break;
    case types::qci:
      j.write_str("qCI", "true");
      break;
    case types::snssai:
      j.write_str("sNSSAI", "true");
      break;
    default:
      log_invalid_choice_id(type_, "test_cond_type_c");
  }
  j.end_obj();
}
SRSASN_CODE test_cond_type_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::gbr:
      HANDLE_CODE(c.get<gbr_e_>().pack(bref));
      break;
    case types::ambr:
      HANDLE_CODE(c.get<ambr_e_>().pack(bref));
      break;
    case types::is_stat:
      HANDLE_CODE(c.get<is_stat_e_>().pack(bref));
      break;
    case types::is_cat_m:
      HANDLE_CODE(c.get<is_cat_m_e_>().pack(bref));
      break;
    case types::rsrp:
      HANDLE_CODE(c.get<rsrp_e_>().pack(bref));
      break;
    case types::rsrq:
      HANDLE_CODE(c.get<rsrq_e_>().pack(bref));
      break;
    case types::ul_r_srp: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<ul_r_srp_e_>().pack(bref));
    } break;
    case types::cqi: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<cqi_e_>().pack(bref));
    } break;
    case types::five_qi: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<five_qi_e_>().pack(bref));
    } break;
    case types::qci: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<qci_e_>().pack(bref));
    } break;
    case types::snssai: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<snssai_e_>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "test_cond_type_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE test_cond_type_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::gbr:
      HANDLE_CODE(c.get<gbr_e_>().unpack(bref));
      break;
    case types::ambr:
      HANDLE_CODE(c.get<ambr_e_>().unpack(bref));
      break;
    case types::is_stat:
      HANDLE_CODE(c.get<is_stat_e_>().unpack(bref));
      break;
    case types::is_cat_m:
      HANDLE_CODE(c.get<is_cat_m_e_>().unpack(bref));
      break;
    case types::rsrp:
      HANDLE_CODE(c.get<rsrp_e_>().unpack(bref));
      break;
    case types::rsrq:
      HANDLE_CODE(c.get<rsrq_e_>().unpack(bref));
      break;
    case types::ul_r_srp: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<ul_r_srp_e_>().unpack(bref));
    } break;
    case types::cqi: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<cqi_e_>().unpack(bref));
    } break;
    case types::five_qi: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<five_qi_e_>().unpack(bref));
    } break;
    case types::qci: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<qci_e_>().unpack(bref));
    } break;
    case types::snssai: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<snssai_e_>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "test_cond_type_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* test_cond_type_c::gbr_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "test_cond_type_c::gbr_e_");
}

const char* test_cond_type_c::ambr_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "test_cond_type_c::ambr_e_");
}

const char* test_cond_type_c::is_stat_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "test_cond_type_c::is_stat_e_");
}

const char* test_cond_type_c::is_cat_m_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "test_cond_type_c::is_cat_m_e_");
}

const char* test_cond_type_c::rsrp_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "test_cond_type_c::rsrp_e_");
}

const char* test_cond_type_c::rsrq_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "test_cond_type_c::rsrq_e_");
}

const char* test_cond_type_c::ul_r_srp_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "test_cond_type_c::ul_r_srp_e_");
}

const char* test_cond_type_c::cqi_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "test_cond_type_c::cqi_e_");
}

const char* test_cond_type_c::five_qi_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "test_cond_type_c::five_qi_e_");
}

const char* test_cond_type_c::qci_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "test_cond_type_c::qci_e_");
}

const char* test_cond_type_c::snssai_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "test_cond_type_c::snssai_e_");
}

const char* test_cond_type_c::types_opts::to_string() const
{
  static const char* options[] = {
      "gBR", "aMBR", "isStat", "isCatM", "rSRP", "rSRQ", "ul-rSRP", "cQI", "fiveQI", "qCI", "sNSSAI"};
  return convert_enum_idx(options, 11, value, "test_cond_type_c::types");
}
uint8_t test_cond_type_c::types_opts::to_number() const
{
  if (value == five_qi) {
    return 5;
  }
  invalid_enum_number(value, "test_cond_type_c::types");
  return 0;
}

// TestCond-Value ::= CHOICE
void test_cond_value_c::destroy_()
{
  switch (type_) {
    case types::value_bool:
      c.destroy<bool>();
      break;
    case types::value_bit_s:
      c.destroy<dyn_bitstring>();
      break;
    case types::value_oct_s:
      c.destroy<unbounded_octstring<true> >();
      break;
    case types::value_prt_s:
      c.destroy<printable_string<0, None, false, true> >();
      break;
    case types::value_real:
      c.destroy<real_s>();
      break;
    default:
      break;
  }
}
void test_cond_value_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::value_int:
      break;
    case types::value_enum:
      break;
    case types::value_bool:
      c.init<bool>();
      break;
    case types::value_bit_s:
      c.init<dyn_bitstring>();
      break;
    case types::value_oct_s:
      c.init<unbounded_octstring<true> >();
      break;
    case types::value_prt_s:
      c.init<printable_string<0, None, false, true> >();
      break;
    case types::value_real:
      c.init<real_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "test_cond_value_c");
  }
}
test_cond_value_c::test_cond_value_c(const test_cond_value_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::value_int:
      c.init(other.c.get<int64_t>());
      break;
    case types::value_enum:
      c.init(other.c.get<int64_t>());
      break;
    case types::value_bool:
      c.init(other.c.get<bool>());
      break;
    case types::value_bit_s:
      c.init(other.c.get<dyn_bitstring>());
      break;
    case types::value_oct_s:
      c.init(other.c.get<unbounded_octstring<true> >());
      break;
    case types::value_prt_s:
      c.init(other.c.get<printable_string<0, None, false, true> >());
      break;
    case types::value_real:
      c.init(other.c.get<real_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "test_cond_value_c");
  }
}
test_cond_value_c& test_cond_value_c::operator=(const test_cond_value_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::value_int:
      c.set(other.c.get<int64_t>());
      break;
    case types::value_enum:
      c.set(other.c.get<int64_t>());
      break;
    case types::value_bool:
      c.set(other.c.get<bool>());
      break;
    case types::value_bit_s:
      c.set(other.c.get<dyn_bitstring>());
      break;
    case types::value_oct_s:
      c.set(other.c.get<unbounded_octstring<true> >());
      break;
    case types::value_prt_s:
      c.set(other.c.get<printable_string<0, None, false, true> >());
      break;
    case types::value_real:
      c.set(other.c.get<real_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "test_cond_value_c");
  }

  return *this;
}
int64_t& test_cond_value_c::set_value_int()
{
  set(types::value_int);
  return c.get<int64_t>();
}
int64_t& test_cond_value_c::set_value_enum()
{
  set(types::value_enum);
  return c.get<int64_t>();
}
bool& test_cond_value_c::set_value_bool()
{
  set(types::value_bool);
  return c.get<bool>();
}
dyn_bitstring& test_cond_value_c::set_value_bit_s()
{
  set(types::value_bit_s);
  return c.get<dyn_bitstring>();
}
unbounded_octstring<true>& test_cond_value_c::set_value_oct_s()
{
  set(types::value_oct_s);
  return c.get<unbounded_octstring<true> >();
}
printable_string<0, None, false, true>& test_cond_value_c::set_value_prt_s()
{
  set(types::value_prt_s);
  return c.get<printable_string<0, None, false, true> >();
}
real_s& test_cond_value_c::set_value_real()
{
  set(types::value_real);
  return c.get<real_s>();
}
void test_cond_value_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::value_int:
      j.write_int("valueInt", c.get<int64_t>());
      break;
    case types::value_enum:
      j.write_int("valueEnum", c.get<int64_t>());
      break;
    case types::value_bool:
      j.write_bool("valueBool", c.get<bool>());
      break;
    case types::value_bit_s:
      j.write_str("valueBitS", c.get<dyn_bitstring>().to_string());
      break;
    case types::value_oct_s:
      j.write_str("valueOctS", c.get<unbounded_octstring<true> >().to_string());
      break;
    case types::value_prt_s:
      j.write_str("valuePrtS", c.get<printable_string<0, None, false, true> >().to_string());
      break;
    case types::value_real:
      j.write_fieldname("valueReal");
      c.get<real_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "test_cond_value_c");
  }
  j.end_obj();
}
SRSASN_CODE test_cond_value_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::value_int:
      HANDLE_CODE(pack_unconstrained_integer(bref, c.get<int64_t>(), false, true));
      break;
    case types::value_enum:
      HANDLE_CODE(pack_unconstrained_integer(bref, c.get<int64_t>(), false, true));
      break;
    case types::value_bool:
      HANDLE_CODE(bref.pack(c.get<bool>(), 1));
      break;
    case types::value_bit_s:
      HANDLE_CODE(c.get<dyn_bitstring>().pack(bref));
      break;
    case types::value_oct_s:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().pack(bref));
      break;
    case types::value_prt_s:
      HANDLE_CODE((c.get<printable_string<0, None, false, true> >().pack(bref)));
      break;
    case types::value_real: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<real_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "test_cond_value_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE test_cond_value_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::value_int:
      HANDLE_CODE(unpack_unconstrained_integer(c.get<int64_t>(), bref, false, true));
      break;
    case types::value_enum:
      HANDLE_CODE(unpack_unconstrained_integer(c.get<int64_t>(), bref, false, true));
      break;
    case types::value_bool:
      HANDLE_CODE(bref.unpack(c.get<bool>(), 1));
      break;
    case types::value_bit_s:
      HANDLE_CODE(c.get<dyn_bitstring>().unpack(bref));
      break;
    case types::value_oct_s:
      HANDLE_CODE(c.get<unbounded_octstring<true> >().unpack(bref));
      break;
    case types::value_prt_s:
      HANDLE_CODE((c.get<printable_string<0, None, false, true> >().unpack(bref)));
      break;
    case types::value_real: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<real_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "test_cond_value_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* test_cond_value_c::types_opts::to_string() const
{
  static const char* options[] = {
      "valueInt", "valueEnum", "valueBool", "valueBitS", "valueOctS", "valuePrtS", "valueReal"};
  return convert_enum_idx(options, 7, value, "test_cond_value_c::types");
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

// MeasurementLabel ::= SEQUENCE
SRSASN_CODE meas_label_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(no_label_present, 1));
  HANDLE_CODE(bref.pack(plmn_id_present, 1));
  HANDLE_CODE(bref.pack(slice_id_present, 1));
  HANDLE_CODE(bref.pack(five_qi_present, 1));
  HANDLE_CODE(bref.pack(qfi_present, 1));
  HANDLE_CODE(bref.pack(qci_present, 1));
  HANDLE_CODE(bref.pack(qcimax_present, 1));
  HANDLE_CODE(bref.pack(qcimin_present, 1));
  HANDLE_CODE(bref.pack(arpmax_present, 1));
  HANDLE_CODE(bref.pack(arpmin_present, 1));
  HANDLE_CODE(bref.pack(bitrate_range_present, 1));
  HANDLE_CODE(bref.pack(layer_mu_mimo_present, 1));
  HANDLE_CODE(bref.pack(sum_present, 1));
  HANDLE_CODE(bref.pack(dist_bin_x_present, 1));
  HANDLE_CODE(bref.pack(dist_bin_y_present, 1));
  HANDLE_CODE(bref.pack(dist_bin_z_present, 1));
  HANDLE_CODE(bref.pack(pre_label_override_present, 1));
  HANDLE_CODE(bref.pack(start_end_ind_present, 1));
  HANDLE_CODE(bref.pack(min_present, 1));
  HANDLE_CODE(bref.pack(max_present, 1));
  HANDLE_CODE(bref.pack(avg_present, 1));

  if (no_label_present) {
    HANDLE_CODE(no_label.pack(bref));
  }
  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.pack(bref));
  }
  if (slice_id_present) {
    HANDLE_CODE(slice_id.pack(bref));
  }
  if (five_qi_present) {
    HANDLE_CODE(pack_integer(bref, five_qi, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (qfi_present) {
    HANDLE_CODE(pack_integer(bref, qfi, (uint8_t)0u, (uint8_t)63u, true, true));
  }
  if (qci_present) {
    HANDLE_CODE(pack_integer(bref, qci, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (qcimax_present) {
    HANDLE_CODE(pack_integer(bref, qcimax, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (qcimin_present) {
    HANDLE_CODE(pack_integer(bref, qcimin, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (arpmax_present) {
    HANDLE_CODE(pack_integer(bref, arpmax, (uint8_t)1u, (uint8_t)15u, true, true));
  }
  if (arpmin_present) {
    HANDLE_CODE(pack_integer(bref, arpmin, (uint8_t)1u, (uint8_t)15u, true, true));
  }
  if (bitrate_range_present) {
    HANDLE_CODE(pack_integer(bref, bitrate_range, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (layer_mu_mimo_present) {
    HANDLE_CODE(pack_integer(bref, layer_mu_mimo, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (sum_present) {
    HANDLE_CODE(sum.pack(bref));
  }
  if (dist_bin_x_present) {
    HANDLE_CODE(pack_integer(bref, dist_bin_x, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (dist_bin_y_present) {
    HANDLE_CODE(pack_integer(bref, dist_bin_y, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (dist_bin_z_present) {
    HANDLE_CODE(pack_integer(bref, dist_bin_z, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (pre_label_override_present) {
    HANDLE_CODE(pre_label_override.pack(bref));
  }
  if (start_end_ind_present) {
    HANDLE_CODE(start_end_ind.pack(bref));
  }
  if (min_present) {
    HANDLE_CODE(min.pack(bref));
  }
  if (max_present) {
    HANDLE_CODE(max.pack(bref));
  }
  if (avg_present) {
    HANDLE_CODE(avg.pack(bref));
  }

  if (ext) {
    HANDLE_CODE(bref.pack(ssb_idx_present, 1));
    HANDLE_CODE(bref.pack(non_go_b_bfmode_idx_present, 1));
    HANDLE_CODE(bref.pack(mimo_mode_idx_present, 1));

    if (ssb_idx_present) {
      HANDLE_CODE(pack_integer(bref, ssb_idx, (uint32_t)1u, (uint32_t)65535u, true, true));
    }
    if (non_go_b_bfmode_idx_present) {
      HANDLE_CODE(pack_integer(bref, non_go_b_bfmode_idx, (uint32_t)1u, (uint32_t)65535u, true, true));
    }
    if (mimo_mode_idx_present) {
      HANDLE_CODE(pack_integer(bref, mimo_mode_idx, (uint8_t)1u, (uint8_t)2u, true, true));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_label_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(no_label_present, 1));
  HANDLE_CODE(bref.unpack(plmn_id_present, 1));
  HANDLE_CODE(bref.unpack(slice_id_present, 1));
  HANDLE_CODE(bref.unpack(five_qi_present, 1));
  HANDLE_CODE(bref.unpack(qfi_present, 1));
  HANDLE_CODE(bref.unpack(qci_present, 1));
  HANDLE_CODE(bref.unpack(qcimax_present, 1));
  HANDLE_CODE(bref.unpack(qcimin_present, 1));
  HANDLE_CODE(bref.unpack(arpmax_present, 1));
  HANDLE_CODE(bref.unpack(arpmin_present, 1));
  HANDLE_CODE(bref.unpack(bitrate_range_present, 1));
  HANDLE_CODE(bref.unpack(layer_mu_mimo_present, 1));
  HANDLE_CODE(bref.unpack(sum_present, 1));
  HANDLE_CODE(bref.unpack(dist_bin_x_present, 1));
  HANDLE_CODE(bref.unpack(dist_bin_y_present, 1));
  HANDLE_CODE(bref.unpack(dist_bin_z_present, 1));
  HANDLE_CODE(bref.unpack(pre_label_override_present, 1));
  HANDLE_CODE(bref.unpack(start_end_ind_present, 1));
  HANDLE_CODE(bref.unpack(min_present, 1));
  HANDLE_CODE(bref.unpack(max_present, 1));
  HANDLE_CODE(bref.unpack(avg_present, 1));

  if (no_label_present) {
    HANDLE_CODE(no_label.unpack(bref));
  }
  if (plmn_id_present) {
    HANDLE_CODE(plmn_id.unpack(bref));
  }
  if (slice_id_present) {
    HANDLE_CODE(slice_id.unpack(bref));
  }
  if (five_qi_present) {
    HANDLE_CODE(unpack_integer(five_qi, bref, (uint16_t)0u, (uint16_t)255u, true, true));
  }
  if (qfi_present) {
    HANDLE_CODE(unpack_integer(qfi, bref, (uint8_t)0u, (uint8_t)63u, true, true));
  }
  if (qci_present) {
    HANDLE_CODE(unpack_integer(qci, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (qcimax_present) {
    HANDLE_CODE(unpack_integer(qcimax, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (qcimin_present) {
    HANDLE_CODE(unpack_integer(qcimin, bref, (uint16_t)0u, (uint16_t)255u, false, true));
  }
  if (arpmax_present) {
    HANDLE_CODE(unpack_integer(arpmax, bref, (uint8_t)1u, (uint8_t)15u, true, true));
  }
  if (arpmin_present) {
    HANDLE_CODE(unpack_integer(arpmin, bref, (uint8_t)1u, (uint8_t)15u, true, true));
  }
  if (bitrate_range_present) {
    HANDLE_CODE(unpack_integer(bitrate_range, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (layer_mu_mimo_present) {
    HANDLE_CODE(unpack_integer(layer_mu_mimo, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (sum_present) {
    HANDLE_CODE(sum.unpack(bref));
  }
  if (dist_bin_x_present) {
    HANDLE_CODE(unpack_integer(dist_bin_x, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (dist_bin_y_present) {
    HANDLE_CODE(unpack_integer(dist_bin_y, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (dist_bin_z_present) {
    HANDLE_CODE(unpack_integer(dist_bin_z, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
  }
  if (pre_label_override_present) {
    HANDLE_CODE(pre_label_override.unpack(bref));
  }
  if (start_end_ind_present) {
    HANDLE_CODE(start_end_ind.unpack(bref));
  }
  if (min_present) {
    HANDLE_CODE(min.unpack(bref));
  }
  if (max_present) {
    HANDLE_CODE(max.unpack(bref));
  }
  if (avg_present) {
    HANDLE_CODE(avg.unpack(bref));
  }

  if (ext) {
    HANDLE_CODE(bref.unpack(ssb_idx_present, 1));
    HANDLE_CODE(bref.unpack(non_go_b_bfmode_idx_present, 1));
    HANDLE_CODE(bref.unpack(mimo_mode_idx_present, 1));

    if (ssb_idx_present) {
      HANDLE_CODE(unpack_integer(ssb_idx, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
    }
    if (non_go_b_bfmode_idx_present) {
      HANDLE_CODE(unpack_integer(non_go_b_bfmode_idx, bref, (uint32_t)1u, (uint32_t)65535u, true, true));
    }
    if (mimo_mode_idx_present) {
      HANDLE_CODE(unpack_integer(mimo_mode_idx, bref, (uint8_t)1u, (uint8_t)2u, true, true));
    }
  }
  return SRSASN_SUCCESS;
}
void meas_label_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (no_label_present) {
    j.write_str("noLabel", "true");
  }
  if (plmn_id_present) {
    j.write_str("plmnID", plmn_id.to_string());
  }
  if (slice_id_present) {
    j.write_fieldname("sliceID");
    slice_id.to_json(j);
  }
  if (five_qi_present) {
    j.write_int("fiveQI", five_qi);
  }
  if (qfi_present) {
    j.write_int("qFI", qfi);
  }
  if (qci_present) {
    j.write_int("qCI", qci);
  }
  if (qcimax_present) {
    j.write_int("qCImax", qcimax);
  }
  if (qcimin_present) {
    j.write_int("qCImin", qcimin);
  }
  if (arpmax_present) {
    j.write_int("aRPmax", arpmax);
  }
  if (arpmin_present) {
    j.write_int("aRPmin", arpmin);
  }
  if (bitrate_range_present) {
    j.write_int("bitrateRange", bitrate_range);
  }
  if (layer_mu_mimo_present) {
    j.write_int("layerMU-MIMO", layer_mu_mimo);
  }
  if (sum_present) {
    j.write_str("sUM", "true");
  }
  if (dist_bin_x_present) {
    j.write_int("distBinX", dist_bin_x);
  }
  if (dist_bin_y_present) {
    j.write_int("distBinY", dist_bin_y);
  }
  if (dist_bin_z_present) {
    j.write_int("distBinZ", dist_bin_z);
  }
  if (pre_label_override_present) {
    j.write_str("preLabelOverride", "true");
  }
  if (start_end_ind_present) {
    j.write_str("startEndInd", start_end_ind.to_string());
  }
  if (min_present) {
    j.write_str("min", "true");
  }
  if (max_present) {
    j.write_str("max", "true");
  }
  if (avg_present) {
    j.write_str("avg", "true");
  }
  if (ext) {
    if (ssb_idx_present) {
      j.write_int("ssbIndex", ssb_idx);
    }
    if (non_go_b_bfmode_idx_present) {
      j.write_int("nonGoB-BFmode-Index", non_go_b_bfmode_idx);
    }
    if (mimo_mode_idx_present) {
      j.write_int("mIMO-mode-Index", mimo_mode_idx);
    }
  }
  j.end_obj();
}

const char* meas_label_s::no_label_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "meas_label_s::no_label_e_");
}

const char* meas_label_s::sum_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "meas_label_s::sum_e_");
}

const char* meas_label_s::pre_label_override_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "meas_label_s::pre_label_override_e_");
}

const char* meas_label_s::start_end_ind_opts::to_string() const
{
  static const char* options[] = {"start", "end"};
  return convert_enum_idx(options, 2, value, "meas_label_s::start_end_ind_e_");
}

const char* meas_label_s::min_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "meas_label_s::min_e_");
}

const char* meas_label_s::max_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "meas_label_s::max_e_");
}

const char* meas_label_s::avg_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "meas_label_s::avg_e_");
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

// TestCondInfo ::= SEQUENCE
SRSASN_CODE test_cond_info_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(test_expr_present, 1));
  HANDLE_CODE(bref.pack(test_value_present, 1));

  HANDLE_CODE(test_type.pack(bref));
  if (test_expr_present) {
    HANDLE_CODE(test_expr.pack(bref));
  }
  if (test_value_present) {
    HANDLE_CODE(test_value.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE test_cond_info_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(test_expr_present, 1));
  HANDLE_CODE(bref.unpack(test_value_present, 1));

  HANDLE_CODE(test_type.unpack(bref));
  if (test_expr_present) {
    HANDLE_CODE(test_expr.unpack(bref));
  }
  if (test_value_present) {
    HANDLE_CODE(test_value.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void test_cond_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("testType");
  test_type.to_json(j);
  if (test_expr_present) {
    j.write_str("testExpr", test_expr.to_string());
  }
  if (test_value_present) {
    j.write_fieldname("testValue");
    test_value.to_json(j);
  }
  j.end_obj();
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

// LabelInfoItem ::= SEQUENCE
SRSASN_CODE label_info_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(meas_label.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE label_info_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(meas_label.unpack(bref));

  return SRSASN_SUCCESS;
}
void label_info_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measLabel");
  meas_label.to_json(j);
  j.end_obj();
}

// LogicalOR ::= ENUMERATED
const char* lc_or_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "lc_or_e");
}

// MatchingCondItem-Choice ::= CHOICE
void matching_cond_item_choice_c::destroy_()
{
  switch (type_) {
    case types::meas_label:
      c.destroy<meas_label_s>();
      break;
    case types::test_cond_info:
      c.destroy<test_cond_info_s>();
      break;
    default:
      break;
  }
}
void matching_cond_item_choice_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::meas_label:
      c.init<meas_label_s>();
      break;
    case types::test_cond_info:
      c.init<test_cond_info_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "matching_cond_item_choice_c");
  }
}
matching_cond_item_choice_c::matching_cond_item_choice_c(const matching_cond_item_choice_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::meas_label:
      c.init(other.c.get<meas_label_s>());
      break;
    case types::test_cond_info:
      c.init(other.c.get<test_cond_info_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "matching_cond_item_choice_c");
  }
}
matching_cond_item_choice_c& matching_cond_item_choice_c::operator=(const matching_cond_item_choice_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::meas_label:
      c.set(other.c.get<meas_label_s>());
      break;
    case types::test_cond_info:
      c.set(other.c.get<test_cond_info_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "matching_cond_item_choice_c");
  }

  return *this;
}
meas_label_s& matching_cond_item_choice_c::set_meas_label()
{
  set(types::meas_label);
  return c.get<meas_label_s>();
}
test_cond_info_s& matching_cond_item_choice_c::set_test_cond_info()
{
  set(types::test_cond_info);
  return c.get<test_cond_info_s>();
}
void matching_cond_item_choice_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::meas_label:
      j.write_fieldname("measLabel");
      c.get<meas_label_s>().to_json(j);
      break;
    case types::test_cond_info:
      j.write_fieldname("testCondInfo");
      c.get<test_cond_info_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "matching_cond_item_choice_c");
  }
  j.end_obj();
}
SRSASN_CODE matching_cond_item_choice_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::meas_label:
      HANDLE_CODE(c.get<meas_label_s>().pack(bref));
      break;
    case types::test_cond_info:
      HANDLE_CODE(c.get<test_cond_info_s>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "matching_cond_item_choice_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE matching_cond_item_choice_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::meas_label:
      HANDLE_CODE(c.get<meas_label_s>().unpack(bref));
      break;
    case types::test_cond_info:
      HANDLE_CODE(c.get<test_cond_info_s>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "matching_cond_item_choice_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* matching_cond_item_choice_c::types_opts::to_string() const
{
  static const char* options[] = {"measLabel", "testCondInfo"};
  return convert_enum_idx(options, 2, value, "matching_cond_item_choice_c::types");
}

// MatchingCondItem ::= SEQUENCE
SRSASN_CODE matching_cond_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(lc_or_present, 1));

  HANDLE_CODE(matching_cond_choice.pack(bref));
  if (lc_or_present) {
    HANDLE_CODE(lc_or.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE matching_cond_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(lc_or_present, 1));

  HANDLE_CODE(matching_cond_choice.unpack(bref));
  if (lc_or_present) {
    HANDLE_CODE(lc_or.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void matching_cond_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("matchingCondChoice");
  matching_cond_choice.to_json(j);
  if (lc_or_present) {
    j.write_str("logicalOR", "true");
  }
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

// MeasurementInfoItem ::= SEQUENCE
SRSASN_CODE meas_info_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(meas_type.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, label_info_list, 1, 2147483647, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_info_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(meas_type.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(label_info_list, bref, 1, 2147483647, true));

  return SRSASN_SUCCESS;
}
void meas_info_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measType");
  meas_type.to_json(j);
  j.start_array("labelInfoList");
  for (const auto& e1 : label_info_list) {
    e1.to_json(j);
  }
  j.end_array();
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

// MatchingUEidPerSubItem ::= SEQUENCE
SRSASN_CODE matching_ueid_per_sub_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ue_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE matching_ueid_per_sub_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ue_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void matching_ueid_per_sub_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ueID");
  ue_id.to_json(j);
  j.end_obj();
}

// MatchingUeCondPerSubItem ::= SEQUENCE
SRSASN_CODE matching_ue_cond_per_sub_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(test_cond_info.pack(bref));

  if (ext) {
    HANDLE_CODE(bref.pack(lc_or_present, 1));

    if (lc_or_present) {
      HANDLE_CODE(lc_or.pack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE matching_ue_cond_per_sub_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(test_cond_info.unpack(bref));

  if (ext) {
    HANDLE_CODE(bref.unpack(lc_or_present, 1));

    if (lc_or_present) {
      HANDLE_CODE(lc_or.unpack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
void matching_ue_cond_per_sub_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("testCondInfo");
  test_cond_info.to_json(j);
  if (ext) {
    if (lc_or_present) {
      j.write_str("logicalOR", "true");
    }
  }
  j.end_obj();
}

// MeasurementCondItem ::= SEQUENCE
SRSASN_CODE meas_cond_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(meas_type.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, matching_cond, 1, 32768, true));

  if (ext) {
    HANDLE_CODE(bref.pack(bin_range_def.is_present(), 1));

    if (bin_range_def.is_present()) {
      HANDLE_CODE(bin_range_def->pack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_cond_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(meas_type.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(matching_cond, bref, 1, 32768, true));

  if (ext) {
    bool bin_range_def_present;
    HANDLE_CODE(bref.unpack(bin_range_def_present, 1));
    bin_range_def.set_present(bin_range_def_present);

    if (bin_range_def.is_present()) {
      HANDLE_CODE(bin_range_def->unpack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
void meas_cond_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measType");
  meas_type.to_json(j);
  j.start_array("matchingCond");
  for (const auto& e1 : matching_cond) {
    e1.to_json(j);
  }
  j.end_array();
  if (ext) {
    if (bin_range_def.is_present()) {
      j.write_fieldname("binRangeDef");
      bin_range_def->to_json(j);
    }
  }
  j.end_obj();
}

// E2SM-KPM-ActionDefinition-Format1 ::= SEQUENCE
SRSASN_CODE e2_sm_kpm_action_definition_format1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_global_id_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, meas_info_list, 1, 65535, true));
  HANDLE_CODE(pack_integer(bref, granul_period, (uint64_t)1u, (uint64_t)4294967295u, false, true));
  if (cell_global_id_present) {
    HANDLE_CODE(cell_global_id.pack(bref));
  }

  if (ext) {
    HANDLE_CODE(bref.pack(dist_meas_bin_range_info.is_present(), 1));

    if (dist_meas_bin_range_info.is_present()) {
      HANDLE_CODE(pack_dyn_seq_of(bref, *dist_meas_bin_range_info, 1, 65535, true));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_action_definition_format1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cell_global_id_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(meas_info_list, bref, 1, 65535, true));
  HANDLE_CODE(unpack_integer(granul_period, bref, (uint64_t)1u, (uint64_t)4294967295u, false, true));
  if (cell_global_id_present) {
    HANDLE_CODE(cell_global_id.unpack(bref));
  }

  if (ext) {
    bool dist_meas_bin_range_info_present;
    HANDLE_CODE(bref.unpack(dist_meas_bin_range_info_present, 1));
    dist_meas_bin_range_info.set_present(dist_meas_bin_range_info_present);

    if (dist_meas_bin_range_info.is_present()) {
      HANDLE_CODE(unpack_dyn_seq_of(*dist_meas_bin_range_info, bref, 1, 65535, true));
    }
  }
  return SRSASN_SUCCESS;
}
void e2_sm_kpm_action_definition_format1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measInfoList");
  for (const auto& e1 : meas_info_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("granulPeriod", granul_period);
  if (cell_global_id_present) {
    j.write_fieldname("cellGlobalID");
    cell_global_id.to_json(j);
  }
  if (ext) {
    if (dist_meas_bin_range_info.is_present()) {
      j.start_array("distMeasBinRangeInfo");
      for (const auto& e1 : *dist_meas_bin_range_info) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// E2SM-KPM-ActionDefinition-Format2 ::= SEQUENCE
SRSASN_CODE e2_sm_kpm_action_definition_format2_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ue_id.pack(bref));
  HANDLE_CODE(subscript_info.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_action_definition_format2_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ue_id.unpack(bref));
  HANDLE_CODE(subscript_info.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2_sm_kpm_action_definition_format2_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ueID");
  ue_id.to_json(j);
  j.write_fieldname("subscriptInfo");
  subscript_info.to_json(j);
  j.end_obj();
}

// E2SM-KPM-ActionDefinition-Format3 ::= SEQUENCE
SRSASN_CODE e2_sm_kpm_action_definition_format3_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(cell_global_id_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, meas_cond_list, 1, 65535, true));
  HANDLE_CODE(pack_integer(bref, granul_period, (uint64_t)1u, (uint64_t)4294967295u, false, true));
  if (cell_global_id_present) {
    HANDLE_CODE(cell_global_id.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_action_definition_format3_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(cell_global_id_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(meas_cond_list, bref, 1, 65535, true));
  HANDLE_CODE(unpack_integer(granul_period, bref, (uint64_t)1u, (uint64_t)4294967295u, false, true));
  if (cell_global_id_present) {
    HANDLE_CODE(cell_global_id.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void e2_sm_kpm_action_definition_format3_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measCondList");
  for (const auto& e1 : meas_cond_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("granulPeriod", granul_period);
  if (cell_global_id_present) {
    j.write_fieldname("cellGlobalID");
    cell_global_id.to_json(j);
  }
  j.end_obj();
}

// E2SM-KPM-ActionDefinition-Format4 ::= SEQUENCE
SRSASN_CODE e2_sm_kpm_action_definition_format4_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, matching_ue_cond_list, 1, 32768, true));
  HANDLE_CODE(subscription_info.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_action_definition_format4_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(matching_ue_cond_list, bref, 1, 32768, true));
  HANDLE_CODE(subscription_info.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2_sm_kpm_action_definition_format4_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("matchingUeCondList");
  for (const auto& e1 : matching_ue_cond_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_fieldname("subscriptionInfo");
  subscription_info.to_json(j);
  j.end_obj();
}

// E2SM-KPM-ActionDefinition-Format5 ::= SEQUENCE
SRSASN_CODE e2_sm_kpm_action_definition_format5_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, matching_ueid_list, 2, 65535, true));
  HANDLE_CODE(subscription_info.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_action_definition_format5_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(matching_ueid_list, bref, 2, 65535, true));
  HANDLE_CODE(subscription_info.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2_sm_kpm_action_definition_format5_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("matchingUEidList");
  for (const auto& e1 : matching_ueid_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_fieldname("subscriptionInfo");
  subscription_info.to_json(j);
  j.end_obj();
}

// E2SM-KPM-ActionDefinition ::= SEQUENCE
SRSASN_CODE e2_sm_kpm_action_definition_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_unconstrained_integer(bref, ric_style_type, false, true));
  HANDLE_CODE(action_definition_formats.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_action_definition_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_unconstrained_integer(ric_style_type, bref, false, true));
  HANDLE_CODE(action_definition_formats.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2_sm_kpm_action_definition_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ric-Style-Type", ric_style_type);
  j.write_fieldname("actionDefinition-formats");
  action_definition_formats.to_json(j);
  j.end_obj();
}

void e2_sm_kpm_action_definition_s::action_definition_formats_c_::destroy_()
{
  switch (type_) {
    case types::action_definition_format1:
      c.destroy<e2_sm_kpm_action_definition_format1_s>();
      break;
    case types::action_definition_format2:
      c.destroy<e2_sm_kpm_action_definition_format2_s>();
      break;
    case types::action_definition_format3:
      c.destroy<e2_sm_kpm_action_definition_format3_s>();
      break;
    case types::action_definition_format4:
      c.destroy<e2_sm_kpm_action_definition_format4_s>();
      break;
    case types::action_definition_format5:
      c.destroy<e2_sm_kpm_action_definition_format5_s>();
      break;
    default:
      break;
  }
}
void e2_sm_kpm_action_definition_s::action_definition_formats_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::action_definition_format1:
      c.init<e2_sm_kpm_action_definition_format1_s>();
      break;
    case types::action_definition_format2:
      c.init<e2_sm_kpm_action_definition_format2_s>();
      break;
    case types::action_definition_format3:
      c.init<e2_sm_kpm_action_definition_format3_s>();
      break;
    case types::action_definition_format4:
      c.init<e2_sm_kpm_action_definition_format4_s>();
      break;
    case types::action_definition_format5:
      c.init<e2_sm_kpm_action_definition_format5_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_sm_kpm_action_definition_s::action_definition_formats_c_");
  }
}
e2_sm_kpm_action_definition_s::action_definition_formats_c_::action_definition_formats_c_(
    const e2_sm_kpm_action_definition_s::action_definition_formats_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::action_definition_format1:
      c.init(other.c.get<e2_sm_kpm_action_definition_format1_s>());
      break;
    case types::action_definition_format2:
      c.init(other.c.get<e2_sm_kpm_action_definition_format2_s>());
      break;
    case types::action_definition_format3:
      c.init(other.c.get<e2_sm_kpm_action_definition_format3_s>());
      break;
    case types::action_definition_format4:
      c.init(other.c.get<e2_sm_kpm_action_definition_format4_s>());
      break;
    case types::action_definition_format5:
      c.init(other.c.get<e2_sm_kpm_action_definition_format5_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_sm_kpm_action_definition_s::action_definition_formats_c_");
  }
}
e2_sm_kpm_action_definition_s::action_definition_formats_c_&
e2_sm_kpm_action_definition_s::action_definition_formats_c_::operator=(
    const e2_sm_kpm_action_definition_s::action_definition_formats_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::action_definition_format1:
      c.set(other.c.get<e2_sm_kpm_action_definition_format1_s>());
      break;
    case types::action_definition_format2:
      c.set(other.c.get<e2_sm_kpm_action_definition_format2_s>());
      break;
    case types::action_definition_format3:
      c.set(other.c.get<e2_sm_kpm_action_definition_format3_s>());
      break;
    case types::action_definition_format4:
      c.set(other.c.get<e2_sm_kpm_action_definition_format4_s>());
      break;
    case types::action_definition_format5:
      c.set(other.c.get<e2_sm_kpm_action_definition_format5_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_sm_kpm_action_definition_s::action_definition_formats_c_");
  }

  return *this;
}
e2_sm_kpm_action_definition_format1_s&
e2_sm_kpm_action_definition_s::action_definition_formats_c_::set_action_definition_format1()
{
  set(types::action_definition_format1);
  return c.get<e2_sm_kpm_action_definition_format1_s>();
}
e2_sm_kpm_action_definition_format2_s&
e2_sm_kpm_action_definition_s::action_definition_formats_c_::set_action_definition_format2()
{
  set(types::action_definition_format2);
  return c.get<e2_sm_kpm_action_definition_format2_s>();
}
e2_sm_kpm_action_definition_format3_s&
e2_sm_kpm_action_definition_s::action_definition_formats_c_::set_action_definition_format3()
{
  set(types::action_definition_format3);
  return c.get<e2_sm_kpm_action_definition_format3_s>();
}
e2_sm_kpm_action_definition_format4_s&
e2_sm_kpm_action_definition_s::action_definition_formats_c_::set_action_definition_format4()
{
  set(types::action_definition_format4);
  return c.get<e2_sm_kpm_action_definition_format4_s>();
}
e2_sm_kpm_action_definition_format5_s&
e2_sm_kpm_action_definition_s::action_definition_formats_c_::set_action_definition_format5()
{
  set(types::action_definition_format5);
  return c.get<e2_sm_kpm_action_definition_format5_s>();
}
void e2_sm_kpm_action_definition_s::action_definition_formats_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::action_definition_format1:
      j.write_fieldname("actionDefinition-Format1");
      c.get<e2_sm_kpm_action_definition_format1_s>().to_json(j);
      break;
    case types::action_definition_format2:
      j.write_fieldname("actionDefinition-Format2");
      c.get<e2_sm_kpm_action_definition_format2_s>().to_json(j);
      break;
    case types::action_definition_format3:
      j.write_fieldname("actionDefinition-Format3");
      c.get<e2_sm_kpm_action_definition_format3_s>().to_json(j);
      break;
    case types::action_definition_format4:
      j.write_fieldname("actionDefinition-Format4");
      c.get<e2_sm_kpm_action_definition_format4_s>().to_json(j);
      break;
    case types::action_definition_format5:
      j.write_fieldname("actionDefinition-Format5");
      c.get<e2_sm_kpm_action_definition_format5_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2_sm_kpm_action_definition_s::action_definition_formats_c_");
  }
  j.end_obj();
}
SRSASN_CODE e2_sm_kpm_action_definition_s::action_definition_formats_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::action_definition_format1:
      HANDLE_CODE(c.get<e2_sm_kpm_action_definition_format1_s>().pack(bref));
      break;
    case types::action_definition_format2:
      HANDLE_CODE(c.get<e2_sm_kpm_action_definition_format2_s>().pack(bref));
      break;
    case types::action_definition_format3:
      HANDLE_CODE(c.get<e2_sm_kpm_action_definition_format3_s>().pack(bref));
      break;
    case types::action_definition_format4: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<e2_sm_kpm_action_definition_format4_s>().pack(bref));
    } break;
    case types::action_definition_format5: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<e2_sm_kpm_action_definition_format5_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "e2_sm_kpm_action_definition_s::action_definition_formats_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_action_definition_s::action_definition_formats_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::action_definition_format1:
      HANDLE_CODE(c.get<e2_sm_kpm_action_definition_format1_s>().unpack(bref));
      break;
    case types::action_definition_format2:
      HANDLE_CODE(c.get<e2_sm_kpm_action_definition_format2_s>().unpack(bref));
      break;
    case types::action_definition_format3:
      HANDLE_CODE(c.get<e2_sm_kpm_action_definition_format3_s>().unpack(bref));
      break;
    case types::action_definition_format4: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<e2_sm_kpm_action_definition_format4_s>().unpack(bref));
    } break;
    case types::action_definition_format5: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<e2_sm_kpm_action_definition_format5_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "e2_sm_kpm_action_definition_s::action_definition_formats_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::to_string() const
{
  static const char* options[] = {"actionDefinition-Format1",
                                  "actionDefinition-Format2",
                                  "actionDefinition-Format3",
                                  "actionDefinition-Format4",
                                  "actionDefinition-Format5"};
  return convert_enum_idx(options, 5, value, "e2_sm_kpm_action_definition_s::action_definition_formats_c_::types");
}
uint8_t e2_sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5};
  return map_enum_number(options, 5, value, "e2_sm_kpm_action_definition_s::action_definition_formats_c_::types");
}

// E2SM-KPM-EventTriggerDefinition-Format1 ::= SEQUENCE
SRSASN_CODE e2_sm_kpm_event_trigger_definition_format1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, report_period, (uint64_t)1u, (uint64_t)4294967295u, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_event_trigger_definition_format1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(report_period, bref, (uint64_t)1u, (uint64_t)4294967295u, false, true));

  return SRSASN_SUCCESS;
}
void e2_sm_kpm_event_trigger_definition_format1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("reportingPeriod", report_period);
  j.end_obj();
}

// E2SM-KPM-EventTriggerDefinition ::= SEQUENCE
SRSASN_CODE e2_sm_kpm_event_trigger_definition_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(event_definition_formats.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_event_trigger_definition_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(event_definition_formats.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2_sm_kpm_event_trigger_definition_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("eventDefinition-formats");
  event_definition_formats.to_json(j);
  j.end_obj();
}

void e2_sm_kpm_event_trigger_definition_s::event_definition_formats_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("eventDefinition-Format1");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2_sm_kpm_event_trigger_definition_s::event_definition_formats_c_::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_event_trigger_definition_s::event_definition_formats_c_::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "e2_sm_kpm_event_trigger_definition_s::event_definition_formats_c_");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2_sm_kpm_event_trigger_definition_s::event_definition_formats_c_::types_opts::to_string() const
{
  static const char* options[] = {"eventDefinition-Format1"};
  return convert_enum_idx(
      options, 1, value, "e2_sm_kpm_event_trigger_definition_s::event_definition_formats_c_::types");
}
uint8_t e2_sm_kpm_event_trigger_definition_s::event_definition_formats_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "e2_sm_kpm_event_trigger_definition_s::event_definition_formats_c_::types");
}

// E2SM-KPM-IndicationHeader-Format1 ::= SEQUENCE
SRSASN_CODE e2_sm_kpm_ind_hdr_format1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(file_formatversion_present, 1));
  HANDLE_CODE(bref.pack(sender_name_present, 1));
  HANDLE_CODE(bref.pack(sender_type_present, 1));
  HANDLE_CODE(bref.pack(vendor_name_present, 1));

  HANDLE_CODE(collet_start_time.pack(bref));
  if (file_formatversion_present) {
    HANDLE_CODE(file_formatversion.pack(bref));
  }
  if (sender_name_present) {
    HANDLE_CODE(sender_name.pack(bref));
  }
  if (sender_type_present) {
    HANDLE_CODE(sender_type.pack(bref));
  }
  if (vendor_name_present) {
    HANDLE_CODE(vendor_name.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_ind_hdr_format1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(file_formatversion_present, 1));
  HANDLE_CODE(bref.unpack(sender_name_present, 1));
  HANDLE_CODE(bref.unpack(sender_type_present, 1));
  HANDLE_CODE(bref.unpack(vendor_name_present, 1));

  HANDLE_CODE(collet_start_time.unpack(bref));
  if (file_formatversion_present) {
    HANDLE_CODE(file_formatversion.unpack(bref));
  }
  if (sender_name_present) {
    HANDLE_CODE(sender_name.unpack(bref));
  }
  if (sender_type_present) {
    HANDLE_CODE(sender_type.unpack(bref));
  }
  if (vendor_name_present) {
    HANDLE_CODE(vendor_name.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void e2_sm_kpm_ind_hdr_format1_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("colletStartTime", collet_start_time.to_string());
  if (file_formatversion_present) {
    j.write_str("fileFormatversion", file_formatversion.to_string());
  }
  if (sender_name_present) {
    j.write_str("senderName", sender_name.to_string());
  }
  if (sender_type_present) {
    j.write_str("senderType", sender_type.to_string());
  }
  if (vendor_name_present) {
    j.write_str("vendorName", vendor_name.to_string());
  }
  j.end_obj();
}

// E2SM-KPM-IndicationHeader ::= SEQUENCE
SRSASN_CODE e2_sm_kpm_ind_hdr_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ind_hdr_formats.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_ind_hdr_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ind_hdr_formats.unpack(bref));

  return SRSASN_SUCCESS;
}
void e2_sm_kpm_ind_hdr_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("indicationHeader-formats");
  ind_hdr_formats.to_json(j);
  j.end_obj();
}

void e2_sm_kpm_ind_hdr_s::ind_hdr_formats_c_::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("indicationHeader-Format1");
  c.to_json(j);
  j.end_obj();
}
SRSASN_CODE e2_sm_kpm_ind_hdr_s::ind_hdr_formats_c_::pack(bit_ref& bref) const
{
  pack_enum(bref, type());
  HANDLE_CODE(c.pack(bref));
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_ind_hdr_s::ind_hdr_formats_c_::unpack(cbit_ref& bref)
{
  types e;
  unpack_enum(e, bref);
  if (e != type()) {
    log_invalid_choice_id(e, "e2_sm_kpm_ind_hdr_s::ind_hdr_formats_c_");
    return SRSASN_ERROR_DECODE_FAIL;
  }
  HANDLE_CODE(c.unpack(bref));
  return SRSASN_SUCCESS;
}

const char* e2_sm_kpm_ind_hdr_s::ind_hdr_formats_c_::types_opts::to_string() const
{
  static const char* options[] = {"indicationHeader-Format1"};
  return convert_enum_idx(options, 1, value, "e2_sm_kpm_ind_hdr_s::ind_hdr_formats_c_::types");
}
uint8_t e2_sm_kpm_ind_hdr_s::ind_hdr_formats_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1};
  return map_enum_number(options, 1, value, "e2_sm_kpm_ind_hdr_s::ind_hdr_formats_c_::types");
}

// MeasurementRecordItem ::= CHOICE
void meas_record_item_c::destroy_()
{
  switch (type_) {
    case types::real:
      c.destroy<real_s>();
      break;
    default:
      break;
  }
}
void meas_record_item_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::integer:
      break;
    case types::real:
      c.init<real_s>();
      break;
    case types::no_value:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_record_item_c");
  }
}
meas_record_item_c::meas_record_item_c(const meas_record_item_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::integer:
      c.init(other.c.get<uint64_t>());
      break;
    case types::real:
      c.init(other.c.get<real_s>());
      break;
    case types::no_value:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_record_item_c");
  }
}
meas_record_item_c& meas_record_item_c::operator=(const meas_record_item_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::integer:
      c.set(other.c.get<uint64_t>());
      break;
    case types::real:
      c.set(other.c.get<real_s>());
      break;
    case types::no_value:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_record_item_c");
  }

  return *this;
}
uint64_t& meas_record_item_c::set_integer()
{
  set(types::integer);
  return c.get<uint64_t>();
}
real_s& meas_record_item_c::set_real()
{
  set(types::real);
  return c.get<real_s>();
}
void meas_record_item_c::set_no_value()
{
  set(types::no_value);
}
void meas_record_item_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::integer:
      j.write_int("integer", c.get<uint64_t>());
      break;
    case types::real:
      j.write_fieldname("real");
      c.get<real_s>().to_json(j);
      break;
    case types::no_value:
      break;
    default:
      log_invalid_choice_id(type_, "meas_record_item_c");
  }
  j.end_obj();
}
SRSASN_CODE meas_record_item_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::integer:
      HANDLE_CODE(pack_integer(bref, c.get<uint64_t>(), (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::real:
      HANDLE_CODE(c.get<real_s>().pack(bref));
      break;
    case types::no_value:
      break;
    default:
      log_invalid_choice_id(type_, "meas_record_item_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_record_item_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::integer:
      HANDLE_CODE(unpack_integer(c.get<uint64_t>(), bref, (uint64_t)0u, (uint64_t)4294967295u, false, true));
      break;
    case types::real:
      HANDLE_CODE(c.get<real_s>().unpack(bref));
      break;
    case types::no_value:
      break;
    default:
      log_invalid_choice_id(type_, "meas_record_item_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* meas_record_item_c::types_opts::to_string() const
{
  static const char* options[] = {"integer", "real", "noValue"};
  return convert_enum_idx(options, 3, value, "meas_record_item_c::types");
}

// MatchingUEidItem-PerGP ::= SEQUENCE
SRSASN_CODE matching_ueid_item_per_gp_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ue_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE matching_ueid_item_per_gp_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ue_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void matching_ueid_item_per_gp_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ueID");
  ue_id.to_json(j);
  j.end_obj();
}

// MeasurementDataItem ::= SEQUENCE
SRSASN_CODE meas_data_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(incomplete_flag_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, meas_record, 1, 2147483647, true));
  if (incomplete_flag_present) {
    HANDLE_CODE(incomplete_flag.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_data_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(incomplete_flag_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(meas_record, bref, 1, 2147483647, true));
  if (incomplete_flag_present) {
    HANDLE_CODE(incomplete_flag.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void meas_data_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("measRecord");
  for (const auto& e1 : meas_record) {
    e1.to_json(j);
  }
  j.end_array();
  if (incomplete_flag_present) {
    j.write_str("incompleteFlag", "true");
  }
  j.end_obj();
}

const char* meas_data_item_s::incomplete_flag_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "meas_data_item_s::incomplete_flag_e_");
}

// MatchingUEidItem ::= SEQUENCE
SRSASN_CODE matching_ueid_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ue_id.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE matching_ueid_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ue_id.unpack(bref));

  return SRSASN_SUCCESS;
}
void matching_ueid_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ueID");
  ue_id.to_json(j);
  j.end_obj();
}

// MatchingUEidPerGP-Item ::= SEQUENCE
SRSASN_CODE matching_ueid_per_gp_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(matched_per_gp.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE matching_ueid_per_gp_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(matched_per_gp.unpack(bref));

  return SRSASN_SUCCESS;
}
void matching_ueid_per_gp_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("matchedPerGP");
  matched_per_gp.to_json(j);
  j.end_obj();
}

void matching_ueid_per_gp_item_s::matched_per_gp_c_::destroy_()
{
  switch (type_) {
    case types::one_or_more_uematched:
      c.destroy<matching_ueid_list_per_gp_l>();
      break;
    default:
      break;
  }
}
void matching_ueid_per_gp_item_s::matched_per_gp_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::no_uematched:
      break;
    case types::one_or_more_uematched:
      c.init<matching_ueid_list_per_gp_l>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "matching_ueid_per_gp_item_s::matched_per_gp_c_");
  }
}
matching_ueid_per_gp_item_s::matched_per_gp_c_::matched_per_gp_c_(
    const matching_ueid_per_gp_item_s::matched_per_gp_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::no_uematched:
      c.init(other.c.get<no_uematched_e_>());
      break;
    case types::one_or_more_uematched:
      c.init(other.c.get<matching_ueid_list_per_gp_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "matching_ueid_per_gp_item_s::matched_per_gp_c_");
  }
}
matching_ueid_per_gp_item_s::matched_per_gp_c_&
matching_ueid_per_gp_item_s::matched_per_gp_c_::operator=(const matching_ueid_per_gp_item_s::matched_per_gp_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::no_uematched:
      c.set(other.c.get<no_uematched_e_>());
      break;
    case types::one_or_more_uematched:
      c.set(other.c.get<matching_ueid_list_per_gp_l>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "matching_ueid_per_gp_item_s::matched_per_gp_c_");
  }

  return *this;
}
matching_ueid_per_gp_item_s::matched_per_gp_c_::no_uematched_e_&
matching_ueid_per_gp_item_s::matched_per_gp_c_::set_no_uematched()
{
  set(types::no_uematched);
  return c.get<no_uematched_e_>();
}
matching_ueid_list_per_gp_l& matching_ueid_per_gp_item_s::matched_per_gp_c_::set_one_or_more_uematched()
{
  set(types::one_or_more_uematched);
  return c.get<matching_ueid_list_per_gp_l>();
}
void matching_ueid_per_gp_item_s::matched_per_gp_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::no_uematched:
      j.write_str("noUEmatched", "true");
      break;
    case types::one_or_more_uematched:
      j.start_array("oneOrMoreUEmatched");
      for (const auto& e1 : c.get<matching_ueid_list_per_gp_l>()) {
        e1.to_json(j);
      }
      j.end_array();
      break;
    default:
      log_invalid_choice_id(type_, "matching_ueid_per_gp_item_s::matched_per_gp_c_");
  }
  j.end_obj();
}
SRSASN_CODE matching_ueid_per_gp_item_s::matched_per_gp_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::no_uematched:
      HANDLE_CODE(c.get<no_uematched_e_>().pack(bref));
      break;
    case types::one_or_more_uematched:
      HANDLE_CODE(pack_dyn_seq_of(bref, c.get<matching_ueid_list_per_gp_l>(), 1, 65535, true));
      break;
    default:
      log_invalid_choice_id(type_, "matching_ueid_per_gp_item_s::matched_per_gp_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE matching_ueid_per_gp_item_s::matched_per_gp_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::no_uematched:
      HANDLE_CODE(c.get<no_uematched_e_>().unpack(bref));
      break;
    case types::one_or_more_uematched:
      HANDLE_CODE(unpack_dyn_seq_of(c.get<matching_ueid_list_per_gp_l>(), bref, 1, 65535, true));
      break;
    default:
      log_invalid_choice_id(type_, "matching_ueid_per_gp_item_s::matched_per_gp_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* matching_ueid_per_gp_item_s::matched_per_gp_c_::no_uematched_opts::to_string() const
{
  static const char* options[] = {"true"};
  return convert_enum_idx(options, 1, value, "matching_ueid_per_gp_item_s::matched_per_gp_c_::no_uematched_e_");
}

const char* matching_ueid_per_gp_item_s::matched_per_gp_c_::types_opts::to_string() const
{
  static const char* options[] = {"noUEmatched", "oneOrMoreUEmatched"};
  return convert_enum_idx(options, 2, value, "matching_ueid_per_gp_item_s::matched_per_gp_c_::types");
}
uint8_t matching_ueid_per_gp_item_s::matched_per_gp_c_::types_opts::to_number() const
{
  if (value == one_or_more_uematched) {
    return 1;
  }
  invalid_enum_number(value, "matching_ueid_per_gp_item_s::matched_per_gp_c_::types");
  return 0;
}

// E2SM-KPM-IndicationMessage-Format1 ::= SEQUENCE
SRSASN_CODE e2_sm_kpm_ind_msg_format1_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_info_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(granul_period_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, meas_data, 1, 65535, true));
  if (meas_info_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, meas_info_list, 1, 65535, true));
  }
  if (granul_period_present) {
    HANDLE_CODE(pack_integer(bref, granul_period, (uint64_t)1u, (uint64_t)4294967295u, false, true));
  }

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_ind_msg_format1_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool meas_info_list_present;
  HANDLE_CODE(bref.unpack(meas_info_list_present, 1));
  HANDLE_CODE(bref.unpack(granul_period_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(meas_data, bref, 1, 65535, true));
  if (meas_info_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(meas_info_list, bref, 1, 65535, true));
  }
  if (granul_period_present) {
    HANDLE_CODE(unpack_integer(granul_period, bref, (uint64_t)1u, (uint64_t)4294967295u, false, true));
  }

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void e2_sm_kpm_ind_msg_format1_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("E2SM-KPM-IndicationMessage-Format1");
  j.start_array("measData");
  for (const auto& e1 : meas_data) {
    e1.to_json(j);
  }
  j.end_array();
  if (meas_info_list.size() > 0) {
    j.start_array("measInfoList");
    for (const auto& e1 : meas_info_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (granul_period_present) {
    j.write_int("granulPeriod", granul_period);
  }
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// MeasurementCondUEidItem ::= SEQUENCE
SRSASN_CODE meas_cond_ueid_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(matching_ueid_list.size() > 0, 1));

  HANDLE_CODE(meas_type.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, matching_cond, 1, 32768, true));
  if (matching_ueid_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, matching_ueid_list, 1, 65535, true));
  }

  if (ext) {
    HANDLE_CODE(bref.pack(matching_ueid_per_gp.is_present(), 1));

    if (matching_ueid_per_gp.is_present()) {
      HANDLE_CODE(pack_dyn_seq_of(bref, *matching_ueid_per_gp, 1, 65535, true));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_cond_ueid_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool matching_ueid_list_present;
  HANDLE_CODE(bref.unpack(matching_ueid_list_present, 1));

  HANDLE_CODE(meas_type.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(matching_cond, bref, 1, 32768, true));
  if (matching_ueid_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(matching_ueid_list, bref, 1, 65535, true));
  }

  if (ext) {
    bool matching_ueid_per_gp_present;
    HANDLE_CODE(bref.unpack(matching_ueid_per_gp_present, 1));
    matching_ueid_per_gp.set_present(matching_ueid_per_gp_present);

    if (matching_ueid_per_gp.is_present()) {
      HANDLE_CODE(unpack_dyn_seq_of(*matching_ueid_per_gp, bref, 1, 65535, true));
    }
  }
  return SRSASN_SUCCESS;
}
void meas_cond_ueid_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("measType");
  meas_type.to_json(j);
  j.start_array("matchingCond");
  for (const auto& e1 : matching_cond) {
    e1.to_json(j);
  }
  j.end_array();
  if (matching_ueid_list.size() > 0) {
    j.start_array("matchingUEidList");
    for (const auto& e1 : matching_ueid_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ext) {
    if (matching_ueid_per_gp.is_present()) {
      j.start_array("matchingUEidPerGP");
      for (const auto& e1 : *matching_ueid_per_gp) {
        e1.to_json(j);
      }
      j.end_array();
    }
  }
  j.end_obj();
}

// UEMeasurementReportItem ::= SEQUENCE
SRSASN_CODE ue_meas_report_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ue_id.pack(bref));
  HANDLE_CODE(meas_report.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ue_meas_report_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ue_id.unpack(bref));
  HANDLE_CODE(meas_report.unpack(bref));

  return SRSASN_SUCCESS;
}
void ue_meas_report_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ueID");
  ue_id.to_json(j);
  j.write_fieldname("measReport");
  meas_report.to_json(j);
  j.end_obj();
}

// E2SM-KPM-IndicationMessage-Format2 ::= SEQUENCE
SRSASN_CODE e2_sm_kpm_ind_msg_format2_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(granul_period_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, meas_data, 1, 65535, true));
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_cond_ueid_list, 1, 65535, true));
  if (granul_period_present) {
    HANDLE_CODE(pack_integer(bref, granul_period, (uint64_t)1u, (uint64_t)4294967295u, false, true));
  }

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_ind_msg_format2_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(granul_period_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(meas_data, bref, 1, 65535, true));
  HANDLE_CODE(unpack_dyn_seq_of(meas_cond_ueid_list, bref, 1, 65535, true));
  if (granul_period_present) {
    HANDLE_CODE(unpack_integer(granul_period, bref, (uint64_t)1u, (uint64_t)4294967295u, false, true));
  }

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void e2_sm_kpm_ind_msg_format2_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("E2SM-KPM-IndicationMessage-Format2");
  j.start_array("measData");
  for (const auto& e1 : meas_data) {
    e1.to_json(j);
  }
  j.end_array();
  j.start_array("measCondUEidList");
  for (const auto& e1 : meas_cond_ueid_list) {
    e1.to_json(j);
  }
  j.end_array();
  if (granul_period_present) {
    j.write_int("granulPeriod", granul_period);
  }
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// E2SM-KPM-IndicationMessage-Format3 ::= SEQUENCE
SRSASN_CODE e2_sm_kpm_ind_msg_format3_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_dyn_seq_of(bref, ue_meas_report_list, 1, 65535, true));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_ind_msg_format3_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_dyn_seq_of(ue_meas_report_list, bref, 1, 65535, true));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void e2_sm_kpm_ind_msg_format3_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("E2SM-KPM-IndicationMessage-Format3");
  j.start_array("ueMeasReportList");
  for (const auto& e1 : ue_meas_report_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
  j.end_obj();
  j.end_array();
}

// E2SM-KPM-IndicationMessage ::= SEQUENCE
SRSASN_CODE e2_sm_kpm_ind_msg_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(ind_msg_formats.pack(bref));

  bref.align_bytes_zero();

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_ind_msg_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(ind_msg_formats.unpack(bref));

  bref.align_bytes();

  return SRSASN_SUCCESS;
}
void e2_sm_kpm_ind_msg_s::to_json(json_writer& j) const
{
  j.start_array();
  j.start_obj();
  j.start_obj("E2SM-KPM-IndicationMessage");
  j.write_fieldname("indicationMessage-formats");
  ind_msg_formats.to_json(j);
  j.end_obj();
  j.end_obj();
  j.end_array();
}

void e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::destroy_()
{
  switch (type_) {
    case types::ind_msg_format1:
      c.destroy<e2_sm_kpm_ind_msg_format1_s>();
      break;
    case types::ind_msg_format2:
      c.destroy<e2_sm_kpm_ind_msg_format2_s>();
      break;
    case types::ind_msg_format3:
      c.destroy<e2_sm_kpm_ind_msg_format3_s>();
      break;
    default:
      break;
  }
}
void e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::ind_msg_format1:
      c.init<e2_sm_kpm_ind_msg_format1_s>();
      break;
    case types::ind_msg_format2:
      c.init<e2_sm_kpm_ind_msg_format2_s>();
      break;
    case types::ind_msg_format3:
      c.init<e2_sm_kpm_ind_msg_format3_s>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_sm_kpm_ind_msg_s::ind_msg_formats_c_");
  }
}
e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::ind_msg_formats_c_(const e2_sm_kpm_ind_msg_s::ind_msg_formats_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::ind_msg_format1:
      c.init(other.c.get<e2_sm_kpm_ind_msg_format1_s>());
      break;
    case types::ind_msg_format2:
      c.init(other.c.get<e2_sm_kpm_ind_msg_format2_s>());
      break;
    case types::ind_msg_format3:
      c.init(other.c.get<e2_sm_kpm_ind_msg_format3_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_sm_kpm_ind_msg_s::ind_msg_formats_c_");
  }
}
e2_sm_kpm_ind_msg_s::ind_msg_formats_c_&
e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::operator=(const e2_sm_kpm_ind_msg_s::ind_msg_formats_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::ind_msg_format1:
      c.set(other.c.get<e2_sm_kpm_ind_msg_format1_s>());
      break;
    case types::ind_msg_format2:
      c.set(other.c.get<e2_sm_kpm_ind_msg_format2_s>());
      break;
    case types::ind_msg_format3:
      c.set(other.c.get<e2_sm_kpm_ind_msg_format3_s>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "e2_sm_kpm_ind_msg_s::ind_msg_formats_c_");
  }

  return *this;
}
e2_sm_kpm_ind_msg_format1_s& e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::set_ind_msg_format1()
{
  set(types::ind_msg_format1);
  return c.get<e2_sm_kpm_ind_msg_format1_s>();
}
e2_sm_kpm_ind_msg_format2_s& e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::set_ind_msg_format2()
{
  set(types::ind_msg_format2);
  return c.get<e2_sm_kpm_ind_msg_format2_s>();
}
e2_sm_kpm_ind_msg_format3_s& e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::set_ind_msg_format3()
{
  set(types::ind_msg_format3);
  return c.get<e2_sm_kpm_ind_msg_format3_s>();
}
void e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::ind_msg_format1:
      j.write_fieldname("indicationMessage-Format1");
      c.get<e2_sm_kpm_ind_msg_format1_s>().to_json(j);
      break;
    case types::ind_msg_format2:
      j.write_fieldname("indicationMessage-Format2");
      c.get<e2_sm_kpm_ind_msg_format2_s>().to_json(j);
      break;
    case types::ind_msg_format3:
      j.write_fieldname("indicationMessage-Format3");
      c.get<e2_sm_kpm_ind_msg_format3_s>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "e2_sm_kpm_ind_msg_s::ind_msg_formats_c_");
  }
  j.end_obj();
}
SRSASN_CODE e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::ind_msg_format1:
      HANDLE_CODE(c.get<e2_sm_kpm_ind_msg_format1_s>().pack(bref));
      break;
    case types::ind_msg_format2:
      HANDLE_CODE(c.get<e2_sm_kpm_ind_msg_format2_s>().pack(bref));
      break;
    case types::ind_msg_format3: {
      varlength_field_pack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<e2_sm_kpm_ind_msg_format3_s>().pack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "e2_sm_kpm_ind_msg_s::ind_msg_formats_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::ind_msg_format1:
      HANDLE_CODE(c.get<e2_sm_kpm_ind_msg_format1_s>().unpack(bref));
      break;
    case types::ind_msg_format2:
      HANDLE_CODE(c.get<e2_sm_kpm_ind_msg_format2_s>().unpack(bref));
      break;
    case types::ind_msg_format3: {
      varlength_field_unpack_guard varlen_scope(bref, true);
      HANDLE_CODE(c.get<e2_sm_kpm_ind_msg_format3_s>().unpack(bref));
    } break;
    default:
      log_invalid_choice_id(type_, "e2_sm_kpm_ind_msg_s::ind_msg_formats_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::to_string() const
{
  static const char* options[] = {
      "indicationMessage-Format1", "indicationMessage-Format2", "indicationMessage-Format3"};
  return convert_enum_idx(options, 3, value, "e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types");
}
uint8_t e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3};
  return map_enum_number(options, 3, value, "e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types");
}

// MeasurementInfo-Action-Item ::= SEQUENCE
SRSASN_CODE meas_info_action_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(meas_id_present, 1));

  HANDLE_CODE(meas_name.pack(bref));
  if (meas_id_present) {
    HANDLE_CODE(pack_integer(bref, meas_id, (uint32_t)1u, (uint32_t)65536u, true, true));
  }

  if (ext) {
    HANDLE_CODE(bref.pack(bin_range_def.is_present(), 1));

    if (bin_range_def.is_present()) {
      HANDLE_CODE(bin_range_def->pack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_info_action_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(meas_id_present, 1));

  HANDLE_CODE(meas_name.unpack(bref));
  if (meas_id_present) {
    HANDLE_CODE(unpack_integer(meas_id, bref, (uint32_t)1u, (uint32_t)65536u, true, true));
  }

  if (ext) {
    bool bin_range_def_present;
    HANDLE_CODE(bref.unpack(bin_range_def_present, 1));
    bin_range_def.set_present(bin_range_def_present);

    if (bin_range_def.is_present()) {
      HANDLE_CODE(bin_range_def->unpack(bref));
    }
  }
  return SRSASN_SUCCESS;
}
void meas_info_action_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("measName", meas_name.to_string());
  if (meas_id_present) {
    j.write_int("measID", meas_id);
  }
  if (ext) {
    if (bin_range_def.is_present()) {
      j.write_fieldname("binRangeDef");
      bin_range_def->to_json(j);
    }
  }
  j.end_obj();
}

// RANfunction-Name ::= SEQUENCE
SRSASN_CODE ra_nfunction_name_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ran_function_instance_present, 1));

  HANDLE_CODE(ran_function_short_name.pack(bref));
  HANDLE_CODE(ran_function_e2_sm_oid.pack(bref));
  HANDLE_CODE(ran_function_description.pack(bref));
  if (ran_function_instance_present) {
    HANDLE_CODE(pack_unconstrained_integer(bref, ran_function_instance, false, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ra_nfunction_name_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ran_function_instance_present, 1));

  HANDLE_CODE(ran_function_short_name.unpack(bref));
  HANDLE_CODE(ran_function_e2_sm_oid.unpack(bref));
  HANDLE_CODE(ran_function_description.unpack(bref));
  if (ran_function_instance_present) {
    HANDLE_CODE(unpack_unconstrained_integer(ran_function_instance, bref, false, true));
  }

  return SRSASN_SUCCESS;
}
void ra_nfunction_name_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("ranFunction-ShortName", ran_function_short_name.to_string());
  j.write_str("ranFunction-E2SM-OID", ran_function_e2_sm_oid.to_string());
  j.write_str("ranFunction-Description", ran_function_description.to_string());
  if (ran_function_instance_present) {
    j.write_int("ranFunction-Instance", ran_function_instance);
  }
  j.end_obj();
}

// RIC-EventTriggerStyle-Item ::= SEQUENCE
SRSASN_CODE ric_event_trigger_style_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_unconstrained_integer(bref, ric_event_trigger_style_type, false, true));
  HANDLE_CODE(ric_event_trigger_style_name.pack(bref));
  HANDLE_CODE(pack_unconstrained_integer(bref, ric_event_trigger_format_type, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_event_trigger_style_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_unconstrained_integer(ric_event_trigger_style_type, bref, false, true));
  HANDLE_CODE(ric_event_trigger_style_name.unpack(bref));
  HANDLE_CODE(unpack_unconstrained_integer(ric_event_trigger_format_type, bref, false, true));

  return SRSASN_SUCCESS;
}
void ric_event_trigger_style_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ric-EventTriggerStyle-Type", ric_event_trigger_style_type);
  j.write_str("ric-EventTriggerStyle-Name", ric_event_trigger_style_name.to_string());
  j.write_int("ric-EventTriggerFormat-Type", ric_event_trigger_format_type);
  j.end_obj();
}

// RIC-ReportStyle-Item ::= SEQUENCE
SRSASN_CODE ric_report_style_item_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_unconstrained_integer(bref, ric_report_style_type, false, true));
  HANDLE_CODE(ric_report_style_name.pack(bref));
  HANDLE_CODE(pack_unconstrained_integer(bref, ric_action_format_type, false, true));
  HANDLE_CODE(pack_dyn_seq_of(bref, meas_info_action_list, 1, 65535, true));
  HANDLE_CODE(pack_unconstrained_integer(bref, ric_ind_hdr_format_type, false, true));
  HANDLE_CODE(pack_unconstrained_integer(bref, ric_ind_msg_format_type, false, true));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ric_report_style_item_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_unconstrained_integer(ric_report_style_type, bref, false, true));
  HANDLE_CODE(ric_report_style_name.unpack(bref));
  HANDLE_CODE(unpack_unconstrained_integer(ric_action_format_type, bref, false, true));
  HANDLE_CODE(unpack_dyn_seq_of(meas_info_action_list, bref, 1, 65535, true));
  HANDLE_CODE(unpack_unconstrained_integer(ric_ind_hdr_format_type, bref, false, true));
  HANDLE_CODE(unpack_unconstrained_integer(ric_ind_msg_format_type, bref, false, true));

  return SRSASN_SUCCESS;
}
void ric_report_style_item_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("ric-ReportStyle-Type", ric_report_style_type);
  j.write_str("ric-ReportStyle-Name", ric_report_style_name.to_string());
  j.write_int("ric-ActionFormat-Type", ric_action_format_type);
  j.start_array("measInfo-Action-List");
  for (const auto& e1 : meas_info_action_list) {
    e1.to_json(j);
  }
  j.end_array();
  j.write_int("ric-IndicationHeaderFormat-Type", ric_ind_hdr_format_type);
  j.write_int("ric-IndicationMessageFormat-Type", ric_ind_msg_format_type);
  j.end_obj();
}

// E2SM-KPM-RANfunction-Description ::= SEQUENCE
SRSASN_CODE e2_sm_kpm_ra_nfunction_description_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ric_event_trigger_style_list.size() > 0, 1));
  HANDLE_CODE(bref.pack(ric_report_style_list.size() > 0, 1));

  HANDLE_CODE(ran_function_name.pack(bref));
  if (ric_event_trigger_style_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_event_trigger_style_list, 1, 63, true));
  }
  if (ric_report_style_list.size() > 0) {
    HANDLE_CODE(pack_dyn_seq_of(bref, ric_report_style_list, 1, 63, true));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE e2_sm_kpm_ra_nfunction_description_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  bool ric_event_trigger_style_list_present;
  HANDLE_CODE(bref.unpack(ric_event_trigger_style_list_present, 1));
  bool ric_report_style_list_present;
  HANDLE_CODE(bref.unpack(ric_report_style_list_present, 1));

  HANDLE_CODE(ran_function_name.unpack(bref));
  if (ric_event_trigger_style_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ric_event_trigger_style_list, bref, 1, 63, true));
  }
  if (ric_report_style_list_present) {
    HANDLE_CODE(unpack_dyn_seq_of(ric_report_style_list, bref, 1, 63, true));
  }

  return SRSASN_SUCCESS;
}
void e2_sm_kpm_ra_nfunction_description_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("ranFunction-Name");
  ran_function_name.to_json(j);
  if (ric_event_trigger_style_list.size() > 0) {
    j.start_array("ric-EventTriggerStyle-List");
    for (const auto& e1 : ric_event_trigger_style_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (ric_report_style_list.size() > 0) {
    j.start_array("ric-ReportStyle-List");
    for (const auto& e1 : ric_report_style_list) {
      e1.to_json(j);
    }
    j.end_array();
  }
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
