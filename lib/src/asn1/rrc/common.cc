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

#include "srsran/asn1/rrc/common.h"
#include <sstream>

using namespace asn1;
using namespace asn1::rrc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// PLMN-Identity ::= SEQUENCE
SRSASN_CODE plmn_id_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(bref.pack(mcc_present, 1));

  if (mcc_present) {
    HANDLE_CODE(pack_fixed_seq_of(bref, &(mcc)[0], mcc.size(), integer_packer<uint8_t>(0, 9)));
  }
  HANDLE_CODE(pack_dyn_seq_of(bref, mnc, 2, 3, integer_packer<uint8_t>(0, 9)));

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_id_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(bref.unpack(mcc_present, 1));

  if (mcc_present) {
    HANDLE_CODE(unpack_fixed_seq_of(&(mcc)[0], bref, mcc.size(), integer_packer<uint8_t>(0, 9)));
  }
  HANDLE_CODE(unpack_dyn_seq_of(mnc, bref, 2, 3, integer_packer<uint8_t>(0, 9)));

  return SRSASN_SUCCESS;
}
void plmn_id_s::to_json(json_writer& j) const
{
  j.start_obj();
  if (mcc_present) {
    j.start_array("mcc");
    for (const auto& e1 : mcc) {
      j.write_int(e1);
    }
    j.end_array();
  }
  j.start_array("mnc");
  for (const auto& e1 : mnc) {
    j.write_int(e1);
  }
  j.end_array();
  j.end_obj();
}

// PLMN-IdentityInfo ::= SEQUENCE
SRSASN_CODE plmn_id_info_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(plmn_id.pack(bref));
  HANDLE_CODE(cell_reserved_for_oper.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE plmn_id_info_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(plmn_id.unpack(bref));
  HANDLE_CODE(cell_reserved_for_oper.unpack(bref));

  return SRSASN_SUCCESS;
}
void plmn_id_info_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("plmn-Identity");
  plmn_id.to_json(j);
  j.write_str("cellReservedForOperatorUse", cell_reserved_for_oper.to_string());
  j.end_obj();
}

const char* plmn_id_info_s::cell_reserved_for_oper_opts::to_string() const
{
  static const char* options[] = {"reserved", "notReserved"};
  return convert_enum_idx(options, 2, value, "plmn_id_info_s::cell_reserved_for_oper_e_");
}

// Alpha-r12 ::= ENUMERATED
const char* alpha_r12_opts::to_string() const
{
  static const char* options[] = {"al0", "al04", "al05", "al06", "al07", "al08", "al09", "al1"};
  return convert_enum_idx(options, 8, value, "alpha_r12_e");
}
float alpha_r12_opts::to_number() const
{
  static const float options[] = {0.0, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
  return map_enum_number(options, 8, value, "alpha_r12_e");
}
const char* alpha_r12_opts::to_number_string() const
{
  static const char* options[] = {"0", "0.4", "0.5", "0.6", "0.7", "0.8", "0.9", "1"};
  return convert_enum_idx(options, 8, value, "alpha_r12_e");
}

// BandclassCDMA2000 ::= ENUMERATED
const char* bandclass_cdma2000_opts::to_string() const
{
  static const char* options[] = {"bc0",       "bc1",     "bc2",    "bc3",    "bc4",       "bc5",       "bc6",
                                  "bc7",       "bc8",     "bc9",    "bc10",   "bc11",      "bc12",      "bc13",
                                  "bc14",      "bc15",    "bc16",   "bc17",   "bc18-v9a0", "bc19-v9a0", "bc20-v9a0",
                                  "bc21-v9a0", "spare10", "spare9", "spare8", "spare7",    "spare6",    "spare5",
                                  "spare4",    "spare3",  "spare2", "spare1"};
  return convert_enum_idx(options, 32, value, "bandclass_cdma2000_e");
}
uint8_t bandclass_cdma2000_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
  return map_enum_number(options, 22, value, "bandclass_cdma2000_e");
}

// FilterCoefficient ::= ENUMERATED
const char* filt_coef_opts::to_string() const
{
  static const char* options[] = {"fc0",
                                  "fc1",
                                  "fc2",
                                  "fc3",
                                  "fc4",
                                  "fc5",
                                  "fc6",
                                  "fc7",
                                  "fc8",
                                  "fc9",
                                  "fc11",
                                  "fc13",
                                  "fc15",
                                  "fc17",
                                  "fc19",
                                  "spare1"};
  return convert_enum_idx(options, 16, value, "filt_coef_e");
}
uint8_t filt_coef_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 13, 15, 17, 19};
  return map_enum_number(options, 15, value, "filt_coef_e");
}

// MBSFN-SubframeConfig ::= SEQUENCE
SRSASN_CODE mbsfn_sf_cfg_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(radioframe_alloc_period.pack(bref));
  HANDLE_CODE(pack_integer(bref, radioframe_alloc_offset, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(sf_alloc.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbsfn_sf_cfg_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(radioframe_alloc_period.unpack(bref));
  HANDLE_CODE(unpack_integer(radioframe_alloc_offset, bref, (uint8_t)0u, (uint8_t)7u));
  HANDLE_CODE(sf_alloc.unpack(bref));

  return SRSASN_SUCCESS;
}
void mbsfn_sf_cfg_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_str("radioframeAllocationPeriod", radioframe_alloc_period.to_string());
  j.write_int("radioframeAllocationOffset", radioframe_alloc_offset);
  j.write_fieldname("subframeAllocation");
  sf_alloc.to_json(j);
  j.end_obj();
}
bool mbsfn_sf_cfg_s::operator==(const mbsfn_sf_cfg_s& other) const
{
  return radioframe_alloc_period == other.radioframe_alloc_period and
         radioframe_alloc_offset == other.radioframe_alloc_offset and sf_alloc == other.sf_alloc;
}

const char* mbsfn_sf_cfg_s::radioframe_alloc_period_opts::to_string() const
{
  static const char* options[] = {"n1", "n2", "n4", "n8", "n16", "n32"};
  return convert_enum_idx(options, 6, value, "mbsfn_sf_cfg_s::radioframe_alloc_period_e_");
}
uint8_t mbsfn_sf_cfg_s::radioframe_alloc_period_opts::to_number() const
{
  static const uint8_t options[] = {1, 2, 4, 8, 16, 32};
  return map_enum_number(options, 6, value, "mbsfn_sf_cfg_s::radioframe_alloc_period_e_");
}

void mbsfn_sf_cfg_s::sf_alloc_c_::destroy_()
{
  switch (type_) {
    case types::one_frame:
      c.destroy<fixed_bitstring<6> >();
      break;
    case types::four_frames:
      c.destroy<fixed_bitstring<24> >();
      break;
    default:
      break;
  }
}
void mbsfn_sf_cfg_s::sf_alloc_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::one_frame:
      c.init<fixed_bitstring<6> >();
      break;
    case types::four_frames:
      c.init<fixed_bitstring<24> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mbsfn_sf_cfg_s::sf_alloc_c_");
  }
}
mbsfn_sf_cfg_s::sf_alloc_c_::sf_alloc_c_(const mbsfn_sf_cfg_s::sf_alloc_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::one_frame:
      c.init(other.c.get<fixed_bitstring<6> >());
      break;
    case types::four_frames:
      c.init(other.c.get<fixed_bitstring<24> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mbsfn_sf_cfg_s::sf_alloc_c_");
  }
}
mbsfn_sf_cfg_s::sf_alloc_c_& mbsfn_sf_cfg_s::sf_alloc_c_::operator=(const mbsfn_sf_cfg_s::sf_alloc_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::one_frame:
      c.set(other.c.get<fixed_bitstring<6> >());
      break;
    case types::four_frames:
      c.set(other.c.get<fixed_bitstring<24> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mbsfn_sf_cfg_s::sf_alloc_c_");
  }

  return *this;
}
fixed_bitstring<6>& mbsfn_sf_cfg_s::sf_alloc_c_::set_one_frame()
{
  set(types::one_frame);
  return c.get<fixed_bitstring<6> >();
}
fixed_bitstring<24>& mbsfn_sf_cfg_s::sf_alloc_c_::set_four_frames()
{
  set(types::four_frames);
  return c.get<fixed_bitstring<24> >();
}
void mbsfn_sf_cfg_s::sf_alloc_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::one_frame:
      j.write_str("oneFrame", c.get<fixed_bitstring<6> >().to_string());
      break;
    case types::four_frames:
      j.write_str("fourFrames", c.get<fixed_bitstring<24> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "mbsfn_sf_cfg_s::sf_alloc_c_");
  }
  j.end_obj();
}
SRSASN_CODE mbsfn_sf_cfg_s::sf_alloc_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::one_frame:
      HANDLE_CODE(c.get<fixed_bitstring<6> >().pack(bref));
      break;
    case types::four_frames:
      HANDLE_CODE(c.get<fixed_bitstring<24> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mbsfn_sf_cfg_s::sf_alloc_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mbsfn_sf_cfg_s::sf_alloc_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::one_frame:
      HANDLE_CODE(c.get<fixed_bitstring<6> >().unpack(bref));
      break;
    case types::four_frames:
      HANDLE_CODE(c.get<fixed_bitstring<24> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mbsfn_sf_cfg_s::sf_alloc_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool mbsfn_sf_cfg_s::sf_alloc_c_::operator==(const sf_alloc_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::one_frame:
      return c.get<fixed_bitstring<6> >() == other.c.get<fixed_bitstring<6> >();
    case types::four_frames:
      return c.get<fixed_bitstring<24> >() == other.c.get<fixed_bitstring<24> >();
    default:
      return true;
  }
  return true;
}

// MBSFN-SubframeConfig-v1430 ::= SEQUENCE
SRSASN_CODE mbsfn_sf_cfg_v1430_s::pack(bit_ref& bref) const
{
  HANDLE_CODE(sf_alloc_v1430.pack(bref));

  return SRSASN_SUCCESS;
}
SRSASN_CODE mbsfn_sf_cfg_v1430_s::unpack(cbit_ref& bref)
{
  HANDLE_CODE(sf_alloc_v1430.unpack(bref));

  return SRSASN_SUCCESS;
}
void mbsfn_sf_cfg_v1430_s::to_json(json_writer& j) const
{
  j.start_obj();
  j.write_fieldname("subframeAllocation-v1430");
  sf_alloc_v1430.to_json(j);
  j.end_obj();
}
bool mbsfn_sf_cfg_v1430_s::operator==(const mbsfn_sf_cfg_v1430_s& other) const
{
  return sf_alloc_v1430 == other.sf_alloc_v1430;
}

void mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::destroy_()
{
  switch (type_) {
    case types::one_frame_v1430:
      c.destroy<fixed_bitstring<2> >();
      break;
    case types::four_frames_v1430:
      c.destroy<fixed_bitstring<8> >();
      break;
    default:
      break;
  }
}
void mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::one_frame_v1430:
      c.init<fixed_bitstring<2> >();
      break;
    case types::four_frames_v1430:
      c.init<fixed_bitstring<8> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_");
  }
}
mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::sf_alloc_v1430_c_(const mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::one_frame_v1430:
      c.init(other.c.get<fixed_bitstring<2> >());
      break;
    case types::four_frames_v1430:
      c.init(other.c.get<fixed_bitstring<8> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_");
  }
}
mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_&
mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::operator=(const mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::one_frame_v1430:
      c.set(other.c.get<fixed_bitstring<2> >());
      break;
    case types::four_frames_v1430:
      c.set(other.c.get<fixed_bitstring<8> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_");
  }

  return *this;
}
fixed_bitstring<2>& mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::set_one_frame_v1430()
{
  set(types::one_frame_v1430);
  return c.get<fixed_bitstring<2> >();
}
fixed_bitstring<8>& mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::set_four_frames_v1430()
{
  set(types::four_frames_v1430);
  return c.get<fixed_bitstring<8> >();
}
void mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::one_frame_v1430:
      j.write_str("oneFrame-v1430", c.get<fixed_bitstring<2> >().to_string());
      break;
    case types::four_frames_v1430:
      j.write_str("fourFrames-v1430", c.get<fixed_bitstring<8> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_");
  }
  j.end_obj();
}
SRSASN_CODE mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::one_frame_v1430:
      HANDLE_CODE(c.get<fixed_bitstring<2> >().pack(bref));
      break;
    case types::four_frames_v1430:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::one_frame_v1430:
      HANDLE_CODE(c.get<fixed_bitstring<2> >().unpack(bref));
      break;
    case types::four_frames_v1430:
      HANDLE_CODE(c.get<fixed_bitstring<8> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool mbsfn_sf_cfg_v1430_s::sf_alloc_v1430_c_::operator==(const sf_alloc_v1430_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::one_frame_v1430:
      return c.get<fixed_bitstring<2> >() == other.c.get<fixed_bitstring<2> >();
    case types::four_frames_v1430:
      return c.get<fixed_bitstring<8> >() == other.c.get<fixed_bitstring<8> >();
    default:
      return true;
  }
  return true;
}

// MeasSubframePattern-r10 ::= CHOICE
void meas_sf_pattern_r10_c::destroy_()
{
  switch (type_) {
    case types::sf_pattern_fdd_r10:
      c.destroy<fixed_bitstring<40> >();
      break;
    case types::sf_pattern_tdd_r10:
      c.destroy<sf_pattern_tdd_r10_c_>();
      break;
    default:
      break;
  }
}
void meas_sf_pattern_r10_c::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sf_pattern_fdd_r10:
      c.init<fixed_bitstring<40> >();
      break;
    case types::sf_pattern_tdd_r10:
      c.init<sf_pattern_tdd_r10_c_>();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_r10_c");
  }
}
meas_sf_pattern_r10_c::meas_sf_pattern_r10_c(const meas_sf_pattern_r10_c& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sf_pattern_fdd_r10:
      c.init(other.c.get<fixed_bitstring<40> >());
      break;
    case types::sf_pattern_tdd_r10:
      c.init(other.c.get<sf_pattern_tdd_r10_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_r10_c");
  }
}
meas_sf_pattern_r10_c& meas_sf_pattern_r10_c::operator=(const meas_sf_pattern_r10_c& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sf_pattern_fdd_r10:
      c.set(other.c.get<fixed_bitstring<40> >());
      break;
    case types::sf_pattern_tdd_r10:
      c.set(other.c.get<sf_pattern_tdd_r10_c_>());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_r10_c");
  }

  return *this;
}
fixed_bitstring<40>& meas_sf_pattern_r10_c::set_sf_pattern_fdd_r10()
{
  set(types::sf_pattern_fdd_r10);
  return c.get<fixed_bitstring<40> >();
}
meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_& meas_sf_pattern_r10_c::set_sf_pattern_tdd_r10()
{
  set(types::sf_pattern_tdd_r10);
  return c.get<sf_pattern_tdd_r10_c_>();
}
void meas_sf_pattern_r10_c::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sf_pattern_fdd_r10:
      j.write_str("subframePatternFDD-r10", c.get<fixed_bitstring<40> >().to_string());
      break;
    case types::sf_pattern_tdd_r10:
      j.write_fieldname("subframePatternTDD-r10");
      c.get<sf_pattern_tdd_r10_c_>().to_json(j);
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_r10_c");
  }
  j.end_obj();
}
SRSASN_CODE meas_sf_pattern_r10_c::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sf_pattern_fdd_r10:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().pack(bref));
      break;
    case types::sf_pattern_tdd_r10:
      HANDLE_CODE(c.get<sf_pattern_tdd_r10_c_>().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_r10_c");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_sf_pattern_r10_c::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sf_pattern_fdd_r10:
      HANDLE_CODE(c.get<fixed_bitstring<40> >().unpack(bref));
      break;
    case types::sf_pattern_tdd_r10:
      HANDLE_CODE(c.get<sf_pattern_tdd_r10_c_>().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_r10_c");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool meas_sf_pattern_r10_c::operator==(const meas_sf_pattern_r10_c& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::sf_pattern_fdd_r10:
      return c.get<fixed_bitstring<40> >() == other.c.get<fixed_bitstring<40> >();
    case types::sf_pattern_tdd_r10:
      return c.get<sf_pattern_tdd_r10_c_>() == other.c.get<sf_pattern_tdd_r10_c_>();
    default:
      return true;
  }
  return true;
}

void meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::destroy_()
{
  switch (type_) {
    case types::sf_cfg1_minus5_r10:
      c.destroy<fixed_bitstring<20> >();
      break;
    case types::sf_cfg0_r10:
      c.destroy<fixed_bitstring<70> >();
      break;
    case types::sf_cfg6_r10:
      c.destroy<fixed_bitstring<60> >();
      break;
    default:
      break;
  }
}
void meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::set(types::options e)
{
  destroy_();
  type_ = e;
  switch (type_) {
    case types::sf_cfg1_minus5_r10:
      c.init<fixed_bitstring<20> >();
      break;
    case types::sf_cfg0_r10:
      c.init<fixed_bitstring<70> >();
      break;
    case types::sf_cfg6_r10:
      c.init<fixed_bitstring<60> >();
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_");
  }
}
meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::sf_pattern_tdd_r10_c_(
    const meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_& other)
{
  type_ = other.type();
  switch (type_) {
    case types::sf_cfg1_minus5_r10:
      c.init(other.c.get<fixed_bitstring<20> >());
      break;
    case types::sf_cfg0_r10:
      c.init(other.c.get<fixed_bitstring<70> >());
      break;
    case types::sf_cfg6_r10:
      c.init(other.c.get<fixed_bitstring<60> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_");
  }
}
meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_&
meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::operator=(const meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_& other)
{
  if (this == &other) {
    return *this;
  }
  set(other.type());
  switch (type_) {
    case types::sf_cfg1_minus5_r10:
      c.set(other.c.get<fixed_bitstring<20> >());
      break;
    case types::sf_cfg0_r10:
      c.set(other.c.get<fixed_bitstring<70> >());
      break;
    case types::sf_cfg6_r10:
      c.set(other.c.get<fixed_bitstring<60> >());
      break;
    case types::nulltype:
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_");
  }

  return *this;
}
fixed_bitstring<20>& meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::set_sf_cfg1_minus5_r10()
{
  set(types::sf_cfg1_minus5_r10);
  return c.get<fixed_bitstring<20> >();
}
fixed_bitstring<70>& meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::set_sf_cfg0_r10()
{
  set(types::sf_cfg0_r10);
  return c.get<fixed_bitstring<70> >();
}
fixed_bitstring<60>& meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::set_sf_cfg6_r10()
{
  set(types::sf_cfg6_r10);
  return c.get<fixed_bitstring<60> >();
}
void meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::to_json(json_writer& j) const
{
  j.start_obj();
  switch (type_) {
    case types::sf_cfg1_minus5_r10:
      j.write_str("subframeConfig1-5-r10", c.get<fixed_bitstring<20> >().to_string());
      break;
    case types::sf_cfg0_r10:
      j.write_str("subframeConfig0-r10", c.get<fixed_bitstring<70> >().to_string());
      break;
    case types::sf_cfg6_r10:
      j.write_str("subframeConfig6-r10", c.get<fixed_bitstring<60> >().to_string());
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_");
  }
  j.end_obj();
}
SRSASN_CODE meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::pack(bit_ref& bref) const
{
  type_.pack(bref);
  switch (type_) {
    case types::sf_cfg1_minus5_r10:
      HANDLE_CODE(c.get<fixed_bitstring<20> >().pack(bref));
      break;
    case types::sf_cfg0_r10:
      HANDLE_CODE(c.get<fixed_bitstring<70> >().pack(bref));
      break;
    case types::sf_cfg6_r10:
      HANDLE_CODE(c.get<fixed_bitstring<60> >().pack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_");
      return SRSASN_ERROR_ENCODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
SRSASN_CODE meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::unpack(cbit_ref& bref)
{
  types e;
  e.unpack(bref);
  set(e);
  switch (type_) {
    case types::sf_cfg1_minus5_r10:
      HANDLE_CODE(c.get<fixed_bitstring<20> >().unpack(bref));
      break;
    case types::sf_cfg0_r10:
      HANDLE_CODE(c.get<fixed_bitstring<70> >().unpack(bref));
      break;
    case types::sf_cfg6_r10:
      HANDLE_CODE(c.get<fixed_bitstring<60> >().unpack(bref));
      break;
    default:
      log_invalid_choice_id(type_, "meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_");
      return SRSASN_ERROR_DECODE_FAIL;
  }
  return SRSASN_SUCCESS;
}
bool meas_sf_pattern_r10_c::sf_pattern_tdd_r10_c_::operator==(const sf_pattern_tdd_r10_c_& other) const
{
  if (type_ != other.type_) {
    return false;
  }
  switch (type_) {
    case types::sf_cfg1_minus5_r10:
      return c.get<fixed_bitstring<20> >() == other.c.get<fixed_bitstring<20> >();
    case types::sf_cfg0_r10:
      return c.get<fixed_bitstring<70> >() == other.c.get<fixed_bitstring<70> >();
    case types::sf_cfg6_r10:
      return c.get<fixed_bitstring<60> >() == other.c.get<fixed_bitstring<60> >();
    default:
      return true;
  }
  return true;
}

// CipheringAlgorithm-r12 ::= ENUMERATED
const char* ciphering_algorithm_r12_opts::to_string() const
{
  static const char* options[] = {"eea0", "eea1", "eea2", "eea3-v1130", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "ciphering_algorithm_r12_e");
}
uint8_t ciphering_algorithm_r12_opts::to_number() const
{
  static const uint8_t options[] = {0, 1, 2, 3};
  return map_enum_number(options, 4, value, "ciphering_algorithm_r12_e");
}

// WLAN-BandIndicator-r13 ::= ENUMERATED
const char* wlan_band_ind_r13_opts::to_string() const
{
  static const char* options[] = {
      "band2dot4", "band5", "band60-v1430", "spare5", "spare4", "spare3", "spare2", "spare1"};
  return convert_enum_idx(options, 8, value, "wlan_band_ind_r13_e");
}
float wlan_band_ind_r13_opts::to_number() const
{
  static const float options[] = {2.4, 5.0, 60.0};
  return map_enum_number(options, 3, value, "wlan_band_ind_r13_e");
}
const char* wlan_band_ind_r13_opts::to_number_string() const
{
  static const char* options[] = {"2.4", "5", "60"};
  return convert_enum_idx(options, 8, value, "wlan_band_ind_r13_e");
}
