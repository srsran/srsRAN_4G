/**
 * Copyright 2013-2022 Software Radio Systems Limited
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

#include "srsran/asn1/rrc/rr_common.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// PHICH-Config ::= SEQUENCE
SRSASN_CODE phich_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(phich_dur.pack(bref));
  HANDLE_CODE(phich_res.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE phich_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(phich_dur.unpack(bref));
  HANDLE_CODE(phich_res.unpack(bref));

  return SRSASN_SUCCESS;
}
void phich_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("phich-Duration", phich_dur.to_string());
  j.write_str("phich-Resource", phich_res.to_string());
  j.end_obj();
}
bool phich_cfg_s::operator==(const phich_cfg_s& other) const
{
  return phich_dur == other.phich_dur and phich_res == other.phich_res;
}

const char* phich_cfg_s::phich_dur_opts::to_string() const
{
  static const char* options[] = {"normal", "extended"};
  return convert_enum_idx(options, 2, value, "phich_cfg_s::phich_dur_e_");
}

const char* phich_cfg_s::phich_res_opts::to_string() const
{
  static const char* options[] = {"oneSixth", "half", "one", "two"};
  return convert_enum_idx(options, 4, value, "phich_cfg_s::phich_res_e_");
}
float phich_cfg_s::phich_res_opts::to_number() const
{
  static const float options[] = {0.16666666666666666, 0.5, 1.0, 2.0};
  return map_enum_number(options, 4, value, "phich_cfg_s::phich_res_e_");
}
const char* phich_cfg_s::phich_res_opts::to_number_string() const
{
  static const char* options[] = {"1/6", "0.5", "1", "2"};
  return convert_enum_idx(options, 4, value, "phich_cfg_s::phich_res_e_");
}

// PLMN-IdentityInfo-r15 ::= SEQUENCE
SRSASN_CODE plmn_id_info_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(plmn_id_minus5_gc_r15.pack(bref));
  HANDLE_CODE(cell_reserved_for_oper_r15.pack(bref));
  HANDLE_CODE(cell_reserved_for_oper_crs_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_id_info_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(plmn_id_minus5_gc_r15.unpack(bref));
  HANDLE_CODE(cell_reserved_for_oper_r15.unpack(bref));
  HANDLE_CODE(cell_reserved_for_oper_crs_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void plmn_id_info_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("plmn-Identity-5GC-r15");
  plmn_id_minus5_gc_r15.to_json(j);
  j.write_str("cellReservedForOperatorUse-r15", cell_reserved_for_oper_r15.to_string());
  j.write_str("cellReservedForOperatorUse-CRS-r15", cell_reserved_for_oper_crs_r15.to_string());
  j.end_obj();
}

void plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_::destroy_()
{
  switch (type_) {
    case types::plmn_id_r15:
      c.destroy<plmn_id_s>();
      break;
    default:
      break;
  }
}
void plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::plmn_id_r15:
      c.init<plmn_id_s>();
      break;
    case types::plmn_idx_r15:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_");
  }
}
plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_::plmn_id_minus5_gc_r15_c_(
    const plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::plmn_id_r15:
      c.init(other.c.get<plmn_id_s>());
      break;
    case types::plmn_idx_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_");
  }
}
plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_&
plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_::operator=(const plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::plmn_id_r15:
      c.set(other.c.get<plmn_id_s>());
      break;
    case types::plmn_idx_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_");
  }

  return *this;
}
plmn_id_s& plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_::set_plmn_id_r15()
{
  set(types::plmn_id_r15);
  return c.get<plmn_id_s>();
}
uint8_t& plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_::set_plmn_idx_r15()
{
  set(types::plmn_idx_r15);
  return c.get<uint8_t>();
}
void plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::plmn_id_r15:
      j.write_fieldname("plmn-Identity-r15");
      c.get<plmn_id_s>().to_json(j);
      break;
    case types::plmn_idx_r15:
      j.write_int("plmn-Index-r15", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_");
  }
  j.end_obj();
}
SRSASN_CODE plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::plmn_id_r15:
      HANDLE_CODE(c.get<plmn_id_s>().pack(bref));
      break;
    case types::plmn_idx_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)6u));
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::plmn_id_r15:
      HANDLE_CODE(c.get<plmn_id_s>().unpack(bref));
      break;
    case types::plmn_idx_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)6u));
      break;
    default:
      log_invalid_choice_id(type_, "plmn_id_info_r15_s::plmn_id_minus5_gc_r15_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* plmn_id_info_r15_s::cell_reserved_for_oper_r15_opts::to_string() const
{
  static const char* options[] = {"reserved", "notReserved"};
  return convert_enum_idx(options, 2, value, "plmn_id_info_r15_s::cell_reserved_for_oper_r15_e_");
}

const char* plmn_id_info_r15_s::cell_reserved_for_oper_crs_r15_opts::to_string() const
{
  static const char* options[] = {"reserved", "notReserved"};
  return convert_enum_idx(options, 2, value, "plmn_id_info_r15_s::cell_reserved_for_oper_crs_r15_e_");
}

// CellIdentity-5GC-r15 ::= CHOICE
void cell_id_minus5_gc_r15_c::destroy_()
{
  switch (type_) {
    case types::cell_id_r15:
      c.destroy<fixed_bitstring<28> >();
      break;
    default:
      break;
  }
}
void cell_id_minus5_gc_r15_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::cell_id_r15:
      c.init<fixed_bitstring<28> >();
      break;
    case types::cell_id_idx_r15:
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_minus5_gc_r15_c");
  }
}
cell_id_minus5_gc_r15_c::cell_id_minus5_gc_r15_c(const cell_id_minus5_gc_r15_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::cell_id_r15:
      c.init(other.c.get<fixed_bitstring<28> >());
      break;
    case types::cell_id_idx_r15:
      c.init(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_minus5_gc_r15_c");
  }
}
cell_id_minus5_gc_r15_c& cell_id_minus5_gc_r15_c::operator=(const cell_id_minus5_gc_r15_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::cell_id_r15:
      c.set(other.c.get<fixed_bitstring<28> >());
      break;
    case types::cell_id_idx_r15:
      c.set(other.c.get<uint8_t>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_minus5_gc_r15_c");
  }

  return *this;
}
fixed_bitstring<28>& cell_id_minus5_gc_r15_c::set_cell_id_r15()
{
  set(types::cell_id_r15);
  return c.get<fixed_bitstring<28> >();
}
uint8_t& cell_id_minus5_gc_r15_c::set_cell_id_idx_r15()
{
  set(types::cell_id_idx_r15);
  return c.get<uint8_t>();
}
void cell_id_minus5_gc_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::cell_id_r15:
      j.write_str("cellIdentity-r15", c.get<fixed_bitstring<28> >().to_string());
      break;
    case types::cell_id_idx_r15:
      j.write_int("cellId-Index-r15", c.get<uint8_t>());
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_minus5_gc_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE cell_id_minus5_gc_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::cell_id_r15:
      HANDLE_CODE(c.get<fixed_bitstring<28> >().pack(bref));
      break;
    case types::cell_id_idx_r15:
      HANDLE_CODE(pack_integer(bref, c.get<uint8_t>(), (uint8_t)1u, (uint8_t)6u));
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_minus5_gc_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_id_minus5_gc_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::cell_id_r15:
      HANDLE_CODE(c.get<fixed_bitstring<28> >().unpack(bref));
      break;
    case types::cell_id_idx_r15:
      HANDLE_CODE(unpack_integer(c.get<uint8_t>(), bref, (uint8_t)1u, (uint8_t)6u));
      break;
    default:
      log_invalid_choice_id(type_, "cell_id_minus5_gc_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

// CellAccessRelatedInfo-5GC-r15 ::= SEQUENCE
SRSASN_CODE cell_access_related_info_minus5_gc_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(ran_area_code_r15_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, plmn_id_list_r15, 1, 6));
  if (ran_area_code_r15_present) {
    HANDLE_CODE(pack_integer(bref, ran_area_code_r15, (uint16_t)0u, (uint16_t)255u));
  }
  HANDLE_CODE(tac_minus5_gc_r15.pack(bref));
  HANDLE_CODE(cell_id_minus5_gc_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE cell_access_related_info_minus5_gc_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(ran_area_code_r15_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(plmn_id_list_r15, bref, 1, 6));
  if (ran_area_code_r15_present) {
    HANDLE_CODE(unpack_integer(ran_area_code_r15, bref, (uint16_t)0u, (uint16_t)255u));
  }
  HANDLE_CODE(tac_minus5_gc_r15.unpack(bref));
  HANDLE_CODE(cell_id_minus5_gc_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void cell_access_related_info_minus5_gc_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("plmn-IdentityList-r15");
  for (const auto& e1 : plmn_id_list_r15) {
    e1.to_json(j);
  }
  j.end_array();
  if (ran_area_code_r15_present) {
    j.write_int("ran-AreaCode-r15", ran_area_code_r15);
  }
  j.write_str("trackingAreaCode-5GC-r15", tac_minus5_gc_r15.to_string());
  j.write_fieldname("cellIdentity-5GC-r15");
  cell_id_minus5_gc_r15.to_json(j);
  j.end_obj();
}

// TDD-Config-v1450 ::= SEQUENCE
SRSASN_CODE tdd_cfg_v1450_s::pack(bit_ref& bref) const
{
  return SRSASN_SUCCESS;
}
SRSASN_CODE tdd_cfg_v1450_s::unpack(cbit_ref& bref)
{
  return SRSASN_SUCCESS;
}
void tdd_cfg_v1450_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("specialSubframePatterns-v1450", "ssp10-CRS-LessDwPTS");
  j.end_obj();
}

// TDD-Config ::= SEQUENCE
SRSASN_CODE tdd_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sf_assign.pack(bref));
  HANDLE_CODE(special_sf_patterns.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE tdd_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sf_assign.unpack(bref));
  HANDLE_CODE(special_sf_patterns.unpack(bref));

  return SRSASN_SUCCESS;
}
void tdd_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("subframeAssignment", sf_assign.to_string());
  j.write_str("specialSubframePatterns", special_sf_patterns.to_string());
  j.end_obj();
}
bool tdd_cfg_s::operator==(const tdd_cfg_s& other) const
{
  return sf_assign == other.sf_assign and special_sf_patterns == other.special_sf_patterns;
}

const char* tdd_cfg_s::sf_assign_opts::to_string() const
{
  static const char* options[] = {"sa0", "sa1", "sa2", "sa3", "sa4", "sa5", "sa6"};
  return convert_enum_idx(options, 7, value, "tdd_cfg_s::sf_assign_e_");
}
uint8_t tdd_cfg_s::sf_assign_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6};
  return map_enum_number(options, 7, value, "tdd_cfg_s::sf_assign_e_");
}

const char* tdd_cfg_s::special_sf_patterns_opts::to_string() const
{
  static const char* options[] = {"ssp0", "ssp1", "ssp2", "ssp3", "ssp4", "ssp5", "ssp6", "ssp7", "ssp8"};
  return convert_enum_idx(options, 9, value, "tdd_cfg_s::special_sf_patterns_e_");
}
uint8_t tdd_cfg_s::special_sf_patterns_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  return map_enum_number(options, 9, value, "tdd_cfg_s::special_sf_patterns_e_");
}

// TDD-Config-v1430 ::= SEQUENCE
SRSASN_CODE tdd_cfg_v1430_s::pack(bit_ref& bref) const
{
  return SRSASN_SUCCESS;
}
SRSASN_CODE tdd_cfg_v1430_s::unpack(cbit_ref& bref)
{
  return SRSASN_SUCCESS;
}
void tdd_cfg_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("specialSubframePatterns-v1430", "ssp10");
  j.end_obj();
}

// TDD-Config-v1130 ::= SEQUENCE
SRSASN_CODE tdd_cfg_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(special_sf_patterns_v1130.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE tdd_cfg_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(special_sf_patterns_v1130.unpack(bref));

  return SRSASN_SUCCESS;
}
void tdd_cfg_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("specialSubframePatterns-v1130", special_sf_patterns_v1130.to_string());
  j.end_obj();
}
bool tdd_cfg_v1130_s::operator==(const tdd_cfg_v1130_s& other) const
{
  return special_sf_patterns_v1130 == other.special_sf_patterns_v1130;
}

const char* tdd_cfg_v1130_s::special_sf_patterns_v1130_opts::to_string() const
{
  static const char* options[] = {"ssp7", "ssp9"};
  return convert_enum_idx(options, 2, value, "tdd_cfg_v1130_s::special_sf_patterns_v1130_e_");
}
uint8_t tdd_cfg_v1130_s::special_sf_patterns_v1130_opts::to_number() const
{
  static const uint8_t options[] = {7, 9};
  return map_enum_number(options, 2, value, "tdd_cfg_v1130_s::special_sf_patterns_v1130_e_");
}

// PRACH-ParametersCE-r13 ::= SEQUENCE
SRSASN_CODE prach_params_ce_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(prach_start_sf_r13_present, 1));
  HANDLE_CODE(bref.pack(max_num_preamb_attempt_ce_r13_present, 1));

  HANDLE_CODE(pack_integer(bref, prach_cfg_idx_r13, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(pack_integer(bref, prach_freq_offset_r13, (uint8_t)0u, (uint8_t)94u));
  if (prach_start_sf_r13_present) {
    HANDLE_CODE(prach_start_sf_r13.pack(bref));
  }
  if (max_num_preamb_attempt_ce_r13_present) {
    HANDLE_CODE(max_num_preamb_attempt_ce_r13.pack(bref));
  }
  HANDLE_CODE(num_repeat_per_preamb_attempt_r13.pack(bref));
  HANDLE_CODE(pack_dyn_seq_of(bref, mpdcch_nbs_to_monitor_r13, 1, 2, integer_packer<uint8_t>(1, 16)));
  HANDLE_CODE(mpdcch_num_repeat_ra_r13.pack(bref));
  HANDLE_CODE(prach_hop_cfg_r13.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE prach_params_ce_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(prach_start_sf_r13_present, 1));
  HANDLE_CODE(bref.unpack(max_num_preamb_attempt_ce_r13_present, 1));

  HANDLE_CODE(unpack_integer(prach_cfg_idx_r13, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(prach_freq_offset_r13, bref, (uint8_t)0u, (uint8_t)94u));
  if (prach_start_sf_r13_present) {
    HANDLE_CODE(prach_start_sf_r13.unpack(bref));
  }
  if (max_num_preamb_attempt_ce_r13_present) {
    HANDLE_CODE(max_num_preamb_attempt_ce_r13.unpack(bref));
  }
  HANDLE_CODE(num_repeat_per_preamb_attempt_r13.unpack(bref));
  HANDLE_CODE(unpack_dyn_seq_of(mpdcch_nbs_to_monitor_r13, bref, 1, 2, integer_packer<uint8_t>(1, 16)));
  HANDLE_CODE(mpdcch_num_repeat_ra_r13.unpack(bref));
  HANDLE_CODE(prach_hop_cfg_r13.unpack(bref));

  return SRSASN_SUCCESS;
}
void prach_params_ce_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("prach-ConfigIndex-r13", prach_cfg_idx_r13);
  j.write_int("prach-FreqOffset-r13", prach_freq_offset_r13);
  if (prach_start_sf_r13_present) {
    j.write_str("prach-StartingSubframe-r13", prach_start_sf_r13.to_string());
  }
  if (max_num_preamb_attempt_ce_r13_present) {
    j.write_str("maxNumPreambleAttemptCE-r13", max_num_preamb_attempt_ce_r13.to_string());
  }
  j.write_str("numRepetitionPerPreambleAttempt-r13", num_repeat_per_preamb_attempt_r13.to_string());
  j.start_array("mpdcch-NarrowbandsToMonitor-r13");
  for (const auto& e1 : mpdcch_nbs_to_monitor_r13) {
    j.write_int(e1);
  }
  j.end_array();
  j.write_str("mpdcch-NumRepetition-RA-r13", mpdcch_num_repeat_ra_r13.to_string());
  j.write_str("prach-HoppingConfig-r13", prach_hop_cfg_r13.to_string());
  j.end_obj();
}

const char* prach_params_ce_r13_s::prach_start_sf_r13_opts::to_string() const
{
  static const char* options[] = {"sf2", "sf4", "sf8", "sf16", "sf32", "sf64", "sf128", "sf256"};
  return convert_enum_idx(options, 8, value, "prach_params_ce_r13_s::prach_start_sf_r13_e_");
}
uint16_t prach_params_ce_r13_s::prach_start_sf_r13_opts::to_number() const
{
  static const uint16_t options[] = {2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(options, 8, value, "prach_params_ce_r13_s::prach_start_sf_r13_e_");
}

const char* prach_params_ce_r13_s::max_num_preamb_attempt_ce_r13_opts::to_string() const
{
  static const char* options[] = {"n3", "n4", "n5", "n6", "n7", "n8", "n10"};
  return convert_enum_idx(options, 7, value, "prach_params_ce_r13_s::max_num_preamb_attempt_ce_r13_e_");
}
uint8_t prach_params_ce_r13_s::max_num_preamb_attempt_ce_r13_opts::to_number() const
{
  static const uint8_t options[] = {3, 4, 5, 6, 7, 8, 10};
  return map_enum_number(options, 7, value, "prach_params_ce_r13_s::max_num_preamb_attempt_ce_r13_e_");
}

const char* prach_params_ce_r13_s::num_repeat_per_preamb_attempt_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "n8", "n16", "n32", "n64", "n128"};
  return convert_enum_idx(options, 8, value, "prach_params_ce_r13_s::num_repeat_per_preamb_attempt_r13_e_");
}
uint8_t prach_params_ce_r13_s::num_repeat_per_preamb_attempt_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8, 16, 32, 64, 128};
  return map_enum_number(options, 8, value, "prach_params_ce_r13_s::num_repeat_per_preamb_attempt_r13_e_");
}

const char* prach_params_ce_r13_s::mpdcch_num_repeat_ra_r13_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "r128", "r256"};
  return convert_enum_idx(options, 9, value, "prach_params_ce_r13_s::mpdcch_num_repeat_ra_r13_e_");
}
uint16_t prach_params_ce_r13_s::mpdcch_num_repeat_ra_r13_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(options, 9, value, "prach_params_ce_r13_s::mpdcch_num_repeat_ra_r13_e_");
}

const char* prach_params_ce_r13_s::prach_hop_cfg_r13_opts::to_string() const
{
  static const char* options[] = {"on", "off"};
  return convert_enum_idx(options, 2, value, "prach_params_ce_r13_s::prach_hop_cfg_r13_e_");
}

// RACH-CE-LevelInfo-r13 ::= SEQUENCE
SRSASN_CODE rach_ce_level_info_r13_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pack_integer(bref, preamb_map_info_r13.first_preamb_r13, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(pack_integer(bref, preamb_map_info_r13.last_preamb_r13, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(ra_resp_win_size_r13.pack(bref));
  HANDLE_CODE(mac_contention_resolution_timer_r13.pack(bref));
  HANDLE_CODE(rar_hop_cfg_r13.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= edt_params_r15.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(edt_params_r15.is_present(), 1));
      if (edt_params_r15.is_present()) {
        HANDLE_CODE(bref.pack(edt_params_r15->mac_contention_resolution_timer_r15_present, 1));
        HANDLE_CODE(pack_integer(bref, edt_params_r15->edt_last_preamb_r15, (uint8_t)0u, (uint8_t)63u));
        HANDLE_CODE(bref.pack(edt_params_r15->edt_small_tbs_enabled_r15, 1));
        HANDLE_CODE(edt_params_r15->edt_tbs_r15.pack(bref));
        if (edt_params_r15->mac_contention_resolution_timer_r15_present) {
          HANDLE_CODE(edt_params_r15->mac_contention_resolution_timer_r15.pack(bref));
        }
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rach_ce_level_info_r13_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(unpack_integer(preamb_map_info_r13.first_preamb_r13, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(preamb_map_info_r13.last_preamb_r13, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(ra_resp_win_size_r13.unpack(bref));
  HANDLE_CODE(mac_contention_resolution_timer_r13.unpack(bref));
  HANDLE_CODE(rar_hop_cfg_r13.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(1);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool edt_params_r15_present;
      HANDLE_CODE(bref.unpack(edt_params_r15_present, 1));
      edt_params_r15.set_present(edt_params_r15_present);
      if (edt_params_r15.is_present()) {
        HANDLE_CODE(bref.unpack(edt_params_r15->mac_contention_resolution_timer_r15_present, 1));
        HANDLE_CODE(unpack_integer(edt_params_r15->edt_last_preamb_r15, bref, (uint8_t)0u, (uint8_t)63u));
        HANDLE_CODE(bref.unpack(edt_params_r15->edt_small_tbs_enabled_r15, 1));
        HANDLE_CODE(edt_params_r15->edt_tbs_r15.unpack(bref));
        if (edt_params_r15->mac_contention_resolution_timer_r15_present) {
          HANDLE_CODE(edt_params_r15->mac_contention_resolution_timer_r15.unpack(bref));
        }
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rach_ce_level_info_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("preambleMappingInfo-r13");
  j.start_obj();
  j.write_int("firstPreamble-r13", preamb_map_info_r13.first_preamb_r13);
  j.write_int("lastPreamble-r13", preamb_map_info_r13.last_preamb_r13);
  j.end_obj();
  j.write_str("ra-ResponseWindowSize-r13", ra_resp_win_size_r13.to_string());
  j.write_str("mac-ContentionResolutionTimer-r13", mac_contention_resolution_timer_r13.to_string());
  j.write_str("rar-HoppingConfig-r13", rar_hop_cfg_r13.to_string());
  if (ext) {
    if (edt_params_r15.is_present()) {
      j.write_fieldname("edt-Parameters-r15");
      j.start_obj();
      j.write_int("edt-LastPreamble-r15", edt_params_r15->edt_last_preamb_r15);
      j.write_bool("edt-SmallTBS-Enabled-r15", edt_params_r15->edt_small_tbs_enabled_r15);
      j.write_str("edt-TBS-r15", edt_params_r15->edt_tbs_r15.to_string());
      if (edt_params_r15->mac_contention_resolution_timer_r15_present) {
        j.write_str("mac-ContentionResolutionTimer-r15",
                    edt_params_r15->mac_contention_resolution_timer_r15.to_string());
      }
      j.end_obj();
    }
  }
  j.end_obj();
}

const char* rach_ce_level_info_r13_s::ra_resp_win_size_r13_opts::to_string() const
{
  static const char* options[] = {"sf20", "sf50", "sf80", "sf120", "sf180", "sf240", "sf320", "sf400"};
  return convert_enum_idx(options, 8, value, "rach_ce_level_info_r13_s::ra_resp_win_size_r13_e_");
}
uint16_t rach_ce_level_info_r13_s::ra_resp_win_size_r13_opts::to_number() const
{
  static const uint16_t options[] = {20, 50, 80, 120, 180, 240, 320, 400};
  return map_enum_number(options, 8, value, "rach_ce_level_info_r13_s::ra_resp_win_size_r13_e_");
}

const char* rach_ce_level_info_r13_s::mac_contention_resolution_timer_r13_opts::to_string() const
{
  static const char* options[] = {"sf80", "sf100", "sf120", "sf160", "sf200", "sf240", "sf480", "sf960"};
  return convert_enum_idx(options, 8, value, "rach_ce_level_info_r13_s::mac_contention_resolution_timer_r13_e_");
}
uint16_t rach_ce_level_info_r13_s::mac_contention_resolution_timer_r13_opts::to_number() const
{
  static const uint16_t options[] = {80, 100, 120, 160, 200, 240, 480, 960};
  return map_enum_number(options, 8, value, "rach_ce_level_info_r13_s::mac_contention_resolution_timer_r13_e_");
}

const char* rach_ce_level_info_r13_s::rar_hop_cfg_r13_opts::to_string() const
{
  static const char* options[] = {"on", "off"};
  return convert_enum_idx(options, 2, value, "rach_ce_level_info_r13_s::rar_hop_cfg_r13_e_");
}

const char* rach_ce_level_info_r13_s::edt_params_r15_s_::edt_tbs_r15_opts::to_string() const
{
  static const char* options[] = {"b328", "b408", "b504", "b600", "b712", "b808", "b936", "b1000or456"};
  return convert_enum_idx(options, 8, value, "rach_ce_level_info_r13_s::edt_params_r15_s_::edt_tbs_r15_e_");
}
uint16_t rach_ce_level_info_r13_s::edt_params_r15_s_::edt_tbs_r15_opts::to_number() const
{
  static const uint16_t options[] = {328, 408, 504, 600, 712, 808, 936, 1000};
  return map_enum_number(options, 8, value, "rach_ce_level_info_r13_s::edt_params_r15_s_::edt_tbs_r15_e_");
}

const char* rach_ce_level_info_r13_s::edt_params_r15_s_::mac_contention_resolution_timer_r15_opts::to_string() const
{
  static const char* options[] = {"sf240", "sf480", "sf960", "sf1920", "sf3840", "sf5760", "sf7680", "sf10240"};
  return convert_enum_idx(
      options, 8, value, "rach_ce_level_info_r13_s::edt_params_r15_s_::mac_contention_resolution_timer_r15_e_");
}
uint16_t rach_ce_level_info_r13_s::edt_params_r15_s_::mac_contention_resolution_timer_r15_opts::to_number() const
{
  static const uint16_t options[] = {240, 480, 960, 1920, 3840, 5760, 7680, 10240};
  return map_enum_number(
      options, 8, value, "rach_ce_level_info_r13_s::edt_params_r15_s_::mac_contention_resolution_timer_r15_e_");
}

// DeltaFList-PUCCH ::= SEQUENCE
SRSASN_CODE delta_flist_pucch_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(delta_f_pucch_format1.pack(bref));
  HANDLE_CODE(delta_f_pucch_format1b.pack(bref));
  HANDLE_CODE(delta_f_pucch_format2.pack(bref));
  HANDLE_CODE(delta_f_pucch_format2a.pack(bref));
  HANDLE_CODE(delta_f_pucch_format2b.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE delta_flist_pucch_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(delta_f_pucch_format1.unpack(bref));
  HANDLE_CODE(delta_f_pucch_format1b.unpack(bref));
  HANDLE_CODE(delta_f_pucch_format2.unpack(bref));
  HANDLE_CODE(delta_f_pucch_format2a.unpack(bref));
  HANDLE_CODE(delta_f_pucch_format2b.unpack(bref));

  return SRSASN_SUCCESS;
}
void delta_flist_pucch_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("deltaF-PUCCH-Format1", delta_f_pucch_format1.to_string());
  j.write_str("deltaF-PUCCH-Format1b", delta_f_pucch_format1b.to_string());
  j.write_str("deltaF-PUCCH-Format2", delta_f_pucch_format2.to_string());
  j.write_str("deltaF-PUCCH-Format2a", delta_f_pucch_format2a.to_string());
  j.write_str("deltaF-PUCCH-Format2b", delta_f_pucch_format2b.to_string());
  j.end_obj();
}
bool delta_flist_pucch_s::operator==(const delta_flist_pucch_s& other) const
{
  return delta_f_pucch_format1 == other.delta_f_pucch_format1 and
         delta_f_pucch_format1b == other.delta_f_pucch_format1b and
         delta_f_pucch_format2 == other.delta_f_pucch_format2 and
         delta_f_pucch_format2a == other.delta_f_pucch_format2a and
         delta_f_pucch_format2b == other.delta_f_pucch_format2b;
}

const char* delta_flist_pucch_s::delta_f_pucch_format1_opts::to_string() const
{
  static const char* options[] = {"deltaF-2", "deltaF0", "deltaF2"};
  return convert_enum_idx(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format1_e_");
}
int8_t delta_flist_pucch_s::delta_f_pucch_format1_opts::to_number() const
{
  static const int8_t options[] = {-2, 0, 2};
  return map_enum_number(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format1_e_");
}

const char* delta_flist_pucch_s::delta_f_pucch_format1b_opts::to_string() const
{
  static const char* options[] = {"deltaF1", "deltaF3", "deltaF5"};
  return convert_enum_idx(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format1b_e_");
}
uint8_t delta_flist_pucch_s::delta_f_pucch_format1b_opts::to_number() const
{
  static const uint8_t options[] = {1, 3, 5};
  return map_enum_number(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format1b_e_");
}

const char* delta_flist_pucch_s::delta_f_pucch_format2_opts::to_string() const
{
  static const char* options[] = {"deltaF-2", "deltaF0", "deltaF1", "deltaF2"};
  return convert_enum_idx(options, 4, value, "delta_flist_pucch_s::delta_f_pucch_format2_e_");
}
int8_t delta_flist_pucch_s::delta_f_pucch_format2_opts::to_number() const
{
  static const int8_t options[] = {-2, 0, 1, 2};
  return map_enum_number(options, 4, value, "delta_flist_pucch_s::delta_f_pucch_format2_e_");
}

const char* delta_flist_pucch_s::delta_f_pucch_format2a_opts::to_string() const
{
  static const char* options[] = {"deltaF-2", "deltaF0", "deltaF2"};
  return convert_enum_idx(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format2a_e_");
}
int8_t delta_flist_pucch_s::delta_f_pucch_format2a_opts::to_number() const
{
  static const int8_t options[] = {-2, 0, 2};
  return map_enum_number(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format2a_e_");
}

const char* delta_flist_pucch_s::delta_f_pucch_format2b_opts::to_string() const
{
  static const char* options[] = {"deltaF-2", "deltaF0", "deltaF2"};
  return convert_enum_idx(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format2b_e_");
}
int8_t delta_flist_pucch_s::delta_f_pucch_format2b_opts::to_number() const
{
  static const int8_t options[] = {-2, 0, 2};
  return map_enum_number(options, 3, value, "delta_flist_pucch_s::delta_f_pucch_format2b_e_");
}

// DeltaFList-SPUCCH-r15 ::= CHOICE
void delta_flist_spucch_r15_c::set(types::options e)
{
  type_ = e;
}
void delta_flist_spucch_r15_c::set_release()
{
  set(types::release);
}
delta_flist_spucch_r15_c::setup_s_& delta_flist_spucch_r15_c::set_setup()
{
  set(types::setup);
  return c;
}
void delta_flist_spucch_r15_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      if (c.delta_f_slot_spucch_format1_r15_present) {
        j.write_str("deltaF-slotSPUCCH-Format1-r15", c.delta_f_slot_spucch_format1_r15.to_string());
      }
      if (c.delta_f_slot_spucch_format1a_r15_present) {
        j.write_str("deltaF-slotSPUCCH-Format1a-r15", c.delta_f_slot_spucch_format1a_r15.to_string());
      }
      if (c.delta_f_slot_spucch_format1b_r15_present) {
        j.write_str("deltaF-slotSPUCCH-Format1b-r15", c.delta_f_slot_spucch_format1b_r15.to_string());
      }
      if (c.delta_f_slot_spucch_format3_r15_present) {
        j.write_str("deltaF-slotSPUCCH-Format3-r15", c.delta_f_slot_spucch_format3_r15.to_string());
      }
      if (c.delta_f_slot_spucch_rm_format4_r15_present) {
        j.write_str("deltaF-slotSPUCCH-RM-Format4-r15", c.delta_f_slot_spucch_rm_format4_r15.to_string());
      }
      if (c.delta_f_slot_spucch_tbcc_format4_r15_present) {
        j.write_str("deltaF-slotSPUCCH-TBCC-Format4-r15", c.delta_f_slot_spucch_tbcc_format4_r15.to_string());
      }
      if (c.delta_f_subslot_spucch_format1and1a_r15_present) {
        j.write_str("deltaF-subslotSPUCCH-Format1and1a-r15", c.delta_f_subslot_spucch_format1and1a_r15.to_string());
      }
      if (c.delta_f_subslot_spucch_format1b_r15_present) {
        j.write_str("deltaF-subslotSPUCCH-Format1b-r15", c.delta_f_subslot_spucch_format1b_r15.to_string());
      }
      if (c.delta_f_subslot_spucch_rm_format4_r15_present) {
        j.write_str("deltaF-subslotSPUCCH-RM-Format4-r15", c.delta_f_subslot_spucch_rm_format4_r15.to_string());
      }
      if (c.delta_f_subslot_spucch_tbcc_format4_r15_present) {
        j.write_str("deltaF-subslotSPUCCH-TBCC-Format4-r15", c.delta_f_subslot_spucch_tbcc_format4_r15.to_string());
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "delta_flist_spucch_r15_c");
  }
  j.end_obj();
}
SRSASN_CODE delta_flist_spucch_r15_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.pack(c.ext, 1);
      HANDLE_CODE(bref.pack(c.delta_f_slot_spucch_format1_r15_present, 1));
      HANDLE_CODE(bref.pack(c.delta_f_slot_spucch_format1a_r15_present, 1));
      HANDLE_CODE(bref.pack(c.delta_f_slot_spucch_format1b_r15_present, 1));
      HANDLE_CODE(bref.pack(c.delta_f_slot_spucch_format3_r15_present, 1));
      HANDLE_CODE(bref.pack(c.delta_f_slot_spucch_rm_format4_r15_present, 1));
      HANDLE_CODE(bref.pack(c.delta_f_slot_spucch_tbcc_format4_r15_present, 1));
      HANDLE_CODE(bref.pack(c.delta_f_subslot_spucch_format1and1a_r15_present, 1));
      HANDLE_CODE(bref.pack(c.delta_f_subslot_spucch_format1b_r15_present, 1));
      HANDLE_CODE(bref.pack(c.delta_f_subslot_spucch_rm_format4_r15_present, 1));
      HANDLE_CODE(bref.pack(c.delta_f_subslot_spucch_tbcc_format4_r15_present, 1));
      if (c.delta_f_slot_spucch_format1_r15_present) {
        HANDLE_CODE(c.delta_f_slot_spucch_format1_r15.pack(bref));
      }
      if (c.delta_f_slot_spucch_format1a_r15_present) {
        HANDLE_CODE(c.delta_f_slot_spucch_format1a_r15.pack(bref));
      }
      if (c.delta_f_slot_spucch_format1b_r15_present) {
        HANDLE_CODE(c.delta_f_slot_spucch_format1b_r15.pack(bref));
      }
      if (c.delta_f_slot_spucch_format3_r15_present) {
        HANDLE_CODE(c.delta_f_slot_spucch_format3_r15.pack(bref));
      }
      if (c.delta_f_slot_spucch_rm_format4_r15_present) {
        HANDLE_CODE(c.delta_f_slot_spucch_rm_format4_r15.pack(bref));
      }
      if (c.delta_f_slot_spucch_tbcc_format4_r15_present) {
        HANDLE_CODE(c.delta_f_slot_spucch_tbcc_format4_r15.pack(bref));
      }
      if (c.delta_f_subslot_spucch_format1and1a_r15_present) {
        HANDLE_CODE(c.delta_f_subslot_spucch_format1and1a_r15.pack(bref));
      }
      if (c.delta_f_subslot_spucch_format1b_r15_present) {
        HANDLE_CODE(c.delta_f_subslot_spucch_format1b_r15.pack(bref));
      }
      if (c.delta_f_subslot_spucch_rm_format4_r15_present) {
        HANDLE_CODE(c.delta_f_subslot_spucch_rm_format4_r15.pack(bref));
      }
      if (c.delta_f_subslot_spucch_tbcc_format4_r15_present) {
        HANDLE_CODE(c.delta_f_subslot_spucch_tbcc_format4_r15.pack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "delta_flist_spucch_r15_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE delta_flist_spucch_r15_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      bref.unpack(c.ext, 1);
      HANDLE_CODE(bref.unpack(c.delta_f_slot_spucch_format1_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.delta_f_slot_spucch_format1a_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.delta_f_slot_spucch_format1b_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.delta_f_slot_spucch_format3_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.delta_f_slot_spucch_rm_format4_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.delta_f_slot_spucch_tbcc_format4_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.delta_f_subslot_spucch_format1and1a_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.delta_f_subslot_spucch_format1b_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.delta_f_subslot_spucch_rm_format4_r15_present, 1));
      HANDLE_CODE(bref.unpack(c.delta_f_subslot_spucch_tbcc_format4_r15_present, 1));
      if (c.delta_f_slot_spucch_format1_r15_present) {
        HANDLE_CODE(c.delta_f_slot_spucch_format1_r15.unpack(bref));
      }
      if (c.delta_f_slot_spucch_format1a_r15_present) {
        HANDLE_CODE(c.delta_f_slot_spucch_format1a_r15.unpack(bref));
      }
      if (c.delta_f_slot_spucch_format1b_r15_present) {
        HANDLE_CODE(c.delta_f_slot_spucch_format1b_r15.unpack(bref));
      }
      if (c.delta_f_slot_spucch_format3_r15_present) {
        HANDLE_CODE(c.delta_f_slot_spucch_format3_r15.unpack(bref));
      }
      if (c.delta_f_slot_spucch_rm_format4_r15_present) {
        HANDLE_CODE(c.delta_f_slot_spucch_rm_format4_r15.unpack(bref));
      }
      if (c.delta_f_slot_spucch_tbcc_format4_r15_present) {
        HANDLE_CODE(c.delta_f_slot_spucch_tbcc_format4_r15.unpack(bref));
      }
      if (c.delta_f_subslot_spucch_format1and1a_r15_present) {
        HANDLE_CODE(c.delta_f_subslot_spucch_format1and1a_r15.unpack(bref));
      }
      if (c.delta_f_subslot_spucch_format1b_r15_present) {
        HANDLE_CODE(c.delta_f_subslot_spucch_format1b_r15.unpack(bref));
      }
      if (c.delta_f_subslot_spucch_rm_format4_r15_present) {
        HANDLE_CODE(c.delta_f_subslot_spucch_rm_format4_r15.unpack(bref));
      }
      if (c.delta_f_subslot_spucch_tbcc_format4_r15_present) {
        HANDLE_CODE(c.delta_f_subslot_spucch_tbcc_format4_r15.unpack(bref));
      }
      break;
    default:
      log_invalid_choice_id(type_, "delta_flist_spucch_r15_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool delta_flist_spucch_r15_c::operator==(const delta_flist_spucch_r15_c& other) const
{
  return type() == other.type() and c.ext == other.c.ext and
         c.delta_f_slot_spucch_format1_r15_present == other.c.delta_f_slot_spucch_format1_r15_present and
         (not c.delta_f_slot_spucch_format1_r15_present or
          c.delta_f_slot_spucch_format1_r15 == other.c.delta_f_slot_spucch_format1_r15) and
         c.delta_f_slot_spucch_format1a_r15_present == other.c.delta_f_slot_spucch_format1a_r15_present and
         (not c.delta_f_slot_spucch_format1a_r15_present or
          c.delta_f_slot_spucch_format1a_r15 == other.c.delta_f_slot_spucch_format1a_r15) and
         c.delta_f_slot_spucch_format1b_r15_present == other.c.delta_f_slot_spucch_format1b_r15_present and
         (not c.delta_f_slot_spucch_format1b_r15_present or
          c.delta_f_slot_spucch_format1b_r15 == other.c.delta_f_slot_spucch_format1b_r15) and
         c.delta_f_slot_spucch_format3_r15_present == other.c.delta_f_slot_spucch_format3_r15_present and
         (not c.delta_f_slot_spucch_format3_r15_present or
          c.delta_f_slot_spucch_format3_r15 == other.c.delta_f_slot_spucch_format3_r15) and
         c.delta_f_slot_spucch_rm_format4_r15_present == other.c.delta_f_slot_spucch_rm_format4_r15_present and
         (not c.delta_f_slot_spucch_rm_format4_r15_present or
          c.delta_f_slot_spucch_rm_format4_r15 == other.c.delta_f_slot_spucch_rm_format4_r15) and
         c.delta_f_slot_spucch_tbcc_format4_r15_present == other.c.delta_f_slot_spucch_tbcc_format4_r15_present and
         (not c.delta_f_slot_spucch_tbcc_format4_r15_present or
          c.delta_f_slot_spucch_tbcc_format4_r15 == other.c.delta_f_slot_spucch_tbcc_format4_r15) and
         c.delta_f_subslot_spucch_format1and1a_r15_present ==
             other.c.delta_f_subslot_spucch_format1and1a_r15_present and
         (not c.delta_f_subslot_spucch_format1and1a_r15_present or
          c.delta_f_subslot_spucch_format1and1a_r15 == other.c.delta_f_subslot_spucch_format1and1a_r15) and
         c.delta_f_subslot_spucch_format1b_r15_present == other.c.delta_f_subslot_spucch_format1b_r15_present and
         (not c.delta_f_subslot_spucch_format1b_r15_present or
          c.delta_f_subslot_spucch_format1b_r15 == other.c.delta_f_subslot_spucch_format1b_r15) and
         c.delta_f_subslot_spucch_rm_format4_r15_present == other.c.delta_f_subslot_spucch_rm_format4_r15_present and
         (not c.delta_f_subslot_spucch_rm_format4_r15_present or
          c.delta_f_subslot_spucch_rm_format4_r15 == other.c.delta_f_subslot_spucch_rm_format4_r15) and
         c.delta_f_subslot_spucch_tbcc_format4_r15_present ==
             other.c.delta_f_subslot_spucch_tbcc_format4_r15_present and
         (not c.delta_f_subslot_spucch_tbcc_format4_r15_present or
          c.delta_f_subslot_spucch_tbcc_format4_r15 == other.c.delta_f_subslot_spucch_tbcc_format4_r15);
}

const char* delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF-1", "deltaF0", "deltaF1", "deltaF2", "deltaF3", "deltaF4", "deltaF5", "deltaF6"};
  return convert_enum_idx(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1_r15_e_");
}
int8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1_r15_opts::to_number() const
{
  static const int8_t options[] = {-1, 0, 1, 2, 3, 4, 5, 6};
  return map_enum_number(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1_r15_e_");
}

const char* delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1a_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF1", "deltaF2", "deltaF3", "deltaF4", "deltaF5", "deltaF6", "deltaF7", "deltaF8"};
  return convert_enum_idx(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1a_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1a_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4, 5, 6, 7, 8};
  return map_enum_number(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1a_r15_e_");
}

const char* delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1b_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF3", "deltaF4", "deltaF5", "deltaF6", "deltaF7", "deltaF8", "deltaF9", "deltaF10"};
  return convert_enum_idx(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1b_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1b_r15_opts::to_number() const
{
  static const uint8_t options[] = {3, 4, 5, 6, 7, 8, 9, 10};
  return map_enum_number(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format1b_r15_e_");
}

const char* delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format3_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF4", "deltaF5", "deltaF6", "deltaF7", "deltaF8", "deltaF9", "deltaF10", "deltaF11"};
  return convert_enum_idx(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format3_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format3_r15_opts::to_number() const
{
  static const uint8_t options[] = {4, 5, 6, 7, 8, 9, 10, 11};
  return map_enum_number(options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_format3_r15_e_");
}

const char* delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_rm_format4_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF13", "deltaF14", "deltaF15", "deltaF16", "deltaF17", "deltaF18", "deltaF19", "deltaF20"};
  return convert_enum_idx(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_rm_format4_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_rm_format4_r15_opts::to_number() const
{
  static const uint8_t options[] = {13, 14, 15, 16, 17, 18, 19, 20};
  return map_enum_number(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_rm_format4_r15_e_");
}

const char* delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_tbcc_format4_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF10", "deltaF11", "deltaF12", "deltaF13", "deltaF14", "deltaF15", "deltaF16", "deltaF17"};
  return convert_enum_idx(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_tbcc_format4_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_tbcc_format4_r15_opts::to_number() const
{
  static const uint8_t options[] = {10, 11, 12, 13, 14, 15, 16, 17};
  return map_enum_number(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_slot_spucch_tbcc_format4_r15_e_");
}

const char* delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1and1a_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF5", "deltaF6", "deltaF7", "deltaF8", "deltaF9", "deltaF10", "deltaF11", "deltaF12"};
  return convert_enum_idx(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1and1a_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1and1a_r15_opts::to_number() const
{
  static const uint8_t options[] = {5, 6, 7, 8, 9, 10, 11, 12};
  return map_enum_number(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1and1a_r15_e_");
}

const char* delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1b_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF6", "deltaF7", "deltaF8", "deltaF9", "deltaF10", "deltaF11", "deltaF12", "deltaF13"};
  return convert_enum_idx(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1b_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1b_r15_opts::to_number() const
{
  static const uint8_t options[] = {6, 7, 8, 9, 10, 11, 12, 13};
  return map_enum_number(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_format1b_r15_e_");
}

const char* delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_rm_format4_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF15", "deltaF16", "deltaF17", "deltaF18", "deltaF19", "deltaF20", "deltaF21", "deltaF22"};
  return convert_enum_idx(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_rm_format4_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_rm_format4_r15_opts::to_number() const
{
  static const uint8_t options[] = {15, 16, 17, 18, 19, 20, 21, 22};
  return map_enum_number(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_rm_format4_r15_e_");
}

const char* delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_tbcc_format4_r15_opts::to_string() const
{
  static const char* options[] = {
      "deltaF10", "deltaF11", "deltaF12", "deltaF13", "deltaF14", "deltaF15", "deltaF16", "deltaF17"};
  return convert_enum_idx(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_tbcc_format4_r15_e_");
}
uint8_t delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_tbcc_format4_r15_opts::to_number() const
{
  static const uint8_t options[] = {10, 11, 12, 13, 14, 15, 16, 17};
  return map_enum_number(
      options, 8, value, "delta_flist_spucch_r15_c::setup_s_::delta_f_subslot_spucch_tbcc_format4_r15_e_");
}

// EDT-PRACH-ParametersCE-r15 ::= SEQUENCE
SRSASN_CODE edt_prach_params_ce_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(edt_prach_params_ce_r15_present, 1));

  if (edt_prach_params_ce_r15_present) {
    HANDLE_CODE(bref.pack(edt_prach_params_ce_r15.prach_start_sf_r15_present, 1));
    HANDLE_CODE(pack_integer(bref, edt_prach_params_ce_r15.prach_cfg_idx_r15, (uint8_t)0u, (uint8_t)63u));
    HANDLE_CODE(pack_integer(bref, edt_prach_params_ce_r15.prach_freq_offset_r15, (uint8_t)0u, (uint8_t)94u));
    if (edt_prach_params_ce_r15.prach_start_sf_r15_present) {
      HANDLE_CODE(edt_prach_params_ce_r15.prach_start_sf_r15.pack(bref));
    }
    HANDLE_CODE(
        pack_dyn_seq_of(bref, edt_prach_params_ce_r15.mpdcch_nbs_to_monitor_r15, 1, 2, integer_packer<uint8_t>(1, 16)));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE edt_prach_params_ce_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(edt_prach_params_ce_r15_present, 1));

  if (edt_prach_params_ce_r15_present) {
    HANDLE_CODE(bref.unpack(edt_prach_params_ce_r15.prach_start_sf_r15_present, 1));
    HANDLE_CODE(unpack_integer(edt_prach_params_ce_r15.prach_cfg_idx_r15, bref, (uint8_t)0u, (uint8_t)63u));
    HANDLE_CODE(unpack_integer(edt_prach_params_ce_r15.prach_freq_offset_r15, bref, (uint8_t)0u, (uint8_t)94u));
    if (edt_prach_params_ce_r15.prach_start_sf_r15_present) {
      HANDLE_CODE(edt_prach_params_ce_r15.prach_start_sf_r15.unpack(bref));
    }
    HANDLE_CODE(unpack_dyn_seq_of(
        edt_prach_params_ce_r15.mpdcch_nbs_to_monitor_r15, bref, 1, 2, integer_packer<uint8_t>(1, 16)));
  }

  return SRSASN_SUCCESS;
}
void edt_prach_params_ce_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (edt_prach_params_ce_r15_present) {
    j.write_fieldname("edt-PRACH-ParametersCE-r15");
    j.start_obj();
    j.write_int("prach-ConfigIndex-r15", edt_prach_params_ce_r15.prach_cfg_idx_r15);
    j.write_int("prach-FreqOffset-r15", edt_prach_params_ce_r15.prach_freq_offset_r15);
    if (edt_prach_params_ce_r15.prach_start_sf_r15_present) {
      j.write_str("prach-StartingSubframe-r15", edt_prach_params_ce_r15.prach_start_sf_r15.to_string());
    }
    j.start_array("mpdcch-NarrowbandsToMonitor-r15");
    for (const auto& e1 : edt_prach_params_ce_r15.mpdcch_nbs_to_monitor_r15) {
      j.write_int(e1);
    }
    j.end_array();
    j.end_obj();
  }
  j.end_obj();
}

const char* edt_prach_params_ce_r15_s::edt_prach_params_ce_r15_s_::prach_start_sf_r15_opts::to_string() const
{
  static const char* options[] = {"sf2", "sf4", "sf8", "sf16", "sf32", "sf64", "sf128", "sf256"};
  return convert_enum_idx(
      options, 8, value, "edt_prach_params_ce_r15_s::edt_prach_params_ce_r15_s_::prach_start_sf_r15_e_");
}
uint16_t edt_prach_params_ce_r15_s::edt_prach_params_ce_r15_s_::prach_start_sf_r15_opts::to_number() const
{
  static const uint16_t options[] = {2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(
      options, 8, value, "edt_prach_params_ce_r15_s::edt_prach_params_ce_r15_s_::prach_start_sf_r15_e_");
}

// PRACH-ConfigInfo ::= SEQUENCE
SRSASN_CODE prach_cfg_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, prach_cfg_idx, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(bref.pack(high_speed_flag, 1));
  HANDLE_CODE(pack_integer(bref, zero_correlation_zone_cfg, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(pack_integer(bref, prach_freq_offset, (uint8_t)0u, (uint8_t)94u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE prach_cfg_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(prach_cfg_idx, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(bref.unpack(high_speed_flag, 1));
  HANDLE_CODE(unpack_integer(zero_correlation_zone_cfg, bref, (uint8_t)0u, (uint8_t)15u));
  HANDLE_CODE(unpack_integer(prach_freq_offset, bref, (uint8_t)0u, (uint8_t)94u));

  return SRSASN_SUCCESS;
}
void prach_cfg_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("prach-ConfigIndex", prach_cfg_idx);
  j.write_bool("highSpeedFlag", high_speed_flag);
  j.write_int("zeroCorrelationZoneConfig", zero_correlation_zone_cfg);
  j.write_int("prach-FreqOffset", prach_freq_offset);
  j.end_obj();
}
bool prach_cfg_info_s::operator==(const prach_cfg_info_s& other) const
{
  return prach_cfg_idx == other.prach_cfg_idx and high_speed_flag == other.high_speed_flag and
         zero_correlation_zone_cfg == other.zero_correlation_zone_cfg and prach_freq_offset == other.prach_freq_offset;
}

// PowerRampingParameters ::= SEQUENCE
SRSASN_CODE pwr_ramp_params_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pwr_ramp_step.pack(bref));
  HANDLE_CODE(preamb_init_rx_target_pwr.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pwr_ramp_params_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(pwr_ramp_step.unpack(bref));
  HANDLE_CODE(preamb_init_rx_target_pwr.unpack(bref));

  return SRSASN_SUCCESS;
}
void pwr_ramp_params_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("powerRampingStep", pwr_ramp_step.to_string());
  j.write_str("preambleInitialReceivedTargetPower", preamb_init_rx_target_pwr.to_string());
  j.end_obj();
}
bool pwr_ramp_params_s::operator==(const pwr_ramp_params_s& other) const
{
  return pwr_ramp_step == other.pwr_ramp_step and preamb_init_rx_target_pwr == other.preamb_init_rx_target_pwr;
}

const char* pwr_ramp_params_s::pwr_ramp_step_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB2", "dB4", "dB6"};
  return convert_enum_idx(options, 4, value, "pwr_ramp_params_s::pwr_ramp_step_e_");
}
uint8_t pwr_ramp_params_s::pwr_ramp_step_opts::to_number() const
{
  static const uint8_t options[] = {0, 2, 4, 6};
  return map_enum_number(options, 4, value, "pwr_ramp_params_s::pwr_ramp_step_e_");
}

const char* pwr_ramp_params_s::preamb_init_rx_target_pwr_opts::to_string() const
{
  static const char* options[] = {"dBm-120",
                                  "dBm-118",
                                  "dBm-116",
                                  "dBm-114",
                                  "dBm-112",
                                  "dBm-110",
                                  "dBm-108",
                                  "dBm-106",
                                  "dBm-104",
                                  "dBm-102",
                                  "dBm-100",
                                  "dBm-98",
                                  "dBm-96",
                                  "dBm-94",
                                  "dBm-92",
                                  "dBm-90"};
  return convert_enum_idx(options, 16, value, "pwr_ramp_params_s::preamb_init_rx_target_pwr_e_");
}
int8_t pwr_ramp_params_s::preamb_init_rx_target_pwr_opts::to_number() const
{
  static const int8_t options[] = {
      -120, -118, -116, -114, -112, -110, -108, -106, -104, -102, -100, -98, -96, -94, -92, -90};
  return map_enum_number(options, 16, value, "pwr_ramp_params_s::preamb_init_rx_target_pwr_e_");
}

// PreambleTransMax ::= ENUMERATED
const char* preamb_trans_max_opts::to_string() const
{
  static const char* options[] = {"n3", "n4", "n5", "n6", "n7", "n8", "n10", "n20", "n50", "n100", "n200"};
  return convert_enum_idx(options, 11, value, "preamb_trans_max_e");
}
uint8_t preamb_trans_max_opts::to_number() const
{
  static const uint8_t options[] = {3, 4, 5, 6, 7, 8, 10, 20, 50, 100, 200};
  return map_enum_number(options, 11, value, "preamb_trans_max_e");
}

// UL-ReferenceSignalsPUSCH ::= SEQUENCE
SRSASN_CODE ul_ref_sigs_pusch_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(group_hop_enabled, 1));
  HANDLE_CODE(pack_integer(bref, group_assign_pusch, (uint8_t)0u, (uint8_t)29u));
  HANDLE_CODE(bref.pack(seq_hop_enabled, 1));
  HANDLE_CODE(pack_integer(bref, cyclic_shift, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_ref_sigs_pusch_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(group_hop_enabled, 1));
  HANDLE_CODE(unpack_integer(group_assign_pusch, bref, (uint8_t)0u, (uint8_t)29u));
  HANDLE_CODE(bref.unpack(seq_hop_enabled, 1));
  HANDLE_CODE(unpack_integer(cyclic_shift, bref, (uint8_t)0u, (uint8_t)7u));

  return SRSASN_SUCCESS;
}
void ul_ref_sigs_pusch_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_bool("groupHoppingEnabled", group_hop_enabled);
  j.write_int("groupAssignmentPUSCH", group_assign_pusch);
  j.write_bool("sequenceHoppingEnabled", seq_hop_enabled);
  j.write_int("cyclicShift", cyclic_shift);
  j.end_obj();
}
bool ul_ref_sigs_pusch_s::operator==(const ul_ref_sigs_pusch_s& other) const
{
  return group_hop_enabled == other.group_hop_enabled and group_assign_pusch == other.group_assign_pusch and
         seq_hop_enabled == other.seq_hop_enabled and cyclic_shift == other.cyclic_shift;
}

// BCCH-Config ::= SEQUENCE
SRSASN_CODE bcch_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(mod_period_coeff.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(mod_period_coeff.unpack(bref));

  return SRSASN_SUCCESS;
}
void bcch_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("modificationPeriodCoeff", mod_period_coeff.to_string());
  j.end_obj();
}

const char* bcch_cfg_s::mod_period_coeff_opts::to_string() const
{
  static const char* options[] = {"n2", "n4", "n8", "n16"};
  return convert_enum_idx(options, 4, value, "bcch_cfg_s::mod_period_coeff_e_");
}
uint8_t bcch_cfg_s::mod_period_coeff_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8, 16};
  return map_enum_number(options, 4, value, "bcch_cfg_s::mod_period_coeff_e_");
}

// BCCH-Config-v1310 ::= SEQUENCE
SRSASN_CODE bcch_cfg_v1310_s::pack(bit_ref& bref) const
{
  return SRSASN_SUCCESS;
}
SRSASN_CODE bcch_cfg_v1310_s::unpack(cbit_ref& bref)
{
  return SRSASN_SUCCESS;
}
void bcch_cfg_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("modificationPeriodCoeff-v1310", "n64");
  j.end_obj();
}

// FreqHoppingParameters-r13 ::= SEQUENCE
SRSASN_CODE freq_hop_params_r13_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(dummy_present, 1));
  HANDLE_CODE(bref.pack(dummy2_present, 1));
  HANDLE_CODE(bref.pack(dummy3_present, 1));
  HANDLE_CODE(bref.pack(interv_ul_hop_cfg_common_mode_a_r13_present, 1));
  HANDLE_CODE(bref.pack(interv_ul_hop_cfg_common_mode_b_r13_present, 1));
  HANDLE_CODE(bref.pack(dummy4_present, 1));

  if (dummy_present) {
    HANDLE_CODE(dummy.pack(bref));
  }
  if (dummy2_present) {
    HANDLE_CODE(dummy2.pack(bref));
  }
  if (dummy3_present) {
    HANDLE_CODE(dummy3.pack(bref));
  }
  if (interv_ul_hop_cfg_common_mode_a_r13_present) {
    HANDLE_CODE(interv_ul_hop_cfg_common_mode_a_r13.pack(bref));
  }
  if (interv_ul_hop_cfg_common_mode_b_r13_present) {
    HANDLE_CODE(interv_ul_hop_cfg_common_mode_b_r13.pack(bref));
  }
  if (dummy4_present) {
    HANDLE_CODE(pack_integer(bref, dummy4, (uint8_t)1u, (uint8_t)16u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_hop_params_r13_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(dummy_present, 1));
  HANDLE_CODE(bref.unpack(dummy2_present, 1));
  HANDLE_CODE(bref.unpack(dummy3_present, 1));
  HANDLE_CODE(bref.unpack(interv_ul_hop_cfg_common_mode_a_r13_present, 1));
  HANDLE_CODE(bref.unpack(interv_ul_hop_cfg_common_mode_b_r13_present, 1));
  HANDLE_CODE(bref.unpack(dummy4_present, 1));

  if (dummy_present) {
    HANDLE_CODE(dummy.unpack(bref));
  }
  if (dummy2_present) {
    HANDLE_CODE(dummy2.unpack(bref));
  }
  if (dummy3_present) {
    HANDLE_CODE(dummy3.unpack(bref));
  }
  if (interv_ul_hop_cfg_common_mode_a_r13_present) {
    HANDLE_CODE(interv_ul_hop_cfg_common_mode_a_r13.unpack(bref));
  }
  if (interv_ul_hop_cfg_common_mode_b_r13_present) {
    HANDLE_CODE(interv_ul_hop_cfg_common_mode_b_r13.unpack(bref));
  }
  if (dummy4_present) {
    HANDLE_CODE(unpack_integer(dummy4, bref, (uint8_t)1u, (uint8_t)16u));
  }

  return SRSASN_SUCCESS;
}
void freq_hop_params_r13_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (dummy_present) {
    j.write_str("dummy", dummy.to_string());
  }
  if (dummy2_present) {
    j.write_fieldname("dummy2");
    dummy2.to_json(j);
  }
  if (dummy3_present) {
    j.write_fieldname("dummy3");
    dummy3.to_json(j);
  }
  if (interv_ul_hop_cfg_common_mode_a_r13_present) {
    j.write_fieldname("interval-ULHoppingConfigCommonModeA-r13");
    interv_ul_hop_cfg_common_mode_a_r13.to_json(j);
  }
  if (interv_ul_hop_cfg_common_mode_b_r13_present) {
    j.write_fieldname("interval-ULHoppingConfigCommonModeB-r13");
    interv_ul_hop_cfg_common_mode_b_r13.to_json(j);
  }
  if (dummy4_present) {
    j.write_int("dummy4", dummy4);
  }
  j.end_obj();
}

const char* freq_hop_params_r13_s::dummy_opts::to_string() const
{
  static const char* options[] = {"nb2", "nb4"};
  return convert_enum_idx(options, 2, value, "freq_hop_params_r13_s::dummy_e_");
}
uint8_t freq_hop_params_r13_s::dummy_opts::to_number() const
{
  static const uint8_t options[] = {2, 4};
  return map_enum_number(options, 2, value, "freq_hop_params_r13_s::dummy_e_");
}

void freq_hop_params_r13_s::dummy2_c_::destroy_() {}
void freq_hop_params_r13_s::dummy2_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
freq_hop_params_r13_s::dummy2_c_::dummy2_c_(const freq_hop_params_r13_s::dummy2_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::interv_fdd_r13:
      c.init(other.c.get<interv_fdd_r13_e_>());
      break;
    case types::interv_tdd_r13:
      c.init(other.c.get<interv_tdd_r13_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::dummy2_c_");
  }
}
freq_hop_params_r13_s::dummy2_c_&
freq_hop_params_r13_s::dummy2_c_::operator=(const freq_hop_params_r13_s::dummy2_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::interv_fdd_r13:
      c.set(other.c.get<interv_fdd_r13_e_>());
      break;
    case types::interv_tdd_r13:
      c.set(other.c.get<interv_tdd_r13_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::dummy2_c_");
  }

  return *this;
}
freq_hop_params_r13_s::dummy2_c_::interv_fdd_r13_e_& freq_hop_params_r13_s::dummy2_c_::set_interv_fdd_r13()
{
  set(types::interv_fdd_r13);
  return c.get<interv_fdd_r13_e_>();
}
freq_hop_params_r13_s::dummy2_c_::interv_tdd_r13_e_& freq_hop_params_r13_s::dummy2_c_::set_interv_tdd_r13()
{
  set(types::interv_tdd_r13);
  return c.get<interv_tdd_r13_e_>();
}
void freq_hop_params_r13_s::dummy2_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::interv_fdd_r13:
      j.write_str("interval-FDD-r13", c.get<interv_fdd_r13_e_>().to_string());
      break;
    case types::interv_tdd_r13:
      j.write_str("interval-TDD-r13", c.get<interv_tdd_r13_e_>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::dummy2_c_");
  }
  j.end_obj();
}
SRSASN_CODE freq_hop_params_r13_s::dummy2_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::interv_fdd_r13:
      HANDLE_CODE(c.get<interv_fdd_r13_e_>().pack(bref));
      break;
    case types::interv_tdd_r13:
      HANDLE_CODE(c.get<interv_tdd_r13_e_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::dummy2_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_hop_params_r13_s::dummy2_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::interv_fdd_r13:
      HANDLE_CODE(c.get<interv_fdd_r13_e_>().unpack(bref));
      break;
    case types::interv_tdd_r13:
      HANDLE_CODE(c.get<interv_tdd_r13_e_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::dummy2_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* freq_hop_params_r13_s::dummy2_c_::interv_fdd_r13_opts::to_string() const
{
  static const char* options[] = {"int1", "int2", "int4", "int8"};
  return convert_enum_idx(options, 4, value, "freq_hop_params_r13_s::dummy2_c_::interv_fdd_r13_e_");
}
uint8_t freq_hop_params_r13_s::dummy2_c_::interv_fdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(options, 4, value, "freq_hop_params_r13_s::dummy2_c_::interv_fdd_r13_e_");
}

const char* freq_hop_params_r13_s::dummy2_c_::interv_tdd_r13_opts::to_string() const
{
  static const char* options[] = {"int1", "int5", "int10", "int20"};
  return convert_enum_idx(options, 4, value, "freq_hop_params_r13_s::dummy2_c_::interv_tdd_r13_e_");
}
uint8_t freq_hop_params_r13_s::dummy2_c_::interv_tdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 5, 10, 20};
  return map_enum_number(options, 4, value, "freq_hop_params_r13_s::dummy2_c_::interv_tdd_r13_e_");
}

void freq_hop_params_r13_s::dummy3_c_::destroy_() {}
void freq_hop_params_r13_s::dummy3_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
freq_hop_params_r13_s::dummy3_c_::dummy3_c_(const freq_hop_params_r13_s::dummy3_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::interv_fdd_r13:
      c.init(other.c.get<interv_fdd_r13_e_>());
      break;
    case types::interv_tdd_r13:
      c.init(other.c.get<interv_tdd_r13_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::dummy3_c_");
  }
}
freq_hop_params_r13_s::dummy3_c_&
freq_hop_params_r13_s::dummy3_c_::operator=(const freq_hop_params_r13_s::dummy3_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::interv_fdd_r13:
      c.set(other.c.get<interv_fdd_r13_e_>());
      break;
    case types::interv_tdd_r13:
      c.set(other.c.get<interv_tdd_r13_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::dummy3_c_");
  }

  return *this;
}
freq_hop_params_r13_s::dummy3_c_::interv_fdd_r13_e_& freq_hop_params_r13_s::dummy3_c_::set_interv_fdd_r13()
{
  set(types::interv_fdd_r13);
  return c.get<interv_fdd_r13_e_>();
}
freq_hop_params_r13_s::dummy3_c_::interv_tdd_r13_e_& freq_hop_params_r13_s::dummy3_c_::set_interv_tdd_r13()
{
  set(types::interv_tdd_r13);
  return c.get<interv_tdd_r13_e_>();
}
void freq_hop_params_r13_s::dummy3_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::interv_fdd_r13:
      j.write_str("interval-FDD-r13", c.get<interv_fdd_r13_e_>().to_string());
      break;
    case types::interv_tdd_r13:
      j.write_str("interval-TDD-r13", c.get<interv_tdd_r13_e_>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::dummy3_c_");
  }
  j.end_obj();
}
SRSASN_CODE freq_hop_params_r13_s::dummy3_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::interv_fdd_r13:
      HANDLE_CODE(c.get<interv_fdd_r13_e_>().pack(bref));
      break;
    case types::interv_tdd_r13:
      HANDLE_CODE(c.get<interv_tdd_r13_e_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::dummy3_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_hop_params_r13_s::dummy3_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::interv_fdd_r13:
      HANDLE_CODE(c.get<interv_fdd_r13_e_>().unpack(bref));
      break;
    case types::interv_tdd_r13:
      HANDLE_CODE(c.get<interv_tdd_r13_e_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::dummy3_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* freq_hop_params_r13_s::dummy3_c_::interv_fdd_r13_opts::to_string() const
{
  static const char* options[] = {"int2", "int4", "int8", "int16"};
  return convert_enum_idx(options, 4, value, "freq_hop_params_r13_s::dummy3_c_::interv_fdd_r13_e_");
}
uint8_t freq_hop_params_r13_s::dummy3_c_::interv_fdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8, 16};
  return map_enum_number(options, 4, value, "freq_hop_params_r13_s::dummy3_c_::interv_fdd_r13_e_");
}

const char* freq_hop_params_r13_s::dummy3_c_::interv_tdd_r13_opts::to_string() const
{
  static const char* options[] = {"int5", "int10", "int20", "int40"};
  return convert_enum_idx(options, 4, value, "freq_hop_params_r13_s::dummy3_c_::interv_tdd_r13_e_");
}
uint8_t freq_hop_params_r13_s::dummy3_c_::interv_tdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 20, 40};
  return map_enum_number(options, 4, value, "freq_hop_params_r13_s::dummy3_c_::interv_tdd_r13_e_");
}

void freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::destroy_() {}
void freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_ul_hop_cfg_common_mode_a_r13_c_(
    const freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::interv_fdd_r13:
      c.init(other.c.get<interv_fdd_r13_e_>());
      break;
    case types::interv_tdd_r13:
      c.init(other.c.get<interv_tdd_r13_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_");
  }
}
freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_&
freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::operator=(
    const freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::interv_fdd_r13:
      c.set(other.c.get<interv_fdd_r13_e_>());
      break;
    case types::interv_tdd_r13:
      c.set(other.c.get<interv_tdd_r13_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_");
  }

  return *this;
}
freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_e_&
freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::set_interv_fdd_r13()
{
  set(types::interv_fdd_r13);
  return c.get<interv_fdd_r13_e_>();
}
freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_e_&
freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::set_interv_tdd_r13()
{
  set(types::interv_tdd_r13);
  return c.get<interv_tdd_r13_e_>();
}
void freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::interv_fdd_r13:
      j.write_str("interval-FDD-r13", c.get<interv_fdd_r13_e_>().to_string());
      break;
    case types::interv_tdd_r13:
      j.write_str("interval-TDD-r13", c.get<interv_tdd_r13_e_>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::interv_fdd_r13:
      HANDLE_CODE(c.get<interv_fdd_r13_e_>().pack(bref));
      break;
    case types::interv_tdd_r13:
      HANDLE_CODE(c.get<interv_tdd_r13_e_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::interv_fdd_r13:
      HANDLE_CODE(c.get<interv_fdd_r13_e_>().unpack(bref));
      break;
    case types::interv_tdd_r13:
      HANDLE_CODE(c.get<interv_tdd_r13_e_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_opts::to_string() const
{
  static const char* options[] = {"int1", "int2", "int4", "int8"};
  return convert_enum_idx(
      options, 4, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_e_");
}
uint8_t freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(
      options, 4, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_fdd_r13_e_");
}

const char* freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_opts::to_string() const
{
  static const char* options[] = {"int1", "int5", "int10", "int20"};
  return convert_enum_idx(
      options, 4, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_e_");
}
uint8_t freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 5, 10, 20};
  return map_enum_number(
      options, 4, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_a_r13_c_::interv_tdd_r13_e_");
}

void freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::destroy_() {}
void freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_ul_hop_cfg_common_mode_b_r13_c_(
    const freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::interv_fdd_r13:
      c.init(other.c.get<interv_fdd_r13_e_>());
      break;
    case types::interv_tdd_r13:
      c.init(other.c.get<interv_tdd_r13_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_");
  }
}
freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_&
freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::operator=(
    const freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::interv_fdd_r13:
      c.set(other.c.get<interv_fdd_r13_e_>());
      break;
    case types::interv_tdd_r13:
      c.set(other.c.get<interv_tdd_r13_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_");
  }

  return *this;
}
freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_e_&
freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::set_interv_fdd_r13()
{
  set(types::interv_fdd_r13);
  return c.get<interv_fdd_r13_e_>();
}
freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_e_&
freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::set_interv_tdd_r13()
{
  set(types::interv_tdd_r13);
  return c.get<interv_tdd_r13_e_>();
}
void freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::interv_fdd_r13:
      j.write_str("interval-FDD-r13", c.get<interv_fdd_r13_e_>().to_string());
      break;
    case types::interv_tdd_r13:
      j.write_str("interval-TDD-r13", c.get<interv_tdd_r13_e_>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::interv_fdd_r13:
      HANDLE_CODE(c.get<interv_fdd_r13_e_>().pack(bref));
      break;
    case types::interv_tdd_r13:
      HANDLE_CODE(c.get<interv_tdd_r13_e_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::interv_fdd_r13:
      HANDLE_CODE(c.get<interv_fdd_r13_e_>().unpack(bref));
      break;
    case types::interv_tdd_r13:
      HANDLE_CODE(c.get<interv_tdd_r13_e_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_opts::to_string() const
{
  static const char* options[] = {"int2", "int4", "int8", "int16"};
  return convert_enum_idx(
      options, 4, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_e_");
}
uint8_t freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 8, 16};
  return map_enum_number(
      options, 4, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_fdd_r13_e_");
}

const char* freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_opts::to_string() const
{
  static const char* options[] = {"int5", "int10", "int20", "int40"};
  return convert_enum_idx(
      options, 4, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_e_");
}
uint8_t freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {5, 10, 20, 40};
  return map_enum_number(
      options, 4, value, "freq_hop_params_r13_s::interv_ul_hop_cfg_common_mode_b_r13_c_::interv_tdd_r13_e_");
}

// HighSpeedConfig-r14 ::= SEQUENCE
SRSASN_CODE high_speed_cfg_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(high_speed_enhanced_meas_flag_r14_present, 1));
  HANDLE_CODE(bref.pack(high_speed_enhanced_demod_flag_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE high_speed_cfg_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(high_speed_enhanced_meas_flag_r14_present, 1));
  HANDLE_CODE(bref.unpack(high_speed_enhanced_demod_flag_r14_present, 1));

  return SRSASN_SUCCESS;
}
void high_speed_cfg_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (high_speed_enhanced_meas_flag_r14_present) {
    j.write_str("highSpeedEnhancedMeasFlag-r14", "true");
  }
  if (high_speed_enhanced_demod_flag_r14_present) {
    j.write_str("highSpeedEnhancedDemodulationFlag-r14", "true");
  }
  j.end_obj();
}

// HighSpeedConfig-v1530 ::= SEQUENCE
SRSASN_CODE high_speed_cfg_v1530_s::pack(bit_ref& bref) const
{
  return SRSASN_SUCCESS;
}
SRSASN_CODE high_speed_cfg_v1530_s::unpack(cbit_ref& bref)
{
  return SRSASN_SUCCESS;
}
void high_speed_cfg_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("highSpeedMeasGapCE-ModeA-r15", "true");
  j.end_obj();
}

// PCCH-Config ::= SEQUENCE
SRSASN_CODE pcch_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(default_paging_cycle.pack(bref));
  HANDLE_CODE(nb.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(default_paging_cycle.unpack(bref));
  HANDLE_CODE(nb.unpack(bref));

  return SRSASN_SUCCESS;
}
void pcch_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("defaultPagingCycle", default_paging_cycle.to_string());
  j.write_str("nB", nb.to_string());
  j.end_obj();
}

const char* pcch_cfg_s::default_paging_cycle_opts::to_string() const
{
  static const char* options[] = {"rf32", "rf64", "rf128", "rf256"};
  return convert_enum_idx(options, 4, value, "pcch_cfg_s::default_paging_cycle_e_");
}
uint16_t pcch_cfg_s::default_paging_cycle_opts::to_number() const
{
  static const uint16_t options[] = {32, 64, 128, 256};
  return map_enum_number(options, 4, value, "pcch_cfg_s::default_paging_cycle_e_");
}

const char* pcch_cfg_s::nb_opts::to_string() const
{
  static const char* options[] = {
      "fourT", "twoT", "oneT", "halfT", "quarterT", "oneEighthT", "oneSixteenthT", "oneThirtySecondT"};
  return convert_enum_idx(options, 8, value, "pcch_cfg_s::nb_e_");
}
float pcch_cfg_s::nb_opts::to_number() const
{
  static const float options[] = {4.0, 2.0, 1.0, 0.5, 0.25, 0.125, 0.0625, 0.03125};
  return map_enum_number(options, 8, value, "pcch_cfg_s::nb_e_");
}
const char* pcch_cfg_s::nb_opts::to_number_string() const
{
  static const char* options[] = {"4", "2", "1", "0.5", "0.25", "1/8", "1/16", "1/32"};
  return convert_enum_idx(options, 8, value, "pcch_cfg_s::nb_e_");
}

// PCCH-Config-v1310 ::= SEQUENCE
SRSASN_CODE pcch_cfg_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(nb_v1310_present, 1));

  HANDLE_CODE(pack_integer(bref, paging_narrow_bands_r13, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(mpdcch_num_repeat_paging_r13.pack(bref));
  if (nb_v1310_present) {
    HANDLE_CODE(nb_v1310.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pcch_cfg_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(nb_v1310_present, 1));

  HANDLE_CODE(unpack_integer(paging_narrow_bands_r13, bref, (uint8_t)1u, (uint8_t)16u));
  HANDLE_CODE(mpdcch_num_repeat_paging_r13.unpack(bref));
  if (nb_v1310_present) {
    HANDLE_CODE(nb_v1310.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pcch_cfg_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("paging-narrowBands-r13", paging_narrow_bands_r13);
  j.write_str("mpdcch-NumRepetition-Paging-r13", mpdcch_num_repeat_paging_r13.to_string());
  if (nb_v1310_present) {
    j.write_str("nB-v1310", nb_v1310.to_string());
  }
  j.end_obj();
}

const char* pcch_cfg_v1310_s::mpdcch_num_repeat_paging_r13_opts::to_string() const
{
  static const char* options[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "r128", "r256"};
  return convert_enum_idx(options, 9, value, "pcch_cfg_v1310_s::mpdcch_num_repeat_paging_r13_e_");
}
uint16_t pcch_cfg_v1310_s::mpdcch_num_repeat_paging_r13_opts::to_number() const
{
  static const uint16_t options[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(options, 9, value, "pcch_cfg_v1310_s::mpdcch_num_repeat_paging_r13_e_");
}

const char* pcch_cfg_v1310_s::nb_v1310_opts::to_string() const
{
  static const char* options[] = {"one64thT", "one128thT", "one256thT"};
  return convert_enum_idx(options, 3, value, "pcch_cfg_v1310_s::nb_v1310_e_");
}
uint16_t pcch_cfg_v1310_s::nb_v1310_opts::to_number() const
{
  static const uint16_t options[] = {64, 128, 256};
  return map_enum_number(options, 3, value, "pcch_cfg_v1310_s::nb_v1310_e_");
}

// PDSCH-ConfigCommon ::= SEQUENCE
SRSASN_CODE pdsch_cfg_common_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, ref_sig_pwr, (int8_t)-60, (int8_t)50));
  HANDLE_CODE(pack_integer(bref, p_b, (uint8_t)0u, (uint8_t)3u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_common_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(ref_sig_pwr, bref, (int8_t)-60, (int8_t)50));
  HANDLE_CODE(unpack_integer(p_b, bref, (uint8_t)0u, (uint8_t)3u));

  return SRSASN_SUCCESS;
}
void pdsch_cfg_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("referenceSignalPower", ref_sig_pwr);
  j.write_int("p-b", p_b);
  j.end_obj();
}
bool pdsch_cfg_common_s::operator==(const pdsch_cfg_common_s& other) const
{
  return ref_sig_pwr == other.ref_sig_pwr and p_b == other.p_b;
}

// PDSCH-ConfigCommon-v1310 ::= SEQUENCE
SRSASN_CODE pdsch_cfg_common_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pdsch_max_num_repeat_cemode_a_r13_present, 1));
  HANDLE_CODE(bref.pack(pdsch_max_num_repeat_cemode_b_r13_present, 1));

  if (pdsch_max_num_repeat_cemode_a_r13_present) {
    HANDLE_CODE(pdsch_max_num_repeat_cemode_a_r13.pack(bref));
  }
  if (pdsch_max_num_repeat_cemode_b_r13_present) {
    HANDLE_CODE(pdsch_max_num_repeat_cemode_b_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pdsch_cfg_common_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pdsch_max_num_repeat_cemode_a_r13_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_max_num_repeat_cemode_b_r13_present, 1));

  if (pdsch_max_num_repeat_cemode_a_r13_present) {
    HANDLE_CODE(pdsch_max_num_repeat_cemode_a_r13.unpack(bref));
  }
  if (pdsch_max_num_repeat_cemode_b_r13_present) {
    HANDLE_CODE(pdsch_max_num_repeat_cemode_b_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pdsch_cfg_common_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pdsch_max_num_repeat_cemode_a_r13_present) {
    j.write_str("pdsch-maxNumRepetitionCEmodeA-r13", pdsch_max_num_repeat_cemode_a_r13.to_string());
  }
  if (pdsch_max_num_repeat_cemode_b_r13_present) {
    j.write_str("pdsch-maxNumRepetitionCEmodeB-r13", pdsch_max_num_repeat_cemode_b_r13.to_string());
  }
  j.end_obj();
}

const char* pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_a_r13_opts::to_string() const
{
  static const char* options[] = {"r16", "r32"};
  return convert_enum_idx(options, 2, value, "pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_a_r13_e_");
}
uint8_t pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_a_r13_opts::to_number() const
{
  static const uint8_t options[] = {16, 32};
  return map_enum_number(options, 2, value, "pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_a_r13_e_");
}

const char* pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_b_r13_opts::to_string() const
{
  static const char* options[] = {"r192", "r256", "r384", "r512", "r768", "r1024", "r1536", "r2048"};
  return convert_enum_idx(options, 8, value, "pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_b_r13_e_");
}
uint16_t pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_b_r13_opts::to_number() const
{
  static const uint16_t options[] = {192, 256, 384, 512, 768, 1024, 1536, 2048};
  return map_enum_number(options, 8, value, "pdsch_cfg_common_v1310_s::pdsch_max_num_repeat_cemode_b_r13_e_");
}

// PRACH-Config-v1430 ::= SEQUENCE
SRSASN_CODE prach_cfg_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, root_seq_idx_high_speed_r14, (uint16_t)0u, (uint16_t)837u));
  HANDLE_CODE(pack_integer(bref, zero_correlation_zone_cfg_high_speed_r14, (uint8_t)0u, (uint8_t)12u));
  HANDLE_CODE(pack_integer(bref, prach_cfg_idx_high_speed_r14, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(pack_integer(bref, prach_freq_offset_high_speed_r14, (uint8_t)0u, (uint8_t)94u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE prach_cfg_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(root_seq_idx_high_speed_r14, bref, (uint16_t)0u, (uint16_t)837u));
  HANDLE_CODE(unpack_integer(zero_correlation_zone_cfg_high_speed_r14, bref, (uint8_t)0u, (uint8_t)12u));
  HANDLE_CODE(unpack_integer(prach_cfg_idx_high_speed_r14, bref, (uint8_t)0u, (uint8_t)63u));
  HANDLE_CODE(unpack_integer(prach_freq_offset_high_speed_r14, bref, (uint8_t)0u, (uint8_t)94u));

  return SRSASN_SUCCESS;
}
void prach_cfg_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rootSequenceIndexHighSpeed-r14", root_seq_idx_high_speed_r14);
  j.write_int("zeroCorrelationZoneConfigHighSpeed-r14", zero_correlation_zone_cfg_high_speed_r14);
  j.write_int("prach-ConfigIndexHighSpeed-r14", prach_cfg_idx_high_speed_r14);
  j.write_int("prach-FreqOffsetHighSpeed-r14", prach_freq_offset_high_speed_r14);
  j.end_obj();
}
bool prach_cfg_v1430_s::operator==(const prach_cfg_v1430_s& other) const
{
  return root_seq_idx_high_speed_r14 == other.root_seq_idx_high_speed_r14 and
         zero_correlation_zone_cfg_high_speed_r14 == other.zero_correlation_zone_cfg_high_speed_r14 and
         prach_cfg_idx_high_speed_r14 == other.prach_cfg_idx_high_speed_r14 and
         prach_freq_offset_high_speed_r14 == other.prach_freq_offset_high_speed_r14;
}

// PRACH-ConfigSIB ::= SEQUENCE
SRSASN_CODE prach_cfg_sib_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, root_seq_idx, (uint16_t)0u, (uint16_t)837u));
  HANDLE_CODE(prach_cfg_info.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE prach_cfg_sib_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(root_seq_idx, bref, (uint16_t)0u, (uint16_t)837u));
  HANDLE_CODE(prach_cfg_info.unpack(bref));

  return SRSASN_SUCCESS;
}
void prach_cfg_sib_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rootSequenceIndex", root_seq_idx);
  j.write_fieldname("prach-ConfigInfo");
  prach_cfg_info.to_json(j);
  j.end_obj();
}

// PRACH-ConfigSIB-v1310 ::= SEQUENCE
SRSASN_CODE prach_cfg_sib_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mpdcch_start_sf_css_ra_r13_present, 1));
  HANDLE_CODE(bref.pack(prach_hop_offset_r13_present, 1));

  HANDLE_CODE(pack_dyn_seq_of(bref, rsrp_thress_prach_info_list_r13, 1, 3, integer_packer<uint8_t>(0, 97)));
  if (mpdcch_start_sf_css_ra_r13_present) {
    HANDLE_CODE(mpdcch_start_sf_css_ra_r13.pack(bref));
  }
  if (prach_hop_offset_r13_present) {
    HANDLE_CODE(pack_integer(bref, prach_hop_offset_r13, (uint8_t)0u, (uint8_t)94u));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, prach_params_list_ce_r13, 1, 4));

  return SRSASN_SUCCESS;
}
SRSASN_CODE prach_cfg_sib_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mpdcch_start_sf_css_ra_r13_present, 1));
  HANDLE_CODE(bref.unpack(prach_hop_offset_r13_present, 1));

  HANDLE_CODE(unpack_dyn_seq_of(rsrp_thress_prach_info_list_r13, bref, 1, 3, integer_packer<uint8_t>(0, 97)));
  if (mpdcch_start_sf_css_ra_r13_present) {
    HANDLE_CODE(mpdcch_start_sf_css_ra_r13.unpack(bref));
  }
  if (prach_hop_offset_r13_present) {
    HANDLE_CODE(unpack_integer(prach_hop_offset_r13, bref, (uint8_t)0u, (uint8_t)94u));
  }
  HANDLE_CODE(unpack_dyn_seq_of(prach_params_list_ce_r13, bref, 1, 4));

  return SRSASN_SUCCESS;
}
void prach_cfg_sib_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("rsrp-ThresholdsPrachInfoList-r13");
  for (const auto& e1 : rsrp_thress_prach_info_list_r13) {
    j.write_int(e1);
  }
  j.end_array();
  if (mpdcch_start_sf_css_ra_r13_present) {
    j.write_fieldname("mpdcch-startSF-CSS-RA-r13");
    mpdcch_start_sf_css_ra_r13.to_json(j);
  }
  if (prach_hop_offset_r13_present) {
    j.write_int("prach-HoppingOffset-r13", prach_hop_offset_r13);
  }
  j.start_array("prach-ParametersListCE-r13");
  for (const auto& e1 : prach_params_list_ce_r13) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

void prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::destroy_() {}
void prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::mpdcch_start_sf_css_ra_r13_c_(
    const prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::fdd_r13:
      c.init(other.c.get<fdd_r13_e_>());
      break;
    case types::tdd_r13:
      c.init(other.c.get<tdd_r13_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_");
  }
}
prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_& prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::operator=(
    const prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::fdd_r13:
      c.set(other.c.get<fdd_r13_e_>());
      break;
    case types::tdd_r13:
      c.set(other.c.get<tdd_r13_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_");
  }

  return *this;
}
prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_&
prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::set_fdd_r13()
{
  set(types::fdd_r13);
  return c.get<fdd_r13_e_>();
}
prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_&
prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::set_tdd_r13()
{
  set(types::tdd_r13);
  return c.get<tdd_r13_e_>();
}
void prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::fdd_r13:
      j.write_str("fdd-r13", c.get<fdd_r13_e_>().to_string());
      break;
    case types::tdd_r13:
      j.write_str("tdd-r13", c.get<tdd_r13_e_>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::fdd_r13:
      HANDLE_CODE(c.get<fdd_r13_e_>().pack(bref));
      break;
    case types::tdd_r13:
      HANDLE_CODE(c.get<tdd_r13_e_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::fdd_r13:
      HANDLE_CODE(c.get<fdd_r13_e_>().unpack(bref));
      break;
    case types::tdd_r13:
      HANDLE_CODE(c.get<tdd_r13_e_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_opts::to_string() const
{
  static const char* options[] = {"v1", "v1dot5", "v2", "v2dot5", "v4", "v5", "v8", "v10"};
  return convert_enum_idx(options, 8, value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
}
float prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_opts::to_number() const
{
  static const float options[] = {1.0, 1.5, 2.0, 2.5, 4.0, 5.0, 8.0, 10.0};
  return map_enum_number(options, 8, value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
}
const char* prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_opts::to_number_string() const
{
  static const char* options[] = {"1", "1.5", "2", "2.5", "4", "5", "8", "10"};
  return convert_enum_idx(options, 8, value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
}

const char* prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_opts::to_string() const
{
  static const char* options[] = {"v1", "v2", "v4", "v5", "v8", "v10", "v20", "spare"};
  return convert_enum_idx(options, 8, value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_");
}
uint8_t prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 5, 8, 10, 20};
  return map_enum_number(options, 7, value, "prach_cfg_sib_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_");
}

// PRACH-ConfigSIB-v1530 ::= SEQUENCE
SRSASN_CODE prach_cfg_sib_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_dyn_seq_of(bref, edt_prach_params_list_ce_r15, 1, 4));

  return SRSASN_SUCCESS;
}
SRSASN_CODE prach_cfg_sib_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_dyn_seq_of(edt_prach_params_list_ce_r15, bref, 1, 4));

  return SRSASN_SUCCESS;
}
void prach_cfg_sib_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.start_array("edt-PRACH-ParametersListCE-r15");
  for (const auto& e1 : edt_prach_params_list_ce_r15) {
    e1.to_json(j);
  }
  j.end_array();
  j.end_obj();
}

// PUCCH-ConfigCommon ::= SEQUENCE
SRSASN_CODE pucch_cfg_common_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(delta_pucch_shift.pack(bref));
  HANDLE_CODE(pack_integer(bref, nrb_cqi, (uint8_t)0u, (uint8_t)98u));
  HANDLE_CODE(pack_integer(bref, ncs_an, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(pack_integer(bref, n1_pucch_an, (uint16_t)0u, (uint16_t)2047u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_common_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(delta_pucch_shift.unpack(bref));
  HANDLE_CODE(unpack_integer(nrb_cqi, bref, (uint8_t)0u, (uint8_t)98u));
  HANDLE_CODE(unpack_integer(ncs_an, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(unpack_integer(n1_pucch_an, bref, (uint16_t)0u, (uint16_t)2047u));

  return SRSASN_SUCCESS;
}
void pucch_cfg_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("deltaPUCCH-Shift", delta_pucch_shift.to_string());
  j.write_int("nRB-CQI", nrb_cqi);
  j.write_int("nCS-AN", ncs_an);
  j.write_int("n1PUCCH-AN", n1_pucch_an);
  j.end_obj();
}
bool pucch_cfg_common_s::operator==(const pucch_cfg_common_s& other) const
{
  return delta_pucch_shift == other.delta_pucch_shift and nrb_cqi == other.nrb_cqi and ncs_an == other.ncs_an and
         n1_pucch_an == other.n1_pucch_an;
}

const char* pucch_cfg_common_s::delta_pucch_shift_opts::to_string() const
{
  static const char* options[] = {"ds1", "ds2", "ds3"};
  return convert_enum_idx(options, 3, value, "pucch_cfg_common_s::delta_pucch_shift_e_");
}
uint8_t pucch_cfg_common_s::delta_pucch_shift_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3};
  return map_enum_number(options, 3, value, "pucch_cfg_common_s::delta_pucch_shift_e_");
}

// PUCCH-ConfigCommon-v1310 ::= SEQUENCE
SRSASN_CODE pucch_cfg_common_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(n1_pucch_an_info_list_r13_present, 1));
  HANDLE_CODE(bref.pack(pucch_num_repeat_ce_msg4_level0_r13_present, 1));
  HANDLE_CODE(bref.pack(pucch_num_repeat_ce_msg4_level1_r13_present, 1));
  HANDLE_CODE(bref.pack(pucch_num_repeat_ce_msg4_level2_r13_present, 1));
  HANDLE_CODE(bref.pack(pucch_num_repeat_ce_msg4_level3_r13_present, 1));

  if (n1_pucch_an_info_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, n1_pucch_an_info_list_r13, 1, 4, integer_packer<uint16_t>(0, 2047)));
  }
  if (pucch_num_repeat_ce_msg4_level0_r13_present) {
    HANDLE_CODE(pucch_num_repeat_ce_msg4_level0_r13.pack(bref));
  }
  if (pucch_num_repeat_ce_msg4_level1_r13_present) {
    HANDLE_CODE(pucch_num_repeat_ce_msg4_level1_r13.pack(bref));
  }
  if (pucch_num_repeat_ce_msg4_level2_r13_present) {
    HANDLE_CODE(pucch_num_repeat_ce_msg4_level2_r13.pack(bref));
  }
  if (pucch_num_repeat_ce_msg4_level3_r13_present) {
    HANDLE_CODE(pucch_num_repeat_ce_msg4_level3_r13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_common_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(n1_pucch_an_info_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(pucch_num_repeat_ce_msg4_level0_r13_present, 1));
  HANDLE_CODE(bref.unpack(pucch_num_repeat_ce_msg4_level1_r13_present, 1));
  HANDLE_CODE(bref.unpack(pucch_num_repeat_ce_msg4_level2_r13_present, 1));
  HANDLE_CODE(bref.unpack(pucch_num_repeat_ce_msg4_level3_r13_present, 1));

  if (n1_pucch_an_info_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(n1_pucch_an_info_list_r13, bref, 1, 4, integer_packer<uint16_t>(0, 2047)));
  }
  if (pucch_num_repeat_ce_msg4_level0_r13_present) {
    HANDLE_CODE(pucch_num_repeat_ce_msg4_level0_r13.unpack(bref));
  }
  if (pucch_num_repeat_ce_msg4_level1_r13_present) {
    HANDLE_CODE(pucch_num_repeat_ce_msg4_level1_r13.unpack(bref));
  }
  if (pucch_num_repeat_ce_msg4_level2_r13_present) {
    HANDLE_CODE(pucch_num_repeat_ce_msg4_level2_r13.unpack(bref));
  }
  if (pucch_num_repeat_ce_msg4_level3_r13_present) {
    HANDLE_CODE(pucch_num_repeat_ce_msg4_level3_r13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pucch_cfg_common_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (n1_pucch_an_info_list_r13_present) {
    j.start_array("n1PUCCH-AN-InfoList-r13");
    for (const auto& e1 : n1_pucch_an_info_list_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (pucch_num_repeat_ce_msg4_level0_r13_present) {
    j.write_str("pucch-NumRepetitionCE-Msg4-Level0-r13", pucch_num_repeat_ce_msg4_level0_r13.to_string());
  }
  if (pucch_num_repeat_ce_msg4_level1_r13_present) {
    j.write_str("pucch-NumRepetitionCE-Msg4-Level1-r13", pucch_num_repeat_ce_msg4_level1_r13.to_string());
  }
  if (pucch_num_repeat_ce_msg4_level2_r13_present) {
    j.write_str("pucch-NumRepetitionCE-Msg4-Level2-r13", pucch_num_repeat_ce_msg4_level2_r13.to_string());
  }
  if (pucch_num_repeat_ce_msg4_level3_r13_present) {
    j.write_str("pucch-NumRepetitionCE-Msg4-Level3-r13", pucch_num_repeat_ce_msg4_level3_r13.to_string());
  }
  j.end_obj();
}

const char* pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level0_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "n8"};
  return convert_enum_idx(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level0_r13_e_");
}
uint8_t pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level0_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level0_r13_e_");
}

const char* pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level1_r13_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "n8"};
  return convert_enum_idx(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level1_r13_e_");
}
uint8_t pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level1_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8};
  return map_enum_number(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level1_r13_e_");
}

const char* pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level2_r13_opts::to_string() const
{
  static const char* options[] = {"n4", "n8", "n16", "n32"};
  return convert_enum_idx(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level2_r13_e_");
}
uint8_t pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level2_r13_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 16, 32};
  return map_enum_number(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level2_r13_e_");
}

const char* pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level3_r13_opts::to_string() const
{
  static const char* options[] = {"n4", "n8", "n16", "n32"};
  return convert_enum_idx(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level3_r13_e_");
}
uint8_t pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level3_r13_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 16, 32};
  return map_enum_number(options, 4, value, "pucch_cfg_common_v1310_s::pucch_num_repeat_ce_msg4_level3_r13_e_");
}

// PUCCH-ConfigCommon-v1430 ::= SEQUENCE
SRSASN_CODE pucch_cfg_common_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pucch_num_repeat_ce_msg4_level3_r14_present, 1));

  if (pucch_num_repeat_ce_msg4_level3_r14_present) {
    HANDLE_CODE(pucch_num_repeat_ce_msg4_level3_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pucch_cfg_common_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pucch_num_repeat_ce_msg4_level3_r14_present, 1));

  if (pucch_num_repeat_ce_msg4_level3_r14_present) {
    HANDLE_CODE(pucch_num_repeat_ce_msg4_level3_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void pucch_cfg_common_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pucch_num_repeat_ce_msg4_level3_r14_present) {
    j.write_str("pucch-NumRepetitionCE-Msg4-Level3-r14", pucch_num_repeat_ce_msg4_level3_r14.to_string());
  }
  j.end_obj();
}

const char* pucch_cfg_common_v1430_s::pucch_num_repeat_ce_msg4_level3_r14_opts::to_string() const
{
  static const char* options[] = {"n64", "n128"};
  return convert_enum_idx(options, 2, value, "pucch_cfg_common_v1430_s::pucch_num_repeat_ce_msg4_level3_r14_e_");
}
uint8_t pucch_cfg_common_v1430_s::pucch_num_repeat_ce_msg4_level3_r14_opts::to_number() const
{
  static const uint8_t options[] = {64, 128};
  return map_enum_number(options, 2, value, "pucch_cfg_common_v1430_s::pucch_num_repeat_ce_msg4_level3_r14_e_");
}

// PUSCH-ConfigCommon ::= SEQUENCE
SRSASN_CODE pusch_cfg_common_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, pusch_cfg_basic.n_sb, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(pusch_cfg_basic.hop_mode.pack(bref));
  HANDLE_CODE(pack_integer(bref, pusch_cfg_basic.pusch_hop_offset, (uint8_t)0u, (uint8_t)98u));
  HANDLE_CODE(bref.pack(pusch_cfg_basic.enable64_qam, 1));
  HANDLE_CODE(ul_ref_sigs_pusch.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_common_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(pusch_cfg_basic.n_sb, bref, (uint8_t)1u, (uint8_t)4u));
  HANDLE_CODE(pusch_cfg_basic.hop_mode.unpack(bref));
  HANDLE_CODE(unpack_integer(pusch_cfg_basic.pusch_hop_offset, bref, (uint8_t)0u, (uint8_t)98u));
  HANDLE_CODE(bref.unpack(pusch_cfg_basic.enable64_qam, 1));
  HANDLE_CODE(ul_ref_sigs_pusch.unpack(bref));

  return SRSASN_SUCCESS;
}
void pusch_cfg_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("pusch-ConfigBasic");
  j.start_obj();
  j.write_int("n-SB", pusch_cfg_basic.n_sb);
  j.write_str("hoppingMode", pusch_cfg_basic.hop_mode.to_string());
  j.write_int("pusch-HoppingOffset", pusch_cfg_basic.pusch_hop_offset);
  j.write_bool("enable64QAM", pusch_cfg_basic.enable64_qam);
  j.end_obj();
  j.write_fieldname("ul-ReferenceSignalsPUSCH");
  ul_ref_sigs_pusch.to_json(j);
  j.end_obj();
}
bool pusch_cfg_common_s::operator==(const pusch_cfg_common_s& other) const
{
  return pusch_cfg_basic.n_sb == other.pusch_cfg_basic.n_sb and
         pusch_cfg_basic.hop_mode == other.pusch_cfg_basic.hop_mode and
         pusch_cfg_basic.pusch_hop_offset == other.pusch_cfg_basic.pusch_hop_offset and
         pusch_cfg_basic.enable64_qam == other.pusch_cfg_basic.enable64_qam and
         ul_ref_sigs_pusch == other.ul_ref_sigs_pusch;
}

const char* pusch_cfg_common_s::pusch_cfg_basic_s_::hop_mode_opts::to_string() const
{
  static const char* options[] = {"interSubFrame", "intraAndInterSubFrame"};
  return convert_enum_idx(options, 2, value, "pusch_cfg_common_s::pusch_cfg_basic_s_::hop_mode_e_");
}

// PUSCH-ConfigCommon-v1270 ::= SEQUENCE
SRSASN_CODE pusch_cfg_common_v1270_s::pack(bit_ref& bref) const
{
  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_common_v1270_s::unpack(cbit_ref& bref)
{
  return SRSASN_SUCCESS;
}
void pusch_cfg_common_v1270_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("enable64QAM-v1270", "true");
  j.end_obj();
}
bool pusch_cfg_common_v1270_s::operator==(const pusch_cfg_common_v1270_s& other) const
{
  return true;
}

// PUSCH-ConfigCommon-v1310 ::= SEQUENCE
SRSASN_CODE pusch_cfg_common_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(pusch_max_num_repeat_cemode_a_r13_present, 1));
  HANDLE_CODE(bref.pack(pusch_max_num_repeat_cemode_b_r13_present, 1));
  HANDLE_CODE(bref.pack(pusch_hop_offset_v1310_present, 1));

  if (pusch_max_num_repeat_cemode_a_r13_present) {
    HANDLE_CODE(pusch_max_num_repeat_cemode_a_r13.pack(bref));
  }
  if (pusch_max_num_repeat_cemode_b_r13_present) {
    HANDLE_CODE(pusch_max_num_repeat_cemode_b_r13.pack(bref));
  }
  if (pusch_hop_offset_v1310_present) {
    HANDLE_CODE(pack_integer(bref, pusch_hop_offset_v1310, (uint8_t)1u, (uint8_t)16u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE pusch_cfg_common_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(pusch_max_num_repeat_cemode_a_r13_present, 1));
  HANDLE_CODE(bref.unpack(pusch_max_num_repeat_cemode_b_r13_present, 1));
  HANDLE_CODE(bref.unpack(pusch_hop_offset_v1310_present, 1));

  if (pusch_max_num_repeat_cemode_a_r13_present) {
    HANDLE_CODE(pusch_max_num_repeat_cemode_a_r13.unpack(bref));
  }
  if (pusch_max_num_repeat_cemode_b_r13_present) {
    HANDLE_CODE(pusch_max_num_repeat_cemode_b_r13.unpack(bref));
  }
  if (pusch_hop_offset_v1310_present) {
    HANDLE_CODE(unpack_integer(pusch_hop_offset_v1310, bref, (uint8_t)1u, (uint8_t)16u));
  }

  return SRSASN_SUCCESS;
}
void pusch_cfg_common_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (pusch_max_num_repeat_cemode_a_r13_present) {
    j.write_str("pusch-maxNumRepetitionCEmodeA-r13", pusch_max_num_repeat_cemode_a_r13.to_string());
  }
  if (pusch_max_num_repeat_cemode_b_r13_present) {
    j.write_str("pusch-maxNumRepetitionCEmodeB-r13", pusch_max_num_repeat_cemode_b_r13.to_string());
  }
  if (pusch_hop_offset_v1310_present) {
    j.write_int("pusch-HoppingOffset-v1310", pusch_hop_offset_v1310);
  }
  j.end_obj();
}

const char* pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_a_r13_opts::to_string() const
{
  static const char* options[] = {"r8", "r16", "r32"};
  return convert_enum_idx(options, 3, value, "pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_a_r13_e_");
}
uint8_t pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_a_r13_opts::to_number() const
{
  static const uint8_t options[] = {8, 16, 32};
  return map_enum_number(options, 3, value, "pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_a_r13_e_");
}

const char* pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_b_r13_opts::to_string() const
{
  static const char* options[] = {"r192", "r256", "r384", "r512", "r768", "r1024", "r1536", "r2048"};
  return convert_enum_idx(options, 8, value, "pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_b_r13_e_");
}
uint16_t pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_b_r13_opts::to_number() const
{
  static const uint16_t options[] = {192, 256, 384, 512, 768, 1024, 1536, 2048};
  return map_enum_number(options, 8, value, "pusch_cfg_common_v1310_s::pusch_max_num_repeat_cemode_b_r13_e_");
}

// RACH-ConfigCommon ::= SEQUENCE
SRSASN_CODE rach_cfg_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(preamb_info.preambs_group_a_cfg_present, 1));
  HANDLE_CODE(preamb_info.nof_ra_preambs.pack(bref));
  if (preamb_info.preambs_group_a_cfg_present) {
    bref.pack(preamb_info.preambs_group_a_cfg.ext, 1);
    HANDLE_CODE(preamb_info.preambs_group_a_cfg.size_of_ra_preambs_group_a.pack(bref));
    HANDLE_CODE(preamb_info.preambs_group_a_cfg.msg_size_group_a.pack(bref));
    HANDLE_CODE(preamb_info.preambs_group_a_cfg.msg_pwr_offset_group_b.pack(bref));
  }
  HANDLE_CODE(pwr_ramp_params.pack(bref));
  HANDLE_CODE(ra_supervision_info.preamb_trans_max.pack(bref));
  HANDLE_CODE(ra_supervision_info.ra_resp_win_size.pack(bref));
  HANDLE_CODE(ra_supervision_info.mac_contention_resolution_timer.pack(bref));
  HANDLE_CODE(pack_integer(bref, max_harq_msg3_tx, (uint8_t)1u, (uint8_t)8u));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= preamb_trans_max_ce_r13_present;
    group_flags[0] |= rach_ce_level_info_list_r13.is_present();
    group_flags[1] |= edt_small_tbs_subset_r15_present;
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(preamb_trans_max_ce_r13_present, 1));
      HANDLE_CODE(bref.pack(rach_ce_level_info_list_r13.is_present(), 1));
      if (preamb_trans_max_ce_r13_present) {
        HANDLE_CODE(preamb_trans_max_ce_r13.pack(bref));
      }
      if (rach_ce_level_info_list_r13.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *rach_ce_level_info_list_r13, 1, 4));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(edt_small_tbs_subset_r15_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rach_cfg_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(preamb_info.preambs_group_a_cfg_present, 1));
  HANDLE_CODE(preamb_info.nof_ra_preambs.unpack(bref));
  if (preamb_info.preambs_group_a_cfg_present) {
    bref.unpack(preamb_info.preambs_group_a_cfg.ext, 1);
    HANDLE_CODE(preamb_info.preambs_group_a_cfg.size_of_ra_preambs_group_a.unpack(bref));
    HANDLE_CODE(preamb_info.preambs_group_a_cfg.msg_size_group_a.unpack(bref));
    HANDLE_CODE(preamb_info.preambs_group_a_cfg.msg_pwr_offset_group_b.unpack(bref));
  }
  HANDLE_CODE(pwr_ramp_params.unpack(bref));
  HANDLE_CODE(ra_supervision_info.preamb_trans_max.unpack(bref));
  HANDLE_CODE(ra_supervision_info.ra_resp_win_size.unpack(bref));
  HANDLE_CODE(ra_supervision_info.mac_contention_resolution_timer.unpack(bref));
  HANDLE_CODE(unpack_integer(max_harq_msg3_tx, bref, (uint8_t)1u, (uint8_t)8u));

  if (ext) {
    ext_groups_unpacker_guard group_flags(2);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(preamb_trans_max_ce_r13_present, 1));
      bool rach_ce_level_info_list_r13_present;
      HANDLE_CODE(bref.unpack(rach_ce_level_info_list_r13_present, 1));
      rach_ce_level_info_list_r13.set_present(rach_ce_level_info_list_r13_present);
      if (preamb_trans_max_ce_r13_present) {
        HANDLE_CODE(preamb_trans_max_ce_r13.unpack(bref));
      }
      if (rach_ce_level_info_list_r13.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*rach_ce_level_info_list_r13, bref, 1, 4));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(edt_small_tbs_subset_r15_present, 1));
    }
  }
  return SRSASN_SUCCESS;
}
void rach_cfg_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("preambleInfo");
  j.start_obj();
  j.write_str("numberOfRA-Preambles", preamb_info.nof_ra_preambs.to_string());
  if (preamb_info.preambs_group_a_cfg_present) {
    j.write_fieldname("preamblesGroupAConfig");
    j.start_obj();
    j.write_str("sizeOfRA-PreamblesGroupA", preamb_info.preambs_group_a_cfg.size_of_ra_preambs_group_a.to_string());
    j.write_str("messageSizeGroupA", preamb_info.preambs_group_a_cfg.msg_size_group_a.to_string());
    j.write_str("messagePowerOffsetGroupB", preamb_info.preambs_group_a_cfg.msg_pwr_offset_group_b.to_string());
    j.end_obj();
  }
  j.end_obj();
  j.write_fieldname("powerRampingParameters");
  pwr_ramp_params.to_json(j);
  j.write_fieldname("ra-SupervisionInfo");
  j.start_obj();
  j.write_str("preambleTransMax", ra_supervision_info.preamb_trans_max.to_string());
  j.write_str("ra-ResponseWindowSize", ra_supervision_info.ra_resp_win_size.to_string());
  j.write_str("mac-ContentionResolutionTimer", ra_supervision_info.mac_contention_resolution_timer.to_string());
  j.end_obj();
  j.write_int("maxHARQ-Msg3Tx", max_harq_msg3_tx);
  if (ext) {
    if (preamb_trans_max_ce_r13_present) {
      j.write_str("preambleTransMax-CE-r13", preamb_trans_max_ce_r13.to_string());
    }
    if (rach_ce_level_info_list_r13.is_present()) {
      j.start_array("rach-CE-LevelInfoList-r13");
      for (const auto& e1 : *rach_ce_level_info_list_r13) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (edt_small_tbs_subset_r15_present) {
      j.write_str("edt-SmallTBS-Subset-r15", "true");
    }
  }
  j.end_obj();
}

const char* rach_cfg_common_s::preamb_info_s_::nof_ra_preambs_opts::to_string() const
{
  static const char* options[] = {
      "n4", "n8", "n12", "n16", "n20", "n24", "n28", "n32", "n36", "n40", "n44", "n48", "n52", "n56", "n60", "n64"};
  return convert_enum_idx(options, 16, value, "rach_cfg_common_s::preamb_info_s_::nof_ra_preambs_e_");
}
uint8_t rach_cfg_common_s::preamb_info_s_::nof_ra_preambs_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64};
  return map_enum_number(options, 16, value, "rach_cfg_common_s::preamb_info_s_::nof_ra_preambs_e_");
}

const char*
rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::size_of_ra_preambs_group_a_opts::to_string() const
{
  static const char* options[] = {
      "n4", "n8", "n12", "n16", "n20", "n24", "n28", "n32", "n36", "n40", "n44", "n48", "n52", "n56", "n60"};
  return convert_enum_idx(
      options, 15, value, "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::size_of_ra_preambs_group_a_e_");
}
uint8_t rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::size_of_ra_preambs_group_a_opts::to_number() const
{
  static const uint8_t options[] = {4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60};
  return map_enum_number(
      options, 15, value, "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::size_of_ra_preambs_group_a_e_");
}

const char* rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_size_group_a_opts::to_string() const
{
  static const char* options[] = {"b56", "b144", "b208", "b256"};
  return convert_enum_idx(
      options, 4, value, "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_size_group_a_e_");
}
uint16_t rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_size_group_a_opts::to_number() const
{
  static const uint16_t options[] = {56, 144, 208, 256};
  return map_enum_number(
      options, 4, value, "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_size_group_a_e_");
}

const char* rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_pwr_offset_group_b_opts::to_string() const
{
  static const char* options[] = {"minusinfinity", "dB0", "dB5", "dB8", "dB10", "dB12", "dB15", "dB18"};
  return convert_enum_idx(
      options, 8, value, "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_pwr_offset_group_b_e_");
}
int8_t rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_pwr_offset_group_b_opts::to_number() const
{
  static const int8_t options[] = {-1, 0, 5, 8, 10, 12, 15, 18};
  return map_enum_number(
      options, 8, value, "rach_cfg_common_s::preamb_info_s_::preambs_group_a_cfg_s_::msg_pwr_offset_group_b_e_");
}

const char* rach_cfg_common_s::ra_supervision_info_s_::ra_resp_win_size_opts::to_string() const
{
  static const char* options[] = {"sf2", "sf3", "sf4", "sf5", "sf6", "sf7", "sf8", "sf10"};
  return convert_enum_idx(options, 8, value, "rach_cfg_common_s::ra_supervision_info_s_::ra_resp_win_size_e_");
}
uint8_t rach_cfg_common_s::ra_supervision_info_s_::ra_resp_win_size_opts::to_number() const
{
  static const uint8_t options[] = {2, 3, 4, 5, 6, 7, 8, 10};
  return map_enum_number(options, 8, value, "rach_cfg_common_s::ra_supervision_info_s_::ra_resp_win_size_e_");
}

const char* rach_cfg_common_s::ra_supervision_info_s_::mac_contention_resolution_timer_opts::to_string() const
{
  static const char* options[] = {"sf8", "sf16", "sf24", "sf32", "sf40", "sf48", "sf56", "sf64"};
  return convert_enum_idx(
      options, 8, value, "rach_cfg_common_s::ra_supervision_info_s_::mac_contention_resolution_timer_e_");
}
uint8_t rach_cfg_common_s::ra_supervision_info_s_::mac_contention_resolution_timer_opts::to_number() const
{
  static const uint8_t options[] = {8, 16, 24, 32, 40, 48, 56, 64};
  return map_enum_number(
      options, 8, value, "rach_cfg_common_s::ra_supervision_info_s_::mac_contention_resolution_timer_e_");
}

// RACH-ConfigCommon-v1250 ::= SEQUENCE
SRSASN_CODE rach_cfg_common_v1250_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(tx_fail_params_r12.conn_est_fail_offset_r12_present, 1));
  HANDLE_CODE(tx_fail_params_r12.conn_est_fail_count_r12.pack(bref));
  HANDLE_CODE(tx_fail_params_r12.conn_est_fail_offset_validity_r12.pack(bref));
  if (tx_fail_params_r12.conn_est_fail_offset_r12_present) {
    HANDLE_CODE(pack_integer(bref, tx_fail_params_r12.conn_est_fail_offset_r12, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE rach_cfg_common_v1250_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(tx_fail_params_r12.conn_est_fail_offset_r12_present, 1));
  HANDLE_CODE(tx_fail_params_r12.conn_est_fail_count_r12.unpack(bref));
  HANDLE_CODE(tx_fail_params_r12.conn_est_fail_offset_validity_r12.unpack(bref));
  if (tx_fail_params_r12.conn_est_fail_offset_r12_present) {
    HANDLE_CODE(unpack_integer(tx_fail_params_r12.conn_est_fail_offset_r12, bref, (uint8_t)0u, (uint8_t)15u));
  }

  return SRSASN_SUCCESS;
}
void rach_cfg_common_v1250_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("txFailParams-r12");
  j.start_obj();
  j.write_str("connEstFailCount-r12", tx_fail_params_r12.conn_est_fail_count_r12.to_string());
  j.write_str("connEstFailOffsetValidity-r12", tx_fail_params_r12.conn_est_fail_offset_validity_r12.to_string());
  if (tx_fail_params_r12.conn_est_fail_offset_r12_present) {
    j.write_int("connEstFailOffset-r12", tx_fail_params_r12.conn_est_fail_offset_r12);
  }
  j.end_obj();
  j.end_obj();
}

const char* rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_count_r12_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n3", "n4"};
  return convert_enum_idx(
      options, 4, value, "rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_count_r12_e_");
}
uint8_t rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_count_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 3, 4};
  return map_enum_number(
      options, 4, value, "rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_count_r12_e_");
}

const char* rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_offset_validity_r12_opts::to_string() const
{
  static const char* options[] = {"s30", "s60", "s120", "s240", "s300", "s420", "s600", "s900"};
  return convert_enum_idx(
      options, 8, value, "rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_offset_validity_r12_e_");
}
uint16_t rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_offset_validity_r12_opts::to_number() const
{
  static const uint16_t options[] = {30, 60, 120, 240, 300, 420, 600, 900};
  return map_enum_number(
      options, 8, value, "rach_cfg_common_v1250_s::tx_fail_params_r12_s_::conn_est_fail_offset_validity_r12_e_");
}

// RSS-Config-r15 ::= SEQUENCE
SRSASN_CODE rss_cfg_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(dur_r15.pack(bref));
  HANDLE_CODE(pack_integer(bref, freq_location_r15, (uint8_t)0u, (uint8_t)98u));
  HANDLE_CODE(periodicity_r15.pack(bref));
  HANDLE_CODE(pwr_boost_r15.pack(bref));
  HANDLE_CODE(pack_integer(bref, time_offset_r15, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rss_cfg_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(dur_r15.unpack(bref));
  HANDLE_CODE(unpack_integer(freq_location_r15, bref, (uint8_t)0u, (uint8_t)98u));
  HANDLE_CODE(periodicity_r15.unpack(bref));
  HANDLE_CODE(pwr_boost_r15.unpack(bref));
  HANDLE_CODE(unpack_integer(time_offset_r15, bref, (uint8_t)0u, (uint8_t)31u));

  return SRSASN_SUCCESS;
}
void rss_cfg_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("duration-r15", dur_r15.to_string());
  j.write_int("freqLocation-r15", freq_location_r15);
  j.write_str("periodicity-r15", periodicity_r15.to_string());
  j.write_str("powerBoost-r15", pwr_boost_r15.to_string());
  j.write_int("timeOffset-r15", time_offset_r15);
  j.end_obj();
}

const char* rss_cfg_r15_s::dur_r15_opts::to_string() const
{
  static const char* options[] = {"sf8", "sf16", "sf32", "sf40"};
  return convert_enum_idx(options, 4, value, "rss_cfg_r15_s::dur_r15_e_");
}
uint8_t rss_cfg_r15_s::dur_r15_opts::to_number() const
{
  static const uint8_t options[] = {8, 16, 32, 40};
  return map_enum_number(options, 4, value, "rss_cfg_r15_s::dur_r15_e_");
}

const char* rss_cfg_r15_s::periodicity_r15_opts::to_string() const
{
  static const char* options[] = {"ms160", "ms320", "ms640", "ms1280"};
  return convert_enum_idx(options, 4, value, "rss_cfg_r15_s::periodicity_r15_e_");
}
uint16_t rss_cfg_r15_s::periodicity_r15_opts::to_number() const
{
  static const uint16_t options[] = {160, 320, 640, 1280};
  return map_enum_number(options, 4, value, "rss_cfg_r15_s::periodicity_r15_e_");
}

const char* rss_cfg_r15_s::pwr_boost_r15_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB3", "dB4dot8", "dB6"};
  return convert_enum_idx(options, 4, value, "rss_cfg_r15_s::pwr_boost_r15_e_");
}
float rss_cfg_r15_s::pwr_boost_r15_opts::to_number() const
{
  static const float options[] = {0.0, 3.0, 4.8, 6.0};
  return map_enum_number(options, 4, value, "rss_cfg_r15_s::pwr_boost_r15_e_");
}
const char* rss_cfg_r15_s::pwr_boost_r15_opts::to_number_string() const
{
  static const char* options[] = {"0", "3", "4.8", "6"};
  return convert_enum_idx(options, 4, value, "rss_cfg_r15_s::pwr_boost_r15_e_");
}

// SoundingRS-UL-ConfigCommon ::= CHOICE
void srs_ul_cfg_common_c::set(types::options e)
{
  type_ = e;
}
void srs_ul_cfg_common_c::set_release()
{
  set(types::release);
}
srs_ul_cfg_common_c::setup_s_& srs_ul_cfg_common_c::set_setup()
{
  set(types::setup);
  return c;
}
void srs_ul_cfg_common_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      j.write_fieldname("setup");
      j.start_obj();
      j.write_str("srs-BandwidthConfig", c.srs_bw_cfg.to_string());
      j.write_str("srs-SubframeConfig", c.srs_sf_cfg.to_string());
      j.write_bool("ackNackSRS-SimultaneousTransmission", c.ack_nack_srs_simul_tx);
      if (c.srs_max_up_pts_present) {
        j.write_str("srs-MaxUpPts", "true");
      }
      j.end_obj();
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_common_c");
  }
  j.end_obj();
}
SRSASN_CODE srs_ul_cfg_common_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.pack(c.srs_max_up_pts_present, 1));
      HANDLE_CODE(c.srs_bw_cfg.pack(bref));
      HANDLE_CODE(c.srs_sf_cfg.pack(bref));
      HANDLE_CODE(bref.pack(c.ack_nack_srs_simul_tx, 1));
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_common_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE srs_ul_cfg_common_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::release:
      break;
    case types::setup:
      HANDLE_CODE(bref.unpack(c.srs_max_up_pts_present, 1));
      HANDLE_CODE(c.srs_bw_cfg.unpack(bref));
      HANDLE_CODE(c.srs_sf_cfg.unpack(bref));
      HANDLE_CODE(bref.unpack(c.ack_nack_srs_simul_tx, 1));
      break;
    default:
      log_invalid_choice_id(type_, "srs_ul_cfg_common_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool srs_ul_cfg_common_c::operator==(const srs_ul_cfg_common_c& other) const
{
  return type() == other.type() and c.srs_bw_cfg == other.c.srs_bw_cfg and c.srs_sf_cfg == other.c.srs_sf_cfg and
         c.ack_nack_srs_simul_tx == other.c.ack_nack_srs_simul_tx and
         c.srs_max_up_pts_present == other.c.srs_max_up_pts_present;
}

const char* srs_ul_cfg_common_c::setup_s_::srs_bw_cfg_opts::to_string() const
{
  static const char* options[] = {"bw0", "bw1", "bw2", "bw3", "bw4", "bw5", "bw6", "bw7"};
  return convert_enum_idx(options, 8, value, "srs_ul_cfg_common_c::setup_s_::srs_bw_cfg_e_");
}
uint8_t srs_ul_cfg_common_c::setup_s_::srs_bw_cfg_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7};
  return map_enum_number(options, 8, value, "srs_ul_cfg_common_c::setup_s_::srs_bw_cfg_e_");
}

const char* srs_ul_cfg_common_c::setup_s_::srs_sf_cfg_opts::to_string() const
{
  static const char* options[] = {"sc0",
                                  "sc1",
                                  "sc2",
                                  "sc3",
                                  "sc4",
                                  "sc5",
                                  "sc6",
                                  "sc7",
                                  "sc8",
                                  "sc9",
                                  "sc10",
                                  "sc11",
                                  "sc12",
                                  "sc13",
                                  "sc14",
                                  "sc15"};
  return convert_enum_idx(options, 16, value, "srs_ul_cfg_common_c::setup_s_::srs_sf_cfg_e_");
}
uint8_t srs_ul_cfg_common_c::setup_s_::srs_sf_cfg_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  return map_enum_number(options, 16, value, "srs_ul_cfg_common_c::setup_s_::srs_sf_cfg_e_");
}

// UL-CyclicPrefixLength ::= ENUMERATED
const char* ul_cp_len_opts::to_string() const
{
  static const char* options[] = {"len1", "len2"};
  return convert_enum_idx(options, 2, value, "ul_cp_len_e");
}
uint8_t ul_cp_len_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "ul_cp_len_e");
}

// UplinkPowerControlCommon ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_common_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, p0_nominal_pusch, (int8_t)-126, (int8_t)24));
  HANDLE_CODE(alpha.pack(bref));
  HANDLE_CODE(pack_integer(bref, p0_nominal_pucch, (int8_t)-127, (int8_t)-96));
  HANDLE_CODE(delta_flist_pucch.pack(bref));
  HANDLE_CODE(pack_integer(bref, delta_preamb_msg3, (int8_t)-1, (int8_t)6));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_common_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(p0_nominal_pusch, bref, (int8_t)-126, (int8_t)24));
  HANDLE_CODE(alpha.unpack(bref));
  HANDLE_CODE(unpack_integer(p0_nominal_pucch, bref, (int8_t)-127, (int8_t)-96));
  HANDLE_CODE(delta_flist_pucch.unpack(bref));
  HANDLE_CODE(unpack_integer(delta_preamb_msg3, bref, (int8_t)-1, (int8_t)6));

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("p0-NominalPUSCH", p0_nominal_pusch);
  j.write_str("alpha", alpha.to_string());
  j.write_int("p0-NominalPUCCH", p0_nominal_pucch);
  j.write_fieldname("deltaFList-PUCCH");
  delta_flist_pucch.to_json(j);
  j.write_int("deltaPreambleMsg3", delta_preamb_msg3);
  j.end_obj();
}

// UplinkPowerControlCommon-v1020 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_common_v1020_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(delta_f_pucch_format3_r10.pack(bref));
  HANDLE_CODE(delta_f_pucch_format1b_cs_r10.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_common_v1020_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(delta_f_pucch_format3_r10.unpack(bref));
  HANDLE_CODE(delta_f_pucch_format1b_cs_r10.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_common_v1020_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("deltaF-PUCCH-Format3-r10", delta_f_pucch_format3_r10.to_string());
  j.write_str("deltaF-PUCCH-Format1bCS-r10", delta_f_pucch_format1b_cs_r10.to_string());
  j.end_obj();
}

const char* ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format3_r10_opts::to_string() const
{
  static const char* options[] = {
      "deltaF-1", "deltaF0", "deltaF1", "deltaF2", "deltaF3", "deltaF4", "deltaF5", "deltaF6"};
  return convert_enum_idx(options, 8, value, "ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format3_r10_e_");
}
int8_t ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format3_r10_opts::to_number() const
{
  static const int8_t options[] = {-1, 0, 1, 2, 3, 4, 5, 6};
  return map_enum_number(options, 8, value, "ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format3_r10_e_");
}

const char* ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format1b_cs_r10_opts::to_string() const
{
  static const char* options[] = {"deltaF1", "deltaF2", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format1b_cs_r10_e_");
}
uint8_t ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format1b_cs_r10_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "ul_pwr_ctrl_common_v1020_s::delta_f_pucch_format1b_cs_r10_e_");
}

// UplinkPowerControlCommon-v1530 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_common_v1530_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(delta_flist_spucch_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_common_v1530_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(delta_flist_spucch_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_common_v1530_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("deltaFList-SPUCCH-r15");
  delta_flist_spucch_r15.to_json(j);
  j.end_obj();
}
bool ul_pwr_ctrl_common_v1530_s::operator==(const ul_pwr_ctrl_common_v1530_s& other) const
{
  return delta_flist_spucch_r15 == other.delta_flist_spucch_r15;
}

// WUS-Config-r15 ::= SEQUENCE
SRSASN_CODE wus_cfg_r15_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(num_pos_r15_present, 1));
  HANDLE_CODE(bref.pack(time_offset_e_drx_long_r15_present, 1));

  HANDLE_CODE(max_dur_factor_r15.pack(bref));
  if (num_pos_r15_present) {
    HANDLE_CODE(num_pos_r15.pack(bref));
  }
  HANDLE_CODE(freq_location_r15.pack(bref));
  HANDLE_CODE(time_offset_drx_r15.pack(bref));
  HANDLE_CODE(time_offset_e_drx_short_r15.pack(bref));
  if (time_offset_e_drx_long_r15_present) {
    HANDLE_CODE(time_offset_e_drx_long_r15.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE wus_cfg_r15_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(num_pos_r15_present, 1));
  HANDLE_CODE(bref.unpack(time_offset_e_drx_long_r15_present, 1));

  HANDLE_CODE(max_dur_factor_r15.unpack(bref));
  if (num_pos_r15_present) {
    HANDLE_CODE(num_pos_r15.unpack(bref));
  }
  HANDLE_CODE(freq_location_r15.unpack(bref));
  HANDLE_CODE(time_offset_drx_r15.unpack(bref));
  HANDLE_CODE(time_offset_e_drx_short_r15.unpack(bref));
  if (time_offset_e_drx_long_r15_present) {
    HANDLE_CODE(time_offset_e_drx_long_r15.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void wus_cfg_r15_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("maxDurationFactor-r15", max_dur_factor_r15.to_string());
  if (num_pos_r15_present) {
    j.write_str("numPOs-r15", num_pos_r15.to_string());
  }
  j.write_str("freqLocation-r15", freq_location_r15.to_string());
  j.write_str("timeOffsetDRX-r15", time_offset_drx_r15.to_string());
  j.write_str("timeOffset-eDRX-Short-r15", time_offset_e_drx_short_r15.to_string());
  if (time_offset_e_drx_long_r15_present) {
    j.write_str("timeOffset-eDRX-Long-r15", time_offset_e_drx_long_r15.to_string());
  }
  j.end_obj();
}

const char* wus_cfg_r15_s::max_dur_factor_r15_opts::to_string() const
{
  static const char* options[] = {"one32th", "one16th", "one8th", "one4th"};
  return convert_enum_idx(options, 4, value, "wus_cfg_r15_s::max_dur_factor_r15_e_");
}
uint8_t wus_cfg_r15_s::max_dur_factor_r15_opts::to_number() const
{
  static const uint8_t options[] = {32, 16, 8, 4};
  return map_enum_number(options, 4, value, "wus_cfg_r15_s::max_dur_factor_r15_e_");
}

const char* wus_cfg_r15_s::num_pos_r15_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "spare1"};
  return convert_enum_idx(options, 4, value, "wus_cfg_r15_s::num_pos_r15_e_");
}
uint8_t wus_cfg_r15_s::num_pos_r15_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "wus_cfg_r15_s::num_pos_r15_e_");
}

const char* wus_cfg_r15_s::freq_location_r15_opts::to_string() const
{
  static const char* options[] = {"n0", "n2", "n4", "spare1"};
  return convert_enum_idx(options, 4, value, "wus_cfg_r15_s::freq_location_r15_e_");
}
uint8_t wus_cfg_r15_s::freq_location_r15_opts::to_number() const
{
  static const uint8_t options[] = {0, 2, 4};
  return map_enum_number(options, 3, value, "wus_cfg_r15_s::freq_location_r15_e_");
}

const char* wus_cfg_r15_s::time_offset_drx_r15_opts::to_string() const
{
  static const char* options[] = {"ms40", "ms80", "ms160", "ms240"};
  return convert_enum_idx(options, 4, value, "wus_cfg_r15_s::time_offset_drx_r15_e_");
}
uint8_t wus_cfg_r15_s::time_offset_drx_r15_opts::to_number() const
{
  static const uint8_t options[] = {40, 80, 160, 240};
  return map_enum_number(options, 4, value, "wus_cfg_r15_s::time_offset_drx_r15_e_");
}

const char* wus_cfg_r15_s::time_offset_e_drx_short_r15_opts::to_string() const
{
  static const char* options[] = {"ms40", "ms80", "ms160", "ms240"};
  return convert_enum_idx(options, 4, value, "wus_cfg_r15_s::time_offset_e_drx_short_r15_e_");
}
uint8_t wus_cfg_r15_s::time_offset_e_drx_short_r15_opts::to_number() const
{
  static const uint8_t options[] = {40, 80, 160, 240};
  return map_enum_number(options, 4, value, "wus_cfg_r15_s::time_offset_e_drx_short_r15_e_");
}

const char* wus_cfg_r15_s::time_offset_e_drx_long_r15_opts::to_string() const
{
  static const char* options[] = {"ms1000", "ms2000"};
  return convert_enum_idx(options, 2, value, "wus_cfg_r15_s::time_offset_e_drx_long_r15_e_");
}
uint16_t wus_cfg_r15_s::time_offset_e_drx_long_r15_opts::to_number() const
{
  static const uint16_t options[] = {1000, 2000};
  return map_enum_number(options, 2, value, "wus_cfg_r15_s::time_offset_e_drx_long_r15_e_");
}

// WUS-Config-v1560 ::= SEQUENCE
SRSASN_CODE wus_cfg_v1560_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pwr_boost_r15.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE wus_cfg_v1560_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(pwr_boost_r15.unpack(bref));

  return SRSASN_SUCCESS;
}
void wus_cfg_v1560_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("powerBoost-r15", pwr_boost_r15.to_string());
  j.end_obj();
}

const char* wus_cfg_v1560_s::pwr_boost_r15_opts::to_string() const
{
  static const char* options[] = {"dB0", "dB1dot8", "dB3", "dB4dot8"};
  return convert_enum_idx(options, 4, value, "wus_cfg_v1560_s::pwr_boost_r15_e_");
}
float wus_cfg_v1560_s::pwr_boost_r15_opts::to_number() const
{
  static const float options[] = {0.0, 1.8, 3.0, 4.8};
  return map_enum_number(options, 4, value, "wus_cfg_v1560_s::pwr_boost_r15_e_");
}
const char* wus_cfg_v1560_s::pwr_boost_r15_opts::to_number_string() const
{
  static const char* options[] = {"0", "1.8", "3", "4.8"};
  return convert_enum_idx(options, 4, value, "wus_cfg_v1560_s::pwr_boost_r15_e_");
}

// RadioResourceConfigCommonSIB ::= SEQUENCE
SRSASN_CODE rr_cfg_common_sib_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(rach_cfg_common.pack(bref));
  HANDLE_CODE(bcch_cfg.pack(bref));
  HANDLE_CODE(pcch_cfg.pack(bref));
  HANDLE_CODE(prach_cfg.pack(bref));
  HANDLE_CODE(pdsch_cfg_common.pack(bref));
  HANDLE_CODE(pusch_cfg_common.pack(bref));
  HANDLE_CODE(pucch_cfg_common.pack(bref));
  HANDLE_CODE(srs_ul_cfg_common.pack(bref));
  HANDLE_CODE(ul_pwr_ctrl_common.pack(bref));
  HANDLE_CODE(ul_cp_len.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ul_pwr_ctrl_common_v1020.is_present();
    group_flags[1] |= rach_cfg_common_v1250.is_present();
    group_flags[2] |= pusch_cfg_common_v1270.is_present();
    group_flags[3] |= bcch_cfg_v1310.is_present();
    group_flags[3] |= pcch_cfg_v1310.is_present();
    group_flags[3] |= freq_hop_params_r13.is_present();
    group_flags[3] |= pdsch_cfg_common_v1310.is_present();
    group_flags[3] |= pusch_cfg_common_v1310.is_present();
    group_flags[3] |= prach_cfg_common_v1310.is_present();
    group_flags[3] |= pucch_cfg_common_v1310.is_present();
    group_flags[4] |= high_speed_cfg_r14.is_present();
    group_flags[4] |= prach_cfg_v1430.is_present();
    group_flags[4] |= pucch_cfg_common_v1430.is_present();
    group_flags[5] |= prach_cfg_v1530.is_present();
    group_flags[5] |= ce_rss_cfg_r15.is_present();
    group_flags[5] |= wus_cfg_r15.is_present();
    group_flags[5] |= high_speed_cfg_v1530.is_present();
    group_flags[6] |= ul_pwr_ctrl_common_v1540.is_present();
    group_flags[7] |= wus_cfg_v1560.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ul_pwr_ctrl_common_v1020.is_present(), 1));
      if (ul_pwr_ctrl_common_v1020.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_v1020->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rach_cfg_common_v1250.is_present(), 1));
      if (rach_cfg_common_v1250.is_present()) {
        HANDLE_CODE(rach_cfg_common_v1250->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pusch_cfg_common_v1270.is_present(), 1));
      if (pusch_cfg_common_v1270.is_present()) {
        HANDLE_CODE(pusch_cfg_common_v1270->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(bcch_cfg_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(pcch_cfg_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(freq_hop_params_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(pdsch_cfg_common_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(pusch_cfg_common_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(prach_cfg_common_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(pucch_cfg_common_v1310.is_present(), 1));
      if (bcch_cfg_v1310.is_present()) {
        HANDLE_CODE(bcch_cfg_v1310->pack(bref));
      }
      if (pcch_cfg_v1310.is_present()) {
        HANDLE_CODE(pcch_cfg_v1310->pack(bref));
      }
      if (freq_hop_params_r13.is_present()) {
        HANDLE_CODE(freq_hop_params_r13->pack(bref));
      }
      if (pdsch_cfg_common_v1310.is_present()) {
        HANDLE_CODE(pdsch_cfg_common_v1310->pack(bref));
      }
      if (pusch_cfg_common_v1310.is_present()) {
        HANDLE_CODE(pusch_cfg_common_v1310->pack(bref));
      }
      if (prach_cfg_common_v1310.is_present()) {
        HANDLE_CODE(prach_cfg_common_v1310->pack(bref));
      }
      if (pucch_cfg_common_v1310.is_present()) {
        HANDLE_CODE(pucch_cfg_common_v1310->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(high_speed_cfg_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(prach_cfg_v1430.is_present(), 1));
      HANDLE_CODE(bref.pack(pucch_cfg_common_v1430.is_present(), 1));
      if (high_speed_cfg_r14.is_present()) {
        HANDLE_CODE(high_speed_cfg_r14->pack(bref));
      }
      if (prach_cfg_v1430.is_present()) {
        HANDLE_CODE(prach_cfg_v1430->pack(bref));
      }
      if (pucch_cfg_common_v1430.is_present()) {
        HANDLE_CODE(pucch_cfg_common_v1430->pack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(prach_cfg_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(ce_rss_cfg_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(wus_cfg_r15.is_present(), 1));
      HANDLE_CODE(bref.pack(high_speed_cfg_v1530.is_present(), 1));
      if (prach_cfg_v1530.is_present()) {
        HANDLE_CODE(prach_cfg_v1530->pack(bref));
      }
      if (ce_rss_cfg_r15.is_present()) {
        HANDLE_CODE(ce_rss_cfg_r15->pack(bref));
      }
      if (wus_cfg_r15.is_present()) {
        HANDLE_CODE(wus_cfg_r15->pack(bref));
      }
      if (high_speed_cfg_v1530.is_present()) {
        HANDLE_CODE(high_speed_cfg_v1530->pack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ul_pwr_ctrl_common_v1540.is_present(), 1));
      if (ul_pwr_ctrl_common_v1540.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_v1540->pack(bref));
      }
    }
    if (group_flags[7]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(wus_cfg_v1560.is_present(), 1));
      if (wus_cfg_v1560.is_present()) {
        HANDLE_CODE(wus_cfg_v1560->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_common_sib_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(rach_cfg_common.unpack(bref));
  HANDLE_CODE(bcch_cfg.unpack(bref));
  HANDLE_CODE(pcch_cfg.unpack(bref));
  HANDLE_CODE(prach_cfg.unpack(bref));
  HANDLE_CODE(pdsch_cfg_common.unpack(bref));
  HANDLE_CODE(pusch_cfg_common.unpack(bref));
  HANDLE_CODE(pucch_cfg_common.unpack(bref));
  HANDLE_CODE(srs_ul_cfg_common.unpack(bref));
  HANDLE_CODE(ul_pwr_ctrl_common.unpack(bref));
  HANDLE_CODE(ul_cp_len.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(8);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ul_pwr_ctrl_common_v1020_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_common_v1020_present, 1));
      ul_pwr_ctrl_common_v1020.set_present(ul_pwr_ctrl_common_v1020_present);
      if (ul_pwr_ctrl_common_v1020.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_v1020->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rach_cfg_common_v1250_present;
      HANDLE_CODE(bref.unpack(rach_cfg_common_v1250_present, 1));
      rach_cfg_common_v1250.set_present(rach_cfg_common_v1250_present);
      if (rach_cfg_common_v1250.is_present()) {
        HANDLE_CODE(rach_cfg_common_v1250->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool pusch_cfg_common_v1270_present;
      HANDLE_CODE(bref.unpack(pusch_cfg_common_v1270_present, 1));
      pusch_cfg_common_v1270.set_present(pusch_cfg_common_v1270_present);
      if (pusch_cfg_common_v1270.is_present()) {
        HANDLE_CODE(pusch_cfg_common_v1270->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool bcch_cfg_v1310_present;
      HANDLE_CODE(bref.unpack(bcch_cfg_v1310_present, 1));
      bcch_cfg_v1310.set_present(bcch_cfg_v1310_present);
      bool pcch_cfg_v1310_present;
      HANDLE_CODE(bref.unpack(pcch_cfg_v1310_present, 1));
      pcch_cfg_v1310.set_present(pcch_cfg_v1310_present);
      bool freq_hop_params_r13_present;
      HANDLE_CODE(bref.unpack(freq_hop_params_r13_present, 1));
      freq_hop_params_r13.set_present(freq_hop_params_r13_present);
      bool pdsch_cfg_common_v1310_present;
      HANDLE_CODE(bref.unpack(pdsch_cfg_common_v1310_present, 1));
      pdsch_cfg_common_v1310.set_present(pdsch_cfg_common_v1310_present);
      bool pusch_cfg_common_v1310_present;
      HANDLE_CODE(bref.unpack(pusch_cfg_common_v1310_present, 1));
      pusch_cfg_common_v1310.set_present(pusch_cfg_common_v1310_present);
      bool prach_cfg_common_v1310_present;
      HANDLE_CODE(bref.unpack(prach_cfg_common_v1310_present, 1));
      prach_cfg_common_v1310.set_present(prach_cfg_common_v1310_present);
      bool pucch_cfg_common_v1310_present;
      HANDLE_CODE(bref.unpack(pucch_cfg_common_v1310_present, 1));
      pucch_cfg_common_v1310.set_present(pucch_cfg_common_v1310_present);
      if (bcch_cfg_v1310.is_present()) {
        HANDLE_CODE(bcch_cfg_v1310->unpack(bref));
      }
      if (pcch_cfg_v1310.is_present()) {
        HANDLE_CODE(pcch_cfg_v1310->unpack(bref));
      }
      if (freq_hop_params_r13.is_present()) {
        HANDLE_CODE(freq_hop_params_r13->unpack(bref));
      }
      if (pdsch_cfg_common_v1310.is_present()) {
        HANDLE_CODE(pdsch_cfg_common_v1310->unpack(bref));
      }
      if (pusch_cfg_common_v1310.is_present()) {
        HANDLE_CODE(pusch_cfg_common_v1310->unpack(bref));
      }
      if (prach_cfg_common_v1310.is_present()) {
        HANDLE_CODE(prach_cfg_common_v1310->unpack(bref));
      }
      if (pucch_cfg_common_v1310.is_present()) {
        HANDLE_CODE(pucch_cfg_common_v1310->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool high_speed_cfg_r14_present;
      HANDLE_CODE(bref.unpack(high_speed_cfg_r14_present, 1));
      high_speed_cfg_r14.set_present(high_speed_cfg_r14_present);
      bool prach_cfg_v1430_present;
      HANDLE_CODE(bref.unpack(prach_cfg_v1430_present, 1));
      prach_cfg_v1430.set_present(prach_cfg_v1430_present);
      bool pucch_cfg_common_v1430_present;
      HANDLE_CODE(bref.unpack(pucch_cfg_common_v1430_present, 1));
      pucch_cfg_common_v1430.set_present(pucch_cfg_common_v1430_present);
      if (high_speed_cfg_r14.is_present()) {
        HANDLE_CODE(high_speed_cfg_r14->unpack(bref));
      }
      if (prach_cfg_v1430.is_present()) {
        HANDLE_CODE(prach_cfg_v1430->unpack(bref));
      }
      if (pucch_cfg_common_v1430.is_present()) {
        HANDLE_CODE(pucch_cfg_common_v1430->unpack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool prach_cfg_v1530_present;
      HANDLE_CODE(bref.unpack(prach_cfg_v1530_present, 1));
      prach_cfg_v1530.set_present(prach_cfg_v1530_present);
      bool ce_rss_cfg_r15_present;
      HANDLE_CODE(bref.unpack(ce_rss_cfg_r15_present, 1));
      ce_rss_cfg_r15.set_present(ce_rss_cfg_r15_present);
      bool wus_cfg_r15_present;
      HANDLE_CODE(bref.unpack(wus_cfg_r15_present, 1));
      wus_cfg_r15.set_present(wus_cfg_r15_present);
      bool high_speed_cfg_v1530_present;
      HANDLE_CODE(bref.unpack(high_speed_cfg_v1530_present, 1));
      high_speed_cfg_v1530.set_present(high_speed_cfg_v1530_present);
      if (prach_cfg_v1530.is_present()) {
        HANDLE_CODE(prach_cfg_v1530->unpack(bref));
      }
      if (ce_rss_cfg_r15.is_present()) {
        HANDLE_CODE(ce_rss_cfg_r15->unpack(bref));
      }
      if (wus_cfg_r15.is_present()) {
        HANDLE_CODE(wus_cfg_r15->unpack(bref));
      }
      if (high_speed_cfg_v1530.is_present()) {
        HANDLE_CODE(high_speed_cfg_v1530->unpack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ul_pwr_ctrl_common_v1540_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_common_v1540_present, 1));
      ul_pwr_ctrl_common_v1540.set_present(ul_pwr_ctrl_common_v1540_present);
      if (ul_pwr_ctrl_common_v1540.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_v1540->unpack(bref));
      }
    }
    if (group_flags[7]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool wus_cfg_v1560_present;
      HANDLE_CODE(bref.unpack(wus_cfg_v1560_present, 1));
      wus_cfg_v1560.set_present(wus_cfg_v1560_present);
      if (wus_cfg_v1560.is_present()) {
        HANDLE_CODE(wus_cfg_v1560->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rr_cfg_common_sib_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("rach-ConfigCommon");
  rach_cfg_common.to_json(j);
  j.write_fieldname("bcch-Config");
  bcch_cfg.to_json(j);
  j.write_fieldname("pcch-Config");
  pcch_cfg.to_json(j);
  j.write_fieldname("prach-Config");
  prach_cfg.to_json(j);
  j.write_fieldname("pdsch-ConfigCommon");
  pdsch_cfg_common.to_json(j);
  j.write_fieldname("pusch-ConfigCommon");
  pusch_cfg_common.to_json(j);
  j.write_fieldname("pucch-ConfigCommon");
  pucch_cfg_common.to_json(j);
  j.write_fieldname("soundingRS-UL-ConfigCommon");
  srs_ul_cfg_common.to_json(j);
  j.write_fieldname("uplinkPowerControlCommon");
  ul_pwr_ctrl_common.to_json(j);
  j.write_str("ul-CyclicPrefixLength", ul_cp_len.to_string());
  if (ext) {
    if (ul_pwr_ctrl_common_v1020.is_present()) {
      j.write_fieldname("uplinkPowerControlCommon-v1020");
      ul_pwr_ctrl_common_v1020->to_json(j);
    }
    if (rach_cfg_common_v1250.is_present()) {
      j.write_fieldname("rach-ConfigCommon-v1250");
      rach_cfg_common_v1250->to_json(j);
    }
    if (pusch_cfg_common_v1270.is_present()) {
      j.write_fieldname("pusch-ConfigCommon-v1270");
      pusch_cfg_common_v1270->to_json(j);
    }
    if (bcch_cfg_v1310.is_present()) {
      j.write_fieldname("bcch-Config-v1310");
      bcch_cfg_v1310->to_json(j);
    }
    if (pcch_cfg_v1310.is_present()) {
      j.write_fieldname("pcch-Config-v1310");
      pcch_cfg_v1310->to_json(j);
    }
    if (freq_hop_params_r13.is_present()) {
      j.write_fieldname("freqHoppingParameters-r13");
      freq_hop_params_r13->to_json(j);
    }
    if (pdsch_cfg_common_v1310.is_present()) {
      j.write_fieldname("pdsch-ConfigCommon-v1310");
      pdsch_cfg_common_v1310->to_json(j);
    }
    if (pusch_cfg_common_v1310.is_present()) {
      j.write_fieldname("pusch-ConfigCommon-v1310");
      pusch_cfg_common_v1310->to_json(j);
    }
    if (prach_cfg_common_v1310.is_present()) {
      j.write_fieldname("prach-ConfigCommon-v1310");
      prach_cfg_common_v1310->to_json(j);
    }
    if (pucch_cfg_common_v1310.is_present()) {
      j.write_fieldname("pucch-ConfigCommon-v1310");
      pucch_cfg_common_v1310->to_json(j);
    }
    if (high_speed_cfg_r14.is_present()) {
      j.write_fieldname("highSpeedConfig-r14");
      high_speed_cfg_r14->to_json(j);
    }
    if (prach_cfg_v1430.is_present()) {
      j.write_fieldname("prach-Config-v1430");
      prach_cfg_v1430->to_json(j);
    }
    if (pucch_cfg_common_v1430.is_present()) {
      j.write_fieldname("pucch-ConfigCommon-v1430");
      pucch_cfg_common_v1430->to_json(j);
    }
    if (prach_cfg_v1530.is_present()) {
      j.write_fieldname("prach-Config-v1530");
      prach_cfg_v1530->to_json(j);
    }
    if (ce_rss_cfg_r15.is_present()) {
      j.write_fieldname("ce-RSS-Config-r15");
      ce_rss_cfg_r15->to_json(j);
    }
    if (wus_cfg_r15.is_present()) {
      j.write_fieldname("wus-Config-r15");
      wus_cfg_r15->to_json(j);
    }
    if (high_speed_cfg_v1530.is_present()) {
      j.write_fieldname("highSpeedConfig-v1530");
      high_speed_cfg_v1530->to_json(j);
    }
    if (ul_pwr_ctrl_common_v1540.is_present()) {
      j.write_fieldname("uplinkPowerControlCommon-v1540");
      ul_pwr_ctrl_common_v1540->to_json(j);
    }
    if (wus_cfg_v1560.is_present()) {
      j.write_fieldname("wus-Config-v1560");
      wus_cfg_v1560->to_json(j);
    }
  }
  j.end_obj();
}

// TimeAlignmentTimer ::= ENUMERATED
const char* time_align_timer_opts::to_string() const
{
  static const char* options[] = {"sf500", "sf750", "sf1280", "sf1920", "sf2560", "sf5120", "sf10240", "infinity"};
  return convert_enum_idx(options, 8, value, "time_align_timer_e");
}
int16_t time_align_timer_opts::to_number() const
{
  static const int16_t options[] = {500, 750, 1280, 1920, 2560, 5120, 10240, -1};
  return map_enum_number(options, 8, value, "time_align_timer_e");
}

// AntennaInfoCommon ::= SEQUENCE
SRSASN_CODE ant_info_common_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(ant_ports_count.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ant_info_common_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(ant_ports_count.unpack(bref));

  return SRSASN_SUCCESS;
}
void ant_info_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("antennaPortsCount", ant_ports_count.to_string());
  j.end_obj();
}
bool ant_info_common_s::operator==(const ant_info_common_s& other) const
{
  return ant_ports_count == other.ant_ports_count;
}

const char* ant_info_common_s::ant_ports_count_opts::to_string() const
{
  static const char* options[] = {"an1", "an2", "an4", "spare1"};
  return convert_enum_idx(options, 4, value, "ant_info_common_s::ant_ports_count_e_");
}
uint8_t ant_info_common_s::ant_ports_count_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4};
  return map_enum_number(options, 3, value, "ant_info_common_s::ant_ports_count_e_");
}

// HighSpeedConfigSCell-r14 ::= SEQUENCE
SRSASN_CODE high_speed_cfg_scell_r14_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(high_speed_enhanced_demod_flag_r14_present, 1));

  return SRSASN_SUCCESS;
}
SRSASN_CODE high_speed_cfg_scell_r14_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(high_speed_enhanced_demod_flag_r14_present, 1));

  return SRSASN_SUCCESS;
}
void high_speed_cfg_scell_r14_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (high_speed_enhanced_demod_flag_r14_present) {
    j.write_str("highSpeedEnhancedDemodulationFlag-r14", "true");
  }
  j.end_obj();
}
bool high_speed_cfg_scell_r14_s::operator==(const high_speed_cfg_scell_r14_s& other) const
{
  return high_speed_enhanced_demod_flag_r14_present == other.high_speed_enhanced_demod_flag_r14_present;
}

// PRACH-Config ::= SEQUENCE
SRSASN_CODE prach_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(prach_cfg_info_present, 1));

  HANDLE_CODE(pack_integer(bref, root_seq_idx, (uint16_t)0u, (uint16_t)837u));
  if (prach_cfg_info_present) {
    HANDLE_CODE(prach_cfg_info.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE prach_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(prach_cfg_info_present, 1));

  HANDLE_CODE(unpack_integer(root_seq_idx, bref, (uint16_t)0u, (uint16_t)837u));
  if (prach_cfg_info_present) {
    HANDLE_CODE(prach_cfg_info.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void prach_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("rootSequenceIndex", root_seq_idx);
  if (prach_cfg_info_present) {
    j.write_fieldname("prach-ConfigInfo");
    prach_cfg_info.to_json(j);
  }
  j.end_obj();
}
bool prach_cfg_s::operator==(const prach_cfg_s& other) const
{
  return root_seq_idx == other.root_seq_idx and prach_cfg_info_present == other.prach_cfg_info_present and
         (not prach_cfg_info_present or prach_cfg_info == other.prach_cfg_info);
}

// PRACH-ConfigSCell-r10 ::= SEQUENCE
SRSASN_CODE prach_cfg_scell_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, prach_cfg_idx_r10, (uint8_t)0u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
SRSASN_CODE prach_cfg_scell_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(prach_cfg_idx_r10, bref, (uint8_t)0u, (uint8_t)63u));

  return SRSASN_SUCCESS;
}
void prach_cfg_scell_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("prach-ConfigIndex-r10", prach_cfg_idx_r10);
  j.end_obj();
}
bool prach_cfg_scell_r10_s::operator==(const prach_cfg_scell_r10_s& other) const
{
  return prach_cfg_idx_r10 == other.prach_cfg_idx_r10;
}

// RACH-ConfigCommonSCell-r11 ::= SEQUENCE
SRSASN_CODE rach_cfg_common_scell_r11_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(pwr_ramp_params_r11.pack(bref));
  HANDLE_CODE(ra_supervision_info_r11.preamb_trans_max_r11.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE rach_cfg_common_scell_r11_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(pwr_ramp_params_r11.unpack(bref));
  HANDLE_CODE(ra_supervision_info_r11.preamb_trans_max_r11.unpack(bref));

  return SRSASN_SUCCESS;
}
void rach_cfg_common_scell_r11_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("powerRampingParameters-r11");
  pwr_ramp_params_r11.to_json(j);
  j.write_fieldname("ra-SupervisionInfo-r11");
  j.start_obj();
  j.write_str("preambleTransMax-r11", ra_supervision_info_r11.preamb_trans_max_r11.to_string());
  j.end_obj();
  j.end_obj();
}
bool rach_cfg_common_scell_r11_s::operator==(const rach_cfg_common_scell_r11_s& other) const
{
  return ext == other.ext and pwr_ramp_params_r11 == other.pwr_ramp_params_r11 and
         ra_supervision_info_r11.preamb_trans_max_r11 == other.ra_supervision_info_r11.preamb_trans_max_r11;
}

// UplinkPowerControlCommonPUSCH-LessCell-v1430 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_common_pusch_less_cell_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(p0_nominal_periodic_srs_r14_present, 1));
  HANDLE_CODE(bref.pack(p0_nominal_aperiodic_srs_r14_present, 1));
  HANDLE_CODE(bref.pack(alpha_srs_r14_present, 1));

  if (p0_nominal_periodic_srs_r14_present) {
    HANDLE_CODE(pack_integer(bref, p0_nominal_periodic_srs_r14, (int8_t)-126, (int8_t)24));
  }
  if (p0_nominal_aperiodic_srs_r14_present) {
    HANDLE_CODE(pack_integer(bref, p0_nominal_aperiodic_srs_r14, (int8_t)-126, (int8_t)24));
  }
  if (alpha_srs_r14_present) {
    HANDLE_CODE(alpha_srs_r14.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_common_pusch_less_cell_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(p0_nominal_periodic_srs_r14_present, 1));
  HANDLE_CODE(bref.unpack(p0_nominal_aperiodic_srs_r14_present, 1));
  HANDLE_CODE(bref.unpack(alpha_srs_r14_present, 1));

  if (p0_nominal_periodic_srs_r14_present) {
    HANDLE_CODE(unpack_integer(p0_nominal_periodic_srs_r14, bref, (int8_t)-126, (int8_t)24));
  }
  if (p0_nominal_aperiodic_srs_r14_present) {
    HANDLE_CODE(unpack_integer(p0_nominal_aperiodic_srs_r14, bref, (int8_t)-126, (int8_t)24));
  }
  if (alpha_srs_r14_present) {
    HANDLE_CODE(alpha_srs_r14.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_common_pusch_less_cell_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (p0_nominal_periodic_srs_r14_present) {
    j.write_int("p0-Nominal-PeriodicSRS-r14", p0_nominal_periodic_srs_r14);
  }
  if (p0_nominal_aperiodic_srs_r14_present) {
    j.write_int("p0-Nominal-AperiodicSRS-r14", p0_nominal_aperiodic_srs_r14);
  }
  if (alpha_srs_r14_present) {
    j.write_str("alpha-SRS-r14", alpha_srs_r14.to_string());
  }
  j.end_obj();
}
bool ul_pwr_ctrl_common_pusch_less_cell_v1430_s::operator==(
    const ul_pwr_ctrl_common_pusch_less_cell_v1430_s& other) const
{
  return p0_nominal_periodic_srs_r14_present == other.p0_nominal_periodic_srs_r14_present and
         (not p0_nominal_periodic_srs_r14_present or
          p0_nominal_periodic_srs_r14 == other.p0_nominal_periodic_srs_r14) and
         p0_nominal_aperiodic_srs_r14_present == other.p0_nominal_aperiodic_srs_r14_present and
         (not p0_nominal_aperiodic_srs_r14_present or
          p0_nominal_aperiodic_srs_r14 == other.p0_nominal_aperiodic_srs_r14) and
         alpha_srs_r14_present == other.alpha_srs_r14_present and
         (not alpha_srs_r14_present or alpha_srs_r14 == other.alpha_srs_r14);
}

// UplinkPowerControlCommonSCell-r10 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_common_scell_r10_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, p0_nominal_pusch_r10, (int8_t)-126, (int8_t)24));
  HANDLE_CODE(alpha_r10.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_common_scell_r10_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(p0_nominal_pusch_r10, bref, (int8_t)-126, (int8_t)24));
  HANDLE_CODE(alpha_r10.unpack(bref));

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_common_scell_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("p0-NominalPUSCH-r10", p0_nominal_pusch_r10);
  j.write_str("alpha-r10", alpha_r10.to_string());
  j.end_obj();
}
bool ul_pwr_ctrl_common_scell_r10_s::operator==(const ul_pwr_ctrl_common_scell_r10_s& other) const
{
  return p0_nominal_pusch_r10 == other.p0_nominal_pusch_r10 and alpha_r10 == other.alpha_r10;
}

// UplinkPowerControlCommonSCell-v1130 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_common_scell_v1130_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(pack_integer(bref, delta_preamb_msg3_r11, (int8_t)-1, (int8_t)6));

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_common_scell_v1130_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(unpack_integer(delta_preamb_msg3_r11, bref, (int8_t)-1, (int8_t)6));

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_common_scell_v1130_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("deltaPreambleMsg3-r11", delta_preamb_msg3_r11);
  j.end_obj();
}
bool ul_pwr_ctrl_common_scell_v1130_s::operator==(const ul_pwr_ctrl_common_scell_v1130_s& other) const
{
  return delta_preamb_msg3_r11 == other.delta_preamb_msg3_r11;
}

// UplinkPowerControlCommonSCell-v1310 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_common_scell_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(delta_f_pucch_format3_r12_present, 1));
  HANDLE_CODE(bref.pack(delta_f_pucch_format1b_cs_r12_present, 1));
  HANDLE_CODE(bref.pack(delta_f_pucch_format4_r13_present, 1));
  HANDLE_CODE(bref.pack(delta_f_pucch_format5_minus13_present, 1));

  HANDLE_CODE(pack_integer(bref, p0_nominal_pucch, (int8_t)-127, (int8_t)-96));
  HANDLE_CODE(delta_flist_pucch.pack(bref));
  if (delta_f_pucch_format3_r12_present) {
    HANDLE_CODE(delta_f_pucch_format3_r12.pack(bref));
  }
  if (delta_f_pucch_format1b_cs_r12_present) {
    HANDLE_CODE(delta_f_pucch_format1b_cs_r12.pack(bref));
  }
  if (delta_f_pucch_format4_r13_present) {
    HANDLE_CODE(delta_f_pucch_format4_r13.pack(bref));
  }
  if (delta_f_pucch_format5_minus13_present) {
    HANDLE_CODE(delta_f_pucch_format5_minus13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_common_scell_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(delta_f_pucch_format3_r12_present, 1));
  HANDLE_CODE(bref.unpack(delta_f_pucch_format1b_cs_r12_present, 1));
  HANDLE_CODE(bref.unpack(delta_f_pucch_format4_r13_present, 1));
  HANDLE_CODE(bref.unpack(delta_f_pucch_format5_minus13_present, 1));

  HANDLE_CODE(unpack_integer(p0_nominal_pucch, bref, (int8_t)-127, (int8_t)-96));
  HANDLE_CODE(delta_flist_pucch.unpack(bref));
  if (delta_f_pucch_format3_r12_present) {
    HANDLE_CODE(delta_f_pucch_format3_r12.unpack(bref));
  }
  if (delta_f_pucch_format1b_cs_r12_present) {
    HANDLE_CODE(delta_f_pucch_format1b_cs_r12.unpack(bref));
  }
  if (delta_f_pucch_format4_r13_present) {
    HANDLE_CODE(delta_f_pucch_format4_r13.unpack(bref));
  }
  if (delta_f_pucch_format5_minus13_present) {
    HANDLE_CODE(delta_f_pucch_format5_minus13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_common_scell_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_int("p0-NominalPUCCH", p0_nominal_pucch);
  j.write_fieldname("deltaFList-PUCCH");
  delta_flist_pucch.to_json(j);
  if (delta_f_pucch_format3_r12_present) {
    j.write_str("deltaF-PUCCH-Format3-r12", delta_f_pucch_format3_r12.to_string());
  }
  if (delta_f_pucch_format1b_cs_r12_present) {
    j.write_str("deltaF-PUCCH-Format1bCS-r12", delta_f_pucch_format1b_cs_r12.to_string());
  }
  if (delta_f_pucch_format4_r13_present) {
    j.write_str("deltaF-PUCCH-Format4-r13", delta_f_pucch_format4_r13.to_string());
  }
  if (delta_f_pucch_format5_minus13_present) {
    j.write_str("deltaF-PUCCH-Format5-13", delta_f_pucch_format5_minus13.to_string());
  }
  j.end_obj();
}
bool ul_pwr_ctrl_common_scell_v1310_s::operator==(const ul_pwr_ctrl_common_scell_v1310_s& other) const
{
  return p0_nominal_pucch == other.p0_nominal_pucch and delta_flist_pucch == other.delta_flist_pucch and
         delta_f_pucch_format3_r12_present == other.delta_f_pucch_format3_r12_present and
         (not delta_f_pucch_format3_r12_present or delta_f_pucch_format3_r12 == other.delta_f_pucch_format3_r12) and
         delta_f_pucch_format1b_cs_r12_present == other.delta_f_pucch_format1b_cs_r12_present and
         (not delta_f_pucch_format1b_cs_r12_present or
          delta_f_pucch_format1b_cs_r12 == other.delta_f_pucch_format1b_cs_r12) and
         delta_f_pucch_format4_r13_present == other.delta_f_pucch_format4_r13_present and
         (not delta_f_pucch_format4_r13_present or delta_f_pucch_format4_r13 == other.delta_f_pucch_format4_r13) and
         delta_f_pucch_format5_minus13_present == other.delta_f_pucch_format5_minus13_present and
         (not delta_f_pucch_format5_minus13_present or
          delta_f_pucch_format5_minus13 == other.delta_f_pucch_format5_minus13);
}

const char* ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format3_r12_opts::to_string() const
{
  static const char* options[] = {
      "deltaF-1", "deltaF0", "deltaF1", "deltaF2", "deltaF3", "deltaF4", "deltaF5", "deltaF6"};
  return convert_enum_idx(options, 8, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format3_r12_e_");
}
int8_t ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format3_r12_opts::to_number() const
{
  static const int8_t options[] = {-1, 0, 1, 2, 3, 4, 5, 6};
  return map_enum_number(options, 8, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format3_r12_e_");
}

const char* ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format1b_cs_r12_opts::to_string() const
{
  static const char* options[] = {"deltaF1", "deltaF2", "spare2", "spare1"};
  return convert_enum_idx(options, 4, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format1b_cs_r12_e_");
}
uint8_t ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format1b_cs_r12_opts::to_number() const
{
  static const uint8_t options[] = {1, 2};
  return map_enum_number(options, 2, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format1b_cs_r12_e_");
}

const char* ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format4_r13_opts::to_string() const
{
  static const char* options[] = {
      "deltaF16", "deltaF15", "deltaF14", "deltaF13", "deltaF12", "deltaF11", "deltaF10", "spare1"};
  return convert_enum_idx(options, 8, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format4_r13_e_");
}
uint8_t ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format4_r13_opts::to_number() const
{
  static const uint8_t options[] = {16, 15, 14, 13, 12, 11, 10};
  return map_enum_number(options, 7, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format4_r13_e_");
}

const char* ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format5_minus13_opts::to_string() const
{
  static const char* options[] = {
      "deltaF13", "deltaF12", "deltaF11", "deltaF10", "deltaF9", "deltaF8", "deltaF7", "spare1"};
  return convert_enum_idx(options, 8, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format5_minus13_e_");
}
uint8_t ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format5_minus13_opts::to_number() const
{
  static const uint8_t options[] = {13, 12, 11, 10, 9, 8, 7};
  return map_enum_number(options, 7, value, "ul_pwr_ctrl_common_scell_v1310_s::delta_f_pucch_format5_minus13_e_");
}

// RadioResourceConfigCommonSCell-r10 ::= SEQUENCE
SRSASN_CODE rr_cfg_common_scell_r10_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(ul_cfg_r10_present, 1));

  HANDLE_CODE(bref.pack(non_ul_cfg_r10.mbsfn_sf_cfg_list_r10_present, 1));
  HANDLE_CODE(bref.pack(non_ul_cfg_r10.tdd_cfg_r10_present, 1));
  HANDLE_CODE(non_ul_cfg_r10.dl_bw_r10.pack(bref));
  HANDLE_CODE(non_ul_cfg_r10.ant_info_common_r10.pack(bref));
  if (non_ul_cfg_r10.mbsfn_sf_cfg_list_r10_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, non_ul_cfg_r10.mbsfn_sf_cfg_list_r10, 1, 8));
  }
  HANDLE_CODE(non_ul_cfg_r10.phich_cfg_r10.pack(bref));
  HANDLE_CODE(non_ul_cfg_r10.pdsch_cfg_common_r10.pack(bref));
  if (non_ul_cfg_r10.tdd_cfg_r10_present) {
    HANDLE_CODE(non_ul_cfg_r10.tdd_cfg_r10.pack(bref));
  }
  if (ul_cfg_r10_present) {
    HANDLE_CODE(bref.pack(ul_cfg_r10.p_max_r10_present, 1));
    HANDLE_CODE(bref.pack(ul_cfg_r10.prach_cfg_scell_r10_present, 1));
    HANDLE_CODE(bref.pack(ul_cfg_r10.ul_freq_info_r10.ul_carrier_freq_r10_present, 1));
    HANDLE_CODE(bref.pack(ul_cfg_r10.ul_freq_info_r10.ul_bw_r10_present, 1));
    if (ul_cfg_r10.ul_freq_info_r10.ul_carrier_freq_r10_present) {
      HANDLE_CODE(pack_integer(bref, ul_cfg_r10.ul_freq_info_r10.ul_carrier_freq_r10, (uint32_t)0u, (uint32_t)65535u));
    }
    if (ul_cfg_r10.ul_freq_info_r10.ul_bw_r10_present) {
      HANDLE_CODE(ul_cfg_r10.ul_freq_info_r10.ul_bw_r10.pack(bref));
    }
    HANDLE_CODE(pack_integer(bref, ul_cfg_r10.ul_freq_info_r10.add_spec_emission_scell_r10, (uint8_t)1u, (uint8_t)32u));
    if (ul_cfg_r10.p_max_r10_present) {
      HANDLE_CODE(pack_integer(bref, ul_cfg_r10.p_max_r10, (int8_t)-30, (int8_t)33));
    }
    HANDLE_CODE(ul_cfg_r10.ul_pwr_ctrl_common_scell_r10.pack(bref));
    HANDLE_CODE(ul_cfg_r10.srs_ul_cfg_common_r10.pack(bref));
    HANDLE_CODE(ul_cfg_r10.ul_cp_len_r10.pack(bref));
    if (ul_cfg_r10.prach_cfg_scell_r10_present) {
      HANDLE_CODE(ul_cfg_r10.prach_cfg_scell_r10.pack(bref));
    }
    HANDLE_CODE(ul_cfg_r10.pusch_cfg_common_r10.pack(bref));
  }

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ul_carrier_freq_v1090_present;
    group_flags[1] |= rach_cfg_common_scell_r11.is_present();
    group_flags[1] |= prach_cfg_scell_r11.is_present();
    group_flags[1] |= tdd_cfg_v1130.is_present();
    group_flags[1] |= ul_pwr_ctrl_common_scell_v1130.is_present();
    group_flags[2] |= pusch_cfg_common_v1270.is_present();
    group_flags[3] |= pucch_cfg_common_r13.is_present();
    group_flags[3] |= ul_pwr_ctrl_common_scell_v1310.is_present();
    group_flags[4] |= high_speed_cfg_scell_r14.is_present();
    group_flags[4] |= prach_cfg_v1430.is_present();
    group_flags[4] |= ul_cfg_r14.is_present();
    group_flags[4] |= harq_ref_cfg_r14_present;
    group_flags[4] |= srs_flex_timing_r14_present;
    group_flags[5] |= mbsfn_sf_cfg_list_v1430.is_present();
    group_flags[6] |= ul_pwr_ctrl_common_scell_v1530.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ul_carrier_freq_v1090_present, 1));
      if (ul_carrier_freq_v1090_present) {
        HANDLE_CODE(pack_integer(bref, ul_carrier_freq_v1090, (uint32_t)65536u, (uint32_t)262143u));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(rach_cfg_common_scell_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(prach_cfg_scell_r11.is_present(), 1));
      HANDLE_CODE(bref.pack(tdd_cfg_v1130.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_pwr_ctrl_common_scell_v1130.is_present(), 1));
      if (rach_cfg_common_scell_r11.is_present()) {
        HANDLE_CODE(rach_cfg_common_scell_r11->pack(bref));
      }
      if (prach_cfg_scell_r11.is_present()) {
        HANDLE_CODE(prach_cfg_scell_r11->pack(bref));
      }
      if (tdd_cfg_v1130.is_present()) {
        HANDLE_CODE(tdd_cfg_v1130->pack(bref));
      }
      if (ul_pwr_ctrl_common_scell_v1130.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_scell_v1130->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pusch_cfg_common_v1270.is_present(), 1));
      if (pusch_cfg_common_v1270.is_present()) {
        HANDLE_CODE(pusch_cfg_common_v1270->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pucch_cfg_common_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_pwr_ctrl_common_scell_v1310.is_present(), 1));
      if (pucch_cfg_common_r13.is_present()) {
        HANDLE_CODE(pucch_cfg_common_r13->pack(bref));
      }
      if (ul_pwr_ctrl_common_scell_v1310.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_scell_v1310->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(high_speed_cfg_scell_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(prach_cfg_v1430.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_cfg_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(harq_ref_cfg_r14_present, 1));
      HANDLE_CODE(bref.pack(srs_flex_timing_r14_present, 1));
      if (high_speed_cfg_scell_r14.is_present()) {
        HANDLE_CODE(high_speed_cfg_scell_r14->pack(bref));
      }
      if (prach_cfg_v1430.is_present()) {
        HANDLE_CODE(prach_cfg_v1430->pack(bref));
      }
      if (ul_cfg_r14.is_present()) {
        HANDLE_CODE(bref.pack(ul_cfg_r14->p_max_r14_present, 1));
        HANDLE_CODE(bref.pack(ul_cfg_r14->prach_cfg_scell_r14_present, 1));
        HANDLE_CODE(bref.pack(ul_cfg_r14->ul_pwr_ctrl_common_pusch_less_cell_v1430_present, 1));
        HANDLE_CODE(bref.pack(ul_cfg_r14->ul_freq_info_r14.ul_carrier_freq_r14_present, 1));
        HANDLE_CODE(bref.pack(ul_cfg_r14->ul_freq_info_r14.ul_bw_r14_present, 1));
        if (ul_cfg_r14->ul_freq_info_r14.ul_carrier_freq_r14_present) {
          HANDLE_CODE(
              pack_integer(bref, ul_cfg_r14->ul_freq_info_r14.ul_carrier_freq_r14, (uint32_t)0u, (uint32_t)262143u));
        }
        if (ul_cfg_r14->ul_freq_info_r14.ul_bw_r14_present) {
          HANDLE_CODE(ul_cfg_r14->ul_freq_info_r14.ul_bw_r14.pack(bref));
        }
        HANDLE_CODE(
            pack_integer(bref, ul_cfg_r14->ul_freq_info_r14.add_spec_emission_scell_r14, (uint8_t)1u, (uint8_t)32u));
        if (ul_cfg_r14->p_max_r14_present) {
          HANDLE_CODE(pack_integer(bref, ul_cfg_r14->p_max_r14, (int8_t)-30, (int8_t)33));
        }
        HANDLE_CODE(ul_cfg_r14->srs_ul_cfg_common_r14.pack(bref));
        HANDLE_CODE(ul_cfg_r14->ul_cp_len_r14.pack(bref));
        if (ul_cfg_r14->prach_cfg_scell_r14_present) {
          HANDLE_CODE(ul_cfg_r14->prach_cfg_scell_r14.pack(bref));
        }
        if (ul_cfg_r14->ul_pwr_ctrl_common_pusch_less_cell_v1430_present) {
          HANDLE_CODE(ul_cfg_r14->ul_pwr_ctrl_common_pusch_less_cell_v1430.pack(bref));
        }
      }
      if (harq_ref_cfg_r14_present) {
        HANDLE_CODE(harq_ref_cfg_r14.pack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(mbsfn_sf_cfg_list_v1430.is_present(), 1));
      if (mbsfn_sf_cfg_list_v1430.is_present()) {
        HANDLE_CODE(pack_dyn_seq_of(bref, *mbsfn_sf_cfg_list_v1430, 1, 8));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ul_pwr_ctrl_common_scell_v1530.is_present(), 1));
      if (ul_pwr_ctrl_common_scell_v1530.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_scell_v1530->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_common_scell_r10_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(ul_cfg_r10_present, 1));

  HANDLE_CODE(bref.unpack(non_ul_cfg_r10.mbsfn_sf_cfg_list_r10_present, 1));
  HANDLE_CODE(bref.unpack(non_ul_cfg_r10.tdd_cfg_r10_present, 1));
  HANDLE_CODE(non_ul_cfg_r10.dl_bw_r10.unpack(bref));
  HANDLE_CODE(non_ul_cfg_r10.ant_info_common_r10.unpack(bref));
  if (non_ul_cfg_r10.mbsfn_sf_cfg_list_r10_present) {
    HANDLE_CODE(unpack_dyn_seq_of(non_ul_cfg_r10.mbsfn_sf_cfg_list_r10, bref, 1, 8));
  }
  HANDLE_CODE(non_ul_cfg_r10.phich_cfg_r10.unpack(bref));
  HANDLE_CODE(non_ul_cfg_r10.pdsch_cfg_common_r10.unpack(bref));
  if (non_ul_cfg_r10.tdd_cfg_r10_present) {
    HANDLE_CODE(non_ul_cfg_r10.tdd_cfg_r10.unpack(bref));
  }
  if (ul_cfg_r10_present) {
    HANDLE_CODE(bref.unpack(ul_cfg_r10.p_max_r10_present, 1));
    HANDLE_CODE(bref.unpack(ul_cfg_r10.prach_cfg_scell_r10_present, 1));
    HANDLE_CODE(bref.unpack(ul_cfg_r10.ul_freq_info_r10.ul_carrier_freq_r10_present, 1));
    HANDLE_CODE(bref.unpack(ul_cfg_r10.ul_freq_info_r10.ul_bw_r10_present, 1));
    if (ul_cfg_r10.ul_freq_info_r10.ul_carrier_freq_r10_present) {
      HANDLE_CODE(
          unpack_integer(ul_cfg_r10.ul_freq_info_r10.ul_carrier_freq_r10, bref, (uint32_t)0u, (uint32_t)65535u));
    }
    if (ul_cfg_r10.ul_freq_info_r10.ul_bw_r10_present) {
      HANDLE_CODE(ul_cfg_r10.ul_freq_info_r10.ul_bw_r10.unpack(bref));
    }
    HANDLE_CODE(
        unpack_integer(ul_cfg_r10.ul_freq_info_r10.add_spec_emission_scell_r10, bref, (uint8_t)1u, (uint8_t)32u));
    if (ul_cfg_r10.p_max_r10_present) {
      HANDLE_CODE(unpack_integer(ul_cfg_r10.p_max_r10, bref, (int8_t)-30, (int8_t)33));
    }
    HANDLE_CODE(ul_cfg_r10.ul_pwr_ctrl_common_scell_r10.unpack(bref));
    HANDLE_CODE(ul_cfg_r10.srs_ul_cfg_common_r10.unpack(bref));
    HANDLE_CODE(ul_cfg_r10.ul_cp_len_r10.unpack(bref));
    if (ul_cfg_r10.prach_cfg_scell_r10_present) {
      HANDLE_CODE(ul_cfg_r10.prach_cfg_scell_r10.unpack(bref));
    }
    HANDLE_CODE(ul_cfg_r10.pusch_cfg_common_r10.unpack(bref));
  }

  if (ext) {
    ext_groups_unpacker_guard group_flags(7);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.unpack(ul_carrier_freq_v1090_present, 1));
      if (ul_carrier_freq_v1090_present) {
        HANDLE_CODE(unpack_integer(ul_carrier_freq_v1090, bref, (uint32_t)65536u, (uint32_t)262143u));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool rach_cfg_common_scell_r11_present;
      HANDLE_CODE(bref.unpack(rach_cfg_common_scell_r11_present, 1));
      rach_cfg_common_scell_r11.set_present(rach_cfg_common_scell_r11_present);
      bool prach_cfg_scell_r11_present;
      HANDLE_CODE(bref.unpack(prach_cfg_scell_r11_present, 1));
      prach_cfg_scell_r11.set_present(prach_cfg_scell_r11_present);
      bool tdd_cfg_v1130_present;
      HANDLE_CODE(bref.unpack(tdd_cfg_v1130_present, 1));
      tdd_cfg_v1130.set_present(tdd_cfg_v1130_present);
      bool ul_pwr_ctrl_common_scell_v1130_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_common_scell_v1130_present, 1));
      ul_pwr_ctrl_common_scell_v1130.set_present(ul_pwr_ctrl_common_scell_v1130_present);
      if (rach_cfg_common_scell_r11.is_present()) {
        HANDLE_CODE(rach_cfg_common_scell_r11->unpack(bref));
      }
      if (prach_cfg_scell_r11.is_present()) {
        HANDLE_CODE(prach_cfg_scell_r11->unpack(bref));
      }
      if (tdd_cfg_v1130.is_present()) {
        HANDLE_CODE(tdd_cfg_v1130->unpack(bref));
      }
      if (ul_pwr_ctrl_common_scell_v1130.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_scell_v1130->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool pusch_cfg_common_v1270_present;
      HANDLE_CODE(bref.unpack(pusch_cfg_common_v1270_present, 1));
      pusch_cfg_common_v1270.set_present(pusch_cfg_common_v1270_present);
      if (pusch_cfg_common_v1270.is_present()) {
        HANDLE_CODE(pusch_cfg_common_v1270->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool pucch_cfg_common_r13_present;
      HANDLE_CODE(bref.unpack(pucch_cfg_common_r13_present, 1));
      pucch_cfg_common_r13.set_present(pucch_cfg_common_r13_present);
      bool ul_pwr_ctrl_common_scell_v1310_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_common_scell_v1310_present, 1));
      ul_pwr_ctrl_common_scell_v1310.set_present(ul_pwr_ctrl_common_scell_v1310_present);
      if (pucch_cfg_common_r13.is_present()) {
        HANDLE_CODE(pucch_cfg_common_r13->unpack(bref));
      }
      if (ul_pwr_ctrl_common_scell_v1310.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_scell_v1310->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool high_speed_cfg_scell_r14_present;
      HANDLE_CODE(bref.unpack(high_speed_cfg_scell_r14_present, 1));
      high_speed_cfg_scell_r14.set_present(high_speed_cfg_scell_r14_present);
      bool prach_cfg_v1430_present;
      HANDLE_CODE(bref.unpack(prach_cfg_v1430_present, 1));
      prach_cfg_v1430.set_present(prach_cfg_v1430_present);
      bool ul_cfg_r14_present;
      HANDLE_CODE(bref.unpack(ul_cfg_r14_present, 1));
      ul_cfg_r14.set_present(ul_cfg_r14_present);
      HANDLE_CODE(bref.unpack(harq_ref_cfg_r14_present, 1));
      HANDLE_CODE(bref.unpack(srs_flex_timing_r14_present, 1));
      if (high_speed_cfg_scell_r14.is_present()) {
        HANDLE_CODE(high_speed_cfg_scell_r14->unpack(bref));
      }
      if (prach_cfg_v1430.is_present()) {
        HANDLE_CODE(prach_cfg_v1430->unpack(bref));
      }
      if (ul_cfg_r14.is_present()) {
        HANDLE_CODE(bref.unpack(ul_cfg_r14->p_max_r14_present, 1));
        HANDLE_CODE(bref.unpack(ul_cfg_r14->prach_cfg_scell_r14_present, 1));
        HANDLE_CODE(bref.unpack(ul_cfg_r14->ul_pwr_ctrl_common_pusch_less_cell_v1430_present, 1));
        HANDLE_CODE(bref.unpack(ul_cfg_r14->ul_freq_info_r14.ul_carrier_freq_r14_present, 1));
        HANDLE_CODE(bref.unpack(ul_cfg_r14->ul_freq_info_r14.ul_bw_r14_present, 1));
        if (ul_cfg_r14->ul_freq_info_r14.ul_carrier_freq_r14_present) {
          HANDLE_CODE(
              unpack_integer(ul_cfg_r14->ul_freq_info_r14.ul_carrier_freq_r14, bref, (uint32_t)0u, (uint32_t)262143u));
        }
        if (ul_cfg_r14->ul_freq_info_r14.ul_bw_r14_present) {
          HANDLE_CODE(ul_cfg_r14->ul_freq_info_r14.ul_bw_r14.unpack(bref));
        }
        HANDLE_CODE(
            unpack_integer(ul_cfg_r14->ul_freq_info_r14.add_spec_emission_scell_r14, bref, (uint8_t)1u, (uint8_t)32u));
        if (ul_cfg_r14->p_max_r14_present) {
          HANDLE_CODE(unpack_integer(ul_cfg_r14->p_max_r14, bref, (int8_t)-30, (int8_t)33));
        }
        HANDLE_CODE(ul_cfg_r14->srs_ul_cfg_common_r14.unpack(bref));
        HANDLE_CODE(ul_cfg_r14->ul_cp_len_r14.unpack(bref));
        if (ul_cfg_r14->prach_cfg_scell_r14_present) {
          HANDLE_CODE(ul_cfg_r14->prach_cfg_scell_r14.unpack(bref));
        }
        if (ul_cfg_r14->ul_pwr_ctrl_common_pusch_less_cell_v1430_present) {
          HANDLE_CODE(ul_cfg_r14->ul_pwr_ctrl_common_pusch_less_cell_v1430.unpack(bref));
        }
      }
      if (harq_ref_cfg_r14_present) {
        HANDLE_CODE(harq_ref_cfg_r14.unpack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool mbsfn_sf_cfg_list_v1430_present;
      HANDLE_CODE(bref.unpack(mbsfn_sf_cfg_list_v1430_present, 1));
      mbsfn_sf_cfg_list_v1430.set_present(mbsfn_sf_cfg_list_v1430_present);
      if (mbsfn_sf_cfg_list_v1430.is_present()) {
        HANDLE_CODE(unpack_dyn_seq_of(*mbsfn_sf_cfg_list_v1430, bref, 1, 8));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ul_pwr_ctrl_common_scell_v1530_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_common_scell_v1530_present, 1));
      ul_pwr_ctrl_common_scell_v1530.set_present(ul_pwr_ctrl_common_scell_v1530_present);
      if (ul_pwr_ctrl_common_scell_v1530.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_scell_v1530->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rr_cfg_common_scell_r10_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("nonUL-Configuration-r10");
  j.start_obj();
  j.write_str("dl-Bandwidth-r10", non_ul_cfg_r10.dl_bw_r10.to_string());
  j.write_fieldname("antennaInfoCommon-r10");
  non_ul_cfg_r10.ant_info_common_r10.to_json(j);
  if (non_ul_cfg_r10.mbsfn_sf_cfg_list_r10_present) {
    j.start_array("mbsfn-SubframeConfigList-r10");
    for (const auto& e1 : non_ul_cfg_r10.mbsfn_sf_cfg_list_r10) {
      e1.to_json(j);
    }
    j.end_array();
  }
  j.write_fieldname("phich-Config-r10");
  non_ul_cfg_r10.phich_cfg_r10.to_json(j);
  j.write_fieldname("pdsch-ConfigCommon-r10");
  non_ul_cfg_r10.pdsch_cfg_common_r10.to_json(j);
  if (non_ul_cfg_r10.tdd_cfg_r10_present) {
    j.write_fieldname("tdd-Config-r10");
    non_ul_cfg_r10.tdd_cfg_r10.to_json(j);
  }
  j.end_obj();
  if (ul_cfg_r10_present) {
    j.write_fieldname("ul-Configuration-r10");
    j.start_obj();
    j.write_fieldname("ul-FreqInfo-r10");
    j.start_obj();
    if (ul_cfg_r10.ul_freq_info_r10.ul_carrier_freq_r10_present) {
      j.write_int("ul-CarrierFreq-r10", ul_cfg_r10.ul_freq_info_r10.ul_carrier_freq_r10);
    }
    if (ul_cfg_r10.ul_freq_info_r10.ul_bw_r10_present) {
      j.write_str("ul-Bandwidth-r10", ul_cfg_r10.ul_freq_info_r10.ul_bw_r10.to_string());
    }
    j.write_int("additionalSpectrumEmissionSCell-r10", ul_cfg_r10.ul_freq_info_r10.add_spec_emission_scell_r10);
    j.end_obj();
    if (ul_cfg_r10.p_max_r10_present) {
      j.write_int("p-Max-r10", ul_cfg_r10.p_max_r10);
    }
    j.write_fieldname("uplinkPowerControlCommonSCell-r10");
    ul_cfg_r10.ul_pwr_ctrl_common_scell_r10.to_json(j);
    j.write_fieldname("soundingRS-UL-ConfigCommon-r10");
    ul_cfg_r10.srs_ul_cfg_common_r10.to_json(j);
    j.write_str("ul-CyclicPrefixLength-r10", ul_cfg_r10.ul_cp_len_r10.to_string());
    if (ul_cfg_r10.prach_cfg_scell_r10_present) {
      j.write_fieldname("prach-ConfigSCell-r10");
      ul_cfg_r10.prach_cfg_scell_r10.to_json(j);
    }
    j.write_fieldname("pusch-ConfigCommon-r10");
    ul_cfg_r10.pusch_cfg_common_r10.to_json(j);
    j.end_obj();
  }
  if (ext) {
    if (ul_carrier_freq_v1090_present) {
      j.write_int("ul-CarrierFreq-v1090", ul_carrier_freq_v1090);
    }
    if (rach_cfg_common_scell_r11.is_present()) {
      j.write_fieldname("rach-ConfigCommonSCell-r11");
      rach_cfg_common_scell_r11->to_json(j);
    }
    if (prach_cfg_scell_r11.is_present()) {
      j.write_fieldname("prach-ConfigSCell-r11");
      prach_cfg_scell_r11->to_json(j);
    }
    if (tdd_cfg_v1130.is_present()) {
      j.write_fieldname("tdd-Config-v1130");
      tdd_cfg_v1130->to_json(j);
    }
    if (ul_pwr_ctrl_common_scell_v1130.is_present()) {
      j.write_fieldname("uplinkPowerControlCommonSCell-v1130");
      ul_pwr_ctrl_common_scell_v1130->to_json(j);
    }
    if (pusch_cfg_common_v1270.is_present()) {
      j.write_fieldname("pusch-ConfigCommon-v1270");
      pusch_cfg_common_v1270->to_json(j);
    }
    if (pucch_cfg_common_r13.is_present()) {
      j.write_fieldname("pucch-ConfigCommon-r13");
      pucch_cfg_common_r13->to_json(j);
    }
    if (ul_pwr_ctrl_common_scell_v1310.is_present()) {
      j.write_fieldname("uplinkPowerControlCommonSCell-v1310");
      ul_pwr_ctrl_common_scell_v1310->to_json(j);
    }
    if (high_speed_cfg_scell_r14.is_present()) {
      j.write_fieldname("highSpeedConfigSCell-r14");
      high_speed_cfg_scell_r14->to_json(j);
    }
    if (prach_cfg_v1430.is_present()) {
      j.write_fieldname("prach-Config-v1430");
      prach_cfg_v1430->to_json(j);
    }
    if (ul_cfg_r14.is_present()) {
      j.write_fieldname("ul-Configuration-r14");
      j.start_obj();
      j.write_fieldname("ul-FreqInfo-r14");
      j.start_obj();
      if (ul_cfg_r14->ul_freq_info_r14.ul_carrier_freq_r14_present) {
        j.write_int("ul-CarrierFreq-r14", ul_cfg_r14->ul_freq_info_r14.ul_carrier_freq_r14);
      }
      if (ul_cfg_r14->ul_freq_info_r14.ul_bw_r14_present) {
        j.write_str("ul-Bandwidth-r14", ul_cfg_r14->ul_freq_info_r14.ul_bw_r14.to_string());
      }
      j.write_int("additionalSpectrumEmissionSCell-r14", ul_cfg_r14->ul_freq_info_r14.add_spec_emission_scell_r14);
      j.end_obj();
      if (ul_cfg_r14->p_max_r14_present) {
        j.write_int("p-Max-r14", ul_cfg_r14->p_max_r14);
      }
      j.write_fieldname("soundingRS-UL-ConfigCommon-r14");
      ul_cfg_r14->srs_ul_cfg_common_r14.to_json(j);
      j.write_str("ul-CyclicPrefixLength-r14", ul_cfg_r14->ul_cp_len_r14.to_string());
      if (ul_cfg_r14->prach_cfg_scell_r14_present) {
        j.write_fieldname("prach-ConfigSCell-r14");
        ul_cfg_r14->prach_cfg_scell_r14.to_json(j);
      }
      if (ul_cfg_r14->ul_pwr_ctrl_common_pusch_less_cell_v1430_present) {
        j.write_fieldname("uplinkPowerControlCommonPUSCH-LessCell-v1430");
        ul_cfg_r14->ul_pwr_ctrl_common_pusch_less_cell_v1430.to_json(j);
      }
      j.end_obj();
    }
    if (harq_ref_cfg_r14_present) {
      j.write_str("harq-ReferenceConfig-r14", harq_ref_cfg_r14.to_string());
    }
    if (srs_flex_timing_r14_present) {
      j.write_str("soundingRS-FlexibleTiming-r14", "true");
    }
    if (mbsfn_sf_cfg_list_v1430.is_present()) {
      j.start_array("mbsfn-SubframeConfigList-v1430");
      for (const auto& e1 : *mbsfn_sf_cfg_list_v1430) {
        e1.to_json(j);
      }
      j.end_array();
    }
    if (ul_pwr_ctrl_common_scell_v1530.is_present()) {
      j.write_fieldname("uplinkPowerControlCommonSCell-v1530");
      ul_pwr_ctrl_common_scell_v1530->to_json(j);
    }
  }
  j.end_obj();
}
bool rr_cfg_common_scell_r10_s::operator==(const rr_cfg_common_scell_r10_s& other) const
{
  return ext == other.ext and non_ul_cfg_r10.dl_bw_r10 == other.non_ul_cfg_r10.dl_bw_r10 and
         non_ul_cfg_r10.ant_info_common_r10 == other.non_ul_cfg_r10.ant_info_common_r10 and
         non_ul_cfg_r10.mbsfn_sf_cfg_list_r10_present == other.non_ul_cfg_r10.mbsfn_sf_cfg_list_r10_present and
         (not non_ul_cfg_r10.mbsfn_sf_cfg_list_r10_present or
          non_ul_cfg_r10.mbsfn_sf_cfg_list_r10 == other.non_ul_cfg_r10.mbsfn_sf_cfg_list_r10) and
         non_ul_cfg_r10.phich_cfg_r10 == other.non_ul_cfg_r10.phich_cfg_r10 and
         non_ul_cfg_r10.pdsch_cfg_common_r10 == other.non_ul_cfg_r10.pdsch_cfg_common_r10 and
         non_ul_cfg_r10.tdd_cfg_r10_present == other.non_ul_cfg_r10.tdd_cfg_r10_present and
         (not non_ul_cfg_r10.tdd_cfg_r10_present or non_ul_cfg_r10.tdd_cfg_r10 == other.non_ul_cfg_r10.tdd_cfg_r10) and
         ul_cfg_r10.ul_freq_info_r10.ul_carrier_freq_r10_present ==
             other.ul_cfg_r10.ul_freq_info_r10.ul_carrier_freq_r10_present and
         (not ul_cfg_r10.ul_freq_info_r10.ul_carrier_freq_r10_present or
          ul_cfg_r10.ul_freq_info_r10.ul_carrier_freq_r10 == other.ul_cfg_r10.ul_freq_info_r10.ul_carrier_freq_r10) and
         ul_cfg_r10.ul_freq_info_r10.ul_bw_r10_present == other.ul_cfg_r10.ul_freq_info_r10.ul_bw_r10_present and
         (not ul_cfg_r10.ul_freq_info_r10.ul_bw_r10_present or
          ul_cfg_r10.ul_freq_info_r10.ul_bw_r10 == other.ul_cfg_r10.ul_freq_info_r10.ul_bw_r10) and
         ul_cfg_r10.ul_freq_info_r10.add_spec_emission_scell_r10 ==
             other.ul_cfg_r10.ul_freq_info_r10.add_spec_emission_scell_r10 and
         ul_cfg_r10.p_max_r10_present == other.ul_cfg_r10.p_max_r10_present and
         (not ul_cfg_r10.p_max_r10_present or ul_cfg_r10.p_max_r10 == other.ul_cfg_r10.p_max_r10) and
         ul_cfg_r10.ul_pwr_ctrl_common_scell_r10 == other.ul_cfg_r10.ul_pwr_ctrl_common_scell_r10 and
         ul_cfg_r10.srs_ul_cfg_common_r10 == other.ul_cfg_r10.srs_ul_cfg_common_r10 and
         ul_cfg_r10.ul_cp_len_r10 == other.ul_cfg_r10.ul_cp_len_r10 and
         ul_cfg_r10.prach_cfg_scell_r10_present == other.ul_cfg_r10.prach_cfg_scell_r10_present and
         (not ul_cfg_r10.prach_cfg_scell_r10_present or
          ul_cfg_r10.prach_cfg_scell_r10 == other.ul_cfg_r10.prach_cfg_scell_r10) and
         ul_cfg_r10.pusch_cfg_common_r10 == other.ul_cfg_r10.pusch_cfg_common_r10 and
         (not ext or
          (ul_carrier_freq_v1090_present == other.ul_carrier_freq_v1090_present and
           (not ul_carrier_freq_v1090_present or ul_carrier_freq_v1090 == other.ul_carrier_freq_v1090) and
           rach_cfg_common_scell_r11.is_present() == other.rach_cfg_common_scell_r11.is_present() and
           (not rach_cfg_common_scell_r11.is_present() or
            *rach_cfg_common_scell_r11 == *other.rach_cfg_common_scell_r11) and
           prach_cfg_scell_r11.is_present() == other.prach_cfg_scell_r11.is_present() and
           (not prach_cfg_scell_r11.is_present() or *prach_cfg_scell_r11 == *other.prach_cfg_scell_r11) and
           tdd_cfg_v1130.is_present() == other.tdd_cfg_v1130.is_present() and
           (not tdd_cfg_v1130.is_present() or *tdd_cfg_v1130 == *other.tdd_cfg_v1130) and
           ul_pwr_ctrl_common_scell_v1130.is_present() == other.ul_pwr_ctrl_common_scell_v1130.is_present() and
           (not ul_pwr_ctrl_common_scell_v1130.is_present() or
            *ul_pwr_ctrl_common_scell_v1130 == *other.ul_pwr_ctrl_common_scell_v1130) and
           pusch_cfg_common_v1270.is_present() == other.pusch_cfg_common_v1270.is_present() and
           (not pusch_cfg_common_v1270.is_present() or *pusch_cfg_common_v1270 == *other.pusch_cfg_common_v1270) and
           pucch_cfg_common_r13.is_present() == other.pucch_cfg_common_r13.is_present() and
           (not pucch_cfg_common_r13.is_present() or *pucch_cfg_common_r13 == *other.pucch_cfg_common_r13) and
           ul_pwr_ctrl_common_scell_v1310.is_present() == other.ul_pwr_ctrl_common_scell_v1310.is_present() and
           (not ul_pwr_ctrl_common_scell_v1310.is_present() or
            *ul_pwr_ctrl_common_scell_v1310 == *other.ul_pwr_ctrl_common_scell_v1310) and
           high_speed_cfg_scell_r14.is_present() == other.high_speed_cfg_scell_r14.is_present() and
           (not high_speed_cfg_scell_r14.is_present() or
            *high_speed_cfg_scell_r14 == *other.high_speed_cfg_scell_r14) and
           prach_cfg_v1430.is_present() == other.prach_cfg_v1430.is_present() and
           (not prach_cfg_v1430.is_present() or *prach_cfg_v1430 == *other.prach_cfg_v1430) and
           ul_cfg_r14->ul_freq_info_r14.ul_carrier_freq_r14_present ==
               other.ul_cfg_r14->ul_freq_info_r14.ul_carrier_freq_r14_present and
           (not ul_cfg_r14->ul_freq_info_r14.ul_carrier_freq_r14_present or
            ul_cfg_r14->ul_freq_info_r14.ul_carrier_freq_r14 ==
                other.ul_cfg_r14->ul_freq_info_r14.ul_carrier_freq_r14) and
           ul_cfg_r14->ul_freq_info_r14.ul_bw_r14_present == other.ul_cfg_r14->ul_freq_info_r14.ul_bw_r14_present and
           (not ul_cfg_r14->ul_freq_info_r14.ul_bw_r14_present or
            ul_cfg_r14->ul_freq_info_r14.ul_bw_r14 == other.ul_cfg_r14->ul_freq_info_r14.ul_bw_r14) and
           ul_cfg_r14->ul_freq_info_r14.add_spec_emission_scell_r14 ==
               other.ul_cfg_r14->ul_freq_info_r14.add_spec_emission_scell_r14 and
           ul_cfg_r14->p_max_r14_present == other.ul_cfg_r14->p_max_r14_present and
           (not ul_cfg_r14->p_max_r14_present or ul_cfg_r14->p_max_r14 == other.ul_cfg_r14->p_max_r14) and
           ul_cfg_r14->srs_ul_cfg_common_r14 == other.ul_cfg_r14->srs_ul_cfg_common_r14 and
           ul_cfg_r14->ul_cp_len_r14 == other.ul_cfg_r14->ul_cp_len_r14 and
           ul_cfg_r14->prach_cfg_scell_r14_present == other.ul_cfg_r14->prach_cfg_scell_r14_present and
           (not ul_cfg_r14->prach_cfg_scell_r14_present or
            ul_cfg_r14->prach_cfg_scell_r14 == other.ul_cfg_r14->prach_cfg_scell_r14) and
           ul_cfg_r14->ul_pwr_ctrl_common_pusch_less_cell_v1430_present ==
               other.ul_cfg_r14->ul_pwr_ctrl_common_pusch_less_cell_v1430_present and
           (not ul_cfg_r14->ul_pwr_ctrl_common_pusch_less_cell_v1430_present or
            ul_cfg_r14->ul_pwr_ctrl_common_pusch_less_cell_v1430 ==
                other.ul_cfg_r14->ul_pwr_ctrl_common_pusch_less_cell_v1430) and
           harq_ref_cfg_r14_present == other.harq_ref_cfg_r14_present and
           (not harq_ref_cfg_r14_present or harq_ref_cfg_r14 == other.harq_ref_cfg_r14) and
           srs_flex_timing_r14_present == other.srs_flex_timing_r14_present and
           mbsfn_sf_cfg_list_v1430.is_present() == other.mbsfn_sf_cfg_list_v1430.is_present() and
           (not mbsfn_sf_cfg_list_v1430.is_present() or *mbsfn_sf_cfg_list_v1430 == *other.mbsfn_sf_cfg_list_v1430) and
           ul_pwr_ctrl_common_scell_v1530.is_present() == other.ul_pwr_ctrl_common_scell_v1530.is_present() and
           (not ul_pwr_ctrl_common_scell_v1530.is_present() or
            *ul_pwr_ctrl_common_scell_v1530 == *other.ul_pwr_ctrl_common_scell_v1530)));
}

const char* rr_cfg_common_scell_r10_s::non_ul_cfg_r10_s_::dl_bw_r10_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(options, 6, value, "rr_cfg_common_scell_r10_s::non_ul_cfg_r10_s_::dl_bw_r10_e_");
}
uint8_t rr_cfg_common_scell_r10_s::non_ul_cfg_r10_s_::dl_bw_r10_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(options, 6, value, "rr_cfg_common_scell_r10_s::non_ul_cfg_r10_s_::dl_bw_r10_e_");
}

const char* rr_cfg_common_scell_r10_s::ul_cfg_r10_s_::ul_freq_info_r10_s_::ul_bw_r10_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(
      options, 6, value, "rr_cfg_common_scell_r10_s::ul_cfg_r10_s_::ul_freq_info_r10_s_::ul_bw_r10_e_");
}
uint8_t rr_cfg_common_scell_r10_s::ul_cfg_r10_s_::ul_freq_info_r10_s_::ul_bw_r10_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(
      options, 6, value, "rr_cfg_common_scell_r10_s::ul_cfg_r10_s_::ul_freq_info_r10_s_::ul_bw_r10_e_");
}

const char* rr_cfg_common_scell_r10_s::ul_cfg_r14_s_::ul_freq_info_r14_s_::ul_bw_r14_opts::to_string() const
{
  static const char* options[] = {"n6", "n15", "n25", "n50", "n75", "n100"};
  return convert_enum_idx(
      options, 6, value, "rr_cfg_common_scell_r10_s::ul_cfg_r14_s_::ul_freq_info_r14_s_::ul_bw_r14_e_");
}
uint8_t rr_cfg_common_scell_r10_s::ul_cfg_r14_s_::ul_freq_info_r14_s_::ul_bw_r14_opts::to_number() const
{
  static const uint8_t options[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(
      options, 6, value, "rr_cfg_common_scell_r10_s::ul_cfg_r14_s_::ul_freq_info_r14_s_::ul_bw_r14_e_");
}

const char* rr_cfg_common_scell_r10_s::harq_ref_cfg_r14_opts::to_string() const
{
  static const char* options[] = {"sa2", "sa4", "sa5"};
  return convert_enum_idx(options, 3, value, "rr_cfg_common_scell_r10_s::harq_ref_cfg_r14_e_");
}
uint8_t rr_cfg_common_scell_r10_s::harq_ref_cfg_r14_opts::to_number() const
{
  static const uint8_t options[] = {2, 4, 5};
  return map_enum_number(options, 3, value, "rr_cfg_common_scell_r10_s::harq_ref_cfg_r14_e_");
}

// UplinkPowerControlCommon-v1310 ::= SEQUENCE
SRSASN_CODE ul_pwr_ctrl_common_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(delta_f_pucch_format4_r13_present, 1));
  HANDLE_CODE(bref.pack(delta_f_pucch_format5_minus13_present, 1));

  if (delta_f_pucch_format4_r13_present) {
    HANDLE_CODE(delta_f_pucch_format4_r13.pack(bref));
  }
  if (delta_f_pucch_format5_minus13_present) {
    HANDLE_CODE(delta_f_pucch_format5_minus13.pack(bref));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE ul_pwr_ctrl_common_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(delta_f_pucch_format4_r13_present, 1));
  HANDLE_CODE(bref.unpack(delta_f_pucch_format5_minus13_present, 1));

  if (delta_f_pucch_format4_r13_present) {
    HANDLE_CODE(delta_f_pucch_format4_r13.unpack(bref));
  }
  if (delta_f_pucch_format5_minus13_present) {
    HANDLE_CODE(delta_f_pucch_format5_minus13.unpack(bref));
  }

  return SRSASN_SUCCESS;
}
void ul_pwr_ctrl_common_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (delta_f_pucch_format4_r13_present) {
    j.write_str("deltaF-PUCCH-Format4-r13", delta_f_pucch_format4_r13.to_string());
  }
  if (delta_f_pucch_format5_minus13_present) {
    j.write_str("deltaF-PUCCH-Format5-13", delta_f_pucch_format5_minus13.to_string());
  }
  j.end_obj();
}

const char* ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format4_r13_opts::to_string() const
{
  static const char* options[] = {
      "deltaF16", "deltaF15", "deltaF14", "deltaF13", "deltaF12", "deltaF11", "deltaF10", "spare1"};
  return convert_enum_idx(options, 8, value, "ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format4_r13_e_");
}
uint8_t ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format4_r13_opts::to_number() const
{
  static const uint8_t options[] = {16, 15, 14, 13, 12, 11, 10};
  return map_enum_number(options, 7, value, "ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format4_r13_e_");
}

const char* ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format5_minus13_opts::to_string() const
{
  static const char* options[] = {
      "deltaF13", "deltaF12", "deltaF11", "deltaF10", "deltaF9", "deltaF8", "deltaF7", "spare1"};
  return convert_enum_idx(options, 8, value, "ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format5_minus13_e_");
}
uint8_t ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format5_minus13_opts::to_number() const
{
  static const uint8_t options[] = {13, 12, 11, 10, 9, 8, 7};
  return map_enum_number(options, 7, value, "ul_pwr_ctrl_common_v1310_s::delta_f_pucch_format5_minus13_e_");
}

// PRACH-Config-v1310 ::= SEQUENCE
SRSASN_CODE prach_cfg_v1310_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(rsrp_thress_prach_info_list_r13_present, 1));
  HANDLE_CODE(bref.pack(mpdcch_start_sf_css_ra_r13_present, 1));
  HANDLE_CODE(bref.pack(prach_hop_offset_r13_present, 1));
  HANDLE_CODE(bref.pack(prach_params_list_ce_r13_present, 1));
  HANDLE_CODE(bref.pack(init_ce_level_r13_present, 1));

  if (rsrp_thress_prach_info_list_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, rsrp_thress_prach_info_list_r13, 1, 3, integer_packer<uint8_t>(0, 97)));
  }
  if (mpdcch_start_sf_css_ra_r13_present) {
    HANDLE_CODE(mpdcch_start_sf_css_ra_r13.pack(bref));
  }
  if (prach_hop_offset_r13_present) {
    HANDLE_CODE(pack_integer(bref, prach_hop_offset_r13, (uint8_t)0u, (uint8_t)94u));
  }
  if (prach_params_list_ce_r13_present) {
    HANDLE_CODE(pack_dyn_seq_of(bref, prach_params_list_ce_r13, 1, 4));
  }
  if (init_ce_level_r13_present) {
    HANDLE_CODE(pack_integer(bref, init_ce_level_r13, (uint8_t)0u, (uint8_t)3u));
  }

  return SRSASN_SUCCESS;
}
SRSASN_CODE prach_cfg_v1310_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(rsrp_thress_prach_info_list_r13_present, 1));
  HANDLE_CODE(bref.unpack(mpdcch_start_sf_css_ra_r13_present, 1));
  HANDLE_CODE(bref.unpack(prach_hop_offset_r13_present, 1));
  HANDLE_CODE(bref.unpack(prach_params_list_ce_r13_present, 1));
  HANDLE_CODE(bref.unpack(init_ce_level_r13_present, 1));

  if (rsrp_thress_prach_info_list_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(rsrp_thress_prach_info_list_r13, bref, 1, 3, integer_packer<uint8_t>(0, 97)));
  }
  if (mpdcch_start_sf_css_ra_r13_present) {
    HANDLE_CODE(mpdcch_start_sf_css_ra_r13.unpack(bref));
  }
  if (prach_hop_offset_r13_present) {
    HANDLE_CODE(unpack_integer(prach_hop_offset_r13, bref, (uint8_t)0u, (uint8_t)94u));
  }
  if (prach_params_list_ce_r13_present) {
    HANDLE_CODE(unpack_dyn_seq_of(prach_params_list_ce_r13, bref, 1, 4));
  }
  if (init_ce_level_r13_present) {
    HANDLE_CODE(unpack_integer(init_ce_level_r13, bref, (uint8_t)0u, (uint8_t)3u));
  }

  return SRSASN_SUCCESS;
}
void prach_cfg_v1310_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rsrp_thress_prach_info_list_r13_present) {
    j.start_array("rsrp-ThresholdsPrachInfoList-r13");
    for (const auto& e1 : rsrp_thress_prach_info_list_r13) {
      j.write_int(e1);
    }
    j.end_array();
  }
  if (mpdcch_start_sf_css_ra_r13_present) {
    j.write_fieldname("mpdcch-startSF-CSS-RA-r13");
    mpdcch_start_sf_css_ra_r13.to_json(j);
  }
  if (prach_hop_offset_r13_present) {
    j.write_int("prach-HoppingOffset-r13", prach_hop_offset_r13);
  }
  if (prach_params_list_ce_r13_present) {
    j.start_array("prach-ParametersListCE-r13");
    for (const auto& e1 : prach_params_list_ce_r13) {
      e1.to_json(j);
    }
    j.end_array();
  }
  if (init_ce_level_r13_present) {
    j.write_int("initial-CE-level-r13", init_ce_level_r13);
  }
  j.end_obj();
}

void prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::destroy_() {}
void prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::set(types::options e)
{
  destroy_();
  type_ = e;
}
prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::mpdcch_start_sf_css_ra_r13_c_(
    const prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::fdd_r13:
      c.init(other.c.get<fdd_r13_e_>());
      break;
    case types::tdd_r13:
      c.init(other.c.get<tdd_r13_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_");
  }
}
prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_& prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::operator=(
    const prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::fdd_r13:
      c.set(other.c.get<fdd_r13_e_>());
      break;
    case types::tdd_r13:
      c.set(other.c.get<tdd_r13_e_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_");
  }

  return *this;
}
prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_&
prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::set_fdd_r13()
{
  set(types::fdd_r13);
  return c.get<fdd_r13_e_>();
}
prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_&
prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::set_tdd_r13()
{
  set(types::tdd_r13);
  return c.get<tdd_r13_e_>();
}
void prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::fdd_r13:
      j.write_str("fdd-r13", c.get<fdd_r13_e_>().to_string());
      break;
    case types::tdd_r13:
      j.write_str("tdd-r13", c.get<tdd_r13_e_>().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_");
  }
  j.end_obj();
}
SRSASN_CODE prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::fdd_r13:
      HANDLE_CODE(c.get<fdd_r13_e_>().pack(bref));
      break;
    case types::tdd_r13:
      HANDLE_CODE(c.get<tdd_r13_e_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::fdd_r13:
      HANDLE_CODE(c.get<fdd_r13_e_>().unpack(bref));
      break;
    case types::tdd_r13:
      HANDLE_CODE(c.get<tdd_r13_e_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}

const char* prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_opts::to_string() const
{
  static const char* options[] = {"v1", "v1dot5", "v2", "v2dot5", "v4", "v5", "v8", "v10"};
  return convert_enum_idx(options, 8, value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
}
float prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_opts::to_number() const
{
  static const float options[] = {1.0, 1.5, 2.0, 2.5, 4.0, 5.0, 8.0, 10.0};
  return map_enum_number(options, 8, value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
}
const char* prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_opts::to_number_string() const
{
  static const char* options[] = {"1", "1.5", "2", "2.5", "4", "5", "8", "10"};
  return convert_enum_idx(options, 8, value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::fdd_r13_e_");
}

const char* prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_opts::to_string() const
{
  static const char* options[] = {"v1", "v2", "v4", "v5", "v8", "v10", "v20", "spare"};
  return convert_enum_idx(options, 8, value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_");
}
uint8_t prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 5, 8, 10, 20};
  return map_enum_number(options, 7, value, "prach_cfg_v1310_s::mpdcch_start_sf_css_ra_r13_c_::tdd_r13_e_");
}

// RadioResourceConfigCommon ::= SEQUENCE
SRSASN_CODE rr_cfg_common_s::pack(bit_ref& bref) const
{
  bref.pack(ext, 1);
  HANDLE_CODE(bref.pack(rach_cfg_common_present, 1));
  HANDLE_CODE(bref.pack(pdsch_cfg_common_present, 1));
  HANDLE_CODE(bref.pack(phich_cfg_present, 1));
  HANDLE_CODE(bref.pack(pucch_cfg_common_present, 1));
  HANDLE_CODE(bref.pack(srs_ul_cfg_common_present, 1));
  HANDLE_CODE(bref.pack(ul_pwr_ctrl_common_present, 1));
  HANDLE_CODE(bref.pack(ant_info_common_present, 1));
  HANDLE_CODE(bref.pack(p_max_present, 1));
  HANDLE_CODE(bref.pack(tdd_cfg_present, 1));

  if (rach_cfg_common_present) {
    HANDLE_CODE(rach_cfg_common.pack(bref));
  }
  HANDLE_CODE(prach_cfg.pack(bref));
  if (pdsch_cfg_common_present) {
    HANDLE_CODE(pdsch_cfg_common.pack(bref));
  }
  HANDLE_CODE(pusch_cfg_common.pack(bref));
  if (phich_cfg_present) {
    HANDLE_CODE(phich_cfg.pack(bref));
  }
  if (pucch_cfg_common_present) {
    HANDLE_CODE(pucch_cfg_common.pack(bref));
  }
  if (srs_ul_cfg_common_present) {
    HANDLE_CODE(srs_ul_cfg_common.pack(bref));
  }
  if (ul_pwr_ctrl_common_present) {
    HANDLE_CODE(ul_pwr_ctrl_common.pack(bref));
  }
  if (ant_info_common_present) {
    HANDLE_CODE(ant_info_common.pack(bref));
  }
  if (p_max_present) {
    HANDLE_CODE(pack_integer(bref, p_max, (int8_t)-30, (int8_t)33));
  }
  if (tdd_cfg_present) {
    HANDLE_CODE(tdd_cfg.pack(bref));
  }
  HANDLE_CODE(ul_cp_len.pack(bref));

  if (ext) {
    ext_groups_packer_guard group_flags;
    group_flags[0] |= ul_pwr_ctrl_common_v1020.is_present();
    group_flags[1] |= tdd_cfg_v1130.is_present();
    group_flags[2] |= pusch_cfg_common_v1270.is_present();
    group_flags[3] |= prach_cfg_v1310.is_present();
    group_flags[3] |= freq_hop_params_r13.is_present();
    group_flags[3] |= pdsch_cfg_common_v1310.is_present();
    group_flags[3] |= pucch_cfg_common_v1310.is_present();
    group_flags[3] |= pusch_cfg_common_v1310.is_present();
    group_flags[3] |= ul_pwr_ctrl_common_v1310.is_present();
    group_flags[4] |= high_speed_cfg_r14.is_present();
    group_flags[4] |= prach_cfg_v1430.is_present();
    group_flags[4] |= pucch_cfg_common_v1430.is_present();
    group_flags[4] |= tdd_cfg_v1430.is_present();
    group_flags[5] |= tdd_cfg_v1450.is_present();
    group_flags[6] |= ul_pwr_ctrl_common_v1530.is_present();
    group_flags[6] |= high_speed_cfg_v1530.is_present();
    group_flags.pack(bref);

    if (group_flags[0]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ul_pwr_ctrl_common_v1020.is_present(), 1));
      if (ul_pwr_ctrl_common_v1020.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_v1020->pack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(tdd_cfg_v1130.is_present(), 1));
      if (tdd_cfg_v1130.is_present()) {
        HANDLE_CODE(tdd_cfg_v1130->pack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(pusch_cfg_common_v1270.is_present(), 1));
      if (pusch_cfg_common_v1270.is_present()) {
        HANDLE_CODE(pusch_cfg_common_v1270->pack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(prach_cfg_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(freq_hop_params_r13.is_present(), 1));
      HANDLE_CODE(bref.pack(pdsch_cfg_common_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(pucch_cfg_common_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(pusch_cfg_common_v1310.is_present(), 1));
      HANDLE_CODE(bref.pack(ul_pwr_ctrl_common_v1310.is_present(), 1));
      if (prach_cfg_v1310.is_present()) {
        HANDLE_CODE(prach_cfg_v1310->pack(bref));
      }
      if (freq_hop_params_r13.is_present()) {
        HANDLE_CODE(freq_hop_params_r13->pack(bref));
      }
      if (pdsch_cfg_common_v1310.is_present()) {
        HANDLE_CODE(pdsch_cfg_common_v1310->pack(bref));
      }
      if (pucch_cfg_common_v1310.is_present()) {
        HANDLE_CODE(pucch_cfg_common_v1310->pack(bref));
      }
      if (pusch_cfg_common_v1310.is_present()) {
        HANDLE_CODE(pusch_cfg_common_v1310->pack(bref));
      }
      if (ul_pwr_ctrl_common_v1310.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_v1310->pack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(high_speed_cfg_r14.is_present(), 1));
      HANDLE_CODE(bref.pack(prach_cfg_v1430.is_present(), 1));
      HANDLE_CODE(bref.pack(pucch_cfg_common_v1430.is_present(), 1));
      HANDLE_CODE(bref.pack(tdd_cfg_v1430.is_present(), 1));
      if (high_speed_cfg_r14.is_present()) {
        HANDLE_CODE(high_speed_cfg_r14->pack(bref));
      }
      if (prach_cfg_v1430.is_present()) {
        HANDLE_CODE(prach_cfg_v1430->pack(bref));
      }
      if (pucch_cfg_common_v1430.is_present()) {
        HANDLE_CODE(pucch_cfg_common_v1430->pack(bref));
      }
      if (tdd_cfg_v1430.is_present()) {
        HANDLE_CODE(tdd_cfg_v1430->pack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(tdd_cfg_v1450.is_present(), 1));
      if (tdd_cfg_v1450.is_present()) {
        HANDLE_CODE(tdd_cfg_v1450->pack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_pack_guard varlen_scope(bref, false);

      HANDLE_CODE(bref.pack(ul_pwr_ctrl_common_v1530.is_present(), 1));
      HANDLE_CODE(bref.pack(high_speed_cfg_v1530.is_present(), 1));
      if (ul_pwr_ctrl_common_v1530.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_v1530->pack(bref));
      }
      if (high_speed_cfg_v1530.is_present()) {
        HANDLE_CODE(high_speed_cfg_v1530->pack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE rr_cfg_common_s::unpack(cbit_ref& bref)
{
  bref.unpack(ext, 1);
  HANDLE_CODE(bref.unpack(rach_cfg_common_present, 1));
  HANDLE_CODE(bref.unpack(pdsch_cfg_common_present, 1));
  HANDLE_CODE(bref.unpack(phich_cfg_present, 1));
  HANDLE_CODE(bref.unpack(pucch_cfg_common_present, 1));
  HANDLE_CODE(bref.unpack(srs_ul_cfg_common_present, 1));
  HANDLE_CODE(bref.unpack(ul_pwr_ctrl_common_present, 1));
  HANDLE_CODE(bref.unpack(ant_info_common_present, 1));
  HANDLE_CODE(bref.unpack(p_max_present, 1));
  HANDLE_CODE(bref.unpack(tdd_cfg_present, 1));

  if (rach_cfg_common_present) {
    HANDLE_CODE(rach_cfg_common.unpack(bref));
  }
  HANDLE_CODE(prach_cfg.unpack(bref));
  if (pdsch_cfg_common_present) {
    HANDLE_CODE(pdsch_cfg_common.unpack(bref));
  }
  HANDLE_CODE(pusch_cfg_common.unpack(bref));
  if (phich_cfg_present) {
    HANDLE_CODE(phich_cfg.unpack(bref));
  }
  if (pucch_cfg_common_present) {
    HANDLE_CODE(pucch_cfg_common.unpack(bref));
  }
  if (srs_ul_cfg_common_present) {
    HANDLE_CODE(srs_ul_cfg_common.unpack(bref));
  }
  if (ul_pwr_ctrl_common_present) {
    HANDLE_CODE(ul_pwr_ctrl_common.unpack(bref));
  }
  if (ant_info_common_present) {
    HANDLE_CODE(ant_info_common.unpack(bref));
  }
  if (p_max_present) {
    HANDLE_CODE(unpack_integer(p_max, bref, (int8_t)-30, (int8_t)33));
  }
  if (tdd_cfg_present) {
    HANDLE_CODE(tdd_cfg.unpack(bref));
  }
  HANDLE_CODE(ul_cp_len.unpack(bref));

  if (ext) {
    ext_groups_unpacker_guard group_flags(7);
    group_flags.unpack(bref);

    if (group_flags[0]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ul_pwr_ctrl_common_v1020_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_common_v1020_present, 1));
      ul_pwr_ctrl_common_v1020.set_present(ul_pwr_ctrl_common_v1020_present);
      if (ul_pwr_ctrl_common_v1020.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_v1020->unpack(bref));
      }
    }
    if (group_flags[1]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool tdd_cfg_v1130_present;
      HANDLE_CODE(bref.unpack(tdd_cfg_v1130_present, 1));
      tdd_cfg_v1130.set_present(tdd_cfg_v1130_present);
      if (tdd_cfg_v1130.is_present()) {
        HANDLE_CODE(tdd_cfg_v1130->unpack(bref));
      }
    }
    if (group_flags[2]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool pusch_cfg_common_v1270_present;
      HANDLE_CODE(bref.unpack(pusch_cfg_common_v1270_present, 1));
      pusch_cfg_common_v1270.set_present(pusch_cfg_common_v1270_present);
      if (pusch_cfg_common_v1270.is_present()) {
        HANDLE_CODE(pusch_cfg_common_v1270->unpack(bref));
      }
    }
    if (group_flags[3]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool prach_cfg_v1310_present;
      HANDLE_CODE(bref.unpack(prach_cfg_v1310_present, 1));
      prach_cfg_v1310.set_present(prach_cfg_v1310_present);
      bool freq_hop_params_r13_present;
      HANDLE_CODE(bref.unpack(freq_hop_params_r13_present, 1));
      freq_hop_params_r13.set_present(freq_hop_params_r13_present);
      bool pdsch_cfg_common_v1310_present;
      HANDLE_CODE(bref.unpack(pdsch_cfg_common_v1310_present, 1));
      pdsch_cfg_common_v1310.set_present(pdsch_cfg_common_v1310_present);
      bool pucch_cfg_common_v1310_present;
      HANDLE_CODE(bref.unpack(pucch_cfg_common_v1310_present, 1));
      pucch_cfg_common_v1310.set_present(pucch_cfg_common_v1310_present);
      bool pusch_cfg_common_v1310_present;
      HANDLE_CODE(bref.unpack(pusch_cfg_common_v1310_present, 1));
      pusch_cfg_common_v1310.set_present(pusch_cfg_common_v1310_present);
      bool ul_pwr_ctrl_common_v1310_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_common_v1310_present, 1));
      ul_pwr_ctrl_common_v1310.set_present(ul_pwr_ctrl_common_v1310_present);
      if (prach_cfg_v1310.is_present()) {
        HANDLE_CODE(prach_cfg_v1310->unpack(bref));
      }
      if (freq_hop_params_r13.is_present()) {
        HANDLE_CODE(freq_hop_params_r13->unpack(bref));
      }
      if (pdsch_cfg_common_v1310.is_present()) {
        HANDLE_CODE(pdsch_cfg_common_v1310->unpack(bref));
      }
      if (pucch_cfg_common_v1310.is_present()) {
        HANDLE_CODE(pucch_cfg_common_v1310->unpack(bref));
      }
      if (pusch_cfg_common_v1310.is_present()) {
        HANDLE_CODE(pusch_cfg_common_v1310->unpack(bref));
      }
      if (ul_pwr_ctrl_common_v1310.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_v1310->unpack(bref));
      }
    }
    if (group_flags[4]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool high_speed_cfg_r14_present;
      HANDLE_CODE(bref.unpack(high_speed_cfg_r14_present, 1));
      high_speed_cfg_r14.set_present(high_speed_cfg_r14_present);
      bool prach_cfg_v1430_present;
      HANDLE_CODE(bref.unpack(prach_cfg_v1430_present, 1));
      prach_cfg_v1430.set_present(prach_cfg_v1430_present);
      bool pucch_cfg_common_v1430_present;
      HANDLE_CODE(bref.unpack(pucch_cfg_common_v1430_present, 1));
      pucch_cfg_common_v1430.set_present(pucch_cfg_common_v1430_present);
      bool tdd_cfg_v1430_present;
      HANDLE_CODE(bref.unpack(tdd_cfg_v1430_present, 1));
      tdd_cfg_v1430.set_present(tdd_cfg_v1430_present);
      if (high_speed_cfg_r14.is_present()) {
        HANDLE_CODE(high_speed_cfg_r14->unpack(bref));
      }
      if (prach_cfg_v1430.is_present()) {
        HANDLE_CODE(prach_cfg_v1430->unpack(bref));
      }
      if (pucch_cfg_common_v1430.is_present()) {
        HANDLE_CODE(pucch_cfg_common_v1430->unpack(bref));
      }
      if (tdd_cfg_v1430.is_present()) {
        HANDLE_CODE(tdd_cfg_v1430->unpack(bref));
      }
    }
    if (group_flags[5]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool tdd_cfg_v1450_present;
      HANDLE_CODE(bref.unpack(tdd_cfg_v1450_present, 1));
      tdd_cfg_v1450.set_present(tdd_cfg_v1450_present);
      if (tdd_cfg_v1450.is_present()) {
        HANDLE_CODE(tdd_cfg_v1450->unpack(bref));
      }
    }
    if (group_flags[6]) {
      varlength_field_unpack_guard varlen_scope(bref, false);

      bool ul_pwr_ctrl_common_v1530_present;
      HANDLE_CODE(bref.unpack(ul_pwr_ctrl_common_v1530_present, 1));
      ul_pwr_ctrl_common_v1530.set_present(ul_pwr_ctrl_common_v1530_present);
      bool high_speed_cfg_v1530_present;
      HANDLE_CODE(bref.unpack(high_speed_cfg_v1530_present, 1));
      high_speed_cfg_v1530.set_present(high_speed_cfg_v1530_present);
      if (ul_pwr_ctrl_common_v1530.is_present()) {
        HANDLE_CODE(ul_pwr_ctrl_common_v1530->unpack(bref));
      }
      if (high_speed_cfg_v1530.is_present()) {
        HANDLE_CODE(high_speed_cfg_v1530->unpack(bref));
      }
    }
  }
  return SRSASN_SUCCESS;
}
void rr_cfg_common_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (rach_cfg_common_present) {
    j.write_fieldname("rach-ConfigCommon");
    rach_cfg_common.to_json(j);
  }
  j.write_fieldname("prach-Config");
  prach_cfg.to_json(j);
  if (pdsch_cfg_common_present) {
    j.write_fieldname("pdsch-ConfigCommon");
    pdsch_cfg_common.to_json(j);
  }
  j.write_fieldname("pusch-ConfigCommon");
  pusch_cfg_common.to_json(j);
  if (phich_cfg_present) {
    j.write_fieldname("phich-Config");
    phich_cfg.to_json(j);
  }
  if (pucch_cfg_common_present) {
    j.write_fieldname("pucch-ConfigCommon");
    pucch_cfg_common.to_json(j);
  }
  if (srs_ul_cfg_common_present) {
    j.write_fieldname("soundingRS-UL-ConfigCommon");
    srs_ul_cfg_common.to_json(j);
  }
  if (ul_pwr_ctrl_common_present) {
    j.write_fieldname("uplinkPowerControlCommon");
    ul_pwr_ctrl_common.to_json(j);
  }
  if (ant_info_common_present) {
    j.write_fieldname("antennaInfoCommon");
    ant_info_common.to_json(j);
  }
  if (p_max_present) {
    j.write_int("p-Max", p_max);
  }
  if (tdd_cfg_present) {
    j.write_fieldname("tdd-Config");
    tdd_cfg.to_json(j);
  }
  j.write_str("ul-CyclicPrefixLength", ul_cp_len.to_string());
  if (ext) {
    if (ul_pwr_ctrl_common_v1020.is_present()) {
      j.write_fieldname("uplinkPowerControlCommon-v1020");
      ul_pwr_ctrl_common_v1020->to_json(j);
    }
    if (tdd_cfg_v1130.is_present()) {
      j.write_fieldname("tdd-Config-v1130");
      tdd_cfg_v1130->to_json(j);
    }
    if (pusch_cfg_common_v1270.is_present()) {
      j.write_fieldname("pusch-ConfigCommon-v1270");
      pusch_cfg_common_v1270->to_json(j);
    }
    if (prach_cfg_v1310.is_present()) {
      j.write_fieldname("prach-Config-v1310");
      prach_cfg_v1310->to_json(j);
    }
    if (freq_hop_params_r13.is_present()) {
      j.write_fieldname("freqHoppingParameters-r13");
      freq_hop_params_r13->to_json(j);
    }
    if (pdsch_cfg_common_v1310.is_present()) {
      j.write_fieldname("pdsch-ConfigCommon-v1310");
      pdsch_cfg_common_v1310->to_json(j);
    }
    if (pucch_cfg_common_v1310.is_present()) {
      j.write_fieldname("pucch-ConfigCommon-v1310");
      pucch_cfg_common_v1310->to_json(j);
    }
    if (pusch_cfg_common_v1310.is_present()) {
      j.write_fieldname("pusch-ConfigCommon-v1310");
      pusch_cfg_common_v1310->to_json(j);
    }
    if (ul_pwr_ctrl_common_v1310.is_present()) {
      j.write_fieldname("uplinkPowerControlCommon-v1310");
      ul_pwr_ctrl_common_v1310->to_json(j);
    }
    if (high_speed_cfg_r14.is_present()) {
      j.write_fieldname("highSpeedConfig-r14");
      high_speed_cfg_r14->to_json(j);
    }
    if (prach_cfg_v1430.is_present()) {
      j.write_fieldname("prach-Config-v1430");
      prach_cfg_v1430->to_json(j);
    }
    if (pucch_cfg_common_v1430.is_present()) {
      j.write_fieldname("pucch-ConfigCommon-v1430");
      pucch_cfg_common_v1430->to_json(j);
    }
    if (tdd_cfg_v1430.is_present()) {
      j.write_fieldname("tdd-Config-v1430");
      tdd_cfg_v1430->to_json(j);
    }
    if (tdd_cfg_v1450.is_present()) {
      j.write_fieldname("tdd-Config-v1450");
      tdd_cfg_v1450->to_json(j);
    }
    if (ul_pwr_ctrl_common_v1530.is_present()) {
      j.write_fieldname("uplinkPowerControlCommon-v1530");
      ul_pwr_ctrl_common_v1530->to_json(j);
    }
    if (high_speed_cfg_v1530.is_present()) {
      j.write_fieldname("highSpeedConfig-v1530");
      high_speed_cfg_v1530->to_json(j);
    }
  }
  j.end_obj();
}
